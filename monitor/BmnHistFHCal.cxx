#include "BmnHistFHCal.h"

#include "TGaxis.h"

BmnHistFHCal::BmnHistFHCal(TString title, TString path, Int_t PeriodID, BmnSetup stp)
    : BmnHist(PeriodID, stp)
{
    fTitle = title;
    fName = title + "_cl";
    SetGeometry(Form("FHCal_geo_period%d.root", PeriodID));
    TGaxis::SetMaxDigits(2);
    CreateHistos();
}

void BmnHistFHCal::SetGeometry(const std::string& path)
{
    LOG(debug) << "BmnHistFHCal::SetGeometry " << path;
    fGeoHandler = new BmnFHCalGeo();
    fGeoHandler->ReadGeometryFromFile(path, false);
}

void BmnHistFHCal::CreateHistos()
{
    TString name;
    name = fTitle + "_h1d_Emodules";
    h1d_Emodules = new TH1F(name, "FHCal signal in modules", kTOTAL_MODULES, 1, kTOTAL_MODULES + 1);
    h1d_Emodules->GetXaxis()->SetTitle("Module id");
    h1d_Emodules->GetXaxis()->SetTitleColor(kOrange + 10);
    h1d_Emodules->GetYaxis()->SetTitle("Average Signal [MIP]");
    h1d_Emodules->GetYaxis()->SetTitleColor(kOrange + 10);
    h1d_Emodules->SetStats(0);

    name = fTitle + "_h2_calo";
    h2_calo = new TH2F(name, "FHCal_Grid", 32, -80, 80, 25, -60, 60);
    h2_calo->GetXaxis()->SetTitle("X position [mm]");
    h2_calo->GetXaxis()->SetTitleColor(kOrange + 10);
    h2_calo->GetYaxis()->SetTitle("Y position [mm]");
    h2_calo->GetYaxis()->SetTitleColor(kOrange + 10);
    h2_calo->GetZaxis()->SetNoExponent(kFALSE);
    h2_calo->SetStats(0);

    name = fTitle + "_h2_calo_text";
    h2_calo_text = (TH2F*)h2_calo->Clone(name.Data());
    h2_calo_text->SetMarkerColor(kBlack);
    h2_calo_text->SetStats(0);

    name = fTitle + "_hModuleProfile";
    for (UInt_t uMod = 1; uMod <= kTOTAL_MODULES; ++uMod) {
        hModuleProfile.at(uMod) = new TH1F(fTitle + Form("_Profile_Mod_%02u", uMod), Form("Mod_%02u", uMod), 10, 1, 11);
        hModuleProfile.at(uMod)->SetTitle(Form("Mod %03u", uMod));
        hModuleProfile.at(uMod)->SetTitleSize(0.9, "t");
        hModuleProfile.at(uMod)->GetXaxis()->SetTitle("Section id");
        hModuleProfile.at(uMod)->GetXaxis()->SetTitleColor(kOrange + 10);
        hModuleProfile.at(uMod)->GetYaxis()->SetTitle("Average Signal [MIP]");
        hModuleProfile.at(uMod)->GetYaxis()->SetTitleColor(kOrange + 10);
        hModuleProfile.at(uMod)->SetStats(0);
    }

    name = fTitle + "CanvasAmplitudes";
    canAmps = new TCanvas(name, name, PAD_WIDTH * FHCal_ROWS, PAD_HEIGHT * FHCal_COLS);
    canAmps->Divide(FHCal_ROWS, FHCal_COLS);
    canAmpsPads.resize(FHCal_ROWS * FHCal_COLS);
    NamesAmps.resize(FHCal_ROWS * FHCal_COLS);
    PadInfo* p1 = new PadInfo();
    p1->aux = {h2_calo, h2_calo_text};
    p1->auxOpt = {"colz logz", "text"};
    canAmpsPads.at(0) = p1;
    PadInfo* p2 = new PadInfo();
    p2->current = h1d_Emodules;
    p2->opt = "hist";
    canAmpsPads.at(1) = p2;
    NamesAmps.at(1) = p2->current->GetName();

    canProfsPads.resize(kTOTAL_MODULES + 1, nullptr);   // pads vector. + 1 blank
    NamesProfs.resize(kTOTAL_MODULES + 1, nullptr);     // pads vector. + 1 blank

    name = fTitle + "CanvasProfilesSmall";
    canProfsSmall = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT);
    canProfsSmall->Divide(5, 7);
    // Hardcode modules positions. TODO it better!
    int ModToPadSmall[35] = {
        1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, -1,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34};   // leaving blank pad for beamhole
    for (UInt_t iPad = 0; iPad < 35; ++iPad) {
        PadInfo* p = new PadInfo();
        canProfsPads.at(iPad) = p;
        if (ModToPadSmall[iPad] < 0)
            continue;
        UInt_t uMod = (UInt_t)ModToPadSmall[iPad];
        hModuleProfile.at(uMod)->GetXaxis()->SetRangeUser(1, 8);
        p->current = hModuleProfile.at(uMod);
        p->opt = "hist";
        NamesProfs.at(iPad) = p->current->GetName();
    }

    name = fTitle + "CanvasProfilesBig";
    canProfsBig = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT);
    canProfsBig->Divide(4, 5);
    int ModToPadBig[20] = {35, 36, 45, 46, 37, 38, 47, 48, 39, 40, 49, 50, 41, 42, 51, 52, 43, 44, 53, 54};
    for (UInt_t iPad = 0; iPad < 20; ++iPad) {
        PadInfo* p = new PadInfo();
        canProfsPads.at(iPad + 35) = p;
        UInt_t uMod = (UInt_t)ModToPadBig[iPad];
        p->current = hModuleProfile.at(uMod);
        p->opt = "hist";
        NamesProfs.at(iPad + 35) = p->current->GetName();
    }

    name = fTitle + "_hSecE";
    hSecE = new TH1F(name, name, 300, 0, 35000);
    hSecE->SetStats(0);
    hSecE->GetYaxis()->SetTitle("Counts");
    hSecE->GetXaxis()->SetTitle("Apml");

    name = fTitle + "_wfmSec";
    wfmSec = new TH1F(name, name, 200, 0, 200);
    wfmSec->SetMarkerStyle(31);
    wfmSec->SetMarkerSize(0.5);
    wfmSec->SetStats(0);

    name = fTitle + "CanvasSelection";
    canSelect = new TCanvas(name, name, PAD_WIDTH * 1, PAD_HEIGHT * 2);
    canSelect->Divide(1, 2);
    canSelectPads.resize(1 * 2);
    NamesSelect.resize(1 * 2);
    PadInfo* ps1 = new PadInfo();
    ps1->current = hSecE;
    ps1->opt = "hist";
    canSelectPads.at(0) = ps1;
    NamesSelect.at(0) = ps1->current->GetName();
    PadInfo* ps2 = new PadInfo();
    ps2->current = wfmSec;
    ps2->opt = "hist lp";
    canSelectPads.at(1) = ps2;
    NamesSelect.at(1) = ps2->current->GetName();

    name = fTitle + "_h2_mod_sec";
    h2_mod_sec = new TH2F(name, "FHCal - Module vs Section", 12, 0, 11, 55, 0, 55);
    h2_mod_sec->GetXaxis()->SetTitle("Section Id");
    h2_mod_sec->GetXaxis()->SetTitleColor(kOrange + 10);
    h2_mod_sec->GetYaxis()->SetTitle("Module Id");
    h2_mod_sec->GetYaxis()->SetTitleColor(kOrange + 10);
    h2_mod_sec->GetZaxis()->SetNoExponent(kFALSE);
    h2_mod_sec->SetStats(0);
    name = fTitle + "Canvas_mod_sec";
    can_mod_sec = new TCanvas(name, name, 1, 1);
    can_mod_sec->Divide(1, 1);
    can_mod_secPads.resize(1 * 1);
    Names_mod_sec.resize(1 * 1);
    PadInfo* pms = new PadInfo();
    pms->current = h2_mod_sec;
    pms->opt = "colz logz";
    can_mod_secPads.at(0) = pms;

    name = fTitle + "_h1_totalE";
    h1_totalE = new TH1F(name, "FHCal total energy", 700, 0, 2100);
    h1_totalE->GetXaxis()->SetTitle("Reconstructed energy [MIP]");
    h1_totalE->GetXaxis()->SetTitleColor(kOrange + 10);
    h1_totalE->GetYaxis()->SetTitle("Counts / events");
    h1_totalE->GetYaxis()->SetTitleColor(kOrange + 10);
    h1_totalE->SetStats(0);
    name = fTitle + "Canvas_totalE";
    can_totalE = new TCanvas(name, name, 1, 1);
    can_totalE->Divide(1, 1);
    can_totalEPads.resize(1 * 1);
    Names_totalE.resize(1 * 1);
    PadInfo* pte = new PadInfo();
    pte->current = h1_totalE;
    pte->opt = "hist";
    can_totalEPads.at(0) = pte;

    name = fTitle + "_h2_totalE_hodoZ2_BT";
    h2_totalE_hodoZ2_BT = new TH2F(name, "FHCal total energy vs Hodo Z^2 BT", 70, 0, 2100000, 50, 0, 3000);
    h2_totalE_hodoZ2_BT->GetXaxis()->SetTitle("fIntegral");
    h2_totalE_hodoZ2_BT->GetXaxis()->SetTitleColor(kOrange + 10);
    h2_totalE_hodoZ2_BT->GetYaxis()->SetTitle("Hodo Z^2");
    h2_totalE_hodoZ2_BT->GetYaxis()->SetTitleColor(kOrange + 10);
    h2_totalE_hodoZ2_BT->SetStats(0);
    name = fTitle + "_h2_totalE_hodoZ2_CCT2";
    h2_totalE_hodoZ2_CCT2 = new TH2F(name, "FHCal total energy vs Hodo Z^2 CCT2", 70, 0, 2100000, 50, 0, 3000);
    h2_totalE_hodoZ2_CCT2->GetXaxis()->SetTitle("fIntegral");
    h2_totalE_hodoZ2_CCT2->GetXaxis()->SetTitleColor(kOrange + 10);
    h2_totalE_hodoZ2_CCT2->GetYaxis()->SetTitle("Hodo Z^2");
    h2_totalE_hodoZ2_CCT2->GetYaxis()->SetTitleColor(kOrange + 10);
    h2_totalE_hodoZ2_CCT2->SetStats(0);
    name = fTitle + "Canvas_totalE_hodoZ2";
    can_totalE_hodoZ2 = new TCanvas(name, name, PAD_WIDTH * 2, PAD_HEIGHT * 1);
    can_totalE_hodoZ2->Divide(2, 1);
    can_totalE_hodoZ2Pads.resize(2 * 1);
    Names_totalE_hodoZ2.resize(2 * 1);
    PadInfo* pfh1 = new PadInfo();
    pfh1->current = h2_totalE_hodoZ2_BT;
    pfh1->opt = "colz logz";
    can_totalE_hodoZ2Pads.at(0) = pfh1;
    Names_totalE_hodoZ2.at(0) = pfh1->current->GetName();
    PadInfo* pfh2 = new PadInfo();
    pfh2->current = h2_totalE_hodoZ2_CCT2;
    pfh2->opt = "colz logz";
    can_totalE_hodoZ2Pads.at(1) = pfh2;
    Names_totalE_hodoZ2.at(1) = pfh2->current->GetName();

    name = fTitle + "_h2_ZL_nEvent";
    h2_ZL_nEvent = new TH2F(name, "FHCal - ZL - nEvent", 500, 0, 10000, 50, -50, 50);
    h2_ZL_nEvent->GetYaxis()->SetTitle("fZL");
    h2_ZL_nEvent->GetYaxis()->SetTitleColor(kOrange + 10);
    h2_ZL_nEvent->GetXaxis()->SetTitle("nEvent");
    h2_ZL_nEvent->GetXaxis()->SetTitleColor(kOrange + 10);
    h2_ZL_nEvent->GetZaxis()->SetNoExponent(kFALSE);
    h2_ZL_nEvent->SetStats(0);
    name = fTitle + "Canvas_ZL_nEvent";
    can_ZL_nEvent = new TCanvas(name, name, 1, 1);
    can_ZL_nEvent->Divide(1, 1);
    can_ZL_nEventPads.resize(1 * 1);
    Names_ZL_nEvent.resize(1 * 1);
    PadInfo* pzl = new PadInfo();
    pzl->current = h2_ZL_nEvent;
    pzl->opt = "colz logz";
    can_ZL_nEventPads.at(0) = pzl;

    name = fTitle + "_h2_mod_ZL_nEvent";
    for (UInt_t uMod = 1; uMod <= kTOTAL_MODULES; ++uMod) {
        h2_mod_ZL_nEvent.at(uMod) =
            new TH2F(fTitle + Form("_ZL_Mod_%02u", uMod), Form("ZL_Mod_%02u", uMod), 500, 0, 10000, 50, -50, 50);
        h2_mod_ZL_nEvent.at(uMod)->SetTitle(Form("Mod %03u", uMod));
        h2_mod_ZL_nEvent.at(uMod)->SetTitleSize(0.9, "t");
        h2_mod_ZL_nEvent.at(uMod)->GetXaxis()->SetTitle("nEvent");
        h2_mod_ZL_nEvent.at(uMod)->GetXaxis()->SetTitleColor(kOrange + 10);
        h2_mod_ZL_nEvent.at(uMod)->GetYaxis()->SetTitle("fZL");
        h2_mod_ZL_nEvent.at(uMod)->GetYaxis()->SetTitleColor(kOrange + 10);
        h2_mod_ZL_nEvent.at(uMod)->GetZaxis()->SetNoExponent(kFALSE);
        h2_mod_ZL_nEvent.at(uMod)->SetStats(0);
    }

    canZLPads.resize(kTOTAL_MODULES + 1, nullptr);   // pads vector. + 1 blank
    NamesZL.resize(kTOTAL_MODULES + 1, nullptr);     // pads vector. + 1 blank

    name = fTitle + "CanvasZLSmall";
    canZLSmall = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT);
    canZLSmall->Divide(5, 7);
    for (UInt_t iPad = 0; iPad < 35; ++iPad) {
        PadInfo* p = new PadInfo();
        canZLPads.at(iPad) = p;
        if (ModToPadSmall[iPad] < 0)
            continue;
        UInt_t uMod = (UInt_t)ModToPadSmall[iPad];
        p->current = h2_mod_ZL_nEvent.at(uMod);
        p->opt = "colz logz";
        NamesZL.at(iPad) = p->current->GetName();
    }

    name = fTitle + "CanvasZLBig";
    canZLBig = new TCanvas(name, name, PAD_WIDTH, PAD_HEIGHT);
    canZLBig->Divide(4, 5);
    for (UInt_t iPad = 0; iPad < 20; ++iPad) {
        PadInfo* p = new PadInfo();
        canZLPads.at(iPad + 35) = p;
        UInt_t uMod = (UInt_t)ModToPadBig[iPad];
        p->current = h2_mod_ZL_nEvent.at(uMod);
        p->opt = "colz logz";
        NamesZL.at(iPad + 35) = p->current->GetName();
    }
}

