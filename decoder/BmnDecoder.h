#ifndef BMNDECODER_H
#define BMNDECODER_H 1

#pragma GCC system_header
#include <deque>
#include <fstream>
#include <map>
#include <vector>
// Boost
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
// ROOT
#include "TClonesArray.h"
#include "TFile.h"
#include "TPRegexp.h"
#include "TString.h"
#include "TTimeStamp.h"
#include "TTree.h"
// FairRoot
#include "FairRootManager.h"
// BmnRoot
#include "BmnAdcQA.h"
#include "BmnCscRaw2Digit.h"
#include "BmnDchRaw2Digit.h"
#include "BmnECALRaw2Digit.h"
#include "BmnEventHeader.h"
#include "BmnFHCalRaw2Digit.h"
#include "BmnGemRaw2Digit.h"
#include "BmnHgndRaw2Digit.h"
#include "BmnHodoRaw2Digit.h"
#include "BmnMetadataRaw.h"
#include "BmnMscRaw2Digit.h"
#include "BmnMwpcRaw2Digit.h"
#include "BmnNdetRaw2Digit.h"
#include "BmnScWallRaw2Digit.h"
#include "BmnSiBTRaw2Digit.h"
#include "BmnSiliconRaw2Digit.h"
#include "BmnTask.h"
#include "BmnTof1Raw2Digit.h"
#include "BmnTof2Raw2DigitNew.h"
#include "BmnTof701Raw2Digit.h"
#include "BmnTrigRaw2Digit.h"
#include "BmnVspRaw2Digit.h"
#include "BmnZDCRaw2Digit.h"
#include "DigiArrays.h"
/********************************************************/
// wait limit for input data (ms)
#define WAIT_LIMIT 40000000
#define UNKNOWN_RUNID 100000
// Debug ADC processor
// #define ADC_QA true

using std::map;
using std::vector;
namespace pt = boost::property_tree;

using json = nlohmann::json;

class BmnDecoder : public BmnTask
{
  public:
    BmnDecoder(TString file = "", TString outfile = "", ULong_t nEvents = 0, ULong_t period = 8);
    virtual ~BmnDecoder();

    InitStatus Init();
    InitStatus ReInit();
    void Exec(Option_t* opt);
    void AssignInputs();
    //    void AssignInputs(std::function<TObject*(TString) > branch_assignment_fun);
    void InitMappers(std::function<void(TString, TObject*)> branch_reg_fun);
    void ClearDigiArrays();
    void ClearSpillDigiArrays();
    void RecalculatePedestals();
    void FillAdcQa();
    void SaveAdcQa();
    void FillProfiles();
    void FillNoisyChannels();
    BmnStatus DecodeDataToDigi();
    BmnStatus DecodeDataToDigiIterate();
    BmnStatus PreInitOffline();
    BmnStatus InitDecoder();
    BmnStatus DecodeOffline();
    void ConfigureStripMappers();
    BmnStatus Finalize();

    DigiArrays GetDigiArraysObject()
    {
        //        fDigiTree->GetEntry(GetEventId());
        DigiArrays d;   // = new DigiArrays();
        d.silicon = silicon;
        d.gem = gem;
        d.csc = csc;
        d.vsp = vsp;
        d.tof400 = tof400;
        d.tof700 = tof700;
        d.tof701 = tof701;
        d.zdc = zdc;
        d.scwall = scwall;
        d.fhcal = fhcal;
        d.hodo = hodo;
        d.ndet = ndet;
        d.hgnd = hgnd;
        d.ecal = ecal;
        d.dch = dch;
        d.mwpc = mwpc;
        d.sibt = sibt;
        d.msc = msc_copy;
        d.header = eventHeader;
        if (fTrigMapper)
            d.trigAr = fTrigMapper->GetTrigArrays();
        return d;
    }

    void SetRawTree(TTree* tree) { fRawTree = tree; }

    TTree* GetDigiTree() { return fDigiTree; }

