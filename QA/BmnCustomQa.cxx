#include "BmnCustomQa.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
// Boost
#include <boost/beast/core/detail/base64.hpp>
// ROOT
#include "TClonesArray.h"
#include "TH1.h"
#include "TH2F.h"
// FairRoot
#include <FairRootManager.h>
#include <fairlogger/Logger.h>
// BmnRoot
#include "BmnEnums.h"
#include "BmnHistManager.h"
#include "BmnMath.h"

#include <THttpServer.h>
#include <THttpWSHandler.h>

namespace fs = std::filesystem;
namespace b64 = boost::beast::detail::base64;

BmnCustomQa::BmnCustomQa(string name, string padConfFile)
    : BmnQaBase(name.c_str(), 1)
    , fPadConfDirWeb("config/")
    , fPadConfDir("config/")
    , fPadConfFileNameDefault(fPadConfDir + "pad-sample-default.json")
    , fPadConfFileName(padConfFile)
    , fPadGenerator(nullptr)
    , fPadTree(nullptr)
    , fTreeTemp(nullptr)
    , can(nullptr)
{
    SetTitle((name + "_title").c_str());
    LOGF(info, "fName  %s", fName.Data());
    LOGF(info, "fTitle  %s", fTitle.Data());
    /*  ifstream in(fPadConfFileName);
     if (!in)
         throw string("Cannot open file " + fPadConfFileName);
     stringstream ss;
     ss << in.rdbuf();
     fPadConfText = ss.str();
     cout << fPadConfText << endl; */
}
/* /root.dst/custom_qa_title/SaveConfig/cmd.json
/root.dst/custom_qa_title/SaveConfig/cmd.json */

BmnCustomQa::~BmnCustomQa()
{
    LOGF(info, "~BmnCustomQa()");
}

InitStatus BmnCustomQa::Init()
{
    fHM = new BmnHistManager();
    ReadDataBranches();
    CreateHistograms();
    if (fMonitorMode) {
        //       fServer->SetTimer(50, kFALSE);
        fServer->Register("/", this);
        fServer->Register("/", can);
        // fServer->Register("/", &fPadConfText);
        fServer->AddLocation(fPadConfDirWeb.c_str(), fPadConfDir.c_str());

        TString path = "/" + fTitle + "/";
        TString cmd = "/" + fName + "/->ResetPadTree()";
        TString cmdTitle = path + "Reset";
        LOGF(info, "cmdTitle  %s", cmdTitle.Data());
        fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");

        cmd = "/" + fName + "/->SaveConfig(%arg1%)";
        cmdTitle = path + "SaveConfig";
        LOGF(info, "cmdTitle  %s", cmdTitle.Data());
        fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");

        fServer->Restrict((TString("/") + fName).Data(),
                          (TString("allow_method=") + "FetchConfig,LoadDefaultConfig").Data());
    } else {
    }
    return kSUCCESS;
}

void BmnCustomQa::Exec(Option_t* opt)
{
    if (fEventNo % 100 == 0)
        printf("Events: %d\n", fEventNo);
    fEventNo++;
    ProcessGlobal();
    if (fMonitorMode) {
        fNItersSinceUpdate++;
        fServer->ProcessRequests();
        chrono::seconds time = chrono::duration_cast<chrono::seconds>(SysClock::now() - fTicksLastUpdate);
        if ((time > fTimeToUpdate) || (fNItersSinceUpdate > fNItersToUpdate)) {
            Redraw();
        }
    }
}

void BmnCustomQa::Finish()
{
    DBG("Finish()");
    // fServer->Restrict((TString("/") + fName).Data(), TString("allow_method=") + "FetchConfig").Data());
    fHM->WriteToFile();
    /*  if (!fMonitorMode) {
         Redraw();
         //        fReport = new BmnCustomQaReport(fOutName);
         //        fReport->SetOnlyPrimes(fPrimes);
         //        fReport->Create(fHM, fOutputDir);
     } */
    Redraw();
    can->SaveAs("custom.pdf");
}

void BmnCustomQa::Redraw()
{
    LOGF(info, "Redraw pid %d", getpid());
    // std::lock_guard<std::mutex> lock(mtx_can);
    /* if (mtx_can.try_lock()) {
        LOGF(info, "Redraw try_lock za");
    } else {
        LOGF(info, "Redraw try_lock ne");
        return;
    } */
    BmnHist::FillPadFromTree(fPadTree, fTreeTemp);
    fTreeTemp->Reset();
    BmnHist::DrawPadFromTree(fPadTree);
    if (fVerbose)
        printf("Draw! iters %d\n", fNItersSinceUpdate);
    fTicksLastUpdate = SysClock::now();
    fNItersSinceUpdate = 0;
    /*  mtx_can.unlock(); */
    return;
}

