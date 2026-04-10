#ifndef PASSIVEDET_H
#define PASSIVEDET_H

#include "FairModule.h"   // for FairModule

#include <Rtypes.h>   // for FairPassiveDet::Class, ClassDef, etc

class FairPassiveDet : public FairModule
{
  public:
    FairPassiveDet(const char* name, const char* Title = "Passive Det");
    FairPassiveDet();

    virtual ~FairPassiveDet();
    virtual void ConstructGeometry();

    virtual FairModule* CloneModule() const;

  private:
    FairPassiveDet(const FairPassiveDet& rhs);

    ClassDef(FairPassiveDet, 1);
};

#endif   // PASSIVEDET_H
