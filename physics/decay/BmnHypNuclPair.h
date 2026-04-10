#include <TMath.h>
#include <TNamed.h>
#include <vector>

using namespace std;
using namespace TMath;

#ifndef BMNHYPNUCLPAIR_H
#define BMNHYPNUCLPAIR_H 1

class BmnHypNuclPair : public TNamed
{
  public:
    /** Default constructor **/
    BmnHypNuclPair();

    /** Destructor **/
    virtual ~BmnHypNuclPair();

    void SetDCA0(Float_t val) { fDCA0 = val; }

    void SetDCA1(Float_t val) { fDCA1 = val; }

    void SetDCA2(Float_t val) { fDCA2 = val; }

    void SetDCA12(Float_t val) { fDCA12 = val; }

    void SetdDCA1(Float_t val) { fdDCA1 = val; }

    void SetdDCA2(Float_t val) { fdDCA2 = val; }

    void SetdDCA12(Float_t val) { fdDCA12 = val; }

    void SetPath(Float_t val) { fPath = val; }

    void SetInvMass(Float_t val) { fInvMass = val; }

    void SetMomXYZ1(Float_t px, Float_t py, Float_t pz)
    {
        fPx1 = px;
        fPy1 = py;
        fPz1 = pz;
    }

    void SetMomXYZ2(Float_t px, Float_t py, Float_t pz)
    {
        fPx2 = px;
        fPy2 = py;
        fPz2 = pz;
    }

    void SetChi2NDF(Float_t val1, Float_t val2)
    {
        fChi2Ndf1 = val1;
        fChi2Ndf2 = val2;
    }

    void SetM2(Float_t val) { fM2 = val; }

    void SetNHitsPair(vector<Int_t> part1, vector<Int_t> part2)
    {
        fNHitsSil1 = part1[0];   // SILICON
        fNHitsGem1 = part1[1];   // GEM

        fNHitsSil2 = part2[0];   // SILICON
        fNHitsGem2 = part2[1];   // GEM
    }

    void SetNHitsPair(Int_t nHitsPart1, Int_t nHitsPart2)
    {
        fNHitsGem1 = nHitsPart1;
        // fNHitsSilPart1 = 0;

        fNHitsGem2 = nHitsPart2;
        // fNHitsSilPart2 = 0;
    }

    void SetNTrInEvent(Int_t n) { fNTrInEvent = n; }

    Int_t GetNHitsPart1(TString det = "")
    {
        if (det.IsNull())
            return fNHitsSil1 + fNHitsGem1;   // nHits per glob. track

        else if (det.Contains("GEM"))
            return fNHitsGem1;

        else if (det.Contains("SILICON"))
            return fNHitsSil1;

        else
            return 0;
    }

    Int_t GetNHitsPart2(TString det = "")
    {
        if (det.IsNull())
            return fNHitsSil2 + fNHitsGem2;   // nHits per glob. track

        else if (det.Contains("GEM"))
            return fNHitsGem2;

        else if (det.Contains("SILICON"))
            return fNHitsSil2;

        else
            return 0;
    }

    Float_t GetInvMass() { return fInvMass; }

    Float_t GetDCA12() { return fDCA12; }

    Float_t GetdDCA12() { return fdDCA12; }

    Float_t GetPath() { return fPath; }

    Float_t GetDCA1() { return fDCA1; }

    Float_t GetDCA2() { return fDCA2; }

    Float_t GetdDCA1() { return fdDCA1; }

    Float_t GetdDCA2() { return fdDCA2; }

    Float_t GetDCA0() { return fDCA0; }

    Float_t GetMom1() { return Sqrt(fPx1 * fPx1 + fPy1 * fPy1 + fPz1 * fPz1); }

    Float_t GetPx1() { return fPx1; }

    Float_t GetPy1() { return fPy1; }

    Float_t GetPz1() { return fPz1; }

    Float_t GetPt1() { return Sqrt(fPx1 * fPx1 + fPy1 * fPy1); }

    Float_t GetMom2() { return Sqrt(fPx2 * fPx2 + fPy2 * fPy2 + fPz2 * fPz2); }

    Float_t GetPx2() { return fPx2; }

    Float_t GetPy2() { return fPy2; }

    Float_t GetPz2() { return fPz2; }

    Float_t GetPt2() { return Sqrt(fPx2 * fPx2 + fPy2 * fPy2); }

    Float_t GetTxPart1() { return fPx1 / fPz1; }

    Float_t GetTxPart2() { return fPx2 / fPz2; }

    Float_t GetTyPart1() { return fPy1 / fPz1; }

    Float_t GetTyPart2() { return fPy2 / fPz2; }

    Float_t GetChi2Ndf1() { return fChi2Ndf1; }

    Float_t GetChi2Ndf2() { return fChi2Ndf2; }

    void SetDedx(Float_t val) { fdEdx = val; }

    Float_t GetDedx() { return fdEdx; }

    Float_t GetM2() { return fM2; }

    Int_t GetNTrInEvent() { return fNTrInEvent; }

  private:
    Float_t fInvMass;

    Float_t fDCA0;

    Float_t fDCA1;
    Float_t fdDCA1;
    Float_t fDCA2;
    Float_t fdDCA2;
    Float_t fDCA12;
    Float_t fdDCA12;

    Float_t fPath;

    Int_t fNHitsSil1;
    Int_t fNHitsGem1;
    Int_t fNHitsSil2;
    Int_t fNHitsGem2;

    Float_t fPx1;
    Float_t fPy1;
    Float_t fPz1;
    Float_t fPx2;
    Float_t fPy2;
    Float_t fPz2;

    Float_t fChi2Ndf1;
    Float_t fChi2Ndf2;

    Float_t fdEdx;
    Float_t fM2;

    Int_t fNTrInEvent;

    ClassDef(BmnHypNuclPair, 2);
};

#endif
