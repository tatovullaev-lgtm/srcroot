//--------------------------------------------------------------------------------------------------------------------------------------
#include "BmnTOF1Point.h"

#include <iostream>

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1Point::BmnTOF1Point()
    : FairMCPoint()
{}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1Point::BmnTOF1Point(Int_t trackID,
                           Int_t detID,
                           TVector3 pos,
                           TVector3 mom,
                           Double_t tof,
                           Double_t length,
                           Double_t eLoss,
                           Int_t detUID)
    : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss)
{
    fDetectorUID = detUID;
}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1Point::~BmnTOF1Point() {}
//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF1Point::Print(const Option_t* opt) const
{
    cout << "-I- BmnTOF1Point: trackId " << fTrackID << " detectorUID " << fDetectorID;
    cout << "\n    Position (" << fX << ", " << fY << ", " << fZ << ") cm";
    cout << "\n    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV";
    cout << "\n    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06
         << " keV.\n";
}
//--------------------------------------------------------------------------------------------------------------------------------------
