// -------------------------------------------------------------------------
// -----                BmnFieldPoint header file                      -----
// -----               Created 28/06/22 by M.Mamaev, S.Merts           -----
// -------------------------------------------------------------------------

#ifndef BMN_FIELD_POINT_H_
#define BMN_FIELD_POINT_H_

#include "Rtypes.h"
#include "TMath.h"

using namespace TMath;

// The basic structure to store the field point
class BmnFieldPoint {

public:

  BmnFieldPoint() {}
  ~BmnFieldPoint() {}
  BmnFieldPoint(Double_t x, Double_t y, Double_t z, Double_t bx, Double_t by, Double_t bz) {
    fX = x;
    fY = y;
    fZ = z;
    fBx = bx;
    fBy = by;
    fBz = bz;
  }

  BmnFieldPoint& operator=(BmnFieldPoint& right) {
    fX = right.GetX();
    fY = right.GetY();
    fZ = right.GetZ();
    fBx = right.GetBx();
    fBy = right.GetBy();
    fBz = right.GetBz();
    return *this;
  }

  Double_t GetX() { return fX; }
  Double_t GetY() { return fY; }
  Double_t GetZ() { return fZ; }
  Double_t GetBx() { return fBx; }
  Double_t GetBy() { return fBy; }
  Double_t GetBz() { return fBz; }

private:
  // coordinates (mm)
  Double_t fX;
  Double_t fY;
  Double_t fZ;
  // magnetic field (Tm)
  Double_t fBx;
  Double_t fBy;
  Double_t fBz;
};



#endif // BMN_FIELD_POINT_H_
