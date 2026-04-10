#include "BmnConverter.h"
// STL
#include <algorithm>
#include <arpa/inet.h> /* For ntohl for Big Endian LAND. */
#include <bitset>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <unistd.h>
// ROOT
#include <ROOT/RDF/RInterface.hxx>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RSnapshotOptions.hxx>
// BmnRoot
#include "BmnConverterTools.h"
#include "BmnRawDataDecoder.h"
#include "FairLogger.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TTree.h"
#include "UniDetectorParameter.h"
#include "UniRawFile.h"
#include "UniRun.h"

namespace fs = std::filesystem;

BmnConverter::BmnConverter(uint32_t threadCount, uint32_t period, TString outDir, bool tempFileOnDisk)
    : temp_file_on_disk(tempFileOnDisk)
    , fPeriodId(period)
    , fRawRunHdrName("RawRunHeader")
    , fMetadataName("RawRunMetadata")
    , fRawTreeName("BMN_RAW")
    , fRawTreeSpillName("BMN_RAW_SPILLS")
    , fRootDirPath(outDir)
    //    , fRootFileOut(nullptr)
    , spillHeader(nullptr)
    , runHeader(make_unique<DigiRunHeader>())
    , metadata(make_unique<BmnMetadataRaw>())
    //    , fMerger(fMerger)
    , fExportJsonBlocks(false)
    , tai_utc_dif(0)
    , fCurWaitingThread(nullptr)
{
    TTree::SetMaxTreeSize(1'000'000'000'000);   // 932 GB
    ROOT::EnableThreadSafety();
    LOGF(info, ANSI_COLOR_BLUE "Starting converter's constructor" ANSI_COLOR_RESET);
    InitUTCShift();
    LOGF(info, "concurrency: %u", std::thread::hardware_concurrency());
    if (fRootDirPath.Length() == 0)
        fRootDirPath = fs::current_path();
    bool dc = fs::create_directories(fRootDirPath.Data());
    LOGF(info, "Directory: %s was %s", fRootDirPath.Data(), dc ? "created" : "already present");
    fRootFileName = TString::Format("%s/raw_temp_%d.root", fRootDirPath.Data(), getpid());
    if (temp_file_on_disk)
        CreateTempFile();

    if (threadCount == 0) {
        if (std::thread::hardware_concurrency() <= 1)
            fThreadCnt = 1;   // either 1 core or no info
        else
            fThreadCnt = std::thread::hardware_concurrency() - 1;
    } else
        fThreadCnt = threadCount;
    fThreads = new BmnThreadManager<BmnConverterThread>();
    LOGF(info, "Creating %u%s", fThreadCnt, (fThreadCnt == 1 ? " converter thread" : " converter threads"));
    for (uint32_t i = 0; i < fThreadCnt; i++) {
        BmnConverterThread* tmp = fThreads->Add(0);
        tmp->SetPeriod(fPeriodId);
        if (temp_file_on_disk)
            tmp->SetMerger(fMerger.get());
        tmp->CreateTrees();
        tmp->RegisterBranches();
        tmp->RegisterSpillBranches();
    }
    LoadConfig();
    // bool convertMSC = conf.get<bool>("Decoder.ProcessMSC", false);
    // if (convertMSC) {
    /*fRawTreeSpills = new TTree("BMN_RAW_SPILLS", "BMN_RAW_SPILLS");
    auto spill_reg_fun = [&] (TString name, TObject * ar) -> void {
        TBranch *b = fRawTreeSpills->Branch(name, &ar);
        LOGF(debug1, "Register branch %p : %s", (void*) b, name.Data());
        return;
    };*/
    /*for(int i = 0; i < fThreadCnt; i++)
        fThreads->GetThread(i)->RegisterSpillBranches();
}*/
    LOGF(info, "Created %u%s", fThreadCnt, (fThreadCnt == 1 ? " converter thread" : " converter threads"));
    LOGF(info, ANSI_COLOR_BLUE "Finished creating converter\n" ANSI_COLOR_RESET);
}

