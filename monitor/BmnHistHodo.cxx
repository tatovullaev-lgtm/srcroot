#include "BmnHistHodo.h"

#include "TGaxis.h"

#include <RtypesCore.h>

BmnHistHodo::BmnHistHodo(TString title, TString path)
    : BmnHist()
{
    fTitle = title;
    fName = title + "_cl";
    TGaxis::SetMaxDigits(2);
    CreateHistos();
}

void BmnHistHodo::CreateHistos()
{
    TString name;
    name = fTitle + "_down_low";
    h2d_EstripDL =
        new TH2F(name, "Hodo signal in strips side DOWN gain LOW", kNstrips + 2, 0, kNstrips + 2, 250, 0, 5000);
    h2d_EstripDL->GetXaxis()->SetTitle("Strip id");
    h2d_EstripDL->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_EstripDL->GetYaxis()->SetTitle("Z^2");
    h2d_EstripDL->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_EstripDL->SetStats(0);
    name = fTitle + "_up_low";
    h2d_EstripUL =
        new TH2F(name, "Hodo signal in strips side UP gain LOW", kNstrips + 2, 0, kNstrips + 2, 250, 0, 5000);
    h2d_EstripUL->GetXaxis()->SetTitle("Strip id");
    h2d_EstripUL->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_EstripUL->GetYaxis()->SetTitle("Z^2");
    h2d_EstripUL->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_EstripUL->SetStats(0);
    name = fTitle + "_down_high";
    h2d_EstripDH =
        new TH2F(name, "Hodo signal in strips side DOWN gain HIGH", kNstrips + 2, 0, kNstrips + 2, 250, 0, 5000);
    h2d_EstripDH->GetXaxis()->SetTitle("Strip id");
    h2d_EstripDH->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_EstripDH->GetYaxis()->SetTitle("Z^2");
    h2d_EstripDH->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_EstripDH->SetStats(0);
    name = fTitle + "_up_high";
    h2d_EstripUH =
        new TH2F(name, "Hodo signal in strips side UP gain HIGH", kNstrips + 2, 0, kNstrips + 2, 250, 0, 5000);
    h2d_EstripUH->GetXaxis()->SetTitle("Strip id");
    h2d_EstripUH->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_EstripUH->GetYaxis()->SetTitle("Z^2");
    h2d_EstripUH->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_EstripUH->SetStats(0);

    name = fTitle + "Canvas_h2d";
    canAmps = new TCanvas(name, name, PAD_WIDTH * Hodo_ROWS, PAD_HEIGHT * Hodo_COLS);
    canAmps->Divide(Hodo_ROWS, Hodo_COLS);
    canAmpsPads.resize(Hodo_ROWS * Hodo_COLS);
    NamesAmps.resize(Hodo_ROWS * Hodo_COLS);
    PadInfo* puh2 = new PadInfo();
    puh2->current = h2d_EstripUH;
    puh2->opt = "colz logz";
    canAmpsPads.at(0) = puh2;
    NamesAmps.at(0) = puh2->current->GetName();
    PadInfo* pul2 = new PadInfo();
    pul2->current = h2d_EstripUL;
    pul2->opt = "colz logz";
    canAmpsPads.at(1) = pul2;
    NamesAmps.at(1) = pul2->current->GetName();
    PadInfo* pdh2 = new PadInfo();
    pdh2->current = h2d_EstripDH;
    pdh2->opt = "colz logz";
    canAmpsPads.at(2) = pdh2;
    NamesAmps.at(2) = pdh2->current->GetName();
    PadInfo* pdl2 = new PadInfo();
    pdl2->current = h2d_EstripDL;
    pdl2->opt = "colz logz";
    canAmpsPads.at(3) = pdl2;
    NamesAmps.at(3) = pdl2->current->GetName();

    name = fTitle + "_profile_down_low";
    h1d_profileDL = new TH1F(name, "Hodo signal in strips side DOWN gain LOW", kNstrips + 2, 0, kNstrips + 2);
    h1d_profileDL->GetXaxis()->SetTitle("Strip id");
    h1d_profileDL->GetXaxis()->SetTitleColor(kOrange + 10);
    h1d_profileDL->GetYaxis()->SetTitle("dE/dx [Z^2]");
    h1d_profileDL->GetYaxis()->SetTitleColor(kOrange + 10);
    h1d_profileDL->SetStats(0);
    name = fTitle + "_profile_up_low";
    h1d_profileUL = new TH1F(name, "Hodo signal in strips side UP gain LOW", kNstrips + 2, 0, kNstrips + 2);
    h1d_profileUL->GetXaxis()->SetTitle("Strip id");
    h1d_profileUL->GetXaxis()->SetTitleColor(kOrange + 10);
    h1d_profileUL->GetYaxis()->SetTitle("dE/dx [Z^2]");
    h1d_profileUL->GetYaxis()->SetTitleColor(kOrange + 10);
    h1d_profileUL->SetStats(0);
    name = fTitle + "_profile_down_high";
    h1d_profileDH = new TH1F(name, "Hodo signal in strips side DOWN gain HIGH", kNstrips + 2, 0, kNstrips + 2);
    h1d_profileDH->GetXaxis()->SetTitle("Strip id");
    h1d_profileDH->GetXaxis()->SetTitleColor(kOrange + 10);
    h1d_profileDH->GetYaxis()->SetTitle("dE/dx [Z^2]");
    h1d_profileDH->GetYaxis()->SetTitleColor(kOrange + 10);
    h1d_profileDH->SetStats(0);
    name = fTitle + "_profile_up_high";
    h1d_profileUH = new TH1F(name, "Hodo signal in strips side UP gain HIGH", kNstrips + 2, 0, kNstrips + 2);
    h1d_profileUH->GetXaxis()->SetTitle("Strip id");
    h1d_profileUH->GetXaxis()->SetTitleColor(kOrange + 10);
    h1d_profileUH->GetYaxis()->SetTitle("dE/dx [Z^2]");
    h1d_profileUH->GetYaxis()->SetTitleColor(kOrange + 10);
    h1d_profileUH->SetStats(0);

    name = fTitle + "Canvas_h1d_prof";
    canProf = new TCanvas(name, name, PAD_WIDTH * Hodo_ROWS, PAD_HEIGHT * Hodo_COLS);
    canProf->Divide(Hodo_ROWS, Hodo_COLS);
    canProfPads.resize(Hodo_ROWS * Hodo_COLS);
    NamesProf.resize(Hodo_ROWS * Hodo_COLS);
    PadInfo* puh1 = new PadInfo();
    puh1->current = h1d_profileUH;
    puh1->opt = "hist";
    canProfPads.at(0) = puh1;
    NamesProf.at(0) = puh1->current->GetName();
    PadInfo* pul1 = new PadInfo();
    pul1->current = h1d_profileUL;
    pul1->opt = "hist";
    canProfPads.at(1) = pul1;
    NamesProf.at(1) = pul1->current->GetName();
    PadInfo* pdh1 = new PadInfo();
    pdh1->current = h1d_profileDH;
    pdh1->opt = "hist";
    canProfPads.at(2) = pdh1;
    NamesProf.at(2) = pdh1->current->GetName();
    PadInfo* pdl1 = new PadInfo();
    pdl1->current = h1d_profileDL;
    pdl1->opt = "hist";
    canProfPads.at(3) = pdl1;
    NamesProf.at(3) = pdl1->current->GetName();

    // TODO it better!
    name = fTitle + "_canv_proj_UH";
    canv_proj_UH = new TCanvas(name, name, 1, 1);
    canv_proj_UH->DivideSquare(kNstrips);
    name = fTitle + "_canv_proj_DH";
    canv_proj_DH = new TCanvas(name, name, 1, 1);
    canv_proj_DH->DivideSquare(kNstrips);
    name = fTitle + "_canv_proj_UL";
    canv_proj_UL = new TCanvas(name, name, 1, 1);
    canv_proj_UL->DivideSquare(kNstrips);
    name = fTitle + "_canv_proj_DL";
    canv_proj_DL = new TCanvas(name, name, 1, 1);
    canv_proj_DL->DivideSquare(kNstrips);
    for (uint32_t i = 1; i <= kNstrips; i++)
        for (uint32_t j = 0; j < kNsides; j++)
            for (uint32_t k = 0; k < kNgains; k++) {
                TString side, gain, nameH;
                uint32_t address = BmnHodoAddress::GetAddress(i, j, k);
                (j == 0) ? side = "DOWN" : side = "UP";
                (k == 0) ? gain = "LOW" : gain = "HIGH";
                nameH = fTitle + Form("_strip_%d_side_%s_gain_%s", i, side.Data(), gain.Data());
                vecProj[address] = new TH1F(nameH, nameH, 250, 0, 5000);
                PadInfo* p = new PadInfo();
                p->current = vecProj[address];
                p->opt = "hist logy";
                canv_proj_Pads[address] = p;
                Namescanv_proj[address] = p->current->GetName();
            }

    name = fTitle + "_hStripSideGain";
    hStripSideGain = new TH1F(name, name, 300, 0, 5000);
    hStripSideGain->SetStats(0);
    hStripSideGain->GetYaxis()->SetTitle("Counts");
    hStripSideGain->GetXaxis()->SetTitle("Apml");

    name = fTitle + "_wfmStrip";
    wfmStrip = new TH1F(name, name, 200, 0, 200);
    wfmStrip->SetMarkerStyle(31);
    wfmStrip->SetMarkerSize(0.5);
    wfmStrip->SetStats(0);

    name = fTitle + "CanvasSelection";
    canSelect = new TCanvas(name, name, PAD_WIDTH * 1, PAD_HEIGHT * 2);
    canSelect->Divide(1, 2);
    canSelectPads.resize(1 * 2);
    NamesSelect.resize(1 * 2);
    PadInfo* ps1 = new PadInfo();
    ps1->current = hStripSideGain;
    ps1->opt = "hist";
    canSelectPads.at(0) = ps1;
    NamesSelect.at(0) = ps1->current->GetName();
    PadInfo* ps2 = new PadInfo();
    ps2->current = wfmStrip;
    ps2->opt = "hist lp";
    canSelectPads.at(1) = ps2;
    NamesSelect.at(1) = ps2->current->GetName();

    name = fTitle + "_hZ2_hodo";
    hZ2_hodo = new TH1F(name, "hZ2_hodo", 250, 0, 5000);
    hZ2_hodo->GetXaxis()->SetTitle("Z^2");
    hZ2_hodo->GetXaxis()->SetTitleColor(kOrange + 10);
    hZ2_hodo->GetYaxis()->SetTitle("Counts / event");
    hZ2_hodo->GetYaxis()->SetTitleColor(kOrange + 10);
    hZ2_hodo->SetOption("hist");
    hZ2_hodo->SetStats(0);

    name = fTitle + "CanvasZ2";
    canZ2 = new TCanvas(name, name, 1, 1);
    canZ2->Divide(1, 1);
    canZ2Pads.resize(1);
    NamesZ2.resize(1);
    PadInfo* pz = new PadInfo();
    pz->current = hZ2_hodo;
    canZ2Pads.at(0) = pz;
    NamesZ2.at(0) = pz->current->GetName();
}

