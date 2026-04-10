/** \file BmnHodoReconstructor.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 03.04.2021
 **/

/** \class BmnHodoReconstructor
 ** \brief Class for BmnHodoEvent reconstruction (creation) from BmnHodoDigi {Data} or BmnHodoDigit {Sim}
 ** \version 1.0
 **/

#ifndef BMNHODORECONSTRUCTOR_H
#define BMNHODORECONSTRUCTOR_H

#include "BmnHodoCuts.h"
#include "BmnHodoDigi.h"
#include "BmnHodoDigit.h"
#include "BmnHodoEvent.h"
#include "BmnHodoRaw2Digit.h"
#include "BmnHodoResolution.h"
#include "BmnHodoStrip.h"
#include "BmnTask.h"
#include "FairLogger.h"
#include "FairRootManager.h"

#include <TClonesArray.h>
#include <TStopwatch.h>
#include <fstream>
#include <iostream>
#include <unordered_map>

using BmnHodoResolution::Kernel;

class BmnHodoReconstructor : public BmnTask
{
  public:
    BmnHodoReconstructor(const std::string& config, bool isExp);
    ~BmnHodoReconstructor();

    virtual InitStatus Init();
    void SetRecoCutsFile(const std::string& cuts);
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    virtual void OnlineWrite(const std::unique_ptr<TTree>& resultTree);

  private:
    FairRootManager* fpFairRootMgr = nullptr;
    TClonesArray* fArrayOfDigits;   // input

    // --- processing pipeline steps ---
    std::vector<BmnHodoDigit*> GetDenoisedDigis();   // noise filtering
    std::vector<BmnHodoDigit*> DesaturateDigis(const std::vector<BmnHodoDigit*>& input) const;
    std::vector<BmnHodoDigit*> ResolvePileup(const std::vector<BmnHodoDigit*>& input);
    std::unordered_map<uint32_t, std::vector<BmnHodoDigit*>> BuildFilteredStripMap();   // strip to digi vec

    std::unordered_map<uint32_t, Kernel> fuoResolutionMap;   // Address to resolution function
    BmnHodoEvent* fBmnHodoEvent = nullptr;                   // output

    BmnHodoCuts* fCuts = nullptr;
    bool fIsExp;
    float fworkTime;

    void ParseConfig(const string& input);

    ClassDef(BmnHodoReconstructor, 3);
};

#endif /* BMNHODORECONSTRUCTOR_H */
