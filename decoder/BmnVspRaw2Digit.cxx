#include "BmnVspRaw2Digit.h"
// FairRoot
#include "FairLogger.h"
// BmnRoot
#include "BmnVSPDigit.h"
#include "BmnVspRawDigit.h"
#include "StsConst.h"

BmnVspRaw2Digit::BmnVspRaw2Digit(int32_t period, int32_t run, TString mapFileName, BmnSetup bmnSetup)
    : fPeriod(period)
    , fRun(run)
    , fSetup(bmnSetup)
    , station_set(BmnVSPStationSet::Create(fPeriod, fSetup))
{}

BmnStatus BmnVspRaw2Digit::FillEvent(TClonesArray* adc, TClonesArray* digits)
{
    uint64_t ulHitTimeFLESbeg = 0;
    bool fIsFirstFound = false;
    for (int32_t iRaw = 0; iRaw < adc->GetEntriesFast(); ++iRaw) {
        BmnVspRawDigit* dig = static_cast<BmnVspRawDigit*>(adc->At(iRaw));
        if (!fIsFirstFound) {
            ulHitTimeFLESbeg = dig->GetTrigTime();
            fIsFirstFound = true;
        }
        for (size_t iHit = 0; iHit < dig->GetNHits(); iHit++) {
            uint16_t usAsicIdx = StsConst::GetIdx_AsicFromELink(dig->GetVecElinkIdx()[iHit]);
            StsConst::SideType sSType = static_cast<StsConst::SideType>(StsConst::GetIdx_SideFromAsic(usAsicIdx));
            uint16_t usSensor =
                StsConst::GetIdx_SensorFromAsic(dig->GetDpbIdx(), usAsicIdx);   // call for the multi DPB analysis
            uint16_t usStrip = StsConst::GetIdx_Strip(usSensor, usAsicIdx, dig->GetVecChan()[iHit]);
            uint64_t ulTimeSinceBegin_ns =
                (dig->GetVecHitTimeFLES()[iHit] - ulHitTimeFLESbeg) * StsConst::kdClockCycleNs;   // in ns
            Double_t dTimeSinceBegin_ns = static_cast<Double_t>(ulTimeSinceBegin_ns);             // in ns
            uint16_t iMod = usSensor > 2 ? (8 - usSensor) : usSensor;                             // @TODO replace
            uint16_t iLayer = (sSType == StsConst::SideType::NEG) ? 0 : 1;                        // @TODO replace
            uint16_t iStation = 0;
            auto* st = station_set->GetStation(iStation);
            auto* mod = st->GetModule(iMod);
            auto& lay2 = mod->GetStripLayer(2);
            uint16_t iStrip =
                //                                (lay.GetStripNumberingOrder() == StripNumberingDirection::RightToLeft)
                //                                ?
                //                                    (lay.GetNStrips() - usStrip) :
                usStrip;
            BmnVSPDigit* newdig = new ((*digits)[digits->GetEntriesFast()])
                BmnVSPDigit(iStation, iMod, iLayer, iStrip, dig->GetVecAdc()[iHit]);
            newdig->SetTTimeSinceBegin_ns(dTimeSinceBegin_ns);
            // repeat to the 3rd layer connected wires
            if ((iLayer == 1) && (iStrip <= lay2.GetLastStripNumber())) {
                BmnVSPDigit* newdig2 = new ((*digits)[digits->GetEntriesFast()]) BmnVSPDigit(newdig);
                newdig2->SetStripLayer(2);
            }
        }
    }
    return kBMNSUCCESS;
}
