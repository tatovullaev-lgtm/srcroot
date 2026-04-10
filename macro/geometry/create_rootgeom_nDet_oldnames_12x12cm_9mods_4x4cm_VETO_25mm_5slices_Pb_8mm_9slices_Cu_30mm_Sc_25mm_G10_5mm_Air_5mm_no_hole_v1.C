//---------------------------
//M.Golubeva (INR RAS), December 2016  
//---------------------------

//create_rootgeom_zdc_oldnames_NICA_36mods_no_hole_v1(Double_t ZDC_Xpos=0,Double_t ZDC_Ypos=0,Double_t ZDC_Zpos=900,Double_t rotY=0)
//ZDC_Xpos - X coordinate of front surface center [cm]
//ZDC_Ypos - Y coordinate of front surface center [cm]
//ZDC_Zpos - Z coordinate of front surface center [cm]
//rotY - rotation around Y axis [deg.]


#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMedium.h"
#include "TFile.h"

#include "../profiling/bmnloadlibs.C"


//Detector's construct parameters ===================

//Global dimensions for ZDC in cm

//12x12cm 9mods
Double_t Zdc_X_size = 6; //12./2.
Double_t Zdc_Y_size = 6; //12./2.
Double_t Zdc_Z_size = 41.25; //82.5./2. with veto

Double_t Zdc_X_size_NICA = 6; //12./2.
Double_t Zdc_Y_size_NICA = 6; //12./2.


//Modules NICA
Double_t Zdc_NColumns_NICA = 3; 
Double_t Zdc_NRows_NICA = 3; 

Double_t Zdc_module_X_size_NICA = 2; //4./2.
Double_t Zdc_module_Y_size_NICA = 2; //4./2.
Double_t Zdc_module_Z_size_NICA = 41.25; //82.5/2. 

//media ============================================
TGeoMedium *pMedVacuum = 0;
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedIron = 0;
TGeoMedium *pMedLead = 0;
TGeoMedium *pMedTungsten = 0;
TGeoMedium *pMedScint = 0;
TGeoMedium *pMedTyvec = 0;
TGeoMedium *pMedSlot = 0;
TGeoMedium *pMedG10 = 0;
TGeoMedium *pMedCopper = 0;

class FairGeoMedia;
class FairGeoBuilder;

void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild) {
//vacuum medium
  FairGeoMedium* mVacuum = geoMedia->getMedium("vacuum");
  if ( ! mVacuum ) Fatal("Main", "FairMedium vacuum not found");
  geoBuild->createMedium(mVacuum);
  pMedVacuum = gGeoManager->GetMedium("vacuum");
  if ( ! pMedVacuum ) Fatal("Main", "Medium vacuum not found");
  
//air medium
  FairGeoMedium* mAir = geoMedia->getMedium("air");
  if ( ! mAir ) Fatal("Main", "FairMedium air not found");
  geoBuild->createMedium(mAir);
  pMedAir = gGeoManager->GetMedium("air");
  if ( ! pMedAir ) Fatal("Main", "Medium air not found");

//iron medium
  FairGeoMedium* mIron = geoMedia->getMedium("iron");
  if ( ! mIron ) Fatal("Main", "FairMedium iron not found");
  geoBuild->createMedium(mIron);
  pMedIron = gGeoManager->GetMedium("iron");
  if ( ! pMedIron ) Fatal("Main", "Medium iron not found");
  
//tyvec medium
  FairGeoMedium* mTyvec = geoMedia->getMedium("tyvec_ZDC");
  if ( ! mTyvec ) Fatal("Main", "FairMedium tyvec not found");
  geoBuild->createMedium(mTyvec);
  pMedTyvec = gGeoManager->GetMedium("tyvec_ZDC");
  if ( ! pMedTyvec ) Fatal("Main", "Medium tyvec not found");

//lead medium
  FairGeoMedium* mLead = geoMedia->getMedium("lead");
  if ( ! mLead ) Fatal("Main", "FairMedium lead not found");
  geoBuild->createMedium(mLead);
  pMedLead = gGeoManager->GetMedium("lead");
  if ( ! pMedLead ) Fatal("Main", "Medium lead not found");
    
//tungten medium
  FairGeoMedium* mTungsten = geoMedia->getMedium("tungsten");
  if ( ! mTungsten ) Fatal("Main", "FairMedium tungsten not found");
  geoBuild->createMedium(mTungsten);
  pMedTungsten = gGeoManager->GetMedium("tungsten");
  if ( ! pMedTungsten ) Fatal("Main", "Medium tungsten not found");
    
// medium polystyrene (Scint.)
  FairGeoMedium* mPolystyrene = geoMedia->getMedium("polystyrene");
  if ( ! mPolystyrene ) Fatal("Main", "FairMedium polystyrene not found");
  geoBuild->createMedium(mPolystyrene);
  pMedScint = gGeoManager->GetMedium("polystyrene");
  if ( ! pMedScint ) Fatal("Main", "Medium polystyrene not found");

// medium plastic (slot)
  FairGeoMedium* mPlastic = geoMedia->getMedium("plasticWLS_ZDC");
  if ( ! mPlastic ) Fatal("Main", "FairMedium plastic not found");
  geoBuild->createMedium(mPlastic);
  pMedSlot = gGeoManager->GetMedium("plasticWLS_ZDC");
  if ( ! pMedSlot ) Fatal("Main", "Medium plastic not found");
    
// medium G10 (PCB)
  FairGeoMedium* mG10 = geoMedia->getMedium("G10");
  if ( ! mG10 ) Fatal("Main", "FairMedium G10 not found");
  geoBuild->createMedium(mG10);
  pMedG10 = gGeoManager->GetMedium("G10");
  if ( ! pMedG10 ) Fatal("Main", "Medium G10 not found");
    
//copper medium
  FairGeoMedium* mCopper = geoMedia->getMedium("copper");
  if ( ! mCopper ) Fatal("Main", "FairMedium copper not found");
  geoBuild->createMedium(mCopper);
  pMedCopper = gGeoManager->GetMedium("copper");
  if ( ! pMedCopper ) Fatal("Main", "Medium copper not found");
    
}//DefineRequiredMedia

