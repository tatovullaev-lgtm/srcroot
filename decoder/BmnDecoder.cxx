#include "BmnDecoder.h"
// System
#include <arpa/inet.h> /* For ntohl for Big Endian LAND. */
// C++
#include <bitset>
#include <chrono>
#include <iostream>
// ROOT
#include "TStopwatch.h"
#include "TSystem.h"
// FairRoot
#include "FairLogger.h"
// BmnRoot
#include "BmnADCDigit.h"
#include "BmnConverterTools.h"
#include "BmnGemStripDigit.h"
#include "BmnHRBDigit.h"
#include "BmnMSCDigit.h"
#include "BmnRawDataDecoder.h"
#include "BmnSyncDigit.h"
#include "BmnTDCDigit.h"
#include "BmnTQDCADCDigit.h"
#include "DigiRunHeader.h"
#include "TangoData.h"
#include "UniDetectorParameter.h"
#include "UniRawFile.h"
#include "UniRun.h"
// Double_t realtime = 0.0;
// Double_t converterTime = 0.0;

BmnDecoder::BmnDecoder(TString file, TString outfile, ULong_t nEvents, ULong_t period)
    : isSpillStart(kFALSE)
    , fSpillCntr(0)
    , fApplyThreshold(kTRUE)
    , isTaskMode(false)
    , isForwardMode(false)
    , fTOF700ReferenceRun(0)
    , fTOF700ReferenceChamber(0)
    , fPeriodId(period)
    , fEventId(0)
    , fNTotalEvents(0)
    , fNSignalEvents(0)
    , fStartEventId(0)
    , fTime_s(0)
    , fTime_ns(0)
    , fRunStartTime(TimeZero)
    , fRunEndTime(TimeZero)
    , fRawTree(nullptr)
    , fRawTreeSpills(nullptr)
    , fDigiTree(nullptr)
    , fRootFileName(file)
    , fCalibFileName("adc-calib.root")
    , fDigiRunHdrName("DigiRunHeader")
    , fDchMapFileName("")
    , fMwpcMapFileName("")
    , fGemMapFileName("")
    , fVspMapFileName("")
    , fTof400PlaceMapFileName("")
    , fTof400StripMapFileName("")
    , fTof701PlaceMapFileName("")
    , fTof701StripMapFileName("")
    , fTof700MapFileName("")
    , fTof700GeomFileName("")
    , fZDCMapFileName("")
    , fZDCCalibrationFileName("")
    , fScWallMapFileName("")
    , fScWallCalibrationFileName("")
    , fFHCalMapFileName("")
    , fFHCalCalibrationFileName("")
    , fHodoMapFileName("")
    , fHodoCalibrationFileName("")
    , fNdetMapFileName("")
    , fNdetCalibrationFileName("")
    , fHgndMapFileName("")
    , fHgndCalibrationFileName("")
    , fECALMapFileName("")
    , fECALCalibrationFileName("")
    , fCscMapFileName("")
    , fTrigPlaceMapFileName("")
    , fTrigChannelMapFileName("")
    , fRawRunHdrName("RawRunHeader")
    , fMetadataName("RawRunMetadata")
    , fRootFileIn(nullptr)
    , fRootFileOut(nullptr)
    , fDigiFileOut(nullptr)
    , fCalibFile(nullptr)
    , fRawFileIn(nullptr)
    , sync(nullptr)
    , adc32(nullptr)
    , adc64(nullptr)
    , adc128(nullptr)
    , adc(nullptr)
    , vsp_raw(nullptr)
    , hrb(nullptr)
    , tdc(nullptr)
    , tqdc_tdc(nullptr)
    , tqdc_adc(nullptr)
    , tdc_hgnd(nullptr)
    , eventHeaderDAQ(nullptr)
    , rawRunHeader(nullptr)
    , msc(nullptr)
    , t0raw(nullptr)
    , silicon(nullptr)
    , sibt(nullptr)
    , gem(nullptr)
    , csc(nullptr)
    , vsp(nullptr)
    , tof400(nullptr)
    , tof700(nullptr)
    , tof701(nullptr)
    , zdc(nullptr)
    , scwall(nullptr)
    , fhcal(nullptr)
    , hodo(nullptr)
    , ndet(nullptr)
    , hgnd(nullptr)
    , ecal(nullptr)
    , dch(nullptr)
    , mwpc(nullptr)
    , msc_copy(nullptr)
    , t0_copy(nullptr)
    , eventHeader(nullptr)
    , spillHeader(nullptr)
    , fMaxEvent(nEvents)
    , fDat(0)
    , syncCounter(0)
    , fCscMapper(nullptr)
    , fGemMapper(nullptr)
    , fVspMapper(nullptr)
    , fSiliconMapper(nullptr)
    , fSiBTMapper(nullptr)
    , fDchMapper(nullptr)
    , fMwpcMapper(nullptr)
    , fTrigMapper(nullptr)
    , fTof400Mapper(nullptr)
    //, fTof700Mapper(nullptr) //Legacy
    , fTof701Mapper(nullptr)
    , fZDCMapper(nullptr)
    , fScWallMapper(nullptr)
    , fFHCalMapper(nullptr)
    , fHodoMapper(nullptr)
    , fNdetMapper(nullptr)
    , fHgndMapper(nullptr)
    , fECALMapper(nullptr)
    , fMSCMapper(nullptr)
    , nSpillEvents(0)
    , fBmnSetup(kBMNSETUP)
    , fEvForPedestals(N_EV_FOR_PEDESTALS)
    , fUseCalibFile(false)
    , fSaveCalibFile(true)
    , fExportJsonBocks(false)
    , fSaveOutput(true)
    , fT0Serial(0)
    , fT0SyncTime(0)
    , fT0Time(0.0)
    , fT0Width(0.0)
    , tai_utc_dif(0)
    , fVerbose(0)
    , _dfl_run_id{{8, 7814}, {9, 8974}}
{
    if (fRootFileName != "") {
        //        printf("fRootFileName %s\n", fRootFileName.Data());
        fRunId = GetRunIdFromFile(fRootFileName);
        TString subName = GetSubNameAfterRunId(file);
        std::printf("RunId %d got from the RawRoot file (BmnDecoder ctor)\n", fRunId);
        fDigiFileName = (outfile == "") ? Form("bmn_run%d%s_digi.root", fRunId, subName.Data()) : outfile;
        if (fRootFileName.Length()) {

        } else {
            Int_t lastSlash = fDigiFileName.Last('/');
            TString digiPath = (lastSlash == TString::kNPOS) ? "" : TString(fDigiFileName(0, lastSlash + 1));
            fRootFileName = Form("%sbmn_run%d%s_raw.root", digiPath.Data(), fRunId, subName.Data());
        }
        fSubName = subName;
    } else
        LOGF(debug, "Creating decoder without input file.");
    //InitUTCShift();
    //Legacy
    for (int c = 0; c < 60; c++) {
        refrun_tof700_slewing[c] = 0;
        refchamber_tof700_slewing[c] = 0;
        type_tof700_slewing[c] = 0;
    }

    // printf("Constructor of DECODER is finished\n");
}

BmnDecoder::~BmnDecoder()
{
    if (fGemMapper)
        delete fGemMapper;
    if (fCscMapper)
        delete fCscMapper;
    if (fVspMapper)
        delete fVspMapper;
    if (fSiliconMapper)
        delete fSiliconMapper;
    if (fSiBTMapper)
        delete fSiBTMapper;
    if (fDchMapper)
        delete fDchMapper;
    if (fMwpcMapper)
        delete fMwpcMapper;
    if (fTrigMapper)
        delete fTrigMapper;
    if (fTof400Mapper)
        delete fTof400Mapper;
    // Legacy
    //  if (fTof700Mapper)
    //      delete fTof700Mapper;
    if (fTof701Mapper)
        delete fTof701Mapper;
    if (fZDCMapper)
        delete fZDCMapper;
    if (fScWallMapper)
        delete fScWallMapper;
    if (fFHCalMapper)
        delete fFHCalMapper;
    if (fHodoMapper)
        delete fHodoMapper;
    if (fNdetMapper)
        delete fNdetMapper;
    if (fHgndMapper)
        delete fHgndMapper;
    if (fECALMapper)
        delete fECALMapper;
    if (fMSCMapper)
        delete fMSCMapper;

    if (gem)
        delete gem;
    if (csc)
        delete csc;
    if (vsp)
        delete vsp;
    if (msc_copy)
        delete msc_copy;
    if (t0_copy)
        delete t0_copy;
    if (dch)
        delete dch;
    if (mwpc)
        delete mwpc;
    if (silicon)
        delete silicon;
    if (tof400)
        delete tof400;
    if (tof700)
        delete tof700;
    if (tof701)
        delete tof701;
    if (zdc)
        delete zdc;
    if (scwall)
        delete scwall;
    if (fhcal)
        delete fhcal;
    if (hodo)
        delete hodo;
    if (ndet)
        delete ndet;
    if (hgnd)
        delete hgnd;
    if (ecal)
        delete ecal;

    if (eventHeader)
        delete eventHeader;
    if (spillHeader)
        delete spillHeader;
    if (eventHeaderDAQ)
        delete eventHeaderDAQ;
}

