//---------------------------
// M.Golubeva (INR RAS), December 2016
//---------------------------

#include "../profiling/bmnloadlibs.C"
#include "TFile.h"
#include "TGeoManager.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"

// Detector's construct parameters ===================

// Double_t Zdc_X_size = 26.9; //58.8/2. //Frame for nDet 8layers
// Double_t Zdc_Y_size = 31.6215; //63.243/2.
// Double_t Zdc_Z_size = 30.875; //61.75/2. with veto

Double_t Zdc_X_size = 22;   // 44/2. //NO Frame for nDet
Double_t Zdc_Y_size = 22;   // 63.243/2.
// Double_t Zdc_Z_size = 49.9; //99.8/2. with veto 16 layers  noAbs 1Cu
// Double_t Zdc_Z_size = 28.2; //56.4/2. with veto 16 layers  noCu 1Cu
// Double_t Zdc_Z_size = 26.65; //53.3/2. with veto 16 layers  noCu
Double_t Zdc_Z_size = 24.8;   // 49.6/2. with veto 8 layers  with Cu

Double_t Zdc_X_size_NICA = 22;   // 44./2.
Double_t Zdc_Y_size_NICA = 22;   // 44./2.

// Modules NICA
Double_t Zdc_NColumns_NICA = 11;
Double_t Zdc_NRows_NICA = 11;

Double_t Zdc_module_X_size_NICA = 2;   // 4./2.
Double_t Zdc_module_Y_size_NICA = 2;   // 4./2.
// Double_t Zdc_module_Z_size_NICA = 24.1; //48.2/2.
// Double_t Zdc_module_Z_size_NICA = 49.9; //99.8/2.
// Double_t Zdc_module_Z_size_NICA = 28.2; //56.4/2.
// Double_t Zdc_module_Z_size_NICA = 26.65; //53.3/2.

Double_t Zdc_module_Z_size_NICA = 1.25;   // 2.5/2.

Double_t Zdc_NSlices_in_module = 8;   // Veto+7 slices
// Double_t Zdc_NSlices_in_module = 16;   //Veto+15 slices
// Double_t Zdc_NSlices_in_module = 2;   //test

// media ============================================
TGeoMedium* pMedVacuum = 0;
TGeoMedium* pMedAir = 0;
TGeoMedium* pMedIron = 0;
TGeoMedium* pMedLead = 0;
TGeoMedium* pMedTungsten = 0;
TGeoMedium* pMedScint = 0;
TGeoMedium* pMedTyvec = 0;
TGeoMedium* pMedSlot = 0;
TGeoMedium* pMedG10 = 0;
TGeoMedium* pMedCopper = 0;
TGeoMedium* pMedPLA = 0;
TGeoMedium* pMedAl = 0;

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

    // iron medium
    FairGeoMedium* mIron = geoMedia->getMedium("iron");
    if (!mIron)
        Fatal("Main", "FairMedium iron not found");
    geoBuild->createMedium(mIron);
    pMedIron = gGeoManager->GetMedium("iron");
    if (!pMedIron)
        Fatal("Main", "Medium iron not found");

    // aluminium medium
    FairGeoMedium* mAl = geoMedia->getMedium("aluminium");
    if (!mAl)
        Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAl);
    pMedAl = gGeoManager->GetMedium("aluminium");
    if (!pMedAl)
        Fatal("Main", "Medium aluminium not found");

    // tyvec medium
    FairGeoMedium* mTyvec = geoMedia->getMedium("tyvec_ZDC");
    if (!mTyvec)
        Fatal("Main", "FairMedium tyvec not found");
    geoBuild->createMedium(mTyvec);
    pMedTyvec = gGeoManager->GetMedium("tyvec_ZDC");
    if (!pMedTyvec)
        Fatal("Main", "Medium tyvec not found");

    // lead medium
    FairGeoMedium* mLead = geoMedia->getMedium("lead");
    if (!mLead)
        Fatal("Main", "FairMedium lead not found");
    geoBuild->createMedium(mLead);
    pMedLead = gGeoManager->GetMedium("lead");
    if (!pMedLead)
        Fatal("Main", "Medium lead not found");

    // tungten medium
    FairGeoMedium* mTungsten = geoMedia->getMedium("tungsten");
    if (!mTungsten)
        Fatal("Main", "FairMedium tungsten not found");
    geoBuild->createMedium(mTungsten);
    pMedTungsten = gGeoManager->GetMedium("tungsten");
    if (!pMedTungsten)
        Fatal("Main", "Medium tungsten not found");

    // medium polystyrene (Scint.)
    FairGeoMedium* mPolystyrene = geoMedia->getMedium("polystyrene");
    if (!mPolystyrene)
        Fatal("Main", "FairMedium polystyrene not found");
    geoBuild->createMedium(mPolystyrene);
    pMedScint = gGeoManager->GetMedium("polystyrene");
    if (!pMedScint)
        Fatal("Main", "Medium polystyrene not found");

    // medium plastic (slot)
    FairGeoMedium* mPlastic = geoMedia->getMedium("plasticWLS_ZDC");
    if (!mPlastic)
        Fatal("Main", "FairMedium plastic not found");
    geoBuild->createMedium(mPlastic);
    pMedSlot = gGeoManager->GetMedium("plasticWLS_ZDC");
    if (!pMedSlot)
        Fatal("Main", "Medium plastic not found");

    // medium G10 (PCB)
    FairGeoMedium* mG10 = geoMedia->getMedium("G10");
    if (!mG10)
        Fatal("Main", "FairMedium G10 not found");
    geoBuild->createMedium(mG10);
    pMedG10 = gGeoManager->GetMedium("G10");
    if (!pMedG10)
        Fatal("Main", "Medium G10 not found");

    // copper medium
    FairGeoMedium* mCopper = geoMedia->getMedium("copper");
    if (!mCopper)
        Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    pMedCopper = gGeoManager->GetMedium("copper");
    if (!pMedCopper)
        Fatal("Main", "Medium copper not found");

    // medium PLA (polylactic acid)
    FairGeoMedium* mPLA = geoMedia->getMedium("PLA");
    if (!mPLA)
        Fatal("Main", "FairMedium PLA not found");
    geoBuild->createMedium(mPLA);
    pMedPLA = gGeoManager->GetMedium("PLA");
    if (!pMedPLA)
        Fatal("Main", "Medium PLA not found");

}   // DefineRequiredMedia

// void FillModule_NICA_simple(TGeoVolume* zdcModuleV_NICA);

