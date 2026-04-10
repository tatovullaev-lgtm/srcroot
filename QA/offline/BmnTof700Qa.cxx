#include "BmnTof700Qa.h"
#include <TStopwatch.h>
#include "BmnGlobalTrack.h"
#include <TFile.h>
#include "UniRun.h"
#include "BmnFieldMap.h"
#include "BmnNewFieldMap.h"
#include "CbmStsTrack.h"
#include "TMath.h"
#include <fstream>

using namespace TMath;

static Double_t fWorkTime = 0.0;

BmnTof700Qa::BmnTof700Qa() {
    fStsTracks = nullptr;
    fGlobalTracks = nullptr;
    fKalman = nullptr;
    fTof700Hits = nullptr;
    fHistoManager = new BmnQaHistoManager();
    fKalmanType = 2; //1 - L1 KF, 2 - Lebedev's KF
    fIsField = kTRUE;
    fEventId = 0;
}

InitStatus BmnTof700Qa::Init() {

    TStopwatch sw;
    sw.Start();

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    /*CbmStsDigiScheme* digiScheme = */CbmStsDigiScheme::Instance(4); //for L1 KF
    fKalman = new BmnKalmanFilter();

    fTof700Hits = (TClonesArray*)ioman->GetObject("BmnTof700Hit");
    fStsTracks = (TClonesArray*)ioman->GetObject("StsVector");
    fStsHits = (TClonesArray*)ioman->GetObject("StsHit");
    fGlobalTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
    fVertex = (CbmVertex*)ioman->GetObject("PrimaryVertex.");

    CreateHistograms();


    ifstream infile("tof700_strip_time_shifts.txt");

    Int_t m, s, n;
    Float_t sh;
    while (infile >> m >> s >> n >> sh) {
        timeShifts[m][s] = sh;
    }

    sw.Stop();
    fWorkTime += sw.RealTime();
    return kSUCCESS;
}

