#ifndef BMNNDETCELL_H
#define BMNNDETCELL_H

#include <BmnNdetAddressMixin.h>
#include <FairHit.h>
#include <TClonesArray.h>

class BmnNdetCell
    : public FairHit
    , public BmnNdetAddressMixin<BmnNdetCell>
{

  public:
    enum class LinkType
    {
        MCTrack,
        MCPoint,
        VacWallPoint,
        SurfaceMCTrack,
        SurfaceMCTrackV2,
        SurfaceMCPoint,
        SurfaceMCPointCrossThr
    };

    BmnNdetCell();
    BmnNdetCell(uint32_t address, double time, double signal, TVector3 posHit, TVector3 posHitErr, int pointIndx);
    BmnNdetCell(const BmnNdetCell&);
    BmnNdetCell(BmnNdetCell&&);
    BmnNdetCell& operator=(const BmnNdetCell&);
    BmnNdetCell& operator=(BmnNdetCell&&);
    virtual ~BmnNdetCell();

    virtual const char* GetClassName() const { return "BmnNdetCell"; }
    void ResetTimeEnergy();
    void reset();
    virtual void Print(Option_t* option = "") const override;

    // Setters
    void SetAddress(uint32_t address) { fAddress = address; }
    void SetSignal(double signal) { fSignal = signal; }
    void SetTime(double time) { FairHit::SetTimeStamp(time); }

    // Getters
    uint32_t GetAddress() const { return fAddress; }
    double GetSignal() const { return fSignal; }
    double GetTime() const { return FairHit::GetTimeStamp(); }

  private:
    uint32_t fAddress;
    double fSignal;

    ClassDefOverride(BmnNdetCell, 5);
};

#endif /* BMNNDETCELL_H */
