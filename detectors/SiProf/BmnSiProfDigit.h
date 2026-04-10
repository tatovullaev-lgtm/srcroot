#ifndef BMNSiProfDIGIT_H
#define BMNSiProfDIGIT_H

#include <BmnStripDigit.h>

using namespace std;

class BmnSiProfDigit : public BmnStripDigit
{
  public:
    BmnSiProfDigit();
    BmnSiProfDigit(BmnSiProfDigit* digit);
    BmnSiProfDigit(Int_t iStation,
                   Int_t iModule,
                   Int_t iStripLayer,
                   Int_t iStripNumber,
                   Double_t iStripSignal,
                   Double_t iStripNoise = -1);

    virtual ~BmnSiProfDigit();

  private:
    ClassDef(BmnSiProfDigit, 2);
};

#endif
