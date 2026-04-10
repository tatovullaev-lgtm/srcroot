#include "BmnProfRaw2Digit.h"

BmnProfRaw2Digit::BmnProfRaw2Digit(Int_t period, Int_t run)
    : BmnAdcProcessor(period, run, "SiProf", ADC_PROF_N_CHANNELS, ADC32_N_SAMPLES)
{
    //    fVerbose = 2;
    cout << "Loading Profilometer Map: Period " << period << ", Run " << run << "..." << endl;
    string dir = string(getenv("VMCWORKDIR")) + "/input/";
    fLocalMapFileName = dir + string(Form("Prof_map_run_%d.json", period));
    fGlobalMapFileName = dir + string(Form("Prof_map_run_%d_global.txt", period));
    ReadLocalMapFile(fLocalMapFileName);
    ReadGlobalMapFile(fGlobalMapFileName);

    InitArrays();
    fSiProfStationSet = GetProfStationSet(period);
    InitNoiseArrays(fSiProfStationSet);
    //    thrMax = 1200;
    //    thrDif = 100;
    thrMax = 1250;
    thrDif = 400;
    fStartIter = 0;
    fNIters = 4;
}

BmnProfRaw2Digit::~BmnProfRaw2Digit()
{
    DeleteNoiseArrays(fSiProfStationSet);
}

unique_ptr<BmnSiProfStationSet> BmnProfRaw2Digit::GetProfStationSet(Int_t period)
{
    TString gPathConfig = getenv("VMCWORKDIR");
    TString xmlConfFileName;
    switch (period) {
        case 8:
            xmlConfFileName = "ProfRun8.xml";
            break;
        default:
            printf("Error! Unknown config!\n");
            return nullptr;
            break;
    }
    TString gPathSiliconConfig = gPathConfig + "/parameters/profilometer/XMLConfigs/";
    return std::make_unique<BmnSiProfStationSet>(gPathSiliconConfig + xmlConfFileName);
}

BmnStatus BmnProfRaw2Digit::ReadGlobalMapFile(string name)
{
    string dummy;
    uint32_t ser = 0;
    uint16_t ch = 0;
    uint16_t station = 0;
    uint16_t mod = 0;
    uint16_t layer = 0;
    ifstream inFile(name);
    if (!inFile.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 2; ++i)
        getline(inFile, dummy);   // comment line in input file
    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch >> station >> mod >> layer;
        if (!inFile.good())
            break;
        auto it = fChannelMaps.find(mod);
        if (it == fChannelMaps.end())
            continue;
        auto itLayer = it->second.find(layer);
        if (itLayer == it->second.end())
            continue;
        ProfiLocal& LayerInfo = itLayer->second;

        auto itGlobal = fGlobalMap.find(make_pair(ser, ch));
        if (itGlobal == fGlobalMap.end()) {
            auto pair = fGlobalMap.insert(make_pair(make_pair(ser, ch), ProfiInfo{station, mod, &LayerInfo}));
            GrabNewSerial(ser);
            itGlobal = pair.first;
        }
        if (fVerbose > 1)
            cout << "ser: " << std::hex << ser << std::dec << " ch: " << ch << endl;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnProfRaw2Digit::ReadLocalMapFile(string name)
{
    try {
        pt::ptree conf;
        pt::read_json(name, conf);
        pt::ptree pads = conf.get_child("modules");
        for (auto v = pads.begin(); v != pads.end(); v++) {
            //            cout << " channels " << (*v).second.get_optional<Int_t>("nAsicChannels") << endl;
            //            cout << " mod      " << (*v).second.get_optional<uint16_t>("moduleNumber") << endl;
            uint8_t mod = (*v).second.get<uint8_t>("moduleNumber");
            pt::ptree maps = (*v).second.get_child("channelMapping");
            for (auto m = maps.begin(); m != maps.end(); m++) {

                auto modIter = fChannelMaps.find(mod);
                if (modIter == fChannelMaps.end()) {
                    if (fVerbose > 0)
                        printf("adding %d to the map\n", mod);
                    auto pair = fChannelMaps.insert(make_pair(mod, LayerIdProfiMap{}));
                    modIter = pair.first;
                }
                auto& localMap = modIter->second;
                ProfiLocal loc{.ChannelName = (*m).second.get<char>("adcChName"),
                               .LayerType = (*m).second.get<char>("layerType")};
                loc.LayerId = (loc.LayerType == 'p') ? 0 : 1;
                auto locPair = localMap.insert(make_pair(loc.LayerId, loc));
                if (fVerbose > 0)
                    printf("adding %d : %d to the map\n", mod, loc.LayerId);
                auto& stripArray = locPair.first->second.StripMap;
                pt::ptree strips = (*m).second.get_child("stripsMapping");
                for (auto stripNode = strips.begin(); stripNode != strips.end(); stripNode++) {
                    auto it = (*stripNode).second.begin();
                    //                    cout << it->second.get_value<int>() << " : " <<
                    //                            (++it)->second.get_value<int>()<< endl;
                    int stripId = (it)->second.get_value<int16_t>();
                    int chanlId = (++it)->second.get_value<int16_t>();
                    stripArray[chanlId] = stripId;
                }
            }
        }
    } catch (boost::exception& e) {
        cerr << boost::diagnostic_information(e);
        cout << "Unable to parse the channel map!\n" << endl;
        return kBMNERROR;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnProfRaw2Digit::FillEvent(TClonesArray* adc, TClonesArray* csc)
{
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    ProcessAdc(adc, csc, kFALSE);
    return kBMNSUCCESS;
}

BmnStatus BmnProfRaw2Digit::FillProfiles(TClonesArray* adc)
{
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    ProcessAdc(adc, nullptr, kTRUE);

    return kBMNSUCCESS;
}

void BmnProfRaw2Digit::MapStrip(ProfiInfo& info,
                                uint16_t ch,
                                Int_t iSmpl,
                                uint16_t& station,
                                uint16_t& mod,
                                uint16_t& lay,
                                int16_t& strip)
{
    ProfiLocal* layerInfo = info.Layer;
    station = info.StationId;
    mod = info.ModuleId;
    lay = layerInfo->LayerId;
    strip = layerInfo->StripMap[iSmpl];
}

BmnStatus BmnProfRaw2Digit::FillNoisyChannels()
{
    // const Int_t kNStripsInBunch = GetNSamples();
    //     const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const Int_t kNThresh = 3;

    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto it = fGlobalMap.find(make_pair(GetSerials()[iCr], iCh));
            if (it == fGlobalMap.end())
                continue;
            ProfiInfo& info = it->second;
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl)
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                    uint16_t station = 0;
                    uint16_t module = 0;
                    uint16_t layer = 0;
                    int16_t strip = -1;
                    MapStrip(info, iCh, iSmpl, station, module, layer, strip);
                    if (strip < 0)
                        continue;
                    fNoisyChannels[station][module][layer][strip] = kTRUE;
                }
        }
    for (Int_t iSt = 0; iSt < fSiProfStationSet->GetNStations(); ++iSt) {
        auto* st = fSiProfStationSet->GetStation(iSt);
        for (Int_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto* mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < mod->GetNStripLayers(); ++iLay) {
                auto& lay = mod->GetStripLayer(iLay);
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                Double_t mean = 0.0;
                for (Int_t iStrip = 0; iStrip < lay.GetNStrips(); ++iStrip) {
                    if (fNoisyChannels[iSt][iMod][iLay][iStrip] == kTRUE)
                        continue;
                    // Double_t curr = prof->GetBinContent(iStrip);
                    Double_t next = prof->GetBinContent(iStrip + 1);
                    mean += next;
                }
                if (mean > 0)
                    mean /= lay.GetNStrips();
                for (Int_t iStrip = 0; iStrip < lay.GetNStrips(); ++iStrip) {
                    if (fNoisyChannels[iSt][iMod][iLay][iStrip] == kTRUE)
                        continue;
                    // Double_t curr = prof->GetBinContent(iStrip);
                    Double_t next = prof->GetBinContent(iStrip + 1);
                    //                        if (kNThresh * meanDiff < next - curr)
                    if (kNThresh * mean < Abs(next - mean))
                        fNoisyChannels[iSt][iMod][iLay][iStrip] = kTRUE;
                }
            }
        }
    }
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto it = fGlobalMap.find(make_pair(GetSerials()[iCr], iCh));
            if (it == fGlobalMap.end())
                continue;
            ProfiInfo& info = it->second;
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl)
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                    uint16_t station = 0;
                    uint16_t module = 0;
                    uint16_t layer = 0;
                    int16_t strip = -1;
                    MapStrip(info, iCh, iSmpl, station, module, layer, strip);
                    if (strip < 0)
                        continue;
                    if (fNoisyChannels[station][module][layer][strip] == kTRUE)
                        GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
                }
        }

    return kBMNSUCCESS;
}

