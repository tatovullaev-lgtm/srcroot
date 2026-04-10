/** BmnGlobalTracking.cxx
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * @since 2014
 * @version 1.0
 **/

#include "BmnGlobalTracking.h"

#include "BmnEventHeader.h"
#include "BmnFieldMap.h"
#include "BmnGemStripHit.h"
#include "TFile.h"
#include "TVectorD.h"
#include "omp.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>

using namespace TMath;

BmnGlobalTracking::BmnGlobalTracking()
    : fInnerTracks(nullptr)
    , fGemTracks(nullptr)
    , fGemVertex(nullptr)
    , fGemHits(nullptr)
    , fSilHits(nullptr)
    , fCscHits(nullptr)
    , fDchHits(nullptr)
    , fTof1Hits(nullptr)
    , fTof2Hits(nullptr)
    , fGemLowerCluster(nullptr)
    , fEvHead(nullptr)
    , fMCTracks(nullptr)
    , fStsHits(nullptr)
    , fStsTracks(nullptr)
    , fGlobalTracks(nullptr)
    , fScWall(nullptr)
    , fInnerTrackBranchName("StsTrack")
    , fDoAlign(kFALSE)
    , fEventNo(0)
    , fPDG(2212)
    , fChiSqCut(100.)
    , fVertex(nullptr)
    , fVertexL1(nullptr)
    , fKalman(nullptr)
    , fTime(0.0)
    , fTOF400_file(0)
    , fTOF700_file(0)
    , fCalib1_file(0)
    , fCalib2_file(0)
    , fNMatchedDch1(0)
    , fNMatchedDch2(0)
    , fNMatchedTof400(0)
    , fNMatchedTof700(0)
    , fNMatchedNearCsc(0)
    , fNMatchedFarCsc(0)
    , fNInnerTracks(0)
    , fNGoodInnerTracks(0)
    , fNGoodGlobalTracks(0)
{}

BmnGlobalTracking::BmnGlobalTracking(Bool_t isExp, Bool_t doAlign)
    : fInnerTracks(nullptr)
    , fGemTracks(nullptr)
    , fGemVertex(nullptr)
    , fGemHits(nullptr)
    , fCscHits(nullptr)
    , fDchHits(nullptr)
    , fTof1Hits(nullptr)
    , fTof2Hits(nullptr)
    , fGemLowerCluster(nullptr)
    , fEvHead(nullptr)
    , fMCTracks(nullptr)
    , fStsHits(nullptr)
    , fStsTracks(nullptr)
    , fGlobalTracks(nullptr)
    , fScWall(nullptr)
    , fInnerTrackBranchName("StsTrack")
    , fPeriod(8)
    , fIsExp(isExp)
    , fDoAlign(doAlign)
    , fEventNo(0)
    , fPDG(2212)
    , fChiSqCut(100.)
    , fVertex(nullptr)
    , fVertexL1(nullptr)
    , fTime(0.0)
    , fTOF400_file(0)
    , fTOF700_file(0)
    , fCalib1_file(0)
    , fCalib2_file(0)
    , fNMatchedDch1(0)
    , fNMatchedDch2(0)
    , fNMatchedTof400(0)
    , fNMatchedTof700(0)
    , fNMatchedNearCsc(0)
    , fNMatchedFarCsc(0)
    , fNInnerTracks(0)
    , fNGoodInnerTracks(0)
    , fNGoodGlobalTracks(0)
{}

BmnGlobalTracking::~BmnGlobalTracking()
{
    if (fGlobalTracks) {
        fGlobalTracks->Delete();
        delete fGlobalTracks;
    }
    if (fTOF400_file)
        delete fTOF400_file;
    if (fTOF700_file)
        delete fTOF700_file;
    if (fCalib1_file)
        delete fCalib1_file;
    if (fCalib2_file)
        delete fCalib2_file;
}

InitStatus BmnGlobalTracking::Init()
{
    if (fVerbose > 1)
        cout << "BmnGlobalTracking::Init started\n";

    fKalman = new BmnKalmanFilter();

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    fSilHits = (TClonesArray*)ioman->GetObject("BmnSiliconHit");
    fGemHits = (TClonesArray*)ioman->GetObject("BmnGemStripHit");
    fCscHits = (TClonesArray*)ioman->GetObject("BmnCSCHit");
    fTof1Hits = (TClonesArray*)ioman->GetObject("BmnTof400Hit");
    fTof2Hits = (TClonesArray*)ioman->GetObject("BmnTof700Hit");

    fVertexL1 = (CbmVertex*)ioman->GetObject("MpdVertex.");
    fScWall = (BmnScWallEvent*)ioman->GetObject("ScWallEvent");

    fStsTracks = (TClonesArray*)ioman->GetObject(fInnerTrackBranchName);
    fStsHits = (TClonesArray*)ioman->GetObject("StsHit");
    fGemLowerCluster = (TClonesArray*)ioman->GetObject("BmnGemLowerCluster");
    if (fStsTracks) {
        printf("BmnGlobalTracking::Init(): branch %s was found!\n", fInnerTrackBranchName.Data());
        fGlobalTracks = new TClonesArray("BmnGlobalTrack", 100);   // out
        ioman->Register("BmnGlobalTrack", "GLOBAL", fGlobalTracks, kTRUE);
    } else {
        fInnerTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
        fGemTracks = (TClonesArray*)ioman->GetObject("BmnGemTrack");
        if (!fInnerTracks) {
            cout << "BmnGlobalTracking::Init(): branch BmnInnerTrack not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        } else {
            printf("BmnGlobalTracking::Init(): branch BmnInnerTrack was found! Global tracks will be based on BMN "
                   "tracks \n");
        }
    }

    fDchTracks = (TClonesArray*)ioman->GetObject("BmnDchTrack");
    // if (!fDchTracks)
    // cout << "BmnGlobalTracking::Init(): branch BmnDchTrack not found!" << endl;

    if (fIsExp) {                                     // In case of exp data we create artificial hits
        fDchHits = new TClonesArray("BmnHit", 100);   // out
        // ioman->Register("BmnDchHit", "DCH", fDchHits, kTRUE);
    } else {   // In case of MC data we get hits from indput tree
        fDchHits = (TClonesArray*)ioman->GetObject("BmnDchHit");
    }

    TString dir = getenv("VMCWORKDIR");
    // TString path = dir + "/input/tof700_strip_time_shifts.txt";
    // ifstream infile(path.Data());

    // Int_t m, s, n;
    // Float_t sh;
    // while (infile >> m >> s >> n >> sh) {
    //     timeShiftsTof700[m][s] = sh;
    // }

    // coefficients from the first iteration
    TString calibFileName1 = dir + "/input/q_calib1.root";
    fCalib1_file = TFile::Open(calibFileName1);
    for (Int_t i = 0; i < nGemSt; ++i) {
        TH1F* hA0 = (TH1F*)fCalib1_file->Get(Form("hA0_run_%d", i));
        TH1F* hA1 = (TH1F*)fCalib1_file->Get(Form("hA1_run_%d", i));
        TH1F* hB0 = (TH1F*)fCalib1_file->Get(Form("hB0_run_%d", i));
        TH1F* hB1 = (TH1F*)fCalib1_file->Get(Form("hB1_run_%d", i));

        qCalib_a0[i] = hA0->GetBinContent(hA0->FindBin(fRunId));
        qCalib_a1[i] = hA1->GetBinContent(hA1->FindBin(fRunId));
        qCalib_b0[i] = hB0->GetBinContent(hB0->FindBin(fRunId));
        qCalib_b1[i] = hB1->GetBinContent(hB1->FindBin(fRunId));

        // cout << a0[i] << " " << a1[i] << " " << b0[i] << " " << b1[i] << endl;
    }
    fCalib1_file->Close();

    // coefficients from the second iteration
    TString calibFileName2 = dir + "/input/q_calib2.root";
    fCalib2_file = TFile::Open(calibFileName2);
    for (Int_t i = 0; i < nGemSt; ++i) {
        TH1F* hA0 = (TH1F*)fCalib2_file->Get(Form("hA0_run_%d", i));
        TH1F* hA1 = (TH1F*)fCalib2_file->Get(Form("hA1_run_%d", i));
        TH1F* hB0 = (TH1F*)fCalib2_file->Get(Form("hB0_run_%d", i));
        TH1F* hB1 = (TH1F*)fCalib2_file->Get(Form("hB1_run_%d", i));

        qCalib_a0[i] *= hA0->GetBinContent(hA0->FindBin(fRunId));
        qCalib_a1[i] *= hA1->GetBinContent(hA1->FindBin(fRunId));
        qCalib_b0[i] =
            qCalib_b0[i] * hA0->GetBinContent(hA0->FindBin(fRunId)) + hB0->GetBinContent(hB0->FindBin(fRunId));
        qCalib_b1[i] =
            qCalib_b1[i] * hA1->GetBinContent(hA1->FindBin(fRunId)) + hB1->GetBinContent(hB1->FindBin(fRunId));

        // cout << a0[i] << " " << a1[i] << " " << b0[i] << " " << b1[i] << endl;
    }
    fCalib2_file->Close();

    TString ResXY_vs_mom_TOF400 = dir + "/input/fitFunctions_ResXY_vs_mom_TOF400.root";
    fTOF400_file = TFile::Open(ResXY_vs_mom_TOF400);
    fitMeanPosResX_tof400 = (TF1*)fTOF400_file->Get("fitMeanPosResX");
    fitMeanPosResY_tof400 = (TF1*)fTOF400_file->Get("fitMeanPosResY");
    fitMeanNegResX_tof400 = (TF1*)fTOF400_file->Get("fitMeanNegResX");
    fitMeanNegResY_tof400 = (TF1*)fTOF400_file->Get("fitMeanNegResY");
    fitSigmaPosResX_tof400 = (TF1*)fTOF400_file->Get("fitSigmaPosResX");
    fitSigmaPosResY_tof400 = (TF1*)fTOF400_file->Get("fitSigmaPosResY");
    fitSigmaNegResX_tof400 = (TF1*)fTOF400_file->Get("fitSigmaNegResX");
    fitSigmaNegResY_tof400 = (TF1*)fTOF400_file->Get("fitSigmaNegResY");
    fTOF400_file->Close();

    TString ResXY_vs_mom_TOF700 = dir + "/input/fitFunctions_ResXY_vs_mom_TOF700.root";
    fTOF700_file = TFile::Open(ResXY_vs_mom_TOF700);
    fitMeanPosResX_tof700 = (TF1*)fTOF700_file->Get("fitMeanPosResX");
    fitMeanPosResY_tof700 = (TF1*)fTOF700_file->Get("fitMeanPosResY");
    fitMeanNegResX_tof700 = (TF1*)fTOF700_file->Get("fitMeanNegResX");
    fitMeanNegResY_tof700 = (TF1*)fTOF700_file->Get("fitMeanNegResY");
    fitSigmaPosResX_tof700 = (TF1*)fTOF700_file->Get("fitSigmaPosResX");
    fitSigmaPosResY_tof700 = (TF1*)fTOF700_file->Get("fitSigmaPosResY");
    fitSigmaNegResX_tof700 = (TF1*)fTOF700_file->Get("fitSigmaNegResX");
    fitSigmaNegResY_tof700 = (TF1*)fTOF700_file->Get("fitSigmaNegResY");
    fTOF700_file->Close();

    fEvHead = (TClonesArray*)ioman->GetObject("EventHeader");
    if (!fEvHead)
        if (fVerbose > 1)
            cout << "Init. No EventHeader array!" << endl;

    if (fVerbose > 1)
        cout << "BmnGlobalTracking::Init finished\n";
    return kSUCCESS;
}

