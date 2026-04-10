// -------------------------------------------------------------------------
// -----                 BmnNdetDigitizer header file                    -----
// -----                 Created 14/08/06  by S.Spataro                   -----
// -----                 Modified by M.Golubeva July 2022                -----
// -------------------------------------------------------------------------

#ifndef BMNNDETDIGITIZER_H
#define BMNNDETDIGITIZER_H 1

#include "BmnNdetDigit.h"
#include "BmnNdetPoint.h"
#include "FairLogger.h"
#include "FairTask.h"
#include "TClonesArray.h"

#include <TRandom3.h>
#include <iostream>
#include <unordered_map>
#include <vector>

class BmnNdetDigitizer : public FairTask
{
  public:
    struct SiPM
    {
        int pixPerMIP;      // MIP to N SiPM pixels
        double noiseMIP;    // MIP noise level
        double gevPerMIP;   // MIP to GeV conversion factor
        TRandom3* random;   // Random number generator

        SiPM()
            : random(new TRandom3())
        {}
        ~SiPM() { delete random; }

        double ModelSiPM(double pfELoss)
        {
            double eMIP = pfELoss / gevPerMIP;
            Long64_t NPix = random->Poisson(eMIP * pixPerMIP);
            double eMIP_smeared = static_cast<double>(NPix) / pixPerMIP;
            double noise = random->Gaus(0, noiseMIP);
            eMIP_smeared += noise;
            return eMIP_smeared * gevPerMIP;
        }
    };

    /** Default constructor **/
    BmnNdetDigitizer(const char* name = "BmnNdet Digi Producer");

    /** Destructor **/
    virtual ~BmnNdetDigitizer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    // Setters for various parameters
    inline void SetTofCut(double setValue) { fTofCut = setValue; }
    inline void SetThreshold(double setValue) { fThreshold = setValue; }
    inline void SetSiPM(int pixPerMIP, double noiseMIP, double gevPerMIP)
    {
        fSiPM.pixPerMIP = pixPerMIP;
        fSiPM.noiseMIP = noiseMIP;
        fSiPM.gevPerMIP = gevPerMIP;
    }

  private:
    double fTofCut;      // [ns] cut to remove parasitic late tracks
    double fThreshold;   // [GeV] noise threshold level
    SiPM fSiPM;          // SiPM properties and methods

    TClonesArray* fPointArray;   // Input array of BmnNdetPoints
    TClonesArray* fDigiArray;    // Output array of BmnNdetDigits

    std::unordered_map<uint32_t, std::vector<BmnNdetPoint*>> fuoHitMap;   // Address to hit vector of different tracks
    void FillHitMap();

    ClassDef(BmnNdetDigitizer, 4);
};

#endif   // BMNNDETDIGITIZER_H