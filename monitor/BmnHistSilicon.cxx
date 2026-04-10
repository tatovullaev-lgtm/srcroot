#include "BmnHistSilicon.h"

#include "BmnStripData.h"

BmnHistSilicon::BmnHistSilicon(TString title, TString path, Int_t PeriodID, BmnSetup setup)
    : BmnHist(PeriodID, setup)
    , stationSet(BmnSiliconStationSet::Create(fPeriodID, fSetup))
    , sumMods(0)
    , maxLayers(0)
    , maxMods(0)
    , fStation(-1)
{
    fTitle = title;
    fName = title + "_cl";
    refPath = path;
    TString name;
    numberOfChips = 5;   // number of chips on each layer for run 8

    const vector<Color_t> ChipColors{kBlue, kYellow + 3, kGreen + 2, kRed, kMagenta - 1};

    numberOfStrips = stationSet->GetSiliconStation(0)->GetModule(0)->GetStripLayer(0).GetNStrips();
    Int_t numberOfStations = stationSet->GetNStations();
    nStations = numberOfStations;
    canStripPads.resize(nStations);
    canSignalPads.resize(nStations);
    canStripSignalPads.resize(nStations);
    canStripNoisePads.resize(nStations);
    canChipSignalPads.resize(nStations);

    NamesStrip.resize(nStations);
    NamesSignal.resize(nStations);
    NamesStripSignal.resize(nStations);
    NamesStripNoise.resize(nStations);
    NamesChipSignal.resize(nStations);

    for (Int_t iStation = 0; iStation < nStations; iStation++) {

        vector<vector<TH1I*>> rowSilicon;              // for stripNumber
        vector<vector<Double_t>> signalsInModule;      // for correlation between 2 layers
        vector<TH2F*> rowSilicon2D;                    // for correlation between 2 layers
        vector<vector<TH2F*>> firstStrip_Signal;       // for strip_Signal data
        vector<vector<TH1F*>> firstStrip_Noise;        // for strip_noise data
        vector<vector<vector<TH1F*>>> rowChipSignal;   // for chip Signal
        BmnSiliconStation* st = stationSet->GetSiliconStation(iStation);

        sumMods += st->GetNModules();
        if ((Int_t)maxMods < st->GetNModules())
            maxMods = st->GetNModules();

        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            vector<TH1I*> colSilicon;              // for stripNumber
            vector<Double_t> signalsInLayers;      // for correlation between 2 layers
            vector<TH2F*> secondStripSignal;       // for strip_Signal data
            vector<TH1F*> secondStripNoise;        // for strip_noise data
            vector<vector<TH1F*>> colChipSignal;   // for chip signal

            BmnSiliconModule* mod = st->GetModule(iModule);
            if ((Int_t)maxLayers < mod->GetNStripLayers())
                maxLayers = mod->GetNStripLayers();
            for (Int_t iLayer = 0; iLayer < 2 /*mod->GetNStripLayers()*/; iLayer++) {
                vector<TH1F*> layerChipSignal;   // for chip signal
                BmnSiliconLayer lay = mod->GetStripLayer(iLayer);
                signalsInLayers.push_back(0);
                name = Form(fTitle + "_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH1I* h = new TH1I(name, name, lay.GetNStrips(), 0, lay.GetNStrips());
                h->GetXaxis()->SetTitle("Strip Number");
                h->GetYaxis()->SetTitle("Activation Count");
                if (iLayer == 0) {
                    h->SetLineColor(1);   // black
                } else {
                    h->SetLineColor(2);
                }   // red
                BmnHist::SetHistStyleTH1(h);
                colSilicon.push_back(h);

                name = Form(fTitle + "StripVsSignal" + "_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH2F* hist = new TH2F(name, name, lay.GetNStrips(), 0, lay.GetNStrips(), 250, 0, 2500);
                hist->GetXaxis()->SetTitle("Strip Number");
                hist->GetYaxis()->SetTitle("Signal");
                BmnHist::SetHistStyleTH1(hist);
                secondStripSignal.push_back(hist);

                name = Form(fTitle + "StripVsNoise" + "_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH1F* stripNoise = new TH1F(name, name, lay.GetNStrips(), 0, lay.GetNStrips());
                stripNoise->SetTitle(name);
                stripNoise->GetXaxis()->SetTitle("Strip Number");
                stripNoise->GetYaxis()->SetTitle("Noise");
                stripNoise->SetLineWidth(2);
                if (iLayer == 0) {
                    stripNoise->SetLineColor(1);   // black
                } else {
                    stripNoise->SetLineColor(2);
                }   // red
                BmnHist::SetHistStyleTH1(stripNoise);
                secondStripNoise.push_back(stripNoise);

                for (UInt_t iChip = 0; iChip < numberOfChips; ++iChip) {
                    name = Form(fTitle + "ChipSignal" + "_Station_%d_module_%d_layer_%d_chip_%d", iStation, iModule,
                                iLayer, iChip);
                    TH1F* chipSignal = new TH1F(name, name, 500, 0, 2500);
                    chipSignal->GetXaxis()->SetTitle("Signal");
                    chipSignal->GetYaxis()->SetTitle("Count");
                    chipSignal->SetLineColor(ChipColors[iChip]);
                    chipSignal->SetLineWidth(2);
                    BmnHist::SetHistStyleTH1(chipSignal);
                    layerChipSignal.push_back(chipSignal);
                }
                colChipSignal.push_back(layerChipSignal);
            }
            name = Form("Signal_Layer0_Layer1_st%d_mod%d", iStation, iModule);
            TH2F* h = new TH2F(name, name, 250, 0, 2500, 250, 0, 2500);
            h->GetXaxis()->SetTitle("Signal in layer 0");
            h->GetYaxis()->SetTitle("Signal in layer 1");
            BmnHist::SetHistStyleTH1(h);
            rowSilicon2D.push_back(h);
            signalsInModule.push_back(signalsInLayers);
            firstStrip_Signal.push_back(secondStripSignal);
            firstStrip_Noise.push_back(secondStripNoise);
            rowSilicon.push_back(colSilicon);
            rowChipSignal.push_back(colChipSignal);
        }
        histSiliconStrip.push_back(rowSilicon);
        signalSilicon.push_back(signalsInModule);
        histSiliconSignal2D.push_back(rowSilicon2D);
        histSiliconStripSignal.push_back(firstStrip_Signal);
        histSiliconStripNoise.push_back(firstStrip_Noise);
        histSiliconChipSignal.push_back(rowChipSignal);
    }
    maxLayers = 2;
    // Create canvas
    name = fTitle + "Canvas";
    canSiliconStrip = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    name = fTitle + "SignalCanvas";
    canSiliconSignal = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    name = fTitle + "StripSignalCanvas";
    canSiliconStripSignal = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    name = fTitle + "StripNoiseCanvas";
    canSiliconStripNoise = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    name = fTitle + "ChipSignalCanvas";
    canSiliconChipSignal = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);

    for (Int_t iStation = 0; iStation < numberOfStations; iStation++) {
        BmnSiliconStation* st = stationSet->GetSiliconStation(iStation);
        Int_t numberOfModules = st->GetNModules();
        canStripPads[iStation].resize(numberOfModules * 1);
        canSignalPads[iStation].resize(numberOfModules * 1);
        canStripSignalPads[iStation].resize(numberOfModules * maxLayers);
        canStripNoisePads[iStation].resize(numberOfModules * 1);
        canChipSignalPads[iStation].resize(numberOfModules * maxLayers);
        NamesStrip[iStation].resize(numberOfModules * 1);
        NamesSignal[iStation].resize(numberOfModules * 1);
        NamesStripSignal[iStation].resize(numberOfModules * maxLayers);
        NamesStripNoise[iStation].resize(numberOfModules * 1);
        NamesChipSignal[iStation].resize(numberOfModules * maxLayers);
    }

    // CREATE DEFAULT CANVASES FOR SILICON PLANE 0
    ChangeStation(0);
}

void BmnHistSilicon::ChangeStation(Int_t stationNumber)
{
    if (fStation == stationNumber)
        return;
    ;
    BmnSiliconStation* st = stationSet->GetSiliconStation(stationNumber);
    Int_t numberOfModules = st->GetNModules();

    ResetCanvases(numberOfModules);
    fStation = stationNumber;
    //    canStrip->Divide(maxLayers, sumMods, 0.0, 0.0);
    Int_t modCtr = 0;   // filling GEM Canvas' pads

    for (auto& pad : canStripPads[fStation])
        if (pad)
            pad = nullptr;

    for (auto& pad : canSignalPads[fStation])
        if (pad)
            pad = nullptr;

    for (auto& pad : canStripSignalPads[fStation])
        if (pad)
            pad = nullptr;

    for (auto& pad : canSignalPads[fStation])
        if (pad)
            pad = nullptr;

    for (auto& pad : canChipSignalPads[fStation])
        if (pad)
            pad = nullptr;

    const vector<TString> LayerNames{"p+", "n+"};
    for (Int_t iModule = 0; iModule < numberOfModules; iModule++) {
        // BmnSiliconModule *mod = st->GetModule(iModule);
        Int_t iPad = iModule;
        PadInfo* p = new PadInfo();
        Double_t l_x0 = 0.15;
        Double_t l_y0 = 0.75;
        Double_t l_w = 0.10;
        Double_t l_h = 0.15;
        TLegend* legend = new TLegend(l_x0, l_y0, l_x0 + l_w, l_y0 + l_h);
        legend->SetTextSize(0.03);
        for (Int_t iLayer = 0; iLayer < 2 /*mod->GetNStripLayers()*/; iLayer++) {
            legend->AddEntry(histSiliconStrip[fStation][iModule][iLayer], LayerNames[iLayer]);
            p->aux.push_back(histSiliconStrip[fStation][iModule][iLayer]);
        }
        p->legend = legend;
        canStripPads[fStation][iPad] = p;
        canSiliconStrip->GetPad(iPad + 1)->SetGrid();
        NamesStrip[fStation][iPad] = canStripPads[fStation][iPad]->name;
        modCtr++;
    }
    modCtr = 0;
    for (Int_t iModule = 0; iModule < numberOfModules; iModule++) {
        PadInfo* p = new PadInfo();
        p->opt = "colz";
        p->current = histSiliconSignal2D[fStation][iModule];
        Int_t iPad = iModule;
        canSignalPads[fStation][iPad] = p;
        canSiliconSignal->GetPad(iPad + 1)->SetGrid();
        NamesSignal[fStation][iPad] = canSignalPads[fStation][iPad]->current->GetName();
    }

    modCtr = 0;

    for (Int_t iModule = 0; iModule < numberOfModules; iModule++) {
        for (Int_t iLayer = 0; iLayer < 2 /*mod->GetNStripLayers()*/; iLayer++) {
            PadInfo* p = new PadInfo();
            p->opt = "colz";
            p->current = histSiliconStripSignal[fStation][iModule][iLayer];
            Int_t iPad = modCtr * maxLayers + iLayer;
            canStripSignalPads[fStation][iPad] = p;
            canSiliconStripSignal->GetPad(iPad + 1)->SetGrid();
            NamesStripSignal[fStation][iPad] = canStripSignalPads[fStation][iPad]->current->GetName();
        }
        modCtr++;
    }
    modCtr = 0;

    for (Int_t iModule = 0; iModule < numberOfModules; iModule++) {
        PadInfo* p = new PadInfo();
        p->opt = "";
        Double_t l_x0 = 0.15;
        Double_t l_y0 = 0.75;
        Double_t l_w = 0.10;
        Double_t l_h = 0.15;
        TLegend* legend = new TLegend(l_x0, l_y0, l_x0 + l_w, l_y0 + l_h);
        legend->SetTextSize(0.03);
        for (Int_t iLayer = 0; iLayer < 2 /*mod->GetNStripLayers()*/; iLayer++) {
            legend->AddEntry(histSiliconStripNoise[fStation][iModule][iLayer], LayerNames[iLayer]);
            p->aux.push_back(histSiliconStripNoise[fStation][iModule][iLayer]);
        }
        p->legend = legend;
        Int_t iPad = iModule;
        p->name = Form(fTitle + "StripVsNoise" + "_Station_%d_module_%d", fStation, iModule);
        canStripNoisePads[fStation][iPad] = p;
        canSiliconStripNoise->GetPad(iPad + 1)->SetGrid();
        NamesStripNoise[stationNumber][iPad] = canStripNoisePads[stationNumber][iPad]->name;
    }
    modCtr = 0;

    for (Int_t iModule = 0; iModule < numberOfModules; iModule++) {
        for (Int_t iLayer = 0; iLayer < 2; iLayer++) {
            PadInfo* p = new PadInfo();
            p->opt = "";
            Double_t l_x0 = 0.75;
            Double_t l_y0 = 0.5;
            Double_t l_w = 0.10;
            Double_t l_h = 0.20;
            TLegend* legend = new TLegend(l_x0, l_y0, l_x0 + l_w, l_y0 + l_h);
            legend->SetTextSize(0.04);
            for (UInt_t iChip = 0; iChip < numberOfChips; iChip++) {
                legend->AddEntry(histSiliconChipSignal[fStation][iModule][iLayer][iChip], Form("ASIC# %d", iChip + 1));
                p->aux.push_back(histSiliconChipSignal[fStation][iModule][iLayer][iChip]);
            }
            p->legend = legend;
            Int_t iPad = modCtr * maxLayers + iLayer;
            p->name = Form(fTitle + "ChipSignal" + "_Station_%d_module_%d", fStation, iModule);
            canChipSignalPads[fStation][iPad] = p;
            canSiliconChipSignal->GetPad(iPad + 1)->SetGrid();
            NamesChipSignal[fStation][iPad] = canChipSignalPads[fStation][iPad]->name;
            canSiliconChipSignal->GetPad(iPad + 1)->SetTitle(NamesChipSignal[fStation][iPad]);
        }
        modCtr++;
    }
    modCtr = 0;

    canSiliconStrip->UpdateAsync();
    canSiliconSignal->UpdateAsync();
    canSiliconStripSignal->UpdateAsync();
    canSiliconStripNoise->UpdateAsync();
    canSiliconChipSignal->UpdateAsync();

    canSiliconStrip->Modified();
    canSiliconSignal->Modified();
    canSiliconStripSignal->Modified();
    canSiliconStripNoise->Modified();
    canSiliconChipSignal->Modified();

    DrawBoth();
}

void BmnHistSilicon::ResetCanvases(Int_t numberOfModules)
{
    // for (auto& pad : canStripPads[fStation])
    //     if (pad)
    //         pad = nullptr;

    // for (auto& pad : canSignalPads[fStation])
    //     if (pad)
    //         pad = nullptr;

    // for (auto& pad : canStripSignalPads[fStation])
    //     if (pad)
    //         pad = nullptr;

    // for (auto& pad : canSignalPads[fStation])
    //     if (pad)
    //         pad = nullptr;

    // for (auto& pad : canChipSignalPads[fStation])
    //     if (pad)
    //         pad = nullptr;

    canSiliconChipSignal->Clear();
    canSiliconStrip->Clear();
    canSiliconSignal->Clear();
    canSiliconStripSignal->Clear();
    canSiliconStripNoise->Clear();

    canSiliconStrip->SetWindowSize(PAD_WIDTH * maxLayers, 400 * numberOfModules);
    canSiliconSignal->SetWindowSize(PAD_WIDTH * maxLayers, 400 * numberOfModules);
    canSiliconStripSignal->SetWindowSize(PAD_WIDTH * maxLayers, 400 * numberOfModules);
    canSiliconStripNoise->SetWindowSize(PAD_WIDTH * maxLayers, 400 * numberOfModules);
    canSiliconChipSignal->SetWindowSize(PAD_WIDTH * maxLayers, 400 * numberOfModules);

    canSiliconStrip->Divide(1, numberOfModules, 0.01, 0.01 / numberOfModules);
    canSiliconSignal->Divide(1, numberOfModules, 0.01, 0.01 / numberOfModules);
    canSiliconStripSignal->Divide(maxLayers, numberOfModules, 0.01 / maxLayers, 0.01 / numberOfModules);
    canSiliconStripNoise->Divide(1, numberOfModules, 0.01, 0.01 / numberOfModules);
    canSiliconChipSignal->Divide(maxLayers, numberOfModules, 0.01 / maxLayers, 0.01 / numberOfModules);
}

BmnHistSilicon::~BmnHistSilicon()
{
    //    delete gemStationSet;
    delete canSiliconStrip;
    delete canSiliconSignal;
    delete canSiliconStripSignal;
    delete canSiliconStripNoise;
    if (fDir)
        return;
    for (Int_t iStation = 0; iStation < nStations; ++iStation) {
        for (auto pad : canStripPads[iStation])
            if (pad)
                delete pad;

        for (auto padSignal : canSignalPads[iStation])
            if (padSignal)
                delete padSignal;

        for (auto padStripSignal : canStripSignalPads[iStation])
            if (padStripSignal)
                delete padStripSignal;

        for (auto padStripNoise : canStripNoisePads[iStation])
            if (padStripNoise)
                delete padStripNoise;

        for (auto padChipSignal : canChipSignalPads[iStation])
            if (padChipSignal)
                delete padChipSignal;
    }
    for (size_t iSt = 0; iSt < histSiliconStrip.size(); ++iSt) {
        for (size_t iMod = 0; iMod < histSiliconStrip[iSt].size(); ++iMod) {
            for (size_t iLayer = 0; iLayer < histSiliconStrip[iSt][iMod].size(); ++iLayer) {
                delete histSiliconStrip[iSt][iMod][iLayer];
                delete histSiliconStripSignal[iSt][iMod][iLayer];
                delete histSiliconStripNoise[iSt][iMod][iLayer];
                for (UInt_t iChip = 0; iChip < numberOfChips; iChip++)
                    delete histSiliconChipSignal[iSt][iMod][iLayer][iChip];
            }
            delete histSiliconSignal2D[iSt][iMod];
        }
    }
}

void BmnHistSilicon::Register(THttpServer* serv)
{
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canSiliconStrip);
    fServer->Register(path, canSiliconSignal);
    fServer->Register(path, canSiliconStripSignal);
    fServer->Register(path, canSiliconStripNoise);
    fServer->Register(path, canSiliconChipSignal);
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
    cmd = "/" + fName + "/->ChangeStation(%arg1%)";
    cmdTitle = path + "ChangeStation";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
}

