#ifndef BMNCONVERTERTOOLS_H
#define BMNCONVERTERTOOLS_H 1

#include <bitset>
// #include <chrono>
// #include <map>
#include <string>
// Nlohmann
#include <nlohmann/json.hpp>
// ROOT
#include <TClonesArray.h>
#include <TString.h>
#include <TTimeStamp.h>
#include <TTree.h>
// FairRoot
#include <FairLogger.h>
// BmnRoot
#include <BmnADCDigit.h>
#include <BmnEnums.h>
#include <BmnFileProp.h>
#include <BmnT0Raw.h>
#include <BmnTrigConfig.h>
#include <RawTypes.h>
#include <SpillStatus.h>

using std::bitset;
using std::map;
using std::vector;

using json = nlohmann::json;

class BmnConverterTools
{
  public:
    static BmnStatus ParseComplexTLV(UInt_t* buf, UInt_t& len, UInt_t& runId);
    static SpillStatus ParseJsonStatus(json& j, Int_t tai_utc_dif = 0);
    static BmnStatus ParseJsonConfig(json& j, BmnTrigConfig& trig_conf);
    static SysPoint ParseTAI(json& j, Int_t tai_utc_dif = 0);
    static BmnStatus ParseRawT0TextConfig(string& s, BmnT0Raw<kT0_BIN_BLOCK_WORDS>* rt0dig);
    static BmnStatus ParseRawFileName(TString s, BmnFileProp& prop);
    static BmnStatus ParseRawFileName(string s, BmnFileProp& prop);
    static TString GetSubNameAfterRunId(TString name);

    /**
     * Parse ADC64
     * format Mstream Waveform V2 from https://afi.jinr.ru/MStreamWaveformDigitizer
     * @param d Data array ptr
     * @param len payload length
     * @param serial Serial number
     * @param arr_selector Function for appropriate TClonesArray selection
     * @return kBMNSUCCESS
     */
    template<typename IntType, typename LamType>
    static BmnStatus Process_ADC64(UInt_t* d, UInt_t len, UInt_t serial, LamType arr_selector)
    {
        UInt_t index = 0;
        TClonesArray* arr = nullptr;
        UInt_t nSmpl = 0;
        vector<IntType> valI;
        while (index < len) {
            MStreamHeader* ms = reinterpret_cast<MStreamHeader*>(d + index);
            //            LOGF(info, "ADC64[%3u/%u] %08x:", index, len ,d[index]);
            index += sizeof(MStreamHeader) / kNBYTESINWORD;
            //            ms->Print();
            UInt_t blockLen = ms->Len;
            //            LOGF(info, "ADC64[%3u/%u] %08x:", index, len ,d[index]);
            switch (ms->Subtype) {
                case 0: {
                    MStream0* ms0 = reinterpret_cast<MStream0*>(d + index);
                    index += sizeof(MStream0) / kNBYTESINWORD;
                    LOGF(debug2, "MS0 taiFlags %u TAI: %s", ms0->Tai.Flags,
                         TTimeStamp(time_t(ms0->Tai.Sec), ms0->Tai.NSec).AsString());
                    UInt_t nCh = static_cast<bitset<kNBYTESINWORD * 8>>(ms0->MaskHi).count()
                                 + static_cast<bitset<kNBYTESINWORD * 8>>(ms0->MaskLo).count();
                    UInt_t payload = len - (sizeof(MStreamHeader) + sizeof(MStream0)) / kNBYTESINWORD;
                    LOGF(debug2, "serial %08X payload %6d nCh %u", serial, payload, nCh);
                    if ((nCh == 0) || (payload == 0))
                        continue;
                    //                    if (ms0->Tai.valid())
                    //                        FillWR(serial, fEventId, ms0->Tai.Sec, ms0->Tai.NSec);
                    if ((payload % nCh != 0)
                        || (payload / nCh <= (sizeof(MStreamHeader) + sizeof(MStreamTAI)) / kNBYTESINWORD))
                        return kBMNERROR;
                    nSmpl = 2 * (payload / nCh - (sizeof(MStreamHeader) + sizeof(MStreamTAI)) / kNBYTESINWORD);
                    LOGF(debug2, "serial %08X payload %6d nCh %u nSmpl %u", serial, payload, nCh, nSmpl);
                    arr = arr_selector(nSmpl);
                    valI.resize(nSmpl, 0);
                } break;
                case 1: {
                    if (arr) {
                        MStreamTAI* mst = reinterpret_cast<MStreamTAI*>(d + index);
                        LOGF(debug2, "MS0 taiFlags %u TAI: %s", mst->Flags,
                             TTimeStamp(time_t(mst->Sec), mst->NSec).AsString());
                        index += sizeof(MStreamTAI) / kNBYTESINWORD;
                        uint8_t iCh = ms->CustomBits;
                        TakeDataWords(nSmpl, d + index, valI);
                        new ((*arr)[arr->GetEntriesFast()]) BmnADCDigit(serial, iCh, nSmpl, valI);
                        index += nSmpl / 2 + nSmpl % 2;
                    } else
                        index += blockLen;
                } break;
                default:
                    LOGF(warning, "Wrong ADC64:Mstream data type %u !", ms->Subtype);
                    index += blockLen;
                    break;
            }
        }
        return kBMNSUCCESS;
    }

  private:
    enum T0Config
    {
        Comment,
        Discriminators,
        DelayLines,
        Switches,
        LV,
        BDMult,
        ExtraDelaysMask,
        Undefined
    };

    template<typename ArrType>
    static void TakeDataWords(UInt_t n, UInt_t* d, ArrType& valI)
    {
        for (UInt_t iWord = 0; iWord < n / 2; ++iWord) {
            valI[2 * iWord + 1] =
                d[iWord] & 0xFFFF;   // take 16 lower bits and put them into corresponded cell of data-array
            valI[2 * iWord] =
                (d[iWord] >> 16) & 0xFFFF;   // take 16 higher bits and put them into corresponded cell of data-array
        }
    }

    static T0Config GetT0Cfg(string str);

    ClassDef(BmnConverterTools, 1);
};

#endif
