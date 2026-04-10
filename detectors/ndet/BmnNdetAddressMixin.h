#ifndef BMNNDETADDRESSMIXIN_H
#define BMNNDETADDRESSMIXIN_H

/** \class BmnNdetAddressMixin
 ** \brief interface to use the address methods
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \version 1.0
 **
 ** BmnNdetAddressMixin is a class to store methods to access the address fields
 **/

#include "BmnNdetAddress.h"

template<typename T>
class BmnNdetAddressMixin
{
  public:
    uint32_t GetSystemId() const { return BmnNdetAddress::GetSystemId(static_cast<const T*>(this)->GetAddress()); }
    uint32_t GetArmId() const { return BmnNdetAddress::GetArmId(static_cast<const T*>(this)->GetAddress()); }
    uint32_t GetRowId() const { return BmnNdetAddress::GetRowId(static_cast<const T*>(this)->GetAddress()); }
    uint32_t GetColumnId() const { return BmnNdetAddress::GetColumnId(static_cast<const T*>(this)->GetAddress()); }
    uint32_t GetLayerId() const { return BmnNdetAddress::GetLayerId(static_cast<const T*>(this)->GetAddress()); }
};

#endif /* BMNNDETADDRESSMIXIN_H */
