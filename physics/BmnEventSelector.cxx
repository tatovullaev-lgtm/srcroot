#include "BmnEventSelector.h"

#include "BmnEventHeader.h"
#include "BmnStripDigit.h"
#include "BmnTof1Digit.h"
#include "BmnTrigDigit.h"
#include "BmnTrigWaveDigit.h"
#include "FairRootManager.h"

#include <TClonesArray.h>
#include <TF1.h>
#include <TFile.h>
#include <TH2F.h>
#include <THnSparse.h>
#include <TProfile.h>
#include <numeric>
#include <string>
#include <vector>

// ---- Default constructor -------------------------------------------
BmnEventSelector::BmnEventSelector()
    : FairTask("BmnEventSelector")
{
    LOG(debug) << "Default Constructor of BmnEventSelector";
}

// ---- Destructor ----------------------------------------------------
BmnEventSelector::~BmnEventSelector()
{
    LOG(debug) << "Destructor of BmnEventSelector";
}

// ----  Initialisation  ----------------------------------------------
void BmnEventSelector::SetParContainers()
{
    LOG(debug) << "SetParContainers of BmnEventSelector";
    // Load all necessary parameter containers from the runtime data base
    /*
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();

    <BmnEventSelectorDataMember> = (<ClassPointer>*)
    (rtdb->getContainer("<ContainerName>"));
    */
}

template<typename T>
T BmnEventSelector::ReadBranch(const char* name)
{
    auto branch = (T)ioman->GetObject(name);
    if (!branch)
        LOG(fatal) << Form("BmnEventSelector: %s branch not found!!!\n", name);
    return branch;
}

bool BmnEventSelector::FitRunDistr(std::string histName, std::vector<double>& fitPar, float fitMin, float fitMax)
{
    cout << endl;
    LOG(info) << __func__ << ": " << histName;
    fitPar.clear();
    auto h2 = static_cast<TH2*>(fInputFile->Get(histName.c_str()));
    if (!h2) {
        fNoCalibrationData = true;
        LOG(error) << "No input histogram: " << histName;
        return false;
    }
    auto runBin = h2->GetXaxis()->FindBin(fRunId);
    auto h1 = h2->ProjectionY(Form("%s_fit", h2->GetName()), runBin, runBin);
    if (!h1 || h1->GetEntries() == 0) {
        fNoCalibrationData = true;
        LOG(error) << "No input histogram " << histName << " for run number " << fRunId;
    }
    h1->Fit("gaus", "", "", fitMin, fitMax);
    auto f = (TF1*)h1->GetListOfFunctions()->FindObject("gaus");
    if (!f) {
        fNoCalibrationData = true;
        LOG(error) << "ERROR fitting distribution for run number " << fRunId;
        return false;
    }
    if (fFillCalibrationHists)
        fOutputList->Add(h1);
    fitPar = {f->GetParameter(0), f->GetParameter(1), f->GetParameter(2)};
    return true;
}

