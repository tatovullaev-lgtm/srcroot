#include "BmnMilleContainer.h"

#include <vector>

BmnMilleContainer::BmnMilleContainer()
: fStation(-1),
  fModule(-1),
  fMeasX(0.),
  fMeasY(0.),
  fdMeasX(0.),
  fdMeasY(0.)
{
    fLocDerX.resize(0);
    fLocDerY.resize(0);
    fGlobDerX.resize(0);    
    fGlobDerY.resize(0);    
}

//BmnMilleContainer::BmnMilleContainer(TString lay) : 
//BmnMilleContainer() {
//   
//}

