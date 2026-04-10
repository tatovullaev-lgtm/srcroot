/** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 10.01.2024
 **
 ** Class for simulation data at digi level
 **/

#ifndef BMNFHCALDIGIT_H
#define BMNFHCALDIGIT_H

#include "BmnAbstractDigit.h"          // for Abstract digit
#include "BmnFHCalAddress.h"           // for BmnFHCalAddress

class BmnFHCalDigit : public BmnAbstractDigit {
public:

  BmnFHCalDigit() : BmnAbstractDigit() {}
  BmnFHCalDigit(uint32_t address, double time, double signal = 0) 
    : BmnAbstractDigit(address, time, signal) {}

  ~BmnFHCalDigit() {};

  // Getters
  uint32_t GetSystemId() const { return BmnFHCalAddress::GetSystemId(fAddress); }
  uint32_t GetModuleType() const { return BmnFHCalAddress::GetModuleType(fAddress); };
  uint32_t GetModuleId() const { return BmnFHCalAddress::GetModuleId(fAddress); }
  uint32_t GetSectionId() const { return BmnFHCalAddress::GetSectionId(fAddress); }

  // Member Functions
  virtual const char* GetClassName() { return "BmnFHCalDigit"; }
  virtual void Print(const Option_t* opt = "") { printf("%s: ModuleId %d Section %d Data %s \n", GetClassName(), GetModuleId(), GetSectionId(), GetStringData().Data()); }

  ClassDef(BmnFHCalDigit, 2);

};

#endif // BMNFHCALDIGIT_H