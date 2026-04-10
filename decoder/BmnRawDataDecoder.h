#ifndef BMNRAWDATADECODER_H
#define BMNRAWDATADECODER_H 1
// STL
#include <deque>
#include <filesystem>
#include <fstream>
#include <map>
#include <vector>
// Boost
#pragma GCC system_header
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
// ROOT
#include "TClonesArray.h"
#include "TFile.h"
#include "TString.h"
#include "TTimeStamp.h"
#include "TTree.h"
// BmnRoot
#include "BmnAdcQA.h"
#include "BmnConverter.h"
#include "BmnCscRaw2Digit.h"
#include "BmnDchRaw2Digit.h"
#include "BmnDecoder.h"
#include "BmnECALRaw2Digit.h"
#include "BmnEventHeader.h"
#include "BmnFHCalRaw2Digit.h"
#include "BmnGemRaw2Digit.h"
#include "BmnHodoRaw2Digit.h"
#include "BmnMscRaw2Digit.h"
#include "BmnMwpcRaw2Digit.h"
#include "BmnNdetRaw2Digit.h"
#include "BmnRawSource.h"
#include "BmnScWallRaw2Digit.h"
#include "BmnSiBTRaw2Digit.h"
#include "BmnSiliconRaw2Digit.h"
#include "BmnTof1Raw2Digit.h"
// #include "BmnTof2Raw2DigitNew.h"
#include "BmnTof701Raw2Digit.h"
#include "BmnTrigRaw2Digit.h"
#include "BmnZDCRaw2Digit.h"
#include "DigiArrays.h"
#include "RawTypes.h"

/********************************************************/
// wait limit for input data (ms)
#define WAIT_LIMIT 40000000
// Debug ADC processor
// #define ADC_QA true

using std::map;
using std::vector;
namespace pt = boost::property_tree;

using json = nlohmann::json;

class BmnRawDataDecoder
{
  public:
    static double correctionGem, correctionCsc, correctionSil;
    static double threshGem, threshCsc, threshSil;
    static int samplesGem, samplesCsc, samplesSil;

    BmnRawDataDecoder(TString file = "", TString outfile = "", ULong_t nEvents = 0, ULong_t period = 8);
    virtual ~BmnRawDataDecoder();

    BmnStatus ConvertStatEvent(UInt_t* buf, UInt_t& len);
    BmnStatus ConvertRawToRoot();
    void FillAdcQa();
    void SaveAdcQa();
    void FillProfiles();
    void FillNoisyChannels();
    BmnStatus DecodeDataToDigi();
    void CreateConverter();
    void CreateDecoder();

    BmnStatus InitConverter(TString FileName);
    BmnStatus InitConverter();
    BmnStatus InitDecoder();
    void ConnectForward();

    BmnStatus ConvertRawToRootIterate(UInt_t* buf, UInt_t len);
    BmnStatus DecodeDataToDigiIterate();

    void SetRunId(UInt_t v)
    {
        fRunId = v;
        if (fRawSource)
            fRawSource->SetRunId(fRunId);
        if (fDecoder)
            fDecoder->SetRunId(fRunId);
    }

    void SetPeriodId(UInt_t v)
    {
        fPeriodId = v;
        if (fRawSource)
            fRawSource->SetPeriodId(fPeriodId);
        if (fDecoder)
            fDecoder->SetPeriodId(fPeriodId);
    }

    void SetApplyThreshold(bool b) { fApplyThreshold = b; }

    //    map<UInt_t, Long64_t> GetTimeShifts() {
    //        return fTimeShifts;
    //    }

    UInt_t GetRunId() const { return fRunId; }

    UInt_t GetPeriodId() const { return fPeriodId; }

    UInt_t GetNevents() const { return fNTotalEvents; }

    UInt_t GetEventId() const
    {
        if (fDecoder)
            return fDecoder->GetEventId();
        if (fRawSource)
            return fRawSource->GetEventId();
    }

    TTimeStamp GetRunStartTime() const
    {
        if (fDecoder)
            return fDecoder->GetRunStartTime();
        if (fRawSource)
            return fRawSource->GetRunStartTime();
    }

