/********************************************************************************
 *    BmnFillDstTask.h                                                          *
 *    BM@N Fill DST Task class implementation                                   *
 *    Fill Event Headers in the DST file after reconstruction                   *
 *    Konstantin Gertsenberger                                                  *
 *    Created: Apr. 25 2019                                                     *
 *******************************************************************************/

#define ANSI_COLOR_RED "\x1b[91m"
#define ANSI_COLOR_BLUE "\x1b[94m"
#define ANSI_COLOR_RESET "\x1b[0m"

#include "BmnFillDstTask.h"
#include "ExtractZ2.h"
//#include "BmnTofCalDigit.h"
#include "BmnTrigDigit.h"
#include "BmnTrigWaveDigit.h"
#include "UniRun.h"
#include "function_set.h"

#include "FairLogger.h"

#include "TDirectory.h"
#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"
#include <TStopwatch.h>

#include <fstream>
#include <iostream>
#include <cstring>

static Double_t workTime = 0.0;
using namespace std;


// ---- Default constructor -------------------------------------------
BmnFillDstTask::BmnFillDstTask() : FairTask("BmnFillDstTask"),
                                   fInputEventHeaderName("BmnEventHeader."),
                                   fOutputEventHeaderName("DstEventHeader."),
                                   fEventHead(NULL),
                                   fDstHead(NULL),
                                   fNEvents(-1),
                                   fIEvent(0),
                                   fPeriodNumber(-1),
                                   fRunNumber(-1),
                                   fZCalib1(1),
                                   fZCalib2(0),
                                   fBC1Calib(0),
                                   fBC2Calib(0),
                                   fDoCalibration(kFALSE),
                                   isSimulationInput(false) {
    LOG(debug) << "Defaul Constructor of BmnFillDstTask";
}

// ---- Constructor with the given event number to be processed -------
BmnFillDstTask::BmnFillDstTask(Long64_t nEvents) : FairTask("BmnFillDstTask"),
                                                   fInputEventHeaderName("BmnEventHeader."),
                                                   fOutputEventHeaderName("DstEventHeader."),
                                                   fEventHead(NULL),
                                                   fDstHead(NULL),
                                                   fNEvents(nEvents),
                                                   fIEvent(0),
                                                   fPeriodNumber(-1),
                                                   fRunNumber(-1),
                                                   fZCalib1(1),
                                                   fZCalib2(0),
                                                   fBC1Calib(0),
                                                   fBC2Calib(0),
                                                   fDoCalibration(kFALSE),
                                                   isSimulationInput(false) {
    fRunHead = new DstRunHeader();
    LOG(debug) << "Constructor of BmnFillDstTask";
}

// Constructor with input Event Header Name and event number to be processed
BmnFillDstTask::BmnFillDstTask(TString input_event_header_name, Long64_t nEvents) : FairTask("BmnFillDstTask"),
                                                                                    fInputEventHeaderName(input_event_header_name),
                                                                                    fOutputEventHeaderName("DstEventHeader."),
                                                                                    fEventHead(NULL),
                                                                                    fDstHead(NULL),
                                                                                    fNEvents(nEvents),
                                                                                    fIEvent(0),
                                                                                    fPeriodNumber(-1),
                                                                                    fRunNumber(-1),
                                                                                    fZCalib1(1),
                                                                                    fZCalib2(0),
                                                                                    fBC1Calib(0),
                                                                                    fBC2Calib(0),
                                                                                    fDoCalibration(kFALSE),
                                                                                    isSimulationInput(false) {
    fRunHead = new DstRunHeader();
    LOG(debug) << "Constructor of BmnFillDstTask";
}

