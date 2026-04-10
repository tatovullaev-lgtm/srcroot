#ifndef BMNFUNCTIONANA_H
#define BMNFUNCTIONANA_H 1

#include "FairField.h"

#include "TString.h"

/** Class with static common functions to simplify experiment code **/
class BmnFunctionAna
{
 public:
    // get magnetic field from a specified DST file
    // fileName: DST file path [in]
    // fieldScale: magnetic field scale [out]; fieldType: magnetic field type [out]
    // returns magnetic field, nullptr - if some errors occured
    static FairField* GetDSTField(TString fileName, Double_t& fieldScale, Int_t& fieldType);

 private:
    BmnFunctionAna() {}
    ~BmnFunctionAna() {}

 ClassDef(BmnFunctionAna,1)
};

#endif
