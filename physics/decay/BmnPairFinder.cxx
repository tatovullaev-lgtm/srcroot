
#include <TCanvas.h>
#include "TFile.h"
#include <TGeoManager.h>
#include "BmnPairFinder.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmStsPoint.h"
#include "BmnVertex.h"
#include "CbmVertex.h"

const Float_t pathCutMin = 1.0;
const Float_t pathCutMax = 100.0;
const Float_t DCA12CutMin = 0.0;
const Float_t DCA12CutMax = 2.0;
const Float_t DCA1CutMin = 0.0;
const Float_t DCA1CutMax = 10.0;
const Float_t DCA2CutMin = 0.0;
const Float_t DCA2CutMax = 10.0;
const Float_t DCA0CutMin = 0.0;
const Float_t DCA0CutMax = 1.0;
const Float_t PVXCutMin = -1.0;
const Float_t PVXCutMax = 1.5;
const Float_t PVYCutMin = -1.0;
const Float_t PVYCutMax = 1.0;
const Float_t PVZCutMin = -0.4;
const Float_t PVZCutMax = 0.4;
const Int_t NTrCutMin = 0;
const Int_t NTrCutMax = 100;

BmnPairFinder::BmnPairFinder(TParticlePDG * particle)
{
    cout << "\nBmnPairFinder::Constructor()" << endl;
    fDstHeader = nullptr;
    fKalman = nullptr;
    fParticlePair = nullptr;
    fVertex = nullptr;
    evNumber = 0;
    fRunId = -10;
    fParticle = unique_ptr<TParticlePDG>(particle);
}

BmnPairFinder::~BmnPairFinder()
{
    cout << "\nBmnPairFinder::Destructor()" << endl;
}

InitStatus BmnPairFinder::Init()
{
    cout << "\nBmnPairFinder::Init()" << endl;
    switch (fParticle->PdgCode()){
        case 310:
            fDecayFirstParticle = unique_ptr<TParticlePDG>(TDatabasePDG::Instance()->GetParticle(211));
            fDecaySecondParticle = unique_ptr<TParticlePDG>(TDatabasePDG::Instance()->GetParticle(-211));
            lowV0Z = -10.0;
            break;
        case 3122:
            fDecayFirstParticle = unique_ptr<TParticlePDG>(TDatabasePDG::Instance()->GetParticle(2212));
            fDecaySecondParticle = unique_ptr<TParticlePDG>(TDatabasePDG::Instance()->GetParticle(-211));
            lowV0Z = 0.0;
            break;
        default:
            LOGF(error, "Unsupported particle %d %s !", fParticle->PdgCode(), fParticle->GetName());
            return kERROR;
            break;
    }
    if (fParticle)
        LOGF(info, "Search decays: %s -> %s   %s",
                fParticle->GetName(), fDecayFirstParticle->GetName(), fDecaySecondParticle->GetName());
    
    FairRootManager *ioman = FairRootManager::Instance();
    fDstHeader = (DstEventHeader *)ioman->GetObject("DstEventHeader.");
    fStsTracks = (TClonesArray *)ioman->GetObject("StsVector");
    fVertex = (CbmVertex *)ioman->GetObject("PrimaryVertex.");

    fParticlePair = new TClonesArray("BmnParticlePair");
    ioman->Register("ParticlePair", "Lambda", fParticlePair, kTRUE);

    fKalman = new BmnKalmanFilter();

    return kSUCCESS;
}

// ------------------------------------------------------------------

