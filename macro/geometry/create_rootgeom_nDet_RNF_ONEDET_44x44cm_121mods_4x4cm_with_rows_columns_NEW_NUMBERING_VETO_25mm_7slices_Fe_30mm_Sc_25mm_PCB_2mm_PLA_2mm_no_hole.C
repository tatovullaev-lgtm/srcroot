//---------------------------
//M.Golubeva (INR RAS), December 2016  
//---------------------------

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMedium.h"
#include "TFile.h"

#include "../profiling/bmnloadlibs.C"


//Detector's construct parameters ===================

//Global dimensions for ZDC in cm
//44x44cm 121mods
Double_t Zdc_X_size = 22; //44./2.
Double_t Zdc_Y_size = 22; //44./2.
//Double_t Zdc_Z_size = 41.25; //82.5./2. with veto
//Double_t Zdc_Z_size = 63; //126/2. with veto
//Double_t Zdc_Z_size = 47.3; //94.6/2. with veto
Double_t Zdc_Z_size = 22.9; //45.8/2. with veto

Double_t Zdc_X_size_NICA = 22; //44./2.
Double_t Zdc_Y_size_NICA = 22; //44./2.

//Modules NICA
Double_t Zdc_NColumns_NICA = 11; 
Double_t Zdc_NRows_NICA = 11; 

Double_t Zdc_module_X_size_NICA = 2; //4./2.
Double_t Zdc_module_Y_size_NICA = 2; //4./2.
//Double_t Zdc_module_Z_size_NICA = 41.25; //82.5/2. 
Double_t Zdc_module_Z_size_NICA = 22.9; //45.8/2. 

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

