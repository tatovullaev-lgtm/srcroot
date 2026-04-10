/**
 * \file BmnTrackingQaExp.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru> - disentangle Exp and MC processing
 * \date 2007-2022
 */

#include "BmnTrackingQaExp.h"

#include "BmnAcceptanceFunction.h"
#include "BmnDchHit.h"
#include "BmnDchTrack.h"
#include "BmnEnums.h"
#include "BmnGemStripHit.h"
#include "BmnGlobalTrack.h"
#include "BmnHistManager.h"
#include "BmnMath.h"
#include "BmnSiBTHit.h"
#include "BmnSiliconHit.h"
#include "BmnTofHit.h"
#include "BmnUtils.h"
#include "CbmBaseHit.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "FairRunAna.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2F.h"

#include <fstream>
#include <iostream>

using lit::FindAndReplace;
using lit::Split;

BmnTrackingQaExp::BmnTrackingQaExp(TString name, UInt_t periodId, BmnSetup stp)
    : BmnQaBase(name, 1)
    , fPeriodId(periodId)
    , fRunId(0)
    , fSetup(stp)
    , fGlobalTracks(nullptr)
    , fStsTracks(nullptr)
    , fSiBTTracks(nullptr)
    , fStsHits(nullptr)
    , fGemHits(nullptr)
    , fSilHits(nullptr)
    , fSiBTHits(nullptr)
    , fTof400Hits(nullptr)
    , fTof700Hits(nullptr)
    , fCscHits(nullptr)
    , fDchTracks(nullptr)
    , fDstEventHeader(nullptr)
    ,
    //  fConfigGem(gemConf),
    //  fConfigSil(silConf),
    //  fConfigCsc(cscConf),
    fNItersSinceUpdate(0)
{}

BmnTrackingQaExp::~BmnTrackingQaExp() {}

InitStatus BmnTrackingQaExp::Init()
{
    fHM = new BmnHistManager();
    fReport = new BmnTrackingQaExpReport(fOutName);
    fReport->SetHM(fHM);
    if (fMonitorMode) {
        fReport->SetMonitorMode(fMonitorMode);
        fReport->SetObjServer(fServer);
        fTicksLastUpdate = chrono::system_clock::now();
    } else {
    }
    SetDefaultDrawStyle();
    ReadDataBranches();
    CreateHistograms();
    if (fMonitorMode) {
        fReport->CallDraw();
        fReport->Register("/");
        RegisterCommands();
        //        fServer->SetTimer(50, kFALSE);
    } else {
    }
    return kSUCCESS;
}

void BmnTrackingQaExp::Exec(Option_t* opt)
{
    if (fEventNo % 5000 == 0)
        printf("EventCnt: %8d  EventId: %8d  RunId: %6u\n", fEventNo, fEventId, fRunId);
    fEventNo++;
    ProcessGlobal();
    if (fMonitorMode) {
        fNItersSinceUpdate++;
        fServer->ProcessRequests();
        chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();
        chrono::seconds time = chrono::duration_cast<chrono::seconds>(now - fTicksLastUpdate);
        // time_t tt = chrono::system_clock::to_time_t(now);
        //         printf("\ntime %s\n", ctime(&tt));
        if ((time > fTimeToUpdate) || (fNItersSinceUpdate > fNItersToUpdate)) {
            fReport->CallDraw();
            if (fVerbose)
                printf("Draw! iters %d\n", fNItersSinceUpdate);
            fTicksLastUpdate = now;
            fNItersSinceUpdate = 0;
        }
    }
    fPrevRunId = fRunId;
}

void BmnTrackingQaExp::Finish()
{
    //    fHM->WriteToFile();
    if (!fMonitorMode) {
        //        fReport = new BmnTrackingQaExpReport(fOutName);
        fReport->CallDraw();
        fReport->Create(fOutputDir);
    } else
        fReport->CallDraw();
    //        fServer->SetTimer(50, kFALSE);
}

