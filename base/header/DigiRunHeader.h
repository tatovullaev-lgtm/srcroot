#ifndef BMNDIGIRUNHEADER_H
#define BMNDIGIRUNHEADER_H

#include <map>
#include <unordered_map>
// ROOT
#include "TObject.h"
#include "TTimeStamp.h"
// BmnRoot
#include "BmnAliases.h"
#include "BmnFunctionSet.h"
#include "BmnSpillInfo.h"
#include "BmnTrigConfig.h"
#include "RawTypes.h"

using std::map;
using std::unordered_map;
using std::vector;

class DigiRunHeader : public TObject
{
  private:
    /** Period Id **/
    UInt_t fPeriodId;
    /** Run Id **/
    UInt_t fRunId;
    /** Event count **/
    ULong64_t fEventCnt;
    TTimeStamp fRunStartTime;
    TTimeStamp fRunEndTime;
    ULong64_t fBeamTrigger;
    ULong64_t fBTnBusy;
    Double_t fBTAccepted;   ///< BT&notBusy normalized according to Acc/(Acc + BP + AP) | for SRC => sum [BT * (DAQ_Busy
                            ///< -peds)/ (DAQ_TRigger - peds)]
    ULong64_t fAccepted;
    ULong64_t fProtection;
    ULong64_t fL0;
    ULong64_t fBP;
    ULong64_t fAP;

    map<SysPoint, BmnSpillInfo> fSpillMap;
    T0ChMap fT0Map;
    NameChMap fTrcBitMap;
    Name2SerCh fScalersMap;
    vector<ULong64_t> fScalersVec;
    NameChMap scalers_name2index_map;                     ///< name -> index in fScalersVec (scaler index)
    ChNameMap scalers_index2name_map;                     ///< index -> name
    map<uint16_t, uint16_t> fTrcIdx2ScalerIdx;            ///<  TRC index -> scaler index
    unordered_map<UInt_t, vector<uint64_t>> fBoardSums;   ///<  (MSC serial, raw sums of the counters)

    std::array<ULong64_t, kT0_BIN_BLOCK_WORDS> rawt0_cntrs = {};
    MapString2Cnt event_trig_count;   // name -> total spill trigger (TRC bit inside events)

    BmnTrigConfig trig_config;

  public:
    /** Default constructor */
    DigiRunHeader();

    /** Constructor
     * \param[in] period_id : unique period id
     * \param[in] run_id : unique run id
     * \param[in] run_start_time : run start time
     * \param[in] run_end_time : run end time
     */
    DigiRunHeader(UInt_t period_id, UInt_t run_id, TTimeStamp run_start_time, TTimeStamp run_end_time);

    /** Destructor */
    virtual ~DigiRunHeader();

    /** Get the period ID for this run */
    ULong64_t GetPeriodId() { return fPeriodId; }
    /** Get the run ID */
    ULong64_t GetRunId() { return fRunId; }
    /** Get the event count */
    ULong64_t GetNEvents() { return fEventCnt; }
    /** Get the BeamTrigger counter */
    ULong64_t GetBT() { return fBeamTrigger; }
    /** Get the BT and not Busy  */
    ULong64_t GetBTnBusy() { return fBTnBusy; }
    /** Get the (BT and not Busy) by live time  */
    Double_t GetBTAccepted() { return fBTAccepted; }
    /** Get the Accepted by live time  */
    ULong64_t GetAccepted() { return fAccepted; }
    /** Get the Trigger Protection  */
    ULong64_t GetProtection() { return fProtection; }
    /** Get the L0  */
    ULong64_t GetL0() { return fL0; }
    /** Ref to the T0 map  */
    T0ChMap& T0Map() { return fT0Map; }
    /** TRC bit map */
    NameChMap& GetTrcBitMap() { return fTrcBitMap; }
    /** MSC bit map */
    Name2SerCh& GetScalerMap() { return fScalersMap; }

    NameChMap& ScalersName2IndexMap() { return scalers_name2index_map; }

    ChNameMap& ScalersIndex2NameMap() { return scalers_index2name_map; }

    map<uint16_t, uint16_t>& TrcIndex2ScalerIndexMap() { return fTrcIdx2ScalerIdx; }

