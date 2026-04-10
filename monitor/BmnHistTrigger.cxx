#include "BmnHistTrigger.h"

#include "BmnAliases.h"
#include "BmnRawDataDecoder.h"

#include <TProfile.h>

BmnHistTrigger::BmnHistTrigger(TString title, TString path, Int_t periodID, BmnSetup setup)
    : BmnHist(periodID, setup)
    , fRows(0)
    , fCols(TRIG_COLS)
    ,
    // BDEvents(new TClonesArray(BmnTrigDigit::Class())),
    // histTriggers(nullptr),
    // histTrigTimeByChannel(nullptr),
    histBDAmpByChannel(nullptr)
    , histBDChannels(nullptr)
    , histBDSimult(nullptr)
    , can2d(nullptr)
    , canTimes(nullptr)
    , canAmp(nullptr)
    , canInitDone(kFALSE)
    , indexBD(0)
    , canGridBD(nullptr)
    , fSelectedBDChannel(-1)
    , fLastSpillId(0)
    , fLastSpillStartTS(0, 0)
{
    fTitle = title;
    fName = title + "_cl";
    TString PeriodSetupExt = Form("%d%s.txt", fPeriodID, ((fSetup == kBMNSETUP) ? "" : "_SRC"));
    TString MapFileName = TString("Trig_map_Run") + PeriodSetupExt;
    TString PlaceMapFileName = TString("Trig_PlaceMap_Run") + PeriodSetupExt;
    TTree* temp_tree = new TTree("Temp", "Temp");
    fTrigMapper = new BmnTrigRaw2Digit(PlaceMapFileName, MapFileName, temp_tree);
    TString name;
    const Int_t rows4Summary = 3;
    // const Int_t rows4Spectrum = 2;
    TPRegexp re("TQDC_(\\S+)");
    vector<TClonesArray*>* arrays = fTrigMapper->GetTrigArrays();
    //    for (auto &map : *fTrigMapper->GetMap()) {
    storeAmpTQDC.resize(arrays->size());
    for (UInt_t iArr = 0; iArr < arrays->size(); ++iArr) {
        TClonesArray* ar = arrays->at(iArr);
        mapArrayNames.insert(make_pair(string(ar->GetName()), iArr));
        TString trName(ar->GetName());
        //        re.Substitute(trName, "$1");
        if (trName.Contains("BD") || trName.Contains("SI")) {
            Bool_t dupl = kFALSE;
            for (auto& tr : trigNames) {
                if (!strcmp(tr.Data(), trName.Data())) {
                    dupl = kTRUE;
                    break;
                }
            }
            if (dupl)
                continue;
            trigNames.push_back(trName);
            shortNames.push_back(trName);
            name = fTitle + "_" + trName + "_Time";
            TH1F* h = new TH1F(name, name, 300, 0, TRIG_TIME_WIN);
            mapTime.insert(make_pair(iArr, h));
            hists.push_back(h);
        } else {
            name = Form("%s", trName.Data());   //, map.module);
            trigNames.push_back(name);
            name = fTitle + "_" + name + "_Time";
            TH1F* h2 = new TH1F(name, name, 300, 0, TRIG_TIME_WIN);
            mapTime.insert(make_pair(iArr, h2));
            //            printf("to map   %s %d\n",name.Data(),iArr);
            hists.push_back(h2);
            Bool_t dupl = kFALSE;
            for (auto& tr : shortNames) {
                if (!strcmp(tr.Data(), trName.Data())) {
                    dupl = kTRUE;
                    break;
                }
            }
            if (dupl)
                continue;
            shortNames.push_back(trName);
        }
    }
    auto itBD = mapArrayNames.find("BD");
    if (itBD != mapArrayNames.end())
        indexBD = itBD->second;
    fRows = hists.size() / fCols + rows4Summary + (hists.size() % fCols > 0);
    name = fTitle + "CanvasTimesByChannel";
    //    can2d = new TCanvas(name, name, PAD_WIDTH * fCols, PAD_HEIGHT * rows4Spectrum);
    //    can2d->Divide(fCols, rows4Spectrum);
    //    can2dPads.resize(fCols * rows4Spectrum);
    name = fTitle + "CanvasTimes";
    canTimes = new TCanvas(name, name, PAD_WIDTH * fCols, PAD_HEIGHT * fRows);
    canTimes->Divide(fCols, fRows, 0.01 / fCols, 0.01 / fRows);
    canTimesPads.resize(fCols * fRows);
    for (Int_t iPad = 0; iPad < fRows * fCols; iPad++) {
        PadInfo* p = new PadInfo();
        canTimesPads[iPad] = p;
        canTimes->GetPad(iPad + 1)->SetGrid();
    }
    name = fTitle + "_BD_Channels";
    histBDChannels = new TH1I(name, name, BD_CHANNELS, 0, BD_CHANNELS);
    histBDChannels->GetXaxis()->SetTitle("Channel #");
    histBDChannels->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_BD_Amp_by_Channels";
    histBDAmpByChannel = new TH2I(name, name, BD_CHANNELS, 0, BD_CHANNELS, TRIG_AMP_BINS_BD, 0, MaxAmpTDC);
    histBDAmpByChannel->GetXaxis()->SetTitle("Channel #");
    histBDAmpByChannel->GetYaxis()->SetTitle("Amplitude, ns");
    name = fTitle + "_BD_Simultaneous";
    histBDSimult = new TH1I(name, name, TRIG_MULTIPL, 1, TRIG_MULTIPL + 1);
    histBDSimult->GetXaxis()->SetTitle("Channels #");
    histBDSimult->GetYaxis()->SetTitle("Activation Count");
    canTimesPads[1]->current = histBDChannels;
    canTimesPads[4]->current = histBDAmpByChannel;
    canTimesPads[4]->opt = "colz";
    canTimesPads[7]->current = histBDSimult;
    for (size_t i = 0; i < hists.size(); ++i) {
        canTimesPads[i + rows4Summary * fCols]->current = hists[i];
        hists[i]->GetYaxis()->SetTitle("Activation Count");
        hists[i]->GetXaxis()->SetTitle("Time, ns");
    }
    for (size_t iPad = 0; iPad < canTimesPads.size(); ++iPad) {
        PadInfo* pad = canTimesPads[iPad];
        TH1* h = pad->current;
        if (h) {
            BmnHist::SetHistStyleTH1(h);
        }
    }
    InitGrids();
    InitTrigMasked();
    InitScalers();
    delete temp_tree;
}

