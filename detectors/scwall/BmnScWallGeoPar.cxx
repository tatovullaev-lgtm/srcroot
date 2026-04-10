/*************************************************************************************
 *
 *         Class BmnScWallGeoPar
 *
 ************************************************************************************/
#include "BmnScWallGeoPar.h"

#include "BmnScWall.h"
#include "FairParamList.h"

#include <iomanip>
#include <iostream>

using namespace std;

BmnScWallGeoPar::BmnScWallGeoPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    fGeoSensNodes = new TObjArray();
    fGeoPassNodes = new TObjArray();
}

BmnScWallGeoPar::~BmnScWallGeoPar(void) {}

// probably the next funtions can be deleted

void BmnScWallGeoPar::clear(void)
{
    if (fGeoSensNodes)
        delete fGeoSensNodes;
    if (fGeoPassNodes)
        delete fGeoPassNodes;
}

void BmnScWallGeoPar::putParams(FairParamList* l)
{
    if (!l)
        return;
    l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
    l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t BmnScWallGeoPar::getParams(FairParamList* l)
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
bool BmnScWallGeoPar::CheckIfSensitive(std::string name)
{
    return name.find("sens") != std::string::npos && name.find("cutted") != std::string::npos;
}