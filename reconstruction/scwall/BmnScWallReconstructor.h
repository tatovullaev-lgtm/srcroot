/** \file BmnScWallReconstructor.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 03.04.2021
 **/

/** \class BmnScWallReconstructor
 ** \brief Class for BmnScWallEvent reconstruction (creation) from BmnScWallDigi {Data} or BmnScWallDigit {Sim}
 ** \version 1.0
 **/

#ifndef BMNSCWALLRECONSTRUCTOR_H
#define BMNSCWALLRECONSTRUCTOR_H

#include "BmnTask.h"
#include "BmnScWallDigi.h"
#include "BmnScWallDigit.h"
#include "BmnScWallCell.h"
#include "BmnScWallEvent.h"
#include "BmnScWallGeo.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include <TStopwatch.h>
#include <TClonesArray.h>

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

class BmnScWallReconstructor : public BmnTask {
  public:
    BmnScWallReconstructor(bool isExp, bool isGlobalCoordinates);
    ~BmnScWallReconstructor();

    virtual InitStatus Init();
    void ParseGeometry();
    void SetRecoCutsFile(TString reco_cuts_file);
    void ParseCuts();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    virtual void OnlineWrite(const std::unique_ptr<TTree> &resultTree);

  private:
    FairRootManager* fpFairRootMgr = nullptr;
    TClonesArray* fArrayOfDigits; // input
    std::vector<TObject*> GetSelectedDigiVector(TString formulaString); // selected input
    std::vector<TObject*> fSelectedDigiVector; // selected input
    BmnScWallEvent* fBmnScWallEvent = nullptr; // output

    TString fRecoCutsFile; // config file with cuts
    TString fSelectionString = "(1)"; // selection cuts
    bool fIsExp;
    bool fIsGlobal;

    float fworkTime;
  ClassDef(BmnScWallReconstructor,3);
};

#endif /* BMNSCWALLRECONSTRUCTOR_H */