void BmnTof700Qa::Exec(Option_t* opt) {

    TStopwatch sw;
    sw.Start();

    // Int_t modOrder[58] = {
    //     0, 1, 4, 7, 10, 13, 6, 30, 24, 32, 26, 34, 28, 52,
    //     14, 15, 16, 17, 18, 19, 36, 39, 37, 40, 38, 41,
    //     3, 12, 2, 5, 8, 11, 20, 31, 25, 33, 27, 35, 29, 51,
    //     42, 43, 44, 45, 46, 47,
    //     53, 54, 55, 56, 57, 58,
    //     22, 21, 23,
    //     48, 49, 50
    // };


    Float_t dx[nModules] = {
        +0.13, +0.15, -0.21, -0.28, +0.09, -0.29, -0.13, -0.28, -0.14, -0.08,
        -0.07, -0.13, -0.30, -0.26, +0.36, +0.24, +0.33, +0.46, -0.06, +0.09,
        -0.07, +0.36, -0.23, +0.18, +0.05, -0.38, +0.16, +0.26, +0.28, -0.21,
        -0.06, -0.29, -0.03, -0.45, +0.04, -0.33, +0.16, +0.15, +0.38, +0.19,
        +0.54, +0.68, -0.16, -0.04, +0.10, +0.23, -0.13, +0.00, -0.10, +0.56,
        +0.50, -0.18, +0.18, +0.44, +0.50, +0.39, +0.29, +0.37, +0.01
    };

    Float_t dy[nModules] = {
        -0.06, -0.08, -0.32, -0.25, -0.13, -0.31, -0.18, -0.19, -0.34, -0.45,
        -0.21, -0.34, -0.22, -0.21, -0.09, -0.22, -0.29, -0.28, -0.02, -0.15,
        -0.34, -0.17, -0.08, -0.01, -0.05, -0.09, +0.01, -0.06, +0.12, +0.00,
        -0.08, -0.20, -0.05, -0.07, +0.13, +0.00, -0.03, +0.02, +0.09, -0.07,
        +0.13, +0.21, -0.12, -0.16, -0.04, -0.02, +0.05, -0.04, -0.10, +0.19,
        +0.10, +0.04, +0.16, -0.01, -0.25, -0.29, -0.16, -0.02, -0.20
    };
    
    if (!IsActive()) return;

    cout << "Event ID: " << fEventId++ << endl;

    if (fVertex->GetNTracks() < 2) return;
    if (fVertex->GetZ() < -0.5 || fVertex->GetZ() > 0.5) return;
    if (fVertex->GetX() < -1.0 || fVertex->GetX() > 1.5) return;
    if (fVertex->GetY() < -1.0 || fVertex->GetY() > 1.2) return;

    //for STS
    for (Int_t iTr = 0; iTr < fStsTracks->GetEntriesFast(); ++iTr) {
        CbmStsTrack* track = (CbmStsTrack*)fStsTracks->At(iTr);

        if (track->GetNStsHits() < 5) continue;

        FairTrackParam parF = *(track->GetParamFirst());
        Int_t pdg = 2212;
        Float_t mom = 0.0;
        if (fIsField) {
            mom = 1.0 / parF.GetQp();
            pdg = (mom > 0) ? 2212 : -211;
        }

        // FairTrackParam parL1 = *(track->GetParamLast());
        // FairTrackParam parL2 = *(track->GetParamLast());

        // PropagateToZ(&parL1, 400, pdg, 1);
        // PropagateToZ(&parL2, 400, pdg, 2);

        // fHistoManager->H2("Compare_X")->Fill(parL1.GetX(), parL2.GetX());
        // fHistoManager->H2("Compare_Y")->Fill(parL1.GetY(), parL2.GetY());
        // fHistoManager->H2("Compare_Tx")->Fill(parL1.GetTx(), parL2.GetTx());
        // fHistoManager->H2("Compare_Ty")->Fill(parL1.GetTy(), parL2.GetTy());
        // fHistoManager->H2("Compare_P")->Fill(1.0 / parL1.GetQp(), 1.0 / parL2.GetQp());


        Double_t length = 0.0;

        if (PropagateToZ(&parF, fVertex->GetZ(), pdg, fKalmanType, &length) == kFALSE) continue;

        //if (fKalman->TGeoTrackPropagate(&parF, fVertex->GetZ(), pdg, nullptr, nullptr, kTRUE) == kBMNERROR) continue;
        if (TMath::Sqrt(TMath::Sq(parF.GetX() - fVertex->GetX()) + TMath::Sq(parF.GetY() - fVertex->GetY())) > 1.0) continue;

        //ADD LOOP OVER HITS TO CALCULATE CBMSTSTRACK LENGTH ====>
        CbmStsHit* cbmhit = (CbmStsHit*)fStsHits->At(track->GetStsHitIndex(0));
        Double_t xPrev = cbmhit->GetX();
        Double_t yPrev = cbmhit->GetY();
        Double_t zPrev = cbmhit->GetZ();
        for (Int_t iHit = 1; iHit < track->GetNStsHits(); ++iHit) {
            cbmhit = (CbmStsHit*)fStsHits->At(track->GetStsHitIndex(iHit));
            length += Sqrt(Sq(cbmhit->GetX() - xPrev) + Sq(cbmhit->GetY() - yPrev) + Sq(cbmhit->GetZ() - zPrev));
            xPrev = cbmhit->GetX();
            yPrev = cbmhit->GetY();
            zPrev = cbmhit->GetZ();
        }
        //<====


        FairTrackParam parL = *(track->GetParamLast());
        Float_t minResX = 100.0;
        Float_t minResY = 100.0;
        Int_t  minIdx = -1;
        for (Int_t iH = 0; iH < fTof700Hits->GetEntriesFast(); ++iH) {
            BmnTofHit* hit = (BmnTofHit*)fTof700Hits->At(iH);
            Int_t mod = ((hit->GetDetectorID() & 0x0000FF00) >> 8) - 1;
            //Double_t hitX = hit->GetX();
            //Double_t hitY = hit->GetY();
            Double_t hitZ = hit->GetZ();// -dz[mod];
            PropagateToZ(&parL, hitZ, pdg, fKalmanType);
            //if (fKalman->TGeoTrackPropagate(&parL, hitZ, pdg, nullptr, nullptr) == kBMNERROR) continue;
            Float_t resX = parL.GetX() - hit->GetX() - GetDxMom(&parL) - dx[mod];
            Float_t resY = parL.GetY() - hit->GetY() - GetDyMom(&parL) - dy[mod];
            if (Abs(resX) > 3.0 * GetSigxMom(&parL) || Abs(resY) > 3.0 * GetSigxMom(&parL)) continue;
            if (Abs(resX) < minResX && Abs(resY) < minResY) {
                minResX = resX;
                minResY = resY;
                minIdx = iH;
            }
        }
        if (minIdx == -1) continue;

        FairTrackParam minParL = *(track->GetParamLast());
        BmnTofHit* minHit = (BmnTofHit*)fTof700Hits->At(minIdx);
        if (PropagateToZ(&minParL, minHit->GetZ(), pdg, fKalmanType, &length) == kFALSE) continue;
        Int_t mod = ((minHit->GetDetectorID() & 0x0000FF00) >> 8) - 1;
        Int_t strip = (minHit->GetDetectorID() & 0x000000FF) - 1;

        // fHistoManager->H1(Form("TOF700_ResidX_%d", mod))->Fill(minResX);
        // fHistoManager->H1(Form("TOF700_ResidY_%d", mod))->Fill(minResY);
        // fHistoManager->H2(Form("TOF700_dx_tx_%d", mod))->Fill(minParL.GetTx(), minResX);

        if (fIsField) {

            mom = 1.0 / parF.GetQp();
            Float_t time = minHit->GetTimeStamp() + timeShifts[mod][strip];
            Float_t beta = length / time / (TMath::C() * 1e-7);
            Float_t m2 = mom * mom * (1.0 / beta / beta - 1.0);

            fHistoManager->H2("TOF700_Banan_total")->Fill(mom, beta);
            fHistoManager->H2("TOF700_M2_total")->Fill(mom, m2);
            fHistoManager->H2("TOF700_dx_mom_total")->Fill(mom, minResX);
            fHistoManager->H2("TOF700_dy_mom_total")->Fill(mom, minResY);


            //cout << minHit->GetLength() << "    " << minHit->GetTimeStamp() << "    " << TMath::C() * 1e-7 << endl;
            //cout << minResX << "    " << minResY << "    " << mom << "    " << m2 << "    " << beta << endl;

            if (mom > 0.0 && mom < 5.0 && m2 >  0.5 && m2 < 1.5) { //p
                // fHistoManager->H1("TOF700_proton_ResidX_total")->Fill(resX);
                // fHistoManager->H1("TOF700_proton_ResidY_total")->Fill(resY);
                fHistoManager->H1(Form("TOF700_proton_ResidX_%d", mod))->Fill(minResX);
                fHistoManager->H1(Form("TOF700_proton_ResidY_%d", mod))->Fill(minResY);
                fHistoManager->H2("TOF700_proton_dx_mom_total")->Fill(mom, minResX);
                fHistoManager->H2("TOF700_proton_dy_mom_total")->Fill(mom, minResY);
                //fHistoManager->H2(Form("TOF700_proton_Banan_%d", mod))->Fill(mom, beta);
                // fHistoManager->H2("TOF700_proton_Mass2_modules")->Fill(mod, m2);
                // fHistoManager->H2(Form("TOF700_proton_Mass2_strips_%d", mod))->Fill(strip, m2);
                //fHistoManager->H2("TOF700_proton_Banan_total")->Fill(mom, beta);
                // fHistoManager->H2("TOF700_proton_Mass2_total")->Fill(mom, m2);
                // fHistoManager->H2("TOF700_proton_dx_tx_total")->Fill(par.GetTx(), resX);
                fHistoManager->H2(Form("TOF700_proton_dx_tx_%d", mod))->Fill(minParL.GetTx(), minResX);
            }

            if (mom > 0.0 && mom < 6.0 && m2 >  3.0 && m2 < 5.5) { //d
                // fHistoManager->H1("TOF700_deutron_ResidX_total")->Fill(resX);
                // fHistoManager->H1("TOF700_deutron_ResidY_total")->Fill(resY);
                fHistoManager->H1(Form("TOF700_deutron_ResidX_%d", mod))->Fill(minResX);
                fHistoManager->H1(Form("TOF700_deutron_ResidY_%d", mod))->Fill(minResY);
                //fHistoManager->H2(Form("TOF700_deutron_Banan_%d", mod))->Fill(mom, beta);
                fHistoManager->H2("TOF700_deutron_dx_mom_total")->Fill(mom, minResX);
                fHistoManager->H2("TOF700_deutron_dy_mom_total")->Fill(mom, minResY);
                // fHistoManager->H2("TOF700_deutron_Mass2_modules")->Fill(mod, m2);
                // fHistoManager->H2(Form("TOF700_deutron_Mass2_strips_%d", mod))->Fill(strip, m2);
                // fHistoManager->H2("TOF700_deutron_Banan_total")->Fill(mom, track->GetBeta(2));
                // fHistoManager->H2("TOF700_deutron_Mass2_total")->Fill(mom, m2);
                // fHistoManager->H2("TOF700_deutron_dx_tx_total")->Fill(par.GetTx(), resX);
                fHistoManager->H2(Form("TOF700_deutron_dx_tx_%d", mod))->Fill(minParL.GetTx(), minResX);
            }

            if (mom > 0.0 && mom < 3.5 && m2 > -0.2 && m2 < 0.2) { //pi+
                // fHistoManager->H1("TOF700_piPlus_ResidX_total")->Fill(resX);
                // fHistoManager->H1("TOF700_piPlus_ResidY_total")->Fill(resY);
                fHistoManager->H1(Form("TOF700_piPlus_ResidX_%d", mod))->Fill(minResX);
                fHistoManager->H1(Form("TOF700_piPlus_ResidY_%d", mod))->Fill(minResY);
                //fHistoManager->H2(Form("TOF700_piPlus_Banan_%d", mod))->Fill(mom, beta);
                fHistoManager->H2("TOF700_piPlus_dx_mom_total")->Fill(mom, minResX);
                fHistoManager->H2("TOF700_piPlus_dy_mom_total")->Fill(mom, minResY);
                // fHistoManager->H2("TOF700_piPlus_Mass2_modules")->Fill(mod, m2);
                // fHistoManager->H2(Form("TOF700_piPlus_Mass2_strips_%d", mod))->Fill(strip, m2);
                // fHistoManager->H2("TOF700_piPlus_Banan_total")->Fill(mom, track->GetBeta(2));
                // fHistoManager->H2("TOF700_piPlus_Mass2_total")->Fill(mom, m2);
                // fHistoManager->H2("TOF700_piPlus_dx_tx_total")->Fill(par.GetTx(), resX);
                fHistoManager->H2(Form("TOF700_piPlus_dx_tx_%d", mod))->Fill(minParL.GetTx(), minResX);
            }

            if (mom > -3.5 && mom < 0.0 && m2 > -0.2 && m2 < 0.2) { //pi-
                // fHistoManager->H1("TOF700_piMinus_ResidX_total")->Fill(resX);
                // fHistoManager->H1("TOF700_piMinus_ResidY_total")->Fill(resY);
                fHistoManager->H1(Form("TOF700_piMinus_ResidX_%d", mod))->Fill(minResX);
                fHistoManager->H1(Form("TOF700_piMinus_ResidY_%d", mod))->Fill(minResY);
                //fHistoManager->H2(Form("TOF700_piMinus_Banan_%d", mod))->Fill(mom, beta);
                fHistoManager->H2("TOF700_piMinus_dx_mom_total")->Fill(mom, minResX);
                fHistoManager->H2("TOF700_piMinus_dy_mom_total")->Fill(mom, minResY);
                // fHistoManager->H2("TOF700_piMinus_Mass2_modules")->Fill(mod, m2);
                // fHistoManager->H2(Form("TOF700_piMinus_Mass2_strips_%d", mod))->Fill(strip, m2);
                // fHistoManager->H2("TOF700_piMinus_Banan_total")->Fill(mom, track->GetBeta(2));
                // fHistoManager->H2("TOF700_piMinus_Mass2_total")->Fill(mom, m2);
                // fHistoManager->H2("TOF700_piMinus_dx_tx_total")->Fill(par.GetTx(), resX);
                fHistoManager->H2(Form("TOF700_piMinus_dx_tx_%d", mod))->Fill(minParL.GetTx(), minResX);
            }
        }
    }

    sw.Stop();
    fWorkTime += sw.RealTime();

}

