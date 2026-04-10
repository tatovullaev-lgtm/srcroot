/** \file BmnHodoEvent.cxx
 ** \author Nikolay Karpushkin
 ** \date 09.09.2022
 **/

#include "BmnHodoEvent.h"

BmnHodoEvent::BmnHodoEvent()
  : TNamed() {
  reset();
}

BmnHodoEvent::~BmnHodoEvent() {
  // Destructor implementation if needed
}

BmnHodoStrip* BmnHodoEvent::GetStrip(uint8_t strip_id) {
  return &fStripsArr.at(strip_id - 1);
}

void BmnHodoEvent::SetStrip(uint8_t strip_id, const BmnHodoStrip& strip) {
  fStripsArr.at(strip_id - 1) = strip;
}

void BmnHodoEvent::reset() {
  fStripsArr.fill({});
}

void BmnHodoEvent::ResetStrips() {
  fTotalSignal = 0.;
  fTotalSignalIntegral = 0.;
  fPileUpStatus = BmnHodoStrip::PileUp::No;

  int counter = 1;
  for (auto& it : fStripsArr) {
    it.SetStripId(counter);
    it.SetSignal(0.);
    it.SetSignalIntegral(0.);
    it.SetPileUpStatus(BmnHodoStrip::PileUp::No);
    counter++;
  }
}

void BmnHodoEvent::SummarizeEvent() {
  fTotalSignal = 0.;
  fTotalSignalIntegral = 0.;
  for (auto& it : fStripsArr) {
    fTotalSignal += it.GetSignal();
    fTotalSignalIntegral += it.GetSignalIntegral();
    fPileUpStatus = std::max(fPileUpStatus, it.GetPileUpStatus());
  }
}

void BmnHodoEvent::Print(Option_t* option) const {
  // Implementation of Print method if needed
}
