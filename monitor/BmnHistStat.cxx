#include "BmnHistStat.h"

#include "BmnRawDataDecoder.h"

BmnHistStat::BmnHistStat(TString title, TString path)
    : BmnHist()
{
    fTitle = title;
    fName = title + "_cl";
    refPath = path;

    initHistDet();
    initHistFD();
    initHistBD();
    initHistCorr();
}

BmnHistStat::~BmnHistStat()
{
    delete canStatDet;
    delete canStatFD;
    delete canStatBD;
    delete canStatCorr;

    if (fDir)
        return;

    delete histGemStat;
    delete histSiliconStat;
    delete histTof400Stat;
    delete histTof700Stat;
    delete histCscStat;
    delete histFHCalStat;

    delete histFDGemStat;
    delete histFDSiliconStat;
    delete histFDTof400Stat;
    delete histFDTof700Stat;
    delete histFDCscStat;
    delete histFDFHCalStat;

    delete histBDGemStat;
    delete histBDSiliconStat;
    delete histBDTof400Stat;
    delete histBDTof700Stat;
    delete histBDCscStat;
    delete histBDFHCalStat;

    delete histGemSilicon;
    delete histGemTof400;
    delete histGemTof700;
    delete histGemCsc;
    delete histGemFHCal;
    delete histCscFHCal;

    for (auto padDet : canStatDetPads)
        delete padDet;

    for (auto padFD : canStatFDPads)
        delete padFD;

    for (auto padBD : canStatBDPads)
        delete padBD;

    for (auto padCorr : canStatCorrPads)
        delete padCorr;
}

void BmnHistStat::Register(THttpServer* serv)
{
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canStatDet);
    fServer->Register(path, canStatFD);
    fServer->Register(path, canStatBD);
    fServer->Register(path, canStatCorr);
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

void BmnHistStat::SetDir(TFile* outFile, TTree* recoTree)
{
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    histGemStat->SetDirectory(fDir);
    histSiliconStat->SetDirectory(fDir);
    histTof400Stat->SetDirectory(fDir);
    histTof700Stat->SetDirectory(fDir);
    histCscStat->SetDirectory(fDir);
    histFHCalStat->SetDirectory(fDir);

    histFDGemStat->SetDirectory(fDir);
    histFDSiliconStat->SetDirectory(fDir);
    histFDTof400Stat->SetDirectory(fDir);
    histFDTof700Stat->SetDirectory(fDir);
    histFDCscStat->SetDirectory(fDir);
    histFDFHCalStat->SetDirectory(fDir);

    histBDGemStat->SetDirectory(fDir);
    histBDSiliconStat->SetDirectory(fDir);
    histBDTof400Stat->SetDirectory(fDir);
    histBDTof700Stat->SetDirectory(fDir);
    histBDCscStat->SetDirectory(fDir);
    histBDFHCalStat->SetDirectory(fDir);

    histGemSilicon->SetDirectory(fDir);
    histGemTof400->SetDirectory(fDir);
    histGemTof700->SetDirectory(fDir);
    histGemCsc->SetDirectory(fDir);
    histGemFHCal->SetDirectory(fDir);
    histCscFHCal->SetDirectory(fDir);
}

void BmnHistStat::DrawBoth()
{
    BmnHist::DrawRef(canStatDet, &canStatDetPads);
    BmnHist::DrawRef(canStatFD, &canStatFDPads);
    BmnHist::DrawRef(canStatBD, &canStatBDPads);
    BmnHist::DrawRef(canStatCorr, &canStatCorrPads);
    return;
}