void BmnHistHodo::Register(THttpServer* serv)
{
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canAmps);
    fServer->Register(path, canProf);
    fServer->Register(path, canZ2);
    fServer->Register(path, canv_proj_DH);
    fServer->Register(path, canv_proj_DL);
    fServer->Register(path, canv_proj_UH);
    fServer->Register(path, canv_proj_UL);
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
    fServer->RegisterCommand(cmdTitle, TString("/") + fName.Data() + "/->SetSelection(%arg1%,%arg2%,%arg3%,%arg4%)",
                             "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
}

void BmnHistHodo::SetDir(TFile* outFile = NULL, TTree* recoTree = NULL)
{
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    h2d_EstripUH->SetDirectory(fDir);
    h2d_EstripDH->SetDirectory(fDir);
    h2d_EstripUL->SetDirectory(fDir);
    h2d_EstripDL->SetDirectory(fDir);
    h1d_profileUH->SetDirectory(fDir);
    h1d_profileDH->SetDirectory(fDir);
    h1d_profileUL->SetDirectory(fDir);
    h1d_profileDL->SetDirectory(fDir);
    hZ2_hodo->SetDirectory(fDir);
    for (uint32_t i = 1; i <= kNstrips; i++)
        for (uint32_t j = 0; j < kNsides; j++)
            for (uint32_t k = 0; k < kNgains; k++) {
                int flatIdx = BmnHodoAddress::GetAddress(i, j, k);
                vecProj.at(flatIdx)->SetDirectory(fDir);
            }
    hStripSideGain->SetDirectory(fDir);
    wfmStrip->SetDirectory(fDir);
}

