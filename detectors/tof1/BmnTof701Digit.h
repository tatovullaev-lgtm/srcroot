//------------------------------------------------------------------------------------------------------------------------
// Legacy
// Keep this class for backward compatibility.

#ifndef __BMNTOF701DIGIT_H
#define __BMNTOF701DIGIT_H 1

#include "BmnTof1Digit.h"
#include "TObject.h"

#include <iostream>
//------------------------------------------------------------------------------------------------------------------------
class BmnTof701Digit : public BmnTof1Digit
{
  public:
    BmnTof701Digit();
    BmnTof701Digit(Short_t plane, Short_t strip, Short_t side, Float_t t, Float_t a);
    BmnTof701Digit(const BmnTof701Digit*, Float_t t, Float_t a);
    virtual ~BmnTof701Digit();

    // Short_t GetPlane() const { return fPlane; }
    // Short_t GetStrip() const { return fStrip; }
    // Short_t GetSide() const { return fSide; }
    // Float_t GetAmplitude() const { return fAmplitude; }
    // Float_t GetTime() const { return fTime; }

    // void SetPlane(Short_t v) { fPlane = v; }
    // void SetStrip(Short_t v) { fStrip = v; }
    // void SetSide(Short_t v) { fSide = v; }
    // void SetAmplitude(Float_t v) { fAmplitude = v; }
    // void SetTime(Float_t v) { fTime = v; }

    // void print(const char* comment = nullptr, std::ostream& os = std::cout) const;

    ClassDef(BmnTof701Digit, 3);
};
//------------------------------------------------------------------------------------------------------------------------
#endif
