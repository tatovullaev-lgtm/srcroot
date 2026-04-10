
#include <TNamed.h>
#include <TVector3.h>
#include <vector>

using namespace std;

#ifndef BMNPARTICLETRIPLE_H
#define BMNPARTICLETRIPLE_H 1

class BmnParticleTriple : public TNamed
{
  public:
    /** Default constructor **/
    BmnParticleTriple();

    /** Destructor **/
    virtual ~BmnParticleTriple();

    void SetV0X(Double_t val) { fV0X = val; }

    void SetV0Y(Double_t val) { fV0Y = val; }

    void SetV0Z(Double_t val) { fV0Z = val; }

    void SetDCA0(Double_t val) { fDCA0 = val; }

    void SetDCA1(Double_t val) { fDCA1 = val; }

    void SetDCA2(Double_t val) { fDCA2 = val; }

    void SetDCA3(Double_t val) { fDCA3 = val; }

    void SetDCA123(Double_t val) { fDCA123 = val; }

    void SetdDCA1(Double_t val) { fdDCA1 = val; }

    void SetdDCA2(Double_t val) { fdDCA2 = val; }

    void SetdDCA3(Double_t val) { fdDCA3 = val; }

    void SetdDCA123(Double_t val) { fdDCA123 = val; }

    void SetPath(Double_t val) { fPath = val; }

    void SetInvMass(Double_t val) { fInvMass = val; }

    void SetAngleDecayProducts(Double_t val) { fAngleDecayProducts = val; }

    void SetMomTriple(Double_t val1, Double_t val2, Double_t val3)
    {
        fMomPart1 = val1;
        fMomPart2 = val2;
        fMomPart3 = val3;
    }

    void SetEtaTriple(Double_t val1, Double_t val2, Double_t val3)
    {
        fEtaPart1 = val1;
        fEtaPart2 = val2;
        fEtaPart3 = val3;
    }

    void SetTxTriple(Double_t val1, Double_t val2, Double_t val3)
    {
        fTxPart1 = val1;
        fTxPart2 = val2;
        fTxPart3 = val3;
    }

    void SetTyTriple(Double_t val1, Double_t val2, Double_t val3)
    {
        fTyPart1 = val1;
        fTyPart2 = val2;
        fTyPart3 = val3;
    }

    void SetChi2Triple(Double_t val1, Double_t val2, Double_t val3)
    {
        fChi2Part1 = val1;
        fChi2Part2 = val2;
        fChi2Part3 = val3;
    }

    void SetNDFTriple(Int_t val1, Int_t val2, Int_t val3)
    {
        fNDFPart1 = val1;
        fNDFPart2 = val2;
        fNDFPart3 = val3;
    }

    void SetBeta400Triple(Double_t val1, Double_t val2, Double_t val3)
    {
        fBeta400Part1 = val1;
        fBeta400Part2 = val2;
        fBeta400Part3 = val3;
    }

    void SetBeta700Triple(Double_t val1, Double_t val2, Double_t val3)
    {
        fBeta700Part1 = val1;
        fBeta700Part2 = val2;
        fBeta700Part3 = val3;
    }

    void SetMass400Triple(Double_t val1, Double_t val2, Double_t val3)
    {
        fMass400Part1 = val1;
        fMass400Part2 = val2;
        fMass400Part3 = val3;
    }

    void SetMass700Triple(Double_t val1, Double_t val2, Double_t val3)
    {
        fMass700Part1 = val1;
        fMass700Part2 = val2;
        fMass700Part3 = val3;
    }

    void SetMCTrackIdPart1(Int_t id);
    void SetMCTrackIdPart2(Int_t id);
    void SetMCTrackIdPart3(Int_t id);
    void SetRecoTrackIdPart1(Int_t id);   // actually it is GEM track Id
    void SetRecoTrackIdPart2(Int_t id);   // actually it is GEM track Id
    void SetRecoTrackIdPart3(Int_t id);

    void SetMCMomPart1(Double_t px, Double_t py, Double_t pz);
    void SetMCMomPart2(Double_t px, Double_t py, Double_t pz);
    void SetMCMomPart3(Double_t px, Double_t py, Double_t pz);

