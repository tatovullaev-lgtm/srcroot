#ifndef BMNMONITOR_H
#define BMNMONITOR_H 1
// STL
#include <exception>
#include <iterator>
#include <regex>
// Auxillary
#include <zmq.h>
// FairSoft
#include <TBufferFile.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TColor.h>
#include <TFile.h>
#include <TFolder.h>
#include <THttpServer.h>
#include <TList.h>
#include <TNamed.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
// BmnRoot
#include <BmnEventHeader.h>
#include <BmnRawDataDecoder.h>
// BmnRoot Monitor
// #include "BmnDataReceiver.h"
#include "BmnOnlineDecoder.h"

#include <BmnRunInfo.h>

#define TTREE_MAX_SIZE 3e11

class BmnMonitor : public TNamed
{
  public:
    BmnMonitor();
    virtual ~BmnMonitor();
    void MonitorStreamZ(TString dir, TString refDir = "", TString decoAddr = "localhost", Int_t webPort = 9000);
    TObjArray* GetAlikeRunsByElog(uint32_t periodID, uint32_t rinID);
    TObjArray* GetAlikeRunsByUniDB(uint32_t periodID, uint32_t rinID);

    void SetPeriodID(Int_t v) { this->fPeriodID = v; }

    Int_t GetPeriodID() const { return fPeriodID; }

    void SetBmnSetup(BmnSetup v) { fSetup = v; }

    BmnSetup GetBmnSetup() const { return fSetup; }

  private:
    void InitServer();
    void ProcessDigi(Int_t iEv);
    void RegisterAll();
    void UpdateRuns();
    BmnStatus CreateFile(uint32_t runID = 0);
    BmnStatus OpenStream();
    void FinishRun();
    void InitHistVectors(vector<BmnHist*>& vec, TString refName = "");

    void* _ctx;
    void* _decoSocket;
    TString _curFile;
    TString _curDir;
    TString _refDir;
    TString fRawDecoAddr;
    TTree* fRecoTree;
    TTree* fRecoTree4Show;
    TFile* fHistOut;
    TFile* fHistOutTemp;
    THttpServer* fServer;
    DigiArrays* fDigiArrays;

    vector<BmnHist*> bhVec;
    vector<BmnHist*> bhVec4show;

    TCanvas* infoCanvas;
    //    TList *refList;
    TList* refTable;
    TList* runPub;
    BmnRunInfo* CurRun;

    //    BmnDataReceiver* dataReceiver;
    BmnOnlineDecoder* onlineDecoder;

    Bool_t keepWorking;
    Int_t _webPort;
    Int_t fTest;
    uint32_t fPeriodID;
    BmnSetup fSetup;
    uint32_t fRunID;
    Int_t fEvents;
    Int_t fNEventsSinceUpdate;
    BmnWorkerState fState;
    Int_t itersToUpdate;
    Int_t decoTimeout;

    ClassDef(BmnMonitor, 1)
};

#endif
