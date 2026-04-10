/** @file BmnHodoReconstructor.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 03.04.2021
 **
 ** Code for BmnHodoEvent reconstruction (creation) from BmnHodoDigi {Data} or BmnHodoDigit {Sim}
 **/

#include "BmnHodoReconstructor.h"

BmnHodoReconstructor::BmnHodoReconstructor(const std::string& config, bool isExp)
    : fArrayOfDigits(nullptr)
    , fBmnHodoEvent(nullptr)
    , fCuts(nullptr)
    , fIsExp(isExp)
    , fworkTime(0.)
{
    ParseConfig(config);
    LOG(detail) << Form("BmnHodoReconstructor: isExp %i", fIsExp);
}

void BmnHodoReconstructor::ParseConfig(const std::string& inputFile)
{
    std::string fullPath = std::string(getenv("VMCWORKDIR")) + "/parameters/hodo/" + inputFile;
    std::ifstream input(fullPath);
    if (!input.is_open()) {
        LOG(error) << "BmnHodoReconstructor::ParseConfig: Cannot open file " << fullPath;
        return;
    }

    nlohmann::json j;
    try {
        input >> j;
    } catch (const std::exception& e) {
        LOG(error) << "BmnHodoReconstructor::ParseConfig: Failed to parse JSON: " << e.what();
        return;
    }

    std::string version = j.value("version", "unknown");
    std::string comment = j.value("comment", "none");

    LOG(debug) << "BmnHodoReconstructor::ParseConfig. Version: " << version;
    LOG(debug) << "BmnHodoReconstructor::ParseConfig. Comment: " << comment;

    fuoResolutionMap.clear();
    for (const auto& entry : j["calibration"]) {
        int strip = entry["strip"];
        int side = entry["side"];
        int gain = entry["gain"];
        double stochastic = entry["stochastic"];
        double constant = entry["constant"];

        uint32_t address = BmnHodoAddress::GetAddress(strip, side, gain);
        Kernel resolution(stochastic, constant);
        fuoResolutionMap.emplace(address, resolution);
    }
}

void BmnHodoReconstructor::SetRecoCutsFile(const std::string& cuts)
{
    std::string fullPath = std::string(getenv("VMCWORKDIR")) + "/parameters/hodo/" + cuts;
    std::ifstream test(fullPath);
    if (!test.is_open()) {
        LOG(warn) << Form("BmnHodoReconstructor: cuts file not found: %s. Using default (no cuts).", fullPath.c_str());
        fCuts = nullptr;
        return;
    }

    fCuts = new BmnHodoCuts(BmnHodoCuts::Load(fullPath, fIsExp));
    LOG(info) << Form("BmnHodoReconstructor: cuts loaded from %s (isExp=%d)", fullPath.c_str(), fIsExp);
}

BmnHodoReconstructor::~BmnHodoReconstructor()
{
    if (fBmnHodoEvent)
        delete fBmnHodoEvent;
    if (fCuts)
        delete fCuts;
}

