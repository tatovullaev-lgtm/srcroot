#ifndef BMNMETADATADECODER_H
#define BMNMETADATADECODER_H 1

#include <fstream>
#include <map>
#include <vector>
// Boost
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
// ROOT
#include <TChain.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TPRegexp.h>
#include <TString.h>
#include <TTimeStamp.h>
#include <TTree.h>
// BmnRoot
#include "BmnEventHeader.h"
#include "BmnMetadataRaw.h"
#include "BmnSpillHeader.h"
#include "BmnTrigRaw2Digit.h"
#include "DigiRunHeader.h"

using std::map;
using std::vector;
namespace pt = boost::property_tree;

class BmnMetadataDecoder : public TObject
{
  public:
    BmnMetadataDecoder(vector<string> file = {}, string outfile = "", ULong_t period = 8, BmnSetup setup = kBMNSETUP);
    virtual ~BmnMetadataDecoder();

    void ClearDigiArrays();
    void ClearSpillDigiArrays();
    BmnStatus ProcessStatusBlocks();
    BmnStatus ProcessSpillTree();
    void GatherStatusBlocksFromDigis();
    void GatherStatusBlocksFromTars();
    BmnStatus DecodeDataToDigiIterate();
    BmnStatus InitDecoder();
    void ResetDecoder(TString file);
    BmnStatus SaveData();
    BmnStatus DisposeDecoder();

    void SetRawTree(TTree* tree) { fRawTree = tree; }

    TTree* GetDigiTree() { return fDigiTree; }

    void SetRunId(UInt_t v) { fRunId = v; }

    void SetPeriodId(UInt_t v) { fPeriodId = v; }

    UInt_t GetRunId() const { return fRunId; }

    UInt_t GetPeriodId() const { return fPeriodId; }

    void SetBmnSetup(BmnSetup v) { fBmnSetup = v; }

    BmnSetup GetBmnSetup() const { return fBmnSetup; }

    void SetMSCMapping(TString map) { fMSCMapFileName = map; }

    void SetRawRootFile(TString filename) { fRootFileName = filename; }

    void SetOutFileName(string filename) { fOutFileName = filename; }

    void SetDecoderConfigFileName(string FileName) { fDecoderConfigFileName = FileName; }

    BmnStatus SetDigiDirectory(string fDigiDir);

    BmnStatus SetTarDirectory(string DirName);

    void SetTarFileNames(vector<string> FileNames) { fTarFileNames = FileNames; }

    TString GetSubNameAfterRunId(TString name);

  private:
    const TTimeStamp TimeZero = TTimeStamp(time_t(0), 0);
    Double_t workTime_cpu = 0.0;
    Double_t workTime_real = 0.0;
    bool get_run_interval_from_file = true;

    pt::ptree conf;

    UInt_t fRunId;
    UInt_t fPeriodId;
    // for event
    Long64_t fTime_s;
    Long64_t fTime_ns;
    // for run
    SysPoint fRunStartTime;
    SysPoint fRunEndTime;

    TTree* fRawTree;
    //    TTree* fRawTreeSpills;
    TTree* fRawTreeSpillsT0;
    TTree* fDigiTree;
    TTree* fDigiTreeSpills;
    string fDecoderConfigFileName;
    string fDigiDir;
    vector<string> fDigiFileNames;
    string fTarDir;
    vector<string> fTarFileNames;
    TString fMSCMapFileName;
    TString fRootFileName;
    string fOutFileName;
    TString fDigiRunHdrName;
    TString fSubName;
    TString fRawRunHdrName;
    TString fMetadataName;

    TFile* fDigiFileOut;
    TFile* fCalibFile;

    unique_ptr<DigiRunHeader> digiRunHeader;
    //    BmnMetadataRaw* metadata;
    // Spill tree arrays
    TClonesArray* msc;
    TClonesArray* t0raw;

    // Digi arrays
    TClonesArray* msc_copy;
    TClonesArray* t0_copy;

    // header array
    BmnEventHeader* inEventHeader;
    BmnSpillHeader* spillHeader;

    /** TRC bit map */
    NameChMap fTrcBitMap;
    /** MSC bit map */
    Name2SerCh fScalersMap;
    /** Raw T0 module map (from Sergeev txt) */
    T0ChMap fRawT0Map;
    BmnMetadataRaw metadata;

    BmnSetup fBmnSetup;
    TString fPeriodSetupExt;
    unique_ptr<BmnTrigRaw2Digit> fTrigMapper;

    map<SysPoint, BmnSpillInfo> fSpillMap;
    BmnStatus LoadConfig();

    ClassDef(BmnMetadataDecoder, 1);
};

#endif