void BmnGlobalTracking::Exec(Option_t* opt)
{
    TStopwatch sw;
    sw.Start();

    fEventNo++;
    if (fStsTracks)
        fGlobalTracks->Delete();

    if (!IsActive())
        return;

    if (fVerbose > 1)
        cout << "\n======================== Global tracking exec started =====================\n" << endl;

    if (!fVertexL1)
        return;
    if (fVertexL1->GetNTracks() < 2)
        return;

    // if (!fInnerTracks) return;
    if (fIsExp) {
        if (fDchHits)
            fDchHits->Delete();
    }

    // Alignment. FIXME: move to DB

    if (fIsExp) {
        if (fPeriod == 7) {
            if (fDchTracks) {
                Double_t dchTxCorr = +0.006;
                Double_t dchTyCorr = -0.0003;
                Double_t dchXCorr = -6.97;
                Double_t dchYCorr = -2.92;
                for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
                    BmnTrack* dchTr = (BmnTrack*)fDchTracks->At(trIdx);
                    FairTrackParam* parDch = dchTr->GetParamFirst();
                    Double_t zDCH = parDch->GetZ();
                    if (zDCH < 550) {          // dch1
                    } else if (zDCH > 650) {   // dch2
                    } else {                   // global dch
                        parDch->SetTx(parDch->GetTx() + dchTxCorr);
                        parDch->SetTy(parDch->GetTy() + dchTyCorr);
                        parDch->SetX(parDch->GetX() + dchXCorr);
                        parDch->SetY(parDch->GetY() + dchYCorr);
                    }
                }
            }
            if (fCscHits) {
                Double_t cscXCorr = +0.87;
                Double_t cscYCorr = -0.12;
                for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
                    BmnHit* hit = (BmnHit*)fCscHits->At(hitIdx);
                    hit->SetX(hit->GetX() + cscXCorr);
                    hit->SetY(hit->GetY() + cscYCorr);
                }
            }
            if (fTof1Hits) {
                Double_t tof400XCorr = -2.03;
                Double_t tof400YCorr = +0.60;
                for (Int_t hitIdx = 0; hitIdx < fTof1Hits->GetEntriesFast(); ++hitIdx) {
                    BmnHit* hit = (BmnHit*)fTof1Hits->At(hitIdx);
                    hit->SetX(hit->GetX() + tof400XCorr);
                    hit->SetY(hit->GetY() + tof400YCorr);
                }
            }
            if (fTof2Hits) {
                Double_t tof700XCorr = +2.00;
                Double_t tof700YCorr = -5.74;
                for (Int_t hitIdx = 0; hitIdx < fTof2Hits->GetEntriesFast(); ++hitIdx) {
                    BmnHit* hit = (BmnHit*)fTof2Hits->At(hitIdx);
                    hit->SetX(hit->GetX() + tof700XCorr);
                    hit->SetY(hit->GetY() + tof700YCorr);
                }
            }
        }
    }

    if (fStsTracks) {   // for run-8
        for (Int_t i = 0; i < fStsTracks->GetEntriesFast(); ++i) {
            CbmStsTrack* cbmTrack = (CbmStsTrack*)fStsTracks->At(i);

            fNInnerTracks++;

            if (cbmTrack->GetNStsHits() < 4)
                continue;

            BmnGlobalTrack globTr;

            globTr.SetGemTrackIndex(i);
            globTr.SetParamFirst(*(cbmTrack->GetParamFirst()));
            globTr.SetParamLast(*(cbmTrack->GetParamLast()));
            globTr.SetNHits(cbmTrack->GetNStsHits());
            globTr.SetNDF(cbmTrack->GetNDF());
            globTr.SetChi2(cbmTrack->GetChi2());
            FairTrackParam par(*(globTr.GetParamLast()));
            fPDG = (globTr.GetP() > 0.) ? 2212 : -211;
            Double_t len = 0.0;
            const Float_t tZ = -0.01942;   // z-coordinate of target for RUN-8
            if (fKalman->TGeoTrackPropagate(&par, tZ, fPDG, nullptr, &len) == kBMNERROR)
                continue;
            globTr.SetLength(len);

            FairTrackParam parF(*(globTr.GetParamFirst()));
            if (fKalman->TGeoTrackPropagate(&parF, fVertexL1->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
                continue;
            Float_t dca = Sqrt(Sq(fVertexL1->GetX() - parF.GetX()) + Sq(fVertexL1->GetY() - parF.GetY()));
            globTr.SetParamFirst(parF);
            globTr.SetDCAInVertex(dca);

            BmnHit* virtualVertexHit = new BmnHit();
            virtualVertexHit->SetXYZ(fVertexL1->GetX(), fVertexL1->GetY(), fVertexL1->GetZ());
            virtualVertexHit->SetDxyz(Sqrt(fVertexL1->GetCovariance(0, 0)), Sqrt(fVertexL1->GetCovariance(1, 1)),
                                      Sqrt(fVertexL1->GetCovariance(2, 2)));

            Double_t chi = 0;
            fKalman->Update(&parF, virtualVertexHit, chi);
            globTr.SetChi2InVertex(chi);

            CalcdQdn(&globTr);

            fNGoodInnerTracks++;
            new ((*fGlobalTracks)[fGlobalTracks->GetEntriesFast()]) BmnGlobalTrack(globTr);
        }

        MatchingTOF400();
        MatchingTOF700();
        MatchingScWall();
        MatchingFarCsc();

        //         for (Int_t i = 0; i < fStsTracks->GetEntriesFast(); ++i) {
        //             CbmStsTrack* cbmTrack = (CbmStsTrack*)fStsTracks->At(i);

        //             fNInnerTracks++;

        //             if (cbmTrack->GetNStsHits() < 4) continue;

        //             BmnGlobalTrack globTr;

        //             globTr.SetGemTrackIndex(i);
        //             globTr.SetParamFirst(*(cbmTrack->GetParamFirst()));
        //             globTr.SetParamLast(*(cbmTrack->GetParamLast()));
        //             globTr.SetNHits(cbmTrack->GetNStsHits());
        //             globTr.SetNDF(cbmTrack->GetNDF());
        //             globTr.SetChi2(cbmTrack->GetChi2());
        //             FairTrackParam par(*(globTr.GetParamLast()));
        //             fPDG = (globTr.GetP() > 0.) ? 2212 : -211;
        //             Double_t len = 0.0;
        //             Double_t zTarget = (fVertexL1) ? fVertexL1->GetZ() : 0.0;
        //             if (fKalman->TGeoTrackPropagate(&par, zTarget, fPDG, nullptr, &len) == kBMNERROR) continue;
        //             globTr.SetLength(len);

        //             fNGoodInnerTracks++;

        //             //Matching with Small CSC1
        //             //vector<Int_t> nearCSCst = { 0, 1 };
        // //            MatchingCSC(&globTr);
        //             //Matching with TOF-400
        // //            MatchingTOF(&globTr, 1);
        //             //Matching with Small CSC2
        //             //vector<Int_t> farCSCst = { 2, 3 };
        //             //MatchingCSC(&globTr, farCSCst);
        //             //Matching with DCH1
        // //            MatchingDCH(&globTr, 1);
        //             //Matching with TOF-700
        //             MatchingTOF(&globTr, 2);
        //             //Matching with DCH2
        // //            MatchingDCH(&globTr, 2);

        //             if (Refit(&globTr) == kBMNERROR) continue;

        //             fNGoodGlobalTracks++;
        //             new ((*fGlobalTracks)[fGlobalTracks->GetEntriesFast()]) BmnGlobalTrack(globTr);
        //         }
    } else if (fInnerTracks) {
        // for (Int_t i = 0; i < fInnerTracks->GetEntriesFast(); ++i) {
        //     BmnGlobalTrack* glTrack = (BmnGlobalTrack*)fInnerTracks->At(i);

        //     //Downstream
        //     //Matching with Small CSC1
        //     //vector<Int_t> nearCSCst = { 0, 1 };
        //     if (!fIsSRC) MatchingCSC(glTrack);
        //     if (!fIsSRC) MatchingTOF(glTrack, 1);
        //     MatchingDCH(glTrack);
        //     MatchingTOF(glTrack, 2);

        //     //Upstream
        //     if (fIsSRC) MatchingUpstream(glTrack);

        //     Refit(glTrack);
        //     //cout << glTrack->GetP() << endl;
        //     if (fIsSRC) UpdateMomentum(glTrack);
        //     CalcdQdn(glTrack);

        //     // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters
        //     of Circle in ZX plane
        //     // Check it!!!
        //     glTrack->SetNDF(glTrack->GetNHits() * 2 - 5);
        // }
    }

    sw.Stop();
    fTime += sw.RealTime();

    // if (fVerbose) cout << "GLOBAL_TRACKING: Number of merged tracks: " << fGlobalTracks->GetEntriesFast() << endl;
    if (fVerbose > 1)
        cout << "\n======================== Global tracking exec finished ====================\n" << endl;
}

BmnStatus BmnGlobalTracking::MatchingTOF400()
{
    if (fVerbose)
        cout << "Matching of TOF-400 started" << endl;
    TClonesArray* tofHits = fTof1Hits;
    if (!tofHits)
        return kBMNERROR;

    // residuals after peak fitting of all-to-all histograms
    // Double_t sigmaXtof1gemResid = (fIsExp) ? 2.0 : 2.0;   // 6.2;                                 //1.34;
    // Double_t sigmaYtof1gemResid = (fIsExp) ? 2.0 : 2.0;   // 1.38;
    // Double_t xCut = 3 * sigmaXtof1gemResid;
    // Double_t yCut = 3 * sigmaYtof1gemResid;

    map<Float_t, pair<Int_t, Int_t>> distPairs = FindPairsTof400(fGlobalTracks, tofHits, -1, -1);

    for (auto const& distPair : distPairs) {
        // Float_t dist = distPair.first;
        pair<Int_t, Int_t> idxes = distPair.second;
        BmnGlobalTrack* track = (BmnGlobalTrack*)fGlobalTracks->At(idxes.first);
        if (track->GetTof1HitIndex() != -1)
            continue;
        BmnHit* hit = (BmnHit*)tofHits->At(idxes.second);
        if (hit->IsUsed())
            continue;

        Double_t len = 0.0;
        FairTrackParam par(*(track->GetParamLast()));
        fKalman->TGeoTrackPropagate(&par, hit->GetZ(), fPDG, nullptr, &len);
        Float_t mom = 1.0 / par.GetQp();
        Float_t xMomCorr = (mom > 0) ? fitMeanPosResX_tof400->Eval(mom) : fitMeanNegResX_tof400->Eval(mom);
        Float_t yMomCorr = (mom > 0) ? fitMeanPosResY_tof400->Eval(mom) : fitMeanNegResY_tof400->Eval(mom);
        hit->SetResXY(par.GetX() - xMomCorr - hit->GetX(), par.GetY() - yMomCorr - hit->GetY());
        track->SetParamLast(par);
        track->SetTof1HitIndex(idxes.second);
        len += track->GetLength();
        track->SetBeta(len / hit->GetTimeStamp() / (TMath::C() * 1e-7), 1);
        hit->SetUsing(kTRUE);
        hit->SetLength(len);   // length from target to Tof-400 hit
        hit->SetModule(((hit->GetDetectorID() & 0x0000FF00) >> 8) - 1);
        track->SetNHits(track->GetNHits() + 1);
        track->SetLength(len);
        fNMatchedTof400++;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingTOF700()
{
    if (fVerbose)
        cout << "Matching of TOF-700 started" << endl;
    TClonesArray* tofHits = fTof2Hits;
    if (!tofHits)
        return kBMNERROR;

    map<Float_t, pair<Int_t, Int_t>> distPairs = FindPairsTof700(fGlobalTracks, tofHits, -1, -1);

    for (auto const& distPair : distPairs) {
        // Float_t dist = distPair.first;
        pair<Int_t, Int_t> idxes = distPair.second;
        BmnGlobalTrack* track = (BmnGlobalTrack*)fGlobalTracks->At(idxes.first);
        if (track->GetTof2HitIndex() != -1)
            continue;
        BmnHit* hit = (BmnHit*)tofHits->At(idxes.second);
        if (hit->IsUsed())
            continue;

        Double_t len = 0.0;
        FairTrackParam par(*(track->GetParamLast()));
        fKalman->TGeoTrackPropagate(&par, hit->GetZ(), fPDG, nullptr, &len);
        Float_t mom = 1.0 / par.GetQp();
        Float_t xMomCorr = (mom > 0) ? fitMeanPosResX_tof700->Eval(mom) : fitMeanNegResX_tof700->Eval(mom);
        Float_t yMomCorr = (mom > 0) ? fitMeanPosResY_tof700->Eval(mom) : fitMeanNegResY_tof700->Eval(mom);
        hit->SetResXY(par.GetX() - xMomCorr - hit->GetX(), par.GetY() - yMomCorr - hit->GetY());
        track->SetParamLast(par);
        track->SetTof2HitIndex(idxes.second);
        len += track->GetLength();
        track->SetBeta(len / (hit->GetTimeStamp()) / (TMath::C() * 1e-7), 2);
        hit->SetUsing(kTRUE);
        hit->SetLength(len);   // length from target to Tof-700 hit
        hit->SetModule(((hit->GetDetectorID() & 0x0000FF00) >> 8) - 1);
        track->SetNHits(track->GetNHits() + 1);
        track->SetLength(len);
        fNMatchedTof700++;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingScWall()
{
    if (fVerbose)
        cout << "Matching of ScWall started " << fScWall << endl;
    if (!fScWall)
        return kBMNERROR;

    for (Int_t trIdx = 0; trIdx < fGlobalTracks->GetEntriesFast(); ++trIdx) {
        BmnGlobalTrack* tr = (BmnGlobalTrack*)fGlobalTracks->At(trIdx);
        FairTrackParam par(*(tr->GetParamLast()));
        Double_t minDX = DBL_MAX;
        Double_t minDY = DBL_MAX;
        Float_t xCut = 20.0;   // cm
        Float_t yCut = 20.0;   // cm
        Int_t minIdx = -1;

        Float_t z = 741.5;
        Int_t nCells = 174;   // BmnScWallEvent::fgkMaxCells; //174
        if (fKalman->TGeoTrackPropagate(&par, z, fPDG, nullptr, nullptr) == kBMNERROR)
            return kBMNERROR;

        for (Int_t i = 1; i <= nCells; ++i) {
            BmnScWallCell* cell = fScWall->GetCell(i);
            if (cell->GetSignal() < 0.001)
                continue;   // skip empty cells

            Float_t dX = par.GetX() - cell->GetX();
            Float_t dY = par.GetY() - cell->GetY();

            if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < Abs(minDX) && Abs(dY) < Abs(minDY)) {
                minDX = dX;
                minDY = dY;
                minIdx = i;
            }
        }
        // cout << minDX << "   " << minDY << endl;

        if (minIdx == -1)
            continue;
        tr->SetScWallCellId(minIdx);
        tr->SetScWallSignal(fScWall->GetCell(minIdx)->GetSignal());
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingFarCsc()
{
    if (fVerbose)
        cout << "Matching of LCSC started" << endl;
    if (!fCscHits)
        return kBMNERROR;

    Double_t dX = 0;
    Double_t dY = 0;
    Double_t dZ = fDoAlign ? -7 : 0;
    const Double_t dXX[] = {0.944, 0.891, 0.885, 0.992, 0.559, 0.582, 0.606, 0.588};
    const Double_t dYY[] = {-0.735, -0.653, -0.583, -1.219, -0.458, -0.376, -0.506, -0.128};

    // residuals after peak fitting of all-to-all histograms
    Double_t sigmaXResid = (fIsExp) ? 2.0 : 2.0;
    Double_t sigmaYResid = (fIsExp) ? 2.0 : 2.0;
    Double_t xCut = 3 * sigmaXResid;
    Double_t yCut = 3 * sigmaYResid;

    Int_t N_tr = fGlobalTracks->GetEntriesFast();
    Int_t N_csc = fCscHits->GetEntriesFast();

    TString VMCWORKDIR = gSystem->Getenv("VMCWORKDIR");
    BmnCSCStationSet stationSet(VMCWORKDIR + "/parameters/csc/XMLConfigs/" + "FullCSCRun8.xml");
    BmnCSCStation* pStation = stationSet.GetStation(4);

    Double_t minZ = 1e5;
    for (Int_t i_csc = 0; i_csc < N_csc; i_csc++) {
        BmnCSCHit* pCscHit = (BmnCSCHit*)fCscHits->At(i_csc);
        if (pCscHit->GetStation() != 4)
            continue;
        if (minZ > pCscHit->GetZ())
            minZ = pCscHit->GetZ();
    }

    for (Int_t i_tr = 0; i_tr < N_tr; i_tr++) {
        BmnGlobalTrack* track = (BmnGlobalTrack*)fGlobalTracks->At(i_tr);
        FairTrackParam parMinZ(*(track->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&parMinZ, minZ, fPDG, nullptr, nullptr) == kBMNERROR)
            continue;
        for (Int_t i_csc = 0; i_csc < N_csc; i_csc++) {
            BmnCSCHit* pCscHit = (BmnCSCHit*)fCscHits->At(i_csc);
            Int_t st_id = pCscHit->GetStation();
            if (st_id != 4)
                continue;
            if (pCscHit->IsUsed())
                continue;
            if (track->GetCscHitIndex(st_id - 1) != -1)
                continue;
            FairTrackParam p = parMinZ;
            Double_t len = 0.0;
            if (fKalman->TGeoTrackPropagate(&p, pCscHit->GetZ() + dZ, fPDG, nullptr, &len) == kBMNERROR)
                continue;
            Int_t mod_id = pCscHit->GetModule();
            if (fDoAlign) {
                dX = dXX[mod_id];
                dY = dYY[mod_id];
            }
            if (pStation->GetPointModuleOwnership(-1.0 * (p.GetX() - dX), p.GetY() - dY, p.GetZ() - dZ) != mod_id)
                continue;   // check if track is in current CSC
            Double_t dx = p.GetX() - (pCscHit->GetX() + dX);
            Double_t dy = p.GetY() - (pCscHit->GetY() + dY);
            if (Abs(dx) < xCut && Abs(dy) < yCut) {
                // Double_t chi = 0;
                pCscHit->SetResXY(dx, dy);
                // fKalman->Update(&p, pCscHit, chi);
                // track->SetChi2(track->GetChi2() + chi);
                // track->SetParamLast(p);
                // track->AddCscHitIndex(st_id - 1,i_csc);
                // len += track->GetLength();
                ////track->SetBeta(len / hit->GetTimeStamp() / (TMath::C() * 1e-7), 2);
                pCscHit->SetUsing(kTRUE);
                pCscHit->SetLength(len);   // length from target to Tof-700 hit
                // track->SetNHits(track->GetNHits() + 1);
                // track->SetLength(len);
                fNMatchedFarCsc++;
            }
        }
    }

    return kBMNSUCCESS;
}

void BmnGlobalTracking::CalcdQdn(BmnGlobalTrack* gl)
{
    // BmnGemTrack* gemTrack = (BmnGemTrack*)fGemTracks->At(tr->GetGemTrackIndex());
    // Double_t totSigLow = 0.0;
    // Double_t totSigUp = 0.0;
    // for (Int_t hitIdx = 0; hitIdx < gemTrack->GetNHits(); ++hitIdx) {
    //     BmnGemStripHit* hit = (BmnGemStripHit*)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
    //     totSigLow += hit->GetStripTotalSignalInLowerLayer();
    //     totSigUp += hit->GetStripTotalSignalInUpperLayer();
    // }
    // totSigLow /= gemTrack->GetNHits();
    // totSigUp /= gemTrack->GetNHits();
    // tr->SetdQdNLower(totSigLow);
    // tr->SetdQdNUpper(totSigUp);

    Float_t mom = gl->GetP();
    if (mom < 0.0)
        return;

    CbmStsTrack* tr = (CbmStsTrack*)fStsTracks->At(gl->GetGemTrackIndex());

    set<Float_t> signalsLow;
    Int_t nHits = tr->GetNStsHits();
    Int_t nGemHits = 0;
    for (Int_t hitIdx = 0; hitIdx < nHits; ++hitIdx) {
        CbmStsHit* hit = (CbmStsHit*)fStsHits->At(tr->GetStsHitIndex(hitIdx));
        if (hit->GetSystemId() != kGEM)
            continue;
        nGemHits++;
        StripCluster* low = (StripCluster*)fGemLowerCluster->At(hit->fDigiF % 1000000);
        Int_t idx =
            hit->GetStationNr() - 1 - 4;   //(hit.GetStationNr() - 1) - station number from 0, -4 for excluding FSD
        Float_t a = (hit->GetY() > 0) ? qCalib_a0[idx] : qCalib_a1[idx];
        Float_t b = (hit->GetY() > 0) ? qCalib_b0[idx] : qCalib_b1[idx];
        signalsLow.insert(low->TotalSignal * a + b);
    }

    Int_t usedHits = (nGemHits == 3)   ? 2
                     : (nGemHits == 4) ? 2
                     : (nGemHits == 5) ? 3
                     : (nGemHits == 6) ? 4
                     : (nGemHits == 7) ? 4
                                       : 0;
    auto it = signalsLow.begin();
    Float_t totSigLow = 0.0;
    for (Int_t i = 0; i < usedHits; ++i) {
        totSigLow += (*it);
        it++;
    }
    Float_t dedx = (usedHits > 0) ? totSigLow / usedHits : -100;
    gl->SetdQdNLower(dedx);

    TF1* hypCut = new TF1("hypCut", "20000*TMath::Exp(-2.0*TMath::Sqrt(x)) + 600.0", 0, 15);

    if (dedx > hypCut->Eval(mom))
        gl->SetZ(2);
    else
        gl->SetZ(1);

    delete hypCut;
}

Int_t BmnGlobalTracking::FindNearestHit(FairTrackParam* par, TClonesArray* hits, Float_t xCut, Float_t yCut)
{
    if (!hits || !par)
        return -1;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Int_t minIdx = -1;
    BmnHit* minHit = nullptr;

    Double_t minZ = 10000.0;
    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);
        if (hit->GetZ() < minZ)
            minZ = hit->GetZ();
    }

    FairTrackParam parMinZ(*par);
    if (fKalman->TGeoTrackPropagate(&parMinZ, minZ, fPDG, nullptr, nullptr) == kBMNERROR)
        return -1;

    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);

        FairTrackParam param = parMinZ;
        if (fKalman->TGeoTrackPropagate(&param, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            continue;

        Float_t dX = param.GetX() - hit->GetX();
        Float_t dY = param.GetY() - hit->GetY();

        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < Abs(minDX) && Abs(dY) < Abs(minDY)) {
            minDX = dX;
            minDY = dY;
            minIdx = hitIdx;
            minHit = hit;
        }
    }
    if (!minHit)
        return -1;

    minHit->SetResXY(minDX, minDY);

    return minIdx;
}

map<Float_t, pair<Int_t, Int_t>> BmnGlobalTracking::FindPairsTof700(TClonesArray* tracks,
                                                                    TClonesArray* hits,
                                                                    Float_t xCut,
                                                                    Float_t yCut)
{

    Bool_t useFixCutX = (xCut > 0);
    Bool_t useFixCutY = (yCut > 0);

    map<Float_t, pair<Int_t, Int_t>> distancesPairs;

    Double_t minZ = 10000.0;
    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);
        if (hit->GetZ() < minZ)
            minZ = hit->GetZ();
    }

    for (Int_t trIdx = 0; trIdx < tracks->GetEntriesFast(); ++trIdx) {
        BmnGlobalTrack* tr = (BmnGlobalTrack*)tracks->At(trIdx);
        FairTrackParam parMinZ(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&parMinZ, minZ, fPDG, nullptr, nullptr) == kBMNERROR)
            continue;
        for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {

            BmnHit* hit = (BmnHit*)hits->At(hitIdx);
            FairTrackParam param = parMinZ;
            if (fKalman->TGeoTrackPropagate(&param, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
                continue;
            Float_t mom = 1.0 / param.GetQp();
            Float_t xMomCorr = (mom > 0) ? fitMeanPosResX_tof700->Eval(mom) : fitMeanNegResX_tof700->Eval(mom);
            Float_t yMomCorr = (mom > 0) ? fitMeanPosResY_tof700->Eval(mom) : fitMeanNegResY_tof700->Eval(mom);
            Float_t xSigma = (mom > 0) ? fitSigmaPosResX_tof700->Eval(mom) : fitSigmaNegResX_tof700->Eval(mom);
            Float_t ySigma = (mom > 0) ? fitSigmaPosResY_tof700->Eval(mom) : fitSigmaNegResY_tof700->Eval(mom);

            Float_t dX = Abs(param.GetX() - xMomCorr - hit->GetX());
            Float_t dY = Abs(param.GetY() - yMomCorr - hit->GetY());
            if (!useFixCutX)
                xCut = 3.0 * xSigma;
            if (!useFixCutY)
                yCut = 3.0 * ySigma;
            if (dX < xCut && dY < yCut) {
                Float_t dist = Sqrt(dX * dX + dY * dY);
                distancesPairs.insert(pair<Float_t, pair<Int_t, Int_t>>(dist, pair<Int_t, Int_t>(trIdx, hitIdx)));
            }
        }
    }

    return distancesPairs;
}

map<Float_t, pair<Int_t, Int_t>> BmnGlobalTracking::FindPairsTof400(TClonesArray* tracks,
                                                                    TClonesArray* hits,
                                                                    Float_t xCut,
                                                                    Float_t yCut)
{

    Bool_t useFixCutX = (xCut > 0);
    Bool_t useFixCutY = (yCut > 0);

    map<Float_t, pair<Int_t, Int_t>> distancesPairs;

    Double_t minZ = 10000.0;
    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);
        if (hit->GetZ() < minZ)
            minZ = hit->GetZ();
    }

    for (Int_t trIdx = 0; trIdx < tracks->GetEntriesFast(); ++trIdx) {
        BmnGlobalTrack* tr = (BmnGlobalTrack*)tracks->At(trIdx);
        FairTrackParam parMinZ(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&parMinZ, minZ, fPDG, nullptr, nullptr) == kBMNERROR)
            continue;
        for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {

            BmnHit* hit = (BmnHit*)hits->At(hitIdx);
            FairTrackParam param = parMinZ;
            if (fKalman->TGeoTrackPropagate(&param, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
                continue;
            Float_t mom = 1.0 / param.GetQp();
            Float_t xMomCorr = (mom > 0) ? fitMeanPosResX_tof400->Eval(mom) : fitMeanNegResX_tof400->Eval(mom);
            Float_t yMomCorr = (mom > 0) ? fitMeanPosResY_tof400->Eval(mom) : fitMeanNegResY_tof400->Eval(mom);
            Float_t xSigma = (mom > 0) ? fitSigmaPosResX_tof400->Eval(mom) : fitSigmaNegResX_tof400->Eval(mom);
            Float_t ySigma = (mom > 0) ? fitSigmaPosResY_tof400->Eval(mom) : fitSigmaNegResY_tof400->Eval(mom);

            Float_t dX = Abs(param.GetX() - xMomCorr - hit->GetX());
            Float_t dY = Abs(param.GetY() - yMomCorr - hit->GetY());
            if (!useFixCutX)
                xCut = 3.0 * xSigma;
            if (!useFixCutY)
                yCut = 3.0 * ySigma;
            if (dX < xCut && dY < yCut) {
                Float_t dist = Sqrt(dX * dX + dY * dY);
                distancesPairs.insert(pair<Float_t, pair<Int_t, Int_t>>(dist, pair<Int_t, Int_t>(trIdx, hitIdx)));
            }
        }
    }

    return distancesPairs;
}

Int_t BmnGlobalTracking::FindNearestHit(FairTrackParam* par,
                                        TClonesArray* hits,
                                        Float_t xCut,
                                        Float_t yCut,
                                        vector<Int_t> stations)
{
    if (!hits || !par)
        return -1;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Int_t minIdx = -1;
    BmnHit* minHit = nullptr;

    Double_t minZ = 10000.0;
    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);
        Int_t st = hit->GetStation();
        if (st != stations[0] && st != stations[1])
            continue;
        if (hit->GetZ() < minZ)
            minZ = hit->GetZ();
    }

    FairTrackParam parMinZ(*par);
    if (fKalman->TGeoTrackPropagate(&parMinZ, minZ, fPDG, nullptr, nullptr) == kBMNERROR)
        return -1;

    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);

        Int_t st = hit->GetStation();
        if (st != stations[0] && st != stations[1])
            continue;

        FairTrackParam param = parMinZ;
        if (fKalman->TGeoTrackPropagate(&param, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            continue;

        Float_t dX = param.GetX() - hit->GetX();
        Float_t dY = param.GetY() - hit->GetY();

        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < Abs(minDX) && Abs(dY) < Abs(minDY)) {
            minDX = dX;
            minDY = dY;
            minIdx = hitIdx;
            minHit = hit;
        }
    }
    if (!minHit)
        return -1;

    minHit->SetResXY(minDX, minDY);

    return minIdx;
}