BmnStatus BmnConverter::LoadConfig()
{
    if (fDecoderConfigFileName.length() == 0)
        fDecoderConfigFileName = string(getenv("VMCWORKDIR")) + "/config/bmnconf.json";
    pt::read_json(fDecoderConfigFileName, conf);
    fExportJsonBlocks = conf.get<bool>("Decoder.ExportJson", false);
    bool ExportExternalSpillStat = conf.get<bool>("Decoder.ExportExternalSpillStat", false);
    for (uint32_t i = 0; i < fThreadCnt; i++) {
        BmnConverterThread* tmp = fThreads->GetThread(i);
        tmp->SetExportExternalSpillStat(ExportExternalSpillStat);
    }
    return kBMNSUCCESS;
}

BmnStatus BmnConverter::CreateTempFile()
{
    LOGF(info, "Creating temporary output root file");
    fRootFileName = TString::Format("%s/raw_temp_%u.root", fRootDirPath.Data(), fRunId);   //, getpid());
    fMerger = make_unique<ROOT::TBufferMerger>(fRootFileName.Data());
    //    fRootFileOut = new TFile(fRootFileName, "recreate");
    if (!fMerger) {
        //    if (!fRootFileOut->IsOpen()) {
        //        delete fRootFileOut;
        LOGF(error, "Couldn't access the output file!!!!!\n");
        return kBMNERROR;
    }
    LOGF(info, "Created temporary output root file");
    return kBMNSUCCESS;
}

Int_t BmnConverter::GetSpillNumber(vector<SysPoint>* spillEnds, vector<SysPoint>* spillStarts, SysPoint time)
{
    auto t = std::upper_bound(spillEnds->begin(), spillEnds->end(), time);
    if (t == spillEnds->end()) {
        return -1;   // currently no spill end;
    } else {
        if (time < (*spillStarts)[t - spillEnds->begin()]) {
            return -2;
        }
        return t - spillEnds->begin();
    }
}

Int_t BmnConverter::GetSpillNumber(vector<SysPoint>* spillEnds, vector<SysPoint>* spillStarts, TTimeStamp time)
{
    SysPoint p{BmnFunctionSet::TimeStamp2TP(time)};
    return GetSpillNumber(spillEnds, spillStarts, p);
}

void BmnConverter::OutputTrees()
{
    fThreads->Finish();
    BmnConverterThread* tmp;
    for (UInt_t i = 0; i < fThreadCnt; i++) {
        LOGF(info, "WriteTree for thread %u", i);
        tmp = fThreads->GetThread(i);
        //        tmp->WriteTree();
        //        tmp->WriteTreeSpills();
        tmp->FinalizeFile();
    }
    {
        auto file = fMerger->GetFile();
        file->WriteObject(runHeader.get(), fRawRunHdrName.Data());
        file->WriteObject(metadata.get(), fMetadataName.Data());
        file->Write();
        file->Close();
        file.reset();
    }
    LOGF(info, "main thread objects written");
    //    LOGF(info, "main thread file closed");
    ////    fMerger->Close();
    fMerger.reset();
    //    LOGF(info, "fMerger reset");
    //    {
    //        TFile file(fRootFileName, "update");
    //        TTree* ev_tree = static_cast<TTree*>(file.Get(fRawTreeName));
    //        Int_t build_status = ev_tree->BuildIndex("BmnEventHeader.fEventId");
    //        LOGF(info, "Building tree index: %d", build_status);
    //        ev_tree->Write();//"", TObject::kOverwrite);
    //        file.Close();
    //    }
}

