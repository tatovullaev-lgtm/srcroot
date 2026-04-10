
#include "BmnHypNuclPairFinder.h"

BmnHypNuclPairFinder::BmnHypNuclPairFinder() {}

BmnHypNuclPairFinder::BmnHypNuclPairFinder(TString hyp, Bool_t exp)
{
    if (hyp.Contains("H3L") || hyp.Contains("h3l")) {
        massPos = fMassHe3;
        massNeg = fMassPi;
    } else if (hyp.Contains("H4L") || hyp.Contains("h4l")) {
        massPos = fMassHe4;
        massNeg = fMassPi;
    } else {
        massPos = fMassHe3;
        massNeg = fMassPi;
    }
    fEventCounter = 0;
    fKalman = nullptr;
    fParticlePair = nullptr;
    fVertex = nullptr;
    fTrigger = nullptr;
    fStsHits = nullptr;
    fUpperGemClusters = nullptr;
    fLowerGemClusters = nullptr;
    fUpperFsdClusters = nullptr;
    fLowerFsdClusters = nullptr;
    fStsTracks = nullptr;
    fGlobTracks = nullptr;
    fTof700Hits = nullptr;
    isExp = exp;
}

BmnHypNuclPairFinder::~BmnHypNuclPairFinder()
{
    // delete fDetector;
}

