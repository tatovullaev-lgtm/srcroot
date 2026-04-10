#if !defined(__CLING__) || defined(__MAKECLING__)
// ROOT includes
#include "TApplication.h"
#include "TDatabasePDG.h"
#include "TGeoManager.h"
#include "TKey.h"
#include "TPRegexp.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TSystem.h"
#include "TVirtualMC.h"

// FairRoot includes
#include "FairBoxGenerator.h"
#include "FairCave.h"
#include "FairDetector.h"
#include "FairIonGenerator.h"
#include "FairMagnet.h"
#include "FairModule.h"
#include "FairParRootFileIo.h"
#include "FairParticleGenerator.h"
#include "FairPipe.h"
#include "FairPrimaryGenerator.h"
#include "FairRootFileSink.h"
#include "FairRunSim.h"
#include "FairRuntimeDb.h"
#include "FairTarget.h"
#include "FairTrajFilter.h"

// BmnRoot includes
#include "BmnBd.h"
#include "BmnBdDigitizer.h"
#include "BmnCSC.h"
#include "BmnCSCConfiguration.h"
#include "BmnCSCDigitizer.h"
#include "BmnDch.h"
#include "BmnEcal.h"
#include "BmnEcalDigitizer.h"
#include "BmnFD.h"
#include "BmnFHCal.h"
#include "BmnFHCalDigitizer.h"
#include "BmnFieldConst.h"
#include "BmnFieldMap.h"
#include "BmnFieldPar.h"
#include "BmnFunctionSet.h"
#include "BmnGemStripDigitizer.h"
#include "BmnGemStripMedium.h"
#include "BmnHodo.h"
#include "BmnHodoDigitizer.h"
#include "BmnMwpc.h"
#include "BmnNdet.h"
#include "BmnNdetDigitizer.h"
#include "BmnNewFieldMap.h"
#include "BmnScWall.h"
#include "BmnScWallDigitizer.h"
// #include "BmnVacuumWallDigitizer.h"
#include "BmnSiBT.h"
#include "BmnSiBTConfiguration.h"
#include "BmnSiBTDigitizer.h"
#include "BmnSiMD.h"
#include "BmnSiMDDigitizer.h"
#include "BmnSiProfDigitizer.h"
#include "BmnSilicon.h"
#include "BmnSiliconDigitizer.h"
#include "BmnTOF.h"
#include "BmnTOF1.h"
#include "BmnTOF701.h"
#include "BmnVacWall.h"
#include "BmnZdc.h"
#include "BmnZdcDigitizer.h"
#include "CbmStack.h"
#include "CbmSts.h"
#include "MpdDCMSMMGenerator.h"
#include "MpdGetNumEvents.h"
#include "MpdLAQGSMGenerator.h"
#include "MpdPHSDGenerator.h"
#include "MpdUnigenGenerator.h"
#include "MpdUrqmdGenerator.h"
#include "UniRun.h"

// GEANT3 and 4 includes
#include "TG4RunConfiguration.h"
#include "TGeant4.h"
#include "TPythia6Decayer.h"
#include "TVirtualMC.h"
// #include "TGeant3.h"
// #include "TGeant3TGeo.h"

#include "../../gconfig/g4Config.C"
// #include "../../gconfig/g3Config.C"
#include "../../gconfig/SetCuts.C"

// C++ includes
#include <iostream>
using namespace std;
#endif

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "../run/geometry_run/geometry_run8.C"

void geant3_setup()
{
    Config();
    SetCuts();
}
void geant4_setup()
{
    Config();
}

#define GEANT4   // Choose: GEANT3 GEANT4
// enumeration of generator names corresponding input files
enum enumGenerators
{
    UNIGEN,
    URQMD,
    QGSM,
    HSD,
    BOX,
    PART,
    ION,
    DCMQGSM,
    DCMSMM
};

