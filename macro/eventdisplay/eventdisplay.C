Bool_t SetupOffline(TString simFile, TString recoFile);
Bool_t SetupOnline(TString endpoint);
void SetupSim(TString simFile, TString recoFile);
void SetupExp(TString recoFile);
Bool_t LoadGeometry(Int_t runPeriod, Int_t runNumber, Bool_t latest = kFALSE);
void SetSimTasks(Color_t mcPointColor = kRed, Color_t recoPointColor = kBlack, Style_t pointMarker = kFullDotSmall);
void SetExpTasks(Color_t pointColor = kRed, Style_t pointMarker = kFullDotSmall);

/// @brief Event Display macro for experimental and simulated data
/// @param simFile ROOT file with MC data (and detector geometry) in case of MC visualization
/// @param recoFile ROOT file with reconstructed data for simulated or experimental
/// @param endpoint ZMQ publisher socket url, for example tcp://localhost:5557.
/// If the values is not empty, the Event Display is started in online mode
void eventdisplay(TString simFile = "$VMCWORKDIR/macro/run/bmnsim.root",
                  TString recoFile = "$VMCWORKDIR/macro/run/bmndst.root",
                  TString endpoint = "")
{
    FairRunAna* runAnaManager = new FairRunAna();
    MpdEventManager* eventManager = new MpdEventManager();

    if (endpoint != "") {
        if (!SetupOnline(endpoint))
            return;
    } else {
        // Offline mode
        if (!SetupOffline(simFile, recoFile))
            return;
    }
    // set output file
    runAnaManager->SetSink(new FairRootFileSink("ed_out.root"));
    runAnaManager->SetGenerateRunInfo(false);

    // light background color by default
    eventManager->background_color = 17;
    eventManager->isDarkColor = false;

    eventManager->Init(3, 3, 10000);
}

Bool_t SetupOffline(TString simFile, TString recoFile)
{
    if (!BmnFunctionSet::CheckFileExist(recoFile)) {
        LOG(error) << "Reconstruction file with was not found: " << recoFile;
        return kFALSE;
    }

    if (BmnFunctionSet::isSimulationFile(recoFile)) {
        // Simulation
        if (!BmnFunctionSet::CheckFileExist(simFile)) {
            LOG(error) << "ERROR: simulation file with detector geometry was not found: " << simFile;
            return kFALSE;
        }

        SetupSim(simFile, recoFile);
        SetSimTasks();
    } else {
        // Experiment
        SetupExp(recoFile);
        SetExpTasks();
    }

    return kTRUE;
}

Bool_t SetupOnline(TString endpoint)
{
    FairRunAna::Instance()->SetSource(new BmnOnlineSource(endpoint));

    if (!LoadGeometry(-1, -1, kTRUE))
        return kFALSE;

    MpdEventManager::Instance()->isOnline = kTRUE;
    MpdEventManager::Instance()->iDataSource = kTRUE;

    SetExpTasks();

    return kTRUE;
}

void SetupSim(TString simFile, TString recoFile)
{
    auto fileSource = new FairFileSource(simFile);
    // add file with reconstructed data as a friend
    fileSource->AddFriend(recoFile);

    FairRunAna::Instance()->SetSource(fileSource);

    // set parameter file with MC data and detector geometry
    auto runtimeDb = FairRunAna::Instance()->GetRuntimeDb();
    auto parIo = new FairParRootFileIo();
    parIo->open(simFile);
    runtimeDb->setFirstInput(parIo);
    //! -----------------------------------
    // runtimeDb->setOutput(parIo);
    // runtimeDb->saveOutput();
    //! -----------------------------------

    MpdEventManager::Instance()->iDataSource = kFALSE;
}

void SetupExp(TString recoFile)
{
    Int_t runPeriod, runNumber;

    FairRunAna::Instance()->SetSource(new BmnFileSource(recoFile, runPeriod, runNumber));
    LoadGeometry(runPeriod, runNumber);

    MpdEventManager::Instance()->iDataSource = kTRUE;
}

Bool_t LoadGeometry(Int_t runPeriod, Int_t runNumber, Bool_t latest = kFALSE)
{
    // get geometry for run
    TString geoFilePath = Form("current_geo_file_%d.root", TRandom(0).Integer(UINT32_MAX));

    int loadGeometryResult = 1;
    if (latest) {
        runPeriod = -1;
        runNumber = -1;
        loadGeometryResult = UniRun::ReadPreviousGeometryFile(runPeriod, runNumber, (char*)geoFilePath.Data());
    } else
        loadGeometryResult = UniRun::ReadGeometryFile(runPeriod, runNumber, (char*)geoFilePath.Data());

    if (loadGeometryResult != 0) {
        LOG(error) << "Could not read geometry file from the database for run period %d and run number %d!" << runPeriod
                   << runNumber;
        return kFALSE;
    }

    // get gGeoManager from ROOT file (if required)
    auto geoFile = new TFile(geoFilePath, "READ");
    if (!geoFile->IsOpen()) {
        LOG(error) << "Could not open ROOT file with geometry: " << geoFilePath;
        return kFALSE;
    }

    TIter next(geoFile->GetListOfKeys());
    auto key = static_cast<TKey*>(next());
    if (TString(key->GetClassName()).BeginsWith("TGeoManager"))
        key->ReadObj();
    else {
        LOG(error) << "TGeoManager is not top element in geometry file " << geoFilePath;
        return kFALSE;
    }

    geoFile->Close();
    remove(geoFilePath);

    return kTRUE;
}

