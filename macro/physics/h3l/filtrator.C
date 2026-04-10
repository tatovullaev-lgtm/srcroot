#include "TCanvas.h"
#include "TF1.h"
#include "TH1.h"
#include "TLegend.h"
#include "TMath.h"

// signal region
//  Float_t leftSig = 2.989;
//  Float_t rightSig = 2.995;
Float_t leftSig = 2.986;
Float_t rightSig = 2.998;

// full region
const Float_t minBound = 2.95;
const Float_t maxBound = 3.05;

Double_t backgroundChe(Double_t* xx, Double_t* p)
{
    Double_t fA = minBound;
    Double_t fB = maxBound;

    vector<Double_t> fT;
    fT.resize(6);   // polynomial

    Double_t x = (2.0 * xx[0] - fA - fB) / (fB - fA);
    int order = fT.size();
    if (order == 1)
        return p[0];
    if (order == 2)
        return p[0] + x * p[1];
    // build the polynomials
    fT[0] = 1;
    fT[1] = x;
    for (Int_t i = 1; i < order; ++i) {
        fT[i + 1] = 2 * x * fT[i] - fT[i - 1];
    }
    Double_t sum = p[0] * fT[0];
    for (int i = 1; i <= order; ++i) {
        sum += p[i] * fT[i];
    }
    return sum;
}

Double_t background5(Double_t* x, Double_t* par)
{
    return par[0] + par[1] * x[0] + par[2] * x[0] * x[0] + par[3] * x[0] * x[0] * x[0]
           + par[4] * x[0] * x[0] * x[0] * x[0] + par[5] * x[0] * x[0] * x[0] * x[0] * x[0];
}

Double_t background3(Double_t* x, Double_t* par)
{
    return par[0] + par[1] * x[0] + par[2] * x[0] * x[0] + par[3] * x[0] * x[0] * x[0];
}

// Gausian Peak function
Double_t gausPeak(Double_t* x, Double_t* par)
{
    Double_t arg = (x[0] - par[1]) / par[2];
    Double_t fitval = par[0] * TMath::Exp(-0.5 * arg * arg);
    return fitval;
}

// Sum of background and peak function
Double_t fitFunction(Double_t* x, Double_t* par)
{
    // return background5(x, par) + gausPeak(x, &par[6]);
    // return background3(x, par) + gausPeak(x, &par[4]);
    return backgroundChe(x, &par[0]) + gausPeak(x, &par[7]);
}

void DrawNicePlot(TH1F* h, Int_t s, Float_t b)
{
    gStyle->SetOptStat(kFALSE);
    auto c = new TCanvas(h->GetName(), h->GetName(), 800, 500);
    h->SetTitle("");
    h->GetXaxis()->SetTitle("M_{^{3}He + #pi^{-}}, GeV/c^{2}");
    h->GetXaxis()->SetTitleSize(0.045);
    h->GetXaxis()->SetTitleOffset(0.9);
    h->GetXaxis()->SetTitleFont(62);
    h->GetXaxis()->SetLabelFont(62);

    h->GetYaxis()->SetTitle(
        Form("Entries / (%d MeV/c^{2})", (int)(1000 * (maxBound - minBound) / h->GetXaxis()->GetNbins() + 0.5)));
    h->GetYaxis()->SetTitleSize(0.045);
    h->GetYaxis()->SetTitleOffset(1.1);
    h->GetYaxis()->SetTitleFont(62);
    h->GetYaxis()->SetLabelFont(62);

    h->SetMarkerStyle(20);
    h->SetMarkerSize(0.6);
    h->SetMarkerColor(kBlack);
    h->SetLineColor(kBlack);
    h->SetMinimum(0);

    TF1* fitS = h->GetFunction("fitFcn");
    if (!fitS)
        return;
    fitS->SetNpx(500);
    fitS->SetLineColor(kBlack);
    TF1* fitB = h->GetFunction("fitBack");
    if (!fitB)
        return;
    fitB->SetLineColor(kBlack);
    fitB->SetLineStyle(7);
    // cout << fitS->GetParameter(8) << " " << fitS->GetParameter(9) << endl;
    auto leg = new TLegend(0.2, 0.2, 0.6, 0.5);
    leg->SetHeader("Parameters:", "L");
    leg->AddEntry((TObject*)0, Form("S = %d", s), "");
    leg->AddEntry((TObject*)0, Form("S / B = %4.2f", s / b), "");
    leg->AddEntry((TObject*)0, Form("S / #sqrt{S + B} = %4.2f", s / TMath::Sqrt(s + b)), "");
    leg->AddEntry((TObject*)0, Form("#mu = %4.4f", fitS->GetParameter(8)), "");
    leg->AddEntry((TObject*)0, Form("#sigma = %4.4f", TMath::Abs(fitS->GetParameter(9))), "");
    h->Draw("pe1");
    leg->Draw();
    c->SaveAs(Form("%s.png", h->GetName()));
}

