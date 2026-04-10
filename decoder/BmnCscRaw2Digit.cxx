#include "BmnCscRaw2Digit.h"

// Boost
#include <boost/program_options.hpp>
// BmnRoot
#include "BmnRawDataDecoder.h"

namespace po = boost::program_options;

BmnCscRaw2Digit::BmnCscRaw2Digit(Int_t period, Int_t run, TString MapFileName, BmnSetup bmnSetup)
    : BmnAdcProcessor(period, run, "CSC", ADC_N_CHANNELS, ADC32_N_SAMPLES)
    , fMapFileName(MapFileName)
{
    fSetup = bmnSetup;
    fCscStationSet = BmnCSCStationSet::Create(period, fSetup);
    InitNoiseArrays(fCscStationSet);

    ReadMapFile();
    GrabSerialsFromMap(fOuterMap);
    InitArrays();
    ReadLocalMaps();
    MatchMaps();
}

BmnCscRaw2Digit::~BmnCscRaw2Digit()
{
    DeleteNoiseArrays(fCscStationSet);
    for (auto& it : fOuterMap)
        for (auto& inner : it.second)
            delete inner.second;
}

BmnStatus BmnCscRaw2Digit::ReadMapFile()
{
    UInt_t ser = 0;
    Int_t ch_lo = 0;
    Int_t ch_hi = 0;
    Short_t chan_shift = 0;
    Short_t mod = 0;
    Short_t lay = 0;
    Short_t station = 0;
    string prefix;

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
        "SIGNAL_CONFIGURATION.Threshold", po::value<Double_t>(&final_thr)->default_value(30), "ADC final threshold")(
        "SIGNAL_CONFIGURATION.ThresholdDif", po::value<Double_t>(&thr_dif)->default_value(15), "ADC final threshold")(
        "SIGNAL_CONFIGURATION.NIterations", po::value<Int_t>(&n_iters)->default_value(5), "ADC final threshold")(
        "SIGNAL_CONFIGURATION.CModCut", po::value<Double_t>(&cmod_cut)->default_value(0), "ADC final threshold");

    // Load config file.
    po::variables_map vm;
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fMapFileName;
    LOG(info) << "name " << name;
    LOG(info) << "Loading CSC Map: Period " << fPeriod << ", Run " << fRun << " " << name;
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
        ss >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> chan_shift >> station >> mod >> lay >> prefix;
        BmnCscMapping* record = new BmnCscMapping();
        record->layer = lay;
        record->serial = ser;
        record->chan_shift = chan_shift;
        record->module = mod;
        record->channel_low = ch_lo / GetNSamples();
        record->channel_high = ch_hi / GetNSamples();
        record->station = station;
        record->prefix = prefix;
        // fill local maps map
        auto itLM = fLocalMapsMap.find(make_pair(prefix, mod));
        if (itLM == fLocalMapsMap.end()) {
            auto p = fLocalMapsMap.insert(make_pair(make_pair(prefix, mod), LocalCSC()));
            p.first->second.shift_map.insert(make_pair(make_pair(mod, lay), chan_shift));
        } else {
            LocalCSC& lcsc = itLM->second;
            lcsc.shift_map.insert(make_pair(make_pair(mod, lay), chan_shift));
        }

        // fill global map
        auto it = fOuterMap.find(ser);
        if (it == fOuterMap.end()) {   // create inner channel map for the serial
            InChanMapCSC inner;
            inner.insert(make_pair(record->channel_low - 1, nullptr));
            inner.insert(make_pair(record->channel_high, record));
            fOuterMap.insert(make_pair(ser, move(inner)));
        } else {   // add range to the existing inner channel map
            InChanMapCSC& inner = it->second;
            auto innerItHi = inner.find(record->channel_high);
            auto innerItLo = inner.find(record->channel_low - 1);
            if (innerItHi == inner.end()) {
                inner.insert(make_pair(record->channel_high, record));
            } else {
                if (innerItHi->second == nullptr) {
                    inner.erase(innerItHi);
                    inner.insert(make_pair(record->channel_high, record));
                } else {
                    delete record;
                    // fprintf(stderr, "Wrong %s map! Overlapping intervals for %08X!\n", fDetName.Data(), ser);
                    // return kBMNERROR;
                }
            }
            if (innerItLo == inner.end()) {
                inner.insert(make_pair(record->channel_low - 1, nullptr));
            }
        }
    }

    return kBMNSUCCESS;
}