void BmnHistFHCal::Register(THttpServer* serv)
{
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canAmps);
    fServer->Register(path, canProfsSmall);
    fServer->Register(path, canProfsBig);
    fServer->Register(path, canSelect);
    fServer->Register(path, can_mod_sec);
    fServer->Register(path, can_totalE);
    fServer->Register(path, can_totalE_hodoZ2);
    fServer->Register(path, can_ZL_nEvent);
    fServer->Register(path, canZLSmall);
    fServer->Register(path, canZLBig);
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
    fServer->RegisterCommand(cmdTitle, TString("/") + fName.Data() + "/->SetSelection(%arg1%,%arg2%,%arg3%)",
                             "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
}

void BmnHistFHCal::SetDir(TFile* outFile = NULL, TTree* recoTree = NULL)
{
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    h1d_Emodules->SetDirectory(fDir);
    h2_calo->SetDirectory(fDir);
    h2_calo_text->SetDirectory(fDir);
    hSecE->SetDirectory(fDir);
    wfmSec->SetDirectory(fDir);
    h2_mod_sec->SetDirectory(fDir);
    h2_ZL_nEvent->SetDirectory(fDir);
    h1_totalE->SetDirectory(fDir);
    h2_totalE_hodoZ2_BT->SetDirectory(fDir);
    h2_totalE_hodoZ2_CCT2->SetDirectory(fDir);
    for (UInt_t uMod = 1; uMod <= kTOTAL_MODULES; ++uMod) {
        hModuleProfile.at(uMod)->SetDirectory(fDir);
        h2_mod_ZL_nEvent.at(uMod)->SetDirectory(fDir);
    }
}