    void SetTOF700ReferenceRun(Int_t n) { fTOF700ReferenceRun = n; }

    void SetTOF700ReferenceChamber(Int_t n) { fTOF700ReferenceChamber = n; }

    void SetRunId(UInt_t v) { fRunId = v; }

    void SetPeriodId(UInt_t v) { fPeriodId = v; }

    void SetApplyThreshold(bool b) { fApplyThreshold = b; }

    unordered_map<UInt_t, Long64_t> GetTimeShifts() { return fTimeShifts; }

    UInt_t GetRunId() const { return fRunId; }

    UInt_t GetPeriodId() const { return fPeriodId; }

    UInt_t GetNevents() const { return fNTotalEvents; }

    UInt_t GetEventId() const { return fEventId; }

    TTimeStamp GetRunStartTime() const { return fRunStartTime; }

    TTimeStamp GetRunEndTime() const { return fRunEndTime; }

    // BmnTof2Raw2DigitNew* GetTof700Mapper() { return fTof700Mapper; }

    BmnZDCRaw2Digit* GetZDCMapper() { return fZDCMapper; }

    BmnScWallRaw2Digit* GetScWallMapper() { return fScWallMapper; }

    BmnFHCalRaw2Digit* GetFHCalMapper() { return fFHCalMapper; }

    BmnHodoRaw2Digit* GetHodoMapper() { return fHodoMapper; }

    BmnNdetRaw2Digit* GetNdetMapper() { return fNdetMapper; }

    BmnHgndRaw2Digit* GetHgndMapper() { return fHgndMapper; }

    BmnECALRaw2Digit* GetECALMapper() { return fECALMapper; }

    void SetTrigPlaceMapping(TString map) { fTrigPlaceMapFileName = map; }

    void SetSiliconMapping(TString map) { fSiliconMapFileName = map; }

    void SetSiBTMapping(TString map) { fSiBTMapFileName = map; }

    void SetCSCMapping(TString map) { fCscMapFileName = map; }

    void SetTrigChannelMapping(TString file) { fTrigChannelMapFileName = file; }

    void SetDchMapping(TString map) { fDchMapFileName = map; }

    void SetMwpcMapping(TString map) { fMwpcMapFileName = map; }

    void SetGemMapping(TString map) { fGemMapFileName = map; }

    void SetVspMapping(TString map) { fVspMapFileName = map; }

    void SetTof400Mapping(TString PlaceMap, TString StripMap)
    {
        fTof400PlaceMapFileName = PlaceMap;
        fTof400StripMapFileName = StripMap;
    }

    void SetTof701Mapping(TString PlaceMap, TString StripMap)
    {
        fTof701PlaceMapFileName = PlaceMap;
        fTof701StripMapFileName = StripMap;
    }

    void SetTof700Mapping(TString map) { fTof700MapFileName = map; }

    void SetTof700Geom(TString geom) { fTof700GeomFileName = geom; }

    // Legacy
    void SetTof700SlewingReference(Int_t chamber, Int_t refrun, Int_t refchamber);

    void SetZDCMapping(TString map) { fZDCMapFileName = map; }

    void SetZDCCalibration(TString cal) { fZDCCalibrationFileName = cal; }

    void SetScWallMapping(TString map) { fScWallMapFileName = map; }

    void SetScWallCalibration(TString cal) { fScWallCalibrationFileName = cal; }

    void SetFHCalMapping(TString map) { fFHCalMapFileName = map; }

    void SetFHCalCalibration(TString cal) { fFHCalCalibrationFileName = cal; }

    void SetHodoMapping(TString map) { fHodoMapFileName = map; }

    void SetHodoCalibration(TString cal) { fHodoCalibrationFileName = cal; }

    void SetNdetMapping(TString map) { fNdetMapFileName = map; }

    void SetNdetCalibration(TString cal) { fNdetCalibrationFileName = cal; }

    void SetHgndMapping(TString map) { fHgndMapFileName = map; }