void BmnTrackingQaExp::ReadDataBranches()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (nullptr == ioman)
        Fatal("Init", "BmnRootManager is not instantiated");

    fGlobalTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
    fStsTracks = (TClonesArray*)ioman->GetObject("StsVector");

    fStsHits = (TClonesArray*)ioman->GetObject("StsHit");

    fTof400Hits = (TClonesArray*)ioman->GetObject("BmnTof400Hit");
    fTof700Hits = (TClonesArray*)ioman->GetObject("BmnTof700Hit");
    fDchTracks = (TClonesArray*)ioman->GetObject("BmnDchTrack");
    fDstEventHeader = static_cast<DstEventHeader*>(ioman->GetObject("DstEventHeader."));

    if (fInnerTrackerSetup[kSILICON]) {
        fSilHits = (TClonesArray*)ioman->GetObject("BmnSiliconHit");
        //        fSilDetector = make_unique<BmnSiliconStationSet>(fConfigSil);
        fSilDetector = BmnSiliconStationSet::Create(fPeriodId, fSetup);
        fReport->AddStationSet(kSILICON, static_cast<void*>(fSilDetector.get()));
    }

    if (fInnerTrackerSetup[kGEM]) {
        fGemHits = (TClonesArray*)ioman->GetObject("BmnGemStripHit");
        //        fGemDetector = make_unique<BmnGemStripStationSet>(fConfigGem);
        fGemDetector = BmnGemStripStationSet::Create(fPeriodId, fSetup);
        static_cast<BmnTrackingQaExpReport*>(fReport)->AddStationSet(kGEM, static_cast<void*>(fGemDetector.get()));
    }

    if (fInnerTrackerSetup[kCSC]) {
        fCscHits = (TClonesArray*)ioman->GetObject("BmnCSCHit");
        //        fCscDetector = make_unique<BmnCSCStationSet>(fConfigCsc);
        fCscDetector = BmnCSCStationSet::Create(fPeriodId, fSetup);
        cout << fCscDetector->GetNStations() << " stations" << endl;
        static_cast<BmnTrackingQaExpReport*>(fReport)->AddStationSet(kCSC, static_cast<void*>(fCscDetector.get()));
    }

    if (fInnerTrackerSetup[kCSC]) {
        fVspHits = (TClonesArray*)ioman->GetObject("BmnVSPHit");
        //        fCscDetector = make_unique<BmnCSCStationSet>(fConfigCsc);
        fVspDetector = BmnVSPStationSet::Create(fPeriodId, fSetup);
        cout << fVspDetector->GetNStations() << " stations" << endl;
        static_cast<BmnTrackingQaExpReport*>(fReport)->AddStationSet(kVSP, static_cast<void*>(fVspDetector.get()));
    }

    fSiBTHits = (TClonesArray*)ioman->GetObject("BmnSiBTHit");
    fSiBTTracks = (TClonesArray*)ioman->GetObject("BmnBeamTrack");

    fVertexL1 = (CbmVertex*)ioman->GetObject("PrimaryVertex.");

    printf("\nBRANCHES READ\n\n");
}