void BmnHistFHCal::SetSelection(Int_t Module, Int_t Section, Int_t Trigger)
{
    fSelectedModule = Module;
    fSelectedSection = Section;
    fSelectedTrigger = Trigger;
    TString command = "(1)";
    TString digiInTree = "FHCalDigi";
    if (fSelectedModule >= 0)
        command = command + Form(" && %s.GetModuleId() == %d", digiInTree.Data(), fSelectedModule);
    if (fSelectedSection >= 0)
        command = command + Form(" && %s.GetSectionId() == %d", digiInTree.Data(), fSelectedSection);
    TString triggerCondition = "";
    if (fSelectedTrigger >= 0)
        triggerCondition = Form(" && %s trigger bit == %d (bitBT=2, bitBC1=3, bitMBT=6, bitNiT=4, bitCCT1=5, "
                                "bitCCT2=8, bitCCT3=12, bitFHCAL=9)",
                                digiInTree.Data(), fSelectedTrigger);
    TString titleSignal = Form("#splitline{Signal %s}{%s}", command.Data(), triggerCondition.Data());
    hSecE->SetTitle(titleSignal);
    TString titleWfm = Form("#splitline{Wfm %s}{%s}", command.Data(), triggerCondition.Data());
    wfmSec->SetTitle(titleWfm);
    hSecE->Reset();
    wfmSec->Reset();
    if (frecoTree != NULL) {
        TString direction = Form("%s.fAmpl>>%s", digiInTree.Data(), hSecE->GetName());
        frecoTree->Draw(direction.Data(), command.Data(), "goff");
        wfmSec->SetTitle(titleWfm);
    }
}