void BmnHypNuclPairFinder::Analysis()
{
    TLorentzVector lPos, lNeg;
    Float_t VPX = fVertex->GetX();
    Float_t VPY = fVertex->GetY();
    Float_t VPZ = fVertex->GetZ();
    Float_t dVPXsq = fVertex->GetCovariance(0, 0);
    Float_t dVPYsq = fVertex->GetCovariance(1, 1);

    vector<FairTrackParam> parInVPPos;
    vector<FairTrackParam> parInVPNeg;
    vector<Float_t> m2Pos700;
    vector<Float_t> chi2NDFPos;
    vector<Float_t> chi2NDFNeg;
    vector<Int_t> nHitsPos;
    vector<Float_t> dedxPos;
    vector<Int_t> nGemHitsNeg;
    vector<Int_t> nGemHitsPos;
    vector<Int_t> nFsdHitsNeg;
    vector<Int_t> nFsdHitsPos;

    TF1* hypCut = new TF1("hypCut", "20000*TMath::Exp(-2.0*TMath::Sqrt(x)) + 600.0", 0, 15);

    // m^2 cut to select He-4 (low)
    TF1* m2CutLow = nullptr;
    // m^2 cut to select He-4 (up)
    TF1* m2CutUp = nullptr;

    if (massPos > 3) {   // He4
        m2CutLow = new TF1("m2CutLow", "-0.0236*x*x+0.0240*x+3.1896", 0, 30);
        m2CutUp = new TF1("m2CutUp", "0.0169*x*x+0.1306*x+4.0525", 0, 30);
    } else {   // He3
        m2CutLow = new TF1("m2CutLow", "-0.0034*x*x-0.0360*x+1.7426", 0, 30);
        m2CutUp = new TF1("m2CutUp", "0.0128*x*x-0.1060*x+2.6261", 0, 30);
    }

    for (Int_t i = 0; i < fGlobTracks->GetEntriesFast(); i++) {
        BmnGlobalTrack* gl = (BmnGlobalTrack*)fGlobTracks->UncheckedAt(i);
        CbmStsTrack* sts = (CbmStsTrack*)fStsTracks->At(gl->GetGemTrackIndex());

        if (!sts->FromVertex())   // хороший ли это кат для пионов? Гелий скорее всего из вершины летят, а вот пионы -
                                  // не ясно...
            continue;

        if (sts->GetNStsHits() < 4)
            continue;

        FairTrackParam par = *(sts->GetParamFirst());
        const Float_t tZ = -0.01942;   // z-coordinate of target for RUN-8
        if (fKalman->TGeoTrackPropagate(&par, tZ, fPDG1, nullptr, nullptr) == kBMNERROR)
            continue;

        Int_t nGEM = 0;
        Int_t nFSD = 0;
        GetNHits(sts, nGEM, nFSD);

        Float_t mom = gl->GetP();
        if (mom > 0) {   //                        He3/He4

            // ДОБАВИТЬ КОРРЕКЦИЮ ИМПУЛЬСА ГЕЛИЯ С УЧЕТОМ ПОПРАВОК БЕТЕ-БЛОХА, КОТОРЫЕ НЕ УЧИТЫВАЕТ КАЛМАН!

            Float_t dedx = gl->GetdQdNLower();
            if (dedx < hypCut->Eval(mom))
                continue;
            Float_t m2 = gl->GetMass2(2);
            if (m2 > m2CutUp->Eval(mom) || m2 < m2CutLow->Eval(mom))
                continue;

            dedxPos.push_back(dedx);
            m2Pos700.push_back(m2);

            parInVPPos.push_back(par);
            chi2NDFPos.push_back(sts->GetChi2() / sts->GetNDF());
            nGemHitsPos.push_back(nGEM);
            nFsdHitsPos.push_back(nFSD);
        } else {   //                               pi-ROR)
            parInVPNeg.push_back(par);
            chi2NDFNeg.push_back(sts->GetChi2() / sts->GetNDF());
            nGemHitsNeg.push_back(nGEM);
            nFsdHitsNeg.push_back(nFSD);
        }
    }

    for (size_t i = 0; i < parInVPPos.size(); ++i) {
        for (size_t j = 0; j < parInVPNeg.size(); ++j) {

            FairTrackParam VP1 = parInVPPos[i];
            FairTrackParam VP2 = parInVPNeg[j];
            FairTrackParam V01 = parInVPPos[i];
            FairTrackParam V02 = parInVPNeg[j];

            Float_t V0Z = FindV0ByVirtualPlanes(&V01, &V02, VPZ, 50.0);
            if (V0Z > 30 || V0Z < 0)
                continue;

            V01 = parInVPPos[i];
            V02 = parInVPNeg[j];

            fKalman->TGeoTrackPropagate(&V01, V0Z, fPDG1, nullptr, nullptr, kTRUE);
            fKalman->TGeoTrackPropagate(&V02, V0Z, fPDG2, nullptr, nullptr, kTRUE);

            Float_t V0X1 = V01.GetX();
            Float_t V0X2 = V02.GetX();
            Float_t V0Y1 = V01.GetY();
            Float_t V0Y2 = V02.GetY();
            Float_t VPX1 = VP1.GetX();
            Float_t VPX2 = VP2.GetX();
            Float_t VPY1 = VP1.GetY();
            Float_t VPY2 = VP2.GetY();

            Float_t dV0X1sq = V01.GetCovariance(0, 0);
            Float_t dV0Y1sq = V01.GetCovariance(1, 1);
            Float_t dV0X2sq = V02.GetCovariance(0, 0);
            Float_t dV0Y2sq = V02.GetCovariance(1, 1);
            Float_t dVPX1sq = VP1.GetCovariance(0, 0);
            Float_t dVPY1sq = VP1.GetCovariance(1, 1);
            Float_t dVPX2sq = VP2.GetCovariance(0, 0);
            Float_t dVPY2sq = VP2.GetCovariance(1, 1);

            Float_t DCA12 = Sqrt(Sq(V0X2 - V0X1) + Sq(V0Y2 - V0Y1));
            Float_t dDCA12 =
                1.0 / DCA12 * Sqrt(Sq(V0X2 - V0X1) * (dV0X1sq + dV0X2sq) + Sq(V0Y2 - V0Y1) * (dV0Y1sq + dV0Y2sq));

            Float_t DCA1 = Sqrt(Sq(VPX1 - VPX) + Sq(VPY1 - VPY));
            Float_t dDCA1 =
                1.0 / DCA1 * Sqrt(Sq(VPX1 - VPX) * (dVPX1sq + dVPXsq) + Sq(VPY1 - VPY) * (dVPY1sq + dVPYsq));
            Float_t DCA2 = Sqrt(Sq(VPX2 - VPX) + Sq(VPY2 - VPY));
            Float_t dDCA2 =
                1.0 / DCA2 * Sqrt(Sq(VPX2 - VPX) * (dVPX2sq + dVPXsq) + Sq(VPY2 - VPY) * (dVPY2sq + dVPYsq));

            Float_t V0X = .5 * (V0X1 + V0X2);
            Float_t V0Y = .5 * (V0Y1 + V0Y2);
            Float_t path = Sqrt(Sq(V0X - VPX) + Sq(V0Y - VPY) + Sq(V0Z - VPZ));

            BmnHypNuclPair partPair;

            partPair.SetM2(m2Pos700[i]);
            partPair.SetChi2NDF(chi2NDFPos[i], chi2NDFNeg[j]);

            partPair.SetDCA1(DCA1);
            partPair.SetDCA2(DCA2);
            partPair.SetDCA12(DCA12);
            partPair.SetdDCA1(dDCA1);
            partPair.SetdDCA2(dDCA2);
            partPair.SetdDCA12(dDCA12);
            partPair.SetPath(path);

            vector<Int_t> nHits1 = {nFsdHitsPos[i], nGemHitsPos[i]};
            vector<Int_t> nHits2 = {nFsdHitsNeg[j], nGemHitsNeg[j]};
            partPair.SetNHitsPair(nHits1, nHits2);

            Float_t p1 = Abs(1. / V01.GetQp());
            p1 *= 2.0;   // p1 is actually p1/Q, Q is 2 for helium
            Float_t p2 = Abs(1. / V02.GetQp());
            Float_t Tx1 = V01.GetTx();
            Float_t Ty1 = V01.GetTy();
            Float_t Tx2 = V02.GetTx();
            Float_t Ty2 = V02.GetTy();
            Float_t A1 = 1. / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
            Float_t A2 = 1. / Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);
            Float_t pz1 = p1 * A1;
            Float_t pz2 = p2 * A2;
            Float_t px1 = Tx1 * pz1;
            Float_t px2 = Tx2 * pz2;
            Float_t py1 = Ty1 * pz1;
            Float_t py2 = Ty2 * pz2;

            partPair.SetMomXYZ1(px1, py1, pz1);
            partPair.SetMomXYZ2(px2, py2, pz2);

            Float_t PzPart0 = pz1 + pz2;
            Float_t PxPart0 = px1 + px2;
            Float_t PyPart0 = py1 + py2;
            Float_t TxPart0 = PxPart0 / PzPart0;
            Float_t TyPart0 = PyPart0 / PzPart0;
            Float_t xPart0 = TxPart0 * (VPZ - V0Z) + V0X;
            Float_t yPart0 = TyPart0 * (VPZ - V0Z) + V0Y;
            Float_t dca0 = Sqrt(Sq(xPart0 - VPX) + Sq(yPart0 - VPY));

            partPair.SetDCA0(dca0);

            lPos.SetXYZM(px1, py1, pz1, massPos);
            lNeg.SetXYZM(px2, py2, pz2, massNeg);

            partPair.SetDedx(dedxPos[i]);
            partPair.SetInvMass(TLorentzVector((lPos + lNeg)).Mag());

            partPair.SetNTrInEvent(fVertex->GetNTracks());

            new ((*fParticlePair)[fParticlePair->GetEntriesFast()]) BmnHypNuclPair(partPair);
        }
    }
    delete hypCut;
    delete m2CutLow;
    delete m2CutUp;
}
// -------------------------------------------------------------------

