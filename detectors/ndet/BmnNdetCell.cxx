/** @file BmnNdetCell.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 27.03.2022
 **
 ** Code of Class for Bmn Ndet Cell data container in event
 **/

#include "BmnNdetCell.h"

#include "BmnDetectorList.h"

BmnNdetCell::BmnNdetCell()
    : FairHit()
    , BmnNdetAddressMixin()
    , fAddress(0)
    , fSignal(0)
{
    SetTimeStamp(0.);
    SetDetectorID(kNDET);
}

BmnNdetCell::BmnNdetCell(uint32_t address,
                         double time,
                         double signal,
                         TVector3 posHit,
                         TVector3 posHitErr,
                         int pointIndx)
    : FairHit(kNDET, posHit, posHitErr, pointIndx)
    , BmnNdetAddressMixin()
    , fAddress(address)
    , fSignal(signal)
{
    SetTimeStamp(time);
}

// Copy constructor
BmnNdetCell::BmnNdetCell(const BmnNdetCell& other)
    : FairHit(other)
    , BmnNdetAddressMixin()
    , fAddress(other.fAddress)
    , fSignal(other.fSignal)
{}

// Move constructor
BmnNdetCell::BmnNdetCell(BmnNdetCell&& other)
    : FairHit(std::move(other))
    , BmnNdetAddressMixin()
    , fAddress(other.fAddress)
    , fSignal(other.fSignal)
{
    other.fAddress = 0;
    other.fSignal = 0;
}

// Copy assignment operator
BmnNdetCell& BmnNdetCell::operator=(const BmnNdetCell& other)
{
    if (this != &other) {
        FairHit::operator=(other);
        fAddress = other.fAddress;
        fSignal = other.fSignal;
    }
    return *this;
}

// Move assignment operator
BmnNdetCell& BmnNdetCell::operator=(BmnNdetCell&& other)
{
    if (this != &other) {
        FairHit::operator=(std::move(other));
        fAddress = other.fAddress;
        fSignal = other.fSignal;
        other.fAddress = 0;
        other.fSignal = 0;
    }
    return *this;
}

BmnNdetCell::~BmnNdetCell() {}

void BmnNdetCell::Print(Option_t* option) const
{
    printf("%s: %s\n", GetClassName(), BmnNdetAddress::GetInfoString(GetAddress()).c_str());
    printf("    Signal: %.2f\n", fSignal);
    printf("    Time: %.2f\n", GetTime());
}

void BmnNdetCell::ResetTimeEnergy()
{
    SetSignal(0.);
    SetTimeStamp(0.);
    ResetLinks();
    SetRefIndex(-1);
}

void BmnNdetCell::reset()
{
    ResetTimeEnergy();
    TVector3 zero;
    SetPosition(zero);
    SetPositionError(zero);
}
