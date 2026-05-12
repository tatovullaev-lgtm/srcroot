R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/geometry_run/geometry_src_run8_Y.C"
// #include "/home/tools/FairSoft/fairroot/install/include/FairPrimaryGenerator.h"
// #include "/home/tools/FairSoft/fairroot/install/include/FairAsciiGenerator.h"
// #include "/home/tools/FairSoft/fairroot/install/include/FairIonGenerator.h"
// #include "/home/tools/FairSoft/fairroot/install/include/FairParticleGenerator.h"

#define ASCII   // Choose generator: URQMD QGSM HSD BOX PART ION ASCII
#define GEANT4  // Choose: GEANT3 GEANT4

// inFile - input file with generator data, if needed
// outFile - output file with MC data, default: srcsim.root
// nStartEvent - start event in the input generator file to begin transporting, default: 0
// nEvents - number of events to transport
// useRealEffects - whether we use realistic effects at simulation (Lorentz, misalignment)

// 48 (EventID = 47) +++    0.899868, 0.420478, 2.36958
// 76 (EventID = 75)

// 5000000
// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_src.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_src_test.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 100, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {


// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei_r0.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei_r1p581.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei_r1p51_5to85deg.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {




// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei_xsJK.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei_xsKammerud.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei_xsFrankfurt.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {



// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei_xsKammerud_abs.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei_xsFrankfurt_abs.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei_xsFrankfurt_rho1_abs.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5000000, Bool_t useRealEffects = kFALSE) {


// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_v1p2_abs.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 1340000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_v1p2_5Mio.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 105580, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_v1p2_10Mio.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 1270615, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_v1p2_50Mio.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 20110073, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_v1p2_50Mio_noInMed.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 1057690, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_v1p2_50Mio_noAbs.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 4235674, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_WF_10Mio_new.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 5932960, Bool_t useRealEffects = kFALSE) {
// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_WF_10Mio.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 1695249, Bool_t useRealEffects = kFALSE) {
// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_WF_10Mio_1.root",
//                 Int_t nStartEvent = 0, Int_t nEvents = 2118026, Bool_t useRealEffects = kFALSE) {
// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_WF_10Mio_2.root",
//                 Int_t nStartEvent = 0, Int_t nEvents = 2119685, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_WF_10Mio_thetaCut.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 128900, Bool_t useRealEffects = kFALSE) {
// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_WF_10Mio_thetaCut_new.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 128900, Bool_t useRealEffects = kFALSE) {
// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_WF_10Mio_thetaCut_old.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 212117, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_WF_10Mio_thetaCut_new.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 2064507, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_WF_10Mio_latest.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 7997467, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_WF_10Mio_old.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 9026061, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_NoAbs_WF_100Mio.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 6444645, Bool_t useRealEffects = kFALSE) {

