/*************************************************************************************
 *
 *         Class BmnHodoGeoPar
 *
 ************************************************************************************/
#include "BmnHodoGeoPar.h"

#include "FairParamList.h"

#include <iomanip>
#include <iostream>

using namespace std;

BmnHodoGeoPar::BmnHodoGeoPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    fGeoSensNodes = new TObjArray();
    fGeoPassNodes = new TObjArray();
}

BmnHodoGeoPar::~BmnHodoGeoPar(void) {}

// probably the next funtions can be deleted

void BmnHodoGeoPar::clear(void)
{
    if (fGeoSensNodes)
        delete fGeoSensNodes;
    if (fGeoPassNodes)
        delete fGeoPassNodes;
}

void BmnHodoGeoPar::putParams(FairParamList* l)
{
    if (!l)
        return;
    l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
    l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t BmnHodoGeoPar::getParams(FairParamList* l)
{
    if (!l)
        return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes))
        return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes))
        return kFALSE;

    return kTRUE;
}

bool BmnHodoGeoPar::CheckIfSensitive(std::string name)
{
    return name.find("sens") != std::string::npos;
}