void BmnPairFinder::Exec(Option_t *option)
{
    // cout << "\nBmnPairFinder::Exec()" << endl;
    // cout << "Event = " << evNumber++ << endl;

    evNumber++;
	if (evNumber % 1000 == 0) printf("Event number: %d\n", evNumber);

    fParticlePair->Delete();

    if (fVertex->GetNTracks() < 2)
        return;

    
    if (fVertex->GetZ() > PVZCutMax || fVertex->GetZ() < PVZCutMin) return;
    if (fVertex->GetX() > PVXCutMax || fVertex->GetX() < PVXCutMin) return;
    if (fVertex->GetY() > PVYCutMax || fVertex->GetY() < PVYCutMin) return;

    if (fStsTracks->GetEntriesFast() > NTrCutMax || fStsTracks->GetEntriesFast() < NTrCutMin) return;

    for (Int_t iTrack = 0; iTrack < fStsTracks->GetEntriesFast(); iTrack++)
    {
        CbmStsTrack *cbmststrack1 = (CbmStsTrack *)fStsTracks->At(iTrack);

        BmnGlobalTrack globTr1;

            //globTr1.SetGemTrackIndex(i);
        globTr1.SetParamFirst(*(cbmststrack1->GetParamFirst()));
        globTr1.SetParamLast(*(cbmststrack1->GetParamLast()));
	Int_t nHitsPos = cbmststrack1->GetNStsHits();
	    //if (nHitsPos < 5) continue;
        globTr1.SetNHits(nHitsPos);
        globTr1.SetNDF(cbmststrack1->GetNDF());
        globTr1.SetChi2(cbmststrack1->GetChi2());

	    FairTrackParam par_prot_Vp = *(globTr1.GetParamFirst());	       

        if (par_prot_Vp.GetQp() < 0) continue;
        if (fKalman->TGeoTrackPropagate(&par_prot_Vp, fVertex->GetZ(), fDecayFirstParticle->PdgCode(), nullptr, nullptr, kTRUE) == kBMNERROR) continue;

        for (Int_t jTrack = 0; jTrack < fStsTracks->GetEntriesFast(); jTrack++)
        {
            CbmStsTrack *cbmststrack2 = (CbmStsTrack *)fStsTracks->At(jTrack);

            BmnGlobalTrack globTr2;

            globTr2.SetParamFirst(*(cbmststrack2->GetParamFirst()));
            globTr2.SetParamLast(*(cbmststrack2->GetParamLast()));
			Int_t nHitsNeg = cbmststrack2->GetNStsHits();
			//if (nHitsNeg < 4) continue;
			globTr2.SetNHits(cbmststrack2->GetNStsHits());
            globTr2.SetNDF(cbmststrack2->GetNDF());
            globTr2.SetChi2(cbmststrack2->GetChi2());
                        
            FairTrackParam par_pion_Vp = *(globTr2.GetParamFirst());
            if (par_pion_Vp.GetQp() > 0) continue; //suppose negative particle is pi-
            if (fKalman->TGeoTrackPropagate(&par_pion_Vp, fVertex->GetZ(), fDecaySecondParticle->PdgCode(), nullptr, nullptr, kTRUE) == kBMNERROR) continue;
            //Float_t vDist = TMath::Sqrt(Sq(par_pion_Vp.GetX() - fVertex->GetX()) + Sq(par_pion_Vp.GetY() - fVertex->GetY()));
            //if (vDist > 2.0) continue;

            BmnParticlePair pair = BmnParticlePair();

            Double_t V0Z = FindV0ByVirtualPlanes(&globTr1, &globTr2, 20, 50);
	                if (V0Z < lowV0Z || V0Z > 50) continue;

            FairTrackParam par_prot_V0 = *(globTr1.GetParamFirst());
            FairTrackParam par_pion_V0 = *(globTr2.GetParamFirst());
			if (fKalman->TGeoTrackPropagate(&par_prot_V0, V0Z, fDecayFirstParticle->PdgCode(), nullptr, nullptr, kTRUE) == kBMNERROR) continue;
			if (fKalman->TGeoTrackPropagate(&par_pion_V0, V0Z, fDecaySecondParticle->PdgCode(), nullptr, nullptr, kTRUE) == kBMNERROR) continue;

			if (fKalman->TGeoTrackPropagate(&par_prot_Vp, fVertex->GetZ(), fDecayFirstParticle->PdgCode(), nullptr, nullptr, kTRUE) == kBMNERROR) continue;
			if (fKalman->TGeoTrackPropagate(&par_pion_Vp, fVertex->GetZ(), fDecaySecondParticle->PdgCode(), nullptr, nullptr, kTRUE) == kBMNERROR) continue;

            Double_t V0X = .5 * (par_prot_V0.GetX() + par_pion_V0.GetX());
			if (V0X < -2.0 || V0X > 2.0) continue;
            Double_t V0Y = .5 * (par_prot_V0.GetY() + par_pion_V0.GetY());
			if (V0Y < -2.0 || V0Y > 2.0) continue;

			Float_t Path = GetPath(fVertex->GetX(), fVertex->GetY(), fVertex->GetZ(), V0X, V0Y, V0Z);
			if (Path < pathCutMin || Path > pathCutMax) continue;

			Float_t DCA0 = FindDCA0(par_prot_V0, par_pion_V0, fVertex->GetX(), fVertex->GetY(), fVertex->GetZ(), V0X, V0Y, V0Z);
			if (DCA0 < DCA0CutMin || DCA0 > DCA0CutMax) continue;

			Float_t DCA1 = TMath::Sqrt(Sq(par_prot_Vp.GetX() - fVertex->GetX()) + Sq(par_prot_Vp.GetY() - fVertex->GetY()));
			if (DCA1 < DCA1CutMin || DCA1 > DCA1CutMax) continue;

			Float_t DCA2 = TMath::Sqrt(Sq(par_pion_Vp.GetX() - fVertex->GetX()) + Sq(par_pion_Vp.GetY() - fVertex->GetY()));
			if (DCA2 < DCA2CutMin || DCA2 > DCA2CutMax) continue;

			Float_t DCA12 = TMath::Sqrt(Sq(par_prot_V0.GetX() - par_pion_V0.GetX()) + Sq(par_prot_V0.GetY() - par_pion_V0.GetY()));
			if (DCA12 < DCA12CutMin || DCA12 > DCA12CutMax) continue;

            pair.SetPath(Path);
            pair.SetDCA0(DCA0);
            pair.SetDCA12(DCA12);
	    pair.SetDCA1(DCA1);
            pair.SetDCA2(DCA2);

            pair.SetV0X(V0X);
            pair.SetV0Y(V0Y);
            pair.SetV0Z(V0Z);

            pair.SetTxPair(par_prot_V0.GetTx(), par_pion_V0.GetTx());
            pair.SetTyPair(par_prot_V0.GetTy(), par_pion_V0.GetTy());


	    pair.SetMomPair(globTr1.GetP(), globTr2.GetP());
            pair.SetNHitsPair(nHitsPos, nHitsNeg);
	    pair.SetChi2Pair(globTr1.GetChi2(), globTr2.GetChi2());
	    pair.SetNDFPair(globTr1.GetNDF(), globTr2.GetNDF());
            pair.SetInvMass(FindInvMass(par_prot_V0, par_pion_V0));
	    pair.SetAngleDecayProducts(FindAngleDecayProducts(par_prot_V0, par_pion_V0));
            pair.SetEtaPair(FindEta(par_prot_V0), FindEta(par_pion_V0));
            new ((*fParticlePair)[fParticlePair->GetEntriesFast()]) BmnParticlePair(pair);		
        }
    }
}
// -------------------------------------------------------------------

