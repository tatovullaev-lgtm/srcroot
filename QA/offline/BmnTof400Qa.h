#include <BmnTask.h>
#include "BmnTofHit.h"
#include <TH1F.h>
#include <TH2F.h>
#include "BmnKalmanFilter.h"
#include <TClonesArray.h>
#include "BmnQaHistoManager.h"

#ifndef BMNTOF400QA_H
#define BMNTOF400QA_H 1

using namespace std;

class BmnTof400Qa : public BmnTask {
public:

    /** Default constructor **/
    BmnTof400Qa();

    /** Destructor **/
    virtual ~BmnTof400Qa() {};

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }

private:

    BmnKalmanFilter* fKalman;       //!
    TClonesArray* fTof400Hits;      //!
    TClonesArray* fStsTracks;       //!
    TClonesArray* fGlobalTracks;    //!

    BmnQaHistoManager* fHistoManager;
    TDirectory* tof400Dir;

    const Int_t nModules = 20;

    ClassDef(BmnTof400Qa, 1);

};

#endif