void BmnHistHodo::SetSelection(Int_t Strip, Int_t Side, Int_t Gain, Int_t Trigger)
{
    fSelectedStrip = Strip;
    fSelectedSide = Side;
    fSelectedGain = Gain;
    fSelectedTrigger = Trigger;
    TString command = "(1)";
    TString digiInTree = "HodoDigi";
    if (fSelectedStrip >= 0)
        command = command + Form(" && %s.GetStripId() == %d", digiInTree.Data(), fSelectedStrip);
    if (fSelectedSide >= 0)
        command = command + Form(" && %s.GetStripSide() == %d", digiInTree.Data(), fSelectedSide);
    if (fSelectedGain >= 0)
        command = command + Form(" && %s.GetGain() == %d", digiInTree.Data(), fSelectedGain);
    TString triggerCondition = "";
    if (fSelectedTrigger >= 0)
        triggerCondition = Form(" && %s trigger bit == %d (bitBT=2, bitBC1=3, bitMBT=6, bitNiT=4, bitCCT1=5, "
                                "bitCCT2=8, bitCCT3=12, bitFHCAL=9)",
                                digiInTree.Data(), fSelectedTrigger);
    TString titleSignal = Form("#splitline{Signal %s}{%s}", command.Data(), triggerCondition.Data());
    hStripSideGain->SetTitle(titleSignal);
    TString titleWfm = Form("#splitline{Wfm %s}{%s}", command.Data(), triggerCondition.Data());
    wfmStrip->SetTitle(titleWfm);
    hStripSideGain->Reset();
    wfmStrip->Reset();
    if (frecoTree != NULL) {
        TString direction = Form("%s.fAmpl>>%s", digiInTree.Data(), hStripSideGain->GetName());
        frecoTree->Draw(direction.Data(), command.Data(), "goff");
        wfmStrip->SetTitle(titleWfm);
    }
}

