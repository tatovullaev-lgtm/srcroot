#include "BmnGemRaw2Digit.h"

// Boost
#include <boost/program_options.hpp>
// BmnRoot
// #include <BmnEventHeader.h>
#include <BmnRawDataDecoder.h>
#include <UniDetectorParameter.h>

namespace po = boost::program_options;

BmnGemRaw2Digit::BmnGemRaw2Digit(Int_t period, Int_t run, TString mapFileName, BmnSetup bmnSetup)
    : BmnAdcProcessor(period, run, "GEM", ADC_N_CHANNELS, ADC32_N_SAMPLES)
    , fSmall(nullptr)
    , fMid(nullptr)
    , fMapFileName(mapFileName)
{
    fSetup = bmnSetup;
    ReadGlobalMap(fMapFileName);
    InitArrays();

    fSmall = new BmnGemMap[N_CH_BUF];
    fMid = new BmnGemMap[N_CH_BUF];
    Byte_t nMods = 4;
    for (Byte_t i = 0; i < nMods; i++) {
        fBigHot.push_back(new BmnGemMap[N_CH_BUF]);
        fBig.push_back(new BmnGemMap[N_CH_BUF]);
    }

    if ((fPeriod >= 8) && (fSetup != kSRCSETUP)) {
        ReadLocalMap("X0_Bottom_Left.txt", fBigHot[2], 2, 2);
        ReadLocalMap("X0_Bottom_Right.txt", fBigHot[3], 2, 3);
        ReadLocalMap("X_Bottom_Left.txt", fBig[2], 0, 2);
        ReadLocalMap("X_Bottom_Right.txt", fBig[3], 0, 3);
        ReadLocalMap("Y0_Bottom_Left.txt", fBigHot[2], 3, 2);
        ReadLocalMap("Y0_Bottom_Right.txt", fBigHot[3], 3, 3);
        ReadLocalMap("Y_Bottom_Left.txt", fBig[2], 1, 2);
        ReadLocalMap("Y_Bottom_Right.txt", fBig[3], 1, 3);

        ReadLocalMap("X0_Top_Left.txt", fBigHot[0], 2, 0);
        ReadLocalMap("X0_Top_Right.txt", fBigHot[1], 2, 1);
        ReadLocalMap("X_Top_Left.txt", fBig[0], 0, 0);
        ReadLocalMap("X_Top_Right.txt", fBig[1], 0, 1);
        ReadLocalMap("Y0_Top_Left.txt", fBigHot[0], 3, 0);
        ReadLocalMap("Y0_Top_Right.txt", fBigHot[1], 3, 1);
        ReadLocalMap("Y_Top_Left.txt", fBig[0], 1, 0);
        ReadLocalMap("Y_Top_Right.txt", fBig[1], 1, 1);
    } else {
        ReadMap("GEM_X0_middle", fMid, 2, 0);
        ReadMap("GEM_Y0_middle", fMid, 3, 0);
        ReadMap("GEM_X1_middle", fMid, 0, 0);
        ReadMap("GEM_Y1_middle", fMid, 1, 0);

        ReadMap("GEM_X0_Big_Left", fBigHot[1], 2, 1);
        ReadMap("GEM_Y0_Big_Left", fBigHot[1], 3, 1);
        ReadMap("GEM_X1_Big_Left", fBig[1], 0, 1);
        ReadMap("GEM_Y1_Big_Left", fBig[1], 1, 1);
        ReadMap("GEM_X0_Big_Right", fBigHot[0], 2, 0);
        ReadMap("GEM_Y0_Big_Right", fBigHot[0], 3, 0);
        ReadMap("GEM_X1_Big_Right", fBig[0], 0, 0);
        ReadMap("GEM_Y1_Big_Right", fBig[0], 1, 0);
    }
    ReadLocalMap("GEM_X_small.txt", fSmall, 0, 0);
    ReadLocalMap("GEM_Y_small.txt", fSmall, 1, 0);

    fGemStationSet = BmnGemStripStationSet::Create(period, fSetup);
    InitNoiseArrays(fGemStationSet);
}