void BmnConverter::OutputSpills()
{
    LOGF(info, ANSI_COLOR_BLUE "Starting to write spills" ANSI_COLOR_RESET);
    TStopwatch* timer = new TStopwatch();
    timer->Start();
    ROOT::EnableImplicitMT(2);
    LOGF(info, "Preparing temporary file for read access");
    OutputTrees();
    LOGF(info, "Trees written");
    TFile* fRootFileOut = new TFile(fRootFileName, "read");
    if (!fRootFileOut->IsOpen()) {
        delete fRootFileOut;
        LOGF(error, "Couldn't access the temp file!!!!!\n");
        return;
    }
    for (UInt_t i = 0; i < fThreadCnt; i++) {
        fThreads->GetThread(i)->SetTrees((TTree*)(fRootFileOut->Get(fRawTreeName + to_string(i))),
                                         (TTree*)(fRootFileOut->Get(fRawTreeSpillName + to_string(i))));
    }
    set<int> spillCheck;
    for (size_t i = 0; i < vecTree.size(); i++)
        spillCheck.insert(vecTree[i].first);
    for (size_t i = 0; i < vecTreeSpills.size(); i++)
        spillCheck.insert(vecTreeSpills[i].first);
    LOGF(info, "Starting to write %d spills", spillCheck.size());
    for (auto t : spillCheck) {
        {
            ROOT::TBufferMerger merger(to_string(fRunId) + (TString) "_s" + to_string(t) + ".root", "recreate");
            BmnConverterThread* tmp;
            for (UInt_t i = 0; i < fThreadCnt; i++) {
                tmp = fThreads->GetWaitingThread();
                tmp->SetMerger(&merger);
                tmp->SetData(3, t);
                tmp->Execute();
            }
            fThreads->Finish();
        }
        TFile file(to_string(fRunId) + (TString) "_s" + to_string(t) + ".root", "update");
        file.WriteObject(runHeader.get(), fRawRunHdrName.Data());
        file.WriteObject(metadata.get(), fMetadataName.Data());
        file.Close();
        if (t == -2)
            LOGF(info, "Finished writing weird events (spill %d)", t);
        else if (t == -1)
            LOGF(info, "Finished writing unfinished spill (spill %d)", t);
        else
            LOGF(info, "Finished writing spill %d", t);
    }
    ROOT::DisableImplicitMT();
    fRootFileOut->Close();
    Close();
    timer->Stop();
    LOGF(info, ANSI_COLOR_BLUE "Finished writing spills in %.3fs\n" ANSI_COLOR_RESET, timer->RealTime());
}

