
#include "TFile.h"
#include "TH1F.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
// #include "BmnParticlePair.h"

Float_t invMassMin = 2.95;
Float_t invMassMax = 3.05;

// //WORKING CODE FOR SCAN BY N HITS
// Float_t pathMin = 3.0;
// Float_t pathMax = 16;
// Float_t dca2Min = 1.2;
// Float_t dca2Max = 30;
// Float_t dca12Min = 0.00;
// Float_t dca12Max = 0.50;
// Float_t dedxMin = 700;
// Float_t dedxMax = 5000;
// Float_t mTOFMin = 1.6;
// Float_t mTOFMax = 2.6;
// Float_t mom1Min = 0.5;
// Float_t mom1Max = 15;
// Float_t mom2Min = -1.0;
// Float_t mom2Max = -0.35;
// Float_t dca0Min = 0;
// Float_t dca0Max = 0.5;
// Float_t dca1Min = 0.00;
// Float_t dca1Max = 20.0;

// const Int_t nFsd1 = 5;
// vector<Int_t> nFsd1Min = { 0, 1, 2, 3, 4 };
// vector<Int_t> nFsd1Max = { 4, 4, 4, 4, 4 };
// const Int_t nFsd2 = 5;
// vector<Int_t> nFsd2Min = { 0, 1, 2, 3, 4 };
// vector<Int_t> nFsd2Max = { 4, 4, 4, 4, 4 };
// const Int_t nGem1 = 8;
// vector<Int_t> nGem1Min = { 0, 1, 2, 3, 4, 5, 6, 7 };
// vector<Int_t> nGem1Max = { 7, 7, 7, 7, 7, 7, 7, 7 };
// const Int_t nGem2 = 8;
// vector<Int_t> nGem2Min = { 0, 1, 2, 3, 4, 5, 6, 7 };
// vector<Int_t> nGem2Max = { 7, 7, 7, 7, 7, 7, 7, 7 };

// void h3lAna(TString inputFileName = "/zfs/store6.hydra.local/merts/h3l_pairs/h3l_pairs_7830_ev0_p17.root", TString
// outputFileName = "/zfs/store6.hydra.local/merts/h3l_plots/h3l_plots_7830_ev0_p17.root") {

//     if (gSystem->AccessPathName(inputFileName)) return;

//     TH1F* hInvMass[nFsd1][nFsd2][nGem1][nGem2];
//     for (Int_t iFsd1 = 0; iFsd1 < nFsd1; iFsd1++) {
//         for (Int_t iFsd2 = 0; iFsd2 < nFsd2; iFsd2++) {
//             for (Int_t iGem1 = 0; iGem1 < nGem1; iGem1++) {
//                 for (Int_t iGem2 = 0; iGem2 < nGem2; iGem2++) {
//                     hInvMass[iFsd1][iFsd2][iGem1][iGem2] = new TH1F(Form("hInvMass_%d_%d_%d_%d", iFsd1, iFsd2, iGem1,
//                     iGem2), "; M; Count", 100, invMassMin, invMassMax);
//                 }
//             }
//         }
//     }

//     cout << "File: " << inputFileName << endl;
//     TFile* myFile = TFile::Open(inputFileName);

//     TTreeReader myReader("bmndata", myFile);
//     TTreeReaderArray<BmnParticlePair> pairs(myReader, "pairs");

//     int evId = 0;

//     while (myReader.Next()) {
//         if (evId % 1000 == 0) printf("evId = %d\n", evId);
//         evId++;
//         for (BmnParticlePair pair : pairs) {
//             Double_t mass = pair.GetInvMass();
//             if (mass > invMassMax) continue;
//             if (mass < invMassMin) continue;

//             Double_t mom1 = pair.GetMomPart1();
//             Double_t mom2 = pair.GetMomPart2();
//             Double_t m1m2 = mom1 / TMath::Abs(mom2);
//             if (m1m2 < 3.8) continue;

//             Double_t m400 = pair.GetMass400Part1();
//             Double_t m700 = pair.GetMass700Part1();
//             Double_t massTof = -1000.0;
//             if (m700 > 0) massTof = m700;
//             else if (m400 > 0) massTof = m400;
//             else continue;