BmnHistTrigger::~BmnHistTrigger()
{
    delete canTimes;
    if (fDir)
        return;
    for (auto& pad : canProfilePads)
        if (pad)
            delete pad;
    for (auto& pad : canTimesPads)
        if (pad)
            delete pad;
    //    for (auto &pad : can2dPads)
    //        delete pad;
    for (auto& pad : padsGridBD)
        if (pad)
            delete pad;
    for (auto& pad : padsGridND)
        if (pad)
            delete pad;
    for (auto& pad : padsTrigStates)
        if (pad)
            delete pad;
    for (auto& pad : padsCorrTQDC)
        if (pad)
            delete pad;
    for (auto& pad : padsScalers)
        if (pad)
            delete pad;
    if (fTrigMapper)
        delete fTrigMapper;
}

void BmnHistTrigger::InitTrigMasked()
{
    DBG("start")
    const vector<string> TrigName{"TQDC_BC1", "TQDC_BC2", "TQDC_FD", "TQDC_FHCAL", "TQDC_tFHCal", "BD"};
    //    const vector<string> TrigName2DX{
    //        "ВС1T vs BC1B", "ВС2T vs BC2B", "VCT vs VCB", "BC1S vs BC2S", "VCS vs BC2S", "FD vs BC2S"};
    //    const vector<string> TrigName2DY{"BD", "SI", "TQDC_FHCAL", "BD"};
    TString canName = fTitle + "_Trig_States";
    const size_t TrigBitsCnt = TrigState.size();
    const Int_t ColsStates = 2;
    const size_t RowsStates = TrigName.size();
    auto bit_map = fTrigMapper->GetTrcMap();
    for (auto& state : TrigState) {
        auto it_bm = bit_map.find(string(state.Name.Data()));
        if (it_bm != bit_map.end()) {
            trig_state_bit_index.push_back(it_bm->second);
        }
    }
    // canvas for 1D trig hists for different trig bits
    canTrigStates = make_unique<TCanvas>(canName, canName, PAD_WIDTH * ColsStates, PAD_HEIGHT * RowsStates);
    canTrigStates->Divide(ColsStates, RowsStates, 0.01 / ColsStates, 0.01 / RowsStates);
    for (const string& detName : TrigName) {
        bool isTDC = (detName.find("BD") != string::npos) || (detName.find("SI") != string::npos);
        vector<TH1*> hist_vec;
        vector<TH1*> hist_vec_log;
        vector<TString> hist_vec_opt;
        vector<TString> hist_vec_opt_log;
        // make a legend
        Double_t l_x0 = 0.75;
        Double_t l_y0 = 0.5;
        Double_t l_w = 0.15;
        Double_t l_h = 0.25;
        TLegend* legend_lin = new TLegend(l_x0, l_y0, l_x0 + l_w, l_y0 + l_h);
        legend_lin->SetTextSize(0.08);
        TLegend* legend_log = new TLegend(l_x0, l_y0, l_x0 + l_w, l_y0 + l_h);
        legend_log->SetTextSize(0.08);
        // run over trig states
        for (auto& state : TrigState) {
            TString hName(fTitle + "_" + detName + "_" + state.Name + "_Amp");
            TString hTitle(detName + "_" /* + state.Name + */ "_Amp");
            TH1* h = new TH1I(hName, hTitle, TRIG_AMP_BINS, 0, isTDC ? MaxAmpTDC : MaxAmpTQDC);
            legend_lin->AddEntry(h, state.Name);
            BmnHist::SetHistStyleTH1(h);
            h->GetXaxis()->SetTitle("Amplitude, ns");
            h->GetYaxis()->SetTitle("Activation Count");
            h->SetLineColor(state.Color);
            TH1* h_log = static_cast<TH1I*>(h->Clone(TString(h->GetName()) + "_log"));
            legend_log->AddEntry(h_log, state.Name);
            hist_vec.push_back(h);
            hist_vec_log.push_back(h_log);
        }
        // fill pad vectors
        PadInfo* pad_lin = new PadInfo(nullptr, move(hist_vec), move(hist_vec_opt), legend_lin);
        PadInfo* pad_log = new PadInfo(nullptr, move(hist_vec_log), move(hist_vec_opt_log), legend_log);
        padsTrigStates.push_back(pad_lin);
        padsTrigStates.push_back(pad_log);
        auto it = mapArrayNames.find(detName);
        if (it != mapArrayNames.end()) {
            mapTBLin.insert(make_pair(it->second, pad_lin));
            pad_log->logy = true;
            mapTBLog.insert(make_pair(it->second, pad_log));
        }
    }

    // Create correlation histograms
    for (auto& corrPair : TrigNameCorrTQDC) {
        string full_name_first = "TQDC_" + corrPair.first;
        string full_name_second = "TQDC_" + corrPair.second;
        uint32_t iFirst(0);
        uint32_t iSecond(0);
        auto itf = mapArrayNames.find(full_name_first);
        if (itf != mapArrayNames.end())
            iFirst = itf->second;
        else
            continue;
        auto its = mapArrayNames.find(full_name_second);
        if (its != mapArrayNames.end())
            iSecond = its->second;
        else
            continue;
        idxCorrTQDC.push_back(make_pair(iFirst, iSecond));
        TString hName(fTitle + "_" + corrPair.first + "_vs_" + corrPair.second + "_Amp");
        TH2I* h = new TH2I(hName, hName, TRIG_AMP_BINS, 0, MaxAmpTQDC_Corr, TRIG_AMP_BINS, 0, MaxAmpTQDC_Corr);
        BmnHist::SetHistStyleTH1(h);
        h->GetXaxis()->SetTitle((corrPair.first + " Amplitude").data());
        h->GetYaxis()->SetTitle((corrPair.second + " Amplitude").data());
        histCorrTQDC.push_back(h);
        PadInfo* pad = new PadInfo(h);
        pad->opt = "colz";
        padsCorrTQDC.push_back(pad);
    }
    canName = fTitle + "_Trig_States_2D";
    size_t Cols2D = 3;
    size_t corrLen = histCorrTQDC.size();
    size_t Rows2D = corrLen / Cols2D + ((corrLen % Cols2D) ? 1 : 0);
    canCorrTQDC = make_unique<TCanvas>(canName, canName, PAD_WIDTH * Cols2D, PAD_HEIGHT * Rows2D);
    canCorrTQDC->Divide(Cols2D, Rows2D, 0.01 / TrigBitsCnt, 0.01 / Rows2D);
    DBG("end")
}

