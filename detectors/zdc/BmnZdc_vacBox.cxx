
/*****************************************************************************
 *
 *         Class BmnZdc
 *         
 *  Adopted for BMN by:   Elena Litvinenko (EL)
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015
 *  Last update:  22-Feb-2012 (EL)  
 *  Last update:  05-May-2016 (MG)  
 *
 *****************************************************************************/

#include <iostream>

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoArb8.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "BmnZdcGeo.h"
#include "FairGeoRootBuilder.h"
#include "CbmStack.h"
#include "BmnZdc.h"
#include "BmnZdcPoint.h"

#include "FairRootManager.h"
#include "FairVolume.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"

#include "TParticlePDG.h"

// -----   Default constructor   -------------------------------------------
BmnZdc::BmnZdc() {
  fZdcCollection        = new TClonesArray("BmnZdcPoint");
  volDetector = 0;
  fPosIndex   = 0; 
  // fpreflag = 0;  
  //fpostflag = 0;
  fVSCVolId=0;
  fVSCVolId_after=0;
  fEventID=-1; 
  fVerboseLevel = 1;

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnZdc::BmnZdc(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fZdcCollection        = new TClonesArray("BmnZdcPoint");
    fPosIndex   = 0;
    volDetector = 0;
    //fpreflag = 0;  
    //fpostflag = 0;
    fVSCVolId=0;
    fVSCVolId_after=0;
    fEventID=-1;
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
BmnZdc::~BmnZdc() {
  if (fZdcCollection) {
    fZdcCollection->Delete();
    delete fZdcCollection;
  }
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void BmnZdc::Initialize() {
  // Init function
  //cout <<"Initialize marina " <<endl;
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();

  fVSCVolId = gMC->VolId("zdc01s");
  fVSCVolId_after = gMC->VolId("zdc01s_after");
  //fVSCVETOVolId = gMC->VolId("ndet01s_VETO");
}
// -------------------------------------------------------------------------
void BmnZdc::BeginEvent(){
  // Begin of the event
  //cout <<"BeginEvent marina " <<endl;
  
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnZdc::ProcessHits(FairVolume* vol) {

  Int_t copyNoVSC,copyNoVTYVEC,copyNoVMOD,copyNoVZDC;
  //Int_t copyNoVSCNICA,copyNoVTYVECNICA,copyNoVMODNICA,copyNoVZDCNICA;
  Int_t copyNoVSC_after,copyNoVTYVECNICA,copyNoVMODNICA,copyNoVZDCNICA;
  Int_t copyNoVSCVETO,copyNoVTYVECVETO,copyNoVMODVETO,copyNoVZDCVETO;
  Int_t copyNoVSCCom,copyNoVTYVECCom,copyNoVMODCom,copyNoVZDCCom;

  Int_t      ivol;
  TLorentzVector tPos1, tMom1;

  Int_t module, module_nica; 
  Int_t slice, slice_nica;
  

  TLorentzVector tPos, tMom;
  Int_t copyNo;
  Int_t copyNo_slice; 
  
  Double_t time=0;
  Double_t length =0;

  TParticle* part;
  Double_t charge;


  //Double_t timeCut = 45;
  Double_t timeCut = 55;
  Double_t  QCF=1; //quenching for Birk
  Double_t  BirkConst = 12.6; //0.126 mm/MeV for polystyrene 
  //0.126 *(0.1/0.001) = 12.6 cm/GeV
  //(0.126 mm/MeV - from Wikipedia, 0.07943mm/MeV є in Geant4)
  
  if (gMC->CurrentVolID(copyNoVSC) != fVSCVolId &&
      gMC->CurrentVolID(copyNoVSC_after) != fVSCVolId_after) 
{
    return kFALSE;
  }
  
  ivol    = vol->getMCid();
  fEventID = gMC->CurrentEvent();

  if (gMC->CurrentVolID(copyNoVSC) == fVSCVolId || gMC->CurrentVolID(copyNoVSC_after) == fVSCVolId_after) {
    //gMC->CurrentVolOffID(0, slice); //no tyvec
    //gMC->CurrentVolOffID(1, module);
    gMC->CurrentVolOffID(0, copyNo); //planeNb 1-6
    gMC->CurrentVolOffID(1, copyNo_slice); 
    //copyNoVTYVECCom = slice; copyNoVMODCom = module;
  }

    if (gMC->IsTrackEntering()) {

     ResetParameters();
      fELoss = 0.;
      time    = 0.;
      length  = 0.;
      gMC->TrackPosition(tPos);
      gMC->TrackMomentum(tMom);

	fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
	part    = gMC->GetStack()->GetCurrentTrack();

    }//if (gMC->IsTrackEntering())
  
  
    if ( gMC->IsTrackExiting()) {

      gMC->TrackPosition(tPos);
      gMC->TrackMomentum(tMom);
      part    = gMC->GetStack()->GetCurrentTrack();
      //fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

// Create BmnNdetPoint
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      time    = gMC->TrackTime() * 1.0e09;
      //length  += gMC->TrackLength();
            
      //if(fELoss>0) {

	length = 1;     

	//cout <<"Exit_WALL " <<length <<" " <<gMC->CurrentVolPath() <<" " <<copyNo_slice <<" " <<copyNo <<" " <<tPos.X() <<" " <<tPos.Y() <<" " <<tPos.Z() <<" " <<tMom.Px() <<" " <<tMom.Py() <<" " <<tMom.Pz() <<" " <<time <<" " <<fELoss  <<" " <<part->GetPDG()->PdgCode() <<" " <<part->GetMother(0) <<endl;
      AddHit(fTrackID, ivol, copyNo_slice, copyNo, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	     TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	     time, length, fELoss);
      /*
      Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
//       Int_t nZdcPoints = (points & (1<<30)) >> 30;
//       nZdcPoints ++;
//       if (nZdcPoints > 1) nZdcPoints = 1;
//      points = ( points & ( ~ (1<<30) ) ) | (nZdcPoints << 30);
      points = ( points & ( ~ (1<<30) ) ) | (1 << 30);
      gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
      */
      ((CbmStack*)gMC->GetStack())->AddPoint(kZDC);

	//}//if(fELoss>0)
    }//if ( gMC->IsTrackExiting()) {
    /*
       Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
//       Int_t nZdcPoints = (points & (1<<30)) >> 30;
//       nZdcPoints ++;
//       if (nZdcPoints > 1) nZdcPoints = 1;
//      points = ( points & ( ~ (1<<30) ) ) | (nZdcPoints << 30);
      points = ( points & ( ~ (1<<30) ) ) | (1 << 30);
      gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);

      ((CbmStack*)gMC->GetStack())->AddPoint(kZDC);
    */







  //////////////////////////////////////////////////
  //cout <<"ProcessHits marina " <<endl;
  // if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;
  
  //Int_t      ivol    = vol->getMCid();
  //TLorentzVector tPos1, tMom1;
  

//commented for vacuum FWall 
/*
#ifndef EDEBUG
      if (fELoss == 0. ) return kFALSE;
#else
      if ((fELoss == 0. ) && 
	  (!((gMC->GetStack()->GetCurrentTrack()->GetPdgCode()==2112)&&(gMC->GetStack()->GetCurrentTrack()->GetMother(0)==-1)))
) return kFALSE;
#endif
*/
//END comment for vacuum FWall 
/*
    if (gMC->IsTrackEntering()) {

      ResetParameters();
      fELoss = 0.;
    }
     if ( gMC->IsTrackExiting()    ||
	 gMC->IsTrackStop()       ||
	 gMC->IsTrackDisappeared()   ) {
*/


/*            
      //old geom
      Int_t copyNo;
      Int_t ivol1 = gMC->CurrentVolID(copyNo);
      //      ivol1 = vol->getVolumeId();
      Int_t iCell, iCell2 ;
      gMC->CurrentVolOffID(1, iCell); 
      gMC->CurrentVolOffID(2, iCell2); 
 */
/*      	
      //new geom
      Int_t copyNo;
      Int_t copyNo_slice; 
      //Int_t ivol1 = gMC->CurrentVolID(copyNo_slice);
      //      ivol1 = vol->getVolumeId();
      Int_t iCell, iCell2 ;
      gMC->CurrentVolOffID(1, copyNo_slice); 
      gMC->CurrentVolOffID(0, copyNo); 

      TParticle* part    = gMC->GetStack()->GetCurrentTrack();
      Double_t charge = part->GetPDG()->Charge() / 3. ;

 // Create BmnZdcPoint
     fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      Double_t time    = gMC->TrackTime() * 1.0e09;
      Double_t length  = gMC->TrackLength();
      TLorentzVector tPos, tMom;
      //gMC->TrackPosition(tPos);
      //gMC->TrackMomentum(tMom);
      Double_t ZZZ; 
    Double_t eLoss   = gMC->Edep();
    if (eLoss != 0.) 
      fELoss += eLoss;

    //if (gMC->IsTrackEntering()) {

    //ResetParameters();
    //fELoss = 0.;
	length = 0; //entering
	fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
	time    = gMC->TrackTime() * 1.0e09;
	gMC->TrackPosition(tPos);
	gMC->TrackMomentum(tMom);

      //cout <<"Enter " <<gMC->CurrentVolPath() <<" " <<ivol1 <<" " <<copyNo_slice <<" " <<gMC->CurrentVolID(copyNo_slice) <<" " <<gMC->CurrentVolOffID(0, iCell) <<" " <<iCell <<" " <<gMC->CurrentVolOffID(1, copyNo) <<" " <<copyNo <<endl;
      //if(copyNo==1 && gMC->IsTrackEntering()) cout <<"Enter_WALL " <<length <<" " <<gMC->CurrentVolPath() <<" " <<copyNo_slice <<" " <<copyNo <<" " <<tPos.X() <<" " <<tPos.Y() <<" " <<tPos.Z() <<" " <<tMom.Px() <<" " <<tMom.Py() <<" " <<tMom.Pz() <<" " <<time <<" " <<fELoss <<endl;
      cout <<"Exit_WALL " <<length <<" " <<gMC->CurrentVolPath() <<" " <<copyNo_slice <<" " <<copyNo <<" " <<tPos.X() <<" " <<tPos.Y() <<" " <<tPos.Z() <<" " <<tMom.Px() <<" " <<tMom.Py() <<" " <<tMom.Pz() <<" " <<time <<" " <<fELoss  <<endl;
      AddHit(fTrackID, ivol, copyNo_slice, copyNo, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	     TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	     time, length, fELoss);
    }//if (gMC->IsTrackEntering())
*/

    /*
    if ( gMC->IsTrackExiting()    ||
	 gMC->IsTrackStop()       ||
	 gMC->IsTrackDisappeared()   ) {
    */

    /*
    if ( gMC->IsTrackExiting() ) {

      //cout <<"Exit " <<gMC->CurrentVolPath() <<" " <<ivol1 <<" " <<copyNo_slice <<" " <<gMC->CurrentVolID(copyNo_slice) <<" " <<iCell <<" " <<gMC->CurrentVolOffID(0, iCell) <<" " <<copyNo <<" " <<gMC->CurrentVolOffID(1, copyNo) <<endl;

    part    = gMC->GetStack()->GetCurrentTrack();
    
// Create BmnZdcPoint
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    time    = gMC->TrackTime() * 1.0e09;
    //length  += gMC->TrackLength();
    length = 1; //exiting
    */
  	
      /*
#ifdef EDEBUG
      static Bool_t already=0;
      if (lEDEBUGcounter<200) {
	std::cout << "EDEBUG-- BmnZdc::ProcessHits: TrackID:" << fTrackID << 
	  //	  " ELoss: " << fELoss << 
	  //	  	  "   particle: " << (part->GetName()) << 
	  "   " << gMC->CurrentVolPath() << " " << tPos.X()<< " " << tPos.Y() << " " << tPos.Z() << "  " << part->GetPDG()->PdgCode()<<
	  //          "   " << (gMC->GetStack()->GetCurrentTrack()->GetMother(1)) << 
	  //	   	  "   "  << ivol << "=="<< gMC->CurrentVolID(copyNo) << ","<< copyNo <<
	  "   "  << ivol << "  " << vol->getRealName() << "   " <<
	  "   "  << gMC->CurrentVolOffID(1,iCell) << " " <<  gMC->CurrentVolOffName(1) << 
	  "   " << iCell << " " << iCell2 << "  "<< gMC->CurrentVolOffName(2) << 
	  "   " << vol->getModId()<< " " << 
	  //	  " " <<  gMC->CurrentVolOffName(2) << " " <<  gMC->CurrentVolOffName(1) << " " << gMC->CurrentVolOffName(0) << " " 
 	  	  // "   " << vol->getRealName() << "  " << gMC->CurrentVolPath() <<
	  	  // "   ivol,iCell,copyNo= " << ivol << ","<< iCell << ","<< copyNo << 
	  	  // "   " << vol->getRealName() << "  "<< gMC->CurrentVolName() << "  "<< gMC->CurrentVolPath() <<
	  	  // "   "  << ivol << ","<< vol->getVolumeId() << " : "<< gMC->CurrentVolID(copyNo) << ","<< copyNo <<
	          //  "  "<< gMC->CurrentVolOffName(2) << "  " <<
	  //	           "  "<< gMC->CurrentVolOffName(2) << "  "<< gMC->CurrentVolOffName(3) <<
	  std::endl;
	//	vol->getGeoNode()->getMotherNode()->getMotherNode()->getCenterPosition().print();
	lEDEBUGcounter++;
      } 
      if ((iCell==2)&&(lEDEBUGcounter>=100)&&(!already)) {
	already=1;
	lEDEBUGcounter=0;
      }
//       if ((part->GetPdgCode())==321) {
// 	std::cout << "EDEBUG-- BmnZdc::ProcessHits(..)  K+:  " << fTrackID << "   " << (  gMC->IsTrackExiting()) << "  " <<
// 	  (gMC->IsTrackStop()) << "  " << (gMC->IsTrackDisappeared()) << "   " << fELoss << "  " << time << std::endl;
//       }
//#endif

//       if(copyNo==1)
// 	AddHit(fTrackID, ivol, copyNo, iCell, TVector3(tPos1.X(), tPos1.Y(), tPos1.Z()),
// 	       TVector3(tMom1.Px(), tMom1.Py(), tMom1.Pz()),
// 	       time, length, fELoss);
//       else 

	AddHit(fTrackID, ivol, copyNo_slice, copyNo, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	       TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	       time, length, fELoss);
#else
*/
    //if(copyNo<=2) cout <<"Exit_WALL " <<length <<" " <<gMC->CurrentVolPath() <<" " <<copyNo_slice <<" " <<copyNo <<" " <<tPos.Z() <<endl;


    /*
      AddHit(fTrackID, ivol, copyNo_slice, copyNo, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	     TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	     time, length, fELoss);
//#endif
  }//if ( gMC->IsTrackExiting()) {
    */
    /*
      Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
//       Int_t nZdcPoints = (points & (1<<30)) >> 30;
//       nZdcPoints ++;
//       if (nZdcPoints > 1) nZdcPoints = 1;
//      points = ( points & ( ~ (1<<30) ) ) | (nZdcPoints << 30);
      points = ( points & ( ~ (1<<30) ) ) | (1 << 30);
      gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);

      ((CbmStack*)gMC->GetStack())->AddPoint(kZDC);
    */
      //}
   
  
    return kTRUE;
  
    //  }
    //#undef EDEBUG
}

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void BmnZdc::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}


// -----   Public method Register   -------------------------------------------
void BmnZdc::Register() {
  FairRootManager::Instance()->Register("ZdcPoint","Zdc", fZdcCollection, kTRUE);
}


// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnZdc::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fZdcCollection;

  return NULL;
}


// -----   Public method Print   ----------------------------------------------
void BmnZdc::Print() const {
    Int_t nHits = fZdcCollection->GetEntriesFast();
    cout << "-I- BmnZdc: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fZdcCollection)[i]->Print();
}




// -----   Public method Reset   ----------------------------------------------
void BmnZdc::Reset() {
   fZdcCollection->Delete();
 
  fPosIndex = 0;
}


// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void BmnZdc::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- BmnZdc: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  BmnZdcPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (BmnZdcPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) BmnZdcPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- BmnZdc: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

 // -----   Public method ConstructGeometry   ----------------------------------

void BmnZdc::ConstructGeometry() {

 	TString fileName = GetGeometryFileName();
	if(fileName.EndsWith(".root")) 
	{
		//FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Constructing ZDC geometry from ROOT file %s", fileName.Data());
		LOG(info) << "Constructing ZDC geometry from ROOT file" <<fileName.Data();
		ConstructRootGeometry();
	}

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  BmnZdcGeo*      zdcGeo = new BmnZdcGeo();
  zdcGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(zdcGeo);

  Bool_t rc = geoFace->readSet(zdcGeo);
  if (rc) zdcGeo->create(geoLoad->getGeoBuilder());
  TList* volList = zdcGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  BmnZdcGeoPar* par=(BmnZdcGeoPar*)(rtdb->getContainer("BmnZdcGeoPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
    aVol = dynamic_cast<FairGeoVolume*> ( node );


    if ( node->isSensitive()  ) {
      fSensNodes->AddLast( aVol );
    }else{
      fPassNodes->AddLast( aVol );
    }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  ProcessNodes ( volList );
}
  
//Check if Sensitive-----------------------------------------------------------
Bool_t BmnZdc::CheckIfSensitive(std::string name) {
    TString tsname = name;
    //if (tsname.Contains("zdc01s") || tsname.Contains("ScH")) {
    if (tsname.Contains("zdc01s") || tsname.Contains("zdc01s_after") || tsname.Contains("ScH")) {
        return kTRUE;
    }
    return kFALSE;
}
 

// -----   Private method AddHit   --------------------------------------------
//BmnZdcPoint* BmnZdc::AddHit(Int_t trackID, Int_t module_groupID, Int_t copyNo, Int_t copyNoMother,
BmnZdcPoint* BmnZdc::AddHit(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother,
			    TVector3 pos, TVector3 mom, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fZdcCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) BmnZdcPoint(trackID, detID, copyNo, copyNoMother,pos, mom, 
				      time, length, eLoss);
}