void BmnTrackingQaExp::CreateHistograms()
{
    // Number of points distributions
    // const Float_t minNofPoints = -0.5;

    // Create number of object histograms
    CreateH1("NGlobTr", "Number of Global tracks", "Counter", 20, 0, 20);
    CreateH1("NInnTr", "Number of Inner tracks", "Counter", 20, 0, 20);

    CreateH1("NGemHits", "Number of GEM hits", "Counter", 50, 0, 50);
    CreateH1("NVspHits", "Number of VSP hits", "Counter", 50, 0, 50);
    CreateH1("NSilHits", "Number of Si hits", "Counter", 50, 0, 50);
    CreateH1("NTof400Hits", "Number of Tof400 hits", "Counter", 50, 0, 50);
    CreateH1("NTof700Hits", "Number of Tof700 hits", "Counter", 200, 0, 200);
    CreateH1("NsCSC1Hits", "Number of hits in small CSC1", "Counter", 50, 0, 50);
    CreateH1("NsCSC2Hits", "Number of hits in small CSC2", "Counter", 50, 0, 50);
    CreateH1("NsCSC3Hits", "Number of hits in small CSC3", "Counter", 50, 0, 50);
    CreateH1("NsCSC4Hits", "Number of hits in small CSC4", "Counter", 50, 0, 50);
    CreateH1("NlCSC5Hits", "Number of hits in 1st large CSC", "Counter", 50, 0, 50);
    CreateH1("NlCSC6Hits", "Number of hits in 2nd large CSC", "Counter", 50, 0, 50);
    CreateH1("NSiBTHits", "Number of SiBT hits", "Counter", 50, 0, 50);

    CreateH1("VertX", "V_{x}, cm", "Counter", 200, -5.0, 5.0);
    CreateH1("VertY", "V_{y}, cm", "Counter", 200, -5.0, 5.0);
    CreateH1("VertZ", "V_{z}, cm", "Counter", 200, -10.0, 10.0);
    CreateH2("VertXY", "V_{x}, cm", "V_{y}, cm", "", 200, -5.0, 5.0, 200, -5.0, 5.0);
    CreateH2("VertZX", "V_{z}, cm", "V_{x}, cm", "", 200, -20.0, 20.0, 200, -5.0, 5.0);
    CreateH2("VertZY", "V_{z}, cm", "V_{y}, cm", "", 200, -20.0, 20.0, 200, -5.0, 5.0);
    CreateH2("VertX_vs_Ntracks", "Number of tracks", "V_{x}, cm", "", 20, 0, 20, 200, -5.0, 5.0);
    CreateH2("VertY_vs_Ntracks", "Number of tracks", "V_{y}, cm", "", 20, 0, 20, 200, -5.0, 5.0);
    CreateH2("VertZ_vs_Ntracks", "Number of tracks", "V_{z}, cm", "", 20, 0, 20, 200, -10.0, 10.0);
    CreateH1("NtrInVertex", "Number of tracks in vertex", "Counter", 10, 0.0, 10.0);

    CreateH1("Rigidity", "P/Q, GeV/c/Q", "", 200, -10.0, 10.0);

    CreateH2("banana_tof400", "P_{rec}/Q, GeV/c", "#beta_{rec}", "", 200, -4, 10, 200, 0.0, 1.1);
    CreateH2("banana_tof700", "P_{rec}/Q, GeV/c", "#beta_{rec}", "", 200, -4, 10, 200, 0.0, 1.1);
    // SiBT
    //  all hits in Event
    CreateH2("x_vs_y_SiBT_st0", "x cm", "y cm", "", 250, -5, 5, 250, -5, 5);
    CreateH2("x_vs_y_SiBT_st1", "x cm", "y cm", "", 250, -5, 5, 250, -5, 5);
    CreateH2("x_vs_y_SiBT_st2", "x cm", "y cm", "", 250, -5, 5, 250, -5, 5);
    // mean position of hits weighted with signals in the layers
    CreateH2("x_vs_y_SiBT_st0_meanInEvent", "x cm", "y cm", "", 250, -5, 5, 250, -5, 5);
    CreateH2("x_vs_y_SiBT_st1_meanInEvent", "x cm", "y cm", "", 250, -5, 5, 250, -5, 5);
    CreateH2("x_vs_y_SiBT_st2_meanInEvent", "x cm", "y cm", "", 250, -5, 5, 250, -5, 5);
    // number of hits in event for each station
    CreateH1("NumberOfHitsInEvent_st0", "NHits", "", 20, 0, 20);
    CreateH1("NumberOfHitsInEvent_st1", "NHits", "", 20, 0, 20);
    CreateH1("NumberOfHitsInEvent_st2", "NHits", "", 20, 0, 20);
    // tracks coordinates and angles
    // CreateH2("SiBTTracks_FirstXY", "x cm", "y cm", "", 200, -10, 10, 200, -10, 10);
    // CreateH2("SiBTTracks_LastXY", "x cm", "y cm", "", 200, -10, 10, 200, -10, 10);
    // CreateH2("SiBTTracksFirstTx_Ty", "Tx", "Ty", "", 150, -0.1, 0.1, 150, -0.1, 0.1);
    // CreateH2("SiBTTracksLastTx_Ty", "Tx", "Ty", "", 150, -0.1, 0.1, 150, -0.1, 0.1);
    CreateH2("VertexX_vs_SiBTTracksX", "vertex: x cm", "sibt: x cm", "", 200, -5, 5, 200, -5, 5);
    CreateH2("VertexY_vs_SiBTTracksY", "vertex: y cm", "sibt: y cm", "", 200, -5, 5, 200, -5, 5);

    // hits residuals
    for (Int_t iSt = 0; iSt < 9; ++iSt) {
        CreateH1(Form("ResX_%dst", iSt), "ResX, cm", "Counter", 100, -0.5, 0.5);
        CreateH1(Form("ResY_%dst", iSt), "ResY, cm", "Counter", 100, -0.5, 0.5);
    }
    if (fInnerTrackerSetup[kSILICON])
        CreateStripHits(fSilDetector, kSILICON);
    if (fInnerTrackerSetup[kGEM])
        CreateStripHits(fGemDetector, kGEM);
    if (fInnerTrackerSetup[kCSC])
        CreateStripHits(fCscDetector, kCSC);
    if (fInnerTrackerSetup[kVSP])
        CreateStripHits(fVspDetector, kVSP);
    CreateTofHits(kTOF1);
    CreateTofHits(kTOF701);

    printf("\nHISTOGRAMS CREATED!\n\n");
}

