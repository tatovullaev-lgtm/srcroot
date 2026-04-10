#ifndef BMNMSCMATRIX_H
#define BMNMSCMATRIX_H

#include <chrono>
#include <map>
#include <vector>
// Boost
#include <boost/dynamic_bitset.hpp>
// #include <boost/numeric/ublas/matrix_sparse.hpp>
// ROOT
// #include <TMatrixTSparse.h>
#include <TObject.h>
// BmnRoot
#include "BmnFunctionSet.h"

using SysClock = std::chrono::system_clock;
using SysPoint = std::chrono::time_point<SysClock>;

using std::string;
using std::vector;

// namespace ublas = boost::numeric::ublas;

// template class TMatrixTSparse<uint8_t>;
// template class ublas::compressed_matrix<uint8_t>;

/**
 * Storage for all MSC counters of the spill
 * @param times_count
 * @param scaler_count
 */
template<typename IntType = uint8_t>
class BmnMSCMatrix : public TObject
{
    typedef map<SysPoint, IntType> TimeLine;
    typedef map<SysPoint, size_t> Time2Pos;
    typedef map<string, TimeLine> Scaler2TimeDist;

    typedef std::map<uint8_t, TimeLine> ScalerIdx2TimeDist;

  public:
    BmnMSCMatrix(size_t times_count = 0,
                 //            uint16_t scaler_count = 0,
                 NameChMap scalerName2Index = NameChMap{})
        : fNRecords{times_count}
        //        , fNScalers{scalerName2Index.size()}   //        , fVector(fNRecords * fNScalers, 0)
        //                , fMatrix{fNRecords, fNScalers, false}
        //                , fMatrix{(int32_t)fNRecords, fNScalers}
        , fScalerName2index{scalerName2Index}
        , fMapVector(GetNScalers())
        , fTimes(fNRecords, SysPoint::min())
        , fBitVecSpill(fNRecords, false)
        , fBitVecBusy(fNRecords, false)
    {}

    //    IntType& Counts(size_t i_record, uint16_t i_scaler) { return fVector[i_record * fNScalers + i_scaler]; }
    //        auto& Counts() { return fMatrix; }
    IntType GetCount(size_t record_id, string scaler_name)
    {
        if (record_id >= GetNRecords())
            return 0;
        TimeLine& row = ScalerTimeLine(scaler_name);
        SysPoint& time = fTimes[record_id];
        return GetCntByTime(row, time);
    }
    IntType GetCount(size_t record_id, uint16_t scaler_id)
    {
        if (record_id >= GetNRecords())
            return 0;
        TimeLine& row = ScalerTimeLine(scaler_id);
        SysPoint& time = fTimes[record_id];
        return GetCntByTime(row, time);
        //            return fMatrix(record_id, scaler_id);
    }
    IntType GetCount(SysPoint& time, string scaler_name)
    {
        auto it = fScalerName2index.find(scaler_name);
        if (it == fScalerName2index.end()) {
            throw out_of_range("Scaler not found");
        }
        return GetCount(time, it->second);
    }
    IntType GetCount(SysPoint& time, uint16_t scaler_id)
    {
        TimeLine& row = ScalerTimeLine(scaler_id);
        return GetCntByTime(row, time);
    }

    void SetCount(size_t record_id, uint16_t scaler_id, IntType val)
    {
        TimeLine& row = ScalerTimeLine(scaler_id);
        SysPoint& time = fTimes[record_id];
        row[time] = val;
        //            fMatrix(record_id, scaler_id) = val;
    }

    void SetCount(size_t record_id, string scaler_name, IntType val)
    {
        TimeLine& row = ScalerTimeLine(scaler_name);
        SysPoint& time = fTimes[record_id];
        row[time] = val;
    }

    SysPoint& Time(size_t record_id) { return fTimes[record_id]; }
    TTimeStamp GetTS(size_t record_id) { return BmnFunctionSet::TimePoint2TS(fTimes[record_id]); }
    void SetTS(size_t record_id, TTimeStamp ts) { fTimes[record_id] = BmnFunctionSet::TimeStamp2TP(ts); }

    NameChMap& GetScalerName2indexMap() { return fScalerName2index; }
    //    NameChMap& ScalerName2indexMap() { return fScalerName2index; }

    TimeLine& ScalerTimeLine(string scaler_name)
    {
        auto it = fScalerName2index.find(scaler_name);
        if (it == fScalerName2index.end()) {
            throw out_of_range("Scaler name not found");
        }
        return ScalerTimeLine(it->second);
    }
    TimeLine& ScalerTimeLine(uint16_t scaler_id)
    {
        if (scaler_id >= fScalerName2index.size())
            throw out_of_range("Scaler index not found");
        return fMapVector[scaler_id];
    }
    static IntType GetCntByTime(TimeLine& row, SysPoint& time)
    {
        auto it = row.find(time);
        if (it == row.end())
            return 0;
        return it->second;
    }

    auto& IsSpillBitVec() { return fBitVecSpill; }

    auto& IsBusyBitVec() { return fBitVecBusy; }

    const size_t GetNRecords() { return fNRecords; }

    const uint16_t GetNScalers() { return fScalerName2index.size(); }
    //    const uint16_t GetNScalers() { return fNScalers; }
    /** Destructor **/
    virtual ~BmnMSCMatrix() {}

  private:
    const size_t fNRecords;
    //    const uint16_t fNScalers;
    NameChMap fScalerName2index;
    vector<TimeLine> fMapVector;
    //    std::map<IntType> fVector;
    //    std::vector<IntType> fVector;
    //    Time2Pos fTimeMap;
    //    ublas::compressed_matrix<IntType> fMatrix; // WTF ROOT cannot work with it in a macro
    //    TMatrixTSparse<uint8_t> fMatrix; // "template" only supports a couple of prebuilt types
    vector<SysPoint> fTimes;
    //    std::vector<bool> fBitVecSpill;
    //    std::vector<bool> fBitVecBusy;
    boost::dynamic_bitset<> fBitVecSpill;
    boost::dynamic_bitset<> fBitVecBusy;

    //    Scaler2TimeDist scalers;
    //    TimeDist ext_cond;

    ClassDef(BmnMSCMatrix, 2);
};

#endif /* BMNMSCMATRIX_H */
