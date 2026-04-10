// Macro for reconstruction of simulated and experimental BM@N events
//
// inFileName - input file with data (MC or exp. data)
// dstFileName - output file with reconstructed data (full DST format)
// nStartEvent - number of first event to process (starts with zero), default: 0
// nEvents - number of events to process after nStartEvent, 0 - all events of the given file after nStartEvent to
// process
R__ADD_INCLUDE_PATH($VMCWORKDIR)
enum enumTracking
{
    VectorFinder,
    L1Tracking,
    CellAuto
};

void run_reco_bmn(TString inFileName = "$VMCWORKDIR/macro/run/bmnsim.root",
                  TString dstFileName = "$VMCWORKDIR/macro/run/bmndst.root",
                  Int_t nStartEvent = 0,
                  Int_t nEvents = 10,
                  enumTracking tracking = VectorFinder)
{
    gDebug = 0;   // Debug option
    // Verbosity level (0 = summary only, 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 0;
    FairLogger::GetLogger()->SetLogScreenLevel("info");

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
    Int_t run_period = 8, run_number = -1;
    Double_t fieldScale = 0.;
    if (!isExp)   // for simulation files
        fFileSource = new FairFileSource(inFileName);
    else   // for experimental files
    {
        // set source as raw root data file (without additional directories)
        fFileSource = new BmnFileSource(inFileName, run_period, run_number);

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
    // ===                          TOF1 hit finder                       === //
    // ====================================================================== //
    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", !isExp, iVerbose, kFALSE);
    tof1HP->SetPeriodRun(run_period, run_number);
    fRunAna->AddTask(tof1HP);

    // ====================================================================== //
    // ===                          TOF2 hit finder                       === //
    // ====================================================================== //
    BmnTofHitProducer* tof2HP = new BmnTofHitProducer("TOF", "TOF700_geometry_run8.txt", !isExp, iVerbose, kFALSE);
    tof2HP->SetTimeResolution(0.115);
    tof2HP->SetProtonTimeCorrectionFile("bmn_run9687_digi_calibration.root");
    tof2HP->SetMCTimeFile("TOF700_MC_xenon_dcmsmm_time_run8.txt");
    tof2HP->SetMainStripSelection(0);   // 0 - minimal time, 1 - maximal amplitude
    tof2HP->SetSelectXYCalibration(
        3);   // 0 - Petukhov, 1 - Panin SRC, 2 - Petukhov Argon (default), 3 - Petukhov Xenon (preliminary)
    tof2HP->SetTimeMin(-2.f);            // minimal digit time
    tof2HP->SetTimeMax(+50.f);           // Maximal digit time
    tof2HP->SetDiffTimeMaxSmall(1.2f);   // Abs maximal difference for small chambers
    tof2HP->SetDiffTimeMaxBig(3.5f);     // Abs maximal difference for big chambers
    fRunAna->AddTask(tof2HP);

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
        fhcalReco->SetRecoCutsFile("FHCAL_reco_cuts_period8.txt");
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
    scwallReco->SetRecoCutsFile("SCWALL_reco_cuts_period8.txt");
    fRunAna->AddTask(scwallReco);

    // ====================================================================== //
    // ===                             Hodo                               === //
    // ====================================================================== //

    BmnHodoReconstructor* hodoReco = new BmnHodoReconstructor("", isExp);
    hodoReco->SetRecoCutsFile("HODO_reco_cuts_period8.txt");
    fRunAna->AddTask(hodoReco);

    // ====================================================================== //
    // ===                             Ndet                               === //
    // ====================================================================== //
    IsGlobalCoordinates = true;
    BmnNdetReconstructor* ndetReco = new BmnNdetReconstructor(isExp, IsGlobalCoordinates);
    ndetReco->SetRecoCutsFile("NDET_reco_cuts_period8.txt");
    fRunAna->AddTask(ndetReco);

    // ====================================================================== //
    // ===                          Vacuum Wall                           === //
    // ====================================================================== //

    BmnVacWallReconstructor* vacReco = new BmnVacWallReconstructor();
    fRunAna->AddTask(vacReco);

    // ====================================================================== //
    // ===                       Inner Tracking                           === //
    // ====================================================================== //
    TString innerTrackBranchName;   // use different track container
    if (tracking == L1Tracking)
        innerTrackBranchName = "StsTrack";
    else
        innerTrackBranchName = "StsVector";
    if (run_period >= 8) {
        BmnToCbmHitConverter* hitConverter = new BmnToCbmHitConverter(iVerbose, isExp);
        // AZ-150224 hitConverter->SetFixedErrors(0.05, 0.25, 0.02, 0.65);
        hitConverter->SetFixedErrors(0.08 / sqrt(12) * 1.2, 0.1234, 0.01 / sqrt(12), 0.1234);   // AZ-150224
        // hitConverter->EnableAlignment(kFALSE);
        fRunAna->AddTask(hitConverter);

        CbmKF* kalman = new CbmKF();
        fRunAna->AddTask(kalman);

        CbmL1* l1 = new CbmL1();
        TString stsMatBudgetFile = "";
        l1->SetMaterialBudgetFileName(stsMatBudgetFile);
        fRunAna->AddTask(l1);

        if (tracking == L1Tracking) {
            FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, "CbmL1StsTrackFinder");
            fRunAna->AddTask(stsFindTracks);
        }
        if (tracking == VectorFinder) {
            BmnStsVectorFinder* vf = new BmnStsVectorFinder();
            vf->SetMatBudgetFileName("$VMCWORKDIR/parameters/sts_matbudget_run8.root");
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
    BmnFillDstTask* dst_task = new BmnFillDstTask(nEvents, isExp, run_period, run_number);
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
    cout << "Input  file is " + inFileName << endl;
    cout << "Output file is " + dstFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}