void BmnConverter::SeparateEventsBySpills()
{
    fThreads->Finish();
    LOGF(info, ANSI_COLOR_BLUE "Starting to split events by spills" ANSI_COLOR_RESET);
    TStopwatch* timer = new TStopwatch();
    timer->Start();
    vector<SysPoint> spillEnds;
    vector<SysPoint> spillStarts;
    for (auto t : fSpillMap) {
        spillStarts.push_back(t.second.start_ts);
        spillEnds.push_back(t.second.stop_ts);
        LOGF(info, "Spill:\n\tstart ts:%s\n\tstop  ts:%s", BmnFunctionSet::TimePoint2String(t.second.start_ts),
             BmnFunctionSet::TimePoint2String(t.second.stop_ts));
    }
    std::sort(spillEnds.begin(), spillEnds.end());
    //    for (size_t i = 0; i < spillEnds.size(); i++) {
    //        LOGF(info, "Spill end detected at: %s", spillEnds[i].AsString());
    //    }
    vector<TTree*> tmp;
    for (UInt_t i = 0; i < fThreadCnt; i++) {
        tmp.push_back(fThreads->GetThread(i)->fRawTree);
        tmp.push_back(fThreads->GetThread(i)->fRawTreeSpills);
    }
    vecTree.clear();
    vecTreeSpills.clear();
    for (size_t i = 0; i < tmp.size(); i += 2) {
        int n = tmp[i]->GetEntriesFast();
        TBranch* branch = tmp[i]->GetBranch("BmnEventHeader.");
        for (int j = 0; j < n; j++) {
            branch->GetEntry(j);
            vecTree.push_back(make_pair(
                GetSpillNumber(&spillEnds, &spillStarts, fThreads->GetThread(i / 2)->eventHeaderDAQ->GetEventTimeTS()),
                make_pair(i / 2, j)));
        }
    }
    for (size_t i = 1; i < tmp.size(); i += 2) {
        int n = tmp[i]->GetEntriesFast();
        TBranch* branch = tmp[i]->GetBranch("MSC");
        TBranch* branchT0 = tmp[i]->GetBranch("T0Raw");
        for (int j = 0; j < n; j++) {
            BmnConverterThread* th = fThreads->GetThread(i / 2);
            SysPoint ts{SysPoint::min()};
            branch->GetEntry(j);
            if (th->msc->GetEntriesFast() > 0) {
                ts = ((BmnMSCDigit<UChar_t>*)(th->msc->First()))->GetTime();
                LOGF(debug2, "branchMSC ts\t%s", BmnFunctionSet::TimePoint2String(ts));
            } else {
                branchT0->GetEntry(j);
                if (th->t0raw->GetEntriesFast() > 0) {
                    ts = ((BmnT0Raw<kT0_BIN_BLOCK_WORDS>*)(th->t0raw->First()))->GetTime();
                    LOGF(debug2, "branchT0 ts\t%s", BmnFunctionSet::TimePoint2String(ts));
                    ts -= 1s;   // temp crutches to fit into the spill time
                } else
                    continue;
            }
            vecTreeSpills.push_back(make_pair(GetSpillNumber(&spillEnds, &spillStarts, ts), make_pair(i / 2, j)));
            LOGF(debug2, "GetSpillNumber %d", GetSpillNumber(&spillEnds, &spillStarts, ts));
        }
    }
    for (UInt_t i = 0; i < fThreadCnt; i++) {
        fThreads->GetThread(i)->RegisterBranches();
        fThreads->GetThread(i)->RegisterSpillBranches();
    }
    std::sort(vecTree.begin(), vecTree.end());
    std::sort(vecTreeSpills.begin(), vecTreeSpills.end());
    runHeader->SetSpillMap(fSpillMap);
    timer->Stop();
    LOGF(info, ANSI_COLOR_BLUE "Finished splitting events by spills in %.3fs\n" ANSI_COLOR_RESET, timer->RealTime());
}

