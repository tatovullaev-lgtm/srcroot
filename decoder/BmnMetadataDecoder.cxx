#include "BmnMetadataDecoder.h"
// STL
#include <bitset>
#include <chrono>
#include <filesystem>
#include <iostream>
// ROOT
#include "TStopwatch.h"
#include "TSystem.h"

#include <ROOT/RDF/RInterface.hxx>
#include <ROOT/RDataFrame.hxx>
// FairRoot
#include "FairLogger.h"
// BmnRoot
#include "BmnADCDigit.h"
#include "BmnConverterTools.h"
#include "BmnGemStripDigit.h"
#include "BmnHRBDigit.h"
#include "BmnMSCDigit.h"
#include "BmnMSCMatrix.h"
#include "BmnMSCZSDigit.h"
#include "BmnRawDataDecoder.h"
#include "BmnSyncDigit.h"
#include "BmnTDCDigit.h"
#include "BmnTQDCADCDigit.h"
#include "DigiRunHeader.h"
#include "TangoData.h"
#include "UniDetectorParameter.h"
#include "UniRawFile.h"
#include "UniRun.h"

namespace fs = std::filesystem;
using namespace std::chrono;

BmnMetadataDecoder::BmnMetadataDecoder(vector<string> files, string outfile, ULong_t period, BmnSetup setup)
    : fPeriodId(period)
    , fTime_s(0)
    , fTime_ns(0)
    , fRunStartTime(SysPoint::min())
    , fRunEndTime(SysPoint::min())
    , fRawTree(nullptr)
    , fDigiTree(nullptr)
    , fDigiFileNames(files)
    , fOutFileName(outfile)
    , fDigiRunHdrName("DigiRunHeader")
    , fRawRunHdrName("RawRunHeader")
    , fMetadataName("RawRunMetadata")
    , fDigiFileOut(nullptr)
    , fCalibFile(nullptr)
    , msc(nullptr)
    , t0raw(nullptr)
    , msc_copy(nullptr)
    , t0_copy(nullptr)
    , inEventHeader(nullptr)
    , spillHeader(nullptr)
    //, metadata(make_unique<BmnMetadataRaw>())
    , fBmnSetup(setup)
    , fPeriodSetupExt(Form("%d%s.txt", fPeriodId, ((fBmnSetup == kBMNSETUP) ? "" : "_SRC")))
//, fTrigMapper(make_unique(TString("Trig_map_Run") + fPeriodSetupExt))
//    , fMSCMapper(nullptr)
{
    TTree::SetMaxTreeSize(1'000'000'000'000);   // 932 GB
    if (fOutFileName.length() == 0)
        fOutFileName = string(Form("bmn_run%d_meta.root", fRunId));
}

BmnMetadataDecoder::~BmnMetadataDecoder()
{
    DisposeDecoder();
}

BmnStatus BmnMetadataDecoder::SetDigiDirectory(string DirName)
{
    fDigiDir = DirName;
    //    string re_str(Form(".*mpd_run_Top_%u(_ev\\d+_p\\d+)\\.root", fRunId));
    string re_str(Form(".*%u(_ev\\d+_p\\d+)[^\\/]*\\.root", fRunId));
    TPRegexp re_dig(re_str);
    // Check if the path exists and is a directory
    if (fs::exists(fDigiDir) && fs::is_directory(fDigiDir)) {
        std::cout << "Target files in a directory: " << fDigiDir << std::endl;
        // Iterate through the directory
        for (const auto& entry : fs::directory_iterator(fDigiDir)) {
            string fileName = entry.path();
            if (re_dig.MatchB(fileName)) {
                std::cout << entry.path().filename() << std::endl;
                fDigiFileNames.push_back(fileName.data());
            }
        }
        if (fDigiFileNames.size() == 0) {
            LOGF(error, "No valid files in te directory!");
            return kBMNERROR;
        }
        return kBMNSUCCESS;
    } else {
        LOGF(error, "Wrong path.");
        return kBMNERROR;
    }
}

BmnStatus BmnMetadataDecoder::SetTarDirectory(string DirName)
{
    fTarDir = DirName;
    //    string re_str(Form(".*mpd_run_Top_%u(_ev\\d+_p\\d+)\\.root", fRunId));
    string re_str(Form(".*mpd_run_Top_%u(_ev\\d+_p\\d+)\\.root.tar", fRunId));
    TPRegexp re_tar(re_str);
    // Check if the path exists and is a directory
    if (fs::exists(fTarDir) && fs::is_directory(fTarDir)) {
        std::cout << "Target files in the directory: " << fTarDir << std::endl;
        // Iterate through the directory
        for (const auto& entry : fs::directory_iterator(fTarDir)) {
            string fileName = entry.path();
            if (re_tar.MatchB(fileName)) {
                std::cout << entry.path().filename() << std::endl;
                fTarFileNames.push_back(fileName.data());
            }
        }
        return kBMNSUCCESS;
    } else {
        LOGF(error, "Wrong path.");
        return kBMNERROR;
    }
}