void BmnHistFHCal::DrawBoth()
{
    BmnHist::DrawRef(canAmps, &canAmpsPads);
    std::vector<PadInfo*> hsubPadsSmall(&canProfsPads[0], &canProfsPads[35]);
    BmnHist::DrawRef(canProfsSmall, &hsubPadsSmall);
    std::vector<PadInfo*> hsubPadsBig(canProfsPads.begin() + 35, canProfsPads.end());
    BmnHist::DrawRef(canProfsBig, &hsubPadsBig);
    std::vector<PadInfo*> hsubPadsZLSmall(&canZLPads[0], &canZLPads[35]);
    BmnHist::DrawRef(canZLSmall, &hsubPadsZLSmall);
    std::vector<PadInfo*> hsubPadsZLBig(canZLPads.begin() + 35, canZLPads.end());
    BmnHist::DrawRef(canZLBig, &hsubPadsZLBig);
    BmnHist::DrawRef(canSelect, &canSelectPads);
    BmnHist::DrawRef(can_mod_sec, &can_mod_secPads);
    BmnHist::DrawRef(can_ZL_nEvent, &can_ZL_nEventPads);
    BmnHist::DrawRef(can_totalE, &can_totalEPads);
    BmnHist::DrawRef(can_totalE_hodoZ2, &can_totalE_hodoZ2Pads);
    return;
}