void create_rootgeom_nDet_RNF_ONEDET_44x44cm_121mods_4x4cm_with_rows_columns_NEW_NUMBERING_VETO_25mm_7slices_Fe_30mm_Sc_25mm_PCB_2mm_PLA_2mm_no_hole(Double_t nDet_dist=780, Double_t rotY=0, Double_t rotX=0) {

  bmnloadlibs(); // load libraries
  
// ----  set working directory  -------------------------------------------
  TString gPath = gSystem->Getenv("VMCWORKDIR");

// -------   Geometry file name (output)   --------------------------------
  const TString geoDetectorName = "nDet_RNF_ONEDET_44x44cm_121mods_4x4cm_with_rows_columns_NEW_NUMBERING_VETO_25mm_7slices_Fe_30mm_Sc_25mm_PCB_2mm_PLA_2mm_no_hole";

//Det 1
  TString sDist = "";
  TString sRotX_1 = "";
  TString sRotX_2 = "";

  TString sRotY_1 = "";
  TString sRotY_2 = "";

  Int_t rotX_1 = Int_t(rotX); //2
  //Int_t rotX_22 = Int_t(100*(rotX-Int_t(rotX)));//9
  Int_t rotX_22 = -Int_t(100*(rotX-Int_t(rotX)));//9
  //Int_t rotX_22 = -Int_t(100*(rotX-Int_t(rotX)))+1;//9

  Int_t rotY_1 = Int_t(rotY); //2
  //Int_t rotY_22 = Int_t(100*(rotY-Int_t(rotY)));//9
  Int_t rotY_22 = -Int_t(100*(rotY-Int_t(rotY)));//9

  sDist += nDet_dist;
  sRotX_1 +=rotX_1;
  sRotX_2 +=rotX_22;
  sRotY_1 +=rotY_1;
  sRotY_2 +=rotY_22;

  TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zdist_" + sDist + "cm_" + "rotY_"  + sRotY_1  +  "." + sRotY_2 + "deg" + "_" + "rotX_"  + sRotX_1 + "." + sRotX_2 + "deg" + ".root";

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
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  top->SetMedium(pMedAir);
  gGeoManager->SetTopVolume(top);
  //gGeoMan->SetTopVisible(1);

// Define ZDC Geometry
  TGeoVolume *NDET_common = new TGeoVolumeAssembly(BmnNdetGeoPar::fCommonVolumeName);
  NDET_common->SetMedium(pMedAir);
  
  TGeoBBox *ZDCS = new TGeoBBox(BmnNdetGeoPar::fDetectorVolumeName, Zdc_X_size, Zdc_Y_size, Zdc_Z_size);
  TGeoVolume *ZDCV = new TGeoVolume(BmnNdetGeoPar::fDetectorVolumeName, ZDCS);
  ZDCV->SetMedium(pMedAir);
  
  TGeoBBox *zdcModuleS_NICA[Int_t(Zdc_NRows_NICA*Zdc_NColumns_NICA)];
  TGeoVolume *zdcModuleV_NICA[Int_t(Zdc_NRows_NICA*Zdc_NColumns_NICA)];

  Int_t rowNb = -10, columnNb = -10; 
  Int_t iMod=-1, modNb=-1;

//Fill ZDC with NICA modules
  Float_t xCur = Zdc_X_size_NICA + Zdc_module_X_size_NICA;//28.5+1.5=30 cm
  Float_t yCur = Zdc_Y_size_NICA + Zdc_module_Y_size_NICA;//28.5+1.5=30 cm

  for(Int_t iy=0; iy<Zdc_NRows_NICA; iy++) {//19
    yCur=yCur - 2.*Zdc_module_Y_size_NICA;
    for(Int_t ix=0; ix<Zdc_NColumns_NICA; ix++) {//19
      xCur=xCur - 2.*Zdc_module_X_size_NICA;
      
      TGeoTranslation *zdcModuleV_position_NICA = new TGeoTranslation(xCur,yCur,0);
      iMod=iy*Zdc_NColumns_NICA + ix;
      modNb=iMod+1;  

      zdcModuleS_NICA[iMod] = new TGeoBBox(BmnNdetGeoPar::fModuleVolumeName, Zdc_module_X_size_NICA, Zdc_module_Y_size_NICA, Zdc_module_Z_size_NICA);
      zdcModuleV_NICA[iMod] = new TGeoVolume(BmnNdetGeoPar::fModuleVolumeName, zdcModuleS_NICA[iMod]);
      zdcModuleV_NICA[iMod]->SetMedium(pMedAir);

      rowNb = Zdc_NColumns_NICA-iy; 
      columnNb = Zdc_NRows_NICA-ix; 
      zdcModuleV_NICA[iMod]->SetName(Form("%s_%d_%d", (BmnNdetGeoPar::fModuleVolumeName).Data(), rowNb, columnNb)) ;
      cout <<"zdcModuleV_NICA " <<zdcModuleV_NICA[iMod]->GetName() <<" " <<rowNb <<" " <<columnNb <<" " <<modNb <<endl; 

      ZDCV->AddNode(zdcModuleV_NICA[iMod],modNb,zdcModuleV_position_NICA);
      cout <<"NICA modNb,xxxx,yyyy " <<modNb <<" " <<xCur <<" " <<yCur <<endl;	

      FillModule_NICA_simple(zdcModuleV_NICA[iMod]);

      if(ix==Zdc_NColumns_NICA-1) xCur=Zdc_X_size_NICA + Zdc_module_X_size_NICA;	
    }//for(Int_t ix==0; ix<7; ix++)
  }//for(Int_t iy==0; iy<7; iy++)
  
  //return;


//Adding the zdc mother volume to the global TOP Volume
  TGeoRotation *rot = new TGeoRotation("rot");
  rot->RotateX(rotX);
  rot->RotateY(rotY);

  Double_t ZDC_center_Xpos;
  Double_t ZDC_center_Ypos;
  Double_t ZDC_center_Zpos;

  Double_t master[3] = { 0, 0, 0 };
  Double_t local[3];
  local[0] = 0; local[1] = 0; local[2] = nDet_dist+Zdc_Z_size; 
  rot->LocalToMaster(local, master);

  ZDC_center_Xpos = master[0];
  ZDC_center_Ypos = master[1];
  ZDC_center_Zpos = master[2];
  cout <<"ZDC center " <<ZDC_center_Xpos <<" " <<ZDC_center_Ypos <<" " <<ZDC_center_Zpos <<endl;
  TGeoCombiTrans *zdcZDC_1 = new TGeoCombiTrans("zdcZDC_1",ZDC_center_Xpos, ZDC_center_Ypos, ZDC_center_Zpos,rot);
  NDET_common->AddNode(ZDCV, 1, zdcZDC_1);//NDET 
  //return;

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
  
  //cout <<"Det_1 " <<nDet_dist <<" " <<ZDC_Xpos <<" " <<ZDC_Ypos <<" " <<ZDC_Zpos <<" " <<rotY <<endl;

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

//Modules
Double_t Zdc_module_X_size = 2; //4./2.
Double_t Zdc_module_Y_size = 2; //4./2.
Double_t Zdc_module_Z_size = 22.9; //45.8/2. 

Double_t Zdc_NSlices_in_module = 8;   //Veto+7 slices

Double_t Zdc_Cu_X_size = 2; //4/2.
Double_t Zdc_Cu_Y_size = 2; //4/2.
Double_t Zdc_Cu_Z_size = 1.5; //3./2.

//Scin. slices
Double_t Zdc_Sc_X_size = 2; //as Pb
Double_t Zdc_Sc_Y_size = 2; //as Pb
Double_t Zdc_Sc_Z_size = 1.25; //2.5/2.

Double_t Zdc_Veto_X_size = 2; //as Pb
Double_t Zdc_Veto_Y_size = 2; //as Pb
Double_t Zdc_Veto_Z_size = 1.25; //2.5/2.

//PLA slices //before and after Sc+PCB
Double_t Zdc_PLA_X_size = 2; //4/2
Double_t Zdc_PLA_Y_size = 2; //4/2.
Double_t Zdc_PLA_Z_size = 0.1; //0.2/2.

//PCB
Double_t Zdc_G10_X_size = 2; //as Pb
Double_t Zdc_G10_Y_size = 2; //as Pb
Double_t Zdc_G10_Z_size = 0.1; //0.2/2.

//Shapes
 TGeoShape *CuS_NICA = new TGeoBBox(BmnNdetGeoPar::Cu_name_NICA, Zdc_Cu_X_size, Zdc_Cu_Y_size, Zdc_Cu_Z_size);
 TGeoVolume *CuV_NICA = new TGeoVolume(BmnNdetGeoPar::Cu_name_NICA, CuS_NICA);
 CuV_NICA->SetLineColor(kBlue);
 CuV_NICA->SetFillColor(kBlue);
 //TGeoMedium *Cu_medium_NICA = pMedCopper; //set medium
 TGeoMedium *Cu_medium_NICA = pMedIron; //set medium
 if(Cu_medium_NICA) CuV_NICA->SetMedium(Cu_medium_NICA);
 else Fatal("Main", "Invalid medium for Cu_NICA!");

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

 TGeoShape *G10S_NICA = new TGeoBBox(BmnNdetGeoPar::G10_name_NICA, Zdc_G10_X_size, Zdc_G10_Y_size, Zdc_G10_Z_size);
 TGeoVolume *G10V_NICA = new TGeoVolume(BmnNdetGeoPar::G10_name_NICA, G10S_NICA);
 G10V_NICA->SetLineColor(kOrange-7);
 G10V_NICA->SetFillColor(kOrange-7);
 TGeoMedium *G10_medium_NICA = pMedG10; //set medium
 if(G10_medium_NICA) G10V_NICA->SetMedium(G10_medium_NICA);
 else Fatal("Main", "Invalid medium for G10_NICA!");

 TGeoShape *PLAS_NICA = new TGeoBBox(BmnNdetGeoPar::PLA_name_NICA, Zdc_PLA_X_size, Zdc_PLA_Y_size, Zdc_PLA_Z_size);
 TGeoVolume *PLAV_NICA = new TGeoVolume(BmnNdetGeoPar::PLA_name_NICA, PLAS_NICA);
 PLAV_NICA->SetLineColor(kViolet);
 PLAV_NICA->SetFillColor(kViolet);
 TGeoMedium *PLA_medium_NICA = pMedPLA; //set medium
 if(PLA_medium_NICA) PLAV_NICA->SetMedium(PLA_medium_NICA);
 else Fatal("Main", "Invalid medium for PLA_NICA!");

 Double_t sample_Z_size = 2.*(Zdc_Cu_Z_size+Zdc_PLA_Z_size+Zdc_Sc_Z_size+Zdc_G10_Z_size+Zdc_PLA_Z_size); //=6.1
 Double_t sample_Veto_size = 2.*(Zdc_PLA_Z_size+Zdc_Sc_Z_size+Zdc_G10_Z_size+Zdc_PLA_Z_size); //=3.1
 Double_t itDelta=0;
 Double_t itInit_Cu, itCur_Cu=0; 
 Double_t itInit_Sc, itCur_Sc=0;
 Double_t itInit_G10, itCur_G10=0;
 Double_t itInit_Air, itCur_Air=0;
 Double_t itInit_Veto;
 Double_t itInit_PLA;

 for(Int_t it=0; it<Zdc_NSlices_in_module; it++) {//0-15
   itInit_PLA=-Zdc_module_Z_size + Zdc_PLA_Z_size + it*sample_Z_size;//before Sc
   mother_volume_NICA->AddNode(PLAV_NICA,2*it+1,new TGeoTranslation(0,0,itInit_PLA));
   cout <<"NICA it,PLA_before " <<itInit_PLA <<" " <<2*it+1 <<endl;

   if(it==0) {
     itInit_Veto=-Zdc_module_Z_size + 2.*Zdc_PLA_Z_size + Zdc_Veto_Z_size + it*sample_Z_size;
     mother_volume_NICA->AddNode(zdcSensitiveVolumeV_VETO,it+1,new TGeoTranslation(0,0,itInit_Veto));
     cout <<"NICA it,Veto " <<itInit_Veto <<" " <<it+1 <<endl;
   }   
   else {
     itInit_Sc=-Zdc_module_Z_size + 2.*Zdc_PLA_Z_size + Zdc_Sc_Z_size + it*sample_Z_size;
     mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA,it+1,new TGeoTranslation(0,0,itInit_Sc));
     cout <<"NICA it,Sc " <<itInit_Sc <<" " <<it+1 <<endl;
   }

   itInit_G10=-Zdc_module_Z_size + 2.*Zdc_PLA_Z_size + 2.*Zdc_Sc_Z_size + Zdc_G10_Z_size + it*sample_Z_size;//PCB
   mother_volume_NICA->AddNode(G10V_NICA,it+1,new TGeoTranslation(0,0,itInit_G10));
   cout <<"NICA it,PCB " <<itInit_G10 <<" " <<it+1 <<endl;

   itInit_PLA=-Zdc_module_Z_size + 2.*Zdc_PLA_Z_size + 2.*Zdc_Sc_Z_size + 2.*Zdc_G10_Z_size + Zdc_PLA_Z_size +it*sample_Z_size;//after Sc
   mother_volume_NICA->AddNode(PLAV_NICA,2*it+2,new TGeoTranslation(0,0,itInit_PLA));
   cout <<"NICA it,PLA_after " <<itInit_PLA <<" " <<2*it+2 <<endl;

   if(it<Zdc_NSlices_in_module-1) {
     itInit_Cu=-Zdc_module_Z_size + sample_Veto_size + Zdc_Cu_Z_size + it*sample_Z_size;//after Sc
     mother_volume_NICA->AddNode(CuV_NICA,it+2,new TGeoTranslation(0,0,itInit_Cu));
     cout <<"NICA it,Cu " <<itInit_Cu <<" ========= " <<it+2 <<endl;
   }
 }//for(Int_t it=0; it<Zdc_NSlices_in_module; it++)
 return 1;


}//FillModule_NICA_simple

