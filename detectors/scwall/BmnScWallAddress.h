/** \class BmnScWallAddress
 ** \brief BmnScWall interface class to the unique address
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \version 1.0
 **
 ** BmnScWallAddress is the class for the concrete interfaces to the unique address, 
 ** which is encoded in a 32-bit field (uint32_t), for the ScWall detector elements.
 **
 **                                     3         2         1         0   Shift  Bits  Values
 ** Current definition:                10987654321098765432109876543210
 ** System id          on bits  0- 4   00000000000000000000000000011111    << 0     5      31
 ** Cell id            on bits  5-14   00000000000000000111111111100000    << 5    10    1023
 **
 **/

#ifndef BMNSCWALLADDRESS_H
#define BMNSCWALLADDRESS_H 1

#include "BmnDetectorList.h"  // for kSCWALL
#include <cassert>  // for assert

class BmnScWallAddress {
public:
  /**
   * \brief Return address from system ID, Cell ID.
   * \param[in] Cell id.
   **/
  static uint32_t GetAddress(uint32_t CellId)
  {
    assert((uint32_t)kSCWALL <= fgkSystemIdLength);
    assert(CellId <= fgkCellIdLength);
    return (  (((uint32_t)kSCWALL) << fgkSystemIdShift) 
            | (CellId << fgkCellIdShift)  );
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
   * \brief Return Cell id from address.
   * \param[in] address Unique channel address.
   * \return Cell id from address.
   **/
  static uint32_t GetCellId(uint32_t address)
  {
    return (address & (fgkCellIdLength << fgkCellIdShift)) >> fgkCellIdShift;
  }

private:
  // Length of the index of the corresponding volume
  static const uint32_t fgkSystemIdLength  = 31;    // 2^5 - 1
  static const uint32_t fgkCellIdLength  = 1023;    // 2^10- 1

  // Number of a start bit for each volume
  static const uint32_t fgkSystemIdShift  = 0;
  static const uint32_t fgkCellIdShift = 5;

};

#endif
