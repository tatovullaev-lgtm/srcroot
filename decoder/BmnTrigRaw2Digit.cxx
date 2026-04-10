#include "BmnTrigRaw2Digit.h"
// C++
#include <climits>
// Boost
#include <boost/program_options.hpp>
// FairRoot
#include "FairLogger.h"

namespace po = boost::program_options;
using namespace TMath;

BmnTrigParameters::BmnTrigParameters()
{
    for (UInt_t i = 0; i < CHANNEL_COUNT_MAX; i++) {
        for (UInt_t j = 0; j < TDC_BIN_COUNT; j++)
            INL[i][j] = 0u;
        ChannelMap[i] = 0u;
        NegativeMap[i] = kFALSE;
        branchArrayPtr[i] = NULL;
        t[i] = 0.0;
    }
    IsT0 = kFALSE;
}

BmnTrigRaw2Digit::BmnTrigRaw2Digit(TString PlacementMapFile, TString StripMapFile, TTree* digiTree)
    : fSetup(kBMNSETUP)
    , fWorkHist(make_unique<TH1I>(Form("workhist_%p", digiTree), "wh", SHRT_MAX - SHRT_MIN, SHRT_MIN, SHRT_MAX))
    , fTrcSerial(0)
{
    fWorkHist->SetDirectory(0);
    ReadPlacementMap(PlacementMapFile);
    if (ReadChannelMap(StripMapFile) == kBMNERROR) {
        printf("Error! Unable to read the trigger channel map!\n");
        return;
    }
    if (digiTree == NULL)
        return;
    // Create corresponding branches for each trigger.       //
    // different channels of the one trigger will be stored //
    // in one branch with different mod ID  //
    map<TString, TClonesArray*> brName2ptr;
    for (BmnTrigChannelData& record : fMap) {
        TString& detName = record.name;
        TClass* cl = detName.Contains("TQDC") ? BmnTrigWaveDigit::Class() : BmnTrigDigit::Class();
        auto it = brName2ptr.find(detName);
        if (it == brName2ptr.end()) {
            TClonesArray* ar = new TClonesArray(cl);
            ar->SetName(detName.Data());
            digiTree->Branch(detName.Data(), &ar);
            trigArrays.push_back(ar);
            record.branchArrayPtr = ar;
            brName2ptr.insert(make_pair(detName, ar));
        } else {
            record.branchArrayPtr = it->second;
        }
    }
    // Fill elements of placement map with channel->(strip, mod, branchRef) map //
    for (BmnTrigChannelData& record : fMap) {
        BmnTrigParameters* par = nullptr;
        map<PlMapKey, BmnTrigParameters*>::iterator itPar = fPlacementMap.find(PlMapKey(record.serial, record.slot));
        if (itPar == fPlacementMap.end()) {
            printf("CrateSeral %08X slot %u not found in the placement map!\n", record.serial, record.slot);
            par = new BmnTrigParameters();
            par->BoardSerial = record.serial;
            par->name = record.name;
            par->ChannelCount = CHANNEL_COUNT_MAX;   // ChanCntByName(record.name);
            fPlacementMap.insert(pair<PlMapKey, BmnTrigParameters*>(PlMapKey(par->BoardSerial, par->slot), par));
        } else
            par = itPar->second;
        par->ChannelMap[record.channel] = record.module;
        par->branchArrayPtr[record.channel] = record.branchArrayPtr;
        par->NegativeMap[record.channel] = record.isNegative;
    }
    for (auto& el : fPlacementMap) {
        BmnTrigParameters* par = el.second;
        ReadINLFromFile(par);
    }
}

// BmnTrigRaw2Digit::BmnTrigRaw2Digit(TString PlacementMapFile, TString StripMapFile, FairRootManager* frm, bool
// saveOutput) :