void BmnMetadataDecoder::GatherStatusBlocksFromDigis()
{
    //    // try get run interval from DB
    //    if (!get_run_interval_from_file) {
    //        UniRun* pRun = UniRun::GetRun(fPeriodId, fRunId);
    //        if (!pRun) {
    //            get_run_interval_from_file = true;
    //        } else {
    //            TDatime fRunStartDate = pRun->GetStartDatetime();
    //            TDatime* fRunEndDate = pRun->GetEndDatetime();
    //            fRunStartTime = BmnFunctionSet::TDatime2GMT(fRunStartDate) - 3h;   // crutch for zone shift
    //            if (fRunEndDate)
    //                fRunEndTime = BmnFunctionSet::TDatime2GMT(*fRunEndDate) - 3h;
    //            else
    //                get_run_interval_from_file = true;
    //            //            LOGF(info, "Set Run from DB:\n\tstart ts: %s\n\tstop  ts: %s",
    //            //                 fRunStartDate.AsString(), fRunEndDate->AsString());
    //            LOGF(info, "Set Run from DB:\n\tstart ts: %s\n\tstop  ts: %s",
    //                 BmnFunctionSet::TimePoint2String(fRunStartTime), BmnFunctionSet::TimePoint2String(fRunEndTime));
    //            delete pRun;
    //        }
    //    }
    // get status blocks, maps and run interval from digi files
    bool maps_recreate = true;
    bool maps_set = false;
    bool tr_conf_set = false;
    BmnTrigConfig tc;
    for (string& name : fDigiFileNames) {
        unique_ptr<TFile> dig_in{TFile::Open(name.c_str(), "READ")};
        if (dig_in->IsOpen() == false) {
            LOGF(error, "\n!!!!\ncannot open file %s !\n", name.c_str());
            //            return kBMNERROR;
        } else {
            LOGF(info, "Starting to decode file %s", name.c_str());
            digiRunHeader.reset(static_cast<DigiRunHeader*>(dig_in->Get(fDigiRunHdrName)));
            if (!maps_set) {
                fRawT0Map = digiRunHeader->T0Map();
                fTrcBitMap = digiRunHeader->GetTrcBitMap();
                fScalersMap = digiRunHeader->GetScalerMap();
                if (get_run_interval_from_file) {
                    fRunStartTime = digiRunHeader->GetRunStartTP();
                    fRunEndTime = digiRunHeader->GetRunEndTP();
                    LOGF(debug, "Set Run:\n\tstart ts: %s\n\tstop  ts: %s",
                         BmnFunctionSet::TimePoint2String(fRunStartTime),
                         BmnFunctionSet::TimePoint2String(fRunEndTime));
                }
                maps_set = true;
            } else {
                if (get_run_interval_from_file) {
                    if (fRunStartTime > digiRunHeader->GetRunStartTP())
                        fRunStartTime = digiRunHeader->GetRunStartTP();
                    if (fRunEndTime < digiRunHeader->GetRunEndTP())
                        fRunEndTime = digiRunHeader->GetRunEndTP();
                    LOGF(debug, "Upd Run:\n\tstart ts: %s\n\tstop  ts: %s",
                         BmnFunctionSet::TimePoint2String(fRunStartTime),
                         BmnFunctionSet::TimePoint2String(fRunEndTime));
                }
            }
            if (!tr_conf_set) {
                if (digiRunHeader->GetTrigConfig().TrigChannelConfig().size() > 0) {
                    tc = digiRunHeader->GetTrigConfig();
                    tr_conf_set = true;
                }
            }
            unique_ptr<BmnMetadataRaw> meta_f{static_cast<BmnMetadataRaw*>(dig_in->Get(fMetadataName))};
            if (meta_f.get() == nullptr) {
                LOGF(warning, "No metadata found in the %s!", name.c_str());
                continue;
            }
            auto& src = meta_f->SpillStatusVec();
            auto& dst = metadata.SpillStatusVec();
            LOGF(debug, "vec size %u", src.size());
            dst.reserve(dst.size() + src.size());
            std::move(std::begin(src), std::end(src), std::back_inserter(dst));
        }
    }
    LOGF(info, "Set Run:\n\tstart ts: %s\n\tstop  ts: %s", BmnFunctionSet::TimePoint2String(fRunStartTime),
         BmnFunctionSet::TimePoint2String(fRunEndTime));
    if (maps_recreate) {
        TString PeriodSetupExt = Form("%d%s.txt", fPeriodId, ((fBmnSetup == kBMNSETUP) ? "" : "_SRC"));
        fTrigMapper = make_unique<BmnTrigRaw2Digit>(TString("Trig_PlaceMap_Run") + PeriodSetupExt,
                                                    TString("Trig_map_Run") + PeriodSetupExt, nullptr);
        digiRunHeader->T0Map() = fTrigMapper->T0Map();
        digiRunHeader->GetTrcBitMap() = fTrigMapper->GetTrcMap();
        digiRunHeader->GetScalerMap() = fTrigMapper->GetScalerMap();
    } else {
        digiRunHeader->T0Map() = fRawT0Map;
        digiRunHeader->GetTrcBitMap() = fTrcBitMap;
        digiRunHeader->GetScalerMap() = fScalersMap;
    }
    digiRunHeader->SetRunStartTP(fRunStartTime);
    digiRunHeader->SetRunEndTP(fRunEndTime);
    digiRunHeader->SetTrigConfig(tc);
}

void BmnMetadataDecoder::GatherStatusBlocksFromTars() {}

