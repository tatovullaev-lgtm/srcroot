#ifndef BMNPILEUPREDUCTIONTASK_H
#define BMNPILEUPREDUCTIONTASK_H

#include "FairTask.h"

class BmnTrigWaveDigit;
class BmnEventHeader;
class FairRootManager;
class TClonesArray;
class TFile;
class TH2F;
class THnSparse;

namespace BmnEventClass
{
enum id
{
    kNull = 0,
    k0,
    k1,
    kV0,
    kV1,
    k00,
    k01,
    k0V0,
    k0V1,
    k10,
    k11,
    k1V0,
    k1V1,
    kMult,
    kUndef,
    kNclasses
};
const std::map<int, std::string> names{
    {kNull, "Null"}, {k0, "0"},     {k1, "1"},     {kV0, "V0"},     {kV1, "V1"},
    {k00, "00"},     {k01, "01"},   {k0V0, "0V0"}, {k0V1, "0V1"},   {k10, "10"},
    {k11, "11"},     {k1V0, "1V0"}, {k1V1, "1V1"}, {kMult, "Mult"}, {kUndef, "Undef"},
};
}   // namespace BmnEventClass

namespace BmnTriggerBits
{
enum triggerBits
{
    kBC1L = 0,
    kpBT,
    kBT,
    kNiT,
    kCCT1,
    kMBT,
    kBTnBUSY,
    kCCT2,
    kNtriggerBits
};
}   // namespace BmnTriggerBits

class BmnBC1hitInfo : public TNamed
{
    friend class BmnEventSelector;

  public:
    BmnBC1hitInfo() {};
    virtual ~BmnBC1hitInfo() {};
    std::vector<float> GetClosestBC1hitsDt(BmnEventClass::id classId)
    {
        std::vector<float> dt{-1.e10, 1.e10};
        for (int i = fCentralHitIndexBC1S - 1; i >= 0; i--) {
            if (fBC1hitClasses.at(i) == classId) {
                dt.at(0) = fRelTimesBC1S.at(i);
                break;
            }
        }
        for (int i = fCentralHitIndexBC1S + 1; i < int(fTimesBC1S.size()); i++) {
            if (fBC1hitClasses.at(i) == classId) {
                dt.at(1) = fRelTimesBC1S.at(i);
                break;
            }
        }
        return dt;
    }

    std::vector<double> GetTimesBC1S() { return fTimesBC1S; }
    std::vector<double> GetRelTimesBC1S() { return fRelTimesBC1S; }
    std::vector<int> GetBC1hitClasses() { return fBC1hitClasses; }
    std::vector<int> GetBC1Speaks() { return fBC1Speaks; }
    std::vector<int> GetBC2ASpeaks() { return fBC2ASpeaks; }
    std::vector<int> GetVCSpeaks() { return fVCSpeaks; }
    std::vector<int> GetFDpeaks() { return fFDpeaks; }
    std::vector<int> GetNdigitsTOF400() { return fNdigitsTOF400; }
    std::vector<int> GetNdigitsTOF700() { return fNdigitsTOF700; }
    std::vector<int> GetNdigitsBD() { return fNdigitsBD; }
    std::vector<float> GetSumAmpBD() { return fSumAmpBD; }
    int GetCentralHitIndexBC1S() { return fCentralHitIndexBC1S; }
    BmnEventClass::id GetEventClass() { return fEventClass; }

  private:
    std::vector<double> fTimesBC1S;
    std::vector<double> fRelTimesBC1S;
    std::vector<int> fBC1hitClasses;
    std::vector<int> fBC1Speaks;
    std::vector<int> fBC2ASpeaks;
    std::vector<int> fVCSpeaks;
    std::vector<int> fFDpeaks;
    std::vector<int> fNdigitsTOF400;
    std::vector<int> fNdigitsTOF700;
    std::vector<int> fNdigitsBD;
    std::vector<float> fSumAmpBD;
    int fCentralHitIndexBC1S;
    BmnEventClass::id fEventClass;

    ClassDef(BmnBC1hitInfo, 2);
};

class BmnEventSelector : public FairTask
{
  public:
    /** Default constructor **/
    BmnEventSelector();

    /** Constructor with parameters (Optional) **/
    //  BmnEventSelector(Int_t verbose);

    /** Destructor **/
    ~BmnEventSelector();

    void SetRunId(int runId) { fRunId = runId; }
    void SetOutputFileName(const char* name) { fOutputFileName = name; }
    void SetInputFileName(const char* name) { fInputFileName = name; }