void BmnCustomQa::ReadDataBranches()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (nullptr == ioman)
        Fatal("Init", "BmnRootManager is not instantiated");
    fTreeTemp = new TTree(fName + "_tree", "qq");
    fTreeTemp->SetDirectory(nullptr);
    TList* brList = ioman->GetBranchNameList();
    for (Int_t i = 0; i < brList->GetEntries(); i++) {
        TString str = static_cast<TObjString*>(brList->At(i))->GetString();
        LOGF(debug, " str: %s", str.Data());
        TObject* obj = ioman->GetObject(str);
        if (obj == nullptr)
            continue;
        LOGF(debug, " obj: %p", (void*)obj);
        TClass* cl = TClass::GetClass(obj->ClassName());
        LOGF(debug, "ClassName: %s", obj->ClassName());
        if (cl == TClonesArray::Class()) {
            TClonesArray* newTCA = static_cast<TClonesArray*>(obj);
            fTreeTemp->Branch(str.Data(), &newTCA);
            //            fArrVec.push_back(newTCA);

        } else {
            if (cl == TFolder::Class())
                continue;
            TNamed* workObj = dynamic_cast<TNamed*>(obj);
            if (workObj == nullptr) {
                LOGF(error, "%s is not a TNamed!", str.Data());
                continue;
            }
            LOGF(debug, " TNamed obj n: %s t: %s", workObj->GetName(), workObj->GetTitle());
            TBranch* b = fTreeTemp->Branch(str.Data(), "TObject", workObj);
            LOGF(debug, " branch 0x%016lX", (uint64_t)b);
            //            fNamVec.push_back(workObj);
        }
    }
    printf("\nBRANCHES READ!\n\n");
}

void BmnCustomQa::CreateHistograms()
{
    can = new TCanvas("CustomCan", "Custom Canvas", 1920, 1080);
    RecreateHistTree();
    printf("\nHISTOGRAMS CREATED!\n\n");
}

void BmnCustomQa::RecreateHistTree()
{
    LOGF(info, "RecreateHistTree pid %d", getpid());
    // std::lock_guard<std::mutex> lock(mtx_can);
    /*  if (mtx_can.try_lock()) {
         LOGF(info, "RecreateHistTree try_lock za");
     } else {
         LOGF(info, "RecreateHistTree try_lock ne");
         return;
     } */
    // mtx_can.lock();
    try {
        can->Clear();
        ReloadConf();
        BmnPadGenerator::PadTree2Canvas(fPadTree, can);
    } catch (std::exception& e) {
        LOGF(error, "RecreateHistTree exception: %s", e.what());
    }
    /*  mtx_can.unlock(); */
    return;
}

void BmnCustomQa::ProcessGlobal()
{
    fTreeTemp->Fill();
    // printf("tree len %lld\n", fTreeTemp->GetEntriesFast());
}

void BmnCustomQa::ResetPadTree()
{
    LOGF(info, "ResetPadTree");
    BmnHist::ResetPadTree(fPadTree);
}

void BmnCustomQa::ReloadConf()
{
    if (fPadGenerator)
        delete fPadGenerator;
    fPadGenerator = new BmnPadGenerator();
    std::ifstream ifs(fPadConfFileName, std::ios::binary);
    if (!ifs) {
        LOGF(error, "Failed to open config file: %s", fPadConfFileName.c_str());
        return;
    }
    string str_out{std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};
    fCurConfig.String() = TString(str_out.c_str());
    fPadGenerator->LoadPTFrom(fPadConfFileName);
    fPadTree = fPadGenerator->GetPadBranch();
}

void BmnCustomQa::SaveConfig(string str)
{
    LOGF(info, "SaveConfig");
    string conf_str;
    conf_str.resize(b64::decoded_size(str.size()));
    auto res = b64::decode(&conf_str[0], str.data(), str.size());
    conf_str.resize(res.first);
    LOG(debug) << conf_str;
    ofstream out(fPadConfFileName, std::ios::out);
    if (!out)
        LOGF(error, "SaveConfig failed to open %s for writing", fPadConfFileName.c_str());
    out << conf_str;
    out.flush();
    out.close();
    RecreateHistTree();
}

TObjString* BmnCustomQa::LoadDefaultConfig()
{
    LOGF(info, "LoadDefaultConfig pid %d", getpid());
    try {
        fs::copy(fPadConfFileNameDefault, fPadConfFileName, fs::copy_options::overwrite_existing);
    } catch (fs::filesystem_error& e) {
        LOGF(error, "LoadDefaultConfig failed to copy %s to %s: %s", fPadConfFileNameDefault.c_str(),
             fPadConfFileName.c_str(), e.what());
        return nullptr;   //"LoadDefaultConfig failed to copy";
    }
    RecreateHistTree();
    return &fCurConfig;
}

TObjString* BmnCustomQa::FetchConfig()
{
    LOGF(info, "FetchConfig");
    return &fCurConfig;
}