BmnStatus BmnGemRaw2Digit::ReadGlobalMap(TString FileName)
{
    string dummy;
    Int_t id = 0;
    UInt_t ser = 0;
    UInt_t ch_lo = 0;
    UInt_t ch_hi = 0;
    UInt_t station = 0;
    UInt_t mod = 0;
    UInt_t zone = 0;
    UInt_t side = 0;
    UInt_t type = 0;

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
        "SIGNAL_CONFIGURATION.Threshold", po::value<Double_t>(&final_thr)->default_value(25), "ADC final threshold")(
        "SIGNAL_CONFIGURATION.ThresholdDif", po::value<Double_t>(&thr_dif)->default_value(10), "ADC final threshold")(
        "SIGNAL_CONFIGURATION.NIterations", po::value<Int_t>(&n_iters)->default_value(5), "ADC final threshold")(
        "SIGNAL_CONFIGURATION.CModCut", po::value<Double_t>(&cmod_cut)->default_value(0), "ADC final threshold");

    // Load config file.
    po::variables_map vm;
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fMapFileName;
    LOG(info) << "Loading GEM Map: Period " << fPeriod << ", Run " << fRun << " " << name;
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
        ss >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> id >> side >> type >> station >> mod >> zone;
        GemMapLine* record = new GemMapLine();
        record->Ch_hi = ch_hi / GetNSamples();
        record->Serial = ser;
        record->Ch_lo = ch_lo / GetNSamples();
        record->Station = station;
        record->Zone = zone;
        record->GEM_id = id;
        record->Side = side;
        record->Type = type;
        record->Module = mod;
        fMap.push_back(record);
        GrabNewSerial(ser);
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::ReadLocalMap(TString FileName, BmnGemMap* m, Int_t lay, Int_t mod)
{
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + FileName;
    //    printf("%s\n", name.Data());
    ifstream inFile(name.Data());
    if (!inFile.is_open()) {
        LOG(error) << "Error opening map-file (" << name << ")!";
        return kBMNERROR;
    }
    Int_t iStrip = 0;
    Int_t chan = 0;
    while (!inFile.eof()) {
        inFile >> chan;
        if (!inFile.good())
            break;
        m[chan] = BmnGemMap(iStrip++, lay, mod);
        //        printf("\t\t strip %4d  lay %4d mod %4d  chan %4d\n", iStrip, lay, mod, chan);
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::ReadMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod)
{
    // Int_t size = 0;
    UniDetectorParameter* par = UniDetectorParameter::GetDetectorParameter("GEM", parName, GetPeriod(), GetRun());
    vector<UniValue*> iiArr;
    if (par != NULL)
        par->GetValue(iiArr);
    delete par;
    //    printf("%20s  mod %d lay %d\n", parName.Data(), mod, lay);
    for (size_t i = 0; i < iiArr.size(); ++i) {
        IIValue* pValue = (IIValue*)iiArr[i];
        m[pValue->value2] = BmnGemMap(pValue->value1 - 1, lay, mod);   // Strip begins from 0
        //        printf("\t\t strip %4d  lay %4d mod %4d  chan %4d %s\n", pValue->value1, lay, mod, pValue->value2,
        //        (pValue->value1 == 0) ? "WFT!!!" : "");
    }

    if (!iiArr.empty())
        for (size_t i = 0; i < iiArr.size(); i++)
            delete iiArr[i];
    return kBMNSUCCESS;
}

BmnGemRaw2Digit::~BmnGemRaw2Digit()
{
    if (fSmall)
        delete[] fSmall;
    if (fMid)
        delete[] fMid;
    for (size_t i = 0; i < fBig.size(); i++) {
        delete[] fBigHot[i];
        delete[] fBig[i];
    }
    if (!fMap.empty())
        for (size_t i = 0; i < fMap.size(); i++)
            delete fMap[i];
    DeleteNoiseArrays(fGemStationSet);
}

BmnStatus BmnGemRaw2Digit::FillProfiles(TClonesArray* adc)
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

BmnStatus BmnGemRaw2Digit::FillNoisyChannels()
{
    const Int_t kNStripsInBunch = BmnRawDataDecoder::samplesGem;
    // cout << "GEM: " << kNStripsInBunch << endl;
    //     const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const double kNThresh = BmnRawDataDecoder::threshGem;

    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh)
            for (auto& it : fMap)
                if ((Int_t)GetSerials()[iCr] == it->Serial && iCh >= it->Ch_lo && iCh <= it->Ch_hi) {
                    for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl)
                        if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                            Int_t station = -1;
                            Int_t strip = -1;
                            Int_t layer = -1;
                            Int_t mod = -1;
                            MapStrip(it, iCh, iSmpl, station, mod, layer, strip);
                            if (strip < 0)
                                continue;
                            fNoisyChannels[station][mod][layer][strip] = kTRUE;
                        }
                }
    for (Int_t iSt = 0; iSt < fGemStationSet->GetNStations(); ++iSt) {
        auto* st = fGemStationSet->GetStation(iSt);
        for (Int_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto* mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < mod->GetNStripLayers(); ++iLay) {
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                auto& lay = mod->GetStripLayer(iLay);
                Int_t kNBunches = lay.GetNStrips() / kNStripsInBunch;
                for (Int_t iBunch = 0; iBunch < kNBunches; ++iBunch) {
                    Double_t mean = 0.0;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE)
                            continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        // Double_t prev = prof->GetBinContent(strip);
                        mean += curr;
                    }
                    mean /= kNStripsInBunch;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE)
                            continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        // Double_t prev = prof->GetBinContent(strip);
                        //                         if (kNThresh * meanDiff < curr - prev)
                        if (kNThresh * mean < Abs(curr - mean))
                            fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
                    }
                }
            }
        }
    }
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh)
            for (auto& it : fMap)
                if ((Int_t)GetSerials()[iCr] == it->Serial && iCh >= it->Ch_lo && iCh <= it->Ch_hi)
                    for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                        Int_t station = -1;
                        Int_t strip = -1;
                        Int_t lay = -1;
                        Int_t mod = -1;
                        MapStrip(it, iCh, iSmpl, station, mod, lay, strip);
                        if (strip < 0)
                            continue;
                        if (fNoisyChannels[station][mod][lay][strip] == kTRUE)
                            GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
                    }

    return kBMNSUCCESS;
}

