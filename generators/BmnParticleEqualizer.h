// -------------------------------------------------------------------------
// -----                  BmnParticleEqualizer header file                 -----
// -----              Created 01/05/2024 by K. Gertsenberger           -----
// -------------------------------------------------------------------------

/** BmnParticleEqualizer.h
 ** @author Konstantin Gertsenberger <gertsen@jinr.ru>
 **
 ** The BmnParticleEqualizer add particles to equalize their number for all momentum intervals
 */

#ifndef BMNPARTICLEEQUALIZER_H
#define BMNPARTICLEEQUALIZER_H

#include "FairGenerator.h"
#include "FairPrimaryGenerator.h"
#include "TF1.h"
#include "TH1D.h"

#include <map>
#include <vector>
using namespace std;

class BmnParticleEqualizer : public FairGenerator
{
  public:
    /** Constructor with input histograms **/
    BmnParticleEqualizer(TString hist_file_name);
    /** Constructor with given PDG codes for particles to be equalized
    **@param pdg_codes Particles to be equalized (PDG codes)
    **@param min_p    Min value of momentum value
    **@param max_p    Max value of momentum value
    **@param intervals Interval count for momentum values to equalize particles at all the intervals independently
    **/
    BmnParticleEqualizer(TString hist_file_name,
                         vector<Int_t> pdg_codes,
                         Double_t min_p = 0.0,
                         Double_t max_p = 4.0,
                         Int_t intervals = 20);

    /** Destructor */
    virtual ~BmnParticleEqualizer() {}

    /** Initializer */
    Bool_t Init();
    /** Creates an event with equal number of particles at all momentum intervals
    **@param primGen   pointer to the FairPrimaryGenerator
    */
    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

    struct stParticleInfo
    {
        // Default 'stParticleInfo' constructor for storing particle distrubutons
        stParticleInfo(int pdg_key, Double_t min_p, Double_t max_p, Int_t interval_count);
        // 'stParticleInfo' destructor, empty becausse of using unique pointers
        ~stParticleInfo() {}

        vector<Int_t> fParticleNumber;   ///< PDG -> Particle Count (divided by momentum intervals -> vector)

        unique_ptr<TH1D> fPHist;       ///< PDG -> P histogram (TH1D) for the particle type
        unique_ptr<TH1D> fPtHist;      ///< PDG -> Pt histogram (TH1D) for the particle type
        unique_ptr<TH1D> fYHist;       ///< PDG -> Y (rapidity) histogram (TH1D) for the particle type
        unique_ptr<TH1D> fEtaHist;     ///< PDG -> Eta (pseudorapidity) histogram (TH1D) for the particle type
        unique_ptr<TH1D> fThetaHist;   ///< PDG -> Theta (polar angle) histogram (TH1D) for the particle type
        unique_ptr<TH1D> fPhiHist;     ///< PDG -> Phi (azimuth angle) histogram (TH1D) for the particle type

        vector<unique_ptr<TH1D>>
            fPHistVector;   ///< P histograms for the particle type (divided by momentum intervals -> vector)
        vector<unique_ptr<TH1D>>
            fEtaHistVector;   ///< Eta histograms for the particle type (divided by momentum intervals -> vector)

        Double_t fY0;      ///< Mean rapidity
        Double_t fSigma;   ///< Sigma in the rapidity distribution
        Double_t fT;       ///< Temperature in the P distribution
    };

    // static function to form sample histograms for equalizing the particle distributions
    static int ProduceSampleHistograms(TString input_list_file,
                                       TString output_histo_file = "$VMCWORKDIR/macro/recotools/particle_hists.root");

    // fit P. Pt, Y, Eta histograms to generate the values according to the analytical functions
    static int FitPHistogram(unique_ptr<TH1D>& hP, int pdg_key, Double_t p_min, Double_t p_max);
    static int FitPtHistogram(unique_ptr<TH1D>& hPt,
                              int pdg_key,
                              Double_t pdg_mass,
                              Double_t pt_min,
                              Double_t pt_max,
                              Double_t& t_output);
    static int FitYHistogram(unique_ptr<TH1D>& hY, int pdg_key, Double_t& y0_output, Double_t& sigma_putput);
    static int FitEtaHistogram(unique_ptr<TH1D>& hEta, int pdg_key, Double_t eta_min, Double_t eta_max);

    /** additional functions to check results of fitting and particle equalizing **/
    // static function to fit the obtained sample histograms (to show whether the results are appropriate)
    static void FitSampleHistograms(TString input_histo_file = "$VMCWORKDIR/macro/recotools/particle_hists.root");
    // static function to show the obtained sample histograms with obtained fit (to check whether the results are
    // appropriate)
    static int ShowSampleHistograms(TString input_histo_file = "$VMCWORKDIR/macro/recotools/particle_hists.root");
    // draw histogram with multiplicity of different particle types for all the momentum intervals
    // as well as P, Pt, Y, Eta, Theta, Phi distributions after the equalizing
    // it work both for SIM and DST files listed in the given text file ('input_list_file' var)
    static int ShowResultDistributions(TString input_list_file);

  private:
    static Double_t fPMin;         ///< Min value of momentum
    static Double_t fPMax;         ///< Max value of momentum
    static Int_t fIntervalCount;   ///< Interval count for momentum to equalize particles at all the intervals
    Double_t fIntervalStep;        ///< Interval step

    // Whether correct input data for the generator is obtained (if false, it will be skipped)
    Bool_t isActive;
    // Particle PDG codes to be equalized
    static vector<Int_t> fParticles;
    // map with the full information on all the particle types
    map<Int_t, unique_ptr<stParticleInfo>> fParticleInfos;
    // Maximum number of a particle type among all particles (divided by P intervals -> vector)
    vector<int> fMaxParticles;

    // Get all sample histograms for all the particle types from the 'input_histo_file' file
    // and calculate fit parameters (if necessary) according to the given histograms
    Bool_t ReadSampleHistograms(TString input_histo_file = "$VMCWORKDIR/macro/recotools/particle_hists.root");

    // define PDG code of a particle by the given mass
    // among particle types set by 'vecParticlePDG' vector or in the TDatabasePDG if the vector is empty
    // tolerance: relative if tolerance > 0, absolute (in GeV) in case of tolerance <= 0
    // return PDG code or 0 if no corresponding particle was found
    static Int_t GetPDGCodeByMass(Double_t mass, Double_t tolerance = 1.e-6, const vector<int>& vecParticlePDG = {});

    ClassDef(BmnParticleEqualizer, 1);
};

#endif
