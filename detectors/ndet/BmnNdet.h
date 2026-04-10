#ifndef BMNNDET_H
#define BMNNDET_H

#include "BmnNdetGeo.h"
#include "BmnNdetGeoPar.h"
#include "BmnNdetPoint.h"
#include "FairDetector.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include <boost/functional/hash.hpp>
#include <map>
#include <string>
#include <unordered_map>

class BmnNdet : public FairDetector
{
  public:
    /** Default constructor **/
    BmnNdet();

    /** Standard constructor.
     *@param name    Detector name
     *@param active  Sensitivity flag
     **/
    BmnNdet(const char* name, Bool_t active);

    /** Destructor **/
    virtual ~BmnNdet();

    /** Initialises detector and FairRun **/
    virtual void Initialize();

    /** Processes hits and creates BmnNdetPoints.
     *@param vol  Pointer to the active volume
     *@return     kTRUE if the volume is sensitive and a hit is processed, otherwise kFALSE
     **/
    virtual Bool_t ProcessHits(FairVolume* vol = nullptr);

    /** Removes zero-energy hits
     *@param point  Pointer to the hit
     *@return       true if the track has non zero impact, otherwise removes it from collection and returns false
     **/
    bool FinaliseTrackProcessing(BmnNdetPoint* point);

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

    /** Checks whether current volume is vacuum
     *@return        kTRUE if the volume is vacuum, otherwise kFALSE
     **/
    virtual Bool_t CheckIfVacuum();

    /** Retrieves the hit index corresponding to a specific address and trackID.
     *@param trackID  trackID
     *@param address  BmnNdetAddress
     *@return         index of the hit if found, otherwise -1
     **/
    Int_t FindHitIndex(int trackID, uint32_t address);

    /** Retrieves the hit corresponding to a specific address and trackID.
     *@param trackID  trackID
     *@param address  BmnNdetAddress
     *@return         Pointer to the hit if found, otherwise nullptr
     **/
    BmnNdetPoint* GetHit(int trackID, uint32_t address);

    /** Adds the hit to hits collection.
     *@param point  BmnNdetPoint
     *@return       Pointer to the hit
     **/
    BmnNdetPoint* AddHit(BmnNdetPoint* point);

    /** Remove hit from hits collection by index.
     *@param index  hit index
     **/
    void RemoveHit(Int_t index);

    /** Returns index of surface point.
     *@param start track index
     **/
    int GetSurfacePoint(int start_track_id);

  private:
    BmnNdetGeo* fGeoHandler;            // Geometry handler
    TClonesArray* fCollection;          // Output Point collection
    TClonesArray* fSurfaceCollection;   // Output Surface Point collection

    ClassDef(BmnNdet, 8)
};

#endif