void BmnConverter::SeparateEventsBySpillsTM()
{
    fThreads->Finish();
    LOGF(info, "Starting separating events by spill");
    TStopwatch* timer = new TStopwatch();
    timer->Start();

    treeTimeMap.clear();
    treeTimeMapDiv.clear();

    vector<SysPoint> spillEnds;
    vector<SysPoint> spillStarts;
    for (auto t : fSpillMap) {
        spillStarts.push_back(t.second.start_ts);
        spillEnds.push_back(t.second.stop_ts);
        LOGF(info, "Spill:\n\tstart ts: %s\n\tstop  ts: %s", BmnFunctionSet::TimePoint2String(t.second.start_ts),
             BmnFunctionSet::TimePoint2String(t.second.stop_ts));
    }
    std::sort(spillEnds.begin(), spillEnds.end());
    for (size_t i = 0; i < spillEnds.size(); i++) {
        LOGF(info, "Spill end detected at: %s", BmnFunctionSet::TimePoint2String(spillEnds[i]));
    }

    for (uint32_t i = 0; i < fThreadCnt; i++) {
        BmnConverterThread* th = fThreads->GetThread(i);
        TTree* tree = th->fRawTree;
        int64_t n = tree->GetEntriesFast();
        TBranch* branch = tree->GetBranch("BmnEventHeader.");
        for (int64_t iEntry = 0; iEntry < n; iEntry++) {
            branch->GetEntry(iEntry);
            //            treeTimeMap.emplace(th->eventHeaderDAQ->GetEventTimeTP(), make_pair(tree, iEntry));
            SysPoint tp = th->eventHeaderDAQ->GetEventTimeTP();
            int32_t sn = GetSpillNumber(&spillEnds, &spillStarts, tp);
            treeTimeMapDiv[sn].emplace(tp, make_pair(tree, iEntry));
        }
    }
    for (const auto& [spill_id, mm] : treeTimeMapDiv) {
        //        const int32_t& spill_id = pmm.first;
        //        const auto& mm = pmm.second;
        LOGF(info, "t %d", spill_id);
        TFile file(to_string(fRunId) + (TString) "_s" + to_string(spill_id) + ".root", "recreate");
        TTree* sortedTree = mm.begin()->second.first->CloneTree(0);
        sortedTree->SetAutoSave(0);
        sortedTree->SetAutoFlush(0);
        sortedTree->SetBasketSize("*", 1073741824);
        if (!file.IsOpen()) {
            LOGF(error, "Couldn't access the output file!!!!!\n");
            break;   // return kBMNERROR;
        }
        LOGF(info, "TFile open");
        for (const pair<const SysPoint, TreePE>& el : mm) {
            const TreePE& pe = el.second;
            pe.first->GetEntry(pe.second);
            sortedTree->Fill();
        }
        file.WriteObject(runHeader.get(), fRawRunHdrName.Data());
        file.WriteObject(metadata.get(), fMetadataName.Data());
        file.Close();
        if (spill_id == -2)
            LOGF(info, "Finished writing weird events (spill %d)", spill_id);
        else if (spill_id == -1)
            LOGF(info, "Finished writing unfinished spill (spill %d)", spill_id);
        else
            LOGF(info, "Finished writing spill %d", spill_id);
    }

    Close();
    timer->Stop();
    LOGF(info, ANSI_COLOR_BLUE "Finished splitting events by spills in %.3fs\n" ANSI_COLOR_RESET, timer->RealTime());
}

void BmnConverter::ReproduceBranches(TTree* inTree, TTree* outTree)
{
    TObjArray* brList = inTree->GetListOfBranches();   // ioman->GetBranchNameList();
    if (inTree->GetEntry(0) == 0) {
        LOGF(error, "Input tree is empty! Recreating empty output...");
        //    for (Int_t i = 0; i < brList->GetEntries(); i++) {
        //        TString str = static_cast<TObjString*> (brList->At(i))->GetString();
        //        TObject *obj = ioman->GetObject(str);
        //        TClass *cl = TClass::GetClass(obj->ClassName());
        ////        printf("ClassName: %s\n", obj->ClassName());
        //        if (cl == TClonesArray::Class()) {
        //            TClonesArray * inTCA = static_cast<TClonesArray*> (obj);
        //            TClonesArray * newTCA = new TClonesArray(inTCA->GetClass());
        //            fTreeTemp->Branch(str.Data(), &newTCA);
        //            fArrVecIn.push_back(inTCA);
        //            fArrVec.push_back(newTCA);
        //
        //        } else {
        //            TNamed* inObj = static_cast<TNamed *> (obj);
        ////            printf(" in obj n: %s t: %s\n", inObj->GetName(), inObj->GetTitle());
        //            TObject* workObj = inObj->Clone(/*TString(inObj->GetName()) + "_clone." + str*/)/*funcNew(0)*/;
        ////            printf("new obj n: %s t: %s\n", workObj->GetName(), workObj->GetTitle());
        //            /*TBranch *b = */fTreeTemp->Branch(str.Data(), "TObject", workObj);
        ////            printf(" branch 0x%016lX\n", (uint64_t)b);
        //            fNamVecIn.push_back(inObj);
        //            fNamVec.push_back(workObj);
        //        }
        //        return;
    }
    for (Int_t i = 0; i < brList->GetEntries(); i++) {
        TBranch* br = static_cast<TBranch*>(brList->At(i));
        TString str(br->GetName());
        TObject* obj = *(reinterpret_cast<TObject**>(br->GetAddress()));
        LOGF(debug, "obj->ClassName() %s", obj->ClassName());
        TClass* cl = TClass::GetClass(obj->ClassName());
        LOGF(debug, "ClassName: %s", br->ClassName());
        if (cl == TClonesArray::Class()) {
            TClonesArray* inTCA = static_cast<TClonesArray*>(obj);
            LOGF(debug, "TCA->GetClass()->GetName() %s", inTCA->GetClass()->GetName());
            outTree->Branch(str.Data(), &inTCA);
            fArrVec.push_back(inTCA);

        } else {
            TNamed* inObj = static_cast<TNamed*>(obj);
            outTree->Branch(str.Data(), "TObject", inObj);
            fNamVec.push_back(inObj);
        }
    }
    return;
}