void BmnHistSilicon::SetDir(TFile* outFile, TTree* recoTree)
{
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    for (auto row : histSiliconStrip)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(fDir);

    for (auto rowSig : histSiliconSignal2D)
        for (auto colSig : rowSig)
            colSig->SetDirectory(fDir);

    for (auto row : histSiliconStripSignal)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(fDir);

    for (auto row : histSiliconStripNoise)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(fDir);

    for (auto row : histSiliconChipSignal)
        for (auto col : row)
            for (auto array : col)
                for (auto el : array)
                    el->SetDirectory(fDir);
}

void BmnHistSilicon::DrawBoth()
{
    BmnHist::DrawRef(canSiliconStrip, &canStripPads[fStation]);
    BmnHist::DrawRef(canSiliconSignal, &canSignalPads[fStation]);
    BmnHist::DrawRef(canSiliconStripSignal, &canStripSignalPads[fStation]);
    BmnHist::DrawRef(canSiliconStripNoise, &canStripNoisePads[fStation]);
    BmnHist::DrawRef(canSiliconChipSignal, &canChipSignalPads[fStation]);
    return;
}

void BmnHistSilicon::FillFromDigi(DigiArrays* fDigiArrays)
{
    TClonesArray* siliconDigits = fDigiArrays->silicon;
    if (!siliconDigits)
        return;
    for (Int_t digIndex = 0; digIndex < siliconDigits->GetEntriesFast(); digIndex++) {
        BmnSiliconDigit* gs = static_cast<BmnSiliconDigit*>(siliconDigits->At(digIndex));
        Int_t module = gs->GetModule();
        Int_t station = gs->GetStation();
        Int_t layer = gs->GetStripLayer();
        Int_t siliconStrip = gs->GetStripNumber();
        histSiliconStrip[station][module][layer]->Fill(siliconStrip);
        histSiliconStripSignal[station][module][layer]->Fill(siliconStrip, gs->GetStripSignal());
        histSiliconStripNoise[station][module][layer]->SetBinContent(siliconStrip, gs->GetStripNoise());
        signalSilicon[station][module][layer] = gs->GetStripSignal();
        histSiliconChipSignal[station][module][layer][Int_t(siliconStrip / (numberOfStrips / numberOfChips))]->Fill(
            gs->GetStripSignal());
    }

    for (size_t iStation = 0; iStation < signalSilicon.size(); ++iStation)
        for (size_t iModule = 0; iModule < signalSilicon[iStation].size(); ++iModule)
            if (signalSilicon[iStation][iModule][0] && signalSilicon[iStation][iModule][1]) {
                histSiliconSignal2D[iStation][iModule]->Fill(signalSilicon[iStation][iModule][0],
                                                             signalSilicon[iStation][iModule][1]);
            }
}