// // анализ через sts треки (новы мэтчинг, пересчет длины и массы)
// void BmnHypNuclPairFinder::Analysis()
// {
//     TLorentzVector lPos, lNeg;
//     Float_t VPX = fVertex->GetX();
//     Float_t VPY = fVertex->GetY();
//     Float_t VPZ = fVertex->GetZ();
//     Float_t dVPXsq = fVertex->GetCovariance(0, 0);
//     Float_t dVPYsq = fVertex->GetCovariance(1, 1);

//     vector<FairTrackParam> parInVPPos;
//     vector<FairTrackParam> parInVPNeg;
//     vector<Float_t> m2Pos400;
//     vector<Float_t> m2Pos700;
//     vector<Float_t> chi2NDFPos;
//     vector<Float_t> chi2NDFNeg;
//     vector<Int_t> nHitsPos;
//     vector<Float_t> dedxPos;
//     vector<Int_t> nGemHitsNeg;
//     vector<Int_t> nGemHitsPos;
//     vector<Int_t> nFsdHitsNeg;
//     vector<Int_t> nFsdHitsPos;

//     TF1* hypCut = new TF1("hypCut", "20000*TMath::Exp(-2.0*TMath::Sqrt(x)) + 600.0", 0, 15);

//     // m^2 cut to select He-4 (low)
//     TF1* m2CutLow = nullptr;
//     // m^2 cut to select He-4 (up)
//     TF1* m2CutUp = nullptr;