void BmnHistHodo::DrawBoth()
{
    BmnHist::DrawRef(canAmps, &canAmpsPads);
    BmnHist::DrawRef(canProf, &canProfPads);
    BmnHist::DrawRef(canZ2, &canZ2Pads);
    std::vector<PadInfo*> hsubPadsUH;
    std::vector<PadInfo*> hsubPadsDH;
    std::vector<PadInfo*> hsubPadsUL;
    std::vector<PadInfo*> hsubPadsDL;
    hsubPadsUH.resize(kNstrips);
    hsubPadsDH.resize(kNstrips);
    hsubPadsUL.resize(kNstrips);
    hsubPadsDL.resize(kNstrips);
    for (uint32_t i = 1; i <= kNstrips; i++)
        for (uint32_t j = 0; j < kNsides; j++)
            for (uint32_t k = 0; k < kNgains; k++) {
                int flatIdx = BmnHodoAddress::GetAddress(i, j, k);
                if (j == 0 && k == 0)
                    hsubPadsDL.at(i - 1) = canv_proj_Pads.at(flatIdx);
                if (j == 0 && k == 1)
                    hsubPadsDH.at(i - 1) = canv_proj_Pads.at(flatIdx);
                if (j == 1 && k == 0)
                    hsubPadsUL.at(i - 1) = canv_proj_Pads.at(flatIdx);
                if (j == 1 && k == 1)
                    hsubPadsUH.at(i - 1) = canv_proj_Pads.at(flatIdx);
            }
    BmnHist::DrawRef(canv_proj_UH, &hsubPadsUH);
    BmnHist::DrawRef(canv_proj_DH, &hsubPadsDH);
    BmnHist::DrawRef(canv_proj_UL, &hsubPadsUL);
    BmnHist::DrawRef(canv_proj_DL, &hsubPadsDL);
    BmnHist::DrawRef(canSelect, &canSelectPads);
    return;
}

void BmnHistHodo::FillFromDigi(DigiArrays* fDigiArrays)
{
    BmnEventHeader* head = fDigiArrays->header;
    TClonesArray* digits = fDigiArrays->hodo;
    if (!digits)
        return;

    float responses[kNstrips + 1][kNsides];
    memset(responses, 0, sizeof(responses[0][0]) * (kNstrips + 1) * kNsides);
    hZ2_hodo->Scale(fEventCounter);
    h1d_profileDL->Scale(fEventCounter);
    h1d_profileDH->Scale(fEventCounter);
    h1d_profileUL->Scale(fEventCounter);
    h1d_profileUH->Scale(fEventCounter);

    // Loop over digis
    for (Int_t iDig = 0; iDig < digits->GetEntriesFast(); iDig++) {
        BmnHodoDigi* digi = (BmnHodoDigi*)digits->At(iDig);
        double signal = digi->GetSignal();
        auto strip = digi->GetStripId();
        auto side = digi->GetStripSide();
        auto gain = digi->GetGain();
        int flatIdx = BmnHodoAddress::GetAddress(strip, side, gain);

        vecProj.at(flatIdx)->Fill(signal);
        if (side == 0 && gain == 0) {
            h2d_EstripDL->Fill(strip, signal);
            h1d_profileDL->Fill(strip, signal);
        }
        if (side == 0 && gain == 1) {
            h2d_EstripDH->Fill(strip, signal);
            h1d_profileDH->Fill(strip, signal);
        }
        if (side == 1 && gain == 0) {
            h2d_EstripUL->Fill(strip, signal);
            h1d_profileUL->Fill(strip, signal);
        }
        if (side == 1 && gain == 1) {
            h2d_EstripUH->Fill(strip, signal);
            h1d_profileUH->Fill(strip, signal);
        }

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
            (Int_t)strip == fSelectedStrip && (Int_t)side == fSelectedSide && (Int_t)gain == fSelectedGain;
        if (isSelected) {
            if ((TrigPattern >> fSelectedTrigger) & 1) {
                hStripSideGain->Fill(digi->fAmpl);
                auto Wfm = digi->GetWfm();
                std::vector<float> points(Wfm.size());
                std::iota(std::begin(points), std::end(points), 0);   // Fill with 0, 1, ..., wfm.back().
                wfmStrip->Reset();
                wfmStrip->GetXaxis()->SetRangeUser(0, Wfm.size());
                for (UInt_t wfm_iter = 0; wfm_iter < Wfm.size(); wfm_iter++)
                    wfmStrip->Fill(wfm_iter, Wfm.at(wfm_iter));
            }
        }
        if (gain != 1)
            continue;
        responses[strip][side] = signal;
    }
    fEventCounter++;
    for (int i = 1; i <= kNstrips; i++)
        if (responses[i][0] * responses[i][1] > 0.001)
            hZ2_hodo->Fill((responses[i][0] + responses[i][1]) / 2);
    hZ2_hodo->Scale(1. / fEventCounter);
    h1d_profileDL->Scale(1. / fEventCounter);
    h1d_profileDH->Scale(1. / fEventCounter);
    h1d_profileUL->Scale(1. / fEventCounter);
    h1d_profileUH->Scale(1. / fEventCounter);
}