    /** Initiliazation of task at the beginning of a run **/
    virtual InitStatus Init();

    void InitHistograms();

    /** ReInitiliazation of task when the runID changes **/
    virtual InitStatus ReInit();

    /** Executed for each event. **/
    virtual void Exec(Option_t* opt);

    /** Load the parameter container from the runtime database **/
    virtual void SetParContainers();

    void Calculate();

    BmnEventClass::id Classify();

    int GetClosestHitIndex(std::vector<double> hitTimes, float refTime);

    /** Finish task called at the end of the run **/
    virtual void Finish();
    void WriteBC1hitInfo(bool write = true) { fWriteBC1hitInfo = write; }
    void FillCalibrationHists(bool fill = true) { fFillCalibrationHists = fill; }
    void FillPerformanceHists(bool fill = true) { fFillPerformanceHists = fill; }

  private:
    BmnEventSelector(const BmnEventSelector&);
    BmnEventSelector operator=(const BmnEventSelector&);
    template<typename T>
    T ReadBranch(const char* name);
    template<typename T>
    T AddHist(T h)
    {
        fOutputList->Add(h);
        return h;
    }
    template<typename T>
    T AddHistClone(T h, const char* postfix = "")
    {
        auto hClone = (T)h->Clone(Form("%s%s", h->GetName(), postfix));
        hClone->SetTitle(Form("%s%s", h->GetTitle(), postfix));
        fOutputList->Add(hClone);
        return hClone;
    }
    void FillHistograms();
    bool FitRunDistr(std::string histName, std::vector<double>& fitPar, float fitMin = 0, float fitMax = 0);
    int GetPeak(BmnTrigWaveDigit* digit, float timeMin = 0, float timeMax = 1e9);

    std::vector<int> GetPeaks(BmnTrigWaveDigit* digit,
                              std::vector<double> hitTimes,
                              float relTimeMin,
                              float relTimeMax,
                              float relHitTimeMin = -1,
                              float relHitTimeMax = 1);

    template<typename T>
    std::vector<int> GetNdigits(TClonesArray* array, std::vector<double> hitTimes, float relTimeMin, float relTimeMax);
    template<typename T>
    std::vector<float> GetSumAmp(TClonesArray* digits,
                                 std::vector<double> hitTimes,
                                 float relTimeMin,
                                 float relTimeMax);
    FairRootManager* ioman;
    BmnEventHeader* fEventHeader;
    TClonesArray* fArrTQDC_BC1S;
    TClonesArray* fArrTQDC_BC2AS;
    TClonesArray* fArrTQDC_VCS;
    TClonesArray* fArrTQDC_FD;
    TClonesArray* fArrBDdigits;
    TClonesArray* fArrTOF400digits;
    TClonesArray* fArrTOF700digits;
    TClonesArray* fArrSILICONdigits;
    TClonesArray* fArrGEMdigits;
    BmnTrigWaveDigit* fTQDC_BC1S;
    BmnTrigWaveDigit* fTQDC_VCS;
    BmnTrigWaveDigit* fTQDC_BC2AS;
    BmnTrigWaveDigit* fTQDC_FD;
    BmnEventClass::id fEventClass = BmnEventClass::kUndef;
    BmnEventClass::id fEventClassOut;
    TList* fOutputList;
    int fRunId = 0;
    std::string fOutputFileName = "es.root";
    std::string fInputFileName = "all.root";
    TFile* fInputFile;
    TFile* fOutputFile;
    TH2F* h2runId_eventClass;
    TH2F* h2runId_time_SingleBC1S;
    TH2F* h2runId_time_BC1S_k0;
    TH2F* h2runId_time_BC1S_k1;
    TH2F* h2runId_time_BC1S_kV0;
    TH2F* h2runId_time_BC1S_kV1;
    TH2F* h2runId_time_SingleVCS_BC1S;
    TH2F* h2runId_time_SingleBC2AS_BC1S;
    TH2F* h2runId_time_SingleFD_BC1S;
    TH2F* h2runId_shapeShifted_SingleBC1S;
    TH2F* h2runId_shapeShiftedBC1S_SingleVCS;
    TH2F* h2runId_shapeShiftedBC1S_SingleBC2AS;
    TH2F* h2runId_shapeShiftedBC1S_SingleFD;
    TH2F* h2runId_shapeShiftedBC1S_BD;
    TH2F* h2runId_shapeShiftedBC1S_TOF400digit;
    TH2F* h2runId_shapeShiftedBC1S_TOF700digit;
    TH2F* h2runId_peak_SingleBC1S;
    TH2F* h2runId_peak_SingleVCS;
    TH2F* h2runId_peak_SingleBC2AS;
    TH2F* h2runId_peak_SingleFD;
    TH2F* h2runId_NdigitsBD;
    TH2F* h2runId_NdigitsBD_SingleBC1S;
    TH2F* h2runId_NdigitsBD_nc;
    TH2F* h2runId_SumAmpBD;
    TH2F* h2runId_SumAmpBD_SingleBC1S;
    TH2F* h2runId_SumAmpBD_nc;
    TH2F* h2runId_NdigitsTOF400;
    TH2F* h2runId_NdigitsTOF400_SingleBC1S;
    TH2F* h2runId_NdigitsTOF400_nc;
    TH2F* h2runId_NdigitsTOF700;
    TH2F* h2runId_NdigitsTOF700_SingleBC1S;
    TH2F* h2runId_NdigitsTOF700_nc;
    TH2F* h2runId_peak_BC1S;
    TH2F* h2runId_peak_VCS;
    TH2F* h2runId_peak_BC2AS;
    TH2F* h2runId_peak_FD;
    TH2F* h2runId_peak_FD_nc;