    void SetNHitsTriple(vector<Int_t> part1, vector<Int_t> part2, vector<Int_t> part3)
    {
        fNHitsSilPart1 = part1[0];   // SILICON
        fNHitsGemPart1 = part1[1];   // GEM

        fNHitsSilPart2 = part2[0];   // SILICON
        fNHitsGemPart2 = part2[1];   // GEM

        fNHitsSilPart3 = part3[0];   // SILICON
        fNHitsGemPart3 = part3[1];   // GEM
    }

    void SetNHitsTriple(Int_t nHitsPart1, Int_t nHitsPart2, Int_t nHitsPart3)
    {
        fNHitsGemPart1 = nHitsPart1;
        // fNHitsSilPart1 = 0;

        fNHitsGemPart2 = nHitsPart2;
        // fNHitsSilPart2 = 0;

        fNHitsGemPart3 = nHitsPart3;
    }

    Int_t GetNHitsPart1(TString det = "")
    {
        if (det.IsNull())
            return fNHitsSilPart1 + fNHitsGemPart1;   // nHits per glob. track

        else if (det.Contains("GEM"))
            return fNHitsGemPart1;

        else if (det.Contains("SILICON"))
            return fNHitsSilPart1;

        else
            return 0;
    }

    Int_t GetNHitsPart2(TString det = "")
    {
        if (det.IsNull())
            return fNHitsSilPart2 + fNHitsGemPart2;   // nHits per glob. track

        else if (det.Contains("GEM"))
            return fNHitsGemPart2;

        else if (det.Contains("SILICON"))
            return fNHitsSilPart2;

        else
            return 0;
    }

    Int_t GetNHitsPart3(TString det = "")
    {
        if (det.IsNull())
            return fNHitsSilPart3 + fNHitsGemPart3;   // nHits per glob. track

        else if (det.Contains("GEM"))
            return fNHitsGemPart3;

        else if (det.Contains("SILICON"))
            return fNHitsSilPart3;

        else
            return 0;
    }

    Double_t GetInvMass() { return fInvMass; }

    Double_t GetAngleDecayProducts() { return fAngleDecayProducts; }

    Double_t GetDCA123() { return fDCA123; }

    Double_t GetdDCA123() { return fdDCA123; }

    Double_t GetPath() { return fPath; }

    Double_t GetDCA1() { return fDCA1; }

    Double_t GetDCA2() { return fDCA2; }

    Double_t GetDCA3() { return fDCA3; }

    Double_t GetdDCA1() { return fdDCA1; }

    Double_t GetdDCA2() { return fdDCA2; }

    Double_t GetdDCA3() { return fdDCA3; }

    Double_t GetDCA0() { return fDCA0; }

    Double_t GetMomPart1() { return fMomPart1; }

    Double_t GetMomPart2() { return fMomPart2; }

    Double_t GetMomPart3() { return fMomPart3; }

    Double_t GetEtaPart1() { return fEtaPart1; }

    Double_t GetEtaPart2() { return fEtaPart2; }

    Double_t GetEtaPart3() { return fEtaPart3; }

    Double_t GetV0X() { return fV0X; }

    Double_t GetV0Y() { return fV0Y; }

    Double_t GetV0Z() { return fV0Z; }

    Double_t GetTxPart1() { return fTxPart1; }

    Double_t GetTxPart2() { return fTxPart2; }

    Double_t GetTxPart3() { return fTxPart3; }

    Double_t GetTyPart1() { return fTyPart1; }

    Double_t GetTyPart2() { return fTyPart2; }

    Double_t GetTyPart3() { return fTyPart3; }

    Double_t GetChi2Part1() { return fChi2Part1; }

    Double_t GetChi2Part2() { return fChi2Part2; }

    Double_t GetChi2Part3() { return fChi2Part3; }

    Int_t GetNDFPart1() { return fNDFPart1; }

    Int_t GetNDFPart2() { return fNDFPart2; }

    Int_t GetNDFPart3() { return fNDFPart3; }

    void SetAlpha1(Double_t val) { fAlpha1 = val; }

    void SetAlpha2(Double_t val) { fAlpha2 = val; }

    void SetPtPodol(Double_t val) { fPtPodol = val; }

    Double_t GetAlpha1() { return fAlpha1; }

