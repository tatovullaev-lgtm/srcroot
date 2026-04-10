#include "BmnProfQa.h"

#include <fstream>
#include <iostream>

#include "BmnAcceptanceFunction.h"
#include "BmnDchHit.h"
#include "BmnDchTrack.h"
#include "BmnEnums.h"
#include "BmnGemStripHit.h"
#include "BmnMath.h"
#include "BmnSiliconHit.h"
#include "BmnUtils.h"
#include "CbmBaseHit.h"
#include "BmnGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "FairRunAna.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2F.h"
#include "BmnHistManager.h"
#include "BmnSiBTHit.h"

using lit::FindAndReplace;
using lit::Split;

BmnProfQa::BmnProfQa(TString name, TString gemConf)
: BmnQaBase(name, 1),
  fProfHits(nullptr),
  fProfDigits(nullptr),
  fDstEventHeader(nullptr),
  fConfigProf(gemConf),
  fNItersSinceUpdate(0)
{}

BmnProfQa::~BmnProfQa()
{}

InitStatus BmnProfQa::Init() {
    printf("BmnProfQa::Init()");
    fHM = new BmnHistManager();
    if (fMonitorMode) {
        fReport = new BmnProfQaReport(fOutName);
        fReport->SetHM(fHM);
        fReport->SetMonitorMode(fMonitorMode);
        fReport->SetObjServer(fServer);
        fTicksLastUpdate = chrono::system_clock::now();
        SetDefaultDrawStyle();
    } else {

    }
    ReadDataBranches();
    CreateHistograms();
    if (fMonitorMode) {
        fReport->CallDraw();
        fReport->Register("/");
        RegisterCommands();
        //        fServer->SetTimer(50, kFALSE);
    } else {

    }
    printf("BmnProfQa::Init() finish");
    return kSUCCESS;
}

void BmnProfQa::Exec(Option_t* opt) {
    if (fEventNo % 1000 == 0) printf("EventCnt: %8d  EventId: %8d  RunId: %6u\n", fEventNo, fEventId, fRunId);
    fEventNo++;
    ProcessGlobal();
    if (fMonitorMode) {
        fNItersSinceUpdate++;
        fServer->ProcessRequests();
        chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();
        chrono::seconds time = chrono::duration_cast<chrono::seconds>(now - fTicksLastUpdate);
        //time_t tt = chrono::system_clock::to_time_t(now);
        //        printf("\ntime %s\n", ctime(&tt));
        if ((time > fTimeToUpdate) || (fNItersSinceUpdate > fNItersToUpdate)) {
            fReport->CallDraw();
            if (fVerbose)
                printf("Draw! iters %d\n", fNItersSinceUpdate);
            fTicksLastUpdate = now;
            fNItersSinceUpdate = 0;
        }
    }
    fPrevRunId = fRunId;
}

void BmnProfQa::Finish() {
    //    fHM->WriteToFile();
    if (!fMonitorMode) {
        fReport = new BmnProfQaReport(fOutName);
        fReport->Create(fHM, fOutputDir);
    } else
        fReport->CallDraw();
    //        fServer->SetTimer(50, kFALSE);
}

void BmnProfQa::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    if (nullptr == ioman) Fatal("Init", "BmnRootManager is not instantiated");
    fDstEventHeader = static_cast<DstEventHeader*> (ioman->GetObject("DstEventHeader."));

    if (fInnerTrackerSetup[kSiProf]) {
        //        fProfHits = (TClonesArray*) ioman->GetObject("BmnProfHit");
        fProfDigits = (TClonesArray*) ioman->GetObject("BmnProfDigit");
        printf("fProfDigits %p\n", fProfDigits);
        fProfDetector = make_unique<BmnSiProfStationSet>(fConfigProf);
        fReport->AddStationSet(
                kSiProf, static_cast<void*> (fProfDetector.get()));
    }

    printf("\nBRANCHES READ\n\n");
}

