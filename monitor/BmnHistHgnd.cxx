#include "BmnHistHgnd.h"

#include "TGaxis.h"

BmnHistHgnd::BmnHistHgnd(TString title, TString path, Int_t PeriodID, BmnSetup stp)
    : BmnHist(PeriodID, stp)
{
    fTitle = title;
    fName = title + "_cl";
    SetGeometry(Form("Hgnd_geo_period%d.root", PeriodID));
    TGaxis::SetMaxDigits(2);
    CreateHistos();
}

void BmnHistHgnd::SetGeometry(const std::string& path)
{
    LOG(debug) << "BmnHistHgnd::SetGeometry " << path;
    fGeoHandler = new BmnHgndGeo();
    fGeoHandler->ReadGeometryFromFile(path);
}

void BmnHistHgnd::CreateHistos()
{
    TString name;
    name = fTitle + "_h1d_ECells";
    h1d_ECells = new TH1F(name, "Hgnd signal in cells", 121, 1, 122);
    h1d_ECells->GetXaxis()->SetTitle("Cell id");
    h1d_ECells->GetXaxis()->SetTitleColor(kOrange + 10);
    h1d_ECells->GetYaxis()->SetTitle("Average Signal");
    h1d_ECells->GetYaxis()->SetTitleColor(kOrange + 10);

    name = fTitle + "_h2_grid";
    h2_grid = new TH2F(name, "Hgnd_Grid", 11, 1, 12, 11, 1, 12);
    h2_grid->GetXaxis()->SetTitle("X position");
    h2_grid->GetXaxis()->SetTitleColor(kOrange + 10);
    h2_grid->GetYaxis()->SetTitle("Y position");
    h2_grid->GetYaxis()->SetTitleColor(kOrange + 10);
    h2_grid->GetZaxis()->SetNoExponent(kFALSE);
    h2_grid->SetStats(0);

    name = fTitle + "_hProfCell";
    for (UInt_t uCell = 1; uCell <= kTOTAL_CELLS; ++uCell) {
        hProfCell.at(uCell) =
            new TH1F(name + Form("_%02u", uCell), Form("Cell_%02u", uCell), kTOTAL_LAYRS, 1, kTOTAL_LAYRS + 1);
        hProfCell.at(uCell)->GetXaxis()->SetTitle("Layer id");
        hProfCell.at(uCell)->GetXaxis()->SetTitleColor(kOrange + 10);
        hProfCell.at(uCell)->GetYaxis()->SetTitle("Average Signal");
        hProfCell.at(uCell)->GetYaxis()->SetTitleColor(kOrange + 10);
    }

    name = fTitle + "_h1d_ProfileSlice";
    h1d_ProfileSlice = new TH1F(name, "Energy in detector slice", kTOTAL_LAYRS, 1, kTOTAL_LAYRS + 1);
    h1d_ProfileSlice->GetXaxis()->SetTitle("Layer id");
    h1d_ProfileSlice->GetXaxis()->SetTitleColor(kOrange + 10);
    h1d_ProfileSlice->GetYaxis()->SetTitle("Events");
    h1d_ProfileSlice->GetYaxis()->SetTitleColor(kOrange + 10);

    name = fTitle + "_h1d_timemin";
    h1d_timemin = new TH1F(name, "Minimum timestamp in event", 1000, 0, 100);
    h1d_timemin->GetXaxis()->SetTitle("time");
    h1d_timemin->GetXaxis()->SetTitleColor(kOrange + 10);
    h1d_timemin->GetYaxis()->SetTitle("Events");
    h1d_timemin->GetYaxis()->SetTitleColor(kOrange + 10);

    name = fTitle + "CanvasAmplitudes";
    canAmps = new TCanvas(name, name, PAD_WIDTH * Hgnd_ROWS, PAD_HEIGHT * Hgnd_COLS);
    canAmps->Divide(Hgnd_ROWS, Hgnd_COLS);
    canAmpsPads.resize(Hgnd_ROWS * Hgnd_COLS);
    NamesAmps.resize(Hgnd_ROWS * Hgnd_COLS);
    PadInfo* p1 = new PadInfo();
    p1->current = h2_grid;
    p1->opt = "colz";   // p->SetLogz();
    canAmpsPads.at(0) = p1;
    //    NamesAmps.at(0) = p1->current->GetName();
    PadInfo* p2 = new PadInfo();
    p2->current = h1d_ECells;
    p2->opt = "hist";
    canAmpsPads.at(1) = p2;
    NamesAmps.at(1) = p2->current->GetName();

    name = fTitle + "CanvasProfiles";
    canv_hProfCell = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT);
    canv_hProfCell->Divide(11, 11);
    canv_hProfCellPads.resize(kTOTAL_CELLS);
    NamesProfs.resize(kTOTAL_CELLS);
    for (UInt_t uCell = 1; uCell <= kTOTAL_CELLS; ++uCell) {
        PadInfo* p = new PadInfo();
        canv_hProfCellPads.at(uCell - 1) = p;
        p->current = hProfCell.at(uCell);
        p->opt = "hist";
        NamesProfs.at(uCell - 1) = p->current->GetName();
    }

    name = fTitle + "CanvasSlice";
    canv_Slice = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT);
    canv_Slice->Divide(2, 1);
    canv_SlicePads.resize(2);
    NamesSlice.resize(2);
    PadInfo* pS1 = new PadInfo();
    canv_SlicePads.at(0) = pS1;
    pS1->current = h1d_ProfileSlice;
    pS1->opt = "hist";
    NamesSlice.at(0) = pS1->current->GetName();
    PadInfo* pS2 = new PadInfo();
    canv_SlicePads.at(1) = pS2;
    pS2->current = h1d_timemin;
    pS2->opt = "hist";
    NamesSlice.at(1) = pS2->current->GetName();

    name = fTitle + "_hChE";
    hChE = new TH1F(name, name, 500, 0, 35000);

    name = fTitle + "_wfmCh";
    wfmCh = new TH1F(name, name, 200, 0, 200);
    wfmCh->SetMarkerStyle(31);
    wfmCh->SetMarkerSize(0.5);

    name = fTitle + "CanvasSelection";
    canSelect = new TCanvas(name, name, PAD_WIDTH * Hgnd_ROWS, PAD_HEIGHT * Hgnd_COLS);
    canSelect->Divide(Hgnd_ROWS, Hgnd_COLS);
    canSelectPads.resize(Hgnd_ROWS * Hgnd_COLS);
    NamesSelect.resize(Hgnd_ROWS * Hgnd_COLS);
    PadInfo* ps1 = new PadInfo();
    ps1->current = hChE;
    ps1->opt = "hist";
    canSelectPads.at(0) = ps1;
    NamesSelect.at(0) = ps1->current->GetName();
    PadInfo* ps2 = new PadInfo();
    ps2->current = wfmCh;
    ps2->opt = "hist lp";
    canSelectPads.at(1) = ps2;
    NamesSelect.at(1) = ps2->current->GetName();

    name = fTitle + "_h2_Layer_Cell";
    h2_Layer_Cell =
        new TH2F(name, "h2_Layer_Cell", kTOTAL_LAYRS + 2, 0, kTOTAL_LAYRS + 2, kTOTAL_CELLS + 2, 0, kTOTAL_CELLS + 2);
    h2_Layer_Cell->GetXaxis()->SetTitle("Layer number");
    h2_Layer_Cell->GetXaxis()->SetTitleColor(kOrange + 10);
    h2_Layer_Cell->GetYaxis()->SetTitle("Cell number");
    h2_Layer_Cell->GetYaxis()->SetTitleColor(kOrange + 10);
    h2_Layer_Cell->GetZaxis()->SetNoExponent(kFALSE);
    h2_Layer_Cell->SetOption("colz");
    h2_Layer_Cell->SetStats(0);

    name = fTitle + "_h1_Cells";
    h1_Cells = new TH1F(name, "h1_Cells", 121, 1, 121);
    h1_Cells->GetXaxis()->SetTitle("Cell number");
    h1_Cells->GetXaxis()->SetTitleColor(kOrange + 10);
    h1_Cells->GetYaxis()->SetTitle("Number of hits / event");
    h1_Cells->GetYaxis()->SetTitleColor(kOrange + 10);
    h1_Cells->SetOption("hist");

    name = fTitle + "CanvasCells";
    canv_Cells = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT);
    canv_Cells->Divide(1, 2);
    canv_CellsPads.resize(2);
    NamesCells.resize(2);
    PadInfo* pC1 = new PadInfo();
    canv_CellsPads.at(0) = pC1;
    pC1->current = h2_Layer_Cell;
    pC1->opt = "colz";
    PadInfo* pC2 = new PadInfo();
    canv_CellsPads.at(1) = pC2;
    pC2->current = h1_Cells;
    pC2->opt = "hist";
    NamesCells.at(1) = pC2->current->GetName();

    name = fTitle + "_h2_Layers_Ampl";
    h2_lrampl = new TH2F(name, "h2_Layers_Ampl", 59, 0, 59, 121, 0, 122);
    h2_lrampl->GetXaxis()->SetTitle("X position");
    h2_lrampl->GetXaxis()->SetTitleColor(kOrange + 10);
    h2_lrampl->GetYaxis()->SetTitle("Y position");
    h2_lrampl->GetYaxis()->SetTitleColor(kOrange + 10);
    h2_lrampl->GetZaxis()->SetNoExponent(kFALSE);
    h2_lrampl->SetOption("colz");
    h2_lrampl->SetStats(0);

    name = fTitle + "_h1_Cells_Ampl";
    h1_CellsAmpl = new TH1F(name, "h1_Cells_Ampl", 121, 1, 122);
    h1_CellsAmpl->GetXaxis()->SetTitle("Cell number");
    h1_CellsAmpl->GetXaxis()->SetTitleColor(kOrange + 10);
    h1_CellsAmpl->GetYaxis()->SetTitle("Average amplitude");
    h1_CellsAmpl->GetYaxis()->SetTitleColor(kOrange + 10);
    h1_CellsAmpl->SetOption("hist");

    name = fTitle + "CanvasAmpl";
    canv_Ampl = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT / 2);
    canv_Ampl->Divide(1, 2);
    canv_AmplPads.resize(2);
    NamesAmpl.resize(2);
    PadInfo* pA1 = new PadInfo();
    canv_AmplPads.at(0) = pA1;
    pA1->current = h2_lrampl;
    pA1->opt = "colz";
    PadInfo* pA2 = new PadInfo();
    canv_AmplPads.at(1) = pA2;
    pA2->current = h1_CellsAmpl;
    pA2->opt = "hist";
    NamesAmpl.at(1) = pA2->current->GetName();

    name = fTitle + "_h2_Layers_Time";
    h2_lrtime = new TH2F(name, "h2_Layers_Time", 59, 0, 59, 121, 0, 121);
    h2_lrtime->GetXaxis()->SetTitle("X position");
    h2_lrtime->GetXaxis()->SetTitleColor(kOrange + 10);
    h2_lrtime->GetYaxis()->SetTitle("Y position");
    h2_lrtime->GetYaxis()->SetTitleColor(kOrange + 10);
    h2_lrtime->GetZaxis()->SetNoExponent(kFALSE);
    h2_lrtime->SetOption("colz");
    h2_lrtime->SetStats(0);

    name = fTitle + "_h1_Cells_Time";
    h1_CellsTime = new TH1F(name, "h1_Cells_Time", 121, 1, 122);
    h1_CellsTime->GetXaxis()->SetTitle("Cell number");
    h1_CellsTime->GetXaxis()->SetTitleColor(kOrange + 10);
    h1_CellsTime->GetYaxis()->SetTitle("Average time");
    h1_CellsTime->GetYaxis()->SetTitleColor(kOrange + 10);
    h1_CellsTime->SetOption("hist");

    name = fTitle + "CanvasTime";
    canv_Time = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT / 2);
    canv_Time->Divide(1, 2);
    canv_TimePads.resize(2);
    NamesTime.resize(2);
    PadInfo* pT1 = new PadInfo();
    canv_TimePads.at(0) = pT1;
    pT1->current = h2_lrtime;
    pT1->opt = "col0z";
    PadInfo* pT2 = new PadInfo();
    canv_TimePads.at(1) = pT2;
    pT2->current = h1_CellsTime;
    pT2->opt = "hist";
    NamesTime.at(1) = pT2->current->GetName();

    int itemp = 0;
    int il = 1;
    int ilch = 0;
    name = fTitle + "_hCellAmp";

    for (UInt_t uCell = 1; uCell <= kTOTAL_LAYRS * kTOTAL_CELLS; ++uCell) {
        itemp = ilch * 121 + il;
        hCellAmp.at(uCell) = new TH1F(name + Form("_%02u", itemp), Form("Cell_%02u", itemp), 100, 0, 10);
        hCellAmp.at(uCell)->GetXaxis()->SetTitle("Signal");
        hCellAmp.at(uCell)->GetXaxis()->SetTitleColor(kOrange + 10);
        ilch++;
        if (uCell % 6 == 0) {
            il++;
            ilch = 0;
        }
    }

    name = fTitle + "CanvasCellAmp";
    canv_hCellAmp = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT);
    canv_hCellAmp->Divide(11, 11, 0.0001, 0.0001);
    canv_hCellAmpPads.resize(kTOTAL_LAYRS * kTOTAL_CELLS);
    NamesCellAmp.resize(kTOTAL_LAYRS * kTOTAL_CELLS);
    for (UInt_t uCell = 1; uCell <= kTOTAL_LAYRS * kTOTAL_CELLS; ++uCell) {
        PadInfo* pa = new PadInfo();
        canv_hCellAmpPads.at(uCell - 1) = pa;
        pa->current = hCellAmp.at(uCell);
        pa->opt = "hist";
        NamesCellAmp.at(uCell - 1) = pa->current->GetName();
    }

    itemp = 0;
    il = 1;
    ilch = 0;
    name = fTitle + "_hCellTime";
    for (UInt_t uCell = 1; uCell <= kTOTAL_LAYRS * kTOTAL_CELLS; ++uCell) {
        itemp = ilch * 121 + il;
        hCellTime.at(uCell) = new TH1F(name + Form("_%02u", itemp), Form("Cell_%02u", itemp), 1000, -300, 300);
        hCellTime.at(uCell)->GetXaxis()->SetTitle("Time");
        hCellTime.at(uCell)->GetXaxis()->SetTitleColor(kOrange + 10);
        ilch++;
        if (uCell % 6 == 0) {
            il++;
            ilch = 0;
        }
    }

    name = fTitle + "CanvasCellTime";
    canv_hCellTime = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT);
    canv_hCellTime->Divide(11, 11, 0.0001, 0.0001);
    canv_hCellTimePads.resize(kTOTAL_LAYRS * kTOTAL_CELLS);
    NamesCellTime.resize(kTOTAL_LAYRS * kTOTAL_CELLS);
    for (UInt_t uCell = 1; uCell <= kTOTAL_LAYRS * kTOTAL_CELLS; ++uCell) {
        PadInfo* pt = new PadInfo();
        canv_hCellTimePads.at(uCell - 1) = pt;
        pt->current = hCellTime.at(uCell);
        pt->opt = "hist";
        NamesCellTime.at(uCell - 1) = pt->current->GetName();
    }
}