BmnStatus BmnHistHodo::SetRefRun(Int_t id)
{
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        //  BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canAmpsPads, NamesAmps);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canProfPads, NamesProf);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canZ2Pads, NamesZ2);

        std::vector<PadInfo*> padVec;
        std::vector<TString> nameVec;

        padVec.resize(canv_proj_Pads.size());
        nameVec.resize(Namescanv_proj.size());

        std::transform(canv_proj_Pads.begin(), canv_proj_Pads.end(), padVec.begin(),
                       [](const auto& kv) { return kv.second; });

        std::transform(Namescanv_proj.begin(), Namescanv_proj.end(), nameVec.begin(),
                       [](const auto& kv) { return kv.second; });

        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, padVec, nameVec);
        DrawBoth();
    }
    return kBMNSUCCESS;
}

void BmnHistHodo::ClearRefRun()
{
    for (auto pad : canAmpsPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canProfPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canZ2Pads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    for (uint32_t i = 1; i <= kNstrips; i++)
        for (uint32_t j = 0; j < kNsides; j++)
            for (uint32_t k = 0; k < kNgains; k++) {
                int flatIdx = BmnHodoAddress::GetAddress(i, j, k);
                auto pad = canv_proj_Pads.at(flatIdx);
                if (pad->ref)
                    delete pad->ref;
                pad->ref = NULL;
            }
    refID = 0;
}

void BmnHistHodo::Reset()
{
    printf("BmnHistHodo : Reset histos\n");
    fEventCounter = 1;
    h2d_EstripUH->Reset();
    h2d_EstripDH->Reset();
    h2d_EstripUL->Reset();
    h2d_EstripDL->Reset();
    h1d_profileUH->Reset();
    h1d_profileDH->Reset();
    h1d_profileUL->Reset();
    h1d_profileDL->Reset();
    hStripSideGain->Reset();
    hZ2_hodo->Reset();

    for (uint32_t i = 1; i <= kNstrips; i++)
        for (uint32_t j = 0; j < kNsides; j++)
            for (uint32_t k = 0; k < kNgains; k++) {
                int flatIdx = BmnHodoAddress::GetAddress(i, j, k);
                vecProj.at(flatIdx)->Reset();
            }
}

BmnHistHodo::~BmnHistHodo()
{
    delete canAmps;
    delete canProf;
    delete canZ2;
    delete canSelect;
    if (fDir != NULL)
        return;
    delete h2d_EstripUH;
    delete h2d_EstripDH;
    delete h2d_EstripUL;
    delete h2d_EstripDL;
    delete h1d_profileUH;
    delete h1d_profileDH;
    delete h1d_profileUL;
    delete h1d_profileDL;
    delete hStripSideGain;
    delete hZ2_hodo;
    for (uint32_t i = 1; i <= kNstrips; i++)
        for (uint32_t j = 0; j < kNsides; j++)
            for (uint32_t k = 0; k < kNgains; k++) {
                int flatIdx = BmnHodoAddress::GetAddress(i, j, k);
                delete vecProj.at(flatIdx);
            }
}
