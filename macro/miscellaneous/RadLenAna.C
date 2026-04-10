/* Macro draws histograms of radiation length scan of the BM@N setup
 * Output result values are "effective radiation lengths" which are calculated
 * as radiation length (cm) divided by distance (cm).
 */

#include <TVector3.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TH2F.h>
#include "TROOT.h"
#include "TMath.h"
R__ADD_INCLUDE_PATH($VMCWORKDIR)

using namespace TMath;

void DrawHisto(TH2F* h, TString xName, TString yName);
void DrawHisto(TH1F* h, TString xName, TString yName, TCanvas* c);

void RadLenAna(TString infile1 = "RadLenSim.root") {

    const Float_t xMin = -500.0;
    const Float_t xMax = +500.0;
    const Int_t xNbin = 3000;
    const Float_t yMin = -400.0;
    const Float_t yMax = +400.0;
    const Int_t yNbin = 3000;
    const Float_t zMin = -500.0;
    const Float_t zMax = +1200.0;
    const Int_t zNbin = 4000;
    const Float_t rMin = 0.0;
    const Float_t rMax = +500.0;
    const Int_t rNbin = 3000;
    const Float_t distMin = 0.0;
    const Float_t distMax = +1200.0;
    const Int_t distNbin = 4000;
    const Float_t thetaMin = 0.0;
    const Float_t thetaMax = +180.0;
    const Int_t thetaNbin = 3000;
    const Float_t phiMin = -180.0;
    const Float_t phiMax = +180.0;
    const Int_t phiNbin = 3000;
    const Float_t etaMin = -10.0;
    const Float_t etaMax = +10.0;
    const Int_t etaNbin = 3000;

    TH2F* hXY = new TH2F("hXY", "Material budget in the BM@N", xNbin, xMin, xMax, yNbin, yMin, yMax);
    TH2F* hXZ = new TH2F("hXZ", "Material budget in the BM@N", zNbin, zMin, zMax, xNbin, xMin, xMax);
    TH2F* hXR = new TH2F("hXR", "Material budget in the BM@N", xNbin, xMin, xMax, rNbin, rMin, rMax);
    TH2F* hXDist = new TH2F("hXDist", "Material budget in the BM@N", xNbin, xMin, xMax, distNbin, distMin, distMax);
    TH2F* hXTheta = new TH2F("hXTheta", "Material budget in the BM@N", xNbin, xMin, xMax, thetaNbin, thetaMin, thetaMax);
    TH2F* hXPhi = new TH2F("hXPhi", "Material budget in the BM@N", xNbin, xMin, xMax, phiNbin, phiMin, phiMax);
    TH2F* hXEta = new TH2F("hXEta", "Material budget in the BM@N", xNbin, xMin, xMax, etaNbin, etaMin, etaMax);

    TH2F* hYZ = new TH2F("hYZ", "Material budget in the BM@N", zNbin, zMin, zMax, yNbin, yMin, yMax);
    TH2F* hYR = new TH2F("hYR", "Material budget in the BM@N", yNbin, yMin, yMax, rNbin, rMin, rMax);
    TH2F* hYDist = new TH2F("hYDist", "Material budget in the BM@N", yNbin, yMin, yMax, distNbin, distMin, distMax);
    TH2F* hYTheta = new TH2F("hYTheta", "Material budget in the BM@N", yNbin, yMin, yMax, thetaNbin, thetaMin, thetaMax);
    TH2F* hYPhi = new TH2F("hYPhi", "Material budget in the BM@N", yNbin, yMin, yMax, phiNbin, phiMin, phiMax);
    TH2F* hYEta = new TH2F("hYEta", "Material budget in the BM@N", yNbin, yMin, yMax, etaNbin, etaMin, etaMax);

    TH2F* hZR = new TH2F("hZR", "Material budget in the BM@N", zNbin, zMin, zMax, rNbin, rMin, rMax);
    TH2F* hZDist = new TH2F("hZDist", "Material budget in the BM@N", zNbin, zMin, zMax, distNbin, distMin, distMax);
    TH2F* hZTheta = new TH2F("hZTheta", "Material budget in the BM@N", zNbin, zMin, zMax, thetaNbin, thetaMin, thetaMax);
    TH2F* hZPhi = new TH2F("hZPhi", "Material budget in the BM@N", zNbin, zMin, zMax, phiNbin, phiMin, phiMax);
    TH2F* hZEta = new TH2F("hZEta", "Material budget in the BM@N", zNbin, zMin, zMax, etaNbin, etaMin, etaMax);

    TH2F* hRDist = new TH2F("hRDist", "Material budget in the BM@N", rNbin, rMin, rMax, distNbin, distMin, distMax);
    TH2F* hRTheta = new TH2F("hRTheta", "Material budget in the BM@N", rNbin, rMin, rMax, thetaNbin, thetaMin, thetaMax);
    TH2F* hRPhi = new TH2F("hRPhi", "Material budget in the BM@N", rNbin, rMin, rMax, phiNbin, phiMin, phiMax);
    TH2F* hREta = new TH2F("hREta", "Material budget in the BM@N", rNbin, rMin, rMax, etaNbin, etaMin, etaMax);

    TH2F* hDistTheta = new TH2F("hDistTheta", "Material budget in the BM@N", distNbin, distMin, distMax, thetaNbin, thetaMin, thetaMax);
    TH2F* hDistPhi = new TH2F("hDistPhi", "Material budget in the BM@N", distNbin, distMin, distMax, phiNbin, phiMin, phiMax);
    TH2F* hDistEta = new TH2F("hDistEta", "Material budget in the BM@N", distNbin, distMin, distMax, etaNbin, etaMin, etaMax);

    TH2F* hThetaPhi = new TH2F("hThetaPhi", "Material budget in the BM@N", thetaNbin, thetaMin, thetaMax, phiNbin, phiMin, phiMax);
    TH2F* hThetaEta = new TH2F("hThetaEta", "Material budget in the BM@N", thetaNbin, thetaMin, thetaMax, etaNbin, etaMin, etaMax);

    TH2F* hPhiEta = new TH2F("hPhiEta", "Material budget in the BM@N", phiNbin, phiMin, phiMax, etaNbin, etaMin, etaMax);

    // TH2F* hZX = new TH2F("hZX", "Material budget in the BM@N", 4000, -500., 1500., 3000, -500., 500.);
    // TH2F* hZY = new TH2F("hZY", "Material budget in the BM@N", 4000, -500., 1500., 3000, -500., 500.);
    // TH2F* hEtaDist = new TH2F("hEtaDist", "Material budget in the BM@N", 3000, -10., 10., 3000, 0., 1500.);



    //     TH2F* hZX_narrow = new TH2F("hZX_narrow", "Material budget in the BM@N, |y| < 10.0 cm", 4000, -500., 1500., 3000, -500., 500.);
    // TH2F* hZY_narrow = new TH2F("hZY_narrow", "Material budget in the BM@N, |y| < 10.0 cm", 4000, -500., 1500., 3000, -500., 500.);

    // const Int_t nBinsZ = 1000;
    // const Int_t nBinsEta = 100;
    // const Int_t nBinsTheta = 500;
    // const Int_t nBinsPhi = 500;

    // TH1F* hZ = new TH1F("hZ", "Material budget in the BM@N for |x| < 1.0 cm & |y| < 1.0 cm", nBinsZ, 0., 1000.);
    // TH1F* hZsum = new TH1F("hZsum", "Integrated material budget in the BM@N for |x| < 1.0 cm & |y| < 1.0 cm", nBinsZ, 0., 1000.);
    // Int_t aZ[nBinsZ];
    // for (Int_t i = 0; i < nBinsZ; ++i) aZ[i] = 0;

    // TH1F* hEta = new TH1F("hEta", "Material budget in the BM@N", nBinsEta, -2., 3.);
    // Int_t aEta[nBinsEta];
    // for (Int_t i = 0; i < nBinsEta; ++i) aEta[i] = 0;

    // Int_t aThetaPhi[nBinsPhi][nBinsTheta];
    // for (Int_t i = 0; i < nBinsPhi; ++i)
    //     for (Int_t j = 0; j < nBinsTheta; ++j)
    //         aThetaPhi[i][j] = 0;
    // TH2F* hThetaPhi = new TH2F("hThetaPhi", "Material budget in the BM@N", nBinsPhi, -180, 180, nBinsTheta, 0., 180.);

    // Int_t aTheta[nBinsEta];
    // for (Int_t i = 0; i < nBinsEta; ++i) aTheta[i] = 0;
    // TH1F* hTheta = new TH1F("hTheta", "Material budget in the BM@N", nBinsEta, 0., 180.);

    TChain* tree = new TChain("bmndata");
    tree->Add(infile1);

    // Activate branches
    TClonesArray* fRadLenPs = NULL;
    tree->SetBranchAddress("RadLen", &fRadLenPs);
    TClonesArray* fMCTracks = NULL;
    tree->SetBranchAddress("MCTrack", &fMCTracks);

    Int_t events = tree->GetEntries();
    cout << " Processing RadLenSimZR..." << endl;
    cout << " Number of events in file = " << events << endl;

    TVector3 PosIn, PosOut, DistVec;

    for (Int_t iEv = 0; iEv < events; iEv++) {
        tree->GetEntry(iEv);

        Int_t fNpoints = fRadLenPs->GetEntriesFast();

        for (Int_t pointIndex = 0; pointIndex < fNpoints; pointIndex++) {
            FairRadLenPoint* RadLenP = (FairRadLenPoint*)fRadLenPs->At(pointIndex);
            Float_t Len = RadLenP->GetRadLength();
            PosIn = RadLenP->GetPosition();
            PosOut = RadLenP->GetPositionOut();
            DistVec = PosIn - PosOut;
            Float_t Dist = DistVec.Mag();
            Float_t R = DistVec.Perp();
            Float_t LenEff = Dist / Len;

            Float_t X = PosIn.X();
            Float_t Y = PosIn.Y();
            Float_t Z = PosIn.Z();
            Float_t Theta = PosIn.Theta();
            Float_t Eta = -Log(Tan(Theta / 2));
            Float_t Phi = PosIn.Phi();
            Phi *= TMath::RadToDeg();
            Theta *= TMath::RadToDeg();

            // hXY->Fill(X, Y, LenEff);
            // //if (Abs(Y) < 10) hZX_narrow->Fill(Z, X, LenEff);
            // hZX->Fill(Z, X, LenEff);
            // //if (Abs(X) < 10) hZY_narrow->Fill(Z, Y, LenEff);
            // hZY->Fill(Z, Y, LenEff);
            // hEtaDist->Fill(Eta, PosIn.Mag(), LenEff);
            // hThetaPhi->Fill(Phi * TMath::RadToDeg(), Theta * TMath::RadToDeg(), LenEff);
            // //if (Abs(X) < 10 && Abs(Y) < 10) hZ->Fill(Z, LenEff);

            //LenEff = 1.0; // tmp

            hXY->Fill(X, Y, LenEff);
            hXZ->Fill(Z, X, LenEff);
            hXR->Fill(X, R, LenEff);
            hXDist->Fill(X, Dist, LenEff);
            hXTheta->Fill(X, Theta, LenEff);
            hXPhi->Fill(X, Phi, LenEff);
            hXEta->Fill(X, Eta, LenEff);
            hYZ->Fill(Z, Y, LenEff);
            hYR->Fill(Y, R, LenEff);
            hYDist->Fill(Y, Dist, LenEff);
            hYTheta->Fill(Y, Theta, LenEff);
            hYPhi->Fill(Y, Phi, LenEff);
            hYEta->Fill(Y, Eta, LenEff);
            hZR->Fill(Z, R, LenEff);
            hZDist->Fill(Z, Dist, LenEff);
            hZTheta->Fill(Z, Theta, LenEff);
            hZPhi->Fill(Z, Phi, LenEff);
            hZEta->Fill(Z, Eta, LenEff);
            hRDist->Fill(R, Dist, LenEff);
            hRTheta->Fill(R, Theta, LenEff);
            hRPhi->Fill(R, Phi, LenEff);
            hREta->Fill(R, Eta, LenEff);
            hDistTheta->Fill(Dist, Theta, LenEff);
            hDistPhi->Fill(Dist, Phi, LenEff);
            hDistEta->Fill(Dist, Eta, LenEff);
            hThetaPhi->Fill(Theta, Phi, LenEff);
            hThetaEta->Fill(Theta, Eta, LenEff);
            hPhiEta->Fill(Phi, Eta, LenEff);

            //if (pointIndex % 100000 == 0) cout << pointIndex * 100.0 / fNpoints << "% processed" << endl;
        }
        if (iEv % 5000 == 0) cout << iEv + 1 << " out of " << events << " events processed" << endl;
    }

    // for (Int_t iEv = 0; iEv < events; iEv++) {
    //     tree->GetEntry(iEv);

    //     for (Int_t iTr = 0; iTr < fMCTracks->GetEntriesFast(); iTr++) {

    //         CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iTr);
    //         Double_t sumEta = 0.0;
    //         Double_t sumRadLenEff = 0.0;
    //         Int_t nPnts = 0;
    //         Float_t Eta = 0.0;
    //         Float_t Phi = 0.0;
    //         Float_t Theta = 0.0;

    //         for (Int_t pointIndex = 0; pointIndex < fRadLenPs->GetEntriesFast(); pointIndex++) {
    //             FairRadLenPoint* RadLenP = (FairRadLenPoint*) fRadLenPs->At(pointIndex);
    //             if (RadLenP->GetTrackID() != iTr) continue;
    //             Float_t Len = RadLenP->GetRadLength();
    //             PosIn = RadLenP->GetPosition();
    //             PosOut = RadLenP->GetPositionOut();
    //             DistVec = PosIn - PosOut;
    //             Float_t Dist = DistVec.Mag();
    //             Float_t LenEff = Dist / Len;
    //             Float_t X = PosIn.X();
    //             Float_t Y = PosIn.Y();
    //             Float_t Z = PosIn.Z();
    //             Theta = PosIn.Theta();
    //             Eta = -Log(Tan(Theta / 2));
    //             Theta *= TMath::RadToDeg();
    //             Phi = PosIn.Phi() * TMath::RadToDeg();

    //             sumRadLenEff += LenEff;
    //             nPnts++;

    //             if (Abs(X) < 1 && Abs(Y) < 1) {
    //                 Int_t binZ = hZ->GetXaxis()->FindBin(Z);
    //                 aZ[binZ]++;
    //                 hZ->Fill(Z, LenEff);
    //             }
    //             //if (pointIndex % 10000 == 0) cout << pointIndex * 100.0 / fRadLenPs->GetEntriesFast() << "% processed" << endl;
    //         }
    //         if (nPnts > 0) {
    //             //                if (iTr % 10 == 0) cout << "n points on track " << iTr << ": " << nPnts << endl;
    //             Int_t binPhi2D = hThetaPhi->GetXaxis()->FindBin(Phi);
    //             Int_t binTheta2D = hThetaPhi->GetYaxis()->FindBin(Theta);
    //             Int_t binEta = hEta->GetXaxis()->FindBin(Eta);
    //             Int_t binTheta = hTheta->GetXaxis()->FindBin(Theta);

    //             aEta[binEta]++;
    //             aTheta[binTheta]++;
    //             hEta->Fill(Eta, sumRadLenEff);
    //             hTheta->Fill(Theta, sumRadLenEff);
    //             aThetaPhi[binPhi2D][binTheta2D]++;
    //             hThetaPhi->Fill(Phi, Theta, sumRadLenEff);
    //         }
    //     }
    //     if (iEv % 5000 == 0) cout << iEv + 1 << " out of " << events << " events processed" << endl;
    // }

    // hEta->SetBinContent(nBinsEta, 0.0);
    // hEta->SetBinError(nBinsEta, 0.0);
    // hTheta->SetBinContent(nBinsEta, 0.0);
    // hTheta->SetBinError(nBinsEta, 0.0);

    // hZ->SetBinContent(nBinsZ, 0.0);
    // hZ->SetBinError(nBinsZ, 0.0);
    // hZ->SetBinContent(0, 0.0);
    // hZ->SetBinError(0, 0.0);

    // Double_t integrZ = 0.0;
    // for (Int_t i = 0; i < nBinsZ; ++i) {
    //     if (aZ[i] != 0) {
    //         hZ->SetBinContent(i, hZ->GetBinContent(i) / aZ[i]);
    //         hZ->SetBinError(i, 0.0);
    //         integrZ += hZ->GetBinContent(i);
    //     }
    //     hZsum->SetBinContent(i, integrZ);
    // }

    // for (Int_t i = 0; i < nBinsEta; ++i) {
    //     if (aEta[i] != 0) {
    //         hEta->SetBinContent(i, hEta->GetBinContent(i) / aEta[i]);
    //         hEta->SetBinError(i, 0.0);
    //     }
    //     if (aTheta[i] != 0) {
    //         hTheta->SetBinContent(i, hTheta->GetBinContent(i) / aTheta[i]);
    //         hTheta->SetBinError(i, 0.0);
    //     }
    // }

    // for (Int_t i = 0; i < nBinsPhi; ++i) {
    //     hThetaPhi->SetBinContent(i, nBinsTheta, 0.0);
    //     hThetaPhi->SetBinError(i, nBinsTheta, 0.0);
    //     hThetaPhi->SetBinContent(i, 0, 0.0);
    //     hThetaPhi->SetBinError(i, 0, 0.0);
    // }
    // for (Int_t i = 0; i < nBinsTheta; ++i) {
    //     hThetaPhi->SetBinContent(nBinsPhi, i, 0.0);
    //     hThetaPhi->SetBinError(nBinsPhi, i, 0.0);
    //     hThetaPhi->SetBinContent(0, i, 0.0);
    //     hThetaPhi->SetBinError(0, i, 0.0);
    // }

    // for (Int_t i = 0; i < nBinsPhi; ++i)
    //     for (Int_t j = 0; j < nBinsTheta; ++j)
    //         if (aThetaPhi[i][j] != 0) {
    //             hThetaPhi->SetBinContent(i, j, hThetaPhi->GetBinContent(i, j) / aThetaPhi[i][j]);
    //             hThetaPhi->SetBinError(i, j, 0.0);
    //         }

    gStyle->SetOptStat(0);

    // TCanvas* c1 = new TCanvas(hXY->GetName(), hXY->GetName(), 800, 600);
    // DrawHisto(hXY, "X, cm", "Y, cm", c1);
    // TCanvas* c2 = new TCanvas(hZX->GetName(), hZX->GetName(), 1000, 800);
    // DrawHisto(hZX, "Z, cm", "X, cm", c2);
    // TCanvas* c3 = new TCanvas(hZX_narrow->GetName(), hZX_narrow->GetName(), 1000, 800);
    // DrawHisto(hZX_narrow, "Z, cm", "X, cm", c3);
    // TCanvas* c4 = new TCanvas(hZY->GetName(), hZY->GetName(), 1000, 600);
    // DrawHisto(hZY, "Z, cm", "Y, cm", c4);
    // TCanvas* c5 = new TCanvas(hZY_narrow->GetName(), hZY_narrow->GetName(), 1000, 600);
    // DrawHisto(hZY_narrow, "Z, cm", "Y, cm", c5);
    // TCanvas* c6 = new TCanvas(hEtaDist->GetName(), hEtaDist->GetName(), 1000, 500);
    // DrawHisto(hEtaDist, "#eta", "Distance from IP, cm", c6);


    // TCanvas* c7 = new TCanvas(hZ->GetName(), hZ->GetName(), 1000, 500);
    // DrawHisto(hZ, "Z, cm", "Radiation length, X/X0", c7);
    // TCanvas* c71 = new TCanvas(hZsum->GetName(), hZsum->GetName(), 1000, 500);
    // DrawHisto(hZsum, "Z, cm", "Integrated radiation length, X/X0", c71);

    // TCanvas* c8 = new TCanvas(hEta->GetName(), hEta->GetName(), 1000, 1000);
    // DrawHisto(hEta, "#eta", "Radiation length, X/X0", c8);
    // TCanvas* c9 = new TCanvas(hThetaPhi->GetName(), hThetaPhi->GetName(), 1000, 1000);
    // DrawHisto(hThetaPhi, "#phi, deg", "#theta, deg", c9);
    // TCanvas* c10 = new TCanvas(hTheta->GetName(), hTheta->GetName(), 1000, 1000);
    // DrawHisto(hTheta, "#theta, deg", "Radiation length, X/X0", c10);

    DrawHisto(hXY, "X, cm", "Y, cm");
    DrawHisto(hXZ, "Z, cm", "X, cm");
    DrawHisto(hXR, "X, cm", "R, cm");
    DrawHisto(hXDist, "X, cm", "Distance from IP, cm");
    DrawHisto(hXTheta, "X, cm", "#theta, deg");
    DrawHisto(hXPhi, "X, cm", "#phi, deg");
    DrawHisto(hXEta, "X, cm", "#eta");
    DrawHisto(hYZ, "Z, cm", "Y, cm");
    DrawHisto(hYR, "Y, cm", "R, cm");
    DrawHisto(hYDist, "Y, cm", "Distance from IP, cm");
    DrawHisto(hYTheta, "Y, cm", "#theta, deg");
    DrawHisto(hYPhi, "Y, cm", "#phi, deg");
    DrawHisto(hYEta, "Y, cm", "#eta");
    DrawHisto(hZR, "Z, cm", "R, cm");
    DrawHisto(hZDist, "Z, cm", "Distance from IP, cm");
    DrawHisto(hZTheta, "Z, cm", "#theta, deg");
    DrawHisto(hZPhi, "Z, cm", "#phi, deg");
    DrawHisto(hZEta, "Z, cm", "#eta");
    DrawHisto(hRDist, "R, cm", "Distance from IP, cm");
    DrawHisto(hRTheta, "R, cm", "#theta, deg");
    DrawHisto(hRPhi, "R, cm", "#phi, deg");
    DrawHisto(hREta, "R, cm", "#eta");
    DrawHisto(hDistTheta, "Distance from IP, cm", "#theta, deg");
    DrawHisto(hDistPhi, "Distance from IP, cm", "#phi, deg");
    DrawHisto(hDistEta, "Distance from IP, cm", "#eta");
    DrawHisto(hThetaPhi, "#theta, deg", "#phi, deg");
    DrawHisto(hThetaEta, "#theta, deg", "#eta");
    DrawHisto(hPhiEta, "#phi, deg", "#eta");

}

void DrawHisto(TH2F* h, TString xName, TString yName) {
    TCanvas* c = new TCanvas(h->GetName(), h->GetName(), 1000, 1000);
    h->GetXaxis()->SetTitle(xName);
    h->GetXaxis()->SetTitleOffset(0.8);
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->SetTitle(yName);
    h->GetYaxis()->SetTitleOffset(0.8);
    h->GetYaxis()->CenterTitle();
    h->GetZaxis()->SetTitle("Radiation length, X/X0");
    h->GetZaxis()->SetTitleOffset(0.7);
    //    c->SetLogz();
    //    h->SetMaximum(10.);
    h->Draw("colz");
    c->SaveAs(Form("%s.pdf", h->GetName()));
}

void DrawHisto(TH1F* h, TString xName, TString yName, TCanvas* c) {
    h->GetXaxis()->SetTitle(xName);
    h->GetXaxis()->SetTitleOffset(0.8);
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->SetTitle(yName);
    h->GetYaxis()->SetTitleOffset(0.8);
    h->GetYaxis()->CenterTitle();
    //    h->SetMaximum(10.);
    h->Draw("L");
    c->SaveAs(Form("%s.pdf", h->GetName()));
}