void BmnHistHgnd::Register(THttpServer* serv)
{
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canAmps);
    fServer->Register(path, canv_hProfCell);
    fServer->Register(path, canv_Slice);
    fServer->Register(path, canv_Cells);
    fServer->Register(path, canSelect);
    //    fServer->Register(path, h2_lrampl);
    //    fServer->Register(path, h1_CellsAmpl);
    fServer->Register(path, canv_Ampl);
    fServer->Register(path, canv_Time);
    fServer->Register(path, canv_hCellAmp);
    fServer->Register(path, canv_hCellTime);

    TString cmd = "/" + fName + "/->Reset()";
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout", "grid11x11");
    TString cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
    cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    cmdTitle = path + "ChangeSelection";
    fServer->RegisterCommand(cmdTitle, TString("/") + fName.Data() + "/->SetSelection(%arg1%,%arg2%,%arg3%)",
                             "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
}

void BmnHistHgnd::SetDir(TFile* outFile = NULL, TTree* recoTree = NULL)
{
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    h1d_ECells->SetDirectory(fDir);
    h2_grid->SetDirectory(fDir);
    h1d_ProfileSlice->SetDirectory(fDir);
    h1d_timemin->SetDirectory(fDir);
    for (UInt_t uCell = 1; uCell <= kTOTAL_CELLS; ++uCell)
        hProfCell.at(uCell)->SetDirectory(fDir);
    for (UInt_t uCell = 1; uCell <= kTOTAL_LAYRS * kTOTAL_CELLS; ++uCell)
        hCellAmp.at(uCell)->SetDirectory(fDir);
    for (UInt_t uCell = 1; uCell <= kTOTAL_LAYRS * kTOTAL_CELLS; ++uCell)
        hCellTime.at(uCell)->SetDirectory(fDir);
    h2_Layer_Cell->SetDirectory(fDir);
    h1_Cells->SetDirectory(fDir);
    hChE->SetDirectory(fDir);
    wfmCh->SetDirectory(fDir);
    h2_lrampl->SetDirectory(fDir);
    h1_CellsAmpl->SetDirectory(fDir);
    h2_lrtime->SetDirectory(fDir);
    h1_CellsTime->SetDirectory(fDir);
}