// ---- Init ----------------------------------------------------------
InitStatus BmnEventSelector::Init()
{
    LOG(debug) << "Initilization of BmnEventSelector";

    if (fFillCalibrationHists) {
        fOutputFile = new TFile(fOutputFileName.c_str(), "recreate");
        fOutputList = new TList;
        InitHistograms();
    }

    fInputFile = new TFile(fInputFileName.c_str());
    if (fInputFile->IsOpen()) {
        vector<double> fitPar;
        if (FitRunDistr("h2runId_time_SingleBC1S", fitPar, 2000, 2500))
            fSingleBC1hitTimeMean = fitPar.at(1);
        LOG(info) << "fSingleBC1hitTimeMean=" << fSingleBC1hitTimeMean;

        if (FitRunDistr("h2runId_shapeShifted_SingleBC1S", fitPar, 0, 100)) {
            fBC1SpeakTimeMin = fitPar.at(1) - 2 * fitPar.at(2);
            fBC1SpeakTimeMax = fitPar.at(1) + 2 * fitPar.at(2);
        }
        LOG(info) << "fBC1SpeakTimeMin=" << fBC1SpeakTimeMin << "\tfBC1SpeakTimeMax=" << fBC1SpeakTimeMax;

        if (FitRunDistr("h2runId_time_SingleVCS_BC1S", fitPar, -10, 10)) {
            fSingleVCS_BC1S_hitTimeMin = fitPar.at(1) - 10;
            fSingleVCS_BC1S_hitTimeMax = fitPar.at(1) + 10;
        }
        LOG(info) << "fSingleVCS_BC1S_hitTimeMin=" << fSingleVCS_BC1S_hitTimeMin
                  << "\tfSingleVCS_BC1S_hitTimeMax=" << fSingleVCS_BC1S_hitTimeMax;

        if (FitRunDistr("h2runId_shapeShiftedBC1S_SingleVCS", fitPar, 0, 80)) {
            fVCSpeakTimeMin = fitPar.at(1) - 2 * fitPar.at(2);
            fVCSpeakTimeMax = fitPar.at(1) + 2 * fitPar.at(2);
        }
        LOG(info) << "fVCSpeakTimeMin=" << fVCSpeakTimeMin << "\tfVCSpeakTimeMax=" << fVCSpeakTimeMax;

        if (FitRunDistr("h2runId_time_SingleBC2AS_BC1S", fitPar, -20, 0)) {
            fSingleBC2AS_BC1S_hitTimeMin = fitPar.at(1) - 2;
            fSingleBC2AS_BC1S_hitTimeMax = fitPar.at(1) + 2;
        }
        LOG(info) << "fSingleBC2AS_BC1S_hitTimeMin=" << fSingleBC2AS_BC1S_hitTimeMin
                  << "\tfSingleBC2AS_BC1S_hitTimeMax=" << fSingleBC2AS_BC1S_hitTimeMax;

        if (FitRunDistr("h2runId_shapeShiftedBC1S_SingleBC2AS", fitPar, 0, 80)) {
            fBC2ASpeakTimeMin = fitPar.at(1) - 2 * fitPar.at(2);
            fBC2ASpeakTimeMax = fitPar.at(1) + 2 * fitPar.at(2);
        }
        LOG(info) << "fBC2ASpeakTimeMin=" << fBC2ASpeakTimeMin << "\tfBC2ASpeakTimeMax=" << fBC2ASpeakTimeMax;

        if (FitRunDistr("h2runId_peak_BC2AS", fitPar, 0., 1.e4))
            fSingleBC2ASpeakMin = fitPar.at(1) - 3 * fitPar.at(2);
        LOG(info) << "\tfSingleBC2ASpeakMin=" << fSingleBC2ASpeakMin;

        if (FitRunDistr("h2runId_time_SingleFD_BC1S", fitPar, 100, 200)) {
            fSingleFD_BC1S_hitTimeMin = fitPar.at(1) - 10;
            fSingleFD_BC1S_hitTimeMax = fitPar.at(1) + 20;
        }
        LOG(info) << "fSingleFD_BC1S_hitTimeMin=" << fSingleFD_BC1S_hitTimeMin
                  << "\tfSingleFD_BC1S_hitTimeMax=" << fSingleFD_BC1S_hitTimeMax;

        if (FitRunDistr("h2runId_shapeShiftedBC1S_SingleFD", fitPar, 150, 250)) {
            fFDpeakTimeMin = fitPar.at(1) - 2 * fitPar.at(2);
            fFDpeakTimeMax = fitPar.at(1) + 2 * fitPar.at(2);
        }
        LOG(info) << "fFDpeakTimeMin=" << fFDpeakTimeMin << "\tfFDpeakTimeMax=" << fFDpeakTimeMax;

        if (FitRunDistr("h2runId_peak_FD_nc", fitPar, 4.e3, 7.e3))
            fSingleFDpeakMin = fitPar.at(1) - 3 * fitPar.at(2);
        LOG(info) << "\tfSingleFDpeakMin=" << fSingleFDpeakMin;

        if (FitRunDistr("h2runId_shapeShiftedBC1S_BD", fitPar, -150, -50)) {
            fBDtimeMin = fitPar.at(1) - 3 * fitPar.at(2);
            fBDtimeMax = fitPar.at(1) + 3 * fitPar.at(2);
        }
        LOG(info) << "fBDtimeMin=" << fBDtimeMin << "\tfBDtimeMax=" << fBDtimeMax;

        if (FitRunDistr("h2runId_shapeShiftedBC1S_TOF400digit", fitPar)) {
            fTOF400digitTimeMin = fitPar.at(1) - 3 * fitPar.at(2);
            fTOF400digitTimeMax = fitPar.at(1) + 3 * fitPar.at(2);
        }
        LOG(info) << "fTOF400digitTimeMin=" << fTOF400digitTimeMin << "\tfTOF400digitTimeMax=" << fTOF400digitTimeMax;

        if (FitRunDistr("h2runId_shapeShiftedBC1S_TOF700digit", fitPar)) {
            fTOF700digitTimeMin = fitPar.at(1) - 3 * fitPar.at(2);
            fTOF700digitTimeMax = fitPar.at(1) + 3 * fitPar.at(2);
        }
        LOG(info) << "fTOF700digitTimeMin=" << fTOF700digitTimeMin << "\tfTOF700digitTimeMax=" << fTOF700digitTimeMax;

    } else {
        fNoCalibrationData = true;
        LOG(error) << "No input histogram file!!!";
    }
    // Get a handle from the IO manager
    ioman = FairRootManager::Instance();
    if (!ioman)
        LOG(fatal) << "Init" << "FairRootManager is not instantiated";

    // Get a pointer to the previous already existing data level
    fEventHeader = ReadBranch<BmnEventHeader*>("BmnEventHeader.");
    fArrTQDC_BC1S = ReadBranch<TClonesArray*>("TQDC_BC1S");
    fArrTQDC_VCS = ReadBranch<TClonesArray*>("TQDC_VCS");
    fArrTQDC_BC2AS = ReadBranch<TClonesArray*>("TQDC_BC2AS");
    fArrTQDC_FD = ReadBranch<TClonesArray*>("TQDC_FD");
    fArrBDdigits = ReadBranch<TClonesArray*>("BD");
    fArrTOF400digits = ReadBranch<TClonesArray*>("TOF400");
    fArrTOF700digits = ReadBranch<TClonesArray*>(
        "TOF701");   // new tof700 digits are stored in TOF701 branch (09.2025). Will be fixed later
    fArrSILICONdigits = ReadBranch<TClonesArray*>("SILICON");
    fArrGEMdigits = ReadBranch<TClonesArray*>("GEM");

    ioman->Register("BmnBC1hitInfo.", "BC1hitInfo", &fBC1hitInfo, fWriteBC1hitInfo);

    return kSUCCESS;
}