void BmnTof700Qa::Finish() {

    TStopwatch sw;
    sw.Start();

    FairRootManager::Instance()->GetOutFile()->cd();
    TDirectory* tof700Dir = FairRootManager::Instance()->GetOutFile()->mkdir("TOF700");

    // for (Int_t i = 0; i < nModules; ++i) {
    //     //fHistoManager->H1(Form("TOF700_dx_tx_%d", i))->Fit("pol1", "QSR", "", -0.5, 0.5);
    //     //fHistoManager->H1(Form("TOF700_ResidX_%d", i))->Fit("gaus", "QSR", "", -2, 2);
    //     //fHistoManager->H1(Form("TOF700_ResidY_%d", i))->Fit("gaus", "QSR", "", -2, 2);
    // }

    tof700Dir->cd();
    fHistoManager->WriteToFile();

    sw.Stop();
    fWorkTime += sw.RealTime();
    printf("Work time of BmnTof700Qa: %4.2f sec.\n", fWorkTime);
}

void BmnTof700Qa::PropagateToZ(FairTrackParam* par, Double_t zDst, Int_t pdg, Int_t kalmanType) {
    if (kalmanType == 1) {//L1 KF
        CbmKFTrack kftr = CbmKFTrack(*par);
        kftr.SetPID(pdg);
        kftr.Extrapolate(zDst);
        kftr.GetTrackParam(*par);
    } else if (kalmanType == 2) { //Lebedev's KF
        fKalman->TGeoTrackPropagate(par, zDst, pdg, nullptr, nullptr);
    }
}