    void PrintScalers()
    {
        for (auto& sca : scalers_name2index_map) {
            printf("MSC %-14s  %10llu\n", sca.first.c_str(), GetScaler(sca.first));
        }
    }

    vector<ULong64_t>& ScalersVec() { return fScalersVec; }

    ULong64_t GetScaler(string name);

    ULong64_t GetRawT0Counter(string name, UInt_t ch = 0);
    //    ULong64_t GetTrcCounter(string name);
    /** Raw T0 counters (sum over all spills)*/
    std::array<ULong64_t, kT0_BIN_BLOCK_WORDS>& RawT0Counters() { return rawt0_cntrs; }

    MapString2Cnt& EventTriggerCnt() { return event_trig_count; }

    //    vector<vector<> >
    /** Get the raw MSC counters  */
    unordered_map<UInt_t, vector<uint64_t>>& GetRawMSC() { return fBoardSums; }
    map<SysPoint, BmnSpillInfo>& GetSpillMap() { return fSpillMap; }

    /** Get DAQ trigger config */
    BmnTrigConfig& GetTrigConfig() { return trig_config; }

    TTimeStamp GetRunStartTime() { return fRunStartTime; }
    SysPoint GetRunStartTP() { return BmnFunctionSet::TimeStamp2TP(fRunStartTime); }
    TTimeStamp GetRunEndTime() { return fRunEndTime; }
    SysPoint GetRunEndTP() { return BmnFunctionSet::TimeStamp2TP(fRunEndTime); }

    /** Set the period ID for this run
     * \param[in] period_id : unique period id
     */
    void SetPeriodId(UInt_t period_id) { fPeriodId = period_id; }

    /** Set the period ID for this run
     * \param[in] run_id : unique run id
     */
    void SetRunId(UInt_t run_id) { fRunId = run_id; }
    /** Set the event count */
    void SetNEvents(ULong64_t events) { fEventCnt = events; }

    /** Set the period ID for this run
     * \param[in] run_start_time : run start time
     */
    void SetRunStartTime(TTimeStamp run_start_time) { fRunStartTime = run_start_time; }
    void SetRunStartTP(SysPoint run_start_time) { fRunStartTime = BmnFunctionSet::TimePoint2TS(run_start_time); }

    /** Set the period ID for this run
     * \param[in] run_end_time : run end time
     */
    void SetRunEndTime(TTimeStamp run_end_time) { fRunEndTime = run_end_time; }
    void SetRunEndTP(SysPoint run_end_time) { fRunEndTime = BmnFunctionSet::TimePoint2TS(run_end_time); }

    /** Set the Beam Trigger counter
     * \param[in] cntr : Beam Trigger counter
     */
    void SetBT(ULong64_t cntr) { fBeamTrigger = cntr; }
    /** Set the BT and Busy counter
     * \param[in] cntr : BT and Busy
     */
    void SetBTnBusy(ULong64_t cntr) { fBTnBusy = cntr; }
    /** Set the (BT and not Busy) by live time
     * \param[in] cntr : BT Accepted
     */
    void SetBTAccepted(Double_t cntr) { fBTAccepted = cntr; }
    /** Set the Accepted counter
     * \param[in] cntr : Accepted counter
     */
    void SetAccepted(ULong64_t cntr) { fAccepted = cntr; }
    /** Set the Trigger Protection counter
     * \param[in] cntr : Trigger Protection counter
     */
    void SetProtection(ULong64_t cntr) { fProtection = cntr; }
    /** Set the L0 counter
     * \param[in] cntr : L0 counter
     */
    void SetL0(ULong64_t cntr) { fL0 = cntr; }
    /** Set the raw MSC counters
     * \param[in] v : map < MSC serial, counters vector>
     */
    void SetRawMSC(unordered_map<UInt_t, vector<uint64_t>> v)
    {
        fBoardSums = v;
        //    printf("inner len %lu\n", fBoardSums.size());
    }
    void SetSpillMap(map<SysPoint, BmnSpillInfo> v)
    {
        fSpillMap = v;
        //    printf("inner len %lu\n", fSpillMap.size());
    }

    void SetTrigConfig(BmnTrigConfig tc) { trig_config = tc; }

    ClassDef(DigiRunHeader, 10)
};

#endif /* BMNDIGIRUNHEADER_H */
