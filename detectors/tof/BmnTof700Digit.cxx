#include "BmnTof700Digit.h"

BmnTof700Digit::BmnTof700Digit()
{
   fPlane=-1;
   fStrip=-1;
   fAmplitude=-1;
   fTime=-1;
   fDiff=-1;
}

BmnTof700Digit::BmnTof700Digit(Short_t plane, Short_t strip, Float_t t, Float_t a, Float_t d)
{
   fPlane=plane;
   fStrip=strip;
   fAmplitude=a;
   fTime=t;
   fDiff=d;
}

BmnTof700Digit::~BmnTof700Digit()
{
}

ClassImp(BmnTof700Digit)