BmnStatus BmnCscRaw2Digit::ReadLocalMap(string& FileName, Int_t* Layers, Int_t* Strips, Int_t& iLay, Int_t& ChanShift)
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
        Int_t chan2048 = chan + ChanShift;
        //        printf("\t\t strip %4d  chan %4d\n", iStrip, chan2048);
        Layers[chan2048] = iLay;
        Strips[chan2048] = iStrip++;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnCscRaw2Digit::ReadLocalMaps()
{
    for (auto& map_el : fLocalMapsMap) {
        const string& prefix = map_el.first.first;
        LocalCSC& info = map_el.second;
        fill_n(info.channel2layer, N_CSC_CHANNELS, -1);
        fill_n(info.channel2strip, N_CSC_CHANNELS, -1);
        for (auto& p : info.shift_map) {
            Int_t iMod = p.first.first;
            Int_t iLay = p.first.second;
            Int_t chan_shift = p.second;
            Int_t written_layer = ((iMod == 1) && (fPeriod < 8)) ? ((iLay + 2) % 4) : iLay;
            string map_file_name = string(getenv("VMCWORKDIR")) + "/input/" + string(fDetName.Data()) + prefix + "m"
                                   + to_string(iMod) + "l" + to_string(written_layer) + ".txt";
            //                            cout << "map " << map_file_name << endl;
            if (ReadLocalMap(map_file_name, info.channel2layer, info.channel2strip, iLay, chan_shift) == kBMNERROR) {
                printf("Error reading %s\n", map_file_name.data());
                return kBMNERROR;
            }
        }
    }
    return kBMNSUCCESS;
}

void BmnCscRaw2Digit::MatchMaps()
{
    for (auto& gl : fOuterMap) {
        //        LOGF(info, "serial %08X ", gl.first);
        for (auto& inner : gl.second) {
            BmnCscMapping*& m = inner.second;
            if (!m)
                continue;
            //        LOGF(info, "int from %d serial %08X  st %d  mod %d", inner.first, m->serial, m->station,
            //        m->module);
            auto lmIt = fLocalMapsMap.find(make_pair(m->prefix, m->module));
            if (lmIt == fLocalMapsMap.end())
                continue;
            m->local_map = &(lmIt->second);
        }
    }
}

BmnStatus BmnCscRaw2Digit::FillEvent(TClonesArray* adc, TClonesArray* digits)
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

BmnStatus BmnCscRaw2Digit::FillProfiles(TClonesArray* adc)
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

BmnStatus BmnCscRaw2Digit::FillNoisyChannels()
{
    const Int_t kNStripsInBunch = BmnRawDataDecoder::samplesCsc;
    // cout << "Csc: " << kNStripsInBunch << endl;
    //     const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const Int_t kNThresh = BmnRawDataDecoder::threshCsc;

    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            InChanMapCSC& inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnCscMapping* rec = innerIt->second;
            if (!rec)
                continue;
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                //                printf("%10s iCr %2d iCh%2d iSmpl %4d\n", fDetName.Data(), iCr,iCh,iSmpl);
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                    Int_t station = -1;
                    Int_t strip = -1;
                    Int_t layer = -1;
                    Int_t module = -1;
                    MapStrip(rec, iCh, iSmpl, station, module, layer, strip);
                    if (strip < 0)
                        continue;
                    fNoisyChannels[station][module][layer][strip] = kTRUE;
                }
            }
        }
    }
    for (Int_t iSt = 0; iSt < fCscStationSet->GetNStations(); ++iSt) {
        auto* st = fCscStationSet->GetStation(iSt);
        for (UInt_t iMod = 0; iMod < (UInt_t)st->GetNModules(); ++iMod) {
            auto* mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < mod->GetNStripLayers(); ++iLay) {
                auto& lay = mod->GetStripLayer(iLay);
                Int_t kNBunches = lay.GetNStrips() / kNStripsInBunch;
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                for (Int_t iBunch = 0; iBunch < kNBunches; ++iBunch) {
                    Double_t mean = 0.0;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch - 1; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE)
                            continue;
                        // Double_t curr = prof->GetBinContent(strip);
                        Double_t next = prof->GetBinContent(strip + 1);
                        mean += next;
                    }
                    mean /= kNStripsInBunch;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch - 1; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE)
                            continue;
                        // Double_t curr = prof->GetBinContent(strip);
                        Double_t next = prof->GetBinContent(strip + 1);
                        //                        if (kNThresh * meanDiff < next - curr)
                        if (kNThresh * mean < Abs(next - mean))
                            fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
                    }
                }
            }
        }
    }
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            InChanMapCSC& inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnCscMapping* rec = innerIt->second;
            if (!rec)
                continue;
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                Int_t station = -1;
                Int_t strip = -1;
                Int_t layer = -1;
                Int_t module = -1;
                MapStrip(rec, iCh, iSmpl, station, module, layer, strip);
                if (strip < 0)
                    continue;
                //        printf("s %2d    m %2d    l %2d    s %4d\n", station, module, layer, strip);
                //        printf("   iCr %2d    iCh %2d    iSmpl %4d\n", iCr,iCh, iSmpl);
                if (fNoisyChannels[station][module][layer][strip] == kTRUE)
                    GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
            }
        }
    }
    return kBMNSUCCESS;
}

