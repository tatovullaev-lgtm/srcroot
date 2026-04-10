#include "BmnHistSiBT.h"

#include "BmnRawDataDecoder.h"
#include "BmnSiBTDigit.h"
#include "BmnStripData.h"

BmnHistSiBT::BmnHistSiBT(TString title, TString path, Int_t periodID)
    : BmnHist(periodID)
    , sibtStationSet(BmnSiBTStationSet::Create(periodID, fSetup))
    , sumMods(0)
    , maxLayers(0)
    , maxMods(0)
{
    LOG(debug) << "INIT OF SiBTHist";
    fTitle = title;
    fName = title + "_cl";
    refPath = path;
    TString name;
    numberOfChips = 2;   // number of chips on each layer for run 8
    const vector<Color_t> ChipColors{kRed + 2, kCyan + 2};
    numberOfStations = sibtStationSet->GetNStations();
    numberOfStrips = sibtStationSet->GetStation(0)->GetModule(0)->GetStripLayer(0).GetNStrips();
    for (Int_t iStation = 0; iStation < numberOfStations; iStation++) {
        vector<vector<Double_t>> signalsInModule;      // for correlation between 2 layers
        vector<vector<TH1F*>> rowSiBT;                 // for stripNumber
        vector<vector<vector<TH1F*>>> rowChipSignal;   // for chip Signal
        vector<TH2F*> rowSiBT2D;                       // for correlation between 2 layers
        vector<vector<TH2F*>> firstStrip_Signal;       // for strip_Signal data
        vector<vector<TH1F*>> firstStrip_Noise;        // for strip_noise data
        BmnSiBTStation* st = sibtStationSet->GetStation(iStation);

        sumMods += st->GetNModules();
        if (maxMods < st->GetNModules())
            maxMods = st->GetNModules();

        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            vector<TH1F*> colSiBT;                 // for stripNumber
            vector<vector<TH1F*>> colChipSignal;   // for chip signal
            vector<Double_t> signalsInLayers;      // for correlation between 2 layers
            vector<TH2F*> secondStripSignal;       // for strip_Signal data
            vector<TH1F*> secondStripNoise;        // for strip_noise data

            BmnSiBTModule* mod = st->GetModule(iModule);
            if ((Int_t)maxLayers < mod->GetNStripLayers())
                maxLayers = mod->GetNStripLayers();

            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                vector<TH1F*> layerChipSignal;   // for chip signal
                BmnSiBTLayer lay = mod->GetStripLayer(iLayer);
                signalsInLayers.push_back(0);
                name = Form(fTitle + "_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH1F* h = new TH1F(name, name, lay.GetNStrips(), 0, lay.GetNStrips());
                h->GetXaxis()->SetTitle("Strip Number");
                h->GetYaxis()->SetTitle("Activation Count");
                h->SetLineWidth(2);
                StripLayerType layer_type = lay.GetType();
                if (layer_type == LowerStripLayer) {
                    h->SetLineColor(1);   // black
                } else {
                    h->SetLineColor(2);
                }   // red
                BmnHist::SetHistStyleTH1(h);
                colSiBT.push_back(h);

                name = Form(fTitle + "StripVsSignal" + "_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH2F* hist = new TH2F(name, name, lay.GetNStrips(), 0, lay.GetNStrips(), 250, 0, 2500);
                hist->GetXaxis()->SetTitle("Strip Number");
                hist->GetYaxis()->SetTitle("Signal");
                secondStripSignal.push_back(hist);

                name = Form(fTitle + "StripVsNoise" + "_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH1F* stripNoise = new TH1F(name, name, lay.GetNStrips(), 0, lay.GetNStrips());
                stripNoise->SetTitle(name);
                stripNoise->GetXaxis()->SetTitle("Strip Number");
                stripNoise->GetYaxis()->SetTitle("Noise");
                stripNoise->SetLineWidth(2);
                if (layer_type == LowerStripLayer) {
                    stripNoise->SetLineColor(1);   // black
                } else {
                    stripNoise->SetLineColor(2);
                }   // red
                secondStripNoise.push_back(stripNoise);
                for (Int_t iChip = 0; iChip < numberOfChips; ++iChip) {
                    name = Form(fTitle + "ChipSignal" + "_Station_%d_module_%d_layer_%d_chip_%d", iStation, iModule,
                                iLayer, iChip);
                    TH1F* chipSignal = new TH1F(name, name, 500, 0, 2500);
                    chipSignal->GetXaxis()->SetTitle("Signal");
                    chipSignal->GetYaxis()->SetTitle("Count");
                    chipSignal->SetLineColor(ChipColors[iChip]);
                    chipSignal->SetLineWidth(2);
                    layerChipSignal.push_back(chipSignal);
                }
                colChipSignal.push_back(layerChipSignal);
            }
            name = Form("Signal_Layer0_Layer1_st%d_mod%d", iStation, iModule);
            TH2F* h = new TH2F(name, name, 250, 0, 2500, 250, 0, 2500);
            h->GetXaxis()->SetTitle("Signal in layer 0");
            h->GetYaxis()->SetTitle("Signal in layer 1");
            rowSiBT2D.push_back(h);
            signalsInModule.push_back(signalsInLayers);
            rowSiBT.push_back(colSiBT);
            rowChipSignal.push_back(colChipSignal);
            firstStrip_Signal.push_back(secondStripSignal);
            firstStrip_Noise.push_back(secondStripNoise);
        }
        signalSiBT.push_back(signalsInModule);
        histSiBTStrip.push_back(rowSiBT);
        histSiBTChipSignal.push_back(rowChipSignal);
        histSiBTSignal2D.push_back(rowSiBT2D);
        histSiBTStripSignal.push_back(firstStrip_Signal);
        histSiBTStripNoise.push_back(firstStrip_Noise);
    }

    // Create canvas
    name = fTitle + "Canvas";
    canSiBTStrip = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    name = fTitle + "SignalCanvas";
    canSiBTSignal = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * maxMods);
    name = fTitle + "StripSignalCanvas";
    canSiBTStripSignal = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    name = fTitle + "StripNoiseCanvas";
    canSiBTStripNoise = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    name = fTitle + "ChipSignalCanvas";
    canSiBTChipSignal = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);

    const vector<TString> LayerNames{"p+", "n+"};

    canSiBTStrip->Divide(maxMods, numberOfStations);
    canSiBTSignal->Divide(numberOfStations, maxMods);
    canSiBTStripSignal->Divide(maxLayers, sumMods);
    canSiBTStripNoise->Divide(maxMods, numberOfStations);
    canSiBTChipSignal->Divide(maxLayers, sumMods);

    Int_t modCtr = 0;   // filling SiBT Canvas' pads
    canStripPads.resize(maxMods * numberOfStations);
    canSignalPads.resize(maxMods * numberOfStations);
    canStripSignalPads.resize(sumMods * maxLayers);
    canStripNoisePads.resize(maxMods * numberOfStations);
    canChipSignalPads.resize(sumMods * maxLayers);
    NamesStrip.resize(maxMods * numberOfStations);
    NamesSignal.resize(maxMods * numberOfStations);
    NamesStripSignal.resize(sumMods * maxLayers);
    NamesStripNoise.resize(maxMods * numberOfStations);
    NamesChipSignal.resize(sumMods * maxLayers);

    for (Int_t iStation = 0; iStation < numberOfStations; iStation++) {
        BmnSiBTStation* st = sibtStationSet->GetStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnSiBTModule* mod = st->GetModule(iModule);
            PadInfo* p = new PadInfo();
            p->opt = "";
            Double_t l_x0 = 0.15;
            Double_t l_y0 = 0.75;
            Double_t l_w = 0.10;
            Double_t l_h = 0.15;
            TLegend* legend = new TLegend(l_x0, l_y0, l_x0 + l_w, l_y0 + l_h);
            legend->SetTextSize(0.03);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                legend->AddEntry(histSiBTStrip[iStation][iModule][iLayer], LayerNames[iLayer]);
                p->aux.push_back(histSiBTStrip[iStation][iModule][iLayer]);
            }
            p->legend = legend;
            Int_t iPad = modCtr * maxMods + iModule;
            p->name = Form(fTitle + "Occupancy" + "_Station_%d_module_%d", iStation, iModule);
            canStripPads[iPad] = p;
            canSiBTStrip->GetPad(iPad + 1)->SetGrid();
            NamesStrip[iPad] = canStripPads[iPad]->name;
            canSiBTStrip->GetPad(iPad + 1)->SetTitle(NamesStripNoise[iPad]);
        }
        modCtr++;
    }

    modCtr = 0;

    for (Int_t iStation = 0; iStation < numberOfStations; iStation++) {
        BmnSiBTStation* st = sibtStationSet->GetStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            // BmnSiBTModule *mod = st->GetModule(iModule);
            PadInfo* p = new PadInfo();
            p->opt = "colz";
            p->current = histSiBTSignal2D[iStation][iModule];
            Int_t iPad = modCtr * maxMods + iModule;
            canSignalPads[iPad] = p;
            canSiBTSignal->GetPad(iPad + 1)->SetGrid();
            NamesSignal[iPad] = canSignalPads[iPad]->current->GetName();
        }
        modCtr++;
    }

    modCtr = 0;

    for (Int_t iStation = 0; iStation < numberOfStations; iStation++) {
        BmnSiBTStation* st = sibtStationSet->GetStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnSiBTModule* mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                PadInfo* p = new PadInfo();
                p->opt = "colz";
                p->current = histSiBTStripSignal[iStation][iModule][iLayer];
                Int_t iPad = modCtr * maxLayers + iLayer;
                canStripSignalPads[iPad] = p;
                canSiBTStripSignal->GetPad(iPad + 1)->SetGrid();
                NamesStripSignal[iPad] = canStripSignalPads[iPad]->current->GetName();
            }
            modCtr++;
        }
    }
    modCtr = 0;

    for (Int_t iStation = 0; iStation < numberOfStations; iStation++) {
        BmnSiBTStation* st = sibtStationSet->GetStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnSiBTModule* mod = st->GetModule(iModule);
            // for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
            PadInfo* p = new PadInfo();
            p->opt = "";
            Double_t l_x0 = 0.15;
            Double_t l_y0 = 0.75;
            Double_t l_w = 0.10;
            Double_t l_h = 0.15;
            TLegend* legend = new TLegend(l_x0, l_y0, l_x0 + l_w, l_y0 + l_h);
            legend->SetTextSize(0.03);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                legend->AddEntry(histSiBTStripNoise[iStation][iModule][iLayer], LayerNames[iLayer]);
                p->aux.push_back(histSiBTStripNoise[iStation][iModule][iLayer]);
            }
            p->legend = legend;
            Int_t iPad = modCtr * maxMods + iModule;
            p->name = Form(fTitle + "StripVsNoise" + "_Station_%d_module_%d", iStation, iModule);
            canStripNoisePads[iPad] = p;
            canSiBTStripNoise->GetPad(iPad + 1)->SetGrid();
            NamesStripNoise[iPad] = canStripNoisePads[iPad]->name;
            canSiBTStripNoise->GetPad(iPad + 1)->SetTitle(NamesStripNoise[iPad]);
            // }
        }
        modCtr++;
    }
    modCtr = 0;

    for (Int_t iStation = 0; iStation < numberOfStations; iStation++) {
        BmnSiBTStation* st = sibtStationSet->GetStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnSiBTModule* mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                PadInfo* p = new PadInfo();
                p->opt = "";
                Double_t l_x0 = 0.15;
                Double_t l_y0 = 0.5;
                Double_t l_w = 0.15;
                Double_t l_h = 0.25;
                TLegend* legend = new TLegend(l_x0, l_y0, l_x0 + l_w, l_y0 + l_h);
                legend->SetTextSize(0.04);
                for (Int_t iChip = 0; iChip < numberOfChips; iChip++) {
                    legend->AddEntry(histSiBTChipSignal[iStation][iModule][iLayer][iChip], Form("ASIC# %d", iChip + 1));
                    p->aux.push_back(histSiBTChipSignal[iStation][iModule][iLayer][iChip]);
                }
                p->legend = legend;
                Int_t iPad = modCtr * maxLayers + iLayer;
                p->name = Form(fTitle + "ChipSignal" + "_Station_%d_module_%d", iStation, iModule);
                canChipSignalPads[iPad] = p;
                canSiBTChipSignal->GetPad(iPad + 1)->SetGrid();
                NamesChipSignal[iPad] = canChipSignalPads[iPad]->name;
                canSiBTChipSignal->GetPad(iPad + 1)->SetTitle(NamesChipSignal[iPad]);
            }
            modCtr++;
        }
    }
    modCtr = 0;
}