// Constructor with input and output Event Header Name, and event number to be processed
BmnFillDstTask::BmnFillDstTask(TString input_event_header_name, TString output_event_header_name, Long64_t nEvents) : FairTask("BmnFillDstTask"),
                                                                                                                      fInputEventHeaderName(input_event_header_name),
                                                                                                                      fOutputEventHeaderName(output_event_header_name),
                                                                                                                      fEventHead(NULL),
                                                                                                                      fDstHead(NULL),
                                                                                                                      fNEvents(nEvents),
                                                                                                                      fIEvent(0),
                                                                                                                      fPeriodNumber(-1),
                                                                                                                      fRunNumber(-1),
                                                                                                                      fZCalib1(1),
                                                                                                                      fZCalib2(0),
                                                                                                                      fBC1Calib(0),
                                                                                                                      fBC2Calib(0),
                                                                                                                      fDoCalibration(kFALSE),
                                                                                                                      isSimulationInput(false) {
    fRunHead = new DstRunHeader();
    LOG(debug) << "Constructor of BmnFillDstTask";
}

// ---- Destructor ----------------------------------------------------
BmnFillDstTask::~BmnFillDstTask() {
    LOG(debug) << "Destructor of BmnFillDstTask";
}

// ----  Initialisation  ----------------------------------------------
void BmnFillDstTask::SetParContainers() {
    LOG(debug) << "SetParContainers of BmnFillDstTask";
    // Load all necessary parameter containers from the runtime data base
    /*
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    <BmnFillDstTaskDataMember> = (<ClassPointer>*)
    (rtdb->getContainer("<ContainerName>"));
    */
}

