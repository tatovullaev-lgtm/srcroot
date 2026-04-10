#ifndef BMNCONVERTERTHREAD_H
#define BMNCONVERTERTHREAD_H 1

#include <queue>
#include <vector>
// ROOT
#include "TClonesArray.h"
#include "TPRegexp.h"
#include "TROOT.h"
#include "TString.h"
#include "TTree.h"

#include <ROOT/TBufferMerger.hxx>
// FairRoot
#include "FairLogger.h"
// BmnRoot
#include "BmnAbstractThread.h"
#include "BmnEnums.h"
#include "BmnEventHeader.h"
#include "BmnSyncDigit.h"
#include "RawTypes.h"

using namespace ROOT;

class BmnConverterThread : public BmnAbstractThread
{
  public:
    TTree* fRawTree;
    TTree* fRawTreeSpills;
    TBufferMerger* merger;
    std::shared_ptr<TBufferMergerFile> mergerFile;
    UInt_t rawData[10000000];

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
    TClonesArray* msc;
    TClonesArray* t0raw;   // Sergeev's txt spill statistics
    BmnEventHeader* eventHeaderDAQ;

    std::map<uint8_t, uint32_t*> time_slice_map;

    struct BmnEventData
    {
        BmnEventType fCurEvType;
        UInt_t fRunId;
        UInt_t fEventId;
        Long64_t fTime_s;
        Long64_t fTime_ns;
    };

    BmnConverterThread(queue<Int_t>* thread_queue,
                       std::mutex* queue_access,
                       std::condition_variable* queue_wait,
                       Int_t id,
                       Bool_t measure_time = kFALSE)
        : BmnAbstractThread(thread_queue, queue_access, queue_wait, id, measure_time)
        , fRawTree(nullptr)
        , fRawTreeSpills(nullptr)
        , merger(nullptr)
        , sync(nullptr)
        , adc32(nullptr)
        , adc64(nullptr)
        , adc128(nullptr)
        , adc(nullptr)
        , vsp(nullptr)
        , hrb(nullptr)
        , tdc(nullptr)
        , tqdc_tdc(nullptr)
        , tqdc_adc(nullptr)
        , tdc_hgnd(nullptr)
        , msc(nullptr)
        , t0raw(nullptr)
        , eventHeaderDAQ(nullptr)
        , tai_utc_dif(0)
    {
        InitUTCShift();
        //        CreateTrees();
    }

    BmnConverterThread(BmnConverterThread const&) = delete;
    BmnConverterThread& operator=(BmnConverterThread const&) = delete;
    virtual ~BmnConverterThread()
    {
        delete fRawTree;
        delete fRawTreeSpills;
    }

    void CreateTrees();
    void RegisterBranches();
    void RegisterSpillBranches();
    void ClearRawArrays();
    void ClearRawSpillArrays();

    void SpillEnd();
    void SetTrees(TTree* tree, TTree* treeSpills);

    void SetPeriod(UInt_t v) { fPeriodId = v; }

    void SetData(Int_t taskId,
                 Int_t len = 0,
                 UInt_t* data = nullptr,
                 BmnEventType fCurEvType = kBMNEMPTY,
                 UInt_t fRunId = 7444,
                 UInt_t fEventId = -1);

    void SetInitData(vector<pair<int, pair<int, int>>>* vec_tree, vector<pair<int, pair<int, int>>>* vec_tree_spills)
    {   // map<TTimeStamp, Int_t>* leaps, TTimeStamp utc_valid, Int_t tai_utc_dif){
        this->vecTree = vec_tree;
        this->vecTreeSpills = vec_tree_spills;
        // this->leaps = leaps;
        // this->utc_valid = utc_valid;
        // this->tai_utc_dif = tai_utc_dif;
    }
    void SetMerger(TBufferMerger* m) { merger = m; }
    void Terminate()
    {
        taskId = -1;
        Execute();
        delete sync;
        delete adc32;
        delete adc64;
        delete adc128;
        delete adc;
        delete vsp;
        delete hrb;
        delete tdc;
        delete tqdc_tdc;
        delete tqdc_adc;
        delete tdc_hgnd;
        delete msc;
        delete t0raw;
        delete eventHeaderDAQ;
        //        LOGF(info, "Closing converter thread %d; Waiting time: %.3fs, Calculation time: %.3fs",
        //         threadId, GetWaitingTime(), GetCalculationTime());
        LOGF(info, "Closing converter thread %d; Waiting time: %.3fs, Calculation time: %.3fs", threadId,
             GetWaitingCTime().count(), GetCalculationCTime().count());
    }

    void WriteTree();
    void WriteTreeSpills();
    void FinalizeFile();
    void SetExportExternalSpillStat(bool v) { fExportExternalSpillStat = v; }

  private:
    Bool_t fExportExternalSpillStat;   // export Sergeev's spill log from virtual device
    UInt_t fPeriodId;

    void WriteSpill(Int_t spillId);
    void Calculate();
    template<typename BrType>
    void BranchRegFun(TString name, BrType** ar)
    {
        /*if (isTaskMode) {
            cout << "Hmm";
            FairRootManager* frm = FairRootManager::Instance();
            frm->Register(name, name + "_dir", (*ar), fSaveOutput);
            //            TClass *cl = TClass::GetClass((*ar)->ClassName());
            //            if (cl == TClonesArray::Class())
            //                frm->Register(name, name + "_dir", static_cast<TClonesArray*> ((*ar)), fSaveOutput);
            //            else
            //                frm->Register(name, name + "_dir", static_cast<TNamed*> ((*ar)), fSaveOutput);
            //            LOG(debug1) << "Register by class " << cl->GetName() << " : " << name;
        } else {*/
        TBranch* b = fRawTree->Branch(name, ar);
        LOGF(debug1, "Register branch %p : %s", (void*)b, name.Data());
        //}
        return;
    }

    // BmnStatus result;
    BmnEventData fCurEvent;
    UInt_t dataLen;

    vector<pair<int, pair<int, int>>>* vecTree;
    vector<pair<int, pair<int, int>>>* vecTreeSpills;

    // Bool_t isTaskMode, fSaveOutput;

    // Map to store pairs <Crate serial> - <crate time - T0 time>
    map<TTimeStamp, Int_t> leaps;
    TTimeStamp utc_valid;
    Int_t tai_utc_dif;

    BmnStatus InitUTCShift();
    Int_t GetUTCShift(TTimeStamp t);
    BmnStatus ProcessEvent(UInt_t* data, UInt_t len);
    BmnStatus ConvertStatEvent(UInt_t* d, UInt_t& len);

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
    BmnStatus FillMSC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t& idx);
    inline BmnSyncDigit* FillWR(UInt_t iSerial, ULong64_t iEvent, Long64_t t_sec, Long64_t t_ns);

    ClassDef(BmnConverterThread, 1);
};

#endif
