#ifndef BMNVACWALL_H
#define BMNVACWALL_H

#include "BmnZdcGeo.h"
#include "FairDetector.h"
#include "FairMCPoint.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"

class BmnVacWall : public FairDetector
{
  public:
    /** Default constructor **/
    BmnVacWall();

    /** Standard constructor.
     *@param name    Detector name
     *@param active  Sensitivity flag
     **/
    BmnVacWall(const char* name, Bool_t active);

    /** Destructor **/
    virtual ~BmnVacWall();

    /** Initialises detector and FairRun **/
    virtual void Initialize();

    /** Processes hits and creates FairMCPoints.
     *@param vol  Pointer to the active volume
     *@return     kTRUE if the volume is sensitive and a hit is processed, otherwise kFALSE
     **/
    virtual Bool_t ProcessHits(FairVolume* vol = nullptr);

    /** Called at the end of each event **/
    virtual void EndOfEvent();

    /** Called at the beginning of each event **/
    virtual void BeginEvent();

    /** Registers the hit collection in the ROOT manager **/
    virtual void Register();

    /** Accessor to the hit collection
     *@param iColl  Index of the collection
     *@return       Pointer to the hit collection
     **/
    virtual TClonesArray* GetCollection(Int_t iColl) const;

    /** Screen output of hit collection **/
    virtual void Print(Option_t*) const;

    /** Clears the hit collection **/
    virtual void Reset();

    /** Constructs geometry from files **/
    virtual void ConstructGeometry();

    /** Checks whether a volume is sensitive based on its name.
     *@param name    Volume name
     *@return        kTRUE if the volume is sensitive, otherwise kFALSE
     **/
    virtual Bool_t CheckIfSensitive(std::string name);

    /** Adds the hit to hits collection.
     *@param point  FairMCPoint
     *@return       Pointer to the hit
     **/
    FairMCPoint* AddHit(FairMCPoint* point);

    /** Remove hit from hits collection by index.
     *@param index  hit index
     **/
    void RemoveHit(Int_t index);

  private:
    BmnZdcGeo* fGeoHandler;
    TClonesArray* fCollection;   // Output collection

    ClassDef(BmnVacWall, 7)
};

#endif
