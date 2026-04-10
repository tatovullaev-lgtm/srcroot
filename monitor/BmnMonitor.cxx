#include "BmnMonitor.h"

#include <chrono>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <time.h>
// ROOT
#include <TLatex.h>
#include <TPaveText.h>
// BmnRoot
#include "BmnHistCsc.h"
#include "BmnHistECAL.h"
#include "BmnHistFHCal.h"
#include "BmnHistGem.h"
#include "BmnHistHodo.h"
#include "BmnHistNdet.h"
#include "BmnHistScWall.h"
#include "BmnHistSiBT.h"
#include "BmnHistSilicon.h"
#include "BmnHistStat.h"
#include "BmnHistToF.h"
#include "BmnHistToF700.h"
#include "BmnHistTrigger.h"
#include "BmnHistVsp.h"
#include "BmnHistZDC.h"

#include <ElogRecord.h>
#include <UniRun.h>

BmnMonitor::BmnMonitor()
    : _ctx(nullptr)
    , fRecoTree(nullptr)
    , fRecoTree4Show(nullptr)
    , fHistOut(nullptr)
    , fHistOutTemp(nullptr)
    , fServer(nullptr)
    , fDigiArrays(nullptr)
    , keepWorking(kTRUE)
    , fSetup(kBMNSETUP)
    , fRunID(0)
    , fEvents(0)
    , fNEventsSinceUpdate(0)
    , fState(kBMNRECON)
    , itersToUpdate(1000)
{
    TString name = "infoCanvas";
    infoCanvas = new TCanvas(name, name, 3 * PAD_WIDTH, 1 * PAD_HEIGHT * 2 / 3);
    infoCanvas->Divide(3, 1);
    //    refList = new TList();
    //    refList->SetName("refList");
    refTable = new TList();
    refTable->SetName("refTable");
    runPub = new TList();
    runPub->SetName("CurRun");
    CurRun = new BmnRunInfo();
    runPub->Add((TObject*)CurRun);
}

BmnMonitor::~BmnMonitor()
{
    DBG("started")
    for (auto h : bhVec)
        if (h)
            delete h;
    bhVec.clear();
    //    delete fRecoTree;
    if (fHistOut)
        delete fHistOut;
    if (infoCanvas)
        delete infoCanvas;
    for (auto h : bhVec4show)
        if (h)
            delete h;
    bhVec4show.clear();
    if (fServer)
        delete fServer;
    if (_ctx) {
        zmq_ctx_destroy(_ctx);
        _ctx = nullptr;
    }
    if (CurRun)
        delete CurRun;
    if (runPub)
        delete runPub;
}

