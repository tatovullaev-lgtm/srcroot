#ifndef BMNNDETDIGIT_H
#define BMNNDETDIGIT_H

#include "BmnAbstractDigit.h"      // for Abstract digit
#include "BmnNdetAddress.h"        // for BmnNdetAddress
#include "BmnNdetAddressMixin.h"   // for BmnNdetAddressMixin

class BmnNdetDigit
    : public BmnAbstractDigit
    , public BmnNdetAddressMixin<BmnNdetDigit>
{
  public:
    BmnNdetDigit()
        : BmnAbstractDigit()
    {}
    BmnNdetDigit(uint32_t address, double time, double signal)
        : BmnAbstractDigit(address, time, signal)
    {}

    ~BmnNdetDigit() {}

    // Member Functions
    virtual const char* GetClassName() { return "BmnNdetDigit"; }
    virtual void Print(const Option_t* opt = "")
    {
        printf("%s: %s Data %s \n",
               GetClassName(),
               BmnNdetAddress::GetInfoString(fAddress).c_str(),
               GetStringData().Data());
    }

    ClassDef(BmnNdetDigit, 6);
};

#endif   // BMNNDETDIGIT_H