inline void BmnCscRaw2Digit::MapStrip(BmnCscMapping* cscM,
                                      UInt_t iCh,
                                      Int_t iSmpl,
                                      Int_t& station,
                                      Int_t& module,
                                      Int_t& layer,
                                      Int_t& strip)
{
    station = cscM->station;
    module = cscM->module;
    Int_t ch2048 = iCh * GetNSamples() + iSmpl;
    //                    printf("s %2d m %2d ch2048 %4d\n", station ,module, ch2048);
    layer = cscM->local_map->channel2layer[ch2048];
    strip = cscM->local_map->channel2strip[ch2048];
    //    printf("s %2d    m %2d    l %2d    ch %4d    s %4d\n", station, module, layer, ch2048, strip);
    return;
}

void BmnCscRaw2Digit::ProcessAdc(TClonesArray* digs, Bool_t doFill)
{
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto innerIt = it->second.lower_bound(iCh);
            if (innerIt == it->second.end())
                continue;
            BmnCscMapping* rec = innerIt->second;
            if (!rec)
                continue;
            Double_t cs = fCMode[iCr][iCh] - fSMode[iCr][iCh];
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if ((GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) || (fAdc[iCr][iCh][iSmpl] == 0))
                    continue;
                // spec threshold
                Double_t SpecThr(0);
                auto itThr = fSpecThreshMap.find(make_pair(GetSerials()[iCr], iCh));
                if (itThr != fSpecThreshMap.end())
                    SpecThr = itThr->second;
                Int_t station = -1;
                Int_t strip = -1;
                Int_t layer = -1;
                Int_t module = -1;
                MapStrip(rec, iCh, iSmpl, station, module, layer, strip);
                //                    cout << "DECODER INFO: " << station << " " << module << " "
                //                    << layer << " " << strip << endl;
                //                                if ((station == 4) && (module==6))
                //                printf("%s st %2d mod %2d  l %2d strip %4d iCr %2d ser %08X ch %2d iSmpl %3d  adc
                //                %3.3f\n",
                //                        fDetName.Data(), station, module, layer, strip, iCr,
                //                        GetSerials()[iCr], iCh, iSmpl, fAdc[iCr][iCh][iSmpl]);
                if (strip < 0)
                    continue;
                Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + cs;
                sig = -sig;
                // Double_t Asig = TMath::Abs(sig);
                Double_t thr = Max(SpecThr, Max(FinalThr, 3.5 * GetPedestalsRMS()[iCr][iCh][iSmpl]));
                if ((!GetApplyThreshold()) || (sig > thr)) {
                    if (doFill) {
                        fSigProf[station][module][layer]->Fill(strip);
                    } else {
                        BmnCSCDigit* resDig = new ((*digs)[digs->GetEntriesFast()])
                            BmnCSCDigit(station, module, layer, strip, sig, GetPedestalsRMS()[iCr][iCh][iSmpl]);
                        resDig->SetIsGoodDigit(kTRUE);
                    }
                }
            }
        }
    }
}
