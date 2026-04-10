#ifndef BMNNDETPOINT_H
#define BMNNDETPOINT_H

#include "BmnNdetAddressMixin.h"
#include "FairMCPoint.h"
#include "TVector3.h"

#include <iostream>

using namespace std;

class BmnNdetPoint
    : public FairMCPoint
    , public BmnNdetAddressMixin<BmnNdetPoint>
{
  public:
    //! Default constructor
    BmnNdetPoint()
        : FairMCPoint()
        , fAddress(0)
    {
        fDetectorID = kNDET;
    }

    /*! Constructor with arguments
     *@param trackID  Index of MCTrack
     *@param address  see BmnNdetAddress
     *@param pos      Coordinates  [cm]
     *@param mom      Momentum of track [GeV]
     *@param tof      Time of flight [ns]
     *@param length   Track length since creation [cm]
     *@param eLoss    Energy deposit [GeV]
     */
    BmnNdetPoint(Int_t trackID,
                 uint32_t address,
                 const TVector3& pos,
                 const TVector3& mom,
                 Double_t tof,
                 Double_t length,
                 Double_t eLoss,
                 UInt_t EventId = 0)
        : FairMCPoint(trackID, kNDET, pos, mom, tof, length, eLoss, EventId)
        , fAddress(address)
    {}

    //! Copy constructor
    BmnNdetPoint(const BmnNdetPoint& point)
        : FairMCPoint(point)
        , fAddress(point.fAddress)
    {}

    //! Move constructor
    BmnNdetPoint(BmnNdetPoint&& point) noexcept
        : FairMCPoint(std::move(point))
        , fAddress(point.fAddress)
    {
        point.fAddress = 0;
    }

    //! Copy assignment operator
    BmnNdetPoint& operator=(const BmnNdetPoint& point)
    {
        if (this != &point) {
            FairMCPoint::operator=(point);
            fAddress = point.fAddress;
        }
        return *this;
    }

    //! Move assignment operator
    BmnNdetPoint& operator=(BmnNdetPoint&& point) noexcept
    {
        if (this != &point) {
            FairMCPoint::operator=(std::move(point));
            fAddress = point.fAddress;
            point.fAddress = 0;
        }
        return *this;
    }

    //! Destructor
    virtual ~BmnNdetPoint() = default;

    uint32_t GetAddress() const { return fAddress; }
    void SetAddress(uint32_t address) { fAddress = address; }

    //! Output to screen
    virtual const char* GetClassName() const { return "BmnNdetPoint"; }
    virtual void Print(const Option_t* opt = "") const
    {
        printf("%s: %s\n", GetClassName(), BmnNdetAddress::GetInfoString(fAddress).c_str());
        printf("    track %d\n", GetTrackID());
        printf("    Position (%.2f, %.2f, %.2f) cm\n", GetX(), GetY(), GetZ());
        printf("    Momentum (%.2f, %.2f, %.2f) GeV\n", GetPx(), GetPy(), GetPz());
        printf("    Time %.2f ns, Length %.2f cm\n", GetTime(), GetLength());
        printf("    Energy loss %.6f GeV\n", GetEnergyLoss());
    }

  private:
    uint32_t fAddress;

    ClassDef(BmnNdetPoint, 5)
};

#endif /* BMNNDETPOINT_H */