InitStatus BmnHodoReconstructor::Init()
{
    fworkTime = 0.;
    fpFairRootMgr = FairRootManager::Instance();
    if (!fpFairRootMgr) {
        LOG(error) << "BmnHodoReconstructor::Init(): FairRootManager instance is nullptr! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    fArrayOfDigits = (TClonesArray*)fpFairRootMgr->GetObject((fIsExp) ? "HodoDigi" : "HodoDigit");
    if (!fArrayOfDigits) {
        LOG(error) << "BmnHodoReconstructor::Init(): branch with Digits not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnHodoEvent = new BmnHodoEvent();
    fBmnHodoEvent->reset();
    fpFairRootMgr->RegisterAny("HodoEvent", fBmnHodoEvent, kTRUE);
    fpFairRootMgr->Register("HodoEvent", "Hodo", fBmnHodoEvent, kFALSE);

    LOG(debug) << "Hodo Reconstructor ready";
    return kSUCCESS;
}

void BmnHodoReconstructor::Exec(Option_t* opt)
{
    if (!IsActive())
        return;

    TStopwatch sw;
    sw.Start();
    fBmnHodoEvent->ResetStrips();
    auto stripMap = BuildFilteredStripMap();

    for (auto& [strip_id, digis] : stripMap) {
        double numerator = 0.0;
        double denominator = 0.0;
        double integrator = 0.0;

        for (auto* digit : digis) {
            auto address = digit->GetAddress();
            auto it = fuoResolutionMap.find(address);
            if (it == fuoResolutionMap.end())
                continue;

            const double signal = digit->GetSignal();
            const double sigma = it->second.sigma(signal);
            if (sigma <= 0)
                continue;

            const double weight = 1.0 / (sigma * sigma);
            numerator += signal * weight;
            denominator += weight;
            if (fIsExp)
                integrator += (static_cast<BmnHodoDigi*>(digit))->fSignalIntegral * weight;
        }

        if (denominator > 0) {
            auto* strip = fBmnHodoEvent->GetStrip(strip_id);
            strip->SetSignal(numerator / denominator);
            if (fIsExp)
                strip->SetSignalIntegral(integrator / denominator);
            else
                strip->SetSignalIntegral(strip->GetSignal());
        } else {
            LOG(debug4) << Form("Strip %d skipped: no valid resolution info", strip_id);
        }
    }

    fBmnHodoEvent->SummarizeEvent();
    sw.Stop();
    fworkTime += sw.RealTime();
}

std::vector<BmnHodoDigit*> BmnHodoReconstructor::GetDenoisedDigis()
{
    std::vector<BmnHodoDigit*> denoised;
    denoised.reserve(fArrayOfDigits->GetEntriesFast());

    for (int i = 0, n = fArrayOfDigits->GetEntriesFast(); i < n; ++i) {
        if (fIsExp) {
            auto* d = static_cast<BmnHodoDigi*>(fArrayOfDigits->At(i));
            if (!d)
                continue;
            if (!fCuts || fCuts->PassExp(d))
                denoised.push_back(d);
        } else {
            auto* d = static_cast<BmnHodoDigit*>(fArrayOfDigits->At(i));
            if (!d)
                continue;
            if (!fCuts || fCuts->PassSim(d))
                denoised.push_back(d);
        }
    }

    LOG(debug4) << Form("Hodo denoise: kept %zu / %d (%.1f%%)%s", denoised.size(), fArrayOfDigits->GetEntriesFast(),
                        100.0 * denoised.size() / std::max(1, fArrayOfDigits->GetEntriesFast()),
                        fCuts ? "" : " [no cuts applied]");
    return denoised;
}

std::vector<BmnHodoDigit*> BmnHodoReconstructor::DesaturateDigis(const std::vector<BmnHodoDigit*>& input) const
{
    std::unordered_map<uint32_t, std::vector<BmnHodoDigit*>> byStrip;
    byStrip.reserve(input.size());

    for (auto* d : input)
        byStrip[d->GetStripId()].push_back(d);

    std::vector<BmnHodoDigit*> result;
    result.reserve(input.size());

    for (auto& [strip, vec] : byStrip) {
        const bool hasClean = std::any_of(vec.begin(), vec.end(), [](auto* d) { return !d->GetIsSaturated(); });

        if (hasClean) {
            for (auto* d : vec)
                if (!d->GetIsSaturated())
                    result.push_back(d);
        } else {
            // all saturated -> prefer lowest-signal among low-gain
            std::vector<BmnHodoDigit*> lowGain;
            for (auto* d : vec)
                if (d->GetGain() == 0)
                    lowGain.push_back(d);

            BmnHodoDigit* chosen = nullptr;
            if (!lowGain.empty()) {
                chosen = *std::min_element(lowGain.begin(), lowGain.end(),
                                           [](auto* a, auto* b) { return a->GetSignal() < b->GetSignal(); });
            } else {
                chosen = *std::min_element(vec.begin(), vec.end(),
                                           [](auto* a, auto* b) { return a->GetSignal() < b->GetSignal(); });
            }
            result.push_back(chosen);
        }
    }
    return result;
}

std::vector<BmnHodoDigit*> BmnHodoReconstructor::ResolvePileup(const std::vector<BmnHodoDigit*>& input)
{
    std::unordered_map<uint32_t, std::vector<BmnHodoDigi*>> byAddr;
    byAddr.reserve(input.size());

    for (auto* d : input)
        byAddr[d->GetAddress()].push_back(static_cast<BmnHodoDigi*>(d));

    std::vector<BmnHodoDigit*> resolved;
    resolved.reserve(byAddr.size());

    for (auto& [addr, vec] : byAddr) {
        const int strip_id = BmnHodoAddress::GetStripId(addr);
        auto* strip = fBmnHodoEvent->GetStrip(strip_id);

        if (vec.size() == 1) {
            auto* d = vec.front();
            if (d->fNpeaks > 1)
                strip->SetPileUpStatus(BmnHodoStrip::PileUp::Resolved);
            resolved.push_back(d);
        } else {
            auto* minAmp = *std::min_element(vec.begin(), vec.end(),
                                             [](auto* a, auto* b) { return a->GetSignal() < b->GetSignal(); });
            strip->SetPileUpStatus(BmnHodoStrip::PileUp::Unresolved);
            resolved.push_back(minAmp);
        }
    }
    return resolved;
}

// Build map: strip_id -> vector of digis
std::unordered_map<uint32_t, std::vector<BmnHodoDigit*>> BmnHodoReconstructor::BuildFilteredStripMap()
{
    // 1. denoise
    auto denoised = GetDenoisedDigis();

    // 2. remove saturated digis (keep one low-gain if all saturated)
    auto desat = DesaturateDigis(denoised);

    // 3. exp only: resolve pileup (one digi per address, mark pileup)
    std::vector<BmnHodoDigit*> processed = (fIsExp) ? ResolvePileup(desat) : desat;

    // 4. build final map strip_id -> digis
    std::unordered_map<uint32_t, std::vector<BmnHodoDigit*>> stripMap;
    stripMap.reserve(processed.size());
    for (auto* digi : processed)
        stripMap[digi->GetStripId()].push_back(digi);

    return stripMap;
}

void BmnHodoReconstructor::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch("HodoEvent", &fBmnHodoEvent);
    resultTree->Fill();
}

void BmnHodoReconstructor::Finish()
{
    printf("Work time of BmnHodoReconstructor: %4.2f sec.\n", fworkTime);
}