Bool_t BmnTof700Qa::PropagateToZ(FairTrackParam* par, Double_t zDst, Int_t pdg, Int_t kalmanType, Double_t* len) {
    Double_t lenPrev = *len;
    CbmKFTrack kftr = CbmKFTrack(*par);
    kftr.SetPID(pdg);
    Int_t n = 10; //number of steps
    Double_t zLength = zDst - par->GetZ();
    Double_t step = zLength / n;
    Double_t xPrev = par->GetX();
    Double_t yPrev = par->GetY();
    Double_t zPrev = par->GetZ();
    Double_t z = zPrev;
    for (Int_t i = 0; i < n; ++i) {
        z += step;
        if (kalmanType == 1) {//L1 KF
            kftr.Extrapolate(z);
            kftr.GetTrackParam(*par);
        } else if (kalmanType == 2) { //Lebedev's KF
            if (fKalman->TGeoTrackPropagate(par, z, pdg, nullptr, nullptr) == kBMNERROR)return kFALSE;
        }
        if (len)
            *len += Sqrt(Sq(par->GetX() - xPrev) + Sq(par->GetY() - yPrev) + Sq(par->GetZ() - zPrev));
        xPrev = par->GetX();
        yPrev = par->GetY();
        zPrev = par->GetZ();
    }
    if (Abs(*len - lenPrev) < 1.0) return kFALSE;
    else return kTRUE;
}

