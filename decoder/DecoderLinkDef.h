// $Id: DecoderLinkDef.h,v
#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class BmnDchRaw2Digit + ;
#pragma link C++ class BmnMwpcRaw2Digit + ;
#pragma link C++ class BmnZDCRaw2Digit + ;
#pragma link C++ class BmnScWallRaw2Digit + ;
#pragma link C++ class BmnFHCalRaw2Digit + ;
#pragma link C++ class BmnHodoRaw2Digit + ;
#pragma link C++ class BmnNdetRaw2Digit + ;
#pragma link C++ class BmnECALRaw2Digit + ;
#pragma link C++ class BmnTof2Raw2DigitNew + ;
#pragma link C++ class BmnTof1Raw2Digit + ;
#pragma link C++ class BmnTof701Raw2Digit + ;
#pragma link C++ class BmnGemRaw2Digit + ;
#pragma link C++ class BmnTrigRaw2Digit + ;
#pragma link C++ class BmnAdcProcessor + ;
#pragma link C++ class BmnAdcQA + ;
#pragma link C++ class BmnSiliconRaw2Digit + ;
#pragma link C++ class BmnSiBTRaw2Digit + ;
#pragma link C++ class BmnCscRaw2Digit + ;
// #pragma link C++ class BmnProfRaw2Digit + ;
// #pragma link C++ class BmnProfAsic2Raw + ;
#pragma link C++ class BmnRawDataDecoder + ;
#pragma link C++ class BmnRawSource + ;
#pragma link C++ class BmnConverter + ;
#pragma link C++ class BmnAbstractThread + ;
#pragma link C++ class BmnConverterThread + ;
#pragma link C++ class BmnThreadManager < BmnConverterThread> + ;
#pragma link C++ class BmnDecoder + ;
#pragma link C++ class BmnMetadataDecoder + ;
// #pragma link C++ class BmnSlewingTOF700 + ;
#pragma link C++ class BmnMscRaw2Digit + ;
#pragma link C++ class WfmProcessor + ;
#pragma link C++ class PsdSignalFitting::PronyFitter;
#pragma link C++ class RLDeconvolutor + ;
#pragma link C++ class BmnHgndRaw2Digit + ;
#pragma link C++ class BmnVspRaw2Digit + ;
// #pragma link C++ class fles::StorableTimeslice + ;
#pragma link C++ class fles::Timeslice + ;

#endif
