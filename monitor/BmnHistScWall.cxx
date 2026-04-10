#include "BmnHistScWall.h"

#include "TGaxis.h"

BmnHistScWall::BmnHistScWall(TString title, TString path, Int_t PeriodID, BmnSetup stp)
    : BmnHist(PeriodID, stp)
{
    fTitle = title;
    fName = title + "_cl";
    SetGeometry(Form("ScWall_geo_period%d.root", PeriodID));
    TGaxis::SetMaxDigits(2);
    CreateHistos();
}

void BmnHistScWall::SetGeometry(const std::string& path)
{
    LOG(debug) << "BmnHistScWall::SetGeometry " << path;
    fGeoHandler = new BmnScWallGeo();
    fGeoHandler->ReadGeometryFromFile(path, false);
}

void BmnHistScWall::CreateHistos()
{
    TString name;
    name = fTitle + "_h2d_Grid";
    h2d_grid = new TH2F(name, "ScWall Grid", 18, -135, 135, 8, -60, 60);
    // h2d_grid = new TH2F(name, "ScWall Grid", fUniqueXpos.size()-1, &fUniqueXpos[0], fUniqueYpos.size()-1,
    // &fUniqueYpos[0]);
    h2d_grid->GetXaxis()->SetTitle("X [cm]");
    h2d_grid->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_grid->GetYaxis()->SetTitle("Y [cm]");
    h2d_grid->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_grid->GetZaxis()->SetNoExponent(kFALSE);
    //  h2d_grid->SetBarOffset(-0.2);
    h2d_grid->SetStats(0);

    name = fTitle + "_h2d_GridCenter";
    h2d_grid_center = new TH2F(name, "ScWall Grid center", 36, -135, 135, 16, -60, 60);
    //    h2d_grid_center = new TH2F(name, "ScWall Grid center", 10, -30, 45, 4, -15, 15);
    h2d_grid_center->GetXaxis()->SetTitle("X [cm]");
    h2d_grid_center->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_grid_center->GetYaxis()->SetTitle("Y [cm]");
    h2d_grid_center->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_grid_center->GetZaxis()->SetNoExponent(kFALSE);
    //  h2d_grid_center->SetBarOffset(-0.4);
    h2d_grid_center->SetMarkerSize(0.6);
    h2d_grid_center->SetStats(0);

    name = fTitle + "_h2d_profile";
    h2d_profile = new TH2F(name, "ScWall Profile", 175, 0, 175, 200, 0, 100);
    h2d_profile->GetXaxis()->SetTitle("Cell id");
    h2d_profile->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_profile->GetYaxis()->SetTitle("dE/dx [Z^2]");
    h2d_profile->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_profile->SetStats(0);
    name = fTitle + "CanvasAmplitudes";

    // text histos
    name = fTitle + "_h2d_cell_id";
    h2d_cell_id = new TH2F(name, "h2d_cell_id", 18, -135, 135, 8, -60, 60);
    h2d_cell_id->GetXaxis()->SetTitle("X [cm]");
    h2d_cell_id->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_cell_id->GetYaxis()->SetTitle("Y [cm]");
    h2d_cell_id->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_cell_id->GetZaxis()->SetNoExponent(kFALSE);
    h2d_cell_id->SetMarkerSize(0.8);
    h2d_cell_id->SetMarkerColor(kBlack);
    // h2d_cell_id->SetBarOffset(0.2);

    name = fTitle + "_h2d_cell_id_center";
    h2d_cell_id_center = new TH2F(name, "h2d_cell_id_center", 10, -45, 30, 4, -15, 15);
    h2d_cell_id_center->GetXaxis()->SetTitle("X [cm]");
    h2d_cell_id_center->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_cell_id_center->GetYaxis()->SetTitle("Y [cm]");
    h2d_cell_id_center->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_cell_id_center->GetZaxis()->SetNoExponent(kFALSE);
    h2d_cell_id_center->SetMarkerSize(0.6);
    h2d_cell_id_center->SetMarkerColor(kBlack);
    // h2d_cell_id_center->SetBarOffset(-0.2);

    name = fTitle + "CanvasAmplitudes";
    canAmps = new TCanvas(name, name, PAD_WIDTH * ScWall_ROWS, PAD_HEIGHT * ScWall_COLS);
    canAmps->Divide(ScWall_ROWS, ScWall_COLS);
    canAmpsPads.resize(ScWall_ROWS * ScWall_COLS);
    NamesAmps.resize(ScWall_ROWS * ScWall_COLS);
    PadInfo* p1 = new PadInfo();
    p1->aux = {h2d_grid_center, h2d_grid, h2d_cell_id, h2d_cell_id_center};
    p1->auxOpt = {"colz logz", "colz", "text", "text"};

    canAmpsPads[0] = p1;
    PadInfo* p2 = new PadInfo();
    p2->current = h2d_profile;
    p2->opt = "colz logy logz";
    canAmpsPads[1] = p2;

    name = fTitle + "_hCellAmpl";
    hCellAmpl = new TH1F(name, name, 400, 0, 35000);
    hCellAmpl->SetStats(0);
    hCellAmpl->GetYaxis()->SetTitle("Counts");
    hCellAmpl->GetXaxis()->SetTitle("fAmpl");

    name = fTitle + "_hCellE";
    hCellE = new TH1F(name, name, 400, 0, 3000);
    hCellE->SetStats(0);
    hCellE->GetYaxis()->SetTitle("Counts");
    hCellE->GetXaxis()->SetTitle("dE/dx");

    name = fTitle + "_wfmCell";
    wfmCell = new TH1F(name, name, 100, 0, 100);
    wfmCell->SetMarkerStyle(31);
    wfmCell->SetMarkerSize(0.5);
    wfmCell->SetStats(0);

    name = fTitle + "CanvasSelection";
    canSelect = new TCanvas(name, name, PAD_WIDTH * 1, PAD_HEIGHT * 3);
    canSelect->Divide(1, 3);
    canSelectPads.resize(1 * 3);
    NamesSelect.resize(1 * 3);
    PadInfo* ps1 = new PadInfo();
    ps1->current = hCellAmpl;
    ps1->opt = "hist";
    canSelectPads.at(0) = ps1;
    NamesSelect.at(0) = ps1->current->GetName();
    PadInfo* ps2 = new PadInfo();
    ps2->current = hCellE;
    ps2->opt = "hist";
    canSelectPads.at(1) = ps2;
    NamesSelect.at(1) = ps2->current->GetName();
    PadInfo* ps3 = new PadInfo();
    ps3->current = wfmCell;
    ps3->opt = "hist lp";
    canSelectPads.at(2) = ps3;
    NamesSelect.at(2) = ps3->current->GetName();

    name = fTitle + "_hModEn_scwall_nt";
    hModEn_scwall_nt = new TH1F(name, "ScWall amp in cells", 200, 0, 200);
    hModEn_scwall_nt->GetXaxis()->SetTitle("Cell id");
    hModEn_scwall_nt->GetXaxis()->SetTitleColor(kOrange + 10);
    hModEn_scwall_nt->GetYaxis()->SetTitle("dE/dx [Z^2]");
    hModEn_scwall_nt->GetYaxis()->SetTitleColor(kOrange + 10);
    hModEn_scwall_nt->SetOption("hist");
    hModEn_scwall_nt->SetStats(0);

    name = fTitle + "CanvasModEn";
    canModEn = new TCanvas(name, name, 1, 1);
    canModEn->Divide(1, 1);
    canModEnPads.resize(1);
    NamesModEn.resize(1);
    PadInfo* pm = new PadInfo();
    pm->current = hModEn_scwall_nt;
    canModEnPads.at(0) = pm;
    NamesModEn.at(0) = pm->current->GetName();
}

