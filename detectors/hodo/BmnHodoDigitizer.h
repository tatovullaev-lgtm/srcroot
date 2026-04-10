#ifndef BMNHODODIGITIZER_H
#define BMNHODODIGITIZER_H

#include "BmnHodoDigit.h"
#include "BmnHodoPoint.h"
#include "BmnHodoResolution.h"
#include "FairLogger.h"
#include "FairTask.h"
#include "TClonesArray.h"

#include <TF1.h>
#include <TRandom3.h>
#include <unordered_map>
#include <vector>

using BmnHodoResolution::Kernel;

class BmnHodoDigitizer : public FairTask
{
  public:
    // Constructor and Destructor
    BmnHodoDigitizer(const char* name = "BmnHodo Digi Producer");
    virtual ~BmnHodoDigitizer();

    // Overridden FairTask methods
    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    // Setter methods
    void SetScale(double scale) { fScale = scale; }
    void SetThreshold(double threshold) { fThreshold = threshold; }
    void SetSaturation(double satur) { fSaturation = satur; }
    void SetConfig(const string& config);

  private:
    double fScale;        // Scale factor [Z^2/GeV]
    double fThreshold;    // Noise threshold level [Z^2]
    double fSaturation;   // Saturation [adc]

    TClonesArray* fPointArray;   // Input array of BmnHodoPoints
    TClonesArray* fDigiArray;    // Output array of BmnHodoDigits

    std::unordered_map<uint32_t, std::vector<BmnHodoPoint*>> fuoHitMap;   // strip to point vector map
    std::unordered_map<uint32_t, std::pair<double, Kernel>>
        fuoResolutionMap;   // Address to pair calib & resolution function
    void FillHitMap();
    void ParseConfig(const string& config);

    float fworkTime;   // Accumulated work time for the digitizer
    ClassDef(BmnHodoDigitizer, 5);
};

#endif /* BMNHODODIGITIZER_H */
