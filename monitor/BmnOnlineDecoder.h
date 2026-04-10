#ifndef BMNONLINEDECODER_H
#define BMNONLINEDECODER_H 1
#include <climits>
// #include <regex>
// FairSoft
#include "TBuffer.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TMessage.h"
#include "TNamed.h"
#include "TObjString.h"
#include "TServerSocket.h"
#include "TSocket.h"
#include "TString.h"

// BmnRoot
#include "BmnDataReceiver.h"
#include "BmnEventHeader.h"
#include "BmnHist.h"
#include "BmnMath.h"

#include <BmnRawDataDecoder.h>

#define RAW_DECODER_SOCKET_PORT 5555
#define RUN_FILE_CHECK_PERIOD 1e5
#define DECO_SOCK_WAIT_PERIOD 20
#define DECO_SOCK_WAIT_LIMIT 40e3
#define INOTIF_BUF_LEN (255 * (sizeof(struct inotify_event) + 255))
#define MIN_REMNANT_LEN 300 * 1024   // crutch actually, will be removed after parser improvement
#define MPD_EVENT_HEAD_WORDS_OLD 3   // sync + payload lenght + iEv
#define MPD_EVENT_HEAD_WORDS 2       // sync + payload lenght

#define STRIP_COMP_COLS 2
#define PAD_WIDTH_SIL 1120
#define PAD_HEIGHT_SIL 630

using std::unique_ptr;
using std::vector;

class BmnOnlineDecoder : public TNamed
{
  public:
    BmnOnlineDecoder();
    virtual ~BmnOnlineDecoder();
    BmnStatus Decode(TString dirname, TString startFile, Bool_t runCurrent);
    BmnStatus OpenStream();
    void ProcessStream();
    BmnStatus CloseStream();
    BmnStatus BatchDirectory(TString dirname);
    BmnStatus BatchDirectoryToSocket(TString dirname);

    void SetBmnSetup(BmnSetup v) { this->fBmnSetup = v; }

    BmnSetup GetBmnSetup() const { return fBmnSetup; }

    void SetPeriodID(uint32_t v) { this->fPeriodID = v; }

    uint32_t GetPeriodID() const { return fPeriodID; }

    void SetDetectorSetup(map<DetectorId, bool> setup) { fDetectorSetup = setup; }

    void SetDaqAddress(TString addr) { fDAQAddr = addr; }
    static void StripView(TString OrigFileName,
                          TString TestFileName,
                          uint32_t periodID = 8,
                          uint32_t runID = 0,
                          BmnSetup fSetup = kBMNSETUP,
                          bool FillOccupancy = true);

