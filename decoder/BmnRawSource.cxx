#include "BmnRawSource.h"
// System
#include <arpa/inet.h> /* For ntohl for Big Endian LAND. */
// C++
#include <chrono>
#include <iostream>
// ROOT
#include "TStopwatch.h"
#include "TSystem.h"
// FairRoot
#include "FairLogger.h"
// BmnRoot
#include "BmnADCDigit.h"
#include "BmnAbstractTDCDigit.h"
#include "BmnConverterTools.h"
#include "BmnGemStripDigit.h"
#include "BmnHRBDigit.h"
#include "BmnHgndRaw.h"
#include "BmnMSCDigit.h"
#include "BmnMSCZSDigit.h"
#include "BmnSyncDigit.h"
#include "BmnTDCDigit.h"
#include "BmnTQDCADCDigit.h"
#include "BmnVspRawDigit.h"
#include "TangoData.h"
#include "UniDetectorParameter.h"
#include "UniRawFile.h"
#include "UniRun.h"

#include <BmnFileProp.h>
// CBM
#include "StorableTimeslice.h"
#include "StsXyterMessage.h"

// Double_t realtime = 0.0;
// Double_t converterTime = 0.0;
using namespace std::chrono;

BmnRawSource::BmnRawSource(TString file, TString outfile, ULong_t nEvents, ULong_t period, bool save)
    : isSpillStart(kFALSE)
    , isTaskMode(kFALSE)
    , fSpillCntr(0)
    , isRawRootInputFile(kFALSE)
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
    , fRootFileName(outfile)
    , fRawFileName(file)
    , fRawRunHdrName("RawRunHeader")
    , fMetadataName("RawRunMetadata")
    , fRootFileIn(nullptr)
    , fRootFileOut(nullptr)
    , fRawFileIn(nullptr)
    , sync(nullptr)
    , adc32(nullptr)
    , adc64(nullptr)
    , adc128(nullptr)
    , adc(nullptr)
    , vsp(nullptr)
    , hrb(nullptr)
    , tdc(nullptr)
    , tqdc_tdc(nullptr)
    , tqdc_adc(nullptr)
    , tdc_hgnd(nullptr)
    , msc(nullptr)
    , eventHeaderDAQ(nullptr)
    , spillHeader(nullptr)
    , runHeader(make_unique<DigiRunHeader>())
    , metadata(make_unique<BmnMetadataRaw>())
    , fMaxEvent(nEvents)
    , fDat(0)
    , syncCounter(0)
    , nSpillEvents(0)
    , fExportJsonBlocks(false)
    , fExportExternalSpillStat(false)
    , tai_utc_dif(0)
    , fVerbose(0)
    , fSaveOutput(save)
{
    LOGF(debug1, "BmnRawSource: input %s, output %s", fRawFileName.Data(), fRootFileName.Data());

    if (fRawFileName != "") {
        BmnFileProp prop;
        BmnConverterTools::ParseRawFileName(fRawFileName, prop);
        fRunId = (prop.GetRunId() > 0) ? prop.GetRunId() : GetRunIdFromFile(fRawFileName);
        TString subName = BmnConverterTools::GetSubNameAfterRunId(file);
        LOG(debug) << "subname " << subName;
        LOG(info) << "RunId " << fRunId << " got from " << (isRawRootInputFile ? "RawRoot" : "RawData") << " file";
        if (fRootFileName.Length()) {

        } else {
            fRootFileName = Form("bmn_run%d%s_raw.root", fRunId, subName.Data());
        }
        fSubName = subName;
    }
    InitUTCShift();
}

BmnRawSource::~BmnRawSource() {}

bool BmnRawSource::Init()
{
    printf(ANSI_COLOR_RED "\n================ CONVERTING ================\n" ANSI_COLOR_RESET);
    isTaskMode = true;
    LoadConfig();
    fRawFileIn = fopen(fRawFileName, "rb");
    if (fRawFileIn == nullptr) {
        printf("\n!!!!!\ncannot open file %s\nConvertRawToRoot stopped\n!!!!!\n\n", fRawFileName.Data());
        return false;
    }
    fseeko64(fRawFileIn, 0, SEEK_END);
    fLengthRawFile = ftello64(fRawFileIn);
    rewind(fRawFileIn);
    printf("\nRAW FILE: ");
    printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET, fRawFileName.Data());
    printf("\nRAW FILE LENGTH: ");
    printf(ANSI_COLOR_BLUE "%.3f MB\n" ANSI_COLOR_RESET, fLengthRawFile / 1024. / 1024.);
    isSpillStart = kTRUE;
    // FairRootManager* frm = FairRootManager::Instance();
    /*auto reg_fun = [&] (TString name, TObject ** ar) -> void
    {
        TClass *cl = TClass::GetClass((*ar)->ClassName());
        if (cl == TClonesArray::Class())
            frm->Register(name, name + "_dir", static_cast<TClonesArray*> ((*ar)), fSaveOutput);
        else
            frm->Register(name, name + "_dir", static_cast<TNamed*> ((*ar)), fSaveOutput);
        LOG(debug1) << "Register by class " << cl->GetName() << " : " << name;
        return;
    };*/
    RegisterBranches();   // reg_fun);
    bool convertMSC = conf.get<bool>("Decoder.ProcessMSC", false);
    if (convertMSC) {
        fRawTreeSpills = new TTree("BMN_RAW_SPILLS", "BMN_RAW_SPILLS");
        //        fRawTreeSpillsT0 = new TTree("BMN_RAW_SPILLS_T0", "BMN_RAW_SPILLS_T0");
        auto spill_reg_fun = [&](TTree* tree, TString name, TObject* ar) -> void {
            TBranch* b = tree->Branch(name, &ar);
            LOGF(debug1, "Register branch %p : %s", (void*)b, name.Data());
            return;
        };
        RegisterSpillBranches(spill_reg_fun);
    }
    return true;
}

BmnStatus BmnRawSource::InitConverter(TString FileName)
{
    printf(ANSI_COLOR_RED "\n================ CONVERTING ================\n" ANSI_COLOR_RESET);
    fRawFileName = FileName;
    fRawFileIn = fopen(fRawFileName, "rb");
    if (fRawFileIn == nullptr) {
        LOGF(error, "\n!!!!!\ncannot open file %s\nConvertRawToRoot are stopped\n!!!!!\n", fRawFileName.Data());
        return kBMNERROR;
    }
    fseeko64(fRawFileIn, 0, SEEK_END);
    fLengthRawFile = ftello64(fRawFileIn);
    rewind(fRawFileIn);
    printf("\nRAW FILE: ");
    printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET, fRawFileName.Data());
    printf("\nRAW FILE LENGTH: ");
    printf(ANSI_COLOR_BLUE "%.3f MB\n" ANSI_COLOR_RESET, fLengthRawFile / 1024. / 1024.);
    //    if (!isTaskMode) {
    //    fRootFileOut = TFile::Open(fRootFileName, "recreate");
    fRootFileOut = new TFile(fRootFileName, "recreate");
    isSpillStart = true;
    if (!fRootFileOut->IsOpen()) {
        delete fRootFileOut;
        return kBMNERROR;
    }
    return InitConverter();
}

BmnStatus BmnRawSource::InitConverter()
{
    fRawTree = new TTree("BMN_RAW", "BMN_RAW");
    LOGF(info, "ROOT FILE: " ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET, fRootFileName.Data());
    LOGF(debug, "fRawTree %p  init", (void*)fRawTree);
    LOGF(debug, "fRawTree path %s", fRawTree->GetDirectory()->GetPath());
    RegisterBranches();
#ifdef MULTITHREAD
    int threadNumber = 0;
    if (threadNumber == 0) {
        if (std::thread::hardware_concurrency() <= 1)
            threadNumber = 1;   // either 1 core or no info
        else
            threadNumber = std::thread::hardware_concurrency() - 1;
    }
    fThreads = new BmnThreadManager();
    for (int i = 0; i < threadNumber; i++)
        ((BmnConverterThread*)(fThreads->Add()))->SetInitData(&leaps, utc_valid, tai_utc_dif, fVerbose);
#endif
    bool convertMSC = conf.get<bool>("Decoder.ProcessMSC", false);
    if (convertMSC) {
        fRawTreeSpills = new TTree("BMN_RAW_SPILLS", "BMN_RAW_SPILLS");
        //        fRawTreeSpillsT0 = new TTree("BMN_RAW_SPILLS_T0", "BMN_RAW_SPILLS_T0");
        auto spill_reg_fun = [&](TTree* tree, TString name, TObject* ar) -> void {
            TBranch* b = tree->Branch(name, &ar);
            LOGF(debug1, "Register branch %p : %s", (void*)b, name.Data());
            return;
        };
        RegisterSpillBranches(spill_reg_fun);
    }
    return kBMNSUCCESS;
}

void BmnRawSource::Close()
{
    LOGF(debug1, "%s ", __func__);
    if (fRawTreeSpills)
        fRawTreeSpills->Write();
    //    if (fRawTreeSpillsT0)
    //        fRawTreeSpillsT0->Write();
    fclose(fRawFileIn);
    delete eventHeaderDAQ;
    delete sync;
    delete adc32;
    delete adc64;
    delete adc128;
    delete adc;
    delete vsp;
    delete hrb;
    delete tdc;
    delete tqdc_tdc;
    delete tqdc_adc;
    delete tdc_hgnd;
    if (msc)
        delete msc;
}

void BmnRawSource::Reset() {}

Int_t BmnRawSource::ReadEvent(UInt_t i)
{
    BmnStatus convert_status = ConvertRawToRootIterateFileRead();
    //    printf("convert_status %d\n", convert_status);
    if (convert_status == kBMNSUCCESS) {
        return ((fCurEvType == kBMNPAYLOAD) || (fCurEvType == kBMNPEDESTAL)) ? 0 : 2;
    } else
        return 1;
}

void BmnRawSource::RegisterBranches()
{   // std::function<void(TString, TObject**) > branch_reg_fun) {
    eventHeaderDAQ = new BmnEventHeader();
    BranchRegFun("BmnEventHeader.", &eventHeaderDAQ);
    //    FairRootManager* frm = FairRootManager::Instance();
    //    frm->Register("BmnEventHeader.", "BmnEventHeader._dir", eventHeaderDAQ, fSaveOutput);
    sync = new TClonesArray(BmnSyncDigit::Class());
    BranchRegFun("SYNC", &sync);
    adc32 = new TClonesArray(BmnADCDigit::Class());
    BranchRegFun("ADC32", &adc32);
    adc64 = new TClonesArray(BmnADCDigit::Class());
    BranchRegFun("ADC64", &adc64);
    adc128 = new TClonesArray(BmnADCDigit::Class());
    BranchRegFun("ADC128", &adc128);
    adc = new TClonesArray(BmnADCDigit::Class());
    BranchRegFun("ADC", &adc);
    vsp = new TClonesArray(BmnVspRawDigit::Class());
    BranchRegFun("VSP_RAW", &vsp);
    tdc = new TClonesArray(BmnTDCDigit::Class());
    BranchRegFun("TDC", &tdc);
    tqdc_adc = new TClonesArray(BmnTQDCADCDigit::Class());
    BranchRegFun("TQDC_ADC", &tqdc_adc);
    tqdc_tdc = new TClonesArray(BmnTDCDigit::Class());
    BranchRegFun("TQDC_TDC", &tqdc_tdc);
    hrb = new TClonesArray(BmnHRBDigit::Class());
    BranchRegFun("HRB", &hrb);
}

