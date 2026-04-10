#include "BmnCombineVertexFinder.h"

#include "BmnMath.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "vector"

using namespace std;
using namespace TMath;

BmnCombineVertexFinder::~BmnCombineVertexFinder() { delete fKalman; }

InitStatus BmnCombineVertexFinder::Init() {
    if (fVerbose > 1)
        cout << "=========================== Combined Vertex finder init started ====================" << endl;

    fKalman = new BmnKalmanFilter();

    // Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (nullptr == ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    fCbmVertex = (CbmVertex*)ioman->GetObject("PrimaryVertex.");  // in
    if (!fCbmVertex) {
        cout << "BmnCombineVertexFinder::Init(): branch PrimaryVertex not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fMpdVertex = (CbmVertex*)ioman->GetObject("MpdVertex.");  // in
    if (!fMpdVertex) {
        cout << "BmnCombineVertexFinder::Init(): branch MpdVertex not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBeamTrackArray = (TClonesArray*)ioman->GetObject("BmnBeamTrack");  // in
    if (!fBeamTrackArray) {
        cout << "BmnCombineVertexFinder::Init(): branch BmnBeamTrack not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fCombineVertexArray = new TClonesArray("CbmVertex", 2);  // out
    ioman->Register("CombineVertex", "VERTEX", fCombineVertexArray, kTRUE);

    if (fVerbose > 1) cout << "=========================== Combined Vertex finder init finished ===================" << endl;

    return kSUCCESS;
}

void BmnCombineVertexFinder::Exec(Option_t* opt) {

    if (!IsActive())
        return;

    if (fVerbose > 1) {
        cout << "======================== Combined Vertex finder exec started  ======================" << endl;
    }

    fCombineVertexArray->Delete();

    CbmVertex combVertexCbm = *fCbmVertex;
    CbmVertex combVertexMpd = *fMpdVertex;

    BmnTrack* beamTrack = nullptr;
    beamTrack = (BmnTrack*)fBeamTrackArray->At(0);
    if (beamTrack != nullptr) {
        combVertexCbm = CombineVertex(fCbmVertex, beamTrack);
        combVertexMpd = CombineVertex(fMpdVertex, beamTrack);
    }

    new ((*fCombineVertexArray)[0]) CbmVertex(combVertexCbm);
    new ((*fCombineVertexArray)[1]) CbmVertex(combVertexMpd);

    if (fVerbose > 1)
        cout << "\n======================== Combined Vertex finder exec finished ======================" << endl;
}

CbmVertex BmnCombineVertexFinder::CombineVertex(CbmVertex* vrtx, BmnTrack* beamTrack) {
    Double_t weightPrimaryVertex[2] = { 0.0, 0.0 };
    Double_t weightBeam[2] = { 0.0, 0.0 };

    FairTrackParam par = *(beamTrack->GetParamLast());
    fKalman->TGeoTrackPropagate(&par, vrtx->GetZ(), 2212, nullptr, nullptr, 1);

    Double_t primaryVertexX = vrtx->GetX();
    Double_t primaryVertexY = vrtx->GetY();

    weightPrimaryVertex[0] = 1 / vrtx->GetCovariance(0, 0);
    weightPrimaryVertex[1] = 1 / vrtx->GetCovariance(1, 1);
    weightBeam[0] = 1 / par.GetCovariance(0, 0);
    weightBeam[1] = 1 / par.GetCovariance(1, 1);
    Double_t beamVertexX = par.GetX();
    Double_t beamVertexY = par.GetY();

    Double_t X = (primaryVertexX * weightPrimaryVertex[0] + beamVertexX * weightBeam[0]) / (weightPrimaryVertex[0] + weightBeam[0]);
    Double_t Y = (primaryVertexY * weightPrimaryVertex[1] + beamVertexY * weightBeam[1]) / (weightPrimaryVertex[1] + weightBeam[1]);
    Double_t Z = vrtx->GetZ();
    Double_t chi2 = vrtx->GetChi2();
    Int_t NTracks = vrtx->GetNTracks() + 1;
    Int_t NDF = NTracks * 2;
    TMatrixFSym* covMat = new TMatrixFSym(3);
    vrtx->CovMatrix(*covMat);

    CbmVertex combVrtx;
    combVrtx.SetVertex(X, Y, Z, chi2, NDF, NTracks, *covMat);
    return combVrtx;
}


void BmnCombineVertexFinder::Finish() {}