void FillModule_NICA_simple(TGeoVolume* zdcModuleV_NICA);

void create_rootgeom_nDet_oldnames_12x12cm_9mods_4x4cm_VETO_25mm_5slices_Pb_8mm_9slices_Cu_30mm_Sc_25mm_G10_5mm_Air_5mm_no_hole_v1(Double_t ZDC_Xpos=0,Double_t ZDC_Ypos=0,Double_t ZDC_Zpos=900,Double_t rotY=0) {

  bmnloadlibs(); // load libraries
  
// ----  set working directory  -------------------------------------------
  TString gPath = gSystem->Getenv("VMCWORKDIR");

// -------   Geometry file name (output)   --------------------------------
  const TString geoDetectorName = "nDet_12x12cm_9mods_4x4cm_VETO_25mm_5slices_Pb_8mm_9slices_Cu_30mm_Sc_25mm_G10_5mm_Air_5mm_no_hole";
  const TString geoDetectorVersion = "v1";
  Double_t ZDC_center_Zpos = ZDC_Zpos + Zdc_Z_size*TMath::Cos(rotY*TMath::DegToRad());
  Double_t ZDC_center_Xpos = ZDC_Xpos + Zdc_Z_size*TMath::Sin(rotY*TMath::DegToRad());
  cout <<"ZDC_center " <<ZDC_center_Xpos <<" " <<ZDC_center_Zpos <<endl;
  //return;

  TString sXshift_1 = "";
  TString sXshift_2 = "";

  TString sYshift_1 = "";
  TString sYshift_2 = "";

  TString sZshift_1 = "";
  TString sZshift_2 = "";

  TString sRotY_1 = "";
  TString sRotY_2 = "";

  Int_t Xshift_1 = Int_t(ZDC_Xpos); //cm
  //Int_t Xshift_2 = Int_t(0.5+100*(ZDC_Xpos-Int_t(ZDC_Xpos))); //cm
  Int_t Xshift_2 = Int_t(0.5+100*(TMath::Abs(ZDC_Xpos)-Int_t(TMath::Abs(ZDC_Xpos)))); //cm

  Int_t Yshift_1 = Int_t(ZDC_Ypos); //cm
  Int_t Yshift_2 = Int_t(100*(ZDC_Ypos-Int_t(ZDC_Ypos))); //cm

  Int_t Zshift_1 = Int_t(ZDC_Zpos); //cm
  Int_t Zshift_2 = Int_t(10*(ZDC_Zpos-Int_t(ZDC_Zpos))); //cm
  //Int_t Zshift_2 = Int_t(10*(ZDC_Zpos-Int_t(ZDC_Zpos)))+1; //cm
  //Int_t Zshift_2 = Int_t(100*(ZDC_Zpos-Int_t(ZDC_Zpos)))+1; //cm

  Int_t rotY_1 = Int_t(rotY); //2
  Int_t rotY_2 = Int_t(100*(rotY-Int_t(rotY)));//9

  sXshift_1 += Xshift_1;
  sXshift_2 += Xshift_2;
  sYshift_1 += Yshift_1;
  sYshift_2 += Yshift_2;
  sZshift_1 += Zshift_1;
  sZshift_2 += Zshift_2;

  sRotY_1 +=rotY_1;
  sRotY_2 +=rotY_2;
 
  TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zpos_" + sZshift_1 + "." + sZshift_2 + "cm" + "_" + "Xshift_" + sXshift_1 + "." + sXshift_2 + "cm" + "_" + "Yshift_" + sYshift_1 + "." + sYshift_2 + "cm" + "_" + "rotationY_"  + sRotY_1  +  "." + sRotY_2 + "deg_" + geoDetectorVersion + ".root";


// ----  global geometry parameters  ---------------------------------------
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();

// -------   Load media from media file   ----------------------------------
  TString medFile = gPath + "/geometry/media.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
  geoMedia->list();
  geoMedia->print();
  DefineRequiredMedia(geoMedia, geoBuild);

// --------------   Create geometry and global top volume  --------------------
  gGeoManager = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoManager->SetName(geoDetectorName + "_geom");
  TGeoVolume* top = new TGeoVolumeAssembly(BmnNdetGeoPar::fTopVolumeName);
  top->SetMedium(pMedAir);
  gGeoManager->SetTopVolume(top);
  //gGeoMan->SetTopVisible(1);

// Define ZDC Geometry
  TGeoVolume *NDET_common = new TGeoVolumeAssembly(BmnNdetGeoPar::fCommonVolumeName);
  NDET_common->SetMedium(pMedAir);
  //NDET_common->SetMedium(pMedVacuum);
  
  TGeoBBox *ZDCS = new TGeoBBox(BmnNdetGeoPar::fDetectorVolumeName, Zdc_X_size, Zdc_Y_size, Zdc_Z_size);
  TGeoVolume *ZDCV = new TGeoVolume(BmnNdetGeoPar::fDetectorVolumeName, ZDCS);
  ZDCV->SetMedium(pMedAir);
  
  TGeoBBox *zdcModuleS_NICA = new TGeoBBox(BmnNdetGeoPar::fModuleVolumeName, Zdc_module_X_size_NICA, Zdc_module_Y_size_NICA, Zdc_module_Z_size_NICA);
  TGeoVolume *zdcModuleV_NICA = new TGeoVolume(BmnNdetGeoPar::fModuleVolumeName, zdcModuleS_NICA);
  zdcModuleV_NICA->SetMedium(pMedIron);

//Fill ZDC with NICA modules
  Float_t xCur = Zdc_X_size_NICA + Zdc_module_X_size_NICA;//28.5+1.5=30 cm
  Float_t yCur = Zdc_Y_size_NICA + Zdc_module_Y_size_NICA;//28.5+1.5=30 cm

  Int_t iMod=-1, modNb=-1;
  for(Int_t iy=0; iy<Zdc_NRows_NICA; iy++) {//19
    yCur=yCur - 2.*Zdc_module_Y_size_NICA;
    for(Int_t ix=0; ix<Zdc_NColumns_NICA; ix++) {//19
      xCur=xCur - 2.*Zdc_module_X_size_NICA;
      
      TGeoTranslation *zdcModuleV_position_NICA = new TGeoTranslation(xCur,yCur,0);
      iMod=iy*Zdc_NColumns_NICA + ix;
      
      modNb=iMod+1;  
      ZDCV->AddNode(zdcModuleV_NICA,modNb,zdcModuleV_position_NICA);
      cout <<"NICA modNb,xxxx,yyyy " <<modNb <<" " <<xCur <<" " <<yCur <<endl;	
      if(ix==Zdc_NColumns_NICA-1) xCur=Zdc_X_size_NICA + Zdc_module_X_size_NICA;	
    }//for(Int_t ix==0; ix<7; ix++)
  }//for(Int_t iy==0; iy<7; iy++)
  
  //return;

  FillModule_NICA_simple(zdcModuleV_NICA);

//Adding the zdc mother volume to the global TOP Volume
  TGeoRotation *rot = new TGeoRotation("rot");
  rot->RotateY(rotY); //in deg.
  TGeoCombiTrans *zdcZDC = new TGeoCombiTrans("zdcZDC",ZDC_center_Xpos, ZDC_Ypos, ZDC_center_Zpos,rot);
  NDET_common->AddNode(ZDCV, 1, zdcZDC);//NDET 
  
  top->AddNode(NDET_common, 0);
  top->SetVisContainers(kTRUE);
  
// ---------------   Finish   ----------------------------------------------
  gGeoManager->CloseGeometry();
  gGeoManager->CheckOverlaps(0.001);
  gGeoManager->PrintOverlaps();
  
  gGeoManager->Test();
  
  TFile* geoFile = new TFile(geoFileName, "RECREATE");
  top->Write();
  geoFile->Close();
  
  //TString gdmlname = gPath + "/geometry/tpc_v7.gdml";
  //gGeoManager->Export(gdmlname);

  //top->Draw("ogl");
  //TGLViewer *v = (TGLViewer*)gPad->GetViewer3D();
  //v->SetStyle(TGLRnrCtx::kOutline);
  //top->Draw("");
  
}//create_rootgeom_zdc_v1