    Double_t GetAlpha2() { return fAlpha2; }

    Double_t GetPtPodol() { return fPtPodol; }

    Double_t GetBeta400Part1() { return fBeta400Part1; }

    Double_t GetBeta400Part2() { return fBeta400Part2; }

    Double_t GetBeta400Part3() { return fBeta400Part3; }

    Double_t GetBeta700Part1() { return fBeta700Part1; }

    Double_t GetBeta700Part2() { return fBeta700Part2; }

    Double_t GetBeta700Part3() { return fBeta700Part3; }

    Double_t GetMass400Part1() { return fMass400Part1; }

    Double_t GetMass400Part2() { return fMass400Part2; }

    Double_t GetMass400Part3() { return fMass400Part3; }

    Double_t GetMass700Part1() { return fMass700Part1; }

    Double_t GetMass700Part2() { return fMass700Part2; }

    Double_t GetMass700Part3() { return fMass700Part3; }

    TVector3 GetMCMomPart1();
    TVector3 GetMCMomPart2();
    TVector3 GetMCMomPart3();

    Int_t GetMCTrackIdPart1();
    Int_t GetMCTrackIdPart2();
    Int_t GetMCTrackIdPart3();

    Int_t GetRecoTrackIdPart1();   // actually it is GEM track Id
    Int_t GetRecoTrackIdPart2();   // actually it is GEM track Id
    Int_t GetRecoTrackIdPart3();   // actually it is GEM track Id

    // Omega cut from M. Zavertyaev

    Double_t Omega() { return (fDCA123 * fDCA123) / (fDCA0 * fDCA0 + 4 * fDCA1 * fDCA2 * fDCA3); }

  private:
    Double_t fInvMass;   // Invariant mass of a considering pair

    Double_t fAngleDecayProducts;   // Angle between the two products of decay

    Double_t fBeta400Part1;
    Double_t fBeta400Part2;
    Double_t fBeta400Part3;

    Double_t fBeta700Part1;
    Double_t fBeta700Part2;
    Double_t fBeta700Part3;

    Double_t fMass400Part1;
    Double_t fMass400Part2;
    Double_t fMass400Part3;

    Double_t fMass700Part1;
    Double_t fMass700Part2;
    Double_t fMass700Part3;

    Double_t fV0X;
    Double_t fV0Y;
    Double_t fV0Z;

    Double_t fDCA0;
    Double_t fDCA1;
    Double_t fDCA2;
    Double_t fDCA3;
    Double_t fdDCA1;
    Double_t fdDCA2;
    Double_t fdDCA3;
    Double_t fDCA123;
    Double_t fdDCA123;

    Double_t fPath;

    Double_t fMomPart1;
    Double_t fMomPart2;
    Double_t fMomPart3;

    Double_t fEtaPart1;
    Double_t fEtaPart2;
    Double_t fEtaPart3;

    Int_t fNHitsSilPart1;
    Int_t fNHitsGemPart1;

    Int_t fNHitsSilPart2;
    Int_t fNHitsGemPart2;

    Int_t fNHitsSilPart3;
    Int_t fNHitsGemPart3;

    Double_t fTxPart1;
    Double_t fTxPart2;
    Double_t fTxPart3;
    Double_t fTyPart1;
    Double_t fTyPart2;
    Double_t fTyPart3;

    Double_t fChi2Part1;
    Double_t fChi2Part2;
    Double_t fChi2Part3;
    Int_t fNDFPart1;
    Int_t fNDFPart2;
    Int_t fNDFPart3;

    // Armenteros-Podolyansky ...
    Double_t fAlpha1;
    Double_t fAlpha2;
    Double_t fPtPodol;

    TVector3 fMCMomPart1;
    TVector3 fMCMomPart2;
    TVector3 fMCMomPart3;

    TVector3 fRecoMomPart1;
    TVector3 fRecoMomPart2;
    TVector3 fRecoMomPar3;

    Int_t fMCTrackIdPart1;
    Int_t fMCTrackIdPart2;
    Int_t fMCTrackIdPart3;
    Int_t fRecoTrackIdPart1;
    Int_t fRecoTrackIdPart2;
    Int_t fRecoTrackIdPart3;

    ClassDef(BmnParticleTriple, 1);
};

#endif