#include "BmnVSPHit.h"

BmnVSPHit::BmnVSPHit() : BmnHit() {
    //fModule = -1;
    fELoss  = 0.0;
    fLowerLayerClusterSize = 0;
    fUpperLayerClusterSize = 0;
    fLowerLayerStripPos = -1.0;
    fUpperLayerStripPos = -1.0;
}

BmnVSPHit::BmnVSPHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: BmnHit(detID, pos, dpos, index) {
    //fModule = -1;
    fELoss = 0.0;
    fLowerLayerClusterSize = 0;
    fUpperLayerClusterSize = 0;
    fLowerLayerStripPos = -1.0;
    fUpperLayerStripPos = -1.0;
    fPdg = -1;
}

BmnVSPHit::~BmnVSPHit()
{}