void BmnHistFHCal::FillFromDigi(DigiArrays* fDigiArrays)
{
    BmnEventHeader* head = fDigiArrays->header;
    TClonesArray* digits = fDigiArrays->fhcal;
    if (!digits)
        return;

    // Hardcoded FHCal sampling factor. Taken from SRC 2022 run
    // double beam_pos = 193;     // beam position in mips
    // double momentum = 3.5;     // AGeV/c
    // double m_parton = 0.939;   // GeV
    // double energ = sqrt(momentum * momentum + m_parton * m_parton);
    // double kin_energ = 12 * (energ - m_parton);
    // double Mip2GeV = kin_energ / beam_pos;
    double sumE = 0;
    double sumInt = 0;

    h1d_Emodules->Scale(fEventCounter);
    h2_calo->Scale(fEventCounter);
    h2_mod_sec->Scale(fEventCounter);
    h1_totalE->Scale(fEventCounter);
    h2_totalE_hodoZ2_BT->Scale(fEventCounter);
    h2_totalE_hodoZ2_CCT2->Scale(fEventCounter);
    for (UInt_t uMod = 1; uMod <= kTOTAL_MODULES; ++uMod)
        hModuleProfile.at(uMod)->Scale(fEventCounter);

    for (Int_t iDig = 0; iDig < digits->GetEntriesFast(); iDig++) {
        BmnFHCalDigi* digi = (BmnFHCalDigi*)digits->At(iDig);
        double signal = digi->GetSignal();   // * Mip20GeV;
        h1d_Emodules->Fill(digi->GetModuleId(), signal);

        const auto& positionMap = fGeoHandler->GetPositionMap();
        auto it = positionMap.find(digi->GetAddress());
        if (it == positionMap.end())
            continue;
        float realXpos = it->second.first.X();
        float realYpos = it->second.first.Y();
        // float realZpos = position.Z();
        h2_calo->Fill(realXpos, realYpos, signal);
        h2_mod_sec->Fill(digi->GetSectionId(), digi->GetModuleId(), signal);
        sumE += signal;
        sumInt += digi->fIntegral;

        h2_ZL_nEvent->Fill(fEventCounterZL, digi->fZL);
        h2_mod_ZL_nEvent.at(digi->GetModuleId())->Fill(fEventCounterZL, digi->fZL);

        // TODO it better! fill one time
        int bin_x = h2_calo_text->GetXaxis()->FindBin(realXpos);
        int bin_y = h2_calo_text->GetYaxis()->FindBin(realYpos);
        int BC = h2_calo_text->GetBinContent(bin_x, bin_y);
        if (BC == 0)
            h2_calo_text->Fill(realXpos, realYpos, digi->GetModuleId());

        UInt_t TrigPattern = head->GetInputSignalsAR();
        //        auto bitBT = (TrigPattern >> 2) & 1;      // BT
        //        auto bitBC1 = (TrigPattern >> 3) & 1;     // BC1
        //        auto bitMBT = (TrigPattern >> 6) & 1;     // MBT
        //        auto bitNiT = (TrigPattern >> 4) & 1;     // NiT
        //        auto bitCCT1 = (TrigPattern >> 5) & 1;    // CCT1
        //        auto bitCCT2 = (TrigPattern >> 8) & 1;    // CCT2
        //        auto bitCCT3 = (TrigPattern >> 12) & 1;   // CCT3
        //        auto bitFHCAL = (TrigPattern >> 9) & 1;   // FHCAL

        bool isSelected =
            ((Int_t)digi->GetModuleId() == fSelectedModule) && ((Int_t)digi->GetSectionId() == fSelectedSection);
        if (isSelected) {
            if ((TrigPattern >> fSelectedTrigger) & 1) {
                hSecE->Fill(digi->fAmpl);
                auto Wfm = digi->GetWfm();
                std::vector<float> points(Wfm.size());
                std::iota(std::begin(points), std::end(points), 0);   // Fill with 0, 1, ..., wfm.back().
                wfmSec->Reset();
                wfmSec->GetXaxis()->SetRangeUser(0, Wfm.size());
                for (UInt_t wfm_iter = 0; wfm_iter < Wfm.size(); wfm_iter++)
                    wfmSec->Fill(wfm_iter, Wfm.at(wfm_iter));
            }
        }

        hModuleProfile.at(digi->GetModuleId())->Fill(digi->GetSectionId(), signal);
    }
    fEventCounter++;
    fEventCounterZL++;
    if (sumE > 0.5)
        h1_totalE->Fill(sumE);
    h1_totalE->Scale(1. / fEventCounter);
    h1d_Emodules->Scale(1. / fEventCounter);
    h2_calo->Scale(1. / fEventCounter);
    h2_mod_sec->Scale(1. / fEventCounter);
    for (UInt_t uMod = 1; uMod <= kTOTAL_MODULES; ++uMod)
        hModuleProfile.at(uMod)->Scale(1. / fEventCounter);
    if (fEventCounterZL >= 10000) {
        h2_ZL_nEvent->Reset();
        for (UInt_t uMod = 1; uMod <= kTOTAL_MODULES; ++uMod) {
            h2_mod_ZL_nEvent.at(uMod)->Reset();
        }
        fEventCounterZL = 0;
    }
    TClonesArray* digits_hodo = fDigiArrays->hodo;
    if (!digits_hodo)
        return;
    double z2 = 0;
    float responses[kNstrips + 1][kNsides];
    memset(responses, 0, sizeof(responses[0][0]) * (kNstrips + 1) * kNsides);
    for (Int_t iDig = 0; iDig < digits_hodo->GetEntriesFast(); iDig++) {
        BmnHodoDigi* digi = (BmnHodoDigi*)digits_hodo->At(iDig);
        double signal = digi->GetSignal();
        auto strip = digi->GetStripId();
        auto side = digi->GetStripSide();
        //        auto gain = digi->GetGain();
        //        if(gain != 1) continue;
        responses[strip][side] = signal;
        z2 += signal;
    }
    //    for (int i = 1; i <= kNstrips; i++)
    //        if(responses[i][0] * responses[i][1] > 0.001)
    //            z2 = (responses[i][0] + responses[i][1])/2;
    //        if(responses[i][0] > 0.001)
    //            z2 = responses[i][0];
    if (z2 > 0.5 && sumInt > 0.5) {
        UInt_t TrigPattern = head->GetInputSignalsAR();
        auto bitBT = (TrigPattern >> 2) & 1;   // BT
        //        auto bitBC1 = (TrigPattern >> 3) & 1; //BC1
        auto bitMBT = (TrigPattern >> 6) & 1;   // MBT
        //        auto bitNiT = (TrigPattern >> 4) & 1; //NiT
        auto bitCCT1 = (TrigPattern >> 5) & 1;    // CCT1
        auto bitCCT2 = (TrigPattern >> 8) & 1;    // CCT2
        auto bitCCT3 = (TrigPattern >> 12) & 1;   // CCT3
        auto bitFHCAL = (TrigPattern >> 9) & 1;   // FHCAL
        bool trigBT =
            (bitBT == 1) && (bitMBT == 0) && (bitCCT1 == 0) && (bitCCT2 == 0) && (bitCCT3 == 0) && (bitFHCAL == 0);
        // bool trigMBT = (bitMBT == 1) && (bitNiT == 0);
        // bool trigCCT1 = (bitCCT1 == 1) && (bitMBT == 0) && (bitCCT2 == 0);
        bool trigCCT2 = (bitCCT2 == 1);
        if (trigBT)
            h2_totalE_hodoZ2_BT->Fill(sumInt, z2);
        if (trigCCT2)
            h2_totalE_hodoZ2_CCT2->Fill(sumInt, z2);
    }
    h2_totalE_hodoZ2_BT->Scale(1. / fEventCounter);
    h2_totalE_hodoZ2_CCT2->Scale(1. / fEventCounter);
}

