#include "BmnOnlineSource.h"

#include <FairRootManager.h>
#include <TBranchElement.h>

BmnOnlineSource::BmnOnlineSource(TString inputTransportAddress)
    : fEndpoint(inputTransportAddress),
      fZmqContext(nullptr),
      fZmqSubSocket(nullptr),
      fAreBranchesRegistered(kFALSE) {}

BmnOnlineSource::~BmnOnlineSource() {}

Bool_t BmnOnlineSource::Init() {
    LOG(info) << "BmnOnlineSource::Init()";

    fZmqContext.reset(zmq_ctx_new(), zmq_ctx_term);
    fZmqSubSocket.reset(zmq_socket(fZmqContext.get(), ZMQ_SUB), zmq_close);

    if (zmq_connect(fZmqSubSocket.get(), fEndpoint.Data()) != 0) {
        LOG(info) << "BmnOnlineSource::Init() | An error occurred while "
                     "connecting the socket to the address '"
                  << fEndpoint << "': " << zmq_strerror(errno);
        return kFALSE;
    }

    if (zmq_setsockopt(fZmqSubSocket.get(), ZMQ_SUBSCRIBE, "", 0) != 0) {
        LOG(error) << "BmnOnlineSource::Init() | An error occurred while "
                      "subscribing the socket to the address '"
                   << fEndpoint << "': " << zmq_strerror(errno);
        return kFALSE;
    }

    return kTRUE;
}

Int_t BmnOnlineSource::ReadEvent(UInt_t) {
    zmq_msg_t _message;
    zmq_msg_init(&_message);

    const auto _messageSize = zmq_msg_recv(&_message, fZmqSubSocket.get(), 0);

    if (_messageSize == -1) {
        LOG(warn) << "BmnOnlineSource::ReadEvent() | An error occurred while "
                     "receiving a message: "
                  << zmq_strerror(errno);
        zmq_msg_close(&_message);
        return kFALSE;
    } else {
        LOG(info) << "BmnOnlineSource::ReadEvent() | Message received: "
                  << _messageSize << " B";
    }

    std::unique_ptr<TTree> tree;

    DeserializeZmqMessage(&_message, tree);
    RegisterBranches(tree);
    UploadDataToRegisteredBranches(tree);

    zmq_msg_close(&_message);

    return 0;
}

void BmnOnlineSource::Close() {
    fZmqSubSocket.reset();
    fZmqContext.reset();

    fRegisteredBranches.clear();
}

void BmnOnlineSource::RegisterBranches(std::unique_ptr<TTree>& tree) {
    if (fAreBranchesRegistered) {
        return;
    }

    const auto _branches = tree->GetListOfBranches();
    for (Long64_t branchIndex = 0; branchIndex < _branches->GetEntriesFast();
         branchIndex++) {
        const auto _branch =
            static_cast<TBranchElement*>(_branches->At(branchIndex));
        const auto _branchName = _branch->GetName();

        if (fRegisteredBranches.find(_branchName) ==
                fRegisteredBranches.end() &&
            strcmp(_branch->GetClassName(), "TClonesArray") == 0) {
            auto _branch_data =
                std::make_unique<TClonesArray>(_branch->GetClonesName());

            FairRootManager::Instance()->RegisterInputObject(
                _branchName, _branch_data.get());

            fRegisteredBranches[_branchName] = std::move(_branch_data);

            LOG(info) << "BmnOnlineSource::RegisterBranches() | Register "
                      << _branchName;
        }
    }

    fAreBranchesRegistered = kTRUE;
}

void BmnOnlineSource::UploadDataToRegisteredBranches(
    std::unique_ptr<TTree>& tree) {
    for (auto& _branch : fRegisteredBranches) {
        _branch.second->Delete();
        auto tmp = _branch.second.get();
        if (tree->SetBranchAddress(_branch.first.c_str(), &tmp)) {
            LOG(error) << "BmnOnlineSource::UploadData(): Branch '"
                       << _branch.first << "' not found!";
        }
    }

    tree->GetEntry(0);
}
