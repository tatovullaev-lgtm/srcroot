/********************************************************************************
 *    BmnFillDstTask.cxx                                                        *
 *    BM@N Fill DST Task class implementation                                   *
 *    Add necessary data and event header to the DST file after reconstruction  *
 *******************************************************************************/

#define ANSI_COLOR_RED "\x1b[91m"
#define ANSI_COLOR_BLUE "\x1b[94m"
#define ANSI_COLOR_RESET "\x1b[0m"

#include "BmnFillDstTask.h"

#include "BmnFieldConst.h"
#include "BmnFieldMap.h"
#include "BmnTrigDigit.h"
#include "ExtractZ2.h"
#include "FairGeoParSet.h"
#include "FairLogger.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TGeoManager.h"
#include "TROOT.h"
#include "TSystem.h"
#include "UniRun.h"
#include "function_set.h"

#include <TStopwatch.h>
#include <fstream>
#include <iostream>
using namespace std;

static Double_t workTime = 0.0;

// ---- Default constructor -------------------------------------------
BmnFillDstTask::BmnFillDstTask()
    : BmnTask("BmnFillDstTask")
    , isExpData(-1)
    , fPeriodNumber(-1)
    , fRunNumber(-1)
    , fMCEventHeader(nullptr)
    , fMCTracks(nullptr)
    , fMCInfoDst(nullptr)
    , fEventHeader(nullptr)
    , fDstHeader(nullptr)
    , fRunHeader(nullptr)
    , fNStartEvent(0)
    , fNEvents(-1)
    , fIEvent(0)
    , fPrimaryVertex(nullptr)
    , fGlobalTracks(nullptr)
    , fZCalib1(1)
    , fZCalib2(0)
    , fBC1Calib(0)
    , fBC2Calib(0)
    , fDoCalibration(kFALSE)
    , fT0(nullptr)
    , fBC1(nullptr)
    , fBC2(nullptr)
    , fBC3(nullptr)
    , fBC4(nullptr)
{
    LOG(debug) << "Defaul Constructor of BmnFillDstTask";
}

// ---- Constructor with start event and event number to be processed, and autodefinition whether experimental data are
// used -------
BmnFillDstTask::BmnFillDstTask(Long64_t nStartEvent, Long64_t nEvents)
    : BmnTask("BmnFillDstTask")
    , isExpData(-1)
    , fPeriodNumber(-1)
    , fRunNumber(-1)
    , fMCEventHeader(nullptr)
    , fMCTracks(nullptr)
    , fMCInfoDst(nullptr)
    , fEventHeader(nullptr)
    , fDstHeader(nullptr)
    , fRunHeader(nullptr)
    , fNStartEvent(nStartEvent)
    , fNEvents(nEvents)
    , fIEvent(0)
    , fPrimaryVertex(nullptr)
    , fGlobalTracks(nullptr)
    , fZCalib1(1)
    , fZCalib2(0)
    , fBC1Calib(0)
    , fBC2Calib(0)
    , fDoCalibration(kFALSE)
    , fT0(nullptr)
    , fBC1(nullptr)
    , fBC2(nullptr)
    , fBC3(nullptr)
    , fBC4(nullptr)
{
    LOG(debug) << "Constructor of BmnFillDstTask";
}

// Constructor with start event and event number to be processed, experimental data flag, period and run numbers
BmnFillDstTask::BmnFillDstTask(Long64_t nStartEvent,
                               Long64_t nEvents,
                               Bool_t isExp,
                               Int_t period_number,
                               Int_t run_number)
    : BmnTask("BmnFillDstTask")
    , fPeriodNumber(period_number)
    , fRunNumber(run_number)
    , fMCEventHeader(nullptr)
    , fMCTracks(nullptr)
    , fMCInfoDst(nullptr)
    , fEventHeader(nullptr)
    , fDstHeader(nullptr)
    , fRunHeader(nullptr)
    , fNStartEvent(nStartEvent)
    , fNEvents(nEvents)
    , fIEvent(0)
    , fPrimaryVertex(nullptr)
    , fGlobalTracks(nullptr)
    , fZCalib1(1)
    , fZCalib2(0)
    , fBC1Calib(0)
    , fBC2Calib(0)
    , fDoCalibration(kFALSE)
    , fT0(nullptr)
    , fBC1(nullptr)
    , fBC2(nullptr)
    , fBC3(nullptr)
    , fBC4(nullptr)
{
    LOG(debug) << "Constructor of BmnFillDstTask";
    if (isExp)
        isExpData = 1;
    else
        isExpData = 0;
}

