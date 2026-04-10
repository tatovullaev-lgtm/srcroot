#include "BmnSiliconRaw2Digit.h"

// Boost
#include <boost/program_options.hpp>
// BmnRoot
#include "BmnRawDataDecoder.h"

namespace po = boost::program_options;

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit(Int_t period, Int_t run, TString MapFileName, BmnSetup bmnSetup)
    : BmnAdcProcessor(period, run, "SILICON", ADC_N_CHANNELS, ADC128_N_SAMPLES)
    , fMapFileName(MapFileName)
{
    fSetup = bmnSetup;
    LOG(info) << "Loading SILICON Map from FILE: Period " << period << ", Run " << run << "...";
    ReadMapFile();
    GrabSerialsFromMap(fOuterMap);
    InitArrays();

    fSilStationSet = BmnSiliconStationSet::Create(period, fSetup);

    Int_t kNStations = fSilStationSet->GetNStations();
    fSigProf = new TH1F***[kNStations];
    fNoisyChannels = new Bool_t***[kNStations];
    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        auto* st = fSilStationSet->GetStation(iSt);
        Int_t kNModules = st->GetNModules();
        fSigProf[iSt] = new TH1F**[kNModules];
        fNoisyChannels[iSt] = new Bool_t**[kNModules];
        for (Int_t iMod = 0; iMod < kNModules; ++iMod) {
            auto* mod = st->GetModule(iMod);
            Int_t kNLayers = 2;   // mod->GetNStripLayers();
            fSigProf[iSt][iMod] = new TH1F*[kNLayers];
            fNoisyChannels[iSt][iMod] = new Bool_t*[kNLayers];
            for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
                BmnSiliconLayer& lay = mod->GetStripLayer(iLay);
                if (iSt != 0) {
                    BmnSiliconLayer& lay1 = mod->GetStripLayer(iLay * 2);
                    BmnSiliconLayer& lay2 = mod->GetStripLayer(iLay * 2 + 1);
                    if (lay1.GetLastStripNumber() > lay2.GetLastStripNumber())
                        lay = lay1;
                    else
                        lay = lay2;
                    //                LOGF(info, "Station: %d,   Module: %2d,   PseudoLayer: %2d,   Strips: %3d", iSt,
                    //                iMod, iLay, lay.GetLastStripNumber() + 1); LOGF(info, "\tLay 1 FirstStripNumber:
                    //                %3d,   LastStripNumber: %3d", lay1.GetFirstStripNumber(),
                    //                lay1.GetLastStripNumber()); LOGF(info, "\tLay 2 FirstStripNumber: %3d,
                    //                LastStripNumber: %3d", lay2.GetFirstStripNumber(), lay2.GetLastStripNumber());
                }
                LOGF(debug, "Station: %d,   Module: %2d,   PseudoLayer: %2d,   Strips: %3d", iSt, iMod, iLay,
                     lay.GetLastStripNumber() + 1);
                // printf("FirstStripNumber: %3d,   LastStripNumber: %3d\n", lay.GetFirstStripNumber(),
                // lay.GetLastStripNumber());
                Int_t kNStrips = lay.GetLastStripNumber() + 1;
                TString histName;
                histName.Form("%s_%d_%d_%d", fDetName.Data(), iSt, iMod, iLay);
                fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, kNStrips, 0, kNStrips);
                fSigProf[iSt][iMod][iLay]->SetDirectory(0);
                fNoisyChannels[iSt][iMod][iLay] = new Bool_t[kNStrips + 30]();
                //                for (Int_t iStrip = 0; iStrip < kNStrips; ++iStrip){
                //                LOGF(info, "fNoisyChannels  Station: %d,   Module: %2d,   PseudoLayer: %2d iStrip: %d
                //                is %d",
                //                        iSt,iMod,iLay, iStrip, fNoisyChannels[iSt][iMod][iLay][iStrip]);
                //                    fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
                //                }
            }
        }
    }
}

BmnSiliconRaw2Digit::~BmnSiliconRaw2Digit()
{
    Int_t kNStations = fSilStationSet->GetNStations();
    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        auto* st = fSilStationSet->GetStation(iSt);
        Int_t kNModules = st->GetNModules();
        for (Int_t iMod = 0; iMod < kNModules; ++iMod) {
            // auto *mod = st->GetModule(iMod);
            Int_t kNLayers = 2;   // mod->GetNStripLayers();
            for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
                delete fSigProf[iSt][iMod][iLay];
                delete[] fNoisyChannels[iSt][iMod][iLay];
            }
            delete[] fSigProf[iSt][iMod];
            delete[] fNoisyChannels[iSt][iMod];
        }
        delete[] fSigProf[iSt];
        delete[] fNoisyChannels[iSt];
    }
    delete[] fNoisyChannels;
    delete[] fSigProf;
    if (canStrip)
        delete canStrip;
    for (auto& it : fOuterMap)
        for (auto& inner : it.second)
            delete inner.second;
}

