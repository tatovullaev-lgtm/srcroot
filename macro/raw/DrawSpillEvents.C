
#include <ROOT/RDF/RInterface.hxx>

// #if !defined(__CINT__) || defined(__MAKECINT__)
// #include <memory>
// #include <chrono>
// #include <iostream>
// #include <map>
//// ROOT
// #include <TString.h>
// #include <TStyle.h>
// #include <TFile.h>
// #include <TTree.h>
// #include <TTimeStamp.h>
//// fairroot
////#include <fairlogger/Logger.h>
// #include <FairLogger.h>
//// BmnRoot
// #include "../../base/header/DigiRunHeader.h"
// #include <BmnMetadataRaw.h>
// #include <BmnSpillHeader.h>
// #endif

#define PAD_WIDTH 3840
#define PAD_HEIGHT 640

using namespace std;

/**
 * DrawSpill
 * Draw the MSC data
 * @param fileName
 * @param examples number of spills to show
 */
void DrawSpillEvents(TString fileName = "bmn_run9362_ev0_p0_digi.root")
{
    if (fileName == "") {
        cout << "File not specified!" << endl;
        return;
    }
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("info");   // for info write "INFO"
    //    const vector<string> evt_tr_names{"BT", "MBT", "CCT1", "CCT2"};
    const vector<string> evt_tr_names{"BT",          "MBT", "CCT1", "CCT2", "RC_Trig_Cand",
                                      "DAQ_RO_Trig", "pBT", "BC1L", "NiT",  "BT&nBUSY"};
    //    gStyle->SetOptStat("e");

    TFile* file = new TFile(fileName, "READ");
    if (file->IsOpen() == false) {
        //        printf("\nCannot open file %s !\n", fileName.Data());
        LOG(error) << "Cannot open file " << fileName << " !" << endl;
        return;
    }
    DigiRunHeader* rh = static_cast<DigiRunHeader*>(file->Get("DigiRunHeader"));
    BmnMetadataRaw* metadata = static_cast<BmnMetadataRaw*>(file->Get("RawRunMetadata"));

    if (rh == nullptr) {
        LOG(error) << "No run header!" << endl;
        return;
    }
    printf("Run Header content:\n");
    UInt_t PeriodId = rh->GetPeriodId();
    UInt_t RunId = rh->GetRunId();
    printf("\tPeriod: %u  Run: %u\n", PeriodId, RunId);
    TTimeStamp RunStartTime = rh->GetRunStartTime();
    TTimeStamp RunEndTime = rh->GetRunEndTime();
    printf("\tSTART  : %s\n", RunStartTime.AsString());
    printf("\tFINISH : %s\n", RunEndTime.AsString());
    auto& tcc = rh->GetTrigConfig().TrigChannelConfig();
    map<SysPoint, BmnSpillInfo>& fSpillMap = rh->GetSpillMap();
    //    map<SysPoint, BmnSpillInfo>& fSpillMap = metadata->SpillMap();
    NameChMap& fTrigBitsMap = metadata->TrcMap();
    Name2SerCh& fScalersMap = metadata->ScalersMap();
    // gStyle->SetTitleSize(22);
    TTree* tree = static_cast<TTree*>(file->Get("spill"));
    BmnSpillHeader* sh = nullptr;
    TClonesArray* msc = nullptr;
    TClonesArray* t0raw = nullptr;
    tree->SetBranchAddress("BmnSpillHeader.", &sh);
    tree->SetBranchAddress("MSC", &msc);
    tree->SetBranchAddress("T0Raw", &t0raw);
    /** canvas for trig counters per spill*/
    vector<unique_ptr<TH1I>> evt_tr_hist;
    size_t trigCount = 1;   // evt_tr_names.size();
    TString name = "Evt_triggers";
    //    auto can{make_unique<TCanvas>(name, name, PAD_WIDTH * 1, PAD_HEIGHT * trigCount)};
    auto can{make_unique<TCanvas>(name, name, 1920, 1080)};
    can->Divide(1, trigCount);
    Double_t left_time(0), right_time(128);
    const int32_t nbinsX = 128 + 1;   // TMath::Min(500.0, (right_time - left_time) / msc_delta);
    const int32_t nbinsY = 250;
    int32_t station = 0;
    int32_t module = 0;
    int32_t layer = 1;
    name = TString("SiBT0");
    unique_ptr<TH2I> hist = make_unique<TH2I>(name, name, nbinsY, 0, 12e9, nbinsX + 1, 0, nbinsX);
    hist->SetLineWidth(3);
    hist->GetXaxis()->SetTitle("time");
    hist->GetYaxis()->SetTitle("X");
    hist->GetXaxis()->SetLabelSize(0.03);
    hist->GetYaxis()->SetLabelSize(0.03);
    //    hist->GetXaxis()->SetTitleOffset(50);
    hist->GetXaxis()->SetLabelOffset(0.028);
    hist->GetYaxis()->SetLabelOffset(0.015);

    if (fSpillMap.size() < 2) {
        printf("No Spill in the map!");
        return;
    }
    //        auto itb = fSpillMap.begin();
    //    BmnSpillInfo& si = (++itb)->second;
    TString file_names = "./bmn_run9362_ev*_p*_digi.root";
    TChain chain("bmndata");
    chain.Add(file_names);
    BmnEventHeader* inEventHeader(nullptr);
    TClonesArray* inSibt(nullptr);
    map<SysPoint, int64_t> ts2event;
    chain.SetBranchAddress("BmnEventHeader.", &inEventHeader);
    chain.SetBranchAddress("SiBT", &inSibt);
    //    for (int64_t iEntry = 0; iEntry < chain.GetEntries(); iEntry++) {
    //        chain.GetEntry(iEntry);
    //        ts2event.insert(make_pair(inEventHeader->GetEventTimeTP(), iEntry));
    //        //        printf("insert iev %u  ientry %li\n", inEventHeader->GetEventId(), iEntry);
    //
    //    }

    ROOT::RDataFrame df{"bmndata", file_names};

    uint32_t ispill(0);
    for (auto& mapel : fSpillMap) {
        hist->Reset();
        BmnSpillInfo& si = mapel.second;

        auto df_spill = df.Filter(
            [&](BmnEventHeader& eh) {
                return ((eh.GetEventTimeTP() >= si.start_ts) && (eh.GetEventTimeTP() <= si.stop_ts));
            },
            {"BmnEventHeader."});

        auto df_sp_digi = df_spill
                              .Define("tss",
                                      [&](BmnEventHeader& eh) {
                                          return std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                     eh.GetEventTimeTP() - si.start_ts)
                                              .count();
                                      },
                                      {"BmnEventHeader."})
                              .Define("strip",
                                      [&](TClonesArray& array) {
                                          std::vector<int32_t> out;
                                          for (int32_t iDig = 0; iDig < array.GetEntries(); iDig++) {
                                              BmnSiBTDigit* dig = static_cast<BmnSiBTDigit*>(array.At(iDig));
                                              if ((dig->GetStation() != station) || (dig->GetModule() != module)
                                                  || (dig->GetStripLayer() != layer))
                                                  continue;
                                              out.push_back(dig->GetStripNumber());
                                          }
                                          return out;
                                      },
                                      {"SiBT"});
        //        auto res = df_sp_digi.Histo1D("tss");//,"strip0");
        auto res = df_sp_digi.Histo2D({name, name, nbinsY, 0, 11e9, nbinsX + 1, 0, nbinsX}, "tss", "strip");
        //        auto res = df_sp_digi.Histo2D("digi", "digi", nbinsY, 0, 12e9, nbinsX + 1, 0, nbinsX);
        can->cd(1);
        res->Draw("colz");
        can->SaveAs(Form("spill_strip_run%u_l%d_spill%d.png", RunId, layer, ispill));

        //        auto it_start = ts2event.lower_bound(si.start_ts);
        //        auto it_stop = ts2event.upper_bound(si.stop_ts);
        //        if (it_start == ts2event.end()) {
        //            LOG(error) << "Cant'f find events in the spill";
        //            return;
        //        }
        //        printf("Spill:\n\tstart ts: %s\n\tstop  ts: %s\n",
        //        BmnFunctionSet::TimePoint2String(si.start_ts).data(),
        //                BmnFunctionSet::TimePoint2String(si.stop_ts).data());
        //        for (auto it_time = it_start; it_time != it_stop; it_time++) {
        //            SysPoint time = it_time->first;
        //            int64_t iEntry = it_time->second;
        //            chain.GetEntry(iEntry);
        //            SysPoint point = inEventHeader->GetEventTimeTP();
        //            uint32_t iev = inEventHeader->GetEventId();
        //            for (int32_t iDig = 0; iDig < inSibt->GetEntries(); iDig++) {
        //                BmnSiBTDigit* dig = static_cast<BmnSiBTDigit*>(inSibt->At(iDig));
        //                if ((dig->GetStation() != station) || (dig->GetModule() != module) || (dig->GetStripLayer() !=
        //                layer))
        //                    continue;
        //                int64_t time_since_start = std::chrono::duration_cast<std::chrono::nanoseconds>(time -
        //                si.start_ts).count(); //.time_since_epoch();
        //                //                        printf("iev %7u  strip %3d time_since_start %12lu   time  %s \n",
        //                //                                iev, dig->GetStripNumber(), time_since_start,
        //                BmnFunctionSet::TimePoint2String(time).data()); hist->Fill(time_since_start,
        //                dig->GetStripNumber());
        //            }
        //
        //        }
        //        can->cd(1);
        //        hist->Draw("colz");
        //        can->SaveAs(Form("spill_strip_run%u_s%u.png", RunId, ispill));
        ispill++;
    }
}
