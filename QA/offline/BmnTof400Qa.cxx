#include "BmnTof400Qa.h"
#include <TStopwatch.h>
#include "BmnGlobalTrack.h"
#include <TFile.h>

static Double_t fWorkTime = 0.0;

BmnTof400Qa::BmnTof400Qa() {
    fStsTracks = nullptr;
    fGlobalTracks = nullptr;
    fKalman = nullptr;
    fTof400Hits = nullptr;
    fHistoManager = new BmnQaHistoManager();
}

InitStatus BmnTof400Qa::Init() {

    TStopwatch sw;
    sw.Start();
    fKalman = new BmnKalmanFilter();

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    fTof400Hits = (TClonesArray*)ioman->GetObject("BmnTof400Hit");
    fStsTracks = (TClonesArray*)ioman->GetObject("StsVector");
    fGlobalTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");

    fHistoManager->Create1 <TH1F>("TOF400_ResidX_total", "Total residuals in X coordinate; dX, cm; N", 100, -5.0, 5.0);
    fHistoManager->Create1 <TH1F>("TOF400_ResidY_total", "Total residuals in Y coordinate; dY, cm; N", 100, -5.0, 5.0);
    fHistoManager->Create2 <TH2F>("TOF400_Banan_total", "Total #beta vs rigidity; rigidity, GeV/c/Q; #beta", 400, -5.0, 10.0, 400, 0.2, 1.1);
    fHistoManager->Create2 <TH2F>("TOF400_Mass2_total", "Total m^{2} vs rigidity; rigidity, GeV/c/Q; m^{2}", 400, -5.0, 10.0, 400, -0.5, 10.0);

    for (Int_t i = 0; i < nModules; ++i) {
        fHistoManager->Create1 <TH1F>(Form("TOF400_ResidX_%d", i), Form("Residuals in X coordinate for module %d; dX, cm; N", i), 100, -5.0, 5.0);
        fHistoManager->Create1 <TH1F>(Form("TOF400_ResidY_%d", i), Form("Residuals in Y coordinate for module %d; dY, cm; N", i), 100, -5.0, 5.0);
    }

    tof400Dir = gFile->mkdir("TOF400");

    sw.Stop();
    fWorkTime += sw.RealTime();

    return kSUCCESS;
}

void BmnTof400Qa::Exec(Option_t* opt) {

    TStopwatch sw;
    sw.Start();

    if (!IsActive()) return;
    for (Int_t iTr = 0; iTr < fGlobalTracks->GetEntriesFast(); ++iTr) {
        BmnGlobalTrack* track = (BmnGlobalTrack*)fGlobalTracks->At(iTr);
        if (track->GetTof1HitIndex() == -1) continue;
        BmnTofHit* hit = (BmnTofHit*)fTof400Hits->At(track->GetTof1HitIndex());
        fHistoManager->H1("TOF400_ResidX_total")->Fill(hit->GetResX());
        fHistoManager->H1("TOF400_ResidY_total")->Fill(hit->GetResY());
        Int_t mod = ((hit->GetDetectorID() & 0x0000FF00) >> 8) - 1;
        fHistoManager->H1(Form("TOF400_ResidX_%d", mod))->Fill(hit->GetResX());
        fHistoManager->H1(Form("TOF400_ResidY_%d", mod))->Fill(hit->GetResY());
        fHistoManager->H2("TOF400_Banan_total")->Fill(track->GetP(), track->GetBeta(1));
        fHistoManager->H2("TOF400_Mass2_total")->Fill(track->GetP(), track->GetMass2(1));
    }

    sw.Stop();
    fWorkTime += sw.RealTime();

}

void BmnTof400Qa::Finish() {

    TStopwatch sw;
    sw.Start();

    for (Int_t i = 0; i < nModules; ++i) {
        fHistoManager->H1(Form("TOF400_ResidX_%d", i))->Fit("gaus", "QSR", "", -2, 2);
        fHistoManager->H1(Form("TOF400_ResidY_%d", i))->Fit("gaus", "QSR", "", -2, 2);
    }

    tof400Dir->cd();
    fHistoManager->WriteToFile();

    sw.Stop();
    fWorkTime += sw.RealTime();

    printf("Work time of BmnTof400Qa: %4.2f sec.\n", fWorkTime);
}