//     if (massPos > 3) {   // He4
//         m2CutLow = new TF1("m2CutLow", "-0.0236*x*x+0.0240*x+3.1896", 0, 30);
//         m2CutUp = new TF1("m2CutUp", "0.0169*x*x+0.1306*x+4.0525", 0, 30);
//     } else {   // He3
//         m2CutLow = new TF1("m2CutLow", "-0.0034*x*x-0.0360*x+1.7426", 0, 30);
//         m2CutUp = new TF1("m2CutUp", "0.0128*x*x-0.1060*x+2.6261", 0, 30);
//     }

//     // создаем истинные пары между самыми близкими треками и хитами.
//     // Но только для положительных, так как pid нужна для гелия
//     map<Float_t, pair<Int_t, Int_t>> distPairs = FindPairs(fStsTracks, fTof700Hits);

//     for (auto const& distPair : distPairs) {
//         // Float_t dist = distPair.first;
//         pair<Int_t, Int_t> idxes = distPair.second;
//         CbmStsTrack* track = (CbmStsTrack*)fStsTracks->At(idxes.first);
//         BmnTofHit* hit = (BmnTofHit*)fTof700Hits->At(idxes.second);
//         if (track->GetFlag() == 777)
//             continue;
//         if (hit->IsUsed())
//             continue;

//         track->SetFlag(777);
//         hit->SetUsing(kTRUE);

//         if (track->GetNStsHits() < 4)
//             continue;

//         Float_t mom = 1.0 / track->GetParamFirst()->GetQp();
//         Float_t dedx = GetdEdx(track);
//         if (dedx < hypCut->Eval(mom))
//             continue;

//         Float_t m2Tof700 = -1000.0;

//         Double_t length1 = 0.0;   // from paramFirst to vertexZ
//         FairTrackParam par = *(track->GetParamFirst());
//         if (fKalman->TGeoTrackPropagate(&par, VPZ, fPDG1, nullptr, &length1) == kBMNERROR)
//             continue;

//         Double_t length2 = 0.0;   // from paramFirst to paramLast
//         // ADD LOOP OVER HITS TO CALCULATE CBMSTSTRACK LENGTH ====>
//         CbmStsHit* cbmhit = (CbmStsHit*)fStsHits->At(track->GetStsHitIndex(0));
//         Double_t xPrev = cbmhit->GetX();
//         Double_t yPrev = cbmhit->GetY();
//         Double_t zPrev = cbmhit->GetZ();
//         for (Int_t iHit = 1; iHit < track->GetNStsHits(); ++iHit) {
//             cbmhit = (CbmStsHit*)fStsHits->At(track->GetStsHitIndex(iHit));
//             length2 += Sqrt(Sq(cbmhit->GetX() - xPrev) + Sq(cbmhit->GetY() - yPrev) + Sq(cbmhit->GetZ() - zPrev));
//             xPrev = cbmhit->GetX();
//             yPrev = cbmhit->GetY();
//             zPrev = cbmhit->GetZ();
//         }
//         //<====

//         Double_t length3_701 = 0.0;   // from paramLast to TOF 701 hit
//         FairTrackParam parL = *(track->GetParamLast());
//         if (fKalman->TGeoTrackPropagate(&parL, hit->GetZ(), fPDG1, nullptr, &length3_701) == kBMNERROR)
//             continue;
//         Float_t beta = (length1 + length2 + length3_701) / hit->GetTimeStamp() / (TMath::C() * 1e-7);
//         m2Tof700 = mom * mom * (1.0 / beta / beta - 1.0);

//         if (m2Tof700 > m2CutUp->Eval(mom) || m2Tof700 < m2CutLow->Eval(mom))
//             continue;

//         Int_t nGEM = 0;
//         Int_t nFSD = 0;
//         GetNHits(track, nGEM, nFSD);

//         parInVPPos.push_back(par);
//         m2Pos400.push_back(-1000);
//         m2Pos700.push_back(m2Tof700);
//         chi2NDFPos.push_back(track->GetChi2() / track->GetNDF());
//         dedxPos.push_back(dedx);
//         nGemHitsPos.push_back(nGEM);
//         nFsdHitsPos.push_back(nFSD);
//     }

//     // далее, обрабатываем отрицательные треки
//     for (Int_t i = 0; i < fStsTracks->GetEntriesFast(); i++) {
//         CbmStsTrack* track = (CbmStsTrack*)fStsTracks->UncheckedAt(i);

//         if (track->GetParamFirst()->GetQp() > 0.0)
//             continue;

