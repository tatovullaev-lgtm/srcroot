
#include "BmnH3LTripleFinder.h"
BmnH3LTripleFinder::BmnH3LTripleFinder()
    : fEventCounter(0)
    , fKalman(nullptr)
    , fParticleTriple(nullptr)
{
    fVertex = nullptr;
}

BmnH3LTripleFinder::~BmnH3LTripleFinder()
{
    // delete fDetector;
}

void BmnH3LTripleFinder::Analysis()
{
    TLorentzVector lPos1, lPos2, lNeg;
    Double_t VPX = fVertex->GetX();
    Double_t VPY = fVertex->GetY();
    Double_t VPZ = fVertex->GetZ();
    Double_t dVPXsq = fVertex->GetCovariance(0, 0);
    Double_t dVPYsq = fVertex->GetCovariance(1, 1);

    vector<FairTrackParam> parInVPPos1;
    vector<FairTrackParam> parInVPPos2;
    vector<FairTrackParam> parInVPNeg;
    vector<Double_t> m2Pos1_400;
    vector<Double_t> m2Pos1_700;
    vector<Double_t> m2Pos2_400;
    vector<Double_t> m2Pos2_700;
    vector<Double_t> chi2NDFPos1;
    vector<Double_t> chi2NDFPos2;
    vector<Double_t> chi2NDFNeg;
    vector<Int_t> nHitsPos1;
    vector<Int_t> nHitsPos2;
    vector<Double_t> dedxPos1;
    vector<Double_t> dedxPos2;
    vector<Int_t> nGemHitsNeg;
    vector<Int_t> nGemHitsPos1;
    vector<Int_t> nGemHitsPos2;
    vector<Int_t> nFsdHitsNeg;
    vector<Int_t> nFsdHitsPos1;
    vector<Int_t> nFsdHitsPos2;

    // cout << FairRunAna::Instance()->GetField()->GetBy(0, 0, 124) << endl;
    // cout << FairRunAna::Instance()->GetGeoFile() << endl;

    // if (fStsTracks->GetEntriesFast() > 100) return; //skip too huge events FIXME!!!

    for (Int_t i = 0; i < fStsTracks->GetEntriesFast(); i++) {
        CbmStsTrack* tr = (CbmStsTrack*)fStsTracks->UncheckedAt(i);

        // if (tr->GetNStsHits() < 4) continue;
        Int_t nGEM = 0;
        Int_t nFSD = 0;
        GetNHits(tr, nGEM, nFSD);

        Float_t mom = 1.0 / tr->GetParamFirst()->GetQp();

        // pi-minus mom: from -0.6 to 0
        // He3 mom: from +0.5 to ...

        Int_t pdg = 0;
        if (mom < 0.0)
            pdg = fPDG3;
        else if (mom > 0.0)
            pdg = fPDG2;

        // if (pdg == fPDG1 && nGEM < 3) continue;

        FairTrackParam par = *(tr->GetParamFirst());
        // cout << " BEFORE: " << 1.0 / par->GetQp() << endl;
        if (fKalman->TGeoTrackPropagate(&par, VPZ, pdg, nullptr, nullptr) == kBMNERROR)
            continue;
        // cout << "  AFTER: " << 1.0 / par->GetQp() << endl;

        Double_t m2Tof400 = 0.0;
        Double_t m2Tof700 = 0.0;
        for (Int_t j = 0; j < fGlobTracks->GetEntriesFast(); j++) {
            BmnGlobalTrack* gl = (BmnGlobalTrack*)fGlobTracks->UncheckedAt(j);
            if (gl->GetGemTrackIndex() == i) {
                m2Tof400 = gl->GetMass2(1);
                m2Tof700 = gl->GetMass2(2);
                break;
            }
        }

        if (pdg == fPDG3) {
            parInVPNeg.push_back(par);
            chi2NDFNeg.push_back(tr->GetChi2() / tr->GetNDF());
            nGemHitsNeg.push_back(nGEM);
            nFsdHitsNeg.push_back(nFSD);
        }

        else
        {
            if (m2Tof400 > 2 || m2Tof700 > 2) {
                parInVPPos1.push_back(par);
                // cout << gl << endl;
                m2Pos1_400.push_back(m2Tof400);
                m2Pos1_700.push_back(m2Tof700);
                chi2NDFPos1.push_back(tr->GetChi2() / tr->GetNDF());
                nHitsPos1.push_back(tr->GetNStsHits());
                dedxPos1.push_back(GetdEdx(tr));
                nGemHitsPos1.push_back(nGEM);
                nFsdHitsPos1.push_back(nFSD);
            }

            else
            {
                parInVPPos2.push_back(par);
                // cout << gl << endl;
                m2Pos2_400.push_back(m2Tof400);
                m2Pos2_700.push_back(m2Tof700);
                chi2NDFPos2.push_back(tr->GetChi2() / tr->GetNDF());
                nHitsPos2.push_back(tr->GetNStsHits());
                dedxPos2.push_back(GetdEdx(tr));
                nGemHitsPos2.push_back(nGEM);
                nFsdHitsPos2.push_back(nFSD);
            }
        }

        // cout << endl;
        // cout << "VPZ = " << Vpz << endl;
        // par.Print();
    }

    // cout << "size1 = " << parInVPPos.size() << " " << "size2 = " << parInVPNeg.size() << endl;

    // for (FairTrackParam par1 : parInVPPos) {
    //     for (FairTrackParam par2 : parInVPNeg) {
    for (Int_t i = 0; i < (Int_t)parInVPPos1.size(); ++i) {
        for (Int_t j = 0; j < (Int_t)parInVPPos2.size(); ++j) {
            for (Int_t k = 0; k < (Int_t)parInVPNeg.size(); ++k) {

                FairTrackParam VP1 = parInVPPos1[i];
                FairTrackParam VP2 = parInVPPos2[j];
                FairTrackParam VP3 = parInVPNeg[k];
                FairTrackParam V01 = parInVPPos1[i];
                FairTrackParam V02 = parInVPPos2[j];
                FairTrackParam V03 = parInVPNeg[k];

                Double_t V0Z = FindV0ByVirtualPlanes(&V01, &V02, &V03, VPZ, 25.0);
                // if (V0Z > 30 || V0Z < -1) continue;

                if (fKalman->TGeoTrackPropagate(&V01, V0Z, fPDG1, nullptr, nullptr, kTRUE) == kBMNERROR)
                    continue;
                if (fKalman->TGeoTrackPropagate(&V02, V0Z, fPDG2, nullptr, nullptr, kTRUE) == kBMNERROR)
                    continue;
                if (fKalman->TGeoTrackPropagate(&V03, V0Z, fPDG3, nullptr, nullptr, kTRUE) == kBMNERROR)
                    continue;

                Double_t V0X1 = V01.GetX();
                Double_t V0X2 = V02.GetX();
                Double_t V0X3 = V03.GetX();
                Double_t V0Y1 = V01.GetY();
                Double_t V0Y2 = V02.GetY();
                Double_t V0Y3 = V03.GetY();
                Double_t VPX1 = VP1.GetX();
                Double_t VPX2 = VP2.GetX();
                Double_t VPX3 = VP3.GetX();
                Double_t VPY1 = VP1.GetY();
                Double_t VPY2 = VP2.GetY();
                Double_t VPY3 = VP3.GetY();

                Double_t dV0X1sq = V01.GetCovariance(0, 0);
                Double_t dV0Y1sq = V01.GetCovariance(1, 1);
                Double_t dV0X2sq = V02.GetCovariance(0, 0);
                Double_t dV0Y2sq = V02.GetCovariance(1, 1);
                Double_t dV0X3sq = V03.GetCovariance(0, 0);
                Double_t dV0Y3sq = V03.GetCovariance(1, 1);
                Double_t dVPX1sq = VP1.GetCovariance(0, 0);
                Double_t dVPY1sq = VP1.GetCovariance(1, 1);
                Double_t dVPX2sq = VP2.GetCovariance(0, 0);
                Double_t dVPY2sq = VP2.GetCovariance(1, 1);
                Double_t dVPX3sq = VP3.GetCovariance(0, 0);
                Double_t dVPY3sq = VP3.GetCovariance(1, 1);

                Double_t DCA12 = Sqrt(Sq(V0X2 - V0X1) + Sq(V0Y2 - V0Y1));
                Double_t DCA13 = Sqrt(Sq(V0X3 - V0X1) + Sq(V0Y3 - V0Y1));
                Double_t DCA23 = Sqrt(Sq(V0X3 - V0X2) + Sq(V0Y3 - V0Y2));
                Double_t DCA123 = (DCA12 + DCA13 + DCA23) / 3.0;

                Double_t dDCA12 =
                    1.0 / DCA12 * Sqrt(Sq(V0X2 - V0X1) * (dV0X1sq + dV0X2sq) + Sq(V0Y2 - V0Y1) * (dV0Y1sq + dV0Y2sq));
                Double_t dDCA13 =
                    1.0 / DCA13 * Sqrt(Sq(V0X3 - V0X1) * (dV0X1sq + dV0X3sq) + Sq(V0Y3 - V0Y1) * (dV0Y1sq + dV0Y3sq));
                Double_t dDCA23 =
                    1.0 / DCA23 * Sqrt(Sq(V0X3 - V0X2) * (dV0X2sq + dV0X3sq) + Sq(V0Y3 - V0Y2) * (dV0Y2sq + dV0Y3sq));
                Double_t dDCA123 = Sqrt(Sq(dDCA12) + Sq(dDCA13) + Sq(dDCA23));

                Double_t DCA1 = Sqrt(Sq(VPX1 - VPX) + Sq(VPY1 - VPY));
                Double_t dDCA1 =
                    1.0 / DCA1 * Sqrt(Sq(VPX1 - VPX) * (dVPX1sq + dVPXsq) + Sq(VPY1 - VPY) * (dVPY1sq + dVPYsq));
                Double_t DCA2 = Sqrt(Sq(VPX2 - VPX) + Sq(VPY2 - VPY));
                Double_t dDCA2 =
                    1.0 / DCA2 * Sqrt(Sq(VPX2 - VPX) * (dVPX2sq + dVPXsq) + Sq(VPY2 - VPY) * (dVPY2sq + dVPYsq));
                Double_t DCA3 = Sqrt(Sq(VPX3 - VPX) + Sq(VPY3 - VPY));
                Double_t dDCA3 =
                    1.0 / DCA3 * Sqrt(Sq(VPX3 - VPX) * (dVPX3sq + dVPXsq) + Sq(VPY3 - VPY) * (dVPY3sq + dVPYsq));

                Double_t V0X = (V0X1 + V0X2 + V0X3) / 3.0;
                Double_t V0Y = (V0Y1 + V0Y2 + V0Y3) / 3.0;
                Double_t path = Sqrt(Sq(V0X - VPX) + Sq(V0Y - VPY) + Sq(V0Z - VPZ));

                BmnParticleTriple partTriple;

                // put mass2 for positive particle in field beta (let it be)
                // for nevative one put -100, we don't need PID for pion
                partTriple.SetMass400Triple(m2Pos1_400[i], m2Pos2_400[j], -100);
                partTriple.SetMass700Triple(m2Pos1_700[i], m2Pos2_700[j], -100);
                partTriple.SetChi2Triple(chi2NDFPos1[i], chi2NDFPos2[j], chi2NDFNeg[k]);

                partTriple.SetV0Z(V0Z);
                partTriple.SetV0X(V0X);
                partTriple.SetV0Y(V0Y);

                partTriple.SetDCA1(DCA1);
                partTriple.SetDCA2(DCA2);
                partTriple.SetDCA3(DCA3);
                partTriple.SetDCA123(DCA123);
                partTriple.SetdDCA1(dDCA1);
                partTriple.SetdDCA2(dDCA2);
                partTriple.SetdDCA3(dDCA3);
                partTriple.SetdDCA123(dDCA123);
                partTriple.SetPath(path);

                vector<Int_t> nHits1 = {nFsdHitsPos1[i], nGemHitsPos1[i]};
                vector<Int_t> nHits2 = {nFsdHitsPos2[j], nGemHitsPos2[j]};
                vector<Int_t> nHits3 = {nFsdHitsNeg[k], nGemHitsNeg[k]};
                partTriple.SetNHitsTriple(nHits1, nHits2, nHits3);

                // Track params. are redefined
                Double_t Tx1, Ty1, Tx2, Ty2, Tx3, Ty3;
                Double_t A1, A2, A3;

                // TVector2 armenPodol;

                Tx1 = V01.GetTx();
                Ty1 = V01.GetTy();
                Tx2 = V02.GetTx();
                Ty2 = V02.GetTy();
                Tx3 = V03.GetTx();
                Ty3 = V03.GetTy();

                Float_t p1 = 1. / V01.GetQp();
                Float_t p2 = 1. / V02.GetQp();
                Float_t p3 = 1. / V03.GetQp();
                partTriple.SetMomTriple(p1, p2, p3 /*_p1, _p2*/);
                partTriple.SetEtaTriple(-100, -100, -100 /*_eta1, _eta2*/);

                // armenPodol = ArmenterosPodol(V01, V02);

                A1 = 1. / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
                lPos1.SetXYZM(Tx1 * A1 * p1, Ty1 * A1 * p1, A1 * p1, fMassD);

                A2 = 1. / Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);
                lPos2.SetXYZM(Tx2 * A2 * p2, Ty2 * A2 * p2, A2 * p2, fMassP);

                p3 *= -1.;   // Since in the calculations pos. mom. values should be used

                A3 = 1. / Sqrt(Tx3 * Tx3 + Ty3 * Ty3 + 1);
                lNeg.SetXYZM(Tx3 * A3 * p3, Ty3 * A3 * p3, p3 * A3, fMassPi);

                partTriple.SetAlpha1(dedxPos1[i]);
                partTriple.SetAlpha2(dedxPos2[j]);   // just use free space for dEdX
                // partTriple.SetPtPodol(armenPodol.Y());
                partTriple.SetInvMass(TLorentzVector((lPos1 + lPos2 + lNeg)).Mag());

                // Part to be used when estimating omega ... (dca0 needed), 0 means a primary particle to be decayed...
                partTriple.SetTxTriple(Tx1, Tx2, Tx3);
                partTriple.SetTyTriple(Ty1, Ty2, Ty3);

                Double_t PzPart1 = Abs(p1) * A1;
                Double_t PzPart2 = Abs(p2) * A2;
                Double_t PzPart3 = Abs(p3) * A3;

                Double_t PzPart0 = PzPart1 + PzPart2 + PzPart3;
                Double_t PxPart0 = PzPart1 * Tx1 + PzPart2 * Tx2 + PzPart3 * Tx3;
                Double_t PyPart0 = PzPart1 * Ty1 + PzPart2 * Ty2 + PzPart3 * Ty3;

                Double_t TxPart0 = PxPart0 / PzPart0;
                Double_t TyPart0 = PyPart0 / PzPart0;

                Double_t xPart0 = TxPart0 * (VPZ - V0Z) + V0X;
                Double_t yPart0 = TyPart0 * (VPZ - V0Z) + V0Y;

                Double_t dca0 = Sqrt(Sq(xPart0 - VPX) + Sq(yPart0 - VPY));

                partTriple.SetDCA0(dca0);

                new ((*fParticleTriple)[fParticleTriple->GetEntriesFast()]) BmnParticleTriple(partTriple);
            }
        }
    }
}
// -------------------------------------------------------------------

