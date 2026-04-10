#ifndef BMNTOCBMHITCONVERTER_H
#define BMNTOCBMHITCONVERTER_H 1

#include "BmnGemStripHit.h"
#include "BmnGemStripStationSet.h"
#include "BmnSiliconHit.h"
#include "BmnSiliconStationSet.h"
#include "BmnTask.h"
#include "BmnVSPStationSet.h"
#include "CbmStsHit.h"
#include "FairMCPoint.h"
#include "TClonesArray.h"
#include "TSystem.h"

#include <iostream>

class BmnToCbmHitConverter : public BmnTask
{
  public:
    BmnToCbmHitConverter();
    // BmnToCbmHitConverter(Int_t iVerbose);
    BmnToCbmHitConverter(Int_t iVerbose, Bool_t isExp = kTRUE);   // AZ-170323

    virtual ~BmnToCbmHitConverter();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void SetGemConfigFile(TString file) { fGemConfigFile = file; }
    void SetSilConfigFile(TString file) { fSilConfigFile = file; }
    void SetVspConfigFile(TString file) { fVspConfigFile = file; }   // AZ-070625

    void SetFixedErrors(Float_t dXgem = 0.015, Float_t dYgem = 0.058, Float_t dXsil = 0.003, Float_t dYsil = 0.021);
    void ApplyAlignment(CbmStsHit* hit);   // AZ-120223

    // to disable alignment (used by default), call EnableAlignment(kFALSE)
    void EnableAlignment(Bool_t is_align_enabled = kTRUE) { fAlignmentEnabled = is_align_enabled; }

    void ReadCorrections(int irun,
                         int igem,
                         std::map<int, std::vector<Double_t>>* locCorX,
                         std::map<int, std::vector<Double_t>>* locCorY,
                         int& nx,
                         int& ny);
    Bool_t CorrectEffic(CbmStsHit* hit, int iq);                  // AZ-211223
    void CorrectHitMC(CbmStsHit* hit, Double_t pmom, int iq);     // AZ-270824
    void AddVspHits();                                            // AZ-060625
    void SetMCcorr(Bool_t mccorr = kTRUE) { fMCcorr = mccorr; }   // AZ-100625

    virtual void OnlineWrite(const std::unique_ptr<TTree>& resultTree);

  private:
    TString fBmnGemHitsBranchName;
    TString fBmnSilHitsBranchName;
    TString fBmnVspHitsBranchName;
    TString fCbmHitsBranchName;

    /** Input array of BNM GEM Hits **/
    TClonesArray* fBmnGemHitsArray;       //!
    TClonesArray* fBmnGemUpperClusters;   //!
    TClonesArray* fBmnGemLowerClusters;   //!
    TClonesArray* fGemPoints;             //! AZ-291223

    /** Input array of BNM Silicon Hits **/
    TClonesArray* fBmnSilHitsArray;       //!
    TClonesArray* fBmnSilUpperClusters;   //!
    TClonesArray* fBmnSilLowerClusters;   //!
    TClonesArray* fSilPoints;             //! AZ-291223

    /** AZ-060625 Input array of VSP Silicon Hits **/
    TClonesArray* fBmnVspHitsArray;       //!
    TClonesArray* fBmnVspUpperClusters;   //!
    TClonesArray* fBmnVspLowerClusters;   //!
    TClonesArray* fVspPoints;             //!

    TClonesArray* fMCTracks;   //! AZ-291223

    /** Output array of CBM Hits **/
    TClonesArray* fCbmHitsArray;   //!

    TString fGemConfigFile;
    TString fSilConfigFile;
    TString fVspConfigFile;

    BmnGemStripStationSet* GemStationSet;   //! Entire GEM detector
    BmnSiliconStationSet* SilStationSet;    //! Entire SILICON detector
    BmnVSPStationSet* fVspStationSet;       //! Entire VSP detector

    Bool_t fUseFixedErrors;
    Float_t fDXgem;
    Float_t fDYgem;
    Float_t fDXsil;
    Float_t fDYsil;
    Bool_t fExp;        // AZ-170323
    Bool_t fMCcorr;     // AZ-100625
    Bool_t fieldFlag;   // AZ-310823
    // whether alignment enabled
    Bool_t fAlignmentEnabled;

    ClassDef(BmnToCbmHitConverter, 0);
};

#endif
