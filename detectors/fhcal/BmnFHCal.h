#ifndef BMNFHCAL_H
#define BMNFHCAL_H

#include "BmnFHCalGeo.h"
#include "BmnFHCalGeoPar.h"
#include "BmnFHCalPoint.h"
#include "FairDetector.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include <map>
#include <string>
#include <unordered_map>

class BmnFHCal : public FairDetector
{
  public:
    /** Default constructor **/
    BmnFHCal();

    /** Standard constructor.
     *@param name    Detector name
     *@param active  Sensitivity flag
     **/
    BmnFHCal(const char* name, Bool_t active);

    /** Destructor **/
    virtual ~BmnFHCal();

    /** Initializes detector and FairRun **/
    virtual void Initialize() override;

    /** Processes hits and creates BmnFHCalPoints.
     *@param vol  Pointer to the active volume
     *@return     kTRUE if the volume is sensitive and a hit is processed, otherwise kFALSE
     **/
    virtual Bool_t ProcessHits(FairVolume* vol = nullptr) override;

    /** Called at the end of each event **/
    virtual void EndOfEvent() override;

    /** Called at the beginning of each event **/
    virtual void BeginEvent() override;

    /** Registers the hit collection in the ROOT manager **/
    virtual void Register() override;

    /** Accessor to the hit collection
     *@param iColl  Index of the collection
     *@return       Pointer to the hit collection
     **/
    virtual TClonesArray* GetCollection(Int_t iColl) const override;

    /** Screen output of hit collection **/
    virtual void Print(Option_t*) const override;

    /** Clears the hit collection **/
    virtual void Reset() override;

    /** Constructs geometry from files **/
    virtual void ConstructGeometry() override;

    /** Checks whether a volume is sensitive based on its name.
     *@param name    Volume name
     *@return        kTRUE if the volume is sensitive, otherwise kFALSE
     **/
    virtual Bool_t CheckIfSensitive(std::string name) override;

    /** Adds the hit to hits collection.
     *@param point  BmnFHCalPoint
     *@return       Pointer to the hit
     **/
    BmnFHCalPoint* AddHit(BmnFHCalPoint* point);

    /** Updates an existing hit by merging information from a new hit,
     * weighting position, momentum, time, and length according to their energy loss.
     * @param existing      Reference to the existing BmnFHCalPoint hit, which will be updated
     * @param update        Reference to the new BmnFHCalPoint hit, providing additional information
     **/
    void UpdateHit(BmnFHCalPoint& existing, const BmnFHCalPoint& update);

    /** Retrieves a hit from the collection based on the detector address.
     * @param address  Detector address of the hit
     * @return         Pointer to the retrieved hit, or nullptr if not found
     **/
    BmnFHCalPoint* GetHit(uint32_t address);

    /** Determines the surface-level Monte Carlo track ID associated with the given
     * starting track ID.
     * @param start_track_id  Initial track ID to trace back to the surface MC track
     * @return                Surface-level MC track ID
     **/
    int GetSurfaceMCTrack(int start_track_id);

  private:
    TClonesArray* fCollection;                  // Collection of hits for the event
    std::unique_ptr<BmnFHCalGeo> fGeoHandler;   // Pointer to the geometry handler
    std::unique_ptr<BmnFHCalPoint> fPoint;      // Temp storage for the current hit being processed
    std::map<uint32_t, FairMultiLinkedData> fMultiLinkMap;

    ClassDefOverride(BmnFHCal, 4)
};

#endif