InitStatus BmnH3LTripleFinder::Init()
{
    cout << "\nBmnH3LTripleFinder::Init()" << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fStsTracks = (TClonesArray*)ioman->GetObject("StsVector");
    fGlobTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
    fStsHits = (TClonesArray*)ioman->GetObject("StsHit");
    fVertex = (CbmVertex*)ioman->GetObject("PrimaryVertex.");
    fUpperClusters = (TClonesArray*)ioman->GetObject("BmnGemUpperCluster");
    fLowerClusters = (TClonesArray*)ioman->GetObject("BmnGemLowerCluster");

    fParticleTriple = new TClonesArray("BmnParticleTriple");
    ioman->Register("par", "H3L", fParticleTriple, kTRUE);

    fKalman = new BmnKalmanFilter();

    return kSUCCESS;
}

// -------------------------------------------------------------------

void BmnH3LTripleFinder::Exec(Option_t* option)
{
    fParticleTriple->Delete();

    fEventCounter++;

    // if (fEventCounter % 100 == 0)
    cout << "Event# " << fEventCounter << endl;

    if (fVertex->GetNTracks() < 2)
        return;
    // if (fVertex->GetX() < -1.0 || fVertex->GetX() > +1.5) return;
    // if (fVertex->GetY() < -1.0 || fVertex->GetY() > +1.2) return;
    // if (fVertex->GetZ() < -0.5 || fVertex->GetZ() > +0.5) return;

    Analysis();
}
// -------------------------------------------------------------------