BmnStatus BmnGlobalTracking::MatchingCSC(BmnGlobalTrack* glTr)
{

    if (fVerbose)
        cout << "Matching of CSC started" << endl;
    if (!fCscHits)
        return kBMNERROR;

    fPDG = (glTr->GetP() > 0.) ? 2212 : -211;

    // residuals after peak fitting of all-to-all histograms
    Double_t sigmaXcscgemResid = 2.0;
    Double_t sigmaYcscgemResid = 2.0;
    Double_t xCut = 3 * sigmaXcscgemResid;
    Double_t yCut = 3 * sigmaYcscgemResid;

    // Double_t minDX = DBL_MAX;
    // Double_t minDY = DBL_MAX;
    // Double_t dX = DBL_MAX;
    // Double_t dY = DBL_MAX;

    Int_t minIdx = FindNearestHit(glTr->GetParamLast(), fCscHits, xCut, yCut);
    if (minIdx == -1)
        return kBMNERROR;

    BmnHit* minHit = (BmnHit*)fCscHits->At(minIdx);
    glTr->AddCscHitIndex(minHit->GetStation(), minIdx);

    Double_t len = 0.0;
    FairTrackParam par(*(glTr->GetParamLast()));
    if (fKalman->TGeoTrackPropagate(&par, minHit->GetZ(), fPDG, nullptr, &len) == kBMNERROR)
        return kBMNERROR;
    Double_t chi = 0;
    if (fKalman->Update(&par, minHit, chi) == kBMNERROR)
        return kBMNERROR;
    glTr->SetChi2(glTr->GetChi2() + chi);
    glTr->SetParamLast(par);
    len += glTr->GetLength();
    glTr->SetLength(len);
    minHit->SetUsing(kTRUE);
    glTr->SetNHits(glTr->GetNHits() + 1);
    if (minHit->GetStation() < 4)
        fNMatchedNearCsc++;
    else
        fNMatchedFarCsc++;
    return kBMNSUCCESS;
}