void BmnTrackingQaExp::ProcessGlobal()
{
    fRunId = fDstEventHeader->GetRunId();
    fEventId = fDstEventHeader->GetEventId();
    if (fRunId != fPrevRunId) {
        printf("run id change  %u  ->  %u\n", fPrevRunId, fRunId);
        fHM->ResetHists();
    }

    if (fVertexL1 != nullptr) {
        if (fVertexL1->GetNTracks() > 1) {
            fHM->H1("VertX")->Fill(fVertexL1->GetX());
            fHM->H1("VertY")->Fill(fVertexL1->GetY());
            fHM->H1("VertZ")->Fill(fVertexL1->GetZ());
            fHM->H1("VertXY")->Fill(fVertexL1->GetX(), fVertexL1->GetY());
            fHM->H1("VertZX")->Fill(fVertexL1->GetZ(), fVertexL1->GetX());
            fHM->H1("VertZY")->Fill(fVertexL1->GetZ(), fVertexL1->GetY());
            fHM->H1("VertX_vs_Ntracks")->Fill(fVertexL1->GetNTracks(), fVertexL1->GetX());
            fHM->H1("VertY_vs_Ntracks")->Fill(fVertexL1->GetNTracks(), fVertexL1->GetY());
            fHM->H1("VertZ_vs_Ntracks")->Fill(fVertexL1->GetNTracks(), fVertexL1->GetZ());
            fHM->H1("NtrInVertex")->Fill(fVertexL1->GetNTracks());
        }
    }

    if (fGlobalTracks) {
        fHM->H1("NGlobTr")->Fill(fGlobalTracks->GetEntriesFast());

        for (Int_t iTrack = 0; iTrack < fGlobalTracks->GetEntriesFast(); iTrack++) {
            BmnGlobalTrack* glTrack = (BmnGlobalTrack*)(fGlobalTracks->At(iTrack));
            if (!glTrack)
                continue;

            FairTrackParam* pf = glTrack->GetParamFirst();

            fHM->H1("Rigidity")->Fill(1.0 / pf->GetQp());

            // TOF400
            if (glTrack->GetTof1HitIndex() != -1) {
                fHM->H2("banana_tof400")->Fill(glTrack->GetP(), glTrack->GetBeta(1));
            }
            // TOF700
            if (glTrack->GetTof2HitIndex() != -1) {
                fHM->H2("banana_tof700")->Fill(glTrack->GetP(), glTrack->GetBeta(2));
            }
            // CSC
            // if (glTrack->GetCscHitIndex(0) != -1) {
            //     BmnHit* cscHit = (BmnHit*) fCscHits->At(glTrack->GetCscHitIndex(0));
            // }
            // DCH1
            if (glTrack->GetDch1TrackIndex() != -1) {
                BmnDchTrack* dchTr = (BmnDchTrack*)fDchTracks->At(glTrack->GetDch1TrackIndex());
                if (dchTr) {
                    ;
                }
            }
            // DCH2
            if (glTrack->GetDch2TrackIndex() != -1) {
                BmnDchTrack* dchTr = (BmnDchTrack*)fDchTracks->At(glTrack->GetDch2TrackIndex());
                if (dchTr) {
                    ;
                }
            }
        }
    }

    // SiBT
    if (fSiBTHits) {
        fHM->H1("NSiBTHits")->Fill(fSiBTHits->GetEntriesFast());
        Double_t x[3] = {0};
        Double_t y[3] = {0};
        Double_t AllEnergy[3] = {0};
        Double_t TotalSignal = 0;
        Int_t siBT_station = 0;
        Int_t NumberOfHitsInStation[3] = {0};
        for (Int_t iSiBTHit = 0; iSiBTHit < fSiBTHits->GetEntriesFast(); ++iSiBTHit) {
            BmnSiBTHit* hit = dynamic_cast<BmnSiBTHit*>(fSiBTHits->At(iSiBTHit));
            siBT_station = hit->GetStation();

            fHM->H2(Form("x_vs_y_SiBT_st%d", siBT_station))->Fill(hit->GetX(), hit->GetY());

            TotalSignal = hit->GetStripTotalSignalInLowerLayer() + hit->GetStripTotalSignalInUpperLayer();
            x[siBT_station] += hit->GetX() * TotalSignal;
            y[siBT_station] += hit->GetY() * TotalSignal;

            NumberOfHitsInStation[siBT_station]++;

            AllEnergy[siBT_station] += TotalSignal;
        }
        if (NumberOfHitsInStation[0] != 0)
            fHM->H2("x_vs_y_SiBT_st0_meanInEvent")->Fill(x[0] / AllEnergy[0], y[0] / AllEnergy[0]);
        if (NumberOfHitsInStation[1] != 0)
            fHM->H2("x_vs_y_SiBT_st1_meanInEvent")->Fill(x[1] / AllEnergy[1], y[1] / AllEnergy[1]);
        if (NumberOfHitsInStation[2] != 0)
            fHM->H2("x_vs_y_SiBT_st2_meanInEvent")->Fill(x[2] / AllEnergy[2], y[2] / AllEnergy[2]);

        fHM->H1("NumberOfHitsInEvent_st0")->Fill(NumberOfHitsInStation[0]);
        fHM->H1("NumberOfHitsInEvent_st1")->Fill(NumberOfHitsInStation[1]);
        fHM->H1("NumberOfHitsInEvent_st2")->Fill(NumberOfHitsInStation[2]);
    }

    if (fSiBTTracks) {
        FairTrackParam* /*paramFirst, * */ paramLast;
        for (Int_t iSiBTTrack = 0; iSiBTTrack < fSiBTTracks->GetEntriesFast(); ++iSiBTTrack) {
            BmnTrack* track = dynamic_cast<BmnTrack*>(fSiBTTracks->At(iSiBTTrack));
            //    paramFirst = track->GetParamFirst();
            paramLast = track->GetParamLast();
            // fHM->H2("SiBTTracks_FirstXY")->Fill(paramFirst->GetX(), paramFirst->GetY());
            // fHM->H2("SiBTTracks_LastXY")->Fill(paramLast->GetX(), paramLast->GetY());
            // fHM->H2("SiBTTracksFirstTx_Ty")->Fill(paramFirst->GetTx(), paramFirst->GetTy());
            // fHM->H2("SiBTTracksLastTx_Ty")->Fill(paramLast->GetTx(), paramLast->GetTy());
            if (fVertexL1 != nullptr) {
                if (fVertexL1->GetNTracks() > 1) {
                    fHM->H2("VertexX_vs_SiBTTracksX")->Fill(fVertexL1->GetX(), paramLast->GetX());
                    fHM->H2("VertexY_vs_SiBTTracksY")->Fill(fVertexL1->GetY(), paramLast->GetY());
                }
            }
        }
    }
    if (fSilHits) {
        fHM->H1("NSilHits")->Fill(fSilHits->GetEntriesFast());
        FillStripHits(kSILICON, fSilHits);
    }
    if (fGemHits) {
        fHM->H1("NGemHits")->Fill(fGemHits->GetEntriesFast());
        FillStripHits(kGEM, fGemHits);
    }
    if (fCscHits) {
        Int_t numberOfHits[6]{};
        for (Int_t iHit = 0; iHit < fCscHits->GetEntriesFast(); ++iHit) {
            BmnHit* cscHit = dynamic_cast<BmnHit*>(fCscHits->At(iHit));
            numberOfHits[cscHit->GetStation()]++;
        }
        fHM->H1("NsCSC1Hits")->Fill(numberOfHits[0]);
        fHM->H1("NsCSC2Hits")->Fill(numberOfHits[1]);
        fHM->H1("NsCSC3Hits")->Fill(numberOfHits[2]);
        fHM->H1("NsCSC4Hits")->Fill(numberOfHits[3]);
        fHM->H1("NlCSC5Hits")->Fill(numberOfHits[4]);
        fHM->H1("NlCSC6Hits")->Fill(numberOfHits[5]);

        FillStripHits(kCSC, fCscHits);
    }
    if (fVspHits) {
        fHM->H1("NVspHits")->Fill(fVspHits->GetEntriesFast());
        FillStripHits(kVSP, fVspHits);
    }
    if (fTof400Hits) {
        fHM->H1("NTof400Hits")->Fill(fTof400Hits->GetEntriesFast());
        FillTofHits(fTof400Hits, kTOF1);
    }
    if (fTof700Hits) {
        fHM->H1("NTof700Hits")->Fill(fTof700Hits->GetEntriesFast());
        FillTofHits(fTof700Hits, kTOF701);
    }
    if (fStsTracks) {
        fHM->H1("NInnTr")->Fill(fStsTracks->GetEntriesFast());
    }
}

