//---------------------------
//M.Golubeva (INR RAS), May 2016
//---------------------------

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMedium.h"
#include "TFile.h"

#include "../profiling/bmnloadlibs.C"

//Detector's position
//const Double_t WALL_Xpos = 0.0;
//const Double_t WALL_Ypos = 0.0;
//const Double_t WALL_Zpos = 950.;//cm to WALL surface
//const Double_t WALL_center_Zpos = 950.5;//9.505 m (950+0.5);//cm
//const Double_t WALL_Zpos = 900.;//cm to WALL surface
//const Double_t WALL_center_Zpos = 900.01;//9.001 m (900+0.01);//cm
//const Double_t WALL_Zpos = 878.1;//cm to WALL surface
//const Double_t WALL_center_Zpos = 878.11;//8.7811 m (878.1+0.01);//cm//vac
//const Double_t WALL_center_Zpos = 878.3;//8.783 m (878.1+0.2);//cm//sc

//Detector's construct parameters ===================

//Global dimensions for WALL in cm
Double_t Wall_X_size = 77.5;//155/2
Double_t Wall_Y_size = 52.5;//105/2
//Double_t Wall_Z_size = 0.01; //0.02/2.//vac
Double_t Wall_Z_size = 0.00001; //0.02/2.//vac

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
  /*
    // medium FscScint (Scint.)
    FairGeoMedium* mPolystyrene = geoMedia->getMedium("FscScint");
    if ( ! mPolystyrene ) Fatal("Main", "FairMedium FscScint not found");
    geoBuild->createMedium(mPolystyrene);
    pMedScint = gGeoManager->GetMedium("FscScint");
    if ( ! pMedScint ) Fatal("Main", "Medium FscScint not found");
  */
}//DefineRequiredMedia