BmnHistSiBT::~BmnHistSiBT()
{
    delete canSiBTStrip;
    delete canSiBTSignal;
    delete canSiBTStripSignal;
    delete canSiBTStripNoise;
    delete canSiBTChipSignal;

    if (fDir)
        return;
    for (auto pad : canStripPads)
        delete pad;

    for (auto padSignal : canSignalPads)
        delete padSignal;

    for (auto padStripSignal : canStripSignalPads)
        delete padStripSignal;

    for (auto padStripNoise : canStripNoisePads)
        delete padStripNoise;

    for (auto padChipSignal : canChipSignalPads)
        delete padChipSignal;

    for (size_t iSt = 0; iSt < histSiBTStrip.size(); ++iSt) {
        for (size_t iMod = 0; iMod < histSiBTStrip[iSt].size(); ++iMod) {
            for (size_t iLayer = 0; iLayer < histSiBTStrip[iSt][iMod].size(); ++iLayer) {
                delete histSiBTStrip[iSt][iMod][iLayer];
                delete histSiBTStripSignal[iSt][iMod][iLayer];
                delete histSiBTStripNoise[iSt][iMod][iLayer];
                for (Int_t iChip = 0; iChip < numberOfChips; iChip++)
                    delete histSiBTChipSignal[iSt][iMod][iLayer][iChip];
            }
            delete histSiBTSignal2D[iSt][iMod];
        }
    }
}

