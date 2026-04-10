/*************************************************************************************
 *
 *         Class BmnNdetGeoPar
 *
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015
 *  Modified by M.Golubeva July 2022
 *
 ************************************************************************************/

#include "BmnNdetGeoPar.h"

#include "BmnNdet.h"
#include "FairParamList.h"

#include <iomanip>
#include <iostream>
using namespace std;

BmnNdetGeoPar::BmnNdetGeoPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{

    fGeoSensNodes = new TObjArray();
    fGeoPassNodes = new TObjArray();
}

BmnNdetGeoPar::~BmnNdetGeoPar(void) {}

// probably the next funtions can be deleted

void BmnNdetGeoPar::clear(void)
{
    if (fGeoSensNodes)
        delete fGeoSensNodes;
    if (fGeoPassNodes)
        delete fGeoPassNodes;
}

void BmnNdetGeoPar::putParams(FairParamList* l)
{
    if (!l)
        return;
    l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
    l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t BmnNdetGeoPar::getParams(FairParamList* l)
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
bool BmnNdetGeoPar::CheckIfSensitive(std::string name)
{
    return (name.find(SensitiveVolume_name_NICA) != std::string::npos)
           || (name.find(SensitiveVolume_name_VETO) != std::string::npos);
}