    void SetHgndCalibration(TString cal) { fHgndCalibrationFileName = cal; }

    void SetECALMapping(TString map) { fECALMapFileName = map; }

    void SetECALCalibration(TString cal) { fECALCalibrationFileName = cal; }

    void SetMSCMapping(TString map) { fMSCMapFileName = map; }

    TString GetRootFileName() { return fRootFileName; }

    void SetDetectorSetup(std::map<DetectorId, bool> setup) { fDetectorSetup = setup; }

    void SetEvForPedestals(UInt_t v) { fEvForPedestals = v; }

    UInt_t GetEvForPedestals() { return fEvForPedestals; }

    void SetBmnSetup(BmnSetup v) { fBmnSetup = v; }

    BmnSetup GetBmnSetup() const { return fBmnSetup; }

    void SetVerbose(Int_t v) { fVerbose = v; }

    Int_t GetVerbose() const { return fVerbose; }

    void SetUseCalibFile(bool v) { fUseCalibFile = v; }

    bool GetUseCalibFile() const { return fUseCalibFile; }

    void SetSaveOutput(bool v) { fSaveOutput = v; }

    bool GetSaveOutput() const { return fSaveOutput; }

    void SetTaskMode(bool v = true) { isTaskMode = v; }

    bool GetTaskMode() const { return isTaskMode; }

    /***
     Set forward mode (without intermediate raw root file)
     */
    void SetForwardMode(bool v = true) { isForwardMode = v; }

    bool GetForwardMode() const { return isForwardMode; }

    UInt_t GetBoundaryRun(UInt_t nSmpl)
    {
        // format for SILICON data was changed during March 2017 seance (run 1542)
        // format for GEM was changed after March 2017 seance (run 1992)
        // so we have to use this crutch.
        return (nSmpl == 128) ? 1542 : 1992;
    }

    void SetRawRootFile(TString filename) { fRootFileName = filename; }

    void SetDigiRootFile(TString filename) { fDigiFileName = filename; }

    TString GetDigiFileName() { return fDigiFileName; }

    void SetDecoderConfigFileName(string FileName) { fDecoderConfigFileName = FileName; }

    Int_t GetRunIdFromFile(TString name);
    TString GetSubNameAfterRunId(TString name);
    void* GetMapper(DetectorId id);

  private:
    const TTimeStamp TimeZero = TTimeStamp(time_t(0), 0);
    Double_t workTime_cpu = 0.0;
    Double_t workTime_real = 0.0;
    //    constexpr static const TPRegexp re_data = ".*\\w+_\\w+_\\w+_(\\d+)([^/]*)\\.data";
    //    constexpr static const TPRegexp re_raw = ".*\\w+_run(\\d+)([^/]*)_raw\\.root";

    std::map<DetectorId, bool> fDetectorSetup;
    pt::ptree conf;
    bool isSpillStart;
    UInt_t fSpillCntr;
    BmnEventType fCurEvType = kBMNPAYLOAD;    // for converter
    BmnEventType fPrevEvType = kBMNPAYLOAD;   // for converter
    bool fApplyThreshold;
    bool isTaskMode;
    bool isForwardMode;

    Int_t fTOF700ReferenceRun;
    Int_t fTOF700ReferenceChamber;

    UInt_t fRunId;
    UInt_t fPeriodId;
    UInt_t fEventId;
    ULong_t fNTotalEvents;
    ULong_t fNSignalEvents;
    UInt_t fStartEventId;
    // for event
    Long64_t fTime_s;
    Long64_t fTime_ns;
    // for run
    TTimeStamp fRunStartTime;
    TTimeStamp fRunEndTime;