BmnTrigRaw2Digit::BmnTrigRaw2Digit(TString PlacementMapFile,
                                   TString StripMapFile,
                                   std::function<void(TString, TClonesArray*)> BranchRegFun)
    : fSetup(kBMNSETUP)
    , fWorkHist(make_unique<TH1I>(Form("workhist_%p", this), "wh", SHRT_MAX - SHRT_MIN, SHRT_MIN, SHRT_MAX))
    , fTrcSerial(0)
{
    fWorkHist->SetDirectory(0);
    ReadPlacementMap(PlacementMapFile);
    if (ReadChannelMap(StripMapFile) == kBMNERROR) {
        printf("Error! Unable to read the trigger channel map!\n");
        return;
    }
    // Create corresponding branches for each trigger.       //
    // different channels of the one trigger will be stored //
    // in one branch with different mod ID  //
    map<TString, TClonesArray*> brName2ptr;
    for (BmnTrigChannelData& record : fMap) {
        TString& detName = record.name;
        TClass* cl = detName.Contains("TQDC") ? BmnTrigWaveDigit::Class() : BmnTrigDigit::Class();
        auto it = brName2ptr.find(detName);
        if (it == brName2ptr.end()) {
            TClonesArray* ar = new TClonesArray(cl);
            ar->SetName(detName.Data());
            BranchRegFun(detName, ar);
            //            frm->Register(detName, "Trig", ar, saveOutput);
            trigArrays.push_back(ar);
            record.branchArrayPtr = ar;
            brName2ptr.insert(make_pair(detName, ar));
        } else
            record.branchArrayPtr = it->second;
    }
    // Fill elements of placement map with channel->(strip, mod, branchRef) map //
    for (BmnTrigChannelData& record : fMap) {
        BmnTrigParameters* par = nullptr;
        map<PlMapKey, BmnTrigParameters*>::iterator itPar = fPlacementMap.find(PlMapKey(record.serial, record.slot));
        if (itPar == fPlacementMap.end()) {
            printf("CrateSeral %08X slot %u not found in the placement map!\n", record.serial, record.slot);
            par = new BmnTrigParameters();
            par->BoardSerial = record.serial;
            par->name = record.name;
            par->ChannelCount = CHANNEL_COUNT_MAX;   // ChanCntByName(record.name);
            fPlacementMap.insert(pair<PlMapKey, BmnTrigParameters*>(PlMapKey(par->BoardSerial, par->slot), par));
        } else
            par = itPar->second;
        par->ChannelMap[record.channel] = record.module;
        par->branchArrayPtr[record.channel] = record.branchArrayPtr;
        par->NegativeMap[record.channel] = record.isNegative;
    }
    for (auto& el : fPlacementMap) {
        BmnTrigParameters* par = el.second;
        ReadINLFromFile(par);
    }
}