Double_t BmnPairFinder::FindV0ByVirtualPlanes(BmnGlobalTrack *track1, BmnGlobalTrack *track2, Double_t z_0, Double_t range)

{

    const Int_t nPlanes = 5; // FIXME

    // if (V0Z < Vpz - 10. || V0Z > fDetector->GetGemStation(5)->GetZPosition()) // FIXME!
    //            continue;

    while (range >= 0.1)
    {
        Double_t zMax = z_0 + range;
        Double_t zMin = z_0 - range;
        Double_t zStep = (zMax - zMin) / nPlanes;

        Double_t zPlane[nPlanes];
        Double_t Dist[nPlanes];

        FairTrackParam par1 = *(track1->GetParamFirst());
        FairTrackParam par2 = *(track2->GetParamFirst());

        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane)
        {
            zPlane[iPlane] = zMax - iPlane * zStep;
            BmnStatus stat1 = fKalman->TGeoTrackPropagate(&par1, zPlane[iPlane], fDecayFirstParticle->PdgCode(), nullptr, nullptr, kTRUE);
            BmnStatus stat2 = fKalman->TGeoTrackPropagate(&par2, zPlane[iPlane], fDecaySecondParticle->PdgCode(), nullptr, nullptr, kTRUE);

            if (stat1 == kBMNERROR || stat2 == kBMNERROR)
            {
                return DBL_MAX;
            }

            Dist[iPlane] = TMath::Sqrt(Sq(par1.GetX() - par2.GetX()) + Sq(par1.GetY() - par2.GetY()));
        }

        map<Double_t, Double_t> dist_zPlanes; // distance --> z-position

        for (Int_t iPlane = 0; iPlane < nPlanes; iPlane++)
            dist_zPlanes[Dist[iPlane]] = zPlane[iPlane];

        z_0 = dist_zPlanes.begin()->second;

        range /= 2;
    }

    return z_0;
}