  private:
    template<typename SST>
    static void InitCanvas(TString DetName,
                           unique_ptr<SST>& stationSet,
                           TCanvas*& canStrip,
                           vector<PadInfo*>& canStripPads,
                           vector<vector<vector<TH1F*>>>* histStrip)
    {
        Int_t sumMods = 0;
        Int_t maxLayers = 0;
        Bool_t isSil = (typeid(SST) == typeid(BmnSiliconStationSet));
        for (Int_t iCol = 0; iCol < STRIP_COMP_COLS; iCol++) {
            for (Int_t iStation = 0; iStation < stationSet->GetNStations(); iStation++) {
                vector<vector<TH1F*>> rowGEM;
                auto* st = stationSet->GetStation(iStation);
                sumMods += st->GetNModules();
                for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                    vector<TH1F*> colGEM;
                    auto* mod = st->GetModule(iModule);
                    if (isSil)
                        maxLayers = 2;
                    else {
                        if (maxLayers < mod->GetNStripLayers())
                            maxLayers = mod->GetNStripLayers();
                    }
                    for (Int_t iLayer = 0; iLayer < (isSil ? 2 : mod->GetNStripLayers()); iLayer++) {
                        auto lay = mod->GetStripLayer(iLayer);
                        if (isSil && iStation != 0) {
                            auto& lay1 = mod->GetStripLayer(iLayer * 2);
                            auto& lay2 = mod->GetStripLayer(iLayer * 2 + 1);
                            if (lay1.GetLastStripNumber() > lay2.GetLastStripNumber())
                                lay = lay1;
                            else
                                lay = lay2;
                        }
                        TString name = Form("%s_%d_Station_%d_module_%d_layer_%d", DetName.Data(), iCol, iStation,
                                            iModule, iLayer);
                        //                        cout<< name << endl;
                        TString title = Form("Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                        TH1F* h;
                        if (isSil)
                            h = new TH1F(name, title, lay.GetLastStripNumber() + 1, 0, lay.GetLastStripNumber() + 1);
                        else
                            h = new TH1F(name, title, lay.GetNStrips(), 0, lay.GetNStrips());
                        h->GetXaxis()->SetTitle("Strip Number");
                        h->GetYaxis()->SetTitle("Activation Count");
                        BmnHist::SetHistStyleTH1(h);
                        colGEM.push_back(h);
                    }
                    rowGEM.push_back(colGEM);
                }
                histStrip[iCol].push_back(rowGEM);
            }
        }
        sumMods = sumMods / STRIP_COMP_COLS;
        TString name = DetName + "Canvas";
        canStrip = new TCanvas(name, name, PAD_WIDTH_SIL * maxLayers, PAD_HEIGHT_SIL * sumMods);
        canStrip->Divide(maxLayers, sumMods, 0.01 / maxLayers, 0.01 / sumMods);
        Int_t modCtr = 0;
        canStripPads.resize(sumMods * maxLayers, nullptr);
        for (Int_t iStation = 0; iStation < stationSet->GetNStations(); iStation++) {
            auto* st = stationSet->GetStation(iStation);
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                auto* mod = st->GetModule(iModule);
                for (Int_t iLayer = 0; iLayer < (isSil ? 2 : mod->GetNStripLayers()); iLayer++) {
                    Int_t iPad = modCtr * maxLayers + iLayer;
                    PadInfo* p = new PadInfo();
                    p->current = histStrip[0][iStation][iModule][iLayer];
                    p->ref = histStrip[1][iStation][iModule][iLayer];
                    p->ref->SetLineColor(kRed);
                    p->normalize = false;
                    canStripPads[iPad] = p;
                    canStrip->GetPad(iPad + 1)->SetGrid();
                }
                modCtr++;
            }
        }
    }

    template<typename DigiType>
    static void FillHists(vector<vector<vector<TH1F*>>>& hist_vec, TClonesArray* digs, bool set_bin_content = false)
    {
        for (Int_t iDig = 0; iDig < digs->GetEntriesFast(); iDig++) {
            DigiType* dig = (DigiType*)digs->UncheckedAt(iDig);
            if (!(dig->IsGoodDigit()))
                continue;
            Int_t module = dig->GetModule();
            Int_t station = dig->GetStation();
            Int_t layer = dig->GetStripLayer();
            Int_t strip = dig->GetStripNumber();
            if (set_bin_content) {
                hist_vec[station][module][layer]->SetBinContent(strip, dig->GetStripNoise());
            } else {
                hist_vec[station][module][layer]->Fill(strip);
            }
        }
    }
    //    void signal_handler(int sig);
    BmnStatus InitDecoder(string);
    BmnStatus InitDecoder(TString);
    BmnStatus InitDecoder(uint32_t runID);
    BmnStatus ConnectDataSocket();
    BmnStatus ConnectDigiSocket();
    BmnStatus StartNewRun(uint32_t runID);
    BmnStatus AddRun2DB();
    void ProcessFileRun(TString digiName, UInt_t timeLimit = WAIT_LIMIT);
    static TString WatchNext(TString dirname, TString filename, Int_t cycleWait);
    static TString WatchNext(Int_t inotifDir, Int_t cycleWait);
    static Int_t GetRunIdFromName(TString name);

    void* _ctx;
    void* _decoSocket;
    TString fDAQAddr;
    BmnSetup fBmnSetup;
    BmnRawDataDecoder* rawDataDecoder;
    map<DetectorId, bool> fDetectorSetup;
    TString _curFile;
    TString _curDir;
    Int_t fEvents;
    uint32_t fPeriodID;
    uint32_t fRunID;
    BmnDataReceiver* dataReceiver;
    Int_t _inotifDir;
    Int_t _inotifDirW;
    Int_t _inotifFile;
    Int_t _inotifFileW;

    Bool_t fKeepWorking;

    deque<UInt_t> data_queue;
    void* _socket_mcast;
    void* _socket_data;
    Byte_t buf[MAX_BUF_LEN];
    //    UInt_t buf[MAX_BUF_LEN];

    ClassDef(BmnOnlineDecoder, 1)
};

#endif /* BMNONLINEDECODER_H */