void BmnHistTrigger::InitScalers()
{   // on rewriting
    //    printf("Init scalers: %s\n", fTitle.Data());
    //    MscMap& smap = fTrigMapper->GetScalerMap();
    //    const Int_t ColsStates = 2;
    //    const size_t RowsStates = ScalerNames.size();
    //    // canvas for 1D trig hists for different trig bits
    //    TString canName = fTitle + "_Scalers";
    //    canScalers = make_unique<TCanvas>(canName, canName, PAD_WIDTH * ColsStates, PAD_HEIGHT * RowsStates);
    //    canScalers->Divide(ColsStates, RowsStates, 0.01 / ColsStates, 0.01 / RowsStates);
    //    for (const string& trig_name : ScalerNames) {
    //        TString hName(fTitle + "_" + trig_name + "__profile");
    //        TString hTitle(trig_name + "__profile");
    //        TH1* hp = new TH1D(hName, hTitle, TRIG_TIME_BINS, 0.0, SCALER_TIME_WIN);
    //        BmnHist::SetHistStyleTH1(hp);
    //        hp->GetXaxis()->SetTitle("Time [s]");
    //        hp->GetYaxis()->SetTitle("Count");
    //        PadInfo* p_profile = new PadInfo(hp);
    //        padsScalers.push_back(p_profile);
    //        hName = fTitle + "_" + trig_name + "__spill";
    //        hTitle = trig_name + "__spill";
    //        TH1* h = new TProfile(hName, hTitle, TRIG_TIME_BINS, 0.0, SCALER_TIME_WIN);
    //        BmnHist::SetHistStyleTH1(h);
    //        h->GetXaxis()->SetTitle("Time [s]");
    //        h->GetYaxis()->SetTitle("Count");
    //        PadInfo* p = new PadInfo(h);
    //        padsScalers.push_back(p);
    //        auto it_sm = smap.find(trig_name);
    //        if (it_sm != smap.end()) {
    //            fill_scaler_vec_profile.push_back(make_pair(it_sm->second, p_profile));
    //            fill_scaler_vec_spill.push_back(make_pair(it_sm->second, p));
    //        }
    //    }
}

