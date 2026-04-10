/*
 * Use this macro to create ROOT geometry of Vacuum Beam Pipe (section 1: before
 * target) for the RUN-8 configuration, according to the measurement scheme
 *
 * Author: Baranov D.
 * Created: 28.11.2022
 *
 *
 * WARNING: all units is in cm!!!
 *
 */

#include "TGeoMatrix.h"
#include "TGeoVolume.h"
#include "TString.h"

// Parameters of Vacuum Beam Pipe

const Int_t NModules = 20;

// Positions
const Double_t XModulePositions[NModules] = {
    +0.375, // [0] module compensator
    +0.298, // [1] module si-prof2
    +0.217, // [2] module si-tracker3
    +0.085, // [3] module bc2
    -0.019, // [4] module vc
    +0.012, // [5] module box7
    +0.0,   // [6] module bellows1
    -0.025, // [7] module si-tracker2
    +0.022, // [8] module box6
    +0.026, // [9] module box5
    +0.0,   // [10] module bellows2
    +0.169, // [11] module si-prof1
    +0.115, // [12] module si-tracker1
    +0.0,   // [13] module bellows3
    +0.102, // [14] module box4
    +0.122, // [15] module box3 long
    +0.028, // [16] module box2
    +0.005, // [17] module bc1
    +0.009, // [18] module box1
    +0.0    // [19] module bellows4
};
const Double_t YModulePositions[NModules] = {
    -0.06,  // [0] module compensator
    -0.049, // [1] module si-prof2
    -0.022, // [2] module si-tracker3
    -0.051, // [3] module bc2
    -0.042, // [4] module vc
    -0.025, // [5] module box7
    +0.0,   // [6] module bellows1
    -0.016, // [7] module si-tracker2
    +0.042, // [8] module box6
    -0.005, // [9] module box5
    +0.0,   // [10] module bellows2
    +0.003, // [11] module si-prof1
    -0.001, // [12] module si-tracker1
    +0.0,   // [13] module bellows3
    -0.1,   // [14] module box4
    -0.052, // [15] module box3 long
    -0.023, // [16] module box2
    -0.005, // [17] module bc1
    +0.014, // [18] module box1
    +0.0    // [19] module bellows4
};
const Double_t ZModulePositions[NModules] = {
    -43.298, // [0] module compensator
    -64.077, // [1] module si-prof2
    //-84.077,  // [2] module si-tracker3
    -84.378,  // [2] module si-tracker3
    -104.004, // [3] module bc2
    -124.018, // [4] module vc
    -144.139, // [5] module box7
    -163.632, // [6] module bellows1
    //-183.124, // [7] module si-tracker2
    -183.225, // [7] module si-tracker2
    -203.19,  // [8] module box6
    -223.33,  // [9] module box5
    -243.218, // [10] module bellows2
    -263.11,  // [11] module si-prof1
    //-283.106, // [12] module si-tracker1
    -283.427, // [12] module si-tracker1
    -302.645, // [13] module bellows3
    -322.184, // [14] module box4
    -362.235, // [15] module box3 long
    -402.357, // [16] module box2
    -422.322, // [17] module bc1
    -442.335, // [18] module box1
    -462.0    // [19] module bellows4
};

// Rotations of modules around Z (deg.)
const Double_t ZStationRotations[NModules] = {
    +0.0,    // [0] module compensator
    -0.059,  // [1] module si-prof2
    -59.949, // [2] module si-tracker3
    -0.602,  // [3] module bc2
    -89.858, // [4] module vc
    +0.0,    // [5] module box7
    +0.0,    // [6] module bellows1
    -29.819, // [7] module si-tracker2
    +0.0,    // [8] module box6
    +0.0,    // [9] module box5
    +0.0,    // [10] module bellows2
    -0.227,  // [11] module si-prof1
    -0.028,  // [12] module si-tracker1
    +0.0,    // [13] module bellows3
    +0.0,    // [14] module box4
    +0.0,    // [15] module box3 long
    +0.0,    // [16] module box2
    -89.902, // [17] module bc1
    +0.0,    // [18] module box1
    +0.0     // [19] module bellows4
};

//------------------------------------------------------------------------------

// GeoManager
TGeoManager *gGeoMan = NULL;

// media
TGeoMedium *pMedVacuum = 0;
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedAluminium = 0;
TGeoMedium *pMedSteel = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedSilicon = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateModule_Compensator(TString module_name);
TGeoVolume *CreateModule_Profilometer(TString module_name);
TGeoVolume *CreateModule_SiTracker(TString module_name);
TGeoVolume *CreateModule_Counter(TString module_name);
TGeoVolume *CreateModule_BareBox(TString module_name);
TGeoVolume *CreateModule_Bellows(TString module_name);

void DefineRequiredMedia(FairGeoMedia *geoMedia, FairGeoBuilder *geoBuild) {

  // vacuum medium
  FairGeoMedium *mVacuum = geoMedia->getMedium("vacuum");
  if (!mVacuum)
    Fatal("Main", "FairMedium vacuum not found");
  geoBuild->createMedium(mVacuum);
  pMedVacuum = gGeoManager->GetMedium("vacuum");
  if (!pMedVacuum)
    Fatal("Main", "Medium vacuum not found");

  // air medium
  FairGeoMedium *mAir = geoMedia->getMedium("air");
  if (!mAir)
    Fatal("Main", "FairMedium air not found");
  geoBuild->createMedium(mAir);
  pMedAir = gGeoManager->GetMedium("air");
  if (!pMedAir)
    Fatal("Main", "Medium air not found");

  // carbon medium
  FairGeoMedium *mCarbon = geoMedia->getMedium("carbon");
  if (!mCarbon)
    Fatal("Main", "FairMedium carbon not found");
  geoBuild->createMedium(mCarbon);
  pMedCarbon = gGeoManager->GetMedium("carbon");
  if (!pMedCarbon)
    Fatal("Main", "Medium carbon not found");

  // aluminium medium
  FairGeoMedium *mAluminium = geoMedia->getMedium("aluminium");
  if (!mAluminium)
    Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAluminium);
  pMedAluminium = gGeoManager->GetMedium("aluminium");
  if (!pMedAluminium)
    Fatal("Main", "Medium aluminium not found");

  // steel medium
  FairGeoMedium *mSteel = geoMedia->getMedium("steel");
  if (!mSteel)
    Fatal("Main", "FairMedium steel not found");
  geoBuild->createMedium(mSteel);
  pMedSteel = gGeoManager->GetMedium("steel");
  if (!pMedSteel)
    Fatal("Main", "Medium steel not found");

  // fiberglass medium
  FairGeoMedium *mFiberGlass = geoMedia->getMedium("fiberglass");
  if (!mFiberGlass)
    Fatal("Main", "FairMedium fiberglass not found");
  geoBuild->createMedium(mFiberGlass);
  pMedFiberGlass = gGeoManager->GetMedium("fiberglass");
  if (!pMedFiberGlass)
    Fatal("Main", "Medium fiberglass not found");

  // silicon medium
  FairGeoMedium *mSilicon = geoMedia->getMedium("silicon");
  if (!mSilicon)
    Fatal("Main", "FairMedium silicon not found");
  geoBuild->createMedium(mSilicon);
  pMedSilicon = gGeoManager->GetMedium("silicon");
  if (!pMedSilicon)
    Fatal("Main", "Medium silicon not found");
}

