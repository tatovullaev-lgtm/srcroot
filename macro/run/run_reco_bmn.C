// Macro for reconstruction of simulated and experimental BM@N events
//
// inFileName - input file with data (MC or exp. data)
// dstFileName - output file with reconstructed data (full DST format)
// nStartEvent - number of first event to process (starts with zero), default: 0
// nEvents - number of events to process after nStartEvent, 0 - all events of the given file after nStartEvent to
// tracking - tracking algorithm: VectorFinder (default), L1Tracking (for online), CellAuto
// #define USE_GEODB
R__ADD_INCLUDE_PATH($VMCWORKDIR);
bool usePreviousIfMissing = false;
enum enumTracking
{
    VectorFinder,
    L1Tracking,
    CellAuto
};

void run_reco_bmn(TString inFileName = "$VMCWORKDIR/macro/run/bmnsim.root",
                  TString dstFileName = "$VMCWORKDIR/macro/run/bmndst.root",
                  Int_t nStartEvent = 0,
                  Int_t nEvents = 0,
                  enumTracking tracking = VectorFinder)
{
    gDebug = 0;   // ROOT debug option
    // if (gEnv) gEnv->SetValue("Root.Stacktrace", 0);
    //  Verbosity level (0 = summary only, 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 0;
    // Log level for LOG(severity): "fatal", "error", "alarm", "important", "warn", "state", "info", "detail", "debug",
    // "debug[1-4]", "trace", "nolog"
    FairLogger::GetLogger()->SetLogScreenLevel("info");
    gRandom->SetSeed(0);   // set seed for PRNG

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    // check input file exists
    if (!BmnFunctionSet::CheckFileExist(inFileName, 1))
        exit(-1);

    // -----   Reconstruction run   --------------------------------------------
    FairRunAna* fRunAna = new FairRunAna();
    fRunAna->SetEventHeader(new DstEventHeader());

    Bool_t isTarget = kTRUE;   // flag for tracking (run with target or not)
    Bool_t isExp = (BmnFunctionSet::isSimulationFile(inFileName)
                    == 0);   // flag for FairRunAna tasks whether simulation or experimental data are used

    // Declare input source as simulation file or experimental data
    FairSource* fFileSource;

    // -1 means use of the BM@N-setup when processing MC-input
    // DO NOT change it manually!
    Int_t run_period = 9, run_number = -1;
    Double_t fieldScale = 0.;
    if (!isExp)   // for simulation files
        fFileSource = new FairFileSource(inFileName);
    else   // for experimental files
    {
        // set source - digit file without auxilary FairRoot directories
        // returns real run_period and run_number from the source file
        fFileSource = new BmnFileSource(inFileName, run_period, run_number);

        // get geometry for run
#ifdef USE_GEODB
        GeoSetup gSetup;
        int errStatus = gSetup.LoadSetupToGeoManager("run9_26.01.0");
        if (errStatus != 0) {
            cout << "ERROR: Geometry not loaded. Error status " << errStatus << endl;
            exit(-1);
        }
#else
        TString geoFileName = Form("current_geo_file_%d.root", UInt_t(gRandom->Integer(UINT32_MAX)));
        Int_t res_code =
            UniRun::ReadGeometryFile(run_period, run_number, (char*)geoFileName.Data(), usePreviousIfMissing);
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
#endif
        // set magnetic field with factor corresponding to the given run
        UniRun* pCurrentRun = UniRun::GetRun(run_period, run_number, usePreviousIfMissing);
        if (pCurrentRun == 0)
            exit(-6);
        Double_t field_voltage = 0;
        if (pCurrentRun->GetFieldVoltage())
            field_voltage = *pCurrentRun->GetFieldVoltage();
        else {
            if (usePreviousIfMissing)   // get value from a previous run with non-null value
            {
                int prev_mag_period = run_period, prev_mag_run = run_number;
                field_voltage = UniRun::FindPreviousFieldVoltage(prev_mag_period, prev_mag_run);
                if ((field_voltage == 0) and (prev_mag_run == run_number) and (prev_mag_period == run_period)) {
                    cout << "ERROR: no any field voltage was found for run " << run_period << ":" << run_number << endl;
                    exit(-8);
                }
            } else {
                cout << "ERROR: no field voltage was found for run " << run_period << ":" << run_number << endl;
                exit(-7);
            }
        }
        Double_t map_voltage = (run_period < 8) ? 55.87 : 112.0;
        if (field_voltage < 10) {
            fieldScale = 0;
        } else
            fieldScale = (field_voltage) / map_voltage;
        BmnFieldMap* magField = nullptr;
#ifdef USE_GEODB
        magField = dynamic_cast<BmnFieldMap*>(gSetup.GetMagneticField(fieldScale));
#else
        magField = new BmnNewFieldMap("FieldMap_1900_extrap_noPed.root");
        magField->SetScale(fieldScale);
#endif
        magField->Init();
        fRunAna->SetField(magField);
        isExp = kTRUE;
        TString targ;
        if (pCurrentRun->GetTargetParticle() == nullptr) {
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
#ifndef USE_GEODB
        remove(geoFileName.Data());
#endif
    }

    fRunAna->SetSource(fFileSource);

    if (dstFileName == "")
        dstFileName = "bmndst.root";
    // if directory for the output file does not exist, then create
    if (BmnFunctionSet::CreateDirectoryTree(dstFileName, 1) < 0)
        exit(-2);
    fRunAna->SetSink(new FairRootFileSink(dstFileName));
    fRunAna->SetGenerateRunInfo(false);

    // if nEvents is equal 0 then all events of the given file starting with "nStartEvent" should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*)inFileName.Data()) - nStartEvent;

    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the FairParAsciiFileIo.
    // The FairParAsciiFileIo will create on fly a concatenated input parameter file, which is then used during the
    // reconstruction.
    TList* parFileNameList = new TList();

    // ====================================================================== //
    // ===                  Pileup Event Classification                   === //
    // ====================================================================== //
    if (run_period == 8 && isExp) {
        BmnEventSelector* eventSelector = new BmnEventSelector();
        eventSelector->SetRunId(run_number);
        eventSelector->SetInputFileName("$VMCWORKDIR/input/eventSelector_calib_run8.root");
        fRunAna->AddTask(eventSelector);
    }

    // ====================================================================== //
    // ===                Fill basic trigger information                  === //
    // ====================================================================== //
    BmnFillTrigInfoDst* trig = new BmnFillTrigInfoDst(isExp);
    fRunAna->AddTask(trig);

    // ====================================================================== //
    // ===                   Hits in front of the target                  === //
    // ====================================================================== //
    if (run_period >= 8) {
        if (isExp) {
            BmnSiBTHitMaker* sibtHM = new BmnSiBTHitMaker(run_period, run_number, isExp);
            fRunAna->AddTask(sibtHM);
        }
    } else {
        if (!isExp) {
            BmnMwpcHitProducer* mwpcHP = new BmnMwpcHitProducer();
            fRunAna->AddTask(mwpcHP);
        }
        BmnMwpcHitFinder* mwpcHM = new BmnMwpcHitFinder(isExp, run_period, run_number);
        fRunAna->AddTask(mwpcHM);
    }

    // ====================================================================== //
    // ===                           VSP hit finder                       === //
    // ====================================================================== //
    BmnVSPHitMaker* vspHM = new BmnVSPHitMaker(run_period, run_number, isExp);
    if (isExp)
        vspHM->SetHitMatching(kFALSE);
    fRunAna->AddTask(vspHM);

    // ====================================================================== //
    // ===                         Silicon hit finder                     === //
    // ====================================================================== //
    BmnSiliconHitMaker* siliconHM = new BmnSiliconHitMaker(run_period, run_number, isExp);
    if (isExp)
        siliconHM->SetHitMatching(kFALSE);
    fRunAna->AddTask(siliconHM);

    // ====================================================================== //
    // ===                          GEM hit finder                        === //
    // ====================================================================== //
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(run_period, run_number, isExp);
    if (isExp)
        gemHM->SetHitMatching(kFALSE);
    fRunAna->AddTask(gemHM);

    // ====================================================================== //
    // ===                          CSC hit finder                        === //
    // ====================================================================== //
    BmnCSCHitMaker* cscHM = new BmnCSCHitMaker(run_period, run_number, isExp);
    if (!isExp)
        cscHM->SetCurrentConfig(BmnCSCConfiguration::FullCSC_Run8);   // set explicitly
    if (isExp)
        cscHM->SetHitMatching(kFALSE);
    fRunAna->AddTask(cscHM);

    // ====================================================================== //
    // ===                          TOF400 hit finder                     === //
    // ====================================================================== //
    BmnTof1HitProducer* tof400HP = new BmnTof1HitProducer("TOF400", kTOF1, !isExp, iVerbose, kFALSE);
    tof400HP->SetPeriodRun(run_period, run_number);
    fRunAna->AddTask(tof400HP);

    // ====================================================================== //
    // ===                          TOF700 hit finder                     === //
    // ====================================================================== //
    BmnTof1HitProducer* tof700HP = new BmnTof1HitProducer("TOF700", kTOF701, !isExp, iVerbose, kFALSE);
    tof700HP->SetPeriodRun(run_period, run_number);
    fRunAna->AddTask(tof700HP);

    // ====================================================================== //
    // ===                          DCH hit finder                        === //
    // ====================================================================== //
    // if (!isExp) {
    //     BmnDchHitProducer* dchHP = new BmnDchHitProducer();
    //     fRunAna->AddTask(dchHP);
    // }

    // ====================================================================== //
    // ===                         FHCAL/ZDC                              === //
    // ====================================================================== //
    bool IsGlobalCoordinates = true;
    if (run_period >= 8) {
        // TODO check isExp
        BmnFHCalReconstructor* fhcalReco = new BmnFHCalReconstructor(isExp, IsGlobalCoordinates);
        fhcalReco->SetRecoCutsFile(Form("FHCAL_reco_cuts_period%d.txt", run_period));
        fRunAna->AddTask(fhcalReco);
    } else {
        BmnZdcAnalyzer* zdcAna = new BmnZdcAnalyzer();
        fRunAna->AddTask(zdcAna);
    }

    // ====================================================================== //
    // ===                            ScWall                              === //
    // ====================================================================== //
    IsGlobalCoordinates = true;
    BmnScWallReconstructor* scwallReco = new BmnScWallReconstructor(isExp, IsGlobalCoordinates);
    scwallReco->SetRecoCutsFile(Form("SCWALL_reco_cuts_period%d.txt", run_period));
    fRunAna->AddTask(scwallReco);

    // ====================================================================== //
    // ===                             Hodo                               === //
    // ====================================================================== //

    BmnHodoReconstructor* hodoReco =
        new BmnHodoReconstructor(Form("HODO_Q_calibration_period%d.json", run_period), isExp);
    hodoReco->SetRecoCutsFile(Form("HODO_reco_cuts_period%d.json", run_period));
    fRunAna->AddTask(hodoReco);

    // ====================================================================== //
    // ===                             Ndet                               === //
    // ====================================================================== //
    IsGlobalCoordinates = true;
    BmnNdetReconstructor* ndetReco = new BmnNdetReconstructor(isExp, IsGlobalCoordinates);
    ndetReco->SetRecoCutsFile(Form("NDET_reco_cuts_period%d.txt", run_period));
    fRunAna->AddTask(ndetReco);

    // ====================================================================== //
    // ===                          Vacuum Wall                           === //
    // ====================================================================== //

    BmnVacWallReconstructor* vacReco = new BmnVacWallReconstructor();
    fRunAna->AddTask(vacReco);

    // ====================================================================== //
    // ===                       Inner Tracking                           === //
    // ====================================================================== //
    if (run_period > 8)
        CbmStsDigiScheme::Instance(9);   // AZ-310126

    TString innerTrackBranchName;   // use different track container
    if (tracking == L1Tracking)
        innerTrackBranchName = "StsTrack";
    else
        innerTrackBranchName = "StsVector";
    if (run_period >= 8) {
        BmnToCbmHitConverter* hitConverter = new BmnToCbmHitConverter(iVerbose, isExp);
        // AZ-150224 hitConverter->SetFixedErrors(0.05, 0.25, 0.02, 0.65);
        hitConverter->SetFixedErrors(0.08 / sqrt(12) * 1.2, 0.1234, 0.01 / sqrt(12), 0.1234);   // AZ-150224
        hitConverter->EnableAlignment(kFALSE);                                                  // AZ-310126
        // AZ-310126
        if (run_period > 8) {
            hitConverter->SetGemConfigFile("GemRun9.xml");
            hitConverter->SetSilConfigFile("SiliconRun9.xml");
            hitConverter->SetVspConfigFile("VSP_Run9.xml");
        }
        //
        fRunAna->AddTask(hitConverter);

        CbmKF* kalman = new CbmKF();
        fRunAna->AddTask(kalman);

        CbmL1* l1 = new CbmL1();
        TString stsMatBudgetFile = "";
        l1->SetMaterialBudgetFileName(stsMatBudgetFile);
        // AZ-310126 fRunAna->AddTask(l1);

        if (tracking == L1Tracking) {
            FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, "CbmL1StsTrackFinder");
            fRunAna->AddTask(stsFindTracks);
        }
        if (tracking == VectorFinder) {
            // BmnStsVectorFinder* vf = new BmnStsVectorFinder();
            BmnStsVectorFinderV9* vf = new BmnStsVectorFinderV9();
            vf->SetMatBudgetFileName("$VMCWORKDIR/parameters/reco/sts_matbudget_run8.root");
            vf->SetNofSiStations(5);   // AZ-310126 - 4Si+VSP
            fRunAna->AddTask(vf);
        }

        BmnStsMatchTracks* stsMatchTracks = new BmnStsMatchTracks(iVerbose);
        stsMatchTracks->SetTrackBranch(innerTrackBranchName);
        fRunAna->AddTask(stsMatchTracks);

    } else {
        BmnInnerTrackingRun7* innerTF = new BmnInnerTrackingRun7(run_number, isTarget);
        innerTF->SetFiltration(isExp);   // we use filtration for experimental data only now
        fRunAna->AddTask(innerTF);
    }

    // ====================================================================== //
    // ===                          Beam Tracking                         === //
    // ====================================================================== //
    if (run_period >= 8) {
        if (isExp) {
            BmnBeamTracking* beamTF = new BmnBeamTracking(run_period);
            fRunAna->AddTask(beamTF);
        }
    } else {
        BmnMwpcTrackFinder* mwpcTF = new BmnMwpcTrackFinder(isExp, run_period, run_number);
        fRunAna->AddTask(mwpcTF);
    }

    // ====================================================================== //
    // ===                          Tracking (DCH)                        === //
    // ====================================================================== //
    // BmnDchTrackFinder* dchTF = new BmnDchTrackFinder(run_period, run_number, isExp);
    // dchTF->SetTransferFunction("transfer_func.txt");
    // fRunAna->AddTask(dchTF);

    // ====================================================================== //
    // ===                      Primary vertex finding                    === //
    // ====================================================================== //
    // if (run_period >= 8) {
    CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
    CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
    findVertex->SetTrackBranch(innerTrackBranchName);
    fRunAna->AddTask(findVertex);

    // AZ-150224
    BmnKFPrimaryVertexFinder* pvMPD = new BmnKFPrimaryVertexFinder();
    fRunAna->AddTask(pvMPD);

    BmnCombineVertexFinder* combPV = new BmnCombineVertexFinder();
    fRunAna->AddTask(combPV);
    //
    // // } else {
    //     BmnVertexFinder* gemVF = new BmnVertexFinder(run_period);
    //     fRunAna->AddTask(gemVF);
    //}

    // ====================================================================== //
    // ===                          Global Tracking                       === //
    // ====================================================================== //
    Bool_t doAlign = kTRUE;
    if (!isExp)
        doAlign = kFALSE;
    BmnGlobalTracking* glTF = new BmnGlobalTracking(isExp, kFALSE /*doAlign*/);
    glTF->SetRunNumber(run_number);
    glTF->SetInnerTracksBranchName(innerTrackBranchName);
    fRunAna->AddTask(glTF);

    // BmnVertexFinder* gemVF = new BmnVertexFinder(run_period);
    // fRunAna->AddTask(gemVF);

    // ====================================================================== //
    // ===           Matching global track to MC track procedure          === //
    // ====================================================================== //
    if (!isExp) {
        BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
        mcMatching->SetInnerTracksBranchName(innerTrackBranchName);
        fRunAna->AddTask(mcMatching);
    }

    // ====================================================================== //
    // ===                      PID procedure                             === //
    // ====================================================================== //
    BmnPid* pid_analyser = new BmnPid();
    fRunAna->AddTask(pid_analyser);

    // ====================================================================== //
    // ===                      Residual analysis                         === //
    // ====================================================================== //
    if (run_period < 8) {
        BmnResiduals* res = new BmnResiduals(run_period, run_number);
        fRunAna->AddTask(res);
    }

    // ====================================================================== //
    // ===                         Centrality                             === //
    // ====================================================================== //
    CentralityClusterizer* cclusterizer = new CentralityClusterizer("XeCsI@3.8AGeV_MBT_7clusters_period8.root");
    fRunAna->AddTask(cclusterizer);

    // Fill DST Event Header
    BmnFillDstTask* dst_task = new BmnFillDstTask(nStartEvent, nEvents, isExp, run_period, run_number);
    fRunAna->AddTask(dst_task);

    // -----   Parameter database (input)  -------------------------------------
    FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(inFileName.Data());
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
    delete pvFinder;
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;   // marker of successful execution
    cout << "Input  file is " + inFileName << endl;
    cout << "Output file is " + dstFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}
