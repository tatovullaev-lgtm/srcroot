//---------------------------
// M.Golubeva (INR RAS), May 2016
//---------------------------

#include "../profiling/bmnloadlibs.C"
#include "TFile.h"
#include "TGeoManager.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"

TGeoManager* gGeoMan = NULL;

// Global dimensions vacBox (for nDet RNF 44x44x94.6) in cm
Double_t Wall_nDet_dist = 0.1;   // cm
// Double_t nDet_Z_size = 22.9; //54.8/2.//Veto+15Cu
Double_t nDet_Z_size = 24.8;   // 49.6/2.//Veto+7Cu

// walls BEFORE AFTER
Double_t Wall_X_size = 22.;       // 44.2/2
Double_t Wall_Y_size = 22.;       // 44.2/2
Double_t Wall_Z_size = 0.00001;   // 0.02/2.//vac

// walls ABOVE UNDER in cm
Double_t Wall_X_size_above = 22.;       // 44/2
Double_t Wall_Y_size_above = 0.00001;   // 0.02/2.//vac;

// walls LEFT RIGHT in cm
Double_t Wall_X_size_left = 0.00001;   // 0.02/2.//vac;
Double_t Wall_Y_size_left = 22;        // 44/2

// media ============================================
TGeoMedium* pMedVacuum = 0;
TGeoMedium* pMedAir = 0;
TGeoMedium* pMedScint = 0;

class FairGeoMedia;
class FairGeoBuilder;

void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild)
{
    // vacuum medium
    FairGeoMedium* mVacuum = geoMedia->getMedium("vacuum");
    if (!mVacuum)
        Fatal("Main", "FairMedium vacuum not found");
    geoBuild->createMedium(mVacuum);
    pMedVacuum = gGeoManager->GetMedium("vacuum");
    if (!pMedVacuum)
        Fatal("Main", "Medium vacuum not found");

    // air medium
    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if (!mAir)
        Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    pMedAir = gGeoManager->GetMedium("air");
    if (!pMedAir)
        Fatal("Main", "Medium air not found");

    // medium polystyrene (Scint.)
    FairGeoMedium* mPolystyrene = geoMedia->getMedium("polystyrene");
    if (!mPolystyrene)
        Fatal("Main", "FairMedium polystyrene not found");
    geoBuild->createMedium(mPolystyrene);
    pMedScint = gGeoManager->GetMedium("polystyrene");
    if (!pMedScint)
        Fatal("Main", "Medium polystyrene not found");
}   // DefineRequiredMedia