void BmnProfQa::CreateHistograms() {
    Int_t MaxSig = 2500;
    const Int_t n_strips(32);
    TString detName;
    BmnDetectorList::GetSystemNameCaps(kSiProf, detName);
    for (Short_t iSt = 0; iSt < fProfDetector->GetNStations(); ++iSt) {
        auto* st = fProfDetector->GetStation(iSt);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            auto* mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                //auto &lay = mod->GetStripLayer(iLayer);
                string name(BmnProfQaReport::HistNameProfDigit(detName, iSt, iModule, iLayer));
                //                CreateH1(BmnSimulationReport::HistNameStripDigit(name, iSt, iModule, iLayer), "Strip #", "Count", "",
                CreateH1(name, "Strip #", "Count", n_strips, 0, n_strips); //lay.GetNStrips(), 0, lay.GetNStrips());
                if (iLayer) {
                    CreateH1(name + "_sig", "Signal", "Count", MaxSig, -MaxSig, 0);
                    CreateH2(name + "_pedestals", "Strip #", "Signal", "", n_strips, 0, n_strips, MaxSig, -MaxSig, 0);
                } else {
                    CreateH1(name + "_sig", "Signal", "Count", MaxSig, 0, MaxSig);
                    CreateH2(name + "_pedestals", "Strip #", "Signal", "", n_strips, 0, n_strips, MaxSig, 0, MaxSig);
                }
                CreateH1(name + "_mean", "Strip #", "Count", n_strips, 0, n_strips); //lay.GetNStrips(), 0, lay.GetNStrips());
                //                BmnHist::SetHistStyleTH1(h);
            }
            string name2d(BmnProfQaReport::HistNameProfHits(detName, iSt));
            CreateH2(name2d, "Strip X", "Strip Y", "", n_strips, 0, n_strips, n_strips, 0, n_strips);
        }
    }

    //    for (Int_t iMap = 0; iMap < NChars; ++iMap) {
    //        auto & map = channel_maps[iMap];
    //        TString detName;
    //
    //        TH1 *h = new TH1F(
    //                Form("h%c_strips", map.LayerType),
    //                Form("%c side", map.LayerType), ASIC_channel, 0, ASIC_channel);
    //        TH1 *hMean = new TH1F(
    //                Form("h%c_strips_mean", map.LayerType),
    //                Form("%c side mean", map.LayerType), ASIC_channel, 0, ASIC_channel);
    //        TH1 *hSig = new TH1F(
    //                Form("h%c_sig", map.LayerType),
    //                Form("%c side signals", map.LayerType), 2 * ADCLimit + 1, -ADCLimit, ADCLimit);
    //        TH1 *hPed = new TH2F(
    //                Form("h%c_ped", map.LayerType),
    //                Form("%c side pedestals", map.LayerType), ASIC_channel, 0, ASIC_channel, NBins, -ADCLimit, ADCLimit);
    //        hVec[NCols * 0 + iMap] = h;
    //        hVec[NCols * 1 + iMap] = hSig;
    //        hVecOpt[NCols * 1 + iMap] = "logy";
    //        hVec[NCols * 2 + iMap] = hMean;
    //        hVec[NCols * 3 + iMap] = hPed;
    //        hVecOpt[NCols * 3 + iMap] = "colz";
    //    }

    //    if (fInnerTrackerSetup[kSiProf])
    //        CreateStripHits(fProfDetector, kSiProf);

    printf("\nHISTOGRAMS CREATED!\n\n");

}

void BmnProfQa::ProcessGlobal() {
    //    fRunId = fDstEventHeader->GetRunId();
    //    fEventId = fDstEventHeader->GetEventId();
    //    if (fRunId != fPrevRunId) {
    //        printf("run id change  %u  ->  %u\n", fPrevRunId, fRunId);
    //        fHM->ResetHists();
    //    }
    TString detName;
    BmnDetectorList::GetSystemNameCaps(kSiProf, detName);

    // for "hit making"
    const Int_t n_strips(32);
    //    vector<bitset < n_strips >> x_hit(fProfDetector->GetNStations(), 0);
    //    vector<bitset < n_strips >> y_hit(fProfDetector->GetNStations(), 0);
    vector<vector < Int_t >> x_hit(fProfDetector->GetNStations(), vector(n_strips, 0));
    vector<vector < Int_t >> y_hit(fProfDetector->GetNStations(), vector(n_strips, 0));

    for (Int_t iDig = 0; iDig < fProfDigits->GetEntriesFast(); ++iDig) {
        BmnSiProfDigit* dig = dynamic_cast<BmnSiProfDigit*> (fProfDigits->At(iDig));
        Int_t iSt = dig->GetStation();
        Int_t iMod = dig->GetModule();
        Int_t iLayer = dig->GetStripLayer();
        Int_t strip = dig->GetStripNumber();
        Double_t signal = dig->GetStripSignal();
        string name(BmnProfQaReport::HistNameProfDigit(detName, iSt, iMod, iLayer));
        fHM->H1(name)->Fill(strip, abs(signal));
        fHM->H1(name)->SetBinError(strip, 0);
        fHM->H1(name + "_sig")->Fill(signal);
        //        fHM->H1(name + "_mean")->Fill(signal);
        fHM->H2(name + "_pedestals")->Fill(strip, signal);

        // temp crutches to make hits
        if (iLayer)
            y_hit[iSt][strip] = -signal;
        else
            x_hit[iSt][strip] = signal;
    }
    for (Short_t iSt = 0; iSt < fProfDetector->GetNStations(); ++iSt) {
        string name2d(BmnProfQaReport::HistNameProfHits(detName, iSt));
        for (size_t iX = 0; iX < n_strips; iX++)
            for (size_t iY = 0; iY < n_strips; iY++)
                if (x_hit[iSt][iX] && y_hit[iSt][iY])
                    fHM->H2(name2d)->Fill(iX, iY, abs(x_hit[iSt][iX]) + abs(y_hit[iSt][iY]));
    }
    //    if (fProfHits) {
    //        FillStripHits(kSiProf, fProfHits);
    //    }
}

//void BmnProfQa::FillProfHits(TClonesArray* hits) {
//    TString detName;
//    BmnDetectorList::GetSystemNameCaps(detId, detName);
//    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); ++iHit) {
//        BmnHit* hit = static_cast<BmnHit*> (hits->At(iHit));
//        Short_t iSt = hit->GetStation();
//        //        if (detId==kCSC)
//        //            printf("hit! station %d {%2.3f, %2.3f}\n", iSt, hit->GetX(), hit->GetY());
//        fHM->H2(BmnSimulationReport::HistNameHits(detName, iSt))->Fill(hit->GetX(), hit->GetY());
//    }
//}

void BmnProfQa::ResetProf() {
    printf("Reset Prof QA\n");
    fHM->ResetHists();
}

void BmnProfQa::RegisterCommands() {
    fServer->Register("/", this);
    //    TString path = "/" + fTitle + "/";
    string path = string("/") + fName.Data() + "_cmd/";
    string cmd = string("/") + fName.Data() + "/->ResetProf()";
    string cmdTitle = path + "ResetProf";
    fServer->RegisterCommand(cmdTitle.data(), cmd.data(), "button;");
}
