// Macro for physics analysis of simulated and experimental BM@N events
//
// dstFileName - input DST file with data (MC or exp. data)
// anaFileName - output file with analysis results
// nStartEvent - number of first event to process (starts with zero), default: 0
// nEvents - number of events to process after nStartEvent, 0 - all events of the given file after nStartEvent to
// process
R__ADD_INCLUDE_PATH($VMCWORKDIR)

void run_ana_bmn(TString dstFileName = "$VMCWORKDIR/macro/run/bmndst.root",
                 TString anaFileName = "$VMCWORKDIR/macro/run/bmnana.root",
                 Int_t nStartEvent = 0,
                 Int_t nEvents = 10)
{
    gDebug = 0;   // ROOT debug option
    // Verbosity level (0 = summary only, 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 0;
    // Log level for LOG(severity): "fatal", "error", "alarm", "important", "warn", "state", "info", "detail", "debug",
    // "debug[1-4]", "trace", "nolog"
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

    Bool_t isTarget = kTRUE;   // run with target or not
    // 'isExp' flag for FairRunAna tasks whether simulation or experimental data are used
    Bool_t isExp = (BmnFunctionSet::isSimulationFile(dstFileName) == 0);

    // get field value from the DST file
    Int_t fieldType = -1;
    Double_t fieldScale = 0.;
    FairField* magField = BmnFunctionAna::GetDSTField(dstFileName, fieldScale, fieldType);
    if (magField != nullptr)
        fRunAna->SetField(magField);

    // Declare input source as simulation file or experimental data
    Int_t run_period = -1, run_number = -1;
    BmnFileSource* fFileSource = new BmnFileSource(dstFileName, run_period, run_number);
    fRunAna->SetSource(fFileSource);

    if (isExp)   // for experimental files
    {
        // get geometry for run
        gRandom->SetSeed(0);
        TString geoFileName = Form("current_geo_file_%d.root", UInt_t(gRandom->Integer(UINT32_MAX)));
        Int_t res_code = UniRun::ReadGeometryFile(run_period, run_number, (char*)geoFileName.Data());
        if (res_code != 0) {
            cout << "ERROR: could not read geometry file from the database" << endl;
            exit(-3);
        }

        // get gGeoManager from ROOT file (if required)
        TFile* geoFile = new TFile(geoFileName, "READ");
        if (!geoFile->IsOpen()) {
            cout << "ERROR: could not open ROOT file with geometry: " + geoFileName << endl;
            exit(-4);
        }
        TList* keyList = geoFile->GetListOfKeys();
        TIter next(keyList);
        TKey* key = (TKey*)next();
        TString className(key->GetClassName());
        if (className.BeginsWith("TGeoManager"))
            key->ReadObj();
        else {
            cout << "ERROR: TGeoManager is not top element in geometry file " + geoFileName << endl;
            exit(-5);
        }

        // set magnet field with factor corresponding to the given run
        UniRun* pCurrentRun = UniRun::GetRun(run_period, run_number);
        if (pCurrentRun == 0)
            exit(-6);
        Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
        if (field_voltage == NULL) {
            cout << "ERROR: no field voltage was found for run " << run_period << ":" << run_number << endl;
            exit(-7);
        }
        Double_t map_current = (run_period == 8) ? 112.0 : (run_period == 7) ? 55.87 : 0.0;
        if (*field_voltage < 10) {
            fieldScale = 0;
        } else
            fieldScale = (*field_voltage) / map_current;

        BmnFieldMap* magField = new BmnNewFieldMap("FieldMap_1900_extrap_noPed.root");
        magField->SetScale(fieldScale);
        magField->Init();
        fRunAna->SetField(magField);
        isExp = kTRUE;
        TString targ;
        if (pCurrentRun->GetTargetParticle() == NULL) {
            targ = "-";
            isTarget = kFALSE;
        } else {
            targ = (pCurrentRun->GetTargetParticle())[0];
            isTarget = kTRUE;
        }
        TString beam = pCurrentRun->GetBeamParticle();
        cout << "\n\n|||||||||||||||| EXPERIMENTAL RUN SUMMARY ||||||||||||||||" << endl;
        cout << "||\t\t\t\t\t\t\t||" << endl;
        cout << "||\t\tPeriod:\t\t" << run_period << "\t\t\t||" << endl;
        cout << "||\t\tNumber:\t\t" << run_number << "\t\t\t||" << endl;
        cout << "||\t\tBeam:\t\t" << beam << "\t\t\t||" << endl;
        cout << "||\t\tTarget:\t\t" << targ << "\t\t\t||" << endl;
        cout << "||\t\tField scale:\t" << setprecision(4) << fieldScale << "\t\t\t||" << endl;
        cout << "||\t\t\t\t\t\t\t||" << endl;
        cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n" << endl;
        remove(geoFileName.Data());
    }

    // if directory for the output file does not exist, then create
    if (BmnFunctionSet::CreateDirectoryTree(anaFileName, 1) < 0)
        exit(-2);
    fRunAna->SetSink(new FairRootFileSink(anaFileName));
    fRunAna->SetGenerateRunInfo(false);

    // if nEvents is equal 0 then all events of the given file starting with "nStartEvent" should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*)dstFileName.Data()) - nStartEvent;

    // Add TObjectString file names to a TList which is passed as input to the FairParAsciiFileIo
    // The FairParAsciiFileIo will create on fly a concatenated input parameter file, which is then used during the
    // analysis
    TList* parFileNameList = new TList();

    // ====================================================================== //
    // ===                          Analysis Tasks                        === //
    // ====================================================================== //
    CbmKF* kalman_filter = new CbmKF();
    fRunAna->AddTask(kalman_filter);

    FairTask* ana_task = new BmnAnalysisTask();
    fRunAna->AddTask(ana_task);

    // -----   Parameter database (input)  -------------------------------------
    FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(dstFileName.Data());
    parIo2->open(parFileNameList, "in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);
    // -------------------------------------------------------------------------

    // -----   Initialize FaiRunAna --------------------------------------------
    fRunAna->GetMainTask()->SetVerbose(iVerbose);
    fRunAna->Init();
    // -------------------------------------------------------------------------

    // -----   Parameter database (output)  ------------------------------------
    FairParRootFileIo* parOut = new FairParRootFileIo(kTRUE);
    // parOut->open(dstFileName);
    parOut->open(gFile);
    rtdb->setOutput(parOut);
    rtdb->writeContainer(rtdb->getContainer("FairGeoParSet"), rtdb->getCurrentRun());
    rtdb->saveOutput();
    // rtdb->print();
    // -------------------------------------------------------------------------

    // -----   Run FairRunAna  -------------------------------------------------
    fRunAna->Run(nStartEvent, nStartEvent + nEvents);
    // -------------------------------------------------------------------------

    // -----   Finish   --------------------------------------------------------
    delete fRunAna;
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