BmnStatus BmnMetadataDecoder::ProcessSpillTree()
{
    TStopwatch timer;
    Double_t rtime;
    Double_t ctime;
    for (auto& mapel : fSpillMap) {
        BmnSpillInfo& si = mapel.second;
        if (si.stop_ts - si.start_ts < 1ms) {
            si.stop_ts = si.start_ts + ((fPeriodId > 8) ? 8s : 5s);
            LOGF(warning,
                 "LOST SPILL END! Making fake one!\n"
                 "Spill:\n\tstart ts: %s\n\tstop  ts: %s",
                 BmnFunctionSet::TimePoint2String(si.start_ts), BmnFunctionSet::TimePoint2String(si.stop_ts));
        }
    }
    metadata.SpillMap() = fSpillMap;
    metadata.RawT0Map() = fRawT0Map;
    metadata.TrcMap() = fTrcBitMap;
    metadata.ScalersMap() = fScalersMap;
    /** Create scaler maps for name <--> (serial,channel) correspondence and navigation in the result matrix */
    // map: <serial, channel> -> name
    SerCh2Name scalers2name_map;
    // map: <serial, channel> -> index in the list (inside spillHeader->ScalersVec(), and MSC matrix)
    map<std::pair<uint32_t, uint16_t>, uint16_t> scalers2index_map;
    map<uint16_t, uint16_t> trcIdx2scalerIdx;
    // map: name -> index in the list
    NameChMap scalers_name2index_map;
    ChNameMap scalers_index2name_map;
    uint16_t scalers_ar_size(0);   // MSC list size
    for (auto& map_el : fScalersMap) {
        const string& name = map_el.first;
        scalers2name_map.insert(make_pair(map_el.second, name));
        scalers_name2index_map.insert(make_pair(name, scalers_ar_size));
        scalers_index2name_map.insert(make_pair(scalers_ar_size, name));
        scalers2index_map.insert(make_pair(map_el.second, scalers_ar_size));
        auto it = fTrcBitMap.find(name);
        if (it != fTrcBitMap.end()) {
            trcIdx2scalerIdx.insert(make_pair(it->second, scalers_ar_size));
        }
        scalers_ar_size++;
    }
    digiRunHeader->ScalersVec().resize(scalers_ar_size, 0);
    LOGF(info, "scalers_ar_size   %d", scalers_ar_size);
    /** Start reading */
    //    ROOT::EnableImplicitMT();
    RDataFrame df{"bmndata", fDigiFileNames};
    RDataFrame df_spills{"spill", fDigiFileNames};
    // calc the average time shift of raw T0 digits
    timer.Start();
    auto df_spills_withT0Raw_in_run = df_spills.Filter(
        [&](TClonesArray& array) {
            if (array.GetEntries()) {
                BmnT0Raw<kT0_BIN_BLOCK_WORDS>* t0raw_dig = static_cast<BmnT0Raw<kT0_BIN_BLOCK_WORDS>*>(array.At(0));
                LOGF(info, "T0 raw time %s tsle %10.2f ", BmnFunctionSet::TimePoint2String(t0raw_dig->GetTime()),
                     t0raw_dig->GetTimeSinceLastEvNs());
                return ((t0raw_dig->GetTimeSinceLastEv() < duration_cast<nanoseconds>(12h))
                        && (t0raw_dig->GetTimeSinceLastEv() > duration_cast<nanoseconds>(-12h)));
            } else
                return false;
        },
        {"T0Raw"});
    auto sum_ptr_dbl =
        df_spills_withT0Raw_in_run
            .Define("sls",
                    [](TClonesArray& array) {
                        return (array.GetEntries())
                                   ? static_cast<BmnT0Raw<kT0_BIN_BLOCK_WORDS>*>(array.At(0))->GetTimeSinceLastEvNs()
                                   : (Double_t)0.0;
                    },
                    {"T0Raw"})
            .Sum("sls");
    Double_t sum = sum_ptr_dbl ? *sum_ptr_dbl : 0.0;
    auto df_spills_in_run_cnt_ptr = df_spills_withT0Raw_in_run.Count();
    LOGF(info, "Raw T0 count : %lld", (*df_spills_in_run_cnt_ptr));
    //        printf("sum : %f\n", sum);
    auto t0raw_shift_av = df_spills_in_run_cnt_ptr ? sum / (*df_spills_in_run_cnt_ptr) : 0.0;
    LOGF(info, "Average raw T0 TimeSinceLastEv %10.3f ns", t0raw_shift_av);
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    LOGF(info, "Real time %f s, CPU time %f s  sum T0Raw timeshifts", rtime, ctime);
    // TRaversing spill chain by hands and rearranging digits by spill time intervals
    TChain* fInTreeSpills = new TChain("spill");
    TChain* fInTreeEvents = new TChain("bmndata");
    for (string& name : fDigiFileNames) {
        LOGF(debug, "Filling tchain with a file %s", name.c_str());
        fInTreeSpills->Add(name.c_str());
        fInTreeEvents->Add(name.c_str());
    }

    fInTreeSpills->GetListOfBranches();   // ROOT magic for SetBranchAddress to work
    fInTreeSpills->SetBranchAddress("MSC", &msc);
    fInTreeSpills->SetBranchAddress("T0Raw", &t0raw);
    fInTreeEvents->SetBranchAddress("BmnEventHeader.", &inEventHeader);
    fDigiFileOut = new TFile(fOutFileName.data(), "recreate", "");   // , 403 LZ4 compression level 3
    fDigiTreeSpills = new TTree("spill", "spill");
    spillHeader = new BmnSpillHeader();
    fDigiTreeSpills->Branch("BmnSpillHeader.", &spillHeader);
    //    fMSCMapper = new BmnMscRaw2Digit(fPeriodId, fRunId, fMSCMapFileName, fInTreeSpills, fDigiTreeSpills);
    //    msc_copy = new TClonesArray(msc->GetClass());
    msc_copy = new TClonesArray(BmnMSCMatrix<UChar_t>::Class());
    fDigiTreeSpills->Branch("MSC", &msc_copy);
    t0_copy = new TClonesArray(t0raw->GetClass());
    fDigiTreeSpills->Branch("T0Raw", &t0_copy);

    size_t msc_entries_cnt_total(0);   // all MSC times (to check presence of MSC outside spills)
    size_t msc_entries_cnt(0);         // MSC entries inside spills
    // make map (time_point -> (i-tree entry,i-digit)) to sort and rearrange MSC by time
    typedef pair<Long64_t, Int_t> Index_Pair;   // (i-tree entry,i-digit)
    map<SysPoint, vector<Index_Pair>> mts;      // for period 8
    //    map<SysPoint, size_t> time2times_map_entry;
    vector<pair<SysPoint, size_t>> vts_t0;   // for period 8

    //    map<SysPoint, vector<uint8_t>> mts9; // for period 9

    LOGF(info, "Start filling timestamp map");
    timer.Start();
    if (fPeriodId > 8) {
        for (Long64_t iEntry = 0; iEntry < fInTreeSpills->GetEntries(); ++iEntry) {
            fInTreeSpills->GetEntry(iEntry);
            /** General time map (high memory usage) */
            ////        df_spills.Foreach(
            ////        [&](TClonesArray& msc_in) {
            ////            for (Int_t iDig = 0; iDig < msc_in.GetEntries(); ++iDig) {
            ////                BmnMSCZSDigit<uint8_t>* dig = static_cast<BmnMSCZSDigit<uint8_t>*>(msc_in.At(iDig));
            //            for (Int_t iDig = 0; iDig < msc->GetEntries(); ++iDig) {
            //                BmnMSCZSDigit<uint8_t>* dig = static_cast<BmnMSCZSDigit<uint8_t>*>(msc->At(iDig));
            //                auto index_it = scalers2index_map.find(make_pair(dig->GetSerial(), dig->GetChannel()));
            //                if (index_it == scalers2index_map.end())
            //                    continue;
            //                uint8_t index = index_it->second;
            //                vector<SysPoint>& times = dig->GetTimes();
            //                vector<uint8_t>& values = dig->GetValues();
            //                vector<uint8_t>& excs = dig->GetExtCond();
            //                for (size_t iTime= 0; iTime< dig->GetNVals(); iTime++){
            //                    SysPoint& time = times[iTime];
            //                    auto it = mts9.find(time);
            //                    if (it == mts9.end()){
            //                        vector<uint8_t> tarr(scalers_ar_size+1, 0); // scalers + ext conditions
            //                        tarr[index] = values[iTime];
            //                        tarr[scalers_ar_size] = excs[iTime];
            //                        mts9.insert(make_pair(time, move(tarr)));
            //                    } else{
            //                        vector<uint8_t>& tarr = it->second;
            //                        tarr[index] += values[iTime];
            //                        tarr[scalers_ar_size] = excs[iTime];
            //                    }
            //                }
            //            }
            ////        },
            ////        {"MSC"});

            Int_t ne_t0 = t0raw->GetEntries();
            if (ne_t0 > 0) {
                BmnT0Raw<kT0_BIN_BLOCK_WORDS>* dig = static_cast<BmnT0Raw<kT0_BIN_BLOCK_WORDS>*>(t0raw->At(0));
                vts_t0.push_back(make_pair(dig->GetTime() - ((Long64_t)t0raw_shift_av) * 1ns - 1s,
                                           iEntry));   // crutch to fit rough time inside the spill
                LOG(info) << "T0 time " << BmnFunctionSet::TimePoint2String(dig->GetTime()) << " iEntry " << iEntry;
            }
        }
    } else {
        for (Long64_t iEntry = 0; iEntry < fInTreeSpills->GetEntries(); ++iEntry) {
            fInTreeSpills->GetEntry(iEntry);
            for (Int_t iDig = 0; iDig < msc->GetEntries(); ++iDig) {
                BmnMSCDigit<UChar_t>* dig = static_cast<BmnMSCDigit<UChar_t>*>(msc->At(iDig));
                mts[dig->GetTime()].push_back(make_pair(iEntry, iDig));
                //            msc_entries_cnt_total++;
            }
            Int_t ne_t0 = t0raw->GetEntries();
            if (ne_t0 > 0) {
                BmnT0Raw<kT0_BIN_BLOCK_WORDS>* dig = static_cast<BmnT0Raw<kT0_BIN_BLOCK_WORDS>*>(t0raw->At(0));
                vts_t0.push_back(make_pair(dig->GetTime() - ((Long64_t)t0raw_shift_av) * 1ns - 1s,
                                           iEntry));   // crutch to fit rough time inside the spill
                LOG(info) << "T0 time " << BmnFunctionSet::TimePoint2String(dig->GetTime()) << " iEntry " << iEntry;
            }
        }
        //    size_t i_rec{0};
        //    for (auto &rec : mts)
        //        time2times_map_entry[rec.first] = i_rec++;
        msc_entries_cnt_total = mts.size();
    }
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    LOGF(info, "Real time %f s, CPU time %f s  timestamp map filling", rtime, ctime);

    LOGF(info, "Run:\n\tstart ts: %s\n\tstop  ts: %s", BmnFunctionSet::TimePoint2String(fRunStartTime),
         BmnFunctionSet::TimePoint2String(fRunEndTime));
    UInt_t spillId = 0;
    ULong64_t n_events_run(0);
    for (auto& mapel : fSpillMap) {
        ClearSpillDigiArrays();
        BmnSpillInfo& si = mapel.second;
        LOGF(info, "Spill:\n\tstart ts: %s\n\tstop  ts: %s", BmnFunctionSet::TimePoint2String(si.start_ts),
             BmnFunctionSet::TimePoint2String(si.stop_ts));
        if ((fRunStartTime > si.stop_ts) || (fRunEndTime < si.start_ts)) {
            LOGF(warning, "Spill is outside the run! Skipping!");
            continue;
        }
        spillHeader->SetStartTime(si.start_ts);
        spillHeader->SetStopTime(si.stop_ts);
        spillHeader->SetSpillId(spillId++);
        // count activated triggers in events
        timer.Start();
        auto df_events_in_spill = df.Filter(
            [&](BmnEventHeader& eh) {
                return ((eh.GetEventTimeTP() >= si.start_ts) && (eh.GetEventTimeTP() <= si.stop_ts));
            },
            {"BmnEventHeader."});
        //        ULong64_t ev_sp_cnt = *(df_spill.Count());
        //        LOGF(info, "NEvents %llu", ev_sp_cnt);
        //        spillHeader->SetNEvents(ev_sp_cnt);

        //        for (auto& el : metadata.TrcMap()) {
        //            auto df_sp_trig = df_spill.Filter(
        //                [el](BmnEventHeader& eh) { return (eh.GetInputSignalsAR() & BIT(el.second)); },
        //                {"BmnEventHeader."});
        //            ULong64_t df_sp_trig_cnt = *(df_sp_trig.Count());
        //            const string& tr_name = el.first;
        //            LOGF(info, "Evt_cnt %-12s %7llu  bit %u", tr_name.c_str(), df_sp_trig_cnt, el.second);
        //            spillHeader->EventTriggerCnt()[tr_name] = df_sp_trig_cnt;
        //            metadata.EventTriggerCnt()[tr_name] += spillHeader->EventTriggerCnt()[tr_name];
        ////                       // crosscheck
        ////                       ULong64_t cnt = 0;
        ////                       for (Int_t iEntry = 0; iEntry < fInTreeEvents->GetEntries(); iEntry++){
        ////                           fInTreeEvents->GetEntry(iEntry);
        ////                           if ((inEventHeader->GetEventTimeTP() >= si.start_ts) &&
        //// (inEventHeader->GetEventTimeTP() <= /                           si.stop_ts)) / if
        //// (inEventHeader->GetInputSignalsAR() & BIT(el.second)){ /                                   cnt++; / //
        //// LOGF(info, "\tiput AR / /%s",std::bitset<8>{inEventHeader->GetInputSignalsAR()}.to_string().c_str()); / } /
        //// } /                       LOGF(info, "%-12s %7llu crosscheck", tr_name.c_str(), cnt);
        //        }
        // crosscheck
        ULong64_t cnt[32] = {};
        UInt_t first_ev_id(kMaxUInt);
        UInt_t last_ev_id(0);
        auto df_events_in_spill_trig_cntr = df_events_in_spill.Filter(
            [&](BmnEventHeader& eh) {
                for (auto& el : digiRunHeader->GetTrcBitMap()) {
                    if (eh.GetInputSignalsAR() & BIT(el.second))
                        cnt[el.second]++;
                }
                if (first_ev_id > eh.GetEventId())
                    first_ev_id = eh.GetEventId();
                if (last_ev_id < eh.GetEventId())
                    last_ev_id = eh.GetEventId();
                return true;
            },
            {"BmnEventHeader."});
        ULong64_t n_events = *(df_events_in_spill_trig_cntr.Count());
        n_events_run += n_events;
        LOGF(info, "Events in the spill %7llu", n_events);
        spillHeader->SetNEvents(n_events);
        spillHeader->SetFirstEventId(first_ev_id);
        spillHeader->SetLastEventId(last_ev_id);
        for (auto& el : digiRunHeader->GetTrcBitMap()) {
            const string& tr_name = el.first;
            spillHeader->EventTriggerCnt()[tr_name] = cnt[el.second];
            digiRunHeader->EventTriggerCnt()[tr_name] += spillHeader->EventTriggerCnt()[tr_name];
            LOGF(info, "Evt_cnt %-11s %7llu", tr_name.c_str(), cnt[el.second]);
        }
        timer.Stop();
        rtime = timer.RealTime();
        ctime = timer.CpuTime();
        LOGF(info, "Real time %f s, CPU time %f s  ev cnt calc", rtime, ctime);

        // calc T0 count inside spill
        std::sort(vts_t0.begin(), vts_t0.end(),
                  [](pair<SysPoint, size_t>& a, pair<SysPoint, size_t>& b) { return (a.first < b.first); });
        vector<pair<SysPoint, size_t>> spillIds_t0;
        timer.Start();
        copy_if(begin(vts_t0), end(vts_t0), std::back_inserter(spillIds_t0),
                [&si](pair<SysPoint, size_t> p) { return (p.first >= si.start_ts) && (p.first <= si.stop_ts); });
        timer.Stop();
        rtime = timer.RealTime();
        ctime = timer.CpuTime();
        LOGF(info, "IDs entries T0:%d", spillIds_t0.size());
        LOGF(debug, "Real time %f s, CPU time %f s  ids copy T0", rtime, ctime);

        LOGF(info, "Start scalers calc");
        timer.Start();
        if (fPeriodId > 8) {
            // calc MSC count inside spill
            auto df_spills_in_spill = df_spills.Filter(
                [&](TClonesArray& tca) {
                    for (Int_t iDig = 0; iDig < tca.GetEntries(); ++iDig) {
                        BmnMSCZSDigit<uint8_t>* dig = static_cast<BmnMSCZSDigit<uint8_t>*>(tca.At(iDig));
                        return ((dig->GetTime() >= si.start_ts) && (dig->GetTime() <= si.stop_ts));
                    }
                    return false;
                },
                {"MSC"});
            /** Local time map (longer calculation) */
            map<SysPoint, vector<uint8_t>> mts9;   // for period 9
            df_spills_in_spill.Foreach(
                [&](TClonesArray& tca) {
                    for (Int_t iDig = 0; iDig < tca.GetEntries(); ++iDig) {
                        BmnMSCZSDigit<uint8_t>* dig = static_cast<BmnMSCZSDigit<uint8_t>*>(tca.At(iDig));
                        auto index_it = scalers2index_map.find(make_pair(dig->GetSerial(), dig->GetChannel()));
                        if (index_it == scalers2index_map.end())
                            continue;
                        uint8_t index = index_it->second;
                        vector<SysPoint>& times = dig->GetTimes();
                        vector<uint8_t>& values = dig->GetValues();
                        vector<uint8_t>& excs = dig->GetExtCond();
                        for (size_t iTime = 0; iTime < dig->GetNVals(); iTime++) {
                            SysPoint& time = times[iTime];
                            auto it = mts9.find(time);
                            if (it == mts9.end()) {
                                vector<uint8_t> tarr(scalers_ar_size + 1, 0);   // scalers + ext conditions
                                tarr[index] = values[iTime];
                                tarr[scalers_ar_size] = excs[iTime];
                                mts9.insert(make_pair(time, move(tarr)));
                            } else {
                                vector<uint8_t>& tarr = it->second;
                                tarr[index] += values[iTime];
                                tarr[scalers_ar_size] = excs[iTime];
                            }
                        }
                    }
                },
                {"MSC"});

            auto itLower = mts9.lower_bound(si.start_ts);
            auto itUpper = mts9.upper_bound(si.stop_ts);
            size_t spillTimesCount = std::distance(itLower, itUpper);
            //            size_t spillTimesCount = mts9.size();
            msc_entries_cnt += spillTimesCount;
            LOGF(info, "Spill times count     %lld", spillTimesCount);
            // create single MSC matrix object for spill
            BmnMSCMatrix<uint8_t>* msc_matrix = new ((*msc_copy)[msc_copy->GetEntries()])
                BmnMSCMatrix<uint8_t>(spillTimesCount, scalers_name2index_map);
            spillHeader->ScalersVec().resize(scalers_ar_size, 0);
            // Fill MSC matrix elements
            size_t i_record{0};
            //            for (auto & [time, vals] : mts9) {
            for (auto itTimes = itLower; itTimes != itUpper; itTimes++) {
                auto& time = itTimes->first;
                vector<uint8_t>& vals = itTimes->second;

                msc_matrix->Time(i_record) = time;
                uint8_t ext_cond = vals[scalers_ar_size];
                msc_matrix->IsSpillBitVec()[i_record] = ext_cond & MSC_EC_SPILL;
                msc_matrix->IsBusyBitVec()[i_record] = ext_cond & MSC_EC_BUSY;
                for (uint16_t iScaler = 0; iScaler < scalers_ar_size; iScaler++) {
                    if (vals[iScaler] == 0)
                        continue;
                    msc_matrix->SetCount(i_record, iScaler, vals[iScaler]);
                    //                    msc_matrix->Counts()(i_record, iScaler) = vals[iScaler];
                    //                spillHeader->Scalers()[name] += vals[iScaler];

                    spillHeader->ScalersVec()[iScaler] += vals[iScaler];
                    LOGF(debug, "el %8llu %2lu count %3u sp %u busy %u  GC", i_record, iScaler,
                         (UChar_t)(msc_matrix->GetCount(i_record, iScaler)),
                         (UInt_t)msc_matrix->IsSpillBitVec()[i_record], (UInt_t)msc_matrix->IsBusyBitVec()[i_record]);
                }
                i_record++;
            }
            if (i_record > spillTimesCount)
                LOGF(error, "\n\t ERROR!!! irec  %lu  spillTimesCount  %lu\n", i_record, spillTimesCount);
        } else {
            // calc MSC count inside spill
            auto itLower = mts.lower_bound(si.start_ts);
            auto itUpper = mts.upper_bound(si.stop_ts);
            Long64_t spillEntriesCumulativeSize{0};
            size_t spillTimesCount = std::distance(itLower, itUpper);
            //        size_t spillTimesShift = std::distance(mts.begin(), itLower);
            for (auto itTimes = itLower; itTimes != itUpper; itTimes++) {
                vector<Index_Pair>& index_vec = itTimes->second;
                spillEntriesCumulativeSize += index_vec.size();
            }
            //        msc_entries_cnt += spillEntriesCumulativeSize;
            msc_entries_cnt += spillTimesCount;
            LOGF(info, "entriesCumulativeSize %lld", spillEntriesCumulativeSize);
            LOGF(info, "Spill times count     %lld", spillTimesCount);
            //        LOGF(info, "Spill times shift     %lld", spillTimesShift);

            // create single MSC matrix object for spill
            BmnMSCMatrix<uint8_t>* msc_matrix = new ((*msc_copy)[msc_copy->GetEntries()])
                BmnMSCMatrix<uint8_t>(spillTimesCount, scalers_name2index_map);
            //            BmnMSCMatrix<UChar_t>(spillTimesCount, scalers_ar_size);
            spillHeader->ScalersVec().resize(scalers_ar_size, 0);

            // Fill MSC matrix elements
            size_t i_record{0};
            for (auto itTimes = itLower; itTimes != itUpper; itTimes++) {
                auto& time = itTimes->first;
                msc_matrix->Time(i_record) = time;
                vector<Index_Pair>& index_vec = itTimes->second;
                //            msc_entries_cnt += index_vec.size();
                LOGF(debug2, "index_vec.size() %8llu", index_vec.size());
                if (index_vec.size() > 1)
                    LOGF(debug, "new time");
                for (Index_Pair& index_pair : index_vec) {
                    Long64_t& iEntry = index_pair.first;
                    Int_t& iDig = index_pair.second;
                    fInTreeSpills->GetEntry(iEntry);
                    if (iDig >= msc->GetEntries()) {
                        LOGF(error, "\n\t ERROR!!! iDig  %d  msc->GetEntries()  %d\n", iDig, msc->GetEntries());
                        break;
                    }
                    BmnMSCDigit<UChar_t>* dig = static_cast<BmnMSCDigit<UChar_t>*>(msc->At(iDig));
                    LOGF(debug2, "iEntry %7d iDig %3d el %8llu  ", iEntry, iDig, i_record);
                    if (index_vec.size() > 1) {
                        LOGF(debug, "dig->IsSpill() %d", dig->IsSpill());
                        LOGF(debug, "dig->IsBusy()  %d", dig->IsBusy());
                    }
                    msc_matrix->IsSpillBitVec()[i_record] = dig->IsSpill();
                    msc_matrix->IsBusyBitVec()[i_record] = dig->IsBusy();
                    for (UInt_t iNum = 0; iNum < MSC_N_COUNTERS; iNum++) {
                        UInt_t val = dig->GetValue()[iNum];
                        //                    if (val == 0)
                        //                        continue;
                        //                auto it = scalers2name_map.find(make_pair(dig->GetSerial(), iNum));
                        //                if (it == scalers2name_map.end())
                        //                    continue;
                        //                string& name = it->second;
                        auto it = scalers2index_map.find(make_pair(dig->GetSerial(), iNum));
                        if (it == scalers2index_map.end())
                            continue;
                        uint16_t iScaler = it->second;
                        //                    msc_matrix->Counts(i_record, iScaler) += val;
                        //                    msc_matrix->Counts()(i_record, iScaler) += val;
                        msc_matrix->SetCount(i_record, iScaler, val);
                        //                    msc_matrix->SetCount(i_record, iScaler) += val;
                        if (index_vec.size() > 1)
                            LOGF(debug, "el %8llu %2lu ser 0x%8X = %u  sp %u busy %u", i_record, iScaler,
                                 dig->GetSerial(), (UChar_t)(msc_matrix->GetCount(i_record, iScaler)),
                                 //                             (UChar_t)(msc_matrix->Counts()(i_record, iScaler)),
                                 (UInt_t)msc_matrix->IsSpillBitVec()[i_record],
                                 (UInt_t)msc_matrix->IsBusyBitVec()[i_record]);
                        //                spillHeader->Scalers()[name] += val;
                        spillHeader->ScalersVec()[iScaler] += val;
                    }
                }
                i_record++;
            }
            if (i_record > spillTimesCount)
                LOGF(error, "\n\t ERROR!!! irec  %lu  spillTimesCount  %lu\n", i_record, spillTimesCount);
        }

        //        for (Long64_t iEntry = 0; iEntry < fInTreeSpills->GetEntries(); ++iEntry) {
        //            fInTreeSpills->GetEntry(iEntry);
        //            for (Int_t iDig = 0; iDig < msc->GetEntries(); ++iDig) {
        //                BmnMSCDigit<UChar_t>* dig = static_cast<BmnMSCDigit<UChar_t>*>(msc->At(iDig));
        //                size_t i_record = time2times_map_entry[dig->GetTime()] - spillTimesShift;
        //                LOGF(info, "iEntry %7d iDig %3d el %8llu  ", iEntry, iDig, i_record);
        //                msc_matrix->Time(i_record) = dig->GetTime();
        //                msc_matrix->IsSpillBitVec()[i_record] = dig->IsSpill();
        //                msc_matrix->IsBusyBitVec()[i_record] = dig->IsBusy();
        //                for (UInt_t iNum = 0; iNum < MSC_N_COUNTERS; iNum++) {
        //                    UInt_t val = dig->GetValue()[iNum];
        //                    if (val == 0)
        //                        continue;
        //                    //                auto it = scalers2name_map.find(make_pair(dig->GetSerial(), iNum));
        //                    //                if (it == scalers2name_map.end())
        //                    //                    continue;
        //                    //                string& name = it->second;
        //                    auto it = scalers2index_map.find(make_pair(dig->GetSerial(), iNum));
        //                    if (it == scalers2index_map.end())
        //                        continue;
        //                    uint16_t index = it->second;
        //                    msc_matrix->Counts(i_record, index) += val;
        //                    LOGF(debug, "el %8llu %2lu = %u  sp %u busy %u",
        //                            i_record,
        //                            index,
        //                            (UChar_t)(msc_matrix->Counts(i_record, index)),
        //                            (UInt_t)msc_matrix->IsSpillBitVec()[i_record],
        //                            (UInt_t)msc_matrix->IsBusyBitVec()[i_record]);
        //                    //                spillHeader->Scalers()[name] += val;
        //                    spillHeader->ScalersVec()[index] += val;
        //                }
        //            }
        //        }
        for (auto& p : spillIds_t0) {
            Long64_t iEntry = p.second;
            fInTreeSpills->GetEntry(iEntry);
            t0_copy->AbsorbObjects(t0raw);
        }
        for (auto& sni : scalers_name2index_map) {
            const string& name = sni.first;
            spillHeader->Scalers()[name] = spillHeader->ScalersVec()[sni.second];
        }
        for (int i = 0; i < scalers_ar_size; i++)
            digiRunHeader->ScalersVec()[i] += spillHeader->ScalersVec()[i];
        //        for (auto& scaler_map_el : spillHeader->Scalers()) {
        //            const string& sc_name = scaler_map_el.first;
        //            metadata.Scalers()[sc_name] += spillHeader->Scalers()[sc_name];
        //        }
        timer.Stop();
        rtime = timer.RealTime();
        ctime = timer.CpuTime();
        LOGF(info, "Real time %f s, CPU time %f s  Scalers calc", rtime, ctime);

        if (t0_copy->GetEntries()) {
            BmnT0Raw<kT0_BIN_BLOCK_WORDS>* t0r = static_cast<BmnT0Raw<kT0_BIN_BLOCK_WORDS>*>(t0_copy->At(0));
            for (UInt_t i = 0; i < kT0_BIN_BLOCK_WORDS; i++) {
                spillHeader->RawT0Counters()[i] = t0r->Counters()[i];
                digiRunHeader->RawT0Counters()[i] += spillHeader->RawT0Counters()[i];
            }
            //            LOGF(info, "comment: %s", t0r->GetComment().data());
            //                for (auto& p : t0r->DelayLines())
            //                    LOGF(info, "Delay %2d : %3d", p.first, p.second);
            //                for (auto& p : t0r->Discriminators()) {
            //                    LOGF(info, "Discriminator %2d : ", p.first);
            //                    p.second.print();
            //                }
            //                for (auto& p : t0r->LVChannelConfig()) {
            //                    LOGF(info, "LV %2d : ", p.first);
            //                    p.second.print();
            //                }
            LOGF(info, "T0 time %s",
                 BmnFunctionSet::TimePoint2String(
                     (static_cast<BmnT0Raw<kT0_BIN_BLOCK_WORDS>*>(t0_copy->At(0)))->GetTime()));
            for (auto& t0_map_el : digiRunHeader->T0Map()) {
                const T0MapKey& key = t0_map_el.first;
                if (key.first.find("BD") == 0)
                    continue;
                LOGF(info, "T0 %-14s.%02u = %9llu", key.first.c_str(), key.second,
                     spillHeader->RawT0Counters()[t0_map_el.second]);
            }
            if (t0_copy->GetEntries() > 1)
                LOGF(error, "More than 1 T0 in the spill %lu ! %d records found", spillHeader->GetSpillId(),
                     t0_copy->GetEntries());
        } else
            LOGF(error, "No T0 in the spill %lu !", spillHeader->GetSpillId());
        for (auto& sca : spillHeader->Scalers()) {
            LOGF(info, "MSC %-14s  %10llu", sca.first.c_str(), sca.second);
        }
        fDigiTreeSpills->Fill();
    }
    digiRunHeader->SetNEvents(n_events_run);
    //    if (msc_entries_cnt_total > msc_entries_cnt)
    LOGF(warning,
         "MSC branch entries %9llu total\n"
         "MSC branch entries %9llu in spills\n"
         "MSC branch entries %9llu outside spills!",
         msc_entries_cnt_total, msc_entries_cnt, msc_entries_cnt_total - msc_entries_cnt);
    digiRunHeader->ScalersName2IndexMap() = move(scalers_name2index_map);
    digiRunHeader->ScalersIndex2NameMap() = move(scalers_index2name_map);
    digiRunHeader->TrcIndex2ScalerIndexMap() = move(trcIdx2scalerIdx);
    LOGF(info, "Evt trigger count Total:");
    for (auto& el : digiRunHeader->GetTrcBitMap()) {
        const string& tr_name = el.first;
        LOGF(info, "%-12s %8llu", tr_name.c_str(), digiRunHeader->EventTriggerCnt()[tr_name]);
    }
    LOGF(info, "Raw T0 Total:");
    for (auto& t0_map_el : fRawT0Map) {
        const T0MapKey& key = t0_map_el.first;
        if (key.first.find("BD") == 0)
            continue;
        LOGF(info, "T0 %-14s.%02u = %10llu", key.first.c_str(), key.second,
             digiRunHeader->GetRawT0Counter(key.first.c_str(), key.second));
    }
    LOGF(info, "MSC Total:");
    digiRunHeader->PrintScalers();
    delete fInTreeSpills;
    //    ROOT::DisableImplicitMT();
    return kBMNSUCCESS;
}

