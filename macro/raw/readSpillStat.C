
#define PAD_WIDTH 3840
#define PAD_HEIGHT 480
/**
 * readSpillStat
 * Example of reading spill metadata
 * @param fileName
 * @param examples number of spills to show
 */
void readSpillStat(TString fileName = "bmn_run7820_meta.root", Long64_t NExamples = 0)
{
    if (fileName == "") {
        cout << "File not specified!" << endl;
        return;
    }
    bool print_trc = false;
    bool print_by_spill = false;
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("info");   // for info write "INFO"
    const vector<string> evt_tr_names{                    //        "BT", "MBT", "CCT1", "CCT2"};
                                      "BT1", "MBT", "CCT1", "nFD"};
    //"BC0","pBT","BT","BC1","NIT","CCT1","MBT","BT1","CCT2","FHCal","BD_BA","nFD","CCT3","CCT4"};
    //    map<string, UInt_t> rfs;
    //    rfs["BT"] = 2000;
    //    rfs["MBT"] = 35;
    //    rfs["CCT1"] = 230;
    //    rfs["CCT2"] = 1;
    gStyle->SetOptStat(2211);

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
    map<SysPoint, BmnSpillInfo>& fSpillMap = metadata->SpillMap();
    NameChMap& fTrigBitsMap = metadata->TrcMap();
    Name2SerCh& fScalersMap = metadata->ScalersMap();
    vector<string> trc_names{"pBT", "BT", "MBT", "NiT", "BT&nBUSY", "CCT1", "CCT2"};
    auto& total_cntrs = metadata->SpillCounters();
    if (print_trc) {
        if (print_by_spill) {
            for (auto& it : metadata->SpillMap()) {
                BmnSpillInfo& info = it.second;
                auto& spill_cntrs = info.counters;
                printf("\n\tstart %s\n\tstop  %s\n", BmnFunctionSet::TimePoint2String(info.start_ts).c_str(),
                       BmnFunctionSet::TimePoint2String(info.stop_ts).c_str());
                if (spill_cntrs.cntr_input.size() <= 1) {
                    printf("not enough data");
                    continue;
                }
                for (string& name : trc_names) {
                    uint16_t icntr = fTrigBitsMap[name];
                    printf("\t icntr %6u %8s\n", icntr, name.c_str());
                    printf("\t%8s          before %8llu     after %8llu\n", name.c_str(),
                           spill_cntrs.cntr_befor_err[icntr], spill_cntrs.cntr_after_err[icntr]);
                    printf("\t%8s   BUSY    input %8llu input_red %8llu\n", name.c_str(),
                           spill_cntrs.cntr_input[6].second[icntr], spill_cntrs.cntr_input_after_red[6].second[icntr]);
                    printf("\t%8s NOT_BUSY  input %8llu input_red %8llu\n", name.c_str(),
                           spill_cntrs.cntr_input[7].second[icntr], spill_cntrs.cntr_input_after_red[7].second[icntr]);
                }
            }
        }
        printf("\n\tTRC TOTAL:\n");
        for (string& name : trc_names) {
            uint16_t icntr = fTrigBitsMap[name];
            printf("\t icntr %6u %8s", icntr, name.c_str());
            printf("\t%8s          before %8llu     after %8llu\n", name.c_str(), total_cntrs.cntr_befor_err[icntr],
                   total_cntrs.cntr_after_err[icntr]);
            printf("\t%8s   BUSY    input %8llu input_red %8llu\n", name.c_str(),
                   total_cntrs.cntr_input[6].second[icntr], total_cntrs.cntr_input_after_red[6].second[icntr]);
            printf("\t%8s NOT_BUSY  input %8llu input_red %8llu\n", name.c_str(),
                   total_cntrs.cntr_input[7].second[icntr], total_cntrs.cntr_input_after_red[7].second[icntr]);
        }
    }

    TTree* tree = static_cast<TTree*>(file->Get("spill"));
    BmnSpillHeader* sh = nullptr;
    TClonesArray* msc = nullptr;
    TClonesArray* t0raw = nullptr;
    tree->SetBranchAddress("BmnSpillHeader.", &sh);
    tree->SetBranchAddress("MSC", &msc);
    tree->SetBranchAddress("T0Raw", &t0raw);
    /** canvas for trig counters per spill*/
    vector<unique_ptr<TH1I>> evt_tr_hist;
    size_t trigCount = evt_tr_names.size();
    TString name = "Evt_triggers";
    auto can{make_unique<TCanvas>(name, name, PAD_WIDTH * 1, PAD_HEIGHT * trigCount)};
    can->Divide(1, trigCount);
    for (Int_t i = 0; i < trigCount; i++) {
        name = TString("Evt_cnt_") + TString(evt_tr_names[i]);
        evt_tr_hist.push_back(make_unique<TH1I>(name, name, 500, 0, (Double_t)1));
    }

    Long64_t NSpills = tree->GetEntries();
    NExamples = (NExamples == 0) ? NSpills : min(NSpills, NExamples);
    for (Int_t iSpill = 0; iSpill < NExamples; iSpill++) {
        tree->GetEntry(iSpill);
        printf("Spill:\n\tstart %s\n\tstop  %s\n", BmnFunctionSet::TimePoint2String(sh->GetStartTime()).c_str(),
               BmnFunctionSet::TimePoint2String(sh->GetStopTime()).c_str());
        for (auto& sca : sh->Scalers()) {
            printf("MSC %-14s  %8lu\n", sca.first.c_str(), sca.second);
        }
        if (t0raw->GetEntries()) {
            printf("T0 time %s\n", BmnFunctionSet::TimePoint2String(
                                       (static_cast<BmnT0Raw<kT0_BIN_BLOCK_WORDS>*>(t0raw->At(0)))->GetTime())
                                       .c_str());
            for (auto& t0_map_el : rh->T0Map()) {
                const T0MapKey& key = t0_map_el.first;
                if (key.first.find("BD") == 0)
                    continue;
                printf("T0 %-14s.%02u = %7u\n", key.first.c_str(), key.second, sh->RawT0Counters()[t0_map_el.second]);
            }
        }
        printf("Event spill counters:\n");
        for (Int_t i = 0; i < trigCount; i++) {
            const string& tr_name = evt_tr_names[i];
            ULong64_t tr_cnt = sh->RawT0Counters()[rh->T0Map()[make_pair(tr_name, 0)]];
            Double_t rf = tcc[tr_name].reduction_en ? (tcc[tr_name].reduction_factor + 1) : 1;
            //            Double_t rf = rfs[tr_name];
            Double_t tr_cnt_ar = tr_cnt / rf;
            printf("\tT0 %-14s = %10llu ; after reduction %10.2f\n", tr_name.c_str(), tr_cnt, tr_cnt_ar);
            ULong64_t ev_tr_cnt = sh->EventTriggerCnt()[tr_name];
            Double_t fract = ev_tr_cnt / tr_cnt_ar;
            printf("\tEvt trig cnt     = %10llu ; (ev_tr_cnt/cnt_after_red) = %10.3f\n", ev_tr_cnt, fract);
            evt_tr_hist[i]->Fill(fract);
        }
    }
    printf("Evt trigger count Total:\n");
    printf("Total %8llu events\n", rh->GetNEvents());
    for (auto& el : rh->GetTrcBitMap()) {
        const string& tr_name = el.first;
        printf("%-12s %8lu\n", tr_name.c_str(), rh->EventTriggerCnt()[tr_name]);
    }
    printf("Raw T0 Total:\n");
    for (auto& t0_map_el : rh->T0Map()) {
        const T0MapKey& key = t0_map_el.first;
        if (key.first.find("BD") == 0)
            continue;
        printf("T0 %-14s.%02u = %10llu\n", key.first.c_str(), key.second,
               rh->GetRawT0Counter(key.first.c_str(), key.second));
    }
    printf("MSC Total:\n");
    rh->PrintScalers();
    if (rh) {
        printf("Total spill counters:\n");
        for (auto& tr_name : evt_tr_names) {
            ULong64_t tr_cnt = rh->GetRawT0Counter(tr_name);
            Double_t rf = tcc[tr_name].reduction_en ? (tcc[tr_name].reduction_factor + 1) : 1;
            //            Double_t rf = rfs[tr_name];
            Double_t tr_cnt_ar = tr_cnt / rf;
            printf("\tT0 %-14s = %10llu ;   after reduction   %10.2f\n", tr_name.c_str(), tr_cnt, tr_cnt_ar);
            ULong64_t ev_tr_cnt = rh->EventTriggerCnt()[tr_name];
            Double_t fract = ev_tr_cnt / tr_cnt_ar;
            printf("\tEvt trig cnt      = %10llu ; (ev_tr_cnt/cnt_after_red) = %10.3f\n", ev_tr_cnt, fract);
        }
    }
    for (Int_t i = 0; i < trigCount; i++) {
        can->cd(i + 1);
        evt_tr_hist[i]->Draw();
    }
    can->SaveAs(Form("evt_cntrs_%d.png", RunId));
    //    file->Close();
}
