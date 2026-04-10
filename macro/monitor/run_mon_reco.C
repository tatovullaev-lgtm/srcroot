void run_mon_reco(TString inputFileName = "tcp://localhost:6666",
                  TString bmndstFileName = "bmndst.root",
                  Int_t nStartEvent = 0,
                  Int_t nEvents = 0,
                  bool isOnline = true)
{
    gDebug = 0;   // Debug option
    // Verbosity level (0 = quiet (progress bar), 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 0;
    Int_t period = 9;

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    FairRunOnline* fRunAna = new FairRunOnline();

    FairSource* source = nullptr;
    if (isOnline)
        source = new BmnMQSource(string(inputFileName), kFALSE);
    else {
        if (!BmnFunctionSet::CheckFileExist(inputFileName, 1))
            exit(-1);
        source = new BmnFileSource(inputFileName);
    }
    fRunAna->SetSource(source);
    // if nEvents is equal 0 then all events of the given file starting with "nStartEvent" should be processed
    if (source->GetSourceType() == kFILE) {
        if (nEvents == 0)
            nEvents = MpdGetNumEvents::GetNumROOTEvents((char*)inputFileName.Data()) - nStartEvent;
    } else {
        nStartEvent = -1;
        nEvents = -1;
    }

    // if directory for the output file does not exist, then create
    if (BmnFunctionSet::CreateDirectoryTree(bmndstFileName, 1) < 0)
        exit(-2);
    //    fRunAna->SetSink(new FairRootFileSink(bmndstFileName));
    //    fRunAna->SetSink(new BmnMQSink());
    //    fRunAna->SetGenerateRunInfo(false);
    BmnTrackingQaExp* trQaAll = new BmnTrackingQaExp("TrackingQa", period);

    trQaAll->SetDetectorPresence(kSILICON, kTRUE);
    trQaAll->SetDetectorPresence(kCSC, kTRUE);
    trQaAll->SetDetectorPresence(kSSD, kFALSE);
    trQaAll->SetDetectorPresence(kGEM, kTRUE);
    trQaAll->SetDetectorPresence(kVSP, kTRUE);
    THttpServer* fServer = nullptr;
    if (isOnline) {
        trQaAll->SetMonitorMode(kTRUE);
        fServer = new THttpServer("fastcgi:8081;noglobal;cors");
        fServer->SetTimer(50, kTRUE);
        trQaAll->SetObjServer(fServer);
    }
    TString innerTrackBranchName;   // use different track container
#ifdef L1Tracking
    innerTrackBranchName = "StsTrack";
#else
    innerTrackBranchName = "StsVector";
#endif
    trQaAll->SetInnerTracksBranchName(innerTrackBranchName);
    fRunAna->AddTask(trQaAll);
    // -------------------------------------------------------------------------
    // -----   Initialize and run   --------------------------------------------
    fRunAna->GetMainTask()->SetVerbose(iVerbose);
    fRunAna->Init();
    cout << "Starting run" << endl;
    if (isOnline) {
        fRunAna->Run(-1, -1);
        fServer->SetTimer(50, kFALSE);
    } else
        fRunAna->Run(nStartEvent, nStartEvent + nEvents);
    // -------------------------------------------------------------------------
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;   // marker of successful execution for CDASH
    cout << "Input  file is " + inputFileName << endl;
    cout << "Output file is " + bmndstFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
    getchar();
}