void BmnHistSiBT::Register(THttpServer* serv)
{
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canSiBTStrip);
    fServer->Register(path, canSiBTSignal);
    fServer->Register(path, canSiBTStripSignal);
    fServer->Register(path, canSiBTStripNoise);
    fServer->Register(path, canSiBTChipSignal);
    fServer->SetItemField(path, "_monitoring", "2000");
    fServer->SetItemField(path, "_layout", "grid3x3");
    TString cmd = "/" + fName + "/->Reset()";
    TString cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    //    fServer->Restrict(cmdTitle.Data(), "deny=guest");
}

void BmnHistSiBT::SetDir(TFile* outFile, TTree* recoTree)
{
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    for (auto row : histSiBTStrip)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(fDir);

    for (auto rowSig : histSiBTSignal2D)
        for (auto colSig : rowSig)
            colSig->SetDirectory(fDir);

    for (auto row : histSiBTStripSignal)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(fDir);

    for (auto row : histSiBTStripNoise)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(fDir);

    for (auto row : histSiBTChipSignal)
        for (auto col : row)
            for (auto array : col)
                for (auto el : array)
                    el->SetDirectory(fDir);
}

void BmnHistSiBT::DrawBoth()
{
    BmnHist::DrawRef(canSiBTStrip, &canStripPads);
    BmnHist::DrawRef(canSiBTSignal, &canSignalPads);
    BmnHist::DrawRef(canSiBTStripSignal, &canStripSignalPads);
    BmnHist::DrawRef(canSiBTStripNoise, &canStripNoisePads);
    BmnHist::DrawRef(canSiBTChipSignal, &canChipSignalPads);
    return;
}

