R__ADD_INCLUDE_PATH($VMCWORKDIR)
// #include "macro/run/geometry_run/geometry_src_hypernis_setup7.C"	// 2 SP-40 HyperNIS magnet and TOFWall, and active planes (Frags, Recoil)
//YN #include "macro/run/geometry_run/geometry_hypernis_setup_67.C"	// 2 SP-40 HyperNIS magnet and TOFWall, and active planes (Frags, Recoil)
#include "macro/run/geometry_run/geometry_hypernis_setup_67_v1.C"	// 2 SP-40 HyperNIS magnet and TOFWall, and active planes (Frags, Recoil)

//#define SRC //GCF //BOX //SRC //BOX //ION //SRC     // Choose generator: URQMD QGSM HSD BOX PART ION SRC
#define ASCII //GCF //BOX //SRC //BOX //ION //SRC //ASCII    // Choose generator: URQMD QGSM HSD BOX PART ION SRC
#define GEANT4  // Choose: GEANT3 GEANT4

// inFile - input file with generator data, if needed
// outFile - output file with MC data, default: srcsim.rootBmnBC
// nStartEvent - start event in the input generator file to begin transporting, default: 0
// nEvents - number of events to transport
// useRealEffects - whether we use realistic effects at simulation (Lorentz, misalignment)

//void run_sim_src(TString inFile = "", TString outFile = "/media/jk/Backup\ Plus/HyperNIS/sim_setup/test_weight.root", Int_t nStartEvent = 0, Int_t nEvents = 100, Bool_t useRealEffects = kFALSE) {
// void run_sim_src_OneField(TString inFile = "", TString outFile = "test100k3_5_1_1dP_BigBigToF_He6.root", Int_t nStartEvent = 0, Int_t nEvents = 100000, Bool_t useRealEffects = kFALSE) {
//void run_sim_HyperNIS_DK(TString inFile = "", TString outFile = "Li_3_3GeV5000000_HyperNIS.root", Int_t nStartEvent = 0, Int_t nEvents = 10000, Bool_t useRealEffects = kFALSE) {
//void run_sim_HyperNIS_DK(TString inFile = "", TString outFile = "Li_3_3GeV5000000_HyperNIS.root", Int_t nStartEvent = 0, Int_t nEvents = 200000, Bool_t useRealEffects = kFALSE) {
void run_sim_HyperNIS_DK(TString inFile = "", TString outFile = "Li_3_3GeV5000000_HyperNIS.root", Int_t nStartEvent = 0, Int_t nEvents = 10, Bool_t useRealEffects = kFALSE) {
//void run_sim_HyperNIS_DK(TString inFile = "", TString outFile = "Li_3_3GeV5000000_HyperNIS.root", Int_t nStartEvent = 0, Int_t nEvents = 1000000, Bool_t useRealEffects = kFALSE) {
    TStopwatch timer;
    timer.Start();
//    gDebug = 0;
    gDebug = 1;

//YN
    Int_t iVerbose = 2;

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
    //primGen->SetBeam(0.5, -4.6, 0.0, 0.0);
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
    gRandom->SetSeed(0);
    double px = gRandom->Gaus(0,0.1);
    double py = gRandom->Gaus(0,0.1);
    double pz = TMath::Sqrt(4*4-(px*px+py*py));
    double vx = gRandom->Gaus(0,1);
    double vy = gRandom->Gaus(0,1);
    //FairIonGenerator* fIongen = new FairIonGenerator(6, 12, 6, 1, px, py, pz, vx, vy, -647.);
    FairIonGenerator* fIongen = new FairIonGenerator(6, 12, 6, 1, 0, 0, 4, 0, 0, -647.);
    primGen->AddGenerator(fIongen);

#else

#ifdef ASCII
//FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("/lhep/users/naryshk/SRC_study_all/gen_for_BMNroot/Generator_input_SRC_sim_T.txt");
//FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("/lhep/users/naryshk/SRC_study_all/gen_for_BMNroot/dp2pn_BMN_input.txt");
FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("/lhep/users/naryshk/SRC_study_all/gen_for_BMNroot/dp2pn_BMN_input.txt");
primGen->AddGenerator(AsciiGen);
#else
#ifdef SRC
//	double A = 7.;
//	double Z = 3.;
//	double P = 3.3; //per nucleon after MWPC2
//	double A = 2.; // for deuterium
	double A = 2.; // for hydrogen
	double Z = 1.;
	double P = 3.3; //per nucleon after MWPC2
//	double P = 3.3; //per nucleon after MWPC2

	//Training 12C  B@1800A  run 2332
	MpdSRCIonGenerator *fSRCgen =

//         new MpdSRCIonGenerator(Z, A, 1, //Z, A, muliplicity of an ion in one event
// 				P*A/1., P*A*1., // Min and Max Total momentum, momentum sigma in GeV/c, middle of LH2
// 				0., 0., 0.0, //X, Y, Z (in cm) of the vertex
// 				0., 0., //  max vertex deviation in X and Y unifrom  (in cm)
// 				0.0,  0.0, // X angle, Y angle [deg],
// 				0., 0.); //Max XA deviation [deg], max YA deviation [deg] uniform (true - gaus)
        new MpdSRCIonGenerator(Z, A, 1, //Z, A, muliplicity of an ion in one event
				P*A/1.1, P*A*1.1, // Min and Max Total momentum, momentum sigma in GeV/c, middle of LH2
				0., 0., 0.0, //X, Y, Z (in cm) of the vertex
				0.4, 0.4, //  max vertex deviation in X and Y unifrom  (in cm)
				0.0,  0.0, // X angle, Y angle [deg],
// 				2.8, 2.8); //Max XA deviation [deg], max YA deviation [deg] uniform (true - gaus)
//				0.6, 0.6); //Max XA deviation [deg], max YA deviation [deg] uniform (true - gaus) - 150 MeV/c per C12
                0.0, 0.0); //Max XA deviation [deg], max YA deviation [deg] uniform (true - gaus)
	primGen->AddGenerator(fSRCgen);
#else

#ifdef GCF 
	MpdGCFGenerator *fGCFgen =
		//new MpdGCFGenerator("ppF", "/media/jk/Backup\ Plus/DubnaMeytal/GCF_tree_xs_finalstatistics_noCuts.root", 0., 0., -665.0);
		new MpdGCFGenerator("ppF", "/media/jk/Backup\ Plus/GCF_JINR21/gcf_3p5GeVc_DAC.root", 0., 0., 0.0);

	primGen->AddGenerator(fGCFgen);
#else

#ifdef BOX
//    gRandom->SetSeed(12345); // Всегда одинаковая последовательность
    gRandom->SetSeed(0);  // Новая последовательность каждый раз
    // ------- Box Generator
    ///FairBoxGenerator* boxGen = new FairBoxGenerator(1000050110, 1); // 13 = muon; 1 = multipl.
    FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 1); // 13 = muon; 1 = multipl.
