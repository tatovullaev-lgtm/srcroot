#include <BmnTask.h>
#include "BmnTofHit.h"
#include <TH1F.h>
#include <TH2F.h>
#include "BmnKalmanFilter.h"
#include <TClonesArray.h>
#include "BmnQaHistoManager.h"
#include "DstRunHeader.h"
#include "CbmVertex.h"
#include "CbmStsDigiScheme.h"
#include "CbmKFTrack.h"
#include "CbmStsHit.h"

#ifndef BMNTOF700QA_H
#define BMNTOF700QA_H 1

using namespace std;

class BmnTof700Qa : public BmnTask {
public:

    /** Default constructor **/
    BmnTof700Qa();

    /** Destructor **/
    virtual ~BmnTof700Qa() {};

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }

    Bool_t PropagateToZ(FairTrackParam *par, Double_t zDst, Int_t pdg, Int_t kalmanType, Double_t* len);
    void PropagateToZ(FairTrackParam* par, Double_t zDst, Int_t pdg, Int_t kalmanType);

    void SetKalmanType(Int_t type) { fKalmanType = type; };
    void SetIsField(Bool_t f) { fIsField = f; };

private:

    Int_t fKalmanType;
    Bool_t fIsField;

    Int_t fEventId;

    BmnKalmanFilter* fKalman;       //!
    TClonesArray* fTof700Hits;      //!
    TClonesArray* fStsHits;         //!
    TClonesArray* fStsTracks;       //!
    TClonesArray* fGlobalTracks;    //!
    CbmVertex* fVertex;    //!
    DstRunHeader* fRunHeader;    //!

    BmnQaHistoManager* fHistoManager;

    void CreateHistograms();
    Float_t GetDxMom(FairTrackParam* par);
    Float_t GetDyMom(FairTrackParam* par);
    Float_t GetSigxMom(FairTrackParam* par);
    Float_t GetSigyMom(FairTrackParam* par);
    

    const static Int_t nModules = 60;
    const static Int_t nStrips = 32;
    Float_t timeShifts[nModules][nStrips];

    ClassDef(BmnTof700Qa, 1);

};

#endif