BmnStatus BmnTrigRaw2Digit::ReadPlacementMap(TString mappingFile)
{
    TString PlMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    LOGF(info, "Reading Triggers placement mapping file %s...", PlMapFileName.Data());
    ifstream pmFile;
    pmFile.open(PlMapFileName.Data());
    if (!pmFile.is_open()) {
        cout << "Error opening map-file (" << PlMapFileName << ")!" << endl;
        return kBMNERROR;
    }
    string dummy;
    string name;
    UInt_t crateSerial, boardSerial;
    UShort_t slot;
    UShort_t isT0;

    pmFile >> dummy >> dummy >> dummy >> dummy >> dummy;
    pmFile >> dummy;
    while (!pmFile.eof()) {
        pmFile >> name >> hex >> crateSerial >> dec >> slot >> hex >> boardSerial >> dec >> isT0;
        if (!pmFile.good())
            break;
        BmnTrigParameters* par = new BmnTrigParameters();
        par->BoardSerial = boardSerial;
        par->CrateSerial = crateSerial;
        par->slot = slot;
        par->name = name;
        par->IsT0 = isT0;
        par->ChannelCount = CHANNEL_COUNT_MAX;   // ChanCntByName(name);
        fPlacementMap.insert(pair<PlMapKey, BmnTrigParameters*>(PlMapKey(par->CrateSerial, par->slot), par));
    }
    pmFile.close();
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::ReadChannelMap(TString mappingFile)
{
    fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    LOGF(info, "Reading Triggers strip mapping file %s...", fMapFileName.Data());

    vector<string> trc_trigger_bits;
    vector<string> scalers;
    vector<string> configuration;
    vector<string> t0_raw;

    // Setup options.
    po::options_description desc("Options");
    desc.add_options()("INPUTS.serial", po::value<string>(), "TRC serial")(
        "INPUTS.channels", po::value<vector<string>>(&trc_trigger_bits)->multitoken(), "TRC trigger input signal bits")(
        "SCALERS.channels", po::value<vector<string>>(&scalers)->multitoken(),
        "Scaler channels")("TDC-CONFIGURATION.channels", po::value<vector<string>>(&configuration)->multitoken(),
                           "TDC configuration")("T0-RAW.virt_serial", po::value<string>(), "T0 virtual serial")(
        "T0-RAW.channels", po::value<vector<string>>(&t0_raw)->multitoken(), "TDC configuration");

    // Load config file.
    po::variables_map vm;
    ifstream config_file(fMapFileName.Data(), ifstream::in);
    if (!config_file.is_open()) {
        printf("%s - file open error!\n", mappingFile.Data());
        return kBMNERROR;
    }
    po::store(po::parse_config_file(config_file, desc), vm);
    config_file.close();
    po::notify(vm);

    stringstream hs(vm["INPUTS.serial"].as<string>());
    hs >> std::hex >> fTrcSerial;
    LOGF(info, "TRC serial %8X", fTrcSerial);

    //========== read TRC trig bits map ==========//
    string tr_name;
    uint16_t bit;
    for (auto it : trc_trigger_bits) {
        istringstream ss(it);
        ss >> tr_name >> bit;
        fTrcBitsMap[tr_name] = bit;
        //        printf("trig[%8s] = %u\n", tr_name.data(), fTrigBitsMap[tr_name]);
    }
    //========== read scalers channel map ==========//
    UInt_t ser;
    uint16_t ch;
    for (auto it : scalers) {
        istringstream ss(it);
        ss >> tr_name >> hex >> ser >> dec >> ch;
        fScalersMap[tr_name] = make_pair(ser, ch);
        LOGF(debug, "msc[%15s] = (0x%08X : %2u)", tr_name.data(), fScalersMap[tr_name].first,
             fScalersMap[tr_name].second);
    }
    //========== read TDC channel map ==========//
    TString name;
    UShort_t slot, mod;
    UShort_t neg;
    for (auto it : configuration) {
        istringstream ss(it);
        ss >> name >> mod >> hex >> ser >> dec >> slot >> ch >> neg;
        if (ch < 0)
            continue;
        BmnTrigChannelData record;
        record.branchArrayPtr = NULL;
        record.name = name;
        record.serial = ser;
        record.module = mod;
        record.slot = slot;
        record.channel = ch;
        record.isNegative = neg > 0 ? kTRUE : kFALSE;
        fMap.push_back(record);
    }
    //========== read T0 raw signals from Sergeev's txt ==========//
    stringstream vs(vm["T0-RAW.virt_serial"].as<string>());
    UInt_t t0_virt_serial;
    vs >> std::hex >> t0_virt_serial;
    LOGF(info, "t0_virt_serial %0X", t0_virt_serial);
    for (auto it : t0_raw) {
        istringstream ss(it);
        ss >> tr_name >> mod >> ch;
        fT0Map[make_pair(tr_name, mod)] = ch;
        LOGF(debug, "trig[%10s] mod %2u  ch %3u", tr_name.data(), mod, ch);
    }
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::ReadINLFromFile(BmnTrigParameters* par)
{
    fstream ff;
    TString fINLFileName =
        TString(getenv("VMCWORKDIR")) + TString("/input/") + Serial2FileName(par->name, par->BoardSerial);
    ff.open((fINLFileName).Data(), ios::in);
    if (!ff.is_open()) {
        cout << "Error opening INL-file (" << fINLFileName << ")!" << endl;
        return kBMNERROR;
    }
    LOGF(info, "Open INL file %s", fINLFileName.Data());
    TPRegexp reInlHdr("\\[.*(inl_corr).*\\]");   // INL block header
    //    regex reInlHdr("\\[.*(inl_corr).*\\]"); // INL block header
    //    regex reInlChannel("\\s*(\\d+)=(.+)"); // chID=c0, c1, ...
    Bool_t isInlHdr = kFALSE;
    while (!ff.eof()) {
        string line;
        std::getline(ff, line, '\n');
        if (reInlHdr.MatchB(line)) {
            //        if (regex_match(line, reInlHdr)) {
            isInlHdr = kTRUE;
            break;
        }
    }
    if (!isInlHdr) {
        printf("Incorrect INL file format!\n");
        return kBMNERROR;
    }
    UShort_t channelID = 0;
    while (!ff.eof()) {
        string line;
        std::getline(ff, line, '\n');
        //        printf("Read %lu \n", line.length());
        //        if (!regex_match(line, reInlChannel))
        //            continue;
        //        line = regex_replace(line, reInlChannel, "$1 $2");
        //        printf("%s\n", line.c_str());
        istringstream ss(line);
        ss >> channelID;
        // printf("Channel ID = %u\n", channelID);
        UShort_t i_bin = 0;
        while (ss.tellg() != -1) {
            if (i_bin > TDC_BIN_COUNT) {
                printf("INL File contains too many bins in channel.\n");
                ff.close();
                return kBMNERROR;
            }
            if (ss.peek() == ',' || ss.peek() == '=') {
                ss.ignore();
            }
            ss >> par->INL[channelID][i_bin];
            i_bin++;
        }
    }
    ff.close();
    return kBMNSUCCESS;
}

void BmnTrigRaw2Digit::ProcessWave(Short_t* iValue, const UShort_t& nVals, Bool_t& isNeg)
{
    Double_t baseLine = GetBaseline(iValue, nVals);
    baseLine = 0.0;
    const UShort_t baseInt = 4;
    for (UShort_t i = 0; i < baseInt; i++)
        baseLine += iValue[i];
    baseLine /= baseInt;
    for (UShort_t i = 0; i < nVals; i++)
        iValue[i] -= baseLine;
    if (isNeg)
        for (UShort_t i = 0; i < nVals; i++)
            iValue[i] = -iValue[i];
}

Double_t BmnTrigRaw2Digit::GetBaseline(Short_t* iValue, const UShort_t& nVals)
{
    fWorkHist->Reset();
    //    TH1I hw("w", "w", nVals, 0, nVals);
    for (UShort_t i = 0; i < nVals; i++) {
        fWorkHist->Fill(iValue[i]);
        //        hw.SetBinContent(i, iValue[i]);
        //        printf("fill %d\n", iValue[i]);
    }
    //    printf("max val %f\n", fWorkHist->GetMaximum());
    //    Double_t bc = fWorkHist->GetXaxis()->GetBinLowEdge(fWorkHist->GetMaximumBin());
    //    printf("bc %f\n", bc);
    //    TCanvas q("q", "q", 1920, 1080);
    //    q.Divide(1, 2);
    //    q.cd(1);
    //    fWorkHist->Draw();
    //    q.cd(2);
    //    hw.Draw();
    //    q.Print("qq.png");
    //    Fatal(__func__, "Exit here!");
    return fWorkHist->GetXaxis()->GetBinLowEdge(fWorkHist->GetMaximumBin());
}

BmnStatus BmnTrigRaw2Digit::FillEvent(TClonesArray* tdc, TClonesArray* adc, unordered_map<UInt_t, Long64_t>& tsMap)
{
    std::vector<Double_t> times;
    std::vector<Double_t> diff;
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); iAdc++) {
        times.clear();
        diff.clear();
        BmnTQDCADCDigit* adcDig = (BmnTQDCADCDigit*)adc->At(iAdc);
        UShort_t iChannel = adcDig->GetChannel();
        auto plIter = fPlacementMap.find(PlMapKey(adcDig->GetSerial(), adcDig->GetSlot()));
        if (plIter == fPlacementMap.end())
            continue;
        BmnTrigParameters* par = plIter->second;
        
            UShort_t iMod = par->ChannelMap[iChannel];
            Double_t adcTimestamp = adcDig->GetAdcTimestamp() * ADC_CLOCK_TQDC16VS;
            Double_t trgTimestamp = adcDig->GetTrigTimestamp() * ADC_CLOCK_TQDC16VS;
            Double_t boardShift = par->IsT0 ? 0 : tsMap[adcDig->GetSerial()];

            for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); ++iTdc) {
                BmnTDCDigit* tdcDig = (BmnTDCDigit*)tdc->At(iTdc);
                if (tdcDig->GetSerial() != adcDig->GetSerial() || tdcDig->GetSlot() != adcDig->GetSlot())
                    continue;
                if (tdcDig->GetChannel() != iChannel)
                    continue;
                Double_t time = (tdcDig->GetValue() + par->INL[iChannel][tdcDig->GetValue() % TDC_BIN_COUNT])
                                * TDC_CLOCK / TDC_BIN_COUNT + boardShift;
                diff.push_back(fabs(time - (adcTimestamp - trgTimestamp)));
                times.push_back(time);
            }
            Double_t matchTime = -999.0;
            Double_t minUsed = 999.0;
            TClonesArray *trigAr = NULL;
            if (diff.size() > 0) {
                auto result = min_element(begin(diff), end(diff));
                int idx = std::distance(begin(diff), result);
                // Found the match, so let's save that as and ADC and corresponding TDC
                matchTime = times.at(idx);
                minUsed = diff.at(idx);
                //            if (minUsed > 296)
                //                matchTime = -999.0;
            }
            trigAr = par->branchArrayPtr[iChannel];
            if (trigAr) {
                BmnTrigWaveDigit* dig = new ((*trigAr)[trigAr->GetEntriesFast()])
                    BmnTrigWaveDigit(iMod, adcDig->GetShortValue(), adcDig->GetNSamples(), trgTimestamp, adcTimestamp, matchTime);
                ProcessWave(dig->GetShortValue(), dig->GetNSamples(), par->NegativeMap[iChannel]);
                //dig->TdcVector() = move(times);
            }
        
    }
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::FillEvent(TClonesArray* tdc, unordered_map<UInt_t, Long64_t>& tsMap)
{
    for (auto& el : fPlacementMap)
        for (UInt_t i = 0; i < CHANNEL_COUNT_MAX; i++)
            el.second->t[i] = -1.0;
    for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); ++iTdc) {
        BmnTDCDigit* tdcDig = (BmnTDCDigit*)tdc->At(iTdc);
        auto plIter = fPlacementMap.find(PlMapKey(tdcDig->GetSerial(), tdcDig->GetSlot()));
        if (plIter == fPlacementMap.end())
            continue;
        BmnTrigParameters* par = plIter->second;
        Double_t boardShift = par->IsT0 ? 0 : tsMap[tdcDig->GetSerial()]; // not valid for 8 run
        UShort_t rChannel = tdcDig->GetHptdcId() * kNCHANNELS + tdcDig->GetChannel();
        Double_t time = (tdcDig->GetValue() + par->INL[rChannel][tdcDig->GetValue() % TDC_BIN_COUNT]) * TDC_CLOCK
                        / TDC_BIN_COUNT + boardShift;
        //        printf("\tCrateSeral %08X slot %02u channel %02u  time %+2.2f  leading %d neg %d\n",
        //        tdcDig->GetSerial(), tdcDig->GetSlot(), rChannel, time,
        //        tdcDig->GetLeading(),par->NegativeMap[rChannel]);
        if (tdcDig->GetLeading() ^ par->NegativeMap[rChannel]) {
            par->t[rChannel] = time;
        } else {
            if (time < par->t[rChannel])
                continue;
            if (par->t[rChannel] < 0)
                continue;
            UShort_t iMod = par->ChannelMap[rChannel];
            TClonesArray* trigAr = par->branchArrayPtr[rChannel];
            if (trigAr == NULL)
                continue;
            Double_t tL = par->t[rChannel];
            Double_t tT = time;
            par->t[rChannel] = -1.0;
            new ((*trigAr)[trigAr->GetEntriesFast()]) BmnTrigDigit(iMod, tL, tT - tL);
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::ClearArrays()
{
    for (TClonesArray* ar : trigArrays)
        ar->Delete();
    return kBMNSUCCESS;
}
