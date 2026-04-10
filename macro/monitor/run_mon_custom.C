
R__ADD_INCLUDE_PATH($VMCWORKDIR)

void run_mon_custom(
    TString inputFileName = "tcp://localhost:6666",
    // TString inputFileName = "/ncx/eos/nica/bmn/exp/dst/run7/prerelease/3590-4707_BMN_Argon/bmn_run4649_dst.root",
    TString padConfName = "pad-sample.json",
    Int_t nStartEvent = 0,
    Int_t nEvents = 0,
    Int_t webPort = 8090)
{
    gDebug = 0;   // Debug option
    // Verbosity level (0 = quiet (progress bar), 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 1;
    Int_t period = 9;
    Bool_t isPrimary = kFALSE;
    bool isNetworkSource = true;
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    if (!isNetworkSource) {
        // check input file exists
        if (!BmnFunctionSet::CheckFileExist(inputFileName, 1))
            exit(-1);
    }

    //    FairRunAna* fRunAna = new FairRunAna();
    FairRunOnline* fRunAna = new FairRunOnline();

    FairSource* source = isNetworkSource ? static_cast<FairSource*>(new BmnMQSource(inputFileName.Data(), kTRUE))
                                         : static_cast<FairSource*>(new BmnFileSource(inputFileName));
    /* FairSource* source = nullptr;
    if (isNetworkSource)
        source = new BmnMQSource("tcp://localhost:6666", kTRUE);
        source = new BmnFileSource(inputFileName); */
    fRunAna->SetSource(source);

    //  if directory for the output file does not exist, then create
    // if (BmnFunctionSet::CreateDirectoryTree(bmndstFileName, 1) < 0) exit(-2);
    // fRunAna->SetSink(new FairRootFileSink(bmndstFileName));
    //    fRunAna->SetSink(new BmnMQSink());

    BmnCustomQa* trQaAll = new BmnCustomQa("custom_qa", string(padConfName.Data()));
    trQaAll->SetMonitorMode(kTRUE);
    TString mode = "fastcgi";   // "http"
    TString cgiStr = Form("%s:%d;noglobal;cors", mode.Data(), webPort);
    THttpServer* fServer = new THttpServer(cgiStr.Data());
    fServer->SetTimer(50, kTRUE);
    trQaAll->SetObjServer(fServer);
    //    source->SetObjServer(fServer);
    fRunAna->AddTask(trQaAll);
    // -------------------------------------------------------------------------
    // -----   Initialize and run   --------------------------------------------
    fRunAna->GetMainTask()->SetVerbose(iVerbose);
    fRunAna->Init();
    cout << "Starting run" << endl;
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*)inputFileName.Data()) - nStartEvent;
    if (isNetworkSource)
        fRunAna->Run(-1, -1);
    else
        fRunAna->Run(nStartEvent, nStartEvent + nEvents);
    // -------------------------------------------------------------------------
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;   // marker of successful execution for CDASH
    cout << "Input  file is " + inputFileName << endl;
    // cout << "Output file is " + bmndstFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    fServer->SetTimer(50, false);
    // ------------------------------------------------------------------------
    // getchar();
    delete fRunAna;
}
