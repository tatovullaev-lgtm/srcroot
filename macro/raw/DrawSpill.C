
#define PAD_WIDTH 3840
#define PAD_HEIGHT 1280

/**
 * DrawSpill
 * Draw the MSC data
 * @param fileName
 * @param examples number of spills to show
 */
void DrawSpill(TString fileName = "bmn_run7820_meta.root", TString digiPath = ".", bool drawBusy = true)
{
    if (fileName == "") {
        cout << "File not specified!" << endl;
        return;
    }
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("info");   // for info write "INFO"
    //    const vector<string> evt_tr_names{"BT", "MBT", "CCT1", "CCT2"};
    const vector<string> evt_tr_names{
        //        "BT", "MBT", "CCT1", "CCT2", "RC_Trig_Cand", "DAQ_RO_Trig", "pBT", "BC1L", "NiT", "BT&nBUSY"};
        //        "BT", "MBT", "CCT1", "CCT2", "nFD", "RC_Trig_Cand", "DAQ_RO_Trig"};

        //        "BC0", "BC1", "pBT", "BT", "BT1", "NIT", "MBT", "CCT1", "CCT2", "CCT3", "CCT4",
        //        "FHCal", "BD_BA", "nFD",
        //        "Busy", "L0_Trig", "L1_Trig", "L1_Trig_v2", "L2_Trig", "L2_Trig_v2"};
        //        "pBT", "BT", "BT1", "NIT", "CCT2", "CCT3", "CCT4", "FHCal", "BD_BA", "L1_Trig", "L2_Trig_v2",
        //        "BC1",
        "BT'", "MBT", "nFD", "CCT1", "L0_Trig", "L1_Trig_v2", "L2_Trig", "Busy"};
    gStyle->SetOptStat("e");

    TFile* file = new TFile(fileName, "READ");
    if (file->IsOpen() == false) {
        LOG(error) << "Cannot open file " << fileName << " !" << endl;
        return;
    }

    //    TFile* filerh = new TFile("bmn_run9454_ev7_p8_digi.root", "READ");
    //    if (filerh->IsOpen() == false) {
    //        LOG(error) << "Cannot open file " << fileName << " !" << endl;
    //        return;
    //    }
    //    DigiRunHeader* rh = static_cast<DigiRunHeader*>(filerh->Get("DigiRunHeader"));
    DigiRunHeader* rh = static_cast<DigiRunHeader*>(file->Get("DigiRunHeader"));
    //    BmnMetadataRaw* metadata = static_cast<BmnMetadataRaw*>(file->Get("RawRunMetadata"));

    if (rh == nullptr) {
        LOG(error) << "No run header!" << endl;
        return;
    }
    printf("Run Header content:\n");
    UInt_t PeriodId = rh->GetPeriodId();
    UInt_t RunId = rh->GetRunId();
    uint64_t msc_delta = PeriodId > 8 ? 16 : 200;
    uint64_t trc_delta = 8;
    uint64_t trc_slices_shift = 4;
    printf("\tPeriod: %u  Run: %u\n", PeriodId, RunId);
    TTimeStamp RunStartTime = rh->GetRunStartTime();
    TTimeStamp RunEndTime = rh->GetRunEndTime();
    printf("\tSTART  : %s\n", RunStartTime.AsString());
    printf("\tFINISH : %s\n", RunEndTime.AsString());
    auto& tcc = rh->GetTrigConfig().TrigChannelConfig();
    NameChMap& fTrigBitsMap = rh->GetTrcBitMap();
    //    map<SysPoint, BmnSpillInfo>& fSpillMap = metadata->SpillMap();
    //    NameChMap& fTrigBitsMap = metadata->TrcMap();
    //    Name2SerCh& fScalersMap = metadata->ScalersMap();
    // gStyle->SetTitleSize(22);
    TTree* tree = static_cast<TTree*>(file->Get("spill"));
    BmnSpillHeader* sh = nullptr;
    TClonesArray* msc = nullptr;
    TClonesArray* t0raw = nullptr;
    tree->SetBranchAddress("BmnSpillHeader.", &sh);
    tree->SetBranchAddress("MSC", &msc);
    tree->SetBranchAddress("T0Raw", &t0raw);
    /** canvas for trig counters per spill*/
    vector<unique_ptr<TH1D>> evt_tr_hist;
    size_t trigCount = evt_tr_names.size();
    size_t padCount = trigCount + 1;
    TString name = "Evt_triggers";
    auto can{make_unique<TCanvas>(name, name, PAD_WIDTH * 1, PAD_HEIGHT * padCount)};
    can->Divide(1, padCount);
    //    Double_t left_time(0), right_time(5e9);
    //                Double_t left_time(0e9), right_time(12e9);
    //        Double_t left_time(1e9), right_time(8e9);
    //    Double_t left_time(5e9), right_time(5.1e9);
    //        Double_t left_time(5.0158e9), right_time(5.016e9);
    //        Double_t left_time(5.01588e9), right_time(5.01598e9);
    //    Double_t left_time(5.0158838e9), right_time(5.0158855e9);
    //    Double_t left_time(5.0158846e9), right_time(5.0158855e9);
    //    Double_t left_time(5.0158852e9), right_time(5.0158855e9);
    //            Double_t left_time(5.0161e9), right_time(5.01635e9);
    Double_t left_time(5.01619245e9), right_time(5.01619275e9);
    UInt_t nbins = TMath::Min(500.0, (right_time - left_time) / msc_delta);
    UInt_t half_bin = (right_time - left_time) / (2 * nbins) - 1;
    printf("NBins = %u\n", nbins);
    UInt_t nbins_trc = TMath::Min(500.0, (right_time - left_time) / trc_delta);

    for (Int_t i = 0; i < trigCount; i++) {
        name = TString("scaler_") + TString(evt_tr_names[i]);
        evt_tr_hist.push_back(make_unique<TH1D>(name, name, nbins, left_time, right_time));
        unique_ptr<TH1D>& h = evt_tr_hist.back();
        h->SetLineWidth(3);
        h->GetXaxis()->SetLabelSize(0.08);
        h->GetYaxis()->SetLabelSize(0.09);
        h->GetXaxis()->SetTitleOffset(50);
        h->GetXaxis()->SetLabelOffset(0.028);
        h->GetYaxis()->SetLabelOffset(0.015);
    }
    // TRC bit state time line
    name = "TRC_bit_state";
    UInt_t n_trc_bits = 16;
    //    unique_ptr<TH2I> h_TRC_time_bits =
    //            make_unique<TH1I>(name, name, nbins_trc, left_time, right_time, n_trc_bits, 0, n_trc_bits);
    //    for (auto &[tr_name: i_bit]: fTrigBitsMap) {
    //        h_TRC_time_bits->GetYaxis()->SetBinLabel(i_bit, tr_name.data());
    //    }
    // Load events dataframe
    TString file_names(Form("%s/bmn_run%u_ev*_p*_digi.root", digiPath.Data(), RunId));
    ROOT::RDataFrame df{"bmndata", file_names};

    Long64_t NSpills = tree->GetEntries();
    for (Int_t iSpill = 0; iSpill < NSpills; iSpill++) {
        tree->GetEntry(iSpill);
        SysPoint left_tp = sh->GetStartTime() + ((uint64_t)left_time) * 1ns;
        SysPoint right_tp = sh->GetStartTime() + ((uint64_t)right_time) * 1ns;
        printf("Spill:\n\tstart %s\n\tstop  %s\n", BmnFunctionSet::TimePoint2String(sh->GetStartTime()).c_str(),
               BmnFunctionSet::TimePoint2String(sh->GetStopTime()).c_str());

        if (msc->GetEntries()) {
            BmnMSCMatrix<UChar_t>* msc_matrix = static_cast<BmnMSCMatrix<UChar_t>*>(msc->At(0));
            for (Int_t i = 0; i < trigCount; i++) {
                evt_tr_hist[i]->Reset();
                string sc_name = evt_tr_names[i];
                auto it = rh->ScalersName2IndexMap().find(sc_name);
                if (it == rh->ScalersName2IndexMap().end())
                    continue;
                uint16_t i_sc = it->second;
                auto& tline = msc_matrix->ScalerTimeLine(sc_name);
                auto itLower = tline.lower_bound(left_tp);
                auto itUpper = tline.upper_bound(right_tp);
                //                auto itLower = tline.lower_bound(sh->GetStartTime() + (uint64_t)left_time * 1ns);
                //                auto itUpper = tline.upper_bound(sh->GetStopTime() + (uint64_t)right_time * 1ns);
                for (auto itTimes = itLower; itTimes != itUpper; itTimes++) {
                    SysPoint time = itTimes->first;
                    uint8_t cnt = itTimes->second;
                    int64_t nans = (time.time_since_epoch() - sh->GetStartTime().time_since_epoch()).count();
                    if (cnt)
                        evt_tr_hist[i]->Fill(nans, cnt);
                }
                //                for (size_t i_rec = 0; i_rec < msc_matrix->GetNRecords(); i_rec++) {
                //                    //                    uint8_t cnt = msc_matrix->Counts(i_rec, i_sc);
                //                    uint8_t cnt = msc_matrix->GetCount(i_rec, i_sc);
                //                    int64_t nans =
                //                            (msc_matrix->Time(i_rec).time_since_epoch() -
                //                            sh->GetStartTime().time_since_epoch()).count();
                //                    //                                        printf("MSC %7zu  %7u  %9ld\n", i_rec,
                //                    cnt, nans);
                //                    //            printf("MSC %u\n", msc_matrix->GetCount(i_rec, i_sc));
                //                    if (cnt)
                //                        evt_tr_hist[i]->Fill(nans, cnt);
                //                }
            }
            for (Int_t i = 0; i < trigCount; i++) {
                TVirtualPad* pad = can->cd(i + 1);
                gStyle->SetTitleFontSize(0.1);
                gStyle->SetTitleX(.5);
                gStyle->SetTitleY(1.01);
                evt_tr_hist[i]->Draw("hist");
                if (drawBusy)
                    for (size_t i_rec = 0; i_rec < msc_matrix->GetNRecords(); i_rec++) {
                        int64_t nans =
                            (msc_matrix->Time(i_rec).time_since_epoch() - sh->GetStartTime().time_since_epoch())
                                .count();
                        if ((nans > left_time) && (nans < right_time) && (msc_matrix->IsBusyBitVec()[i_rec])) {
                            int64_t nans_end = nans + msc_delta;
                            int32_t bin = evt_tr_hist[i]->FindBin(nans);
                            double low = evt_tr_hist[i]->GetBinLowEdge(bin);
                            double bin_width = evt_tr_hist[i]->GetXaxis()->GetBinWidth(bin);
                            TBox* box = new TBox(
                                //                                    nans/* - half_bin*/, 0,
                                low, 0,
                                //                                    nans_end/* - half_bin*/,
                                low + bin_width, evt_tr_hist[i]->GetMaximum());
                            box->SetFillColor(kRed);
                            box->SetFillStyle(3001);
                            box->Draw("same");
                        }
                    }
            }
            //                  TH1D* div = (TH1D*)evt_tr_hist[8]->Clone("L1/BC1");
            //                  div->Divide(evt_tr_hist[7].get());
            //                  div->SetTitle("nFD/CCT1");
            //                  can->cd(padCount);
            //                  div->Draw("hist");
            auto df_spill = df.Filter(
                [&](BmnEventHeader& eh) {
                    return ((eh.GetEventTimeTP() >= left_tp) && (eh.GetEventTimeTP() <= right_tp));
                },
                {"BmnEventHeader."});

            auto df_sp_digi =
                df_spill
                    .Define("trctime",
                            [&](BmnEventHeader& eh) {
                                //                        vector<pair<uint64_t, uint8_t>> bit_time_line;
                                //                        printf("Ev %u\n", eh.GetEventId());
                                vector<uint64_t> time_line;
                                uint64_t nans_ev_slice = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                             eh.GetEventTimeTP() - sh->GetStartTime())
                                                             .count();
                                nans_ev_slice -= trc_delta * trc_slices_shift;
                                //                        printf("%u  %zu\n", eh.GetEventId(),
                                //                        eh.GetInputSignalsVector().size());
                                for (uint32_t& bit_slice : eh.GetInputSignalsVector()) {
                                    nans_ev_slice += trc_delta;
                                    for (uint8_t i_bit = 0; i_bit < n_trc_bits; i_bit++) {
                                        if (bit_slice & BIT(i_bit)) {
                                            //  bit_time_line.push_back(make_pair(nans_ev_slice, i_bit));
                                            //                                    printf("%lu  %u\n", nans_ev_slice,
                                            //                                    i_bit);
                                            time_line.push_back(nans_ev_slice);
                                        }
                                    }
                                }
                                return time_line;
                            },
                            {"BmnEventHeader."})
                    .Define("trcbit",
                            [&](BmnEventHeader& eh) {
                                vector<uint8_t> trig_line;
                                uint64_t nans_ev_slice = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                             eh.GetEventTimeTP() - sh->GetStartTime())
                                                             .count();
                                nans_ev_slice -= trc_delta * trc_slices_shift;
                                for (uint32_t& bit_slice : eh.GetInputSignalsVector()) {
                                    nans_ev_slice += trc_delta;
                                    for (uint8_t i_bit = 0; i_bit < n_trc_bits; i_bit++) {
                                        if (bit_slice & BIT(i_bit))
                                            trig_line.push_back(i_bit);
                                    }
                                }
                                return trig_line;
                            },
                            {"BmnEventHeader."});
            auto res = df_sp_digi.Histo2D(
                {name, name, (int32_t)nbins_trc, left_time, right_time, (int32_t)n_trc_bits, 0, (double)n_trc_bits},
                "trctime", "trcbit");
            //                                          "trctb.first", "trctb.second");
            res->GetXaxis()->SetLabelSize(0.08);
            res->GetYaxis()->SetLabelSize(0.09);
            res->SetLineWidth(4);
            for (auto& map_el : fTrigBitsMap)
                res->GetYaxis()->SetBinLabel(map_el.second + 1, map_el.first.data());
            TVirtualPad* pad = can->cd(padCount);
            double xmin = left_time;    // pad->GetUxmin();
            double xmax = right_time;   // pad->GetUxmax();
            double step = (right_time - left_time) / nbins_trc;
            //            res->GetXaxis()->SetNdivisions(nbins_trc);
            gStyle->SetGridWidth(4);
            pad->SetGrid(0, 1);
            res->Draw("col");
            res->Draw("same Y+");

            for (double x = xmin; x <= xmax; x += step) {
                TLine* lx = new TLine(x, 0, x, n_trc_bits);
                lx->SetLineStyle(7);
                lx->SetLineWidth(4);
                lx->SetLineColor(kGray + 2);
                lx->Draw("same");
            }
            //            TAxis *left = res->GetYaxis();
            //            TAxis *right = (TAxis*)left->Clone();
            //            right->SetLabelOffset(1.0);
            //            right->Draw("HIST SAME Y+");
            //                        TVirtualPad* pad2 = can->cd(padCount);
            //               pad2->SetFillColor(0);
            //               pad2->SetFillStyle(4000);
            //               pad2->SetFrameFillStyle(0);
            //                        res->Draw("same Y+");
            //            pad->Update();

            can->SaveAs(Form("scalers_%u_s%d.png", RunId, iSpill));
            //            delete div;
            break;
        }
    }
}
