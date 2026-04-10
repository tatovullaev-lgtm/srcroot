/** @file BmnFHCalEvent.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 27.03.2022
 **
 ** Code of Class for Bmn FHCal data container in event 
 **/

#include "BmnFHCalEvent.h"

void BmnFHCalEvent::SummarizeEvent()
{
  fTotalEnergy = 0.0;
  for (auto& module : fModulesVec) {
    module.SummarizeModule();
    fTotalEnergy += module.GetEnergy();
  }
}
