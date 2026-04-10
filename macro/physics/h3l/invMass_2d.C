
#include "TFile.h"
#include "TH1F.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
// #include "BmnParticlePair.h"

// best without norming for dca2 and dca12
Float_t pathMin = 3.5;
Float_t pathMax = 16;
Float_t dca2Min = 0.95;
Float_t dca2Max = 30;
Float_t dca12Min = 0.0;
Float_t dca12Max = 0.5;
Float_t dedxMin = 700;
Float_t dedxMax = 5000;
Float_t mTOFMin = 1.6;
Float_t mTOFMax = 2.6;
Float_t mom1Min = 0.5;
Float_t mom1Max = 15.0;
Float_t mom2Min = -1.00;
Float_t mom2Max = -0.35;
Float_t m1m2Min = 3.8;
Float_t m1m2Max = 20;
Float_t dca0Min = 0.0;
Float_t dca0Max = 0.5;
Float_t dca1Min = 0.06;
Float_t dca1Max = 20.0;

Float_t invMassMin = 2.95;
Float_t invMassMax = 3.05;

Int_t nBins = 50;

void invMass_2d(TString inputFileName = "/zfs/store6.hydra.local/merts/h3l_pairs/h3l_pairs_8306_ev1_p9.root",
                TString outputFileName = "/zfs/store6.hydra.local/merts/h3l_plots/h3l_plots_7830_ev0_p17.root")
{

    if (gSystem->AccessPathName(inputFileName))
        return;

    TH1F* hInvMass = new TH1F("hInvMass", "; M; Count", nBins, invMassMin, invMassMax);
    TH2F* hM_p1 = new TH2F("hM_p1", "; Mom1; M", nBins, mom1Min, mom1Max, nBins, invMassMin, invMassMax);
    TH2F* hM_p2 = new TH2F("hM_p2", "; Mom2; M", nBins, mom2Min, mom2Max, nBins, invMassMin, invMassMax);
    TH2F* hM_dca0 = new TH2F("hM_dca0", "; DCA0; M", nBins, dca0Min, dca0Max, nBins, invMassMin, invMassMax);
    TH2F* hM_dca12 = new TH2F("hM_dca12", "; DCA12; M", nBins, 0, 1, nBins, invMassMin, invMassMax);
    TH2F* hM_dca1 = new TH2F("hM_dca1", "; DCA1; M", nBins, 0, 3, nBins, invMassMin, invMassMax);
    TH2F* hM_dca2 = new TH2F("hM_dca2", "; DCA2; M", nBins, 0, 4, nBins, invMassMin, invMassMax);
    TH2F* hM_path = new TH2F("hM_path", "; path; M", nBins, pathMin, pathMax, nBins, invMassMin, invMassMax);
    TH2F* hM_p1p2 = new TH2F("hM_p1p2", "; Mom1/Mom2; M", nBins, m1m2Min, m1m2Max, nBins, invMassMin, invMassMax);
    TH2F* hM_m400 = new TH2F("hM_m400", "; mass2 in tof400; M", nBins, 1.4, 2.6, nBins, invMassMin, invMassMax);
    TH2F* hM_dedx = new TH2F("hM_dedx", "; dE/dx; M", nBins, dedxMin, dedxMax, nBins, invMassMin, invMassMax);
    TH2F* hM_m700 = new TH2F("hM_m700", "; mass2 in tof700; M", nBins, 1.4, 2.6, nBins, invMassMin, invMassMax);
    TH2F* hM_nH1fsd = new TH2F("hM_nH1fsd", "; N FSD hits in track1; M", 5, 0, 5, nBins, invMassMin, invMassMax);
    TH2F* hM_nH2fsd = new TH2F("hM_nH2fsd", "; N FSD hits in track2; M", 5, 0, 5, nBins, invMassMin, invMassMax);
    TH2F* hM_nH1gem = new TH2F("hM_nH1gem", "; N GEM hits in track1; M", 8, 0, 8, nBins, invMassMin, invMassMax);
    TH2F* hM_nH2gem = new TH2F("hM_nH2gem", "; N GEM hits in track2; M", 8, 0, 8, nBins, invMassMin, invMassMax);
    TH2F* hM_dca12norm =
        new TH2F("hM_dca12norm", "; DCA12 / dDca12; M", nBins, dca12Min, dca12Max, nBins, invMassMin, invMassMax);
    TH2F* hM_dca1norm = new TH2F("hM_dca1norm", "; DCA1 / dDca1; M", nBins, 0, 10, nBins, invMassMin, invMassMax);
    TH2F* hM_dca2norm =
        new TH2F("hM_dca2norm", "; DCA2 / dDca2; M", nBins, dca2Min, dca2Max, nBins, invMassMin, invMassMax);
    TH2F* hM_chi1 = new TH2F("hM_chi1", "; #chi^{2}/NDF for track1; M", nBins, 0, 5, nBins, invMassMin, invMassMax);
    TH2F* hM_chi2 = new TH2F("hM_chi2", "; #chi^{2}/NDF for track2; M", nBins, 0, 5, nBins, invMassMin, invMassMax);
    TH2F* hM_v0x = new TH2F("hM_v0x", "; V0x, cm; M", nBins, -5, 5, nBins, invMassMin, invMassMax);
    TH2F* hM_v0y = new TH2F("hM_v0y", "; V0y, cm; M", nBins, -5, 5, nBins, invMassMin, invMassMax);
    TH2F* hM_v0z = new TH2F("hM_v0z", "; V0z, cm; M", nBins, -1, 30, nBins, invMassMin, invMassMax);
    TH2F* hTof400m2_dedx = new TH2F("hTof400m2_dedx", "; mass2 in tof400; dE/dx", 500, 0, 10, 500, dedxMin, dedxMax);
    TH2F* hTof700m2_dedx = new TH2F("hTof700m2_dedx", "; mass2 in tof700; dE/dx", 500, 0, 10, 500, dedxMin, dedxMax);
    TH2F* hP1_P2 = new TH2F("hP1_P2", "; Mom2; Mom1", 200, 0, 1, 200, 0, 10);

    cout << "File: " << inputFileName << endl;
    TFile* myFile = TFile::Open(inputFileName, "READ");

    TTreeReader myReader("bmndata", myFile);
    TTreeReaderArray<BmnParticlePair> pairs(myReader, "pairs");

    int evId = 0;

    while (myReader.Next()) {
        if (evId % 1000 == 0)
            printf("evId = %d\n", evId);
        evId++;
        for (BmnParticlePair pair : pairs) {

            Double_t m400 = pair.GetMass400Part1();
            Double_t m700 = pair.GetMass700Part1();
            Double_t dedx = pair.GetAlpha();
            hTof400m2_dedx->Fill(m400, dedx);
            hTof700m2_dedx->Fill(m700, dedx);
            Double_t massTof = -1000.0;
            if (m700 > 0)
                massTof = m700;
            else if (m400 > 0)
                massTof = m400;
            else
                continue;

            Double_t mass = pair.GetInvMass();
            if (mass > invMassMax)
                continue;
            if (mass < invMassMin)
                continue;

            Int_t nH1fsd = pair.GetNHitsPart1("SILICON");
            Int_t nH2fsd = pair.GetNHitsPart2("SILICON");
            Int_t nH1gem = pair.GetNHitsPart1("GEM");
            Int_t nH2gem = pair.GetNHitsPart2("GEM");
            // if (nH1fsd < 0) continue;
            if (nH2fsd < 1)
                continue;
            // if (nH1gem < 0) continue;
            if (nH2gem < 3)
                continue;

            Double_t mom1 = pair.GetMomPart1();
            Double_t mom2 = pair.GetMomPart2();
            Double_t m1m2 = mom1 / TMath::Abs(mom2);
            Double_t path = pair.GetPath();
            Double_t dca0 = pair.GetDCA0();
            Double_t dca12 = pair.GetDCA12();
            Double_t dca1 = pair.GetDCA1();
            Double_t dca2 = pair.GetDCA2();

            Double_t dca1norm = pair.GetDCA1() / pair.GetdDCA1();
            Double_t dca2norm = pair.GetDCA2() / pair.GetdDCA2();
            Double_t dca12norm = pair.GetDCA12() / pair.GetdDCA12();
            Double_t chi1 = pair.GetChi2Part1();
            Double_t chi2 = pair.GetChi2Part2();
            Double_t v0x = pair.GetV0X();
            Double_t v0y = pair.GetV0Y();
            Double_t v0z = pair.GetV0Z();

            if (path < pathMin || path > pathMax)
                continue;
            if (dca1norm < dca1Min || dca1norm > dca1Max)
                continue;
            if (dca2norm < dca2Min || dca2norm > dca2Max)
                continue;
            if (dca12norm < dca12Min || dca12norm > dca12Max)
                continue;
            // if (dedx < dedxMin || dedx > dedxMax) continue;
            if (massTof < mTOFMin || massTof > mTOFMax)
                continue;
            if (mom1 < mom1Min || mom1 > mom1Max)
                continue;
            if (mom2 < mom2Min || mom2 > mom2Max)
                continue;
            if (m1m2 < m1m2Min || m1m2 > m1m2Max)
                continue;
            if (dca0 < dca0Min || dca0 > dca0Max)
                continue;

            hInvMass->Fill(mass);
            hM_p1->Fill(mom1, mass);
            hM_p2->Fill(mom2, mass);
            hM_dca0->Fill(dca0, mass);
            hM_dca12->Fill(dca12, mass);
            hM_dca1->Fill(dca1, mass);
            hM_dca2->Fill(dca2, mass);
            hM_path->Fill(path, mass);
            hM_m400->Fill(m400, mass);
            hM_m700->Fill(m700, mass);
            hM_p1p2->Fill(m1m2, mass);
            hM_nH1fsd->Fill(nH1fsd, mass);
            hM_nH2fsd->Fill(nH2fsd, mass);
            hM_nH1gem->Fill(nH1gem, mass);
            hM_nH2gem->Fill(nH2gem, mass);
            hM_dca12norm->Fill(dca12norm, mass);
            hM_dca1norm->Fill(dca1norm, mass);
            hM_dca2norm->Fill(dca2norm, mass);
            hM_chi1->Fill(chi1, mass);
            hM_chi2->Fill(chi2, mass);
            hM_v0x->Fill(v0x, mass);
            hM_v0y->Fill(v0y, mass);
            hM_v0z->Fill(v0z, mass);
            hM_dedx->Fill(dedx, mass);
            hP1_P2->Fill(TMath::Abs(mom2), mom1);
        }
    }
    myFile->Close();

    TFile outFile(outputFileName, "RECREATE");
    hInvMass->Write();
    hM_p1->Write();
    hM_p2->Write();
    hM_dca0->Write();
    hM_dca12->Write();
    hM_dca1->Write();
    hM_dca2->Write();
    hM_path->Write();
    hM_p1p2->Write();
    hM_m400->Write();
    hM_m700->Write();
    hM_dedx->Write();
    hM_nH1fsd->Write();
    hM_nH2fsd->Write();
    hM_nH1gem->Write();
    hM_nH2gem->Write();
    hM_dca12norm->Write();
    hM_dca1norm->Write();
    hM_dca2norm->Write();
    hM_chi1->Write();
    hM_chi2->Write();
    hM_v0x->Write();
    hM_v0y->Write();
    hM_v0z->Write();
    hTof400m2_dedx->Write();
    hTof700m2_dedx->Write();
    hP1_P2->Write();

    outFile.Close();
}
