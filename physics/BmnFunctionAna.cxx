#include "BmnFunctionAna.h"
#include "DstRunHeader.h"
#include "BmnNewFieldMap.h"
#include "BmnFieldMapSym3.h"

#include "FairLogger.h"

#include "TSystem.h"
#include "TFile.h"

#include <iostream>
using namespace std;

// get magnetic field from a specified DST file
// fileName: DST file path [in]
// fieldScale: magnetic field scale [out]; fieldType: magnetic field type [out]
// returns magnetic field, nullptr - if some errors occured
FairField* BmnFunctionAna::GetDSTField(TString dstFileName, Double_t& fieldScale, Int_t& fieldType)
{
    gSystem->ExpandPathName(dstFileName);
    if (gSystem->AccessPathName(dstFileName.Data()) == true)
    {
        LOG(error)<<"GetDSTField: no specified file: "<<dstFileName;
        return nullptr;
    }

    TFile* dst_file = TFile::Open(dstFileName.Data());
    if (dst_file->IsZombie())
    {
        LOG(error)<<"GetDSTField: opening the input file failed: "<<dstFileName;
        return nullptr;
    }

    // Get a pointer to "DstRunHeader"
    DstRunHeader* pRunHeader = dynamic_cast<DstRunHeader*>(dst_file->Get("DstRunHeader"));
    if (!pRunHeader)
    {
        LOG(error)<<"GetDSTField: No 'DstRunHeader' branch found in the input file"<<dstFileName;
        dst_file->Close();
        return nullptr;
    }

    // get field type and scale from the 'DstRunHeader' object
    fieldType = pRunHeader->GetFieldType();
    fieldScale = pRunHeader->GetFieldScale();
    TString fieldName = pRunHeader->GetFieldName();
    FairField* magField = nullptr;
    switch (fieldType)
    {
    case 1:
    {
        magField = new BmnNewFieldMap(fieldName.Data());
        (dynamic_cast<BmnNewFieldMap*>(magField))->SetScale(fieldScale);
        magField->Init();
        break;
    }
    case 3:
    {
        magField = new BmnFieldMapSym3(fieldName.Data());
        (dynamic_cast<BmnFieldMapSym3*>(magField))->SetScale(fieldScale);
        magField->Init();
        break;
    }
    default:
        LOG(error)<<"GetDSTField: This type of the magnetic field is not supported: "<<fieldType;
    }

    dst_file->Close();
    return magField;
}