    TTree* fRawTree;
    TTree* fRawTreeSpills;
    //    TTree* fRawTreeSpillsT0;
    TTree* fDigiTree;
    TTree* fDigiTreeSpills;
    string fDecoderConfigFileName;
    TString fRootFileName;
    TString fDigiFileName;
    TString fCalibFileName;
    TString fDigiRunHdrName;
    TString fDchMapFileName;
    TString fMwpcMapFileName;
    TString fGemMapFileName;
    TString fVspMapFileName;
    TString fTof400PlaceMapFileName;
    TString fTof400StripMapFileName;
    TString fTof701PlaceMapFileName;
    TString fTof701StripMapFileName;
    TString fTof700MapFileName;
    TString fTof700GeomFileName;
    TString fZDCMapFileName;
    TString fZDCCalibrationFileName;
    TString fScWallMapFileName;
    TString fScWallCalibrationFileName;
    TString fFHCalMapFileName;
    TString fFHCalCalibrationFileName;
    TString fHodoMapFileName;
    TString fHodoCalibrationFileName;
    TString fNdetMapFileName;
    TString fNdetCalibrationFileName;
    TString fHgndMapFileName;
    TString fHgndCalibrationFileName;
    TString fECALMapFileName;
    TString fECALCalibrationFileName;
    TString fMSCMapFileName;
    TString fSiliconMapFileName;
    TString fSiBTMapFileName;
    TString fCscMapFileName;
    TString fTrigPlaceMapFileName;
    TString fTrigChannelMapFileName;
    TString fSubName;
    TString fRawRunHdrName;
    TString fMetadataName;

    TFile* fRootFileIn;
    TFile* fRootFileOut;
    TFile* fDigiFileOut;
    TFile* fCalibFile;
    FILE* fRawFileIn;

    // DAQ arrays
    TClonesArray* sync;
    TClonesArray* adc32;    // gem & csc
    TClonesArray* adc64;    // SiBT
    TClonesArray* adc128;   // FSD
    TClonesArray* adc;      // zdc & ecal & scwall & fhcal
    TClonesArray* vsp_raw;
    TClonesArray* hrb;
    TClonesArray* tdc;
    TClonesArray* tqdc_tdc;
    TClonesArray* tqdc_adc;
    TClonesArray* tdc_hgnd;
    BmnEventHeader* eventHeaderDAQ;
    DigiRunHeader* rawRunHeader;
    BmnMetadataRaw* metadata;
    // Spill tree arrays
    TClonesArray* msc;
    TClonesArray* t0raw;

    // Digi arrays
    TClonesArray* silicon;
    TClonesArray* sibt;
    TClonesArray* gem;
    TClonesArray* csc;
    TClonesArray* vsp;
    TClonesArray* tof400;
    TClonesArray* tof700;
    TClonesArray* tof701;
    TClonesArray* zdc;
    TClonesArray* scwall;
    TClonesArray* fhcal;
    TClonesArray* hodo;
    TClonesArray* ndet;
    TClonesArray* hgnd;
    TClonesArray* ecal;
    TClonesArray* dch;
    TClonesArray* mwpc;
    TClonesArray* msc_copy;
    TClonesArray* t0_copy;

    // header array
    BmnEventHeader* eventHeader;
    BmnSpillHeader* spillHeader;

    Long64_t NRawTreeEvents;
    Long64_t NRawTreeSpills;
    Long64_t fMaxEvent;

    UInt_t fDat;   // current 32-bits word
    UInt_t syncCounter;
    BmnCscRaw2Digit* fCscMapper;
    BmnGemRaw2Digit* fGemMapper;
    BmnVspRaw2Digit* fVspMapper;
    BmnSiliconRaw2Digit* fSiliconMapper;
    BmnSiBTRaw2Digit* fSiBTMapper;
    BmnDchRaw2Digit* fDchMapper;
    BmnMwpcRaw2Digit* fMwpcMapper;
    BmnTrigRaw2Digit* fTrigMapper;
    BmnTof1Raw2Digit* fTof400Mapper;
    BmnTof2Raw2DigitNew* fTof700Mapper;
    BmnTof701Raw2Digit* fTof701Mapper;
    BmnZDCRaw2Digit* fZDCMapper;
    BmnScWallRaw2Digit* fScWallMapper;
    BmnFHCalRaw2Digit* fFHCalMapper;
    BmnHodoRaw2Digit* fHodoMapper;
    BmnNdetRaw2Digit* fNdetMapper;
    BmnHgndRaw2Digit* fHgndMapper;
    BmnECALRaw2Digit* fECALMapper;
    BmnMscRaw2Digit* fMSCMapper;