// ---- Init ----------------------------------------------------------
InitStatus BmnFillDstTask::Init() {
    LOG(debug) << "Initilization of BmnFillDstTask";

    // Get a handle from the IO manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        LOG(error) << "Init: FairRootManager is not instantiated!\n"
                   << "BmnFillDstTask will be inactive";
        return kERROR;
    }

    // Get a pointer to the input Event Header
    TObject* pObj = ioman->GetObject(fInputEventHeaderName);
    if (!pObj) {
        // if no input Event Header was found, searching for "MCEventHeader."
        fMCEventHead = (FairMCEventHeader*)ioman->GetObject("MCEventHeader.");
        if (!fMCEventHead) {
            LOG(error) << "No input Event Header (" << fInputEventHeaderName << " or MCEventHeader.) was found!\n"
                       << "BmnFillDstTask will be inactive!";
            return kERROR;
        }
        isSimulationInput = true;
    } else {
        if (pObj->InheritsFrom(FairMCEventHeader::Class())) {
            fMCEventHead = (FairMCEventHeader*)pObj;
            isSimulationInput = true;
        } else
            fEventHead = (BmnEventHeader*)pObj;
    }
    if (fDoCalibration) {
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
            if (safeindex == 20000) cout << "run number not found in file " << gPathFullBC << endl;

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
                float a_float;
                fZCalib1 = a.Atof();
                TString c(ab(a.Length() + 1, ab.Length()));
                //cout<<"c = "<<c.Data()<<endl;
                TString b(c(0, c.First(" ")));
                fZCalib2 = b.Atof();
                //cout<<"ab = "<<ab.Data()<<", a = "<<fZCalib1<<", b = "<<fZCalib2<<endl;
            }
        }
        //f.close();
    }

    //Get input branches
    fT01_1 = (TClonesArray*)ioman->GetObject("T0_1_A");
    fT01_2 = (TClonesArray*)ioman->GetObject("T0_2_A");
    fBC1_1 = (TClonesArray*)ioman->GetObject("TQDC_T0_1_A");
    fBC1_2 = (TClonesArray*)ioman->GetObject("TQDC_T0_2_A");
    fBC2_1 = (TClonesArray*)ioman->GetObject("TQDC_BC2_1_A");
    fBC2_2 = (TClonesArray*)ioman->GetObject("TQDC_BC2_2_A");
    fBC3_1 = (TClonesArray*)ioman->GetObject("TQDC_BC3_1");
    fBC3_2 = (TClonesArray*)ioman->GetObject("TQDC_BC3_2");
    fBC3_1_X10 = (TClonesArray*)ioman->GetObject("TQDC_BC3_1_X10");
    fBC3_2_X10 = (TClonesArray*)ioman->GetObject("TQDC_BC3_2_X10");
    fBC3_S = (TClonesArray*)ioman->GetObject("TQDC_BC3_SUM");
    fBC4_1 = (TClonesArray*)ioman->GetObject("TQDC_BC4_1");
    fBC4_2 = (TClonesArray*)ioman->GetObject("TQDC_BC4_2");
    fBC4_1_X10 = (TClonesArray*)ioman->GetObject("TQDC_BC4_1_X10");
    fBC4_2_X10 = (TClonesArray*)ioman->GetObject("TQDC_BC4_2_X10");
    fBC4_S = (TClonesArray*)ioman->GetObject("TQDC_BC4_SUM");
    fBC5_1 = (TClonesArray*)ioman->GetObject("TQDC_BC5_1");
    fBC5_2 = (TClonesArray*)ioman->GetObject("TQDC_BC5_2");
    fBC5_1_X10 = (TClonesArray*)ioman->GetObject("TQDC_BC5_1_X10");
    fBC5_2_X10 = (TClonesArray*)ioman->GetObject("TQDC_BC5_2_X10");
    fBC5_S = (TClonesArray*)ioman->GetObject("TQDC_BC5_SUM");
    
    fVeto = (TClonesArray*)ioman->GetObject("TQDC_VETO");

    // new branches with TofCal
    fTofCal = (TClonesArray*)ioman->GetObject("TofCal");


     
    // Get a pointer to the output DST Event Header
    fDstHead = (DstEventHeader*)ioman->GetObject(fOutputEventHeaderName);
    
    if (!fDstHead) {
        LOG(error) << "No Event Header(" << fOutputEventHeaderName << ") prepared for the output DST file!\n"
                   << "BmnFillDstTask will be inactive";
        return kERROR;
    }
    
    

    // Create the TClonesArray for the output data and register
    // it in the IO manager
    /*
    <OutputDataLevel> = new TClonesArray("OutputDataLevelName", 100);
    ioman->Register("OutputDataLevelName","OutputDataLevelName",<OutputDataLevel>,kTRUE);
    */

    if (ioman->CheckMaxEventNo(fNEvents) < fNEvents)
        fNEvents = ioman->CheckMaxEventNo(fNEvents);

    // Fill Run Header from the Database
    if (fRunNumber > 0) {
        InitParticleInfo();

        UniRun* pCurrentRun = UniRun::GetRun(fPeriodNumber, fRunNumber);
        if (pCurrentRun != 0) {
            fRunHead->SetPeriodRun(fPeriodNumber, fRunNumber);
            TDatime start_date = pCurrentRun->GetStartDatetime();
            TTimeStamp tsStartTime(start_date.GetYear(), start_date.GetMonth(), start_date.GetDay(),
                                   start_date.GetHour(), start_date.GetMinute(), start_date.GetSecond());
            fRunHead->SetStartTime(tsStartTime);
            TDatime* end_date  = pCurrentRun->GetEndDatetime();
            if (end_date  != NULL) {
                
                //TTimeStamp tsEndTime((time_t) pCurrentRun->GetEndDatetime()->Convert(), 0);
                
                TTimeStamp tsEndTime(end_date->GetYear(), end_date->GetMonth(), end_date->GetDay(),
                                     end_date->GetHour(), end_date->GetMinute(), end_date->GetSecond());
                fRunHead->SetFinishTime(tsEndTime);
            }
            delete end_date;
            TString beam = pCurrentRun->GetBeamParticle();
            beam.ToLower();
            map<TString, stParticleInfo>::iterator it = mapParticleInfo.find(beam);
            if (it != mapParticleInfo.end()) {
                fRunHead->SetBeamA(it->second.A);
                fRunHead->SetBeamZ(it->second.Z);
            }
            double* energy = pCurrentRun->GetEnergy();
            if (energy != NULL){
                fRunHead->SetBeamEnergy(*energy);
                delete energy;
            }
                
            TString* target = pCurrentRun->GetTargetParticle();
            if (target != NULL) {
                target->ToLower();
                it = mapParticleInfo.find(*target);
                if (it != mapParticleInfo.end()) {
                    fRunHead->SetTargetA(it->second.A);
                    fRunHead->SetTargetZ(it->second.Z);
                }
                delete target;
            }
            Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
            if (field_voltage != NULL){
                fRunHead->SetMagneticField(*field_voltage);
                delete field_voltage;
            }
        }  // if (pCurrentRun != 0)
        delete pCurrentRun;
    }      // FIll Run Header from the Database


    return kSUCCESS;
}