void BmnEventSelector::InitHistograms()
{
    h2runId_eventClass = AddHist(new TH2F("h2runId_eventClass", "Event class;runId;Event class", 1, fRunId, fRunId + 1,
                                          BmnEventClass::kNclasses, 0, BmnEventClass::kNclasses));
    for (int i = 0; i < BmnEventClass::kNclasses; i++)
        h2runId_eventClass->GetYaxis()->SetBinLabel(i + 1, BmnEventClass::names.at(i).c_str());
    auto h2runId_time =
        new TH2F("h2runId_time", "TDC time;runId;TDC time (ns);N entries", 1, fRunId, fRunId + 1, 900, 0, 3600);
    h2runId_time_SingleBC1S = AddHistClone(h2runId_time, "_SingleBC1S");
    h2runId_time_BC1S_k0 = AddHistClone(h2runId_time, "_BC1S_k0");
    h2runId_time_BC1S_k1 = AddHistClone(h2runId_time, "_BC1S_k1");
    h2runId_time_BC1S_kV0 = AddHistClone(h2runId_time, "_BC1S_kV0");
    h2runId_time_BC1S_kV1 = AddHistClone(h2runId_time, "_BC1S_kV1");
    h2runId_time_SingleVCS_BC1S = AddHistClone(h2runId_time, "_SingleVCS_BC1S");
    h2runId_time_SingleVCS_BC1S->GetYaxis()->SetLimits(-50, 50);
    h2runId_time_SingleBC2AS_BC1S = AddHistClone(h2runId_time, "_SingleBC2AS_BC1S");
    h2runId_time_SingleBC2AS_BC1S->GetYaxis()->SetLimits(-20, 0);
    h2runId_time_SingleFD_BC1S = AddHistClone(h2runId_time, "_SingleFD_BC1S");
    h2runId_time_SingleFD_BC1S->GetYaxis()->SetLimits(-50, 200);
    auto h2runId_shape =
        new TH2F("h2runId_shape", "TQDC shape;runId;Time (ns);TQDC signal", 1, fRunId, fRunId + 1, 500, -400, 3600);
    h2runId_shapeShifted_SingleBC1S = AddHistClone(h2runId_shape, "Shifted_SingleBC1S");
    h2runId_shapeShiftedBC1S_SingleVCS = AddHistClone(h2runId_shape, "ShiftedBC1S_SingleVCS");
    h2runId_shapeShiftedBC1S_SingleBC2AS = AddHistClone(h2runId_shape, "ShiftedBC1S_SingleBC2AS");
    h2runId_shapeShiftedBC1S_SingleFD = AddHistClone(h2runId_shape, "ShiftedBC1S_SingleFD");
    h2runId_shapeShiftedBC1S_BD = AddHistClone(h2runId_shape, "ShiftedBC1S_BD");
    h2runId_shapeShiftedBC1S_BD->GetYaxis()->SetLimits(-450, 0);
    h2runId_NdigitsBD =
        AddHist(new TH2F("h2runId_NdigitsBD", "NdigitsBD;runId;NdigitsBD", 1, fRunId, fRunId + 1, 50, 0, 50));
    h2runId_SumAmpBD =
        AddHist(new TH2F("h2runId_SumAmpBD", "SumAmpBD;runId;SumAmpBD", 1, fRunId, fRunId + 1, 200, 0, 800));
    h2runId_NdigitsBD_SingleBC1S = AddHistClone(h2runId_NdigitsBD, "_SingleBC1S");
    h2runId_SumAmpBD_SingleBC1S = AddHistClone(h2runId_SumAmpBD, "_SingleBC1S");
    h2runId_NdigitsBD_nc = AddHistClone(h2runId_NdigitsBD, "_nc");
    h2runId_SumAmpBD_nc = AddHistClone(h2runId_SumAmpBD, "_nc");
    auto h2runId_NdigitsTOF =
        new TH2F("h2runId_NdigitsTOF", "NdigitsTOF;runId;NdigitsTOF", 1, fRunId, fRunId + 1, 400, 0, 400);
    h2runId_NdigitsTOF400 = AddHistClone(h2runId_NdigitsTOF, "400");
    h2runId_NdigitsTOF400_SingleBC1S = AddHistClone(h2runId_NdigitsTOF, "400_SingleBC1S");
    h2runId_NdigitsTOF400_nc = AddHistClone(h2runId_NdigitsTOF, "400_nc");
    h2runId_NdigitsTOF700 = AddHistClone(h2runId_NdigitsTOF, "700");
    h2runId_NdigitsTOF700_SingleBC1S = AddHistClone(h2runId_NdigitsTOF, "700_SingleBC1S");
    h2runId_NdigitsTOF700_nc = AddHistClone(h2runId_NdigitsTOF, "700_nc");
    auto h2runId_peak = new TH2F("h2runId_peak", "Peak;runId;Peak;N entries", 1, fRunId, fRunId + 1, 300, 0, 3e4);
    h2runId_peak_SingleBC1S = AddHistClone(h2runId_peak, "_SingleBC1S");
    h2runId_peak_BC1S = AddHistClone(h2runId_peak, "_BC1S");
    h2runId_peak_SingleVCS = AddHistClone(h2runId_peak, "_SingleVCS");
    h2runId_peak_VCS = AddHistClone(h2runId_peak, "_VCS");
    h2runId_peak_SingleBC2AS = AddHistClone(h2runId_peak, "_SingleBC2AS");
    h2runId_peak_BC2AS = AddHistClone(h2runId_peak, "_BC2AS");
    h2runId_peak_SingleFD = AddHistClone(h2runId_peak, "_SingleFD");
    h2runId_peak_FD = AddHistClone(h2runId_peak, "_FD");
    h2runId_peak_FD_nc = AddHistClone(h2runId_peak, "_FD_nc");

    h2runId_shapeShiftedBC1S_TOF400digit =
        AddHist(new TH2F("h2runId_shapeShiftedBC1S_TOF400digit", "TOF400 digit times;runId;Time (ns)", 1, fRunId,
                         fRunId + 1, 1000, -700, -600));
    h2runId_shapeShiftedBC1S_TOF700digit =
        AddHist(new TH2F("h2runId_shapeShiftedBC1S_TOF700digit", "TOF700 digit times;runId;Time (ns)", 1, fRunId,
                         fRunId + 1, 1000, -2300, -1800));

    if (fFillPerformanceHists) {
        vector<string> trigNames{"BT", "CCT1", "CCT2", "MBT"};
        vector<int> bins{
            fRunMax - fRunMin, int(trigNames.size()), BmnEventClass::kNclasses, 185, 10000,
        };
        vector<double> xmin{
            double(fRunMin), 0, 0, -2200, 0,
        };
        vector<double> xmax{
            double(fRunMax), double(trigNames.size()), double(BmnEventClass::kNclasses), 1500, 10000,
        };
        auto h5runId_trigger_eventClass_dt_n =
            new THnSparseI("h5runId_trigger_eventClass_dt_n", "Number of;runId;trigger;Event class;#Deltat (ns);N",
                           bins.size(), bins.data(), xmin.data(), xmax.data());
        for (uint i = 0; i < trigNames.size(); i++)
            h5runId_trigger_eventClass_dt_n->GetAxis(1)->SetBinLabel(i + 1, trigNames.at(i).c_str());
        for (int i = 0; i < BmnEventClass::kNclasses; i++)
            h5runId_trigger_eventClass_dt_n->GetAxis(2)->SetBinLabel(i + 1, BmnEventClass::names.at(i).c_str());

        int n = 400;
        vector<int> rebin = {1, 1, 1, 1, int(xmax.at(4) / n)};
        auto h5runId_trigger_eventClass_dt_nTOF =
            static_cast<THnSparse*>(h5runId_trigger_eventClass_dt_n->Clone())->Rebin(rebin.data());
        h5runId_trigger_eventClass_dt_nTOF->SetName("h5runId_trigger_eventClass_dt_nTOF");
        h5runId_trigger_eventClass_dt_nTOF->SetTitle("Number of TOF");
        h5runId_trigger_eventClass_dt_nTOF->GetAxis(4)->SetLimits(0, n);
        h5runId_trigger_eventClass_dt_nTOF400digits = AddHistClone(h5runId_trigger_eventClass_dt_nTOF, "400digits");
        h5runId_trigger_eventClass_dt_nTOF700digits = AddHistClone(h5runId_trigger_eventClass_dt_nTOF, "700digits");

        n = 5000;
        rebin = {1, 1, 1, 1, int(xmax.at(4) / n)};
        auto h5runId_trigger_eventClass_dt_nSILICON =
            static_cast<THnSparse*>(h5runId_trigger_eventClass_dt_n->Clone())->Rebin(rebin.data());
        h5runId_trigger_eventClass_dt_nSILICON->SetName("h5runId_trigger_eventClass_dt_nSILICON");
        h5runId_trigger_eventClass_dt_nSILICON->SetTitle("Number of SILICON");
        h5runId_trigger_eventClass_dt_nSILICON->GetAxis(4)->SetLimits(0, n);
        h5runId_trigger_eventClass_dt_nSILICONdigits = AddHistClone(h5runId_trigger_eventClass_dt_nSILICON, "digits");

        h5runId_trigger_eventClass_dt_nGEMdigits = AddHistClone(h5runId_trigger_eventClass_dt_n, "GEMdigits");
    }
}

