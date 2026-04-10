/** \class BmnAbstractDigit
 ** \brief Abstract base data class to store digital information 
 ** \version 1.0
 **/

#ifndef BMNABSTRACTDIGIT_H
#define BMNABSTRACTDIGIT_H

#include <TObject.h>
#include <TString.h>
#include <cstdint>

class BmnAbstractDigit : public TObject {
public:

  BmnAbstractDigit() : TObject(), fAddress(0), fTime(0.0), fSignal(0.0) {}
  BmnAbstractDigit(uint32_t address, double time, double signal)
      : TObject(), fAddress(address), fTime(time), fSignal(signal) {}

  ~BmnAbstractDigit() {}

  virtual void reset() {
    fAddress = 0;
    fTime = 0.0;
    fSignal = 0.0;
  }

  TString GetStringData() { return Form("Address %d Time %f Signal %f", GetAddress(), GetTime(), GetSignal()); }

  // Getters
  uint32_t GetAddress() const { return fAddress; }
  double GetTime() const { return fTime; }
  double GetSignal() const { return fSignal; }

  // Setters
  void SetAddress(uint32_t address) { fAddress = address; }
  void SetTime(double time) { fTime = time; }
  void SetSignal(double signal) { fSignal = signal; }

protected:

  uint32_t fAddress;         // address of digi according to BmnAddress scheme
  double fTime;              // time
  double fSignal;            // calibrated and digitized value

  ClassDef(BmnAbstractDigit, 1);

};

#endif // BMNABSTRACTDIGIT_H