void FillModule_NICA_simple(TGeoVolume *mother_volume_NICA) {

//Detector's construct parameters ===================

//Global dimensions for ZDC in cm

//12x12cm 9mods 
Double_t Zdc_X_size = 6; //12/2.
Double_t Zdc_Y_size = 6; //12/2.
Double_t Zdc_Z_size = 41.25; //82.5/2. 


//Modules
Double_t Zdc_module_X_size = 2; //4./2.
Double_t Zdc_module_Y_size = 2; //4./2.
Double_t Zdc_module_Z_size = 41.25; //82.5/2. 
/*
//Fe slices
Double_t Zdc_Fe_X_size = 7.4; //(15.-2.*0.1)/2.
Double_t Zdc_Fe_Y_size = 7.35; //(15.-2.*0.15)/2.
Double_t Zdc_Fe_Z_size = 1.; //2./2.
*/
//Pb slices //Fe slices, do not change names //5slices Pb 8mm
Double_t Zdc_NPb_in_module_5mm = 5;   
Double_t Zdc_Pb_X_size_5mm = 2; //4/2
Double_t Zdc_Pb_Y_size_5mm = 2; //4/2.
Double_t Zdc_Pb_Z_size_5mm = 0.4; //0.8/2.

 Double_t Zdc_NPb_in_module_10mm = 9;   //9 slices Cu 3cm
Double_t Zdc_Pb_X_size_10mm = 2; //4/2.
Double_t Zdc_Pb_Y_size_10mm = 2; //4/2.
Double_t Zdc_Pb_Z_size_10mm = 1.5; //3./2.
/*
Double_t Zdc_NPb_in_module_20mm = 10;   
Double_t Zdc_Pb_X_size_20mm = 1.4; //(3.-2.*0.1)/2.
Double_t Zdc_Pb_Y_size_20mm = 1.35; //(3.-2.*0.15)/2.
Double_t Zdc_Pb_Z_size_20mm = 1; //2/2.
*/
//Scin. slices
/*
Double_t Zdc_Sc_X_size = 7.38; //(15.-2.*0.1-2.*0.02)/2.
Double_t Zdc_Sc_Y_size = 7.33; //(15.-2.*0.15-2.*0.02)/2.
Double_t Zdc_Sc_Z_size = 0.2; //0.4/2.
*/
//Double_t Zdc_NSc_in_module = 30;
Double_t Zdc_Sc_X_size = 2; //as Pb
Double_t Zdc_Sc_Y_size = 2; //as Pb
 Double_t Zdc_Sc_Z_size = 1.25; //2.5/2.

//Double_t Zdc_NVeto_in_module = 30;
Double_t Zdc_Veto_X_size = 2; //as Pb
Double_t Zdc_Veto_Y_size = 2; //as Pb
 Double_t Zdc_Veto_Z_size = 1.25; //2.5/2.

//Plastic slices (do not change name)
//Double_t Zdc_NAir_in_module = 30;
Double_t Zdc_Air_X_size = 2; //as Pb
Double_t Zdc_Air_Y_size = 2; //as Pb
Double_t Zdc_Air_Z_size = 0.25; //0.5/2.

//Double_t Zdc_NG10_in_module = 30;
Double_t Zdc_G10_X_size = 2; //as Pb
Double_t Zdc_G10_Y_size = 2; //as Pb
Double_t Zdc_G10_Z_size = 0.25; //0.5/2.
/*
//Tyvec slices
Double_t Zdc_NTyvec_in_module = 42;//7sect
Double_t Zdc_Tyvec_X_size = 7.4; //(15.-2.*0.1)/2.
Double_t Zdc_Tyvec_Y_size = 7.35; //(15.-2.*0.15)/2.
Double_t Zdc_Tyvec_Z_size = 0.22; //0.44/2.
*/
/*
//Slot slices (divided by 2)
Double_t Zdc_Y_slot_position = 0.75;

Double_t Zdc_VRFM_X_size = 0.1; //Fe
Double_t Zdc_VRFM_Y_size = 6.1;
Double_t Zdc_VRFM_Z_size = 1.;

Double_t Zdc_VRPM_X_size = 0.1; //Pb
Double_t Zdc_VRPM_Y_size = 6.1;
Double_t Zdc_VRPM_Z_size = 0.8;

Double_t Zdc_VRYM_X_size = 0.01; //Tyvec_Y
Double_t Zdc_VRYM_Y_size = 6.1;
Double_t Zdc_VRYM_Z_size = 0.22;

Double_t Zdc_VRAM_X_size = 0.09; //Tyvec_A
Double_t Zdc_VRAM_Y_size = 6.1;
Double_t Zdc_VRAM_Z_size = 0.01;

Double_t Zdc_VRSM_X_size = 0.09; //Scint
Double_t Zdc_VRSM_Y_size = 6.1;
Double_t Zdc_VRSM_Z_size = 0.2;
*/

//TString Fe_name_NICA = "Fe_NICA";
//TString slot_Fe_name = "Fe_slot";
 //TString BmnNdetGeoPar::Pb_name_NICA_5mm = "ndet01a_NICA_5mm";
 //TString BmnNdetGeoPar::Pb_name_NICA_10mm = "ndet01a_NICA_10mm";
 //TString Pb_name_NICA_20mm = "ndet01a_NICA_20mm";
 //TString BmnNdetGeoPar::Air_name_NICA = "ndet01air_NICA";
 //TString BmnNdetGeoPar::G10_name_NICA = "ndet01g10_NICA";
 //TString slot_Pb_name = "Pb_slot";
 //TString BmnNdetGeoPar::SensitiveVolume_name_NICA = "ndet01s_NICA";
 //TString BmnNdetGeoPar::SensitiveVolume_name_VETO = "ndet01s_VETO";
 //TString slot_Sc_name = "Sc_slot";
 //TString Tyvec_name_NICA = "Tyvec_NICA";
 //TString slot_Tyvec_Y_name = "Tyvec_slot_Y";
 //TString slot_Tyvec_A_name = "Tyvec_slot_A";
 
//Shapes
/*
 TGeoShape *FeS_NICA = new TGeoBBox(Fe_name_NICA, Zdc_Fe_X_size, Zdc_Fe_Y_size, Zdc_Fe_Z_size);
 TGeoVolume *FeV_NICA = new TGeoVolume(Fe_name_NICA, FeS_NICA);
 TGeoMedium *Fe_medium_NICA = pMedIron; //set medium
 if(Fe_medium_NICA) FeV_NICA->SetMedium(Fe_medium_NICA);
 else Fatal("Main", "Invalid medium for Fe_NICA!");
*/ 

 TGeoShape *PbS_NICA_5mm = new TGeoBBox(BmnNdetGeoPar::Pb_name_NICA_5mm, Zdc_Pb_X_size_5mm, Zdc_Pb_Y_size_5mm, Zdc_Pb_Z_size_5mm);
 TGeoVolume *PbV_NICA_5mm = new TGeoVolume(BmnNdetGeoPar::Pb_name_NICA_5mm, PbS_NICA_5mm);
 PbV_NICA_5mm->SetLineColor(kBlue);
 PbV_NICA_5mm->SetFillColor(kBlue);
 //TGeoMedium *Pb_medium_NICA = pMedLead; //set medium
 //TGeoMedium *Pb_medium_NICA_5mm = pMedIron; //set medium
 //TGeoMedium *Pb_medium_NICA_5mm = pMedTungsten; //set medium
 TGeoMedium *Pb_medium_NICA_5mm = pMedLead; //set medium
 if(Pb_medium_NICA_5mm) PbV_NICA_5mm->SetMedium(Pb_medium_NICA_5mm);
 else Fatal("Main", "Invalid medium for Pb_NICA_5mm!");
 
TGeoShape *PbS_NICA_10mm = new TGeoBBox(BmnNdetGeoPar::Pb_name_NICA_10mm, Zdc_Pb_X_size_10mm, Zdc_Pb_Y_size_10mm, Zdc_Pb_Z_size_10mm);
 TGeoVolume *PbV_NICA_10mm = new TGeoVolume(BmnNdetGeoPar::Pb_name_NICA_10mm, PbS_NICA_10mm);
 PbV_NICA_10mm->SetLineColor(kMagenta);
 PbV_NICA_10mm->SetFillColor(kMagenta);
 //TGeoMedium *Pb_medium_NICA = pMedLead; //set medium
 //TGeoMedium *Pb_medium_NICA_10mm = pMedIron; //set medium
 //TGeoMedium *Pb_medium_NICA_10mm = pMedTungsten; //set medium
 TGeoMedium *Pb_medium_NICA_10mm = pMedCopper; //set medium
 if(Pb_medium_NICA_10mm) PbV_NICA_10mm->SetMedium(Pb_medium_NICA_10mm);
 else Fatal("Main", "Invalid medium for Pb_NICA_10mm!");
 /* 
TGeoShape *PbS_NICA_20mm = new TGeoBBox(Pb_name_NICA_20mm, Zdc_Pb_X_size_20mm, Zdc_Pb_Y_size_20mm, Zdc_Pb_Z_size_20mm);
 TGeoVolume *PbV_NICA_20mm = new TGeoVolume(Pb_name_NICA_20mm, PbS_NICA_20mm);
 PbV_NICA_20mm->SetLineColor(kAzure);
 //PbV_NICA_20mm->SetLineWidth(8);
 PbV_NICA_20mm->SetFillColor(kAzure);
 //TGeoMedium *Pb_medium_NICA = pMedLead; //set medium
 TGeoMedium *Pb_medium_NICA_20mm = pMedIron; //set medium
 if(Pb_medium_NICA_20mm) PbV_NICA_20mm->SetMedium(Pb_medium_NICA_20mm);
 else Fatal("Main", "Invalid medium for Pb_NICA_20mm!");
 */
 TGeoShape *zdcSensitiveVolumeS_NICA = new TGeoBBox(BmnNdetGeoPar::SensitiveVolume_name_NICA, Zdc_Sc_X_size, Zdc_Sc_Y_size, Zdc_Sc_Z_size);
 TGeoVolume *zdcSensitiveVolumeV_NICA = new TGeoVolume(BmnNdetGeoPar::SensitiveVolume_name_NICA, zdcSensitiveVolumeS_NICA);
 zdcSensitiveVolumeV_NICA->SetLineColor(kRed);
 zdcSensitiveVolumeV_NICA->SetFillColor(kRed);
 TGeoMedium *Sc_medium_NICA = pMedScint; //set medium
 if(Sc_medium_NICA) zdcSensitiveVolumeV_NICA->SetMedium(Sc_medium_NICA);
 else Fatal("Main", "Invalid medium for Sc_NICA!");

 TGeoShape *zdcSensitiveVolumeS_VETO = new TGeoBBox(BmnNdetGeoPar::SensitiveVolume_name_VETO, Zdc_Veto_X_size, Zdc_Veto_Y_size, Zdc_Veto_Z_size);
 TGeoVolume *zdcSensitiveVolumeV_VETO = new TGeoVolume(BmnNdetGeoPar::SensitiveVolume_name_VETO, zdcSensitiveVolumeS_VETO);
 zdcSensitiveVolumeV_VETO->SetLineColor(kGray);
 zdcSensitiveVolumeV_VETO->SetFillColor(kGray);
 TGeoMedium *Sc_medium_VETO = pMedScint; //set medium
 if(Sc_medium_VETO) zdcSensitiveVolumeV_VETO->SetMedium(Sc_medium_VETO);
 else Fatal("Main", "Invalid medium for VETO!");

 TGeoShape *AirS_NICA = new TGeoBBox(BmnNdetGeoPar::Air_name_NICA, Zdc_Air_X_size, Zdc_Air_Y_size, Zdc_Air_Z_size);
 TGeoVolume *AirV_NICA = new TGeoVolume(BmnNdetGeoPar::Air_name_NICA, AirS_NICA);
 AirV_NICA->SetLineColor(kGreen);
 AirV_NICA->SetFillColor(kGreen);
 TGeoMedium *Air_medium_NICA = pMedAir; //set medium
 //TGeoMedium *Air_medium_NICA = pMedSlot; //set medium
 if(Air_medium_NICA) AirV_NICA->SetMedium(Air_medium_NICA);
 else Fatal("Main", "Invalid medium for Air_NICA!");

 TGeoShape *G10S_NICA = new TGeoBBox(BmnNdetGeoPar::G10_name_NICA, Zdc_G10_X_size, Zdc_G10_Y_size, Zdc_G10_Z_size);
 TGeoVolume *G10V_NICA = new TGeoVolume(BmnNdetGeoPar::G10_name_NICA, G10S_NICA);
 G10V_NICA->SetLineColor(kOrange-7);
 G10V_NICA->SetFillColor(kOrange-7);
 //TGeoMedium *Air_medium_NICA = pMedAir; //set medium
 TGeoMedium *G10_medium_NICA = pMedG10; //set medium
 if(G10_medium_NICA) G10V_NICA->SetMedium(G10_medium_NICA);
 else Fatal("Main", "Invalid medium for G10_NICA!");

 /*
 TGeoShape *TyvecS_NICA = new TGeoBBox(Tyvec_name_NICA, Zdc_Tyvec_X_size, Zdc_Tyvec_Y_size, Zdc_Tyvec_Z_size);
 TGeoVolume *TyvecV_NICA = new TGeoVolume(Tyvec_name_NICA, TyvecS_NICA);
 TGeoMedium *Tyvec_medium_NICA = pMedTyvec; //set medium
 if(Tyvec_medium_NICA) TyvecV_NICA->SetMedium(Tyvec_medium_NICA);
 else Fatal("Main", "Invalid medium for Tyvec_NICA!");
 */
 /*    
//Slot's shapes
 TGeoShape *slot_FeS = new TGeoBBox(slot_Fe_name, Zdc_VRFM_X_size, Zdc_VRFM_Y_size, Zdc_VRFM_Z_size);
 TGeoVolume *slot_FeV = new TGeoVolume(slot_Fe_name, slot_FeS);
 TGeoMedium *slot_Fe_medium = pMedSlot; //set medium
 if(slot_Fe_medium) slot_FeV->SetMedium(slot_Fe_medium);
 else Fatal("Main", "Invalid medium for slot_Fe!");

 TGeoShape *slot_PbS = new TGeoBBox(slot_Pb_name, Zdc_VRPM_X_size, Zdc_VRPM_Y_size, Zdc_VRPM_Z_size);
 TGeoVolume *slot_PbV = new TGeoVolume(slot_Pb_name, slot_PbS);
 TGeoMedium *slot_Pb_medium = pMedSlot; //set medium
 if(slot_Pb_medium) slot_PbV->SetMedium(slot_Pb_medium);
 else Fatal("Main", "Invalid medium for slot_Pb!");
 
 TGeoShape *slot_ScS = new TGeoBBox(slot_Sc_name, Zdc_VRSM_X_size, Zdc_VRSM_Y_size, Zdc_VRSM_Z_size);
 TGeoVolume *slot_ScV = new TGeoVolume(slot_Sc_name, slot_ScS);
 TGeoMedium *slot_Sc_medium = pMedSlot; //set medium
 if(slot_Sc_medium) slot_ScV->SetMedium(slot_Sc_medium);
 else Fatal("Main", "Invalid medium for slot_Sc!");

 TGeoShape *slot_Tyvec_YS = new TGeoBBox(slot_Tyvec_Y_name, Zdc_VRYM_X_size, Zdc_VRYM_Y_size, Zdc_VRYM_Z_size);
 TGeoVolume *slot_Tyvec_YV = new TGeoVolume(slot_Tyvec_Y_name, slot_Tyvec_YS);
 TGeoMedium *slot_Tyvec_Y_medium = pMedSlot; //set medium
 if(slot_Tyvec_Y_medium) slot_Tyvec_YV->SetMedium(slot_Tyvec_Y_medium);
 else Fatal("Main", "Invalid medium for slot_Tyvec_Y!");

 TGeoShape *slot_Tyvec_AS = new TGeoBBox(slot_Tyvec_A_name, Zdc_VRAM_X_size, Zdc_VRAM_Y_size, Zdc_VRAM_Z_size);
 TGeoVolume *slot_Tyvec_AV = new TGeoVolume(slot_Tyvec_A_name, slot_Tyvec_AS);
 TGeoMedium *slot_Tyvec_A_medium = pMedSlot; //set medium
 if(slot_Tyvec_A_medium) slot_Tyvec_AV->SetMedium(slot_Tyvec_A_medium);
 else Fatal("Main", "Invalid medium for slot_Tyvec_A!");
 */
 /*
//insert Fe
 mother_volume_NICA->AddNode(FeV_NICA,1,new TGeoTranslation(0, 0, -(Zdc_module_Z_size-Zdc_Fe_Z_size)));
 FeV_NICA->AddNode(slot_FeV,1,new TGeoTranslation(Zdc_Fe_X_size-Zdc_VRFM_X_size, Zdc_Y_slot_position, 0.));
 mother_volume_NICA->AddNode(FeV_NICA,2,new TGeoTranslation(0, 0, (Zdc_module_Z_size-Zdc_Fe_Z_size)));
 */
 /*
//insert Tyvec
 mother_volume_NICA->AddNode(TyvecV_NICA, 1, new TGeoTranslation(0,0, -(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-Zdc_Tyvec_Z_size)));
 cout <<"NICA tyvec 1 " <<-(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-Zdc_Tyvec_Z_size) <<endl;
 TyvecV_NICA->AddNode(slot_Tyvec_YV,1,new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRYM_X_size), Zdc_Y_slot_position, 0));
 TyvecV_NICA->AddNode(slot_Tyvec_AV, 1, new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRAM_X_size-2.*Zdc_VRYM_X_size), Zdc_Y_slot_position, -(Zdc_Tyvec_Z_size-Zdc_VRAM_Z_size)));
 TyvecV_NICA->AddNode(slot_Tyvec_AV, 2, new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRAM_X_size-2.*Zdc_VRYM_X_size), Zdc_Y_slot_position,(Zdc_Tyvec_Z_size-Zdc_VRAM_Z_size)));
 
//insert Scint into Tyvec
 TyvecV_NICA->AddNode(zdcSensitiveVolumeV_NICA, 1, new TGeoTranslation(0., 0., 0.));
 zdcSensitiveVolumeV_NICA->AddNode(slot_ScV, 1, new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRAM_X_size-2.*Zdc_VRYM_X_size), Zdc_Y_slot_position, 0));
 */ 
 /*
//insert Scint 
 mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA, 1, new TGeoTranslation(0,0, -(Zdc_module_Z_size-2.*Zdc_Pb_Z_size-Zdc_Sc_Z_size)));
 cout <<"NICA Sc 1 " <<-(Zdc_module_Z_size-2.*Zdc_Pb_Z_size-Zdc_Sc_Z_size) <<endl;
 */

 Double_t itDelta=0;
 Double_t itInit_Pb, itCur_Pb=0; 
 Double_t itInit_Sc, itCur_Sc=0;
 Double_t itInit_G10, itCur_G10=0;
 Double_t itInit_Air, itCur_Air=0;
 Double_t itInit_Veto;

//insert Veto slice     
 itInit_Veto=-(Zdc_module_Z_size-Zdc_Veto_Z_size); 
 mother_volume_NICA->AddNode(zdcSensitiveVolumeV_VETO,Zdc_NPb_in_module_5mm+Zdc_NPb_in_module_10mm+1,new TGeoTranslation(0,0,itInit_Veto));
 cout <<"NICA Veto " <<itInit_Veto <<endl;

//insert Pb_5mm and Sc and G10 and Air slices     
 itDelta=2.*(Zdc_Pb_Z_size_5mm+Zdc_Sc_Z_size+Zdc_G10_Z_size+Zdc_Air_Z_size);

 itInit_Pb=-(Zdc_module_Z_size-2.*Zdc_Veto_Z_size-Zdc_Pb_Z_size_5mm); 
 itCur_Pb=0; 
 itInit_Sc = -(Zdc_module_Z_size-2.*Zdc_Veto_Z_size-2.*Zdc_Pb_Z_size_5mm-Zdc_Sc_Z_size); 
 itCur_Sc=0;
 itInit_G10 = -(Zdc_module_Z_size-2.*Zdc_Veto_Z_size-2.*Zdc_Pb_Z_size_5mm-2.*Zdc_Sc_Z_size-Zdc_G10_Z_size); 
 itCur_G10=0; 
 itInit_Air = -(Zdc_module_Z_size-2.*Zdc_Veto_Z_size-2.*Zdc_Pb_Z_size_5mm-2.*Zdc_Sc_Z_size-2.*Zdc_G10_Z_size-Zdc_Air_Z_size); 
 itCur_Air=0; 

 for(Int_t it=1; it<Zdc_NPb_in_module_5mm+1; it++) {
   itCur_Pb =itInit_Pb + (it-1)*itDelta;
   mother_volume_NICA->AddNode(PbV_NICA_5mm,it,new TGeoTranslation(0,0,itCur_Pb));
   cout <<"NICA it,itCur Pb_5mm " <<it <<" " <<itCur_Pb <<endl;

   itCur_Sc =itInit_Sc + (it-1)*itDelta;
   mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA,it,new TGeoTranslation(0,0,itCur_Sc));
   cout <<"NICA it,itCur Sc " <<it <<" " <<itCur_Sc <<endl;

   itCur_G10 =itInit_G10 + (it-1)*itDelta;
   mother_volume_NICA->AddNode(G10V_NICA,it,new TGeoTranslation(0,0,itCur_G10));
   cout <<"NICA it,itCur G10 " <<it <<" " <<itCur_G10 <<endl;

   itCur_Air =itInit_Air + (it-1)*itDelta;
   mother_volume_NICA->AddNode(AirV_NICA,it,new TGeoTranslation(0,0,itCur_Air));
   cout <<"NICA it,itCur Air " <<it <<" " <<itCur_Air <<endl;
 }//for(Int_t it=1; it<Zdc_NPb_in_module_5mm+1; it++)