BmnStatus BmnConverter::FeedEvent(UInt_t* buf, UInt_t len, Int_t type)
{
    fEventId = buf[0];
    if (type == 1) {
        fCurEvType = kBMNPAYLOAD;
        QuickProcessEvent(buf, len);
        return kBMNSUCCESS;
    } else if (type == 2) {
        fCurEvType = kBMNSTAT;
        QuickConvertStatEvent(buf, len);
        return kBMNSUCCESS;
    } else
        return kBMNERROR;
}

BmnStatus BmnConverter::FeedFile(TString name, Bool_t getRunId, Bool_t getSubName)
{
    if (getRunId)
        fRunId = BmnRawDataDecoder::GetRunIdFromFile(name);
    if (getSubName)
        fSubName = BmnConverterTools::GetSubNameAfterRunId(name);
    fRawFileIn = fopen(name, "rb");
    if (fRawFileIn == nullptr) {
        LOGF(error, "Couldn't open file %s!!!!!\n", name.Data());
        return kBMNERROR;
    }
    LOGF(info, ANSI_COLOR_BLUE "Starting to convert file %s" ANSI_COLOR_RESET, name.Data());
    LOGF(info, "Period ID: %u,   Run ID: %u,   Subname: %s", fPeriodId, fRunId, fSubName.Data());
    //    if (!fRootFileOut)
    //        if (CreateTempFile() == kBMNERROR)
    //            return kBMNERROR;
    for (UInt_t i = 0; i < fThreadCnt; i++)
        fThreads->GetThread(i)->SetInitData(&vecTree, &vecTreeSpills);
    TStopwatch* timer = new TStopwatch();
    timer->Start();
    while ((ConvertRawToRootIterateFileRead() != kBMNERROR)) {
        //    while ((ConvertRawToRootIterateFileReadFaster() != kBMNERROR)) {
        // if ((fStartEventId == 0) && (fEventId))
        // fStartEventId = fEventId;
        //                if (cc++>10000)
        //                    break;
    }
    //    LOGF(info,"tq size %ld  ",fThreads->threadQueue.size());
    if (fCurWaitingThread) {   // crutch
        fCurWaitingThread->CancelReady();
        fCurWaitingThread = nullptr;
    }
    //    LOGF(info,"tq size %ld  ",fThreads->threadQueue.size());
    fThreads->Finish();
    timer->Stop();
    LOGF(info, ANSI_COLOR_BLUE "Converted file %s in %.3fs\n" ANSI_COLOR_RESET, name.Data(), timer->RealTime());
    std::fclose(fRawFileIn);
    return kBMNSUCCESS;
}