BmnStatus BmnMetadataDecoder::ProcessStatusBlocks()
{
    GatherStatusBlocksFromDigis();
    // sort status blocks
    std::sort(metadata.SpillStatusVec().begin(), metadata.SpillStatusVec().end(), [](SpillStatus& a, SpillStatus& b) {
        return ((a.start_ts < b.start_ts) || ((a.start_ts == b.start_ts) && (a.cur_spill_time_ms < b.cur_spill_time_ms))
                //                        ((a.start_ts == b.start_ts) && (a.stop_ts < b.stop_ts))
        );
    });
    for (SpillStatus& ss : metadata.SpillStatusVec()) {
        LOG(debug) << BmnFunctionSet::TimePoint2String(ss.start_ts) << " start";
        LOG(debug) << BmnFunctionSet::TimePoint2String(ss.stop_ts) << "  stop";
    }
    // make spill map
    for (SpillStatus& ss : metadata.SpillStatusVec()) {
        SysPoint ts_spill_start = ss.start_ts;
        SysPoint ts_spill_stop = ss.stop_ts;
        auto spill_it = fSpillMap.find(ts_spill_start);
        if (spill_it == fSpillMap.end()) {
            bool inserted;
            std::tie(spill_it, inserted) = fSpillMap.insert(
                make_pair(ts_spill_start, BmnSpillInfo{.start_ts = ts_spill_start, .stop_ts = ts_spill_start}));
            LOGF(debug, "spill record inserted %d", inserted);
            BmnSpillInfo& si = spill_it->second;
            si.counters.ResizeTo(ss.counters);
        }
        BmnSpillInfo& info = spill_it->second;
        if (info.stop_ts < ts_spill_stop)
            info.stop_ts = ts_spill_stop;
        info.vec_spill_status.push_back(ss);
    }
    for (auto& it : fSpillMap) {
        BmnSpillInfo& info = it.second;
        LOGF(info, "\tstart %s\n\tstop  %s", BmnFunctionSet::TimePoint2String(info.start_ts),
             BmnFunctionSet::TimePoint2String(info.stop_ts));
    }
    if (fSpillMap.size() == 0) {
        LOGF(error, "Spill map is empty!");
        return kBMNERROR;
    }
    // calculate counters for each spill & sum total ones
    auto& total_cntrs = metadata.SpillCounters();
    total_cntrs.ResizeTo(fSpillMap.begin()->second.counters);
    for (auto& it : fSpillMap) {
        BmnSpillInfo& info = it.second;
        size_t blocks_count = info.vec_spill_status.size();
        if (blocks_count == 0)
            continue;
        auto& first_ss = info.vec_spill_status.front();
        auto& last_ss = info.vec_spill_status.back();
        info.counters = last_ss.counters;
        auto& spill_cntrs = info.counters;
        for (size_t i = 0; i < SpillStatus::counters_cnt; i++) {
            spill_cntrs.cntr_after_err[i] -= first_ss.counters.cntr_after_err[i];
            spill_cntrs.cntr_befor_err[i] -= first_ss.counters.cntr_befor_err[i];
            total_cntrs.cntr_after_err[i] += spill_cntrs.cntr_after_err[i];
            total_cntrs.cntr_befor_err[i] += spill_cntrs.cntr_befor_err[i];
        }
        for (size_t i_pair = 0; i_pair < spill_cntrs.cntr_input.size(); i_pair++) {
            auto& spill_p_input = spill_cntrs.cntr_input[i_pair];
            auto& first_p_input = first_ss.counters.cntr_input[i_pair];
            auto& spill_p_input_ar = spill_cntrs.cntr_input_after_red[i_pair];
            auto& first_p_input_ar = first_ss.counters.cntr_input_after_red[i_pair];
            auto& total_p_input = total_cntrs.cntr_input[i_pair];
            auto& total_p_input_ar = total_cntrs.cntr_input_after_red[i_pair];
            for (size_t i = 0; i < SpillStatus::counters_cnt; i++) {
                spill_p_input.second[i] -= first_p_input.second[i];
                spill_p_input_ar.second[i] -= first_p_input_ar.second[i];
                total_p_input.second[i] += spill_p_input.second[i];
                total_p_input_ar.second[i] += spill_p_input_ar.second[i];
                //                LOGF(info, "cntr %2u input %s total %lu", i, total_p_input.first.c_str(),
                //                total_p_input.second[i]);
            }
        }
    }
    metadata.SpillMap() = fSpillMap;
    vector<string> trc_names{"pBT", "BT", "MBT", "NiT", "BT&nBUSY", "CCT1", "CCT2"};
    //    for (auto& it : metadata.SpillMap()) {
    //        BmnSpillInfo& info = it.second;
    //        auto& spill_cntrs = info.counters;
    //        LOGF(info, "\n\tstart %s\n\tstop  %s", BmnFunctionSet::TimePoint2String(info.start_ts),
    //             BmnFunctionSet::TimePoint2String(info.stop_ts));
    //        if (spill_cntrs.cntr_input.size() <= 1) {
    //            LOGF(warning, "not enough data");
    //            continue;
    //        }
    //        for (string& name : trc_names) {
    //            uint16_t icntr = fTrigBitsMap[name];
    //            LOGF(info, "\t icntr %6u %8s", icntr, name.c_str());
    //            LOGF(info, "\t%8s          before %8lu     after %8lu ", name.c_str(),
    //            spill_cntrs.cntr_befor_err[icntr],
    //                 spill_cntrs.cntr_after_err[icntr]);
    //            LOGF(info, "\t%8s   BUSY    input %8lu input_red %8lu ", name.c_str(),
    //                 spill_cntrs.cntr_input[6].second[icntr], spill_cntrs.cntr_input_after_red[6].second[icntr]);
    //            LOGF(info, "\t%8s NOT_BUSY  input %8lu input_red %8lu ", name.c_str(),
    //                 spill_cntrs.cntr_input[7].second[icntr], spill_cntrs.cntr_input_after_red[7].second[icntr]);
    //        }
    //    }
    LOGF(info, "\n\tTRC TOTAL:");
    for (string& name : trc_names) {

        uint16_t icntr = fTrcBitMap[name];
        LOGF(info, "\t icntr %6u %8s", icntr, name.c_str());
        LOGF(info, "\t%8s          before %8lu     after %8lu ", name.c_str(), total_cntrs.cntr_befor_err[icntr],
             total_cntrs.cntr_after_err[icntr]);
        LOGF(info, "\t%8s   BUSY    input %8lu input_red %8lu ", name.c_str(), total_cntrs.cntr_input[6].second[icntr],
             total_cntrs.cntr_input_after_red[6].second[icntr]);
        LOGF(info, "\t%8s NOT_BUSY  input %8lu input_red %8lu ", name.c_str(), total_cntrs.cntr_input[7].second[icntr],
             total_cntrs.cntr_input_after_red[7].second[icntr]);
    }
    //    SaveData();
    //    DisposeDecoder();

    return kBMNSUCCESS;
}