    TTimeStamp GetRunEndTime() const
    {
        if (fDecoder)
            return fDecoder->GetRunEndTime();
        if (fRawSource)
            return fRawSource->GetRunEndTime();
    }

    TString GetRootFileName() { return fRootFileName; }

    void SetDetectorSetup(std::map<DetectorId, bool> setup)
    {
        fDetectorSetup = setup;
        if (fDecoder)
            fDecoder->SetDetectorSetup(fDetectorSetup);
    }

    void SetDecoderConfigFileName(string FileName)
    {
        fDecoderConfigFileName = FileName;
        if (fRawSource)
            fRawSource->SetDecoderConfigFileName(fDecoderConfigFileName);
        if (fDecoder)
            fDecoder->SetDecoderConfigFileName(fDecoderConfigFileName);
    }

    void SetEvForPedestals(UInt_t v)
    {
        fEvForPedestals = v;
        if (fDecoder)
            fDecoder->SetEvForPedestals(fEvForPedestals);
    }

    UInt_t GetEvForPedestals() { return fEvForPedestals; }

    void SetBmnSetup(BmnSetup v) { fBmnSetup = v; }

    BmnSetup GetBmnSetup() const { return fBmnSetup; }

    void SetVerbose(Int_t v) { fVerbose = v; }

    Int_t GetVerbose() const { return fVerbose; }

    void SetUseCalibFile(bool v) { fUseCalibFile = v; }

    bool GetUseCalibFile() const { return fUseCalibFile; }

    void SetRawRootDirPath(TString path)
    {
        fRootDirPath = path;
        fRootFileName = Form("%sbmn_run%d%s_raw.root", fRootDirPath.Data(), fRunId, fSubName.Data());
    }

    void SetRawRootFile(TString filename) { fRootFileName = filename; }

    void SetDigiRootFile(TString filename) { fDigiFileName = filename; }

    static Int_t GetRunIdFromFile(TString name);
    static Bool_t IsRootFile(TString name);
    static Bool_t IsRawFile(TString name);

    BmnRawSource* GetRawConverter() const { return fRawSource; }

    BmnDecoder* GetDecoder() const { return fDecoder; }

    void SetTaskMode(bool v = true)
    {
        isTaskMode = v;
        if (fRawSource)
            fRawSource->SetTaskMode(isTaskMode);
        if (fDecoder)
            fDecoder->SetTaskMode(isTaskMode);
    }

    bool GetTaskMode() const { return isTaskMode; }

    /***
     Set forward mode (without intermediate raw root file)
     */
    void SetForwardMode(bool v = true)
    {
        isForwardMode = v;
        if (fRawSource)
            fRawSource->SetForwardMode(isForwardMode);
        if (fDecoder)
            fDecoder->SetForwardMode(isForwardMode);
    }

    bool GetForwardMode() const { return isForwardMode; }

  private:
    BmnRawSource* fRawSource;
    BmnDecoder* fDecoder;

    Bool_t isTaskMode;
    Bool_t isForwardMode;

    const TTimeStamp TimeZero = TTimeStamp(time_t(0), 0);
    Double_t workTime_cpu = 0.0;
    Double_t workTime_real = 0.0;

    std::map<DetectorId, bool> fDetectorSetup;
    bool isRawRootInputFile;
    bool fApplyThreshold;

    UInt_t fRunId;
    UInt_t fPeriodId;
    //    UInt_t fEventId;
    UInt_t fNTotalEvents;
    UInt_t fNSignalEvents;
    UInt_t fStartEventId;

    string fDecoderConfigFileName;
    TString fRootFileName;
    TString fRootDirPath;
    TString fDigiDirPath;
    TString fRawFileName;
    TString fDigiFileName;
    TString fCalibFileName;
    TString fSubName;

    ULong_t fMaxEvent;

    UInt_t fDat;   // current 32-bits word
    UInt_t syncCounter;
    BmnSetup fBmnSetup;
    UInt_t fPedEvCntrBySpill;
    UInt_t fPedEvCntr;
    UInt_t fNoiseEvCntr;
    Int_t fEvForPedestals;
    Bool_t fPedEnough;
    Bool_t fNoiseEnough;
    Bool_t fUseCalibFile;
    UInt_t fT0Serial;

    Int_t fVerbose;
};

#endif