BmnStatus BmnSiliconRaw2Digit::ReadMapFile()
{
    UInt_t ser = 0;
    Int_t ch_lo = 0;
    Int_t ch_hi = 0;
    Int_t mod_adc = 0;
    Int_t mod = 0;
    Int_t lay = 0;
    Int_t station = 0;

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
        "SIGNAL_CONFIGURATION.Threshold", po::value<Double_t>(&final_thr)->default_value(150), "ADC final threshold")(
        "SIGNAL_CONFIGURATION.ThresholdDif", po::value<Double_t>(&thr_dif)->default_value(100), "ADC threshold step")(
        "SIGNAL_CONFIGURATION.NIterations", po::value<Int_t>(&n_iters)->default_value(6), "ADC filter iterations")(
        "SIGNAL_CONFIGURATION.CModCut", po::value<Double_t>(&cmod_cut)->default_value(100), "ADC CModCut");

    // Load config file.
    po::variables_map vm;
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fMapFileName;
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
        ss >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> mod_adc >> mod >> lay >> station;
        BmnSiliconMapping* record = new BmnSiliconMapping();
        record->layer = lay;
        record->serial = ser;
        record->module = mod;
        if (ch_lo < ch_hi) {
            record->channel_low = ch_lo;
            record->channel_high = ch_hi;
        } else {
            record->channel_low = ch_hi;
            record->channel_high = ch_lo;
            record->inverted = true;
        }
        record->station = station;
        fMap.push_back(record);
        auto it = fOuterMap.find(ser);
        if (it == fOuterMap.end()) {   // create inner channel map for the serial
            InChanMapSil inner;
            inner.insert(make_pair(record->channel_low - 1, nullptr));
            inner.insert(make_pair(record->channel_high, record));
            fOuterMap.insert(make_pair(ser, move(inner)));
        } else {   // add range to the existing inner channel map
            InChanMapSil& inner = it->second;
            auto innerItHi = inner.find(record->channel_high);
            auto innerItLo = inner.find(record->channel_low - 1);
            if (innerItHi == inner.end()) {
                inner.insert(make_pair(record->channel_high, record));
            } else {
                if (innerItHi->second == nullptr) {
                    inner.erase(innerItHi);
                    inner.insert(make_pair(record->channel_high, record));
                } else {
                    fprintf(stderr, "Wrong %s map! Overlapping intervals for %08X!\n", fDetName.Data(), ser);
                    return kBMNERROR;
                }
            }
            if (innerItLo == inner.end()) {
                inner.insert(make_pair(record->channel_low - 1, nullptr));
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnSiliconRaw2Digit::FillEvent(TClonesArray* adc, TClonesArray* digits)
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

BmnStatus BmnSiliconRaw2Digit::FillProfiles(TClonesArray* adc)
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

BmnStatus BmnSiliconRaw2Digit::FillNoisyChannels()
{
    const Int_t kNStripsInBunch = BmnRawDataDecoder::samplesSil;   // GetNSamples();
    //    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    Double_t kNThresh = BmnRawDataDecoder::threshSil;
    Double_t correction = BmnRawDataDecoder::correctionSil;
    LOGF(info, "Searching Noisy Channels in Silicon Strip Detectors");
    LOGF(info, "Window size: %3d,   Threshold: %.2f,   Correction: %3d", kNStripsInBunch, kNThresh,
         (int)round(correction));
    // repeat noisy channels in the physical terms (station/module/layer)
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            InChanMapSil& inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnSiliconMapping* rec = innerIt->second;
            if (!rec)
                continue;
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                    Int_t iStrip = MapStrip(rec, iCh, iSmpl);
                    //
                    //        auto * st = fSilStationSet->GetStation(rec->station);
                    //            auto *mod = st->GetModule(rec->module);
                    //                BmnSiliconLayer& lay = mod->GetStripLayer(rec->layer);
                    //                if(rec->station != 0) {
                    //                    BmnSiliconLayer& lay1 = mod->GetStripLayer(rec->layer * 2);
                    //                    BmnSiliconLayer& lay2 = mod->GetStripLayer(rec->layer * 2 + 1);
                    //                    if(lay1.GetLastStripNumber() > lay2.GetLastStripNumber()) lay = lay1;
                    //                    else lay = lay2;
                    //                }
                    //
                    //                        if (iStrip >= lay.GetLastStripNumber() || iStrip < 0)
                    //                LOGF(info, "fNoisyChannels  Station: %d,   Module: %2d,   PseudoLayer: %2d iStrip:
                    //                %d set true",
                    //                        rec->station, rec->module, rec->layer, iStrip);
                    fNoisyChannels[rec->station][rec->module][rec->layer][iStrip] = kTRUE;
                }
            }
        }
    }
    int newNoisyCnt = 0;
    //    LOGF(info, "mark noisy");
    // mark noisy
    for (Int_t iSt = 0; iSt < fSilStationSet->GetNStations(); ++iSt) {
        auto* st = fSilStationSet->GetStation(iSt);
        for (Int_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto* mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < 2 /*mod->GetNStripLayers()*/; ++iLay) {
                //                LOGF(info, "fNoisyChannels  Station: %d,   Module: %2d,   PseudoLayer: %2d iStrip: %d
                //                set true",
                //                        rec->station, rec->module, rec->layer, iStrip);
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                /** Big module has 2 separate parts of each layer*/
                // Int_t realILayer = (mod->GetNStripLayers() == 4) ? 2 * iLay : iLay;
                BmnSiliconLayer& lay = mod->GetStripLayer(iLay);
                if (iSt != 0) {
                    BmnSiliconLayer& lay1 = mod->GetStripLayer(iLay * 2);
                    BmnSiliconLayer& lay2 = mod->GetStripLayer(iLay * 2 + 1);
                    if (lay1.GetLastStripNumber() > lay2.GetLastStripNumber())
                        lay = lay1;
                    else
                        lay = lay2;
                }
                Double_t sum;
                Int_t properStrips;
                bool newNoisyChannels = true;
                while (newNoisyChannels) {
                    sum = 0.0;
                    properStrips = 0;
                    newNoisyChannels = false;
                    // calculating initial window
                    vector<int> noisyChannels;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip;   // + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE)
                            continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        sum += curr;
                        if (curr > 0)   // current channel might be broken
                            properStrips++;
                    }
                    //    LOGF(info, "checking noisy channels");
                    // marking noisy channels
                    for (Int_t strip = 0; strip < lay.GetLastStripNumber() + 1; strip++) {
                        // checking if this channel is noisy
                        //                                    LOGF(info, "check noise on iSt %d iMod %d iLay %d strip
                        //                                    %d",
                        //                                            iSt, iMod, iLay, strip);
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kFALSE) {
                            Double_t curr = prof->GetBinContent(strip + 1);
                            if (curr > 0 && properStrips > 0) {
                                if (properStrips == 1
                                    || (kNThresh * (sum - curr) / (properStrips - 1) < curr - correction))
                                {
                                    LOGF(debug2, "profile noise on iSt %d iMod %d iLay %d strip %d", iSt, iMod, iLay,
                                         strip);
                                    newNoisyCnt++;
                                    newNoisyChannels = true;
                                    noisyChannels.push_back(strip);
                                }
                            }
                        }
                        // moving the current frame for calculating mean
                        if (strip >= (kNStripsInBunch - 1) / 2
                            && lay.GetLastStripNumber() + 1 - strip > (kNStripsInBunch / 2) + 1)
                        {
                            if ((fNoisyChannels[iSt][iMod][iLay][strip - (kNStripsInBunch - 1) / 2] == kFALSE)) {
                                sum -= prof->GetBinContent(strip - (kNStripsInBunch - 1) / 2
                                                           + 1);   // first element in window
                                if (prof->GetBinContent(strip - (kNStripsInBunch - 1) / 2 + 1) > 0)
                                    properStrips--;
                            }
                            //                                    LOGF(info, "check noise on iSt %d iMod %d iLay %d
                            //                                    strip %d  left bound",
                            //                                            iSt, iMod, iLay, strip);
                            if ((fNoisyChannels[iSt][iMod][iLay][strip + (kNStripsInBunch / 2) + 1] == kFALSE)) {
                                sum += prof->GetBinContent(strip + (kNStripsInBunch / 2) + 1
                                                           + 1);   // element right after the window
                                if (prof->GetBinContent(strip + (kNStripsInBunch / 2) + 1 + 1) > 0)
                                    properStrips++;
                            }
                        }
                    }
                    //    LOGF(info, "marking noisy channels");
                    // marking noisy channels
                    for (auto strip : noisyChannels) {
                        fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
                        for (auto& it : fMap)
                            if (it->station == iSt && it->module == iMod && it->layer == iLay) {
                                UInt_t iCr = 0;
                                for (iCr = 0; iCr < GetSerials().size(); iCr++) {
                                    if (GetSerials()[iCr] == it->serial)
                                        break;
                                }
                                UInt_t iCh = it->inverted ? (it->channel_high - strip / GetNSamples())
                                                          : (it->channel_low + strip / GetNSamples());
                                UInt_t iSmpl = (strip) % GetNSamples();
                                GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
                            }
                    }
                }
            }
        }
    }
    //    LOGF(info, "Marked %d new channels as noisy on Silicon Strip Detectors", newNoisyCnt);
    // repeat noisy channels back into the electronics terms
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            InChanMapSil& inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnSiliconMapping* rec = innerIt->second;
            if (!rec)
                continue;
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                Int_t iStrip = MapStrip(rec, iCh, iSmpl);
                if (fNoisyChannels[rec->station][rec->module][rec->layer][iStrip] == kTRUE)
                    GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
            }
        }
    }
    //    LOGF(info, "Finished marking on Silicon Strip Detectors");
    return kBMNSUCCESS;
}