void create_rootgeom_nDet_noFrame_Planes_RNF_TWODET_44x44cm_121mods_4x4cm_with_rows_columns_NEW_NUMBERING_VETO_25mm_7slices_Cu_30mm_Sc_25mm_PCB_1mm_PLA_2mm_Al_1_5mm_Air_3mm(
    Double_t nDet_dist_1 = 730,
    Double_t rotY_1 = -10,
    Double_t rotX_1 = -4.5,
    Double_t nDet_dist_2 = 730,
    Double_t rotY_2 = -10,
    Double_t rotX_2 = 4.5)
{

    bmnloadlibs();   // load libraries

    // ----  set working directory  -------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   --------------------------------
    const TString geoDetectorName = "nDet_noFrame_Planes_RNF_TWODET_44x44cm_121mods_4x4cm_with_rows_columns_NEW_"
                                    "NUMBERING_VETO_25mm_7slices_Cu_30mm_Sc_25mm_PCB_1mm_PLA_2mm_Al_1_5mm_Air_3mm";

    // Det 1
    TString sDist_1_1 = "";
    TString sRotX_1_1 = "";
    TString sRotX_2_1 = "";

    TString sRotY_1_1 = "";
    TString sRotY_2_1 = "";

    Int_t rotX_1_1 = Int_t(rotX_1);   // 2
    // Int_t rotX_22_1 = Int_t(100*(rotX_1-Int_t(rotX_1)));//9 //+4.5deg
    Int_t rotX_22_1 = -Int_t(100 * (rotX_1 - Int_t(rotX_1)));   // 9  //-4.5deg
    // Int_t rotX_22_1 = -Int_t(100*(rotX_1-Int_t(rotX_1)))+1;//9

    Int_t rotY_1_1 = Int_t(rotY_1);   // 2
    // Int_t rotY_22_1 = Int_t(100*(rotY_1-Int_t(rotY_1)));//9
    Int_t rotY_22_1 = -Int_t(100 * (rotY_1 - Int_t(rotY_1)));   // 9

    sDist_1_1 += nDet_dist_1;
    sRotX_1_1 += rotX_1_1;
    sRotX_2_1 += rotX_22_1;
    sRotY_1_1 += rotY_1_1;
    sRotY_2_1 += rotY_22_1;

    // Det
    TString sDist_2_2 = "";
    TString sRotX_2_2 = "";
    TString sRotX_22_2 = "";

    TString sRotY_2_2 = "";
    TString sRotY_22_2 = "";

    Int_t rotX_2_2 = Int_t(rotX_2);                            // 2
    Int_t rotX_22_2 = Int_t(100 * (rotX_2 - Int_t(rotX_2)));   // 9    //+4.5
    // Int_t rotX_22_2 = -Int_t(100*(rotX_2-Int_t(rotX_2)));//9 //-4.5
    // Int_t rotX_22_2 = -Int_t(100*(rotX_2-Int_t(rotX_2)))+1;//9

    Int_t rotY_2_2 = Int_t(rotY_2);                            // 2
    Int_t rotY_22_2 = Int_t(100 * (rotY_2 - Int_t(rotY_2)));   // 9
    // Int_t rotY_22_2 = -Int_t(100*(rotY_2-Int_t(rotY_2)));//9

    sDist_2_2 += nDet_dist_2;
    sRotX_2_2 += rotX_2_2;
    sRotX_22_2 += rotX_22_2;
    sRotY_2_2 += rotY_2_2;
    sRotY_22_2 += rotY_22_2;

    TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "ZdistDET_1_" + sDist_1_1 + "cm_" + "rotY_"
                          + sRotY_1_1 + "." + sRotY_2_1 + "deg" + "_" + "rotX_" + sRotX_1_1 + "." + sRotX_2_1 + "deg"
                          + "_" + "ZdistDET_2_" + sDist_2_2 + "cm_" + "rotY_" + sRotY_2_2 + "." + sRotY_22_2 + "deg"
                          + "_" + "rotX_" + sRotX_2_2 + "." + sRotX_22_2 + "deg" + ".root";

    /*
  //Det 1
    TString sDist_1_1 = "";
    TString sRotX_1_1 = "";
    TString sRotX_2_1 = "";

    TString sRotY_1_1 = "";
    TString sRotY_2_1 = "";

    Int_t rotX_1_1 = Int_t(rotX_1); //2
    //Int_t rotX_22_1 = Int_t(100*(rotX_1-Int_t(rotX_1)));//9 //+4.5deg
    Int_t rotX_22_1 = -Int_t(100*(rotX_1-Int_t(rotX_1)));//9  //-4.5deg
    //Int_t rotX_22_1 = -Int_t(100*(rotX_1-Int_t(rotX_1)))+1;//9

    Int_t rotY_1_1 = Int_t(rotY_1); //2
    //Int_t rotY_22_1 = Int_t(100*(rotY_1-Int_t(rotY_1)));//9
    Int_t rotY_22_1 = -Int_t(100*(rotY_1-Int_t(rotY_1)));//9

    sDist_1_1 += nDet_dist_1;
    sRotX_1_1 +=rotX_1_1;
    sRotX_2_1 +=rotX_22_1;
    sRotY_1_1 +=rotY_1_1;
    sRotY_2_1 +=rotY_22_1;
    */
    /*
   //Det
     TString sDist_2_2 = "";
     TString sRotX_2_2 = "";
     TString sRotX_22_2 = "";

     TString sRotY_2_2 = "";
     TString sRotY_22_2 = "";

     Int_t rotX_2_2 = Int_t(rotX_2); //2
     Int_t rotX_22_2 = Int_t(100*(rotX_2-Int_t(rotX_2)));//9    //+4.5
     //Int_t rotX_22_2 = -Int_t(100*(rotX_2-Int_t(rotX_2)));//9 //-4.5
     //Int_t rotX_22_2 = -Int_t(100*(rotX_2-Int_t(rotX_2)))+1;//9

     Int_t rotY_2_2 = Int_t(rotY_2); //2
     //Int_t rotY_22_2 = Int_t(100*(rotY_2-Int_t(rotY_2)));//9
     Int_t rotY_22_2 = -Int_t(100*(rotY_2-Int_t(rotY_2)));//9

     sDist_2_2 += nDet_dist_2;
     sRotX_2_2 +=rotX_2_2;
     sRotX_22_2 +=rotX_22_2;
     sRotY_2_2 +=rotY_2_2;
     sRotY_22_2 +=rotY_22_2;
     */
    // TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zdist_" + sDist_1_1 + "cm_" + "rotY_"  +
    // sRotY_1_1  +  "." + sRotY_2_1 + "deg" + "_" + "rotX_"  + sRotX_1_1 + "." + sRotX_2_1 + "deg" + ".root";

    // ----  global geometry parameters  ---------------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();

    // -------   Load media from media file   ----------------------------------
    TString medFile = gPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    FairGeoMedia* geoMedia = geoFace->getMedia();
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

    // Define ZDC Geometry
    TGeoVolume* NDET_common = new TGeoVolumeAssembly(BmnNdetGeoPar::fCommonVolumeName);
    NDET_common->SetMedium(pMedAir);

    Double_t env_margin = 0.0001;
    TGeoBBox* envShape = new TGeoBBox(BmnNdetGeoPar::fEnvelopeVolumeName, Zdc_X_size + env_margin,
                                      Zdc_Y_size + env_margin, Zdc_Z_size + env_margin);
    TGeoVolume* NDET_envelope = new TGeoVolume(BmnNdetGeoPar::fEnvelopeVolumeName, envShape, pMedVacuum);

    TGeoBBox* ZDCS = new TGeoBBox(BmnNdetGeoPar::fDetectorVolumeName, Zdc_X_size, Zdc_Y_size, Zdc_Z_size);
    TGeoVolume* ZDCV = new TGeoVolume(BmnNdetGeoPar::fDetectorVolumeName, ZDCS, pMedAir);
    NDET_envelope->AddNode(ZDCV, 1, new TGeoTranslation(0., 0., 0.));

    // TGeoBBox *zdcModuleS_NICA[Int_t(Zdc_NRows_NICA*Zdc_NColumns_NICA)];
    // TGeoVolume *zdcModuleV_NICA[Int_t(Zdc_NRows_NICA*Zdc_NColumns_NICA)];
    TGeoBBox* zdcModuleS_NICA[Int_t(Zdc_NRows_NICA * Zdc_NColumns_NICA * Zdc_NSlices_in_module)];
    TGeoVolume* zdcModuleV_NICA[Int_t(Zdc_NRows_NICA * Zdc_NColumns_NICA * Zdc_NSlices_in_module)];

    Int_t rowNb = -10, columnNb = -10;
    Int_t iMod = -1, modNb = -1, modNb_addnode = -1;
    Int_t Zdc_nbCells_in_layer;
    Zdc_nbCells_in_layer = Int_t(Zdc_NRows_NICA * Zdc_NColumns_NICA);

    // Planes Cu, PCB, AL, PLA ===================
    // Double_t Zdc_Air_2mm = 0.2; //2mm Air arround 1Cu
    Double_t Zdc_Air_2mm = 0.;   // 0mm Air arround 1Cu
    // Double_t Zdc_Air_Z_size = 3.4; //Air before sample
    Double_t Zdc_Air_Z_size = 0.3;   // Air before sample //arround Cu

    // Scin. slices
    Double_t Zdc_Sc_X_size = 2;      // as Pb
    Double_t Zdc_Sc_Y_size = 2;      // as Pb
    Double_t Zdc_Sc_Z_size = 1.25;   // 2.5/2.

    Double_t Zdc_Veto_X_size = 2;      // as Pb
    Double_t Zdc_Veto_Y_size = 2;      // as Pb
    Double_t Zdc_Veto_Z_size = 1.25;   // 2.5/2.

    // Cu plane
    Double_t Zdc_Cu_X_size = 22;    // 44/2.
    Double_t Zdc_Cu_Y_size = 22;    // 44/2.
    Double_t Zdc_Cu_Z_size = 1.5;   // 3./2.
    // Double_t Zdc_Cu_Z_size = 0; //0./2.

    // Al slices
    Double_t Zdc_Al_X_size = 22;      // 44/2. //noCu
    Double_t Zdc_Al_Y_size = 22;      // 44/2.
    Double_t Zdc_Al_Z_size = 0.075;   // 0.15/2.

    // PCB //G10
    Double_t Zdc_G10_X_size = 22;     // 44/2
    Double_t Zdc_G10_Y_size = 22;     // 44/4
    Double_t Zdc_G10_Z_size = 0.05;   // 0.1/2.

    // PLA slices
    Double_t Zdc_PLA_X_size = 22;    // 44/2
    Double_t Zdc_PLA_Y_size = 22;    // 44/2.
    Double_t Zdc_PLA_Z_size = 0.1;   // 0.2/2.

    TGeoShape* CuS_NICA =
        new TGeoBBox(BmnNdetGeoPar::Cu_name_NICA, Zdc_Cu_X_size, Zdc_Cu_Y_size, Zdc_Cu_Z_size);   // Copper
    TGeoVolume* CuV_NICA = new TGeoVolume(BmnNdetGeoPar::Cu_name_NICA, CuS_NICA);
    CuV_NICA->SetLineColor(kMagenta);
    CuV_NICA->SetFillColor(kMagenta);
    TGeoMedium* Cu_medium_NICA = pMedCopper;   // set medium
    if (Cu_medium_NICA)
        CuV_NICA->SetMedium(Cu_medium_NICA);
    else
        Fatal("Main", "Invalid medium for Cu_NICA!");

    TGeoShape* AlS_NICA =
        new TGeoBBox(BmnNdetGeoPar::Al_name_NICA, Zdc_Al_X_size, Zdc_Al_Y_size, Zdc_Al_Z_size);   // Aluminium
    TGeoVolume* AlV_NICA = new TGeoVolume(BmnNdetGeoPar::Al_name_NICA, AlS_NICA);
    AlV_NICA->SetLineColor(kRed);   // light-blue
    AlV_NICA->SetFillColor(kRed);
    TGeoMedium* Al_medium_NICA = pMedAl;   // set medium
    if (Al_medium_NICA)
        AlV_NICA->SetMedium(Al_medium_NICA);
    else
        Fatal("Main", "Invalid medium for Al_NICA!");

    TGeoShape* G10S_NICA =
        new TGeoBBox(BmnNdetGeoPar::G10_name_NICA, Zdc_G10_X_size, Zdc_G10_Y_size, Zdc_G10_Z_size);   // PCB
    TGeoVolume* G10V_NICA = new TGeoVolume(BmnNdetGeoPar::G10_name_NICA, G10S_NICA);
    G10V_NICA->SetLineColor(kGreen);
    G10V_NICA->SetFillColor(kGreen);
    TGeoMedium* G10_medium_NICA = pMedG10;   // set medium
    if (G10_medium_NICA)
        G10V_NICA->SetMedium(G10_medium_NICA);
    else
        Fatal("Main", "Invalid medium for G10_NICA!");

    TGeoShape* PLAS_NICA =
        new TGeoBBox(BmnNdetGeoPar::PLA_name_NICA, Zdc_PLA_X_size, Zdc_PLA_Y_size, Zdc_PLA_Z_size);   // PLA
    TGeoVolume* PLAV_NICA = new TGeoVolume(BmnNdetGeoPar::PLA_name_NICA, PLAS_NICA);
    PLAV_NICA->SetLineColor(kYellow + 2);
    PLAV_NICA->SetFillColor(kYellow + 2);
    TGeoMedium* PLA_medium_NICA = pMedPLA;   // set medium
    if (PLA_medium_NICA)
        PLAV_NICA->SetMedium(PLA_medium_NICA);
    else
        Fatal("Main", "Invalid medium for PLA_NICA!");

    TGeoShape* zdcSensitiveVolumeS_NICA =
        new TGeoBBox(BmnNdetGeoPar::SensitiveVolume_name_NICA, Zdc_Sc_X_size, Zdc_Sc_Y_size, Zdc_Sc_Z_size);
    TGeoVolume* zdcSensitiveVolumeV_NICA =
        new TGeoVolume(BmnNdetGeoPar::SensitiveVolume_name_NICA, zdcSensitiveVolumeS_NICA);
    zdcSensitiveVolumeV_NICA->SetLineColor(kBlue);
    zdcSensitiveVolumeV_NICA->SetFillColor(kBlue);
    TGeoMedium* Sc_medium_NICA = pMedScint;   // set medium
    if (Sc_medium_NICA)
        zdcSensitiveVolumeV_NICA->SetMedium(Sc_medium_NICA);
    else
        Fatal("Main", "Invalid medium for Sc_NICA!");

    TGeoShape* zdcSensitiveVolumeS_VETO =
        new TGeoBBox(BmnNdetGeoPar::SensitiveVolume_name_VETO, Zdc_Veto_X_size, Zdc_Veto_Y_size, Zdc_Veto_Z_size);
    TGeoVolume* zdcSensitiveVolumeV_VETO =
        new TGeoVolume(BmnNdetGeoPar::SensitiveVolume_name_VETO, zdcSensitiveVolumeS_VETO);
    zdcSensitiveVolumeV_VETO->SetLineColor(kBlue + 3);
    zdcSensitiveVolumeV_VETO->SetFillColor(kBlue + 3);
    TGeoMedium* Sc_medium_VETO = pMedScint;   // set medium
    if (Sc_medium_VETO)
        zdcSensitiveVolumeV_VETO->SetMedium(Sc_medium_VETO);
    else
        Fatal("Main", "Invalid medium for VETO!");

    Double_t dist_before_sample;
    Double_t sample_Z_size =
        2. * (Zdc_G10_Z_size + Zdc_PLA_Z_size + Zdc_Sc_Z_size + Zdc_Al_Z_size + Zdc_G10_Z_size);   //=3.05 cm
    // Double_t sample_Veto_size = 2.*(Zdc_PLA_Z_size+Zdc_Sc_Z_size+Zdc_G10_Z_size+Zdc_PLA_Z_size); //=3.1
    Double_t itDelta = 0;
    Double_t itInit_Cu, itCur_Cu = 0;
    Double_t itInit_Sc, itCur_Sc = 0;
    Double_t itInit_G10_before, itCur_G10_before = 0;
    Double_t itInit_G10_after, itCur_G10_after = 0;
    Double_t itInit_Air, itCur_Air = 0;
    Double_t itInit_Al, itCur_Al = 0;
    Double_t itInit_Veto;
    Double_t itInit_PLA;

    // Double_t dist_to_nDet_surface = Zdc_Z_size - Zdc_module_Z_size_NICA;
    Double_t dist_to_nDet_surface = 0;

    for (Int_t it = 0; it < Zdc_NSlices_in_module; it++) {   // 0-15 //VETO+15slices

        Float_t xCur = Zdc_X_size_NICA + Zdc_module_X_size_NICA;   // 28.5+1.5=30 cm
        Float_t yCur = Zdc_Y_size_NICA + Zdc_module_Y_size_NICA;   // 28.5+1.5=30 cm
        rowNb = -10;
        columnNb = -10;
        iMod = -1;
        modNb = -1;
        modNb_addnode = -1;

        if (it == 0) {
            itInit_G10_before = -Zdc_Z_size + dist_to_nDet_surface + Zdc_G10_Z_size;   // before Sc
            ZDCV->AddNode(G10V_NICA, 2 * it + 1, new TGeoTranslation(0, 0, itInit_G10_before));
            cout << "ZDCV it,PCB_before " << itInit_G10_before << " " << 2 * it + 1 << endl;

            itInit_PLA = -Zdc_Z_size + dist_to_nDet_surface + 2. * Zdc_G10_Z_size + Zdc_PLA_Z_size;
            ZDCV->AddNode(PLAV_NICA, it + 1, new TGeoTranslation(0, 0, itInit_PLA));
            cout << "ZDCV it,PLA " << itInit_PLA << " " << it + 1 << endl;
            /*
            itInit_Veto=-Zdc_Z_size + dist_to_nDet_surface + 2.*Zdc_G10_Z_size + 2.*Zdc_PLA_Z_size + Zdc_Veto_Z_size;
            ZDCV->AddNode(zdcSensitiveVolumeV_VETO,it+1,new TGeoTranslation(0,0,itInit_Veto));
            cout <<"ZDCV it,Veto " <<itInit_Veto <<" " <<it+1 <<endl;
            */
            itInit_Veto =
                -Zdc_Z_size + dist_to_nDet_surface + 2. * Zdc_G10_Z_size + 2. * Zdc_PLA_Z_size + Zdc_Veto_Z_size;
            cout << "ZDCV it,Veto " << itInit_Veto << " " << it + 1 << endl;

            for (Int_t iy = 0; iy < Zdc_NRows_NICA; iy++) {   // 19
                yCur = yCur - 2. * Zdc_module_Y_size_NICA;
                for (Int_t ix = 0; ix < Zdc_NColumns_NICA; ix++) {   // 19
                    xCur = xCur - 2. * Zdc_module_X_size_NICA;

                    TGeoTranslation* zdcModuleV_position_NICA = new TGeoTranslation(xCur, yCur, itInit_Veto);
                    // iMod=iy*Zdc_NColumns_NICA + ix;
                    // modNb=iMod+1;
                    iMod = iy * Zdc_NColumns_NICA + ix + it * Zdc_nbCells_in_layer;
                    modNb = iMod + 1;

                    zdcModuleS_NICA[iMod] = new TGeoBBox(BmnNdetGeoPar::fModuleVolumeName, Zdc_module_X_size_NICA,
                                                         Zdc_module_Y_size_NICA, Zdc_module_Z_size_NICA);
                    zdcModuleV_NICA[iMod] = new TGeoVolume(BmnNdetGeoPar::fModuleVolumeName, zdcModuleS_NICA[iMod]);
                    zdcModuleV_NICA[iMod]->SetMedium(pMedAir);

                    rowNb = Zdc_NColumns_NICA - iy;
                    columnNb = Zdc_NRows_NICA - ix;
                    // zdcModuleV_NICA[iMod]->SetName(Form("%s_%d_%d", (BmnNdetGeoPar::fModuleVolumeName).Data(), rowNb,
                    // columnNb));
                    zdcModuleV_NICA[iMod]->SetName(
                        Form("%s_%d_%d_%d", (BmnNdetGeoPar::fModuleVolumeName).Data(), rowNb, columnNb, it + 1));
                    // cout <<"zdcModuleV_NICA " <<zdcModuleV_NICA[iMod]->GetName() <<" " <<rowNb <<" " <<columnNb <<" "
                    // <<modNb <<" " <<it+1 <<endl;

                    zdcModuleV_NICA[iMod]->AddNode(zdcSensitiveVolumeV_VETO, modNb, new TGeoTranslation(0, 0, 0));
                    ZDCV->AddNode(zdcModuleV_NICA[iMod], modNb, zdcModuleV_position_NICA);
                    // cout <<"NICA modNb,xxxx,yyyy " <<modNb <<" " <<xCur <<" " <<yCur <<endl;

                    // FillModule_NICA_simple(zdcModuleV_NICA[iMod]);

                    if (ix == Zdc_NColumns_NICA - 1)
                        xCur = Zdc_X_size_NICA + Zdc_module_X_size_NICA;
                }   // for(Int_t ix==0; ix<7; ix++)
            }   // for(Int_t iy==0; iy<7; iy++)

            itInit_Al = -Zdc_Z_size + dist_to_nDet_surface + 2. * Zdc_G10_Z_size + 2. * Zdc_PLA_Z_size
                        + 2. * Zdc_Veto_Z_size + Zdc_Al_Z_size;   // after Sc
            ZDCV->AddNode(AlV_NICA, it + 1, new TGeoTranslation(0, 0, itInit_Al));
            cout << "ZDCV it Al " << itInit_Al << " " << it + 1 << endl;

            itInit_G10_after = -Zdc_Z_size + dist_to_nDet_surface + 2. * Zdc_G10_Z_size + 2. * Zdc_PLA_Z_size
                               + 2. * Zdc_Veto_Z_size + 2. * Zdc_Al_Z_size + Zdc_G10_Z_size;   // after Sc
            ZDCV->AddNode(G10V_NICA, 2 * it + 2, new TGeoTranslation(0, 0, itInit_G10_after));
            cout << "ZDCV PCB_after " << itInit_G10_after << " " << 2 * it + 2 << " =====" << endl;
            // itInit_Cu=-Zdc_Z_size + dist_to_nDet_surface + sample_Z_size + Zdc_Air_2mm + Zdc_Cu_Z_size;//after Veto
            // ZDCV->AddNode(CuV_NICA,it+1,new TGeoTranslation(0,0,itInit_Cu));
            // cout <<"ZDCV it,Cu " <<itInit_Cu <<" " <<it+1 <<endl;

        }   // it==0
        else
        {
            // dist_before_sample = it*sample_Z_size + it*Zdc_Air_Z_size;
            // dist_before_sample = it*sample_Z_size + it*Zdc_Air_Z_size + it*Zdc_Cu_Z_size;
            // dist_before_sample = it*sample_Z_size + it*Zdc_Air_Z_size + (it-1)*2.*Zdc_Cu_Z_size;
            dist_before_sample =
                it * sample_Z_size + it * Zdc_Air_Z_size + (it - 1) * 2. * Zdc_Cu_Z_size + (it - 1) * Zdc_Air_Z_size;

            itInit_Cu = -Zdc_Z_size + dist_to_nDet_surface + dist_before_sample + Zdc_Cu_Z_size;
            ZDCV->AddNode(CuV_NICA, it + 1, new TGeoTranslation(0, 0, itInit_Cu));
            cout << "ZDCV it,Cu " << itInit_Cu << " " << it + 1 << endl;

            itInit_G10_before = -Zdc_Z_size + dist_to_nDet_surface + dist_before_sample + 2. * Zdc_Cu_Z_size
                                + Zdc_Air_Z_size + Zdc_G10_Z_size;   // before Sc
            ZDCV->AddNode(G10V_NICA, 2 * it + 1, new TGeoTranslation(0, 0, itInit_G10_before));
            cout << "ZDCV it,PCB_before " << itInit_G10_before << " " << 2 * it + 1 << endl;

            itInit_PLA = -Zdc_Z_size + dist_to_nDet_surface + dist_before_sample + 2. * Zdc_Cu_Z_size + Zdc_Air_Z_size
                         + 2. * Zdc_G10_Z_size + Zdc_PLA_Z_size;
            ZDCV->AddNode(PLAV_NICA, it + 1, new TGeoTranslation(0, 0, itInit_PLA));
            cout << "ZDCV it,PLA " << itInit_PLA << " " << it + 1 << endl;
            /*
            itInit_Sc=-Zdc_module_Z_size + dist_before_sample + 2.*Zdc_G10_Z_size + 2.*Zdc_PLA_Z_size + Zdc_Sc_Z_size;
            mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA,it+1,new TGeoTranslation(0,0,itInit_Veto));
            cout <<"ZDCV it,Sc " <<itInit_Sc <<" " <<it+1 <<endl;
            */

            itInit_Sc = -Zdc_Z_size + dist_before_sample + 2. * Zdc_Cu_Z_size + Zdc_Air_Z_size + 2. * Zdc_G10_Z_size
                        + 2. * Zdc_PLA_Z_size + Zdc_Sc_Z_size;
            cout << "ZDCV it,Sc " << itInit_Sc << " " << it << endl;
            for (Int_t iy = 0; iy < Zdc_NRows_NICA; iy++) {   // 19
                yCur = yCur - 2. * Zdc_module_Y_size_NICA;
                for (Int_t ix = 0; ix < Zdc_NColumns_NICA; ix++) {   // 19
                    xCur = xCur - 2. * Zdc_module_X_size_NICA;

                    TGeoTranslation* zdcModuleV_position_NICA = new TGeoTranslation(xCur, yCur, itInit_Sc);
                    // iMod=iy*Zdc_NColumns_NICA + ix;
                    // modNb=iMod+1;
                    iMod = iy * Zdc_NColumns_NICA + ix + it * Zdc_nbCells_in_layer;
                    modNb = iMod + 1;

                    zdcModuleS_NICA[iMod] = new TGeoBBox(BmnNdetGeoPar::fModuleVolumeName, Zdc_module_X_size_NICA,
                                                         Zdc_module_Y_size_NICA, Zdc_module_Z_size_NICA);
                    zdcModuleV_NICA[iMod] = new TGeoVolume(BmnNdetGeoPar::fModuleVolumeName, zdcModuleS_NICA[iMod]);
                    zdcModuleV_NICA[iMod]->SetMedium(pMedAir);

                    rowNb = Zdc_NColumns_NICA - iy;
                    columnNb = Zdc_NRows_NICA - ix;
                    // zdcModuleV_NICA[iMod]->SetName(Form("%s_%d_%d", (BmnNdetGeoPar::fModuleVolumeName).Data(), rowNb,
                    // columnNb));
                    zdcModuleV_NICA[iMod]->SetName(
                        Form("%s_%d_%d_%d", (BmnNdetGeoPar::fModuleVolumeName).Data(), rowNb, columnNb, it + 1));
                    // cout <<"zdcModuleV_NICA " <<zdcModuleV_NICA[iMod]->GetName() <<" " <<rowNb <<" " <<columnNb <<" "
                    // <<modNb <<" " <<it+1 <<endl;

                    zdcModuleV_NICA[iMod]->AddNode(zdcSensitiveVolumeV_NICA, modNb, new TGeoTranslation(0, 0, 0));
                    ZDCV->AddNode(zdcModuleV_NICA[iMod], modNb, zdcModuleV_position_NICA);
                    // cout <<"NICA modNb,xxxx,yyyy " <<modNb <<" " <<xCur <<" " <<yCur <<endl;

                    // FillModule_NICA_simple(zdcModuleV_NICA[iMod]);

                    if (ix == Zdc_NColumns_NICA - 1)
                        xCur = Zdc_X_size_NICA + Zdc_module_X_size_NICA;
                }   // for(Int_t ix==0; ix<7; ix++)
            }   // for(Int_t iy==0; iy<7; iy++)

            itInit_Al = -Zdc_Z_size + dist_to_nDet_surface + dist_before_sample + 2. * Zdc_Cu_Z_size + Zdc_Air_Z_size
                        + 2. * Zdc_G10_Z_size + 2. * Zdc_PLA_Z_size + 2. * Zdc_Veto_Z_size
                        + Zdc_Al_Z_size;   // after Sc
            ZDCV->AddNode(AlV_NICA, it + 1, new TGeoTranslation(0, 0, itInit_Al));
            cout << "ZDCV it Al " << itInit_Al << " " << it + 1 << endl;

            itInit_G10_after = -Zdc_Z_size + dist_to_nDet_surface + dist_before_sample + 2. * Zdc_Cu_Z_size
                               + Zdc_Air_Z_size + 2. * Zdc_G10_Z_size + 2. * Zdc_PLA_Z_size + 2. * Zdc_Veto_Z_size
                               + 2. * Zdc_Al_Z_size + Zdc_G10_Z_size;   // after Sc
            ZDCV->AddNode(G10V_NICA, 2 * it + 2, new TGeoTranslation(0, 0, itInit_G10_after));
            cout << "ZDCV PCB_after " << itInit_G10_after << " " << 2 * it + 2 << " === " << endl;
        }   // else
    }   // for(Int_t it=0; it<Zdc_NSlices_in_module; it++)
        // return;
    /*
   //Fill ZDC with NICA modules

     for(Int_t iy=0; iy<Zdc_NRows_NICA; iy++) {//19
       yCur=yCur - 2.*Zdc_module_Y_size_NICA;
       for(Int_t ix=0; ix<Zdc_NColumns_NICA; ix++) {//19
         xCur=xCur - 2.*Zdc_module_X_size_NICA;

         TGeoTranslation *zdcModuleV_position_NICA = new TGeoTranslation(xCur,yCur,0);
         iMod=iy*Zdc_NColumns_NICA + ix;
         modNb=iMod+1;

         zdcModuleS_NICA[iMod] = new TGeoBBox(BmnNdetGeoPar::fModuleVolumeName, Zdc_module_X_size_NICA,
   Zdc_module_Y_size_NICA, Zdc_module_Z_size_NICA); zdcModuleV_NICA[iMod] = new
   TGeoVolume(BmnNdetGeoPar::fModuleVolumeName, zdcModuleS_NICA[iMod]); zdcModuleV_NICA[iMod]->SetMedium(pMedAir);

         rowNb = Zdc_NColumns_NICA-iy;
         columnNb = Zdc_NRows_NICA-ix;
         zdcModuleV_NICA[iMod]->SetName(Form("%s_%d_%d", (BmnNdetGeoPar::fModuleVolumeName).Data(), rowNb, columnNb));
         cout <<"zdcModuleV_NICA " <<zdcModuleV_NICA[iMod]->GetName() <<" " <<rowNb <<" " <<columnNb <<" " <<modNb
   <<endl;

         zdcModuleV_NICA[iMod]->AddNode(zdcSensitiveVolumeV_VETO,modNb,zdcModuleV_position_NICA);
         ZDCV->AddNode(zdcModuleV_NICA[iMod],modNb,zdcModuleV_position_NICA);
         cout <<"NICA modNb,xxxx,yyyy " <<modNb <<" " <<xCur <<" " <<yCur <<endl;

         FillModule_NICA_simple(zdcModuleV_NICA[iMod]);

         if(ix==Zdc_NColumns_NICA-1) xCur=Zdc_X_size_NICA + Zdc_module_X_size_NICA;
       }//for(Int_t ix==0; ix<7; ix++)
     }//for(Int_t iy==0; iy<7; iy++)

     //return;
     */

    /*
   //Al edges  FRAME ===================================
   Double_t Al_edge_vert_X_size = 1; //2/2
   Double_t Al_edge_vert_Y_size = 31.6215; //63.243/2
   Double_t Al_edge_vert_Z_size = 1; //2/2

   Double_t Al_edge_hor_front_X_size = 24.9; //(53.8-2-2)/2
   Double_t Al_edge_hor_front_Y_size = 1; //2/2
   Double_t Al_edge_hor_front_Z_size = 1; //2/2

   Double_t Al_edge_hor_side_X_size = 1; //2/2
   Double_t Al_edge_hor_side_Y_size = 1; //2/2
   Double_t Al_edge_hor_side_Z_size = 28.875; //(61.75-4)/2

   //Shapes Al edges
    TGeoShape *Al_edge_vertS = new TGeoBBox(BmnNdetGeoPar::Al_edge_vert, Al_edge_vert_X_size, Al_edge_vert_Y_size,
   Al_edge_vert_Z_size); TGeoVolume *Al_edge_vertV = new TGeoVolume(BmnNdetGeoPar::Al_edge_vert, Al_edge_vertS);
    Al_edge_vertV->SetLineColor(kRed+4);
    Al_edge_vertV->SetFillColor(kRed+4);
    TGeoMedium *Al_edge_vert_medium = pMedAl; //set medium
    if(Al_edge_vert_medium) Al_edge_vertV->SetMedium(Al_edge_vert_medium);
    else Fatal("Main", "Invalid medium for Al_edge_vert!");

    TGeoShape *Al_edge_hor_frontS = new TGeoBBox(BmnNdetGeoPar::Al_edge_hor_front, Al_edge_hor_front_X_size,
   Al_edge_hor_front_Y_size, Al_edge_hor_front_Z_size); TGeoVolume *Al_edge_hor_frontV = new
   TGeoVolume(BmnNdetGeoPar::Al_edge_hor_front, Al_edge_hor_frontS); Al_edge_hor_frontV->SetLineColor(kRed+4);
    Al_edge_hor_frontV->SetFillColor(kRed+4);
    TGeoMedium *Al_edge_hor_front_medium = pMedAl; //set medium
    if(Al_edge_hor_front_medium) Al_edge_hor_frontV->SetMedium(Al_edge_hor_front_medium);
    else Fatal("Main", "Invalid medium for Al_edge_hor_front!");

    TGeoShape *Al_edge_hor_sideS = new TGeoBBox(BmnNdetGeoPar::Al_edge_hor_side, Al_edge_hor_side_X_size,
   Al_edge_hor_side_Y_size, Al_edge_hor_side_Z_size); TGeoVolume *Al_edge_hor_sideV = new
   TGeoVolume(BmnNdetGeoPar::Al_edge_hor_side, Al_edge_hor_sideS); Al_edge_hor_sideV->SetLineColor(kRed+4);
    Al_edge_hor_sideV->SetFillColor(kRed+4);
    TGeoMedium *Al_edge_hor_side_medium = pMedAl; //set medium
    if(Al_edge_hor_side_medium) Al_edge_hor_sideV->SetMedium(Al_edge_hor_side_medium);
    else Fatal("Main", "Invalid medium for Al_edge_hor_side!");

    //====================================
    Double_t xx, yy, zz;
    //Left front vertical edge
    xx = Zdc_X_size - Al_edge_vert_X_size;
    yy = 0;
    zz = -Zdc_Z_size + Al_edge_vert_Z_size;
    ZDCV->AddNode(Al_edge_vertV,1,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_front_vert_left_1  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //Right front vertical edge
    xx = -Zdc_X_size + Al_edge_vert_X_size;
    yy = 0;
    zz = -Zdc_Z_size + Al_edge_vert_Z_size;
    ZDCV->AddNode(Al_edge_vertV,2,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_front_vert_right_2  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //Left back vertical edge
    xx = Zdc_X_size - Al_edge_vert_X_size;
    yy = 0;
    zz = Zdc_Z_size - Al_edge_vert_Z_size;
    ZDCV->AddNode(Al_edge_vertV,3,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_back_vert_left_3  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //Right back vertical edge
    xx = -Zdc_X_size + Al_edge_vert_X_size;
    yy = 0;
    zz = Zdc_Z_size - Al_edge_vert_Z_size;
    ZDCV->AddNode(Al_edge_vertV,4,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_back_vert_right_4  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //==============================
    //Top front horizontal edge
    xx = 0;
    yy = Zdc_Y_size - Al_edge_hor_front_Y_size;
    zz = -Zdc_Z_size + Al_edge_hor_front_Z_size;
    ZDCV->AddNode(Al_edge_hor_frontV,1,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_hor_front_top_1  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //Bottom front horizontal edge
    xx = 0;
    yy = -Zdc_Y_size + Al_edge_hor_front_Y_size;
    zz = -Zdc_Z_size + Al_edge_hor_front_Z_size;
    ZDCV->AddNode(Al_edge_hor_frontV,2,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_hor_front_bottom_2  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //Top back horizontal edge
    xx = 0;
    yy = Zdc_Y_size - Al_edge_hor_front_Y_size;
    zz = Zdc_Z_size - Al_edge_hor_front_Z_size;
    ZDCV->AddNode(Al_edge_hor_frontV,3,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_hor_back_top_3  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //Bottom back horizontal edge
    xx = 0;
    yy = -Zdc_Y_size + Al_edge_hor_front_Y_size;
    zz = Zdc_Z_size - Al_edge_hor_front_Z_size;
    ZDCV->AddNode(Al_edge_hor_frontV,4,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_hor_back_bottom_4  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //==============================
    //Top side left horizontal edge
    xx = Zdc_X_size - Al_edge_hor_side_X_size;
    yy = Zdc_Y_size - Al_edge_hor_side_Y_size;
    zz = 0;
    ZDCV->AddNode(Al_edge_hor_sideV,1,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_hor_side_top_left_1  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //Bottom side left horizontal edge
    xx = Zdc_X_size - Al_edge_hor_side_X_size;
    yy = -Zdc_Y_size + Al_edge_hor_side_Y_size;
    zz = 0;
    ZDCV->AddNode(Al_edge_hor_sideV,2,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_hor_side_bottom_left_2  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //Top side right horizontal edge
    xx = -Zdc_X_size + Al_edge_hor_side_X_size;
    yy = Zdc_Y_size - Al_edge_hor_side_Y_size;
    zz = 0;
    ZDCV->AddNode(Al_edge_hor_sideV,3,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_hor_side_top_right_3  " <<xx <<" " <<yy <<" " <<zz <<endl;

    //Bottom side right horizontal edge
    xx = -Zdc_X_size + Al_edge_hor_side_X_size;
    yy = -Zdc_Y_size + Al_edge_hor_side_Y_size;
    zz = 0;
    ZDCV->AddNode(Al_edge_hor_sideV,3,new TGeoTranslation(xx,yy,zz));
    cout <<"Al_edge_hor_side_bottom_right_3  " <<xx <<" " <<yy <<" " <<zz <<endl;
   */

    // Adding the zdc mother volume to the global TOP Volume

    // DET_1
    TGeoRotation* rot = new TGeoRotation("rot");
    rot->RotateX(rotX_1);
    rot->RotateY(rotY_1);

    Double_t ZDC_center_Xpos;
    Double_t ZDC_center_Ypos;
    Double_t ZDC_center_Zpos;

    Double_t master[3] = {0, 0, 0};
    Double_t local[3];
    local[0] = 0;
    local[1] = 0;
    local[2] = nDet_dist_1 + Zdc_Z_size;
    rot->LocalToMaster(local, master);

    ZDC_center_Xpos = master[0];
    ZDC_center_Ypos = master[1];
    ZDC_center_Zpos = master[2];
    cout << "ZDC center " << ZDC_center_Xpos << " " << ZDC_center_Ypos << " " << ZDC_center_Zpos << endl;
    TGeoCombiTrans* zdcZDC_1 = new TGeoCombiTrans("zdcZDC_1", ZDC_center_Xpos, ZDC_center_Ypos, ZDC_center_Zpos, rot);
    NDET_common->AddNode(NDET_envelope, 1, zdcZDC_1);   // NDET
    // return;

    // DET_2
    TGeoRotation* rot2 = new TGeoRotation("rot2");
    rot2->RotateX(rotX_2);
    rot2->RotateY(rotY_2);

    Double_t ZDC_center_Xpos_2;
    Double_t ZDC_center_Ypos_2;
    Double_t ZDC_center_Zpos_2;

    Double_t master_2[3] = {0, 0, 0};
    Double_t local_2[3];
    local_2[0] = 0;
    local_2[1] = 0;
    local_2[2] = nDet_dist_2 + Zdc_Z_size;
    rot2->LocalToMaster(local_2, master_2);

    ZDC_center_Xpos_2 = master_2[0];
    ZDC_center_Ypos_2 = master_2[1];
    ZDC_center_Zpos_2 = master_2[2];
    cout << "ZDC center_2 " << ZDC_center_Xpos_2 << " " << ZDC_center_Ypos_2 << " " << ZDC_center_Zpos_2 << endl;
    TGeoCombiTrans* zdcZDC_2 =
        new TGeoCombiTrans("zdcZDC_2", ZDC_center_Xpos_2, ZDC_center_Ypos_2, ZDC_center_Zpos_2, rot2);
    NDET_common->AddNode(NDET_envelope, 2, zdcZDC_2);   // NDET
    // return;

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

    // TString gdmlname = gPath + "/geometry/tpc_v7.gdml";
    // gGeoManager->Export(gdmlname);

    // top->Draw("ogl");
    // TGLViewer *v = (TGLViewer*)gPad->GetViewer3D();
    // v->SetStyle(TGLRnrCtx::kOutline);
    // top->Draw("");

}   // create_rootgeom_zdc_v1

