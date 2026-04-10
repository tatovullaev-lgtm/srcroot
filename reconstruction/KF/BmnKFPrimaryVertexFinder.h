/** The BmnKFPrimaryVertexFinder class - adaptation of MpdKfPrimaryVertexFinder
 *
 * @author  A.Zinchenko
 * @version 1.0
 * @since   12.10.2023
 *
 * Class to find primary vertex with the Kalman Filter method
 *
 */
#ifndef BMNKFPRIMARYVERTEXFINDER_H
#define BMNKFPRIMARYVERTEXFINDER_H

#include "CbmKFTrackInterface.h"
#include "CbmKFVertexInterface.h"
#include "CbmVertex.h"
#include "FairTask.h"
#include "TClonesArray.h"

#include <TH1.h>
#include <TH3.h>   // AZ-300525
#include <TMatrixD.h>
#include <vector>

class BmnKFPrimaryVertexFinder : public FairTask
{
    std::vector<CbmKFTrackInterface*> fTracks;
    TClonesArray* fInputTracks;
    CbmVertex* fPrimVert;
    TH1D *fHist[2], *fHistw[2], *fHistPca, *fHistErr[2], *fHistZ, *fHistR;
    TH2D* fHZR;               // AZ-310525
    TH3D *fHist3, *fHistw3;   // AZ-300525
    Double_t fSigma[3];
    Double_t fMean[3];
    std::vector<int> fTrkID;   // AZ-090824 - track indices

  public:
    // AZ-151023 BmnKFPrimaryVertexFinder():fTracks() { Clear(); };
    BmnKFPrimaryVertexFinder()
        : fTracks()
        , fPrimVert(nullptr) {};
    ~BmnKFPrimaryVertexFinder()
    {
        if (fPrimVert)
            delete fPrimVert;
    };

    InitStatus Init();
    void Exec(Option_t* option);
    void Clear();
    void AddTrack(CbmKFTrackInterface* Track);
    void SetTracks(std::vector<CbmKFTrackInterface*>& vTracks);
    void EvalVertex();
    void Fit(CbmKFVertexInterface& vtx);
    // void ComputeAandB(TMatrixD &xk0, const CbmKFTrack &track, Double_t zhit, TMatrixD &a, TMatrixD &b, TMatrixD
    // &ck0);
    Bool_t ComputeAandB(TMatrixD& xk0,
                        CbmKFTrackInterface& track,
                        Double_t zhit,
                        TMatrixD& a,
                        TMatrixD& b,
                        TMatrixD& ck0);
    void FindPca(CbmKFTrackInterface& track, Double_t* vert);
    CbmVertex* GetVertex() const { return fPrimVert; }                      // AZ-250525
    void SetTrackArray(TClonesArray* trArray) { fInputTracks = trArray; }   // AZ-230825 - set track array

    ClassDef(BmnKFPrimaryVertexFinder, 0);
};

#endif /* !BMNKFPRIMARYVERTEXFINDER_H */