void BmnTof700Qa::CreateHistograms() {

    // fHistoManager->Create2 <TH2F>("Compare_X", "; X, L1; X, Leb", 400, -100.0, 100.0, 400, -100.0, 100.0);
    // fHistoManager->Create2 <TH2F>("Compare_Y", "; Y, L1; Y, Leb", 400, -100.0, 100.0, 400, -100.0, 100.0);
    // fHistoManager->Create2 <TH2F>("Compare_Tx", "; Tx, L1; Tx, Leb", 400, -1.0, 1.0, 400, -1.0, 1.0);
    // fHistoManager->Create2 <TH2F>("Compare_Ty", "; Ty, L1; Ty, Leb", 400, -1.0, 1.0, 400, -1.0, 1.0);
    // fHistoManager->Create2 <TH2F>("Compare_P", "; P, L1; P, Leb", 400, -5.0, 10.0, 400, -5.0, 10.0);

    fHistoManager->Create2 <TH2F>("TOF700_Banan_total", "Total #beta vs rigidity; rigidity, GeV/c/Q; #beta", 800, -5.0, 10.0, 800, 0.2, 1.1);
    fHistoManager->Create2 <TH2F>("TOF700_M2_total", "Total m^{2} vs rigidity; rigidity, GeV/c/Q; m^{2}, GeV/c^{2}", 800, -5.0, 10.0, 800, -0.5, 10);
    fHistoManager->Create2 <TH2F>("TOF700_dx_mom_total", "Total dX vs rigidity; rigidity, GeV/c/Q; dX, cm", 800, -5.0, 10.0, 800, -5.0, 5.0);
    fHistoManager->Create2 <TH2F>("TOF700_dy_mom_total", "Total dY vs rigidity; rigidity, GeV/c/Q; dY, cm", 800, -5.0, 10.0, 800, -5.0, 5.0);

    fHistoManager->Create2 <TH2F>("TOF700_proton_dx_mom_total", "Total dX vs rigidity; rigidity, GeV/c/Q; dX, cm", 800, -5.0, 10.0, 800, -5.0, 5.0);
    fHistoManager->Create2 <TH2F>("TOF700_proton_dy_mom_total", "Total dY vs rigidity; rigidity, GeV/c/Q; dY, cm", 800, -5.0, 10.0, 800, -5.0, 5.0);

    fHistoManager->Create2 <TH2F>("TOF700_deutron_dx_mom_total", "Total dX vs rigidity; rigidity, GeV/c/Q; dX, cm", 800, -5.0, 10.0, 800, -5.0, 5.0);
    fHistoManager->Create2 <TH2F>("TOF700_deutron_dy_mom_total", "Total dY vs rigidity; rigidity, GeV/c/Q; dY, cm", 800, -5.0, 10.0, 800, -5.0, 5.0);

    fHistoManager->Create2 <TH2F>("TOF700_piMinus_dx_mom_total", "Total dX vs rigidity; rigidity, GeV/c/Q; dX, cm", 800, -5.0, 10.0, 800, -5.0, 5.0);
    fHistoManager->Create2 <TH2F>("TOF700_piMinus_dy_mom_total", "Total dY vs rigidity; rigidity, GeV/c/Q; dY, cm", 800, -5.0, 10.0, 800, -5.0, 5.0);

    fHistoManager->Create2 <TH2F>("TOF700_piPlus_dx_mom_total", "Total dX vs rigidity; rigidity, GeV/c/Q; dX, cm", 800, -5.0, 10.0, 800, -5.0, 5.0);
    fHistoManager->Create2 <TH2F>("TOF700_piPlus_dy_mom_total", "Total dY vs rigidity; rigidity, GeV/c/Q; dY, cm", 800, -5.0, 10.0, 800, -5.0, 5.0);

    // fHistoManager->Create1 <TH1F>("TOF700_proton_ResidX_total", "Total residuals in X coordinate; dX, cm; N", 100, -5.0, 5.0);
    // fHistoManager->Create1 <TH1F>("TOF700_proton_ResidY_total", "Total residuals in Y coordinate; dY, cm; N", 100, -5.0, 5.0);
    // fHistoManager->Create2 <TH2F>("TOF700_proton_Banan_total", "Total #beta vs rigidity; rigidity, GeV/c/Q; #beta", 400, -5.0, 10.0, 400, 0.2, 1.1);
    // fHistoManager->Create2 <TH2F>("TOF700_proton_Mass2_total", "Total m^{2} vs rigidity; rigidity, GeV/c/Q; m^{2}", 400, -5.0, 10.0, 400, -0.5, 10.0);
    // fHistoManager->Create2 <TH2F>("TOF700_proton_dx_tx_total", "Total dX vs Tx; t_{x}; dx, cm", 400, -0.5, 0.5, 400, -5.0, 5.0);
    // fHistoManager->Create2 <TH2F>("TOF700_proton_Mass2_modules", "m^{2} vs module; module number; m^{2}", 60, 0, 60, 400, -0.5, 1.5);

    // fHistoManager->Create1 <TH1F>("TOF700_deutron_ResidX_total", "Total residuals in X coordinate; dX, cm; N", 100, -5.0, 5.0);
    // fHistoManager->Create1 <TH1F>("TOF700_deutron_ResidY_total", "Total residuals in Y coordinate; dY, cm; N", 100, -5.0, 5.0);
    // fHistoManager->Create2 <TH2F>("TOF700_deutron_Banan_total", "Total #beta vs rigidity; rigidity, GeV/c/Q; #beta", 400, -5.0, 10.0, 400, 0.2, 1.1);
    // fHistoManager->Create2 <TH2F>("TOF700_deutron_Mass2_total", "Total m^{2} vs rigidity; rigidity, GeV/c/Q; m^{2}", 400, -5.0, 10.0, 400, -0.5, 10.0);
    // fHistoManager->Create2 <TH2F>("TOF700_deutron_dx_tx_total", "Total dX vs Tx; t_{x}; dx, cm", 400, -0.5, 0.5, 400, -5.0, 5.0);
    // fHistoManager->Create2 <TH2F>("TOF700_deutron_Mass2_modules", "m^{2} vs module; module number; m^{2}", 60, 0, 60, 400, -0.5, 1.5);

    // fHistoManager->Create1 <TH1F>("TOF700_piMinus_ResidX_total", "Total residuals in X coordinate; dX, cm; N", 100, -5.0, 5.0);
    // fHistoManager->Create1 <TH1F>("TOF700_piMinus_ResidY_total", "Total residuals in Y coordinate; dY, cm; N", 100, -5.0, 5.0);
    // fHistoManager->Create2 <TH2F>("TOF700_piMinus_Banan_total", "Total #beta vs rigidity; rigidity, GeV/c/Q; #beta", 400, -5.0, 10.0, 400, 0.2, 1.1);
    // fHistoManager->Create2 <TH2F>("TOF700_piMinus_Mass2_total", "Total m^{2} vs rigidity; rigidity, GeV/c/Q; m^{2}", 400, -5.0, 10.0, 400, -0.5, 10.0);
    // fHistoManager->Create2 <TH2F>("TOF700_piMinus_dx_tx_total", "Total dX vs Tx; t_{x}; dx, cm", 400, -0.5, 0.5, 400, -5.0, 5.0);
    // fHistoManager->Create2 <TH2F>("TOF700_piMinus_Mass2_modules", "m^{2} vs module; module number; m^{2}", 60, 0, 60, 400, -0.5, 1.5);

    // fHistoManager->Create1 <TH1F>("TOF700_piPlus_ResidX_total", "Total residuals in X coordinate; dX, cm; N", 100, -5.0, 5.0);
    // fHistoManager->Create1 <TH1F>("TOF700_piPlus_ResidY_total", "Total residuals in Y coordinate; dY, cm; N", 100, -5.0, 5.0);
    // fHistoManager->Create2 <TH2F>("TOF700_piPlus_Banan_total", "Total #beta vs rigidity; rigidity, GeV/c/Q; #beta", 400, -5.0, 10.0, 400, 0.2, 1.1);
    // fHistoManager->Create2 <TH2F>("TOF700_piPlus_Mass2_total", "Total m^{2} vs rigidity; rigidity, GeV/c/Q; m^{2}", 400, -5.0, 10.0, 400, -0.5, 10.0);
    // fHistoManager->Create2 <TH2F>("TOF700_piPlus_dx_tx_total", "Total dX vs Tx; t_{x}; dx, cm", 400, -0.5, 0.5, 400, -5.0, 5.0);
    // fHistoManager->Create2 <TH2F>("TOF700_piPlus_Mass2_modules", "m^{2} vs module; module number; m^{2}", 60, 0, 60, 400, -0.5, 1.5);

    for (Int_t i = 0; i < nModules; ++i) {

        // fHistoManager->Create1 <TH1F>(Form("TOF700_ResidX_%d", i), Form("Residuals in X coordinate for module %d; dX, cm; N", i), 100, -5.0, 5.0);
        // fHistoManager->Create1 <TH1F>(Form("TOF700_ResidY_%d", i), Form("Residuals in Y coordinate for module %d; dY, cm; N", i), 100, -5.0, 5.0);
        // fHistoManager->Create2 <TH2F>(Form("TOF700_dx_tx_%d", i), Form("dX vs Tx for module %d; t_{x}; dx, cm", i), 400, -0.5, 0.5, 400, -5.0, 5.0);

        // fHistoManager->Create2 <TH2F>(Form("TOF700_proton_Mass2_strips_%d", i), Form("m^{2} vs strips for module %d; strip number; m^{2}", i), 32, 0, 32, 100, -0.5, 1.5);
        fHistoManager->Create1 <TH1F>(Form("TOF700_proton_ResidX_%d", i), Form("Residuals in X coordinate for module %d; dX, cm; N", i), 100, -5.0, 5.0);
        fHistoManager->Create1 <TH1F>(Form("TOF700_proton_ResidY_%d", i), Form("Residuals in Y coordinate for module %d; dY, cm; N", i), 100, -5.0, 5.0);
        fHistoManager->Create2 <TH2F>(Form("TOF700_proton_dx_tx_%d", i), Form("dX vs Tx for module %d; t_{x}; dx, cm", i), 400, -0.5, 0.5, 400, -5.0, 5.0);
        //fHistoManager->Create2 <TH2F>(Form("TOF700_proton_Banan_%d", i), Form("#beta vs rigidity for module %d; rigidity, GeV/c/Q; #beta", i), 400, -5.0, 10.0, 400, 0.2, 1.1);

        // fHistoManager->Create2 <TH2F>(Form("TOF700_deutron_Mass2_strips_%d", i), Form("m^{2} vs strips for module %d; strip number; m^{2}", i), 32, 0, 32, 100, -0.5, 1.5);
        fHistoManager->Create1 <TH1F>(Form("TOF700_deutron_ResidX_%d", i), Form("Residuals in X coordinate for module %d; dX, cm; N", i), 100, -5.0, 5.0);
        fHistoManager->Create1 <TH1F>(Form("TOF700_deutron_ResidY_%d", i), Form("Residuals in Y coordinate for module %d; dY, cm; N", i), 100, -5.0, 5.0);
        fHistoManager->Create2 <TH2F>(Form("TOF700_deutron_dx_tx_%d", i), Form("dX vs Tx for module %d; t_{x}; dx, cm", i), 400, -0.5, 0.5, 400, -5.0, 5.0);
        //fHistoManager->Create2 <TH2F>(Form("TOF700_deutron_Banan_%d", i), Form("#beta vs rigidity for module %d; rigidity, GeV/c/Q; #beta", i), 400, -5.0, 10.0, 400, 0.2, 1.1);

        // fHistoManager->Create2 <TH2F>(Form("TOF700_piMinus_Mass2_strips_%d", i), Form("m^{2} vs strips for module %d; strip number; m^{2}", i), 32, 0, 32, 100, -0.5, 1.5);
        fHistoManager->Create1 <TH1F>(Form("TOF700_piMinus_ResidX_%d", i), Form("Residuals in X coordinate for module %d; dX, cm; N", i), 100, -5.0, 5.0);
        fHistoManager->Create1 <TH1F>(Form("TOF700_piMinus_ResidY_%d", i), Form("Residuals in Y coordinate for module %d; dY, cm; N", i), 100, -5.0, 5.0);
        fHistoManager->Create2 <TH2F>(Form("TOF700_piMinus_dx_tx_%d", i), Form("dX vs Tx for module %d; t_{x}; dx, cm", i), 400, -0.5, 0.5, 400, -5.0, 5.0);
        //fHistoManager->Create2 <TH2F>(Form("TOF700_piMinus_Banan_%d", i), Form("#beta vs rigidity for module %d; rigidity, GeV/c/Q; #beta", i), 400, -5.0, 10.0, 400, 0.2, 1.1);

        // fHistoManager->Create2 <TH2F>(Form("TOF700_piPlus_Mass2_strips_%d", i), Form("m^{2} vs strips for module %d; strip number; m^{2}", i), 32, 0, 32, 100, -0.5, 1.5);
        fHistoManager->Create1 <TH1F>(Form("TOF700_piPlus_ResidX_%d", i), Form("Residuals in X coordinate for module %d; dX, cm; N", i), 100, -5.0, 5.0);
        fHistoManager->Create1 <TH1F>(Form("TOF700_piPlus_ResidY_%d", i), Form("Residuals in Y coordinate for module %d; dY, cm; N", i), 100, -5.0, 5.0);
        fHistoManager->Create2 <TH2F>(Form("TOF700_piPlus_dx_tx_%d", i), Form("dX vs Tx for module %d; t_{x}; dx, cm", i), 400, -0.5, 0.5, 400, -5.0, 5.0);
        //fHistoManager->Create2 <TH2F>(Form("TOF700_piPlus_Banan_%d", i), Form("#beta vs rigidity for module %d; rigidity, GeV/c/Q; #beta", i), 400, -5.0, 10.0, 400, 0.2, 1.1);
        //fHistoManager->Create1 <TH1F>(Form("TOF700_Mass2_%d", i), Form("m^{2} for module %d; rigidity, GeV/c/Q; m^{2}", i), 400, -0.5, 10.0);
    }
}

