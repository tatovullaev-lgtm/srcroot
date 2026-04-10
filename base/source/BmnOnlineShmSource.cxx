#include "BmnOnlineShmSource.h"

#include <FairRootManager.h>
#include <TBranchElement.h>
#include <TMessage.h>
#include <TSystem.h>

#include <chrono>
#include <thread>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

const auto configFilePath = gSystem->ExpandPathName("$VMCWORKDIR/config/online-shm-config.json");

class BmnTMessage : public TMessage {
   public:
    BmnTMessage(void *buf, Int_t len) : TMessage(buf, len) { ResetBit(kIsOwner); }
};

BmnOnlineShmSource::BmnOnlineShmSource() : FairOnlineSource(), fIsRegisterBranches(kFALSE) {}

BmnOnlineShmSource::~BmnOnlineShmSource() {}

Bool_t BmnOnlineShmSource::Init() {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        configFile.close();
        LOG(fatal) << "BmnOnlineShmSource::Init(): File " << configFilePath << " not found!";
    }

    json shmConfig;
    configFile >> shmConfig;
    configFile.close();

    try {
        fShmName = shmConfig["bmnShmOptions"]["shmName"];
        fWriteSemName = shmConfig["bmnShmOptions"]["writeSemName"];
        fReadSemName = shmConfig["bmnShmOptions"]["readSemName"];
        fMsgName = shmConfig["bmnShmOptions"]["msgName"];
    } catch (const json::type_error &e) {
        LOG(fatal) << "BmnOnlineShmSource::Init(): File " << configFilePath
                   << " does not contain the required data or the calorimeter type is incorrectly set!";
    }

    while (kTRUE) {
        try {
            fShmSegment = std::make_unique<bIpc::managed_shared_memory>(bIpc::open_only, fShmName.c_str());

            fWriteSem = std::make_unique<bIpc::named_semaphore>(bIpc::open_only, fWriteSemName.c_str());
            fReadSem = std::make_unique<bIpc::named_semaphore>(bIpc::open_only, fReadSemName.c_str());

            break;
        } catch (const bIpc::interprocess_exception &e) {
            LOG(error) << "BmnOnlineShmSource::Init(): Shared memory objects such as " << fShmName << ", "
                       << fWriteSemName << " and " << fReadSemName
                       << " have not been created, check that BmnOnlineEDSink is running!";
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return kTRUE;
}

void BmnOnlineShmSource::RegisterBranches(std::unique_ptr<TTree> &inputTree) {
    fRegisteredBranches.clear();

    auto branches = inputTree->GetListOfBranches();
    for (Long64_t branchIndex = 0; branchIndex < branches->GetEntriesFast(); branchIndex++) {
        auto branch = static_cast<TBranchElement *>(branches->At(branchIndex));
        if (strcmp(branch->GetClassName(), "TClonesArray") == 0) {
            fRegisteredBranches[branch->GetName()] = std::make_unique<TClonesArray>(branch->GetClonesName());
        }
    }

    for (auto &branch : fRegisteredBranches) {
        FairRootManager::Instance()->RegisterInputObject(branch.first.c_str(), branch.second.get());
    }
}

void BmnOnlineShmSource::UploadData(std::unique_ptr<TTree> &inputTree) {
    for (auto &branch : fRegisteredBranches) {
        branch.second->Delete();
        auto tmp = branch.second.get();
        if (inputTree->SetBranchAddress(branch.first.c_str(), &tmp)) {
            LOG(error) << "BmnOnlineShmSource::UploadData(): branch '" << branch.first << "' not found!";
        }
    }

    inputTree->GetEntry(0);
}

Int_t BmnOnlineShmSource::ReadEvent(UInt_t) {
    fReadSem->wait();

    auto msg = fShmSegment->find<char>(fMsgName.c_str());

    auto inputMessage = std::make_unique<BmnTMessage>(msg.first, msg.second);
    auto inputTree = std::unique_ptr<TTree>(static_cast<TTree *>(inputMessage->ReadObjectAny(inputMessage->GetClass())));

    if (!fIsRegisterBranches) {
        RegisterBranches(inputTree);
        fIsRegisterBranches = kTRUE;
    }

    UploadData(inputTree);

    inputMessage.reset();
    inputTree.reset();
    fShmSegment->destroy<char>(fMsgName.c_str());
    fWriteSem->post();

    return 0;
}

void BmnOnlineShmSource::Close() {}