void BmnHistSiBT::FillFromDigi(DigiArrays* fDigiArrays)
{
    TClonesArray* SiBTDigits = fDigiArrays->sibt;
    if (!SiBTDigits)
        return;
    for (Int_t digIndex = 0; digIndex < SiBTDigits->GetEntriesFast(); digIndex++) {
        BmnSiBTDigit* gs = static_cast<BmnSiBTDigit*>(SiBTDigits->At(digIndex));
        Int_t module = gs->GetModule();
        Int_t station = gs->GetStation();
        Int_t layer = gs->GetStripLayer();
        Int_t SiBTStrip = gs->GetStripNumber();
        histSiBTStrip[station][module][layer]->Fill(SiBTStrip);
        histSiBTStripSignal[station][module][layer]->Fill(SiBTStrip, gs->GetStripSignal());
        histSiBTStripNoise[station][module][layer]->SetBinContent(SiBTStrip, gs->GetStripNoise());
        signalSiBT[station][module][layer] = gs->GetStripSignal();
        histSiBTChipSignal[station][module][layer][Int_t(SiBTStrip / (numberOfStrips / numberOfChips))]->Fill(
            gs->GetStripSignal());
    }

    for (size_t iStation = 0; iStation < signalSiBT.size(); ++iStation)
        for (size_t iModule = 0; iModule < signalSiBT[iStation].size(); ++iModule)
            if (signalSiBT[iStation][iModule][0] && signalSiBT[iStation][iModule][1]) {
                histSiBTSignal2D[iStation][iModule]->Fill(signalSiBT[iStation][iModule][0],
                                                          signalSiBT[iStation][iModule][1]);
            }
}