void BmnRawSource::RegisterSpillBranches(std::function<void(TTree* tree, TString, TObject*)> branch_reg_fun)
{
    msc = new TClonesArray(fPeriodId > 8   ? BmnMSCZSDigit<UChar_t>::Class()
                           : fPeriodId > 7 ? BmnMSCDigit<UChar_t>::Class()
                                           : BmnMSCDigit<UInt_t>::Class());
    branch_reg_fun(fRawTreeSpills, "MSC", msc);
    t0raw = new TClonesArray(BmnT0Raw<kT0_BIN_BLOCK_WORDS>::Class());
    branch_reg_fun(fRawTreeSpills, "T0Raw", t0raw);
    tdc_hgnd = new TClonesArray(BmnAbstractTDCDigit::Class());
    branch_reg_fun(fRawTreeSpills, "TDC_HGND", tdc_hgnd);
    //    spillHeader = new BmnSpillHeader();
    //    GenRegFun("BmnSpillHeader.", spillHeader);
}

BmnStatus BmnRawSource::ParseJsonTLV(UInt_t* d, UInt_t& len)
{
    try {
        UInt_t idx = 0;
        idx++;   // skip evId (it is 0)
        DeviceHeader* dh = reinterpret_cast<DeviceHeader*>(d + idx);
        idx += sizeof(DeviceHeader) / kNBYTESINWORD;
        //        dh->Print();
        UInt_t str_len = len - idx * kNBYTESINWORD;
        string str(reinterpret_cast<const char*>(d + idx), str_len);
        json j = json::parse(str);
        //        cout << "json type : " << j.type_name() << endl;
        // Int_t key_cnt(0);
        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            //  std::cout << "["<< key_cnt <<"] : "<< it.key() << '\n';
            string jkey(it.key());
            //            printf("j[%d] : %s\n", key_cnt, jkey.c_str());
            if (!jkey.compare("status")) {
                auto& j_runtime = it.value()["runTime"];
                auto j_spill = j_runtime["spill"];
                Int_t phase = static_cast<Int_t>(j_spill["phase"]);
                // UInt_t curSpillTimeMs = j_spill["curSpillTimeMs"];
                auto wr = j_runtime["time"]["WR"];
                Int_t ns = static_cast<Int_t>(wr["ns"]);
                Int_t sec = static_cast<Int_t>(wr["sec"]);
                if (tai_utc_dif == 0)
                    tai_utc_dif = GetUTCShift(TTimeStamp(time_t(sec), ns));   // crutch
                sec -= tai_utc_dif;
                SpillStatus ss = BmnConverterTools::ParseJsonStatus(it.value(), tai_utc_dif);
                // skip invalid timestamps
                if (!ss.times_valid)
                    continue;
                //                if (ts_spill_start > eventHeaderDAQ->GetSpillStartTS()) {
                //                    eventHeaderDAQ->SetSpillStartTS(ts_spill_start); // @TODO: rewrite with basing on
                //                    spill map eventHeaderDAQ->GetSpillId()++; LOGF(debug, "spill_id %d",
                //                    eventHeaderDAQ->GetSpillId());
                //                }
                LOGF(info, "JSON status phase %d", phase);
                LOGF(info, "ts :\t%s start", BmnFunctionSet::TimePoint2String(ss.start_ts));
                if (phase == 0)
                    LOGF(info, "ts :\t%s  stop", BmnFunctionSet::TimePoint2String(ss.stop_ts));
                auto spill_it = fSpillMap.find(ss.start_ts);
                if (spill_it == fSpillMap.end()) {
                    bool inserted;
                    std::tie(spill_it, inserted) = fSpillMap.insert(
                        make_pair(ss.start_ts,
                                  BmnSpillInfo{//                        .jsons = vector<json>(1, it.value()),
                                               .start_ts = ss.start_ts,
                                               .stop_ts = ss.start_ts}));
                    LOGF(debug, "spill record inserted %d", inserted);
                }
                BmnSpillInfo& info = spill_it->second;
                //                info.jsons.push_back(it.value());
                if (info.stop_ts < ss.stop_ts)
                    info.stop_ts = ss.stop_ts;
                if (fExportJsonBlocks) {
                    std::ofstream outfile(Form("j_%s_%d_%9d_%09d_%02X_%08X_phase_%d_%s.json", jkey.c_str(), fRunId, sec,
                                               ns, dh->DeviceId, dh->Serial, phase, fSubName.Data()),
                                          std::ofstream::binary);
                    outfile << std::setw(4) << j << std::endl;
                    outfile.close();
                }
                metadata->SpillStatusVec().push_back(move(ss));
            } else {
                LOGF(info, "JSON config");
                if (!jkey.compare("config")) {
                    string start_DT = it.value()["meta"]["startDateTime"];
                    if (fExportJsonBlocks) {
                        std::ofstream outfile(Form("j_%s_%d_%s_%02X_%08X.json", jkey.c_str(), fRunId, start_DT.data(),
                                                   dh->DeviceId, dh->Serial),
                                              std::ofstream::binary);
                        outfile << std::setw(4) << j << std::endl;
                        outfile.close();
                    }
                    BmnStatus jconfigStatus =
                        BmnConverterTools::ParseJsonConfig(it.value(), runHeader->GetTrigConfig());
                    if (jconfigStatus == kBMNERROR)
                        LOGF(error, "Parsing JSON config failed!");
                } else
                    LOGF(info, "Unknown json key %s", jkey.data());
            }
        }
    } catch (std::exception& ex) {
        LOGF(error, "Exception for JSON block: %s", ex.what());
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::ConvertStatEvent(UInt_t* d, UInt_t& len)
{
    eventHeaderDAQ->SetEventType(fCurEvType);
    //        Int_t ml = msc->GetEntriesFast();
    ProcessEvent(d, len);
    //        if (msc->GetEntriesFast() == ml) {
    //            LOGF(info, "MSC not Filled!");
    //        }
    //    if (fRootFileOut) fRawTree->Fill();
    if (fPeriodId > 7)
        FinalizeStat();
    else if ((fCurEvType == kBMNEOS) && (fPrevEvType != kBMNEOS)) {
        LOGF(debug1, "\tspill! nSpillEvents %d", nSpillEvents);
        isSpillStart = kTRUE;
        nSpillEvents = 0;
        fSpillCntr++;
        //        if (msc->GetEntriesFast() > 0)
        if (fRawTreeSpills)
            FinalizeStat();
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::ConvertRawToRoot()
{
    if (isRawRootInputFile) {
        LOGF(info, "Input is raw root file. Nothing to do!");
        return kBMNSUCCESS;
    }
    if (InitConverter(fRawFileName) == kBMNERROR) {
        LOGF(error, "Init converter failed!");
        return kBMNERROR;
    }
    while ((ConvertRawToRootIterateFileRead() != kBMNERROR)) {
        if ((fStartEventId == 0) && (fEventId))
            fStartEventId = fEventId;
    }

    for (auto& it : fSpillMap) {
        BmnSpillInfo& si = it.second;
        LOGF(info, "\tstart %s\n\tstop  %s", BmnFunctionSet::TimePoint2String(si.start_ts),
             BmnFunctionSet::TimePoint2String(si.stop_ts));
    }

    fRunEndTime = TTimeStamp(time_t(fTime_s), fTime_ns);
    Int_t shift = GetUTCShift(fRunEndTime);
    if ((shift != tai_utc_dif)) {
        LOGF(info, "START (event %u):\t%s", fStartEventId, fRunStartTime.AsString());
        LOGF(info, "FINISH (event %u):\t%s", fEventId, fRunEndTime.AsString());
        LOG(info) << "Total Events:\t" << fNTotalEvents;
        LOG(info) << "nSignal Events:\t" << fNSignalEvents;
        //    Int_t shift = GetUTCShift(fRunEndTime);
        //    if ((shift != tai_utc_dif)) {
        //        LOGF(info, "start dif %d end dif %d", tai_utc_dif, shift);
        //        fprintf(stderr, ANSI_COLOR_RED "Critical Warning! Leap second added during the %i run!\n\n"
        //        ANSI_COLOR_RESET, fRunId);
    }
    //    fRunEndTime = TTimeStamp(time_t(fTime_s - shift), fTime_ns);
    //    fRawTree->Fill();

    fCurentPositionRawFile = ftello64(fRawFileIn);
    LOG(info) << "Read " << fNTotalEvents << " events; " << fCurentPositionRawFile << " bytes ("
              << fCurentPositionRawFile / 1024. / 1024. << " Mb)";
    fRawTree->Write();
#ifdef MULTITHREAD
    fThreads->Terminate();
#endif
    if (fRawTreeSpills)
        fRawTreeSpills->Write();
    //    if (fRawTreeSpillsT0)
    //        fRawTreeSpillsT0->Write();
    runHeader->SetSpillMap(move(fSpillMap));
    fRootFileOut->WriteObject(runHeader.get(), fRawRunHdrName.Data());
    fRootFileOut->WriteObject(metadata.get(), fMetadataName.Data());
    fRootFileOut->Close();
    fclose(fRawFileIn);
    delete sync;
    delete adc32;
    delete adc64;
    delete adc128;
    delete adc;
    delete vsp;
    delete hrb;
    delete tdc;
    delete tqdc_tdc;
    delete tqdc_adc;
    delete tdc_hgnd;
    delete msc;
    if (eventHeaderDAQ)
        delete eventHeaderDAQ;

    // KG: add raw file metadata to the Condition Database if not exists
    try {
        int period_id = (int)fPeriodId, run_id = (int)fRunId, start_event = (int)fStartEventId,
            end_event = (int)fEventId,
            event_count = fNSignalEvents;   // (int) fNevents;

        if (UniRawFile::CheckRawFileExists(period_id, run_id, start_event, end_event) == 0) {
            TDatime fFileStartDate(Int_t(fRunStartTime.GetDate(kFALSE)), Int_t(fRunStartTime.GetTime(kFALSE)));
            TDatime fFileEndDate(Int_t(fRunEndTime.GetDate(kFALSE)), Int_t(fRunEndTime.GetTime(kFALSE)));

            /*int error_write_db = false;
            UniRun* pRun = UniRun::GetRun(period_id, run_id);
            if (!pRun) {
                pRun = UniRun::CreateRun(period_id, run_id, 0, fFileStartDate, &fFileEndDate, "", nullptr, nullptr,
                                         nullptr, &event_count, nullptr);
                if (pRun)
                    cout << "New run has been successfully created in the Condition Database with number " << period_id
                         << ":" << run_id << endl;
                else
                    error_write_db = true;
            } else {
                TDatime fRunStartDate = pRun->GetStartDatetime();
                TDatime* fRunEndDate = pRun->GetEndDatetime();
                if (fFileStartDate.Convert() < fRunStartDate.Convert())
                    if (pRun->SetStartDatetime(fFileStartDate) < 0)
                        error_write_db = true;
                if ((!fRunEndDate) || ((fRunEndDate) && (fFileEndDate.Convert() > fRunEndDate->Convert())))
                    if (pRun->SetEndDatetime(&fFileEndDate) < 0)
                        error_write_db = true;
            }
            if (!error_write_db) {*/

            TString strFileHash = UniRawFile::CalculateFileHash(fRawFileName);
            TString* pFileHash = nullptr;
            if (strFileHash != "")
                pFileHash = new TString(strFileHash);
            else
                cout << "WARNING: a problem occurred while calculating checksum for the raw file: " << fRawFileName
                     << endl;

            UniRawFile* pRawFile =
                UniRawFile::CreateRawFile(period_id, run_id, start_event, end_event, fFileStartDate, fFileEndDate,
                                          fRawFileName, event_count, fLengthRawFile, pFileHash);
            if (pRawFile) {
                // int sum_event_count = UniRawFile::GetSumEventCount(period_id, run_id);
                // if (sum_event_count > (pRun->GetEventCount() ? (*pRun->GetEventCount()) : 0))
                // pRun->SetEventCount(&sum_event_count);
                cout << "Raw file information has been successfully added to the Condition Database for run "
                     << period_id << ":" << run_id << " and event interval " << start_event << "-" << end_event << endl;
                delete pRawFile;
            }

            if (pFileHash)
                delete pFileHash;
            // } // if (!error_write_db)
            // delete pRun;
        }   // if (UniRawFile::CheckRawFileExists
    } catch (std::exception& ex) {
        printf("Exception of working with the Condition Database: %s\n", ex.what());
    }

    LOG(info) << "Real time " << workTime_real << " s, CPU time " << workTime_cpu << " s for Converter\n";
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::wait_file(Int_t len, UInt_t limit)
{
    Long_t pos = ftello64(fRawFileIn);
    UInt_t t = 0;
    UInt_t dt = 1000000;
    while (fLengthRawFile < pos + len) {
        //        gSystem->ProcessEvents();
        if (t > limit)
            return kBMNERROR;
        usleep(dt);
        fseeko64(fRawFileIn, 0, SEEK_END);
        fLengthRawFile = ftello64(fRawFileIn);
        fseeko64(fRawFileIn, pos - fLengthRawFile, SEEK_CUR);
        t += dt;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::ConvertRawToRootIterate(UInt_t* buf, UInt_t len)
{
    fEventId = buf[0];
    //        printf("EventID = %d\n", fEventId);
    //    if (fEventId <= 0) return kBMNERROR;
    return ProcessEvent(buf, len);
}

BmnStatus BmnRawSource::ConvertRawToRootIterateFile(UInt_t limit)
{
    while (kTRUE) {
        //        if (wait_file(4 * kWORDSIZE, limit) == kBMNERROR) {
        //            return kBMNTIMEOUT;
        //            LOGF(error, "file timeout");
        //        }
        fCurentPositionRawFile = ftello64(fRawFileIn);
        BmnStatus cst = ConvertRawToRootIterateFileRead();
        if (cst == kBMNSUCCESS)
            return kBMNSUCCESS;
        if (cst == kBMNCONTINUE)
            continue;
        else
            return kBMNERROR;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::ConvertRawToRootIterateFileRead()
{
    // TStopwatch sw;
    // sw.Start();
    //    printf("fMaxEvent  %lu, fNSignalEvents %u, fMaxEvent %lu\n", fMaxEvent, fNSignalEvents, fMaxEvent);
    fCurEvType = kBMNEMPTY;
    if (fMaxEvent > 0 && fNSignalEvents >= fMaxEvent)
        return kBMNERROR;
    if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1)
        return kBMNERROR;
    fCurentPositionRawFile = ftello64(fRawFileIn);
    if (fCurentPositionRawFile >= fLengthRawFile)
        return kBMNERROR;
    switch (fDat) {
        case SYNC_EVENT:
        case SYNC_EVENT_OLD:
            fCurEvType = kBMNPAYLOAD;
            LOGF(debug, ANSI_COLOR_BLUE "SYNC_EVENT %08X" ANSI_COLOR_RESET, fDat);
            // read number of bytes in event
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1)
                return kBMNERROR;
            if (fDat % kNBYTESINWORD) {
                LOGF(error, "WTF?? in the main event: fDat == %u", fDat);
            }
            fDat = fDat / kNBYTESINWORD + (fPeriodId <= 7 ? 1 : 0);   // bytes --> words
            //                            printf("ev length %d\n", fDat);
            // read array of current event data and process them
            if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat)
                return kBMNERROR;
            fEventId = data[0];
            LOGF(debug, ANSI_COLOR_BLUE "iEv = %u" ANSI_COLOR_RESET, fEventId);
            if (fEventId <= 0)
                return kBMNCONTINUE;   // skip bad events
#ifdef MULTITHREAD
            QuickProcessEvent(data, fDat);
#else
            ProcessEvent(data, fDat);
#endif

            //            LOGF(trace, "evType %d", fCurEvType);
            //            LOGF(debug, "eventHeaderDAQ %p", (void*) eventHeaderDAQ);
            if (!isTaskMode) {
                //                LOGF(debug, "fRawTree %p", (void*) fRawTree);
                //                LOGF(debug, "fRawTree %s", fRawTree->GetName());
                //                if (fRootFileOut)
#ifndef MULTITHREAD
                /*Int_t fr = */ fRawTree->Fill();
#endif
            }
            LOGF(debug1, "isSpillStart %d", isSpillStart);
            if (isSpillStart == kTRUE)
                isSpillStart = kFALSE;
            fNSignalEvents++;
            nSpillEvents++;
            break;
        case SYNC_EOS:
            fCurEvType = kBMNEOS;
        case SYNC_STAT:
            LOGF(debug, ANSI_COLOR_BLUE "SYNC_STAT %08X" ANSI_COLOR_RESET, fDat);
            if (fDat == SYNC_STAT)
                fCurEvType = kBMNSTAT;
            // read number of bytes in event
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1)
                return kBMNERROR;
            if (fDat % kNBYTESINWORD) {
                LOGF(error, "WTF?? in the STAT: fDat == %u", fDat);
            }
            LOGF(debug3, "STAT ev length bytes %d", fDat);
            fDat = fDat / kNBYTESINWORD + (fPeriodId <= 7 ? 1 : 0);   // bytes --> words
            // read array of current event data and process them
            if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat)
                return kBMNERROR;
            ConvertStatEvent(data, fDat);
            break;
        case SYNC_RUN_START:
            LOGF(info, "RUN START");
        case SYNC_RUN_STOP:
            if (fDat == SYNC_RUN_STOP)
                LOGF(info, "RUN STOP");
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1)
                return kBMNERROR;
            if (fread(data, 1, fDat, fRawFileIn) != fDat)
                return kBMNERROR;
            BmnConverterTools::ParseComplexTLV(data, fDat, fRunId);
            break;
        case SYNC_FILE_BEGIN:
            LOGF(info, "FILE BEGIN");
        case SYNC_FILE_END:
            if (fDat == SYNC_FILE_END)
                LOGF(info, "FILE END");
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1)
                return kBMNERROR;
            if (fread(data, 1, fDat, fRawFileIn) != fDat)
                return kBMNERROR;
            BmnConverterTools::ParseComplexTLV(data, fDat, fRunId);
            break;
        case SYNC_JSON:
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1)
                return kBMNERROR;
            LOGF(debug, "SYNC JSON len %u", fDat);
            if (fread(data, 1, fDat, fRawFileIn) != fDat)
                return kBMNERROR;
            ParseJsonTLV(data, fDat);
            break;
        default:
            LOGF(error, "unrecognized sync %08X", fDat);
            break;
    }
    fPrevEvType = fCurEvType;
    // sw.Stop();
    // converterTime += sw.RealTime();
    return kBMNSUCCESS;
}

