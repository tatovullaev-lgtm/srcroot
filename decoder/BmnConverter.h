#ifndef BMNCONVERTER_H
#define BMNCONVERTER_H 1

// #define MULTITHREAD true

#include <bitset>
#include <fstream>
#include <map>
#include <vector>
// Boost
// #pragma GCC system_header
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
// Nlohmann
#include <nlohmann/json.hpp>
// ROOT
#include "TClonesArray.h"
#include "TFile.h"
#include "TPRegexp.h"
#include "TROOT.h"
#include "TString.h"
#include "TTimeStamp.h"
#include "TTree.h"

#include <ROOT/TBufferMerger.hxx>
// FairRoot
#include "FairRootManager.h"
#include "FairSource.h"
// BmnRoot
#include "BmnADCDigit.h"
#include "BmnDetectorList.h"
#include "BmnEventHeader.h"
#include "BmnGemStripDigit.h"
#include "BmnHRBDigit.h"
#include "BmnMSCDigit.h"
#include "BmnMath.h"
#include "BmnMetadataRaw.h"
#include "BmnSpillHeader.h"
#include "BmnSyncDigit.h"
#include "BmnTDCDigit.h"
#include "BmnTQDCADCDigit.h"
#include "BmnThreadManager.h"
#include "BmnTrigConfig.h"
#include "DigiRunHeader.h"
#include "RawTypes.h"

/********************************************************/
// wait limit for input data (ms)
#define WAIT_LIMIT 40000000

typedef pair<uint32_t, uint64_t> TreePos;
typedef pair<TTree*, uint64_t> TreePE;
typedef multimap<SysPoint, TreePE> MapTP2TreePE;

using std::map;
using std::vector;
namespace pt = boost::property_tree;

using json = nlohmann::json;

class BmnConverter
{   //: public FairSource {
  public:
    vector<pair<int, pair<int, int>>> vecTree;
    vector<pair<int, pair<int, int>>> vecTreeSpills;
    //    map<int32_t, multimap<SysPoint, TreePos>> treeMap; // spillId, time point, tree(thread) id, tree entry
    //    map<int32_t, multimap<SysPoint, TreePos>> treeMapSpills;
    map<int32_t, map<int32_t, TreePos>> treeMap;               // spillId, event Id, tree entry
    map<int32_t, multimap<SysPoint, TreePos>> treeMapSpills;   // spill Id, time point, tree entry
    multimap<SysPoint, TreePE> treeTimeMap;                    // time point, tree ptr, tree entry
    map<int32_t, MapTP2TreePE> treeTimeMapDiv;
    BmnThreadManager<BmnConverterThread>* fThreads;
    uint32_t fThreadCnt;
    BmnConverter(uint32_t threadCount = 0, uint32_t period = 8, TString outDir = ".", bool tempFileOnDisk = false);
    virtual ~BmnConverter() {}

    // Int_t ReadEvent(UInt_t i = 0);

    static BmnStatus ParseComplexTLV(UInt_t* buf, UInt_t& len, UInt_t& runId);
    BmnStatus ParseJsonTLV(UInt_t* buf, UInt_t& len);

    void OutputSpills();
    //    BmnStatus FeedSpill(TString name, Bool_t getRunId = kTRUE, Bool_t getSubName = kTRUE);
    BmnStatus FeedFile(TString name, Bool_t getRunId = kTRUE, Bool_t getSubName = kTRUE);
    BmnStatus FeedEvent(UInt_t* buf, UInt_t len, Int_t type);

    void SeparateEventsBySpills();
    void SeparateEventsBySpillsTM();
    void ReproduceBranches(TTree* inTree, TTree* outTree);

    void SetRunId(uint32_t v) { fRunId = v; }

    void SetPeriod(uint32_t v) { fPeriodId = v; }

    void SetSubName(TString v) { fSubName = v; }

    void SetSpillMap(map<SysPoint, BmnSpillInfo> spillMap) { fSpillMap = spillMap; }

    map<SysPoint, BmnSpillInfo> GetSpillMap() { return fSpillMap; }

    bool GetTempFileOnDisk() { return temp_file_on_disk; }

    void Close()
    {
        fThreads->Terminate();
        //        if (fRootFileOut && fRootFileOut->IsOpen())
        //            fRootFileOut->Close();
    }

    void SetDecoderConfigFileName(string FileName) { fDecoderConfigFileName = FileName; }

    void OutputTrees();

  private:
    const TTimeStamp TimeZero = TTimeStamp(time_t(0), 0);

    pt::ptree conf;
    bool temp_file_on_disk;
    uint32_t fPeriodId;
    uint32_t fRunId;
    BmnEventType fCurEvType;   // = kBMNPAYLOAD; // for converter
    // BmnEventType fPrevEvType = kBMNPAYLOAD; // for converter
    // bool isRawRootInputFile;
    FILE* fRawFileIn;
    TString fSubName;

    uint32_t fEventId;

    string fDecoderConfigFileName;
    TString fRawRunHdrName;
    TString fMetadataName;
    TString fRawTreeName;
    TString fRawTreeSpillName;
    TString fRootFileName;
    TString fRootDirPath;

    //    TFile* fRootFileOut;

    // header array
    BmnSpillHeader* spillHeader;
    // Raw run header
    unique_ptr<DigiRunHeader> runHeader;
    unique_ptr<BmnMetadataRaw> metadata;
    unique_ptr<ROOT::TBufferMerger> fMerger;

    vector<TObject*> fNamVec;
    vector<TClonesArray*> fArrVec;

    uint32_t data[10000000];
    // ULong_t fMaxEvent;

    uint32_t fDat;   // current 32-bits word
    // UInt_t syncCounter;

    // UInt_t nSpillEvents;
    Bool_t fExportJsonBlocks;

    map<TTimeStamp, Int_t> leaps;
    TTimeStamp utc_valid;
    Int_t tai_utc_dif;
    Int_t cc = 0;
    map<SysPoint, BmnSpillInfo> fSpillMap;
    BmnConverterThread* fCurWaitingThread;
    // Bool_t fSaveOutput;

    Int_t GetSpillNumber(vector<SysPoint>* spillEnds, vector<SysPoint>* spillStarts, SysPoint time);
    Int_t GetSpillNumber(vector<SysPoint>* spillEnds, vector<SysPoint>* spillStarts, TTimeStamp time);
    BmnStatus ConvertRawToRootIterateFileRead();
    void QuickProcessEvent(UInt_t* d, UInt_t& len);
    void QuickConvertStatEvent(UInt_t* buf, UInt_t& len);
    /**
     * QuickProcessEvent reduced version in order to skip additional memcpy
     *  and read raw data directly into the thread's buffer
     * @param tmp thread pointer
     * @param len data length
     */
    void QuickProcessEvent(BmnConverterThread*& tmp, UInt_t& len);
    void QuickConvertStatEvent(BmnConverterThread*& tmp, UInt_t& len);
    BmnStatus LoadConfig();
    BmnStatus CreateTempFile();

    BmnStatus InitUTCShift();
    Int_t GetUTCShift(TTimeStamp t);

    ClassDef(BmnConverter, 1);
};

#endif