void BmnMonitor::MonitorStreamZ(TString dirname, TString refDir, TString decoAddr, Int_t webPort)
{
    DBG("started")
    fState = kBMNWAIT;
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_SUB);
    if (_decoSocket == nullptr) {
        DBGERR("zmq socket")
        return;
    }
    if (zmq_setsockopt(_decoSocket, ZMQ_SUBSCRIBE, nullptr, 0) == -1) {
        DBGERR("zmq subscribe")
        return;
    }
    Int_t rcvBuf = MAX_BUF_LEN;
    if (zmq_setsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, sizeof(rcvBuf)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
    if (zmq_setsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, sizeof(rcvBuf)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
    _webPort = webPort;
    gSystem->ExpandPathName(dirname);
    printf("dir %s\n", dirname.Data());
    _curDir = dirname + "/";
    if (refDir == "")
        _refDir = _curDir;
    else {
        gSystem->ExpandPathName(refDir);
        _refDir = refDir + "/";
    }
    printf("Ref dir set to %s\n", _refDir.Data());
    InitServer();
    RegisterAll();
    for (auto h : bhVec4show)
        h->DrawBoth();
    fRawDecoAddr = decoAddr;
    TString conStr = Form("tcp://%s:%d", fRawDecoAddr.Data(), RAW_DECODER_SOCKET_PORT);
    if (zmq_connect(_decoSocket, conStr.Data()) != 0) {
        DBGERR("zmq connect")
        return;
    }
    printf("Monitor listens to %s\n", conStr.Data());
    zmq_msg_t msg;
    TBufferFile t(TBuffer::kRead);
    t.SetReadMode();
    Int_t frame_size = 0;
    decoTimeout = 0;
    keepWorking = kTRUE;
    while (keepWorking) {
        //        gSystem->ProcessEvents();
        fServer->ProcessRequests();
        zmq_msg_init(&msg);
        frame_size = zmq_msg_recv(&msg, _decoSocket, ZMQ_DONTWAIT);   // ZMQ_DONTWAIT
        //    printf("frame_size %d\n", frame_size);
        if (frame_size == -1) {
            if (errno == EAGAIN) {
                usleep(DECO_SOCK_WAIT_PERIOD * 1000);
                decoTimeout += DECO_SOCK_WAIT_PERIOD;
                if ((decoTimeout > DECO_SOCK_WAIT_LIMIT) && (fState == kBMNWORK)) {
                    // FinishRun();
                    fState = kBMNWAIT;
                    // keepWorking = false; // @TODO Remove
                    // fServer->SetTimer(50, kTRUE);
                    DBG("state changed to kBMNWAIT")
                }
            } else {
                printf("Receive error № %d #%s\n", errno, zmq_strerror(errno));
                return;
            }
        } else {
            decoTimeout = 0;
            t.Reset();
            t.SetBuffer(zmq_msg_data(&msg), zmq_msg_size(&msg));
            fDigiArrays = (DigiArrays*)(t.ReadObject(DigiArrays::Class()));
            //            gSystem->ProcessEvents();
            //            if (fDigiArrays->header->GetEntriesFast() > 0) {
            BmnEventHeader* head = fDigiArrays->header;
            uint32_t runID = head->GetRunId();
            switch (fState) {
                case kBMNWAIT:
                    FinishRun();
                    fRunID = runID;
                    CreateFile(fRunID);
                    DBG("state changed to kBMNWORK")
                    // fServer->SetTimer(0, kTRUE);
                    fState = kBMNWORK;
                    ProcessDigi(0);
                    break;
                case kBMNWORK:
                    if (fRunID != runID) {
                        printf("change run %5u to %5u\n", fRunID, runID);
                        FinishRun();
                        fRunID = runID;
                        CreateFile(fRunID);
                    }
                    ProcessDigi(0);
                    break;
                default:
                    break;
            }
            //            } else {
            //                printf("No header??\n");
            //            }
            fDigiArrays->Clear();
            delete fDigiArrays;
            t.DetachBuffer();
        }
        zmq_msg_close(&msg);
    }
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = nullptr;
}

void BmnMonitor::InitServer()
{
    TString mode = "fastcgi";   // "http"
    TString cgiStr = Form("%s:%d;noglobal;cors", mode.Data(), _webPort);
    if (gSystem->AccessPathName(_curDir + "auth.htdigest") != 0) {
        printf("Authorization file not found\nStarting server without authorization\n");
        fServer = new THttpServer(cgiStr.Data());
    } else
        fServer = new THttpServer(TString(cgiStr + "?auth_file=auth.htdigest&auth_domain=root").Data());
    fServer->SetTimer(50, kTRUE);
}

void BmnMonitor::InitHistVectors(vector<BmnHist*>& vec, TString refName)
{
    vec.push_back(new BmnHistGem(refName + "GEM", _curDir, fPeriodID, fSetup));
    vec.push_back(new BmnHistVsp(refName + "VSP", _curDir, fPeriodID, fSetup));
    vec.push_back(new BmnHistSilicon(refName + "Silicon", _curDir, fPeriodID, fSetup));
    //    vec.push_back(new BmnHistZDC(refName + "ZDC", _curDir));
    //    vec.push_back(new BmnHistECAL(refName + "ECAL", _curDir));
    vec.push_back(new BmnHistToF(refName + "ToF400", _curDir));
    vec.push_back(new BmnHistToF700(refName + "ToF700", _curDir));
    vec.push_back(new BmnHistTrigger(refName + "Triggers", _curDir, fPeriodID, fSetup));
    vec.push_back(new BmnHistScWall(refName + "ScWall", _curDir, fPeriodID, fSetup));
    vec.push_back(new BmnHistCsc(refName + "CSC", _curDir, fPeriodID, fSetup));
    //    vec.push_back(new BmnHistFHCal(refName + "FHCal", _curDir, fPeriodID, fSetup));
    vec.push_back(new BmnHistHodo(refName + "Hodo", _curDir));
    vec.push_back(new BmnHistNdet(refName + "Ndet", _curDir, fPeriodID, fSetup));
    vec.push_back(new BmnHistSiBT(refName + "SiBT", _curDir, fPeriodID));
    vec.push_back(new BmnHistStat(refName + "Statistics", _curDir));
}

BmnStatus BmnMonitor::CreateFile(uint32_t runID)
{
    fEvents = 0;
    UpdateRuns();
    TString outHistName = Form("%s/bmn_run%04u_hist.root", _curDir.Data(), runID);
    fHistOut = new TFile(outHistName, "recreate");
    if (fHistOut)
        printf("file %s created\n", outHistName.Data());
    if (fHistOutTemp)
        printf("file tempo.root created\n");

    TString refName = Form("ref%06u_", fRunID);
    InitHistVectors(bhVec, refName);

    for (auto h : bhVec) {
        h->SetDir(fHistOut, fRecoTree);
        h->SetPeriodID(fPeriodID);
    }
    for (auto h : bhVec4show) {
        //        h->SetDir(fHistOutTemp, fRecoTree4Show);
        h->ClearRefRun();
        h->Reset();
        h->SetPeriodID(fPeriodID);
    }
    refTable->Clear();
    runPub->Clear();
    return kBMNSUCCESS;
}

void BmnMonitor::ProcessDigi(Int_t iEv)
{
    // histograms fill//
    fEvents++;
    fNEventsSinceUpdate++;
    BmnEventHeader* head = fDigiArrays->header;
    //    printf("Run %8u Event %8u  EType %d\n", head->GetRunId(), head->GetEventId(), head->GetEventType());
    //    printf("fDigiArrays.msc->GetEntriesFast() %d \n", fDigiArrays->msc->GetEntriesFast());
    for (auto h : bhVec)
        if (h) {
            fServer->ProcessRequests();
            // clock_t prev = clock();
            h->FillFromDigi(fDigiArrays);
            // clock_t now = clock();
            //             if (head->GetEventType() == kBMNSTAT)
            //                 printf("%15s fill %f\n",h->GetName(), (now - prev)/ (double) CLOCKS_PER_SEC);
        }
    // Fill data Tree //
    if (fRecoTree)
        fRecoTree->Fill();
    // fill histograms what will be shown on the site//
    for (auto h : bhVec4show)
        if (h) {
            fServer->ProcessRequests();
            h->FillFromDigi(fDigiArrays);
        }
    if ((fNEventsSinceUpdate > 200)
        && ((head->GetEventType() == kBMNPAYLOAD) || (head->GetEventType() == kBMNPEDESTAL)))
    {
        // print info canvas //
        // infoCanvas->Clear();
        TVirtualPad* pad = infoCanvas->cd(1);
        pad->Clear();
        TString runType;
        //        switch (head->GetTrigType()) {
        //            case kBMNBEAM:
        //                runType = "beam";
        //                break;
        //            case kBMNMINBIAS:
        //                runType = "target";
        //                break;
        //            default:
        //                runType = "???";
        //                break;
        //        }
        TLatex Tl;
        Tl.SetTextAlign(12);
        Tl.SetTextSize(0.16);
        TString shownID = head->GetRunId() == UNKNOWN_RUNID ? " o_O" : TString::UItoa(head->GetRunId(), 10);
        Tl.DrawLatex(0.3, 0.9, Form("Run: %s", shownID.Data()));
        Tl.DrawLatex(0.3, 0.6, Form("Event: %d", head->GetEventId()));
        Tl.DrawLatex(0.3, 0.3, Form("Run Type: %s", runType.Data()));
        Tl.Draw();
        pad->Update();
        pad->Modified();
        pad = infoCanvas->cd(2);
        pad->Clear();
        //        TLatex l2;
        Tl.SetTextAlign(12);
        Tl.SetTextSize(0.16);
        Tl.DrawLatex(0.3, 0.9, Form("Energy: %1.2f", CurRun->GetEnergy()));
        Tl.DrawLatex(0.3, 0.7, Form("Beam: %s", CurRun->GetBeamParticle().Data()));
        Tl.DrawLatex(0.3, 0.5, Form("Target: %s", CurRun->GetTargetParticle().Data()));
        Tl.DrawLatex(0.3, 0.3, Form("Field Voltage: %1.2f", CurRun->GetFieldVoltage()));
        Tl.Draw();
        pad->Update();
        pad->Modified();
        for (auto h : bhVec4show)
            h->DrawBoth();
        if (fEvents % 1000 == 0)
            if (refTable->GetEntries() == 0)
                UpdateRuns();
        fNEventsSinceUpdate = 0;
    }
}

void BmnMonitor::RegisterAll()
{
    InitHistVectors(bhVec4show);
    fServer->Register("/", infoCanvas);
    // fServer->Register("/", refList);
    fServer->Register("/", refTable);
    fServer->Register("/", runPub);
    for (auto h : bhVec4show) {
        h->SetDir(nullptr, nullptr);
        h->Register(fServer);
        h->SetRefPath(_refDir);
        h->SetPeriodID(fPeriodID);
    }
}

void BmnMonitor::UpdateRuns()
{
    //    struct dirent **namelist;
    //    TPRegexp re(".*bmn_run0*(\\d+)_hist.root");
    //    Int_t n;
    //    refList->Clear();
    //    n = scandir(_refDir, &namelist, 0, versionsort);
    //    if (n < 0)
    //        perror("scandir");
    //    else {
    //        for (Int_t i = 0; i < n; ++i) {
    //            TObjArray *subStr = re.MatchS(namelist[i]->d_name);
    //            if (subStr->GetEntriesFast() > 1)
    //                refList->Add((TObjString*) subStr->At(1));
    //            free(namelist[i]);
    //            subStr->Clear();
    //        }
    //        free(namelist);
    //    }

    if (refTable->GetEntries())
        return;
    refTable->Clear();
    runPub->Clear();
    TObjArray* refRuns = GetAlikeRunsByUniDB(fPeriodID, fRunID);
    if (refRuns == nullptr) {
        fprintf(stderr, "Ref list is empty!\n");
        delete CurRun;
        CurRun = new BmnRunInfo();
        CurRun->SetRunNumber(fRunID);
        runPub->Add((TObject*)CurRun);
        return;
    }
    runPub->Add((TObject*)CurRun);
    for (Int_t iRun = 0; iRun < refRuns->GetEntriesFast(); iRun++) {
        UniRun* run = (UniRun*)refRuns->At(iRun);
        BmnRunInfo* runInfo = new BmnRunInfo(run);
        refTable->Add((TObject*)runInfo);
        printf("run %04d Energy %f Voltage %f Beam %s Target %s Date %d\n", run->GetRunNumber(),
               run->GetEnergy() ? *run->GetEnergy() : -1, run->GetFieldVoltage() ? *run->GetFieldVoltage() : -1,
               run->GetBeamParticle().Data(), run->GetTargetParticle() ? (*run->GetTargetParticle()).Data() : "",
               run->GetStartDatetime().GetDate());
    }
    refRuns->Delete();
    delete refRuns;
}

void BmnMonitor::FinishRun()
{
    DBG("started")
    if (fHistOut) {
        printf("fHistOut  Write result = %d\n", fHistOut->Write());
        fHistOut->Close();
        fHistOut = nullptr;
    }
    for (auto h : bhVec)
        if (h)
            delete h;
    bhVec.clear();
}

TObjArray* BmnMonitor::GetAlikeRunsByElog(uint32_t periodID, uint32_t runID)
{
    if (runID == UNKNOWN_RUNID)
        return nullptr;
    TPRegexp reElementName(".*(\\w+).*");
    TString beamParticle = "";
    TString targetParticle = "";
    Double_t beamEnergy = 0;
    Double_t I_SP41 = 0;
    TObjArray* recs = ElogRecord::GetRecords(periodID, runID);
    if (recs == nullptr) {
        fprintf(stderr, "Run not found in ELOG!\n");
        return nullptr;
    }
    for (Int_t iRec = 0; iRec < recs->GetEntriesFast(); iRec++) {
        ElogRecord* rec = (ElogRecord*)recs->At(iRec);
        TObjArray* strings = reElementName.MatchS(rec->GetBeam()->Data());
        if (strings->GetEntriesFast() > 1)
            beamParticle = ((TObjString*)strings->At(1))->GetString();
        strings->Clear("C");
        strings = reElementName.MatchS(rec->GetTarget()->Data());
        if (strings->GetEntriesFast() > 1)
            targetParticle = ((TObjString*)strings->At(1))->GetString();
        strings->Clear("C");
        beamEnergy = *rec->GetEnergy() > 0 ? *rec->GetEnergy() : 0;
        I_SP41 = *rec->GetSp41() > 0 ? *rec->GetSp41() : 0;
    }
    if (beamParticle == "" || targetParticle == "" || beamEnergy == 0 || I_SP41 == 0) {
        fprintf(stderr, "Not enough run data in ELOG!\n");
        return nullptr;
    }
    printf("search  Energy %f I %f beam %s target %s\n\n", beamEnergy, I_SP41, beamParticle.Data(),
           targetParticle.Data());
    TObjArray arrayConditions;
    UniSearchCondition* searchCondition = new UniSearchCondition(UniSearchCondition::columnBeamParticle,
                                                                 UniSearchCondition::conditionEqual, beamParticle);
    arrayConditions.Add((TObject*)searchCondition);
    searchCondition = new UniSearchCondition(UniSearchCondition::columnTargetParticle,
                                             UniSearchCondition::conditionEqual, targetParticle);
    arrayConditions.Add((TObject*)searchCondition);
    searchCondition =
        new UniSearchCondition(UniSearchCondition::columnEnergy, UniSearchCondition::conditionEqual, beamEnergy);
    arrayConditions.Add((TObject*)searchCondition);
    //    searchCondition = new UniSearchCondition(colu, conditionEqual, beamEnergy);
    //    arrayConditions.Add((TObject*) searchCondition);

    TObjArray* refRuns = UniRun::Search(arrayConditions);
    arrayConditions.SetOwner(kTRUE);
    arrayConditions.Delete();
    for (Int_t iRun = 0; iRun < refRuns->GetEntriesFast(); iRun++) {
        UniRun* run = (UniRun*)refRuns->At(iRun);
        printf("run %04d Energy %f Voltage %f Date %d\n", run->GetRunNumber(), *run->GetEnergy(),
               *run->GetFieldVoltage(), run->GetStartDatetime().GetDate());
    }
    refRuns->Delete();

    return nullptr;   // FIXME
}

TObjArray* BmnMonitor::GetAlikeRunsByUniDB(uint32_t periodID, uint32_t runID)
{
    if (runID == UNKNOWN_RUNID)
        return nullptr;
    printf("getalike \n");
    unique_ptr<UniRun> Run{UniRun::GetRun(periodID, runID)};
    printf("getalike request returned\n");
    if (Run.get() == nullptr) {
        //        fprintf(stderr, "Run %d not found in UniDB!\n", runID);
        return nullptr;
    }
    if (!(Run->GetFieldVoltage() && Run->GetEnergy())) {
        fprintf(stderr, "Not enough info on current run!\n");
        return nullptr;
    }
    if (CurRun)
        delete CurRun;
    CurRun = new BmnRunInfo(Run.get());
    TObjArray arrayConditions;
    TString beamParticle = Run->GetBeamParticle();
    printf("Beam particle %s\n", Run->GetBeamParticle().Data());
    UniSearchCondition* searchCondition = new UniSearchCondition(UniSearchCondition::columnBeamParticle,
                                                                 UniSearchCondition::conditionEqual, beamParticle);
    arrayConditions.Add((TObject*)searchCondition);
    TString targetParticle = Run->GetTargetParticle() ? *Run->GetTargetParticle() : "";
    printf("Target particle %s\n", targetParticle.Data());
    searchCondition = new UniSearchCondition(UniSearchCondition::columnTargetParticle,
                                             UniSearchCondition::conditionEqual, targetParticle);
    arrayConditions.Add((TObject*)searchCondition);
    if (Run->GetEnergy()) {
        printf("Beam Energy %f\n", *Run->GetEnergy());
        Double_t beamEnergy = *Run->GetEnergy();
        searchCondition =
            new UniSearchCondition(UniSearchCondition::columnEnergy, UniSearchCondition::conditionEqual, beamEnergy);
        arrayConditions.Add((TObject*)searchCondition);
    }
    if (Run->GetFieldVoltage()) {
        printf("Field voltage %f\n", *Run->GetFieldVoltage());
        Double_t V_SP41 = *Run->GetFieldVoltage();
        searchCondition = new UniSearchCondition(UniSearchCondition::columnFieldVoltage,
                                                 UniSearchCondition::conditionLessOrEqual, V_SP41 * 1.15);
        arrayConditions.Add((TObject*)searchCondition);
        searchCondition = new UniSearchCondition(UniSearchCondition::columnFieldVoltage,
                                                 UniSearchCondition::conditionGreaterOrEqual, V_SP41 * 0.85);
        arrayConditions.Add((TObject*)searchCondition);
    }
    // delete Run;
    printf("search\n");
    TObjArray* refRuns = UniRun::Search(arrayConditions);
    printf("search request returned\n");
    arrayConditions.SetOwner(kTRUE);
    arrayConditions.Delete();
    return refRuns;
}