void BmnHistHgnd::SetSelection(Int_t Cell, Int_t Layer, Int_t Trigger)
{
    fSelectedCell = Cell;
    fSelectedLayer = Layer;
    fSelectedTrigger = Trigger;
    TString command = "(1)";
    TString digiInTree = "HgndDigi";
    if (fSelectedCell >= 0)
        command = command + Form("&& %s.GetCellId() == %d", digiInTree.Data(), fSelectedCell);
    if (fSelectedLayer >= 0)
        command = command + Form("&& %s.GetLayerId() == %d", digiInTree.Data(), fSelectedLayer);
    TString triggerCondition = "";
    if (fSelectedTrigger >= 0)
        triggerCondition = Form(" && %s trigger bit == %d (bitBT=2, bitBC1=3, bitMBT=6, bitNiT=4, bitCCT1=5, "
                                "bitCCT2=8, bitCCT3=12, bitFHCAL=9)",
                                digiInTree.Data(), fSelectedTrigger);
    TString titleSignal = Form("#splitline{Signal %s}{%s}", command.Data(), triggerCondition.Data());
    TString titleWfm = Form("#splitline{Wfm %s}{%s}", command.Data(), triggerCondition.Data());
    hChE->Reset();
    wfmCh->Reset();
    if (frecoTree != NULL) {
        hChE->SetTitle(titleSignal);
        TString direction = Form("%s.fAmpl>>%s", digiInTree.Data(), hChE->GetName());
        frecoTree->Draw(direction.Data(), command.Data(), "goff");
        wfmCh->SetTitle(titleWfm);
    }
}