Double_t BmnPairFinder::FindInvMass(FairTrackParam pi_plus_meson_V0, FairTrackParam pi_minus_meson_V0)
{

    Double_t Tx1, Ty1, Tx2, Ty2, p1, p2;
    Double_t A1, A2;
    TLorentzVector lPos, lNeg;

    Tx1 = pi_plus_meson_V0.GetTx();
    Ty1 = pi_plus_meson_V0.GetTy();
    Tx2 = pi_minus_meson_V0.GetTx();
    Ty2 = pi_minus_meson_V0.GetTy();
    p1 = 1. / pi_plus_meson_V0.GetQp();
    //cout<<"small_test_1:"<<""<<p1<<endl;
    p2 = 1. / pi_minus_meson_V0.GetQp();
    //cout<<"small_test_2:"<<""<<p2<<endl;
    // armenPodol = ArmenterosPodol(proton_V0, pion_V0);

    A1 = 1. / TMath::Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
    lPos.SetXYZM(Tx1 * A1 * p1, Ty1 * A1 * p1, p1 * A1, fDecayFirstParticle->Mass());

    p2 *= -1.; // Since in the calculations pos. mom. values should be used

    A2 = 1. / TMath::Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);
    lNeg.SetXYZM(Tx2 * A2 * p2, Ty2 * A2 * p2, p2 * A2, fDecaySecondParticle->Mass());

    // partPair.SetAlpha(armenPodol.X());
    // partPair.SetPtPodol(armenPodol.Y());
    return (TLorentzVector((lPos + lNeg)).Mag());
}

Double_t BmnPairFinder::GetPath(Double_t vPx, Double_t vPy, Double_t vPz, Double_t v0x, Double_t v0y, Double_t v0z)
{
    TVector3 V0(v0x, v0y, v0z);
    TVector3 Vp(vPx, vPy, vPz);
    return TVector3(V0 - Vp).Mag();
}