void run_sim_src_Y(TString inFile = "", TString outFile = "/media/srcuser3/TOSHIBA EXT/SRC_Simulation/SRC_input/output/srcsim_SRC_gen_T.root",
                 Int_t nStartEvent = 0, Int_t nEvents = 10000000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_Abs_CarlosWF.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 2118819, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_delta_test.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 100000, Bool_t useRealEffects = kFALSE) {
// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_delta_test_frag.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 100000, Bool_t useRealEffects = kFALSE) {
// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_delta_test_frag_highstat.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 1000000, Bool_t useRealEffects = kFALSE) {
// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_delta_test_frag_newXS.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 100000, Bool_t useRealEffects = kFALSE) {

// void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim_alexei_test.root",
//                  Int_t nStartEvent = 0, Int_t nEvents = 100, Bool_t useRealEffects = kFALSE) {

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
    // primGen->SetTarget(-647.5, 0.0);
    // primGen->SmearVertexZ(kFALSE);
    // primGen->SmearVertexXY(kFALSE);
#ifdef ASCII
    // std::string file_dir = "/data/yaopeng/bmnroot_Yaopeng/macro/run/";
    // std::string Ascii_input_filename = file_dir + "Generator_input.txt";
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator(Ascii_input_filename);
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_alexei.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_alexei_r0.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_alexei_r1p581.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_alexei_r1p51_5to85deg.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_alexei_xsJK.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_alexei_xsKammerud.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_alexei_xsFrankfurt.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_alexei_xsKammerud_abs.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_alexei_xsFrankfurt_abs.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_alexei_xsFrankfurt_rho1_abs.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_v1p2_abs.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_v1p2_5Mio.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_v1p2_10Mio.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_v1p2_50Mio.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_v1p2_50Mio_noInMed.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_v1p2_50Mio_noAbs.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_Abs_WF_10Mio_new.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_Abs_WF_10Mio.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_Abs_WF_10Mio_1.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_Abs_WF_10Mio_2.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_Abs_WF_10Mio_thetaCut.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_Abs_WF_10Mio_thetaCut_new.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_Abs_WF_10Mio_thetaCut_old.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_Abs_WF_10Mio_latest.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_Abs_WF_10Mio_old.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_NoAbs_WF_100Mio.txt");
    FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("/media/srcuser3/TOSHIBA EXT/SRC_Simulation/SRC_input/txt/Generator_input_SRC_sim_T.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_Abs_CarlosWF.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_delta_test.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_delta_test_frag.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_delta_test_frag_highstat.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_delta_test_frag_newXS.txt");

    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input_SRC.txt");
    // FairAsciiGenerator* AsciiGen = new FairAsciiGenerator("Generator_input.txt");
    primGen->AddGenerator(AsciiGen);
#else
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
            // new FairParticleGenerator(2212, 1, 0.899868, 0.420478, 2.36958, 0, 0, -576.2);
            new FairParticleGenerator(2212, 1, 0.899868, 0.000000, 2.36958, 0, 0, -576.2);
    primGen->AddGenerator(partGen);

/*
2	47	0	0	-576.2
2212	0.899868	0.420478	2.36958

    FairParticleGenerator(Int_t pdgid,
                          Int_t mult,
                          Double32_t px,
                          Double32_t py,
                          Double32_t pz,
                          Double32_t vx = 0.,
                          Double32_t vy = 0.,
                          Double32_t vz = 0.);
    Momentum components [GeV]
    vx,vy,vz Vertex coordinates [cm], Default (0,0,0)
*/

#else
#ifdef ION
    // ------- Ion Generator
    // Start beam from a far point to check mom. reconstruction procedure
    // FairIonGenerator* fIongen = new FairIonGenerator(6, 12, 6, 1, 0., 0., 4., 0., 0., -647.);
    FairIonGenerator* fIongen = new FairIonGenerator(6, 12, 6, 1, 0., 0., 4., 0., 0., -550.);
    primGen->AddGenerator(fIongen);

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
    cout<<1<<endl;
                
    // -----   Create magnetic field   ----------------------------------------1
    // Double_t fieldScale = 0.897;
    // BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v5_ascii_Extrap.root");
    // magField->SetScale(fieldScale);
    // fRun->SetField(magField);

    // Double_t fieldScale = 0.9;
    // BmnFieldMap* magField = new BmnNewFieldMap("FieldMap_1900.root");
    // magField->SetScale(fieldScale);
    // fRun->SetField(magField);
    // -----   Create magnetic field   ----------------------------------------1

    fRun->SetStoreTraj(kTRUE);
    fRun->SetRadLenRegister(kFALSE); // radiation length manager
    cout<<2<<endl;

    // SI-Digitizer
    // BmnSiliconConfiguration::SILICON_CONFIG si_config = BmnSiliconConfiguration::RunSRCSpring2018;
    // BmnSiliconDigitizer* siliconDigit = new BmnSiliconDigitizer();
    // siliconDigit->SetCurrentConfig(si_config);
    // siliconDigit->SetOnlyPrimary(kFALSE);
    // fRun->AddTask(siliconDigit);

    // GEM-Digitizer
    // BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSRCSpring2018;
    // if (useRealEffects)
    //     BmnGemStripMedium::GetInstance().SetCurrentConfiguration(BmnGemStripMediumConfiguration::ARC4H10_90_10_E_880_1920_2780_3160_B_0_9T);
    // BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
    // gemDigit->SetCurrentConfig(gem_config);
    // gemDigit->SetOnlyPrimary(kFALSE);
    // gemDigit->SetStripMatching(kTRUE);
    // fRun->AddTask(gemDigit);

    // CSC-Digitizer
    // BmnCSCConfiguration::CSC_CONFIG csc_config = BmnCSCConfiguration::RunSRCSpring2018;
    // BmnCSCDigitizer* cscDigit = new BmnCSCDigitizer();
    // cscDigit->SetCurrentConfig(csc_config);
    // cscDigit->SetOnlyPrimary(kFALSE);
    // cscDigit->SetStripMatching(kTRUE);
    // fRun->AddTask(cscDigit);

    fRun->Init();
    cout<<333<<endl;
    // -----   Create magnetic field   ----------------------------------------2
    // magField->Print("");
    // -----   Create magnetic field   ----------------------------------------2

    // Trajectories Visualization (TGeoManager only)
    // FairTrajFilter* trajFilter = FairTrajFilter::Instance();
    // // Set cuts for storing the trajectories
    // trajFilter->SetStepSizeCut(0.01); // 1 cm
    // trajFilter->SetVertexCut(-200., -200., -150., 200., 200., 1100.);
    // trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
    // trajFilter->SetEnergyCut(0., 4.); // 0 < Etot < 1.04 GeV //
    // trajFilter->SetStorePrimaries(kTRUE);
    // trajFilter->SetStoreSecondaries(kTRUE); //kFALSE

    // Fill the Parameter containers for this run
    FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
    cout<<4<<endl;

    // -----   Create magnetic field   ----------------------------------------3
    // BmnFieldPar* fieldPar = (BmnFieldPar*) rtdb->getContainer("BmnFieldPar");
    // fieldPar->SetParameters(magField);
    // fieldPar->setChanged();
    // fieldPar->setInputVersion(fRun->GetRunId(), 1);
    // -----   Create magnetic field   ----------------------------------------3

    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
    //AZ output->open(parFile.Data());
    output->open(gFile);
    rtdb->setOutput(output);
    cout<<5<<endl;

    rtdb->saveOutput();
    rtdb->print();

    // Transport nEvents
    fRun->Run(nEvents);
    cout<<6<<endl;

    //gGeoManager->CheckOverlaps(0.0001);
    //gGeoManager->PrintOverlaps();
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
