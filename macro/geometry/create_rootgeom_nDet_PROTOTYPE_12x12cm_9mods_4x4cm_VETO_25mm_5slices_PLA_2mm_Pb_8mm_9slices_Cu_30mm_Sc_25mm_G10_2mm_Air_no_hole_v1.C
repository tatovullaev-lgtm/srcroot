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
//Double_t Zdc_Z_size = 41.25; //82.5./2. with veto
Double_t Zdc_Z_size = 45.1; //90.2/2. with veto

Double_t Zdc_X_size_NICA = 6; //12./2.
Double_t Zdc_Y_size_NICA = 6; //12./2.


//Modules NICA
Double_t Zdc_NColumns_NICA = 3; 
Double_t Zdc_NRows_NICA = 3; 

Double_t Zdc_module_X_size_NICA = 2; //4./2.
Double_t Zdc_module_Y_size_NICA = 2; //4./2.
Double_t Zdc_module_Z_size_NICA = 45.1; //90.2/2. 

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
TGeoMedium *pMedPLA = 0;

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
    
// medium PLA (polylactic acid) 
  FairGeoMedium* mPLA = geoMedia->getMedium("PLA");
  if ( ! mPLA ) Fatal("Main", "FairMedium PLA not found");
  geoBuild->createMedium(mPLA);
  pMedPLA = gGeoManager->GetMedium("PLA");
  if ( ! pMedPLA ) Fatal("Main", "Medium PLA not found");
}//DefineRequiredMedia

void FillModule_NICA_simple(TGeoVolume* zdcModuleV_NICA);

