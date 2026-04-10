#ifndef BMNFHCALDIGITIZER_H
#define BMNFHCALDIGITIZER_H

#include "BmnFHCalDigit.h"
#include "BmnFHCalPoint.h"
#include "FairLogger.h"
#include "FairTask.h"
#include "TClonesArray.h"

#include <TRandom3.h>
#include <iostream>
#include <unordered_map>
#include <vector>

class BmnFHCalDigitizer : public FairTask
{
  public:
    struct SiPM
    {
        int Npixels;        // Total number of pixels in SiPM (for saturation)
        int pixPerMIP;      // MIP to N SiPM pixels with PDE taken into account
        double noiseMIP;    // MIP noise level
        double gevPerMIP;   // MIP to GeV conversion factor

        SiPM() {}
        ~SiPM() {}

        ULong64_t ModelNpixels(double meanPixels) { return gRandom->Poisson(meanPixels); }

        ULong64_t ModelSaturation(ULong64_t firedPixels)
        {
            return static_cast<ULong64_t>(
                std::round(Npixels * (1.0 - std::exp(-static_cast<double>(firedPixels) / Npixels))));
        }

        double ModelNoise(double meanMIP) { return gRandom->Gaus(meanMIP, noiseMIP); }

        double ModelResponse(double pfELoss)
        {
            auto meanPixels = (pfELoss / gevPerMIP) * pixPerMIP;
            auto firedPixels = ModelNpixels(meanPixels);
            firedPixels = ModelSaturation(firedPixels);
            LOG(debug2) << "Modeled SiPM fired pixels " << firedPixels;
            return gevPerMIP * ModelNoise(static_cast<double>(firedPixels) / pixPerMIP);
        }
    };

    // Constructor and Destructor
    BmnFHCalDigitizer(const char* name = "BmnFHCal Digi Producer");
    virtual ~BmnFHCalDigitizer();

    // Overridden FairTask methods
    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    // Setter methods
    inline void SetScale(double scale) { fScale = scale; }
    inline void SetThreshold(double setValue) { fThreshold = setValue; }
    inline void SetTimeCut(double setValue) { fTimeCut = setValue; }
    inline void SetSiPM(int Npixels, int pixPerMIP, double noiseMIP, double gevPerMIP)
    {
        fSiPM.Npixels = Npixels;
        fSiPM.pixPerMIP = pixPerMIP;
        fSiPM.noiseMIP = noiseMIP;
        fSiPM.gevPerMIP = gevPerMIP;
    }

  private:
    double fScale;       // Scale factor
    double fThreshold;   // Noise threshold level
    double fTimeCut;     // Cut times above this
    SiPM fSiPM;          // SiPM properties and methods

    TClonesArray* fPointArray;   // Input array of BmnFHCalPoints
    TClonesArray* fDigiArray;    // Output array of BmnFHCalDigits

    std::unordered_map<uint32_t, std::vector<BmnFHCalPoint*>>
        fuoHitMap;   // Physical address (w/o scint) to point vector map
    void FillHitMap();

    float fworkTime;   // Accumulated work time for the digitizer
    ClassDef(BmnFHCalDigitizer, 2);
};

#endif   // BMNFHCALDIGITIZER_H
