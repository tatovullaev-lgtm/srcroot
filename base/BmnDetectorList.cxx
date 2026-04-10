#include "BmnDetectorList.h"

#include "TString.h"

// -----   GetSystemName   -------------------------------------------------
void BmnDetectorList::GetSystemName(DetectorId det, TString& name)
{
    switch (det) {
        case kREF:
            name = "ref";
            break;
        case kBD:
            name = "bd";
            break;
        case kGEM:
            name = "gem";
            break;
        case kTOF1:
            name = "tof400";
            break;
        case kDCH:
            name = "dch";
            break;
        case kTOF:
            name = "tof";
            break;
        case kTOF701:
            name = "tof700";
            break;
        case kZDC:
            name = "zdc";
            break;
        case kVSP:
            name = "vsp";
            break;
        case kMWPC:
            name = "mwpc";
            break;
        case kECAL:
            name = "ecal";
            break;
        case kCSC:
            name = "csc";
            break;
        case kSILICON:
            name = "silicon";
            break;
        case kFD:
            name = "fd";
            break;
        case kSiMD:
            name = "simd";
            break;
        case kSiBT:
            name = "sibt";
            break;
        case kSiProf:
            name = "siprof";
            break;
        case kARMTRIG:
            name = "armtrig";
            break;
        case kBC:
            name = "bc";
            break;
        case kSCWALL:
            name = "scwall";
            break;
        case kHODO:
            name = "hodo";
            break;
        case kFHCAL:
            name = "fhcal";
            break;
        case kNDET:
            name = "ndet";
            break;
        case kSSD:
            name = "ssd";
            break;
        case kVACWALL:
            name = "vacwall";
            break;
        default:
            name = "unknown";
            break;
    }
}

// -----   GetSystemName   -------------------------------------------------
void BmnDetectorList::GetSystemName(Int_t det, TString& name)
{
    if (det < kNOFDETS)
        GetSystemName(DetectorId(det), name);
    else
        name = "unknown";
}

// -----   GetSystemNameCaps   ---------------------------------------------
void BmnDetectorList::GetSystemNameCaps(DetectorId det, TString& name)
{
    switch (det) {
        case kREF:
            name = "REF";
            break;
        case kBD:
            name = "BD";
            break;
        case kGEM:
            name = "GEM";
            break;
        case kTOF1:
            name = "TOF400";
            break;
        case kDCH:
            name = "DCH";
            break;
        case kTOF:
            name = "TOF";
            break;
        case kTOF701:
            name = "TOF700";
            break;
        case kZDC:
            name = "ZDC";
            break;
        case kVSP:
            name = "VSP";
            break;
        case kMWPC:
            name = "MWPC";
            break;
        case kECAL:
            name = "ECAL";
            break;
        case kCSC:
            name = "CSC";
            break;
        case kSILICON:
            name = "SILICON";
            break;
        case kFD:
            name = "FD";
            break;
        case kSiMD:
            name = "SiMD";
            break;
        case kSiBT:
            name = "SiBT";
            break;
        case kSiProf:
            name = "SiProf";
            break;
        case kARMTRIG:
            name = "ARMTRIG";
            break;
        case kBC:
            name = "BC";
            break;
        case kSCWALL:
            name = "SCWALL";
            break;
        case kHODO:
            name = "HODO";
            break;
        case kFHCAL:
            name = "FHCAL";
            break;
        case kNDET:
            name = "NDET";
            break;
        case kSSD:
            name = "SSD";
            break;
        case kVACWALL:
            name = "vacwall";
            break;
        default:
            name = "UNKNOWN";
            break;
    }
}

// -----   GetSystemNameCaps   ---------------------------------------------
void BmnDetectorList::GetSystemNameCaps(Int_t det, TString& name)
{
    if (det < kNOFDETS)
        GetSystemNameCaps(DetectorId(det), name);
    else
        name = "unknown";
}