// ---- Destructor ----------------------------------------------------
BmnFillDstTask::~BmnFillDstTask()
{
    LOG(debug) << "Destructor of BmnFillDstTask";
    if (fMCInfoDst)
        delete fMCInfoDst;
    if (fRunHeader)
        delete fRunHeader;
}

// Set headers with autodefinition whether experimental or simulation data are used
bool BmnFillDstTask::autoSetHeaders()
{
    FairRootManager* ioman = FairRootManager::Instance();

    isExpData = 1;
    // Get a pointer to the input Event Header
    TObject* pObj = ioman->GetObject("BmnEventHeader.");
    if (!pObj) {
        // if no input Event Header was found, searching for "MCEventHeader."
        fMCEventHeader = (FairMCEventHeader*)ioman->GetObject("MCEventHeader.");
        if (!fMCEventHeader)
            return false;
        isExpData = 0;
    } else {
        if (pObj->InheritsFrom(FairMCEventHeader::Class())) {
            fMCEventHeader = (FairMCEventHeader*)pObj;
            isExpData = 0;
        } else
            fEventHeader = (BmnEventHeader*)pObj;
    }

    return true;
}

// ---- Initialization of task at the beginning ------------------------
InitStatus BmnFillDstTask::Init()
{
    LOG(debug) << "Initialization of BmnFillDstTask";

    fRunHeader = new DstRunHeader();

    // Get a handle from the IO manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        LOG(error) << "Init: FairRootManager is not instantiated! BmnFillDstTask task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    if (isExpData == -1) {
        if (!autoSetHeaders()) {
            LOG(error) << "No input Event Header (BmnEventHeader. or MCEventHeader.) found! BmnFillDstTask task will "
                          "be deactivated";
            return kERROR;
        }
    } else {
        if (isExpData == 0)
            fMCEventHeader = (FairMCEventHeader*)ioman->GetObject("MCEventHeader.");
        else
            fEventHeader = (BmnEventHeader*)ioman->GetObject("BmnEventHeader.");
    }

    applyZCalibration();

    // Get input branches
    if (isExpData == 1) {
        fT0 = (TClonesArray*)ioman->GetObject("BC2");
        fBC1 = (TClonesArray*)ioman->GetObject("TQDC_BC1");
        fBC2 = (TClonesArray*)ioman->GetObject("TQDC_BC2");
        fBC3 = (TClonesArray*)ioman->GetObject("TQDC_BC3");
        fBC4 = (TClonesArray*)ioman->GetObject("TQDC_BC4");
    }
    // Get a pointer to the output DST Event Header
    fDstHeader = (DstEventHeader*)ioman->GetObject("DstEventHeader.");
    if (!fDstHeader) {
        LOG(error) << "No DstEventHeader prepared for the output DST file! BmnFillDstTask task will be deactivated";
        return kERROR;
    }

    if (ioman->CheckMaxEventNo(fNEvents) < fNEvents)
        fNEvents = ioman->CheckMaxEventNo(fNEvents);

    // Fill Run Header with the magnetic field and geometry to simply use in physics analyses
    FairField* fField = FairRunAna::Instance()->GetField();
    if (fField != nullptr) {
        fRunHeader->SetFieldType(fField->GetType());
        if (fField->GetType() == 0) {
            BmnFieldConst* fFieldConst = reinterpret_cast<BmnFieldConst*>(fField);
            fRunHeader->SetFieldScale(-1);
            fRunHeader->SetFieldName(TString::Format(
                "%f:%f:%f:%f:%f:%f:%f:%f:%f", fFieldConst->GetXmin(), fFieldConst->GetXmax(), fFieldConst->GetYmin(),
                fFieldConst->GetYmax(), fFieldConst->GetZmin(), fFieldConst->GetZmax(), fFieldConst->GetBx(),
                fFieldConst->GetBy(), fFieldConst->GetBz()));
        } else {
            BmnFieldMap* fFieldMap = reinterpret_cast<BmnFieldMap*>(fField);
            fRunHeader->SetFieldScale(fFieldMap->GetScale());
            fRunHeader->SetFieldName(fFieldMap->GetName());
        }
    }
    fRunHeader->SetGeometryName(TString::Format("&%d:%d", fPeriodNumber, fRunNumber));

    // Fill Run Header from the Condition Database
    if (fRunNumber > 0) {
        InitParticleInfo();

        // Save gGeoManager in the container (this call creates the container if it does not exist)
        FairGeoParSet* geopar = dynamic_cast<FairGeoParSet*>(FairRuntimeDb::instance()->getContainer("FairGeoParSet"));
        if (geopar) {
            geopar->SetGeometry(gGeoManager);
            geopar->setChanged();
            // geopar->setInputVersion(version, index);
        }

        UniRun* pCurrentRun = UniRun::GetRun(fPeriodNumber, fRunNumber);
        if (pCurrentRun != 0) {
            fRunHeader->SetPeriodRun(fPeriodNumber, fRunNumber);
            TDatime start_date = pCurrentRun->GetStartDatetime();
            TTimeStamp tsStartTime(start_date.GetYear(), start_date.GetMonth(), start_date.GetDay(),
                                   start_date.GetHour(), start_date.GetMinute(), start_date.GetSecond());
            fRunHeader->SetStartTime(tsStartTime);
            TDatime* end_date = pCurrentRun->GetEndDatetime();
            if (end_date != NULL) {

                // TTimeStamp tsEndTime((time_t) pCurrentRun->GetEndDatetime()->Convert(), 0);

                TTimeStamp tsEndTime(end_date->GetYear(), end_date->GetMonth(), end_date->GetDay(), end_date->GetHour(),
                                     end_date->GetMinute(), end_date->GetSecond());
                fRunHeader->SetFinishTime(tsEndTime);
            }
            delete end_date;
            TString beam = pCurrentRun->GetBeamParticle();
            beam.ToLower();
            map<TString, stParticleInfo>::iterator it = mapParticleInfo.find(beam);
            if (it != mapParticleInfo.end()) {
                fRunHeader->SetBeamA(it->second.A);
                fRunHeader->SetBeamZ(it->second.Z);
            }
            double* energy = pCurrentRun->GetEnergy();
            if (energy != NULL) {
                fRunHeader->SetBeamEnergy(*energy);
                delete energy;
            }

            TString* target = pCurrentRun->GetTargetParticle();
            if (target != NULL) {
                target->ToLower();
                it = mapParticleInfo.find(*target);
                if (it != mapParticleInfo.end()) {
                    fRunHeader->SetTargetA(it->second.A);
                    fRunHeader->SetTargetZ(it->second.Z);
                }
                delete target;
            }
        }   // if (pCurrentRun != 0)
        delete pCurrentRun;
    }   // FIll Run Header from the Database

    // Get summary event info to print
    fPrimaryVertex = (CbmVertex*)ioman->GetObject("PrimaryVertex.");
    if (!fPrimaryVertex) {
        LOG(debug) << "BmnFillDstTask::Init(): branch PrimaryVertex not found! The task is deactivated" << endl;
        // SetActive(kFALSE);
        // return kERROR;
    }
    fGlobalTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
    if (!fGlobalTracks) {
        LOG(debug) << "BmnFfBC1illDstTask::Init(): branch BmnGlobalTrack not found! The task is deactivated" << endl;
        // SetActive(kFALSE);
        // return kERROR;
    }

    // Add MC information (header and tracks)
    if (isExpData == 0) {
        fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");
        if (!fMCTracks)
            LOG(debug) << "BmnFillDstTask::Init(): branch MCTrack not found" << endl;

        // Create and register MCInfo object for MC output data
        fMCInfoDst = new BmnMCInfoDst();
        ioman->Register("BmnMCInfo.", "MC", fMCInfoDst, kTRUE);
    }

    return kSUCCESS;
}

