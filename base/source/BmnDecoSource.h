#ifndef BMNDECOSOURCE_H
#define BMNDECOSOURCE_H

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <zmq.h>
// ROOT
#include "TChain.h"
#include "TString.h"

#include <TBufferFile.h>
// FairRoot
#include "FairEventHeader.h"
#include "FairOnlineSource.h"
#include "FairRootFileSink.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
// BmnRoot
#include "BmnEnums.h"
#include "BmnEventHeader.h"
#include "DigiArrays.h"

using std::string;
using std::vector;

class BmnDecoSource : public FairOnlineSource
{
  public:
    BmnDecoSource(TString addr = "tcp://localhost:5555", Int_t periodId = 9);
    virtual ~BmnDecoSource();

    Bool_t Init();
    Int_t ReadEvent(UInt_t i = 0);
    void Close();
    //    void Reset();
    //    void FillEventHeader(FairEventHeader* feh);
    virtual Int_t CheckMaxEventNo(Int_t end_ev) { return end_ev; }
    FairRunAna* GetRunInstance() { return fRunInst; }
    void SetRunInstance(FairRunAna* run) { fRunInst = run; }

  private:
    TString GetDstNameFromRunId(Int_t runId)
    {
        TString name(Form("bmn_run%d_dst.root", runId));
        return name;
    }
    FairRunAna* fRunInst;
    zmq_msg_t _msg;
    TBufferFile* _tBuf;
    void* _ctx;
    void* _decoSocket;
    TString _addrString;
    DigiArrays* fDigiArrays;

    Bool_t fFirstEvent;

    Int_t fRunId;
    Int_t fPeriodId;

    Int_t iEventNumber;
    Int_t iT0BranchIndex;
    BmnEventHeader* fEventHeader;
    TClonesArray* fGemDigits;
    TClonesArray* fVspDigits;
    TClonesArray* fSilDigits;
    TClonesArray* fSiBTDigits;
    TClonesArray* fCscDigits;
    TClonesArray* fTof400Digits;
    TClonesArray* fTof700Digits;
    vector<TClonesArray*> fT0Digits;
    //    TClonesArray* fT0Digits;
    bool fInitDone;
    const Int_t TimeDelta = 50000;   // sleep micro seconds

    ClassDef(BmnDecoSource, 1);
};

#endif /* BMNDECOSOURCE_H */