// ---- ReInit  -------------------------------------------------------
InitStatus BmnEventSelector::ReInit()
{
    LOG(debug) << "Initilization of BmnEventSelector";
    return kSUCCESS;
}

int BmnEventSelector::GetClosestHitIndex(std::vector<double> hitTimes, float refTime)
{
    if (hitTimes.size() == 0)
        return -1;
    int index = 0;
    float min = fabs(hitTimes.at(0) - refTime);
    for (uint i = 1; i < hitTimes.size(); i++) {
        auto dt = fabs(hitTimes.at(i) - refTime);
        if (dt < min) {
            min = dt;
            index = i;
        }
    }
    return index;
}

int BmnEventSelector::GetPeak(BmnTrigWaveDigit* digit, float timeMin, float timeMax)
{
    if (!digit)
        return 0;
    int binMin = timeMin / fNsPerChannel;
    uint binMax = timeMax / fNsPerChannel;
    if (binMin < 0 || binMax >= digit->GetNSamples())
        return -999;
    return digit->GetPeak();
    //return digit->GetPeak(binMin, binMax);//tim
}

template<typename T>
std::vector<float> BmnEventSelector::GetSumAmp(TClonesArray* array,
                                               std::vector<double> hitTimes,
                                               float relTimeMin,
                                               float relTimeMax)
{
    std::vector<float> sum(hitTimes.size(), 0.);
    for (uint i = 0; i < hitTimes.size(); i++) {
        for (auto obj : *array) {
            auto digit = static_cast<T>(obj);
            auto relTime = digit->GetTime() - hitTimes.at(i);
            if (relTimeMin < relTime && relTime < relTimeMax)
                sum.at(i) += digit->GetAmp();
        }
    }
    return sum;
}

