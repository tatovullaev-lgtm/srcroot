#ifndef BMNRAWSOURCE_H
#define BMNRAWSOURCE_H 1

// #define MULTITHREAD true
//  STL
#include <bitset>
#include <fstream>
#include <map>
#include <vector>

#pragma GCC system_header
// Boost
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
// nlohmann
#include <nlohmann/json.hpp>
// ROOT
#include "TClonesArray.h"
#include "TFile.h"
#include "TPRegexp.h"
#include "TString.h"
#include "TTimeStamp.h"
#include "TTree.h"
// FairRoot
#include "FairRootManager.h"
#include "FairSource.h"
// BmnRoot
#include "BmnDetectorList.h"
#include "BmnEventHeader.h"
#include "BmnMath.h"
#include "BmnMetadataRaw.h"
#include "BmnSpillHeader.h"
#include "BmnT0Raw.h"
#include "BmnThreadManager.h"
#include "BmnTrigConfig.h"
#include "DigiRunHeader.h"
#include "RawTypes.h"

/********************************************************/
// wait limit for input data (ms)
#define WAIT_LIMIT 40000000

using std::map;
using std::vector;
namespace pt = boost::property_tree;

using json = nlohmann::json;

class BmnRawSource : public FairSource
{
  public:
    BmnRawSource(vector<TString> files, ULong_t period = 8);
    BmnRawSource(TString file = "", TString outfile = "", ULong_t nEvents = 0, ULong_t period = 8, bool save = false);
    virtual ~BmnRawSource();

    bool Init();
    Int_t ReadEvent(UInt_t i = 0);
    void Close();
    void Reset();

    Bool_t SpecifyRunId() { return kTRUE; }

    virtual Source_Type GetSourceType() { return kFILE; }

    virtual void SetParUnpackers() { LOGF(debug, "%s", __func__); }

    virtual Bool_t InitUnpackers()
    {
        LOGF(debug, "%s", __func__);
        return kTRUE;
    }

    virtual Bool_t ReInitUnpackers()
    {
        LOGF(debug, "%s", __func__);
        return kTRUE;
    }

    void FillEventHeader(FairEventHeader* feh)
    {
        LOGF(debug1, "FillEventHeader does nothing");
        return;
    }

    BmnStatus ParseJsonTLV(UInt_t* buf, UInt_t& len);
    BmnStatus ConvertStatEvent(UInt_t* buf, UInt_t& len);
    BmnStatus ConvertRawToRoot();
    BmnStatus ConvertRawToRootIterate(UInt_t* buf, UInt_t len);
    BmnStatus ConvertRawToRootIterateFile(UInt_t limit = WAIT_LIMIT);
    BmnStatus ConvertRawToRootIterateFileRead();
    void ClearRawArrays();
    void ClearRawSpillArrays();
    BmnStatus FinishRun();
    BmnStatus InitConverter(TString FileName);
    BmnStatus InitConverter();
    BmnStatus wait_file(Int_t len, UInt_t limit = WAIT_LIMIT);

    void SetRunId(UInt_t v) { fRunId = v; }

    void SetPeriodId(UInt_t v) { fPeriodId = v; }

    UInt_t GetRunId() const { return fRunId; }

    UInt_t GetPeriodId() const { return fPeriodId; }

    UInt_t GetNevents() const { return fNTotalEvents; }

    UInt_t GetEventId() const { return fEventId; }

    TTimeStamp GetRunStartTime() const { return fRunStartTime; }

    TTimeStamp GetRunEndTime() const { return fRunEndTime; }

    TString GetRootFileName() { return fRootFileName; }

    void SetVerbose(Int_t v) { fVerbose = v; }

    Int_t GetVerbose() const { return fVerbose; }

    UInt_t GetBoundaryRun(UInt_t nSmpl)
    {
        // format for SILICON data was changed during March 2017 seance (run 1542)
        // format for GEM was changed after March 2017 seance (run 1992)
        // so we have to use this crutch.
        return (nSmpl == 128) ? 1542 : 1992;
    }

    void SetRawRootFile(TString filename) { fRootFileName = filename; }

