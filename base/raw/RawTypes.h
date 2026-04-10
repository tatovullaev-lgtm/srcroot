#ifndef RAWTYPES_H
#define RAWTYPES_H

/***************** SET OF DAQ CONSTANTS *****************/
const uint32_t SYNC_EVENT = 0x2A50D5AF;
const uint32_t SYNC_EVENT_OLD = 0x2A502A50;
const uint32_t SYNC_STAT = 0x4A62B59D;   // Statistics
const uint32_t SYNC_EOS = 0x4A624A62;    // End Of Spill
const uint32_t SYNC_JSON = 0x4E4F534A;
const uint32_t SYNC_RUN_START = 0x72617453;
const uint32_t SYNC_RUN_STOP = 0x706F7453;
const uint32_t SYNC_FILE_BEGIN = 0x67654246;
const uint32_t SYNC_FILE_END = 0x646E4546;
const uint32_t RECORD_RUN_NUMBER = 0x236E7552;   // Run ID
const uint32_t RECORD_RUN_INDEX = 0x78646E49;    // Event Builder Name
const uint32_t RECORD_EVENT_ORDER = 0x71655345;
const uint32_t RECORD_FILE_ID = 0x64496946;
const size_t kWORDSIZE = sizeof(uint32_t);
const uint32_t kNBYTESINWORD = kWORDSIZE;

// FVME data types
const uint32_t kMODDATAMAX = 0x7;
const uint32_t kMODHEADER = 0x8;
const uint32_t kMODTRAILER = 0x9;
const uint32_t kEVHEADER = 0xA;
const uint32_t kEVTRAILER = 0xB;
const uint32_t kSPILLHEADER = 0xC;
const uint32_t kSPILLTRAILER = 0xD;
const uint32_t kSTATUS = 0xE;
const uint32_t kPADDING = 0xF;
const uint32_t TMWR_TAI = 2;
const uint32_t TMWR_REL_TS = 4;
const uint32_t TMWR_REL_TS_TB = 5;
const uint32_t TMWR_PULSE = 7;
const uint32_t TMWR_EOS_CNT = 1;
const uint32_t TMWR_EOS_CNT_MATCHED = 0;

// MStream Data Type
const uint32_t kMSTREAM_TYPE_TDC = 0x0;
const uint32_t kMSTREAM_TYPE_ADC = 0x1;
const uint32_t kMSTREAM_TYPE_TRIG = 0xA;
const uint32_t kMSTREAM_TYPE_STAT = 0xF;

// module ID
const uint32_t kTDC64V = 0x10;   // DCH
const uint32_t kTDC64VHLE = 0x53;
const uint32_t kTDC64VHLE_E = 0xD3;
const uint32_t kTDC72VHL = 0x12;
const uint32_t kTDC32VL = 0x11;
const uint32_t kTQDC16 = 0x09;
const uint32_t kTQDC16VS = 0x56;
const uint32_t kVIRTUAL_DEVICE = 0x56;
const uint32_t kTQDC16VS_E = 0xD6;
const uint32_t kTRIG = 0xA;
const uint32_t kTTVXS = 0xCF;
const uint32_t kMSC16V = 0xF;
const uint32_t kMSC16VE_E = 0xD8;
const uint32_t kUT24VE_TRC = 0xE3;
const uint32_t kUT24VE = 0xC9;
const uint32_t kUT24VE_ = 0x49;
const uint32_t kADC64VE = 0xD4;
const uint32_t kADC64VE_XGE = 0xD9;
const uint32_t kADC64WR = 0xCA;
const uint32_t kHRB = 0xC2;
const uint32_t kTDC72VXS = 0xD0;
const uint32_t kFVME = 0xD1;
const uint32_t kGENERIC_DEVICE = 0xDA;
const uint32_t kVSP_SERIALS = 0x00535453;
const uint32_t kHGND_SERIALS = 0x4e446574;
const uint32_t SERIAL_RUN_DOCUMENT = 0x43526372;   // rcRC
const uint32_t SERIAL_TO_CFG = 0x30543074;         // t0T0
const uint32_t kU40VE_RC = 0x4C;

// event type trigger
const uint32_t kEVENTTYPESLOT = 12;
const uint32_t kWORDTAI = 2;
const uint32_t kWORDTRIG = 3;
const uint32_t kWORDAUX = 4;
const uint32_t kTRIGBEAM = 6;
const uint32_t kTRIGMINBIAS = 1;

const uint32_t TDC_EV_HEADER = 2;
const uint32_t TDC_EV_TRAILER = 3;
const uint32_t TDC_LEADING = 4;
const uint32_t TDC_TRAILING = 5;
const uint32_t TDC_ERROR = 6;

const uint32_t MSC_N_COUNTERS = 16;
const uint8_t MSC_EC_SPILL = 1;
const uint8_t MSC_EC_BUSY = (1 << 1);
const uint8_t MSC_TIME_SLICE_INFO = 0xE;

const uint32_t kT0_BIN_BLOCK_WORDS = 256;
const uint32_t kT0_BIN_BLOCK_2020_SEC = 0;
const uint32_t kT0_BIN_BLOCK_UNIX_SEC = 248;

const double TRC_CLOCK_NS = 8;

#pragma pack(push, 1)

struct __attribute__((packed)) DeviceHeader
{
    void Print()
    {
        printf("\tDeviceHeader:\n");
        printf("Serial   : 0x%08X\n", Serial);
        printf("Length   :   %8u\n", Len);
        printf("DeviceId :   %8X\n", DeviceId);
    }
    const uint32_t GetNWords() { return Len / kWORDSIZE; }

    uint32_t Serial;
    uint32_t Len : 24;
    uint8_t DeviceId : 8;
};

/**
 * M-Stream Header
 * https://afi.jinr.ru/MpdDeviceRawDataFormat
 */
struct __attribute__((packed)) MStreamHeader
{
    void Print()
    {
        printf("\tMStreamHeader:\n");
        printf("Subtype   :   %8u\n", Subtype);
        printf("Length    :   %8u words\n", Len);
        printf("CustomBits:   %8u\n", CustomBits);
    }
    uint8_t Subtype : 2;
    uint32_t Len : 22;
    uint8_t CustomBits : 8;
};

struct __attribute__((packed)) MStreamTAI
{
    bool valid() { return Flags == 2; }
    uint32_t Sec;
    uint8_t Flags : 2;
    uint32_t NSec : 30;
};

struct __attribute__((packed)) MStream0
{
    MStreamTAI Tai;
    uint32_t MaskLo;
    uint32_t MaskHi;
};

/**
 *  https://afi.jinr.ru/DataFormatMSC_ETH
 */
struct __attribute__((packed)) MSC16VE_EHeader
{
    MStreamHeader Hdr;
    MStreamTAI Tai;
    uint8_t NCntrBits : 4;
    uint8_t ChanNumber : 8;   // since V2
    uint8_t ExtCondCnt : 4;   // since V2
    uint32_t : 12;
    uint8_t Version : 4;   // (Version-1)
    uint32_t SliceInt;     // Slice interval [ns]

    uint8_t GetVersion() { return Version + 1; }
};

/**
 * TQDC-E Data Block header
 *  https://afi.jinr.ru/DataFormatTQDC16VSE
 */
struct __attribute__((packed)) TqdcDataHeader
{
    uint16_t Len : 16;
    uint8_t AdcBits : 3;
    uint8_t : 5;
    uint8_t Chan : 4;
    uint8_t DataType : 4;
};

#pragma pack(pop)

#endif