//         if (track->GetNStsHits() < 4)
//             continue;
//         Int_t nGEM = 0;
//         Int_t nFSD = 0;
//         GetNHits(track, nGEM, nFSD);

//         FairTrackParam par = *(track->GetParamFirst());
//         if (fKalman->TGeoTrackPropagate(&par, VPZ, fPDG2, nullptr, nullptr) == kBMNERROR)
//             continue;
//         parInVPNeg.push_back(par);
//         chi2NDFNeg.push_back(track->GetChi2() / track->GetNDF());
//         nGemHitsNeg.push_back(nGEM);
//         nFsdHitsNeg.push_back(nFSD);
//     }

//     // cout << "size1 = " << parInVPPos.size() << " " << "size2 = " << parInVPNeg.size() << endl;

//     // for (FairTrackParam par1 : parInVPPos) {
//     //     for (FairTrackParam par2 : parInVPNeg) {
//     for (size_t i = 0; i < parInVPPos.size(); ++i) {
//         for (size_t j = 0; j < parInVPNeg.size(); ++j) {

//             FairTrackParam VP1 = parInVPPos[i];
//             FairTrackParam VP2 = parInVPNeg[j];
//             FairTrackParam V01 = parInVPPos[i];
//             FairTrackParam V02 = parInVPNeg[j];

//             Float_t V0Z = FindV0ByVirtualPlanes(&V01, &V02, VPZ, 50.0);
//             if (V0Z > 30 || V0Z < -1)
//                 continue;

//             V01 = parInVPPos[i];
//             V02 = parInVPNeg[j];

//             fKalman->TGeoTrackPropagate(&V01, V0Z, fPDG1, nullptr, nullptr, kTRUE);
//             fKalman->TGeoTrackPropagate(&V02, V0Z, fPDG2, nullptr, nullptr, kTRUE);

//             Float_t V0X1 = V01.GetX();
//             Float_t V0X2 = V02.GetX();
//             Float_t V0Y1 = V01.GetY();
//             Float_t V0Y2 = V02.GetY();
//             Float_t VPX1 = VP1.GetX();
//             Float_t VPX2 = VP2.GetX();
//             Float_t VPY1 = VP1.GetY();
//             Float_t VPY2 = VP2.GetY();

//             Float_t dV0X1sq = V01.GetCovariance(0, 0);
//             Float_t dV0Y1sq = V01.GetCovariance(1, 1);
//             Float_t dV0X2sq = V02.GetCovariance(0, 0);
//             Float_t dV0Y2sq = V02.GetCovariance(1, 1);
//             Float_t dVPX1sq = VP1.GetCovariance(0, 0);
//             Float_t dVPY1sq = VP1.GetCovariance(1, 1);
//             Float_t dVPX2sq = VP2.GetCovariance(0, 0);
//             Float_t dVPY2sq = VP2.GetCovariance(1, 1);

//             Float_t DCA12 = Sqrt(Sq(V0X2 - V0X1) + Sq(V0Y2 - V0Y1));
//             Float_t dDCA12 =
//                 1.0 / DCA12 * Sqrt(Sq(V0X2 - V0X1) * (dV0X1sq + dV0X2sq) + Sq(V0Y2 - V0Y1) * (dV0Y1sq + dV0Y2sq));

//             Float_t DCA1 = Sqrt(Sq(VPX1 - VPX) + Sq(VPY1 - VPY));
//             Float_t dDCA1 =
//                 1.0 / DCA1 * Sqrt(Sq(VPX1 - VPX) * (dVPX1sq + dVPXsq) + Sq(VPY1 - VPY) * (dVPY1sq + dVPYsq));
//             Float_t DCA2 = Sqrt(Sq(VPX2 - VPX) + Sq(VPY2 - VPY));
//             Float_t dDCA2 =
//                 1.0 / DCA2 * Sqrt(Sq(VPX2 - VPX) * (dVPX2sq + dVPXsq) + Sq(VPY2 - VPY) * (dVPY2sq + dVPYsq));

//             Float_t V0X = .5 * (V0X1 + V0X2);
//             Float_t V0Y = .5 * (V0Y1 + V0Y2);
//             Float_t path = Sqrt(Sq(V0X - VPX) + Sq(V0Y - VPY) + Sq(V0Z - VPZ));

//             BmnHypNuclPair partPair;