BmnStatus BmnConverter::ConvertRawToRootIterateFileRead()
{
    // TStopwatch sw;
    // sw.Start();
    //    printf("fMaxEvent  %lu, fNSignalEvents %u, fMaxEvent %lu\n", fMaxEvent, fNSignalEvents, fMaxEvent);
    fCurEvType = kBMNEMPTY;
    // if (fMaxEvent > 0 && fNSignalEvents >= fMaxEvent) return kBMNERROR;
    if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1)
        return kBMNERROR;
    // fCurentPositionRawFile = ftello64(fRawFileIn);
    // if (fCurentPositionRawFile >= fLengthRawFile) return kBMNERROR;
    switch (fDat) {
        case SYNC_EVENT:
        case SYNC_EVENT_OLD:
            fCurEvType = kBMNPAYLOAD;
            LOGF(debug2, ANSI_COLOR_BLUE "SYNC_EVENT %08X" ANSI_COLOR_RESET, fDat);
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
            if (fEventId <= 0) {
                return kBMNCONTINUE;   // skip bad events
            }
            QuickProcessEvent(data, fDat);
            // ProcessEvent(data, fDat);

            //            LOGF(trace, "evType %d", fCurEvType);
            //            LOGF(debug, "eventHeaderDAQ %p", (void*) eventHeaderDAQ);
            // LOGF(debug1, "isSpillStart %d", isSpillStart);
            // if (isSpillStart == kTRUE)
            //    isSpillStart = kFALSE;
            // fNTotalEvents++;
            // fNSignalEvents++;
            // nSpillEvents++;
            break;
        case SYNC_EOS:
            fCurEvType = kBMNEOS;
        case SYNC_STAT:
            LOGF(debug2, ANSI_COLOR_BLUE "SYNC_STAT %08X" ANSI_COLOR_RESET, fDat);
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
            // ConvertStatEvent(data, fDat);
            QuickConvertStatEvent(data, fDat);
            break;
        case SYNC_RUN_START:
            LOGF(debug, "RUN START");
        case SYNC_RUN_STOP:
            if (fDat == SYNC_RUN_STOP)
                LOGF(debug, "RUN STOP");
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1)
                return kBMNERROR;
            if (fread(data, 1, fDat, fRawFileIn) != fDat)
                return kBMNERROR;
            BmnConverterTools::ParseComplexTLV(data, fDat, fRunId);
            break;
        case SYNC_FILE_BEGIN:
            LOGF(debug, "FILE BEGIN");
        case SYNC_FILE_END:
            if (fDat == SYNC_FILE_END)
                LOGF(debug, "FILE END");
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
    return kBMNSUCCESS;
}

// BmnStatus BmnConverter::FeedSpill(TString name, Bool_t getRunId, Bool_t getSubName)
//{   // Can be rewritten to work in multiple threads
//     if (getRunId)
//         fRunId = BmnRawDataDecoder::GetRunIdFromFile(name);
//     if (getSubName)
//         fSubName = BmnRawDataDecoder::GetSubNameFromFile(name);
//     TFile* fileIn = new TFile(name, "read");
//     if (fileIn == nullptr) {
//         LOGF(error, "Couldn't open file %s!!!!!\n", name.Data());
//         return kBMNERROR;
//     }
//     LOGF(info, ANSI_COLOR_BLUE "Starting to recover spill file %s" ANSI_COLOR_RESET, name.Data());
//     LOGF(info, "Period ID: %u,   Run ID: %u,   Subname: %s", fPeriodId, fRunId, fSubName.Data());
//     if (!fRootFileOut->IsOpen()) {
//         delete fRootFileOut;
//         LOGF(error, "Couldn't access the output file!!!!!\n");
//         return kBMNERROR;
//     }
//     TTree* tree = (TTree*)(fileIn->Get("BMN_RAW"));
//     TTree* treeSpills = (TTree*)(fileIn->Get("BMN_RAW_SPILLS"));
//     if (!tree || !treeSpills) {
//         LOGF(error, "Couldn't find needed trees in the spill file\n");
//         return kBMNERROR;
//     }
//     TStopwatch* timer = new TStopwatch();
//     timer->Start();
//     fThreads->Finish();
//     Int_t sigN = tree->GetEntriesFast(), statN = treeSpills->GetEntriesFast();
//     Int_t totalSigN = 0, totalStatN = 0;
//     for (UInt_t i = 0; i < fThreadCnt; i++) {
//         BmnConverterThread* tmp = fThreads->GetThread(i);
//         Int_t curSigN = sigN / fThreadCnt + (sigN % fThreadCnt > i ? 1 : 0);
//         Int_t curStatN = statN / fThreadCnt + (statN % fThreadCnt > i ? 1 : 0);
//         tree->SetBranchAddress("BmnEventHeader.", &(tmp->eventHeaderDAQ));
//         tree->SetBranchAddress("SYNC", &(tmp->sync));
//         tree->SetBranchAddress("ADC32", &(tmp->adc32));
//         tree->SetBranchAddress("ADC64", &(tmp->adc64));
//         tree->SetBranchAddress("ADC128", &(tmp->adc128));
//         tree->SetBranchAddress("ADC", &(tmp->adc));
//         tree->SetBranchAddress("TDC", &(tmp->tdc));
//         tree->SetBranchAddress("TQDC_ADC", &(tmp->tqdc_adc));
//         tree->SetBranchAddress("TQDC_TDC", &(tmp->tqdc_tdc));
//         tree->SetBranchAddress("HRB", &(tmp->hrb));
//         treeSpills->SetBranchAddress("MSC", &(tmp->msc));
//         treeSpills->SetBranchAddress("T0Raw", &(tmp->t0raw));
//         for (int j = 0; j < curSigN; j++) {
//             tree->GetEntry(totalSigN++);
//             tmp->fRawTree->Fill();
//         }
//         for (int j = 0; j < curStatN; j++) {
//             treeSpills->GetEntry(totalStatN++);
//             tmp->fRawTreeSpills->Fill();
//         }
//         LOGF(info, "Finished copying events to thread %d", i);
//     }
//     fileIn->Close();
//     fRootFileOut->cd();
//     timer->Stop();
//     LOGF(info, ANSI_COLOR_BLUE "Recovered spill file %s in %.3fs\n" ANSI_COLOR_RESET, name.Data(),
//     timer->RealTime()); return kBMNSUCCESS;
// }