BmnStatus BmnMetadataDecoder::InitDecoder()
{
    LoadConfig();

    return kBMNSUCCESS;
}

void BmnMetadataDecoder::ClearDigiArrays()
{
    if (fTrigMapper)
        fTrigMapper->ClearArrays();
    inEventHeader->Clear();
}

void BmnMetadataDecoder::ClearSpillDigiArrays()
{
    if (spillHeader)
        spillHeader->Clear();
    if (msc_copy)
        msc_copy->Delete();
    if (t0_copy)
        t0_copy->Delete();
}

void BmnMetadataDecoder::ResetDecoder(TString file)
{

    fRunStartTime = SysPoint::min();
    fRunEndTime = SysPoint::min();
    fRootFileName = file;
    LOGF(debug, "RunId %d got from the file (ResetDecoder)", fRunId);
    fRawTree->Reset();
    fDigiTree->Reset();
}

BmnStatus BmnMetadataDecoder::SaveData()
{
    metadata.SpillStatusVec()
        .clear();   // !!! temporary remove raw trc statuses from file @TODO resolve the writing issue
    for (auto& it : metadata.SpillMap()) {
        BmnSpillInfo& info = it.second;
        info.vec_spill_status.clear();
    }
    //    gDebug = 2;
    LOGF(debug, "Writing run header to file");
    Int_t wret = fDigiFileOut->WriteObject(digiRunHeader.get(), fDigiRunHdrName.Data());
    if (wret > 0)
        LOGF(debug, "Written bytes %d", wret);
    else
        DBGERR("WriteObject")

    LOGF(debug, "Writing metadata to file");
    TStopwatch timer;
    Double_t rtime;
    Double_t ctime;
    timer.Start();
    wret = fDigiFileOut->WriteObject(&metadata, fMetadataName.Data());
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    LOGF(debug, "Real time %f s, CPU time %f s  WriteObject", rtime, ctime);
    if (wret > 0)
        LOGF(debug, "Written bytes %d", wret);
    else
        DBGERR("WriteObject")

    LOGF(debug, "Flushing other data to file");
    fDigiFileOut->Write();
    LOGF(debug, "Closing the file");
    fDigiFileOut->Close();

    return kBMNSUCCESS;
}