void create_rootgeom_VacuumPipe_section1() {

  // ----  set working directory  --------------------------------------------
  TString gPath = gSystem->Getenv("VMCWORKDIR");

  // -------   Geometry file name (output)   ----------------------------------
  const TString geoDetectorName = "VacuumPipe_section1";
  const TString geoDetectorVersion = "Run8";
  const TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" +
                              geoDetectorVersion + ".root";

  // ----  global geometry parameters  ---------------------------------------
  FairGeoLoader *geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();

  // -------   Load media from media file   ----------------------------------
  TString medFile = gPath + "/geometry/media.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  FairGeoMedia *geoMedia = geoFace->getMedia();
  FairGeoBuilder *geoBuild = geoLoad->getGeoBuilder();

  DefineRequiredMedia(geoMedia, geoBuild);
  // -------------------------------------------------------------------------

  // --------------   Create geometry and top volume  ------------------------
  gGeoMan = (TGeoManager *)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName(geoDetectorName + "_geom");
  TGeoVolume *top = new TGeoVolumeAssembly("TOP");
  top->SetMedium(pMedAir);
  gGeoMan->SetTopVolume(top);
  // gGeoMan->SetTopVisible(1);
  //  -------------------------------------------------------------------------

  // Define TOP Geometry
  TGeoVolume *VacuumPipe_section1 = new TGeoVolumeAssembly(geoDetectorName);
  VacuumPipe_section1->SetMedium(pMedVacuum);

  TGeoVolume *module_compensator =
      CreateModule_Compensator(TString("VacuumPipe_ModuleCompensator"));
  TGeoVolume *module_profilometer2 =
      CreateModule_Profilometer(TString("VacuumPipe_ModuleProfilometer2"));
  TGeoVolume *module_sitracker3 =
      CreateModule_SiTracker(TString("VacuumPipe_ModuleSiTracker3"));
  TGeoVolume *module_BC2 =
      CreateModule_Counter(TString("VacuumPipe_ModuleBC2"));
  TGeoVolume *module_VC = CreateModule_Counter(TString("VacuumPipe_ModuleVC"));

  TGeoVolume *module_box7 =
      CreateModule_BareBox(TString("VacuumPipe_ModuleBox7"));
  TGeoVolume *module_bellows1 =
      CreateModule_Bellows(TString("VacuumPipe_ModuleBellows1"));
  TGeoVolume *module_box6 =
      CreateModule_BareBox(TString("VacuumPipe_ModuleBox6"));
  TGeoVolume *module_box5 =
      CreateModule_Counter(TString("VacuumPipe_ModuleBox5"));
  TGeoVolume *module_bellows2 =
      CreateModule_Bellows(TString("VacuumPipe_ModuleBellows2"));

  TGeoVolume *module_sitracker2 =
      CreateModule_SiTracker(TString("VacuumPipe_ModuleSiTracker2"));
  TGeoVolume *module_profilometer1 =
      CreateModule_Profilometer(TString("VacuumPipe_ModuleProfilometer1"));
  TGeoVolume *module_sitracker1 =
      CreateModule_SiTracker(TString("VacuumPipe_ModuleSiTracker1"));
  TGeoVolume *module_bellows3 =
      CreateModule_Bellows(TString("VacuumPipe_ModuleBellows3"));
  TGeoVolume *module_box4 =
      CreateModule_Counter(TString("VacuumPipe_ModuleBox4"));

  TGeoVolume *module_box3_long =
      CreateModule_BareBox(TString("VacuumPipe_ModuleBox3_long"));
  TGeoVolume *module_box2 =
      CreateModule_Counter(TString("VacuumPipe_ModuleBox2"));
  TGeoVolume *module_BC1 =
      CreateModule_Counter(TString("VacuumPipe_ModuleBC1"));
  TGeoVolume *module_box1 =
      CreateModule_BareBox(TString("VacuumPipe_ModuleBox1"));
  TGeoVolume *module_bellows4 =
      CreateModule_Bellows(TString("VacuumPipe_ModuleBellows4"));

  TGeoCombiTrans *module_compensator_transform = new TGeoCombiTrans();
  module_compensator_transform->SetTranslation(
      XModulePositions[0], YModulePositions[0], ZModulePositions[0]);

  TGeoCombiTrans *module_profilometer2_transform = new TGeoCombiTrans();
  module_profilometer2_transform->RotateZ(ZStationRotations[1]);
  module_profilometer2_transform->SetTranslation(
      XModulePositions[1], YModulePositions[1], ZModulePositions[1]);

  TGeoCombiTrans *module_sitracker3_transform = new TGeoCombiTrans();
  module_sitracker3_transform->RotateZ(ZStationRotations[2]);
  module_sitracker3_transform->SetTranslation(
      XModulePositions[2], YModulePositions[2], ZModulePositions[2]);

  TGeoCombiTrans *module_BC2_transform = new TGeoCombiTrans();
  module_BC2_transform->RotateZ(ZStationRotations[3]);
  module_BC2_transform->SetTranslation(XModulePositions[3], YModulePositions[3],
                                       ZModulePositions[3]);

  TGeoCombiTrans *module_VC_transform = new TGeoCombiTrans();
  module_VC_transform->RotateZ(ZStationRotations[4]);
  module_VC_transform->SetTranslation(XModulePositions[4], YModulePositions[4],
                                      ZModulePositions[4]);

  TGeoCombiTrans *module_box7_transform = new TGeoCombiTrans();
  module_box7_transform->RotateZ(ZStationRotations[5]);
  module_box7_transform->SetTranslation(
      XModulePositions[5], YModulePositions[5], ZModulePositions[5]);

  TGeoCombiTrans *module_bellows1_transform = new TGeoCombiTrans();
  module_bellows1_transform->RotateZ(ZStationRotations[6]);
  module_bellows1_transform->SetTranslation(
      XModulePositions[6], YModulePositions[6], ZModulePositions[6]);

  TGeoCombiTrans *module_sitracker2_transform = new TGeoCombiTrans();
  module_sitracker2_transform->RotateZ(ZStationRotations[7]);
  module_sitracker2_transform->SetTranslation(
      XModulePositions[7], YModulePositions[7], ZModulePositions[7]);

  TGeoCombiTrans *module_box6_transform = new TGeoCombiTrans();
  module_box6_transform->RotateZ(ZStationRotations[8]);
  module_box6_transform->SetTranslation(
      XModulePositions[8], YModulePositions[8], ZModulePositions[8]);

  TGeoCombiTrans *module_box5_transform = new TGeoCombiTrans();
  module_box5_transform->RotateZ(ZStationRotations[9]);
  module_box5_transform->SetTranslation(
      XModulePositions[9], YModulePositions[9], ZModulePositions[9]);

  TGeoCombiTrans *module_bellows2_transform = new TGeoCombiTrans();
  module_bellows2_transform->RotateZ(ZStationRotations[10]);
  module_bellows2_transform->SetTranslation(
      XModulePositions[10], YModulePositions[10], ZModulePositions[10]);

  TGeoCombiTrans *module_profilometer1_transform = new TGeoCombiTrans();
  module_profilometer1_transform->RotateZ(ZStationRotations[11]);
  module_profilometer1_transform->SetTranslation(
      XModulePositions[11], YModulePositions[11], ZModulePositions[11]);

  TGeoCombiTrans *module_sitracker1_transform = new TGeoCombiTrans();
  module_sitracker1_transform->RotateZ(ZStationRotations[12]);
  module_sitracker1_transform->SetTranslation(
      XModulePositions[12], YModulePositions[12], ZModulePositions[12]);

  TGeoCombiTrans *module_bellows3_transform = new TGeoCombiTrans();
  module_bellows3_transform->RotateZ(ZStationRotations[13]);
  module_bellows3_transform->SetTranslation(
      XModulePositions[13], YModulePositions[13], ZModulePositions[13]);

  TGeoCombiTrans *module_box4_transform = new TGeoCombiTrans();
  module_box4_transform->RotateZ(ZStationRotations[14]);
  module_box4_transform->SetTranslation(
      XModulePositions[14], YModulePositions[14], ZModulePositions[14]);

  TGeoCombiTrans *module_box3_long_transform = new TGeoCombiTrans();
  module_box3_long_transform->RotateZ(ZStationRotations[15]);
  module_box3_long_transform->SetTranslation(
      XModulePositions[15], YModulePositions[15], ZModulePositions[15]);

  TGeoCombiTrans *module_box2_transform = new TGeoCombiTrans();
  module_box2_transform->RotateZ(ZStationRotations[16]);
  module_box2_transform->SetTranslation(
      XModulePositions[16], YModulePositions[16], ZModulePositions[16]);

  TGeoCombiTrans *module_BC1_transform = new TGeoCombiTrans();
  module_BC1_transform->RotateZ(ZStationRotations[17]);
  module_BC1_transform->SetTranslation(
      XModulePositions[17], YModulePositions[17], ZModulePositions[17]);

  TGeoCombiTrans *module_box1_transform = new TGeoCombiTrans();
  module_box1_transform->RotateZ(ZStationRotations[18]);
  module_box1_transform->SetTranslation(
      XModulePositions[18], YModulePositions[18], ZModulePositions[18]);

  TGeoCombiTrans *module_bellows4_transform = new TGeoCombiTrans();
  module_bellows4_transform->RotateZ(ZStationRotations[19]);
  module_bellows4_transform->SetTranslation(
      XModulePositions[19], YModulePositions[19], ZModulePositions[19]);

  VacuumPipe_section1->AddNode(module_compensator, 0,
                               module_compensator_transform);
  VacuumPipe_section1->AddNode(module_profilometer2, 0,
                               module_profilometer2_transform);
  VacuumPipe_section1->AddNode(module_sitracker3, 0,
                               module_sitracker3_transform);
  VacuumPipe_section1->AddNode(module_BC2, 0, module_BC2_transform);
  VacuumPipe_section1->AddNode(module_VC, 0, module_VC_transform);
  VacuumPipe_section1->AddNode(module_box7, 0, module_box7_transform);
  VacuumPipe_section1->AddNode(module_bellows1, 0, module_bellows1_transform);
  VacuumPipe_section1->AddNode(module_box6, 0, module_box6_transform);
  VacuumPipe_section1->AddNode(module_box5, 0, module_box5_transform);
  VacuumPipe_section1->AddNode(module_bellows2, 0, module_bellows2_transform);
  VacuumPipe_section1->AddNode(module_sitracker2, 0,
                               module_sitracker2_transform);
  VacuumPipe_section1->AddNode(module_profilometer1, 0,
                               module_profilometer1_transform);
  VacuumPipe_section1->AddNode(module_sitracker1, 0,
                               module_sitracker1_transform);
  VacuumPipe_section1->AddNode(module_bellows3, 0, module_bellows3_transform);
  VacuumPipe_section1->AddNode(module_box4, 0, module_box4_transform);
  VacuumPipe_section1->AddNode(module_box3_long, 0, module_box3_long_transform);
  VacuumPipe_section1->AddNode(module_box2, 0, module_box2_transform);
  VacuumPipe_section1->AddNode(module_BC1, 0, module_BC1_transform);
  VacuumPipe_section1->AddNode(module_box1, 0, module_box1_transform);
  VacuumPipe_section1->AddNode(module_bellows4, 0, module_bellows4_transform);

  top->AddNode(VacuumPipe_section1, 0);
  top->SetVisContainers(kTRUE);

  // ---------------   Finish   ----------------------------------------------
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.0001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();
  // gGeoMan->RestoreMasterVolume();

  TFile *geoFile = new TFile(geoFileName, "RECREATE");
  top->Write();
  geoFile->Close();
  top->Draw("ogl");
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule_Compensator(TString module_name) {

  Double_t length1 = 15.0;      // cm
  Double_t length2 = 4.6;       // cm
  Double_t length2_vac = 4.095; // cm

  Double_t innerRadius1 = 21.3 * 0.5;
  Double_t outerRadius1 = 22.3 * 0.5;
  Double_t flangeRadius1 = 24.0 * 0.5;

  Double_t innerRadius2 = 19.7 * 0.5;
  Double_t outerRadius2 = 21.1 * 0.5;
  Double_t flangeRadius2 = 24.0 * 0.5;

  Double_t flangeThickness = 1.2;

  // shapes
  TGeoShape *vacTube1S =
      new TGeoTube(TString("vacTube1S") += TString("_") + module_name,
                   innerRadius1, outerRadius1, length1 * 0.5);
  TGeoShape *vacTube2S =
      new TGeoTube(TString("vacTube2S") += TString("_") + module_name,
                   innerRadius2, outerRadius2, length2 * 0.5);
  TGeoShape *vacTube1VacuumS =
      new TGeoTube(TString("vacTube1VacuumS") += TString("_") + module_name,
                   0.0, innerRadius1, length1 * 0.5);
  TGeoShape *vacTube2VacuumS =
      new TGeoTube(TString("vacTube2VacuumS") += TString("_") + module_name,
                   0.0, innerRadius2, length2_vac * 0.5);
  TGeoShape *vacTubeFlange1S =
      new TGeoTube(TString("vacTubeFlange1S") += TString("_") + module_name,
                   outerRadius1, flangeRadius1, flangeThickness * 0.5);
  TGeoShape *vacTubeFlange2S =
      new TGeoTube(TString("vacTubeFlange2S") += TString("_") + module_name,
                   outerRadius2, flangeRadius2, flangeThickness * 0.5);

  // volumes
  TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
  TGeoVolume *vacTube1V = new TGeoVolume(
      TString("vacTube1V") += TString("_") + module_name, vacTube1S);
  TGeoVolume *vacTube2V = new TGeoVolume(
      TString("vacTube2V") += TString("_") + module_name, vacTube2S);
  TGeoVolume *vacTube1VacuumV =
      new TGeoVolume(TString("vacTube1VacuumV") += TString("_") + module_name,
                     vacTube1VacuumS);
  TGeoVolume *vacTube2VacuumV =
      new TGeoVolume(TString("vacTube2VacuumV") += TString("_") + module_name,
                     vacTube2VacuumS);
  TGeoVolume *vacTubeFlange1V =
      new TGeoVolume(TString("vacTubeFlange1V") += TString("_") + module_name,
                     vacTubeFlange1S);
  TGeoVolume *vacTubeFlange2V =
      new TGeoVolume(TString("vacTubeFlange2V") += TString("_") + module_name,
                     vacTubeFlange2S);

  // volume medium
  TGeoMedium *vacTubeV_medium = pMedAluminium;
  if (vacTubeV_medium) {
    vacTube1V->SetMedium(vacTubeV_medium);
    vacTube2V->SetMedium(vacTubeV_medium);
    vacTubeFlange1V->SetMedium(vacTubeV_medium);
    vacTubeFlange2V->SetMedium(vacTubeV_medium);
  } else
    Fatal("Main", "Invalid medium for vacTubeV!");

  TGeoMedium *vacTubeVacuum_medium = pMedVacuum;
  if (vacTubeVacuum_medium) {
    vacTube1VacuumV->SetMedium(vacTubeVacuum_medium);
    vacTube2VacuumV->SetMedium(vacTubeVacuum_medium);
  } else
    Fatal("Main", "Invalid medium for vacTubeV!");

  // volume visual property (transparency)
  vacTube1V->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTube1V->SetTransparency(0);

  vacTube2V->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTubeFlange1V->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTubeFlange2V->SetLineColor(TColor::GetColor("#ffcccc"));

  // part positions in the vacuum tube box
  TGeoCombiTrans *vacTube1V_transf[1];
  vacTube1V_transf[0] = new TGeoCombiTrans();
  vacTube1V_transf[0]->SetTranslation(
      0.0, 0.0, +length1 * 0.5 - (length1 + length2) * 0.5);

  TGeoCombiTrans *vacTube2V_transf[1];
  vacTube2V_transf[0] = new TGeoCombiTrans();
  vacTube2V_transf[0]->SetTranslation(
      0.0, 0.0, -length2 * 0.5 + (length1 + length2) * 0.5);

  TGeoCombiTrans *vacTubeVacuum2V_transf[1];
  vacTubeVacuum2V_transf[0] = new TGeoCombiTrans();
  vacTubeVacuum2V_transf[0]->SetTranslation(0.0, 0.0,
                                            -length2 * 0.5 +
                                                (length1 + length2) * 0.5 -
                                                (length2 - length2_vac) * 0.5);

  TGeoCombiTrans *vacTubeFlange1V_transf[1];
  vacTubeFlange1V_transf[0] = new TGeoCombiTrans();
  vacTubeFlange1V_transf[0]->SetTranslation(
      0.0, 0.0, -(length1 + length2) * 0.5 + flangeThickness * 0.5);
  TGeoCombiTrans *vacTubeFlange2V_transf[1];
  vacTubeFlange2V_transf[0] = new TGeoCombiTrans();
  vacTubeFlange2V_transf[0]->SetTranslation(
      0.0, 0.0, +(length1 + length2) * 0.5 - flangeThickness * 0.5);

  vacTubeBoxV->AddNode(vacTube1V, 0, vacTube1V_transf[0]);
  vacTubeBoxV->AddNode(vacTube2V, 0, vacTube2V_transf[0]);
  vacTubeBoxV->AddNode(vacTube1VacuumV, 0, vacTube1V_transf[0]);
  vacTubeBoxV->AddNode(vacTube2VacuumV, 0, vacTubeVacuum2V_transf[0]);
  vacTubeBoxV->AddNode(vacTubeFlange1V, 0, vacTubeFlange1V_transf[0]);
  vacTubeBoxV->AddNode(vacTubeFlange2V, 0, vacTubeFlange2V_transf[0]);

  return vacTubeBoxV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule_Profilometer(TString module_name) {

  Double_t length1 = 20.0; // cm
  Double_t length2 = 20.8; // cm

  Double_t innerRadius1 = 19.6 * 0.5;
  Double_t outerRadius1 = 20.0 * 0.5;
  Double_t flangeRadius1 = 24.0 * 0.5;

  Double_t innerRadius2 = 14.6 * 0.5;
  Double_t outerRadius2 = 15.0 * 0.5;
  Double_t flangeRadius2 = 18.0 * 0.5;

  Double_t flangeThickness = 1.2;

  // shapes
  TGeoShape *vacTubeS =
      new TGeoTube(TString("vacTubeS") += TString("_") + module_name,
                   innerRadius1, outerRadius1, length1 * 0.5);
  TGeoShape *vacTubeVacuumS =
      new TGeoTube(TString("vacTubeVacuumS") += TString("_") + module_name, 0.0,
                   innerRadius1, length1 * 0.5);
  TGeoShape *vacTubeVacuumGapS =
      new TGeoTube(TString("vacTubeVacuumGapS") += TString("_") + module_name,
                   0.0, innerRadius1, 0.018 * 0.5);
  TGeoShape *vacTubeHoleS =
      new TGeoTube(TString("vacTubeHoleS") += TString("_") + module_name, 0.0,
                   outerRadius2, length2 * 0.5);

  TGeoShape *sideTubeS =
      new TGeoTube(TString("sideTubeS") += TString("_") + module_name,
                   innerRadius2, outerRadius2, length2 * 0.5);
  TGeoShape *sideTubeCutS =
      new TGeoTube(TString("sideTubeCutS") += TString("_") + module_name, 0.0,
                   innerRadius1, length1 * 0.5);

  TGeoShape *vacTubeFlangeS =
      new TGeoTube(TString("vacTubeFlangeS") += TString("_") + module_name,
                   outerRadius1, flangeRadius1, flangeThickness * 0.5);
  TGeoShape *sideTubeFlangeS =
      new TGeoTube(TString("sideTubeFlangeS") += TString("_") + module_name,
                   outerRadius2, flangeRadius2, flangeThickness * 0.5);

  TGeoCombiTrans *vacTubeHole_transf = new TGeoCombiTrans();
  vacTubeHole_transf->SetName(TString("vacTubeHole_transf") +=
                              TString("_") + module_name);
  vacTubeHole_transf->RotateY(90.0);
  vacTubeHole_transf->SetDx(-length2 * 0.5);
  vacTubeHole_transf->SetDy(0.0);
  vacTubeHole_transf->SetDz(0.0);
  vacTubeHole_transf->RegisterYourself();

  TGeoCompositeShape *vacTubeWithHoleS = new TGeoCompositeShape();
  vacTubeWithHoleS->SetName(TString("vacTubeWithHoleS") +=
                            TString("_") + module_name);
  {
    TString expression = "vacTubeS";
    expression += TString("_") + module_name;
    expression += "-vacTubeHoleS";
    expression += TString("_") + module_name;
    expression += ":vacTubeHole_transf";
    expression += TString("_") + module_name;
    vacTubeWithHoleS->MakeNode(expression);
    vacTubeWithHoleS->ComputeBBox(); // need to compute a bounding box
  }

  TGeoCombiTrans *sideTube_transf = new TGeoCombiTrans();
  sideTube_transf->SetName(TString("sideTube_transf") +=
                           TString("_") + module_name);
  sideTube_transf->RotateY(90.0);
  sideTube_transf->SetTranslation(-length2 * 0.5, 0.0, 0.0);
  sideTube_transf->RegisterYourself();

  TGeoCompositeShape *sideTubeWithCutS = new TGeoCompositeShape();
  sideTubeWithCutS->SetName(TString("sideTubeWithCutS") +=
                            TString("_") + module_name);
  {
    TString expression = "sideTubeS";
    expression += TString("_") + module_name;
    expression += ":sideTube_transf";
    expression += TString("_") + module_name;
    expression += "-sideTubeCutS";
    expression += TString("_") + module_name;
    sideTubeWithCutS->MakeNode(expression);
    sideTubeWithCutS->ComputeBBox(); // need to compute a bounding box
  }

  TGeoCombiTrans *vacTubeVacuumGap_transf = new TGeoCombiTrans();
  vacTubeVacuumGap_transf->SetName(TString("vacTubeVacuumGap_transf") +=
                                   TString("_") + module_name);
  vacTubeVacuumGap_transf->SetTranslation(0.0, 0.0, +0.018 * 0.5);
  vacTubeVacuumGap_transf->RegisterYourself();

  TGeoCompositeShape *vacTubeVacuumWithGapS = new TGeoCompositeShape();
  vacTubeVacuumWithGapS->SetName(TString("vacTubeVacuumWithGapS") +=
                                 TString("_") + module_name);
  {
    TString expression = "vacTubeVacuumS";
    expression += TString("_") + module_name;
    expression += "-vacTubeVacuumGapS";
    expression += TString("_") + module_name;
    expression += ":vacTubeVacuumGap_transf";
    expression += TString("_") + module_name;
    vacTubeVacuumWithGapS->MakeNode(expression);
    vacTubeVacuumWithGapS->ComputeBBox(); // need to compute a bounding box
  }

  // volumes
  // frame volumes
  TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
  TGeoVolume *vacTubeWithHoleV =
      new TGeoVolume(TString("vacTubeWithHoleV") += TString("_") + module_name,
                     vacTubeWithHoleS);
  TGeoVolume *sideTubeWithCutV =
      new TGeoVolume(TString("sideTubeWithCutV") += TString("_") + module_name,
                     sideTubeWithCutS);
  TGeoVolume *vacTubeVacuumV =
      new TGeoVolume(TString("vacTubeVacuumV") += TString("_") + module_name,
                     vacTubeVacuumWithGapS);
  TGeoVolume *vacTubeFlangeV = new TGeoVolume(
      TString("vacTubeFlangeV") += TString("_") + module_name, vacTubeFlangeS);
  TGeoVolume *sideTubeFlangeV =
      new TGeoVolume(TString("sideTubeFlangeV") += TString("_") + module_name,
                     sideTubeFlangeS);

  // volume medium
  TGeoMedium *vacTubeWithHoleV_medium = nullptr;
  if (module_name.Contains("Profilometer1"))
    vacTubeWithHoleV_medium = pMedSteel;
  if (module_name.Contains("Profilometer2"))
    vacTubeWithHoleV_medium = pMedAluminium;
  if (vacTubeWithHoleV_medium) {
    vacTubeWithHoleV->SetMedium(vacTubeWithHoleV_medium);
    sideTubeWithCutV->SetMedium(vacTubeWithHoleV_medium);
    vacTubeFlangeV->SetMedium(vacTubeWithHoleV_medium);
    sideTubeFlangeV->SetMedium(vacTubeWithHoleV_medium);
  } else
    Fatal("Main", "Invalid medium for vacTubeWithHoleV!");

  vacTubeVacuumV->SetMedium(pMedVacuum);

  // volume visual property (transparency)
  vacTubeWithHoleV->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTubeWithHoleV->SetTransparency(0);
  sideTubeWithCutV->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));
  sideTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));

  // part positions in the vacuum tube box
  TGeoCombiTrans *vacTubeFlangeV_transf[2];

  vacTubeFlangeV_transf[0] = new TGeoCombiTrans();
  vacTubeFlangeV_transf[0]->SetTranslation(
      0.0, 0.0, -length1 * 0.5 + flangeThickness * 0.5);

  vacTubeFlangeV_transf[1] = new TGeoCombiTrans();
  vacTubeFlangeV_transf[1]->SetTranslation(
      0.0, 0.0, +length1 * 0.5 - flangeThickness * 0.5);

  TGeoCombiTrans *sideTubeFlangeV_transf[1];

  sideTubeFlangeV_transf[0] = new TGeoCombiTrans();
  sideTubeFlangeV_transf[0]->RotateY(90.0);
  sideTubeFlangeV_transf[0]->SetTranslation(-length2 + flangeThickness * 0.5,
                                            0.0, 0.0);

  vacTubeBoxV->AddNode(vacTubeWithHoleV, 0, new TGeoCombiTrans());
  vacTubeBoxV->AddNode(sideTubeWithCutV, 0, new TGeoCombiTrans());
  vacTubeBoxV->AddNode(vacTubeVacuumV, 0, new TGeoCombiTrans());
  vacTubeBoxV->AddNode(vacTubeFlangeV, 0, vacTubeFlangeV_transf[0]);
  vacTubeBoxV->AddNode(vacTubeFlangeV, 1, vacTubeFlangeV_transf[1]);
  vacTubeBoxV->AddNode(sideTubeFlangeV, 0, sideTubeFlangeV_transf[0]);

  return vacTubeBoxV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule_SiTracker(TString module_name) {

  Double_t length1 = 19.6; // cm
  Double_t length2 = 15.0; // cm

  Double_t innerRadius1 = 21.3 * 0.5;
  Double_t outerRadius1 = 21.9 * 0.5;
  Double_t flangeRadius1 = 24.0 * 0.5;

  Double_t innerRadius2 = 15.3 * 0.5;
  Double_t outerRadius2 = 15.9 * 0.5;
  Double_t flangeRadius2 = 18.0 * 0.5;

  Double_t flangeThickness = 1.2;

  // shapes
  TGeoShape *vacTubeS =
      new TGeoTube(TString("vacTubeS") += TString("_") + module_name,
                   innerRadius1, outerRadius1, length1 * 0.5);
  TGeoShape *vacTubeHoleS =
      new TGeoTube(TString("vacTubeHoleS") += TString("_") + module_name, 0.0,
                   outerRadius2, length2 * 0.5);
  TGeoShape *vacTubeVacuumS =
      new TGeoTube(TString("vacTubeVacuumS") += TString("_") + module_name, 0.0,
                   innerRadius1, length1 * 0.5);
  TGeoShape *vacTubeVacuumGapS =
      new TGeoTube(TString("vacTubeVacuumGapS") += TString("_") + module_name,
                   0.0, innerRadius1, 0.018 * 0.5);

  TGeoShape *sideTubeS =
      new TGeoTube(TString("sideTubeS") += TString("_") + module_name,
                   innerRadius2, outerRadius2, length2 * 0.5);
  TGeoShape *sideTubeCutS =
      new TGeoTube(TString("sideTubeCutS") += TString("_") + module_name, 0.0,
                   innerRadius1, length1 * 0.5);

  TGeoShape *vacTubeFlangeS =
      new TGeoTube(TString("vacTubeFlangeS") += TString("_") + module_name,
                   outerRadius1, flangeRadius1, flangeThickness * 0.5);
  TGeoShape *sideTubeFlangeS =
      new TGeoTube(TString("sideTubeFlangeS") += TString("_") + module_name,
                   outerRadius2, flangeRadius2, flangeThickness * 0.5);

  TGeoCombiTrans *vacTubeHole_transf = new TGeoCombiTrans();
  vacTubeHole_transf->SetName(TString("vacTubeHole_transf") +=
                              TString("_") + module_name);
  vacTubeHole_transf->RotateY(90.0);
  vacTubeHole_transf->SetDx(-length2);
  vacTubeHole_transf->SetDy(0.0);
  vacTubeHole_transf->SetDz(0.0);
  vacTubeHole_transf->RegisterYourself();

  TGeoCompositeShape *vacTubeWithHoleS = new TGeoCompositeShape();
  vacTubeWithHoleS->SetName(TString("vacTubeWithHoleS") +=
                            TString("_") + module_name);
  {
    TString expression = "vacTubeS";
    expression += TString("_") + module_name;
    expression += "-vacTubeHoleS";
    expression += TString("_") + module_name;
    expression += ":vacTubeHole_transf";
    expression += TString("_") + module_name;
    vacTubeWithHoleS->MakeNode(expression);
    vacTubeWithHoleS->ComputeBBox(); // need to compute a bounding box
  }

  TGeoCombiTrans *sideTube_transf = new TGeoCombiTrans();
  sideTube_transf->SetName(TString("sideTube_transf") +=
                           TString("_") + module_name);
  sideTube_transf->RotateY(90.0);
  sideTube_transf->SetTranslation(-length2 * 0.5, 0.0, 0.0);
  sideTube_transf->RegisterYourself();

  TGeoCompositeShape *sideTubeWithCutS = new TGeoCompositeShape();
  sideTubeWithCutS->SetName(TString("sideTubeWithCutS") +=
                            TString("_") + module_name);
  {
    TString expression = "sideTubeS";
    expression += TString("_") + module_name;
    expression += ":sideTube_transf";
    expression += TString("_") + module_name;
    expression += "-sideTubeCutS";
    expression += TString("_") + module_name;
    sideTubeWithCutS->MakeNode(expression);
    sideTubeWithCutS->ComputeBBox(); // need to compute a bounding box
  }

  TGeoCombiTrans *vacTubeVacuumGap_transf = new TGeoCombiTrans();
  vacTubeVacuumGap_transf->SetName(TString("vacTubeVacuumGap_transf") +=
                                   TString("_") + module_name);
  vacTubeVacuumGap_transf->SetTranslation(0.0, 0.0, +0.018 * 0.5);
  vacTubeVacuumGap_transf->RegisterYourself();

  TGeoCompositeShape *vacTubeVacuumWithGapS = new TGeoCompositeShape();
  vacTubeVacuumWithGapS->SetName(TString("vacTubeVacuumWithGapS") +=
                                 TString("_") + module_name);
  {
    TString expression = "vacTubeVacuumS";
    expression += TString("_") + module_name;
    expression += "-vacTubeVacuumGapS";
    expression += TString("_") + module_name;
    expression += ":vacTubeVacuumGap_transf";
    expression += TString("_") + module_name;
    vacTubeVacuumWithGapS->MakeNode(expression);
    vacTubeVacuumWithGapS->ComputeBBox(); // need to compute a bounding box
  }

  // volumes
  // frame volumes
  TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
  TGeoVolume *vacTubeWithHoleV =
      new TGeoVolume(TString("vacTubeWithHoleV") += TString("_") + module_name,
                     vacTubeWithHoleS);
  TGeoVolume *vacTubeVaccumV =
      new TGeoVolume(TString("vacTubeVaccumV") += TString("_") + module_name,
                     vacTubeVacuumWithGapS);
  TGeoVolume *sideTubeWithCutV =
      new TGeoVolume(TString("sideTubeWithCutV") += TString("_") + module_name,
                     sideTubeWithCutS);
  TGeoVolume *vacTubeFlangeV = new TGeoVolume(
      TString("vacTubeFlangeV") += TString("_") + module_name, vacTubeFlangeS);
  TGeoVolume *sideTubeFlangeV =
      new TGeoVolume(TString("sideTubeFlangeV") += TString("_") + module_name,
                     sideTubeFlangeS);

  // volume medium
  TGeoMedium *vacTubeWithHoleV_medium = nullptr;
  if (module_name.Contains("SiTracker1"))
    vacTubeWithHoleV_medium = pMedSteel;
  if (module_name.Contains("SiTracker2"))
    vacTubeWithHoleV_medium = pMedSteel;
  if (module_name.Contains("SiTracker3"))
    vacTubeWithHoleV_medium = pMedAluminium;
  if (vacTubeWithHoleV_medium) {
    vacTubeWithHoleV->SetMedium(vacTubeWithHoleV_medium);
    sideTubeWithCutV->SetMedium(vacTubeWithHoleV_medium);
    vacTubeFlangeV->SetMedium(vacTubeWithHoleV_medium);
    sideTubeFlangeV->SetMedium(vacTubeWithHoleV_medium);
  } else
    Fatal("Main", "Invalid medium for vacTubeWithHoleV!");

  vacTubeVaccumV->SetMedium(pMedVacuum);

  // volume visual property (transparency)
  vacTubeWithHoleV->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTubeWithHoleV->SetTransparency(0);
  sideTubeWithCutV->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));
  sideTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));

  // part positions in the vacuum tube box
  TGeoCombiTrans *vacTubeFlangeV_transf[2];

  vacTubeFlangeV_transf[0] = new TGeoCombiTrans();
  vacTubeFlangeV_transf[0]->SetTranslation(
      0.0, 0.0, -length1 * 0.5 + flangeThickness * 0.5);

  vacTubeFlangeV_transf[1] = new TGeoCombiTrans();
  vacTubeFlangeV_transf[1]->SetTranslation(
      0.0, 0.0, +length1 * 0.5 - flangeThickness * 0.5);

  TGeoCombiTrans *sideTubeFlangeV_transf[1];

  sideTubeFlangeV_transf[0] = new TGeoCombiTrans();
  sideTubeFlangeV_transf[0]->RotateY(90.0);
  sideTubeFlangeV_transf[0]->SetTranslation(-length2 + flangeThickness * 0.5,
                                            0.0, 0.0);

  vacTubeBoxV->AddNode(vacTubeWithHoleV, 0, new TGeoCombiTrans());
  vacTubeBoxV->AddNode(vacTubeVaccumV, 0, new TGeoCombiTrans());
  vacTubeBoxV->AddNode(sideTubeWithCutV, 0, new TGeoCombiTrans());
  vacTubeBoxV->AddNode(vacTubeFlangeV, 0, vacTubeFlangeV_transf[0]);
  vacTubeBoxV->AddNode(vacTubeFlangeV, 1, vacTubeFlangeV_transf[1]);
  vacTubeBoxV->AddNode(sideTubeFlangeV, 0, sideTubeFlangeV_transf[0]);

  return vacTubeBoxV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule_Counter(TString module_name) {

  Double_t length1 = 19.6; // cm
  Double_t length2 = 15.0; // cm

  Double_t innerRadius1 = 21.3 * 0.5;
  Double_t outerRadius1 = 21.9 * 0.5;
  Double_t flangeRadius1 = 24.0 * 0.5;

  Double_t innerRadius2 = 9.56 * 0.5;
  Double_t outerRadius2 = 10.2 * 0.5;
  Double_t flangeRadius2 = 13.0 * 0.5;

  Double_t flangeThickness = 1.2;

  // shapes
  TGeoShape *vacTubeS =
      new TGeoTube(TString("vacTubeS") += TString("_") + module_name,
                   innerRadius1, outerRadius1, length1 * 0.5);
  TGeoShape *vacTubeHoleS =
      new TGeoTube(TString("vacTubeHoleS") += TString("_") + module_name, 0.0,
                   outerRadius2, length2 * 0.5);
  TGeoShape *vacTubeVacuumS =
      new TGeoTube(TString("vacTubeVacuumS") += TString("_") + module_name, 0.0,
                   innerRadius1, length1 * 0.5);

  TGeoShape *sideTubeS =
      new TGeoTube(TString("sideTubeS") += TString("_") + module_name,
                   innerRadius2, outerRadius2, length2 * 0.5);
  TGeoShape *sideTubeCutS =
      new TGeoTube(TString("sideTubeCutS") += TString("_") + module_name, 0.0,
                   innerRadius1, length1 * 0.5);

  TGeoShape *vacTubeFlangeS =
      new TGeoTube(TString("vacTubeFlangeS") += TString("_") + module_name,
                   outerRadius1, flangeRadius1, flangeThickness * 0.5);
  TGeoShape *sideTubeFlangeS =
      new TGeoTube(TString("sideTubeFlangeS") += TString("_") + module_name,
                   outerRadius2, flangeRadius2, flangeThickness * 0.5);

  TGeoCombiTrans *vacTubeHole_transf1 = new TGeoCombiTrans();
  vacTubeHole_transf1->SetName(TString("vacTubeHole_transf1") +=
                               TString("_") + module_name);
  vacTubeHole_transf1->RotateY(90.0);
  vacTubeHole_transf1->SetDx(-length2);
  vacTubeHole_transf1->SetDy(0.0);
  vacTubeHole_transf1->SetDz(0.0);
  vacTubeHole_transf1->RegisterYourself();

  TGeoCombiTrans *vacTubeHole_transf2 = new TGeoCombiTrans();
  vacTubeHole_transf2->SetName(TString("vacTubeHole_transf2") +=
                               TString("_") + module_name);
  vacTubeHole_transf2->RotateY(90.0);
  vacTubeHole_transf2->SetDx(+length2);
  vacTubeHole_transf2->SetDy(0.0);
  vacTubeHole_transf2->SetDz(0.0);
  vacTubeHole_transf2->RegisterYourself();

  TGeoCombiTrans *vacTubeHole_transf3 = new TGeoCombiTrans();
  vacTubeHole_transf3->SetName(TString("vacTubeHole_transf3") +=
                               TString("_") + module_name);
  vacTubeHole_transf3->RotateY(90.0);
  vacTubeHole_transf3->RotateZ(90.0);
  vacTubeHole_transf3->SetDx(0.0);
  vacTubeHole_transf3->SetDy(+length2);
  vacTubeHole_transf3->SetDz(0.0);
  vacTubeHole_transf3->RegisterYourself();

  TGeoCombiTrans *vacTubeHole_transf4 = new TGeoCombiTrans();
  vacTubeHole_transf4->SetName(TString("vacTubeHole_transf4") +=
                               TString("_") + module_name);
  vacTubeHole_transf4->RotateY(90.0);
  vacTubeHole_transf4->RotateZ(90.0);
  vacTubeHole_transf4->SetDx(0.0);
  vacTubeHole_transf4->SetDy(-length2);
  vacTubeHole_transf4->SetDz(0.0);
  vacTubeHole_transf4->RegisterYourself();

  TGeoCompositeShape *vacTubeWithHoleS = new TGeoCompositeShape();
  vacTubeWithHoleS->SetName(TString("vacTubeWithHoleS") +=
                            TString("_") + module_name);
  {
    TString expression = "vacTubeS";
    expression += TString("_") + module_name;
    expression += "-vacTubeHoleS";
    expression += TString("_") + module_name;
    expression += ":vacTubeHole_transf1";
    expression += TString("_") + module_name;
    expression += "-vacTubeHoleS";
    expression += TString("_") + module_name;
    expression += ":vacTubeHole_transf2";
    expression += TString("_") + module_name;
    expression += "-vacTubeHoleS";
    expression += TString("_") + module_name;
    expression += ":vacTubeHole_transf3";
    expression += TString("_") + module_name;
    expression += "-vacTubeHoleS";
    expression += TString("_") + module_name;
    expression += ":vacTubeHole_transf4";
    expression += TString("_") + module_name;
    vacTubeWithHoleS->MakeNode(expression);
    vacTubeWithHoleS->ComputeBBox(); // need to compute a bounding box
  }

  TGeoCombiTrans *sideTube_transf = new TGeoCombiTrans();
  sideTube_transf->SetName(TString("sideTube_transf") +=
                           TString("_") + module_name);
  sideTube_transf->RotateY(90.0);
  sideTube_transf->SetTranslation(-length2 * 0.5, 0.0, 0.0);
  sideTube_transf->RegisterYourself();

  TGeoCompositeShape *sideTubeWithCutS = new TGeoCompositeShape();
  sideTubeWithCutS->SetName(TString("sideTubeWithCutS") +=
                            TString("_") + module_name);
  {
    TString expression = "sideTubeS";
    expression += TString("_") + module_name;
    expression += ":sideTube_transf";
    expression += TString("_") + module_name;
    expression += "-sideTubeCutS";
    expression += TString("_") + module_name;
    sideTubeWithCutS->MakeNode(expression);
    sideTubeWithCutS->ComputeBBox(); // need to compute a bounding box
  }

  // volumes
  // frame volumes
  TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
  TGeoVolume *vacTubeWithHoleV =
      new TGeoVolume(TString("vacTubeWithHoleV") += TString("_") + module_name,
                     vacTubeWithHoleS);
  TGeoVolume *vacTubeVacuumV = new TGeoVolume(
      TString("vacTubeVacuumV") += TString("_") + module_name, vacTubeVacuumS);
  TGeoVolume *sideTubeWithCutV =
      new TGeoVolume(TString("sideTubeWithCutV") += TString("_") + module_name,
                     sideTubeWithCutS);
  TGeoVolume *vacTubeFlangeV = new TGeoVolume(
      TString("vacTubeFlangeV") += TString("_") + module_name, vacTubeFlangeS);
  TGeoVolume *sideTubeFlangeV =
      new TGeoVolume(TString("sideTubeFlangeV") += TString("_") + module_name,
                     sideTubeFlangeS);

  // volume medium
  TGeoMedium *vacTubeWithHoleV_medium = nullptr;
  if (module_name.Contains("VC"))
    vacTubeWithHoleV_medium = pMedSteel;
  if (module_name.Contains("BC1"))
    vacTubeWithHoleV_medium = pMedAluminium;
  if (module_name.Contains("BC2"))
    vacTubeWithHoleV_medium = pMedAluminium;
  if (module_name.Contains("Box"))
    vacTubeWithHoleV_medium = pMedSteel;
  if (vacTubeWithHoleV_medium) {
    vacTubeWithHoleV->SetMedium(vacTubeWithHoleV_medium);
    sideTubeWithCutV->SetMedium(vacTubeWithHoleV_medium);
    vacTubeFlangeV->SetMedium(vacTubeWithHoleV_medium);
    sideTubeFlangeV->SetMedium(vacTubeWithHoleV_medium);
  } else
    Fatal("Main", "Invalid medium for vacTubeWithHoleV!");

  vacTubeVacuumV->SetMedium(pMedVacuum);

  // volume visual property (transparency)
  vacTubeWithHoleV->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTubeWithHoleV->SetTransparency(0);
  sideTubeWithCutV->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));
  sideTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));

  // part positions in the vacuum tube box
  TGeoCombiTrans *sideTubeWithCutV_transf[4];
  sideTubeWithCutV_transf[0] = new TGeoCombiTrans();
  sideTubeWithCutV_transf[0]->SetTranslation(0.0, 0.0, 0.0);
  sideTubeWithCutV_transf[1] = new TGeoCombiTrans();
  sideTubeWithCutV_transf[1]->RotateZ(180.0);
  sideTubeWithCutV_transf[1]->SetTranslation(0.0, 0.0, 0.0);
  sideTubeWithCutV_transf[2] = new TGeoCombiTrans();
  sideTubeWithCutV_transf[2]->RotateZ(90.0);
  sideTubeWithCutV_transf[2]->SetTranslation(0.0, 0.0, 0.0);
  sideTubeWithCutV_transf[3] = new TGeoCombiTrans();
  sideTubeWithCutV_transf[3]->RotateZ(-90.0);
  sideTubeWithCutV_transf[3]->SetTranslation(0.0, 0.0, 0.0);

  TGeoCombiTrans *vacTubeFlangeV_transf[2];
  vacTubeFlangeV_transf[0] = new TGeoCombiTrans();
  vacTubeFlangeV_transf[0]->SetTranslation(
      0.0, 0.0, -length1 * 0.5 + flangeThickness * 0.5);
  vacTubeFlangeV_transf[1] = new TGeoCombiTrans();
  vacTubeFlangeV_transf[1]->SetTranslation(
      0.0, 0.0, +length1 * 0.5 - flangeThickness * 0.5);

  TGeoCombiTrans *sideTubeFlangeV_transf[4];
  sideTubeFlangeV_transf[0] = new TGeoCombiTrans();
  sideTubeFlangeV_transf[0]->RotateY(90.0);
  sideTubeFlangeV_transf[0]->SetTranslation(-length2 + flangeThickness * 0.5,
                                            0.0, 0.0);
  sideTubeFlangeV_transf[1] = new TGeoCombiTrans();
  sideTubeFlangeV_transf[1]->RotateY(90.0);
  sideTubeFlangeV_transf[1]->SetTranslation(+length2 - flangeThickness * 0.5,
                                            0.0, 0.0);
  sideTubeFlangeV_transf[2] = new TGeoCombiTrans();
  sideTubeFlangeV_transf[2]->RotateY(90.0);
  sideTubeFlangeV_transf[2]->SetTranslation(-length2 + flangeThickness * 0.5,
                                            0.0, 0.0);
  sideTubeFlangeV_transf[2]->RotateZ(90.0);
  sideTubeFlangeV_transf[3] = new TGeoCombiTrans();
  sideTubeFlangeV_transf[3]->RotateY(90.0);
  sideTubeFlangeV_transf[3]->SetTranslation(-length2 + flangeThickness * 0.5,
                                            0.0, 0.0);
  sideTubeFlangeV_transf[3]->RotateZ(-90.0);

  vacTubeBoxV->AddNode(vacTubeWithHoleV, 0, new TGeoCombiTrans());
  vacTubeBoxV->AddNode(vacTubeVacuumV, 0, new TGeoCombiTrans());

  vacTubeBoxV->AddNode(sideTubeWithCutV, 0, sideTubeWithCutV_transf[0]);
  vacTubeBoxV->AddNode(sideTubeWithCutV, 1, sideTubeWithCutV_transf[1]);
  vacTubeBoxV->AddNode(sideTubeWithCutV, 2, sideTubeWithCutV_transf[2]);
  vacTubeBoxV->AddNode(sideTubeWithCutV, 3, sideTubeWithCutV_transf[3]);

  vacTubeBoxV->AddNode(vacTubeFlangeV, 0, vacTubeFlangeV_transf[0]);
  vacTubeBoxV->AddNode(vacTubeFlangeV, 1, vacTubeFlangeV_transf[1]);

  vacTubeBoxV->AddNode(sideTubeFlangeV, 0, sideTubeFlangeV_transf[0]);
  vacTubeBoxV->AddNode(sideTubeFlangeV, 1, sideTubeFlangeV_transf[1]);
  vacTubeBoxV->AddNode(sideTubeFlangeV, 2, sideTubeFlangeV_transf[2]);
  vacTubeBoxV->AddNode(sideTubeFlangeV, 3, sideTubeFlangeV_transf[3]);

  return vacTubeBoxV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule_BareBox(TString module_name) {

  Double_t length = 20.0; // cm

  if (module_name.Contains("long"))
    length = 60.0;

  Double_t innerRadius = 19.6 * 0.5;
  Double_t outerRadius = 20.0 * 0.5;
  Double_t flangeRadius = 24.0 * 0.5;

  Double_t flangeThickness = 1.2;

  // shapes
  TGeoShape *vacTubeS =
      new TGeoTube(TString("vacTubeS") += TString("_") + module_name,
                   innerRadius, outerRadius, length * 0.5);
  TGeoShape *vacTubeVacuumS =
      new TGeoTube(TString("vacTubeVacuumS") += TString("_") + module_name, 0.0,
                   innerRadius, length * 0.5);
  TGeoShape *vacTubeFlangeS =
      new TGeoTube(TString("vacTubeFlangeS") += TString("_") + module_name,
                   outerRadius, flangeRadius, flangeThickness * 0.5);

  // volumes
  TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
  TGeoVolume *vacTubeV = new TGeoVolume(
      TString("vacTubeV") += TString("_") + module_name, vacTubeS);
  TGeoVolume *vacTubeVacuumV = new TGeoVolume(
      TString("vacTubeVacuumV") += TString("_") + module_name, vacTubeVacuumS);
  TGeoVolume *vacTubeFlangeV = new TGeoVolume(
      TString("vacTubeFlangeV") += TString("_") + module_name, vacTubeFlangeS);

  // volume medium
  TGeoMedium *vacTubeV_medium = pMedSteel;
  if (vacTubeV_medium) {
    vacTubeV->SetMedium(vacTubeV_medium);
    vacTubeFlangeV->SetMedium(vacTubeV_medium);
  } else
    Fatal("Main", "Invalid medium for vacTubeV!");

  vacTubeVacuumV->SetMedium(pMedVacuum);

  // volume visual property (transparency)
  vacTubeV->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTubeV->SetTransparency(0);
  vacTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));

  // part positions in the vacuum tube box
  TGeoCombiTrans *vacTubeV_transf[1];
  vacTubeV_transf[0] = new TGeoCombiTrans();
  vacTubeV_transf[0]->SetTranslation(0.0, 0.0, 0.0);

  TGeoCombiTrans *vacTubeFlangeV_transf[2];
  vacTubeFlangeV_transf[0] = new TGeoCombiTrans();
  vacTubeFlangeV_transf[0]->SetTranslation(
      0.0, 0.0, -length * 0.5 + flangeThickness * 0.5);
  vacTubeFlangeV_transf[1] = new TGeoCombiTrans();
  vacTubeFlangeV_transf[1]->SetTranslation(
      0.0, 0.0, +length * 0.5 - flangeThickness * 0.5);

  vacTubeBoxV->AddNode(vacTubeV, 0, vacTubeV_transf[0]);
  vacTubeBoxV->AddNode(vacTubeVacuumV, 0, vacTubeV_transf[0]);
  vacTubeBoxV->AddNode(vacTubeFlangeV, 0, vacTubeFlangeV_transf[0]);
  vacTubeBoxV->AddNode(vacTubeFlangeV, 1, vacTubeFlangeV_transf[1]);

  return vacTubeBoxV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule_Bellows(TString module_name) {

  Double_t length = 18.6; // cm

  Double_t innerRadius = 21.3 * 0.5;
  Double_t outerRadius = 21.9 * 0.5;
  Double_t flangeRadius = 24.0 * 0.5;

  Double_t flangeThickness = 1.2;

  // shapes
  TGeoShape *vacTubeS =
      new TGeoTube(TString("vacTubeS") += TString("_") + module_name,
                   innerRadius, outerRadius, length * 0.5);
  TGeoShape *vacTubeVacuumS =
      new TGeoTube(TString("vacTubeVacuumS") += TString("_") + module_name, 0.0,
                   innerRadius, length * 0.5);
  TGeoShape *vacTubeFlangeS =
      new TGeoTube(TString("vacTubeFlangeS") += TString("_") + module_name,
                   outerRadius, flangeRadius, flangeThickness * 0.5);

  // volumes
  TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
  TGeoVolume *vacTubeV = new TGeoVolume(
      TString("vacTubeV") += TString("_") + module_name, vacTubeS);
  TGeoVolume *vacTubeVacuumV = new TGeoVolume(
      TString("vacTubeVacuumV") += TString("_") + module_name, vacTubeVacuumS);
  TGeoVolume *vacTubeFlangeV = new TGeoVolume(
      TString("vacTubeFlangeV") += TString("_") + module_name, vacTubeFlangeS);

  // volume medium
  TGeoMedium *vacTubeV_medium = pMedSteel;
  if (vacTubeV_medium) {
    vacTubeV->SetMedium(vacTubeV_medium);
    vacTubeFlangeV->SetMedium(vacTubeV_medium);
  } else
    Fatal("Main", "Invalid medium for vacTubeV!");

  vacTubeVacuumV->SetMedium(pMedVacuum);

  // volume visual property (transparency)
  vacTubeV->SetLineColor(TColor::GetColor("#ffcccc"));
  vacTubeV->SetTransparency(0);
  vacTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));

  // part positions in the vacuum tube box
  TGeoCombiTrans *vacTubeV_transf[1];
  vacTubeV_transf[0] = new TGeoCombiTrans();
  vacTubeV_transf[0]->SetTranslation(0.0, 0.0, 0.0);

  TGeoCombiTrans *vacTubeFlangeV_transf[2];
  vacTubeFlangeV_transf[0] = new TGeoCombiTrans();
  vacTubeFlangeV_transf[0]->SetTranslation(
      0.0, 0.0, -length * 0.5 + flangeThickness * 0.5);
  vacTubeFlangeV_transf[1] = new TGeoCombiTrans();
  vacTubeFlangeV_transf[1]->SetTranslation(
      0.0, 0.0, +length * 0.5 - flangeThickness * 0.5);

  vacTubeBoxV->AddNode(vacTubeV, 0, vacTubeV_transf[0]);
  vacTubeBoxV->AddNode(vacTubeVacuumV, 0, vacTubeV_transf[0]);
  vacTubeBoxV->AddNode(vacTubeFlangeV, 0, vacTubeFlangeV_transf[0]);
  vacTubeBoxV->AddNode(vacTubeFlangeV, 1, vacTubeFlangeV_transf[1]);

  return vacTubeBoxV;
}