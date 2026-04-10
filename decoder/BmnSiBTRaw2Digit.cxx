// Boost
#include <boost/program_options.hpp>
// BmnRoot
#include "BmnSiBTRaw2Digit.h"

namespace po = boost::program_options;

BmnSiBTRaw2Digit::BmnSiBTRaw2Digit(Int_t period, Int_t run, TString MapFileName, BmnSetup bmnSetup)
    : BmnAdcProcessor(period, run, "SiBT", ADC_N_CHANNELS, ADC_SIBT_N_SAMPLES)
{
    fSetup = bmnSetup;

    fMapFileName = MapFileName;
    ReadMapFile();
    ReadLocalMaps();
    InitArrays();

    fSiBTStationSet = BmnSiBTStationSet::Create(period, fSetup);
    InitNoiseArrays(fSiBTStationSet);
}

BmnSiBTRaw2Digit::~BmnSiBTRaw2Digit()
{
    DeleteNoiseArrays(fSiBTStationSet);
    for (auto& it : fOuterMap)
        for (auto& inner : it.second)
            delete inner.second;
}

BmnStatus BmnSiBTRaw2Digit::ReadLocalMap(string& FileName, StripMap& Strips)
{
    ifstream inFile(FileName.data());
    if (!inFile.is_open()) {
        cout << "Error opening map-file (" << FileName << ")!" << endl;
        return kBMNERROR;
    }
    Int_t iStrip(0);
    Int_t chan(0);
    while (!inFile.eof()) {
        inFile >> chan;
        if (!inFile.good())
            break;
        Strips[chan] = iStrip++;
        //        printf("\t\t strip %4d  chan %4d\n", Strips[chan], chan);
    }
    return kBMNSUCCESS;
}

BmnStatus BmnSiBTRaw2Digit::ReadLocalMaps()
{
    for (auto& map_el : fGlobalMap) {
        BmnSiBTMapping& info = map_el.second;
        auto it = fLocalMaps.find(info.channel_name);
        if (it != fLocalMaps.end()) {
            info.strips = &(it->second);
        } else {
            string map_file_name =
                string(getenv("VMCWORKDIR")) + "/input/" + string(fDetName.Data()) + "_" + info.channel_name + ".txt";
            StripMap strips(fNSamples, 0);
            LOGF(info, "Reading: %s", map_file_name.data());
            ReadLocalMap(map_file_name, strips);
            fLocalMaps[info.channel_name] = move(strips);
            info.strips = &(fLocalMaps[info.channel_name]);
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnSiBTRaw2Digit::ReadMapFile()
{
    UInt_t ser = 0;
    Short_t ch_lo = 0;
    Int_t strip_shift = 0;
    Char_t ch_name;
    Short_t mod = 0;
    Short_t lay = 0;
    Short_t station = 0;
    string dummy;

    vector<string> channel_map;
    vector<string> spec_thr;
    Double_t final_thr = 0;
    Double_t thr_dif = 0;
    Int_t n_iters = 0;
    Double_t cmod_cut = 0;

    // Setup options.
    po::options_description desc("Options");
    desc.add_options()("CHANNEL_MAP.channels", po::value<vector<string>>(&channel_map)->multitoken(),
                       "ADC channel -> strip map")("SIGNAL_CONFIGURATION.spec_thr",
                                                   po::value<vector<string>>(&spec_thr)->multitoken(),
                                                   "ADC channel -> strip map")(
        "SIGNAL_CONFIGURATION.Threshold", po::value<Double_t>(&final_thr)->default_value(950), "ADC final threshold")(
        "SIGNAL_CONFIGURATION.ThresholdDif", po::value<Double_t>(&thr_dif)->default_value(50), "ADC final threshold")(
        "SIGNAL_CONFIGURATION.NIterations", po::value<Int_t>(&n_iters)->default_value(6), "ADC final threshold")(
        "SIGNAL_CONFIGURATION.CModCut", po::value<Double_t>(&cmod_cut)->default_value(0), "ADC final threshold");

    // Load config file.
    po::variables_map vm;
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fMapFileName;
    LOG(info) << "Loading SiBT Map: Period " << fPeriod << ", Run " << fRun << " " << name;
    ifstream config_file(name.Data(), ifstream::in);
    if (!config_file.is_open()) {
        LOG(error) << "Error opening map-file (" << name << ")!";
        return kBMNERROR;
    }
    po::store(po::parse_config_file(config_file, desc), vm);
    config_file.close();
    po::notify(vm);

    // set thresholds
    SetThreshold(final_thr, thr_dif, n_iters, cmod_cut);

    // set specific thresholds
    Int_t ch = 0;
    Double_t thr = 0;
    for (auto& str : spec_thr) {
        istringstream ss(str);
        ss >> std::hex >> ser >> std::dec >> ch >> thr;
        fSpecThreshMap.insert(make_pair(make_pair(ser, ch), thr));
        LOGF(debug, "Serial  0x%08X Channel %2d  Threshold %3.3f", ser, ch, thr);
    }

    // set channel map
    for (auto& str : channel_map) {
        istringstream ss(str);
        ss >> std::hex >> ser >> std::dec >> ch_lo >> strip_shift >> ch_name >> mod >> lay >> station;
        fGlobalMap.insert(make_pair(make_pair(ser, ch_lo), BmnSiBTMapping{.layer = lay,
                                                                          .serial = ser,
                                                                          .module = mod,
                                                                          .channel = ch_lo,
                                                                          .strip_shift = strip_shift,
                                                                          .channel_name = ch_name,
                                                                          .station = station,
                                                                          .strips = nullptr}));
        GrabNewSerial(ser);
    }
    return kBMNSUCCESS;
}

BmnStatus BmnSiBTRaw2Digit::FillEvent(TClonesArray* adc, TClonesArray* digits)
{
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    ProcessAdc(digits, kFALSE);
    return kBMNSUCCESS;
}

BmnStatus BmnSiBTRaw2Digit::FillProfiles(TClonesArray* adc)
{
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    ProcessAdc(nullptr, kTRUE);

    return kBMNSUCCESS;
}

BmnStatus BmnSiBTRaw2Digit::FillNoisyChannels()
{
    const Int_t kNStripsInBunch = GetNSamples();
    //    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    Int_t kNThresh = 3;
    // repeat noisy channels in the physical terms (station/module/layer)
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto it = fGlobalMap.find(make_pair(GetSerials()[iCr], iCh));
            if (it == fGlobalMap.end())
                continue;

            //            InChanMapSiBT & inner = it->second;
            //            auto innerIt = inner.lower_bound(iCh);
            //            if (innerIt == inner.end())
            //                continue;
            BmnSiBTMapping& rec = it->second;
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                    Int_t iStrip = MapStrip(rec, iSmpl);
                    fNoisyChannels[rec.station][rec.module][rec.layer][iStrip] = kTRUE;
                }
            }
        }
    }
    // mark noisy
    for (Int_t iSt = 0; iSt < fSiBTStationSet->GetNStations(); ++iSt) {
        auto* st = fSiBTStationSet->GetStation(iSt);
        for (Int_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto* mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < mod->GetNStripLayers(); ++iLay) {
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                //                        printf("SigProf %s_%d_%d_%d\n", fDetName.Data(), iSt, iMod, iLay);
                auto& lay = mod->GetStripLayer(iLay);
                Int_t kNBunches = lay.GetNStrips() / kNStripsInBunch;
                for (Int_t iBunch = 0; iBunch < kNBunches; ++iBunch) {
                    Double_t mean = 0.0;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE)
                            continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        // Double_t next = prof->GetBinContent(strip);
                        mean += curr;
                    }
                    mean /= kNStripsInBunch;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE)
                            continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        // Double_t next = prof->GetBinContent(strip);
                        //                         if (kNThresh * meanDiff < next - curr)
                        if ((kNThresh * Abs(mean) < Abs(curr - mean)) /* || (kNThresh * meanDiff < -next + curr)*/) {
                            LOGF(debug2, "profile noise on iSt %d iMod %d iLay %d strip %d", iSt, iMod, iLay, strip);
                            fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
                        }
                    }
                }
            }
        }
    }
    // repeat noisy channels back into the electronics terms
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto it = fGlobalMap.find(make_pair(GetSerials()[iCr], iCh));
            if (it == fGlobalMap.end())
                continue;
            BmnSiBTMapping& rec = it->second;
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                Int_t iStrip = MapStrip(rec, iSmpl);
                if (fNoisyChannels[rec.station][rec.module][rec.layer][iStrip] == kTRUE)
                    GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
            }
        }
    }
    return kBMNSUCCESS;
}