void BmnTrackingQaExp::ResetSiBT()
{
    printf("ResetSiBT DST\n");
    fHM->H2("x_vs_y_SiBT_st0")->Reset();
    fHM->H2("x_vs_y_SiBT_st1")->Reset();
    fHM->H2("x_vs_y_SiBT_st2")->Reset();
    fHM->H2("x_vs_y_SiBT_st0_meanInEvent")->Reset();
    fHM->H2("x_vs_y_SiBT_st1_meanInEvent")->Reset();
    fHM->H2("x_vs_y_SiBT_st2_meanInEvent")->Reset();
    // fHM->H2("SiBTTracks_LastXY")->Reset();
    // fHM->H2("SiBTTracks_FirstXY")->Reset();
    // fHM->H2("SiBTTracksLastTx_Ty")->Reset();
    // fHM->H2("SiBTTracksFirstTx_Ty")->Reset();
    fHM->H2("VertexX_vs_SiBTTracksX")->Reset();
    fHM->H2("VertexY_vs_SiBTTracksY")->Reset();
    fHM->H2("NumberOfHitsInEvent_st0")->Reset();
    fHM->H2("NumberOfHitsInEvent_st1")->Reset();
    fHM->H2("NumberOfHitsInEvent_st2")->Reset();
}

void BmnTrackingQaExp::RegisterCommands()
{
    fServer->Register("/", this);
    //    TString path = "/" + fTitle + "/";
    string path = string("/") + fName.Data() + "_cmd/";
    string cmd = string("/") + fName.Data() + "/->ResetSiBT()";
    string cmdTitle = path + "ResetSiBT";
    fServer->RegisterCommand(cmdTitle.data(), cmd.data(), "button;");
}

void BmnTrackingQaExp::CreateTofHits(DetectorId detId)
{
    TString detName;
    BmnDetectorList::GetSystemNameCaps(detId, detName);
    detName = detName + "_X_Y";
    CreateH2(detName.Data(), "x cm", "y cm", "", QA_COORD_BINS, -230, 230, QA_COORD_BINS, -200, 200);
}

void BmnTrackingQaExp::FillTofHits(TClonesArray* tof_hits, DetectorId detId)
{
    TString detName;
    BmnDetectorList::GetSystemNameCaps(detId, detName);
    detName = detName + "_X_Y";
    for (Int_t iHit = 0; iHit < tof_hits->GetEntriesFast(); ++iHit) {
        BmnTofHit* hit = dynamic_cast<BmnTofHit*>(tof_hits->At(iHit));
        fHM->H2(detName)->Fill(hit->GetX(), hit->GetY());
    }
}