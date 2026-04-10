//---------------------------
//M.Golubeva (INR RAS), May 2016
//---------------------------

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMedium.h"
#include "TFile.h"

#include "../profiling/bmnloadlibs.C"

TGeoManager* gGeoMan = NULL;

//Global dimensions vacBox (for nDet RNF 44x44x94.6) in cm
Double_t Wall_nDet_dist = 0.1;//cm
//Double_t nDet_Z_size = 47.3; //94.6/2.//Veto+15Cu
Double_t nDet_Z_size = 22.9; //54.8/2.//Veto+15Cu

//walls BEFORE AFTER
Double_t Wall_X_size = 22.;//44.2/2
Double_t Wall_Y_size = 22.;//44.2/2
Double_t Wall_Z_size = 0.00001; //0.02/2.//vac
//Double_t Wall_Z_size = 0.001; //0.02/2.//vac
//Double_t Wall_Z_size_forNdet_RNF = 47.4; //94.8/2.//vac

//walls ABOVE UNDER in cm
Double_t Wall_X_size_above = 22.;//44/2
Double_t Wall_Y_size_above = 0.00001; //0.02/2.//vac;
//Double_t Wall_Z_size_above = 47.3; //94.6/2.//vac

//walls LEFT RIGHT in cm
Double_t Wall_X_size_left = 0.00001; //0.02/2.//vac;
//Double_t Wall_X_size_left = 0.001; //0.02/2.//vac;
Double_t Wall_Y_size_left = 22;//44/2
//Double_t Wall_Z_size_left = 47.3; //94.6/2.//vac


//media ============================================
TGeoMedium *pMedVacuum = 0;
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedScint = 0;

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

// medium polystyrene (Scint.)
  FairGeoMedium* mPolystyrene = geoMedia->getMedium("polystyrene");
  if ( ! mPolystyrene ) Fatal("Main", "FairMedium polystyrene not found");
  geoBuild->createMedium(mPolystyrene);
  pMedScint = gGeoManager->GetMedium("polystyrene");
  if ( ! pMedScint ) Fatal("Main", "Medium polystyrene not found");
}//DefineRequiredMedia