//             Double_t path = pair.GetPath();
//             Double_t dca0 = pair.GetDCA0();
//             Double_t dca1/*dca1norm*/ = pair.GetDCA1() / pair.GetdDCA1();
//             Double_t dca2/*dca2norm*/ = pair.GetDCA2() / pair.GetdDCA2();
//             Double_t dca12/*dca12norm*/ = pair.GetDCA12() / pair.GetdDCA12();
//             Double_t dedx = pair.GetAlpha();

//             if (path < pathMin || path > pathMax) continue;
//             if (dca1 < dca1Min || dca1 > dca1Max) continue;
//             if (dca2 < dca2Min || dca2 > dca2Max) continue;
//             if (dca12 < dca12Min || dca12 > dca12Max) continue;
//             //if (dedx < dedxMin || dedx > dedxMax) continue;
//             if (massTof < mTOFMin || massTof > mTOFMax) continue;
//             if (mom1 < mom1Min || mom1 > mom1Max) continue;
//             if (mom2 < mom2Min || mom2 > mom2Max) continue;
//             if (dca0 < dca0Min || dca0 > dca0Max) continue;

//             Int_t nH1fsd = pair.GetNHitsPart1("SILICON");
//             Int_t nH2fsd = pair.GetNHitsPart2("SILICON");
//             Int_t nH1gem = pair.GetNHitsPart1("GEM");
//             Int_t nH2gem = pair.GetNHitsPart2("GEM");

//             for (Int_t iFsd1 = 0; iFsd1 < nFsd1; iFsd1++) {
//                 if (nH1fsd < nFsd1Min[iFsd1] || nH1fsd > nFsd1Max[iFsd1]) continue;
//                 for (Int_t iFsd2 = 0; iFsd2 < nFsd2; iFsd2++) {
//                     if (nH2fsd < nFsd2Min[iFsd2] || nH2fsd > nFsd2Max[iFsd2]) continue;
//                     for (Int_t iGem1 = 0; iGem1 < nGem1; iGem1++) {
//                         if (nH1gem < nGem1Min[iGem1] || nH1gem > nGem1Max[iGem1]) continue;
//                         for (Int_t iGem2 = 0; iGem2 < nGem2; iGem2++) {
//                             if (nH2gem < nGem2Min[iGem2] || nH2gem > nGem2Max[iGem2]) continue;
//                             hInvMass[iFsd1][iFsd2][iGem1][iGem2]->Fill(mass);
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     myFile->Close();

//     TFile outFile(outputFileName, "RECREATE");
//     for (Int_t iFsd1 = 0; iFsd1 < nFsd1; iFsd1++) {
//         for (Int_t iFsd2 = 0; iFsd2 < nFsd2; iFsd2++) {
//             for (Int_t iGem1 = 0; iGem1 < nGem1; iGem1++) {
//                 for (Int_t iGem2 = 0; iGem2 < nGem2; iGem2++) {
//                     hInvMass[iFsd1][iFsd2][iGem1][iGem2]->Write();
//                 }
//             }
//         }
//     }
//     outFile.Close();
// }

// WORKING CODE FOR SCAN BY STANDART PARAMETERS
// GIVE BEST RESULTS WITH OLD PAIRS (IN separate folder pairs_by_mass_and_dedx)
// BEST PARAMETERS FOR TOF700 (7/10/24)
//  const Int_t nPath = 1;
//  vector<Float_t> pathMin = { 3.0 };
//  vector<Float_t> pathMax = { 16 };
//  const Int_t nDca2 = 1;
//  vector<Float_t> dca2Min = { 1.40 };
//  vector<Float_t> dca2Max = { 30.0 };
//  const Int_t nDca12 = 1;
//  vector<Float_t> dca12Min = { 0.01 };
//  vector<Float_t> dca12Max = { 0.35 };
//  const Int_t nDedx = 1;
//  vector<Float_t> dedxMin = { 700 };
//  vector<Float_t> dedxMax = { 5000 };
//  const Int_t nMTOF = 1;
//  vector<Float_t> mTOFMin = { 1.4 };
//  vector<Float_t> mTOFMax = { 2.6 };
//  const Int_t nMom1 = 1;
//  vector<Float_t> mom1Min = { 0.50 };
//  vector<Float_t> mom1Max = { 8.0 };
//  const Int_t nMom2 = 1;
//  vector<Float_t> mom2Min = { -1.00 };
//  vector<Float_t> mom2Max = { -0.35 };
//  const Int_t nDca1 = 1;
//  vector<Float_t> dca1Min = { 0.00};
//  vector<Float_t> dca1Max = { 20.0};
//  const Int_t nDca0 = 1;
//  vector<Float_t> dca0Min = { 0.0 };
//  vector<Float_t> dca0Max = { 0.6 };