void BmnGemRaw2Digit::ProcessAdc(TClonesArray* gem, Bool_t doFill)
{
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            for (auto& it : fMap) {
                // it->Print();
                if ((Int_t)GetSerials()[iCr] == it->Serial && iCh >= it->Ch_lo && iCh <= it->Ch_hi) {
                    for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                        if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE)
                            continue;

                        Int_t station = -1;
                        Int_t strip = -1;
                        Int_t layer = -1;
                        Int_t mod = -1;
                        MapStrip(it, iCh, iSmpl, station, mod, layer, strip);
                        // cout << "DECODER INFO: " <<  station << " " << mod << " " << layer << " " << strip << endl;
                        if (strip < 0)
                            continue;

                        Double_t sig =
                            fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + fCMode[iCr][iCh] - fSMode[iCr][iCh];
                        // Double_t Asig = TMath::Abs(sig);
                        Double_t thr = Max(FinalThr, 3.5 * GetPedestalsRMS()[iCr][iCh][iSmpl]);
                        if ((!GetApplyThreshold()) || (sig > thr)) {
                            if (doFill /* && (Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) < cmodcut)*/) {
                                fSigProf[station][mod][layer]->Fill(strip);
                            } else {
                                BmnGemStripDigit* resDig = new ((*gem)[gem->GetEntriesFast()]) BmnGemStripDigit(
                                    station, mod, layer, strip, sig, GetPedestalsRMS()[iCr][iCh][iSmpl]);
                                //                                if ((Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) >
                                //                                cmodcut))
                                //                                    resDig->SetIsGoodDigit(kFALSE);
                                //                                else
                                resDig->SetIsGoodDigit(kTRUE);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}

BmnStatus BmnGemRaw2Digit::FillEvent(TClonesArray* adc, TClonesArray* gem)
{
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    ProcessAdc(gem, kFALSE);
    return kBMNSUCCESS;
}

inline void BmnGemRaw2Digit::MapStrip(GemMapLine* gemM,
                                      UInt_t ch,
                                      Int_t iSmpl,
                                      Int_t& station,
                                      Int_t& mod,
                                      Int_t& lay,
                                      Int_t& strip)
{
    Int_t ch2048 = ch * GetNSamples() + iSmpl;
    UInt_t realChannel = ch2048;
    BmnGemMap* fBigMap = nullptr;
    mod = gemM->Module;
    UInt_t side = gemM->Side;
    if (mod > 1)
        side += 2;
    if (gemM->GEM_id < 5) {
        if (gemM->GEM_id < 0) {
            Int_t chShift = -512;
            realChannel += chShift;
            fBigMap = fSmall;
        } else {
            fBigMap = fMid;
            if ((gemM->Ch_hi - gemM->Ch_lo) < 128 / GetNSamples())
                realChannel = 2048 + ch2048 - gemM->Ch_lo * GetNSamples();
        }
    } else {
        if (gemM->Zone == 0) {   // hot zone
            Int_t chShift = (gemM->Ch_lo == 0) ? 0 : -1024;
            if (fPeriod < 8)
                chShift += 1024;
            // For 6-7 run
            // Int_t chShift = (gemM->Ch_lo == 0) ? 1024 : 0;
            realChannel += chShift;
            fBigMap = fBigHot[side];
        } else {   // big zone
            fBigMap = fBig[side];
            if ((gemM->Ch_hi - gemM->Ch_lo) < 128 / GetNSamples())
                realChannel = 2048 + ch2048 - gemM->Ch_lo * GetNSamples();
        }
    }
    station = gemM->Station;
    lay = fBigMap[realChannel].lay;
    strip = fBigMap[realChannel].strip;
    return;
}