//void create_rootgeom_vacBox_ONEDET_for_nDet_RNF_44x44cm_16slices(Double_t vacBox_dist=0,Double_t WALL_Xpos=0,Double_t WALL_Ypos=0,Double_t WALL_Zpos=900,Double_t WALL_Xpos_after=0,Double_t WALL_Ypos_after=0,Double_t WALL_Zpos_after=900,Double_t WALL_Xpos_ABOVE=0,Double_t WALL_Ypos_ABOVE=0,Double_t WALL_Zpos_ABOVE=900,Double_t WALL_Xpos_LEFT=0,Double_t WALL_Ypos_LEFT=0,Double_t WALL_Zpos_LEFT=900,Double_t WALL_Xpos_RIGHT=0,Double_t WALL_Ypos_RIGHT=0,Double_t WALL_Zpos_RIGHT=900,Double_t rotY=0) {
void create_rootgeom_vacBox_ONEDET_for_nDet_RNF_44x44cm_8slices(Double_t nDet_dist=780, Double_t rotY=0, Double_t rotX=0) {

    // Load necessary libraries
    bmnloadlibs(); // load libraries

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ---------------------------------
    const TString geoDetectorName = "vacBox_ONEDET_for_nDet_RNF_44x44cm_8slices";

    Double_t vacBox_dist;
    vacBox_dist = nDet_dist - Wall_nDet_dist;

    //TString sDist = "";
  TString sDist_1 = "";
  TString sDist_2 = "";

  TString sRotX_1 = "";
  TString sRotX_2 = "";

  TString sRotY_1 = "";
  TString sRotY_2 = "";

  Int_t dist_1 = Int_t(vacBox_dist); //2
  //Int_t dist_22 = Int_t(100*(vacBox_dist-Int_t(vacBox_dist)));//9
  Int_t dist_22 = Int_t(100*(vacBox_dist-Int_t(vacBox_dist)))+1;//9

  Int_t rotX_1 = Int_t(rotX); //2
  //Int_t rotX_22 = Int_t(100*(rotX-Int_t(rotX)));//9
  Int_t rotX_22 = -Int_t(100*(rotX-Int_t(rotX)));//9
  //Int_t rotX_22 = -Int_t(100*(rotX-Int_t(rotX)))+1;//9

  Int_t rotY_1 = Int_t(rotY); //2
  //Int_t rotY_22 = Int_t(100*(rotY-Int_t(rotY)));//9
  Int_t rotY_22 = -Int_t(100*(rotY-Int_t(rotY)));//9

  //sDist += vacBox_dist;
  sDist_1 += dist_1;
  sDist_2 += dist_22;
  sRotX_1 +=rotX_1;
  sRotX_2 +=rotX_22;
  sRotY_1 +=rotY_1;
  sRotY_2 +=rotY_22;

  TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zdist_" + sDist_1 + "." + sDist_2 + "cm_" + "rotY_"  + sRotY_1  +  "." + sRotY_2 + "deg" + "_" + "rotX_"  + sRotX_1 + "." + sRotX_2 + "deg" + ".root";

    /*
//Det 1
  TString sDist_1 = "";
  TString sDist_2 = "";
  TString sRotY_1 = "";
  TString sRotY_2 = "";

  Int_t dist_1 = Int_t(vacBox_dist); //2
  Int_t dist_22 = Int_t(100*(vacBox_dist-Int_t(vacBox_dist)))+1;//9
  //Int_t dist_22 = Int_t(100*(vacBox_dist-Int_t(vacBox_dist)));//9

  Int_t rotY_1 = Int_t(rotY); //2
  //Int_t rotY_22 = Int_t(100*(rotY-Int_t(rotY)));//9
  Int_t rotY_22 = -Int_t(100*(rotY-Int_t(rotY)));//9

  sDist_1 += dist_1;
  sDist_2 += dist_22;
  sRotY_1 +=rotY_1;
  sRotY_2 +=rotY_22;
    
  TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zdist_" + sDist_1 +  "." + sDist_2 +"cm" + "_" + "rotY_"  + sRotY_1 +  "." + sRotY_2 + "deg" + ".root";
    */
    // ----  global geometry parameters  ---------------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();

    // -------   Load media from media file   ----------------------------------
    TString medFile = gPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
    //geoMedia->list();
    //geoMedia->print();
    DefineRequiredMedia(geoMedia, geoBuild);
    // -----------------------------------------------------------------------

    // --------------   Create geometry and global top volume  ---------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);

    // Define TOP Geometry
    TGeoVolume* WALL = new TGeoVolumeAssembly("WALL");
    WALL->SetMedium(pMedVacuum);
    //TGeoVolume* WALL_1 = new TGeoVolumeAssembly("WALL_1");
    //WALL_1->SetMedium(pMedVacuum);

  Double_t WALL_center_Xpos;
  Double_t WALL_center_Ypos;
  Double_t WALL_center_Zpos;

  TGeoRotation *rot = new TGeoRotation("rot");
  rot->RotateX(rotX);
  rot->RotateY(rotY);

  Double_t master[3];
  Double_t local[3];

//===========================
//Wall before
  TGeoBBox *wallSensitiveVolumeS_before = new TGeoBBox("zdc01s", Wall_X_size, Wall_Y_size, Wall_Z_size);
  TGeoVolume *wallSensitiveVolumeV_before = new TGeoVolume("zdc01s", wallSensitiveVolumeS_before);
  wallSensitiveVolumeV_before->SetMedium(pMedVacuum);
  
  master[0] = 0; master[1] = 0; master[2] = 0; 
  local[0] = 0; local[1] = 0; local[2] = vacBox_dist;//699.9 
  rot->LocalToMaster(local, master);

  WALL_center_Xpos = master[0];
  WALL_center_Ypos = master[1];
  WALL_center_Zpos = master[2];
    //TGeoRotation *rot = new TGeoRotation("rot");//vacBox_1
    //rot->RotateY(rotY);
    TGeoCombiTrans *wallWALL_before = new TGeoCombiTrans("wallWALL_before",WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos,rot);
    //cout <<"WALL before " <<WALL_center_Xpos <<" " <<WALL_center_Ypos <<" " <<WALL_center_Zpos <<endl;
    WALL->AddNode(wallSensitiveVolumeV_before,1,wallWALL_before);
    //WALL_1->AddNode(wallSensitiveVolumeV_before,1,wallWALL_before);
    //return;            
    /*
//Wall after

    //TGeoBBox *wallSensitiveVolumeS_after = new TGeoBBox("zdc01s_after", Wall_X_size, Wall_Y_size, Wall_Z_size);
    //TGeoVolume *wallSensitiveVolumeV_after = new TGeoVolume("zdc01s_after", wallSensitiveVolumeS_after);
    //wallSensitiveVolumeV_after->SetMedium(pMedVacuum);

    rot = new TGeoRotation("rot");//vacBox_1
    rot->RotateY(rotY);
    TGeoCombiTrans *wallWALL_after = new TGeoCombiTrans("wallWALL_after",WALL_Xpos_after, WALL_Ypos_after, WALL_center_Zpos_after,rot);
    //TGeoVolume *WALL_2 = new TGeoVolume("WALL_2", wallSensitiveVolumeS_after);
    //WALL_2->AddNode(wallSensitiveVolumeV_after,2,wallWALL_after);
    WALL->AddNode(wallSensitiveVolumeV_before,2,wallWALL_after);
    //top->AddNode(wallSensitiveVolumeV_after,2,wallWALL_after);
    */          
//===========================
//Wall after
    //rot = new TGeoRotation("rot");//vacBox_1
    //rot->RotateY(rotY);
  master[0] = 0; master[1] = 0; master[2] = 0; 
  local[0] = 0; local[1] = 0; local[2] = nDet_dist+2.*nDet_Z_size+Wall_nDet_dist; 
  rot->LocalToMaster(local, master);

  WALL_center_Xpos = master[0];
  WALL_center_Ypos = master[1];
  WALL_center_Zpos = master[2];
    TGeoCombiTrans *wallWALL_after = new TGeoCombiTrans("wallWALL_after",WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos,rot);
    WALL->AddNode(wallSensitiveVolumeV_before,2,wallWALL_after);
    
//===========================
//Wall above
    TGeoBBox *wallSensitiveVolumeS_above = new TGeoBBox("zdc01s", Wall_X_size_above, Wall_Y_size_above, nDet_Z_size);
    TGeoVolume *wallSensitiveVolumeV_above = new TGeoVolume("zdc01s", wallSensitiveVolumeS_above);
    wallSensitiveVolumeV_above->SetMedium(pMedVacuum);

    //rot = new TGeoRotation("rot");//vacBox_1
    //rot->RotateY(rotY);
  master[0] = 0; master[1] = 0; master[2] = 0; 
  local[0] = 0; local[1] = Wall_Y_size+Wall_nDet_dist; local[2] = nDet_dist+nDet_Z_size; 
  rot->LocalToMaster(local, master);

  WALL_center_Xpos = master[0];
  WALL_center_Ypos = master[1];
  WALL_center_Zpos = master[2];
    TGeoCombiTrans *wallWALL_above = new TGeoCombiTrans("wallWALL_above",WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos,rot);
    //cout <<"WALL_above_Ypos " <<WALL_Zpos_ABOVE <<endl;
    WALL->AddNode(wallSensitiveVolumeV_above,3,wallWALL_above);

//===========================
//Wall under
  master[0] = 0; master[1] = 0; master[2] = 0; 
  local[0] = 0; local[1] = -(Wall_Y_size+Wall_nDet_dist); local[2] = nDet_dist+nDet_Z_size; 
  rot->LocalToMaster(local, master);

  WALL_center_Xpos = master[0];
  WALL_center_Ypos = master[1];
  WALL_center_Zpos = master[2];
  //rot = new TGeoRotation("rot");//vacBox_1
  //rot->RotateY(rotY);
    TGeoCombiTrans *wallWALL_under = new TGeoCombiTrans("wallWALL_under",WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos,rot);
    //cout <<"WALL_above_Ypos " <<WALL_Zpos_ <<endl;
    WALL->AddNode(wallSensitiveVolumeV_above,4,wallWALL_under);

//===========================
//Wall left
  master[0] = 0; master[1] = 0; master[2] = 0; 
  local[0] = Wall_X_size+Wall_nDet_dist; local[1] = 0; local[2] = nDet_dist+nDet_Z_size; 
  rot->LocalToMaster(local, master);

  WALL_center_Xpos = master[0];
  WALL_center_Ypos = master[1];
  WALL_center_Zpos = master[2];
    TGeoBBox *wallSensitiveVolumeS_left = new TGeoBBox("zdc01s", Wall_X_size_left, Wall_Y_size_left, nDet_Z_size);
    TGeoVolume *wallSensitiveVolumeV_left = new TGeoVolume("zdc01s", wallSensitiveVolumeS_left);
    wallSensitiveVolumeV_left->SetMedium(pMedVacuum);

    //rot = new TGeoRotation("rot");//vacBox_1
    //rot->RotateY(rotY);
    TGeoCombiTrans *wallWALL_left = new TGeoCombiTrans("wallWALL_left",WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos,rot);
    //cout <<"WALL_left_Xpos " <<WALL_Zpos_LEFT <<endl;
    WALL->AddNode(wallSensitiveVolumeV_left,5,wallWALL_left);

//===========================
//Wall right
  master[0] = 0; master[1] = 0; master[2] = 0; 
  local[0] = -(Wall_X_size+Wall_nDet_dist); local[1] = 0; local[2] = nDet_dist+nDet_Z_size; 
  rot->LocalToMaster(local, master);

  WALL_center_Xpos = master[0];
  WALL_center_Ypos = master[1];
  WALL_center_Zpos = master[2];
  //rot = new TGeoRotation("rot");//vacBox_1
  //rot->RotateY(rotY);
    TGeoCombiTrans *wallWALL_right = new TGeoCombiTrans("wallWALL_right",WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos,rot);
    //cout <<"WALL_right_Xpos " <<WALL_Zpos_RIGHT <<endl;
    WALL->AddNode(wallSensitiveVolumeV_left,6,wallWALL_right);
    
//
    top->AddNode(WALL, 0);
    //top->AddNode(WALL_1, 0);
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


