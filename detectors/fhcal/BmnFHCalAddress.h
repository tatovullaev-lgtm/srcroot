/** \class BmnFHCalAddress
 ** \brief BmnFHCal interface class to the unique address
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \version 1.0
 **
 ** BmnFHCalAddress is the class for the concrete interfaces to the unique address,
 ** which is encoded in a 32-bit field (uint32_t), for the FHCal detector elements.
 **
 **                                     3         2         1         0   Shift  Bits  Values
 ** Current definition:                10987654321098765432109876543210
 ** System id          on bits  0- 4   00000000000000000000000000011111    << 0     5      31
 ** ModType            on bits  5- 6   00000000000000000000000001100000    << 5     2      3
 ** ModIdx             on bits  7-13   00000000000000000011111110000000    << 7     7      127
 ** SecIdx             on bits 14-17   00000000000000111100000000000000    <<14     4      15
 ** ScintIdx           on bits 18-20   00000000000111000000000000000000    <<18     3      7
 **
 **/

#ifndef BmnFHCalAddress_H
#define BmnFHCalAddress_H 1

#include "BmnDetectorList.h"   // for kFHCAL

#include <cassert>   // for assert
#include <sstream>   // for std::ostringstream

class BmnFHCalAddress
{
  public:
    virtual ~BmnFHCalAddress() {}
    /**
     * \brief Return address from system ID, Module type, Module ID, Section ID, Scintillator ID (optional).
     * \param[in] Module type.
     * \param[in] Module id.
     * \param[in] Section id.
     * \param[in] Scintillator id.
     * \return Address from system ID, Module type, Module ID, Section ID, Scintillator id (optional).
     **/
    static uint32_t GetAddress(uint32_t ModuleType, uint32_t ModuleId, uint32_t SectionId, uint32_t ScintillatorId = 0)
    {
        assert((uint32_t)kFHCAL <= fgkSystemIdLength);
        assert(ModuleType <= fgkModTypeLength);
        assert(ModuleId <= fgkModIdxLength);
        assert(SectionId <= fgkSecIdxLength);
        assert(ScintillatorId <= fgkScintIdxLength);
        return ((((uint32_t)kFHCAL) << fgkSystemIdShift) | (ModuleType << fgkModTypeShift)
                | (ModuleId << fgkModIdxShift) | (SectionId << fgkSecIdxShift) | (ScintillatorId << fgkScintIdxShift));
    }

    /**
     * \brief Return a formatted string with all address components.
     * \param[in] address Unique channel address.
     * \return Formatted string with address components
     **/
    static std::string GetInfoString(uint32_t address)
    {
        std::ostringstream oss;
        oss << "Address: " << address << ", SystemId: " << GetSystemId(address)
            << ", ModuleType: " << GetModuleType(address) << ", ModuleId: " << GetModuleId(address)
            << ", SectionId: " << GetSectionId(address) << ", ScintillatorId: " << GetScintillatorId(address);
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
     * \brief Return Module id from address.
     * \param[in] address Unique channel address.
     * \return Module id from address.
     **/
    static uint32_t GetModuleType(uint32_t address)
    {
        return (address & (fgkModTypeLength << fgkModTypeShift)) >> fgkModTypeShift;
    }

    /**
     * \brief Return Module id from address.
     * \param[in] address Unique channel address.
     * \return Module id from address.
     **/
    static uint32_t GetModuleId(uint32_t address)
    {
        return (address & (fgkModIdxLength << fgkModIdxShift)) >> fgkModIdxShift;
    }

    /**
     * \brief Return Section id from address.
     * \param[in] address Unique channel address.
     * \return Section id from address.
     **/
    static uint32_t GetSectionId(uint32_t address)
    {
        return (address & (fgkSecIdxLength << fgkSecIdxShift)) >> fgkSecIdxShift;
    }

    /**
     * \brief Return Scintillator id from address.
     * \param[in] address Unique channel address.
     * \return Scintillator id from address.
     **/
    static uint32_t GetScintillatorId(uint32_t address)
    {
        return (address & (fgkScintIdxLength << fgkScintIdxShift)) >> fgkScintIdxShift;
    }

    /**
     * \brief Return Physical address (w/o scintillator id) from address.
     * \param[in] address Unique channel address.
     * \return Physical address (w/o scintillator id)
     **/
    static uint32_t GetPhysicalAddress(uint32_t address) { return address & ((1 << fgkScintIdxShift) - 1); }

  private:
    // Length of the index of the corresponding volume
    static const uint32_t fgkSystemIdLength = 31;   // 2^5 - 1
    static const uint32_t fgkModTypeLength = 3;     // 2^2 - 1
    static const uint32_t fgkModIdxLength = 127;    // 2^7 - 1
    static const uint32_t fgkSecIdxLength = 15;     // 2^4 - 1
    static const uint32_t fgkScintIdxLength = 7;    // 2^3 - 1

    // Number of a start bit for each volume
    static const uint32_t fgkSystemIdShift = 0;
    static const uint32_t fgkModTypeShift = 5;
    static const uint32_t fgkModIdxShift = 7;
    static const uint32_t fgkSecIdxShift = 14;
    static const uint32_t fgkScintIdxShift = 18;

    ClassDef(BmnFHCalAddress, 2)
};

#endif