void BmnHistTrigger::InitGrids()
{
    const vector<Color_t> GridColors{kYellow + 2, kBlue, kRed, kGreen + 1, kCyan, kMagenta, kGray, kBlack};
    const Int_t GridColsBD = 4;
    const Int_t GroupSizeBD = 5;
    // Barrel
    TString canName = fTitle + "_BD_Grid";
    Int_t GridRowsBD = BD_CHANNELS / (GroupSizeBD * GridColsBD);
    canGridBD = make_unique<TCanvas>(canName, canName, PAD_WIDTH * GridColsBD, PAD_HEIGHT * GridRowsBD);
    canGridBD->Divide(GridColsBD, GridRowsBD, 0.01 / GridColsBD, 0.01 / GridRowsBD);
    Int_t iMod(0);
    for (Int_t iRow = 0; iRow < GridRowsBD; ++iRow) {
        for (Int_t iCol = 0; iCol < GridColsBD; ++iCol) {
            PadInfo* p = new PadInfo();
            //            p->opt = "nostat";
            for (Int_t iGr = 0; iGr < GroupSizeBD; ++iGr) {
                TString name = Form(fTitle + "_BD_Amp_mod_%d", iMod++);
                TH1F* h = new TH1F(name, name, TRIG_AMP_BINS_BD, 0, MaxAmpTDC_BD);
                h->GetXaxis()->SetTitle("Amplitude, ns");
                h->GetYaxis()->SetTitle("Activation Count");
                h->SetLineColor(GridColors[iGr]);
                BmnHist::SetHistStyleTH1(h);
                histsGridBD.push_back(h);
                p->aux.push_back(h);
            }
            padsGridBD.push_back(p);
        }
    }
    // Neutron Detector Trigger
    canName = fTitle + "_ND_Grid";
    Int_t GridRowsND = 4;
    vector<string> endnamesND{"F1", "F2", "S1", "S2", "V"};
    Int_t GridColsND = endnamesND.size();
    canGridND = make_unique<TCanvas>(canName, canName, PAD_WIDTH * GridColsND, PAD_HEIGHT * GridRowsND);
    canGridND->Divide(GridColsND, GridRowsND, 0.01 / GridColsND, 0.01 / GridRowsND);
    iMod = 0;
    for (Int_t iRow = 0; iRow < GridRowsND; ++iRow) {
        for (Int_t iCol = 0; iCol < GridColsND; ++iCol) {
            char* detName = Form("ND%s%d", endnamesND[iCol].data(), (iRow + 1));
            PadInfo* p = new PadInfo();
            string full_det_name(string("TQDC_") + detName);
            auto it = mapArrayNames.find(full_det_name);
            if (it != mapArrayNames.end()) {
                mapGridND.insert(make_pair(it->second, p));
            }
            p->opt = "nostat";
            TString name = Form(fTitle + "_%s_Amp", detName);
            {
                TH1F* h = new TH1F(name, name, TRIG_AMP_BINS, 0, MaxAmpTQDC);
                h->GetXaxis()->SetTitle("Amplitude");
                h->GetYaxis()->SetTitle("Activation Count");
                h->SetLineColor(kBlue);
                BmnHist::SetHistStyleTH1(h);
                p->current = h;
            }
            {
                name += "_match";
                TH1F* h = new TH1F(name, name, TRIG_AMP_BINS, 0, MaxAmpTQDC);
                h->GetXaxis()->SetTitle("Amplitude");
                h->GetYaxis()->SetTitle("Activation Count");
                h->SetLineColor(kMagenta);
                BmnHist::SetHistStyleTH1(h);
                p->aux.push_back(h);
            }
            padsGridND.push_back(p);
        }
    }
}