template<typename T>
std::vector<int> BmnEventSelector::GetNdigits(TClonesArray* array,
                                              std::vector<double> hitTimes,
                                              float relTimeMin,
                                              float relTimeMax)
{
    std::vector<int> n(hitTimes.size(), 0);
    for (uint i = 0; i < hitTimes.size(); i++) {
        for (auto obj : *array) {
            auto relTime = static_cast<T>(obj)->GetTime() - hitTimes.at(i);
            if (relTimeMin < relTime && relTime < relTimeMax)
                n.at(i)++;
        }
    }
    return n;
}

std::vector<int> BmnEventSelector::GetPeaks(BmnTrigWaveDigit* digit,
                                            std::vector<double> hitTimes,
                                            float relTimeMin,
                                            float relTimeMax,
                                            float relHitTimeMin,
                                            float relHitTimeMax)
{
    std::vector<int> ret(hitTimes.size(), -999);
    if (!digit)
        return ret;
    auto hitTimesDigit = digit->TdcVector();
    auto tMax = fNsPerChannel * digit->GetNSamples();
    for (uint i = 0; i < hitTimes.size(); i++) {
        auto hitTime = hitTimes.at(i);
        auto hitTimeMin = hitTime + relHitTimeMin;
        auto hitTimeMax = hitTime + relHitTimeMax;
        if (hitTimeMin < 0 || hitTimeMax > tMax)
            continue;
        auto hitFound = true;   // FIX ME!!! -> false
        for (uint j = 0; j < hitTimesDigit.size(); j++) {
            auto hitTimeDigit = hitTimesDigit.at(j);
            if (hitTimeMin <= hitTimeDigit && hitTimeDigit <= hitTimeMax) {
                hitFound = true;
                break;
            }
        }
        auto timeMin = hitTime + relTimeMin;
        auto timeMax = hitTime + relTimeMax;
        if (hitFound)
            ret.at(i) = GetPeak(digit, timeMin, timeMax);
        else
            ret.at(i) = 0;
    }
    return ret;
}

void BmnEventSelector::Calculate()
{
    fTQDC_BC1S = (BmnTrigWaveDigit*)fArrTQDC_BC1S->At(0);
    fTQDC_VCS = (BmnTrigWaveDigit*)fArrTQDC_VCS->At(0);
    fTQDC_BC2AS = (BmnTrigWaveDigit*)fArrTQDC_BC2AS->At(0);
    fTQDC_FD = (BmnTrigWaveDigit*)fArrTQDC_FD->At(0);

    if (!fTQDC_BC1S || !fTQDC_VCS || !fTQDC_FD)
        return;

    fTimesBC1S = fTQDC_BC1S->TdcVector();
    fNhitsBC1S = fTimesBC1S.size();
    if (fNhitsBC1S == 0) {
        if (fFillCalibrationHists)
            h2runId_eventClass->Fill(fRunId, BmnEventClass::kNull);
        return;
    }
    fTimesVCS = fTQDC_VCS->TdcVector();
    fNhitsVCS = fTimesVCS.size();
    fTimesBC2AS = fTQDC_BC2AS->TdcVector();
    fNhitsBC2AS = fTimesBC2AS.size();
    fTimesFD = fTQDC_FD->TdcVector();
    fNhitsFD = fTimesFD.size();

    fBC1Speaks = GetPeaks(fTQDC_BC1S, fTimesBC1S, fBC1SpeakTimeMin, fBC1SpeakTimeMax);
    fVCSpeaks = GetPeaks(fTQDC_VCS, fTimesBC1S, fVCSpeakTimeMin, fVCSpeakTimeMax, fSingleVCS_BC1S_hitTimeMin,
                         fSingleVCS_BC1S_hitTimeMax);
    fBC2ASpeaks = GetPeaks(fTQDC_BC2AS, fTimesBC1S, fBC2ASpeakTimeMin, fBC2ASpeakTimeMax, fSingleBC2AS_BC1S_hitTimeMin,
                           fSingleBC2AS_BC1S_hitTimeMax);
    fFDpeaks = GetPeaks(fTQDC_FD, fTimesBC1S, fFDpeakTimeMin, fFDpeakTimeMax, fSingleFD_BC1S_hitTimeMin,
                        fSingleFD_BC1S_hitTimeMax);
    fNdigitsBD = GetNdigits<BmnTrigDigit*>(fArrBDdigits, fTimesBC1S, fBDtimeMin, fBDtimeMax);
    fSumAmpBD = GetSumAmp<BmnTrigDigit*>(fArrBDdigits, fTimesBC1S, fBDtimeMin, fBDtimeMax);
    fNdigitsTOF400 = GetNdigits<BmnTof1Digit*>(fArrTOF400digits, fTimesBC1S, fTOF400digitTimeMin, fTOF400digitTimeMax);
    fNdigitsTOF700 = GetNdigits<BmnTof1Digit*>(fArrTOF700digits, fTimesBC1S, fTOF700digitTimeMin, fTOF700digitTimeMax);
}

