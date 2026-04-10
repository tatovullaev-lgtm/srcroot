// #include <BmnEnums.h>
//  file: full path to raw file
//  nEvents: if 0 then decode all events
//  doConvert: convert RAW --> ROOT before decoding or use file converted before
//  todo : 0: RAW_ROOT -> DIGI, 1: RAW -> DIGI, 2: RAW -> RAW_ROOT


//include for compile ++
#include <TString.h>
#include <TSystem.h>
#include <FairLogger.h>
#include <TStopwatch.h>
#include </zfs/scratch/lhep-src/srcroot/bmnroot/decoder/BmnRawDataDecoder.h>
#include </zfs/scratch/lhep-src/srcroot/bmnroot/base/BmnEnums.h>
#include "run_raw_bmn_Sasha.h"


TString gRawRootFilePath = "";   // store intermediate RAW ROOT file path

void run_raw_bmn(TString file = "mpd_run_Top_4032.data",
                 TString outfile = "4032_digi_test.root",
                 Long_t nEvents = 0,
                 UInt_t period = 8,
                 UInt_t todo = 1,
                 Bool_t doHoldRawRoot = kTRUE)
{
    gSystem->ExpandPathName(file);
    gSystem->ExpandPathName(outfile);
    bool doConvert = (todo == 1) || (todo == 2);
    bool doDecode = (todo == 1) || (todo == 0);

    Int_t iVerbose = 1;   ///<- Verbosity level: 0 - summary info on progress; 1 - event level, 2 - event objects level,
                          ///< 3 - full debug output
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("info");   // for info write "INFO"

    TStopwatch timer;
    TStopwatch timer_conv;
    timer.Start();
    cout<<1<<endl;
    cerr<<"1e"<<endl;
    BmnRawDataDecoder* rdd = new BmnRawDataDecoder(file, outfile, nEvents, period);
    BmnStatus convert_status;
    BmnStatus decode_status;
    //    rdd->SetRawRootDirPath("/dev/shm/temp_raw/");
    // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnSetup stp = kSRCSETUP;   //(decoder->GetRunId() >= 2041 && decoder->GetRunId() <= 3588) ? kSRCSETUP : kBMNSETUP;
    rdd->SetBmnSetup(stp);
    rdd->SetVerbose(iVerbose);
    cout<<2<<endl;
    std::map<DetectorId, bool> setup;       // flags to determine BM@N setup
    setup.insert(std::make_pair(kBC, 1));   // TRIGGERS
    setup.insert(std::make_pair(kMWPC, 1)); // MWPC
    setup.insert(std::make_pair(kSILICON, 1));   // SILICON
    // setup.insert(std::make_pair(kGEM, 1));       // GEM
    setup.insert(std::make_pair(kTOF1, 1));      // TOF-400
    // setup.insert(std::make_pair(kTOF, 1));       // TOF-700
    // setup.insert(std::make_pair(kTOF701, 1));   // TOF-701
    setup.insert(std::make_pair(kDCH, 1));      // DCH
    // // setup.insert(std::make_pair(kZDC,       0)); // ZDC
    // // setup.insert(std::make_pair(kECAL,      0)); // ECAL
    setup.insert(std::make_pair(kLAND,      0)); // LAND
    setup.insert(std::make_pair(kTOFCAL,    1)); // LAND
    // setup.insert(std::make_pair(kCSC, 1));      // CSC
    // setup.insert(std::make_pair(kSCWALL, 1));   // SCWALL
    // setup.insert(std::make_pair(kFHCAL, 1));    // FHCAL
    // setup.insert(std::make_pair(kHODO, 1));     // HODO
    // setup.insert(std::make_pair(kNDET, 1));     // Neutron Calorimeter Prototype
    // setup.insert(std::make_pair(kHGND, 1));     // Neutron Calorimeter
    // setup.insert(std::make_pair(kSiBT, 1));     // SiBD
    // setup.insert(std::make_pair(kVSP, 1));      // VSP
    rdd->SetDetectorSetup(setup);
    cout<<3<<endl;
    // Set converter/decoder general parameters
    rdd->SetDecoderConfigFileName(string(getenv("VMCWORKDIR")) + "/config/bmnconf.json");
    cout<<std::flush<<endl;
    cout<<4<<endl;
    if (doConvert) {
        timer_conv.Start();
        BmnStatus convert_status = rdd->ConvertRawToRoot();   // Convert raw data in .data format into adc-,tdc-, ...,
                                                              // sync-digits in .root format
        if (convert_status == kBMNERROR)
            return;
        gRawRootFilePath = rdd->GetRootFileName();
        timer_conv.Stop();
    }
    cout<<5<<endl;
    if (doDecode) {
        rdd->CreateDecoder();
        cout<<6<<endl;
        BmnDecoder* decoder = rdd->GetDecoder();
        cout<<7<<endl;
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
        decoder->SetHodoMapping(Form("HODO_map_period%d%s.json", period, ((stp == kBMNSETUP) ? "" : "_SRC")));
        decoder->SetHodoCalibration(
            Form("HODO_Q_calibration_period%d%s.json", period, ((stp == kBMNSETUP) ? "" : "_SRC")));
        decoder->SetNdetMapping(TString("NDET_map_period") + PeriodSetupExt);
        if ((decoder->GetRunId() >= 7506 && decoder->GetRunId() <= 7534)
            || (decoder->GetRunId() >= 8203 && decoder->GetRunId() <= 8258))
            decoder->SetNdetCalibration(TString("NDET_calibration_period")
                                        + Form("%d_run_7506_7534_or_8203_8258.txt", period));
        else
            decoder->SetNdetCalibration(TString("NDET_calibration_period") + PeriodSetupExt);
        // decoder->SetECALMapping(TString("ECAL_map_period_") + PeriodSetupExt);
        // decoder->SetECALCalibration("");
        decoder->SetMwpcMapping(TString("MWPC_map_period") + ((period == 6 && decoder->GetRunId() < 1397) ? 5 : PeriodSetupExt));
        // decoder->SetMwpcMapping(TString("MWPC_map_period") + ((period == 6 && decoder->GetRunId() < 1397) ? 5 :
        // PeriodSetupExt));


        decoder->SetLANDMapping("land_mapping_jinr_triplex_2022.txt");
        decoder->SetLANDPedestal("r0030_land_clock_2022.hh");
        decoder->SetLANDTCal("r0030_land_tcal_2022.hh");
        decoder->SetLANDDiffSync("r352_cosmic1.hh");
        decoder->SetLANDVScint("neuland_sync_2.txt");    

        decoder->SetTofCalMapping("tofcal_mapping_jinr_triplex.txt.t0ref");
        decoder->SetTofCalPedestal("tofcal_ped_JK.hh");
        decoder->SetTofCalTCal("tofcal_tcal_JK.hh");
        decoder->SetTofCalDiffSync("tofcal_diffsync_cosmic1.hh");
        decoder->SetTofCalVScint("tofcal_sync_2022.txt");
        cout<<8<<endl;
        decoder->SetApplyThreshold(kTRUE);         // set kFALSE to check pedestals
        decoder->SetUseCalibFile(kFALSE);          // set kTRUE to save and load strip signal calibration data
        cout<<9<<endl;
        decode_status = rdd->DecodeDataToDigi();   // Decode data into detector-digits using current mappings.
        cout<<10<<endl;
        if (!doHoldRawRoot)
            gSystem->Exec(TString::Format("rm -f %s", decoder->GetRootFileName().Data()));
    }
    cout<<11<<endl;
    timer.Stop();

    if ((doConvert && (convert_status == kBMNSUCCESS)) || (doDecode && (decode_status == kBMNSUCCESS)))
        printf("\tMacro finished successfully!\n\n");   // marker of successfully execution for software testing systems
    if (doConvert) {
        Double_t rtime_conv = timer_conv.RealTime();
        Double_t ctime_conv = timer_conv.CpuTime();
        printf("Converter: Real time %f s, CPU time %f s\n", rtime_conv, ctime_conv);
    }
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("All steps: Real time %f s, CPU time %f s\n", rtime, ctime);
    delete rdd;
}

void func() {
    run_raw_bmn();
}
