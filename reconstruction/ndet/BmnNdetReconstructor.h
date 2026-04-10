/** \file BmnNdetReconstructor.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 03.04.2021
 **/

/** \class BmnNdetReconstructor
 ** \brief Class for BmnNdetEvent reconstruction (creation) from BmnNdetDigi {Data} or BmnNdetDigit {Sim}
 ** \version 1.0
 **/

#ifndef BMNNDETRECONSTRUCTOR_H
#define BMNNDETRECONSTRUCTOR_H

#include "BmnNdetEvent.h"
#include "BmnNdetGeo.h"
#include "BmnTask.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"

#include <TClonesArray.h>
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>

class BmnNdetReconstructor : public BmnTask
{
  public:
    BmnNdetReconstructor(bool isExp, bool isGlobalCoordinates);
    ~BmnNdetReconstructor();

    virtual InitStatus Init();
    void ParseGeometry();
    void SetRecoCutsFile(TString reco_cuts_file);
    void ParseCuts();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    virtual void OnlineWrite(const std::unique_ptr<TTree>& resultTree);

  private:
    FairRootManager* fpFairRootMgr = nullptr;
    BmnNdetGeo* fGeoHandler = nullptr;                                    // Geometry handler
    TClonesArray* fArrayOfDigits = nullptr;                               // input
    TClonesArray* fArrayOfPoints = nullptr;                               // input
    std::vector<TObject*> GetSelectedDigiVector(TString formulaString);   // selected input
    std::vector<TObject*> fSelectedDigiVector;                            // selected input
    BmnNdetEvent* fBmnNdetEvent = nullptr;                                // output
    TClonesArray* fArrayOfSurfacePoints = nullptr;                        // output

    void MatchMCTracks();

    TString fRecoCutsFile;              // config file with cuts
    TString fSelectionString = "(1)";   // selection cuts
    bool fIsExp;
    bool fDoMatchMC = false;
    bool fIsGlobal;

    float fworkTime;
    ClassDef(BmnNdetReconstructor, 6);
};

#endif /* BMNNDETRECONSTRUCTOR_H */