//insert Pb_10mm and Sc and G10 and Air slices     
 itDelta=2.*(Zdc_Pb_Z_size_10mm+Zdc_Sc_Z_size+Zdc_G10_Z_size+Zdc_Air_Z_size);

 Double_t size_5mm;
 //size_5mm = Zdc_NPb_in_module_5mm*2.*(Zdc_Pb_Z_size_5mm+Zdc_Sc_Z_size+Zdc_Air_Z_size);
 size_5mm = 2.*Zdc_Veto_Z_size+Zdc_NPb_in_module_5mm*2.*(Zdc_Pb_Z_size_5mm+Zdc_Sc_Z_size+Zdc_G10_Z_size+Zdc_Air_Z_size);
 itInit_Pb=-(Zdc_module_Z_size-size_5mm-Zdc_Pb_Z_size_10mm); 
 itCur_Pb=0; 
 itInit_Sc = -(Zdc_module_Z_size-size_5mm-2.*Zdc_Pb_Z_size_10mm-Zdc_Sc_Z_size); 
 itCur_Sc=0;
 itInit_G10 = -(Zdc_module_Z_size-size_5mm-2.*Zdc_Pb_Z_size_10mm-2.*Zdc_Sc_Z_size-Zdc_G10_Z_size); 
 itCur_G10=0; 
 itInit_Air = -(Zdc_module_Z_size-size_5mm-2.*Zdc_Pb_Z_size_10mm-2.*Zdc_Sc_Z_size-2.*Zdc_G10_Z_size-Zdc_Air_Z_size); 
 itCur_Air=0; 

 for(Int_t it=1; it<Zdc_NPb_in_module_10mm+1; it++) {
   itCur_Pb =itInit_Pb + (it-1)*itDelta;
   mother_volume_NICA->AddNode(PbV_NICA_10mm,it,new TGeoTranslation(0,0,itCur_Pb));
   cout <<"NICA it,itCur Pb_10mm " <<it <<" " <<itCur_Pb <<endl;

   itCur_Sc =itInit_Sc + (it-1)*itDelta;
   mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA,it+Zdc_NPb_in_module_5mm,new TGeoTranslation(0,0,itCur_Sc));
   cout <<"NICA it,itCur Sc " <<it+Zdc_NPb_in_module_5mm <<" " <<itCur_Sc <<endl;

   itCur_G10 =itInit_G10 + (it-1)*itDelta;
   mother_volume_NICA->AddNode(G10V_NICA,it+Zdc_NPb_in_module_5mm,new TGeoTranslation(0,0,itCur_G10));
   cout <<"NICA it,itCur G10 " <<it+Zdc_NPb_in_module_5mm <<" " <<itCur_G10 <<endl;

   itCur_Air =itInit_Air + (it-1)*itDelta;
   mother_volume_NICA->AddNode(AirV_NICA,it+Zdc_NPb_in_module_5mm,new TGeoTranslation(0,0,itCur_Air));
   cout <<"NICA it,itCur Air " <<it+Zdc_NPb_in_module_5mm <<" " <<itCur_Air <<endl;
 }//for(Int_t it=1; it<Zdc_NPb_in_module_10mm+1; it++)
 /*
//insert Pb_20mm and Sc and Air slices     
 itDelta=2.*(Zdc_Pb_Z_size_20mm+Zdc_Sc_Z_size+Zdc_Air_Z_size);

 Double_t size_10mm;
 size_10mm = size_5mm+Zdc_NPb_in_module_10mm*2.*(Zdc_Pb_Z_size_10mm+Zdc_Sc_Z_size+Zdc_Air_Z_size);//14cm+19cm
 itInit_Pb=-(Zdc_module_Z_size-size_10mm-Zdc_Pb_Z_size_20mm); 
 itCur_Pb=0; 
 itInit_Sc = -(Zdc_module_Z_size-size_10mm-2.*Zdc_Pb_Z_size_20mm-Zdc_Sc_Z_size); 
 itCur_Sc=0;
 itInit_Air = -(Zdc_module_Z_size-size_10mm-2.*Zdc_Pb_Z_size_20mm-2.*Zdc_Sc_Z_size-Zdc_Air_Z_size); 
 itCur_Air=0; 

 for(Int_t it=1; it<Zdc_NPb_in_module_20mm+1; it++) {
   itCur_Pb =itInit_Pb + (it-1)*itDelta;
   mother_volume_NICA->AddNode(PbV_NICA_20mm,it,new TGeoTranslation(0,0,itCur_Pb));
   cout <<"NICA it,itCur Pb_20mm " <<it <<" " <<itCur_Pb <<endl;

   itCur_Sc =itInit_Sc + (it-1)*itDelta;
   mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA,it+Zdc_NPb_in_module_5mm+Zdc_NPb_in_module_10mm,new TGeoTranslation(0,0,itCur_Sc));
   cout <<"NICA it,itCur Sc " <<it+Zdc_NPb_in_module_5mm+Zdc_NPb_in_module_10mm <<" " <<itCur_Sc <<endl;

   itCur_Air =itInit_Air + (it-1)*itDelta;
   mother_volume_NICA->AddNode(AirV_NICA,it+Zdc_NPb_in_module_5mm+Zdc_NPb_in_module_10mm,new TGeoTranslation(0,0,itCur_Air));
   cout <<"NICA it,itCur Air " <<it+Zdc_NPb_in_module_5mm+Zdc_NPb_in_module_10mm <<" " <<itCur_Air <<endl;
 }//for(Int_t it=1; it<Zdc_NPb_in_module_20mm+1; it++)
 */
 return 1;

 cout <<"Zdc_module_Z_size " <<Zdc_module_Z_size <<endl;


}//FillModule_NICA_simple

