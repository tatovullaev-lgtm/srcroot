/** \file BmnFHCalReconstructor.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 03.04.2021
 **/

/** \class BmnFHCalReconstructor
 ** \brief Class for BmnFHCalEvent reconstruction (creation) from BmnFHCalDigi {Data} or BmnFHCalDigit {Sim}
 ** \version 1.0
 **/

#ifndef BMNFHCALRECONSTRUCTOR_H
#define BMNFHCALRECONSTRUCTOR_H

#include "BmnFHCalDigi.h"
#include "BmnFHCalDigit.h"
#include "BmnFHCalEvent.h"
#include "BmnFHCalModule.h"
#include "BmnTask.h"
#include "CbmMCTrack.h"
#include "FairLogger.h"
#include "FairRootManager.h"

#include <TClonesArray.h>
#include <TStopwatch.h>
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>

class BmnFHCalReconstructor : public BmnTask
{
  public:
    BmnFHCalReconstructor(bool isExp, bool isGlobalCoordinates);
    ~BmnFHCalReconstructor();

    virtual InitStatus Init();
    void ParseGeometry();
    void SetRecoCutsFile(TString reco_cuts_file);
    void ParseCuts();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    virtual void OnlineWrite(const std::unique_ptr<TTree>& resultTree);

  private:
    FairRootManager* fpFairRootMgr = nullptr;
    TClonesArray* fArrayOfDigits = nullptr;                               // input
    TClonesArray* fArrayOfPoints = nullptr;                               // input
    std::vector<TObject*> GetSelectedDigiVector(TString formulaString);   // selected input
    std::vector<TObject*> fSelectedDigiVector;                            // selected input
    BmnFHCalEvent* fBmnFHCalEvent = nullptr;                              // output
    void MatchMCTracks();

    TString fRecoCutsFile;              // config file with cuts
    TString fSelectionString = "(1)";   // selection cuts
    bool fIsExp;
    bool fIsGlobal;
    bool fDoMatchMC = false;

    float fworkTime;
    ClassDef(BmnFHCalReconstructor, 6);
};

#endif /* BMNFHCALRECONSTRUCTOR_H */