void BmnRawSource::FinalizeStat()
{
    //    printf("Fill spill! msc count %d\n", msc->GetEntriesFast());
    if (fRawTreeSpills
        && ((msc->GetEntriesFast() > 0) || (t0raw->GetEntriesFast() > 0) || (tdc_hgnd->GetEntriesFast() > 0)))
    {
        fRawTreeSpills->Fill();
        //        t0raw->Delete();
        //        msc->Delete();
    }
}

void BmnRawSource::QuickProcessEvent(UInt_t* d, UInt_t len)
{
    // Getting a free thread
    BmnConverterThread* tmp = (BmnConverterThread*)fThreads->GetWaitingThread();
    // Copying data to the free thread
    // tmp->SetData(len, d, fCurEvType, fRunId, fPeriodId, fEventId, fTime_s, fTime_ns, 0);
    tmp->Execute();
}

BmnStatus BmnRawSource::ProcessEvent(UInt_t* d, UInt_t len)
{
    TStopwatch timer;
    Double_t rtime;
    Double_t ctime;
    timer.Start();
    ClearRawArrays();
    ClearRawSpillArrays();
    if ((fNTotalEvents % 5000 == 0) && (fCurEvType != kBMNSTAT) && (fEventId > 0))
        LOG(info) << "Converted events:" << fNTotalEvents;
    //    } else if (fVerbose == 0)
    //        DrawBar(fCurentPositionRawFile, fLengthRawFile);

    UInt_t idx = 1;
    //    BmnEventType evType = kBMNPAYLOAD;

    BmnTrigInfo trigInfo;
    while (idx < len) {
        Bool_t recognized = kTRUE;
        DeviceHeader* dh = reinterpret_cast<DeviceHeader*>(d + idx);
        LOGF(debug2, "[%u / %u]", idx, len);
        //        dh->Print();
        UInt_t serial = dh->Serial;
        uint8_t id = dh->DeviceId;
        UInt_t payload = dh->GetNWords();
        idx += sizeof(DeviceHeader) / kNBYTESINWORD;
        if (payload > 2000000) {
            LOGF(warn, "Event %d:\n serial = 0x%06X\n id = Ox%02X\n payload = %d", fEventId, serial, id, payload);
            break;
        }
        if (fCurEvType == kBMNSTAT)
            LOGF(debug4, "STAT iev %7u  idx %6u/%6u   idev %02X serial 0x%08X payload %4u", fEventId, idx, len, id,
                 serial, payload);
        switch (id) {
            case kTQDC16VS_E:
                FillTQDC_E(&d[idx], serial, payload);
                break;
            case kTDC72VXS:      // Trig & ToF400 VXS TDC
            case kTDC64VHLE_E:   // ToF700 VHL TDC
                FillTDC72VXS(&d[idx], serial, payload, id);
                break;
            case kTTVXS:
                FillTTVXS(&d[idx], serial, payload);
                break;
            case kGENERIC_DEVICE:
                ProcessGenericDevice(&d[idx], dh);
                break;
            case kMSC16VE_E:
                if (msc) {
                    FillMSC16VE_E(&d[idx], dh);
                }
                break;
            case kADC64VE_XGE:
            case kADC64VE: {
                BmnStatus ret = BmnConverterTools::Process_ADC64<Short_t>(&d[idx], payload, serial,
                                                                          [&](Int_t n_samples) -> TClonesArray* {
                                                                              switch (n_samples) {
                                                                                  case 32:
                                                                                      return adc32;
                                                                                      break;
                                                                                  case 64:
                                                                                      return adc64;
                                                                                      break;
                                                                                  case 128:
                                                                                      return adc128;
                                                                                      break;
                                                                                  default:
                                                                                      return nullptr;
                                                                                      break;
                                                                              }
                                                                          });
                if (ret == kBMNERROR)
                    LOGF(warning, "Malformed data from ADC64 %08X in the event %u", serial, fEventId);
                break;
            }
            case kADC64WR: {
                BmnStatus ret = BmnConverterTools::Process_ADC64<UShort_t>(
                    &d[idx], payload, serial, [&](Int_t n_samples) -> TClonesArray* { return adc; });
                if (ret == kBMNERROR)
                    LOGF(warning, "Malformed data from ADC64 %08X in the event %u", serial, fEventId);
                break;
            }
            case kFVME:
                Process_FVME(&d[idx], payload, serial, trigInfo);
                break;
            case kHRB:
                Process_HRB(&d[idx], payload, serial);
                break;
            case kUT24VE_TRC:
                FillUT24VE_TRC(&d[idx], serial, payload);
                break;
            case kVIRTUAL_DEVICE:
                FillVirtualDevice(&d[idx], dh);
                break;
            default:
                //                printf("Device id %02X not recognized\n", id);
                recognized = kFALSE;
                break;
        }
        if (payload + idx > len) {
            printf(
                "Error in the event #%d: 0x%08X : %2X device payload length mismatch! pauload %5u idx/len: %5u/%5u \n",
                fEventId, serial, id, payload, idx, len);
            return kBMNERROR;
        } else {
            if (recognized)
                idx += payload;
            else
                idx--;
        }
    }
    if (time_slice_map.size())
        FillVSP(nullptr);
    if (fRunStartTime == TimeZero && fCurEvType == kBMNPAYLOAD) {
        fRunStartTime = TTimeStamp(time_t(fTime_s), fTime_ns);
        LOGF(info, "Run start ts %s", fRunStartTime.AsString());
    }
    eventHeaderDAQ->SetRunId(fRunId);
    eventHeaderDAQ->SetPeriodId(fPeriodId);
    eventHeaderDAQ->SetEventId(fEventId);
    eventHeaderDAQ->SetEventTimeTS(TTimeStamp(time_t(fTime_s), fTime_ns));
    eventHeaderDAQ->SetEventTime(TTimeStamp(time_t(fTime_s), fTime_ns).AsDouble());
    eventHeaderDAQ->SetEventType(fCurEvType);
    eventHeaderDAQ->SetTripWord(kFALSE);
    eventHeaderDAQ->SetTrigInfo(trigInfo);
    eventHeaderDAQ->SetSpillStart(isSpillStart);
    if ((fCurEvType == kBMNSTAT) || (fCurEvType == kBMNEOS))
        eventHeaderDAQ->SetEventId(0);
    //    printf("Event Type %d\n", fCurEvType);
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    workTime_cpu += rtime;
    workTime_real += ctime;
    if (fRunStartTime == TimeZero)
        fRunStartTime = TTimeStamp(time_t(fTime_s), fTime_ns);
    //                                printf("\nReal time %f s, CPU time %f s  Process Event\n", rtime, ctime);
    fNTotalEvents++;
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::Process_FVME(UInt_t* d, UInt_t len, UInt_t serial, BmnTrigInfo& trigInfo)
{
    LOGF(debug2, "FVME serial %08X len %u", serial, len);
    if (fCurEvType == kBMNSTAT) {
        fCurEvType = kBMNEOS;
        LOGF(debug, "Stat FVME serial %08X  efter iEv %u", serial, fEventId);
    }
    UInt_t modId = 0;
    UInt_t slot = 0;
    UInt_t type = 0;
    for (UInt_t i = 0; i < len; i++) {
        type = d[i] >> 28;
        //        if (fCurEvType == kBMNSTAT)
        //            printf("     type %02X  i %u\n", type, i);
        switch (type) {
            case kEVHEADER:
                //                if (fCurEvType == kBMNSTAT)
                //                    printf("Ev header \n");
                break;
            case kEVTRAILER:
                //                if (fCurEvType == kBMNSTAT)
                //                    printf("Ev trailer\n");
                return kBMNSUCCESS;
                break;
            case kSTATUS:
            case kPADDING:
                break;
            case kMODHEADER:
                modId = (d[i] >> 16) & 0x7F;
                slot = (d[i] >> 23) & 0x1F;
                //                if (fCurEvType == kBMNSTAT)
                //                    printf("\tmodid 0x%02X slot %d serial 0x%08X\n", modId, slot, serial);
                break;
            case kMODTRAILER:
                //                if (fCurEvType == kBMNSTAT)
                //                    printf("\tmodule trailer\n");
                if (!((d[i] >> 16) & 0x1))
                    printf(ANSI_COLOR_RED "Readout overflow error\n" ANSI_COLOR_RESET);
                if (!((d[i] >> 17) & 0x1))
                    printf(ANSI_COLOR_RED "Readout error\n" ANSI_COLOR_RESET);
                if (!((d[i] >> 18) & 0x1))
                    printf(ANSI_COLOR_RED "TTC error\n" ANSI_COLOR_RESET);
                if (!((d[i] >> 19) & 0x1))
                    printf(ANSI_COLOR_RED "Access error\n" ANSI_COLOR_RESET);
                modId = 0x00;
                slot = 0x00;
                break;
            default:   // data
            {
                //                if (fCurEvType == kBMNSTAT)
                //                    printf("\t\tmodId 0x%X serial 0x%08X\n", modId, serial);
                // BmnStatus parsing_status(kBMNSUCCESS);
                switch (modId) {
                    case kTDC64V:
                    case kTDC64VHLE:
                    case kTDC72VHL:
                    case kTDC32VL:
                        FillTDC(d, serial, slot, modId, i);
                        break;
                    case kTQDC16:
                    case kTQDC16VS:
                        FillTQDC(d, serial, slot, modId, i);
                        break;
                    case kMSC16V:
                        FillMSC(d, serial, slot, i);
                        break;
                    case kTRIG:
                        /*parsing_status = */ FillFVME2TMWR(d, serial, i, len);
                        break;
                    case kU40VE_RC:
                        FillU40VE(d, slot, i, trigInfo);
                        break;
                }
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::Process_HRB(UInt_t* d, UInt_t len, UInt_t serial)
{
    // UInt_t evId = d[0];
    UInt_t tH = d[1];
    UInt_t tL = d[2];
    UInt_t nWords = 4;                   // 4 words per plane (per 96 channels, why 4 words - 3 is enough???)
    UInt_t nSmpl = (len - 3) / nWords;   // 3 words are read now. Why divide by 4 (To ask Vadim)

    for (UInt_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        for (UInt_t iWord = 0; iWord < nWords; ++iWord) {
            UInt_t word32 = d[3 + iWord + iSmpl * nWords];
            for (Int_t iCh = 0; iCh < 32; ++iCh) {
                if (word32 & BIT(iCh)) {
                    TClonesArray& ar_hrb = *hrb;
                    new (ar_hrb[hrb->GetEntriesFast()]) BmnHRBDigit(serial, iCh + 32 * iWord, iSmpl, tH, tL);
                }
            }
        }
    }

    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillU40VE(UInt_t* d, UInt_t slot, UInt_t& idx, BmnTrigInfo& trigInfo)
{
    UInt_t type = d[idx] >> 28;
    //                printf("\t\tU40VE start type %u slot %u idx %u\n", type, slot, idx);
    Bool_t countersDone = kFALSE;
    while (type == kWORDTAI || type == kWORDTRIG || type == kWORDAUX) {
        if (fPeriodId > 4 && type == kWORDTRIG && slot == kEVENTTYPESLOT) {
            fCurEvType = ((d[idx] & BIT(3)) >> 3) ? kBMNPEDESTAL : kBMNPAYLOAD;
            // UInt_t trigSrc = ((d[idx] >> 16) & (BIT(8) - 1));
            //             printf("EvId %6u trig source %u  evType %d\n", fEventId, trigSrc, evType);
            //             if (!( ((d[idx]>>10) & 0x1) ^ (fPeriodId >= 7 && fBmnSetup == kBMNSETUP)))
            //                 printf("Ev not Good!\n");
            //             printf("evGood %d\n", (d[idx] & BIT(10)));
        }
        if (type == kWORDAUX && !countersDone) {
            trigInfo.SetTrigCand(d[idx + 0] & 0xFFFFFFF);
            trigInfo.SetTrigAccepted(d[idx + 1] & 0xFFFFFFF);
            trigInfo.SetTrigBefo(d[idx + 2] & 0xFFFFFFF);
            trigInfo.SetTrigAfter(d[idx + 3] & 0xFFFFFFF);
            trigInfo.SetTrigRjct(d[idx + 4] & 0xFFFFFFF);
            trigInfo.SetTrigAll(d[idx + 5] & 0xFFFFFFF);
            trigInfo.SetTrigAvail(d[idx + 6] & 0xFFFFFFF);
            idx += 4;
            countersDone = kTRUE;
        }
        idx++;   // go to the next DATA-word
        type = d[idx] >> 28;
        //                    printf("\t            type %u slot %u idx %u\n", type, slot, idx);
    }

    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillTDC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t& idx)
{
    UInt_t type = d[idx] >> 28;
    while (type != kMODTRAILER) {   // data will be finished when module trailer appears
        if (type == TDC_ERROR) {
            if (fVerbose == 0) {
                LOGF(warning, "TDC (modID 0x%02X serial 0x%08X slot %d tdcID %d) error flags: 0x%04X", modId, serial,
                     slot, ((d[idx] >> 24) & 0xF), (d[idx] & ((1 << 15) - 1)));
                if ((d[idx] & BIT(12)) || (d[idx] & BIT(13))) {
                    LOGF(warning, "TDC data loss in 0x%08X ", serial);
                    return kBMNERROR;
                }
            }
        }
        if (type == TDC_LEADING || type == TDC_TRAILING) {
            UInt_t tdcId = (d[idx] >> 24) & 0xF;
            UInt_t time =
                (modId == kTDC64V) ? (d[idx] & 0x7FFFF) : ((d[idx] & 0x7FFFF) << 2) | ((d[idx] & 0x180000) >> 19);
            UInt_t channel = (modId == kTDC64V) ? (d[idx] >> 19) & 0x1F : (d[idx] >> 21) & 0x7;
            // if (modId == kTDC64V && tdcId == 2) channel += 32;
            TClonesArray& ar_tdc = *tdc;
            new (ar_tdc[tdc->GetEntriesFast()])
                BmnTDCDigit(serial, modId, slot, (type == TDC_LEADING), channel, tdcId, time);
            if (fCurEvType == kBMNSTAT)
                LOGF(debug4, "tdc  %08X : %d channel %d", serial, slot, channel);
        }
        idx++;   // go to the next DATA-word
        type = d[idx] >> 28;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillTQDC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t& idx)
{
    UInt_t type = d[idx] >> 28;
    UShort_t trigTimestamp = 0;
    UShort_t adcTimestamp = 0;
    UShort_t tdcTimestamp = 0;
    UInt_t iSampl = 0;
    UInt_t channel = 0;
    Short_t valI[ADC_SAMPLING_LIMIT];
    Bool_t inADC = kFALSE;
    while (type != kMODTRAILER) {
        if (type == TDC_ERROR) {
            if (fVerbose == 0) {
                LOGF(warning, "In event %8u: TDC (serial 0x%08X slot %d tdcID %d) error flags: 0x%04X", fEventId,
                     serial, slot, ((d[idx] >> 24) & ((1 << 4) - 1)), (d[idx] & ((1 << 15) - 1)));
                if ((d[idx] & BIT(12)) || (d[idx] & BIT(13))) {
                    LOGF(warning, "\tTQDC data loss in 0x%08X ", serial);
                    return kBMNERROR;
                }
            }
        }
        UInt_t mode = (d[idx] >> 26) & 0x3;
        if (!inADC) {   //       printf("type %d mode %d word %0X\n", type, mode, d[idx]);
            if ((mode == 0) && (type == TDC_LEADING || type == TDC_TRAILING)) {   // TDC time
                channel = (d[idx] >> 19) & 0x1F;
                UInt_t time = ((d[idx] & 0x7FFFF) << 2) | ((d[idx] >> 24) & 0x3);   // in 25 ps
                //               printf("TDC time %d channel %d\n", time, channel);
                new ((*tqdc_tdc)[tqdc_tdc->GetEntriesFast()])
                    BmnTDCDigit(serial, modId, slot, (type == TDC_LEADING), channel, 0, time, tdcTimestamp);
                //                printf("tqdc tdc %08X : %d channel %d\n", serial, slot, channel);
            } else if ((type == 4) && (mode != 0)) {   // Trig | ADC Timestamp
                channel = (d[idx] >> 19) & 0x1F;
                if (d[idx] & BIT(16)) {   // ADC TS
                    adcTimestamp = d[idx] & 0xFFFF;
                    inADC = kTRUE;
                } else {   // Trig TS
                    trigTimestamp = d[idx] & 0xFFFF;
                }
            } else if (type == TDC_EV_HEADER) {
                tdcTimestamp = d[idx] & 0xFFF;
                // UInt_t iEv = (d[idx] >> 12) & 0xFFF;
                //                printf("TDC ev header: %d\n", iEv);
            } else if (type == TDC_EV_TRAILER) {
                // UInt_t iEv = (d[idx] >> 12) & 0xFFF;
                //                printf("TDC ev trailer: %d\n", iEv);
            }
        } else {
            if ((type == 5) && (mode == 2) && (iSampl < ADC_SAMPLING_LIMIT)) {
                Short_t val = (d[idx] & ((1 << 14) - 1)) - (1 << (14 - 1));
                valI[iSampl++] = val;
            } else {
                new ((*tqdc_adc)[tqdc_adc->GetEntriesFast()])
                    BmnTQDCADCDigit(serial, channel, slot, iSampl, valI, trigTimestamp, adcTimestamp);
                inADC = kFALSE;
                iSampl = 0;
                --idx;
                //                printf("tqdc adc %08X : %d channel %d\n", serial, slot, channel);
            }
        }
        type = d[++idx] >> 28;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillBlockADC(UInt_t* d, UInt_t serial, uint8_t channel, uint16_t& len, TClonesArray* ar)
{
    int16_t valI[ADC_SAMPLING_LIMIT];
    const uint8_t NBytesInSample = 2;
    uint16_t iWord = 0;
    while (iWord < len) {
        uint16_t adcTS = d[iWord] & (BIT(16) - 1);
        uint16_t SigLen = d[iWord] >> 16;
        uint16_t NSamples = SigLen / NBytesInSample;
        uint16_t NSampleWords = SigLen / kNBYTESINWORD + ((SigLen % kNBYTESINWORD) ? 1 : 0);
        //        printf("adc len %2u ts %3u NSampleWords %u\n", NSamples, adcTS, NSampleWords);
        if (iWord + NSampleWords + 1 > len) {
            printf("Error! TQDC ADC wrong payload length! iWord %u SigLen %u len %u\n", iWord, SigLen, len);
            return kBMNERROR;
        }
        uint16_t iSampleWord = 0;
        while (iSampleWord++ < NSampleWords) {
            int16_t adcLo = static_cast<int16_t>(d[iWord + iSampleWord] & (BIT(16) - 1));
            int16_t adcHi = static_cast<int16_t>(d[iWord + iSampleWord] >> 16);
            //            printf("\tadcHi %4d  adcLow %4d\n", adcHi, adcLo);
            valI[iSampleWord * 2 - 2] = adcLo;
            valI[iSampleWord * 2 - 1] = adcHi;
        }
        // no slot id for ethernet
        if (NSamples)
            new ((*ar)[ar->GetEntriesFast()]) BmnTQDCADCDigit(serial, channel, 0, NSamples, valI, 0, adcTS);
        iWord += iSampleWord;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillBlockTDC(UInt_t* d, UInt_t serial, uint16_t& len, TClonesArray* ar, UInt_t modid = 0)
{
    // uint16_t tdcTS = 0;
    // uint8_t tdcId = 0;
    uint16_t tdcLine = 0;
    while (tdcLine < len) {
        UInt_t word = d[tdcLine];
        UInt_t bt = word >> 28;
        switch (bt) {
            case TDC_EV_HEADER: {
                // uint16_t evId = (word >> 12) & (BIT(12) - 1);
                // tdcId = (word >> 24) & (BIT(4) - 1);
                // tdcTS = word & (BIT(12) - 1);
                //                 printf("\tTDC header  ev %u TS %u\n", evId, tdcTS);
                break;
            }
            case TDC_EV_TRAILER: {
                // uint16_t evId = (word >> 12) & (BIT(12) - 1);
                // uint16_t tdcWC = word & (BIT(12) - 1);
                //                 printf("\tTDC trailer ev %u WC %u\n", evId, tdcWC);
                break;
            }
            case TDC_LEADING:
            case TDC_TRAILING: {
                uint8_t channel = (word >> 21) & (BIT(7) - 1);
                //                uint16_t time = (word>>2) & (BIT(19) - 1);
                UInt_t time = word & (BIT(21) - 1);
                //                printf("\tTDC %s ch %u id %d time %u\n", (bt == TDC_LEADING) ? "leading" : "trailing",
                //                channel, tdcId, time);
                new ((*ar)[ar->GetEntriesFast()]) BmnTDCDigit(serial, modid, 0, (bt == TDC_LEADING), channel,
                                                              0 /*tdcId*/, time, 0 /*tdcTS*/);   // ignore tdcId in TQDC
                break;
            }
            case TDC_ERROR:
                LOGF(warning, "TDC (serial 0x%08X tdcID %d) in event %u error flags: 0x%04X %s", serial,
                     ((word >> 24) & ((1 << 4) - 1)), fEventId, (word & ((1 << 15) - 1)),
                     ((word & BIT(12)) || (word & BIT(13))) ? "data lost" : "");
                break;
        }
        tdcLine++;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillTQDC_E(UInt_t* d, UInt_t serial, UInt_t& len)
{
    UInt_t index = 0;
    // MStreamHeader* ms = reinterpret_cast<MStreamHeader*> (d);
    index += sizeof(MStreamHeader) / kNBYTESINWORD;
    //    ms->Print();
    MStreamTAI* ms0 = reinterpret_cast<MStreamTAI*>(d + index);
    if (ms0->valid())
        FillWR(serial, fEventId, ms0->Sec, ms0->NSec);
    index += sizeof(MStreamTAI) / kNBYTESINWORD;
    //    printf("len %u msHeader len %u\n", len, ms.Len / kNBYTESINWORD);
    //    printf("taiFlags %u TAI %s\n",
    //            ms0->Flags, TTimeStamp(time_t(ms0->Sec), ms0->NSec).AsString());
    while (index < len) {
        TqdcDataHeader* th = reinterpret_cast<TqdcDataHeader*>(d + index);
        index += sizeof(TqdcDataHeader) / kNBYTESINWORD;
        //                printf("TQDC DataType %u channel %2u adcBits %u len %4u  %8X\n", th->DataType, th->Chan,
        //                th->AdcBits, th->Len,*th);
        uint16_t blockLen = th->Len / kNBYTESINWORD;
        switch (th->DataType) {
            case kMSTREAM_TYPE_TDC:
                FillBlockTDC(d + index, serial, blockLen, tqdc_tdc);
                break;
            case kMSTREAM_TYPE_ADC:
                FillBlockADC(d + index, serial, th->Chan, blockLen, tqdc_adc);
                break;
            default:
                printf("Wrong TQDC data type %u !\n", th->DataType);
                break;
        }
        index += blockLen;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillTDC72VXS(UInt_t* d, UInt_t serial, UInt_t& len, UInt_t modid = 0)
{
    UInt_t index = 0;
    //    MStreamHeader* ms = reinterpret_cast<MStreamHeader*> (d);
    index += sizeof(MStreamHeader) / kNBYTESINWORD;
    //    ms->Print();
    MStreamTAI* ms0 = reinterpret_cast<MStreamTAI*>(d + index);
    if (ms0->valid())
        FillWR(serial, fEventId, ms0->Sec, ms0->NSec);
    //        printf("taiFlags %u TAI %s\n",
    //                ms0->Flags, TTimeStamp(time_t(ms0->Sec), ms0->NSec).AsString());
    index += sizeof(MStreamTAI) / kNBYTESINWORD;
    while (index < len) {
        uint8_t dtype = d[index] >> 28;
        bool overflow = d[index] & BIT(16);
        uint16_t blockLen = (d[index++] & (BIT(16) - 1)) / kNBYTESINWORD;
        if (!overflow)
            switch (dtype) {
                case kMSTREAM_TYPE_TDC:
                    //                    printf("TDC at index %4u  len %4u\n", index, blockLen);
                    FillBlockTDC(d + index, serial, blockLen, tdc, modid);
                    break;
                case kMSTREAM_TYPE_STAT:
                    break;
                default:
                    printf("Wrong VXS data type %u !\n", dtype);
                    break;
            }
        index += blockLen;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillTTVXS(UInt_t* d, UInt_t serial, UInt_t& len)
{
    UInt_t index = 0;
    // MStreamHeader* ms = reinterpret_cast<MStreamHeader*> (d);
    index += sizeof(MStreamHeader) / kNBYTESINWORD;
    //    ms->Print();
    MStreamTAI* ms0 = reinterpret_cast<MStreamTAI*>(d + index);
    index += sizeof(MStreamTAI) / kNBYTESINWORD;
    if (ms0->valid())
        FillWR(serial, fEventId, ms0->Sec, ms0->NSec);
    //    LOGF(debug, "ts :\t %s TTVXS 0x%08X valid %d",
    //            TTimeStamp(time_t(ms0->Sec), ms0->NSec).AsString(), serial, ms0->valid());
    //    LOGF(debug, "ts :\t %10u %9u TTVXS 0x%08X", ms0->Sec, ms0->NSec, serial);
    while (index < len) {
        uint8_t dtype = d[index] >> 28;
        bool overflow = d[index] & BIT(16);
        uint16_t blockLen = (d[index++] & (BIT(16) - 1)) / kNBYTESINWORD;
        //        printf("type 0x%02X  len %4u\n", dtype, blockLen);
        if (!overflow)
            switch (dtype) {
                case kMSTREAM_TYPE_TRIG: {
                    //                    printf("Trig at index %4u  len %4u\n", index, blockLen);
                    //                    uint8_t trigType = (d[index] << 8) & (BIT(8) - 1);
                    //                    uint8_t trigSrc = d[index] & (BIT(8) - 1);
                    //                    printf("Trig type %4u  src %4u\n", trigType, trigSrc);
                } break;
                case kMSTREAM_TYPE_STAT:
                    break;
                default:
                    printf("Wrong TTVXS data type %u !\n", dtype);
                    break;
            }
        index += blockLen;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::ProcessGenericDevice(UInt_t* d, DeviceHeader* dev_hdr)
{
    //    dev_hdr->Print();
    if ((dev_hdr->Serial & 0x00FFFFFF) == kVSP_SERIALS)
        return MapVSP(d, dev_hdr);
    else if (dev_hdr->Serial == kHGND_SERIALS)
        return FillTDC250HGND(d, dev_hdr->Serial, dev_hdr->Len);
    else
        LOGF(error, "BmnRawSource::ProcessGenericDevice: Unknown device serial 0x%08X", dev_hdr->Serial);

    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillTDC250HGND(UInt_t* d, UInt_t serial, UInt_t len)
{
    if (!d || len == 0)
        return kBMNSUCCESS;

    uint16_t* words = reinterpret_cast<uint16_t*>(d);
    size_t total_words = len / sizeof(uint16_t);
    size_t index = 0;

    std::vector<uint16_t> event_words;
    std::unique_ptr<BmnHgndRaw::FIFO_block> event;

    while (index < total_words) {
        uint16_t* word = words + index;
        if (!BmnHgndRaw::Block::is_word_event_header(*word)) {
            LOGF(error, "BmnRawSource::FillTDC250HGND: Wrong header 0x%04X", *word);
            return kBMNERROR;
        }

        auto event_type = BmnHgndRaw::Block::get_event_packet_version(*word);
        uint32_t event_length = BmnHgndRaw::Block::get_block_size(event_type);   // in 16-bit words

        LOGF(debug4, "Event type %d, 16bit words %d (index=%zu, total=%zu)", (uint32_t)event_type, event_length, index,
             total_words);

        if (index + event_length > total_words) {
            LOGF(error, "BmnRawSource::FillTDC250HGND: Event block overruns buffer");
            return kBMNERROR;
        }

        event_words.assign(word, word + event_length);
        if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug4)) {
            for (size_t i = 0; i < event_words.size(); ++i) {
                LOGF(debug4, "0x%04x", event_words[i]);
            }
        }

        if (event_type == BmnHgndRaw::Block::EventPacketVersion::V1) {
            event = std::make_unique<BmnHgndRaw::Event_type_1>(event_words);
        } else if (event_type == BmnHgndRaw::Block::EventPacketVersion::V2) {
            event = std::make_unique<BmnHgndRaw::Event_type_2>(event_words);
            if (!event->validate()) {
                LOGF(error, "BmnRawSource::FillTDC250HGND: Event block validation failed");
                return kBMNERROR;
            }

            auto* ev2 = dynamic_cast<BmnHgndRaw::Event_type_2*>(event.get());
            if (!ev2) {
                LOGF(error, "BmnRawSource::FillTDC250HGND: Event cast to Event_type_2 failed");
                return kBMNERROR;
            }

            uint8_t trise_100ps = ev2->TDC_rise_time;      // raw time rise
            uint8_t tfall_100ps = ev2->TDC_falling_time;   // raw time fall
            int64_t time_sec = ev2->timestamp >> 31;       // timestamp, seconds part
            time_sec -= tai_utc_dif;
            uint64_t time_100ps = ((ev2->timestamp & 0x7ffffff0) >> 4) * 320;   // timestamp, ns part. 32ns step
            time_100ps += (ev2->timestamp & 0xf) * 32;                          // 3.2ns step
            time_100ps += tfall_100ps;                                          // 100ps step
            uint64_t time_nsec = time_100ps / 10;
            uint16_t residual_100ps = static_cast<uint16_t>(time_100ps % 10);   // 100ps step subns part. trise
            uint16_t plen_100ps =
                (ev2->pulse_length + 1) * 32 + tfall_100ps - trise_100ps;   // pulse len x100ps +fall -rise

            LOGF(debug4, "ch_num = 0x%02X", ev2->channel_number);
            LOGF(debug4, "timestamp = 0x%016lX", ev2->timestamp);
            LOGF(debug4, "time_sec = 0x%08X", time_sec);
            LOGF(debug4, "time_nsec = 0x%08X", time_nsec);
            LOGF(debug4, "trise_100ps = 0x%02X", trise_100ps);
            LOGF(debug4, "tfall_100ps = 0x%02X", tfall_100ps);
            LOGF(debug4, "time_100ps = 0x%04X", residual_100ps);
            LOGF(debug4, "plen_100ps = 0x%04X", plen_100ps);
            LOGF(debug4, "global_ch = 0x%04X", ev2->channel_number_global);

            new ((*tdc_hgnd)[tdc_hgnd->GetEntriesFast()]) BmnAbstractTDCDigit(
                serial, ev2->channel_number_global, time_sec, time_nsec, residual_100ps, plen_100ps);
        } else {
            LOGF(error, "BmnRawSource::FillTDC250HGND: Wrong event type");
            return kBMNERROR;
        }

        index += event_length;
    }

    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::MapVSP(UInt_t* d, DeviceHeader* dev_hdr)
{
    LOGF(debug, "MapVSP");
    if (dev_hdr->GetNWords() < 3) {
        LOGF(warning, "Empty VSP block");
        return kBMNERROR;
    }
    uint8_t iComp = (dev_hdr->Serial & 0xFF000000) >> 24;
    time_slice_map[iComp] = d;
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillVSP(TClonesArray* ar)
{
    LOGF(debug, "FillVSP");
    const uint8_t kuMaxComps = 10;                    // maximum AFCKs (comps)
    uint16_t fvuCurrentTsMsbCycle[kuMaxComps] = {};   // FLES Timestamp cycling
    uint64_t fvulCurrentTsMsb[kuMaxComps] = {};       // FLES Ts_Msb buffer values
    unique_ptr<fles::StorableTimeslice> stti(new fles::StorableTimeslice(1));
    uint8_t nComp = time_slice_map.size();
    //    if (nComp != 2)
    //      LOGF(warning, "\t ncomps = %lu", nComp);
    // append components with a single timeslice
    for (uint8_t iw01 = 0; iw01 < nComp; iw01++)
        (*stti).append_component(1);
    // append microslice #0 to each component
    for (auto& el : time_slice_map) {
        uint8_t iw01 = el.first;
        auto* mslice_descr = el.second;
        (*stti).append_microslice(iw01, 0, *((fles::MicrosliceDescriptor*)mslice_descr),
                                  (uint8_t*)mslice_descr + sizeof(fles::MicrosliceDescriptor));
    }
    for (uint8_t uw02 = 0; uw02 < nComp; uw02++) {
        // microslice descriptor #0 (must be only one microslice) of component (FEB) #uw02
        const fles::MicrosliceDescriptor& msd = (*stti).descriptor(uw02, 0);
        // get the current DPB idx
        uint16_t fuCurrentEquipmentId = static_cast<uint16_t>(msd.eq_id);
        uint16_t fusCurrentDpbIdx = static_cast<uint32_t>(fuCurrentEquipmentId & 0xFFFF);

        // get the trigger number from microslice
        uint64_t tgn = msd.idx;
        if (tgn != fEventId)
            LOGF(warning, "!!! Misplaced VSP event!  tgn = %lu while EventId = %u !!!\n", tgn, fEventId);
        uint32_t tgNev = static_cast<uint32_t>(tgn);   // Ex24
        //      printf("fles tgN = %lu\t",tgn); printf("fles CurrDpbId = %u\n", fusCurrentDpbIdx);
        //      if ( tgn% kuUpdateTgN==0 ) {
        //	printf("ev. tgN = %u\t",tgNev);
        //	printf("fles tgN = %lu\t",tgn);
        //	printf("fles CurrDpbId = %u\n", fusCurrentDpbIdx);}
        // do what is needed with the microslice

        uint32_t size = 0;          // the buffer size of the Microslice
        uint32_t uNbMessages = 0;   // the number of complete messages
        // double dTriggerTime = 0;      // TrigTime in ns
        uint64_t uiTrgTimeFLES = 0;   // TrigTime in clocks
        uint64_t uiHitTimeFLES = 0;   // Hits Time in clocks

        size = msd.size;
        static const uint32_t kuBytesPerMessage = 4;
        // Compute the number of complete messages in the input microslice buffer
        uNbMessages = (size - (size % kuBytesPerMessage)) / kuBytesPerMessage;
        // Prepare variables for the loop on contents
        // const uint32_t* pInBuff = reinterpret_cast<const uint32_t*>( (*stti).content(0,0) );
        const uint32_t* pInBuff = reinterpret_cast<const uint32_t*>((*stti).content(uw02, 0));

        // uiTrgTimeFLES = 0;
        uint64_t uiTrgTime_TsMsb = 0;
        uint64_t uiHitTime_TsMsb = 0;
        uint64_t uiTrgTime_Epoch = 0;

        uint16_t uiTsMsbMessageNumber = 0;   // to take only the 1st TsMsb message
        uint16_t uiEpochMessageNumber = 0;   // to take only the 1st TsMsb message

        uint16_t usHitMissedEvts = 0;   // local counter of Hits with the Missed Events Flag
        if (uNbMessages == 0)
            continue;
        BmnVspRawDigit* dig = new ((*vsp)[vsp->GetEntriesFast()]) BmnVspRawDigit(tgn, fusCurrentDpbIdx);

        for (uint32_t uIdx = 0; uIdx < uNbMessages; ++uIdx) {
            // Fill message
            uint32_t ulData = static_cast<uint32_t>(pInBuff[uIdx]);

            stsxyter::Message mess(static_cast<uint32_t>(ulData & 0xFFFFFFFF));
            stsxyter::MessType typeMess = mess.GetMessType();
            // if (fbTxtFileOutEna) *foutHitInfoUNI << "typeMess " <<  static_cast< uint16_t >(  typeMess) << std::endl;
            switch (typeMess) {
                case stsxyter::MessType::Epoch: {
                    // The first message in the TS is a special ones: EPOCH
                    uiEpochMessageNumber++;
                    // dTriggerTime = mess.GetEpochVal() * stsxyter::kdClockCycleNs;
                    if (uiEpochMessageNumber == 1)
                        uiTrgTime_Epoch = mess.GetEpochVal();   //  all 29 bits, may24
                    else
                        printf("fles WARNING: more than one EPOCH message in event tgN = %lu, exactly: %u\n", tgn,
                               uiEpochMessageNumber);
                    break;
                }
                case stsxyter::MessType::TsMsb: {
                    uiTsMsbMessageNumber++;
                    uint64_t uVal = mess.GetTsMsbValBinning();
                    if (uVal < fvulCurrentTsMsb[uw02]) {
                        printf("TsMsbCycle: event trigger# %u", tgNev);
                        printf("\tDPB: %u, Old TsMsb= %lu, new TsMsb=%lu, Old MsbCy=%u\n", uw02, fvulCurrentTsMsb[uw02],
                               uVal, fvuCurrentTsMsbCycle[uw02]);

                        if ((fvulCurrentTsMsb[uw02] - uVal) > 10000000)
                            fvuCurrentTsMsbCycle[uw02]++;

                    }   // if( uVal < fvulCurrentTsMsb[uw02] )
                    fvulCurrentTsMsb[uw02] = uVal;
                    uiHitTime_TsMsb = uVal << 10;   // shift to 10 bits;
                    uiHitTime_TsMsb += static_cast<uint64_t>(stsxyter::kulTsCycleNbBinsBinning)
                                       * fvuCurrentTsMsbCycle[uw02];   // TsMsbCycle added
                    if (uiTsMsbMessageNumber == 1) {
                        uiTrgTime_TsMsb =
                            uiHitTime_TsMsb & 0xFFE0000000;   // remove the lower 29 bits, the faster procedure
                    }
                    break;
                }   // case stsxyter::MessType::TsMsb :
                case stsxyter::MessType::Hit: {
                    uint16_t usRawTs = static_cast<uint16_t>(mess.GetHitTimeBinning());
                    uiHitTimeFLES = uiHitTime_TsMsb + static_cast<uint64_t>(usRawTs);
                    uint16_t usElinkIdx = mess.GetLinkIndexHitBinning();
                    uint16_t usChan = mess.GetHitChannel();
                    uint16_t usRawAdc = mess.GetHitAdc();

                    if (mess.IsHitMissedEvts())
                        usHitMissedEvts++;
                    dig->AppendHit(uiHitTimeFLES, usElinkIdx, usChan, usRawAdc);
                    break;
                }
                default:
                    break;
            }   // switch( typeMess )

        }   // for( uint32_t uIdx = 0; uIdx < uNbMessages; ++uIdx )
        dig->SetTHitMissedEvts(usHitMissedEvts);
        uiTrgTimeFLES = uiTrgTime_TsMsb + uiTrgTime_Epoch;
        dig->SetTTrigTime(uiTrgTimeFLES);
    }
    time_slice_map.clear();
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillMSC16VE_E(UInt_t* d, DeviceHeader* dev_hdr)
{
    //    msc->Delete();
    UInt_t index = 0;
    const uint32_t len = dev_hdr->GetNWords();
    MSC16VE_EHeader* ms = reinterpret_cast<MSC16VE_EHeader*>(d);
    index += sizeof(MSC16VE_EHeader) / kNBYTESINWORD;
    //        ms->Hdr.Print();
    if (ms->Hdr.Len > (len - 1)) {
        LOGF(error, "MSC header payload length mismatch!");
        return kBMNERROR;
    }
    //   LOGF(info, "MSC TAI\t%s taiFlags %u ", TTimeStamp(time_t(ms->Tai.Sec), ms->Tai.NSec).AsString(),
    //   ms->Tai.Flags); LOGF(info, "MSC ver %u  CntrBitsNum %u  SliceInt %3u ns  ChanNum %2u  ExtCondNum %u",
    //       ms->GetVersion(), ms->NCntrBits, ms->SliceInt, ms->ChanNumber, ms->ExtCondCnt);
    Bool_t hasValues = kFALSE;
    std::array<uint8_t, MSC_N_COUNTERS> cntrs = {};
    switch (ms->GetVersion()) {
        case 1: {
            const uint8_t NumsInWord = 4;
            while (index < len) {
                uint8_t type = d[index] >> 28;
                //        printf(" index %4u  len %4u   type %u\n", index, len, type);
                if (type == MSC_TIME_SLICE_INFO) {
                    if (hasValues) {
                        UInt_t ext_cond = (d[index] >> 24) & (BIT(4) - 1);
                        UInt_t sliceNum = (d[index]) & (BIT(24) - 1);
                        auto sc_time_shift = ms->SliceInt * sliceNum * 1ns;
                        SysPoint p{(ms->Tai.Sec - tai_utc_dif) * 1s + ms->Tai.NSec * 1ns + sc_time_shift};
                        LOGF(debug, "MSC ext_cond %2u  sliceNum %7u  time %s", ext_cond, sliceNum,
                             BmnFunctionSet::TimePoint2String(p).c_str());
                        new ((*msc)[msc->GetEntriesFast()])
                            BmnMSCDigit<uint8_t>(dev_hdr->Serial, 0, cntrs, fEventId, p, ext_cond);
                        hasValues = kFALSE;
                        memset(cntrs.data(), 0, sizeof(UChar_t) * MSC_N_COUNTERS);
                    }
                } else {
                    for (uint8_t i = 0; i < NumsInWord; i++) {
                        UInt_t cnt = (d[index] >> (i * ms->NCntrBits)) & (BIT(ms->NCntrBits) - 1);
                        cntrs[type * NumsInWord + i] = cnt;
                        //                LOGF(info,"\tcnt[%2u] = %2u", (type * NumsInWord + i), cnt);
                        hasValues = kTRUE;
                    }
                }
                index++;
            }
        } break;
        case 2: {
            UInt_t missing_hits = d[index++];
            if (missing_hits)
                LOGF(info, "missing_hits %3u !!!", missing_hits);
            LOGF(debug, "missing_hits %u", missing_hits);
            //            uint8_t channel = ms->ChanNumber;
            //            TimeDist& scaler_cnts = metadata->Scalers()[make_pair(serial, channel)];
            //            TimeDist& ext_cnts = metadata->ExtConditions();
            SysPoint point{(ms->Tai.Sec - tai_utc_dif) * 1s + ms->Tai.NSec * 1ns};
            uint32_t n_slices = ms->Hdr.Len - 1 - (sizeof(MSC16VE_EHeader) - sizeof(MStreamHeader)) / kNBYTESINWORD;
            BmnMSCZSDigit<uint8_t>* dig = new ((*msc)[msc->GetEntriesFast()])
                BmnMSCZSDigit<uint8_t>(dev_hdr->Serial, ms->ChanNumber, point, n_slices);
            vector<uint8_t>& vals = dig->GetValues();
            vector<SysPoint>& times = dig->GetTimes();
            vector<uint8_t>& conds = dig->GetExtCond();
            int32_t istart = index;
            while (index < len) {
                uint8_t cnt = d[index] & (BIT(ms->NCntrBits) - 1);
                uint8_t ext_cond = (d[index] >> ms->NCntrBits) & (BIT(ms->ExtCondCnt) - 1);
                UInt_t sliceNum = d[index] >> (ms->NCntrBits + ms->ExtCondCnt);
                auto sc_time_shift = ms->SliceInt * sliceNum * 1ns;
                SysPoint point_slice{point + sc_time_shift};
                int32_t iVal = index - istart;
                //                                LOGF(info, "iVal %4u", iVal);
                vals[iVal] = cnt;
                times[iVal] = point_slice;
                conds[iVal] = ext_cond;
                //                                LOGF(info, "MSC i %2u  len %3u", index, len);
                LOGF(debug, "MSC ext_cond %2u cnt %2u sliceNum %7u  time %s", ext_cond, cnt, sliceNum,
                     BmnFunctionSet::TimePoint2String(point_slice).c_str());

                //                scaler_cnts.insert(make_pair(p, cnt));
                //                ext_cnts.insert(make_pair(p, ext_cond));

                //                cntrs[ms->ChanNumber] = cnt;
                //                new ((*msc)[msc->GetEntriesFast()]) BmnMSCDigit<uint8_t>(serial, 0, cntrs, fEventId,
                //                p, ext_cond); memset(cntrs.data(), 0, sizeof(UChar_t) * MSC_N_COUNTERS);
                index++;
            }
        } break;
        default:
            LOGF(warning, "MSC16 ver $u not implemented", ms->GetVersion());
            return kBMNERROR;
            break;
    }
    //    if ((fRootFileOut) && (msc->GetEntries()))
    //        fRawTreeSpills->Fill();
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillVirtualDevice(UInt_t* d, DeviceHeader* dh)
{
    LOGF(info, "Found virtual device 0x%08X block", dh->Serial);
    LOGF(info, "Ev %u  type %u", fEventId, (UInt_t)fCurEvType);
    if (dh->Serial != SERIAL_TO_CFG) {
        LOGF(info, "Not T0 device");
        return kBMNSUCCESS;
    }
    const Int_t bin_block_len = kT0_BIN_BLOCK_WORDS * kNBYTESINWORD;
    if (fExportExternalSpillStat) {
        std::ofstream outfile(Form("virt_device_run_%u_%08X_%02X_%s_t_%lld_%lld.txt", fRunId, dh->Serial, dh->DeviceId,
                                   fSubName.Data(), fTime_s, fTime_ns),
                              ios::out | ios::binary);
        for (UInt_t i = 0; i < kT0_BIN_BLOCK_WORDS; i++)
            outfile << std::hex << d[i];
        outfile.write(reinterpret_cast<const char*>(d) + bin_block_len, dh->Len - bin_block_len);
    }
    //    t0raw->Delete();
    auto lastT0 = new ((*t0raw)[t0raw->GetEntriesFast()]) BmnT0Raw<kT0_BIN_BLOCK_WORDS>(d);
    const Int_t date_len = 19;   // 23.12.2022 14:17:46
    TString str(reinterpret_cast<const char*>(d + kT0_BIN_BLOCK_WORDS), date_len);
    TPRegexp re_str_cfg_t0("(\\d{2})\\.(\\d{2})\\.(\\d{4})\\s(\\d{2}:\\d{2}:\\d{2})");
    int32_t re_subst_ret = re_str_cfg_t0.Substitute(str, "$3-$2-$1 $4");
    LOGF(info, "ts line return %d", re_subst_ret);
    for (UInt_t i = 0; i < kT0_BIN_BLOCK_WORDS; i++) {
        LOGF(debug, "cntr[%3d]: %8u", i, d[i]);
    }
    uint64_t t_2020_sec(d[kT0_BIN_BLOCK_2020_SEC]);
    uint64_t t_unix_sec(*reinterpret_cast<uint64_t*>(d + kT0_BIN_BLOCK_UNIX_SEC));
    //    LOGF(info, "t_2020_sec %u", t_2020_sec);
    //    LOGF(info, "t_unix_sec %u", t_unix_sec);
    TTimeStamp ts_unix_sec(t_unix_sec, 0);
    TTimeStamp ts_2020_sec(t_2020_sec + TTimeStamp(2020, 1, 1, 0, 0, 0), 0);
    LOGF(info, "unix ts: %s", ts_unix_sec.AsString());
    LOGF(info, "2020 ts: %s", ts_2020_sec.AsString());
    uint32_t trig_bit_conf(d[kT0_BIN_BLOCK_WORDS - 1]);
    lastT0->SetTriggerMask(trig_bit_conf);
    bitset<32> trig_bit_conf_bs(trig_bit_conf);
    LOGF(debug, "trig bitset %u", trig_bit_conf);
    LOGF(debug, "trig bitset %s", trig_bit_conf_bs.to_string());
    uint32_t pos = kT0_BIN_BLOCK_WORDS * kNBYTESINWORD + date_len;
    string tcfg(reinterpret_cast<const char*>(d) + pos, dh->Len - pos);
    BmnConverterTools::ParseRawT0TextConfig(tcfg, lastT0);

    ts_unix_sec =
        TTimeStamp(ts_unix_sec.GetSec() - 3 * 3600,
                   ts_unix_sec.GetNanoSec());   // crutch: shifting time zone from Moscow -> GMT for uniformity
    if (t_unix_sec > 0)
        lastT0->SetTS(ts_unix_sec);
    else {
        if (t_2020_sec > 0) {
            lastT0->SetTS(ts_2020_sec);
        } else if (re_subst_ret > 0) {
            TDatime dt(str);
            TTimeStamp ts_from_string(static_cast<UInt_t>(dt.GetDate()), static_cast<UInt_t>(dt.GetTime()), 0u);
            LOGF(info, "ts line: %s", ts_from_string.AsString());
            ts_from_string = TTimeStamp(
                ts_from_string.GetSec() - 3 * 3600,
                ts_from_string.GetNanoSec());   // crutch: shifting time zone from Moscow -> GMT for uniformity
                                                //    struct tm tm_time;
                                                //    strptime(str.Data(), "%d.%m.%Y %H:%M:%S", &tm_time);
                                                //    std::time_t time_t_time = std::mktime(&tm_time);
                                                //    SysPoint time_point = SysClock::from_time_t(time_t_time);
            //    time_point -= 3h;   // crutch: shifting time zone from Moscow -> GMT for uniformity
            //    LOG(info) << "T0 point: " << BmnFunctionSet::TimePoint2String(time_point);
            //    lastT0->SetTime(time_point);
            lastT0->SetTS(ts_from_string);
        } else
            LOGF(warning, "Unable to find raw T0 block timestamp!");
    }
    auto tsle = lastT0->GetTime() - BmnFunctionSet::TimeStamp2TP(TTimeStamp(time_t(fTime_s), fTime_ns));
    //    lastT0->SetTimeSinceLastEv(tsle);
    // Get time since last event (surely it will be incorrect for several ns, but at average precise enough to assign it
    // to the spill)
    lastT0->SetTimeSinceLastEvNs(duration_cast<nanoseconds>(tsle).count());
    //    if (fRootFileOut)
    //        fRawTreeSpillsT0->Fill();
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillUT24VE_TRC(UInt_t* d, UInt_t& serial, UInt_t& len)
{
    UInt_t index = 0;
    MStreamHeader* ms = reinterpret_cast<MStreamHeader*>(d);
    index += sizeof(MStreamHeader) / kNBYTESINWORD;
    //    ms->Print();
    // start bit input time in the MStreamTAI, and should be the 4th word timeslice
    MStreamTAI* ms0 = reinterpret_cast<MStreamTAI*>(d + index);
    if (ms0->valid()) {
        BmnSyncDigit* sd = FillWR(serial, fEventId, ms0->Sec, ms0->NSec);
        fTime_ns = sd->GetTime_ns();
        fTime_s = sd->GetTime_sec();
    }
    index += sizeof(MStreamTAI) / kNBYTESINWORD;
    if (ms->Len / kNBYTESINWORD > len)
        LOGF(error, "UT24VE-TRC Error! MSHeader payload length larger than from device header!");
    fCurEvType = (d[index] & BIT(16)) ? kBMNPEDESTAL : kBMNPAYLOAD;
    //     bool randomTrigger = d[index] & BIT(17);
    //     bool periodicTrigger = d[index] & BIT(18);
    //     bool externalTTL = d[index] & BIT(19);
    LOGF(debug, "FillUT24VE_TRC etype %u WR %08X ts: %s", (uint32_t)fCurEvType, serial,
         TTimeStamp(time_t(fTime_s), fTime_ns).AsString());
    eventHeaderDAQ->SetInputSignalsAR(d[index++]);
    eventHeaderDAQ->SetInputSignalsBR(d[index]);
    vector<uint32_t>& vec = eventHeaderDAQ->GetInputSignalsVector();
    while (++index < len) {
        uint32_t sig_states = d[index];
        vec.push_back(sig_states);
        bitset<32> sig_states_bs(sig_states);
        LOGF(debug2, "TRC states d[%3u] %s", index, sig_states_bs.to_string());
    }
    return kBMNSUCCESS;
}

BmnSyncDigit* BmnRawSource::FillWR(UInt_t serial, ULong64_t iEvent, Long64_t t_sec, Long64_t t_ns)
{
    //    LOGF(debug4, "Fill WR %08X", serial);
    //    LOGF(info, "Fill WR %08X   RS ts %s", serial, TTimeStamp(time_t(t_sec), t_ns).AsString());
    if (tai_utc_dif == 0) {
        tai_utc_dif = GetUTCShift(TTimeStamp(time_t(t_sec), t_ns));
        if (tai_utc_dif == 0)
            LOGF(warn, "Possibly wrong state of %08X", serial);
    }
    return new ((*sync)[sync->GetEntriesFast()]) BmnSyncDigit(serial, iEvent, t_sec - tai_utc_dif, t_ns);
}

BmnStatus BmnRawSource::FillFVME2TMWR(UInt_t* d, UInt_t serial, UInt_t& idx, UInt_t& len)
{
    while (idx < len) {
        UInt_t word = d[idx];
        UInt_t id = word >> 28;
        switch (id) {
            case TMWR_TAI: {
                UInt_t d0 = d[idx + 0];
                UInt_t d1 = d[idx + 1];
                UInt_t d2 = d[idx + 2];
                UInt_t d3 = d[idx + 3];
                if ((d0 >> 28) != 2 || (d1 >> 28) != 2 || (d2 >> 28) != 2 || (d3 >> 28) != 2)
                    return kBMNERROR;   // check TAI code
                Long64_t ts_t0_s = -1;
                Long64_t ts_t0_ns = -1;
                Long64_t GlobalEvent = -1;
                ts_t0_ns = (d0 & 0x0FFFFFFF) | ((d1 & 0x3) << 28);
                ts_t0_s = ((d1 >> 4) & 0xFFFFFF) | ((d2 & 0xFFFF) << 24);
                GlobalEvent = ((d3 & 0x0FFFFFFF) << 12) | ((d2 >> 16) & 0xFFF);
                BmnSyncDigit* sd = FillWR(serial, GlobalEvent, ts_t0_s, ts_t0_ns);
                if (fPeriodId < 8) {
                    fTime_ns = sd->GetTime_ns();
                    fTime_s = sd->GetTime_sec();
                }
                idx += 4;
                LOGF(debug2, "ts :\t %10lli %9lli TMWR  0x%08X", ts_t0_s, ts_t0_ns, serial);
            } break;
            case TMWR_REL_TS: {
                UInt_t d0 = d[idx + 0];
                UInt_t d1 = d[idx + 1];
                if (((d0 >> 28) != 4) || ((d1 >> 28) != 5))
                    return kBMNERROR;
                // Int_t rts(((d0 & (BIT(24) - 1)) << 8) | ((d1 >> 20) & (BIT(8) - 1)));
                // UInt_t ext_word((d1 >> 16) & (BIT(4) - 1));
                // UInt_t trg_word(d1 & (BIT(16) - 1));
                idx += 2;
                //                printf("rts %7d  ext_tr word %u  tr word %u\n" , rts, ext_word, trg_word);
            } break;
            case TMWR_EOS_CNT_MATCHED:
            case TMWR_EOS_CNT: {
                // UInt_t d0 = d[idx + 0];
                // UInt_t logic_cnt(d0 & (BIT(28) - 1));
                //                 printf("logic_cnt %7u %s\n" , logic_cnt, id == TMWR_EOS_CNT_MATCHED ? "matched" :
                //                 "");
                idx++;
            } break;
            case TMWR_PULSE:
            case TMWR_REL_TS_TB:
                idx++;   // just skip
                break;
            case kMODTRAILER:
            case kSTATUS:
            case kPADDING:
                idx--;
                return kBMNSUCCESS;
            default:
                printf("unrecognized TMWR id %u\n", id);
                idx++;
                break;
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawSource::FillMSC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t& idx)
{
    UInt_t type = d[idx] >> 28;
    UInt_t iCnt = 0;
    BmnMSCDigit<UInt_t>* dig = new ((*msc)[msc->GetEntriesFast()]) BmnMSCDigit<UInt_t>(serial, slot, fEventId);
    std::array<UInt_t, MSC_N_COUNTERS>& cntrArrCur = dig->GetValue();
    //    printf("MSC type %u serial %08X last eventID = %6u\n", type, serial, fEventId);
    while (type < 6) {
        if (type < 5) {
            // UInt_t cnt3 = (d[idx] >> 21) & (BIT(8) - 1);
            // UInt_t cnt2 = (d[idx] >> 14) & (BIT(8) - 1);
            // UInt_t cnt1 = (d[idx] >> 7) & (BIT(8) - 1);
            // UInt_t cnt0 = d[idx] & (BIT(8) - 1);
            //             printf("type = %u  %06u  %06u  %06u  %06u  \n", type, cnt3, cnt2, cnt1, cnt0);
        } else if (type == 5) {
            UInt_t cnt = d[idx] & (BIT(28) - 1);
            if (iCnt >= MSC_N_COUNTERS)
                continue;
            cntrArrCur[iCnt++] = cnt;
            //            printf("\ttype = %u  arr[%2u] = %8u\n", type, iCnt - 1, cntrArrCur[iCnt - 1]);
        }
        type = (d[++idx] >> 28) & (BIT(5) - 1);
    }

    return kBMNSUCCESS;
};

void BmnRawSource::ClearRawArrays()
{
    sync->Delete();
    tdc->Delete();
    tqdc_adc->Delete();
    tqdc_tdc->Delete();
    hrb->Delete();
    adc32->Delete();
    adc64->Delete();
    adc128->Delete();
    adc->Delete();
    vsp->Delete();
    eventHeaderDAQ->Clear();
}

void BmnRawSource::ClearRawSpillArrays()
{
    if (fRawTreeSpills) {
        msc->Delete();
        t0raw->Delete();
        tdc_hgnd->Delete();
    }
}

BmnStatus BmnRawSource::FinishRun()
{
    // Double_t fSize = Double_t(fLengthRawFile / 1024. / 1024.);
    // Int_t nEv = fNTotalEvents;

    //    if (!UniRun::GetRun(fPeriodId, fRunId))
    //        UniRun::CreateRun(fPeriodId, fRunId, fRawFileName, "", nullptr, nullptr, fRunStartTime, &fRunEndTime,
    //        &nEv, nullptr, &fSize, nullptr);

    return kBMNSUCCESS;
}

Int_t BmnRawSource::GetRunIdFromFile(TString name)
{
    fRunId = 0;
    TPRegexp re(".+\\.root");
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
            fRootFileIn->Close();
        }
        delete fRootFileIn;
    }
    if (fRunId > 0) {
        isRawRootInputFile = true;
        return fRunId;
    }

    FILE* file = fopen(name.Data(), "rb");
    if (file == nullptr) {
        printf("File %s is not open!!!\n", name.Data());
        return -1;
    }
    UInt_t word;
    Int_t cntr = 0;
    while (fread(&word, kWORDSIZE, 1, file)) {
        // printf("%X\n", word);
        if (cntr == 20) {
            printf("After checking %d words in input file run number not found!\n", cntr);
            printf("Let's get run number from file name\n");
            break;
        }
        if (word == RECORD_RUN_NUMBER) {
            if (fread(&word, kWORDSIZE, 1, file) != 1)
                break;   // skip word
            if (fread(&fRunId, kWORDSIZE, 1, file) != 1)
                break;
            return fRunId;
        }
        cntr++;
    }
    fclose(file);
    if (fRunId <= 0) {
        // Int_t run = 0;
        // sscanf(&(((char *)name.Data())[strlen(name.Data())-9]), "%d", &run);
        TPRegexp(".*\\w+_\\w+_\\w+_(\\d+)[^/]*\\.data").Substitute(name, "$1");

        // cout << name << endl;

        return name.Atoi();

    } else
        return fRunId;
}

BmnStatus BmnRawSource::InitUTCShift()
{
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1972, 1, 1, 0, 0, 9), 10));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1972, 7, 1, 0, 0, 10), 11));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1973, 1, 1, 0, 0, 11), 12));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1974, 1, 1, 0, 0, 12), 13));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1975, 1, 1, 0, 0, 13), 14));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1976, 1, 1, 0, 0, 14), 15));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1977, 1, 1, 0, 0, 15), 16));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1978, 1, 1, 0, 0, 16), 17));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1979, 1, 1, 0, 0, 17), 18));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1980, 1, 1, 0, 0, 18), 19));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1981, 7, 1, 0, 0, 19), 20));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1982, 7, 1, 0, 0, 20), 21));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1983, 7, 1, 0, 0, 21), 22));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1985, 7, 1, 0, 0, 22), 23));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1988, 1, 1, 0, 0, 23), 24));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1990, 1, 1, 0, 0, 24), 25));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1991, 1, 1, 0, 0, 25), 26));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1992, 7, 1, 0, 0, 26), 27));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1993, 7, 1, 0, 0, 27), 28));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1994, 7, 1, 0, 0, 28), 29));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1996, 1, 1, 0, 0, 29), 30));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1997, 7, 1, 0, 0, 30), 31));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1999, 1, 1, 0, 0, 31), 32));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2006, 1, 1, 0, 0, 32), 33));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2009, 1, 1, 0, 0, 33), 34));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2012, 7, 1, 0, 0, 34), 35));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2015, 7, 1, 0, 0, 35), 36));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2017, 1, 1, 0, 0, 36), 37));
    utc_valid = TTimeStamp(2026, 12, 30, 0, 0, 1);
    return kBMNSUCCESS;
}

Int_t BmnRawSource::GetUTCShift(TTimeStamp t)
{
    if (t < leaps.begin()->first) {
        LOGF(warning, "Wrong time! %s", t.AsString());
        return 0;
    }
    if (t > utc_valid)
        LOGF(warning, "Warning! Leap seconds table expired!");
    Int_t shift = 0;
    auto it = leaps.lower_bound(t);
    if ((it == leaps.end()))
        it--;
    else if (it->first > t)
        it--;
    shift = it->second;
    return shift;
}

BmnStatus BmnRawSource::LoadConfig()
{
    if (fDecoderConfigFileName.length() == 0)
        fDecoderConfigFileName = string(getenv("VMCWORKDIR")) + "/config/bmnconf.json";
    pt::read_json(fDecoderConfigFileName, conf);
    fExportJsonBlocks = conf.get<bool>("Decoder.ExportJson", false);
    fExportExternalSpillStat = conf.get<bool>("Decoder.ExportExternalSpillStat", false);
    return kBMNSUCCESS;
}
