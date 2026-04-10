#ifndef BmnCombineVertexFinder_H
#define BmnCombineVertexFinder_H

#include "BmnEnums.h"
#include "BmnGlobalTrack.h"
#include "BmnKalmanFilter.h"
#include "CbmVertex.h"

#include "FairField.h"
#include "FairRunAna.h"
#include "FairTask.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TVector3.h"
#include <TStopwatch.h>

using namespace std;

class BmnCombineVertexFinder : public FairTask
{
public:
  // Constructors/Destructors ---------
  BmnCombineVertexFinder() {};
  virtual ~BmnCombineVertexFinder();

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();

  CbmVertex CombineVertex(CbmVertex* vrtx, BmnTrack* beamTrack);

private:

  //in branches
  CbmVertex* fCbmVertex;   //!
  CbmVertex* fMpdVertex;   //!
  TClonesArray* fBeamTrackArray;   //!
  //out branches
  TClonesArray* fCombineVertexArray;         //!

  BmnKalmanFilter* fKalman;           //!

  ClassDef(BmnCombineVertexFinder, 1);
};

#endif /* BmnCombineVertexFinder_H */
