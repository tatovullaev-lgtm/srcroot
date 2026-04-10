#ifndef BMNCSCHITMAKER_H
#define BMNCSCHITMAKER_H 1

#include "BmnADCDigit.h"
#include "BmnEventHeader.h"
#include "BmnProfRaw2Digit.h"
#include "BmnProfRawTools.h"
#include "BmnSiProfDigit.h"
#include "FairTask.h"
#include "Rtypes.h"
#include "TClonesArray.h"
#include "TRegexp.h"
#include "TString.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

class BmnProfAsic2Raw : public FairTask
{
  public:
    BmnProfAsic2Raw(Int_t run_period = 9, Int_t run_number = 0, Int_t verb = 0);

    virtual ~BmnProfAsic2Raw();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

  private:
    void CopyDataToPedMap(TClonesArray* adcGem, UInt_t ev);
    UInt_t fPeriodId;
    UInt_t fRunId;
    UInt_t fPedEvCntr;
    UInt_t fNoiseEvCntr;
    Int_t fEvForPedestals;
    Int_t fEv;
    Bool_t fPedEnough;

    BmnProfRaw2Digit* fMapper;

    TString fInputEventHeaderName;
    TString fInputAdcBranchName;
    TString fWorkAdcBranchName;

    TString fOutputProfBranchName;

    BmnEventHeader* fEventHead;   //!
    /** Input array of ADC signals **/
    TClonesArray* fInAsicArray;
    /** Intermediate array of ADC signals **/
    TClonesArray* fWorkAdcArray;

    /** Output array of Profilometer digits **/
    TClonesArray* fBmnProfDigitArray;

    ClassDef(BmnProfAsic2Raw, 1);
};

#endif