BmnStatus BmnHistSilicon::SetRefRun(Int_t id)
{
    // ONLY FOR STATION 0
    if (refID != id) {
        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        for (Int_t iStation = 0; iStation < nStations; ++iStation)
            BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canStripPads[iStation], NamesStrip[iStation]);
        DrawBoth();
    }

    return kBMNSUCCESS;
}

void BmnHistSilicon::ClearRefRun()
{
    for (Int_t iStation = 0; iStation < nStations; ++iStation) {
        for (auto& pad : canStripPads[iStation]) {
            if (pad) {
                if (pad->ref)
                    delete pad->ref;
                pad->ref = NULL;
            }
        }
        for (auto& padSignal : canSignalPads[iStation]) {
            if (padSignal) {
                if (padSignal->ref)
                    delete padSignal->ref;
                padSignal->ref = NULL;
            }
        }
        for (auto& padStripSignal : canStripSignalPads[iStation]) {
            if (padStripSignal) {
                if (padStripSignal->ref)
                    delete padStripSignal->ref;
                padStripSignal->ref = NULL;
            }
        }
        for (auto& padStripNoise : canStripNoisePads[iStation]) {
            if (padStripNoise) {
                if (padStripNoise->ref)
                    delete padStripNoise->ref;
                padStripNoise->ref = NULL;
            }
        }
        for (auto padChipSignal : canChipSignalPads[iStation]) {
            if (padChipSignal) {
                if (padChipSignal->ref)
                    delete padChipSignal->ref;
                padChipSignal->ref = NULL;
            }
        }
    }
    refID = 0;
}

void BmnHistSilicon::Reset()
{
    printf("BmnHistSilicon : Reset histos\n");

    for (auto& rowStripSignal : histSiliconStrip)
        for (auto colStripSignal : rowStripSignal)
            for (auto elStripSignal : colStripSignal)
                elStripSignal->Reset();

    for (auto& row : histSiliconSignal2D)
        for (auto col : row)
            col->Reset();

    for (auto& rowStripSignal : histSiliconStripSignal)
        for (auto colStripSignal : rowStripSignal)
            for (auto elStripSignal : colStripSignal)
                elStripSignal->Reset();

    for (auto& rowStripNoise : histSiliconStripNoise)
        for (auto colStripNoise : rowStripNoise)
            for (auto elStripNoise : colStripNoise)
                elStripNoise->Reset();

    for (auto rowChipSignal : histSiliconChipSignal)
        for (auto colChipSignal : rowChipSignal)
            for (auto arrayChipSignal : colChipSignal)
                for (auto elChipSignal : arrayChipSignal)
                    elChipSignal->Reset();
}