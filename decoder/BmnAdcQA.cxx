#include "BmnAdcQA.h"

#define PAD_WIDTH 1920
#define PAD_HEIGHT 1920

BmnAdcQA::BmnAdcQA(Int_t period, Int_t run, TString det)
: fVerbose(0),
  fSetup(kBMNSETUP),
  fDetName(det),
  fPeriod(period),
  fRun(run)
{}

BmnAdcQA::~BmnAdcQA() {
}

void BmnAdcQA::SetAveragePedestal(UInt_t ser, Int_t ch, Int_t smpl, Int_t spill_id, Double_t content) {
    auto it = fAdcHists.find(AdcKey(ser, ch, smpl));
    if (it == fAdcHists.end()) {
        TString name(Form("Pedestal_serial_%08X_ch_%02d_smpl_%d", ser, ch, smpl));
        const Int_t n_spills = 40;
        auto p = fAdcHists.insert(make_pair(AdcKey(ser, ch, smpl), std::make_unique<TH1D>(name, name, n_spills, 0, n_spills)));
        it = p.first;
        it->second->SetDirectory(0);
    }
    auto& h = it->second;
    h->SetBinContent(spill_id, content);
    printf("Pedestal serial %08X ch %2d smpl %3d  spill %d %4.2f\n", ser, ch, smpl, spill_id, content);
}

void BmnAdcQA::SaveHists() {
    TString file_name(Form("adc-per%02d-run%06d-%s.pdf", fPeriod, fRun, fDetName.Data()));
    TCanvas *c = new TCanvas("can_pedestals", "can", PAD_WIDTH, PAD_HEIGHT);
    c->Print(file_name + "[");
    for (auto &it : fAdcHists) {
        TH1 * h = it.second.get();
        //const char* name = h->GetName();
        c->Clear("D");
        c->cd();
        h->Draw();
        c->Print(file_name);
    }
    c->Print(file_name + "]");
}

void BmnAdcQA::ClearDebugHists() {
//    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
//        hPedSi[iCr]->Reset();
//        hSModeSi[iCr]->Reset();
//        hCModeSi[iCr]->Reset();
//        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
//            hPedLine[iCr][iCh]->Reset();
//            hSMode[iCr][iCh]->Reset();
//            hCMode[iCr][iCh]->Reset();
//        }
//    }
}
