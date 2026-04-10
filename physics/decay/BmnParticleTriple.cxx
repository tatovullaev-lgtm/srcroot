
#include "BmnParticleTriple.h"

BmnParticleTriple::BmnParticleTriple()
    : fV0X(0.)
    , fV0Y(0.)
    , fV0Z(0.)
    , fBeta400Part1(0.)
    , fBeta400Part2(0.)
    , fBeta400Part3(0.)
    , fBeta700Part1(0.)
    , fBeta700Part2(0.)
    , fBeta700Part3(0.)
    , fDCA0(0.)
    , fDCA1(0.)
    , fDCA2(0.)
    , fDCA3(0.)
    , fDCA123(0.)
    , fPath(0.)
    , fInvMass(0.)
    , fAngleDecayProducts(0.)
    , fMomPart1(0.)
    , fMomPart2(0.)
    , fMomPart3(0.)
    , fEtaPart1(0.)
    , fEtaPart2(0.)
    , fEtaPart3(0.)
    , fTxPart1(-1.)
    , fTxPart2(-1.)
    , fTxPart3(-1.)
    , fTyPart1(-1.)
    , fTyPart2(-1.)
    , fTyPart3(-1.)
    , fChi2Part1(0.)
    , fChi2Part2(0.)
    , fChi2Part3(0.)
    , fNDFPart1(0.)
    , fNDFPart2(0.)
    , fNDFPart3(0.)
    , fNHitsSilPart1(0)
    , fNHitsSilPart2(0)
    , fNHitsSilPart3(0)
    , fNHitsGemPart1(0)
    , fNHitsGemPart2(0)
    , fNHitsGemPart3(0)
    , fAlpha1(0.)
    , fAlpha2(0.)
    , fPtPodol(0.)
    , fRecoTrackIdPart1(-1)
    , fRecoTrackIdPart2(-1)
    , fRecoTrackIdPart3(-1)
    , fMCTrackIdPart1(-1)
    , fMCTrackIdPart2(-1)
    , fMCTrackIdPart3(-1)
{
    fMCMomPart1.SetXYZ(0., 0., 0.);
    fMCMomPart2.SetXYZ(0., 0., 0.);
    fMCMomPart3.SetXYZ(0., 0., 0.);
    fRecoMomPart1.SetXYZ(0., 0., 0.);
    fRecoMomPart2.SetXYZ(0., 0., 0.);
    // fRecoMomPart3.SetXYZ(0., 0., 0.);
}

// Setters

void BmnParticleTriple::SetMCMomPart1(Double_t px, Double_t py, Double_t pz)
{
    fMCMomPart1.SetXYZ(px, py, pz);
}

void BmnParticleTriple::SetMCMomPart2(Double_t px, Double_t py, Double_t pz)
{
    fMCMomPart2.SetXYZ(px, py, pz);
}

void BmnParticleTriple::SetMCMomPart3(Double_t px, Double_t py, Double_t pz)
{
    fMCMomPart3.SetXYZ(px, py, pz);
}

// Getters

TVector3 BmnParticleTriple::GetMCMomPart1()
{
    return fMCMomPart1;
}

TVector3 BmnParticleTriple::GetMCMomPart2()
{
    return fMCMomPart2;
}

TVector3 BmnParticleTriple::GetMCMomPart3()
{
    return fMCMomPart3;
}

Int_t BmnParticleTriple::GetMCTrackIdPart1()
{
    return fMCTrackIdPart1;
}

Int_t BmnParticleTriple::GetMCTrackIdPart2()
{
    return fMCTrackIdPart2;
}

Int_t BmnParticleTriple::GetMCTrackIdPart3()
{
    return fMCTrackIdPart3;
}

Int_t BmnParticleTriple::GetRecoTrackIdPart1()
{
    return fRecoTrackIdPart1;
}

Int_t BmnParticleTriple::GetRecoTrackIdPart2()
{
    return fRecoTrackIdPart2;
}

Int_t BmnParticleTriple::GetRecoTrackIdPart3()
{
    return fRecoTrackIdPart3;
}

void BmnParticleTriple::SetMCTrackIdPart1(Int_t id)
{
    fMCTrackIdPart1 = id;
}

void BmnParticleTriple::SetMCTrackIdPart2(Int_t id)
{
    fMCTrackIdPart2 = id;
}

void BmnParticleTriple::SetMCTrackIdPart3(Int_t id)
{
    fMCTrackIdPart3 = id;
}

void BmnParticleTriple::SetRecoTrackIdPart1(Int_t id)
{
    fRecoTrackIdPart1 = id;
}

void BmnParticleTriple::SetRecoTrackIdPart2(Int_t id)
{
    fRecoTrackIdPart2 = id;
}

void BmnParticleTriple::SetRecoTrackIdPart3(Int_t id)
{
    fRecoTrackIdPart3 = id;
}

BmnParticleTriple::~BmnParticleTriple() {}