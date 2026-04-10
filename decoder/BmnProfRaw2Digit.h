#ifndef BMNPROFRAW2DIGIT_H
#define BMNPROFRAW2DIGIT_H

// Auxillary
#include <boost/circular_buffer.hpp>
#include <boost/exception/all.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
// Root
#include "TClonesArray.h"
#include "TH1F.h"
#include "TMath.h"
#include "TString.h"
#include "TTree.h"
// BmnRoot
#include "BmnADCDigit.h"
#include "BmnAdcProcessor.h"
#include "BmnEnums.h"
#include "BmnMath.h"
#include "BmnProfRawTools.h"
#include "BmnSiProfDigit.h"

#include <BmnSiBTStationSet.h>

using std::map;
using std::vector;
using namespace TMath;
namespace pt = boost::property_tree;

struct ProfiLocal
{
    char ChannelName = 0;
    char LayerType = 0;
    uint16_t LayerId = 0;
    int16_t StripMap[BmnProfRawTools::ChannelCnt()] = {};
};

typedef map<uint16_t, ProfiLocal> LayerIdProfiMap;          // layerId -> layer local map
typedef map<uint16_t, LayerIdProfiMap> Mod2LocalProfiMap;   // module -> local maps

struct ProfiInfo
{
    uint16_t StationId = 0;
    uint16_t ModuleId = 0;
    ProfiLocal* Layer = nullptr;
};
typedef map<PlMapKey, ProfiInfo> GlobalProfiMap;   // {serial, channel} -> info

class BmnProfRaw2Digit : public BmnAdcProcessor
{
  public:
    BmnProfRaw2Digit(Int_t period, Int_t run);
    ~BmnProfRaw2Digit();

    GlobalProfiMap& GetGlobalMap() { return fGlobalMap; }

    BmnStatus FillEvent(TClonesArray* adc, TClonesArray* csc);
    BmnStatus FillProfiles(TClonesArray* adc);
    BmnStatus FillNoisyChannels();

    static unique_ptr<BmnSiProfStationSet> GetProfStationSet(Int_t period);

    int FillExtractedBits(TClonesArray* fInAdcArray, TClonesArray* fWorkAdcArray)
    {
        for (Int_t idigit = 0; idigit < fInAdcArray->GetEntriesFast(); idigit++) {
            BmnADCDigit* digit = (BmnADCDigit*)fInAdcArray->At(idigit);
            uint16_t* arr = digit->GetUShortValue();
            for (auto& m : fChannelMaps)
                for (auto& vecEl : m.second) {
                    int16_t arr_out[BmnProfRawTools::ChannelCnt()] = {};
                    for (size_t j = 0; j < BmnProfRawTools::ChannelCnt(); j++) {
                        arr_out[j] = BmnProfRawTools::adc_ch(arr, j * 6, vecEl.second.ChannelName)
                                     * 16;   // x16 because ADC processor works also with ADC64 shifted values
                        //                        printf("arr[%c][%2ld] = %hd\n", vecEl.second.ChannelName, j,
                        //                        arr_out[j]);
                    }
                    /*BmnADCDigit* wd = */ new ((*fWorkAdcArray)[fWorkAdcArray->GetEntriesFast()])
                        BmnADCDigit(digit->GetSerial(),
                                    vecEl.second.LayerId,   // for simplicity: layerId == ChannelId
                                    BmnProfRawTools::ChannelCnt(), arr_out);
                    //                int16_t * arr_out = wd->GetShortValue();
                }
        }
        return 0;
    }

  private:
    string fLocalMapFileName;
    string fGlobalMapFileName;

    GlobalProfiMap fGlobalMap;
    Mod2LocalProfiMap fChannelMaps;

    unique_ptr<BmnSiBTStationSet> fSiBTStationSet;

    void MapStrip(ProfiInfo& info,
                  uint16_t ch,
                  Int_t iSmpl,
                  uint16_t& station,
                  uint16_t& mod,
                  uint16_t& lay,
                  int16_t& strip);
    void ProcessAdc(TClonesArray* adc, TClonesArray* csc, Bool_t doFill);
    BmnStatus ReadGlobalMapFile(string name);
    BmnStatus ReadLocalMapFile(string name);

    ClassDef(BmnProfRaw2Digit, 1);
};

#endif /* BMNPROFRAW2DIGIT_H */