void BmnHistStat::FillFromDigi(DigiArrays* fDigiArrays)
{
    const uint32_t timewin_begin = 312;
    const uint32_t timewin_end = 362;
    BmnEventHeader* header = fDigiArrays->header;
    if (header->GetEventType() != 1)
        return;
    TClonesArray* gemDigits = fDigiArrays->gem;
    TClonesArray* siliconDigits = fDigiArrays->silicon;
    TClonesArray* tof400Digits = fDigiArrays->tof400;
    TClonesArray* tof700Digits = fDigiArrays->tof701;
    if (tof700Digits == nullptr)
        tof700Digits = fDigiArrays->tof700;
    TClonesArray* cscDigits = fDigiArrays->csc;
    TClonesArray* fhcalDigits = fDigiArrays->fhcal;

    vector<TClonesArray*>* trigDigits = fDigiArrays->trigAr;

    if (gemDigits)
        histGemStat->Fill(gemDigits->GetEntriesFast());
    if (siliconDigits)
        histSiliconStat->Fill(siliconDigits->GetEntriesFast());
    if (tof400Digits)
        histTof400Stat->Fill(tof400Digits->GetEntriesFast());
    if (tof700Digits)
        histTof700Stat->Fill(tof700Digits->GetEntriesFast());
    if (cscDigits)
        histCscStat->Fill(cscDigits->GetEntriesFast());
    if (fhcalDigits)
        histFHCalStat->Fill(fhcalDigits->GetEntriesFast());
    if (trigDigits) {
        Int_t fdPosition = getPositionInArray(trigDigits, "TQDC_FD");
        Int_t bdPosition = getPositionInArray(trigDigits, "BD");
        TClonesArray* TQDC_FD = trigDigits->at(fdPosition);   // 6 element -FD
        TClonesArray* TQDC_BD = trigDigits->at(bdPosition);   // 43 element -BD
        if (TQDC_FD->GetEntriesFast() != 0) {
            BmnTrigWaveDigit* fdDigit = dynamic_cast<BmnTrigWaveDigit*>(TQDC_FD->At(0));
            Double_t peakFD = fdDigit->GetPeak();
            if (gemDigits) {
                histFDGemStat->Fill(peakFD, gemDigits->GetEntriesFast());
            }
            if (siliconDigits) {
                histFDSiliconStat->Fill(peakFD, siliconDigits->GetEntriesFast());
            }
            if (tof400Digits) {
                histFDTof400Stat->Fill(peakFD, tof400Digits->GetEntriesFast());
            }
            if (tof700Digits) {
                histFDTof700Stat->Fill(peakFD, tof700Digits->GetEntriesFast());
            }
            if (cscDigits) {
                histFDCscStat->Fill(peakFD, cscDigits->GetEntriesFast());
            }
            if (fhcalDigits) {
                histFDFHCalStat->Fill(peakFD, fhcalDigits->GetEntriesFast());
            }
        }
        if (TQDC_BD) {
            Double_t numberMod = TQDC_BD->GetEntriesFast();
            if (gemDigits) {
                histBDGemStat->Fill(numberMod, gemDigits->GetEntriesFast());
            }
            if (siliconDigits) {
                histBDSiliconStat->Fill(numberMod, siliconDigits->GetEntriesFast());
            }
            if (tof400Digits) {
                histBDTof400Stat->Fill(numberMod, tof400Digits->GetEntriesFast());
            }
            if (tof700Digits) {
                histBDTof700Stat->Fill(numberMod, tof700Digits->GetEntriesFast());
            }
            if (cscDigits) {
                histBDCscStat->Fill(numberMod, cscDigits->GetEntriesFast());
            }
            if (fhcalDigits) {
                histBDFHCalStat->Fill(numberMod, fhcalDigits->GetEntriesFast());
            }
        }
    }

    if (gemDigits && siliconDigits)
        histGemSilicon->Fill(gemDigits->GetEntriesFast(), siliconDigits->GetEntriesFast());
    if (gemDigits && tof400Digits)
        histGemTof400->Fill(gemDigits->GetEntriesFast(), tof400Digits->GetEntriesFast());
    if (gemDigits && tof700Digits)
        histGemTof700->Fill(gemDigits->GetEntriesFast(), tof700Digits->GetEntriesFast());
    if (gemDigits && cscDigits)
        histGemCsc->Fill(gemDigits->GetEntriesFast(), cscDigits->GetEntriesFast());
    if (gemDigits && fhcalDigits)
        histGemFHCal->Fill(gemDigits->GetEntriesFast(), fhcalDigits->GetEntriesFast());
    if (cscDigits && fhcalDigits)
        histCscFHCal->Fill(cscDigits->GetEntriesFast(), fhcalDigits->GetEntriesFast());
}

BmnStatus BmnHistStat::SetRefRun(Int_t id)
{
    if (refID != id) {
        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canStatDetPads, NamesStatDet);
        // BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canStatFDPads, NamesStatFD);
        // BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canStatBDPads, NamesStatBD);
        // BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canStatCorrPads, NamesStatCorr);
        DrawBoth();
    }

    return kBMNSUCCESS;
}

