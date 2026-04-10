#include "BmnHistVsp.h"

#include "BmnRawDataDecoder.h"
#include "BmnVSPDigit.h"
#include "BmnVSPStationSet.h"

#include <algorithm>
#include <numeric>

BmnHistVsp::BmnHistVsp(TString title, TString path, Int_t PeriodID, BmnSetup stp)
    : BmnHist(PeriodID, stp)
{
    const double MinTime = -100;   // ns
    const double MaxTime = 2200;   // ns
    const double MaxTimeBins = 200;
    sumMods = 0;
    maxLayers = 0;
    refPath = path;
    fTitle = title;
    fName = title + "_cl";
    TString name;
    unique_ptr<BmnVSPStationSet> gemStationSet = BmnVSPStationSet::Create(fPeriodID, fSetup);
    for (Int_t iStation = 0; iStation < 1 /*gemStationSet->GetNStations()*/; iStation++) {
        vector<vector<TH1I*>> rowGEM;
        vector<vector<TH1I*>> rowTimes;
        auto* st = gemStationSet->GetStation(iStation);
        sumMods += st->GetNModules();
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            vector<TH1I*> colGEM;
            vector<TH1I*> colTimes;
            auto* mod = st->GetModule(iModule);
            if ((Int_t)maxLayers < mod->GetNStripLayers())
                maxLayers = mod->GetNStripLayers();
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                auto& lay = mod->GetStripLayer(iLayer);
                // Occupancy
                name = Form(fTitle + "_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH1I* h = new TH1I(name, name, lay.GetNStrips(), 0, lay.GetNStrips());
                h->GetXaxis()->SetTitle("Strip Number");
                h->GetYaxis()->SetTitle("Activation Count");
                BmnHist::SetHistStyleTH1(h);
                colGEM.push_back(h);
                // Time since start time
                name = Form(fTitle + "_Time_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH1I* hTime = new TH1I(name, name, MaxTimeBins, MinTime, MaxTime);
                hTime->GetXaxis()->SetTitle("Time [ns]");
                hTime->GetYaxis()->SetTitle("Activation Count");
                BmnHist::SetHistStyleTH1(hTime);
                colTimes.push_back(hTime);
            }
            rowGEM.push_back(colGEM);
            rowTimes.push_back(colTimes);
        }
        histStripOccup.push_back(rowGEM);
        histTimes.push_back(rowTimes);
    }
    // Create canvas for occupancy
    Int_t modCtr = 0;
    name = fTitle + "Canvas";
    canStripOccup = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    canStripOccup->Divide(maxLayers, sumMods, 0.01 / maxLayers, 0.01 / sumMods);
    canStripPads.resize(sumMods * maxLayers);
    Names.resize(sumMods * maxLayers);
    // Create canvas for times
    name = fTitle + "CanvasTimes";
    canTimes = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    canTimes->Divide(maxLayers, sumMods, 0.01 / maxLayers, 0.01 / sumMods);
    canTimesPads.resize(sumMods * maxLayers);
    NamesTimes.resize(sumMods * maxLayers);
    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
        auto* st = gemStationSet->GetStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            auto* mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                {
                    PadInfo* p = new PadInfo();
                    p->opt = "";
                    p->current = histStripOccup[iStation][iModule][iLayer];
                    Int_t iPad = modCtr * maxLayers + iLayer;
                    canStripPads[iPad] = p;
                    canStripOccup->GetPad(iPad + 1)->SetGrid();
                    Names[iPad] = canStripPads[iPad]->current->GetName();
                }
                {
                    PadInfo* p = new PadInfo();
                    p->opt = "";
                    p->current = histTimes[iStation][iModule][iLayer];
                    Int_t iPad = modCtr * maxLayers + iLayer;
                    canTimesPads[iPad] = p;
                    canTimes->GetPad(iPad + 1)->SetGrid();
                    NamesTimes[iPad] = canTimesPads[iPad]->current->GetName();
                }
            }
            modCtr++;
        }
    }
}

BmnHistVsp::~BmnHistVsp()
{
    delete canStripOccup;
    delete canTimes;
    if (fDir)
        return;
    for (auto pad : canStripPads)
        delete pad;
    for (auto pad : canTimesPads)
        delete pad;
    return;
}

void BmnHistVsp::Register(THttpServer* serv)
{
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canStripOccup);
    fServer->Register(path, canTimes);
    fServer->SetItemField(path, "_monitoring", "2000");
    fServer->SetItemField(path, "_layout", "grid3x3");
    TString cmd = "/" + fName + "/->Reset()";
    TString cmdTitle = path + "Reset";
    //    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    //    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmdTitle = path + TString("Reset");
    fServer->RegisterCommand(cmdTitle, TString("/") + fName.Data() + "/->Reset()", "button;");
}

void BmnHistVsp::SetDir(TFile* outFile, TTree* recoTree)
{
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    for (auto row : histStripOccup)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(fDir);
    for (auto row : histTimes)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(fDir);
}

void BmnHistVsp::DrawBoth()
{
    BmnHist::DrawRef(canStripOccup, &canStripPads);
    BmnHist::DrawRef(canTimes, &canTimesPads);
}

void BmnHistVsp::FillFromDigi(DigiArrays* fDigiArrays)
{
    TClonesArray* gemDigits = fDigiArrays->vsp;
    if (!gemDigits)
        return;
    for (Int_t digIndex = 0; digIndex < gemDigits->GetEntriesFast(); digIndex++) {
        BmnVSPDigit* gs = static_cast<BmnVSPDigit*>(gemDigits->At(digIndex));
        Int_t module = gs->GetModule();
        Int_t station = gs->GetStation();
        Int_t layer = gs->GetStripLayer();
        Int_t gemStrip = gs->GetStripNumber();
        double time_since_beg = gs->GetTimeSinceBegin_ns();
        histStripOccup[station][module][layer]->Fill(gemStrip);
        histTimes[station][module][layer]->Fill(time_since_beg);
    }
}

BmnStatus BmnHistVsp::SetRefRun(Int_t id)
{
    if (refID != id) {
        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canStripPads, Names);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canTimesPads, NamesTimes);
        DrawBoth();
    }

    return kBMNSUCCESS;
}

void BmnHistVsp::ClearRefRun()
{
    for (auto pad : canStripPads) {
        if (pad)
            if (pad->ref) {
                delete pad->ref;
                pad->ref = nullptr;
            }
    }
    for (auto pad : canTimesPads) {
        if (pad)
            if (pad->ref) {
                delete pad->ref;
                pad->ref = nullptr;
            }
    }
    refID = 0;
}

void BmnHistVsp::Reset()
{
    for (auto row : histStripOccup)
        for (auto col : row)
            for (auto el : col)
                el->Reset();
    for (auto row : histTimes)
        for (auto col : row)
            for (auto el : col)
                el->Reset();
}