void BmnHistTrigger::InitHistsFromArr(vector<TClonesArray*>* trigAr)
{
    regex reBD("BD\\S+");
    regex reND("TQDC_ND\\S+");
    regex reQBC2("TQDC_BC2\\S+");
    regex reQBC1("TQDC_BC1");
    regex reQUnit("TQDC_UNIT_\\d+");
    regex reX10("TQDC_.+_X10.*");
    // const Int_t rows4Spectrum = 2;
    Int_t arLen = trigAr->size();
    TString name;
    fCols = LIN_LOG_COLS;
    fRows = 0;   // / fSrcCols;
    //    hists.resize(fRows);
    //    histsAux.resize(fRows);
    //    for (Int_t i = 0; i < hists.size(); i++) {
    //        hists[i].resize(fCols, nullptr);
    //        histsAux[i].resize(fCols, nullptr);
    //    }
    for (Int_t i = 0; i < arLen; ++i) {
        TClonesArray* ar = trigAr->at(i);
        TString arName(ar->GetName());
        if (regex_match(arName.Data(), reBD) || regex_match(arName.Data(), reND)) {

        } else {
            //            if (ar->GetClass() == BmnTrigWaveDigit::Class()) {
            //                regex re("TQDC_(.+)");
            //                TString pureName(regex_replace(ar->GetName(), re, "$1"));
            //                NamesAmp.push_back(pureName);
            //            } else
            //                NamesAmpTDC.push_back(arName);

            Int_t maxTime = (ar->GetClass() == BmnTrigWaveDigit::Class())
                                ? (regex_match(ar->GetName(), reQBC2)
                                       ? MaxAmpTQDC_BC2
                                       : (regex_match(ar->GetName(), reQUnit)
                                              ? MaxAmpTQDC_Unit
                                              : (regex_match(ar->GetName(), reQBC1) ? MaxAmpTQDC_Beam : MaxAmpTQDC)))
                                : (regex_match(ar->GetName(), reBD) ? MaxAmpTDC_BD : MaxAmpTDC);
            name =
                fTitle + "_" + ar->GetName() + ((ar->GetClass() == BmnTrigWaveDigit::Class()) ? "_Peak" : "_Amplitude");
            TH1F* h = new TH1F(name, name, 800, 0, maxTime);
            h->GetXaxis()->SetTitle("Amplitude" + (ar->GetClass() == BmnTrigWaveDigit::Class()) ? "" : ", ns");
            h->GetYaxis()->SetTitle("Activation Count");
            h->SetLineColor(kBlue);
            BmnHist::SetHistStyleTH1(h);
            PadInfo* pad_lin = new PadInfo(h);
            mapAmpLin.insert(make_pair(i, pad_lin));
            TH1F* h_log = static_cast<TH1F*>(h->Clone(TString(h->GetName()) + "_log"));
            PadInfo* pad_log = new PadInfo(h_log);
            pad_log->logy = true;
            mapAmpLog.insert(make_pair(i, pad_log));
            padsCanAmp.push_back(pad_lin);
            padsCanAmp.push_back(pad_log);
            //            NamesAmp.push_back(pad_lin->current->GetName());
            //            NamesAmp.push_back(pad_log->current->GetName());
            if (ar->GetClass() == BmnTrigWaveDigit::Class()) {
                name = fTitle + "_" + ar->GetName()
                       + ((ar->GetClass() == BmnTrigWaveDigit::Class()) ? "_Peak" : "_Amplitude") + "_filtered";
                TH1F* h_match = new TH1F(name, name, 800, 0, maxTime);
                h_match->GetXaxis()->SetTitle((ar->GetClass() == BmnTrigWaveDigit::Class()) ? "_Peak" : "_Amplitude");
                h_match->GetYaxis()->SetTitle("Activation Count");
                BmnHist::SetHistStyleTH1(h_match);
                h_match->SetLineColor(kMagenta);
                pad_lin->aux.push_back(h_match);
                pad_lin->auxOpt.push_back("");
                TH1F* h_match_log = static_cast<TH1F*>(h_match->Clone(TString(h_match->GetName()) + "_log"));
                pad_log->aux.push_back(h_match_log);
            }
            ++fRows;
        }
    }
    name = fTitle + "Canvas_Amplitudes";
    canAmp = new TCanvas(name, name, PAD_WIDTH * fCols, PAD_HEIGHT * fRows);
    canAmp->Divide(fCols, fRows, 0.01 / fCols, 0.01 / fRows);
    if (fServer) {
        TString path = "/" + fTitle + "/";
        fServer->Register(path, canAmp);
    }
}