void BmnHistStat::ClearRefRun()
{
    for (auto pad : canStatDetPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

void BmnHistStat::Reset()
{
    printf("ResetStat\n");
    histGemStat->Reset();
    histSiliconStat->Reset();
    histTof400Stat->Reset();
    histTof700Stat->Reset();
    histCscStat->Reset();
    histFHCalStat->Reset();

    histFDGemStat->Reset();
    histFDSiliconStat->Reset();
    histFDTof400Stat->Reset();
    histFDTof700Stat->Reset();
    histFDCscStat->Reset();
    histFDFHCalStat->Reset();

    histBDGemStat->Reset();
    histBDSiliconStat->Reset();
    histBDTof400Stat->Reset();
    histBDTof700Stat->Reset();
    histBDCscStat->Reset();
    histBDFHCalStat->Reset();

    histGemSilicon->Reset();
    histGemTof400->Reset();
    histGemTof700->Reset();
    histGemCsc->Reset();
    histGemFHCal->Reset();
    histCscFHCal->Reset();
}

void BmnHistStat::initHistDet()
{
    TString name;
    name = fTitle + "_GEM";
    histGemStat = new TH1I(name, name, 250, 1, Max_GEM_Cnt);
    histGemStat->GetXaxis()->SetTitle("Count in event");
    histGemStat->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_SILICON";
    histSiliconStat = new TH1I(name, name, 250, 1, Max_FSD_Cnt);
    histSiliconStat->GetXaxis()->SetTitle("Count in event");
    histSiliconStat->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_ToF400";
    histTof400Stat = new TH1I(name, name, 100, 1, Max_ToF_Cnt);
    histTof400Stat->GetXaxis()->SetTitle("Count in event");
    histTof400Stat->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_ToF700";
    histTof700Stat = new TH1I(name, name, 100, 1, Max_ToF_Cnt);
    histTof700Stat->GetXaxis()->SetTitle("Count in event");
    histTof700Stat->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_CSC";
    histCscStat = new TH1I(name, name, 200, 1, Max_CSC_Cnt);
    histCscStat->GetXaxis()->SetTitle("Count in event");
    histCscStat->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_FHCAL";
    histFHCalStat = new TH1I(name, name, 250, 1, Max_FHC_Cnt);
    histFHCalStat->GetXaxis()->SetTitle("Count in event");
    histFHCalStat->GetYaxis()->SetTitle("Activations count");

    name = fTitle + "Canvas";
    canStatDet = new TCanvas(name, name, PAD_WIDTH * 2, PAD_HEIGHT * 3);
    canStatDet->Divide(3, 2);
    canStatDetPads.resize(3 * 2);
    NamesStatDet.resize(3 * 2);
    for (Int_t iPad = 0; iPad < 6; iPad++) {
        PadInfo* p = new PadInfo();
        canStatDetPads[iPad] = p;
        canStatDet->GetPad(iPad + 1)->SetGrid();
    }
    canStatDetPads[0]->current = histGemStat;
    canStatDetPads[1]->current = histSiliconStat;
    canStatDetPads[2]->current = histTof400Stat;
    canStatDetPads[3]->current = histTof700Stat;
    canStatDetPads[4]->current = histCscStat;
    canStatDetPads[5]->current = histFHCalStat;
    for (size_t iPad = 0; iPad < canStatDetPads.size(); iPad++)
        if (canStatDetPads[iPad]->current) {
            NamesStatDet.push_back(canStatDetPads[iPad]->current->GetName());
            BmnHist::SetHistStyleTH1(canStatDetPads[iPad]->current);
        }
}

void BmnHistStat::initHistFD()
{
    TString name;
    name = fTitle + "_FD_GEM";
    histFDGemStat = new TH2F(name, name, 300, 0, Max_FD_Sig, 250, 1, Max_GEM_Cnt);
    histFDGemStat->GetXaxis()->SetTitle("FD.GetPeak()");
    histFDGemStat->GetYaxis()->SetTitle("Number of digits in event");

    name = fTitle + "_FD_Silicon";
    histFDSiliconStat = new TH2F(name, name, 300, 0, Max_FD_Sig, 250, 1, Max_FSD_Cnt);
    histFDSiliconStat->GetXaxis()->SetTitle("FD.GetPeak()");
    histFDSiliconStat->GetYaxis()->SetTitle("Number of digits in event");

    name = fTitle + "_FD_Tof400";
    histFDTof400Stat = new TH2F(name, name, 300, 0, Max_FD_Sig, 100, 1, Max_ToF_Cnt);
    histFDTof400Stat->GetXaxis()->SetTitle("FD.GetPeak()");
    histFDTof400Stat->GetYaxis()->SetTitle("Number of digits in event");

    name = fTitle + "_FD_Tof700";
    histFDTof700Stat = new TH2F(name, name, 300, 0, Max_FD_Sig, 100, 1, Max_ToF_Cnt);
    histFDTof700Stat->GetXaxis()->SetTitle("FD.GetPeak()");
    histFDTof700Stat->GetYaxis()->SetTitle("Number of digits in event");

    name = fTitle + "_FD_CSC";
    histFDCscStat = new TH2F(name, name, 300, 0, Max_FD_Sig, 150, 1, Max_CSC_Cnt);
    histFDCscStat->GetXaxis()->SetTitle("FD.GetPeak()");
    histFDCscStat->GetYaxis()->SetTitle("Number of digits in event");

    name = fTitle + "_FD_FHCAL";
    histFDFHCalStat = new TH2F(name, name, 300, 0, Max_FD_Sig, 250, 1, Max_FHC_Cnt);
    histFDFHCalStat->GetXaxis()->SetTitle("FD.GetPeak()");
    histFDFHCalStat->GetYaxis()->SetTitle("Number of digits in event");

    name = fTitle + "FD_DetCanvas";
    canStatFD = new TCanvas(name, name, PAD_WIDTH * 2, PAD_HEIGHT * 3);
    canStatFD->Divide(3, 2);
    canStatFDPads.resize(3 * 2);
    NamesStatFD.resize(3 * 2);
    for (Int_t iPad = 0; iPad < 6; iPad++) {
        PadInfo* p = new PadInfo();
        p->opt = "colz";
        canStatFDPads[iPad] = p;
        canStatFD->GetPad(iPad + 1)->SetGrid();
    }
    canStatFDPads[0]->current = histFDGemStat;
    canStatFDPads[1]->current = histFDSiliconStat;
    canStatFDPads[2]->current = histFDTof400Stat;
    canStatFDPads[3]->current = histFDTof700Stat;
    canStatFDPads[4]->current = histFDCscStat;
    canStatFDPads[5]->current = histFDFHCalStat;
    for (size_t iPad = 0; iPad < canStatFDPads.size(); iPad++)
        if (canStatFDPads[iPad]->current) {
            NamesStatFD.push_back(canStatFDPads[iPad]->current->GetName());
            // BmnHist::SetHistStyleTH2(canStatFDPads[iPad]->current);
        }
}

void BmnHistStat::initHistBD()
{
    TString name;
    name = fTitle + "_BD_GEM";
    histBDGemStat = new TH2F(name, name, Max_BD_Cnt - 1, 1, Max_BD_Cnt, 250, 1, Max_GEM_Cnt);
    histBDGemStat->GetXaxis()->SetTitle("Number of digits in BD");
    histBDGemStat->GetYaxis()->SetTitle("Number of digits in event GEM");

    name = fTitle + "_BD_Silicon";
    histBDSiliconStat = new TH2F(name, name, Max_BD_Cnt - 1, 1, Max_BD_Cnt, 250, 1, Max_FSD_Cnt);
    histBDSiliconStat->GetXaxis()->SetTitle("Number of digits in BD");
    histBDSiliconStat->GetYaxis()->SetTitle("Number of digits in event Silicon");

    name = fTitle + "_BD_Tof400";
    histBDTof400Stat = new TH2F(name, name, Max_BD_Cnt - 1, 1, Max_BD_Cnt, 100, 1, Max_ToF_Cnt);
    histBDTof400Stat->GetXaxis()->SetTitle("Number of digits in BD");
    histBDTof400Stat->GetYaxis()->SetTitle("Number of digits in event Tof400");

    name = fTitle + "_BD_Tof700";
    histBDTof700Stat = new TH2F(name, name, Max_BD_Cnt - 1, 1, Max_BD_Cnt, 100, 1, Max_ToF_Cnt);
    histBDTof700Stat->GetXaxis()->SetTitle("Number of digits in BD");
    histBDTof700Stat->GetYaxis()->SetTitle("Number of digits in event Tof700");

    name = fTitle + "_BD_CSC";
    histBDCscStat = new TH2F(name, name, Max_BD_Cnt - 1, 1, Max_BD_Cnt, 150, 1, Max_CSC_Cnt);
    histBDCscStat->GetXaxis()->SetTitle("Number of digits in BD");
    histBDCscStat->GetYaxis()->SetTitle("Number of digits in event CSC");

    name = fTitle + "_BD_FHCAL";
    histBDFHCalStat = new TH2F(name, name, Max_BD_Cnt - 1, 1, Max_BD_Cnt, 250, 1, Max_FHC_Cnt);
    histBDFHCalStat->GetXaxis()->SetTitle("Number of digits in BD");
    histBDFHCalStat->GetYaxis()->SetTitle("Number of digits in event FHCAL");

    name = fTitle + "BD_DetCanvas";
    canStatBD = new TCanvas(name, name, PAD_WIDTH * 2, PAD_HEIGHT * 3);
    canStatBD->Divide(3, 2);
    canStatBDPads.resize(3 * 2);
    NamesStatBD.resize(3 * 2);
    for (Int_t iPad = 0; iPad < 6; iPad++) {
        PadInfo* p = new PadInfo();
        p->opt = "colz";
        canStatBDPads[iPad] = p;
        canStatBD->GetPad(iPad + 1)->SetGrid();
    }
    canStatBDPads[0]->current = histBDGemStat;
    canStatBDPads[1]->current = histBDSiliconStat;
    canStatBDPads[2]->current = histBDTof400Stat;
    canStatBDPads[3]->current = histBDTof700Stat;
    canStatBDPads[4]->current = histBDCscStat;
    canStatBDPads[5]->current = histBDFHCalStat;
    for (size_t iPad = 0; iPad < canStatBDPads.size(); iPad++)
        if (canStatBDPads[iPad]->current) {
            NamesStatBD.push_back(canStatBDPads[iPad]->current->GetName());
        }
}

void BmnHistStat::initHistCorr()
{
    TString name;
    name = fTitle + "_GEM_Silicon";
    histGemSilicon = new TH2F(name, name, 250, 1, Max_GEM_Cnt, 250, 1, Max_FSD_Cnt);
    histGemSilicon->GetXaxis()->SetTitle("Number of digits in event GEM");
    histGemSilicon->GetYaxis()->SetTitle("Number of digits in event Silicon");

    name = fTitle + "_GEM_Tof400";
    histGemTof400 = new TH2F(name, name, 250, 1, Max_GEM_Cnt, 100, 1, Max_ToF_Cnt);
    histGemTof400->GetXaxis()->SetTitle("Number of digits in event GEM");
    histGemTof400->GetYaxis()->SetTitle("Number of digits in event Tof400");

    name = fTitle + "_GEM_Tof700";
    histGemTof700 = new TH2F(name, name, 250, 1, Max_GEM_Cnt, 100, 1, Max_ToF_Cnt);
    histGemTof700->GetXaxis()->SetTitle("Number of digits in event GEM");
    histGemTof700->GetYaxis()->SetTitle("Number of digits in event Tof700");

    name = fTitle + "_GEM_CSC";
    histGemCsc = new TH2F(name, name, 250, 1, Max_GEM_Cnt, 150, 1, Max_CSC_Cnt);
    histGemCsc->GetXaxis()->SetTitle("Number of digits in event GEM");
    histGemCsc->GetYaxis()->SetTitle("Number of digits in event CSC");

    name = fTitle + "_GEM_FHCAL";
    histGemFHCal = new TH2F(name, name, 250, 1, Max_GEM_Cnt, 250, 1, Max_FHC_Cnt);
    histGemFHCal->GetXaxis()->SetTitle("Number of digits in event GEM");
    histGemFHCal->GetYaxis()->SetTitle("Number of digits in event FHCAL");

    name = fTitle + "_CSC_FHCAL";
    histCscFHCal = new TH2F(name, name, 150, 1, Max_CSC_Cnt, 250, 1, Max_FHC_Cnt);
    histCscFHCal->GetXaxis()->SetTitle("Number of digits in event CSC");
    histCscFHCal->GetYaxis()->SetTitle("Number of digits in event FHCAL");

    name = fTitle + "CorrCanvas";
    canStatCorr = new TCanvas(name, name, PAD_WIDTH * 2, PAD_HEIGHT * 3);
    canStatCorr->Divide(3, 2);
    canStatCorrPads.resize(3 * 2);
    NamesStatCorr.resize(3 * 2);
    for (Int_t iPad = 0; iPad < 6; iPad++) {
        PadInfo* p = new PadInfo();
        p->opt = "colz";
        canStatCorrPads[iPad] = p;
        canStatCorr->GetPad(iPad + 1)->SetGrid();
    }
    canStatCorrPads[0]->current = histGemSilicon;
    canStatCorrPads[1]->current = histGemTof400;
    canStatCorrPads[2]->current = histGemTof700;
    canStatCorrPads[3]->current = histGemCsc;
    canStatCorrPads[4]->current = histGemFHCal;
    canStatCorrPads[5]->current = histCscFHCal;
    for (size_t iPad = 0; iPad < canStatCorrPads.size(); iPad++)
        if (canStatCorrPads[iPad]->current) {
            NamesStatCorr.push_back(canStatCorrPads[iPad]->current->GetName());
        }
}

Int_t BmnHistStat::getPositionInArray(vector<TClonesArray*>* trigDigits, TString name)
{
    for (size_t iTrig = 0; iTrig < trigDigits->size(); ++iTrig) {
        TClonesArray* array = trigDigits->at(iTrig);
        TString nameArray = array->GetName();
        if (nameArray.EqualTo(name))
            return iTrig;
    }

    return -1;
}