void BmnH3LTripleFinder::Finish()
{
    delete fKalman;
    // delete fMagField;
    cout << "\n-I- [BmnH3LTripleFinder::Finish] " << endl;
}

Double_t BmnH3LTripleFinder::FindV0ByVirtualPlanes(FairTrackParam* par1,
                                                   FairTrackParam* par2,
                                                   FairTrackParam* par3,
                                                   Double_t z_0,
                                                   Double_t range)
{
    const Int_t nPlanes = 15;   // FIXME

    while (range >= 0.1) {
        Double_t zMax = z_0 + range;
        Double_t zMin = z_0 - range;
        Double_t zStep = (zMax - zMin) / nPlanes;

        Double_t minDist = 1000.0;
        Double_t minZ = -1000.0;

        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            Double_t z = zMax - iPlane * zStep;
            BmnStatus stat1 = fKalman->TGeoTrackPropagate(par1, z, fPDG1, nullptr, nullptr, kTRUE);
            BmnStatus stat2 = fKalman->TGeoTrackPropagate(par2, z, fPDG2, nullptr, nullptr, kTRUE);
            BmnStatus stat3 = fKalman->TGeoTrackPropagate(par3, z, fPDG3, nullptr, nullptr, kTRUE);

            if (stat1 == kBMNERROR || stat2 == kBMNERROR || stat3 == kBMNERROR)
                return DBL_MAX;

            Double_t d12 = Sqrt(Sq(par1->GetX() - par2->GetX()) + Sq(par1->GetY() - par2->GetY()));
            Double_t d13 = Sqrt(Sq(par1->GetX() - par3->GetX()) + Sq(par1->GetY() - par3->GetY()));
            Double_t d23 = Sqrt(Sq(par2->GetX() - par3->GetX()) + Sq(par2->GetY() - par3->GetY()));
            Double_t d123 = Sqrt(Sq(d23) + Sq(d12) + Sq(d13));

            if (d123 < minDist) {
                minDist = d123;
                minZ = z;
            }
        }

        z_0 = minZ;
        range *= 0.5;
    }
    return z_0;
}