    THnSparse* h5runId_trigger_eventClass_dt_nTOF400digits;
    THnSparse* h5runId_trigger_eventClass_dt_nTOF700digits;
    THnSparse* h5runId_trigger_eventClass_dt_nSILICONdigits;
    THnSparse* h5runId_trigger_eventClass_dt_nGEMdigits;

    int fNchannelsTQDC = 450;
    int fNhitsBC1S;
    int fNhitsVCS;
    int fNhitsBC2AS;
    int fNhitsFD;
    std::vector<double> fTimesBC1S;
    std::vector<double> fRelTimesBC1S;
    std::vector<double> fTimesVCS;
    std::vector<double> fTimesBC2AS;
    std::vector<double> fTimesFD;
    float fSingleBC1hitTimeMean = 2050;
    float fBC1SpeakTimeMin = 0;
    float fBC1SpeakTimeMax = 80;
    float fSingleVCS_BC1S_hitTimeMin = -5;
    float fSingleVCS_BC1S_hitTimeMax = 5;
    float fVCSpeakTimeMin = 0;
    float fVCSpeakTimeMax = 300;
    float fSingleVCSpeakMin = 500;
    float fSingleBC2AS_BC1S_hitTimeMin = -8.5;
    float fSingleBC2AS_BC1S_hitTimeMax = -7;
    float fBC2ASpeakTimeMin = 0;
    float fBC2ASpeakTimeMax = 300;
    float fSingleBC2ASpeakMin = 1200;
    float fSingleFD_BC1S_hitTimeMin = 110;
    float fSingleFD_BC1S_hitTimeMax = 160;
    float fFDpeakTimeMin = 0;
    float fFDpeakTimeMax = 250;
    float fSingleFDpeakMin = 1;
    float fBDtimeMin = -125;
    float fBDtimeMax = -117;
    float fTOF400digitTimeMin = -670;
    float fTOF400digitTimeMax = -640;
    float fTOF700digitTimeMin = -2080;
    float fTOF700digitTimeMax = -2030;
    int fNsPerChannel = 8;
    int fCentralHitIndexBC1S;
    float fCentralHitTimeBC1S;
    int fRunMin{6667};
    int fRunMax{8428};
    unsigned int fTrigAR;
    unsigned int fNdigitsSILICON{0};
    unsigned int fNdigitsGEM{0};
    std::vector<int> fBC1Speaks;
    std::vector<int> fVCSpeaks;
    std::vector<int> fBC2ASpeaks;
    std::vector<int> fFDpeaks;
    std::vector<int> fNdigitsBD;
    std::vector<float> fSumAmpBD;
    std::vector<int> fNdigitsTOF400;
    std::vector<int> fNdigitsTOF700;
    std::vector<int> fBC1hitClasses;
    bool fNoCalibrationData = false;
    bool fWriteBC1hitInfo = true;
    bool fFillCalibrationHists = false;
    bool fFillPerformanceHists = false;
    BmnBC1hitInfo fBC1hitInfo;
    int iEvent = -1;

    ClassDef(BmnEventSelector, 2);
};

#endif