// ---- ReInit  -------------------------------------------------------
InitStatus BmnFillDstTask::ReInit() {
    LOG(debug) << "Re-initilization of BmnFillDstTask";
    return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------
void BmnFillDstTask::Exec(Option_t* /*option*/) {
    
    TStopwatch sw;
    sw.Start();
    
    LOG(debug) << "Exec of BmnFillDstTask";

    // fill output DST event header
    if (isSimulationInput) {
        fDstHead->SetRunId(fMCEventHead->GetRunID());
        fDstHead->SetEventId(fMCEventHead->GetEventID());
        fDstHead->SetEventTime(fMCEventHead->GetT());
        fDstHead->SetEventTimeTS((TTimeStamp)convert_double_to_timespec(fMCEventHead->GetT()));
        fDstHead->SetB(fMCEventHead->GetB());
    } else {
        fDstHead->SetRunId(fEventHead->GetRunId());
        fDstHead->SetEventId(fEventHead->GetEventId());
        fDstHead->SetEventTime(fEventHead->GetEventTime());
        fDstHead->SetEventTimeTS(fEventHead->GetEventTimeTS());
    }


    //calculate Z2in and Z2out:
    Double_t BC1_12 = -100.0, BC2_12 = -100.0;
    Double_t BC3_12 = -100.0, BC3_12_X10 = -100.0, BC3_S = -100.0;
    Double_t BC4_12 = -100.0, BC4_12_X10 = -100.0, BC4_S = -100.0;
    Double_t BC5_12 = -100.0, BC5_12_X10 = -100.0, BC5_S = -100.0;
    Double_t Veto_T = -100.0;
    Double_t BC1_cor = -100.0, BC2_cor = -100.0; //cor
    Double_t BC3_cor = -100.0;
    Double_t BC4_cor = -100.0;
    Double_t BC5_cor = -100.0;
    
    Short_t ZinBC12 = -100;
    Short_t ZinBC12_cor = -100; //cor
    //Short_t ZinBC2 = -100;
    Short_t ZoutBC34_12 = -100;
    Short_t ZoutBC34_12_cor = -100; //cor
    Short_t ZoutBC34_S = -100;
    Short_t ZoutBC35_12 = -100;
    Short_t ZoutBC35_12_cor = -100; //cor
    Short_t ZoutBC35_S = -100;
    Short_t ZoutBC45_12 = -100;
    Short_t ZoutBC45_12_cor = -100; //cor
    Short_t ZoutBC45_S = -100;
    Short_t Veto = -1;
    BmnTrigDigit* digT01 = NULL;
    BmnTrigDigit* digT02 = NULL;
    Int_t t0Count1 = 0;
    Int_t t0Count2 = 0;
    Double_t unused=0;
    Short_t unused1=0;


    // new TofCal
    vector<Double_t> TofCal_Time0, TofCal_Time1, TofCal_Amp0, TofCal_Amp1, TofCal_X, TofCal_Y;
    vector<Int_t> TofCal_Bar, TofCal_Plane, TofCal_Arm, TofCal_GlobalBar;

    // new BC
    BmnTrigWaveDigit* BC1_Time0 = NULL, *BC1_Time1 = NULL, *BC2_Time0 = NULL, *BC2_Time1 = NULL;
    
    // trigger information
    Int_t trig_scaler_af[14];
    Bool_t trig_tpat_af[14];
	  for(Int_t i=0; i<14; i++){
    		trig_scaler_af[i] = 0;
    		trig_tpat_af[i] = kFALSE;
    }
   // BmnEventHeader *headerData = new BmnEventHeader();
    UInt_t tpat_af = fEventHead->GetInputSignalsAR(); //after reduction
    //Selection 2^X =
			//0 	SRC
			//1 	Recoil
			//2 	ARM-AND
			//3 	ARM-OR
			//4 	SRC-OR
			//5 	FRAG
			//6 	GBEAM
			//7 	BT
			//8 	BT&NBSY
			//9 	BC1L
			//10 	-
			//11 	LASER
			//12 	-
			//13 	arm-or-test 
			//
			//cout << i << "  " << tpat_bf << endl;
     //TString trigger ("");
     //TString tr[14]={"SRC  ", "Recoil  ", "ARM-AND  ", "ARM-OR  ", "SRC-OR  ", "FRAG  ", "GBEAM  ", "BT  ", "BT&NBSY  ", "BC1L  ", "  ", "LASER  ", "  ", "arm-or-test  "};
     int trigger=tpat_af;
			for(Int_t t=0; t<14; t++){
			
				if( tpat_af & UInt_t(pow(2,t)) ){
					trig_scaler_af[t]++;
					trig_tpat_af[t] = kTRUE;
					//trigger = trigger + tr[t];
				//	h_tpat_af->Fill(t);
				}
			}
		
    // exclude laser trigger events
    //cout<<trigger<<"  "<<tpat_af<<"  "<<fEventHead->GetEventId()<<"  "<<endl;
    fDstHead->SetTriger(trigger);
    //fDstHead->SetScaler(trig_scaler_af);
    /*if(trig_tpat_af[11]!=1) {*/

//    for (UInt_t i = 0; i < fVeto->GetEntriesFast(); i++) {  
//      BmnTrigWaveDigit *veto = (BmnTrigWaveDigit *)fVeto->At(i);
//      if (veto->GetTime()<0) Veto=0;
//      if (veto->GetTime()>0 && Veto!=0) Veto=1;
//      //else Veto=0;
//    }

      
    Double_t t0Time1=-1; 
    Double_t t0Time2=-1; 
    short int charge; 
    for (UInt_t i = 0; i < fT01_1->GetEntriesFast(); i++) {
        digT01 = (BmnTrigDigit*)fT01_1->At(i);
        if (digT01->GetMod() == 0) t0Count1++;
        if (t0Count1==1) t0Time1 = digT01->GetTime();
    }
    for (UInt_t i = 0; i < fT01_2->GetEntriesFast(); i++) {
        digT02 = (BmnTrigDigit*)fT01_2->At(i);
        if (digT02->GetMod() == 0) t0Count2++;
        if (t0Count2==1) t0Time2 = digT02->GetTime();
    }
       
        grabZ2V(fVeto, t0Time1, Veto_T);  
        if (Veto_T>0) Veto=1;
        else Veto=0;   
        
            if (t0Time1>0 && t0Time2>0) {

              grabZ2(fBC3_1_X10, fBC3_2_X10, fBC4_1_X10, fBC4_2_X10, t0Time1, t0Time2, BC3_12_X10, BC4_12_X10, fBC1Calib, fBC2Calib, charge, 34);
              grabZ2(fBC3_1_X10, fBC3_2_X10, fBC5_1_X10, fBC5_2_X10, t0Time1, t0Time2, BC3_12_X10, BC5_12_X10, fBC1Calib, fBC2Calib, charge, 35);
              grabZ2(fBC4_1_X10, fBC4_2_X10, fBC5_1_X10, fBC5_2_X10, t0Time1, t0Time2, BC4_12_X10, BC5_12_X10, fBC1Calib, fBC2Calib, charge, 45);
              grabZ2(fBC1_1, fBC1_2, fBC2_1, fBC2_2, t0Time1, t0Time2, BC1_12, BC2_12, fBC1Calib, fBC2Calib, ZinBC12, 12);
              grabZ2(fBC3_1, fBC3_2, fBC4_1, fBC4_2, t0Time1, t0Time2, BC3_12, BC4_12, fBC1Calib, fBC2Calib, ZoutBC34_12, 34);
              grabZ2(fBC3_1, fBC3_2, fBC5_1, fBC5_2, t0Time1, t0Time2, BC3_12, BC5_12, fBC1Calib, fBC2Calib, ZoutBC35_12, 35);
              grabZ2(fBC4_1, fBC4_2, fBC5_1, fBC5_2, t0Time1, t0Time2, BC4_12, BC5_12, fBC1Calib, fBC2Calib, ZoutBC45_12, 45);

              grabZ2S(fBC3_S, fBC4_S, t0Time1, t0Time2, BC3_S, BC4_S, fBC1Calib, fBC2Calib, ZoutBC34_S, 34);
              grabZ2S(fBC3_S, fBC5_S, t0Time1, t0Time2, BC3_S, BC5_S, fBC1Calib, fBC2Calib, ZoutBC35_S, 35);
              grabZ2S(fBC4_S, fBC5_S, t0Time1, t0Time2, BC4_S, BC5_S, fBC1Calib, fBC2Calib, ZoutBC45_S, 45);
             
              
            }

        // new part with BC, TofCal
        
        Short_t BC1_T0 = -1, BC1_T1 = -1, BC2_T0 = -1, BC2_T1 = -1;
        if (fBC1_1->GetEntriesFast() > 0) {
            BC1_Time0 = (BmnTrigWaveDigit*)fBC1_1->At(0);
            BC1_T0 = BC1_Time0->GetTime();
        }

        if (fBC1_2->GetEntriesFast() > 0) {
            BC1_Time1 = (BmnTrigWaveDigit*)fBC1_2->At(0);
            BC1_T1 = BC1_Time1->GetTime();
        }

        if (fBC2_1->GetEntriesFast() > 0) {
            BC2_Time0 = (BmnTrigWaveDigit*)fBC2_1->At(0);
            BC2_T0 = BC2_Time0->GetTime();
        }

        if (fBC2_2->GetEntriesFast() > 0) {
            BC2_Time1 = (BmnTrigWaveDigit*)fBC2_2->At(0);
            BC2_T1 = BC2_Time1->GetTime();
        }

        
        if (trig_tpat_af[11]!=1) {fDstHead->SetIs_laser(0);}
        else {fDstHead->SetIs_laser(1);}
        //fDstHead->SetIs_laser(Is_laser);
    

        fDstHead->SetBC1_Time0(BC1_T0);
        fDstHead->SetBC1_Time1(BC1_T1);
        fDstHead->SetBC2_Time0(BC2_T0);
        fDstHead->SetBC2_Time1(BC2_T1);

        Int_t TofCal_nHits = 0;
        Int_t nTofCal = fTofCal->GetEntriesFast();
        for(Int_t i = 0; i < nTofCal; i++){
                BmnTofCalDigit* tofCal = (BmnTofCalDigit *)fTofCal->At(i);
                TofCal_Bar.push_back(tofCal->GetBar());
                TofCal_Plane.push_back(tofCal->GetPlane());
                TofCal_Time0.push_back(tofCal->GetTime(0));
                TofCal_Time1.push_back(tofCal->GetTime(1));
                TofCal_Amp0.push_back(tofCal->GetEnergy(0));
                TofCal_Amp1.push_back(tofCal->GetEnergy(1));
                TofCal_X.push_back(tofCal->GetX());
                TofCal_Y.push_back(tofCal->GetY());

                TofCal_Arm.push_back(tofCal->GetArm());
                TofCal_GlobalBar.push_back(tofCal->GetGlobBar());

                TofCal_nHits = TofCal_nHits+1;
        }


                fDstHead->SetTofCal_Bar(TofCal_Bar);
                fDstHead->SetTofCal_Plane(TofCal_Plane);
                fDstHead->SetTofCal_Time0(TofCal_Time0);
                fDstHead->SetTofCal_Time1(TofCal_Time1);
                fDstHead->SetTofCal_Amp0(TofCal_Amp0);
                fDstHead->SetTofCal_Amp1(TofCal_Amp1);
                fDstHead->SetTofCal_X(TofCal_X);
                fDstHead->SetTofCal_Y(TofCal_Y);

                fDstHead->SetTofCal_Arm(TofCal_Arm);
                fDstHead->SetTofCal_GlobalBar(TofCal_GlobalBar);

                fDstHead->SetTofCal_nHits(TofCal_nHits);
  

        const int runId = fDstHead ? (int)fDstHead->GetRunId() : -1;  //cor

        double amp_BC1 = 0.0;
        double amp_BC2 = 0.0;
        double amp_BC3 = 0.0;
        double amp_BC4 = 0.0;
        double amp_BC5 = 0.0;
        int Run = 0;

        TString gPathWorkdir1 = gSystem->Getenv("VMCWORKDIR");
        TString gPathamp_run_coef = gPathWorkdir1 + "/input/output_approx_values_all_BC.txt";
        std::ifstream in(gPathamp_run_coef.Data(), ios::in);

        if (!in.is_open()) {
            std::cerr << "Couldn't open the file with BC amplitudes" << std::endl;
        } else {
            std::string header;
            std::getline(in, header); 

            while (in >> Run >> amp_BC1 >> amp_BC2 >> amp_BC3 >> amp_BC4 >> amp_BC5) {
                if (Run == runId) break;
            }

            in.close();
        }

        BC1_cor = 0.0;
        BC2_cor = 0.0;
        BC3_cor = 0.0;
        BC4_cor = 0.0;
        BC5_cor = 0.0;

        if (amp_BC1 > 0.0) BC1_cor = BC1_12 * 2000.0 / amp_BC1;
        if (amp_BC2 > 0.0) BC2_cor = BC2_12 * 2000.0 / amp_BC2;
        if (amp_BC3 > 0.0) BC3_cor = BC3_12 * 4500.0 / amp_BC3;
        if (amp_BC4 > 0.0) BC4_cor = BC4_12 * 4500.0 / amp_BC4;
        if (amp_BC5 > 0.0) BC5_cor = BC5_12 * 4500.0 / amp_BC5;


        if (t0Time1>0 && t0Time2>0) {
            grabZ2_cor(fBC1_1, fBC1_2, fBC2_1, fBC2_2, t0Time1, t0Time2, BC1_cor, BC2_cor, fBC1Calib, fBC2Calib, ZinBC12_cor, 12);
            grabZ2_cor(fBC3_1, fBC3_2, fBC4_1, fBC4_2, t0Time1, t0Time2, BC3_cor, BC4_cor, fBC1Calib, fBC2Calib, ZoutBC34_12_cor, 34);
            grabZ2_cor(fBC3_1, fBC3_2, fBC5_1, fBC5_2, t0Time1, t0Time2, BC3_cor, BC5_cor, fBC1Calib, fBC2Calib, ZoutBC35_12_cor, 35);
            grabZ2_cor(fBC4_1, fBC4_2, fBC5_1, fBC5_2, t0Time1, t0Time2, BC4_cor, BC5_cor, fBC1Calib, fBC2Calib, ZoutBC45_12_cor, 45);
        }
        //cout<<"BC1_cor = "<<BC1_cor<<"\tBC2_cor = "<<BC2_cor<<"\tBC3_cor = "<<BC3_cor<<"\tBC4_cor = "<<BC4_cor<<"\tBC5_cor = "<<BC5_cor<<endl;
        //cout<<"ZinBC12_cor = "<<ZinBC12_cor<<"\tZoutBC34_12_cor = "<<ZoutBC34_12_cor<<"\tZoutBC35_12_cor = "<<ZoutBC35_12_cor<<"\tZoutBC45_12_cor = "<<ZoutBC45_12_cor<<endl;

        fDstHead->SetBC1_cor(BC1_cor);
        fDstHead->SetBC2_cor(BC2_cor);
        fDstHead->SetBC3_cor(BC3_cor);
        fDstHead->SetBC4_cor(BC4_cor);
        fDstHead->SetBC5_cor(BC5_cor);

        fDstHead->SetBC1_12(BC1_12);
        fDstHead->SetBC2_12(BC2_12);
        fDstHead->SetBC3_12(BC3_12);
        fDstHead->SetBC4_12(BC4_12);
        fDstHead->SetBC5_12(BC5_12);
        fDstHead->SetBC3_12_X10(BC3_12_X10);
        fDstHead->SetBC4_12_X10(BC4_12_X10);
        fDstHead->SetBC5_12_X10(BC5_12_X10);
        fDstHead->SetBC3_S(BC3_S);
        fDstHead->SetBC4_S(BC4_S);
        fDstHead->SetBC5_S(BC5_S);
        fDstHead->SetZinBC12(ZinBC12);
        fDstHead->SetZoutBC34_12(ZoutBC34_12);
        fDstHead->SetZoutBC35_12(ZoutBC35_12);
        fDstHead->SetZoutBC45_12(ZoutBC45_12);
        fDstHead->SetZinBC12_cor(ZinBC12_cor);
        fDstHead->SetZoutBC34_12_cor(ZoutBC34_12_cor);
        fDstHead->SetZoutBC35_12_cor(ZoutBC35_12_cor);
        fDstHead->SetZoutBC45_12_cor(ZoutBC45_12_cor);
        fDstHead->SetZoutBC34_S(ZoutBC34_S);
        fDstHead->SetZoutBC45_S(ZoutBC45_S);
        fDstHead->SetZoutBC35_S(ZoutBC35_S);
        fDstHead->SetVeto(Veto);
        
  /*}*/ // laser exclude

   TofCal_Bar.clear(); TofCal_Plane.clear();
   TofCal_Time0.clear(); TofCal_Time1.clear();
   TofCal_Amp0.clear(); TofCal_Amp1.clear();
   TofCal_X.clear(); TofCal_Y.clear();
   TofCal_Arm.clear(); TofCal_GlobalBar.clear();

     // printing progress bar in terminal
    if (fVerbose == 0) {
        if (gROOT->IsBatch()) {
            if (fIEvent % 1000 == 0)
                printf("%lld/%lld\n", fIEvent, fNEvents);
        } else {
            UInt_t fRunId = (fDstHead) ? fDstHead->GetRunId() : 0;

            if (isSimulationInput)
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
        cout << "Event #" << fIEvent << endl;
    }
    fIEvent++;
    
    if (fIEvent == fNEvents) printf("\n");

    sw.Stop();
    workTime += sw.RealTime();
}

// ---- Finish --------------------------------------------------------
void BmnFillDstTask::Finish() {
    LOG(debug) << "Finish of BmnFillDstTask";

    FairRootManager* ioman = FairRootManager::Instance();
    FairSink* fSink = ioman->GetSink();
    fSink->WriteObject(fRunHead, "DstRunHeader", TObject::kSingleKey);

    printf("Work time of BmnFillDstTask: %4.2f sec.\n", workTime);
}

void BmnFillDstTask::InitParticleInfo() {
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

ClassImp(BmnFillDstTask)
