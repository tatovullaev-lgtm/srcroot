#include "BmnHist.h"

BmnHist::BmnHist(Int_t PeriodID, BmnSetup setup)
    : isShown(kFALSE)
    , fServer(nullptr)
    , frecoTree(nullptr)
    , fDir(nullptr)
    , refFile(nullptr)
    , fPeriodID(PeriodID)
    , fSetup(setup)
{}

BmnHist::~BmnHist() {}

void BmnHist::DrawRef(unique_ptr<TCanvas>& canGemStrip, vector<PadInfo*>* canGemStripPads)
{
    DrawRef(canGemStrip.get(), canGemStripPads);
}

void BmnHist::DrawRef(TCanvas* can, vector<PadInfo*>* canPads)
{
    for (size_t iPad = 0; iPad < canPads->size(); iPad++) {
        TPad* pad = static_cast<TPad*>(can->cd(iPad + 1));
        PadInfo* info = canPads->at(iPad);
        if ((!pad) || (!info))
            continue;
        if (info->logx)
            pad->SetLogx();
        if (info->logy)
            pad->SetLogy();
        if (info->logz)
            pad->SetLogz();
        DrawPad(pad, info);
    }
    can->Update();
    can->Modified();
}

void BmnHist::DrawPad(TVirtualPad* pad, PadInfo* info)
{
    pad->Clear();
    Double_t maxy(0);
    // TClass * cl_base(nullptr);
    if (info->current) {
        //        maxy = info->current->GetMaximum();

        maxy = info->current->GetBinContent(info->current->GetMaximumBin());
        if (info->ref) {
            if (info->normalize && (info->ref->Integral() > 0))
                maxy = TMath::Max(maxy, info->ref->GetMaximum() * info->current->Integral() / info->ref->Integral());
            else
                maxy = TMath::Max(maxy, info->ref->GetMaximum());
        }
    }
    if (info->aux.size()) {
        for (size_t it = 0; it < info->aux.size(); it++) {
            auto h_aux = info->aux.at(it);
            maxy = TMath::Max(maxy, h_aux->GetBinContent(h_aux->GetMaximumBin()));
        }
        if (info->aux[0]->Integral())
            info->aux[0]->SetMaximum(1.2 * maxy);
    }
    if (info->current) {
        if (info->current->Integral())
            info->current->SetMaximum(1.2 * maxy);
        info->current->Draw(info->opt.c_str());
        if (info->ref) {
            if (info->normalize && (info->ref->Integral() > 0))
                info->ref->DrawNormalized((info->opt + "same").c_str(), info->current->Integral());
            else
                info->ref->Draw((info->opt + "same").c_str());
        }
    }
    for (size_t it = 0; it < info->aux.size(); it++) {
        auto h = info->aux.at(it);
        if (h) {
            TString opt = (it || info->current) ? "same " : "";
            if (it < info->auxOpt.size())
                opt += info->auxOpt.at(it);
            h->Draw(opt);
        }
    }
    if (info->legend)
        info->legend->Draw();
    pad->Modified();
}

void BmnHist::FillPad(PadInfo* info, TTree* tree)
{
    // printf("%s  dir %p\n", info->temp->GetName(), (void*)info->temp->GetDirectory());
    info->temp->GetDirectory()->cd();
    /*Long64_t dr = */ tree->Draw((info->variable + ">>" + info->temp->GetName()).c_str(), info->selection.c_str(),
                                  (info->opt + " goff").c_str());
    //    printf("draw %lld integral %f\n", dr, *info->temp->GetIntegral());
    //    printf(" max %f\n", info->temp->GetMaximum());
    info->current->Add(info->temp);
    info->temp->Reset();
}

BmnStatus BmnHist::LoadRefRun(Int_t refID,
                              TString FullName,
                              TString fTitle,
                              vector<PadInfo*> canPads,
                              vector<TString> Names)
{
    return LoadRefRun(refID, FullName, fTitle, canPads);
}

BmnStatus BmnHist::LoadRefRun(Int_t refID, TString FullName, TString fTitle, vector<PadInfo*> canPads)
{
    printf("Loading ref histos\n");
    TFile* refFile = new TFile(FullName, "read");
    if (refFile->IsOpen() == false) {
        printf("Cannot open file %s !\n", FullName.Data());
        return kBMNERROR;
    }
    TString refName = Form("ref%06d_", refID);
    for (size_t iPad = 0; iPad < canPads.size(); iPad++) {
        PadInfo* info = canPads[iPad];
        if (!info)
            continue;
        if (!(info->current))
            continue;
        TString name = info->current->GetName();
        delete info->ref;
        info->ref = NULL;
        TH1* tempH = NULL;
        tempH = static_cast<TH1*>(refFile->Get(refName + fTitle + "_hists/" + refName + name));
        if (tempH == NULL) {
            tempH = static_cast<TH1*>(refFile->Get(fTitle + "_hists/" + name));
        }
        if (tempH == NULL) {
            printf("Cannot load %s !\n", name.Data());
            continue;
        }
        info->ref = static_cast<TH1*>(tempH->Clone(refName + name));
        info->ref->SetLineColor(kRed);
        info->ref->SetDirectory(0);
        printf("Loaded %s \n", info->ref->GetName());
    }
    delete refFile;
    refFile = NULL;
    return kBMNSUCCESS;
}

void BmnHist::ResetPadTree(BmnPadBranch* br)
{
    if (PadInfo* info = br->GetPadInfo()) {
        if (info->current)
            info->current->Reset();
        if (info->ref)
            info->ref->Reset();
        for (auto& h : info->aux)
            if (h)
                h->Reset();
    } else {
        for (auto& b : br->GetBranchesRef()) {
            if (b)
                ResetPadTree(b);
        }
    }
    return;
}

BmnStatus BmnHist::DrawPadFromTree(BmnPadBranch* br)
{
    if (!br)
        return kBMNERROR;
    if (PadInfo* info = br->GetPadInfo()) {
        TVirtualPad* pad = info->padPtr;
        DrawPad(pad, info);
    } else {
        for (auto& b : br->GetBranchesRef()) {
            DrawPadFromTree(b);
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnHist::FillPadFromTree(BmnPadBranch* br, TTree* tree)
{
    if (!br)
        return kBMNERROR;
    if (PadInfo* info = br->GetPadInfo()) {
        // TVirtualPad* pad = info->padPtr;
        FillPad(info, tree);
    } else {
        for (auto& b : br->GetBranchesRef()) {
            FillPadFromTree(b, tree);
        }
    }
    return kBMNSUCCESS;
}

void BmnHist::SetHistStyleTH1(TH1* h)
{
    h->SetTitleSize(0.07, "XY");
    h->SetLabelSize(0.07, "XY");
    h->GetXaxis()->SetTitleColor(kOrange + 10);
    h->GetXaxis()->SetTitleOffset(0.7);
    h->GetXaxis()->SetTitleSize(0.06);
    h->GetYaxis()->SetTitleColor(kOrange + 10);
    h->GetYaxis()->SetTitleOffset(0.6);
}