void BmnHistTrigger::FillFromDigi(DigiArrays* fDigiArrays)
{
    const uint32_t timewin_begin = 312;
    const uint32_t timewin_end = 362;
    //    const double timewin_begin_ns = timewin_begin * ADC_CLOCK_TQDC16VS;
    //    const double timewin_end_ns = timewin_end * ADC_CLOCK_TQDC16VS;
    //    const vector<uint8_t> trc_map = {};
    BmnEventHeader* eHdr = fDigiArrays->header;
    //    if ((eHdr->GetEventType() == kBMNSTAT) || (eHdr->GetEventType() == kBMNEOS)) {
    //        FillMSC(fDigiArrays);
    //        return;
    //    }   // else
    // return; // temporary
    UInt_t bitsAR = eHdr->GetInputSignalsAR();
    // UInt_t bitsBR = eHdr->GetInputSignalsBR();
    vector<TClonesArray*>* trigAr = fDigiArrays->trigAr;
    Int_t bdCount = 0;
    if (!canInitDone) {
        InitHistsFromArr(trigAr);
        canInitDone = kTRUE;
        //        if (isShown) Register(fServer);
        //        SetDir(fDir);

        for (auto& el : padsCanAmp) {
            if (el)
                if (el->current)
                    el->current->SetDirectory(fDir);
        }
        for (auto& el : mapAmpLin) {
            if (el.second) {
                if (el.second->current)
                    el.second->current->SetDirectory(fDir);
                for (auto& aux_hist : el.second->aux)
                    aux_hist->SetDirectory(fDir);
            }
        }
        for (auto& el : mapAmpLog) {
            if (el.second) {
                if (el.second->current)
                    el.second->current->SetDirectory(fDir);
                for (auto& aux_hist : el.second->aux)
                    aux_hist->SetDirectory(fDir);
            }
        }
    }
    //    BDEvents->Clear();
    for (size_t iArr = 0; iArr < (*trigAr).size(); ++iArr) {
        TClonesArray* ar = (*trigAr)[iArr];
        //        const char* arName = ar->GetName();
        if (ar->GetClass() == BmnTrigDigit::Class()) {   // TDC data
            Bool_t isBarrel = (iArr == indexBD);         //! strcmp(arName, "BD");
            //            Bool_t isSiMD = (iArr == indexSI);           //! strcmp(arName, "SI");
            for (Int_t digIndex = 0; digIndex < ar->GetEntriesFast(); digIndex++) {
                BmnTrigDigit* td = static_cast<BmnTrigDigit*>(ar->At(digIndex));
                Short_t iMod = td->GetMod();
                // Double_t time = td->GetTime();
                Double_t amp = td->GetAmp();
                if (isBarrel) {
                    bdCount++;
                    histBDChannels->Fill(iMod);
                    histBDAmpByChannel->Fill(iMod, amp, 1);
                    histsGridBD[iMod]->Fill(amp);
                }
                {
                    auto it = mapTime.find(iArr);
                    if (it != mapTime.end()) {
                        TH1* h = it->second;
                        h->Fill(td->GetTime());
                    }
                }
                {
                    auto it = mapAmpLin.find(iArr);
                    if (it != mapAmpLin.end()) {
                        PadInfo* p = it->second;
                        p->current->Fill(td->GetAmp());
                    }
                }
                {
                    auto it = mapAmpLog.find(iArr);
                    if (it != mapAmpLog.end()) {
                        PadInfo* p = it->second;
                        p->current->Fill(td->GetAmp());
                    }
                }
                {
                    auto it = mapTBLin.find(iArr);
                    if (it != mapTBLin.end()) {
                        PadInfo* p = it->second;
                        for (size_t iState = 0; iState < trig_state_bit_index.size(); ++iState) {
                            if (bitsAR & BIT(trig_state_bit_index[iState]))
                                p->aux[iState]->Fill(td->GetAmp());
                        }
                    }
                }
                {
                    auto it = mapTBLog.find(iArr);
                    if (it != mapTBLog.end()) {
                        PadInfo* p = it->second;
                        for (size_t iState = 0; iState < trig_state_bit_index.size(); ++iState) {
                            if (bitsAR & BIT(trig_state_bit_index[iState]))
                                p->aux[iState]->Fill(td->GetAmp());
                        }
                    }
                }
            }
        }
        if (ar->GetClass() == BmnTrigWaveDigit::Class()) {   // TQDC ADC data
            for (Int_t digIndex = 0; digIndex < ar->GetEntriesFast(); digIndex++) {
                BmnTrigWaveDigit* td = static_cast<BmnTrigWaveDigit*>(ar->At(digIndex));
                storeAmpTQDC[iArr] = td->GetPeak();

                vector<double>& vec = td->TdcVector();
                //                vector<double> vec_in_win;
                //                std::copy_if(vec.begin(), vec.end(), std::back_inserter(vec_in_win), [=](double
                //                tdc_time) {
                //                    return ((tdc_time >= timewin_begin_ns) && (tdc_time <= timewin_end_ns));
                //                });

                double time = vec.size() ? vec[0] : -DBL_MAX;
                //                double time = vec_in_win.size() ? vec_in_win[0] : -DBL_MAX;
                //                printf("time    %f\n",time);
                {
                    auto it = mapGridND.find(iArr);
                    if (it != mapGridND.end()) {
                        PadInfo* p = it->second;
                        p->current->Fill(storeAmpTQDC[iArr]);
                        if (p->aux.size())
                            if (time > -900)
                                p->aux[0]->Fill(storeAmpTQDC[iArr]);
                    }
                }
                {
                    auto it = mapTime.find(iArr);
                    if (it != mapTime.end()) {
                        TH1* h = it->second;
                        h->Fill(time);
                    }
                }
                {
                    auto it = mapAmpLin.find(iArr);
                    if (it != mapAmpLin.end()) {
                        PadInfo* p = it->second;
                        p->current->Fill(storeAmpTQDC[iArr]);
                        if (p->aux.size())
                            if (time > -900)
                                p->aux[0]->Fill(storeAmpTQDC[iArr]);
                    }
                }
                {
                    auto it = mapAmpLog.find(iArr);
                    if (it != mapAmpLog.end()) {
                        PadInfo* p = it->second;
                        p->current->Fill(storeAmpTQDC[iArr]);
                        if (p->aux.size())
                            if (time > -900)
                                p->aux[0]->Fill(storeAmpTQDC[iArr]);
                    }
                }
                {
                    auto it = mapTBLin.find(iArr);
                    if (it != mapTBLin.end()) {
                        PadInfo* p = it->second;
                        for (size_t iState = 0; iState < trig_state_bit_index.size(); ++iState) {
                            if (bitsAR & BIT(trig_state_bit_index[iState]))
                                p->aux[iState]->Fill(storeAmpTQDC[iArr]);
                        }
                    }
                }
                {
                    auto it = mapTBLog.find(iArr);
                    if (it != mapTBLog.end()) {
                        PadInfo* p = it->second;
                        for (size_t iState = 0; iState < trig_state_bit_index.size(); ++iState) {
                            if (bitsAR & BIT(trig_state_bit_index[iState]))
                                p->aux[iState]->Fill(storeAmpTQDC[iArr]);
                        }
                    }
                }
            }
        }
    }
    // fill correlation histograms
    for (size_t iCorr = 0; iCorr < idxCorrTQDC.size(); ++iCorr) {
        auto& idxPair = idxCorrTQDC[iCorr];
        histCorrTQDC[iCorr]->Fill(storeAmpTQDC[idxPair.first], storeAmpTQDC[idxPair.second]);
    }
    if (bdCount)
        histBDSimult->Fill(bdCount);
}

