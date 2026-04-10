/********************************************************************************
 *    BmnMCInfoDst.cxx                                                          *
 *******************************************************************************/
#include "BmnMCInfoDst.h"

// Default constructor
BmnMCInfoDst::BmnMCInfoDst()
    : TNamed("BmnMCInfoDST", "MC")
    , fB(0.)
    , fX(0.)
    , fY(0.)
    , fZ(0.)
    , fRotX(0.)
    , fRotY(0.)
    , fRotZ(0.)
{
    fMCTracks = new TClonesArray("CbmMCTrack");
}

// Copy constructor
BmnMCInfoDst::BmnMCInfoDst(const BmnMCInfoDst& other)
    : TNamed("BmnMCInfoDST", "MC")
    , fB{other.fB}
    , fX(other.fX)
    , fY(other.fY)
    , fZ(other.fZ)
    , fRotX(other.fRotX)
    , fRotY(other.fRotY)
    , fRotZ(other.fRotZ)
{
    if (other.fMCTracks)
        fMCTracks = new TClonesArray(*other.fMCTracks);
    else
        fMCTracks = nullptr;
}

// Copy assignment operator
BmnMCInfoDst& BmnMCInfoDst::operator=(const BmnMCInfoDst& other)
{
    if (&other != this) {
        fB = other.fB;
        fX = other.fX;
        fY = other.fY;
        fZ = other.fZ;
        fRotX = other.fRotX;
        fRotY = other.fRotY;
        fRotZ = other.fRotZ;

        // delete the previous fMCTrack
        if (fMCTracks) {
            fMCTracks->Delete();
            delete fMCTracks;
        }
        if (other.fMCTracks)
            fMCTracks = new TClonesArray(*other.fMCTracks);
        else
            fMCTracks = nullptr;
    }
    return *this;
}

// Move constructor
BmnMCInfoDst::BmnMCInfoDst(BmnMCInfoDst&& other)
    : TNamed("BmnMCInfoDST", "MC")
    , fB{other.fB}
    , fX(other.fX)
    , fY(other.fY)
    , fZ(other.fZ)
    , fRotX(other.fRotX)
    , fRotY(other.fRotY)
    , fRotZ(other.fRotZ)
{
    fMCTracks = std::exchange(other.fMCTracks, nullptr);
}

// Move assignment operator
BmnMCInfoDst& BmnMCInfoDst::operator=(BmnMCInfoDst&& other)
{
    fB = other.fB;
    fX = other.fX;
    fY = other.fY;
    fZ = other.fZ;
    fRotX = other.fRotX;
    fRotY = other.fRotY;
    fRotZ = other.fRotZ;

    // delete the previous fMCTrack
    if (fMCTracks) {
        fMCTracks->Delete();
        delete fMCTracks;
    }
    fMCTracks = std::exchange(other.fMCTracks, nullptr);

    return *this;
}

void BmnMCInfoDst::SetVertex(Double_t x, Double_t y, Double_t z)
{
    fX = x;
    fY = y;
    fZ = z;
}

void BmnMCInfoDst::Reset()
{
    fX = fY = fZ = fB = 0.;
    fRotX = fRotY = fRotZ = 0.;
    if (fMCTracks)
        fMCTracks->Delete();
}

// Destructor
BmnMCInfoDst::~BmnMCInfoDst()
{
    if (fMCTracks) {
        fMCTracks->Delete();
        delete fMCTracks;
    }
}