// BmnStatus BmnGlobalTracking::MatchingCSC(BmnGlobalTrack* glTr, vector<Int_t> stations) {

//     if (fVerbose) cout << "Matching of CSC started" << endl;
//     if (!fCscHits) return kBMNERROR;

//     fPDG = (glTr->GetP() > 0.) ? 2212 : -211;

//     //residuals after peak fitting of all-to-all histograms
//     Double_t sigmaXcscgemResid = 2.0;
//     Double_t sigmaYcscgemResid = 2.0;
//     Double_t xCut = 3 * sigmaXcscgemResid;
//     Double_t yCut = 3 * sigmaYcscgemResid;

//     Double_t minDX = DBL_MAX;
//     Double_t minDY = DBL_MAX;
//     Double_t dX = DBL_MAX;
//     Double_t dY = DBL_MAX;

//     Int_t minIdx = FindNearestHit(glTr->GetParamLast(), fCscHits, xCut, yCut, stations);
//     if (minIdx == -1) return kBMNERROR;

//     Int_t id = (stations[0] < 2) ? 0 : 1;
//     glTr->AddCscHitIndex(id, minIdx);

//     BmnHit* minHit = (BmnHit*)fCscHits->At(minIdx);

//     Double_t len = 0.0;
//     FairTrackParam par(*(glTr->GetParamLast()));
//     if (fKalman->TGeoTrackPropagate(&par, minHit->GetZ(), fPDG, nullptr, &len) == kBMNERROR) return kBMNERROR;
//     Double_t chi = 0;
//     if (fKalman->Update(&par, minHit, chi) == kBMNERROR) return kBMNERROR;
//     glTr->SetChi2(glTr->GetChi2() + chi);
//     glTr->SetParamLast(par);
//     len += glTr->GetLength();
//     glTr->SetLength(len);
//     minHit->SetUsing(kTRUE);
//     glTr->SetNHits(glTr->GetNHits() + 1);
//     if (id == 0) fNMatchedNearCsc++;
//     else fNMatchedFarCsc++;
//     return kBMNSUCCESS;
// }