void BmnHistScWall::Register(THttpServer* serv)
{
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canAmps);
    fServer->Register(path, canModEn);
    fServer->Register(path, canSelect);
    TString cmd = "/" + fName + "/->Reset()";
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout", "grid3x3");
    TString cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
    cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    cmdTitle = path + "ChangeSelection";
    fServer->RegisterCommand(cmdTitle, TString("/") + fName.Data() + "/->SetSelection(%arg1%,%arg2%)", "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
}

void BmnHistScWall::SetDir(TFile* outFile = NULL, TTree* recoTree = NULL)
{
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    h2d_grid->SetDirectory(fDir);
    h2d_grid_center->SetDirectory(fDir);
    h2d_profile->SetDirectory(fDir);
    hCellE->SetDirectory(fDir);
    hCellAmpl->SetDirectory(fDir);
    wfmCell->SetDirectory(fDir);
    h2d_cell_id->SetDirectory(fDir);
    h2d_cell_id_center->SetDirectory(fDir);
    hModEn_scwall_nt->SetDirectory(fDir);
}

void BmnHistScWall::SetSelection(Int_t Cell, Int_t Trigger)
{
    fSelectedCell = Cell;
    fSelectedTrigger = Trigger;
    TString command = "(1)";
    TString digiInTree = "ScWallDigi";
    if (fSelectedCell >= 0)
        command = Form(" && %s.GetCellId() == %d", digiInTree.Data(), fSelectedCell);
    TString triggerCondition = "";
    if (fSelectedTrigger >= 0)
        triggerCondition = Form(" && %s trigger bit == %d (bitBT=2, bitBC1=3, bitMBT=6, bitNiT=4, bitCCT1=5, "
                                "bitCCT2=8, bitCCT3=12, bitFHCAL=9)",
                                digiInTree.Data(), fSelectedTrigger);
    TString titleSignal = Form("#splitline{Signal %s}{%s}", command.Data(), triggerCondition.Data());
    TString titleAmpl = Form("#splitline{Ampl %s}{%s}", command.Data(), triggerCondition.Data());
    TString titleWfm = Form("#splitline{Wfm %s}{%s}", command.Data(), triggerCondition.Data());
    hCellE->SetTitle(titleSignal);
    hCellAmpl->SetTitle(titleAmpl);
    wfmCell->SetTitle(titleWfm);
    hCellE->Reset();
    hCellAmpl->Reset();
    wfmCell->Reset();
    if (frecoTree != NULL) {
        TString direction = Form("%s.fAmpl>>%s", digiInTree.Data(), hCellE->GetName());
        frecoTree->Draw(direction.Data(), command.Data(), "goff");
        wfmCell->SetTitle(titleWfm);
    }
}