void SetSimTasks(Color_t mcPointColor = kRed, Color_t recoPointColor = kBlack, Style_t pointMarker = kFullDotSmall)
{
    vector<TString> mcTasks = {"ArmTrigPoint", "BCPoint",     "FDPoint",     "MWPCPoint", "BdPoint",
                               "SiliconPoint", "StsPoint",    "CSCPoint",    "SiBTPoint", "SiProfPoint",
                               "TOF400Point",  "DCHPoint",    "TOF700Point", "SsdPoint",  "VacWallPoint",
                               "HodoPoint",    "ScWallPoint", "FHCalPoint",  "NdetPoint", "TOF701Point"};

    vector<TString> recoTasks = {
        "BmnMwpcHit", "BmnArmTrigHit", "BmnBCHit",  "BmnGemStripHit", "BmnTof400Hit", "StsHit",
        "BmnDchHit",  "BmnTof701Hit",  "BmnCSCHit", "BmnSiliconHit",  "BmnTof700Hit", "BmnUpstreamHit",
    };

    // draw MC Points
    for (auto mcTask : mcTasks)
        MpdEventManager::Instance()->AddTask(new MpdMCPointDraw(mcTask, mcPointColor, pointMarker));

    // draw MC Geometry Tracks
    MpdEventManager::Instance()->AddTask(new MpdMCTracks("GeoTracks"));

    // draw Reconstructed Detector Hits
    for (auto recoTask : recoTasks)
        MpdEventManager::Instance()->AddTask(new MpdHitPointSetDraw(recoTask, recoPointColor, pointMarker));

    MpdEventManager::Instance()->AddTask(new BmnCaloTowerDraw("ZdcDigit", kZDC));
    MpdEventManager::Instance()->AddTask(new BmnCaloTowerDraw("FHCalDigit", kFHCAL));
    MpdEventManager::Instance()->AddTask(new BmnCaloTowerDraw("EcalDigit", kECAL));

    // draw MWPC Tracks
    MpdEventManager::Instance()->AddTask(new BmnTrackDrawH("BmnMwpcTrack", "BmnMwpcHit"));
    // draw GEM Tracks
    MpdEventManager::Instance()->AddTask(new BmnTrackDrawH("BmnGemTrack", "BmnGemStripHit"));
    MpdEventManager::Instance()->AddTask(new BmnTrackDrawH("StsTrack", "StsHit"));
    MpdEventManager::Instance()->AddTask(new BmnTrackDrawH("StsVector", "StsHit"));

    // draw Reconstructed Global Tracks
    MpdEventManager::Instance()->AddTask(new BmnGlobalTrackDraw("BmnGlobalTrack"));
}

void SetExpTasks(Color_t pointColor = kRed, Style_t pointMarker = kFullDotSmall)
{
    vector<TString> expTasks = {
        "BmnTof701Hit", "BmnSiliconHit", "BmnGemStripHit", "BmnDchHit", "BmnSiBTHit", "StsHit",
        "BmnTof400Hit", "BmnTof700Hit",  "BmnUpstreamHit", "BmnCSCHit", "BmnMwpcHit",
    };

    // draw Reconstructed Detector Hits
    for (auto expTask : expTasks) {
        MpdEventManager::Instance()->AddTask(new MpdHitPointSetDraw(expTask, pointColor, pointMarker));
    }

    MpdEventManager::Instance()->AddTask(new BmnCaloTowerDraw("ZdcDigit", kZDC));
    MpdEventManager::Instance()->AddTask(new BmnCaloTowerDraw("FHCalDigit", kFHCAL));
    MpdEventManager::Instance()->AddTask(new BmnCaloTowerDraw("EcalDigit", kECAL));

    // draw MWPC Tracks
    MpdEventManager::Instance()->AddTask(new BmnTrackDrawH("BmnMwpcTrack", "BmnMwpcHit"));
    // draw GEM Tracks
    MpdEventManager::Instance()->AddTask(new BmnTrackDrawH("BmnGemTrack", "BmnGemStripHit"));
    MpdEventManager::Instance()->AddTask(new BmnTrackDrawH("BmnBeamTrack", "BmnSiBTHit"));
    MpdEventManager::Instance()->AddTask(new BmnTrackDrawH("StsTrack", "StsHit"));
    MpdEventManager::Instance()->AddTask(new BmnTrackDrawH("StsVector", "StsHit"));

    // draw Reconstructed Global Tracks
    MpdEventManager::Instance()->AddTask(new BmnGlobalTrackDraw("BmnGlobalTrack"));
}