BmnStatus BmnGlobalTracking::MatchingDCH(BmnGlobalTrack* tr)
{

    if (fVerbose)
        cout << "Matching of DCH started" << endl;

    if (!fDchTracks)
        return kBMNERROR;
    if (fDchTracks->GetEntriesFast() == 0)
        return kBMNERROR;
    fPDG = (tr->GetP() > 0.) ? 2212 : -211;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    BmnTrack* minTrack = nullptr;
    Int_t minTrackId = -1;

    // residuals after peak fitting of all-to-all histograms
    Double_t sigmaXdchGgemResid = (fIsExp) ? 5.54 : 1;
    Double_t sigmaYdchGgemResid = (fIsExp) ? 2.33 : 1;
    Double_t xCut = 3 * sigmaXdchGgemResid;
    Double_t yCut = 3 * sigmaYdchGgemResid;

    for (Int_t iTr = 0; iTr < fDchTracks->GetEntriesFast(); ++iTr) {
        BmnTrack* dchTr = (BmnTrack*)fDchTracks->At(iTr);
        if (!dchTr)
            continue;
        if (dchTr->GetNHits() < 10)
            continue;   // use only global DCH tracks
        FairTrackParam glPar(*(tr->GetParamLast()));
        FairTrackParam dchPar(*(dchTr->GetParamFirst()));
        if (fKalman->TGeoTrackPropagate(&glPar, dchPar.GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            continue;
        // Double_t dist = Sqrt(Sq(par.GetX() - hit->GetX()) + Sq(par.GetY() - hit->GetY()));
        Double_t dX = glPar.GetX() - dchPar.GetX();
        Double_t dY = glPar.GetY() - dchPar.GetY();
        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minTrack = dchTr;
            minTrackId = iTr;
            minDX = dX;
            minDY = dY;
        }
    }

    if (minTrack == nullptr)
        return kBMNERROR;

    FairTrackParam glPar(*(tr->GetParamLast()));
    FairTrackParam dchPar(*(minTrack->GetParamFirst()));
    Double_t len = tr->GetLength();
    fKalman->TGeoTrackPropagate(&glPar, dchPar.GetZ(), fPDG, nullptr, &len);
    Double_t chi = 0;
    UpdateTrackParam(&glPar, &dchPar, chi);
    tr->SetChi2(tr->GetChi2() + chi);

    tr->SetDchTrackIndex(minTrackId);
    tr->SetNHits(tr->GetNHits() + minTrack->GetNHits());
    tr->SetParamLast(glPar);
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingDCH(BmnGlobalTrack* tr, Int_t num)
{

    if (fVerbose)
        cout << "Matching of DCH started" << endl;

    if (!fDchTracks)
        return kBMNERROR;
    if (fDchTracks->GetEntriesFast() == 0)
        return kBMNERROR;
    fPDG = (tr->GetP() > 0.) ? 2212 : -211;

    // residuals after peak fitting of all-to-all histograms
    Double_t sigmaXdchGgemResid = (fIsExp) ? 5.54 : 1;
    Double_t sigmaYdchGgemResid = (fIsExp) ? 2.33 : 1;
    Double_t xCut = 3 * sigmaXdchGgemResid;
    Double_t yCut = 3 * sigmaYdchGgemResid;

    // dch1 ~510, dch2 ~710, dchGlob ~610
    Double_t zDchTh1 = 550.0;
    Double_t zDchTh2 = 650.0;

    Double_t minZ = 10000.0;
    for (Int_t iTr = 0; iTr < fDchTracks->GetEntriesFast(); ++iTr) {
        BmnTrack* dchTr = (BmnTrack*)fDchTracks->At(iTr);
        Double_t trZ = dchTr->GetParamFirst()->GetZ();
        Bool_t ok = kFALSE;
        if (trZ < zDchTh1 && num == 1)
            ok = kTRUE;
        if (trZ > zDchTh2 && num == 2)
            ok = kTRUE;
        if (!ok)
            continue;
        if (trZ < minZ)
            minZ = trZ;
    }

    FairTrackParam parMinZ(*(tr->GetParamLast()));
    if (fKalman->TGeoTrackPropagate(&parMinZ, minZ, fPDG, nullptr, nullptr) == kBMNERROR)
        return kBMNERROR;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    BmnTrack* minTrack = nullptr;
    Int_t minIdx = -1;

    for (Int_t iTr = 0; iTr < fDchTracks->GetEntriesFast(); ++iTr) {
        BmnTrack* dchTr = (BmnTrack*)fDchTracks->At(iTr);
        if (!dchTr)
            continue;

        Double_t trZ = dchTr->GetParamFirst()->GetZ();

        Bool_t ok = kFALSE;
        if (trZ < zDchTh1 && num == 1)
            ok = kTRUE;
        if (trZ > zDchTh2 && num == 2)
            ok = kTRUE;
        if (!ok)
            continue;

        FairTrackParam param = parMinZ;
        if (fKalman->TGeoTrackPropagate(&param, trZ, fPDG, nullptr, nullptr) == kBMNERROR)
            continue;

        Float_t dX = param.GetX() - dchTr->GetParamFirst()->GetX();
        Float_t dY = param.GetY() - dchTr->GetParamFirst()->GetY();

        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < Abs(minDX) && Abs(dY) < Abs(minDY)) {
            minTrack = dchTr;
            minIdx = iTr;
            minDX = dX;
            minDY = dY;
        }
    }
    if (minIdx == -1)
        return kBMNERROR;

    Double_t len = 0.0;
    FairTrackParam par(*(tr->GetParamLast()));
    FairTrackParam dchPar(*(minTrack->GetParamFirst()));
    if (fKalman->TGeoTrackPropagate(&par, dchPar.GetZ(), fPDG, nullptr, &len) == kBMNERROR)
        return kBMNERROR;
    Double_t chi = 0;
    UpdateTrackParam(&par, &dchPar, chi);
    tr->SetChi2(tr->GetChi2() + chi);
    tr->SetParamLast(par);
    len += tr->GetLength();
    tr->SetNHits(tr->GetNHits() + 1);
    tr->SetLength(len);

    if (num == 1) {
        tr->SetDch1TrackIndex(minIdx);
        fNMatchedDch1++;
    } else {
        tr->SetDch2TrackIndex(minIdx);
        fNMatchedDch2++;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::Refit(BmnGlobalTrack* tr)
{

    if (fVerbose)
        cout << "REFIT started" << endl;

    FairTrackParam parFirst = *(tr->GetParamFirst());
    fPDG = (parFirst.GetQp() > 0.) ? 2212 : -211;
    Double_t chi = 0.0;
    Double_t totChi2 = 0.0;

    //================Refitting to the end of the global track===========================
    // if (tr->GetSilTrackIndex() != -1) {
    //     BmnTrack* silTrack = (BmnTrack*)fSiliconTracks->At(tr->GetSilTrackIndex());
    //     for (Int_t hitIdx = 0; hitIdx < silTrack->GetNHits(); hitIdx++) {
    //         BmnSiliconHit* hit = (BmnSiliconHit*)fSilHits->At(silTrack->GetHitIndex(hitIdx));
    //         if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return
    //         kBMNERROR; if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
    //     }
    // }

    if (fStsTracks) {
        CbmStsTrack* cbmTrack = (CbmStsTrack*)fStsTracks->At(tr->GetGemTrackIndex());
        for (Int_t hitIdx = 0; hitIdx < cbmTrack->GetNStsHits(); hitIdx++) {
            CbmStsHit* hit = (CbmStsHit*)fStsHits->At(cbmTrack->GetStsHitIndex(hitIdx));
            if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
                return kBMNERROR;
            if (fKalman->Update(&parFirst, (BmnHit*)hit, chi) == kBMNERROR)
                return kBMNERROR;
        }
    } else if (fInnerTracks) {
        BmnGemTrack* gemTrack = (BmnGemTrack*)fGemTracks->At(tr->GetGemTrackIndex());
        for (Int_t hitIdx = 0; hitIdx < gemTrack->GetNHits(); hitIdx++) {
            BmnGemStripHit* hit = (BmnGemStripHit*)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
            if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
                return kBMNERROR;
            if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR)
                return kBMNERROR;
        }
    }

    if (tr->GetCscHitIndex(0) != -1) {
        BmnHit* hit = (BmnHit*)fCscHits->At(tr->GetCscHitIndex(0));
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR)
            return kBMNERROR;
        totChi2 += chi;
    }

    if (tr->GetTof1HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof1Hits->At(tr->GetTof1HitIndex());
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR)
            return kBMNERROR;
    }

    if (tr->GetCscHitIndex(1) != -1) {
        BmnHit* hit = (BmnHit*)fCscHits->At(tr->GetCscHitIndex(1));
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR)
            return kBMNERROR;
        totChi2 += chi;
    }

    if (tr->GetTof2HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof2Hits->At(tr->GetTof2HitIndex());
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR)
            return kBMNERROR;
    }

    if (tr->GetDchTrackIndex() != -1) {
        BmnTrack* dchTrack = (BmnTrack*)fDchTracks->At(tr->GetDchTrackIndex());
        FairTrackParam dchPar(*(dchTrack->GetParamFirst()));
        if (fKalman->TGeoTrackPropagate(&parFirst, dchPar.GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            return kBMNERROR;
        UpdateTrackParam(&parFirst, &dchPar, chi);
        totChi2 += chi;
    }

    if (!IsParCorrect(&parFirst))
        tr->SetFlag(-1);
    tr->SetParamLast(parFirst);

    FairTrackParam parLast = *(tr->GetParamLast());

    // =============Refitting to the vertex =================

    if (tr->GetTof2HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof2Hits->At(tr->GetTof2HitIndex());
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR)
            return kBMNERROR;
        totChi2 += chi;
    }

    if (tr->GetCscHitIndex(1) != -1) {
        BmnHit* hit = (BmnHit*)fCscHits->At(tr->GetCscHitIndex(1));
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR)
            return kBMNERROR;
        totChi2 += chi;
    }

    if (tr->GetTof1HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof1Hits->At(tr->GetTof1HitIndex());
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR)
            return kBMNERROR;
        totChi2 += chi;
    }

    if (tr->GetCscHitIndex(0) != -1) {
        BmnHit* hit = (BmnHit*)fCscHits->At(tr->GetCscHitIndex(0));
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR)
            return kBMNERROR;
        totChi2 += chi;
    }

    if (fStsTracks) {
        CbmStsTrack* cbmTrack = (CbmStsTrack*)fStsTracks->At(tr->GetGemTrackIndex());
        for (Int_t hitIdx = cbmTrack->GetNStsHits() - 1; hitIdx >= 0; hitIdx--) {
            CbmStsHit* hit = (CbmStsHit*)fStsHits->At(cbmTrack->GetStsHitIndex(hitIdx));
            if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
                return kBMNERROR;
            if (fKalman->Update(&parFirst, (BmnHit*)hit, chi) == kBMNERROR)
                return kBMNERROR;
            totChi2 += chi;
        }
    } else if (fInnerTracks) {
        BmnGemTrack* gemTrack = (BmnGemTrack*)fGemTracks->At(tr->GetGemTrackIndex());
        for (Int_t hitIdx = gemTrack->GetNHits() - 1; hitIdx >= 0; hitIdx--) {
            BmnGemStripHit* hit = (BmnGemStripHit*)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
            if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
                return kBMNERROR;
            if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR)
                return kBMNERROR;
            totChi2 += chi;
        }
    }

    // if (tr->GetSilTrackIndex() != -1) {
    //     BmnTrack* silTrack = (BmnTrack*)fSiliconTracks->At(tr->GetSilTrackIndex());
    //     for (Int_t hitIdx = silTrack->GetNHits() - 1; hitIdx >= 0; hitIdx--) {
    //         BmnSiliconHit* hit = (BmnSiliconHit*)fSilHits->At(silTrack->GetHitIndex(hitIdx));
    //         if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return
    //         kBMNERROR; if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR; totChi2 += chi;
    //     }
    // }

    // if (tr->GetUpstreamTrackIndex() != -1) {
    //     BmnTrack* upsTrack = (BmnTrack*)fUpstreamTracks->At(tr->GetUpstreamTrackIndex());
    //     FairTrackParam upsPar(*(upsTrack->GetParamLast()));
    //     if (fKalman->TGeoTrackPropagate(&parFirst, upsPar.GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return
    //     kBMNERROR; UpdateTrackParam(&parFirst, &upsPar, chi); totChi2 += chi;
    // }

    TVector3 pos;
    if (fVertexL1) {
        pos = TVector3(fVertexL1->GetX(), fVertexL1->GetY(), fVertexL1->GetZ());
    } else {
        pos = TVector3(0.0, 0.0, 0.0);
    }
    TVector3 dpos = TVector3(0.05, 0.05, 0.0);
    if (fKalman->TGeoTrackPropagate(&parFirst, pos.z(), fPDG, nullptr, nullptr) == kBMNERROR)
        return kBMNERROR;
    // update in virtual vertex hit
    BmnHit* vertexHit = new BmnHit(0, pos, dpos, -1);
    // printf("BEFORE\n"); parFirst.Print();
    if (fKalman->Update(&parFirst, vertexHit, chi) == kBMNERROR)
        return kBMNERROR;
    // printf("AFTER\n"); parFirst.Print();
    // printf("chiInVertex = %f\n", chi);
    tr->SetChi2InVertex(chi);

    if (!IsParCorrect(&parFirst))
        tr->SetFlag(-1);
    tr->SetChi2(totChi2);
    tr->SetParamFirst(parFirst);

    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::UpdateMomentum(BmnGlobalTrack* tr)
{
    FairTrackParam par = *(tr->GetParamFirst());
    Int_t pdg = (par.GetQp() > 0) ? 2212 : -211;
    fKalman->TGeoTrackPropagate(&par, -200, pdg, nullptr, nullptr);
    Double_t Alpha_in = ATan(par.GetTx());
    // Double_t Bdl = 2.856;     //Abs(MagFieldIntegral(par, -200.0, 1000.0, 1.0) * 0.001);
    Double_t Bdl = Abs(MagFieldIntegral(par, -200.0, 600.0, 1.0) * 0.001);
    fKalman->TGeoTrackPropagate(&par, 600, pdg, nullptr, nullptr);
    Double_t Alpha_out = ATan(par.GetTx());
    if (tr->GetDchTrackIndex() != -1) {
        BmnTrack* matchedDch = (BmnTrack*)fDchTracks->At(tr->GetDchTrackIndex());
        Alpha_out = ATan(matchedDch->GetParamFirst()->GetTx());
    }
    // if (tr->GetUpstreamTrackIndex() != -1) {
    //     BmnTrack* upTrack = (BmnTrack*)fUpstreamTracks->At(tr->GetUpstreamTrackIndex());
    //     if (upTrack != nullptr)
    //         Alpha_in = ATan(upTrack->GetParamLast()->GetTx());
    // }
    Double_t momInt = 0.3 * Bdl / (Sin(Alpha_out) - Sin(Alpha_in));
    // Double_t momInt = 0.3 * Bdl / (Alpha_out - Alpha_in);
    if (!IsNaN(momInt) && Abs(momInt) > 0.01 && Abs(momInt) < 100) {
        tr->GetParamFirst()->SetQp(1.0 / momInt);
    }
    // cout << Alpha_in << " " << Alpha_out << " " << Bdl << " " << momInt << endl;

    return kBMNSUCCESS;
}

