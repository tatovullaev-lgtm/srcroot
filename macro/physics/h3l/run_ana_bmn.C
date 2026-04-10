// Macro for physics analysis of simulated and experimental BM@N events
//
// dstFileName - input DST file with data (MC or exp. data)
// anaFileName - output file with analysis results
// nStartEvent - number of first event to process (starts with zero), default: 0
// nEvents - number of events to process after nStartEvent, 0 - all events of the given file after nStartEvent to
// process
R__ADD_INCLUDE_PATH($VMCWORKDIR)

void run_ana_bmn(
    TString dstFileName = "/eos/eosnica.jinr.ru/nica/bmn/exp/dst/run8/24.04.0/mpd_run_Top_7329_ev0_p3.root",
    TString anaFileName = "pairs.root",
    Int_t nStartEvent = 0,
    Int_t nEvents = 0)
{
    gDebug = 0;   // Debug option
    // Verbosity level (0 = summary only, 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 0;
    FairLogger::GetLogger()->SetLogScreenLevel("info");

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    // check input file exists
    if (!BmnFunctionSet::CheckFileExist(dstFileName, 1))
        exit(-1);

    // -----   Analysis run   --------------------------------------------
    FairRunAna* fRunAna = new FairRunAna();
    // fRunAna->SetEventHeader(new AnaEventHeader());

    Bool_t isExp = (BmnFunctionSet::isSimulationFile(dstFileName)
                    == 0);   // flag for FairRunAna tasks whether simulation or experimental data are used

    Int_t fieldType = -1;
    Double_t fieldScale = 0.;
    FairField* magField = BmnFunctionAna::GetDSTField(dstFileName, fieldScale, fieldType);
    if (magField != nullptr)
        fRunAna->SetField(magField);

    // Declare input source as simulation file or experimental data
    // FairSource* fFileSource = new FairFileSource(dstFileName);
    Int_t run_period = 8, run_number = -1;
    FairSource* fFileSource = new BmnFileSource(dstFileName, run_period, run_number);
    fRunAna->SetSource(fFileSource);

    // gRandom->SetSeed(0);
    // TString geoFileName = Form("current_geo_file_%d.root", UInt_t(gRandom->Integer(UINT32_MAX)));
    // Int_t res_code = UniRun::ReadGeometryFile(run_period, run_number, (char*)geoFileName.Data());
    // TGeoManager::Import(geoFileName);

    // if directory for the output file does not exist, then create
    if (BmnFunctionSet::CreateDirectoryTree(anaFileName, 1) < 0)
        exit(-2);
    fRunAna->SetSink(new FairRootFileSink(anaFileName));
    fRunAna->SetGenerateRunInfo(false);

    // if nEvents is equal 0 then all events of the given file starting with "nStartEvent" should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*)dstFileName.Data()) - nStartEvent;

    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the FairParAsciiFileIo.
    // The FairParAsciiFileIo will create on fly a concatenated input parameter file, which is then used during the
    // reconstruction.
    TList* parFileNameList = new TList();

    // ====================================================================== //
    // ===                          Analysis Tasks                        === //
    // ====================================================================== //

    FairTask* hypNucl = new BmnHypNuclPairFinder("H3L");
    fRunAna->AddTask(hypNucl);

    // remove(geoFileName.Data());

    // -----   Parameter database   --------------------------------------------
    FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(dstFileName.Data());
    parIo2->open(parFileNameList, "in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
    // -------------------------------------------------------------------------

    // -----   Initialize and run   --------------------------------------------
    fRunAna->GetMainTask()->SetVerbose(iVerbose);
    fRunAna->Init();
    fRunAna->Run(nStartEvent, nStartEvent + nEvents);
    // -------------------------------------------------------------------------

    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;   // marker of successful execution
    cout << "Input  file is " + dstFileName << endl;
    cout << "Output file is " + anaFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}