void create_rootgeom_nDet_PROTOTYPE_12x12cm_9mods_4x4cm_VETO_25mm_5slices_PLA_2mm_Pb_8mm_9slices_Cu_30mm_Sc_25mm_G10_2mm_Air_no_hole_v1(Double_t ZDC_Xpos=0,Double_t ZDC_Ypos=0,Double_t ZDC_Zpos=900,Double_t rotY=0) {

  bmnloadlibs(); // load libraries
  
// ----  set working directory  -------------------------------------------
  TString gPath = gSystem->Getenv("VMCWORKDIR");

// -------   Geometry file name (output)   --------------------------------
  const TString geoDetectorName = "nDet_PROTOTYPE_12x12cm_9mods_4x4cm_VETO_25mm_5sl_PLA_2mm_Pb_8mm_9sl_Cu_30mm_Sc_25mm_G10_2mm_Air_no_hole";
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
  //Int_t Xshift_2 = Int_t(0.5+100*(TMath::Abs(ZDC_Xpos)-Int_t(TMath::Abs(ZDC_Xpos)))); //cm
  Int_t Xshift_2 = Int_t(0.5+1000*(TMath::Abs(ZDC_Xpos)-Int_t(TMath::Abs(ZDC_Xpos)))); //cm
  Int_t Xshift_3 = 0; //cm

  Int_t Yshift_1 = Int_t(ZDC_Ypos); //cm
  //Int_t Yshift_2 = Int_t(100*(ZDC_Ypos-Int_t(ZDC_Ypos))); //cm
  Int_t Yshift_2 = Int_t(1000*(ZDC_Ypos-Int_t(ZDC_Ypos))); //cm

  Int_t Zshift_1 = Int_t(ZDC_Zpos); //cm
  //Int_t Zshift_2 = Int_t(10*(ZDC_Zpos-Int_t(ZDC_Zpos))); //cm
  //Int_t Zshift_2 = Int_t(10*(ZDC_Zpos-Int_t(ZDC_Zpos)))+1; //cm
  //Int_t Zshift_2 = Int_t(100*(ZDC_Zpos-Int_t(ZDC_Zpos)))+1; //cm
  Int_t Zshift_2 = Int_t(1000*(ZDC_Zpos-Int_t(ZDC_Zpos))); //cm

  Int_t rotY_1 = Int_t(rotY); //2
  //Int_t rotY_2 = Int_t(100*(rotY-Int_t(rotY)));//9
  Int_t rotY_2 = TMath::Abs(Int_t(10*(rotY-Int_t(rotY))));

  sXshift_1 += Xshift_1;
  sXshift_2 += Xshift_3;
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
  ZDCV->SetMedium(pMedAir);//done from Air, so do not insert Air when fill module with materials

  TGeoBBox *zdcModuleS_NICA = new TGeoBBox(BmnNdetGeoPar::fModuleVolumeName, Zdc_module_X_size_NICA, Zdc_module_Y_size_NICA, Zdc_module_Z_size_NICA);
  TGeoVolume *zdcModuleV_NICA = new TGeoVolume(BmnNdetGeoPar::fModuleVolumeName, zdcModuleS_NICA);

  //zdcModuleV_NICA->SetMedium(pMedIron);
  zdcModuleV_NICA->SetMedium(pMedAir);

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

//12x12cm 9mods //not used
Double_t Zdc_X_size = 6; //12/2.
Double_t Zdc_Y_size = 6; //12/2.
Double_t Zdc_Z_size = 45.1; //90.2/2. 


//Modules
Double_t Zdc_module_X_size = 2; //4./2.
Double_t Zdc_module_Y_size = 2; //4./2.
Double_t Zdc_module_Z_size = 45.1; //90.2/2. 

//Pb slices //Fe slices, do not change names //5slices Pb 8mm
Double_t Zdc_NPb_in_module_5mm = 5;   
Double_t Zdc_Pb_X_size_5mm = 2; //4/2
Double_t Zdc_Pb_Y_size_5mm = 2; //4/2.
Double_t Zdc_Pb_Z_size_5mm = 0.4; //0.8/2.

//PLA slices //5slices PLA 2mm before PB slices
Double_t Zdc_PLA_X_size_5mm = 2; //4/2
Double_t Zdc_PLA_Y_size_5mm = 2; //4/2.
Double_t Zdc_PLA_Z_size_5mm = 0.1; //0.2/2.

//Pb slices //Fe slices, do not change names //5slices Pb 8mm
Double_t Zdc_NPb_in_module_10mm = 9;   //9 slices Cu 3cm
Double_t Zdc_Pb_X_size_10mm = 2; //4/2.
Double_t Zdc_Pb_Y_size_10mm = 2; //4/2.
Double_t Zdc_Pb_Z_size_10mm = 1.5; //3./2.

Double_t Zdc_Sc_X_size = 2; //as Pb
Double_t Zdc_Sc_Y_size = 2; //as Pb
Double_t Zdc_Sc_Z_size = 1.25; //2.5/2.

Double_t Zdc_Veto_X_size = 2; //as Pb
Double_t Zdc_Veto_Y_size = 2; //as Pb
Double_t Zdc_Veto_Z_size = 1.25; //2.5/2.
 
//PCB
Double_t Zdc_G10_X_size = 2; //as Pb
Double_t Zdc_G10_Y_size = 2; //as Pb
Double_t Zdc_G10_Z_size = 0.1; //0.2/2.

//AIR Not used in Prototype
//Plastic slices (do not change name)
//Double_t Zdc_NAir_in_module = 30;
Double_t Zdc_Air_X_size = 2; //as Pb
Double_t Zdc_Air_Y_size = 2; //as Pb
Double_t Zdc_Air_Z_size = 0.25; //0.5/2.
 
Int_t Zdc_NSlises_in_module;
 Zdc_NSlises_in_module = Zdc_NPb_in_module_5mm + Zdc_NPb_in_module_10mm;
Double_t Scint_edge_Z_coord[Zdc_NSlises_in_module];//5Pb,9Cu

//Shapes
 TGeoShape *PbS_NICA_5mm = new TGeoBBox(BmnNdetGeoPar::Pb_name_NICA_5mm, Zdc_Pb_X_size_5mm, Zdc_Pb_Y_size_5mm, Zdc_Pb_Z_size_5mm);
 TGeoVolume *PbV_NICA_5mm = new TGeoVolume(BmnNdetGeoPar::Pb_name_NICA_5mm, PbS_NICA_5mm);
 PbV_NICA_5mm->SetLineColor(kBlue);
 PbV_NICA_5mm->SetFillColor(kBlue);
 TGeoMedium *Pb_medium_NICA_5mm = pMedLead; //set medium
 if(Pb_medium_NICA_5mm) PbV_NICA_5mm->SetMedium(Pb_medium_NICA_5mm);
 else Fatal("Main", "Invalid medium for Pb_NICA_5mm!");
 
TGeoShape *PbS_NICA_10mm = new TGeoBBox(BmnNdetGeoPar::Pb_name_NICA_10mm, Zdc_Pb_X_size_10mm, Zdc_Pb_Y_size_10mm, Zdc_Pb_Z_size_10mm);
 TGeoVolume *PbV_NICA_10mm = new TGeoVolume(BmnNdetGeoPar::Pb_name_NICA_10mm, PbS_NICA_10mm);
 PbV_NICA_10mm->SetLineColor(kMagenta);
 PbV_NICA_10mm->SetFillColor(kMagenta);
 TGeoMedium *Pb_medium_NICA_10mm = pMedCopper; //set medium
 if(Pb_medium_NICA_10mm) PbV_NICA_10mm->SetMedium(Pb_medium_NICA_10mm);
 else Fatal("Main", "Invalid medium for Pb_NICA_10mm!");

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
 if(Air_medium_NICA) AirV_NICA->SetMedium(Air_medium_NICA);
 else Fatal("Main", "Invalid medium for Air_NICA!");

 TGeoShape *G10S_NICA = new TGeoBBox(BmnNdetGeoPar::G10_name_NICA, Zdc_G10_X_size, Zdc_G10_Y_size, Zdc_G10_Z_size);
 TGeoVolume *G10V_NICA = new TGeoVolume(BmnNdetGeoPar::G10_name_NICA, G10S_NICA);
 G10V_NICA->SetLineColor(kOrange-7);
 G10V_NICA->SetFillColor(kOrange-7);
 TGeoMedium *G10_medium_NICA = pMedG10; //set medium
 if(G10_medium_NICA) G10V_NICA->SetMedium(G10_medium_NICA);
 else Fatal("Main", "Invalid medium for G10_NICA!");

 TGeoShape *PLAS_NICA = new TGeoBBox(BmnNdetGeoPar::PLA_name_NICA, Zdc_PLA_X_size_5mm, Zdc_PLA_Y_size_5mm, Zdc_PLA_Z_size_5mm);
 TGeoVolume *PLAV_NICA = new TGeoVolume(BmnNdetGeoPar::PLA_name_NICA, PLAS_NICA);
 PLAV_NICA->SetLineColor(kViolet);
 PLAV_NICA->SetFillColor(kViolet);
 TGeoMedium *PLA_medium_NICA = pMedPLA; //set medium
 if(PLA_medium_NICA) PLAV_NICA->SetMedium(PLA_medium_NICA);
 else Fatal("Main", "Invalid medium for PLA_NICA!");

 Double_t itDelta=0;
 Double_t itInit_Pb, itCur_Pb=0; 
 Double_t itInit_Sc, itCur_Sc=0;
 Double_t itInit_G10, itCur_G10=0;
 Double_t itInit_Air, itCur_Air=0;
 Double_t itInit_Veto;
 Double_t itInit_PLA;

//insert Veto slice     
 itInit_Veto=-(Zdc_module_Z_size-Zdc_Veto_Z_size); 
 mother_volume_NICA->AddNode(zdcSensitiveVolumeV_VETO,1,new TGeoTranslation(0,0,itInit_Veto));
 cout <<"NICA Veto " <<Zdc_module_Z_size <<" " <<Zdc_Veto_Z_size <<" " <<itInit_Veto <<endl;

//insert G10 slice after Veto     
 itInit_G10=-(Zdc_module_Z_size-2.*Zdc_Veto_Z_size-Zdc_G10_Z_size); 
 //mother_volume_NICA->AddNode(zdcSensitiveVolumeV_VETO,1,new TGeoTranslation(0,0,itInit_G10));
 mother_volume_NICA->AddNode(G10V_NICA,1,new TGeoTranslation(0,0,itInit_G10));
 cout <<"NICA G10_1 " <<itInit_G10 <<endl;

 for(Int_t it=0; it<Zdc_NSlises_in_module; it++) {//measured
   if(it==0) Scint_edge_Z_coord[it] = -40.1;
   if(it==1) Scint_edge_Z_coord[it] = -34.7;
   if(it==2) Scint_edge_Z_coord[it] = -30.8;
   if(it==3) Scint_edge_Z_coord[it] = -26.6;
   if(it==4) Scint_edge_Z_coord[it] = -21.7;
   if(it==5) Scint_edge_Z_coord[it] = -14.6;
   if(it==6) Scint_edge_Z_coord[it] = -7.4;
   if(it==7) Scint_edge_Z_coord[it] = 0;
   if(it==8) Scint_edge_Z_coord[it] = 6.4;
   if(it==9) Scint_edge_Z_coord[it] = 13.3;
   if(it==10) Scint_edge_Z_coord[it] = 20.4;
   if(it==11) Scint_edge_Z_coord[it] = 27.7;
   if(it==12) Scint_edge_Z_coord[it] = 34.6;
   if(it==13) Scint_edge_Z_coord[it] = 42.2;
 }
 for(Int_t it=0; it<Zdc_NSlises_in_module; it++) {//0-13
   if(it<=4) {//Pb part
     itInit_PLA=Scint_edge_Z_coord[it]-2.*Zdc_Pb_Z_size_5mm-Zdc_PLA_Z_size_5mm; 
     mother_volume_NICA->AddNode(PLAV_NICA,it+2,new TGeoTranslation(0,0,itInit_PLA));
     cout <<"NICA it,PLA " <<Scint_edge_Z_coord[it] <<" " <<2.*Zdc_Pb_Z_size_5mm <<" " <<Zdc_PLA_Z_size_5mm <<" " <<itInit_PLA <<" " <<it+2 <<endl;

     itInit_Pb=Scint_edge_Z_coord[it]-Zdc_Pb_Z_size_5mm; 
     mother_volume_NICA->AddNode(PbV_NICA_5mm,it+2,new TGeoTranslation(0,0,itInit_Pb));
     cout <<"NICA it,Pb " <<itInit_Pb <<" " <<it+2 <<endl;

     itInit_Sc=Scint_edge_Z_coord[it]+Zdc_Sc_Z_size;    
     mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA,it+2,new TGeoTranslation(0,0,itInit_Sc));
     cout <<"NICA it,Sc " <<itInit_Sc <<" " <<it+2 <<endl;

     itInit_G10=Scint_edge_Z_coord[it]+2.*Zdc_Sc_Z_size+Zdc_G10_Z_size; 
     mother_volume_NICA->AddNode(G10V_NICA,it+2,new TGeoTranslation(0,0,itInit_G10));//G10 - 1 -> after Veto
     cout <<"NICA it,G10 " <<itInit_G10 <<" ===== " <<" " <<it+2 <<endl;
   }//Pb
   else {//Cu part//it=5 - 13
     itInit_Pb=Scint_edge_Z_coord[it]-Zdc_Pb_Z_size_10mm; 
     mother_volume_NICA->AddNode(PbV_NICA_10mm,it+2,new TGeoTranslation(0,0,itInit_Pb));
     cout <<"NICA it,Cu " <<itInit_Pb <<" " <<it+2 <<endl;

   itInit_Sc=Scint_edge_Z_coord[it]+Zdc_Sc_Z_size; 
   mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA,it+2,new TGeoTranslation(0,0,itInit_Sc));
   cout <<"NICA it,Sc " <<itInit_Sc <<" " <<it+2 <<endl;

   itInit_G10=Scint_edge_Z_coord[it]+2.*Zdc_Sc_Z_size+Zdc_G10_Z_size;    
   mother_volume_NICA->AddNode(G10V_NICA,it+2,new TGeoTranslation(0,0,itInit_G10));//G10 - 1 -> after Veto
   cout <<"NICA it,G10 " <<itInit_G10 <<" ==== " <<it+2 <<endl;

   }//Cu part
 }//for(Int_t it=0; it<Zdc_NSlises_in_module; it++)

 return 1;

 cout <<"Zdc_module_Z_size " <<Zdc_module_Z_size <<endl;


}//FillModule_NICA_simple