// genFile - input file with generator data, if needed
// simFile - output file with MC data, default: bmnsim.root
// nStartEvent - start event in the input generator file to begin transporting, default: 0
// nEvents - number of events to transport
// generatorName - generator name for the input file (enumeration above)
// useRealEffects - whether we use realistic effects at simulation (Lorentz, misalignment)
// saveGeoInfo - whether add 'GeoTracks' to simFile for Event Display and create "full_geometry.root" with full setup
// geometry
void run_sim_bmn(TString genFile = "DCMSMM_XeCsI_3.9AGeV_mb_10k_142.r12",
                 TString simFile = "$VMCWORKDIR/macro/run/bmnsim.root",
                 Int_t nStartEvent = 0,
                 Int_t nEvents = 10,
                 enumGenerators generatorName = BOX,
                 Bool_t useRealEffects = kTRUE,
                 Bool_t saveGeoInfo = kFALSE)
{
    gDebug = 0;   // ROOT debug option
    // Verbosity level (0 = summary only, 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 0;
    // Log level for LOG(severity): "fatal", "error", "alarm", "important", "warn", "state", "info", "detail", "debug",
    // "debug[1-4]", "trace", "nolog"
    FairLogger::GetLogger()->SetLogScreenLevel("info");

    TStopwatch timer;
    timer.Start();

    // -----   Create simulation run   ----------------------------------------
    FairRunSim* fRun = new FairRunSim();
    fRun->SetUseFairLinks(kTRUE);

    // Choose the Geant Navigation System
#ifdef GEANT4
    fRun->SetName("TGeant4");
#else
    fRun->SetName("TGeant3");
#endif

    geometry(fRun);   // load BM@N geometry

    // Use the experiment specific MC Event header instead of the default one
    // This one stores additional information about the reaction plane
    // MpdMCEventHeader* mcHeader = new MpdMCEventHeader();
    // fRun->SetMCEventHeader(mcHeader);

    // Create and Set Event Generator
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    // Smearing of beam interaction point, if needed, and primary vertex position
    // DO NOT do it in corresponding gen. sections to avoid incorrect summation!!!
    // parameters for period 8
    primGen->SetBeam(0.4376, 0.1239, 0.4537, 0.6925);   // (beamX0, beamY0, beamSigmaX, beamSigmaY)
    primGen->SetBeamAngle(0.011, 0.0, 0.0, 0.0);        // (beamAngleX0, beamAngleY0, beamAngleSigmaX, beamAngleSigmaY)
    primGen->SetTarget(-0.01942, 0.175);                // (targetZ, targetDz)
    primGen->SmearVertexZ(kTRUE);
    primGen->SmearGausVertexXY(kTRUE);
    gRandom->SetSeed(0);

    switch (generatorName) {
        case UNIGEN: {
            if (!BmnFunctionSet::CheckFileExist(genFile, 1))
                exit(-1);

            auto* unigen = new MpdUnigenGenerator(genFile);
            unigen->RegisterIons();
            primGen->AddGenerator(unigen);
            unigen->SetEventPlane(-M_PI, M_PI);
            auto n_events_in_tree = unigen->GetNEntries();
            if (nStartEvent >= n_events_in_tree) {
                cout << "You are trying to specify the event out reach of current tree" << generatorName << endl;
                exit(-3);
            }
            auto n_avail_events = n_events_in_tree - nStartEvent - 1;
            if (nEvents <= 0)
                nEvents = n_avail_events;
            else
                nEvents = std::min<int>(n_avail_events, nEvents);
            cout << nEvents << " are availiable and will be processed" << endl;
            if (nStartEvent > 0)
                unigen->SkipEvents(nStartEvent);
            break;
        }
            // ------- UrQMD Generator
        case URQMD: {
            if (!BmnFunctionSet::CheckFileExist(genFile, 1))
                exit(-1);

            MpdUrqmdGenerator* urqmdGen = new MpdUrqmdGenerator(genFile);
            // urqmdGen->SetEventPlane(0., 360.);
            primGen->AddGenerator(urqmdGen);
            if (nStartEvent > 0)
                urqmdGen->SkipEvents(nStartEvent);

            // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
            if (nEvents == 0)
                nEvents = MpdGetNumEvents::GetNumURQMDEvents(genFile.Data()) - nStartEvent;
            break;
        }

            // ------- Particle Generator
        case PART: {
            // FairParticleGenerator generates a single particle type (PDG, mult, [GeV] px, py, pz, [cm] vx, vy, vz)
            FairParticleGenerator* partGen = new FairParticleGenerator(211, 10, 1, 0, 3, 1, 0, 0);
            primGen->AddGenerator(partGen);
            break;
        }

            // ------- Ion Generator
        case ION: {
            // Start beam from a far point to check mom. reconstruction procedure (Z, A, q, mult, [GeV] px, py, pz, [cm]
            // vx, vy, vz)
            FairIonGenerator* fIongen = new FairIonGenerator(54, 124, 54, 1, 0., 0., 4.65, 0., 0., 0.);   // Xe
            primGen->AddGenerator(fIongen);
            break;
        }

            // ------- Box Generator
        case BOX: {
            gRandom->SetSeed(0);
            FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 1);   //(PDG code, multiplicity)
            boxGen->SetPRange(0.2, 5.0);                                // GeV/c, setPRange vs setPtRange
            boxGen->SetPhiRange(0, 360);                                // Azimuth angle range [degree]
            boxGen->SetThetaRange(0, 40.0);                             // Polar angle in lab system range [degree]
            primGen->AddGenerator(boxGen);
            break;
        }

            // ------- HSD/PHSD Generator
        case HSD: {
            if (!BmnFunctionSet::CheckFileExist(genFile, 1))
                exit(-1);

            MpdPHSDGenerator* hsdGen = new MpdPHSDGenerator(genFile.Data());
            // hsdGen->SetPsiRP(0.); // set fixed Reaction Plane angle instead of random
            primGen->AddGenerator(hsdGen);
            if (nStartEvent > 0)
                hsdGen->SkipEvents(nStartEvent);

            // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
            if (nEvents == 0)
                nEvents = MpdGetNumEvents::GetNumPHSDEvents(genFile.Data()) - nStartEvent;
            break;
        }

            // ------- LAQGSM/DCM-QGSM Generator
        case QGSM:
        case DCMQGSM: {

            if (!BmnFunctionSet::CheckFileExist(genFile, 1))
                exit(-1);

            MpdLAQGSMGenerator* guGen = new MpdLAQGSMGenerator(genFile.Data(), kFALSE);
            primGen->AddGenerator(guGen);
            if (nStartEvent > 0)
                guGen->SkipEvents(nStartEvent);

            // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
            if (nEvents == 0)
                nEvents = MpdGetNumEvents::GetNumQGSMEvents(genFile.Data()) - nStartEvent;
            break;
        }
        case DCMSMM: {

            if (!BmnFunctionSet::CheckFileExist(genFile, 1))
                exit(-1);

            MpdDCMSMMGenerator* smmGen = new MpdDCMSMMGenerator(genFile.Data());
            primGen->AddGenerator(smmGen);
            if (nStartEvent > 0)
                smmGen->SkipEvents(nStartEvent);

            // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
            if (nEvents == 0)
                nEvents = MpdGetNumEvents::GetNumDCMSMMEvents(genFile.Data()) - nStartEvent;
            break;
        }

        default: {
            cout << "ERROR: Generator name was not pre-defined: " << generatorName << endl;
            exit(-3);
        }
    }   // end of switch (generatorName)

    if (simFile == "")
        simFile = "$VMCWORKDIR/macro/run/bmnsim.root";
    // if directory for the output file does not exist, then create
    if (BmnFunctionSet::CreateDirectoryTree(simFile, 1) < 0)
        exit(-2);
    fRun->SetSink(new FairRootFileSink(simFile.Data()));
    fRun->SetIsMT(false);

    // -----   Create magnetic field   ----------------------------------------
    // To work with old map use the next line:
    BmnFieldMap* magField = new BmnNewFieldMap("FieldMap_1900_extrap_noPed.root");
    Double_t fieldScale = 0.929;   // 1.861 for 900 A, 0.929 for 1900 A;
    magField->SetScale(fieldScale);
    fRun->SetField(magField);

    fRun->SetStoreTraj(kTRUE);
    fRun->SetRadLenRegister(kFALSE);   // radiation length manager

    // -----   Digitizers: converting MC points to detector digits   -----------

    // SiBT-Digitizer
    BmnSiBTConfiguration::SiBT_CONFIG sibt_config = BmnSiBTConfiguration::Run8;
    BmnSiBTDigitizer* sibtDigit = new BmnSiBTDigitizer();
    sibtDigit->SetCurrentConfig(sibt_config);
    fRun->AddTask(sibtDigit);

    // SiProf-Digitizer
    BmnSiProfConfiguration::SiProf_CONFIG siprof_config = BmnSiProfConfiguration::Run8;
    BmnSiProfDigitizer* siprofDigit = new BmnSiProfDigitizer();
    siprofDigit->SetCurrentConfig(siprof_config);
    fRun->AddTask(siprofDigit);

    // BD-Digitizer
    BmnBdDigitizer* bdDigit = new BmnBdDigitizer();
    fRun->AddTask(bdDigit);

    // SiMD-Digitizer
    BmnSiMDDigitizer* simdDigit = new BmnSiMDDigitizer();
    fRun->AddTask(simdDigit);

    // SI-Digitizer
    BmnSiliconConfiguration::SILICON_CONFIG si_config = BmnSiliconConfiguration::Run8;
    BmnSiliconDigitizer* siliconDigit = new BmnSiliconDigitizer();
    siliconDigit->SetCurrentConfig(si_config);
    siliconDigit->SetUseRealEffects(useRealEffects);
    siliconDigit->SetNoise(45.0, 150.0);   // AZ-150224
    fRun->AddTask(siliconDigit);

    // GEM-Digitizer
    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::Run8;
    if (useRealEffects)
        // AZ-150224
        // BmnGemStripMedium::GetInstance().SetCurrentConfiguration(BmnGemStripMediumConfiguration::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_8T);
        BmnGemStripMedium::GetInstance().SetCurrentConfiguration(
            BmnGemStripMediumConfiguration::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_7T);   // AZ-150224
    BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
    gemDigit->SetCurrentConfig(gem_config);
    gemDigit->SetUseRealEffects(useRealEffects);
    gemDigit->SetNoise(10.0, 25.0);   // AZ-150224
    fRun->AddTask(gemDigit);

    // CSC-Digitizer
    BmnCSCConfiguration::CSC_CONFIG csc_config = BmnCSCConfiguration::FullCSC_Run8;
    BmnCSCDigitizer* cscDigit = new BmnCSCDigitizer();
    cscDigit->SetCurrentConfig(csc_config);
    fRun->AddTask(cscDigit);

    // // ZDC-Digitizer (for period < 8)
    // BmnZdcDigitizer* zdcDigit = new BmnZdcDigitizer();
    // zdcDigit->SetScale(39e3);
    // zdcDigit->SetThreshold(500.);
    // fRun->AddTask(zdcDigit);

    // FHCal-Digitizer (for period >= 8)
    BmnFHCalDigitizer* fhcalDigit = new BmnFHCalDigitizer();
    fhcalDigit->SetScale(1.0 / 0.0048);   // scale. Convert Geant GeV to MIPs
    fhcalDigit->SetThreshold(0.);
    fhcalDigit->SetTimeCut(1000.);
    fhcalDigit->SetSiPM(90000,     // [N]   N pixels in SiPM
                        40,        // [N]   MIP to N SiPM pixels
                        0.3,       // [MIP] noise level
                        0.0048);   // [GeV] energy deposited by 1 MIP in FHCal section
    fRun->AddTask(fhcalDigit);

    // ScWall-Digitizer
    BmnScWallDigitizer* fScWallDigit = new BmnScWallDigitizer();
    fScWallDigit->SetScale(1.);
    fScWallDigit->SetThreshold(0.);
    fScWallDigit->SetGeV2MIP(0.0021);   // 2.1 MeV from 1 MIP in 1cm plastic
    fScWallDigit->SetMIP2Pix(55);       // 15 pix for 1 MIP in SiPM
    fScWallDigit->SetMIPNoise(0.1);     // electronic noise in MIP scale
    fRun->AddTask(fScWallDigit);

    // Hodo-Digitizer
    BmnHodoDigitizer* fHodoDigit = new BmnHodoDigitizer();
    fHodoDigit->SetScale(1256);                         // [Z^2/GeV] set Xe peak to 54*54
    fHodoDigit->SetThreshold(16.);                      // [Z^2] we see noise below this in data
    fHodoDigit->SetEnergyResolutionTerms(0.085, 0.0);   // stochastic [sqrt(GeV)] and constant [] terms
    fRun->AddTask(fHodoDigit);

    // Ndet-Digitizer
    BmnNdetDigitizer* fNdetDigit = new BmnNdetDigitizer();
    fNdetDigit->SetTofCut(100.);        // [ns]  cut to remove parasitic late tracks
    fNdetDigit->SetThreshold(0.0025);   // [GeV] 0.5 MIP * 5 MeV for 25 mm plastic scint
    fNdetDigit->SetSiPM(165,            // [N]   MIP to N SiPM pixels (by LPI test march 2024)
                        0.3,            // [MIP] noise level
                        0.005);         // [GeV] MIP to GeV conversion factor = 5 MeV for 25 mm plastic scint
    fRun->AddTask(fNdetDigit);

    // // ECAL-Digitizer (for period < 8)
    // BmnEcalDigitizer* ecalDigit = new BmnEcalDigitizer();
    // fRun->AddTask(ecalDigit);

    fRun->SetStoreTraj(saveGeoInfo);
    fRun->Init();
    magField->Print("");

    // Cuts for Trajectories Visualization (TGeoManager only)
    if (saveGeoInfo) {
        FairTrajFilter* trajFilter = FairTrajFilter::Instance();
        // Set cuts for storing the trajectories
        trajFilter->SetStepSizeCut(0.1);   // [cm]
        trajFilter->SetVertexCut(-200., -200., -200., 200., 200.,
                                 1100.);      // (vxMin, vyMin, vzMin, vxMax, vyMax, vzMax)
        trajFilter->SetMomentumCutP(50e-3);   // p_lab > 50 MeV
        // trajFilter->SetEnergyCut(0., 60.);  // 0 < Etot < 60 GeV
        trajFilter->SetStorePrimaries(kTRUE);
        trajFilter->SetStoreSecondaries(kTRUE);   // kFALSE
    }

    // Fill the Parameter containers for this run
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
    // AZ output->open(parFile.Data());
    output->open(gFile);
    rtdb->setOutput(output);

    rtdb->saveOutput();
    rtdb->print();

    // Transport nEvents
    fRun->Run(nEvents);

    gGeoManager->CheckOverlaps(0.0001);
    gGeoManager->PrintOverlaps();
    if (saveGeoInfo) {
        fRun->CreateGeometryFile("full_geometry.root");   // save the full setup geometry to the additional file
    }

    if ((generatorName == QGSM) || (generatorName == DCMQGSM)) {
        TString Pdg_table_name = TString::Format("%s%s%c%s", gSystem->BaseName(genFile.Data()), ".g",
                                                 (fRun->GetName())[6], ".pdg_table.dat");
        (TDatabasePDG::Instance())->WritePDGTable(Pdg_table_name.Data());
    }
    delete fRun;
    delete magField;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    cout << "Macro finished successfully." << endl;   // marker of successfully execution for software testing systems
}

int main(int argc, char** arg)
{
    run_sim_bmn();
}