Double_t BmnH3LTripleFinder::GetdEdx(CbmStsTrack* tr)
{
    set<Double_t> signalsUpp;
    // set<Double_t> signalsLow;
    for (Int_t hitIdx = 0; hitIdx < tr->GetNStsHits(); ++hitIdx) {
        CbmStsHit* hit = (CbmStsHit*)fStsHits->At(tr->GetStsHitIndex(hitIdx));

        if (hit->GetSystemId() != kGEM)
            continue;

        StripCluster* upp = (StripCluster*)fUpperClusters->At(hit->fDigiB % 1000000);
        // StripCluster low = lowClusters[hit.fDigiF % 1000000];

        signalsUpp.insert(upp->TotalSignal);
        // signalsLow.insert(low.TotalSignal);
    }

    Int_t nGemHits = signalsUpp.size();
    if (nGemHits < 3)
        return 0;

    Int_t shift = (nGemHits == 3)   ? 2
                  : (nGemHits == 4) ? 2
                  : (nGemHits == 5) ? 3
                  : (nGemHits == 6) ? 4
                  : (nGemHits == 7) ? 4
                                    : 0;

    auto itUpp = signalsUpp.begin();
    // auto itLow = signalsLow.begin();

    // Double_t totSigLow = 0.0;
    Double_t totSigUpp = 0.0;
    for (Int_t i = 0; i < nGemHits - shift; ++i) {
        totSigUpp += (*itUpp);
        // totSigLow += (*itLow);
        itUpp++;
        // itLow++;
    }

    // totSigLow /= (nGemHits - shift);
    totSigUpp /= (nGemHits - shift);
    return totSigUpp;
}

void BmnH3LTripleFinder::GetNHits(CbmStsTrack* tr, Int_t& nGEM, Int_t& nFSD)
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

ClassImp(BmnH3LTripleFinder);