InitStatus BmnDecoder::Init()
{
    isTaskMode = true;
    FairRootManager* frm = FairRootManager::Instance();
    bool save_output = fSaveOutput;

    auto reg_fun = [&](TString name, TObject* ar) -> void {
        TClass* cl = TClass::GetClass(ar->ClassName());
        if (cl == TClonesArray::Class())
            frm->Register(name, name + "_dir", static_cast<TClonesArray*>(ar), save_output);
        else
            frm->Register(name, name + "_dir", static_cast<TNamed*>(ar), save_output);
        LOG(debug1) << "Register by class " << cl->GetName() << " : " << name;
        return;
    };
    LoadConfig();
    AssignInputs();
    InitMappers(reg_fun);

    fPedEvCntr = 0;          // counter for pedestal events between two recalculations
    fPedEvCntrBySpill = 0;   // counter for pedestal events between two spills
    fNoiseEvCntr = 0;        // counter of event for noise channel detection
    fPedEnough = kFALSE;
    fNoiseEnough = kFALSE;
    if (fUseCalibFile)
        LoadCalibFile(fCalibFileName);
    ConfigureStripMappers();
    fNSignalEvents = 0;
    fNTotalEvents = 0;
    fSpillCntr = 0;
    return kSUCCESS;
}

InitStatus BmnDecoder::ReInit()
{
    return kSUCCESS;
}

void BmnDecoder::Exec(Option_t* opt)
{
    DecodeDataToDigiIterate();

    return;
}

BmnStatus BmnDecoder::PreInitOffline()
{
    std::printf("[INFO] " ANSI_COLOR_BLUE "Starting to decode file %s\n" ANSI_COLOR_RESET, fRootFileName.Data());

    fRootFileIn = new TFile(fRootFileName, "READ");
    if (fRootFileIn->IsOpen() == false) {
        LOGF(error, "\n!!!!\ncannot open file %s \nDecodeDataToDigi stopped\n!!!!", fRootFileName.Data());
        return kBMNERROR;
    } else {
        LOGF(info, "Input file: %s,   Output file: %s", fRootFileName.Data(), fDigiFileName.Data());
    }
    fRawTree = (TTree*)fRootFileIn->Get("BMN_RAW");
    AssignInputs();

    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    NRawTreeEvents = fRawTree->GetEntries();   //(fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ?
    // fRawTree->GetEntries() : fMaxEvent;
    NRawTreeSpills = fRawTreeSpills ? fRawTreeSpills->GetEntries() : 0;
    if (fMaxEvent == 0)
        fMaxEvent = NRawTreeEvents;
    fMaxEvent = Min(fMaxEvent, NRawTreeEvents);
    return kBMNSUCCESS;
}

BmnStatus BmnDecoder::DecodeDataToDigi()
{
    if (PreInitOffline() == kBMNERROR)
        return kBMNERROR;   
    InitDecoder();
    DecodeOffline();
    return kBMNSUCCESS;
}