void BmnConverter::QuickConvertStatEvent(UInt_t* d, UInt_t& len)
{
    BmnConverterThread* tmp = fThreads->GetWaitingThread();
    tmp->SetData(2, len, d, fCurEvType, fRunId, fEventId);
    tmp->Execute();
}

void BmnConverter::QuickProcessEvent(UInt_t* d, UInt_t& len)
{
    BmnConverterThread* tmp = fThreads->GetWaitingThread();
    tmp->SetData(1, len, d, fCurEvType, fRunId, fEventId);
    tmp->Execute();
}

void BmnConverter::QuickConvertStatEvent(BmnConverterThread*& tmp, UInt_t& len)
{
    //    LOGF(info, "QuickConvertStatEvent thread %d", tmp->GetId());
    tmp->SetData(2, len, nullptr, fCurEvType, fRunId, fEventId);
    tmp->Execute();
    tmp = nullptr;
    //    LOGF(info, "QuickConvertStatEvent thread %d done", tmp->GetId());
}

void BmnConverter::QuickProcessEvent(BmnConverterThread*& tmp, UInt_t& len)
{
    //    LOGF(info, "QuickProcessEvent thread %d", tmp->GetId());
    tmp->SetData(1, len, nullptr, fCurEvType, fRunId, fEventId);
    tmp->Execute();
    //    LOGF(info, "QuickProcessEvent thread %d done", tmp->GetId());
    tmp = nullptr;
}

BmnStatus BmnConverter::ParseJsonTLV(UInt_t* d, UInt_t& len)
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
            //            printf("j[%d] : %s\n", key_cnt++, jkey.c_str());
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
                    std::ofstream outfile(Form("j_%s_%d_%9d_%09d_%02X_%08X_phase_%d.json", jkey.c_str(), fRunId, sec,
                                               ns, dh->DeviceId, dh->Serial, phase),
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

BmnStatus BmnConverter::InitUTCShift()
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
    utc_valid = TTimeStamp(2026, 1, 1, 0, 0, 1);
    return kBMNSUCCESS;
}

Int_t BmnConverter::GetUTCShift(TTimeStamp t)
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