//             // put mass2 for positive particle in field beta (let it be)
//             // for nevative one put -100, we don't need PID for pion
//             partPair.SetM2(m2Pos700[i]);
//             partPair.SetChi2NDF(chi2NDFPos[i], chi2NDFNeg[j]);

//             partPair.SetDCA1(DCA1);
//             partPair.SetDCA2(DCA2);
//             partPair.SetDCA12(DCA12);
//             partPair.SetdDCA1(dDCA1);
//             partPair.SetdDCA2(dDCA2);
//             partPair.SetdDCA12(dDCA12);
//             partPair.SetPath(path);

//             vector<Int_t> nHits1 = {nFsdHitsPos[i], nGemHitsPos[i]};
//             vector<Int_t> nHits2 = {nFsdHitsNeg[j], nGemHitsNeg[j]};
//             partPair.SetNHitsPair(nHits1, nHits2);

//             Float_t p1 = Abs(1. / V01.GetQp());
//             p1 *= 2.0;   // p1 is actually p1/Q, Q is 2 for helium
//             Float_t p2 = Abs(1. / V02.GetQp());
//             Float_t Tx1 = V01.GetTx();
//             Float_t Ty1 = V01.GetTy();
//             Float_t Tx2 = V02.GetTx();
//             Float_t Ty2 = V02.GetTy();
//             Float_t A1 = 1. / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
//             Float_t A2 = 1. / Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);
//             Float_t pz1 = p1 * A1;
//             Float_t pz2 = p2 * A2;
//             Float_t px1 = Tx1 * pz1;
//             Float_t px2 = Tx2 * pz2;
//             Float_t py1 = Ty1 * pz1;
//             Float_t py2 = Ty2 * pz2;

//             partPair.SetMomXYZ1(px1, py1, pz1);
//             partPair.SetMomXYZ2(px2, py2, pz2);

//             Float_t PzPart0 = pz1 + pz2;
//             Float_t PxPart0 = px1 + px2;
//             Float_t PyPart0 = py1 + py2;
//             Float_t TxPart0 = PxPart0 / PzPart0;
//             Float_t TyPart0 = PyPart0 / PzPart0;
//             Float_t xPart0 = TxPart0 * (VPZ - V0Z) + V0X;
//             Float_t yPart0 = TyPart0 * (VPZ - V0Z) + V0Y;
//             Float_t dca0 = Sqrt(Sq(xPart0 - VPX) + Sq(yPart0 - VPY));

//             partPair.SetDCA0(dca0);

//             // partPair.SetEtaPair(-100, -100/*_eta1, _eta2*/);

//             lPos.SetXYZM(px1, py1, pz1, massPos);
//             lNeg.SetXYZM(px2, py2, pz2, massNeg);

//             partPair.SetDedx1(dedxPos[i]);
//             partPair.SetInvMass(TLorentzVector((lPos + lNeg)).Mag());

//             // partPair.SetTxPair(Tx1, Tx2);
//             // partPair.SetTyPair(Ty1, Ty2);
//             partPair.SetMass1(massPos);
//             partPair.SetMass2(massNeg);

//             partPair.SetNTrInEvent(fVertex->GetNTracks());

//             new ((*fParticlePair)[fParticlePair->GetEntriesFast()]) BmnHypNuclPair(partPair);
//         }
//     }
//     delete hypCut;
//     delete m2CutLow;
//     delete m2CutUp;
// }
// -------------------------------------------------------------------

InitStatus BmnHypNuclPairFinder::Init()
{
    cout << "\nBmnHypNuclPairFinder::Init()" << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fStsTracks = (TClonesArray*)ioman->GetObject("StsVector");
    fGlobTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
    fStsHits = (TClonesArray*)ioman->GetObject("StsHit");
    // fVertex = (CbmVertex*)ioman->GetObject("PrimaryVertex.");
    fVertex = (CbmVertex*)ioman->GetObject("MpdVertex.");
    fTrigger = (BmnTrigInfoDst*)ioman->GetObject("BmnTrigInfo.");
    fUpperGemClusters = (TClonesArray*)ioman->GetObject("BmnGemUpperCluster");
    fLowerGemClusters = (TClonesArray*)ioman->GetObject("BmnGemLowerCluster");
    fUpperFsdClusters = (TClonesArray*)ioman->GetObject("BmnSiliconUpperCluster");
    fLowerFsdClusters = (TClonesArray*)ioman->GetObject("BmnSiliconLowerCluster");
    fTof700Hits = (TClonesArray*)ioman->GetObject("BmnTof700Hit");
    // fTof700Hits = (TClonesArray*)ioman->GetObject("BmnTof701Hit");

    fParticlePair = new TClonesArray("BmnHypNuclPair");
    ioman->Register("pairs", "HYP", fParticlePair, kTRUE);

    fKalman = new BmnKalmanFilter();

    return kSUCCESS;
}

