
#include "BmnTof700Hit.h"

BmnTof700Hit::BmnTof700Hit() : BmnHit() {
 }

BmnTof700Hit::BmnTof700Hit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: BmnHit(detID, pos, dpos, index){
}

BmnTof700Hit::~BmnTof700Hit() {
 }

ClassImp(BmnTof700Hit)