/*
void FillModule_NICA_simple(TGeoVolume *mother_volume_NICA) {

//Detector's construct parameters ===================

//Global dimensions for ZDC in cm

//Modules
Double_t Zdc_module_X_size = 2; //4./2.
Double_t Zdc_module_Y_size = 2; //4./2.
//Double_t Zdc_module_Z_size = 24.1; //48.2/2.
//Double_t Zdc_module_Z_size = 49.9; //99.8/2.
Double_t Zdc_module_Z_size = 26.65; //53.3/2.

//Double_t Zdc_NSlices_in_module = 8;   //Veto+7 slices
Double_t Zdc_NSlices_in_module = 16;   //Veto+15 slices

//Double_t Zdc_Air_2mm = 0.2; //2mm Air arround 1Cu
//Double_t Zdc_Air_2mm = 0; //2mm Air arround 1Cu
//Double_t Zdc_Air_Z_size = 3.4; //Air before sample
 Double_t Zdc_Air_Z_size = 0.3; //Air before sample

//ScAss slices PCB+PLA+Sc+Al+PCB
Double_t Zdc_ScAss_X_size = 2; //4/2.
Double_t Zdc_ScAss_Y_size = 2; //4/2.
Double_t Zdc_ScAss_Z_size = 1.525; //3.05/2.

//VetoAss slice PCB+PLA+Veto+Al+PCB
Double_t Zdc_VetoAss_X_size = 2; //4/2.
Double_t Zdc_VetoAss_Y_size = 2; //4/2.
Double_t Zdc_VetoAss_Z_size = 1.525; //3.05/2.

//Cu slice
Double_t Zdc_Cu_X_size = 2; //4/2.
Double_t Zdc_Cu_Y_size = 2; //4/2.
Double_t Zdc_Cu_Z_size = 1.5; //3./2.

//Al slices
Double_t Zdc_Al_X_size = 2; //4/2. //noCu
Double_t Zdc_Al_Y_size = 2; //4/2.
Double_t Zdc_Al_Z_size = 0.075; //0.15/2.

//PCB
Double_t Zdc_G10_X_size = 2; //as Pb
Double_t Zdc_G10_Y_size = 2; //as Pb
Double_t Zdc_G10_Z_size = 0.05; //0.1/2.

//PLA slices
Double_t Zdc_PLA_X_size = 2; //4/2
Double_t Zdc_PLA_Y_size = 2; //4/2.
Double_t Zdc_PLA_Z_size = 0.1; //0.2/2.

//Scin. slices
Double_t Zdc_Sc_X_size = 2; //as Pb
Double_t Zdc_Sc_Y_size = 2; //as Pb
Double_t Zdc_Sc_Z_size = 1.25; //2.5/2.

Double_t Zdc_Veto_X_size = 2; //as Pb
Double_t Zdc_Veto_Y_size = 2; //as Pb
Double_t Zdc_Veto_Z_size = 1.25; //2.5/2.

//Shapes
 TGeoShape *zdcSensitiveVolumeS_NICA = new TGeoBBox(BmnNdetGeoPar::SensitiveVolume_name_NICA, Zdc_Sc_X_size,
Zdc_Sc_Y_size, Zdc_Sc_Z_size); TGeoVolume *zdcSensitiveVolumeV_NICA = new
TGeoVolume(BmnNdetGeoPar::SensitiveVolume_name_NICA, zdcSensitiveVolumeS_NICA);
 zdcSensitiveVolumeV_NICA->SetLineColor(kBlue);
 zdcSensitiveVolumeV_NICA->SetFillColor(kBlue);
 TGeoMedium *Sc_medium_NICA = pMedScint; //set medium
 if(Sc_medium_NICA) zdcSensitiveVolumeV_NICA->SetMedium(Sc_medium_NICA);
 else Fatal("Main", "Invalid medium for Sc_NICA!");

 TGeoShape *zdcSensitiveVolumeS_VETO = new TGeoBBox(BmnNdetGeoPar::SensitiveVolume_name_VETO, Zdc_Veto_X_size,
Zdc_Veto_Y_size, Zdc_Veto_Z_size); TGeoVolume *zdcSensitiveVolumeV_VETO = new
TGeoVolume(BmnNdetGeoPar::SensitiveVolume_name_VETO, zdcSensitiveVolumeS_VETO);
 zdcSensitiveVolumeV_VETO->SetLineColor(kBlue);
 zdcSensitiveVolumeV_VETO->SetFillColor(kBlue);
 TGeoMedium *Sc_medium_VETO = pMedScint; //set medium
 if(Sc_medium_VETO) zdcSensitiveVolumeV_VETO->SetMedium(Sc_medium_VETO);
 else Fatal("Main", "Invalid medium for VETO!");

 Double_t dist_before_sample;
 Double_t sample_Z_size = 2.*(Zdc_G10_Z_size+Zdc_PLA_Z_size+Zdc_Sc_Z_size+Zdc_Al_Z_size+Zdc_G10_Z_size); //=3.05 cm
 //Double_t sample_Veto_size = 2.*(Zdc_PLA_Z_size+Zdc_Sc_Z_size+Zdc_G10_Z_size+Zdc_PLA_Z_size); //=3.1
 Double_t itDelta=0;
 Double_t itInit_Cu, itCur_Cu=0;
 Double_t itInit_Sc, itCur_Sc=0;
 Double_t itInit_G10_before, itCur_G10_before=0;
 Double_t itInit_G10_after, itCur_G10_after=0;
 Double_t itInit_Air, itCur_Air=0;
 Double_t itInit_Al, itCur_Al=0;
 Double_t itInit_Veto;
 Double_t itInit_PLA;

 for(Int_t it=0; it<Zdc_NSlices_in_module; it++) {//0-15 //VETO+15slices
   if(it==0) {
     itInit_Veto=-Zdc_module_Z_size + 2.*Zdc_G10_Z_size + 2.*Zdc_PLA_Z_size + Zdc_Veto_Z_size;
     mother_volume_NICA->AddNode(zdcSensitiveVolumeV_VETO,it+1,new TGeoTranslation(0,0,itInit_Veto));
     cout <<"NICA it,Veto " <<itInit_Veto <<" " <<it+1 <<endl;
   }//it==0
   else {
     dist_before_sample = it*sample_Z_size + it*Zdc_Air_Z_size;

     itInit_Sc=-Zdc_module_Z_size + dist_before_sample + 2.*Zdc_G10_Z_size + 2.*Zdc_PLA_Z_size + Zdc_Sc_Z_size;
     mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA,it+1,new TGeoTranslation(0,0,itInit_Sc));
     cout <<"NICA it,Sc " <<itInit_Sc <<" " <<it+1 <<endl;
   }//it>0
 }//for(Int_t it=0; it<Zdc_NSlices_in_module; it++)

//return 1;


}//FillModule_NICA_simple
*/