BmnStatus BmnHistFHCal::SetRefRun(Int_t id)
{
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canAmpsPads, NamesAmps);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canProfsPads, NamesProfs);
        DrawBoth();
    }
    return kBMNSUCCESS;
}

void BmnHistFHCal::ClearRefRun()
{
    for (auto pad : canAmpsPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canProfsPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

void BmnHistFHCal::Reset()
{
    printf("BmnHistFHCal : Reset histos\n");
    fEventCounter = 1;
    h1d_Emodules->Reset();
    h2_calo->Reset();
    h2_calo_text->Reset();
    wfmSec->Reset();
    hSecE->Reset();
    h2_mod_sec->Reset();
    //    h2_ZL_nEvent->Reset();
    h1_totalE->Reset();
    h2_totalE_hodoZ2_BT->Reset();
    h2_totalE_hodoZ2_CCT2->Reset();
    for (UInt_t uMod = 1; uMod <= kTOTAL_MODULES; ++uMod) {
        hModuleProfile.at(uMod)->Reset();
        //      h2_mod_ZL_nEvent.at(uMod)->Reset();
    }
}
BmnHistFHCal::~BmnHistFHCal()
{
    if (fGeoHandler)
        delete fGeoHandler;
    delete canAmps;
    delete canProfsSmall;
    delete canProfsBig;
    delete canSelect;
    delete can_mod_sec;
    delete can_ZL_nEvent;
    if (fDir != NULL)
        return;
    delete h1d_Emodules;
    delete h2_calo;
    delete h2_calo_text;
    delete wfmSec;
    delete hSecE;
    delete h2_mod_sec;
    delete h2_ZL_nEvent;
    delete h1_totalE;
    delete h2_totalE_hodoZ2_BT;
    delete h2_totalE_hodoZ2_CCT2;
    for (UInt_t uMod = 1; uMod <= kTOTAL_MODULES; ++uMod) {
        delete hModuleProfile.at(uMod);
        delete h2_mod_ZL_nEvent.at(uMod);
    }
}