BmnEventClass::id BmnEventSelector::Classify()
{
    using namespace BmnEventClass;
    BmnEventClass::id status;

    for (int i = 0; i < fNhitsBC1S; i++) {
        status = k0;
        if (fNoCalibrationData || fBC1Speaks.at(i) < -998 || fVCSpeaks.at(i) < -998 || fBC2ASpeaks.at(i) < -998
            || fFDpeaks.at(i) < -998)
        {
            fBC1hitClasses.push_back(kUndef);
            continue;
        }

        if (fFDpeaks.at(i) < fSingleFDpeakMin)
            status = k1;
        if (fVCSpeaks.at(i) > fSingleVCSpeakMin || fBC2ASpeaks.at(i) < fSingleBC2ASpeakMin) {
            if (status == k0)
                status = kV0;
            if (status == k1)
                status = kV1;
        }
        fBC1hitClasses.push_back(status);
    }

    fCentralHitIndexBC1S = GetClosestHitIndex(fTimesBC1S, fSingleBC1hitTimeMean);
    if (fCentralHitIndexBC1S < 0)
        return kNull;

    if (fBC1hitClasses.at(fCentralHitIndexBC1S) == k0
        || fBC1hitClasses.at(fCentralHitIndexBC1S) == kV0)   // search backwards
    {
        const float DTmax = 300;   // ns around mean for single BC1S hit
        for (int i = fCentralHitIndexBC1S - 1; i >= 0 && fSingleBC1hitTimeMean - fTimesBC1S.at(i) < DTmax; i--) {
            if (fBC1hitClasses.at(i) == k1 || fBC1hitClasses.at(i) == kV1) {
                fCentralHitIndexBC1S = i;
                break;
            }
        }
    }
    fCentralHitTimeBC1S = fTimesBC1S.at(fCentralHitIndexBC1S);
    fRelTimesBC1S = fTimesBC1S;
    for (auto& t : fRelTimesBC1S)
        t -= fCentralHitTimeBC1S;

    if (fNhitsBC1S == 1)
        return status;
    if (fNhitsBC1S == 2) {
        int c = fCentralHitIndexBC1S;
        int nc = abs(c - 1);

        if (fBC1hitClasses.at(c) == k0 && fBC1hitClasses.at(nc) == k0)
            return k00;
        if (fBC1hitClasses.at(c) == k0 && fBC1hitClasses.at(nc) == k1)
            return k01;
        if (fBC1hitClasses.at(c) == k0 && fBC1hitClasses.at(nc) == kV0)
            return k0V0;
        if (fBC1hitClasses.at(c) == k0 && fBC1hitClasses.at(nc) == kV1)
            return k0V1;
        if (fBC1hitClasses.at(c) == k1 && fBC1hitClasses.at(nc) == k0)
            return k10;
        if (fBC1hitClasses.at(c) == k1 && fBC1hitClasses.at(nc) == k1)
            return k11;
        if (fBC1hitClasses.at(c) == k1 && fBC1hitClasses.at(nc) == kV0)
            return k1V0;
        if (fBC1hitClasses.at(c) == k1 && fBC1hitClasses.at(nc) == kV1)
            return k1V1;
    }
    if (fNhitsBC1S > 2)
        return kMult;
    return kUndef;
}