//YN    boxGen->SetPRange(3.99, 4.01); // GeV/c, setPRange vs setPtRange
//    boxGen->SetPRange(1.99, 4.01); // GeV/c, setPRange vs setPtRange
    boxGen->SetPRange(2.0, 2.0); // GeV/c, setPRange vs setPtRange
//    boxGen->SetPRange(10.99, 11.01); // GeV/c, setPRange vs setPtRange
//    boxGen->SetPRange(100.99, 400.01); // GeV/c, setPRange vs setPtRange
// phi=90° points along the positive y-axis
//wrk    boxGen->SetPhiRange(170, 190); // Azimuth angle range [degree]
    boxGen->SetPhiRange(165, 195); // Azimuth angle range [degree]
//    boxGen->SetPhiRange(150, 210); // Azimuth angle range [degree]
//    boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
//YN    boxGen->SetPhiRange(0, 1); // Azimuth angle range [degree]
    //boxGen->SetThetaRange(0, 0.5); // Polar angle in lab system range [degree]
//    boxGen->SetThetaRange(25.0, 55.0); // Polar angle in lab system range [degree]
    boxGen->SetThetaRange(0.0, 90.0); // Polar angle in lab system range [degree]
    boxGen->SetBoxXYZ(0.,0.,0.,0.,0.);
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
#endif
#endif

    fRun->SetSink(new FairRootFileSink(outFile.Data()));
    fRun->SetIsMT(false);

    // -----   Create magnetic field   ----------------------------------------
    ////Double_t fieldScale = 1800. / 900.;
    ////Double_t fieldScale = 1.932;//1800A run 2332
    //Double_t fieldScale = 2;//0.64;
    //BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    //magField->SetScale(fieldScale);
    ////fRun->SetField(magField);
    
    //first SP-40
    Double_t fieldScale = -7.05;
//    Double_t fieldScale = -14.5;
//        Double_t fieldScale = 0;
    BmnFieldMap* magField = new BmnNewFieldMap("sp40magnet_qi.root");
    magField->SetPosition(0,0,550.);
    magField->SetScale(fieldScale);
//     fRun->SetField(magField);
    
    //second SP-40
//     Double_t fieldScale2 = -10.;
   // Double_t fieldScale2 = -7.05;