BmnStatus BmnMetadataDecoder::DisposeDecoder()
{
    //    if (fTrigMapper)
    //        delete fTrigMapper;
    //    if (fMSCMapper)
    //        delete fMSCMapper;

    if (msc_copy)
        delete msc_copy;
    if (t0_copy)
        delete t0_copy;

    if (inEventHeader)
        delete inEventHeader;
    if (spillHeader)
        delete spillHeader;

    return kBMNSUCCESS;
}

TString BmnMetadataDecoder::GetSubNameAfterRunId(TString name)
{
    TPRegexp re_data(".*\\w+_\\w+_\\w+_(\\d+)([^/]*)\\.data");
    TPRegexp re_raw(".*\\w+_run(\\d+)([^/]*)_raw\\.root");
    TString str(name);
    if (re_data.MatchB(str))
        re_data.Substitute(str, "$2");
    else {
        if (re_raw.MatchB(str))
            re_raw.Substitute(str, "$2");

        else
            return "";
    }
    return str;
}

BmnStatus BmnMetadataDecoder::LoadConfig()
{
    LOGF(debug, "%s", __func__);
    if (fDecoderConfigFileName.length() == 0)
        fDecoderConfigFileName = string(getenv("VMCWORKDIR")) + "/config/bmnconf.json";
    pt::read_json(fDecoderConfigFileName, conf);
    return kBMNSUCCESS;
}
