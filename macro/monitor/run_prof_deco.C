R__ADD_INCLUDE_PATH($VMCWORKDIR)

void run_prof_deco(TString inputFileName = "localhost",
        TString bmndstFileName = "$VMCWORKDIR/macro/monitor/prodigi.root",
        Int_t nStartEvent = 0, Int_t nEvents = 10) {
    gDebug = 0; // Debug option
    // Verbosity level (0 = quiet (progress bar), 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 0;
    Int_t period = 8;
    Bool_t isPrimary = kFALSE;
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    //    // check input file exists
    //    if (!BmnFunctionSet::CheckFileExist(inputFileName, 1)) exit(-1);

    // -----   Reconstruction run   --------------------------------------------
    //    FairRunAna* fRunAna = new FairRunAna();
    FairRunOnline* fRunAna = new FairRunOnline();
    //    fRunAna->SetEventHeader(new DstEventHeader());

    // Declare input source as simulation file or experimental data
    BmnProfilometerSource * source = new BmnProfilometerSource({
        Form("tcp://%s:5601", inputFileName.Data()),
        Form("tcp://%s:5602", inputFileName.Data())
    },
    {
        "board1", "board2"
    });
    fRunAna->SetSource(source);

    // if directory for the output file does not exist, then create
    //    if (BmnFunctionSet::CreateDirectoryTree(bmndstFileName, 1) < 0) exit(-2);
    //    fRunAna->SetGenerateRunInfo(false);

    BmnProfAsic2Raw * proTask = new BmnProfAsic2Raw();
    fRunAna->AddTask(proTask);
    //    fRunAna->SetSink(new FairRootFileSink(bmndstFileName));
    fRunAna->SetSink(new BmnMQSink(5603));

    // -------------------------------------------------------------------------
    // -----   Initialize and run   --------------------------------------------
    fRunAna->GetMainTask()->SetVerbose(iVerbose);
    fRunAna->Init();
    cout << "Starting run" << endl;
    fRunAna->Run(-1, -1);
    // -------------------------------------------------------------------------
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl; // marker of successful execution for CDASH
    cout << "Input  file is " + inputFileName << endl;
    cout << "Output file is " + bmndstFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}