//     Double_t fieldScale2 = 0; // checking for global system
    // BmnFieldMap* magField2 = new BmnNewFieldMap("sp40magnet_qi.root");
    // magField2->SetPosition(0,0,925.); //good position
    // magField2->SetScale(fieldScale2);
//     fRun->SetField(magField2);

//    BmnMultiField* magMultiField = new BmnMultiField();
//    magMultiField->AddField(magField);
//    magMultiField->AddField(magField2);
//
//    magMultiField->Init();

//    fRun->SetField(magMultiField);
    fRun->SetField(magField);
    //fRun->SetField(magField2);


//    cout << "FIELD  " << magMultiField->GetBy(0., 0., 550.) << endl;
//    cout << "FIELD  " << magMultiField->GetBy(0., 0., 925.) << endl;


    fRun->SetStoreTraj(kTRUE);
    fRun->SetRadLenRegister(kFALSE); // radiation length manager

    // Calorimeter Digitizer
//    BmnCalorimeterDigitizer* caloDigit = new BmnCalorimeterDigitizer();
    //caloDigit->SetCurrentConfig(si_config);
    //caloDigit->SetOnlyPrimary(kFALSE);
//    fRun->AddTask(caloDigit);
    
    // TOFWall Digitizer
//YN    BmnTOFWallDigitizer* tofwallDigit = new BmnTOFWallDigitizer();
    //tofwallDigit->SetOnlyPrimary(kFALSE);
//YN    fRun->AddTask(tofwallDigit);
    
    // ActivePlane Digitizer
//YN    BmnActivePlaneDigitizer* activeplanesDigit = new BmnActivePlaneDigitizer();
//YN    fRun->AddTask(activeplanesDigit);
    // Frag1
    //BmnActivePlaneDigitizer* activeplaneDigit = new BmnActivePlaneDigitizer();
    //fRun->AddTask(activeplaneDigit);
    // Recoil
    //BmnActivePlaneDigitizer* activeplaneDigit2 = new BmnActivePlaneDigitizer();
    //fRun->AddTask(activeplaneDigit2);

    // SI-Digitizer
    BmnSiliconConfiguration::SILICON_CONFIG si_config = BmnSiliconConfiguration::RunSRCSpring2018;
    BmnSiliconDigitizer* siliconDigit = new BmnSiliconDigitizer();
    siliconDigit->SetCurrentConfig(si_config);
    siliconDigit->SetOnlyPrimary(kFALSE);
 //   fRun->AddTask(siliconDigit);

    // GEM-Digitizer
    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSRCSpring2018;
    if (useRealEffects)
        BmnGemStripMedium::GetInstance().SetCurrentConfiguration(BmnGemStripMediumConfiguration::ARC4H10_90_10_E_880_1920_2780_3160_B_0_9T);
    BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
    gemDigit->SetCurrentConfig(gem_config);
    gemDigit->SetOnlyPrimary(kFALSE);
    gemDigit->SetStripMatching(kTRUE);
    //JK
    //fRun->AddTask(gemDigit);

    // CSC-Digitizer
     BmnCSCConfiguration::CSC_CONFIG csc_config = BmnCSCConfiguration::RunSRCSpring2018;
     BmnCSCDigitizer* cscDigit = new BmnCSCDigitizer();

     cscDigit->SetCurrentConfig(csc_config);
     cscDigit->SetOnlyPrimary(kFALSE);
     cscDigit->SetStripMatching(kTRUE);
//YN
    fRun->AddTask(cscDigit);

    fRun->Init();
//    magField->Print("");
//    magField2->Print("");

    // Trajectories Visualization (TGeoManager only)                    ?????????????????
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
//
//     BmnFieldPar* fieldPar2 = (BmnFieldPar*) rtdb->getContainer("BmnFieldPar");
//     fieldPar2->SetParameters(magField2);
//     fieldPar2->setChanged();
//     fieldPar2->setInputVersion(fRun->GetRunId(), 1);

    // BmnFieldPar* fieldPar = (BmnFieldPar*) rtdb->getContainer("BmnFieldPar");
    // fieldPar->SetParameters(magMultiField);
    // fieldPar->setChanged();
    // fieldPar->setInputVersion(fRun->GetRunId(), 1);

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
    gGeoManager->Export("test.gdml");
    fRun->CreateGeometryFile("full_geometry.root");  // save the full setup geometry to the additional file

#ifdef QGSM
    TString Pdg_table_name = TString::Format("%s%s%c%s", gSystem->BaseName(inFile.Data()), ".g", (fRun->GetName())[6], ".pdg_table.dat");
    (TDatabasePDG::Instance())->WritePDGTable(Pdg_table_name.Data());
#endif

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
//    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    cout << "Macro finished successfully." << endl; // marker of successfully execution for software testing systems
}