void BmnHistTrigger::FillMSC(DigiArrays* fDigiArrays)
{
    BmnEventHeader* eHdr = fDigiArrays->header;
    if (fLastSpillId != eHdr->GetSpillId()) {
        fLastSpillId = eHdr->GetSpillId();
        fLastSpillStartTS = eHdr->GetSpillStartTS();
        for (size_t i = 0; i < ScalerNames.size(); ++i) {
            auto& el = fill_scaler_vec_spill[i];
            el.second->current->Reset();
            el.second->current->SetTitle(Form("%s__spill_%d", ScalerNames[i].data(), fLastSpillId));
        }
    }
    //    //    printf("ts :\t %10ld %9d     spill start\n", fLastSpillStartTS.GetSec(),
    //    fLastSpillStartTS.GetNanoSec()); TClonesArray* msc = fDigiArrays->msc; for (Int_t i = 0; i <
    //    msc->GetEntriesFast(); i++) {
    //        BmnMSCDigit* td = static_cast<BmnMSCDigit*> (msc->At(i));
    //        UInt_t* sc = td->GetValue();
    //        for (auto& el : fill_scaler_vec_profile) {
    //            uint16_t index = el.first;
    //            PadInfo * info = el.second;
    //            Double_t time_ms =  (td->GetTime().AsDouble() - fLastSpillStartTS.AsDouble()); // microseconds
    ////                        printf("time %f\n", time_ms);
    //            //                printf("ts :\t %10ld %9d  msc\n", td->GetTime().GetSec(),
    //            td->GetTime().GetNanoSec()); info->current->Fill(time_ms, sc[index]);
    //
    //        }
    //        for (auto& el : fill_scaler_vec_spill) {
    //            uint16_t index = el.first;
    //            PadInfo * info = el.second;
    //            Double_t time_ms =  (td->GetTime().AsDouble() - fLastSpillStartTS.AsDouble()); // microseconds
    //            //            printf("time %f\n", time_ms);
    //            //                printf("ts :\t %10ld %9d  msc\n", td->GetTime().GetSec(),
    //            td->GetTime().GetNanoSec()); info->current->Fill(time_ms, sc[index]);
    //
    //        }
    //    }
}

void BmnHistTrigger::SetBDChannel(Int_t iSelChannel)
{
    //    TString title;
    //    if (iSelChannel > (histBDSpecific->GetNbinsX() - 1)) {
    //        printf("Wrong channel!\n");
    //        return;
    //    }
    //    printf("Set channel: %d\n", fSelectedBDChannel);
    //    fSelectedBDChannel = iSelChannel;
    //    TString command;
    //    if (fSelectedBDChannel >= 0)
    //        command = Form("fMod == %d", fSelectedBDChannel);
    //    if (iSelChannel == -1)
    //        title = Form("BD for All Channels");
    //
    //    else
    //        title = "BD Time Length For: " + command;
    //    histBDSpecific->SetTitle(title);
    //    histBDSpecific->Reset();
    //    TString direction = "fTime>>" + TString(histBDSpecific->GetName());
    //    frecoTree->Draw(direction, command, "");
}

