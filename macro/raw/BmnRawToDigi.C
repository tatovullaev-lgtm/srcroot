#include <FairLogger.h>

// file: full path to raw file
// nEvents: if 0 then decode all events

void BmnRawToDigi(TString file = "", TString outfile = "", Long_t nEvents = 0, UInt_t period = 8)
{

    Int_t iVerbose = 1;   ///<- Verbosity level: 0 - Progress Bar; 1 - short info on passed events
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");

    TStopwatch timer;
    timer.Start();

    BmnDecoder* decoder = new BmnDecoder(file, outfile, 0, 8);
    decoder->SetVerbose(iVerbose);
    BmnSetup stp = kBMNSETUP;
    std::map<DetectorId, bool> setup;       // flags to determine BM@N setup
    setup.insert(std::make_pair(kBC, 1));   // TRIGGERS
    // setup.insert(std::make_pair(kMWPC,      0)); // MWPC
    setup.insert(std::make_pair(kSILICON, 1));   // SILICON
    setup.insert(std::make_pair(kGEM, 1));       // GEM
    setup.insert(std::make_pair(kTOF1, 1));      // TOF-400
    setup.insert(std::make_pair(kTOF, 1));       // TOF-700
    setup.insert(std::make_pair(kDCH, 1));       // DCH
    // setup.insert(std::make_pair(kZDC,       0)); // ZDC
    // setup.insert(std::make_pair(kECAL,      0)); // ECAL
    setup.insert(std::make_pair(kCSC, 1));      // CSC
    setup.insert(std::make_pair(kSCWALL, 1));   // SCWALL
    setup.insert(std::make_pair(kFHCAL, 1));    // FHCAL
    setup.insert(std::make_pair(kHODO, 1));     // HODO
    setup.insert(std::make_pair(kNDET, 1));     // Neutron Calorimeter
    setup.insert(std::make_pair(kSiBT, 1));     // SiBD
    decoder->SetDetectorSetup(setup);
    TString PeriodSetupExt = Form("%d%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"));
    decoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt);
    decoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt);
    decoder->SetSiliconMapping(TString("SILICON_map_run") + PeriodSetupExt);
    decoder->SetSiBTMapping(TString("SiBT_map_period") + PeriodSetupExt);
    decoder->SetGemMapping(TString("GEM_map_run") + PeriodSetupExt);
    decoder->SetCSCMapping(TString("CSC_map_period")
                           + Form("%d%s%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"),
                                  (decoder->GetRunId() < 7013 ? "_before_run_7013" : "")));
    decoder->SetMSCMapping(TString("MSC_map_Run") + PeriodSetupExt);
    // in case comment out the line decoder->SetTof400Mapping("...")
    // the maps of TOF400 will be read from DB (only for JINR network)
    decoder->SetTOF700ReferenceRun(7012);
    decoder->SetTof700Geom(TString("TOF700_geometry_run") + PeriodSetupExt);
    decoder->SetTof400Mapping(TString("TOF400_PlaceMap_RUN") + PeriodSetupExt,
                              TString("TOF400_StripMap_RUN") + PeriodSetupExt);
    if (decoder->GetRunId() >= 4278 && decoder->GetPeriodId() == 7)
        decoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d_from_run_4278.txt", period));
    else
        decoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d.txt", period));
    // decoder->SetZDCMapping("ZDC_map_period_5.txt");
    // decoder->SetZDCCalibration("zdc_muon_calibration.txt");
    decoder->SetScWallMapping(TString("SCWALL_map_period") + PeriodSetupExt);
    decoder->SetScWallCalibration(TString("SCWALL_calibration_period") + PeriodSetupExt);
    decoder->SetFHCalMapping(TString("FHCAL_map_period") + PeriodSetupExt);
    decoder->SetFHCalCalibration(TString("FHCAL_calibration_period") + PeriodSetupExt);
    decoder->SetHodoMapping(Form("HODO_map_period%d%s.json", period, ((stp == kBMNSETUP) ? "" : "_SRC")));
    decoder->SetHodoCalibration(Form("HODO_Q_calibration_period%d%s.json", period, ((stp == kBMNSETUP) ? "" : "_SRC")));
    decoder->SetNdetMapping(TString("NDET_map_period") + PeriodSetupExt);
    if ((decoder->GetRunId() >= 7506 && decoder->GetRunId() <= 7534)
        || (decoder->GetRunId() >= 8203 && decoder->GetRunId() <= 8258))
        decoder->SetNdetCalibration(TString("NDET_calibration_period")
                                    + Form("%d_run_7506_7534_or_8203_8258.txt", period));
    else
        decoder->SetNdetCalibration(TString("NDET_calibration_period") + PeriodSetupExt);
    // decoder->SetECALMapping(TString("ECAL_map_period_") + PeriodSetupExt);
    // decoder->SetECALCalibration("");
    // decoder->SetMwpcMapping(TString("MWPC_map_period") + ((period == 6 && decoder->GetRunId() < 1397) ? 5 :
    // PeriodSetupExt));

    decoder->SetApplyThreshold(kTRUE);   // set kFALSE to check pedestals
    decoder->SetUseCalibFile(kFALSE);    // set kTRUE to save and load strip signal calibration data
    decoder->DecodeDataToDigi();

    timer.Stop();
    printf("\tMacro finished successfully!\n\n");   // marker of successfully execution for software testing systems
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("Real time %f s, CPU time %f s\n", rtime, ctime);
    delete decoder;
}