void BmnHistHgnd::DrawBoth()
{
    BmnHist::DrawRef(canAmps, &canAmpsPads);
    BmnHist::DrawRef(canv_hProfCell, &canv_hProfCellPads);
    BmnHist::DrawRef(canv_hCellAmp, &canv_hCellAmpPads);
    BmnHist::DrawRef(canv_hCellTime, &canv_hCellTimePads);
    BmnHist::DrawRef(canv_Slice, &canv_SlicePads);
    BmnHist::DrawRef(canv_Cells, &canv_CellsPads);
    BmnHist::DrawRef(canSelect, &canSelectPads);
    BmnHist::DrawRef(canv_Ampl, &canv_AmplPads);
    BmnHist::DrawRef(canv_Time, &canv_TimePads);
    return;
}

void BmnHistHgnd::FillFromDigi(DigiArrays* fDigiArrays)
{
    BmnEventHeader* head = fDigiArrays->header;
    TClonesArray* digits = fDigiArrays->hgnd;
    if (!digits)
        return;

    h1d_ECells->Scale(fEventCounter);
    h2_grid->Scale(fEventCounter);
    h1d_ProfileSlice->Scale(fEventCounter);
    h2_Layer_Cell->Scale(fEventCounter);
    h1_Cells->Scale(fEventCounter);
    for (UInt_t uCell = 1; uCell <= kTOTAL_CELLS; ++uCell)
        hProfCell.at(uCell)->Scale(fEventCounter);

    double trigtimeshift = 0.0;
    for (Int_t iDig = 0; iDig < digits->GetEntriesFast(); iDig++) {
        BmnHgndDigi* digi = (BmnHgndDigi*)digits->At(iDig);
        if (digi->GetLayerId() == 17 && digi->GetRowId() == 1 && digi->GetColumnId() == 1) {
            trigtimeshift = digi->GetTime();
        }
    }

    double mintime = numeric_limits<float>::max();
    for (Int_t iDig = 0; iDig < digits->GetEntriesFast(); iDig++) {
        BmnHgndDigi* digi = (BmnHgndDigi*)digits->At(iDig);
        double signal = digi->GetSignal();
        double time = 200 - trigtimeshift + digi->GetTime();
        auto row = digi->GetRowId();
        auto col = digi->GetColumnId();
        auto cell = (row - 1) * 11 + col;
        auto layer = digi->GetLayerId();
        if (layer == 0)
            continue;
        const auto& positionMap = fGeoHandler->GetPositionMap();
        auto it = positionMap.find(digi->GetAddress());
        if (it == positionMap.end())
            continue;
        // float realXpos = it->second.first.X();
        // float realYpos = it->second.first.Y();
        // float realZpos = position.Z();
        if (time < mintime)
            mintime = time;
        // unsigned int channel = MapCell2Ch.at(cell);
        unsigned int channel = cell;
        h1d_ECells->Fill(channel, signal);
        h2_grid->Fill(col, row, signal);
        h2_Layer_Cell->Fill(layer, channel, signal);

        UInt_t TrigPattern = head->GetInputSignalsAR();
        //        auto bitBT = (TrigPattern >> 2) & 1;      // BT
        //        auto bitBC1 = (TrigPattern >> 3) & 1;     // BC1
        //        auto bitMBT = (TrigPattern >> 6) & 1;     // MBT
        //        auto bitNiT = (TrigPattern >> 4) & 1;     // NiT
        //        auto bitCCT1 = (TrigPattern >> 5) & 1;    // CCT1
        //        auto bitCCT2 = (TrigPattern >> 8) & 1;    // CCT2
        //        auto bitCCT3 = (TrigPattern >> 12) & 1;   // CCT3
        //        auto bitFHCAL = (TrigPattern >> 9) & 1;   // FHCAL

        /////////////////////////////TODO FIXME
        bool isSelected = ((Int_t)cell == fSelectedCell) && ((Int_t)digi->GetLayerId() == fSelectedLayer);
        if (isSelected) {
            if ((TrigPattern >> fSelectedTrigger) & 1) {
                hChE->Fill(digi->fAmpl);
                auto Wfm = digi->GetWfm();
                std::vector<float> points(Wfm.size());
                std::iota(std::begin(points), std::end(points), 0);   // Fill with 0, 1, ..., wfm.back().
                wfmCh->Reset();
                wfmCh->GetXaxis()->SetRangeUser(0, Wfm.size());
                for (UInt_t wfm_iter = 0; wfm_iter < Wfm.size(); wfm_iter++)
                    wfmCh->Fill(wfm_iter, Wfm.at(wfm_iter));
            }
        }

        hProfCell.at(channel)->Fill(digi->GetLayerId(), signal);
        h1d_ProfileSlice->Fill(digi->GetLayerId(), signal);

        int ch_flat_idx = channel + (layer - 1) * kTOTAL_CELLS;
        h1_Cells->Fill(ch_flat_idx, 1);

        meanamp.at(ch_flat_idx - 1) = (meanamp.at(ch_flat_idx - 1) * ch_entries.at(ch_flat_idx - 1) + signal)
                                      / (ch_entries.at(ch_flat_idx - 1) + 1);
        meantime.at(ch_flat_idx - 1) = (meantime.at(ch_flat_idx - 1) * ch_entries.at(ch_flat_idx - 1) + time)
                                       / (ch_entries.at(ch_flat_idx - 1) + 1);
        ch_entries.at(ch_flat_idx - 1) += 1.0;
        /////////////////////////////TODO FIXME begin
        int ix = col + (layer - 1) * 11 + (layer - 1);
        h2_lrampl->SetBinContent(ix, row, meanamp.at(ch_flat_idx - 1));
        h1_CellsAmpl->SetBinContent(ch_flat_idx, meanamp.at(ch_flat_idx - 1));
        h2_lrtime->SetBinContent(ix, row, meantime.at(ch_flat_idx - 1));
        h1_CellsTime->SetBinContent(ch_flat_idx, meantime.at(ch_flat_idx - 1));
        /////////////////////////////TODO FIXME end
        // printf("channel: %d realX:%f realY:%f LR:%d GetX:%d GetY:%d mean: %f \n", channel, realXpos, realYpos, layer,
        // digi->GetX(), digi->GetY(), mean);
        int itemp = ((ch_flat_idx - 1) % 121) * 1 + layer;
        hCellAmp.at(itemp)->Fill(signal);
        hCellTime.at(itemp)->Fill(time);
    }
    fEventCounter++;

    h1d_ECells->Scale(1. / fEventCounter);
    h2_grid->Scale(1. / fEventCounter);
    h1d_ProfileSlice->Scale(1. / fEventCounter);
    for (UInt_t uCell = 1; uCell <= kTOTAL_CELLS; ++uCell)
        hProfCell.at(uCell)->Scale(1. / fEventCounter);
    h2_Layer_Cell->Scale(1. / fEventCounter);
    h1_Cells->Scale(1. / fEventCounter);

    h1d_timemin->Fill(mintime);
}