BmnStatus BmnDecoder::DecodeOffline()
{
    /****************** 1st Iter of Si/GEM mapper   *************/
    // Double_t rtime;
    // Double_t ctime;
    TStopwatch timer;
    if (fGemMapper || fSiliconMapper || fCscMapper || fSiBTMapper /* || fVspMapper */) {
        std::printf("[INFO] Searching for pedestal events\n");
        // printf("\n[INFO]" ANSI_COLOR_BLUE "1st iter Collecting data for ADC pedestals calculation:\n"
        // ANSI_COLOR_RESET); printf("\tNumber of requested pedestal events is "); printf(ANSI_COLOR_RED "%d\n"
        // ANSI_COLOR_RESET, fEvForPedestals);
        for (Long64_t iEv = 0; iEv < NRawTreeEvents; ++iEv) {
            fRawTree->GetEntry(iEv);

            if (!eventHeaderDAQ)
                continue;
            fCurEventType = eventHeaderDAQ->GetEventType();
            if (fCurEventType != kBMNPEDESTAL)
                continue;
            if (fPedEvCntr != fEvForPedestals) {
                CopyDataToPedMap(fPedEvCntr);
                fPedEvCntr++;
                //                } else if (fVerbose == 0)
                //                    DrawBar(fPedEvCntr, fEvForPedestals);
            } else
                break;
        }
        /*if (fPedEvCntr != fEvForPedestals) {
            printf(ANSI_COLOR_RED "\n[WARNING]" ANSI_COLOR_RESET);
            printf(" Not enough pedestal events (%d instead of %d)\n", fPedEvCntr, fEvForPedestals);
        }*/
        LOGF(info, "Found a total of %d pedestal events", fPedEvCntr);
        RecalculatePedestals();
        //        if (fGemMapper) {
        //            fGemMapper->DrawDebugHists2D();
        //            fGemMapper->DrawDebugHists();
        ////            fGemMapper->ClearDebugHists();
        //            return kBMNSUCCESS;
        //        }
        //        if (fSiliconMapper) {
        //            fSiliconMapper->DrawDebugHists2D();
        //            fSiliconMapper->DrawDebugHists();
        //            fSiliconMapper->ClearDebugHists();
        ////            return kBMNSUCCESS;
        //        }
        /*fPedEvCntr = 0;
        printf("\n[INFO]" ANSI_COLOR_BLUE " Clear noisy channels:\n" ANSI_COLOR_RESET);
        printf("\tFilling signal profiles for station-module-layer histograms\n");
        UInt_t nEvForNoiseCorrection = 10000;
        printf("\tNumber of requested events is ");
        printf(ANSI_COLOR_RED "%u\n" ANSI_COLOR_RESET, nEvForNoiseCorrection);
        printf("\tActual number of events is ");
        printf(ANSI_COLOR_RED "%lld\n" ANSI_COLOR_RESET, NRawTreeEvents);*/
        std::printf("[INFO] Searching for signal events\n");
        for (Long64_t iEv = 0; iEv < NRawTreeEvents; ++iEv) {
            fRawTree->GetEntry(iEv);
            if (!eventHeaderDAQ)
                continue;
            fCurEventType = eventHeaderDAQ->GetEventType();
            if (fCurEventType == kBMNPEDESTAL) {
                /*CopyDataToPedMap(fPedEvCntr);
                fPedEvCntr++;
                if ((fPedEvCntr == fEvForPedestals - 1)) {
                    RecalculatePedestals();
                    fPedEvCntr = 0;
                }*/
            } else {   // payload
                //                if (fPrevEventType == kBMNPEDESTAL && fPedEvCntr == fEvForPedestals - 1)
                if (fCurEventType == kBMNPAYLOAD) {
                    fNoiseEvCntr++;
                    FillProfiles();
                    if (iEv != 0 && fNoiseEvCntr % 2500 == 0)
                        LOG(info) << "Profile event #" << fNoiseEvCntr << "/" << fEvForNoiseCorr << ";";
                }
            }
            fPrevEventType = fCurEventType;
            if (fNoiseEvCntr >= fEvForNoiseCorr)
                break;
        }
        LOGF(info, "Found a total of %d payload events", fNoiseEvCntr);
        FillNoisyChannels();
        RecalculatePedestals();
    }

    /******************  End of the 1st iter ******************/
    fCurEventType = kBMNPAYLOAD;
    fPrevEventType = fCurEventType;
    fNoiseEvCntr = 0;
    fPedEvCntr = 0;
    vector<UInt_t> startTripEvent;
    vector<UInt_t> endTripEvent;
    //    //check for trip information
    //    TString date_start = fRunStartTime.AsString("s"); // 1252 run
    //    TString date_end = fRunEndTime.AsString("s");
    //    std::printf("START (event %u):\t%s\n", fStartEventId, fRunStartTime.AsString());
    //    std::printf("FINISH (event %u):\t%s\n", fEventId, fRunEndTime.AsString());
    //    TangoData db_tango((char*) date_start.Data(), (char*) date_end.Data(), (char*) "gem", (char*) "trip");
    //    TangoData::enumConditions condition = TangoData::conditionEqual;
    //    bool condition_value = 1;
    //    vector<int> map_channel_run5{ 1, 3, 0, 5, 2, 6, 4, 7};
    //    vector<int> map_channel_run6_b1529{ 1, 3, 0, 5, 2, 6, 4};
    //    vector<int> map_channel_run6_a1529{ 1, 3, 0, 5, 6, 4, 2};
    //    vector<int>* map_channel = nullptr;
    //    switch (fPeriodId) {
    //        case 5:
    //            map_channel = &map_channel_run5;
    //            break;
    //        case 6:
    //            if (fRunId < 1569)
    //                map_channel = &map_channel_run6_b1529;
    //            else
    //                map_channel = &map_channel_run6_a1529;
    //            break;
    //        default:
    //            std::printf("Warning: unknown GEM Tango channel map for the run!\n");
    //            break;
    //
    //    }
    //
    //    UInt_t runLength = fRunEndTime.AsDouble() - fRunStartTime.AsDouble(); //in seconds.nanoseconds
    //    Double_t timeStep = runLength * 1.0 / fNTotalEvents; //time for one event
    //    //printf("Run duration = %d sec.\t TimeStep = %f sec./event\n", runLength, timeStep);
    //    if (map_channel) {
    //        TObjArray* tango_data_gem = db_tango.SearchTangoIntervals(condition, condition_value, map_channel);
    //        if (tango_data_gem) {
    //            for (Int_t i = 0; i < tango_data_gem->GetEntriesFast(); ++i) {
    //                TObjArray* currGemTripInfo = (TObjArray*) tango_data_gem->At(i);
    //                if (currGemTripInfo->GetEntriesFast() != 0)
    //                    for (Int_t j = 0; j < currGemTripInfo->GetEntriesFast(); ++j) {
    //                        TangoTimeInterval* ti = (TangoTimeInterval*) currGemTripInfo->At(j);
    //                        startTripEvent.push_back(UInt_t((ti->start_time.Convert() - fRunStartTime.AsDouble()) /
    //                        timeStep)); endTripEvent.push_back(UInt_t((ti->end_time.Convert() -
    //                        fRunStartTime.AsDouble()) / timeStep));
    //                    }
    //            }
    //        }
    //    }

    std::printf("[INFO]");
    std::printf(ANSI_COLOR_BLUE " Main loop over events:\n" ANSI_COLOR_RESET);
    for (Long64_t iEv = 0; iEv < fMaxEvent; ++iEv) {
        ClearDigiArrays();

        fRawTree->GetEntry(iEv);

        fCurEventType = eventHeaderDAQ->GetEventType();
        fNTotalEvents++;
        if ((fCurEventType == kBMNPAYLOAD))   // || (fCurEventType == kBMNPEDESTAL))
            fNSignalEvents++;
        fEventId = eventHeaderDAQ->GetEventId();
        if (iEv != 0 && iEv % 2500 == 0)
            LOGF(info, "Decoded  %7lld events. EventId %7lld", iEv, fEventId);
        if ((fStartEventId == 0) && (fEventId))
            fStartEventId = fEventId;
        FillTimeShiftsMap();
        if (!eventHeaderDAQ)
            continue;
        fSpillCntr += eventHeaderDAQ->GetSpillStart() ? 1 : 0;
        //        if (eventHeaderDAQ->GetSpillStart())
        //            printf("Spills: %6d  iEv %6d\n", fSpillCntr, iEv);
        Bool_t isTripEvent = kFALSE;
        for (size_t iTrip = 0; iTrip < startTripEvent.size(); ++iTrip) {
            if (eventHeaderDAQ->GetEventId() > startTripEvent[iTrip]
                && eventHeaderDAQ->GetEventId() < endTripEvent[iTrip])
            {
                isTripEvent = kTRUE;
                break;
            }
        }
        if (fVerbose > 0)
            if ((fNSignalEvents % 5000 == 0) && ((fCurEventType == kBMNPAYLOAD) || (fCurEventType == kBMNPEDESTAL)))
                cout << "Digitization: " << fNSignalEvents << "/" << fNTotalEvents << "/" << fMaxEvent
                     << " processed; Spill #" << fSpillCntr << endl;
        //        } else if (fVerbose == 0)
        //            DrawBar(fNSignalEvents, fMaxEvent);

        if (fTrigMapper) {
            timer.Start();
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc, fTimeShifts);
            timer.Stop();
            // rtime = timer.RealTime();
            // ctime = timer.CpuTime();
            //                     printf("\nReal time %f s, CPU time %f s  fTrigMapper TQDC\n", rtime, ctime);
            timer.Start();
            fTrigMapper->FillEvent(tdc, fTimeShifts);
            timer.Stop();
            // rtime = timer.RealTime();
            // ctime = timer.CpuTime();
            //                     printf("Real time %f s, CPU time %f s  fTrigMapper TDC\n", rtime, ctime);
            fT0Time = 0.;
            fT0Width = 0.;
            GetT0Info(fT0Time, fT0Width);
        }
        eventHeader->SetRunId(eventHeaderDAQ->GetRunId());
        eventHeader->SetEventId(eventHeaderDAQ->GetEventId());
        eventHeader->SetPeriodId(eventHeaderDAQ->GetPeriodId());
        eventHeader->SetEventTimeTS(TTimeStamp(time_t(fTime_s), fTime_ns));
        eventHeader->SetEventTime(TTimeStamp(time_t(fTime_s), fTime_ns).AsDouble());
        eventHeader->SetEventType(fCurEventType);
        eventHeader->SetTripWord(isTripEvent);
        eventHeader->SetTrigInfo(eventHeaderDAQ->GetTrigInfo());
        eventHeader->SetTimeShift(fTimeShifts);
        eventHeader->SetStartSignalInfo(fT0Time, fT0Width);
        eventHeader->SetSpillStart(eventHeaderDAQ->GetSpillStart());
        eventHeader->SetInputSignalsAR(eventHeaderDAQ->GetInputSignalsAR());
        eventHeader->SetInputSignalsBR(eventHeaderDAQ->GetInputSignalsBR());
        eventHeader->SetInputSignalsVector(move(eventHeaderDAQ->GetInputSignalsVector()));
        eventHeader->SetSpillId(eventHeaderDAQ->GetSpillId());
        eventHeader->SetSpillStartTS(eventHeaderDAQ->GetSpillStartTS());

        if (fCurEventType == kBMNPEDESTAL) {
            fPedEvCntrBySpill++;
            if (fPedEvCntr < (fEvForPedestals - 1)) {
                CopyDataToPedMap(fPedEvCntr);
                fPedEvCntr++;
            }
        } else {
            if ((fPrevEventType == kBMNPEDESTAL) && (fPedEvCntr == (fEvForPedestals - 1))) {
                RecalculatePedestals();
#ifdef ADC_QA
                FillAdcQa();
#endif
            }
            if (fCurEventType == kBMNPAYLOAD) {   // payload
                timer.Start();
                if (fCscMapper)
                    fCscMapper->FillEvent(adc32, csc);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                                 printf("\nReal time %f s, CPU time %f s
                //                                                 fCscMapper\n", rtime, ctime);
                timer.Start();

                //                auto start = chrono::high_resolution_clock::now();
                //                auto stop = chrono::high_resolution_clock::now();
                // clock_t c_start = clock();
                if (fGemMapper)
                    fGemMapper->FillEvent(adc32, gem);
                // clock_t c_stop = clock();
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                 printf("Real time %2.6f s, CPU time %2.6f s  fGemMapper\n", rtime, ctime);
                //                 printf("           clock() CPU time %2.6f s  fGemMapper\n",
                //                                         (Double_t)(c_stop-c_start)/CLOCKS_PER_SEC);
                timer.Start();
                if (fSiliconMapper)
                    fSiliconMapper->FillEvent(adc128, silicon);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                                 printf("Real time %f s, CPU time %f s
                //                                                 fSiliconMapper\n", rtime, ctime);
                if (fSiBTMapper)
                    fSiBTMapper->FillEvent(adc64, sibt);
                if (fVspMapper)
                    fVspMapper->FillEvent(vsp_raw, vsp);
                // #endif
                timer.Start();
                if (fDchMapper)
                    fDchMapper->FillEvent(tdc, &fTimeShifts, dch, fT0Time);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                     printf("Real time %f s, CPU time %f s  fDchMapper\n", rtime,
                //                                     ctime);
                timer.Start();
                if (fMwpcMapper)
                    fMwpcMapper->FillEvent(hrb, mwpc);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                     printf("Real time %f s, CPU time %f s  fMwpcMapper\n", rtime,
                //                                     ctime);
                timer.Start();
                if (fTof400Mapper)
                    fTof400Mapper->FillEvent(tdc, &fTimeShifts, tof400);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                     printf("Real time %f s, CPU time %f s  fTof400Mapper\n", rtime,
                //                                     ctime);

                // Legacy
                //  timer.Start();
                //  //            if (fTof700Mapper && fT0Time != 0. && fT0Width != 0.) fTof700Mapper->fillEvent(tdc,
                //  //            &fTimeShifts, fT0Time, fT0Width, tof700);
                //  if (fTof700Mapper)
                //      fTof700Mapper->fillEvent(tdc, &fTimeShifts, fT0Time, fT0Width, tof700);
                //  timer.Stop();
                //  rtime = timer.RealTime();
                //  ctime = timer.CpuTime();
                //                                      printf("Real time %f s, CPU time %f s  fTof700Mapper\n", rtime,
                //                                      ctime);
                timer.Start();
                if (fTof701Mapper)
                    fTof701Mapper->FillEvent(tdc, &fTimeShifts, tof701);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                     printf("Real time %f s, CPU time %f s  fTof701Mapper\n", rtime,
                //                                     ctime);
                timer.Start();
                if (fZDCMapper)
                    fZDCMapper->fillEvent(adc, zdc);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                     printf("Real time %f s, CPU time %f s  fZDCMapper\n", rtime,
                //                                     ctime);
                timer.Start();
                if (fScWallMapper)
                    fScWallMapper->fillEvent(adc, scwall);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                     printf("Real time %f s, CPU time %f s  fScWallMapper\n", rtime,
                //                                     ctime);
                timer.Start();
                if (fFHCalMapper)
                    fFHCalMapper->fillEvent(adc, fhcal);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                     printf("Real time %f s, CPU time %f s  fFHCalMapper\n", rtime,
                //                                     ctime);
                timer.Start();
                if (fHodoMapper)
                    fHodoMapper->fillEvent(tqdc_tdc, tqdc_adc, hodo);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                     printf("Real time %f s, CPU time %f s  fHodoMapper\n", rtime,
                //                                     ctime);
                timer.Start();
                if (fNdetMapper)
                    fNdetMapper->fillEvent(tqdc_tdc, tqdc_adc, &fTimeShifts, ndet);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                     printf("Real time %f s, CPU time %f s  fNdetMapper\n", rtime,
                //                                     ctime);
                timer.Start();
                if (fECALMapper)
                    fECALMapper->fillEvent(adc, ecal);
                timer.Stop();
                // rtime = timer.RealTime();
                // ctime = timer.CpuTime();
                //                                 printf("Real time %f s, CPU time %f s  fECALMapper\n", rtime, ctime);
            }
        }
        if (fMSCMapper && (fPeriodId == 8))
            fMSCMapper->SumEvent7(msc, eventHeader, spillHeader, fPedEvCntrBySpill);

        fDigiTree->Fill();
#ifdef FILL_RMS
        if (fCurEventType == kBMNPAYLOAD) {   // testing RMS
            printf("Test RMS. Exit");
            break;
        }
#endif
        fPrevEventType = fCurEventType;
        if ((Long_t)fNSignalEvents >= fMaxEvent)
            break;
    }
    //    if (fPeriodId >= 8) {
    //        // select index intervals for each spill time interval
    //        vector<pair<SysPoint, size_t>> vts;
    //        for (Long64_t iEntry = 0; iEntry < NRawTreeSpills; ++iEntry) {
    //            fRawTreeSpills->GetEntry(iEntry);
    //            Int_t ne = msc->GetEntriesFast();
    //            if (ne > 0) {
    //                BmnMSCDigit* dig = static_cast<BmnMSCDigit*>(msc->At(0));
    //                vts.push_back(make_pair(dig->GetTime(), iEntry));
    //            }
    //        }
    //        vector<pair<SysPoint, size_t>> vts_t0;
    //        for (Long64_t iEntry = 0; iEntry < fRawTreeSpillsT0->GetEntries(); ++iEntry) {
    //            fRawTreeSpillsT0->GetEntry(iEntry);
    //            Int_t ne = t0raw->GetEntriesFast();
    //            if (ne > 0) {
    //                BmnT0Raw<kT0_BIN_BLOCK_WORDS>* dig = static_cast<BmnT0Raw<kT0_BIN_BLOCK_WORDS>*>(t0raw->At(0));
    //                vts_t0.push_back(make_pair(dig->GetTime() - 1s, iEntry)); // crutch to fit rough time inside the
    //                spill LOG(info) << "T0 time " << BmnFunctionSet::TimePoint2String(dig->GetTime())
    //                        << " iEntry " << iEntry;
    //            }
    //        }
    //        auto smap = rawRunHeader->GetSpillMap();
    //        UInt_t spillId = 0;
    //        for (auto& mapel : smap) {
    //            ClearSpillDigiArrays();
    //            spillHeader->Clear();
    //            BmnSpillInfo& si = mapel.second;
    //            LOGF(info, "Spill:\n\tstart ts: %s\n\tstop  ts: %s", BmnFunctionSet::TimePoint2String(si.start_ts),
    //                 BmnFunctionSet::TimePoint2String(si.stop_ts));
    //            spillHeader->SetStartTime(si.start_ts);
    //            spillHeader->SetStopTime(si.stop_ts);
    //            spillHeader->SetSpillId(spillId++);
    //            // calc MSC count inside spill
    //            vector<pair<SysPoint, size_t>> spillIds;
    //            timer.Start();
    //            copy_if(begin(vts), end(vts), std::back_inserter(spillIds),
    //                    [&si](pair<SysPoint, size_t> p) { return (p.first >= si.start_ts) && (p.first <= si.stop_ts);
    //                    });
    //            timer.Stop();
    //            rtime = timer.RealTime();
    //            ctime = timer.CpuTime();
    //            LOGF(info, "IDs entries MSC:%d", spillIds.size());
    //            LOGF(info, "Real time %f s, CPU time %f s  ids copy MSC", rtime, ctime);
    //            timer.Start();
    //            Long64_t entriesCumulativeSize{0};
    //            for (auto& p : spillIds) {
    //                //                LOGF(debug3, "MSC record: %6d ts: %s", p.second, p.first.AsString());
    //                Long64_t iEntry = p.second;
    //                fRawTreeSpills->GetEntry(iEntry);
    //                entriesCumulativeSize += msc->GetEntriesFast();
    //            }
    //            timer.Stop();
    //            rtime = timer.RealTime();
    //            ctime = timer.CpuTime();
    //            LOGF(info, "Real time %f s, CPU time %f s  cumulate ", rtime, ctime);
    //            LOGF(info, "msc_copy size %lld ", msc_copy->GetEntriesFast());
    //            LOGF(info, "msc_copy adds %lld ", entriesCumulativeSize);
    //            msc_copy->Expand(entriesCumulativeSize);
    //            // T0
    //            vector<pair<SysPoint, size_t>> spillIds_t0;
    //            timer.Start();
    //            copy_if(begin(vts_t0), end(vts_t0), std::back_inserter(spillIds_t0),
    //                    [&si](pair<SysPoint, size_t> p) { return (p.first >= si.start_ts) && (p.first <= si.stop_ts);
    //                    });
    //            timer.Stop();
    //            rtime = timer.RealTime();
    //            ctime = timer.CpuTime();
    //            LOGF(info, "IDs entries T0:%d", spillIds_t0.size());
    //            LOGF(info, "Real time %f s, CPU time %f s  ids copy T0", rtime, ctime);
    //            for (auto& p : spillIds) {
    //                //                LOGF(debug3, "MSC record: %6d ts: %s", p.second, p.first.AsString());
    //                Long64_t iEntry = p.second;
    //                fRawTreeSpills->GetEntry(iEntry);
    //                //                LOGF(info, "Get[%d] %d",iEntry,fRawTreeSpills->GetEntry(iEntry));
    //                timer.Start();
    //                if (fMSCMapper)
    //                    fMSCMapper->SumEvent(msc, eventHeader, nullptr, fPedEvCntrBySpill);
    //                timer.Stop();
    //                rtime = timer.RealTime();
    //                ctime = timer.CpuTime();
    //                //                LOGF(info, "Real time %f s, CPU time %f s  SumEvent", rtime, ctime);
    //                timer.Start();
    //                msc_copy->AbsorbObjects(msc);
    //                timer.Stop();
    //                rtime = timer.RealTime();
    //                ctime = timer.CpuTime();
    //                //                LOGF(info, "Real time %f s, CPU time %f s  AbsorbObjects", rtime, ctime);
    //            }
    //            for (auto& p : spillIds_t0) {
    //                Long64_t iEntry = p.second;
    //                fRawTreeSpillsT0->GetEntry(iEntry);
    //                t0_copy->AbsorbObjects(t0raw);
    //            }
    //            fDigiTreeSpills->Fill();
    //        }
    //    } else
    {
        for (UInt_t iSpillEntry = 0; iSpillEntry < NRawTreeSpills; ++iSpillEntry) {
            ClearSpillDigiArrays();
            fRawTreeSpills->GetEntry(iSpillEntry);
            //            if (fMSCMapper)
            //                fMSCMapper->SumEvent(msc, eventHeader, spillHeader, fPedEvCntrBySpill);
            msc_copy->AbsorbObjects(msc);
            t0_copy->AbsorbObjects(t0raw);
            if (t0_copy->GetEntries()) {
                //                LOGF(info, "t0_copy entries %d ", t0_copy->GetEntries());
                BmnT0Raw<kT0_BIN_BLOCK_WORDS>* dig = static_cast<BmnT0Raw<kT0_BIN_BLOCK_WORDS>*>(t0_copy->At(0));
                LOG(info) << "T0 time " << BmnFunctionSet::TimePoint2String(dig->GetTime()) << " iEntry "
                          << iSpillEntry;
            }
            timer.Start();
            if (fHgndMapper)
                fHgndMapper->fillEvent(tdc_hgnd, fT0SyncTime, hgnd);
            timer.Stop();
            // rtime = timer.RealTime();
            // ctime = timer.CpuTime();
            //                                     printf("Real time %f s, CPU time %f s  fHgndMapper\n", rtime,
            //                                     ctime);
            fDigiTreeSpills->Fill();
        }
    }
#ifdef ADC_QA
    SaveAdcQa();
#endif
    //    if (fSiliconMapper) {
    //        fSiliconMapper->DrawDebugHists2D();
    //        //                            fSiliconMapper->DrawDebugHists();
    //        //                            fSiliconMapper->ClearDebugHists();
    //        //        return kBMNSUCCESS;
    //    }
    // Legacy
    // if (fTof700Mapper) {
    //     fTof700Mapper->WriteSlewingResults();
    //     fDigiFileOut->cd();
    // }
    fRunEndTime = TTimeStamp(time_t(fTime_s), fTime_ns);
    DigiRunHeader* runHeader = new DigiRunHeader(fPeriodId, fRunId, fRunStartTime, fRunEndTime);
    runHeader->SetTrigConfig(rawRunHeader->GetTrigConfig());
    runHeader->SetSpillMap(move(rawRunHeader->GetSpillMap()));
    if (fTrigMapper) {
        runHeader->T0Map() = fTrigMapper->T0Map();
        runHeader->GetTrcBitMap() = fTrigMapper->GetTrcMap();
        runHeader->GetScalerMap() = fTrigMapper->GetScalerMap();
    }
    if (fMSCMapper)
        fMSCMapper->FillRunHeader(runHeader);
    fDigiFileOut->WriteObject(runHeader, fDigiRunHdrName.Data());
    fDigiFileOut->WriteObject(metadata, fMetadataName.Data());

    std::printf(ANSI_COLOR_RED "\n=============== RUN" ANSI_COLOR_RESET);
    std::printf(ANSI_COLOR_BLUE " %04d " ANSI_COLOR_RESET, fRunId);
    std::printf(ANSI_COLOR_RED "SUMMARY ===============\n" ANSI_COLOR_RESET);
    std::printf("START (event %u):\t%s\n", fStartEventId, fRunStartTime.AsString());
    std::printf("FINISH (event %u):\t%s\n", fEventId, fRunEndTime.AsString());
    std::printf("Total Events:\t%lu\nSignal Events:\t%lu\n", fNTotalEvents, fNSignalEvents);
    std::printf(ANSI_COLOR_RED "================================================\n" ANSI_COLOR_RESET);

    if (fRawTree)
        fRawTree->Delete();
    //    fDigiTree->Write();
    //    if (fDigiTreeSpills) fDigiTreeSpills->Write();

    fDigiFileOut->Write();
    fDigiFileOut->Close();
    delete runHeader;
    fRootFileIn->Close();
    Finalize();
    return kBMNSUCCESS;
}

BmnStatus BmnDecoder::InitDecoder()
{
    auto reg_fun = [&](TString name, TObject* ar) -> void {
        fDigiTree->Branch(name, &ar);
        LOGF(debug1, "Register branch: %s", name.Data());
        return;
    };
    LoadConfig();
    InitMappers(reg_fun);

    fPedEvCntr = 0;          // counter for pedestal events between two recalculations
    fPedEvCntrBySpill = 0;   // counter for pedestal events between two spills
    fNoiseEvCntr = 0;        // counter of event for noise channel detection
    fPedEnough = kFALSE;
    fNoiseEnough = kFALSE;
    if (fUseCalibFile)
        LoadCalibFile(fCalibFileName);
    ConfigureStripMappers();
    fNSignalEvents = 0;
    fNTotalEvents = 0;
    fSpillCntr = 0;
    return kBMNSUCCESS;
}

void BmnDecoder::AssignInputs()
{
    Assignment_Fun("SYNC", &sync);
    Assignment_Fun("ADC32", &adc32);
    Assignment_Fun("ADC64", &adc64);
    Assignment_Fun("ADC128", &adc128);
    Assignment_Fun("ADC", &adc);
    Assignment_Fun("VSP_RAW", &vsp_raw);
    Assignment_Fun("TDC", &tdc);
    Assignment_Fun("TQDC_ADC", &tqdc_adc);
    Assignment_Fun("TQDC_TDC", &tqdc_tdc);
    Assignment_Fun("HRB", &hrb);
    Assignment_Fun("BmnEventHeader.", &eventHeaderDAQ);
    if ((!isForwardMode) && (!isTaskMode)) {
        fRawTreeSpills = (TTree*)fRootFileIn->Get("BMN_RAW_SPILLS");
        if (fRawTreeSpills) {
            fRawTreeSpills->SetBranchAddress("MSC", &msc);
            fRawTreeSpills->SetBranchAddress("T0Raw", &t0raw);
            fRawTreeSpills->SetBranchAddress("TDC_HGND", &tdc_hgnd);
        }
        rawRunHeader = static_cast<DigiRunHeader*>(fRootFileIn->Get(fRawRunHdrName));
        metadata = static_cast<BmnMetadataRaw*>(fRootFileIn->Get(fMetadataName));
        //                    printf("extracted raw tcc %lu after\n",
        //                    rawRunHeader->GetTrigConfig().TrigChannelConfig().size());
    }
    return;
}

void BmnDecoder::ConfigureStripMappers()
{
    if (fGemMapper) {
        fGemMapper->SetApplyThreshold(fApplyThreshold);
        fGemMapper->SetEvForPedestals(fEvForPedestals);
    }
    if (fCscMapper) {
        fCscMapper->SetApplyThreshold(fApplyThreshold);
        fCscMapper->SetEvForPedestals(fEvForPedestals);
    }
    if (fSiliconMapper) {
        fSiliconMapper->SetApplyThreshold(fApplyThreshold);
        fSiliconMapper->SetEvForPedestals(fEvForPedestals);
    }
    if (fSiBTMapper) {
        fSiBTMapper->SetApplyThreshold(fApplyThreshold);
        fSiBTMapper->SetEvForPedestals(fEvForPedestals);
    }
    //    if (fVspMapper) {
    //        fVspMapper->SetApplyThreshold(fApplyThreshold);
    //        fVspMapper->SetEvForPedestals(fEvForPedestals);
    //    }
}

void BmnDecoder::InitMappers(std::function<void(TString, TObject*)> branch_reg_fun)
{
    uint32_t run4init = (fRunId == UNKNOWN_RUNID) ? _dfl_run_id[fPeriodId] : fRunId;
    fDigiTree =
        new TTree(conf.get<string>("Decoder.DigiTreeName").c_str(), conf.get<string>("Decoder.DigiTreeTitle").c_str());
    eventHeader = new BmnEventHeader();
    cout<<11<<endl; 
    Register_Fun("BmnEventHeader.", &eventHeader);
    if (fRawTreeSpills) {
        fDigiTreeSpills = new TTree("spill", "spill");
        spillHeader = new BmnSpillHeader();
        fDigiTreeSpills->Branch("BmnSpillHeader.", &spillHeader);
        fMSCMapper = new BmnMscRaw2Digit(fPeriodId, run4init, fMSCMapFileName, fRawTreeSpills, fDigiTreeSpills);
        msc_copy = new TClonesArray(msc->GetClass());
        fDigiTreeSpills->Branch("MSC", &msc_copy);
        t0_copy = new TClonesArray(t0raw->GetClass());
        fDigiTreeSpills->Branch("T0Raw", &t0_copy);
        hgnd = new TClonesArray("BmnHgndDigi");
        fDigiTreeSpills->Branch("HgndDigi", &hgnd);
    }
    cout<<12<<endl; 
    // check if detector is in setup and is active
    if (fDetectorSetup.count(kBC) > 0 && fDetectorSetup.at(kBC) == 1) {
        fTrigMapper = new BmnTrigRaw2Digit(fTrigPlaceMapFileName, fTrigChannelMapFileName, branch_reg_fun);
        cout<<13<<endl; 
        if (fT0Serial == 0) {
            fT0Serial = fTrigMapper->GetT0Serial();
            cout<<14<<endl; 
            LOGF(info, "T0 serial 0x%X got from trig mapping (InitMappers)", fT0Serial);
        }
        cout<<15<<endl; 
        fTrigMapper->SetSetup(fBmnSetup);
        cout<<16<<endl; 
    }
    
    if (fDetectorSetup.count(kMWPC) > 0 && fDetectorSetup.at(kMWPC) == 1) {
        mwpc = new TClonesArray("BmnMwpcDigit");
        Register_Fun("MWPC", &mwpc);
        fMwpcMapper = new BmnMwpcRaw2Digit(fMwpcMapFileName);
    }

    if (fDetectorSetup.count(kSILICON) > 0 && fDetectorSetup.at(kSILICON) == 1) {
        silicon = new TClonesArray("BmnSiliconDigit");
        Register_Fun("SILICON", &silicon);
        fSiliconMapper = new BmnSiliconRaw2Digit(fPeriodId, run4init, fSiliconMapFileName, fBmnSetup);
        fSiliconMapper->SetVerbose(0);
#ifdef ADC_QA
        fFsdQa = new BmnAdcQA(fPeriodId, run4init, "FSD");
#endif
    }

    if (fDetectorSetup.count(kSiBT) > 0 && fDetectorSetup.at(kSiBT) == 1) {
        sibt = new TClonesArray("BmnSiBTDigit");
        Register_Fun("SiBT", &sibt);
        fSiBTMapper = new BmnSiBTRaw2Digit(fPeriodId, run4init, fSiBTMapFileName, fBmnSetup);
    }

    if (fDetectorSetup.count(kGEM) > 0 && fDetectorSetup.at(kGEM) == 1) {
        gem = new TClonesArray("BmnGemStripDigit");
        Register_Fun("GEM", &gem);
        fGemMapper = new BmnGemRaw2Digit(fPeriodId, run4init, fGemMapFileName, fBmnSetup);
#ifdef ADC_QA
        fGemQa = new BmnAdcQA(fPeriodId, run4init, "GEM");
#endif
    }

    if (fDetectorSetup.count(kVSP) > 0 && fDetectorSetup.at(kVSP) == 1) {
        LOGF(info, "----------   Init VSP mapper   ----------");
        vsp = new TClonesArray("BmnVSPDigit");
        Register_Fun("VSP", &vsp);
        fVspMapper = new BmnVspRaw2Digit(fPeriodId, run4init, fVspMapFileName, fBmnSetup);
    }

    if (fDetectorSetup.count(kTOF1) > 0 && fDetectorSetup.at(kTOF1) == 1) {
        LOGF(info, "----------   Init TOF400 mapper   ----------");
        tof400 = new TClonesArray("BmnTof1Digit");
        Register_Fun("TOF400", &tof400);
        fTof400Mapper = new BmnTof1Raw2Digit();
        fTof400Mapper->setVerbose(fVerbose);
        Bool_t FlagTemp = fTof400Mapper->setRun(fPeriodId, run4init);
        if (FlagTemp == kFALSE) {
            if (fTof400PlaceMapFileName.Sizeof() > 1 && fTof400StripMapFileName.Sizeof() > 1) {
                TString dir = Form("%s%s", getenv("VMCWORKDIR"), "/input/");
                FlagTemp = fTof400Mapper->setMapFromFile(dir + fTof400PlaceMapFileName.Data(),
                                                         dir + fTof400StripMapFileName.Data());
            }
        }
        LOGF(info, "----------   Initialization of TOF400 mapper finished    ----------");
    }

    if (fDetectorSetup.count(kTOF701) > 0 && fDetectorSetup.at(kTOF701) == 1) {
        LOGF(info, "----------   Init TOF701 mapper   ----------");
        tof701 = new TClonesArray("BmnTof701Digit");
        Register_Fun("TOF701", &tof701);
        fTof701Mapper = new BmnTof701Raw2Digit();
        fTof701Mapper->setVerbose(fVerbose);
        Bool_t FlagTemp = fTof701Mapper->setRun(fPeriodId, run4init);
        if (FlagTemp == kFALSE) {
            if (fTof701PlaceMapFileName.Sizeof() > 1 && fTof701StripMapFileName.Sizeof() > 1) {
                TString dir = Form("%s%s", getenv("VMCWORKDIR"), "/input/");
                FlagTemp = fTof701Mapper->setMapFromFile(dir + fTof701PlaceMapFileName.Data(),
                                                         dir + fTof701StripMapFileName.Data());
                fTof701Mapper->setINLFromFile(Form("%s%s", dir.Data(), "TOF701_ListINLFiles_Run8.txt"));
            }
        }
        LOGF(info, "----------   Initialization of TOF701 mapper finished    ----------");
    }

    // Legacy
    if (fDetectorSetup.count(kTOF) > 0 && fDetectorSetup.at(kTOF) == 1) {
        LOGF(info, "----------   Init TOF700 mapper   ----------");
        if (fTOF700ReferenceRun <= 0) {
            UniDetectorParameter* pDetectorParameter = UniDetectorParameter::GetDetectorParameter(
                "TOF2", "slewing_file_id", fPeriodId,
                run4init);   //(detector_name, parameter_name, period_number, run_number)
            if (pDetectorParameter) {
                IntValue* pReferenceRun = (IntValue*)pDetectorParameter->GetValue();
                fTOF700ReferenceRun = pReferenceRun->value;
                delete pDetectorParameter;
            } else {
                LOGF(error, "Not found slewing run ID for run %d in DB", run4init);
            }
        }
        tof700 = new TClonesArray("BmnTof2Digit");
        Register_Fun("TOF700", &tof700);
        fTof700Mapper = new BmnTof2Raw2DigitNew(fTof700MapFileName, fRootFileName, fTOF700ReferenceRun,
                                                fTOF700ReferenceChamber, fTof700GeomFileName);
        for (int i = 0; i < 60; i++) {
            if (type_tof700_slewing[i]) {
                fTof700Mapper->SetSlewingReference(i + 1, refrun_tof700_slewing[i], refchamber_tof700_slewing[i]);
            }
        }
        //        fTof700Mapper->readSlewingT0();
        //        fTof700Mapper->readSlewingLimits();
        fTof700Mapper->readSlewing();
        fTof700Mapper->BookSlewingResults();
        LOGF(info, "----------   Initialization of TOF700 mapper finished    ----------");
    }

    if (fDetectorSetup.count(kDCH) > 0 && fDetectorSetup.at(kDCH) == 1) {
        dch = new TClonesArray("BmnDchDigit");
        Register_Fun("DCH", &dch);
        fDchMapper = new BmnDchRaw2Digit(fPeriodId, run4init);
    }

    if (fDetectorSetup.count(kZDC) > 0 && fDetectorSetup.at(kZDC) == 1) {
        zdc = new TClonesArray("BmnZDCDigit");
        Register_Fun("ZdcDigit", &zdc);
        fZDCMapper = new BmnZDCRaw2Digit(fPeriodId, run4init, fZDCMapFileName, fZDCCalibrationFileName);
    }

    if (fDetectorSetup.count(kECAL) > 0 && fDetectorSetup.at(kECAL) == 1) {
        ecal = new TClonesArray("BmnECALDigit");
        Register_Fun("EcalDigit", &ecal);
        fECALMapper = new BmnECALRaw2Digit(run4init);
    }

    if (fDetectorSetup.count(kSCWALL) > 0 && fDetectorSetup.at(kSCWALL) == 1) {
        LOG(info) << "scwall in setup";
        scwall = new TClonesArray("BmnScWallDigi");
        Register_Fun("ScWallDigi", &scwall);
        fScWallMapper = new BmnScWallRaw2Digit(fPeriodId, run4init, fScWallMapFileName, fScWallCalibrationFileName);
        fScWallMapper->print();
    }

    if (fDetectorSetup.count(kFHCAL) > 0 && fDetectorSetup.at(kFHCAL) == 1) {
        LOG(info) << "fhcal in setup";
        fhcal = new TClonesArray("BmnFHCalDigi");
        Register_Fun("FHCalDigi", &fhcal);
        fFHCalMapper = new BmnFHCalRaw2Digit(fPeriodId, run4init, fFHCalMapFileName, fFHCalCalibrationFileName);
        fFHCalMapper->print();
    }

    if (fDetectorSetup.count(kHODO) > 0 && fDetectorSetup.at(kHODO) == 1) {
        LOG(info) << "hodo in setup";
        hodo = new TClonesArray("BmnHodoDigi");
        Register_Fun("HodoDigi", &hodo);
        fHodoMapper =
            new BmnHodoRaw2Digit(fPeriodId, run4init, fHodoMapFileName.Data(), fHodoCalibrationFileName.Data());
        fHodoMapper->print();
    }

    if (fDetectorSetup.count(kNDET) > 0 && fDetectorSetup.at(kNDET) == 1) {
        LOG(info) << "ndet in setup";
        ndet = new TClonesArray("BmnNdetDigi");
        Register_Fun("NdetDigi", &ndet);
        fNdetMapper = new BmnNdetRaw2Digit(fPeriodId, run4init, fNdetMapFileName, fNdetCalibrationFileName);
        fNdetMapper->print();
    }

    if (fDetectorSetup.count(kHGND) > 0 && fDetectorSetup.at(kHGND) == 1) {
        LOG(info) << "hgnd in setup";
        fHgndMapper = new BmnHgndRaw2Digit(fPeriodId, fRunId, fHgndMapFileName.Data(), fHgndCalibrationFileName.Data());
        fHgndMapper->print();
    }

    if (fDetectorSetup.count(kCSC) > 0 && fDetectorSetup.at(kCSC) == 1) {
        csc = new TClonesArray("BmnCSCDigit");
        Register_Fun("CSC", &csc);
        fCscMapper = new BmnCscRaw2Digit(fPeriodId, run4init, fCscMapFileName, fBmnSetup);
#ifdef ADC_QA
        fCscQa = new BmnAdcQA(fPeriodId, run4init, "CSC");
#endif
    }
    return;
}

BmnStatus BmnDecoder::LoadCalibFile(TString& FileName)
{
    TDirectory* temp_dir = gDirectory;
    fCalibFile = new TFile(FileName, "READ");
    if (fCalibFile)
        if (fCalibFile->IsOpen()) {
            if (fSiliconMapper)
                fSiliconMapper->LoadFilterInfo(fCalibFile);
            if (fGemMapper)
                fGemMapper->LoadFilterInfo(fCalibFile);
            if (fCscMapper)
                fCscMapper->LoadFilterInfo(fCalibFile);
            if (fSiBTMapper)
                fSiBTMapper->LoadFilterInfo(fCalibFile);
            //            if (fVspMapper)
            //                fVspMapper->LoadFilterInfo(fCalibFile);
            fCalibFile->Close();
            fPedEnough = kTRUE;
            fNoiseEnough = kTRUE;
            LOGF(info, "ADC calibration file loaded!");
        }
    gDirectory = temp_dir;
    return kBMNSUCCESS;
}

BmnStatus BmnDecoder::SaveCalibFile(TString& FileName)
{
    fCalibFile = new TFile(FileName, "RECREATE");
    if (fCalibFile) {
        if (fSiliconMapper)
            fSiliconMapper->SaveFilterInfo(fCalibFile);
        if (fGemMapper)
            fGemMapper->SaveFilterInfo(fCalibFile);
        if (fCscMapper)
            fCscMapper->SaveFilterInfo(fCalibFile);
        if (fSiBTMapper)
            fSiBTMapper->SaveFilterInfo(fCalibFile);
        //        if (fVspMapper)
        //            fVspMapper->SaveFilterInfo(fCalibFile);
        fCalibFile->Close();
        LOGF(info, "ADC calibration file saved!");
    }
    return kBMNSUCCESS;
}

void BmnDecoder::ClearDigiArrays()
{
    if (dch)
        dch->Delete();
    if (mwpc)
        mwpc->Delete();
    if (gem)
        gem->Delete();
    if (csc)
        csc->Delete();
    if (vsp)
        vsp->Delete();
    if (sibt)
        sibt->Delete();
    if (silicon)
        silicon->Delete();
    if (tof400)
        tof400->Delete();
    if (tof700)
        tof700->Delete();
    if (tof701)
        tof701->Delete();
    if (zdc)
        zdc->Delete();
    if (scwall)
        scwall->Delete();
    if (fhcal)
        fhcal->Delete();
    if (hodo)
        hodo->Delete();
    if (ndet)
        ndet->Delete();
    if (ecal)
        ecal->Delete();
    if (fTrigMapper)
        fTrigMapper->ClearArrays();
    fTimeShifts.clear();
    eventHeader->Clear();
}

void BmnDecoder::ClearSpillDigiArrays()
{
    if (spillHeader)
        spillHeader->Clear();
    if (msc_copy)
        msc_copy->Delete();
    if (t0_copy)
        t0_copy->Delete();
    if (hgnd)
        hgnd->Delete();
}

void BmnDecoder::RecalculatePedestals()
{
    LOGF(info, "Starting to correct pedestal events");
    if (fGemMapper)
        fGemMapper->RecalculatePedestalsAugmented();
    if (fCscMapper)
        fCscMapper->RecalculatePedestalsAugmented();
    if (fSiliconMapper)
        fSiliconMapper->RecalculatePedestalsAugmented();
    if (fSiBTMapper)
        fSiBTMapper->RecalculatePedestalsAugmented();
    //    if (fVspMapper)
    //        fVspMapper->RecalculatePedestalsAugmented();
    fPedEvCntr = 0;
    fPedEnough = kTRUE;
    LOGF(info, "Finished correcting pedestal events");
}

void BmnDecoder::FillAdcQa()
{
    if (fFsdQa) {
        UInt_t serial = 0x0AADD6E9;
        Int_t i_serial = fSiliconMapper->GetSerialMap()[serial];
        Int_t ch = 10;
        Int_t smpl = 3;
        fFsdQa->SetAveragePedestal(serial, ch, smpl, fSpillCntr, fSiliconMapper->GetPedestal(i_serial, ch, smpl));
        serial = 0x04E993A5;
        i_serial = fSiliconMapper->GetSerialMap()[serial];
        ch = 8;
        smpl = 5;
        fFsdQa->SetAveragePedestal(serial, ch, smpl, fSpillCntr, fSiliconMapper->GetPedestal(i_serial, ch, smpl));
    }
    if (fGemQa) {
        UInt_t serial = 0x0aad5333;
        Int_t i_serial = fGemMapper->GetSerialMap()[serial];
        Int_t ch = 47;
        Int_t smpl = 25;
        fGemQa->SetAveragePedestal(serial, ch, smpl, fSpillCntr, fGemMapper->GetPedestal(i_serial, ch, smpl));
    }
    if (fCscQa) {
        UInt_t serial = 0x0A7B2235;
        Int_t i_serial = fCscMapper->GetSerialMap()[serial];
        Int_t ch = 7;
        Int_t smpl = 14;
        fCscQa->SetAveragePedestal(serial, ch, smpl, fSpillCntr, fCscMapper->GetPedestal(i_serial, ch, smpl));
    }
}

void BmnDecoder::SaveAdcQa()
{
    if (fFsdQa)
        fFsdQa->SaveHists();
    if (fGemQa)
        fGemQa->SaveHists();
    if (fCscQa)
        fCscQa->SaveHists();
}

void BmnDecoder::FillProfiles()
{
    if (fGemMapper)
        fGemMapper->FillProfiles(adc32);
    if (fCscMapper)
        fCscMapper->FillProfiles(adc32);
    if (fSiliconMapper)
        fSiliconMapper->FillProfiles(adc128);
    //    if (fVspMapper)
    //        fVspMapper->FillProfiles(vsp_raw);
    // if (fSiBTMapper) fSiBTMapper->FillProfiles(adc64);
}

void BmnDecoder::FillNoisyChannels()
{
    LOGF(info, "Marking noisy channels from signal events");
    if (fGemMapper)
        fGemMapper->FillNoisyChannels();
    if (fCscMapper) {
        //        BmnRawDataDecoder::samplesCsc = 32;
        //        BmnRawDataDecoder::threshCsc = 2;
        fCscMapper->FillNoisyChannels();
    }
    //    if (fVspMapper) {
    //        //        BmnRawDataDecoder::samplesCsc = 32;
    //        //        BmnRawDataDecoder::threshCsc = 2;
    //        fVspMapper->FillNoisyChannels();
    //    }
    if (fSiliconMapper) {
        // extremely high picks
        BmnRawDataDecoder::samplesSil = 5;
        BmnRawDataDecoder::threshSil = 8;
        BmnRawDataDecoder::correctionSil = 20;
        fSiliconMapper->FillNoisyChannels();

        // low groups of noisy channels
        BmnRawDataDecoder::samplesSil = 20;
        BmnRawDataDecoder::threshSil = 2;
        BmnRawDataDecoder::correctionSil = 20;

        fSiliconMapper->FillNoisyChannels();
    }
    // if (fSiBTMapper) fSiBTMapper->FillNoisyChannels();
}

BmnStatus BmnDecoder::DecodeDataToDigiIterate()
{
    ClearDigiArrays();
    fCurEventType = eventHeaderDAQ->GetEventType();
    fEventId = eventHeaderDAQ->GetEventId();
    //    LOGF(info, "iEv %6u  fPedEnough %d, fNoiseEnough %d", fEventId, fPedEnough, fNoiseEnough);
    FillTimeShiftsMap();
    if ((fCurEventType == kBMNPAYLOAD) || (fCurEventType == kBMNPEDESTAL))
        fNTotalEvents++;
    if ((fCurEventType == kBMNPAYLOAD))
        fNSignalEvents++;

    if (fVerbose > 0) {
        if ((fNSignalEvents % 5000 == 0) && ((fCurEventType == kBMNPAYLOAD) || (fCurEventType == kBMNPEDESTAL)))
            LOG(info) << "Digitization: " << fNSignalEvents << "/" << fNTotalEvents << "/" << fMaxEvent
                      << " processed; Spill #" << fSpillCntr;
    }
    //        printf("fCurEventType %d\n", fCurEventType);
    fT0Time = 0.;
    if (fTrigMapper) {
        fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc, fTimeShifts);
        fTrigMapper->FillEvent(tdc, fTimeShifts);
        fT0Time = 0.0;
        fT0Width = 0.0;
        GetT0Info(fT0Time, fT0Width);
    }
    if ((fCurEventType == kBMNPEDESTAL)) {
        //        if ((Int_t)fPedEvCntr == fEvForPedestals - 1)
        //            return kBMNERROR;   // FIX return!
        CopyDataToPedMap(fPedEvCntr++);
    } else {   // payload
        if (fPrevEventType == kBMNPEDESTAL) {
            if (fPedEvCntr >= fEvForPedestals - 1) {
                RecalculatePedestals();
                fPedEvCntr = 0;
                fPedEnough = true;
            }
        }
        if (fPedEnough) {
            if (fNoiseEnough) {
                if (fGemMapper)
                    fGemMapper->FillEvent(adc32, gem);
                if (fCscMapper)
                    fCscMapper->FillEvent(adc32, csc);
                if (fSiliconMapper)
                    fSiliconMapper->FillEvent(adc128, silicon);
                if (fSiBTMapper)
                    fSiBTMapper->FillEvent(adc64, sibt);
            } else {
                if (fCurEventType == kBMNPAYLOAD) {
                    fNoiseEvCntr++;
                    if (fVerbose > 0) {
                        if (fNoiseEvCntr % 1000 == 0)
                            LOG(info) << "Profile event #" << fNoiseEvCntr << "/" << fEvForNoiseCorr << ";";
                    }
                    FillProfiles();
                    if (fNoiseEvCntr >= fEvForNoiseCorr) {
                        LOGF(info, ANSI_COLOR_BLUE " Marking noisy channels" ANSI_COLOR_RESET);
                        FillNoisyChannels();
                        RecalculatePedestals();
                        fNoiseEnough = true;
                    }
                }
            }
        }
        if (fVspMapper)
            fVspMapper->FillEvent(vsp_raw, vsp);
        if (fDchMapper)
            fDchMapper->FillEvent(tdc, &fTimeShifts, dch, fT0Time);
        if (fMwpcMapper)
            fMwpcMapper->FillEvent(hrb, mwpc);
        if (fTof400Mapper)
            fTof400Mapper->FillEvent(tdc, &fTimeShifts, tof400);
        // Legacy
        //        if (fTof700Mapper && fT0Time != 0. && fT0Width != 0.) fTof700Mapper->fillEvent(tdc, &fTimeShifts,
        //        fT0Time, fT0Width, tof700);
        // if (fTof700Mapper)
        //     fTof700Mapper->fillEvent(tdc, &fTimeShifts, fT0Time, fT0Width, tof700);
        if (fTof701Mapper)
            fTof701Mapper->FillEvent(tdc, &fTimeShifts, tof701);
        if (fZDCMapper)
            fZDCMapper->fillEvent(adc, zdc);
        if (fScWallMapper)
            fScWallMapper->fillEvent(adc, scwall);
        if (fFHCalMapper)
            fFHCalMapper->fillEvent(adc, fhcal);
        if (fHodoMapper)
            fHodoMapper->fillEvent(tqdc_tdc, tqdc_adc, hodo);
        if (fNdetMapper)
            fNdetMapper->fillEvent(tqdc_tdc, tqdc_adc, &fTimeShifts, ndet);
        if (fHgndMapper)
            fHgndMapper->fillEvent(tdc_hgnd, fT0SyncTime, hgnd);
        if (fECALMapper)
            fECALMapper->fillEvent(adc, ecal);
    }
    fRunEndTime = TTimeStamp(time_t(fTime_s), fTime_ns);
    eventHeader->SetRunId(eventHeaderDAQ->GetRunId());
    eventHeader->SetEventId(eventHeaderDAQ->GetEventId());
    eventHeader->SetPeriodId(eventHeaderDAQ->GetPeriodId());
    eventHeader->SetEventTimeTS(fRunEndTime);
    eventHeader->SetEventTime(fRunEndTime.AsDouble());
    eventHeader->SetEventType(fCurEventType);
    eventHeader->SetTripWord(kFALSE);
    eventHeader->SetTrigInfo(eventHeaderDAQ->GetTrigInfo());
    eventHeader->SetTimeShift(fTimeShifts);
    eventHeader->SetStartSignalInfo(fT0Time, fT0Width);
    eventHeader->SetSpillStart(eventHeaderDAQ->GetSpillStart());
    eventHeader->SetInputSignalsAR(eventHeaderDAQ->GetInputSignalsAR());
    eventHeader->SetInputSignalsBR(eventHeaderDAQ->GetInputSignalsBR());
    eventHeader->SetInputSignalsVector(move(eventHeaderDAQ->GetInputSignalsVector()));
    eventHeader->SetSpillId(eventHeaderDAQ->GetSpillId());
    eventHeader->SetSpillStartTS(eventHeaderDAQ->GetSpillStartTS());
    //        fDigiTree->Fill();
    fPrevEventType = fCurEventType;
    msc_copy->AbsorbObjects(msc);
    t0_copy->AbsorbObjects(t0raw);

    return kBMNSUCCESS;
}

BmnStatus BmnDecoder::Finalize()
{
    if (fUseCalibFile && fNoiseEnough)
        SaveCalibFile(fCalibFileName);
    return kBMNSUCCESS;
}

BmnStatus BmnDecoder::FillTimeShiftsMap()
{
    if (fT0Serial == 0)
        return kBMNERROR;
    fT0SyncTime = 0;
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*)sync->At(i);
        //        if (fTrigMapper)
        //        if (syncDig->GetSerial() == fTrigMapper->GetTrcSerial()) {
        //            LOGF(debug, "TRC sync: %s", TTimeStamp(time_t(fTime_s), fTime_ns).AsString());
        //        break;
        //        }
        if (syncDig->GetSerial() == fT0Serial) {
            fT0SyncTime = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
            fTime_s = syncDig->GetTime_sec();
            fTime_ns = syncDig->GetTime_ns();
            // printf("serial %08X sync: %s\n", syncDig->GetSerial(), TTimeStamp(time_t(fTime_s), fTime_ns).AsString());
            if (fRunStartTime == TimeZero) {
                fRunStartTime = TTimeStamp(time_t(fTime_s), fTime_ns);
            }
            LOGF(debug, "T0  sync: %s", TTimeStamp(time_t(fTime_s), fTime_ns).AsString());
            break;
        }
    }
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*)sync->At(i);
        Long64_t syncTime = (fT0SyncTime == 0.0) ? 0 : syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
        fTimeShifts.insert(pair<UInt_t, Long64_t>(syncDig->GetSerial(), syncTime - fT0SyncTime));
    }
    return kBMNSUCCESS;
}

void BmnDecoder::CopyDataToPedMap(UInt_t ev)
{
    if (ev < fEvForPedestals) {
        if (fVerbose > 0)
            if (fPedEvCntr % 100 == 0 && fPedEvCntr > 0)
                cout << "Pedestal event #" << fPedEvCntr << "/" << fEvForPedestals << ";" << endl;
        if (fGemMapper)
            fGemMapper->CopyData2PedMap(adc32, ev);
        if (fSiliconMapper)
            fSiliconMapper->CopyData2PedMap(adc128, ev);
        if (fSiBTMapper)
            fSiBTMapper->CopyData2PedMap(adc64, ev);
        if (fCscMapper)
            fCscMapper->CopyData2PedMap(adc32, ev);
        //        if (fVspMapper)
        //            fVspMapper->CopyData2PedMap(vsp_raw, vsp);
    }
}

// Legacy
void BmnDecoder::SetTof700SlewingReference(Int_t chamber, Int_t refrun, Int_t refchamber)
{
    if (chamber <= 0 || chamber > 60) {
        std::printf("Wrong slewing chamber number %d\n", chamber);
        return;
    }
    if (refchamber <= 0 || refchamber > 60) {
        std::printf("Wrong slewing reference chamber number %d\n", refchamber);
        return;
    }
    if (refrun < 0 || refrun > 9999) {
        std::printf("Wrong slewing reference run number %d\n", refrun);
        return;
    }
    refrun_tof700_slewing[chamber - 1] = refrun;
    refchamber_tof700_slewing[chamber - 1] = refchamber;
    type_tof700_slewing[chamber - 1] = 1;
    return;
}

Int_t BmnDecoder::GetRunIdFromFile(TString name)
{
    fRunId = 0;
    TPRegexp re(".+\\.root");
    TPRegexp re_data(".*\\w+_\\w+_\\w+_(\\d+)([^/]*)\\.data");
    if (re.MatchB(name)) {
        fRootFileIn = new TFile(name, "READ");
        if (fRootFileIn->IsOpen()) {
            BmnEventHeader* tempHdr = nullptr;
            TTree* RawTree = (TTree*)fRootFileIn->Get("BMN_RAW");
            if (RawTree) {
                RawTree->SetBranchAddress("BmnEventHeader.", &tempHdr);
                if (RawTree->GetEntriesFast()) {
                    RawTree->GetEntry(0);
                    fRunId = tempHdr->GetRunId();
                    fRootFileName = name;
                }
                delete RawTree;
            }
            delete tempHdr;
            fRootFileIn->Close();
        }
        delete fRootFileIn;
    }
    if (fRunId > 0)
        return fRunId;
    if (fRunId <= 0) {
        // Int_t run = 0;
        // sscanf(&(((char *)name.Data())[strlen(name.Data())-9]), "%d", &run);
        re_data.Substitute(name, "$1");

        // cout << name << endl;

        return name.Atoi();

    } else
        return fRunId;
}

BmnStatus BmnDecoder::GetT0Info(Double_t& t0time, Double_t& t0width)
{
    vector<TClonesArray*>* trigArr = fTrigMapper->GetTrigArrays();
    BmnTrigDigit* dig = 0;
    for (auto ar : *trigArr) {
        if ((fPeriodId == 8) && (fBmnSetup == kSRCSETUP)) {   // temporary crutches
            if ((fRunId >= 986) && (fRunId <= 1086)) {
                if ((strcmp(ar->GetName(), "BC2_1_A")))
                    continue;
            } else {
                if ((strcmp(ar->GetName(), "T0_1_A")))
                    continue;
            }
        } else {
            if (fPeriodId > 6) {
                if (fPeriodId == 8) {
                    if (strcmp(ar->GetName(), "BC2AT"))
                        continue;
                } else {
                    if (strcmp(ar->GetName(), "BC2"))
                        continue;
                }
            } else {
                if (strcmp(ar->GetName(), "T0"))
                    continue;
            }
        }
        for (int i = 0; i < ar->GetEntriesFast(); i++) {
            dig = (BmnTrigDigit*)ar->At(i);
            if (dig->GetMod() == 0) {
                t0time = dig->GetTime();
                t0width = dig->GetAmp();
                //		printf(" t0 %f t0w %f n %d\n", t0time, t0width, ar->GetEntriesFast());
                return kBMNSUCCESS;
            }
        }
    }
    return kBMNERROR;
}

TString BmnDecoder::GetSubNameAfterRunId(TString name)
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

BmnStatus BmnDecoder::LoadConfig()
{
    LOGF(debug, "%s", __func__);
    if (fDecoderConfigFileName.length() == 0)
        fDecoderConfigFileName = string(getenv("VMCWORKDIR")) + "/config/bmnconf.json";
    pt::read_json(fDecoderConfigFileName, conf);
    fExportJsonBocks = conf.get<bool>("Decoder.ExportJson", false);
    fEvForPedestals = conf.get<Int_t>("Decoder.NEventsForPedestals", N_EV_FOR_PEDESTALS);
    fEvForNoiseCorr = conf.get<Int_t>("Decoder.NEventsForNoiseCorr", 20000);
    return kBMNSUCCESS;
}

void* BmnDecoder::GetMapper(DetectorId id)
{
    switch (id) {
        case DetectorId::kBC:
            return fTrigMapper;
        case DetectorId::kCSC:
            return fCscMapper;
        case DetectorId::kFHCAL:
            return fFHCalMapper;
        case DetectorId::kGEM:
            return fGemMapper;
        case DetectorId::kHGND:
            return fHgndMapper;
        case DetectorId::kHODO:
            return fHodoMapper;
        case DetectorId::kNDET:
            return fNdetMapper;
        case DetectorId::kSCWALL:
            return fScWallMapper;
        case DetectorId::kSILICON:
            return fSiliconMapper;
        case DetectorId::kSiBT:
            return fSiBTMapper;
        case DetectorId::kTOF1:
            return fTof400Mapper;
        case DetectorId::kTOF701:
            return fTof701Mapper;
        case DetectorId::kVSP:
            return fVspMapper;
        default:
            LOGF(error, "Unsupported DetectorID");
            return nullptr;
    }
}
