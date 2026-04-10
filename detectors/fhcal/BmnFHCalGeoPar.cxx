/*************************************************************************************
 *
 *         Class BmnFHCalGeoPar
 *
 ************************************************************************************/
#include "BmnFHCalGeoPar.h"

#include "BmnFHCal.h"
#include "FairParamList.h"

#include <iomanip>
#include <iostream>

using namespace std;

BmnFHCalGeoPar::BmnFHCalGeoPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    fGeoSensNodes = new TObjArray();
    fGeoPassNodes = new TObjArray();
}

BmnFHCalGeoPar::~BmnFHCalGeoPar(void) {}

// probably the next funtions can be deleted

void BmnFHCalGeoPar::clear(void)
{
    if (fGeoSensNodes)
        delete fGeoSensNodes;
    if (fGeoPassNodes)
        delete fGeoPassNodes;
}

void BmnFHCalGeoPar::putParams(FairParamList* l)
{
    if (!l)
        return;
    l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
    l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t BmnFHCalGeoPar::getParams(FairParamList* l)
{
    if (!l)
        return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes))
        return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes))
        return kFALSE;

    return kTRUE;
}

//------   CheckIfSensitive     -----------------------------------------------
bool BmnFHCalGeoPar::CheckIfSensitive(std::string name)
{
    return name.find(SensitiveVolume_name) != std::string::npos;
}
