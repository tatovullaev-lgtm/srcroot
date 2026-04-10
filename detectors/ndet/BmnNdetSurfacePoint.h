#pragma once

#include "FairMCPoint.h"

class BmnNdetSurfacePoint : public FairMCPoint
{
    int fPdg = 0;
    uint8_t fArmID = 0;
    uint8_t fPlaneID = 0;
    uint8_t fDirectionID = 0;

  public:
    BmnNdetSurfacePoint() = default;

    BmnNdetSurfacePoint(int trackID,
                        const TVector3& pos,
                        const TVector3& mom,
                        double time,
                        double length,
                        int pdg,
                        unsigned eventID,
                        uint8_t armID,
                        uint8_t planeID,
                        uint8_t directionID)
        : FairMCPoint(trackID, kNDET, pos, mom, time, length, 0.0, eventID)
        , fPdg(pdg)
        , fArmID(armID)
        , fPlaneID(planeID)
        , fDirectionID(directionID)
    {}

    int GetPdg() const { return fPdg; }
    uint8_t GetArmID() const { return fArmID; }
    uint8_t GetPlaneID() const { return fPlaneID; }
    uint8_t GetDirectionID() const { return fDirectionID; }

    ClassDef(BmnNdetSurfacePoint, 3)
};
