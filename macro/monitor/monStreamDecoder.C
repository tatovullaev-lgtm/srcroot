void monStreamDecoder(TString addr = "localhost:21323",   //"c5n18.he.jinr.ru:213222"
                      Int_t periodID = 9,
                      BmnSetup bmnSetup = kBMNSETUP)
{
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("info");   // info/debug/warning ...

    BmnOnlineDecoder* deco = new BmnOnlineDecoder();
    deco->SetPeriodID(periodID);
    deco->SetBmnSetup(bmnSetup);
    deco->SetDaqAddress(addr);
    std::map<DetectorId, bool> setup;            // flags to determine BM@N setup
    setup.insert(std::make_pair(kBC, 1));        // TRIGGERS
    setup.insert(std::make_pair(kSILICON, 1));   // SILICON
    setup.insert(std::make_pair(kGEM, 1));       // GEM
    setup.insert(std::make_pair(kTOF1, 1));      // TOF-400
    // setup.insert(std::make_pair(kTOF, 1));       // TOF-700
    setup.insert(std::make_pair(kTOF701, 1));   // TOF-701
    setup.insert(std::make_pair(kCSC, 1));      // CSC
    setup.insert(std::make_pair(kSCWALL, 1));   // SCWALL
    setup.insert(std::make_pair(kFHCAL, 1));    // FHCAL
    setup.insert(std::make_pair(kHODO, 1));     // HODO
    setup.insert(std::make_pair(kNDET, 1));     // Neutron Calorimeter Prototype
    setup.insert(std::make_pair(kHGND, 0));     // Neutron Calorimeter
    setup.insert(std::make_pair(kSiBT, 1));     // SiBD
    setup.insert(std::make_pair(kVSP, 1));      // VSP
    deco->SetDetectorSetup(setup);
    deco->ProcessStream();
    delete deco;
}