void BmnGlobalTracking::Finish()
{
    delete fKalman;
    printf("Work time of BmnGlobalTracking: %4.2f sec.\n", fTime);
    PrintStatistics();
}

Double_t BmnGlobalTracking::MagFieldIntegral(FairTrackParam& par, Double_t zMin, Double_t zMax, Double_t step)
{
    /*
    field in kG
    step in cm
    */
    Int_t pdg = (par.GetQp() > 0.) ? 2212 : -211;
    FairField* field = FairRunAna::Instance()->GetField();
    fKalman->TGeoTrackPropagate(&par, zMin, pdg, NULL, NULL, kTRUE);
    Double_t z = par.GetZ();
    Double_t integral = 0.0;
    while (z < zMax) {
        z += step;
        fKalman->TGeoTrackPropagate(&par, z, pdg, NULL, NULL, kTRUE);
        integral += field->GetBy(par.GetX(), par.GetY(), par.GetZ());
    }
    integral *= step;
    return integral;
}

void BmnGlobalTracking::PrintStatistics()
{
    printf("================================================================================\n");
    printf("=======  Statistics of Global tracking\n");
    printf("=======  Number of input inner tracks: %ld\n", fNInnerTracks);
    printf("=======  Number of input inner tracks with 4+ hits extrapolated to the Vertex: %ld (%4.1f%%)\n",
           fNGoodInnerTracks, fNGoodInnerTracks * 100.0 / fNInnerTracks);
    printf("=======  Number of tracks matched with near CSC: %ld\n", fNMatchedNearCsc);
    printf("=======  Number of tracks matched with TOF-400: %ld\n", fNMatchedTof400);
    printf("=======  Number of tracks matched with far CSC: %ld\n", fNMatchedFarCsc);
    printf("=======  Number of tracks matched with DCH-1: %ld\n", fNMatchedDch1);
    printf("=======  Number of tracks matched with TOF-700: %ld\n", fNMatchedTof700);
    printf("=======  Number of tracks matched with DCH-2: %ld\n", fNMatchedDch2);
    printf("=======  Number of refitted output tracks: %ld (%4.1f%%)\n", fNGoodGlobalTracks,
           fNGoodGlobalTracks * 100.0 / fNInnerTracks);
    printf("================================================================================\n");
}

void BmnGlobalTracking::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch("BmnDchHit", &fDchHits);
    if (fStsTracks) {
        resultTree->Branch("BmnGlobalTrack", &fGlobalTracks);
    }
    resultTree->Fill();
}