const Int_t nPath = 1;
vector<Float_t> pathMin = {3.7};
vector<Float_t> pathMax = {16};
const Int_t nDca2 = 1;
vector<Float_t> dca2Min = {1.20};
vector<Float_t> dca2Max = {30.0};
const Int_t nDca12 = 1;
vector<Float_t> dca12Min = {0.0};
vector<Float_t> dca12Max = {0.50};
const Int_t nDedx = 1;
vector<Float_t> dedxMin = {700};
vector<Float_t> dedxMax = {5000};
const Int_t nMTOF = 1;
vector<Float_t> mTOFMin = {1.6};
vector<Float_t> mTOFMax = {2.6};
const Int_t nMom1 = 1;
vector<Float_t> mom1Min = {2.00};
vector<Float_t> mom1Max = {15.0};
const Int_t nMom2 = 1;
vector<Float_t> mom2Min = {-0.90};
vector<Float_t> mom2Max = {-0.36};
const Int_t nDca1 = 1;
vector<Float_t> dca1Min = {0.04};
vector<Float_t> dca1Max = {12.0};
const Int_t nDca0 = 1;
vector<Float_t> dca0Min = {0.02};
vector<Float_t> dca0Max = {0.50};

void h3lAna(TString inputFileName = "/zfs/store6.hydra.local/merts/h3l_pairs/h3l_pairs_7830_ev0_p17.root",
            TString outputFileName = "/zfs/store6.hydra.local/merts/h3l_plots/h3l_plots_7830_ev0_p17.root")
{

    if (gSystem->AccessPathName(inputFileName))
        return;

    TH1F* hInvMass[nPath][nDca2][nDca12][nDedx][nMTOF][nMom1][nMom2][nDca1][nDca0];
    for (Int_t iPath = 0; iPath < nPath; iPath++) {
        for (Int_t iDca2 = 0; iDca2 < nDca2; iDca2++) {
            for (Int_t iDca12 = 0; iDca12 < nDca12; iDca12++) {
                for (Int_t iDedx = 0; iDedx < nDedx; iDedx++) {
                    for (Int_t iMTOF = 0; iMTOF < nMTOF; iMTOF++) {
                        for (Int_t iMom1 = 0; iMom1 < nMom1; iMom1++) {
                            for (Int_t iMom2 = 0; iMom2 < nMom2; iMom2++) {
                                for (Int_t iDca1 = 0; iDca1 < nDca1; iDca1++) {
                                    for (Int_t iDca0 = 0; iDca0 < nDca0; iDca0++) {
                                        hInvMass[iPath][iDca2][iDca12][iDedx][iMTOF][iMom1][iMom2][iDca1][iDca0] =
                                            new TH1F(Form("hInvMass_%d_%d_%d_%d_%d_%d_%d_%d_%d",
                                                          iPath,
                                                          iDca2,
                                                          iDca12,
                                                          iDedx,
                                                          iMTOF,
                                                          iMom1,
                                                          iMom2,
                                                          iDca1,
                                                          iDca0),
                                                     "; M; Count",
                                                     100,
                                                     invMassMin,
                                                     invMassMax);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cout << "File: " << inputFileName << endl;
    TFile* myFile = TFile::Open(inputFileName);

    TTreeReader myReader("bmndata", myFile);
    TTreeReaderArray<BmnParticlePair> pairs(myReader, "pairs");

    int evId = 0;

    while (myReader.Next()) {
        if (evId % 1000 == 0)
            printf("evId = %d\n", evId);
        evId++;
        for (BmnParticlePair pair : pairs) {
            Double_t mass = pair.GetInvMass();
            if (mass > invMassMax)
                continue;
            if (mass < invMassMin)
                continue;

            Double_t m400 = pair.GetMass400Part1();
            Double_t m700 = pair.GetMass700Part1();
            Double_t massTof = -1000.0;
            if (m700 > 0)
                massTof = m700;
            else if (m400 > 0)
                massTof = m400;
            else
                continue;

            Int_t nH1fsd = pair.GetNHitsPart1("SILICON");
            Int_t nH2fsd = pair.GetNHitsPart2("SILICON");
            Int_t nH1gem = pair.GetNHitsPart1("GEM");
            Int_t nH2gem = pair.GetNHitsPart2("GEM");
            // if (nH1fsd > 0) continue;
            if (nH2fsd < 1)
                continue;
            // if (nH1gem < 4) continue;
            if (nH2gem < 3)
                continue;

            Double_t mom1 = pair.GetMomPart1();
            Double_t mom2 = pair.GetMomPart2();
            Double_t m1m2 = mom1 / TMath::Abs(mom2);
            if (m1m2 < 3.8)
                continue;

            Double_t path = pair.GetPath();
            Double_t dca0 = pair.GetDCA0();
            // Double_t dca12 = pair.GetDCA12();
            // Double_t dca1 = pair.GetDCA1();
            // Double_t dca2 = pair.GetDCA2();
            Double_t dca1 /*dca1norm*/ = pair.GetDCA1() / pair.GetdDCA1();
            Double_t dca2 /*dca2norm*/ = pair.GetDCA2() / pair.GetdDCA2();
            Double_t dca12 /*dca12norm*/ = pair.GetDCA12() / pair.GetdDCA12();
            // Double_t chi1 = pair.GetChi2Part1();
            // Double_t chi2 = pair.GetChi2Part2();
            // Double_t v0x = pair.GetV0X();
            // Double_t v0y = pair.GetV0Y();
            // Double_t v0z = pair.GetV0Z();
            Double_t dedx = 1000;   // just for test  pair.GetAlpha();

            for (Int_t iPath = 0; iPath < nPath; iPath++) {
                if (path < pathMin[iPath] || path > pathMax[iPath])
                    continue;
                for (Int_t iDca2 = 0; iDca2 < nDca2; iDca2++) {
                    if (dca2 < dca2Min[iDca2] || dca2 > dca2Max[iDca2])
                        continue;
                    for (Int_t iDca12 = 0; iDca12 < nDca12; iDca12++) {
                        if (dca12 < dca12Min[iDca12] || dca12 > dca12Max[iDca12])
                            continue;
                        for (Int_t iDedx = 0; iDedx < nDedx; iDedx++) {
                            if (dedx < dedxMin[iDedx] || dedx > dedxMax[iDedx])
                                continue;
                            for (Int_t iMTOF = 0; iMTOF < nMTOF; iMTOF++) {
                                if (massTof < mTOFMin[iMTOF] || massTof > mTOFMax[iMTOF])
                                    continue;
                                for (Int_t iMom1 = 0; iMom1 < nMom1; iMom1++) {
                                    if (mom1 < mom1Min[iMom1] || mom1 > mom1Max[iMom1])
                                        continue;
                                    for (Int_t iMom2 = 0; iMom2 < nMom2; iMom2++) {
                                        if (mom2 < mom2Min[iMom2] || mom2 > mom2Max[iMom2])
                                            continue;
                                        for (Int_t iDca1 = 0; iDca1 < nDca1; iDca1++) {
                                            if (dca1 < dca1Min[iDca1] || dca1 > dca1Max[iDca1])
                                                continue;
                                            for (Int_t iDca0 = 0; iDca0 < nDca0; iDca0++) {
                                                if (dca0 < dca0Min[iDca0] || dca0 > dca0Max[iDca0])
                                                    continue;
                                                hInvMass[iPath][iDca2][iDca12][iDedx][iMTOF][iMom1][iMom2][iDca1][iDca0]
                                                    ->Fill(mass);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    myFile->Close();

    TFile outFile(outputFileName, "RECREATE");
    for (Int_t iPath = 0; iPath < nPath; iPath++) {
        for (Int_t iDca2 = 0; iDca2 < nDca2; iDca2++) {
            for (Int_t iDca12 = 0; iDca12 < nDca12; iDca12++) {
                for (Int_t iDedx = 0; iDedx < nDedx; iDedx++) {
                    for (Int_t iMTOF = 0; iMTOF < nMTOF; iMTOF++) {
                        for (Int_t iMom1 = 0; iMom1 < nMom1; iMom1++) {
                            for (Int_t iMom2 = 0; iMom2 < nMom2; iMom2++) {
                                for (Int_t iDca1 = 0; iDca1 < nDca1; iDca1++) {
                                    for (Int_t iDca0 = 0; iDca0 < nDca0; iDca0++) {
                                        hInvMass[iPath][iDca2][iDca12][iDedx][iMTOF][iMom1][iMom2][iDca1][iDca0]
                                            ->Write();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    outFile.Close();
}

// const Int_t nOmega = 10;
// vector<Float_t> omegaMin = { 0.1, 0.34, 0.5, 0.75, 1.0, 2.0, 3.0, 5.0, 7.5, 10.0 };
// vector<Float_t> omegaMax = { 100, 100, 100 ,100, 100, 100, 100, 100, 100, 100 };
// const Int_t nPath = 1;
// vector<Float_t> pathMin = { 3.0 };
// vector<Float_t> pathMax = { 18 };
// const Int_t nDedx = 1;
// //dedx >200 => maxS, dedx >600 => maxSIGN
// vector<Float_t> dedxMin = { 600 };
// vector<Float_t> dedxMax = { 5000 };
// const Int_t nMTOF = 1;
// vector<Float_t> mTOFMin = { 1.8 };
// vector<Float_t> mTOFMax = { 2.6 };
// const Int_t nMom1 = 1;
// vector<Float_t> mom1Min = { 0.50 };
// vector<Float_t> mom1Max = { 15.0 };
// const Int_t nMom2 = 1;
// vector<Float_t> mom2Min = { -1.00 };
// vector<Float_t> mom2Max = { -0.18 };
// const Int_t nDca0 = 1;
// vector<Float_t> dca0Min = { 0.0 };
// vector<Float_t> dca0Max = { 4.0 };

// void h3lAna(TString inputFileName = "/zfs/store6.hydra.local/merts/h3l_pairs/h3l_pairs_7830_ev0_p17.root", TString
// outputFileName = "/zfs/store6.hydra.local/merts/h3l_plots/h3l_plots_7830_ev0_p17.root") {

//     if (gSystem->AccessPathName(inputFileName)) return;

//     TH1F* hInvMass[nOmega][nPath][nDedx][nMTOF][nMom1][nMom2][nDca0];
//     for (Int_t iOmega = 0; iOmega < nOmega; iOmega++) {
//         for (Int_t iPath = 0; iPath < nPath; iPath++) {
//             for (Int_t iDedx = 0; iDedx < nDedx; iDedx++) {
//                 for (Int_t iMTOF = 0; iMTOF < nMTOF; iMTOF++) {
//                     for (Int_t iMom1 = 0; iMom1 < nMom1; iMom1++) {
//                         for (Int_t iMom2 = 0; iMom2 < nMom2; iMom2++) {
//                             for (Int_t iDca0 = 0; iDca0 < nDca0; iDca0++) {
//                                 hInvMass[iOmega][iPath][iDedx][iMTOF][iMom1][iMom2][iDca0] = new
//                                 TH1F(Form("hInvMass_%d_%d_%d_%d_%d_%d_%d", iOmega, iPath, iDedx, iMTOF, iMom1, iMom2,
//                                 iDca0), "; M; Count", 100, invMassMin, invMassMax);
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     cout << "File: " << inputFileName << endl;
//     TFile* myFile = TFile::Open(inputFileName);

//     TTreeReader myReader("bmndata", myFile);
//     TTreeReaderArray<BmnParticlePair> pairs(myReader, "pairs");

//     int evId = 0;

//     while (myReader.Next()) {
//         if (evId % 1000 == 0) printf("evId = %d\n", evId);
//         evId++;
//         for (BmnParticlePair pair : pairs) {
//             Double_t mass = pair.GetInvMass();
//             if (mass > invMassMax) continue;
//             if (mass < invMassMin) continue;

//             Double_t m400 = pair.GetMass400Part1();
//             Double_t m700 = pair.GetMass700Part1();
//             Double_t massTof = -1000.0;
//             if (m700 > 0) massTof = m700;
//             else if (m400 > 0) massTof = m400;
//             else continue;

//             Int_t nH1fsd = pair.GetNHitsPart1("SILICON");
//             Int_t nH2fsd = pair.GetNHitsPart2("SILICON");
//             Int_t nH1gem = pair.GetNHitsPart1("GEM");
//             Int_t nH2gem = pair.GetNHitsPart2("GEM");
//             if (nH1fsd < 0) continue;
//             if (nH2fsd < 2) continue;
//             if (nH1gem < 0) continue;
//             if (nH2gem < 3) continue;

//             Double_t mom1 = pair.GetMomPart1();
//             Double_t mom2 = pair.GetMomPart2();
//             Double_t m1m2 = mom1 / TMath::Abs(mom2);
//             if (m1m2 < 4) continue;

//             Double_t path = pair.GetPath();
//             Double_t dca0 = pair.GetDCA0();
//             //Double_t dca12 = pair.GetDCA12();
//             //Double_t dca1 = pair.GetDCA1();
//             //Double_t dca2 = pair.GetDCA2();
//             Double_t dca1/*dca1norm*/ = pair.GetDCA1() / pair.GetdDCA1();
//             Double_t dca2/*dca2norm*/ = pair.GetDCA2() / pair.GetdDCA2();
//             Double_t dca12/*dca12norm*/ = pair.GetDCA12() / pair.GetdDCA12();
//             Double_t omega = dca1 * dca2 / dca12 / dca12;
//             //Double_t chi1 = pair.GetChi2Part1();
//             //Double_t chi2 = pair.GetChi2Part2();
//             //Double_t v0x = pair.GetV0X();
//             //Double_t v0y = pair.GetV0Y();
//             //Double_t v0z = pair.GetV0Z();
//             Double_t dedx = pair.GetAlpha();

//             for (Int_t iOmega = 0; iOmega < nOmega; iOmega++) {
//                 if (omega < omegaMin[iOmega] || omega > omegaMax[iOmega]) continue;
//                 for (Int_t iPath = 0; iPath < nPath; iPath++) {
//                     if (path < pathMin[iPath] || path > pathMax[iPath]) continue;
//                     for (Int_t iDedx = 0; iDedx < nDedx; iDedx++) {
//                         if (dedx < dedxMin[iDedx] || dedx > dedxMax[iDedx]) continue;
//                         for (Int_t iMTOF = 0; iMTOF < nMTOF; iMTOF++) {
//                             if (massTof < mTOFMin[iMTOF] || massTof > mTOFMax[iMTOF]) continue;
//                             for (Int_t iMom1 = 0; iMom1 < nMom1; iMom1++) {
//                                 if (mom1 < mom1Min[iMom1] || mom1 > mom1Max[iMom1]) continue;
//                                 for (Int_t iMom2 = 0; iMom2 < nMom2; iMom2++) {
//                                     if (mom2 < mom2Min[iMom2] || mom2 > mom2Max[iMom2]) continue;
//                                     for (Int_t iDca0 = 0; iDca0 < nDca0; iDca0++) {
//                                         if (dca0 < dca0Min[iDca0] || dca0 > dca0Max[iDca0]) continue;
//                                         hInvMass[iOmega][iPath][iDedx][iMTOF][iMom1][iMom2][iDca0]->Fill(mass);
//                                     }
//                                 }
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     myFile->Close();

//     TFile outFile(outputFileName, "RECREATE");
//     for (Int_t iOmega = 0; iOmega < nOmega; iOmega++) {
//         for (Int_t iPath = 0; iPath < nPath; iPath++) {
//             for (Int_t iDedx = 0; iDedx < nDedx; iDedx++) {
//                 for (Int_t iMTOF = 0; iMTOF < nMTOF; iMTOF++) {
//                     for (Int_t iMom1 = 0; iMom1 < nMom1; iMom1++) {
//                         for (Int_t iMom2 = 0; iMom2 < nMom2; iMom2++) {
//                             for (Int_t iDca0 = 0; iDca0 < nDca0; iDca0++) {
//                                 hInvMass[iOmega][iPath][iDedx][iMTOF][iMom1][iMom2][iDca0]->Write();
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }
//     outFile.Close();
// }
