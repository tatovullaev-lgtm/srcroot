#ifndef BMNFHCALPOINT_H
#define BMNFHCALPOINT_H

#include "BmnFHCalAddress.h"
#include "FairMCPoint.h"
#include "TVector3.h"

#include <iostream>

class BmnFHCalPoint : public FairMCPoint
{
  public:
    //! Default constructor
    BmnFHCalPoint()
        : FairMCPoint()
    {
        SetDetectorID(kFHCAL);
    }

    /*! Constructor with arguments
     * @param trackID         Index of MCTrack
     * @param address         Unique address (see BmnFHCalAddress)
     * @param pos             Coordinates [cm]
     * @param mom             Momentum of track [GeV]
     * @param tof             Time of flight [ns]
     * @param length          Track length since creation [cm]
     * @param eLoss           Energy deposit [GeV]
     * @param EventId         Event ID (default 0)
     */
    BmnFHCalPoint(Int_t trackID,
                  uint32_t address,
                  const TVector3& pos,
                  const TVector3& mom,
                  Double_t tof,
                  Double_t length,
                  Double_t eLoss,
                  UInt_t EventId = 0)
        : FairMCPoint(trackID, address, pos, mom, tof, length, eLoss, EventId)
    {}

    //! Copy constructor
    BmnFHCalPoint(const BmnFHCalPoint& point) = default;

    //! Move constructor
    BmnFHCalPoint(BmnFHCalPoint&& point) noexcept = default;

    //! Copy assignment operator
    BmnFHCalPoint& operator=(const BmnFHCalPoint& point) = default;

    //! Move assignment operator
    BmnFHCalPoint& operator=(BmnFHCalPoint&& point) noexcept = default;

    //! Destructor
    virtual ~BmnFHCalPoint() = default;

    //! Address manipulation
    void SetAddress(uint32_t address) { SetDetectorID(address); }
    uint32_t GetAddress() const { return GetDetectorID(); }
    uint32_t GetSystemId() const { return BmnFHCalAddress::GetSystemId(GetAddress()); }
    uint32_t GetModuleType() const { return BmnFHCalAddress::GetModuleType(GetAddress()); }
    uint32_t GetModuleId() const { return BmnFHCalAddress::GetModuleId(GetAddress()); }
    uint32_t GetSectionId() const { return BmnFHCalAddress::GetSectionId(GetAddress()); }
    uint32_t GetScintillatorId() const { return BmnFHCalAddress::GetScintillatorId(GetAddress()); }

    //! Output to screen
    virtual const char* GetClassName() const { return "BmnFHCalPoint"; }
    virtual void Print(const Option_t* opt = "") const override
    {
        printf("%s:\n", GetClassName());
        printf("    %s\n", BmnFHCalAddress::GetInfoString(GetAddress()).c_str());
        FairMCPoint::Print(opt);   // Call base class print
    }

  private:
    ClassDefOverride(BmnFHCalPoint, 3)
};

#endif /* BMNFHCALPOINT_H */