    void SetRawEventStat()
    {
        fCurEvType = kBMNSTAT;
        eventHeaderDAQ->SetEventType(fCurEvType);
    }

    void SetDecoderConfigFileName(string FileName)
    {
        fDecoderConfigFileName = FileName;
        LoadConfig();
    }

    TTree* GetRawTree() { return fRawTree; }

    TTree* GetRawTreeSpills() { return fRawTreeSpills; }

    Int_t GetRunIdFromFile(TString name);

    void SetSaveOutput(bool v) { fSaveOutput = v; }

    bool GetSaveOutput() const { return fSaveOutput; }

    void SetTaskMode(bool v = true) { isTaskMode = v; }

    bool GetTaskMode() const { return isTaskMode; }

    void SetForwardMode(bool v = true) { isForwardMode = v; }

    bool GetForwardMode() const { return isForwardMode; }

  private:
    //    static const Int_t T0_bin_block_words = 256;//BmnT0Raw::CountersCount();
    BmnThreadManager<BmnConverterThread>* fThreads;
    const TTimeStamp TimeZero = TTimeStamp(time_t(0), 0);
    Double_t workTime_cpu = 0.0;
    Double_t workTime_real = 0.0;

    pt::ptree conf;
    bool isSpillStart;
    bool isTaskMode;
    bool isForwardMode;
    UInt_t fSpillCntr;
    BmnEventType fCurEvType = kBMNPAYLOAD;    // for converter
    BmnEventType fPrevEvType = kBMNPAYLOAD;   // for converter
    bool isRawRootInputFile;

    UInt_t fRunId;
    UInt_t fPeriodId;
    UInt_t fEventId;
    UInt_t fNTotalEvents;
    UInt_t fNSignalEvents;
    UInt_t fStartEventId;
    // for event
    Long64_t fTime_s;
    Long64_t fTime_ns;
    // for run
    TTimeStamp fRunStartTime;
    TTimeStamp fRunEndTime;

    Long64_t fLengthRawFile;
    Long64_t fCurentPositionRawFile;

    TTree* fRawTree;
    TTree* fRawTreeSpills;
    string fDecoderConfigFileName;
    TString fRootFileName;
    TString fRawFileName;
    TString fSubName;
    TString fRawRunHdrName;
    TString fMetadataName;

    TFile* fRootFileIn;
    TFile* fRootFileOut;
    FILE* fRawFileIn;

    // DAQ arrays
    TClonesArray* sync;
    TClonesArray* adc32;    // gem
    TClonesArray* adc64;    // SiBT
    TClonesArray* adc128;   // sts
    TClonesArray* adc;      // zdc & ecal & scwall & fhcal
    TClonesArray* vsp;
    TClonesArray* hrb;
    TClonesArray* tdc;
    TClonesArray* tqdc_tdc;
    TClonesArray* tqdc_adc;
    TClonesArray* tdc_hgnd;   // hgnd
    // Spill tree arrays
    TClonesArray* msc;
    TClonesArray* t0raw;
    BmnEventHeader* eventHeaderDAQ;

    // header array
    BmnSpillHeader* spillHeader;
    // Raw run header
    unique_ptr<DigiRunHeader> runHeader;

    unique_ptr<BmnMetadataRaw> metadata;

    UInt_t data[10000000];
    ULong_t fMaxEvent;

    UInt_t fDat;   // current 32-bits word
    UInt_t syncCounter;

    UInt_t nSpillEvents;
    Bool_t fExportJsonBlocks;
    Bool_t fExportExternalSpillStat;

    UInt_t n_msc_spill = 0;
    UInt_t n_msc_spill_ar[16] = {};

    std::map<uint8_t, uint32_t*> time_slice_map;

    map<TTimeStamp, Int_t> leaps;
    TTimeStamp utc_valid;
    Int_t tai_utc_dif;
    Int_t fVerbose;
    map<SysPoint, BmnSpillInfo> fSpillMap;
    //    map<TTimeStamp, BmnSpillInfo> fSpillMap;
    Bool_t fSaveOutput;