void BmnProfRaw2Digit::ProcessAdc(TClonesArray* adc, TClonesArray* arr, Bool_t doFill)
{
    Double_t FinalThr = thrMax - (fNIters - 1) * thrDif;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            auto it = fGlobalMap.find(make_pair(GetSerials()[iCr], iCh));
            if (it == fGlobalMap.end())
                continue;
            ProfiInfo& info = it->second;
            // Double_t cs = fCMode[iCr][iCh] - fSMode[iCr][iCh];
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE)
                    continue;
                uint16_t station = 0;
                uint16_t module = 0;
                uint16_t layer = 0;
                int16_t strip = -1;
                MapStrip(info, iCh, iSmpl, station, module, layer, strip);
                if (strip < 0)
                    continue;
                // board1 1200, 1600
                // board2 1200, 1400
                // spec threshold
                Double_t SpecThr(0);
                if (station == 0) {
                    if (layer == 0)
                        SpecThr = 1200;
                    else
                        SpecThr = 1600;
                } else {
                    if (layer == 0)
                        SpecThr = 1200;
                    else
                        SpecThr = 1400;
                }
                Double_t sig = fAdc[iCr][iCh][iSmpl];   // - fPedVal[iCr][iCh][iSmpl] + cs;
                // Double_t Asig = TMath::Abs(sig);
                Double_t thr = Max(SpecThr, Max(FinalThr, 3.5 * GetPedestalsRMS()[iCr][iCh][iSmpl]));
                if (((sig > thr) && (info.Layer->LayerType == 'p')) || ((sig < -thr) && (info.Layer->LayerType == 'n')))
                {
                    if (doFill) {
                        fSigProf[station][module][layer]->Fill(strip);
                    } else {
                        //                        printf("st %d mod %d l %d strip %d ped %2.3f sig %2.3f thr %2.3f \n",
                        //                        station, module ,layer, strip, fPedVal[iCr][iCh][iSmpl], sig, thr);
                        BmnSiProfDigit* resDig = new ((*arr)[arr->GetEntriesFast()])
                            BmnSiProfDigit(station, module, layer, strip, sig, GetPedestalsRMS()[iCr][iCh][iSmpl]);
                        resDig->SetIsGoodDigit(kTRUE);
                    }
                }
            }
        }
    }
}
