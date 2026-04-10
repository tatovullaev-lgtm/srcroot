#include "BmnHodoStrip.h"

#include <iostream>
using namespace std;

BmnHodoStrip::BmnHodoStrip()
  : fStripId()
  , fStripSignal()
  , fStripSignalIntegral()
  , fPileUpStatus(PileUp::No)  // Initialize fPileUpStatus to PileUp::No
{
}

BmnHodoStrip::BmnHodoStrip(const BmnHodoStrip& other)
  : fStripId(other.fStripId)
  , fStripSignal(other.fStripSignal)
  , fStripSignalIntegral(other.fStripSignalIntegral)
  , fPileUpStatus(other.fPileUpStatus)
{
}

BmnHodoStrip::BmnHodoStrip(BmnHodoStrip&& other)
  : fStripId(other.fStripId)
  , fStripSignal(other.fStripSignal)
  , fStripSignalIntegral(other.fStripSignalIntegral)
  , fPileUpStatus(other.fPileUpStatus)
{
}

BmnHodoStrip& BmnHodoStrip::operator=(const BmnHodoStrip& other)
{
  if (this != &other) {
    fStripId = other.fStripId;
    fStripSignal = other.fStripSignal;
    fStripSignalIntegral = other.fStripSignalIntegral;
    fPileUpStatus = other.fPileUpStatus;
  }
  return *this;
}

BmnHodoStrip& BmnHodoStrip::operator=(BmnHodoStrip&& other)
{
  if (this != &other) {
    fStripId = other.fStripId;
    fStripSignal = other.fStripSignal;
    fStripSignalIntegral = other.fStripSignalIntegral;
    fPileUpStatus = other.fPileUpStatus;
  }
  return *this;
}

void BmnHodoStrip::Print(Option_t *option) const
{
  // Implement your Print method if needed
}

void BmnHodoStrip::reset()
{
  fStripId = 0;
  fStripSignal = 0.;
  fStripSignalIntegral = 0.;
  fPileUpStatus = PileUp::No;  // Reset fPileUpStatus to PileUp::No
}