void BmnHistScWall::DrawBoth()
{
    BmnHist::DrawRef(canAmps, &canAmpsPads);
    BmnHist::DrawRef(canModEn, &canModEnPads);
    BmnHist::DrawRef(canSelect, &canSelectPads);
    return;
}

void BmnHistScWall::FillFromDigi(DigiArrays* fDigiArrays)
{
    BmnEventHeader* head = fDigiArrays->header;
    TClonesArray* digits = fDigiArrays->scwall;
    if (!digits)
        return;

    h2d_grid->Scale(fEventCounter);
    h2d_grid_center->Scale(fEventCounter);
    hModEn_scwall_nt->Scale(fEventCounter);

    // Loop over digis
    for (Int_t iDig = 0; iDig < digits->GetEntriesFast(); iDig++) {
        BmnScWallDigi* digi = (BmnScWallDigi*)digits->At(iDig);
        double signal = digi->GetSignal();
        UInt_t cell_id = digi->GetCellId();
        if (cell_id > kTOTAL_CELLS)
            continue;
        const auto& positionMap = fGeoHandler->GetPositionMap();
        auto it = positionMap.find(digi->GetAddress());
        if (it == positionMap.end())
            continue;
        float x_pos = it->second.first.X();
        float y_pos = it->second.first.Y();

        UInt_t TrigPattern = head->GetInputSignalsAR();
        // auto bitBT = (TrigPattern >> 2) & 1;      // BT
        // auto bitBC1 = (TrigPattern >> 3) & 1;     // BC1
        // auto bitMBT = (TrigPattern >> 6) & 1;     // MBT
        // auto bitNiT = (TrigPattern >> 4) & 1;     // NiT
        // auto bitCCT1 = (TrigPattern >> 5) & 1;    // CCT1
        // auto bitCCT2 = (TrigPattern >> 8) & 1;    // CCT2
        // auto bitCCT3 = (TrigPattern >> 12) & 1;   // CCT3
        // auto bitFHCAL = (TrigPattern >> 9) & 1;   // FHCAL

        bool isSelected = ((Int_t)digi->GetCellId() == fSelectedCell);
        if (isSelected) {
            if ((TrigPattern >> fSelectedTrigger) & 1) {
                hCellAmpl->Fill(digi->fAmpl);
                hCellE->Fill(signal);
                auto Wfm = digi->GetWfm();
                std::vector<float> points(Wfm.size());
                std::iota(std::begin(points), std::end(points), 0);   // Fill with 0, 1, ..., wfm.back().
                wfmCell->Reset();
                wfmCell->GetXaxis()->SetRangeUser(0, Wfm.size());
                for (UInt_t wfm_iter = 0; wfm_iter < Wfm.size(); wfm_iter++)
                    wfmCell->Fill(wfm_iter, Wfm.at(wfm_iter));
            }
        }

        h2d_profile->Fill(digi->GetCellId(), signal);
        hModEn_scwall_nt->Fill(cell_id, signal);
        if (cell_id <= 40)   // inner block of the wall
        {
            h2d_grid_center->Fill(x_pos, y_pos, signal);
            int bin_x = h2d_cell_id_center->GetXaxis()->FindBin(x_pos);
            int bin_y = h2d_cell_id_center->GetYaxis()->FindBin(y_pos);
            int BC = h2d_cell_id_center->GetBinContent(bin_x, bin_y);
            if (BC == 0)
                h2d_cell_id_center->Fill(x_pos, y_pos, cell_id);
        } else   // outer block of the wall
        {
            h2d_grid->Fill(x_pos, y_pos, signal);
            int bin_x = h2d_cell_id->GetXaxis()->FindBin(x_pos);
            int bin_y = h2d_cell_id->GetYaxis()->FindBin(y_pos);
            int BC = h2d_cell_id->GetBinContent(bin_x, bin_y);
            if (BC == 0)
                h2d_cell_id->Fill(x_pos, y_pos, cell_id);
        }
    }
    fEventCounter++;
    h2d_grid->Scale(1. / fEventCounter);
    h2d_grid_center->Scale(1. / fEventCounter);
    hModEn_scwall_nt->Scale(1. / fEventCounter);
}