Float_t BmnTof700Qa::GetDxMom(FairTrackParam* par) {
    // DX PROTONS
    // 3.70/x^0.28-2.10
    // DX PI-MINUS
    //-7.61/(-x)^0.15+5.85

    Float_t mom = 1.0 / par->GetQp();

    return (mom > 0) ? 3.70 / TMath::Power(mom, 0.28) - 2.10 : -7.61 / TMath::Power(-1.0 * mom, 0.15) + 5.85;
}

Float_t BmnTof700Qa::GetDyMom(FairTrackParam* par) {
    // DY PROTONS
    // -1.56/x^0.94+0.18
    // DY PI-MINUS
    // +10.81/(-x)^0.05-9.53

    Float_t mom = 1.0 / par->GetQp();

    return (mom > 0) ? -1.56 / TMath::Power(mom, 0.94) + 0.18 : 10.81 / TMath::Power(-1.0 * mom, 0.05) - 9.53;
}

Float_t BmnTof700Qa::GetSigxMom(FairTrackParam* par) {
    // SIGMA DX DEUTERONS
    // 0.66/x^1.21+1.00
    // SIGMA DX PI-MINUS
    //-1.01/x^1.00+0.86

    Float_t mom = 1.0 / par->GetQp();

    return (mom > 0) ? 0.66 / TMath::Power(mom, 1.21) + 1.00 : -1.01 / TMath::Power(mom, 1.00) + 0.86;
}

Float_t BmnTof700Qa::GetSigyMom(FairTrackParam* par) {
    // SIGMA DY DEUTERONS
    // 0.98/x^1.20+0.57
    // SIGMA DY PI-MINUS
    // -0.87/x^1.00+0.58

    Float_t mom = 1.0 / par->GetQp();

    return (mom > 0) ? 0.98 / TMath::Power(mom, 1.20) + 0.57 : -0.87 / TMath::Power(mom, 1.00) + 0.58;
}
