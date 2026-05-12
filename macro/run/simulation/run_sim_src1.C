R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/geometry_run/geometry_src_run8.C"

#define SRC     // Choose generator: SRC URQMD QGSM HSD BOX PART ION
#define GEANT4  // Choose: GEANT3 GEANT4

// inFile - input file with generator data, if needed
// outFile - output file with MC data, default: srcsim.root
// nStartEvent - start event in the input generator file to begin transporting, default: 0
// nEvents - number of events to transport
// useRealEffects - whether we use realistic effects at simulation (Lorentz, misalignment)

void run_sim_src1(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/simulation/Sim_For_MDF_FullGeom_With_Target_ELosses_Field_Scaling.root", Int_t nStartEvent = 0, Int_t nEvents = 1, Bool_t useRealEffects = kFALSE) {
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    // -----   Create simulation run   ----------------------------------------
    FairRunSim* fRun = new FairRunSim();

    // Choose the Geant Navigation System
#ifdef GEANT3
    fRun->SetName("TGeant3");
#else
    fRun->SetName("TGeant4");
#endif

    geometry(fRun); // load SRC geometry

    // Create and Set Event Generator
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    // Smearing of beam interaction point, if needed, and primary vertex position
    // DO NOT do it in corresponding gen. sections to avoid incorrect summation!!!
   // primGen->SetBeam(0.5, -4.6, 0.0, 0.0);
    //primGen->SetTarget(-647.5, 0.0);
    //primGen->SmearVertexZ(kFALSE);
    //primGen->SmearVertexXY(kFALSE);

#ifdef URQMD
    // ------- UrQMD Generator
    if (!BmnFunctionSet::CheckFileExist(inFile)) return;

    MpdUrqmdGenerator* urqmdGen = new MpdUrqmdGenerator(inFile);
    //urqmdGen->SetEventPlane(0., 360.);
    primGen->AddGenerator(urqmdGen);
    if (nStartEvent > 0) urqmdGen->SkipEvents(nStartEvent);

    // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumURQMDEvents(inFile.Data()) - nStartEvent;

#else
#ifdef PART
    // ------- Particle Generator
    FairParticleGenerator* partGen =
            new FairParticleGenerator(211, 10, 1, 0, 3, 1, 0, 0);
    primGen->AddGenerator(partGen);

#else
#ifdef ION
    // ------- Ion Generator
    // Start beam from a far point to check mom. reconstruction procedure
    FairIonGenerator* fIongen = new FairIonGenerator(6, 12, 6, 1, 0., 0., 4., 0., 0., -647.);
    primGen->AddGenerator(fIongen);

#else

#ifdef SRC
	double A = 12.;
	double Z = 6.;
	double P = 3.693; //before target  //3.75;//3.997433; //per nucleon after MWPC2

	//Training 12C  B@1800A  run 2332
	MpdSRCIonGenerator *fSRCgen =
		new MpdSRCIonGenerator(Z, A, 1, //Z, A, muliplicity of an ion in one event
				P*A-0.00001, P*A+0.00001, // Min and Max Total momentum, momentum sigma in GeV/c, middle of LH2 
				0.5, -4.6, -500.0, //X, Y, Z (in cm) of the vertex
				0., 0., //  max vertex deviation in X and Y unifrom  (in cm)
				0.0,  0.0, // X angle, Y angle [deg], 
				0, 0); //Max XA deviation [deg], max YA deviation [deg] uniform
        
//  	MpdSRCIonGenerator *fSRCgen =
//		new MpdSRCIonGenerator(Z, A, 1, //Z, A, muliplicity of an ion in one event
//				P*A-27, P*A+15, // Min and Max Total momentum, momentum sigma in GeV/c, middle of LH2 
//				0., -5., -647.0, //X, Y, Z (in cm) of the vertex
//				0., 0., //  max vertex deviation in X and Y unifrom  (in cm)
//				0.0,  0.0, // X angle, Y angle [deg], 
//				0., 0.); //Max XA deviation [deg], max YA deviation [deg] uniform
//	
	primGen->AddGenerator(fSRCgen);
#else


#ifdef BOX
    gRandom->SetSeed(0);
    // ------- Box Generator
    FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 10); // 13 = muon; 1 = multipl.
    boxGen->SetPRange(2., 2.); // GeV/c, setPRange vs setPtRange
    boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
    boxGen->SetThetaRange(0., 40.); // Polar angle in lab system range [degree]
    primGen->AddGenerator(boxGen);

#else
#ifdef HSD
    // ------- HSD/PHSD Generator
    if (!BmnFunctionSet::CheckFileExist(inFile)) return;

    MpdPHSDGenerator* hsdGen = new MpdPHSDGenerator(inFile.Data());
    //hsdGen->SetPsiRP(0.); // set fixed Reaction Plane angle instead of random
    primGen->AddGenerator(hsdGen);
    if (nStartEvent > 0) hsdGen->SkipEvents(nStartEvent);

    // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumPHSDEvents(inFile.Data()) - nStartEvent;

