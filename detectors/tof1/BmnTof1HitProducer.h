#ifndef BMNTOF1HITPRODUCER_H
#define BMNTOF1HITPRODUCER_H 1

#include "BmnTOF1Detector.h"
#include "BmnTof1GeoUtils.h"
#include "BmnTof1HitProducerIdeal.h"
#include "BmnTrigDigit.h"
#include "BmnTrigWaveDigit.h"

#include <TEfficiency.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TRandom2.h>

class BmnTof1HitProducer : public BmnTof1HitProducerIdeal
{
    Int_t fVerbose;          // reimplement fVerbose variable from FairTask
    Double_t fTimeSigma;     // Uncertainties of time, gaus sigma [ns],  default: 100 ps
    Double_t fErrX, fErrY;   // Uncertainties of coordinates, gaus sigma [cm], dX= 10./sqrt(12.) mm, default: dY= 5 mm.
    TF1* fEffFunc;
    TRandom2* pRandom;             //!
    BmnTof1GeoUtils* pGeoUtils;    //!
    BmnTOF1Detector** pDetector;   //!
    Int_t fNDetectors;
    DetectorId fDetectorId;

    Int_t fPeriod, fRun;

    BmnEventHeader* fEventHeader;   //!

    Bool_t HitExist(const Double_t& val, const Bool_t& isWide_strip);
    Bool_t DoubleHitExist(const Double_t& val, const Bool_t& isWide_strip, const DetectorId& flag);
    Bool_t TripleHitExist(const Double_t& val, const Bool_t& isWide_strip, const DetectorId& flag);

    TString fDetName, fPointsName, fDigitsName, fHitsName;
    TString NameFileLRcorrection, NameFileSlewingCorrection, NameFileTimeShiftCorrection;
    Bool_t FlagFileLRcorrection, FlagFileSlewingCorrection, FlagFileTimeShiftCorrection;

    BmnTrigDigit* FingT0Digit();
    Bool_t IsFile(TString NameFile);
    Bool_t SetCorrFiles();
    Bool_t OutOfRange(Int_t iPlane);

    InitStatus LoadDetectorConfiguration();

  public:
    BmnTof1HitProducer(const char* name,
                       DetectorId detId,
                       Bool_t useMCdata = true,
                       Int_t verbose = 1,
                       Bool_t DoTest = false);
    virtual ~BmnTof1HitProducer();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    virtual void OnlineWrite(const std::unique_ptr<TTree>& resultTree);

    void SetTimeResolution(Double_t sigma) { fTimeSigma = sigma; };
    void SetAlongStripResolution(Double_t Xerr) { fErrX = Xerr; };
    void SetSeed(UInt_t seed = 0);

    TString GetParameters()
    {
        char s[32];
        TString buf = "\n Run parameters: fTimeSigma=";
        sprintf(s, "%.5g", fTimeSigma);
        buf += (s);
        buf += " ns, fErrX=";
        sprintf(s, "%.4g", fErrX);
        buf += (s);
        buf += " cm, fErrY=";
        sprintf(s, "%.4g", fErrY);
        buf += (s);
        buf += " cm, fDoTest=";
        buf += fDoTest;
        buf += ", fDoMergeHits=";
        buf += fDoMergeHits;
        return buf;
    }

    void SetPeriodRun(Int_t p, Int_t r)
    {
        fPeriod = p;
        fRun = r;
    }

    ClassDef(BmnTof1HitProducer, 3);
};

#endif