void fitator(TH1F* h, Double_t& Tout, Double_t& Bout)
{

    if (h->GetEntries() == 0)
        return;

    Int_t leftBin = h->FindBin(leftSig);
    Int_t rightBin = h->FindBin(rightSig);

    vector<Float_t> values;
    for (Int_t i = leftBin; i < rightBin; i++) {
        values.push_back(h->GetBinContent(i));
        h->SetBinContent(i, 0);
    }
    TF1* fitBack = new TF1("fitBack", backgroundChe, minBound, maxBound, 7);
    h->Fit(fitBack, "QR", "", minBound, maxBound);

    Double_t B = 0;
    Double_t T = 0;

    for (Int_t i = leftBin; i < rightBin; i++) {
        h->SetBinContent(i, values[i - leftBin]);
        T += values[i - leftBin];
        B += fitBack->Eval(h->GetBinCenter(i));
    }

    TF1* fitFcn = new TF1("fitFcn", fitFunction, minBound, maxBound, 10);
    fitFcn->SetLineColor(kBlue);

    fitFcn->SetParameter(0, fitBack->GetParameter(0));
    fitFcn->SetParameter(1, fitBack->GetParameter(1));
    fitFcn->SetParameter(2, fitBack->GetParameter(2));
    fitFcn->SetParameter(3, fitBack->GetParameter(3));
    fitFcn->SetParameter(4, fitBack->GetParameter(4));
    fitFcn->SetParameter(5, fitBack->GetParameter(5));
    fitFcn->SetParameter(6, fitBack->GetParameter(6));
    fitFcn->SetParameter(7, 400 /*fitSig->GetParameter(0)*/);
    fitFcn->SetParameter(8, 2.992 /*fitSig->GetParameter(1)*/);
    fitFcn->SetParameter(9, 0.003 /*fitSig->GetParameter(2)*/);

    h->Fit(fitFcn, "QR+", "ep", minBound, maxBound);

    // h->GetYaxis()->SetRangeUser(0, 9000);

    Tout = T;
    Bout = B;
}

void filtrator(TString inFileName = "/zfs/store6.hydra.local/merts/h3l_plots/H3L.root")
{

    TFile* f = TFile::Open(inFileName);

    TIter keyList(f->GetListOfKeys());
    TKey* key;

    TList* l1 = new TList();

    Int_t nFiltered = 0;

    cout << "Number of total hist = " << f->GetListOfKeys()->GetSize() << endl;

    Float_t maxS = 0.0;
    Float_t maxS_B = 0.0;
    Float_t maxS_S = 0.0;
    Float_t maxSB = 0.0;
    Float_t maxSB_B = 0.0;
    Float_t maxSB_S = 0.0;
    Float_t maxSig = 0.0;
    Float_t maxSig_B = 0.0;
    Float_t maxSig_S = 0.0;
    TH1F* hMaxS = nullptr;
    TH1F* hMaxSB = nullptr;
    TH1F* hMaxSig = nullptr;

    while ((key = (TKey*)keyList())) {
        TH1F* h = (TH1F*)key->ReadObj();

        TString name = h->GetName();
        if (!name.Contains("hInvMass"))
            continue;

        // h->Rebin(2);
        h->Rebin(2);

        Double_t T = 0;
        Double_t B = 0;
        fitator(h, T, B);

        Float_t S = T - B;
        Float_t SB = S / B;
        Float_t SIGN = S / TMath::Sqrt(T);

        if (S <= 0 || B <= 0 || T <= 0)
            continue;

        if (S > maxS) {
            maxS = S;
            maxS_B = B;
            maxS_S = S;
            hMaxS = h;
            hMaxS->SetTitle(Form("maxS___S:%4.2f_B:%4.2f_SB:%4.2f_SIGN:%4.2f", S, B, SB, SIGN));
        }

        if (SB > maxSB) {
            maxSB = SB;
            maxSB_B = B;
            maxSB_S = S;
            hMaxSB = h;
            hMaxSB->SetTitle(Form("maxSB___S:%4.2f_B:%4.2f_SB:%4.2f_SIGN:%4.2f", S, B, SB, SIGN));
        }

        if (SIGN > maxSig) {
            maxSig = SIGN;
            maxSig_B = B;
            maxSig_S = S;
            hMaxSig = h;
            hMaxSig->SetTitle(Form("maxSig___S:%4.2f_B:%4.2f_SB:%4.2f_SIGN:%4.2f", S, B, SB, SIGN));
        }

        // if (B < 10000) continue;
        if (S < 700)
            continue;
        if (SB < 0.025)
            continue;
        if (SIGN < 4.55)
            continue;
        cout << h->GetName() << ":\t" << B << "\t" << S << "\t" << SIGN << "\t" << SB << endl;
        // string s1_SN_ratio = ", #frac{S}{N} = " + to_string(SN_ratio);

        // string s1_total = s1_mu + s1_sigma + s1_chisquared_ndf + s1_SN_ratio;

        // auto leg = new TLegend(0.6, 0.65, 0.88, 0.85);
        // leg->SetHeader("Parameters of interest", "C");
        // leg->AddEntry(signalFcn, s1_total.c_str(), "l");
        // leg->Draw();

        nFiltered++;
        l1->Add(h);
    }

    l1->Add(hMaxS);
    l1->Add(hMaxSB);
    l1->Add(hMaxSig);

    DrawNicePlot(hMaxS, maxS_S, maxS_B);
    DrawNicePlot(hMaxSB, maxSB_S, maxSB_B);
    DrawNicePlot(hMaxSig, maxSig_S, maxSig_B);

    cout << "Number of filtered histo = " << nFiltered << endl;

    TFile* fOut = new TFile("/zfs/store6.hydra.local/merts/h3l_plots/H3L_filtered.root", "recreate");
    l1->Write("InvMasses", TObject::kSingleKey);
    fOut->Close();
}