// -------------------------------------------------------------------

void BmnHypNuclPairFinder::Exec(Option_t* option)
{
    fParticlePair->Delete();

    fEventCounter++;

    // if (fEventCounter % 100 == 0)
    cout << "Event# " << fEventCounter << endl;

    if (isExp) {
        if (!(fTrigger->IsTriggerBitTrueAR(7)))   // bit #7 is for CCT2
            return;
        if (fVertex->GetNTracks() < 2)
            return;
        if (fVertex->GetZ() < -0.5 || fVertex->GetZ() > +0.5)
            return;

        Float_t centerOfTargetX = 0.40;
        Float_t centerOfTargetY = 0.15;
        Float_t radiusOfTarget = 1.2;

        if (TMath::Sqrt(TMath::Sq(fVertex->GetX() - centerOfTargetX) + TMath::Sq(fVertex->GetY() - centerOfTargetY))
            > radiusOfTarget)
            return;
    }
    Analysis();
}
// -------------------------------------------------------------------

void BmnHypNuclPairFinder::Finish()
{
    delete fKalman;
    // delete fMagField;
    cout << "\n-I- [BmnHypNuclPairFinder::Finish] " << endl;
}

Float_t BmnHypNuclPairFinder::FindV0ByVirtualPlanes(FairTrackParam* par1,
                                                    FairTrackParam* par2,
                                                    Float_t z_0,
                                                    Float_t range)
{
    const Int_t nPlanes = 5;   // FIXME

    while (range >= 0.1) {
        Float_t zMax = z_0 + range;
        Float_t zMin = z_0 - range;
        Float_t zStep = (zMax - zMin) / nPlanes;

        Float_t minDist = 1000.0;
        Float_t minZ = -1000.0;

        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            Float_t z = zMax - iPlane * zStep;
            BmnStatus stat1 = fKalman->TGeoTrackPropagate(par1, z, fPDG1, nullptr, nullptr, kTRUE);
            BmnStatus stat2 = fKalman->TGeoTrackPropagate(par2, z, fPDG2, nullptr, nullptr, kTRUE);

            if (stat1 == kBMNERROR || stat2 == kBMNERROR)
                return DBL_MAX;

            Float_t d = Sqrt(Sq(par1->GetX() - par2->GetX()) + Sq(par1->GetY() - par2->GetY()));
            if (d < minDist) {
                minDist = d;
                minZ = z;
            }
        }

        z_0 = minZ;
        range *= 0.5;
    }
    return z_0;
}

Float_t BmnHypNuclPairFinder::GetdEdx(CbmStsTrack* tr)
{
    if (isExp) {
        // set<Float_t> signalsUpp;
        set<Float_t> signalsLow;
        Int_t nHits = tr->GetNStsHits();
        Int_t nGemHits = 0;
        for (Int_t hitIdx = 0; hitIdx < nHits; ++hitIdx) {
            CbmStsHit* hit = (CbmStsHit*)fStsHits->At(tr->GetStsHitIndex(hitIdx));

            if (hit->GetSystemId() != kGEM)
                continue;

            nGemHits++;

            // StripCluster* upp = (StripCluster*)fUpperGemClusters->At(hit->fDigiB % 1000000);
            StripCluster* low = (StripCluster*)fLowerGemClusters->At(hit->fDigiF % 1000000);

            // signalsUpp.insert(upp->TotalSignal * aUpp[hit->GetStationNr() - 1] + bUpp[hit->GetStationNr() - 1]);
            signalsLow.insert(low->TotalSignal * aLow[hit->GetStationNr() - 1] + bLow[hit->GetStationNr() - 1]);
        }

        if (nGemHits < 3)
            return 0.0;

        Int_t usedHits = (nGemHits == 3)   ? 2
                         : (nGemHits == 4) ? 2
                         : (nGemHits == 5) ? 3
                         : (nGemHits == 6) ? 4
                         : (nGemHits == 7) ? 4
                                           : 0;

        // auto itUpp = signalsUpp.begin();
        auto itLow = signalsLow.begin();

        Float_t totSigLow = 0.0;
        // Float_t totSigUpp = 0.0;
        for (Int_t i = 0; i < usedHits; ++i) {
            // totSigUpp += (*itUpp);
            totSigLow += (*itLow);
            // itUpp++;
            itLow++;
        }

        totSigLow /= usedHits;
        // totSigUpp /= usedHits;
        // return 0.5 * (totSigLow + totSigUpp);
        return totSigLow;
    } else {
        return 10000;
    }
}