    template<typename BrType>
    void BranchRegFun(TString name, BrType** ar)
    {
        if (isTaskMode) {
            FairRootManager* frm = FairRootManager::Instance();
            frm->Register(name, name + "_dir", (*ar), fSaveOutput);
        } else {
            TBranch* b = fRawTree->Branch(name, ar);
            LOGF(debug1, "Register branch %p for obj %p : %s", (void*)b, (void*)ar, name.Data());
        }
        return;
    }

    BmnStatus InitUTCShift();
    Int_t GetUTCShift(TTimeStamp t);
    BmnStatus GetT0Info(Double_t& t0time, Double_t& t0width);
    BmnStatus ProcessEvent(UInt_t* data, UInt_t len);
    void QuickProcessEvent(UInt_t* d, UInt_t len);
    /**
     * Finishes the spill during raw conversion
     */
    void FinalizeStat();

    BmnStatus Process_FVME(UInt_t* data, UInt_t len, UInt_t serial, BmnTrigInfo& spillInfo);
    BmnStatus Process_HRB(UInt_t* data, UInt_t len, UInt_t serial);
    BmnStatus FillU40VE(UInt_t* d, UInt_t slot, UInt_t& idx, BmnTrigInfo& spillInfo);
    BmnStatus FillBlockTDC(UInt_t* d, UInt_t serial, uint16_t& len, TClonesArray* ar, UInt_t modid);
    BmnStatus FillBlockADC(UInt_t* d, UInt_t serial, uint8_t channel, uint16_t& len, TClonesArray* ar);
    BmnStatus FillTDC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t& idx);
    BmnStatus FillTQDC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t& idx);
    /**
     * Parse TQDC16VS-E MStream data block
     * https://afi.jinr.ru/DataFormatTQDC16VSE
     * @param d data pointer
     * @param serial device serial
     * @param len payload length
     * @return operation success
     */
    BmnStatus FillTQDC_E(UInt_t* d, UInt_t serial, UInt_t& len);
    /**
     * Parse MSC16VE-E MStream data block
     * https://afi.jinr.ru/DataFormatMSC_ETH
     * @param d data pointer
     * @param serial device serial
     * @param len payload length
     * @return operation success
     */
    BmnStatus FillMSC16VE_E(UInt_t* d, DeviceHeader* dev_hdr);
    /**
     * https://afi.jinr.ru/TDC72VXS_DataFormat
     */
    BmnStatus FillTDC72VXS(UInt_t* d, UInt_t serial, UInt_t& len, UInt_t modid);
    /**
     * https://afi.jinr.ru/TTVXS_DataFormat
     */
    BmnStatus FillTTVXS(UInt_t* d, UInt_t serial, UInt_t& len);
    BmnStatus ProcessGenericDevice(UInt_t* d, DeviceHeader* dev_hdr);
    /**
     * Parse custom HGND TDC data block
     */
    BmnStatus FillTDC250HGND(UInt_t* d, UInt_t serial, UInt_t len);
    /**
     * Parse VSP data block
     */
    BmnStatus MapVSP(UInt_t* d, DeviceHeader* dev_hdr);
    BmnStatus FillVSP(TClonesArray* ar);
    /**
     * Parse UT24VE-TRC MStream data block
     * https://afi.jinr.ru/DataFormatUT24VE-TRC
     * @param d data pointer
     * @param serial device serial
     * @param len payload length
     * @param evType calibration/payload event
     * @return operation success
     */
    BmnStatus FillUT24VE_TRC(UInt_t* d, UInt_t& serial, UInt_t& len);
    BmnStatus FillVirtualDevice(UInt_t* d, DeviceHeader* dh);
    BmnStatus FillFVME2TMWR(UInt_t* d, UInt_t serial, UInt_t& idx, UInt_t& len);
    inline BmnSyncDigit* FillWR(UInt_t iSerial, ULong64_t iEvent, Long64_t t_sec, Long64_t t_ns);

    BmnStatus FillMSC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t& idx);
    BmnStatus LoadConfig();
    void RegisterBranches();   // std::function<void(TString, TObject**) > branch_reg_fun);
    void RegisterSpillBranches(std::function<void(TTree* tree, TString, TObject*)> branch_reg_fun);
    //    void RegisterBranches(bool save = true);

    ClassDef(BmnRawSource, 1);
};

#endif
