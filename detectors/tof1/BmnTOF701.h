#ifndef __BMNTOF701_H
#define __BMNTOF701_H 1

#include "BmnTOF1Geo.h"
#include "BmnTOF1GeoPar.h"
#include "BmnTOF1Point.h"
#include "CbmStack.h"
#include "FairDetector.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include <TObjArray.h>
#include <TVirtualMC.h>
#include <assert.h>
#include <iostream>
#include <string>

class BmnTOF701 : public FairDetector
{
  public:
    BmnTOF701();
    BmnTOF701(const char* name, Bool_t active);
    virtual ~BmnTOF701();

    virtual void Initialize();
    virtual Bool_t ProcessHits(FairVolume* vol = nullptr);
    virtual void EndOfEvent();
    virtual void BeginEvent();
    virtual void Register();
    virtual TClonesArray* GetCollection(Int_t iColl) const;
    virtual void Print(Option_t*) const;
    virtual void Reset();
    virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset);
    virtual void ConstructGeometry();
    virtual void ConstructAsciiGeometry();
    virtual Bool_t CheckIfSensitive(std::string name);

    BmnTOF1Point* AddPoint(Int_t trackID,
                           Int_t detID,
                           TVector3 pos,
                           TVector3 mom,
                           Double_t tof,
                           Double_t length,
                           Double_t eLoss,
                           Int_t volUID);

  private:
    TLorentzVector fPos;   //! position
    TLorentzVector fMom;   //! momentum
    Double32_t fTime;      //! time
    Double32_t fLength;    //! length
    Double32_t fELoss;     //! energy loss
    Int_t fPosIndex;       //!
    const double nan;      //! KG: nan is set to -10 000 000, but i think it should be 0

    TClonesArray* fTofCollection;   //! Hit collection

    // reset all parameters
    void ResetParameters();
    BmnTOF701(const BmnTOF701&) = delete;
    BmnTOF701 operator=(const BmnTOF701&) = delete;

    ClassDef(BmnTOF701, 4)
};

inline void BmnTOF701::ResetParameters()
{
    fPos.SetXYZM(nan, nan, nan, nan);
    fMom.SetXYZM(nan, nan, nan, nan);
    fTime = fLength = fELoss = nan;
    fPosIndex = 0;
}

#endif