// ---- ReInit  -------------------------------------------------------
InitStatus BmnFillDstTask::ReInit()
{
    LOG(debug) << "Re-initilization of BmnFillDstTask";
    return kSUCCESS;
}

// ---- ReInit  -------------------------------------------------------
void BmnFillDstTask::Reset()
{
    if (fMCInfoDst != nullptr)
        if (fMCInfoDst->GetMCTracks() != nullptr)
            fMCInfoDst->GetMCTracks()->Delete();
}

// ---- Exec ----------------------------------------------------------
void BmnFillDstTask::Exec(Option_t* /*option*/)
{

    TStopwatch sw;
    sw.Start();

    LOG(debug) << "Exec of BmnFillDstTask";
    Reset();

    // fill output DST event header
    if (isExpData == 0) {
        fDstHeader->SetRunId(fMCEventHeader->GetRunID());
        fDstHeader->SetEventId(fMCEventHeader->GetEventID());
        fDstHeader->SetEventTime(fMCEventHeader->GetT());
        fDstHeader->SetEventTimeTS((TTimeStamp)convert_double_to_timespec(fMCEventHeader->GetT()));
        fDstHeader->SetB(fMCEventHeader->GetB());

        // copy MC information to DST branch in case of simulated data
        fMCInfoDst->SetB(fMCEventHeader->GetB());
        fMCInfoDst->SetVertex(fMCEventHeader->GetX(), fMCEventHeader->GetY(), fMCEventHeader->GetZ());
        fMCInfoDst->SetRotX(fMCEventHeader->GetRotX());
        fMCInfoDst->SetRotY(fMCEventHeader->GetRotY());
        fMCInfoDst->SetRotZ(fMCEventHeader->GetRotZ());
        fMCInfoDst->GetMCTracks()->AbsorbObjects(fMCTracks);
    } else {
        fDstHeader->SetRunId(fEventHeader->GetRunId());
        fDstHeader->SetEventId(fEventHeader->GetEventId());
        fDstHeader->SetEventTime(fEventHeader->GetEventTime());
        fDstHeader->SetEventTimeTS(fEventHeader->GetEventTimeTS());
    }

    // calculate Z2in and Z2out:
    Double_t Z2in = -100.0, Z2out = -100.0;
    Double_t adcIn = -100.0, adcOut = -100.0;
    Short_t Zin = -100;
    Short_t Zout = -100;
    if (fT0 && fBC1 && fBC2 && fBC3 && fBC4) {
        BmnTrigDigit* digT0 = NULL;
        Int_t t0Count = 0;
        for (Int_t i = 0; i < fT0->GetEntriesFast(); i++) {
            digT0 = (BmnTrigDigit*)fT0->At(i);
            if (digT0->GetMod() == 0)
                t0Count++;
        }
        if (t0Count == 1) {
            Double_t t0Time = digT0->GetTime();

            grabZ2(fBC1, fBC2, t0Time, Z2in, adcIn, fBC1Calib, fBC2Calib, Zin, true);
            grabZ2(fBC3, fBC4, t0Time, Z2out, adcOut, 0, 0, Zout, false);
            if (Z2out != -1000) {
                Z2out = sqrt(Z2out);
                Z2out = Z2out * fZCalib1 + fZCalib2;
                Z2out = Z2out * Z2out;
            }
            // cout<<Z2out;
        }
        fDstHeader->SetZ2in(Z2in);
        fDstHeader->SetZ2out(Z2out);
        fDstHeader->SetADCin(adcIn);
        fDstHeader->SetADCout(adcOut);
        fDstHeader->SetZin(Zin);
    } else if (fT0 && fBC1 && fBC2) {
        BmnTrigDigit* digT0 = NULL;
        Int_t t0Count = 0;
        for (Int_t i = 0; i < fT0->GetEntriesFast(); i++) {
            digT0 = (BmnTrigDigit*)fT0->At(i);
            if (digT0->GetMod() == 0)
                t0Count++;
        }
        if (t0Count == 1) {
            Double_t t0Time = digT0->GetTime();

            grabZ2(fBC1, fBC2, t0Time, Z2in, adcIn, fBC1Calib, fBC2Calib, Zin, true);
        }
        fDstHeader->SetZ2in(Z2in);
        //            fDstHead->SetZ2out(Z2out);
        fDstHeader->SetADCin(adcIn);
        //            fDstHead->SetADCout(adcOut);
        fDstHeader->SetZin(Zin);
    }

    Double_t Z1 = -100.0, Z2 = -100.0, Z3 = -100.0, Z4 = -100.0, ADC1 = -100.0, ADC2 = -100.0, ADC3 = -100.0,
             ADC4 = -100.0;
    if (fT0) {
        Int_t t0Count = 0;
        BmnTrigDigit* digT0 = NULL;
        Double_t t0Time = -100000;
        for (Int_t i = 0; i < fT0->GetEntriesFast(); i++) {
            digT0 = (BmnTrigDigit*)fT0->At(i);
            if (digT0->GetMod() == 0) {
                t0Count++;
                t0Time = digT0->GetTime();
            }
        }
        if (t0Count == 1) {
            if (fBC1 && fBC2) {
                grabZ2OR(fBC1, fBC2, t0Time, Z1, Z2, ADC1, ADC2, true);
            }
            if (fBC3 && fBC4) {
                grabZ2OR(fBC3, fBC4, t0Time, Z3, Z4, ADC3, ADC4, false);
            }
        }

        // no calibration from single adc to charge yet
        // fDstHeader->SetZ1(Z1);
        // fDstHeader->SetZ2(Z2);
        // fDstHeader->SetZ3(Z3);
        // fDstHeader->SetZ4(Z4);

        fDstHeader->SetADC1(ADC1);
        fDstHeader->SetADC2(ADC2);
        fDstHeader->SetADC3(ADC3);
        fDstHeader->SetADC4(ADC4);
    }

    // printing progress bar in terminal (hidden option now)
    if (fVerbose == -9999) {
        if (gROOT->IsBatch()) {
            if (fIEvent % 1000 == 0)
                printf("%lld/%lld\n", fIEvent, fNEvents);
        } else {
            UInt_t fRunId = (fDstHeader) ? fDstHeader->GetRunId() : 0;

            if (isExpData == 0)
                printf(ANSI_COLOR_RED "SIMULATION RUN" ANSI_COLOR_RESET);
            else {
                printf(ANSI_COLOR_BLUE "RUN-" ANSI_COLOR_RESET);
                printf(ANSI_COLOR_RED "%d" ANSI_COLOR_RESET, fRunId);
            }
            printf(ANSI_COLOR_BLUE ": [");

            Float_t progress = fIEvent * 1.0 / fNEvents;
            Int_t barWidth = 70;

            Int_t pos = barWidth * progress;
            for (Int_t i = 0; i < barWidth; ++i) {
                if (i < pos)
                    printf("=");
                else if (i == pos)
                    printf(">");
                else
                    printf(" ");
            }

            printf("] " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_RED "%d%%\r" ANSI_COLOR_RESET, Int_t(progress * 100.0 + 0.5));
            cout.flush();
        }
    } else {
        LOG(info) << "Event #" << fNStartEvent + fIEvent - 1;
        if (fPrimaryVertex) {
            TString strVertex = TString::Format("  vertex = %.3f:%.3f:%.3f", fPrimaryVertex->GetX(),
                                                fPrimaryVertex->GetY(), fPrimaryVertex->GetZ());
            LOG(info) << strVertex.Data();
        }
        if (fGlobalTracks)
            LOG(info) << "\tglobal tracks = " << fGlobalTracks->GetEntries();
    }
    fIEvent++;

    if (fIEvent == fNEvents)
        printf("\n");

    sw.Stop();
    workTime += sw.RealTime();
}

