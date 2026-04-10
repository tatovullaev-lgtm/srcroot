/** \file BmnVacWallReconstructor.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 03.04.2021
 **/

/** \class BmnVacWallReconstructor
 ** \brief Class for BmnVacWall points info transition on reconstruction stage
 ** \version 1.0
 **/

#ifndef BMNVacWallRECONSTRUCTOR_H
#define BMNVacWallRECONSTRUCTOR_H

#include "BmnTask.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include <TStopwatch.h>
#include <TClonesArray.h>

#include <iostream>
#include <fstream>

class BmnVacWallReconstructor : public BmnTask {
  public:
    BmnVacWallReconstructor(){};
    ~BmnVacWallReconstructor(){};

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    virtual void OnlineWrite(const std::unique_ptr<TTree> &resultTree);

  private:
    FairRootManager* fpFairRootMgr = nullptr;
    TClonesArray* fArrayOfPoints; // input and output

    float fworkTime;
  ClassDef(BmnVacWallReconstructor,2);
};

#endif /* BMNVacWallRECONSTRUCTOR_H */
