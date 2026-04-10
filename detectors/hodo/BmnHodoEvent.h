/** \file BmnHodoEvent.h
 ** \author Nikolay Karpushkin
 ** \date 09.09.2022
 **/

/** \class BmnHodoEvent
 ** \brief Class for Bmn Hodo data container in event
 ** \version 0.1
 **/

#ifndef BMNHODOEVENT_H
#define BMNHODOEVENT_H

#include "BmnHodoStrip.h"

#include <TClonesArray.h>
#include <TNamed.h>
#include <array>
#include <iostream>
#include <vector>

class BmnHodoEvent : public TNamed
{
  public:
    BmnHodoEvent();
    ~BmnHodoEvent();

    virtual const char* GetClassName() { return "BmnHodoEvent"; }

    BmnHodoStrip* GetStrip(uint8_t strip_id);
    void SetStrip(uint8_t strip_id, const BmnHodoStrip& strip);

    float GetTotalSignal() const { return fTotalSignal; }
    float GetTotalSignalIntegral() const { return fTotalSignalIntegral; }
    BmnHodoStrip::PileUp GetPileUpStatus() const { return fPileUpStatus; }

    void reset();
    void ResetStrips();
    void SummarizeEvent();
    virtual void Print(Option_t* option = "") const;

    static const int fgkMaxStrips = 24;

  private:
    float fTotalSignal;
    float fTotalSignalIntegral;
    BmnHodoStrip::PileUp fPileUpStatus;
    std::array<BmnHodoStrip, fgkMaxStrips> fStripsArr;

    ClassDef(BmnHodoEvent, 3);
};

#endif /* BMNHODOEVENT_H */