Double_t BmnPairFinder::FindDCA0(FairTrackParam pi_plus_meson_V0, FairTrackParam pi_minus_meson_V0, Double_t vPx, Double_t vPy, Double_t vPz, Double_t v0x, Double_t v0y, Double_t v0z)
{
    Double_t Tx1, Ty1, Tx2, Ty2, p1, p2;
    Double_t A1, A2;
    TVector3 V0(v0x, v0y, v0z);
    TVector3 Vp(vPx, vPy, vPz);

    Tx1 = pi_plus_meson_V0.GetTx();
    Ty1 = pi_plus_meson_V0.GetTy();
    Tx2 = pi_minus_meson_V0.GetTx();
    Ty2 = pi_minus_meson_V0.GetTy();
    p1 = 1. / pi_plus_meson_V0.GetQp();
    p2 = 1. / pi_minus_meson_V0.GetQp();

    A1 = 1. / TMath::Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
    p2 *= -1.;
    A2 = 1. / TMath::Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);

    Double_t PzPart1 = Abs(p1) * A1;
    Double_t PzPart2 = Abs(p2) * A2;

    Double_t PzPart0 = PzPart1 + PzPart2;
    Double_t PxPart0 = PzPart1 * Tx1 + PzPart2 * Tx2;
    Double_t PyPart0 = PzPart1 * Ty1 + PzPart2 * Ty2;

    Double_t TxPart0 = PxPart0 / PzPart0;
    Double_t TyPart0 = PyPart0 / PzPart0;

    Double_t xPart0 = TxPart0 * (Vp.Z() - V0.Z()) + V0.X();
    Double_t yPart0 = TyPart0 * (Vp.Z() - V0.Z()) + V0.Y();

    Double_t dca0 = TMath::Sqrt(Sq(xPart0 - Vp.X()) + Sq(yPart0 - Vp.Y()));

    return dca0;
}

Double_t BmnPairFinder::FindEta(FairTrackParam pi_plus_meson_V0)
{
    Double_t Tx = pi_plus_meson_V0.GetTx();
    Double_t Ty = pi_plus_meson_V0.GetTy();
    Double_t p = 1. / pi_plus_meson_V0.GetQp();

    Double_t Pz = Abs(p) / TMath::Sqrt(1 + Tx * Tx + Ty * Ty);

    Double_t eta = 0.5 * Log((Abs(p) + Pz) / (Abs(p) - Pz));

    return eta;
}

Double_t BmnPairFinder::FindAngleDecayProducts(FairTrackParam proton_V0, FairTrackParam pi_minus_meson_V0)
{
    Double_t Tx1, Ty1, Tx2, Ty2, p1, p2;
    Double_t A1, A2;
    TVector3 Pproton;
    TVector3 Pnegpimeson;

    Tx1 = proton_V0.GetTx();
    Ty1 = proton_V0.GetTy();
    Tx2 = pi_minus_meson_V0.GetTx();
    Ty2 = pi_minus_meson_V0.GetTy();

    p1 = 1. / proton_V0.GetQp();
    p2 = 1. / pi_minus_meson_V0.GetQp();

    A1 = 1. / TMath::Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
    p2 *= -1.;
    A2 = 1. / TMath::Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);

    Pproton.SetX(Tx1 * A1 * p1);
    Pproton.SetY(Ty1 * A1 * p1);
    Pproton.SetZ(p1 * A1);

    Pnegpimeson.SetX(Tx2 * A2 * p2);
    Pnegpimeson.SetY(Ty2 * A2 * p2);
    Pnegpimeson.SetZ(p2 * A2);

    Double_t dot_product = Pproton.Dot(Pnegpimeson);
    //cout<<"1:"<<dot_product<<endl;
    Double_t mag_Pproton = Pproton.Mag();
    //cout<<"2:"<<mag_Pproton<<endl;
    Double_t mag_Pnegpimeson = Pnegpimeson.Mag();
    //cout<<"3:"<<mag_Pnegpimeson<<endl;
    //cout<<"4:"<<ACos(dot_product/(mag_Pproton*mag_Pnegpimeson))<<endl;
    //cout<<"5:"<<ACos(dot_product/(mag_Pproton*mag_Pnegpimeson))*180/Pi()<<endl;
    return ACos(dot_product/(mag_Pproton*mag_Pnegpimeson))*180/Pi();
}

void BmnPairFinder::Finish()
{

    cout << "\n-I- [BmnPairFinder::Finish] " << endl;
}
