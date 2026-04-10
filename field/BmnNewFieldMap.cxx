// -------------------------------------------------------------------------
// -----                    BmnNewFieldMap source file                 -----
// -----                   Created 03/02/2015  by P. Batyuk            -----
// -------------------------------------------------------------------------
#include "BmnNewFieldMap.h"

#include "BmnFieldPar.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "TArrayF.h"
#include "TMath.h"

using namespace TMath;

BmnNewFieldMap::BmnNewFieldMap()
    : BmnFieldMap()
{
    fType = 1;
}

BmnNewFieldMap::BmnNewFieldMap(const char* mapFileName)
    : BmnFieldMap(mapFileName)
{
    fType = 1;
}

BmnNewFieldMap::BmnNewFieldMap(BmnFieldPar* fieldPar)
    : BmnFieldMap(fieldPar)
{
    fType = 1;
}

BmnNewFieldMap::~BmnNewFieldMap() {}

Double_t BmnNewFieldMap::GetBx(Double_t x, Double_t y, Double_t z)
{
    return FieldInterpolate(fBx, x, y, z);
}

Double_t BmnNewFieldMap::GetBy(Double_t x, Double_t y, Double_t z)
{
    return FieldInterpolate(fBy, x, y, z);
}

Double_t BmnNewFieldMap::GetBz(Double_t x, Double_t y, Double_t z)
{
    return FieldInterpolate(fBz, x, y, z);
}

Bool_t BmnNewFieldMap::IsInside(Double_t x,
                                Double_t y,
                                Double_t z,
                                Int_t& ix,
                                Int_t& iy,
                                Int_t& iz,
                                Double_t& dx,
                                Double_t& dy,
                                Double_t& dz)
{
    // ---  Check for being outside the map range
    if (x < fXmin || x >= fXmax || y < fYmin || y >= fYmax || z < fZmin || z >= fZmax) {
        ix = iy = iz = 0;
        dx = dy = dz = 0.;
        return kFALSE;
    }

    // Relative distance from grid point (in units of cell size) and grid cell numbers

    Double_t xx, yy, zz;
    xx = (x - fXmin) / fXstep;
    yy = (y - fYmin) / fYstep;
    zz = (z - fZmin) / fZstep;
    ix = Int_t(xx);
    iy = Int_t(yy);
    iz = Int_t(zz);
    dx = xx - Double_t(ix);
    dy = yy - Double_t(iy);
    dz = zz - Double_t(iz);

    return kTRUE;
}

Double_t BmnNewFieldMap::FieldInterpolate(TArrayF* fcomp, Double_t x, Double_t y, Double_t z)
{
    Int_t ix = 0;
    Int_t iy = 0;
    Int_t iz = 0;
    Double_t dx = 0.;
    Double_t dy = 0.;
    Double_t dz = 0.;

    if (x < fXmin || x >= fXmax || y < fYmin || y >= fYmax || z < fZmin || z >= fZmax) {
        return 0.;
    }

    // Relative distance from grid point (in units of cell size) and grid cell numbers

    Double_t xx, yy, zz;
    xx = (x - fXmin) / fXstep;
    yy = (y - fYmin) / fYstep;
    zz = (z - fZmin) / fZstep;
    ix = Int_t(xx);
    iy = Int_t(yy);
    iz = Int_t(zz);
    dx = xx - Double_t(ix);
    dy = yy - Double_t(iy);
    dz = zz - Double_t(iz);

    Int_t ixyz1 = ix * fNy * fNz + iy * fNz + iz;
    Int_t ixyz2 = ixyz1 + fNy * fNz;
    Int_t ixyz3 = ixyz1 + fNz;
    Int_t ixyz4 = ixyz2 + fNz;

    fHa[0][0][0] = fcomp->At(ixyz1);
    fHa[1][0][0] = fcomp->At(ixyz2);
    fHa[0][1][0] = fcomp->At(ixyz3);
    fHa[1][1][0] = fcomp->At(ixyz4);
    fHa[0][0][1] = fcomp->At(ixyz1 + 1);
    fHa[1][0][1] = fcomp->At(ixyz2 + 1);
    fHa[0][1][1] = fcomp->At(ixyz3 + 1);
    fHa[1][1][1] = fcomp->At(ixyz4 + 1);

    return Interpolate(dx, dy, dz);
}

void BmnNewFieldMap::FillParContainer()
{
    TString MapName = GetName();
    LOG(debug) << "BmnNewFieldMap::FillParContainer() ";
    FairRun* fRun = FairRun::Instance();
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    // Bool_t kParameterMerged = kTRUE;
    BmnFieldPar* fieldPar = (BmnFieldPar*)rtdb->getContainer("BmnFieldPar");
    fieldPar->SetParameters(this);
    fieldPar->setInputVersion(fRun->GetRunId(), 1);
    fieldPar->setChanged();
}