void BmnHypNuclPairFinder::GetNHits(CbmStsTrack* tr, Int_t& nGEM, Int_t& nFSD)
{
    nGEM = 0;
    nFSD = 0;
    for (Int_t hitIdx = 0; hitIdx < tr->GetNStsHits(); ++hitIdx) {
        CbmStsHit* hit = (CbmStsHit*)fStsHits->At(tr->GetStsHitIndex(hitIdx));

        if (hit->GetSystemId() == kGEM)
            nGEM++;
        else if (hit->GetSystemId() == kSILICON)
            nFSD++;
    }
}

Float_t BmnHypNuclPairFinder::GetDyTof700(FairTrackParam* par)
{
    if (isExp) {
        Float_t mom = 1.0 / par->GetQp();
        return (mom > 0) ? 0.17 * mom - 0.40 : 0.0;
    } else
        return 0.0;
}

Float_t BmnHypNuclPairFinder::GetDxTof700(FairTrackParam* par)
{
    if (isExp) {
        return 0.0;
    } else
        return 0.0;
}

Float_t BmnHypNuclPairFinder::GetSigXTof700He3(FairTrackParam* par)
{
    if (isExp) {
        Float_t mom = 1.0 / par->GetQp();
        return (mom > 0) ? 6.58 * TMath::Exp(-1.73 * mom) + 0.69 : 1.0;
    } else
        return 1.0;
}

Float_t BmnHypNuclPairFinder::GetSigYTof700He3(FairTrackParam* par)
{
    if (isExp) {
        Float_t mom = 1.0 / par->GetQp();
        return (mom > 0) ? 7.52 * TMath::Exp(-2.08 * mom) + 0.53 : 1.0;
    } else
        return 1.0;
}

map<Float_t, pair<Int_t, Int_t>> BmnHypNuclPairFinder::FindPairs(TClonesArray* tracks, TClonesArray* hits)
{

    map<Float_t, pair<Int_t, Int_t>> distancesPairs;

    // Double_t minZ = 10000.0;
    // for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
    //     BmnHit* hit = (BmnHit*)hits->At(hitIdx);
    //     if (hit->GetZ() < minZ)
    //         minZ = hit->GetZ();
    // }

    Float_t zTof701HitsMin = 616;   // median z position for tof701

    for (Int_t trIdx = 0; trIdx < tracks->GetEntriesFast(); ++trIdx) {
        CbmStsTrack* tr = (CbmStsTrack*)tracks->At(trIdx);
        if (tr->GetParamFirst()->GetQp() < 0.0)
            continue;
        FairTrackParam parMinZ(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&parMinZ, zTof701HitsMin, fPDG1, nullptr, nullptr) == kBMNERROR)
            continue;
        for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
            BmnHit* hit = (BmnHit*)hits->At(hitIdx);
            FairTrackParam param = parMinZ;
            if (fKalman->TGeoTrackPropagate(&param, hit->GetZ(), fPDG1, nullptr, nullptr) == kBMNERROR)
                continue;
            Float_t dX = Abs(param.GetX() - GetDxTof700(&param) - hit->GetX());
            Float_t dY = Abs(param.GetY() - GetDyTof700(&param) - hit->GetY());
            Float_t xCut = 3.0 * GetSigXTof700He3(&param);
            Float_t yCut = 3.0 * GetSigYTof700He3(&param);
            if (dX < xCut && dY < yCut) {
                Float_t dist = Sqrt(dX * dX + dY * dY);
                hit->SetUsing(kFALSE);
                distancesPairs.insert(pair<Float_t, pair<Int_t, Int_t>>(dist, pair<Int_t, Int_t>(trIdx, hitIdx)));
            }
        }
    }

    return distancesPairs;
}

ClassImp(BmnHypNuclPairFinder);