void create_rootgeom_vacBox_TWODET_Planes_for_nDet_RNF_44x44cm_8slices(Double_t nDet_dist_1 = 700,
                                                                       Double_t rotY_1 = -10,
                                                                       Double_t rotX_1 = -4.5,
                                                                       Double_t nDet_dist_2 = 700,
                                                                       Double_t rotY_2 = -10,
                                                                       Double_t rotX_2 = 4.5)
{

    // Load necessary libraries
    bmnloadlibs();   // load libraries

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ---------------------------------
    const TString geoDetectorName = "vacBox_TWODET_Planes_for_nDet_RNF_44x44cm_8slices";

    // DET 1
    Double_t vacBox_dist_1;
    vacBox_dist_1 = nDet_dist_1 - Wall_nDet_dist;

    TString sDist_1_1 = "";
    TString sDist_2_1 = "";

    TString sRotX_1_1 = "";
    TString sRotX_2_1 = "";

    TString sRotY_1_1 = "";
    TString sRotY_2_1 = "";

    Int_t dist_1_1 = Int_t(vacBox_dist_1);   // 2
    // Int_t dist_22_1 = Int_t(100*(vacBox_dist_1-Int_t(vacBox_dist_1)));//9
    Int_t dist_22_1 = Int_t(100 * (vacBox_dist_1 - Int_t(vacBox_dist_1))) + 1;   // 9

    Int_t rotX_1_1 = Int_t(rotX_1);   // 2
    // Int_t rotX_22_1 = Int_t(100*(rotX_1-Int_t(rotX_1)));//9    //+4.5deg
    Int_t rotX_22_1 = -Int_t(100 * (rotX_1 - Int_t(rotX_1)));   // 9 //-4.5
    // Int_t rotX_22_1 = -Int_t(100*(rotX_1-Int_t(rotX_1)))+1;//9

    Int_t rotY_1_1 = Int_t(rotY_1);   // 2
    // Int_t rotY_22_1 = Int_t(100*(rotY_1-Int_t(rotY_1)));//9
    Int_t rotY_22_1 = -Int_t(100 * (rotY_1 - Int_t(rotY_1)));   // 9

    sDist_1_1 += dist_1_1;
    sDist_2_1 += dist_22_1;
    sRotX_1_1 += rotX_1_1;
    sRotX_2_1 += rotX_22_1;
    sRotY_1_1 += rotY_1_1;
    sRotY_2_1 += rotY_22_1;

    // DET 2
    Double_t vacBox_dist_2;
    vacBox_dist_2 = nDet_dist_2 - Wall_nDet_dist;

    TString sDist_1_2 = "";
    TString sDist_2_2 = "";

    TString sRotX_1_2 = "";
    TString sRotX_2_2 = "";

    TString sRotY_1_2 = "";
    TString sRotY_2_2 = "";

    Int_t dist_1_2 = Int_t(vacBox_dist_2);   // 2
    // Int_t dist_22_2 = Int_t(100*(vacBox_dist_2-Int_t(vacBox_dist_2)));//9
    Int_t dist_22_2 = Int_t(100 * (vacBox_dist_2 - Int_t(vacBox_dist_2))) + 1;   // 9

    Int_t rotX_1_2 = Int_t(rotX_2);                            // 2
    Int_t rotX_22_2 = Int_t(100 * (rotX_2 - Int_t(rotX_2)));   // 9
    // Int_t rotX_22_2 = -Int_t(100*(rotX_2-Int_t(rotX_2)));//9
    // Int_t rotX_22_2 = -Int_t(100*(rotX_2-Int_t(rotX_2)))+1;//9

    Int_t rotY_1_2 = Int_t(rotY_2);   // 2
    // Int_t rotY_22_2 = Int_t(100*(rotY_2-Int_t(rotY_2)));//9
    Int_t rotY_22_2 = -Int_t(100 * (rotY_2 - Int_t(rotY_2)));   // 9

    sDist_1_2 += dist_1_2;
    sDist_2_2 += dist_22_2;
    sRotX_1_2 += rotX_1_2;
    sRotX_2_2 += rotX_22_2;
    sRotY_1_2 += rotY_1_2;
    sRotY_2_2 += rotY_22_2;

    TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "ZdistDET_1_" + sDist_1_1 + "." + sDist_2_1
                          + "cm_" + "rotY_" + sRotY_1_1 + "." + sRotY_2_1 + "deg" + "_" + "rotX_" + sRotX_1_1 + "."
                          + sRotX_2_1 + "deg" + "_ZdistDET_2_" + sDist_1_2 + "." + sDist_2_2 + "cm_" + "rotY_"
                          + sRotY_1_2 + "." + sRotY_2_2 + "deg" + "_" + "rotX_" + sRotX_1_2 + "." + sRotX_2_2
                          + "deg"
                            ".root";

    // ----  global geometry parameters  ---------------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();

    // -------   Load media from media file   ----------------------------------
    TString medFile = gPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
    // geoMedia->list();
    // geoMedia->print();
    DefineRequiredMedia(geoMedia, geoBuild);
    // -----------------------------------------------------------------------

    // --------------   Create geometry and global top volume  ---------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);

    Double_t WALL_center_Xpos;
    Double_t WALL_center_Ypos;
    Double_t WALL_center_Zpos;

    Double_t master[3];
    Double_t local[3];

    TGeoVolume* WALL = new TGeoVolumeAssembly("WALL");
    WALL->SetMedium(pMedVacuum);

    // DET 1
    TGeoRotation* rot = new TGeoRotation("rot");
    rot->RotateX(rotX_1);
    rot->RotateY(rotY_1);

    // DET 2
    TGeoRotation* rot_2 = new TGeoRotation("rot_2");
    rot_2->RotateX(rotX_2);
    rot_2->RotateY(rotY_2);

    //===========================
    // Wall before
    TGeoBBox* wallSensitiveVolumeS_before = new TGeoBBox("zdc01s", Wall_X_size, Wall_Y_size, Wall_Z_size);
    TGeoVolume* wallSensitiveVolumeV_before = new TGeoVolume("zdc01s", wallSensitiveVolumeS_before);
    wallSensitiveVolumeV_before->SetMedium(pMedVacuum);

    // DET 1
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = 0;
    local[1] = 0;
    local[2] = vacBox_dist_1;   // 699.9
    rot->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_before =
        new TGeoCombiTrans("wallWALL_before", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot);
    // cout <<"WALL before " <<WALL_center_Xpos <<" " <<WALL_center_Ypos <<" " <<WALL_center_Zpos <<endl;
    WALL->AddNode(wallSensitiveVolumeV_before, 1, wallWALL_before);
    // return;

    // DET 2
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = 0;
    local[1] = 0;
    local[2] = vacBox_dist_2;   // 699.9
    rot_2->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_before_2 =
        new TGeoCombiTrans("wallWALL_before_2", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot_2);
    // cout <<"WALL before " <<WALL_center_Xpos <<" " <<WALL_center_Ypos <<" " <<WALL_center_Zpos <<endl;
    WALL->AddNode(wallSensitiveVolumeV_before, 11, wallWALL_before_2);
    // return;

    //===========================
    // Wall after
    // DET 1
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = 0;
    local[1] = 0;
    local[2] = nDet_dist_1 + 2. * nDet_Z_size + Wall_nDet_dist;
    rot->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_after =
        new TGeoCombiTrans("wallWALL_after", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot);
    WALL->AddNode(wallSensitiveVolumeV_before, 2, wallWALL_after);

    // DET 2
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = 0;
    local[1] = 0;
    local[2] = nDet_dist_2 + 2. * nDet_Z_size + Wall_nDet_dist;
    rot_2->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_after_2 =
        new TGeoCombiTrans("wallWALL_after_2", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot_2);
    WALL->AddNode(wallSensitiveVolumeV_before, 12, wallWALL_after_2);

    //===========================
    // Wall above
    TGeoBBox* wallSensitiveVolumeS_above = new TGeoBBox("zdc01s", Wall_X_size_above, Wall_Y_size_above, nDet_Z_size);
    TGeoVolume* wallSensitiveVolumeV_above = new TGeoVolume("zdc01s", wallSensitiveVolumeS_above);
    wallSensitiveVolumeV_above->SetMedium(pMedVacuum);

    // DET 1
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = 0;
    local[1] = Wall_Y_size + Wall_nDet_dist;
    local[2] = nDet_dist_1 + nDet_Z_size;
    rot->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_above =
        new TGeoCombiTrans("wallWALL_above", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot);
    // cout <<"WALL_above_Ypos " <<WALL_Zpos_ABOVE <<endl;
    WALL->AddNode(wallSensitiveVolumeV_above, 3, wallWALL_above);

    // DET 2
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = 0;
    local[1] = Wall_Y_size + Wall_nDet_dist;
    local[2] = nDet_dist_2 + nDet_Z_size;
    rot_2->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_above_2 =
        new TGeoCombiTrans("wallWALL_above_2", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot_2);
    // cout <<"WALL_above_Ypos " <<WALL_Zpos_ABOVE <<endl;
    WALL->AddNode(wallSensitiveVolumeV_above, 13, wallWALL_above_2);
    //===========================
    // Wall under
    // DET 1
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = 0;
    local[1] = -(Wall_Y_size + Wall_nDet_dist);
    local[2] = nDet_dist_1 + nDet_Z_size;
    rot->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_under =
        new TGeoCombiTrans("wallWALL_under", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot);
    // cout <<"WALL_above_Ypos " <<WALL_Zpos_ <<endl;
    WALL->AddNode(wallSensitiveVolumeV_above, 4, wallWALL_under);

    // DET 2
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = 0;
    local[1] = -(Wall_Y_size + Wall_nDet_dist);
    local[2] = nDet_dist_2 + nDet_Z_size;
    rot_2->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_under_2 =
        new TGeoCombiTrans("wallWALL_under_2", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot_2);
    // cout <<"WALL_above_Ypos " <<WALL_Zpos_ <<endl;
    WALL->AddNode(wallSensitiveVolumeV_above, 14, wallWALL_under_2);

    //===========================
    // Wall left
    TGeoBBox* wallSensitiveVolumeS_left = new TGeoBBox("zdc01s", Wall_X_size_left, Wall_Y_size_left, nDet_Z_size);
    TGeoVolume* wallSensitiveVolumeV_left = new TGeoVolume("zdc01s", wallSensitiveVolumeS_left);
    wallSensitiveVolumeV_left->SetMedium(pMedVacuum);

    // DET 1
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = Wall_X_size + Wall_nDet_dist;
    local[1] = 0;
    local[2] = nDet_dist_1 + nDet_Z_size;
    rot->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_left =
        new TGeoCombiTrans("wallWALL_left", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot);
    // cout <<"WALL_left_Xpos " <<WALL_Zpos_LEFT <<endl;
    WALL->AddNode(wallSensitiveVolumeV_left, 5, wallWALL_left);

    // DET 2
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = Wall_X_size + Wall_nDet_dist;
    local[1] = 0;
    local[2] = nDet_dist_2 + nDet_Z_size;
    rot_2->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_left_2 =
        new TGeoCombiTrans("wallWALL_left_2", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot_2);
    // cout <<"WALL_left_Xpos " <<WALL_Zpos_LEFT <<endl;
    WALL->AddNode(wallSensitiveVolumeV_left, 15, wallWALL_left_2);

    //===========================
    // Wall right
    // DET 1
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = -(Wall_X_size + Wall_nDet_dist);
    local[1] = 0;
    local[2] = nDet_dist_1 + nDet_Z_size;
    rot->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_right =
        new TGeoCombiTrans("wallWALL_right", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot);
    // cout <<"WALL_right_Xpos " <<WALL_Zpos_RIGHT <<endl;
    WALL->AddNode(wallSensitiveVolumeV_left, 6, wallWALL_right);

    // DET 2
    master[0] = 0;
    master[1] = 0;
    master[2] = 0;
    local[0] = -(Wall_X_size + Wall_nDet_dist);
    local[1] = 0;
    local[2] = nDet_dist_2 + nDet_Z_size;
    rot_2->LocalToMaster(local, master);

    WALL_center_Xpos = master[0];
    WALL_center_Ypos = master[1];
    WALL_center_Zpos = master[2];

    TGeoCombiTrans* wallWALL_right_2 =
        new TGeoCombiTrans("wallWALL_right_2", WALL_center_Xpos, WALL_center_Ypos, WALL_center_Zpos, rot_2);
    // cout <<"WALL_right_Xpos " <<WALL_Zpos_RIGHT <<endl;
    WALL->AddNode(wallSensitiveVolumeV_left, 16, wallWALL_right_2);

    //
    top->AddNode(WALL, 0);
    top->SetVisContainers(kTRUE);

    // ---------------   Finish   ----------------------------------------------
    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();

    gGeoManager->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();

    // TString gdmlname = gPath + "/geometry/tpc_v7.gdml";
    // gGeoManager->Export(gdmlname);

    // top->Draw("ogl");
    // TGLViewer *v = (TGLViewer*)gPad->GetViewer3D();
    // v->SetStyle(TGLRnrCtx::kOutline);
    // top->Draw("");

}   // create_rootgeom_zdc_v1