BmnStatus BmnHistHgnd::SetRefRun(Int_t id)
{
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canAmpsPads, NamesAmps);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canv_hProfCellPads, NamesProfs);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canv_hCellAmpPads, NamesCellAmp);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canv_hCellTimePads, NamesCellTime);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canv_SlicePads, NamesSlice);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canv_CellsPads, NamesCells);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canv_AmplPads, NamesAmpl);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canv_TimePads, NamesTime);
        DrawBoth();
    }
    return kBMNSUCCESS;
}

void BmnHistHgnd::ClearRefRun()
{
    for (auto pad : canAmpsPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canv_hProfCellPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canv_hCellAmpPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canv_hCellTimePads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canv_SlicePads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canv_CellsPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canv_AmplPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canv_TimePads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

void BmnHistHgnd::Reset()
{
    printf("BmnHistHgnd : Reset histos\n");
    fEventCounter = 1;
    h1d_ECells->Reset();
    h2_grid->Reset();
    h1d_ProfileSlice->Reset();
    h1d_timemin->Reset();
    hChE->Reset();
    wfmCh->Reset();
    for (UInt_t uCell = 1; uCell <= kTOTAL_CELLS; ++uCell)
        hProfCell.at(uCell)->Reset();
    for (UInt_t uCell = 1; uCell <= kTOTAL_LAYRS * kTOTAL_CELLS; ++uCell)
        hCellAmp.at(uCell)->Reset();
    for (UInt_t uCell = 1; uCell <= kTOTAL_LAYRS * kTOTAL_CELLS; ++uCell)
        hCellTime.at(uCell)->Reset();
    h2_Layer_Cell->Reset();
    h1_Cells->Reset();
    h2_lrampl->Reset();
    h1_CellsAmpl->Reset();
    h2_lrtime->Reset();
    h1_CellsTime->Reset();

    std::fill(std::begin(meanamp), std::end(meanamp), 0.0);
    std::fill(std::begin(meantime), std::end(meantime), 0.0);
    std::fill(std::begin(ch_entries), std::end(ch_entries), 1.0);
}
BmnHistHgnd::~BmnHistHgnd()
{
    if (fGeoHandler)
        delete fGeoHandler;
    delete canAmps;
    delete canv_hProfCell;
    delete canv_hCellAmp;
    delete canv_hCellTime;
    delete canv_Slice;
    delete canv_Cells;
    delete canSelect;
    delete canv_Ampl;
    delete canv_Time;
    if (fDir != NULL)
        return;
    delete h1d_ECells;
    delete h2_grid;
    delete h1d_ProfileSlice;
    delete h1d_timemin;
    delete hChE;
    delete wfmCh;
    for (UInt_t uCell = 1; uCell <= kTOTAL_CELLS; ++uCell)
        delete hProfCell.at(uCell);
    for (UInt_t uCell = 1; uCell <= kTOTAL_LAYRS * kTOTAL_CELLS; ++uCell)
        delete hCellAmp.at(uCell);
    for (UInt_t uCell = 1; uCell <= kTOTAL_LAYRS * kTOTAL_CELLS; ++uCell)
        delete hCellTime.at(uCell);
    delete h2_Layer_Cell;
    delete h1_Cells;
    delete h2_lrampl;
    delete h1_CellsAmpl;
    delete h2_lrtime;
    delete h1_CellsTime;
}
