#if !defined(__CLING__) || defined(__MAKECLING__)
// ROOT includes
#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TROOT.h"

#include "BmnRawDataDecoder.h"
#include "BmnEnums.h"
#include <iostream>
using namespace std;
#endif


void BmnDataToRoot(TString file = "", TString outfile = "", Long_t nEvents = 0, UInt_t period = 8, Bool_t doConvert = kTRUE, Bool_t doHoldRawRoot = kFALSE){
    gSystem->ExpandPathName(file);
    gSystem->ExpandPathName(outfile);

    Int_t iVerbose = 1; ///<- Verbosity level: 0 - Progress Bar; 1 - short info on passed events

    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, outfile, nEvents, period);
    // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnSetup stp = kBMNSETUP;//(decoder->GetRunId() >= 2041 && decoder->GetRunId() <= 3588) ? kSRCSETUP : kBMNSETUP;
    decoder->SetBmnSetup(stp);
    decoder->SetVerbose(iVerbose);

    std::map<DetectorId, bool> setup; // flags to determine BM@N setup
    setup.insert(std::make_pair(kBC,        1)); // TRIGGERS
    setup.insert(std::make_pair(kMWPC,      0)); // MWPC
    setup.insert(std::make_pair(kSILICON,   1)); // SILICON
    setup.insert(std::make_pair(kGEM,       1)); // GEM
    setup.insert(std::make_pair(kTOF1,      0)); // TOF-400
    setup.insert(std::make_pair(kTOF,       0)); // TOF-700
    setup.insert(std::make_pair(kDCH,       0)); // DCH
    setup.insert(std::make_pair(kZDC,       0)); // ZDC
    setup.insert(std::make_pair(kECAL,      0)); // ECAL
    setup.insert(std::make_pair(kCSC,       0)); // CSC
    setup.insert(std::make_pair(kSCWALL,    0)); // SCWALL
    setup.insert(std::make_pair(kFHCAL,     0)); // FHCAL
    setup.insert(std::make_pair(kHODO,      0)); // HODO
    setup.insert(std::make_pair(kSiBT,      1)); // SiBD
    decoder->SetDetectorSetup(setup);

if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    
    TString PeriodSetupExt = Form("%d%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"));
    decoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt);
    decoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt);
    decoder->SetSiliconMapping(TString("SILICON_map_run") + PeriodSetupExt);
    decoder->SetSiBTMapping(TString("SiBT_map_period") + PeriodSetupExt);
    decoder->SetGemMapping(TString("GEM_map_run") + PeriodSetupExt);
    decoder->SetCSCMapping(TString("CSC_map_period") + PeriodSetupExt);
    decoder->SetMSCMapping(TString("MSC_map_Run") + PeriodSetupExt);
    // in case comment out the line decoder->SetTof400Mapping("...")  
    // the maps of TOF400 will be read from DB (only for JINR network)
    decoder->SetTOF700ReferenceRun(-1);
    decoder->SetTof700Geom(TString("TOF700_geometry_run") + PeriodSetupExt);
    decoder->SetTof400Mapping(TString("TOF400_PlaceMap_RUN") + PeriodSetupExt, TString("TOF400_StripMap_RUN") + PeriodSetupExt);
    if (decoder->GetRunId() >= 4278 && decoder->GetPeriodId() == 7)
        decoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d_from_run_4278.txt", period));
    else
        decoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d.txt", period));
    //decoder->SetZDCMapping("ZDC_map_period_5.txt");
    //decoder->SetZDCCalibration("zdc_muon_calibration.txt");
    decoder->SetScWallMapping(TString("SCWALL_map_period") + PeriodSetupExt);
    decoder->SetScWallCalibration(TString("SCWALL_calibration_period") + PeriodSetupExt);
    decoder->SetFHCalMapping(TString("FHCAL_map_period") + PeriodSetupExt);
    decoder->SetFHCalCalibration(TString("FHCAL_calibration_period") + PeriodSetupExt);
    decoder->SetHodoMapping(TString("HODO_Q_map_period") + PeriodSetupExt);
    decoder->SetHodoCalibration(TString("HODO_Q_calibration_period") + PeriodSetupExt);
    decoder->SetNdetMapping(TString("NDET_map_period") + PeriodSetupExt);
    decoder->SetNdetCalibration(TString("NDET_calibration_period") + PeriodSetupExt);
    //decoder->SetECALMapping(TString("ECAL_map_period_") + PeriodSetupExt);
    //decoder->SetECALCalibration("");
    //decoder->SetMwpcMapping(TString("MWPC_map_period") + ((period == 6 && decoder->GetRunId() < 1397) ? 5 : PeriodSetupExt));

    decoder->SetApplyThreshold(kTRUE); // set kFALSE to check pedestals
    //if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    BmnStatus decoStatus = decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.
    if (decoStatus == kBMNSUCCESS) {
        if (!doHoldRawRoot) gSystem->Exec(TString::Format("rm -f %s", decoder->GetRootFileName().Data()));
        printf("\tMacro finished successfully!\n\n"); // marker of successfully execution for software testing systems
    }

    delete decoder;
}

int main(int argc, char** arg){
    BmnDataToRoot();
    return 1;
}