// ---- Finish --------------------------------------------------------
void BmnFillDstTask::Finish()
{
    LOG(debug) << "Finish of BmnFillDstTask";

    FairRootManager* ioman = FairRootManager::Instance();
    FairSink* fSink = ioman->GetSink();
    fSink->WriteObject(fRunHeader, "DstRunHeader", TObject::kSingleKey);

    printf("Work time of BmnFillDstTask: %4.2f sec.\n", workTime);
}

// ----  Load the parameter container from the runtime database  ------
void BmnFillDstTask::SetParContainers()
{
    LOG(debug) << "SetParContainers of BmnFillDstTask";
    // Load all necessary parameter containers from the runtime data base
    /*
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    <BmnFillDstTaskDataMember> = (<ClassPointer>*)
    (rtdb->getContainer("<ContainerName>"));
    */
}

void BmnFillDstTask::applyZCalibration()
{
    if (!fDoCalibration)
        return;

    // Read in the z-calibration file
    TString gPathWorkdir = gSystem->Getenv("VMCWORKDIR");
    TString gPathFullBC = gPathWorkdir + "/input/BC12Corrections.txt";
    ifstream fin(gPathFullBC.Data());
    int runBC = 0;
    int safeindex = 0;
    while (runBC != fRunNumber && safeindex != 20000) {
        fin >> runBC;
        fin >> fBC1Calib;
        fin >> fBC2Calib;
        safeindex = safeindex + 1;
    }
    if (fVerbose > 0)
        if (safeindex == 20000)
            cout << "run number not found in file " << gPathFullBC << endl;

    TString gPathFull = gPathWorkdir + "/input/ZOutCorrections5.txt";

    string line;
    ifstream f(gPathFull.Data(), ios::in);
    vector<Double_t> axisAttr;

    while (!f.eof()) {
        getline(f, line);

        TString currString(line);
        int run;
        TString str_run(currString(0, 4));
        run = str_run.Atoi();

        if (run == fRunNumber) {
            //	cout<<"++++++++++++FILLDSTTASK!!!!"<<endl;
            cout << currString.Data() << endl;
            TString ab(currString(5, currString.Length()));
            TString a(ab(0, ab.First(" ")));
            // float a_float;
            fZCalib1 = a.Atof();
            TString c(ab(a.Length() + 1, ab.Length()));
            // cout<<"c = "<<c.Data()<<endl;
            TString b(c(0, c.First(" ")));
            fZCalib2 = b.Atof();
            // cout<<"ab = "<<ab.Data()<<", a = "<<fZCalib1<<", b = "<<fZCalib2<<endl;
        }
    }
    // f.close();
}

void BmnFillDstTask::InitParticleInfo()
{
    stParticleInfo particle_info;
    particle_info.A = 2;
    particle_info.Z = 1;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("d", particle_info));
    particle_info.A = 12;
    particle_info.Z = 6;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("c", particle_info));
    particle_info.A = 40;
    particle_info.Z = 18;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("ar", particle_info));
    particle_info.A = 84;
    particle_info.Z = 36;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("kr", particle_info));
    particle_info.A = 27;
    particle_info.Z = 13;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("al", particle_info));
    particle_info.A = 207;
    particle_info.Z = 82;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("pb", particle_info));
    particle_info.A = 119;
    particle_info.Z = 50;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("sn", particle_info));
    particle_info.A = 64;
    particle_info.Z = 29;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("cu", particle_info));
    particle_info.A = 1;
    particle_info.Z = 1;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("h2", particle_info));
    particle_info.A = 29;
    particle_info.Z = 0;
    mapParticleInfo.insert(pair<TString, stParticleInfo>("c2h4", particle_info));

    return;
}
