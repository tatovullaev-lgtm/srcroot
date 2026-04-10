#ifndef BMNBDDIGITIZER_H
#define BMNBDDIGITIZER_H 1

#include "FairTask.h"

#include <TStopwatch.h>
#include <map>

class TClonesArray;

class BmnBdDigitizer : public FairTask
{

  public:
    /** Default constructor **/
    BmnBdDigitizer();

    /** Destructor **/
    ~BmnBdDigitizer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    virtual void Finish();

  private:
    /** Input array of BmnBdPoints **/
    TClonesArray* fPointArray;

    /** Output array of BmnBdDigits **/
    TClonesArray* fDigitArray;

    BmnBdDigitizer(const BmnBdDigitizer&);
    BmnBdDigitizer& operator=(const BmnBdDigitizer&);

    Int_t fNMod;
    Double_t fNoiseSigma;
    Double_t fThreshold;
    Int_t fFiredChannels;

    Double_t ConvertAmplitudeToTime(Double_t amplitude);

    ClassDef(BmnBdDigitizer, 0);
};

#endif