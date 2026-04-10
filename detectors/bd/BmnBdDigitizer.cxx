// -------------------------------------------------------------------------
// -----                   BmnBdDigitizer source file                  -----
// -----                  Created 11/04/22  by S. Merts                -----
// -----                  Updated 11/04/25  by N. Lashmanov            -----
// -------------------------------------------------------------------------

#include "BmnBdDigitizer.h"

#include "BmnBdPoint.h"
#include "BmnTrigDigit.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom3.h"

#include <iostream>
#include <map>

using namespace std;
using namespace TMath;

static Double_t workTime = 0.0;

// Default constructor for BmnBdDigitizer.
BmnBdDigitizer::BmnBdDigitizer()
    : FairTask("Bmn BD Digitizer")
    , fPointArray(nullptr)
    , fDigitArray(nullptr)
    , fNMod(40)
    , fNoiseSigma(0.00001)
    ,   // Default noise level (GeV)
    fThreshold(0.0004)
    ,   // Default signal threshold (GeV)
    fFiredChannels(0)
{}

// Destructor for BmnBdDigitizer.
BmnBdDigitizer::~BmnBdDigitizer()
{
    if (fDigitArray) {
        delete fDigitArray;
        fDigitArray = nullptr;
    }
}

// Converts amplitude (GeV) to signal duration (ns).
Double_t BmnBdDigitizer::ConvertAmplitudeToTime(Double_t amplitude)
{

    const Double_t eMin = 0.0003;   // 300 keV in GeV
    const Double_t eMax = 0.05;     // 50 MeV in GeV
    const Double_t tMin = 6.0;      // ns
    const Double_t tMax = 18.0;     // ns

    // Linear interpolation
    if (amplitude <= eMin)
        return tMin;
    if (amplitude >= eMax)
        return tMax;

    Double_t fraction = (amplitude - eMin) / (eMax - eMin);
    Double_t timeNs = tMin + fraction * (tMax - tMin);
    return timeNs;
}

InitStatus BmnBdDigitizer::Init()
{
    // Get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- BmnBdDigitizer::Init: RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // Get input array
    fPointArray = (TClonesArray*)ioman->GetObject("BdPoint");
    if (!fPointArray) {
        cout << "-E- BmnBdDigitizer::Init: Branch BdPoint not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    // Create and register output array
    fDigitArray = new TClonesArray("BmnTrigDigit");
    ioman->Register("BdDigit", "BD", fDigitArray, kTRUE);

    if (fVerbose)
        cout << "BmnBdDigitizer::Init() finished successfully\n";
    return kSUCCESS;
}

// Executes the digitization process for each event.
void BmnBdDigitizer::Exec(Option_t* opt)
{
    if (!IsActive())
        return;

    TStopwatch sw;
    sw.Start();

    // Check output array
    if (!fDigitArray)
        Fatal("Exec", "No BdDigit array");

    // Reset output array
    fDigitArray->Delete();
    fFiredChannels = 0;

    // Map for storing energies at modules
    map<Int_t, pair<Double_t, Double_t>> ampTimeMap;

    // Random number generator for noise
    TRandom3 rand(0);

    // Loop over MCPoints
    for (Int_t iPoint = 0; iPoint < fPointArray->GetEntriesFast(); iPoint++) {
        BmnBdPoint* point = (BmnBdPoint*)fPointArray->At(iPoint);
        if (!point)
            continue;

        Int_t mod = point->GetCopy() - 1;

        // Validate module number
        if (mod < 0 || mod >= fNMod) {
            if (fVerbose)
                cout << "-W- BmnBdDigitizer::Exec: Invalid module number " << mod << endl;
            continue;
        }

        // Add energy loss with noise
        Double_t energyLoss = point->GetEnergyLoss();
        Double_t noise = rand.Gaus(0, fNoiseSigma);
        Double_t totalEnergy = energyLoss + noise;
        Double_t time = point->GetTime();

        auto it = ampTimeMap.find(mod);
        if (it != ampTimeMap.end()) {
            it->second.first += totalEnergy;
            if (time < it->second.second)
                it->second.second = time;
        } else {
            ampTimeMap[mod] = {totalEnergy, time};
        }
    }

    // Create digits for modules with energy above threshold
    for (auto mit : ampTimeMap) {
        if (mit.second.first < fThreshold)
            continue;   // Skip signals below threshold

        BmnTrigDigit digi;
        digi.SetMod((Short_t)mit.first);
        Double_t duration = ConvertAmplitudeToTime(mit.second.first);
        digi.SetAmp(duration);
        digi.SetTime(mit.second.second);
        // digi.SetFiredChannels(fFiredChannels + 1);
        new ((*fDigitArray)[fDigitArray->GetEntriesFast()]) BmnTrigDigit(digi);
        fFiredChannels++;   // Increment fired channels counter
    }

    if (fVerbose) {
        // printf("Number of BmnBdPoints = %d\n", fPointArray->GetEntriesFast());
        // printf("Number of BmnBdDigits = %d\n", fDigitArray->GetEntriesFast());
        // printf("Number of BmnBdDigits = %d\n", fFiredChannels);
        // cout << "BmnBdDigitizer::Exec() finished\n";
    }

    sw.Stop();
    workTime += sw.RealTime();
}

// Finalizes the digitizer task.
void BmnBdDigitizer::Finish()
{
    printf("Work time of the BD digitizer: %.4f sec.\n", workTime);
}