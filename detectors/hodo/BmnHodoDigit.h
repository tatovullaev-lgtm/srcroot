/** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 10.01.2024
 **
 ** Class for simulation data at digi level
 **/

#ifndef BMNHODOGIGIT_H
#define BMNHODOGIGIT_H

#include "BmnAbstractDigit.h"   // for Abstract digit
#include "BmnHodoAddress.h"     // for BmnHodoAddress

class BmnHodoDigit : public BmnAbstractDigit
{
  public:
    BmnHodoDigit()
        : BmnAbstractDigit()
        , fIsSaturated(false)
    {}
    BmnHodoDigit(uint32_t address, double time, double signal = 0)
        : BmnAbstractDigit(address, time, signal)
        , fIsSaturated(false)
    {}

    ~BmnHodoDigit() {};

    // Getters
    uint32_t GetSystemId() const { return BmnHodoAddress::GetSystemId(fAddress); }
    uint32_t GetStripId() const { return BmnHodoAddress::GetStripId(fAddress); }
    uint32_t GetStripSide() const { return BmnHodoAddress::GetStripSide(fAddress); }
    uint32_t GetGain() const { return BmnHodoAddress::GetGain(fAddress); }
    bool GetIsSaturated() const { return fIsSaturated; }

    // Member Functions
    virtual const char* GetClassName() { return "BmnHodoDigit"; }
    virtual void Print(const Option_t* opt = "")
    {
        printf("%s: StripId %d Data %s \n", GetClassName(), GetStripId(), GetStringData().Data());
    }

    void SetIsSaturated(bool satur) { fIsSaturated = satur; }

  private:
    bool fIsSaturated;

    ClassDef(BmnHodoDigit, 3);
};

#endif   // BMNHODOGIGIT_H