void BmnHistTrigger::Register(THttpServer* serv)
{
    isShown = kTRUE;
    fServer = serv;
    if (canTimes == nullptr)

        return;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canTimes);
    //    fServer->Register(path, can2d);
    fServer->Register(path, canGridBD.get());
    fServer->Register(path, canGridND.get());
    fServer->Register(path, canTrigStates.get());
    fServer->Register(path, canCorrTQDC.get());
    //    fServer->Register(path, canScalers.get());

    TString cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    TString cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmdTitle = path + "ChangeBDChannel";
    fServer->RegisterCommand(cmdTitle.Data(), "/" + fName + "/->SetBDChannel(%arg1%)", "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "hidden=guest");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), "/" + fName + "/->Reset()", "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
}

void BmnHistTrigger::SetDir(TFile* outFile = nullptr, TTree* recoTree = nullptr)
{
    frecoTree = recoTree;
    fDir = nullptr;
    if (outFile)
        fDir = outFile->mkdir(fTitle + "_hists");
    //        dir->cd();
    //    if (BDEvents)
    //        delete BDEvents;
    //    BDEvents = new TClonesArray("BmnTrigDigit");
    //    if (recoTree)
    //        recoTree->Branch(fTitle + "_BD", &BDEvents);
    if (!canTimes)

        return;
    SetDir(fDir);
}

void BmnHistTrigger::SetDir(TDirectory* Dir)
{
    fDir = Dir;
    //    for (auto &el : can2dPads) {
    //        if (el->current)
    //            el->current->SetDirectory(fDir);
    //    }
    for (auto& el : canTimesPads) {
        if (el->current)
            el->current->SetDirectory(fDir);
    }
    for (auto& el : padsGridBD) {
        if (el)
            if (el->current)
                el->current->SetDirectory(fDir);
    }
    for (auto& el : padsGridND) {
        if (el)
            if (el->current)
                el->current->SetDirectory(fDir);
    }
    for (auto& el : padsCanAmp) {
        if (el)
            if (el->current)
                el->current->SetDirectory(fDir);
    }
    for (auto& el : padsTrigStates) {
        if (el) {
            if (el->current)
                el->current->SetDirectory(fDir);
            for (auto& aux_hist : el->aux)
                aux_hist->SetDirectory(fDir);
        }
    }
    for (auto& el : padsCorrTQDC) {
        if (el)
            if (el->current)
                el->current->SetDirectory(fDir);
    }
    for (auto& el : padsScalers) {
        if (el)
            if (el->current)
                el->current->SetDirectory(fDir);
    }
    for (auto& el : mapGridND) {
        if (el.second) {
            if (el.second->current)
                el.second->current->SetDirectory(fDir);

            for (auto& aux_hist : el.second->aux)
                aux_hist->SetDirectory(fDir);
        }
    }
}

void BmnHistTrigger::Reset()
{
    //    for (auto &el : can2dPads)
    //        el->Reset();
    for (auto& el : canTimesPads)
        if (el)
            el->Reset();
    for (auto& el : canProfilePads)
        if (el)
            el->Reset();
    for (auto& el : padsCanAmp)
        el->Reset();
    for (auto& el : padsGridBD)
        el->Reset();
    for (auto& el : padsGridND)
        el->Reset();
    for (auto& el : padsTrigStates)
        el->Reset();
    for (auto& el : padsCorrTQDC)
        if (el)
            el->Reset();
    //    for (auto& el : padsScalers)
    //        el->Reset();
}

void BmnHistTrigger::DrawBoth()
{
    if (canTimes)
        BmnHist::DrawRef(canTimes, &canTimesPads);
    if (canGridBD)
        BmnHist::DrawRef(canGridBD, &padsGridBD);
    if (canGridND)
        BmnHist::DrawRef(canGridND, &padsGridND);
    if (canAmp)
        BmnHist::DrawRef(canAmp, &padsCanAmp);
    if (canTrigStates)
        BmnHist::DrawRef(canTrigStates, &padsTrigStates);
    if (canCorrTQDC)
        BmnHist::DrawRef(canCorrTQDC, &padsCorrTQDC);

    if (canScalers)
        BmnHist::DrawRef(canScalers, &padsScalers);
}

BmnStatus BmnHistTrigger::SetRefRun(Int_t id)
{
    if (refID != id) {

        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canTimesPads);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, padsCanAmp);
        //        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canPadsPeakTQDC);
        DrawBoth();
    }
    return kBMNSUCCESS;
}

void BmnHistTrigger::ClearRefRun()
{
    for (auto& pad : canTimesPads) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = nullptr;
    }
    for (auto& pad : padsCanAmp) {
        if (pad->ref)
            delete pad->ref;
        pad->ref = nullptr;
    }
    refID = 0;
}
