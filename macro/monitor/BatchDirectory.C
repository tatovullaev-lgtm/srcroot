
// R__LOAD_LIBRARY(libFairMQ)
void BatchDirectory(TString dirName = "/bmn/run/current/", BmnSetup stp = kBMNSETUP, Int_t periodId = 8)
{
    // #if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    //     gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    // #endif
    //     bmnloadlibs();
    //     int64_t gl = gSystem->Load("libBmnMonitor");
    //     printf("gl %ld\n", gl);
    BmnOnlineDecoder* bm = new BmnOnlineDecoder();
    bm->SetBmnSetup(stp);
    bm->SetPeriodID(periodId);

    std::map<DetectorId, bool> setup;            // flags to determine BM@N setup
    setup.insert(std::make_pair(kBC, 1));        // TRIGGERS
    setup.insert(std::make_pair(kMWPC, 1));      // MWPC
    setup.insert(std::make_pair(kSILICON, 1));   // SILICON
    setup.insert(std::make_pair(kGEM, 1));       // GEM
    setup.insert(std::make_pair(kTOF1, 1));      // TOF-400
    setup.insert(std::make_pair(kTOF, 1));       // TOF-700
    setup.insert(std::make_pair(kTOF701, 1));    // TOF-701
    setup.insert(std::make_pair(kDCH, 0));       // DCH
    setup.insert(std::make_pair(kZDC, 0));       // ZDC
    setup.insert(std::make_pair(kECAL, 0));      // ECAL
    setup.insert(std::make_pair(kCSC, 1));       // CSC
    setup.insert(std::make_pair(kSCWALL, 1));    // SCWALL
    setup.insert(std::make_pair(kFHCAL, 1));     // FHCAL
    setup.insert(std::make_pair(kHODO, 1));      // HODO
    setup.insert(std::make_pair(kSiBT, 1));      // kSiBT
    setup.insert(std::make_pair(kNDET, 1));      // NDET
    bm->SetDetectorSetup(setup);

    bm->BatchDirectory(dirName);
    delete bm;
}
