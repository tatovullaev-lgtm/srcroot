#include "../run/run_reco_bmn.C"
#include "updateContent.C"

/**
 * Macro for processing test data before beam available
 * @param input raw binary data
 * @param output output DST file
 * @param nEvents events to process
 * @param period
 * @param doConvert
 * @param doHoldRawRoot
 * @return
 */
int BmnPreRunTester(TString input = "",
                    TString output = "",
                    Long_t nEvents = 0,
                    UInt_t period = 9,
                    bool doConvert = true,
                    bool doHoldRawRoot = true)
{
    gSystem->ExpandPathName(input);
    gSystem->ExpandPathName(output);

    Int_t iVerbose = 0;   ///<- Verbosity level: 0 - summary info on progress; 1 - event level, 2 - event objects level,
    ///< 3 - full debug output
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("info");   // for info write "INFO"
    BmnSetup stp = kBMNSETUP;

    TStopwatch timer;
    TStopwatch timer_conv;
    timer.Start();
    TString root_file_name;
    BmnRawSource* conv = new BmnRawSource(input, "", nEvents, period);
    root_file_name = conv->GetRootFileName();
    if (doConvert) {
        timer_conv.Start();
        BmnStatus convert_status = conv->ConvertRawToRoot();
        delete conv;
        timer_conv.Stop();
        if (convert_status == kBMNERROR) {
            printf("Converter failed!\n");
            return -1;
        }
    }
    //    else
    //        root_file_name = file;
    TString root_file_name_marked_ped = TString("marked_ped_") + root_file_name;
    if (updateContent(root_file_name, root_file_name_marked_ped)) {
        printf("updateContent failed!\n");
        return -2;
    }
    BmnDecoder* decoder = new BmnDecoder(root_file_name_marked_ped, "", nEvents, period);
    decoder->SetBmnSetup(stp);
    decoder->SetVerbose(iVerbose);

    std::map<DetectorId, bool> setup;       // flags to determine BM@N setup
    setup.insert(std::make_pair(kBC, 0));   // TRIGGERS
    // setup.insert(std::make_pair(kMWPC,      0)); // MWPC
    setup.insert(std::make_pair(kSILICON, 0));   // SILICON
    setup.insert(std::make_pair(kGEM, 1));       // GEM
    setup.insert(std::make_pair(kTOF1, 0));      // TOF-400
    setup.insert(std::make_pair(kTOF, 0));       // TOF-700
    setup.insert(std::make_pair(kTOF701, 0));    // TOF-701
    setup.insert(std::make_pair(kDCH, 0));       // DCH
    // setup.insert(std::make_pair(kZDC,       0)); // ZDC
    // setup.insert(std::make_pair(kECAL,      0)); // ECAL
    setup.insert(std::make_pair(kCSC, 0));      // CSC
    setup.insert(std::make_pair(kSCWALL, 0));   // SCWALL
    setup.insert(std::make_pair(kFHCAL, 0));    // FHCAL
    setup.insert(std::make_pair(kHODO, 0));     // HODO
    setup.insert(std::make_pair(kNDET, 0));     // Neutron Calorimeter Prototype
    setup.insert(std::make_pair(kHGND, 0));     // Neutron Calorimeter
    setup.insert(std::make_pair(kSiBT, 0));     // SiBD
    decoder->SetDetectorSetup(setup);

    // Set converter/decoder general parameters
    decoder->SetDecoderConfigFileName(string(getenv("VMCWORKDIR")) + "/config/bmnconf.json");

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
    decoder->SetTof400Mapping(TString("TOF400_PlaceMap_RUN") + PeriodSetupExt,
                              TString("TOF400_StripMap_RUN") + PeriodSetupExt);
    decoder->SetTof701Mapping(TString("TOF701_PlaceMap_RUN") + PeriodSetupExt,
                              TString("TOF701_StripMap_RUN") + PeriodSetupExt);
    // decoder->SetZDCMapping("ZDC_map_period_5.txt");
    // decoder->SetZDCCalibration("zdc_muon_calibration.txt");
    decoder->SetScWallMapping(TString("SCWALL_map_period") + PeriodSetupExt);
    decoder->SetScWallCalibration(TString("SCWALL_calibration_period") + PeriodSetupExt);
    decoder->SetFHCalMapping(TString("FHCAL_map_period") + PeriodSetupExt);
    decoder->SetFHCalCalibration(TString("FHCAL_calibration_period") + PeriodSetupExt);
    decoder->SetHodoMapping(TString("HODO_Q_map_period") + PeriodSetupExt);
    decoder->SetHodoCalibration(TString("HODO_Q_calibration_period") + PeriodSetupExt);
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

    decoder->SetApplyThreshold(kTRUE);                    // set kFALSE to check pedestals
    decoder->SetUseCalibFile(kFALSE);                     // set kTRUE to save and load strip signal calibration data
    BmnStatus decoStatus = decoder->DecodeDataToDigi();   // Decode data into detector-digits using current mappings.
    timer.Stop();
    if (decoStatus == kBMNSUCCESS) {
        if (!doHoldRawRoot)
            gSystem->Exec(TString::Format("rm -f %s", decoder->GetRootFileName().Data()));
        printf("\tMacro finished successfully!\n\n");   // marker of successfully execution for software testing systems
        if (doConvert) {
            Double_t rtime_conv = timer_conv.RealTime();
            Double_t ctime_conv = timer_conv.CpuTime();
            printf("Converter: Real time %f s, CPU time %f s\n", rtime_conv, ctime_conv);
        }
        Double_t rtime = timer.RealTime();
        Double_t ctime = timer.CpuTime();
        printf("All steps: Real time %f s, CPU time %f s\n", rtime, ctime);
    }
    TString digi_file_name = decoder->GetDigiFileName();
    delete decoder;
    run_reco_bmn(digi_file_name, output, 0, nEvents);
    return 0;
}
