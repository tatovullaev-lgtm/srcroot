#ifndef BMNMOTHERFITTERPART_H
#define BMNMOTHERFITTERPART_H

/// \ingroup rec
/// \class BmnMotherFitterPart
/// \brief Kalman filter mother particle fitter for the BM@N detector
/// \brief (using BmnParticle)
///
/// \author Alexander Zinchenko, LHEP JINR Dubna

// class MpdKalmanTrack;
class CbmStsTrack;
class CbmVertex;
// class MpdKalmanFilter;
class CbmKF;
class TVector3;
#include "BmnParticle.h"
#include "FairTask.h"

#include <TH1D.h>
#include <TMatrixD.h>
#include <vector>
using std::vector;

class TClonesArray;

class BmnMotherFitterPart : public FairTask
{

  public:
    static BmnMotherFitterPart* Instance();   ///< get singleton instance
    static BmnMotherFitterPart* Instance(const char* name,
                                         const char* title = "FAIR Task");   ///< get singleton instance

    virtual void Exec(Option_t* option);

    void Reset();
    void Register();

    // virtual InitStatus ReInit();
    // void WeightAtDca(MpdParticle *part, MpdKalmanTrack &track, Double_t *vert); // obtain MpdParticle weight at DCA
    Double_t BuildMother(
        BmnParticle* mother,
        vector<BmnParticle*>& vDaught);   // build mother from daughters
                                          // Double_t  BuildMother(BmnParticle *mother, vector<CbmStsTrack *> &vTracks,
                                          // vector<BmnParticle *> &vDaught);
    Double_t FindVertex(vector<BmnParticle*> vDaught, TVector3& vtx);   // find vertex from daughter tracks
    Bool_t ComputeAandB(const TMatrixD& xk0,
                        const BmnParticle& part,
                        TMatrixD& a,
                        TMatrixD& b,
                        TMatrixD& ck0,
                        Bool_t flag = kTRUE);   // compute matrices of derivatives
    Double_t FieldConst() const { return fieldConst; }
    TMatrixD ComputeQmatr(vector<BmnParticle*> vDaught);
    TMatrixD& GetCovariance() { return fCovar; }                    // access to covariance matrix
    Double_t Chi2Vertex(BmnParticle* part, const CbmVertex* vtx);   ///< compute Chi2 w.r.t. vertex

  protected:
    virtual InitStatus Init();
    virtual InitStatus ReInit();
    virtual void Finish();
    virtual ~BmnMotherFitterPart();   ///< Destructor

  private:
    BmnMotherFitterPart();                                                    ///< Default ctor
    BmnMotherFitterPart(const char* name, const char* title = "FAIR Task");   ///< Ctor
    static void DestroyInstance()
    {
        if (fgMF)
            delete fgMF;
    }   // automatic deleting
    Bool_t EvalVertex(vector<BmnParticle*> vDaught);   // evaluate vertex from daughter tracks
    Bool_t Smooth(vector<BmnParticle*> vDaught);       // smooth (update track momenta and track lengths)
    Bool_t ComputeAB0(const TMatrixD& xk0,
                      const BmnParticle& part,
                      TMatrixD& a,
                      TMatrixD& b,
                      TMatrixD& ck0,
                      Bool_t flag);   // compute matrices of derivatives for neutrals
    TVector3 Parabola(TVector3& pos0, TVector3& pos1, TVector3& pos2);

  private:
    static BmnMotherFitterPart* fgMF;
    Double_t fieldConst;   ///< mag. field constant
    Double_t fVtx[3];      ///< decay vertex position
    TMatrixD fCovar;       ///< covariance matrix
    TH1D* fTestHist;       ///< histogram for testing

    // MpdKalmanFilter *fKF;
    CbmKF* fKF;

    ClassDef(BmnMotherFitterPart, 0);
};
#endif