void BmnSiBTRaw2Digit::ProcessAdc(TClonesArray* digs, Bool_t doFill)
{
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto it = fGlobalMap.find(make_pair(GetSerials()[iCr], iCh));
            if (it == fGlobalMap.end())
                continue;
            // spec threshold
            Double_t SpecThr(0);
            auto itThr = fSpecThreshMap.find(make_pair(GetSerials()[iCr], iCh));
            if (itThr != fSpecThreshMap.end())
                SpecThr = itThr->second;
            BmnSiBTMapping& rec = it->second;
            Short_t station = rec.station;
            Short_t module = rec.module;
            Short_t layer = rec.layer;
            Double_t cs = fCMode[iCr][iCh] - fSMode[iCr][iCh];
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if ((GetNoisyChipChannels()[iCr][iCh][iSmpl]
                     == kTRUE))   ///* || (fPedVal[iCr][iCh][iSmpl] == 0.0)*/) continue;
                    LOGF(debug2, "noisy %4d %4d %4d", iCr, iCh, iSmpl);
                Int_t strip = MapStrip(rec, iSmpl);
                //                printf("%c layer %d iCh %3d iSmpl %2d, iStrip %2d\n", rec.channel_name, rec.layer,
                //                iCh, iSmpl, strip);
                Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + cs;
                if (layer == 1)
                    sig = -sig;
                // Double_t Asig = TMath::Abs(sig);
                Double_t thr = (SpecThr > 0) ? SpecThr : FinalThr;
                thr = Max(thr, 3.5 * GetPedestalsRMS()[iCr][iCh][iSmpl]);
                if ((!GetApplyThreshold()) || (sig > thr)) {
                    //                    printf("st %d mod %d l %d strip %d ped %2.3f sig %2.3f thr %2.3f PedestalsRMS
                    //                    %2.3f\n",
                    //                            station, module, layer, strip, fPedVal[iCr][iCh][iSmpl], sig, thr,
                    //                            GetPedestalsRMS()[iCr][iCh][iSmpl]);
                    if (doFill) {
                        fSigProf[station][module][layer]->Fill(strip);
                    } else {
                        BmnSiBTDigit* resDig = new ((*digs)[digs->GetEntriesFast()])
                            BmnSiBTDigit(station, module, layer, strip, sig, GetPedestalsRMS()[iCr][iCh][iSmpl]);
                        resDig->SetIsGoodDigit(kTRUE);
                    }
                }
            }
        }
    }
}
