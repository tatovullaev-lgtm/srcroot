/** \class BmnNdetAddress
 ** \brief BmnNdet interface class to the unique address
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \version 1.0
 **
 ** BmnNdetAddress is the class for the concrete interfaces to the unique address,
 ** which is encoded in a 32-bit field (uint32_t), for the Ndet detector elements.
 **
 **                                     3         2         1         0   Shift  Bits  Values
 ** Current definition:                10987654321098765432109876543210
 ** System id          on bits  0- 4   00000000000000000000000000011111    << 0     5      31
 ** ArmId              on bits  5- 6   00000000000000000000000001100000    << 5     2      3
 ** RowId              on bits  7-10   00000000000000000000011110000000    << 7     4      15
 ** ColumnId           on bits 11-14   00000000000000000111100000000000    << 11    4      15
 ** LayerId            on bits 15-19   00000000000011111000000000000000    <<15     5      31
 **
 **/

#ifndef BmnNdetAddress_H
#define BmnNdetAddress_H 1

#include "BmnDetectorList.h"   // for kNDET

#include <cassert>   // for assert
#include <sstream>   // for std::ostringstream

class BmnNdetAddress
{
  public:
    virtual ~BmnNdetAddress() {}
    /**
     * \brief Return address
     * \param[in] ArmId arm id.
     * \param[in] RowId row id.
     * \param[in] ColumnId column id.
     * \param[in] LayerId layer id.
     * \return Address from system ID, ArmId, RowId, ColumnId, LayerId.
     **/
    static uint32_t GetAddress(uint32_t ArmId, uint32_t RowId, uint32_t ColumnId, uint32_t LayerId)
    {
        assert((uint32_t)kNDET <= fgkSystemIdLength);
        assert(ArmId <= fgkArmIdLength);
        assert(RowId <= fgkRowIdLength);
        assert(ColumnId <= fgkColumnIdLength);
        assert(LayerId <= fgkLayerIdLength);
        return ((((uint32_t)kNDET) << fgkSystemIdShift) | (ArmId << fgkArmIdShift) | (RowId << fgkRowIdShift)
                | (ColumnId << fgkColumnIdShift) | (LayerId << fgkLayerIdShift));
    }

    /**
     * \brief Return a formatted string with all address components.
     * \param[in] address Unique channel address.
     * \return Formatted string with address components
     **/
    static std::string GetInfoString(uint32_t address)
    {
        std::ostringstream oss;
        oss << "Address: " << address << ", SystemId: " << GetSystemId(address) << ", ArmId: " << GetArmId(address)
            << ", RowId: " << GetRowId(address) << ", ColumnId: " << GetColumnId(address)
            << ", LayerId: " << GetLayerId(address);
        return oss.str();
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
     * \brief Return Arm id from address.
     * \param[in] address Unique channel address.
     * \return Arm id from address.
     **/
    static uint32_t GetArmId(uint32_t address)
    {
        return (address & (fgkArmIdLength << fgkArmIdShift)) >> fgkArmIdShift;
    }

    /**
     * \brief Return Row id from address.
     * \param[in] address Unique channel address.
     * \return Row id from address.
     **/
    static uint32_t GetRowId(uint32_t address)
    {
        return (address & (fgkRowIdLength << fgkRowIdShift)) >> fgkRowIdShift;
    }

    /**
     * \brief Return Column id from address.
     * \param[in] address Unique channel address.
     * \return Column id from address.
     **/
    static uint32_t GetColumnId(uint32_t address)
    {
        return (address & (fgkColumnIdLength << fgkColumnIdShift)) >> fgkColumnIdShift;
    }

    /**
     * \brief Return Layer id from address.
     * \param[in] address Unique channel address.
     * \return Layer id from address.
     **/
    static uint32_t GetLayerId(uint32_t address)
    {
        return (address & (fgkLayerIdLength << fgkLayerIdShift)) >> fgkLayerIdShift;
    }

  private:
    // Length of the index of the corresponding volume
    static const uint32_t fgkSystemIdLength = 31;   // 2^5 - 1
    static const uint32_t fgkArmIdLength = 3;       // 2^2 - 1
    static const uint32_t fgkRowIdLength = 15;      // 2^4 - 1
    static const uint32_t fgkColumnIdLength = 15;   // 2^4 - 1
    static const uint32_t fgkLayerIdLength = 31;    // 2^5 - 1

    // Number of a start bit for each volume
    static const uint32_t fgkSystemIdShift = 0;
    static const uint32_t fgkArmIdShift = 5;
    static const uint32_t fgkRowIdShift = 7;
    static const uint32_t fgkColumnIdShift = 11;
    static const uint32_t fgkLayerIdShift = 15;

    ClassDef(BmnNdetAddress, 1)
};

#endif
