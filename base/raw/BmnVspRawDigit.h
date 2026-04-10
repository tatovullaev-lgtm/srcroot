#pragma once
// #ifndef BMNVSPRAWDIGIT_H
// #define BMNVSPRAWDIGIT_H
//  STL
#include <vector>
// BmnRoot
#include <TObject.h>

using std::vector;

class BmnVspRawDigit : public TObject
{
  public:
    BmnVspRawDigit(uint64_t TrgEvId = 0, uint16_t DpbIdx = 0, uint64_t TrgTimeFLES = 0);

    void AppendHit(uint64_t HitTime, uint16_t ElinkIdx, uint16_t Chan, uint16_t Adc);

    size_t GetNHits() { return fVecHitTimeFLES.size(); }

    uint16_t GetDpbIdx() const { return fDpbIdx; }

    uint64_t GetTrgEvId() const { return fTriggerFLES; }

    uint64_t GetTrigTime() const { return fTrgTimeFLES; }

    void SetTTrigTime(uint64_t val) { fTrgTimeFLES = val; }

    uint16_t GetHitMissedEvts() const { return fHitMissedEvts; }

    void SetTHitMissedEvts(uint16_t val) { fHitMissedEvts = val; }

    vector<uint64_t>& GetVecHitTimeFLES() { return fVecHitTimeFLES; }

    void SetTVecHitTimeFLES(vector<uint64_t> val) { fVecHitTimeFLES = val; }

    vector<uint16_t>& GetVecElinkIdx() { return fVecElinkIdx; }

    void SetTVecElinkIdx(vector<uint16_t> val) { fVecElinkIdx = val; }

    vector<uint16_t>& GetVecChan() { return fVecChan; }

    void SetTVecChan(vector<uint16_t> val) { fVecChan = val; }

    vector<uint16_t>& GetVecAdc() { return fVecAdc; }

    void SetTVecAdc(vector<uint16_t> val) { fVecAdc = val; }
    virtual ~BmnVspRawDigit();

  protected:
    uint64_t fTriggerFLES;
    uint16_t fDpbIdx;
    uint64_t fTrgTimeFLES;
    uint16_t fHitMissedEvts;
    vector<uint64_t> fVecHitTimeFLES;
    vector<uint16_t> fVecElinkIdx;
    vector<uint16_t> fVecChan;
    vector<uint16_t> fVecAdc;

    ClassDef(BmnVspRawDigit, 1);
};

// #endif /* BMNVSPRAWDIGIT_H */