// ---- FillHistograms
// ----------------------------------------------------------
void BmnEventSelector::FillHistograms()
{
    using namespace BmnTriggerBits;
    bool isBT_AR = fTrigAR & BIT(kBT);
    bool isMBT_AR = fTrigAR & BIT(kMBT);
    bool isCCT1_AR = fTrigAR & BIT(kCCT1);
    bool isCCT2_AR = fTrigAR & BIT(kCCT2);
    vector<bool> trigAR = {isBT_AR, isCCT1_AR, isCCT2_AR, isMBT_AR};

    fNdigitsSILICON = 0;
    for (auto digit : *fArrSILICONdigits)
        if (static_cast<BmnStripDigit*>(digit)->IsGoodDigit())
            fNdigitsSILICON++;

    fNdigitsGEM = 0;
    for (auto digit : *fArrGEMdigits)
        if (static_cast<BmnStripDigit*>(digit)->IsGoodDigit())
            fNdigitsGEM++;

    h2runId_eventClass->Fill(fRunId, fEventClass);
    for (int i = 0; i < fNhitsBC1S; i++) {
        if (fBC1hitClasses.at(i) == BmnEventClass::k0)
            h2runId_time_BC1S_k0->Fill(fRunId, fTimesBC1S.at(i));
        else if (fBC1hitClasses.at(i) == BmnEventClass::k1)
            h2runId_time_BC1S_k1->Fill(fRunId, fTimesBC1S.at(i));
        else if (fBC1hitClasses.at(i) == BmnEventClass::kV0)
            h2runId_time_BC1S_kV0->Fill(fRunId, fTimesBC1S.at(i));
        else if (fBC1hitClasses.at(i) == BmnEventClass::kV1)
            h2runId_time_BC1S_kV1->Fill(fRunId, fTimesBC1S.at(i));
        h2runId_peak_BC1S->Fill(fRunId, fBC1Speaks.at(i));
        h2runId_peak_VCS->Fill(fRunId, fVCSpeaks.at(i));
        h2runId_peak_BC2AS->Fill(fRunId, fBC2ASpeaks.at(i));
        h2runId_peak_FD->Fill(fRunId, fFDpeaks.at(i));
        h2runId_NdigitsBD->Fill(fRunId, fNdigitsBD.at(i));
        h2runId_SumAmpBD->Fill(fRunId, fSumAmpBD.at(i));
        h2runId_NdigitsTOF400->Fill(fRunId, fNdigitsTOF400.at(i));
        h2runId_NdigitsTOF700->Fill(fRunId, fNdigitsTOF700.at(i));
        if (i != fCentralHitIndexBC1S) {
            h2runId_peak_FD_nc->Fill(fRunId, fFDpeaks.at(i));
            h2runId_NdigitsBD_nc->Fill(fRunId, fNdigitsBD.at(i));
            h2runId_SumAmpBD_nc->Fill(fRunId, fSumAmpBD.at(i));
            h2runId_NdigitsTOF400_nc->Fill(fRunId, fNdigitsTOF400.at(i));
            h2runId_NdigitsTOF700_nc->Fill(fRunId, fNdigitsTOF700.at(i));
        }
    }

    if (fNhitsBC1S == 1) {
        h2runId_time_SingleBC1S->Fill(fRunId, fTimesBC1S.at(0));
        auto pulseBC1S = fTQDC_BC1S->GetShortValue();
        for (uint i = 0; i < fTQDC_BC1S->GetNSamples(); i++) {
            auto channelTime = i * fNsPerChannel;
            h2runId_shapeShifted_SingleBC1S->Fill(fRunId, channelTime - fTimesBC1S.at(0), pulseBC1S[i]);
        }
        h2runId_peak_SingleBC1S->Fill(fRunId, fBC1Speaks.at(0));

        for (auto digit : *fArrBDdigits) {
            auto time = static_cast<BmnTrigDigit*>(digit)->GetTime() - fTimesBC1S.at(0);
            h2runId_shapeShiftedBC1S_BD->Fill(fRunId, time);
        }
        h2runId_NdigitsBD_SingleBC1S->Fill(fRunId, fNdigitsBD.at(0));
        h2runId_SumAmpBD_SingleBC1S->Fill(fRunId, fSumAmpBD.at(0));
        h2runId_NdigitsTOF400_SingleBC1S->Fill(fRunId, fNdigitsTOF400.at(0));
        h2runId_NdigitsTOF700_SingleBC1S->Fill(fRunId, fNdigitsTOF700.at(0));

        for (auto digit : *fArrTOF400digits) {
            auto time = static_cast<BmnTof1Digit*>(digit)->GetTime() - fTimesBC1S.at(0);
            h2runId_shapeShiftedBC1S_TOF400digit->Fill(fRunId, time);
        }

        for (auto digit : *fArrTOF700digits) {
            auto time = static_cast<BmnTof1Digit*>(digit)->GetTime() - fTimesBC1S.at(0);
            h2runId_shapeShiftedBC1S_TOF700digit->Fill(fRunId, time);
        }

        if (fNhitsFD == 1) {
            h2runId_time_SingleFD_BC1S->Fill(fRunId, fTimesFD.at(0) - fTimesBC1S.at(0));
            h2runId_peak_SingleFD->Fill(fRunId, fFDpeaks.at(0));
            auto pulseFD = fTQDC_FD->GetShortValue();
            for (uint i = 0; i < fTQDC_FD->GetNSamples(); i++) {
                auto channelTime = i * fNsPerChannel;
                h2runId_shapeShiftedBC1S_SingleFD->Fill(fRunId, channelTime - fTimesBC1S.at(0), pulseFD[i]);
            }
        }
        if (fNhitsBC2AS == 1) {
            h2runId_time_SingleBC2AS_BC1S->Fill(fRunId, fTimesBC2AS.at(0) - fTimesBC1S.at(0));
            h2runId_peak_SingleBC2AS->Fill(fRunId, fBC2ASpeaks.at(0));
            auto pulseBC2AS = fTQDC_BC2AS->GetShortValue();
            for (uint i = 0; i < fTQDC_BC2AS->GetNSamples(); i++) {
                auto channelTime = i * fNsPerChannel;
                h2runId_shapeShiftedBC1S_SingleBC2AS->Fill(fRunId, channelTime - fTimesBC1S.at(0), pulseBC2AS[i]);
            }
        }
    }

    if (fNhitsBC1S == 2 && fNhitsVCS == 1) {
        h2runId_time_SingleVCS_BC1S->Fill(fRunId, fTimesVCS.at(0) - fTimesBC1S.at(0));
        h2runId_peak_SingleVCS->Fill(fRunId, fVCSpeaks.at(0));
        auto closestBC1Sindex = GetClosestHitIndex(fTimesBC1S, fTimesVCS.at(0));
        auto closestBC1Stime = fTimesBC1S.at(closestBC1Sindex);
        auto pulseVCS = fTQDC_VCS->GetShortValue();
        for (uint i = 0; i < fTQDC_VCS->GetNSamples(); i++) {
            auto channelTime = i * fNsPerChannel;
            h2runId_shapeShiftedBC1S_SingleVCS->Fill(fRunId, channelTime - closestBC1Stime, pulseVCS[i]);
        }
    }

    if (fFillPerformanceHists) {
        float dt = 0;
        if (fNhitsBC1S > 1) {
            float dtLeft = 1e10, dtRight = 1e10;
            if (fCentralHitIndexBC1S > 0)
                dtLeft = fRelTimesBC1S.at(fCentralHitIndexBC1S - 1);
            if (fCentralHitIndexBC1S < fNhitsBC1S - 1)
                dtRight = fRelTimesBC1S.at(fCentralHitIndexBC1S + 1);
            if (fabs(dtLeft) < dtRight)
                dt = dtLeft;
            else
                dt = dtRight;
        }

        vector<double> x = {fRunId + 0.5, 0., fEventClass + 0.5, dt, 0.};
        for (uint i = 0; i < trigAR.size(); i++) {
            if (trigAR.at(i)) {
                x.at(1) = i + 0.5;
                x.at(4) = fNdigitsTOF400.at(fCentralHitIndexBC1S);
                h5runId_trigger_eventClass_dt_nTOF400digits->Fill(x.data());
                x.at(4) = fNdigitsTOF700.at(fCentralHitIndexBC1S);
                h5runId_trigger_eventClass_dt_nTOF700digits->Fill(x.data());
                x.at(4) = fNdigitsSILICON;
                h5runId_trigger_eventClass_dt_nSILICONdigits->Fill(x.data());
                x.at(4) = fNdigitsGEM;
                h5runId_trigger_eventClass_dt_nGEMdigits->Fill(x.data());
            }
        }
    }
}