#else
#ifdef QGSM
    // ------- LAQGSM/DCM-QGSM Generator
    if (!BmnFunctionSet::CheckFileExist(inFile)) return;

    MpdLAQGSMGenerator* guGen = new MpdLAQGSMGenerator(inFile.Data(), kFALSE);
    primGen->AddGenerator(guGen);
    if (nStartEvent > 0) guGen->SkipEvents(nStartEvent);

    // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumQGSMEvents(inFile.Data()) - nStartEvent;

#endif
#endif
#endif
#endif
#endif
#endif
#endif

    fRun->SetSink(new FairRootFileSink(outFile.Data()));
    fRun->SetIsMT(false);

    // -----   Create magnetic field   ----------------------------------------
    Double_t fieldScale = 1650. / 900./1.094;
    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v5_ascii_Extrap.root");
    magField->SetScale(fieldScale);
    fRun->SetField(magField);

    fRun->SetStoreTraj(kTRUE);
    fRun->SetRadLenRegister(kFALSE); // radiation length manager

//    // SI-Digitizer
//    BmnSiliconConfiguration::SILICON_CONFIG si_config = BmnSiliconConfiguration::RunSRCSpring2018;
//    BmnSiliconDigitizer* siliconDigit = new BmnSiliconDigitizer();
//    siliconDigit->SetCurrentConfig(si_config);
//    siliconDigit->SetOnlyPrimary(kFALSE);
//    fRun->AddTask(siliconDigit);

//    // GEM-Digitizer
//    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSRCSpring2018;
//    if (useRealEffects)
//        BmnGemStripMedium::GetInstance().SetCurrentConfiguration(BmnGemStripMediumConfiguration::ARC4H10_90_10_E_880_1920_2780_3160_B_0_9T);
//    BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
//    gemDigit->SetCurrentConfig(gem_config);
//    gemDigit->SetOnlyPrimary(kFALSE);
//    gemDigit->SetStripMatching(kTRUE);
//    fRun->AddTask(gemDigit);
//
//    // CSC-Digitizer
//    BmnCSCConfiguration::CSC_CONFIG csc_config = BmnCSCConfiguration::RunSRCSpring2018;
//    BmnCSCDigitizer* cscDigit = new BmnCSCDigitizer();
//    cscDigit->SetCurrentConfig(csc_config);
//    cscDigit->SetOnlyPrimary(kFALSE);
//    cscDigit->SetStripMatching(kTRUE);
//    fRun->AddTask(cscDigit);

    fRun->Init();
    magField->Print("");

//    // Trajectories Visualization (TGeoManager only)
//    FairTrajFilter* trajFilter = FairTrajFilter::Instance();
//    // Set cuts for storing the trajectories
//    trajFilter->SetStepSizeCut(0.01); // 1 cm
//    trajFilter->SetVertexCut(-200., -200., -150., 200., 200., 1100.);
//    trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//    trajFilter->SetEnergyCut(0., 4.); // 0 < Etot < 1.04 GeV //
//    trajFilter->SetStorePrimaries(kTRUE);
//    trajFilter->SetStoreSecondaries(kTRUE); //kFALSE
    
    
     // Trajectories Visualization (TGeoManager only)
    FairTrajFilter* trajFilter = FairTrajFilter::Instance();
    // Set cuts for storing the trajectories
    trajFilter->SetStepSizeCut(0.01); // 1 cm
    //trajFilter->SetVertexCut(-200., -200., -150., 200., 200., 1100.);
    trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
    //trajFilter->SetEnergyCut(0., 4.); // 0 < Etot < 1.04 GeV //
    trajFilter->SetStorePrimaries(kTRUE);
    //trajFilter->SetStoreSecondaries(kTRUE); //kFALSE
    trajFilter->SetStoreSecondaries(kFALSE); //kFALSE


    // Fill the Parameter containers for this run
    FairRuntimeDb *rtdb = fRun->GetRuntimeDb();

    BmnFieldPar* fieldPar = (BmnFieldPar*) rtdb->getContainer("BmnFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
    fieldPar->setInputVersion(fRun->GetRunId(), 1);
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
    //AZ output->open(parFile.Data());
    output->open(gFile);
    rtdb->setOutput(output);

    rtdb->saveOutput();
    rtdb->print();

    // Transport nEvents
    fRun->Run(nEvents);

    gGeoManager->CheckOverlaps(0.0001);
    gGeoManager->PrintOverlaps();
    fRun->CreateGeometryFile("full_geometry.root");  // save the full setup geometry to the additional file

#ifdef QGSM
    TString Pdg_table_name = TString::Format("%s%s%c%s", gSystem->BaseName(inFile.Data()), ".g", (fRun->GetName())[6], ".pdg_table.dat");
    (TDatabasePDG::Instance())->WritePDGTable(Pdg_table_name.Data());
#endif

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    cout << "Macro finished successfully." << endl; // marker of successfully execution for software testing systems
}
