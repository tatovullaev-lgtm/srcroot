#ifndef BMNMQSOURCE_H
#define BMNMQSOURCE_H

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
#include "FairRootManager.h"
#include "FairRunAna.h"
// BmnRoot
#include "BmnEnums.h"
#include "BmnEventHeader.h"
#include "BmnParts.h"
#include "CbmVertex.h"
#include "DstEventHeader.h"

class BmnMQSource : public FairOnlineSource
{
  public:
    BmnMQSource(std::string addr = "tcp://localhost:6666", Bool_t toFile = kFALSE);
    virtual ~BmnMQSource();

    Bool_t Init();
    Int_t ReadEvent(UInt_t i = 0);
    void Close();
    //    void Reset();
    void FillEventHeader(FairEventHeader* feh);
    virtual Int_t CheckMaxEventNo(Int_t end_ev) { return end_ev; }
    FairRunAna* GetRunInstance() { return fRunInst; }
    void SetRunInstance(FairRunAna* run) { fRunInst = run; }

  private:
    TString GetDstNameFromRunId(Int_t runId)
    {
        TString name(Form("bmn_run%d_dst.root", runId));
        return name;
    }
    Bool_t InitZMQ();
    FairRunAna* fRunInst;
    zmq_msg_t _msg;
    TBufferFile* _tBuf;
    void* _ctx;
    void* _decoSocket;
    std::string _addrString;
    std::vector<TNamed*> fNamVec;
    std::vector<TClonesArray*> fArrVec;

    const Int_t TimeDelta = 50000;   // sleep micro seconds
    Bool_t fFirstEvent;
    Bool_t fToFile;

    Int_t fRunId;
    Int_t fPeriodId;

    Int_t iEventNumber;
    Int_t iT0BranchIndex;
    FairEventHeader* fEventHeader;
    ClassDef(BmnMQSource, 1);
};

#endif /* BMNMQSOURCE_H */
