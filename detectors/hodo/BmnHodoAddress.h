/** \class BmnHodoAddress
 ** \brief BmnHodo interface class to the unique address
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \version 1.0
 **
 ** BmnHodoAddress is the class for the concrete interfaces to the unique address,
 ** which is encoded in a 32-bit field (uint32_t), for the Hodo detector elements.
 **
 **                                     3         2         1         0   Shift  Bits  Values
 ** Current definition:                10987654321098765432109876543210
 ** System id          on bits  0- 4   00000000000000000000000000011111    << 0     5      31
 ** Strip idx          on bits  6-10   00000000000000000000011111000000    << 6     5      31
 ** Strip side         on bits 11-11   00000000000000000000100000000000    <<11     1      1
 ** Gain               on bits 12-12   00000000000000000001000000000000    <<12     1      1
 **
 **/

#ifndef BmnHodoAddress_H
#define BmnHodoAddress_H 1

#include "BmnDetectorList.h"   // for kHodo

#include <cassert>   // for assert

class BmnHodoAddress
{
  public:
    /**
     * \brief Return address from system ID, StripId, StripSide, Gain.
     * \param[in] StripId.
     * \param[in] StripSide.
     * \param[in] Gain.
     * \return Address from system ID, StripId, StripSide, Gain.
     **/
    static uint32_t GetAddress(uint32_t StripId, uint32_t StripSide, uint32_t Gain)
    {
        assert((uint32_t)kHODO <= fgkSystemIdLength);
        assert(StripId <= fgkStripIdLength);
        assert(StripSide <= fgkStripSideLength);
        assert(Gain <= fgkGainLength);
        return ((((uint32_t)kHODO) << fgkSystemIdShift) | (StripId << fgkStripIdShift)
                | (StripSide << fgkStripSideShift) | (Gain << fgkGainShift));
    }

    /**
     * \brief Return System identifier from address.
     * \param[in] address Unique channel address.
     * \return System identifier from address.
     **/
    static uint32_t GetSystemId(uint32_t address)
    {
        return (address & (fgkSystemIdLength << fgkSystemIdShift)) >> fgkSystemIdShift;
    }

    /**
     * \brief Return Strip id from address.
     * \param[in] address Unique channel address.
     * \return Strip id from address.
     **/
    static uint32_t GetStripId(uint32_t address)
    {
        return (address & (fgkStripIdLength << fgkStripIdShift)) >> fgkStripIdShift;
    }

    /**
     * \brief Return Strip side from address.
     * \param[in] address Unique channel address.
     * \return Strip side from address.
     **/
    static uint32_t GetStripSide(uint32_t address)
    {
        return (address & (fgkStripSideLength << fgkStripSideShift)) >> fgkStripSideShift;
    }

    /**
     * \brief Return Gain from address.
     * \param[in] address Unique channel address.
     * \return Gain from address.
     **/
    static uint32_t GetGain(uint32_t address) { return (address & (fgkGainLength << fgkGainShift)) >> fgkGainShift; }

  private:
    // Length of the index of the corresponding volume
    static const uint32_t fgkSystemIdLength = 31;   // 2^5 - 1
    static const uint32_t fgkStripIdLength = 31;    // 2^5 - 1
    static const uint32_t fgkStripSideLength = 1;   // 2^1 - 1
    static const uint32_t fgkGainLength = 1;        // 2^1 - 1

    // Number of a start bit for each volume
    static const uint32_t fgkSystemIdShift = 0;
    static const uint32_t fgkStripIdShift = 6;
    static const uint32_t fgkStripSideShift = 11;
    static const uint32_t fgkGainShift = 12;
};

#endif