    BmnAdcQA* fFsdQa;
    BmnAdcQA* fGemQa;
    BmnAdcQA* fCscQa;

    UInt_t nSpillEvents;
    BmnEventType fCurEventType;    // for decoder
    BmnEventType fPrevEventType;   // for decoder
    BmnSetup fBmnSetup;
    UInt_t fPedEvCntrBySpill;
    UInt_t fPedEvCntr;
    UInt_t fNoiseEvCntr;
    UInt_t fEvForPedestals;
    UInt_t fEvForNoiseCorr;
    bool fPedEnough;
    bool fNoiseEnough;
    bool fUseCalibFile;
    bool fSaveCalibFile;
    bool fExportJsonBocks;
    bool fSaveOutput;
    //    GemMapValue* fGemMap;
    UInt_t fT0Serial;

    // Map to store pairs <Crate serial> - <crate time - T0 time>
    unordered_map<UInt_t, Long64_t> fTimeShifts;
    Long64_t fT0SyncTime;   // ns
    Double_t fT0Time;       // ns
    Double_t fT0Width;      // ns
    map<TTimeStamp, Int_t> leaps;
    TTimeStamp utc_valid;
    Int_t tai_utc_dif;
    Int_t fVerbose;
    map<TTimeStamp, BmnSpillInfo> fSpillMap;
    map<uint32_t, uint32_t> _dfl_run_id;

    int refrun_tof700_slewing[60];
    int refchamber_tof700_slewing[60];
    int type_tof700_slewing[60];

    template<typename BrType>
    void Assignment_Fun(const TString name, BrType** ob)
    {
        if (isTaskMode) {
            FairRootManager* frm = FairRootManager::Instance();
            *ob = static_cast<BrType*>(frm->GetObject(name));
            LOG(debug) << "Getting: " << name << "  p: " << ob;
        } else {
            TBranch* b = fRawTree->GetBranch(name);
            //            LOGF(debug, "branch dir: %p path: %s", (void*) b->GetDirectory(),
            //            b->GetDirectory()->GetPath());
            auto brAddr = b->GetAddress();
            if (brAddr) {
                LOG(debug) << "Assigning branch " << b << " name " << name << " addr " << (void*)brAddr;
                *ob = reinterpret_cast<BrType*>(*(reinterpret_cast<void**>(brAddr)));
            } else {
                fRawTree->SetBranchAddress(name, ob);
                LOG(debug) << "   Set    branch " << b << " name " << name << " addr " << (void*)*ob;
            }
        }
    };

    template<typename BrType>
    void Register_Fun(const TString name, BrType** ob)
    {
        if (isTaskMode) {
            FairRootManager* frm = FairRootManager::Instance();
            frm->Register(name, name + "_dir", *ob, fSaveOutput);
        } else {
            TBranch* b = fDigiTree->Branch(name, ob);
            LOGF(debug1, "Register branch %p : %s", (void*)b, name.Data());
        }
    };

    BmnStatus InitUTCShift();
    Int_t GetUTCShift(TTimeStamp t);
    BmnStatus GetT0Info(Double_t& t0time, Double_t& t0width);
    inline void FillWR(UInt_t iSerial, ULong64_t iEvent, Long64_t t_sec, Long64_t t_ns);
    BmnStatus FillTimeShiftsMap();
    BmnStatus LoadCalibFile(TString& FileName);
    BmnStatus SaveCalibFile(TString& FileName);

    void CopyDataToPedMap(UInt_t ev);
    BmnStatus LoadConfig();

    ClassDef(BmnDecoder, 2);
};

#endif