void create_rootgeom_wall_oldnames_vacuum_zdc_155x105cm(Double_t WALL_Xpos=0,Double_t WALL_Ypos=0,Double_t WALL_Zpos=900,Double_t rotY=0) {

    // Load necessary libraries
    //gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ---------------------------------
    const TString geoDetectorName = "wall_vacuum_zdc_155x105cm";
    const TString geoDetectorVersion = "v1";
    //TString geoFileName = gPath + "/geometry/" + geoDetectorName + geoDetectorVersion + ".root";
    //Double_t WALL_center_Zpos = WALL_Zpos + Wall_Z_size;;//8.7811 m (878.1+0.01);//cm//vac
    Double_t WALL_center_Zpos = WALL_Zpos;//cm//vac
    /*
  TString sXshift = "";
  TString sYshift = "";
  TString sZshift = "";
  Int_t Xshift = Int_t(WALL_Xpos*10.); //mm
  Int_t Yshift = Int_t(WALL_Ypos*10.); //mm
  Int_t Zshift = Int_t(WALL_Zpos*10.); //mm
  sXshift += Xshift;
  sYshift += Yshift;
  sZshift += Zshift;
    */

  TString sXshift_1 = "";
  TString sXshift_2 = "";
  TString sXshift_3 = "";

  TString sYshift_0 = "-";
  TString sYshift_1 = "";
  TString sYshift_2 = "";
  //TString sYshift_3 = "";

  TString sZshift_1 = "";
  TString sZshift_2 = "";

  TString sRotY_1 = "";
  TString sRotY_2 = "";

  Int_t Xshift_1 = Int_t(WALL_Xpos); //cm
  //Int_t Xshift_2 = Int_t(0.1+10*(WALL_Xpos-Int_t(WALL_Xpos))); //cm
  //Int_t Xshift_2 = Int_t(0.5+100*(WALL_Xpos-Int_t(WALL_Xpos))); //cm
  //Int_t Xshift_2 = Int_t(0.5+100*(TMath::Abs(WALL_Xpos)-Int_t(TMath::Abs(WALL_Xpos)))); //cm
  //Int_t Xshift_2 = Int_t(100*(WALL_Xpos-Int_t(WALL_Xpos))); //cm
  //Int_t Xshift_2 = 0; //cm
  Int_t Xshift_2 = Int_t(100*(WALL_Xpos-Int_t(WALL_Xpos)))+1; //cm
  //Int_t Xshift_3 = Int_t(100*(WALL_Xpos-Int_t(WALL_Xpos)))+1; //cm
  sXshift_1 += Xshift_1;
  sXshift_2 += Xshift_2;
  //sXshift_3 += Xshift_3;

  Int_t Yshift_1 = Int_t(WALL_Ypos); //cm
  //Int_t Yshift_2 = Int_t(100*(WALL_Ypos-Int_t(WALL_Ypos)))+1; //cm
  //Int_t Yshift_2 = Int_t(100*(WALL_Ypos-Int_t(WALL_Ypos))); //cm
  Int_t Yshift_2 = TMath::Abs(Int_t(10*(WALL_Ypos))); //cm
  //Int_t Yshift_2 = Int_t(0.5+100*(TMath::Abs(ZDC_Ypos)-Int_t(TMath::Abs(ZDC_Ypos)))); //cm
  sYshift_1 += Yshift_1;
  sYshift_2 += Yshift_2;

  Int_t Zshift_1 = Int_t(WALL_Zpos); //cm
  //Int_t Zshift_2 = Int_t(100*(WALL_Zpos-Int_t(WALL_Zpos))); //cm
  //Int_t Zshift_2 = Int_t(100*(WALL_Zpos-Int_t(WALL_Zpos)))+1; //cm
  //Int_t Zshift_2 = Int_t(10*(WALL_Zpos-Int_t(WALL_Zpos)))+1; //cm
  Int_t Zshift_2 = Int_t(10*(WALL_Zpos-Int_t(WALL_Zpos))); //cm
  //Int_t Zshift_2 = Int_t(10*(WALL_Zpos-Int_t(WALL_Zpos)))+1; //cm
  sZshift_1 += Zshift_1;
  sZshift_2 += Zshift_2;
  cout <<"Zshift_1,Zshift_2 " <<Zshift_1 <<" " <<10*(WALL_Zpos-Int_t(WALL_Zpos)) <<" " <<Zshift_2 <<endl;

  Int_t rotY_1 = Int_t(rotY); //2 from 2.9
  Int_t rotY_2 = Int_t(100*(rotY-Int_t(rotY)));//9 from 2.9
  //Int_t rotY_2 = Int_t(100*(rotY-Int_t(rotY)))+1;//9 from 2.9
  sRotY_1 +=rotY_1;
  sRotY_2 +=rotY_2;

  //TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zpos_" + sZshift + "mm" + "_" + "Xshift_" + sXshift + "mm" + "_" + "Yshift_" + sYshift + "mm" +  "_" + geoDetectorVersion + ".root";
  //TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zpos_" + sZshift_1 + "." + sZshift_2 + "cm" + "_" + "Xshift_" + sXshift_1 + "." + sXshift_2 + "cm" + "_" + "Yshift_" + sYshift_1 + "." + sYshift_2 + "cm" + "_" + "rotationY_"  + sRotY_1  +  "." + sRotY_2 + "deg_" + geoDetectorVersion + ".root";
  TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zpos_" + sZshift_1 + "." + sZshift_2 + "cm" + "_" + "Xshift_" + sXshift_1 + "." + sXshift_2 + "cm" + "_" + "Yshift_" + sYshift_0 + sYshift_1 + "." + sYshift_2 + "cm" + "_" + "rotationY_"  + sRotY_1  +  "." + sRotY_2 + "deg_" + geoDetectorVersion + ".root";
  //TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zpos_" + sZshift_1 + "." + sZshift_2 + "cm" + "_" + "Xshift_" + sXshift_1 + "." + sXshift_2 + sXshift_3 + "cm" + "_" + "Yshift_" + sYshift_1 + "." + sYshift_2 + "cm" + "_" + "rotationY_"  + sRotY_1  +  "." + sRotY_2 + "deg_" + geoDetectorVersion + ".root";
    // -------------------------------------------------------------------------

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
    // -------------------------------------------------------------------------

    // --------------   Create geometry and global top volume  ------------------------
    gGeoManager = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoManager->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    //top->SetMedium(pMedVacuum);
    gGeoManager->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

    // Define WALL Geometry
    //TString wall_common_name = "WALL_common";
    TString wall_common_name = "ZDC_common";
    TGeoVolume *ZDC_common = new TGeoVolumeAssembly(wall_common_name);
    ZDC_common->SetMedium(pMedAir);
    //ZDC_common->SetMedium(pMedVacuum);

    TString wall_name = "zdc01";
    //TGeoTube *WALLS = new TGeoTube(wall_name, Wall_inner_radius, Wall_outer_radius, Wall_Z_size);
    TGeoBBox *WALLS = new TGeoBBox(wall_name, Wall_X_size, Wall_Y_size, Wall_Z_size);
    TGeoVolume *WALLV = new TGeoVolume(wall_name, WALLS);
    //WALLV->SetMedium(pMedAir);
    WALLV->SetMedium(pMedVacuum);
    //WALLV->SetMedium(pMedScint);

    TString wallSensitive_name = "zdc01s";
    //TGeoTube *wallSensitiveVolumeS = new TGeoTube(wallSensitive_name, Wall_inner_radius, Wall_outer_radius, Wall_Z_size);
    TGeoBBox *wallSensitiveVolumeS = new TGeoBBox(wallSensitive_name, Wall_X_size, Wall_Y_size, Wall_Z_size);
    TGeoVolume *wallSensitiveVolumeV = new TGeoVolume(wallSensitive_name, wallSensitiveVolumeS);
    //wallSensitiveVolumeV->SetMedium(pMedScint);
    wallSensitiveVolumeV->SetMedium(pMedVacuum);

    //insert wallSensitiveVolumeV into WALLV 
    TGeoTranslation *wall_position = new TGeoTranslation(0.,0.,0.);
    WALLV->AddNode(wallSensitiveVolumeV,1,wall_position);

    //Adding the wall mother volume to the global TOP Volume
    //TGeoTranslation *wallWALL = new TGeoTranslation("wallWALL",WALL_Xpos, WALL_Ypos, WALL_center_Zpos);

    TGeoRotation *rot = new TGeoRotation("rot");
    rot->RotateY(rotY);
    TGeoCombiTrans *wallWALL = new TGeoCombiTrans("wallWALL",WALL_Xpos, WALL_Ypos, WALL_center_Zpos,rot);
    ZDC_common->AddNode(WALLV, 1, wallWALL);//WALL right

    /*
    TGeoRotation *rotWALL = new TGeoRotation();
    rotWALL->RotateY(30.);
    TGeoCombiTrans *wallWALL2 = new TGeoCombiTrans(WALL_Xpos, WALL_Ypos, -WALL_center_Zpos, rotWALL);
    WALL_common->AddNode(WALLV, 2, wallWALL2);//WALL left
    */

    //top->AddNode(WALL_common, 0);
    top->AddNode(ZDC_common, 0);

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