BmnStatus BmnHistSiBT::SetRefRun(Int_t id)
{
    if (refID != id) {
        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canStripPads, NamesStrip);
        //        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canSignalPads, NamesSignal);
        //        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canStripSignalPads, NamesStripSignal);
        DrawBoth();
    }

    return kBMNSUCCESS;
}

void BmnHistSiBT::ClearRefRun()
{
    printf("BmnHistSiBT : ClearRefRun\n");
    for (auto pad : canStripPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }

    for (auto padSignal : canSignalPads) {
        if (padSignal->ref)
            delete padSignal->ref;
        padSignal->ref = NULL;
    }

    for (auto padStripSignal : canStripSignalPads) {
        if (padStripSignal->ref)
            delete padStripSignal->ref;
        padStripSignal->ref = NULL;
    }

    for (auto padStripNoise : canStripNoisePads) {
        if (padStripNoise->ref)
            delete padStripNoise->ref;
        padStripNoise->ref = NULL;
    }

    for (auto padChipSignal : canChipSignalPads) {
        if (padChipSignal->ref)
            delete padChipSignal->ref;
        padChipSignal->ref = NULL;
    }

    refID = 0;
}

void BmnHistSiBT::Reset()
{
    printf("BmnHistSiBT : Reset histos\n");

    for (auto rowStripSignal : histSiBTStrip)
        for (auto colStripSignal : rowStripSignal)
            for (auto elStripSignal : colStripSignal)
                elStripSignal->Reset();

    for (auto row : histSiBTSignal2D)
        for (auto col : row)
            col->Reset();

    for (auto rowStripSignal : histSiBTStripSignal)
        for (auto colStripSignal : rowStripSignal)
            for (auto elStripSignal : colStripSignal)
                elStripSignal->Reset();

    for (auto rowStripNoise : histSiBTStripNoise)
        for (auto colStripNoise : rowStripNoise)
            for (auto elStripNoise : colStripNoise)
                elStripNoise->Reset();

    for (auto rowChipSignal : histSiBTChipSignal)
        for (auto colChipSignal : rowChipSignal)
            for (auto arrayChipSignal : colChipSignal)
                for (auto elChipSignal : arrayChipSignal)
                    elChipSignal->Reset();
}