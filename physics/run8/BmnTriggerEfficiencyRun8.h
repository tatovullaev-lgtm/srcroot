#ifndef BMNTRIGEFFRUN8_H
#define BMNTRIGEFFRUN8_H 1

#include "BmnTrigInfoDst.h"
#include "CbmVertex.h"

#include <TChain.h>
#include <TFile.h>
#include <TH2D.h>
#include <TNamed.h>
#include <TString.h>
#include <TTree.h>

using namespace std;

class BmnTriggerEfficiencyRun8 : public TNamed
{
  public:
    BmnTriggerEfficiencyRun8();
    BmnTriggerEfficiencyRun8(TString dstFileName);
    BmnTriggerEfficiencyRun8(Int_t runId, BmnTrigInfoDst* trigInfo, CbmVertex* primaryVertex);

    Bool_t IsValid() { return isValid; }
    Int_t GetNBd() { return fNBd; }

    Double_t GetBDEfficiency(Int_t runId, Int_t ntrPV);
    Double_t GetFDEfficiency(Int_t runId, Int_t ntrPV);
    Double_t GetTriggerEfficiency(Int_t runId, Int_t ntrPV);

    // Input: ntrPV = PrimaryVertex.GetNTracks() from dst
    // Returns true on success, false - error
    // Returns efficiency and errors via arguments:
    // eff - efficiency of BD*FD, statErr - statistical error,
    // systErr - systematic error, errSystErr - error of systematic error
    //
    // See use example in macro/physics/trigger/CheckTriggerEfficiencyInterfaceRun8.C
    Bool_t GetTriggerEfficiencyWithErrors(Int_t runId,
                                          Int_t ntrPV,
                                          Double_t& eff,
                                          Double_t& statErr,
                                          Double_t& systErr,
                                          Double_t& errSystErr);
    Bool_t GetTriggerEfficiencyWithErrors(Double_t& eff, Double_t& statErr, Double_t& systErr, Double_t& errSystErr);

    // Returns true on success, false - error
    // Returns flux rejection factor due pile up (BC2) cut and its error via arguments
    //
    // See use example in macro/physics/trigger/CheckTriggerEfficiencyInterfaceRun8.C
    Bool_t GetFluxRejectionFactorWithError(Int_t runId, Double_t& factor, Double_t& err);
    Bool_t GetFluxRejectionFactorWithError(Double_t& factor, Double_t& err);
    Bool_t IsEventAnalysable();
    Bool_t IsEventAnalysable(Int_t evId);

    virtual ~BmnTriggerEfficiencyRun8()
    {
        if (fEffFile1)
            delete fEffFile1;
        if (fEffFile2)
            delete fEffFile2;
        if (fEffFile3)
            delete fEffFile3;
        if (fDstFile)
            delete fDstFile;
    }

  private:
    const Int_t FDTrigWindow = 4;

    Bool_t isValid;
    Int_t fNBd;
    TFile* fEffFile1;
    TFile* fEffFile2;
    TFile* fEffFile3;
    TH2D* fHBd;
    TH2D* fHFd;
    TH2D* fHCctSyst;
    TH1D* fHFrf;

    TFile* fDstFile;
    TTree* fDstTree;
    BmnTrigInfoDst* fBTrigInfo;
    CbmVertex* fBPrimaryVertex;

    Int_t fRunId;
    Double_t fBC2IntCut;
    Int_t fFDTrigWindowLeftEdge;
    Int_t fFDPeakLimit;

    Double_t GetEfficiencyOrError(TH2D* hist, Int_t runId, Int_t ntrPV, Bool_t isEff);
    void InitEfficiencyInput();
    void SetBC2IntCutByRun(Int_t runId);
    void SetFDTrigWindowLeftEdgeByRun(Int_t runId);
    void SetFDPeakLimitByRun(Int_t runId);
    Int_t GetBC2Int(Int_t leftEl, Int_t rightEl);
    Bool_t IsBDTriggered();
    Bool_t IsFDTriggered();

    ClassDef(BmnTriggerEfficiencyRun8, 3)
};

#endif
