#ifndef BMNVSPDIGIT_H
#define BMNVSPDIGIT_H

#include <BmnStripDigit.h>

using namespace std;

class BmnVSPDigit : public BmnStripDigit
{
  public:
    BmnVSPDigit();
    BmnVSPDigit(BmnVSPDigit* digit);
    BmnVSPDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal);
    BmnVSPDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal, Double_t iNoise);

    Double_t GetTimeSinceBegin_ns() const { return fTimeSinceBegin_ns; }

    void SetTTimeSinceBegin_ns(Double_t val) { fTimeSinceBegin_ns = val; }

    virtual ~BmnVSPDigit();

  private:
    Double_t fTimeSinceBegin_ns;
    ClassDef(BmnVSPDigit, 3);
};

#endif /* BMNVSPDIGIT_H */