// ---- Exec ----------------------------------------------------------
void BmnEventSelector::Exec(Option_t* /*option*/)
{
    // cout << "Event " << ++iEvent << endl;
    fTrigAR = fEventHeader->GetInputSignalsAR();
    fEventClass = BmnEventClass::kUndef;
    fNhitsBC1S = 0;
    fCentralHitIndexBC1S = -1;
    fTimesBC1S.clear();
    fRelTimesBC1S.clear();
    fBC1Speaks.clear();
    fBC1hitClasses.clear();
    fNdigitsBD.clear();
    fSumAmpBD.clear();
    fBC2ASpeaks.clear();
    fVCSpeaks.clear();
    fFDpeaks.clear();
    fNdigitsTOF400.clear();
    fNdigitsTOF700.clear();
    fNdigitsSILICON = -999;
    fNdigitsGEM = -999;
    Calculate();
    fEventClass = Classify();

    fBC1hitInfo.fEventClass = fEventClass;
    fBC1hitInfo.fCentralHitIndexBC1S = fCentralHitIndexBC1S;
    fBC1hitInfo.fTimesBC1S = fTimesBC1S;
    fBC1hitInfo.fRelTimesBC1S = fRelTimesBC1S;
    fBC1hitInfo.fBC1hitClasses = fBC1hitClasses;
    fBC1hitInfo.fBC1Speaks = fBC1Speaks;
    fBC1hitInfo.fVCSpeaks = fVCSpeaks;
    fBC1hitInfo.fBC2ASpeaks = fBC2ASpeaks;
    fBC1hitInfo.fFDpeaks = fFDpeaks;
    fBC1hitInfo.fNdigitsBD = fNdigitsBD;
    fBC1hitInfo.fSumAmpBD = fSumAmpBD;
    fBC1hitInfo.fNdigitsTOF400 = fNdigitsTOF400;
    fBC1hitInfo.fNdigitsTOF700 = fNdigitsTOF700;

    if (fFillCalibrationHists && fEventClass != BmnEventClass::kNull)
        FillHistograms();
}

// ---- Finish --------------------------------------------------------
void BmnEventSelector::Finish()
{
    LOG(debug) << "Finish of BmnEventSelector";
    if (fFillCalibrationHists) {
        fOutputFile->cd();
        for (auto h : *fOutputList) {
            string className = h->ClassName();
            if (className.find("THn") == 0) {
                vector<int> axes = {0, 1, 2, 4};
                static_cast<THnSparse*>(h)->Projection(axes.size(), axes.data())->Write();
                axes = {1, 2, 3, 4};
                static_cast<THnSparse*>(h)->Projection(axes.size(), axes.data())->Write();
            } else
                h->Write();
        }
        for (auto h : *fOutputList) {
            string name = h->GetName();
            string className = h->ClassName();
            if (className.find("TH2") == 0 && name.find("h2runId") == 0) {
                // static_cast<TH2*>(h)->ProfileX()->Write();
                static_cast<TH2*>(h)->ProjectionY()->Write();
            }
        }
        fOutputFile->Close();
    }
}
