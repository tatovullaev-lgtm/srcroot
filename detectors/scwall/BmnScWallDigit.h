/** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 10.01.2024
 **
 ** Class for simulation data at digi level
 **/

#ifndef BMNSCWALLGIGIT_H
#define BMNSCWALLGIGIT_H

#include "BmnAbstractDigit.h"          // for Abstract digit
#include "BmnScWallAddress.h"          // for BmnScWallAddress

class BmnScWallDigit : public BmnAbstractDigit {
public:

  BmnScWallDigit() : BmnAbstractDigit() {}
  BmnScWallDigit(uint32_t address, double time, double signal = 0) 
    : BmnAbstractDigit(address, time, signal) {}

  ~BmnScWallDigit() {};

  // Getters
  uint32_t GetSystemId() const { return BmnScWallAddress::GetSystemId(fAddress); }
  uint32_t GetCellId() const { return BmnScWallAddress::GetCellId(fAddress); }

  // Member Functions
  virtual const char* GetClassName() { return "BmnScWallDigit"; }
  virtual void Print(const Option_t* opt = "") { printf("%s: CellId %d Data %s \n", GetClassName(), GetCellId(), GetStringData().Data()); }

  ClassDef(BmnScWallDigit, 2);

};

#endif // BMNSCWALLGIGIT_H