BmnStatus BmnHistScWall::SetRefRun(Int_t id)
{
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canAmpsPads, NamesAmps);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canModEnPads, NamesModEn);
        DrawBoth();
    }
    return kBMNSUCCESS;
}

void BmnHistScWall::ClearRefRun()
{
    for (auto pad : canAmpsPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canModEnPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

void BmnHistScWall::Reset()
{
    printf("BmnHistScWall : Reset histos\n");
    fEventCounter = 1;
    h2d_grid->Reset();
    h2d_grid_center->Reset();
    h2d_profile->Reset();
    h2d_cell_id->Reset();
    h2d_cell_id_center->Reset();
    wfmCell->Reset();
    hCellE->Reset();
    hCellAmpl->Reset();
    hModEn_scwall_nt->Reset();
}

BmnHistScWall::~BmnHistScWall()
{
    if (fGeoHandler)
        delete fGeoHandler;
    delete canAmps;
    delete canModEn;
    delete canSelect;
    if (fDir != NULL)
        return;
    delete h2d_grid;
    delete h2d_grid_center;
    delete h2d_profile;
    delete h2d_cell_id;
    delete h2d_cell_id_center;
    delete wfmCell;
    delete hCellE;
    delete hCellAmpl;
    delete hModEn_scwall_nt;
}