void BmnSiliconRaw2Digit::ProcessAdc(TClonesArray* digs, Bool_t doFill)
{
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto innerIt = it->second.lower_bound(iCh);
            if (innerIt == it->second.end())
                continue;
            BmnSiliconMapping* rec = innerIt->second;
            if (!rec)
                continue;
            // spec threshold
            Double_t SpecThr(0);
            auto itThr = fSpecThreshMap.find(make_pair(GetSerials()[iCr], iCh));
            if (itThr != fSpecThreshMap.end())
                SpecThr = itThr->second;
            // strip map
            Short_t station = rec->station;
            Short_t module = rec->module;
            Short_t layer = rec->layer;
            //            printf("st %d mod %d l %d\n", station, module ,layer);
            Double_t cs = fCMode[iCr][iCh] - fSMode[iCr][iCh];
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if ((GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) /* || (fPedVal[iCr][iCh][iSmpl] == 0.0)*/)
                    continue;
                Int_t strip = MapStrip(rec, iCh, iSmpl);
                if (strip < 0)
                    continue;
                Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + cs;
                // LOGF(debug2,"%s st %2d mod %2d l %2d strip %4d  ser %08X ch %2d iSmpl %3d pedestal
                // %4.2f CMode %4.2f SMode %4.2f GetPedestalsRMS() %4.2f sig %4.2f\n",
                // fDetName.Data(), station, module, layer, strip, GetSerials()[iCr], iCh, iSmpl,
                // fPedVal[iCr][iCh][iSmpl], fCMode[iCr][iCh], fSMode[iCr][iCh],
                // GetPedestalsRMS()[iCr][iCh][iSmpl], sig);
                if (layer == 1)
                    sig = -sig;
                // Double_t Asig = TMath::Abs(sig);
                //                Double_t thr = Max(SpecThr, Max(FinalThr, 3.5 * GetPedestalsRMS()[iCr][iCh][iSmpl]));
                //                // default
                Double_t thr = Max(SpecThr, FinalThr);
                if ((!GetApplyThreshold()) || (sig > thr)) {
                    if (doFill) {
                        if (Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) < cmodcut)
                            fSigProf[station][module][layer]->Fill(strip);
                    }
                    if (digs) {
                        // if ((fVerbose > 0) && (GetSerials()[iCr] == 0x0AADF52A) && (!doFill))
                        // LOGF(debug2, "%s st %2d mod %2d l %2d strip %4d  ser %08X ch %2d iSmpl"
                        // " %3d pedestal %4.2f CMode %4.2f SMode %4.2f GetPedestalsRMS() %4.2f" "sig %4.2f\n",
                        // fDetName.Data(), station, module, layer, strip, GetSerials()[iCr], iCh, iSmpl,
                        // fPedVal[iCr][iCh][iSmpl], fCMode[iCr][iCh],
                        // fSMode[iCr][iCh], GetPedestalsRMS()[iCr][iCh][iSmpl], sig);
                        BmnSiliconDigit* resDig = new ((*digs)[digs->GetEntriesFast()])
                            BmnSiliconDigit(station, module, layer, strip, sig, GetPedestalsRMS()[iCr][iCh][iSmpl]);
                        if ((Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) > cmodcut))
                            resDig->SetIsGoodDigit(kFALSE);
                        else
                            resDig->SetIsGoodDigit(kTRUE);
                    }
                }
            }
        }
    }
}
