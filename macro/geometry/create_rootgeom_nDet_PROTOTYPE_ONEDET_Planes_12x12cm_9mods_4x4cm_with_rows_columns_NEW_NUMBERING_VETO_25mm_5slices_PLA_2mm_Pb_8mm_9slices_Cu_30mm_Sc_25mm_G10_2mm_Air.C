//---------------------------
// M.Golubeva (INR RAS), December 2016
//---------------------------

#include "../profiling/bmnloadlibs.C"
#include "TFile.h"
#include "TGeoManager.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"

// Detector's construct parameters ===================

// Global dimensions for ZDC in cm

// 12x12cm 9mods
Double_t Zdc_X_size = 6;      // 12./2.
Double_t Zdc_Y_size = 6;      // 12./2.
Double_t Zdc_Z_size = 45.1;   // 90.2/2. with veto

Double_t Zdc_X_size_NICA = 6;   // 12./2.
Double_t Zdc_Y_size_NICA = 6;   // 12./2.

// Modules NICA
Double_t Zdc_NColumns_NICA = 3;
Double_t Zdc_NRows_NICA = 3;

Double_t Zdc_module_X_size_NICA = 2;   // 4./2.
Double_t Zdc_module_Y_size_NICA = 2;   // 4./2.
// Double_t Zdc_module_Z_size_NICA = 45.1;   // 90.2/2.

Double_t Zdc_module_Z_size_NICA = 1.25;      // 2.5/2.
Int_t Zdc_NSlices_in_module_withVeto = 15;   // Veto+14 slices
Int_t Zdc_NSlices_in_module = 14;            // 5Pb+9Cu

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

// int FillModule_NICA_simple(TGeoVolume* zdcModuleV_NICA);

void create_rootgeom_nDet_PROTOTYPE_ONEDET_Planes_12x12cm_9mods_4x4cm_with_rows_columns_NEW_NUMBERING_VETO_25mm_5slices_PLA_2mm_Pb_8mm_9slices_Cu_30mm_Sc_25mm_G10_2mm_Air(
    Double_t nDet_dist_1 = 595.617,
    Double_t Yshift_1 = 0.518,
    Double_t rotY_1 = -27.3,
    Double_t rotX_1 = 0)
{

    bmnloadlibs();   // load libraries

    // ----  set working directory  -------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   --------------------------------

    const TString geoDetectorName =
        "nDet_PROTOTYPE_ONEDET_Planes_12x12cm_9mods_4x4cm_with_rows_columns_NEW_NUMBERING_VETO_"
        "25mm_5slices_PLA_2mm_Pb_8mm_9slices_Cu_30mm_Sc_25mm_G10_2mm_Air";

    // const TString geoDetectorName = "test_nDet_PROTOTYPE_ONEDET_12x12cm_with_Ysh";

    // TString sXsh_1_1 = "";//not needed
    // TString sXsh_2_1 = "";

    TString sYsh_1_1 = "";
    TString sYsh_2_1 = "";

    TString sDist_1_1 = "";
    TString sDist_2_1 = "";

    TString sRotX_1_1 = "";
    TString sRotX_2_1 = "";

    TString sRotY_1_1 = "";
    TString sRotY_2_1 = "";

    // Int_t Xsh_1_1 = Int_t(Xshift_1); //2  //not needed
    // Int_t Xsh_22_1 = Int_t(100*(Xshift_1-Int_t(Xshift_1)));//9 //+4.5deg

    Int_t Ysh_1_1 = Int_t(Yshift_1);   // 2
    // Int_t Ysh_22_1 = Int_t(100*(Yshift_1-Int_t(Yshift_1)));//9 //+4.5deg
    Int_t Ysh_22_1 = Int_t(1000 * (Yshift_1 - Int_t(Yshift_1)));   // 9 //+4.5deg

    Int_t dist_1_1 = Int_t(nDet_dist_1);   // 2
    // Int_t dist_22_1 = Int_t(100*(nDet_dist_1-Int_t(nDet_dist_1)));//9 //+4.5deg
    // Int_t dist_22_1 = Int_t(100*(nDet_dist_1-Int_t(nDet_dist_1)))+1;//9 //+4.5deg
    // Int_t dist_22_1 = Int_t(1000*(nDet_dist_1-Int_t(nDet_dist_1)));//9 //+4.5deg
    Int_t dist_22_1 = Int_t(1000 * (nDet_dist_1 - Int_t(nDet_dist_1))) + 1;   // 9 //+4.5deg

    Int_t rotX_1_1 = Int_t(rotX_1);   // 2
    // Int_t rotX_22_1 = Int_t(100*(rotX_1-Int_t(rotX_1)));//9 //+4.5deg
    Int_t rotX_22_1 = -Int_t(100 * (rotX_1 - Int_t(rotX_1)));   // 9  //-4.5deg
    // Int_t rotX_22_1 = -Int_t(100*(rotX_1-Int_t(rotX_1)))+1;//9

    Int_t rotY_1_1 = Int_t(rotY_1);   // 2
    // Int_t rotY_22_1 = Int_t(100*(rotY_1-Int_t(rotY_1)));//9
    Int_t rotY_22_1 = -Int_t(100 * (rotY_1 - Int_t(rotY_1)));   // 9

    // sXsh_1_1 +=Xsh_1_1;//not needed
    sYsh_1_1 += Ysh_1_1;
    sYsh_2_1 += Ysh_22_1;
    sDist_1_1 += dist_1_1;
    sDist_2_1 += dist_22_1;
    sRotX_1_1 += rotX_1_1;
    sRotX_2_1 += rotX_22_1;
    sRotY_1_1 += rotY_1_1;
    sRotY_2_1 += rotY_22_1;

    // TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "ZdistDET_1_" + sDist_1_1 + "." + sDist_2_1
    // + "cm_" + "rotY_"  + sRotY_1_1  +  "." + sRotY_2_1 + "deg" + "_" + "rotX_"  + sRotX_1_1 + "." + sRotX_2_1 + "deg"
    // + ".root";
    TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "ZdistDET_1_" + sDist_1_1 + "." + sDist_2_1
                          + "cm_Ysh_" + sYsh_1_1 + "." + sYsh_2_1 + "cm_rotY_" + sRotY_1_1 + "." + sRotY_2_1
                          + "deg_rotX_" + sRotX_1_1 + "." + sRotX_2_1 + "deg" + ".root";

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
    TGeoVolume* top = new TGeoVolumeAssembly(BmnNdetGeoPar::fTopVolumeName);
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
    NDET_envelope->AddNode(ZDCV, 0, new TGeoTranslation(0., 0., 0.));

    // TGeoBBox* zdcModuleS_NICA[Int_t(Zdc_NRows_NICA * Zdc_NColumns_NICA)];
    // TGeoVolume* zdcModuleV_NICA[Int_t(Zdc_NRows_NICA * Zdc_NColumns_NICA)];
    TGeoBBox* zdcModuleS_NICA[Int_t(Zdc_NRows_NICA * Zdc_NColumns_NICA * Zdc_NSlices_in_module_withVeto)];
    TGeoVolume* zdcModuleV_NICA[Int_t(Zdc_NRows_NICA * Zdc_NColumns_NICA * Zdc_NSlices_in_module_withVeto)];

    Int_t rowNb = -10, columnNb = -10;
    Int_t iMod = -1, modNb = -1;

    Int_t Zdc_nbCells_in_layer;
    Zdc_nbCells_in_layer = Int_t(Zdc_NRows_NICA * Zdc_NColumns_NICA);

    // Planes Pb, Cu, PCB, PLA ===================

    // Double_t Zdc_Air_2mm = 0.2; //2mm Air arround 1Cu
    ////Double_t Zdc_Air_2mm = 0.; //0mm Air arround 1Cu
    // Double_t Zdc_Air_Z_size = 3.4; //Air before sample
    ////Double_t Zdc_Air_Z_size = 0.3; //Air before sample //arround Cu

    // Scin. slices
    Double_t Zdc_Sc_X_size = 2;      // as Pb
    Double_t Zdc_Sc_Y_size = 2;      // as Pb
    Double_t Zdc_Sc_Z_size = 1.25;   // 2.5/2.

    Double_t Zdc_Veto_X_size = 2;      // as Pb
    Double_t Zdc_Veto_Y_size = 2;      // as Pb
    Double_t Zdc_Veto_Z_size = 1.25;   // 2.5/2.

    // Pb plane
    //  Pb slices //5slices Pb 8mm
    Double_t Zdc_NPb_in_module = 5;
    Double_t Zdc_Pb_X_size = 6;     // 12/2.
    Double_t Zdc_Pb_Y_size = 6;     // 12/2.
    Double_t Zdc_Pb_Z_size = 0.4;   // 0.8/2.

    // Cu plane
    // Double_t Zdc_NPb_in_module_10mm = 9;   // 9 slices Cu 3cm
    Double_t Zdc_NCu_in_module = 9;   // 9 slices Cu 3cm
    Double_t Zdc_Cu_X_size = 6;       // 12/2.
    Double_t Zdc_Cu_Y_size = 6;       // 12/2.
    Double_t Zdc_Cu_Z_size = 1.5;     // 3./2.

    // Al slices
    // Double_t Zdc_Al_X_size = 22; //44/2. //noCu
    // Double_t Zdc_Al_Y_size = 22; //44/2.
    // Double_t Zdc_Al_Z_size = 0.075; //0.15/2.

    // PCB plane //G10
    Double_t Zdc_G10_X_size = 6;     // 12/2
    Double_t Zdc_G10_Y_size = 6;     // 12/4
    Double_t Zdc_G10_Z_size = 0.1;   // 0.2/2.

    // PLA plane
    Double_t Zdc_PLA_X_size = 6;     // 12/2
    Double_t Zdc_PLA_Y_size = 6;     // 12/2.
    Double_t Zdc_PLA_Z_size = 0.1;   // 0.2/2.

    TGeoShape* PbS_NICA =
        new TGeoBBox(BmnNdetGeoPar::Pb_name_NICA, Zdc_Pb_X_size, Zdc_Pb_Y_size, Zdc_Pb_Z_size);   // Lead
    TGeoVolume* PbV_NICA = new TGeoVolume(BmnNdetGeoPar::Pb_name_NICA, PbS_NICA);
    PbV_NICA->SetLineColor(kMagenta);
    PbV_NICA->SetFillColor(kMagenta);
    TGeoMedium* Pb_medium_NICA = pMedLead;   // set medium
    if (Pb_medium_NICA)
        PbV_NICA->SetMedium(Pb_medium_NICA);
    else
        Fatal("Main", "Invalid medium for Pb_NICA!");

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

    // TGeoShape *AlS_NICA = new TGeoBBox(BmnNdetGeoPar::Al_name_NICA, Zdc_Al_X_size, Zdc_Al_Y_size, Zdc_Al_Z_size);
    // //Aluminium TGeoVolume *AlV_NICA = new TGeoVolume(BmnNdetGeoPar::Al_name_NICA, AlS_NICA);
    // AlV_NICA->SetLineColor(kRed);//light-blue
    // AlV_NICA->SetFillColor(kRed);
    // TGeoMedium *Al_medium_NICA = pMedAl; //set medium
    // if(Al_medium_NICA) AlV_NICA->SetMedium(Al_medium_NICA);
    // else Fatal("Main", "Invalid medium for Al_NICA!");

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

    Double_t itDelta = 0;
    Double_t itInit_Pb, itCur_Pb = 0;
    Double_t itInit_Cu, itCur_Cu = 0;
    Double_t itInit_Sc, itCur_Sc = 0;
    Double_t itInit_G10, itCur_G10 = 0;   // PCB
    Double_t itInit_Veto;
    Double_t itInit_PLA;

    Double_t dist_to_nDet_surface = 0;
    Double_t Scint_edge_Z_coord[Zdc_NSlices_in_module];   // 5Pb,9Cu

    for (Int_t it = 0; it < Zdc_NSlices_in_module; it++) {   // measured
        if (it == 0)
            Scint_edge_Z_coord[it] = -40.1;
        if (it == 1)
            Scint_edge_Z_coord[it] = -34.7;
        if (it == 2)
            Scint_edge_Z_coord[it] = -30.8;
        if (it == 3)
            Scint_edge_Z_coord[it] = -26.6;
        if (it == 4)
            Scint_edge_Z_coord[it] = -21.7;
        if (it == 5)
            Scint_edge_Z_coord[it] = -14.6;
        if (it == 6)
            Scint_edge_Z_coord[it] = -7.4;
        if (it == 7)
            Scint_edge_Z_coord[it] = 0;
        if (it == 8)
            Scint_edge_Z_coord[it] = 6.4;
        if (it == 9)
            Scint_edge_Z_coord[it] = 13.3;
        if (it == 10)
            Scint_edge_Z_coord[it] = 20.4;
        if (it == 11)
            Scint_edge_Z_coord[it] = 27.7;
        if (it == 12)
            Scint_edge_Z_coord[it] = 34.6;
        if (it == 13)
            Scint_edge_Z_coord[it] = 42.2;
    }

    for (Int_t it = 0; it < Zdc_NSlices_in_module_withVeto; it++) {   // 0-14 //VETO+14slices

        Float_t xCur = Zdc_X_size_NICA + Zdc_module_X_size_NICA;   // 28.5+1.5=30 cm
        Float_t yCur = Zdc_Y_size_NICA + Zdc_module_Y_size_NICA;   // 28.5+1.5=30 cm
        rowNb = -10;
        columnNb = -10;
        iMod = -1;
        modNb = -1;   // modNb_addnode=-1;

        if (it == 0) {   // Veto  layer=0 ==================
            // itInit_Veto = -(Zdc_module_Z_size - Zdc_Veto_Z_size);
            itInit_Veto = -(Zdc_Z_size - Zdc_Veto_Z_size);
            cout << "ZDCV it,Veto " << itInit_Veto << " " << it + 1 << endl;

            for (Int_t iy = 0; iy < Zdc_NRows_NICA; iy++) {   // 19
                yCur = yCur - 2. * Zdc_module_Y_size_NICA;
                for (Int_t ix = 0; ix < Zdc_NColumns_NICA; ix++) {   // 19
                    xCur = xCur - 2. * Zdc_module_X_size_NICA;

                    TGeoTranslation* zdcModuleV_position_NICA = new TGeoTranslation(xCur, yCur, itInit_Veto);
                    // iMod = iy * Zdc_NColumns_NICA + ix;
                    // modNb = iMod + 1;
                    iMod = iy * Zdc_NColumns_NICA + ix + it * Zdc_nbCells_in_layer;
                    modNb = iMod + 1;

                    zdcModuleS_NICA[iMod] = new TGeoBBox(BmnNdetGeoPar::fModuleVolumeName, Zdc_module_X_size_NICA,
                                                         Zdc_module_Y_size_NICA, Zdc_module_Z_size_NICA);
                    zdcModuleV_NICA[iMod] = new TGeoVolume(BmnNdetGeoPar::fModuleVolumeName, zdcModuleS_NICA[iMod]);
                    zdcModuleV_NICA[iMod]->SetMedium(pMedAir);

                    rowNb = Zdc_NColumns_NICA - iy;
                    columnNb = Zdc_NRows_NICA - ix;
                    // zdcModuleV_NICA[iMod]->SetName(Form("%s_%d_%d",
                    //                                  (BmnNdetGeoPar::fModuleVolumeName).Data(),
                    //                                  rowNb,
                    //                                  columnNb));   // ix+1 - columnNb //new numbering
                    // cout << "zdcModuleV_NICA " << zdcModuleV_NICA[iMod]->GetName() << " " << rowNb << " " << columnNb
                    // << " "
                    //   << modNb << endl;

                    zdcModuleV_NICA[iMod]->SetName(
                        Form("%s_%d_%d_%d", (BmnNdetGeoPar::fModuleVolumeName).Data(), rowNb, columnNb, it + 1));
                    // cout <<"zdcModuleV_NICA " <<zdcModuleV_NICA[iMod]->GetName() <<" " <<rowNb <<" " <<columnNb <<" "
                    // <<modNb <<" " <<it+1 <<endl;
                    zdcModuleV_NICA[iMod]->AddNode(zdcSensitiveVolumeV_VETO, modNb, new TGeoTranslation(0, 0, 0));

                    ZDCV->AddNode(zdcModuleV_NICA[iMod], modNb, zdcModuleV_position_NICA);
                    // cout << "NICA modNb,xxxx,yyyy " << modNb << " " << xCur << " " << yCur << endl;
                    if (ix == Zdc_NColumns_NICA - 1)
                        xCur = Zdc_X_size_NICA + Zdc_module_X_size_NICA;
                    // FillModule_NICA_simple(zdcModuleV_NICA[iMod]);

                }   // for(Int_t ix==0; ix<7; ix++)
            }   // for(Int_t iy==0; iy<7; iy++)

            // insert G10 slice after Veto
            // itInit_G10 = -(Zdc_module_Z_size - 2. * Zdc_Veto_Z_size - Zdc_G10_Z_size);
            itInit_G10 = -(Zdc_Z_size - 2. * Zdc_Veto_Z_size - Zdc_G10_Z_size);
            ZDCV->AddNode(G10V_NICA, 1, new TGeoTranslation(0, 0, itInit_G10));
            cout << "NICA G10_1 " << itInit_G10 << endl;
        }   // it==0 //veto+G10

        if (it > 0 && it <= 5) {   // Pb part layers 1-5  ================
            itInit_PLA = Scint_edge_Z_coord[it - 1] - 2. * Zdc_Pb_Z_size - Zdc_PLA_Z_size;
            ZDCV->AddNode(PLAV_NICA, it + 1, new TGeoTranslation(0, 0, itInit_PLA));
            cout << "NICA it,PLA " << Scint_edge_Z_coord[it - 1] << " " << 2. * Zdc_Pb_Z_size << " " << Zdc_PLA_Z_size
                 << " " << itInit_PLA << " " << it + 1 << endl;
            itInit_Pb = Scint_edge_Z_coord[it - 1] - Zdc_Pb_Z_size;
            ZDCV->AddNode(PbV_NICA, it + 1, new TGeoTranslation(0, 0, itInit_Pb));
            cout << "NICA it,Pb " << itInit_Pb << " " << it + 1 << endl;

            itInit_Sc = Scint_edge_Z_coord[it - 1] + Zdc_Sc_Z_size;
            cout << "NICA it,Sc " << itInit_Sc << " " << it + 1 << endl;

            rowNb = -10;
            columnNb = -10;
            iMod = -1;
            modNb = -1;   // modNb_addnode=-1;

            for (Int_t iy = 0; iy < Zdc_NRows_NICA; iy++) {   // 19
                yCur = yCur - 2. * Zdc_module_Y_size_NICA;
                for (Int_t ix = 0; ix < Zdc_NColumns_NICA; ix++) {   // 19
                    xCur = xCur - 2. * Zdc_module_X_size_NICA;

                    TGeoTranslation* zdcModuleV_position_NICA = new TGeoTranslation(xCur, yCur, itInit_Sc);
                    // iMod = iy * Zdc_NColumns_NICA + ix;
                    // modNb = iMod + 1;
                    iMod = iy * Zdc_NColumns_NICA + ix + it * Zdc_nbCells_in_layer;
                    modNb = iMod + 1;

                    zdcModuleS_NICA[iMod] = new TGeoBBox(BmnNdetGeoPar::fModuleVolumeName, Zdc_module_X_size_NICA,
                                                         Zdc_module_Y_size_NICA, Zdc_module_Z_size_NICA);
                    zdcModuleV_NICA[iMod] = new TGeoVolume(BmnNdetGeoPar::fModuleVolumeName, zdcModuleS_NICA[iMod]);
                    zdcModuleV_NICA[iMod]->SetMedium(pMedAir);

                    rowNb = Zdc_NColumns_NICA - iy;
                    columnNb = Zdc_NRows_NICA - ix;
                    // zdcModuleV_NICA[iMod]->SetName(Form("%s_%d_%d",
                    //                                  (BmnNdetGeoPar::fModuleVolumeName).Data(),
                    //                                  rowNb,
                    //                                  columnNb));   // ix+1 - columnNb //new numbering
                    // cout << "zdcModuleV_NICA " << zdcModuleV_NICA[iMod]->GetName() << " " << rowNb << " " << columnNb
                    // << " "
                    //   << modNb << endl;

                    zdcModuleV_NICA[iMod]->SetName(
                        Form("%s_%d_%d_%d", (BmnNdetGeoPar::fModuleVolumeName).Data(), rowNb, columnNb, it + 1));
                    // cout <<"zdcModuleV_NICA " <<zdcModuleV_NICA[iMod]->GetName() <<" " <<rowNb <<" " <<columnNb <<" "
                    // <<modNb <<" " <<it+1 <<endl;
                    zdcModuleV_NICA[iMod]->AddNode(zdcSensitiveVolumeV_NICA, modNb, new TGeoTranslation(0, 0, 0));

                    ZDCV->AddNode(zdcModuleV_NICA[iMod], modNb, zdcModuleV_position_NICA);
                    // cout << "NICA modNb,xxxx,yyyy " << modNb << " " << xCur << " " << yCur << endl;
                    if (ix == Zdc_NColumns_NICA - 1)
                        xCur = Zdc_X_size_NICA + Zdc_module_X_size_NICA;
                    // FillModule_NICA_simple(zdcModuleV_NICA[iMod]);

                }   // for(Int_t ix==0; ix<7; ix++)
            }   // for(Int_t iy==0; iy<7; iy++)

            itInit_G10 = Scint_edge_Z_coord[it - 1] + 2. * Zdc_Sc_Z_size + Zdc_G10_Z_size;
            ZDCV->AddNode(G10V_NICA, it + 1, new TGeoTranslation(0, 0, itInit_G10));   // G10 - 1 -> after Veto
            cout << "NICA it,G10 " << itInit_G10 << " ===== " << " " << it + 1 << endl;
        }   // if (it>0 && it <= 5)// Pb part

        if (it > 5) {   // Cu part// layers 6-14  ======================
            itInit_Cu = Scint_edge_Z_coord[it - 1] - Zdc_Cu_Z_size;
            ZDCV->AddNode(CuV_NICA, it + 1, new TGeoTranslation(0, 0, itInit_Cu));
            cout << "NICA it,Cu " << itInit_Cu << " " << it + 1 << endl;

            itInit_Sc = Scint_edge_Z_coord[it - 1] + Zdc_Sc_Z_size;
            cout << "NICA it,Sc " << itInit_Sc << " " << it + 1 << endl;

            rowNb = -10;
            columnNb = -10;
            iMod = -1;
            modNb = -1;   // modNb_addnode=-1;

            for (Int_t iy = 0; iy < Zdc_NRows_NICA; iy++) {   // 19
                yCur = yCur - 2. * Zdc_module_Y_size_NICA;
                for (Int_t ix = 0; ix < Zdc_NColumns_NICA; ix++) {   // 19
                    xCur = xCur - 2. * Zdc_module_X_size_NICA;

                    TGeoTranslation* zdcModuleV_position_NICA = new TGeoTranslation(xCur, yCur, itInit_Sc);
                    // iMod = iy * Zdc_NColumns_NICA + ix;
                    // modNb = iMod + 1;
                    iMod = iy * Zdc_NColumns_NICA + ix + it * Zdc_nbCells_in_layer;
                    modNb = iMod + 1;

                    zdcModuleS_NICA[iMod] = new TGeoBBox(BmnNdetGeoPar::fModuleVolumeName, Zdc_module_X_size_NICA,
                                                         Zdc_module_Y_size_NICA, Zdc_module_Z_size_NICA);
                    zdcModuleV_NICA[iMod] = new TGeoVolume(BmnNdetGeoPar::fModuleVolumeName, zdcModuleS_NICA[iMod]);
                    zdcModuleV_NICA[iMod]->SetMedium(pMedAir);

                    rowNb = Zdc_NColumns_NICA - iy;
                    columnNb = Zdc_NRows_NICA - ix;
                    // zdcModuleV_NICA[iMod]->SetName(Form("%s_%d_%d",
                    //                                  (BmnNdetGeoPar::fModuleVolumeName).Data(),
                    //                                  rowNb,
                    //                                  columnNb));   // ix+1 - columnNb //new numbering
                    // cout << "zdcModuleV_NICA " << zdcModuleV_NICA[iMod]->GetName() << " " << rowNb << " " << columnNb
                    // << " "
                    //   << modNb << endl;

                    zdcModuleV_NICA[iMod]->SetName(
                        Form("%s_%d_%d_%d", (BmnNdetGeoPar::fModuleVolumeName).Data(), rowNb, columnNb, it + 1));
                    // cout <<"zdcModuleV_NICA " <<zdcModuleV_NICA[iMod]->GetName() <<" " <<rowNb <<" " <<columnNb <<" "
                    // <<modNb <<" " <<it+1 <<endl;
                    zdcModuleV_NICA[iMod]->AddNode(zdcSensitiveVolumeV_NICA, modNb, new TGeoTranslation(0, 0, 0));

                    ZDCV->AddNode(zdcModuleV_NICA[iMod], modNb, zdcModuleV_position_NICA);
                    // cout << "NICA modNb,xxxx,yyyy " << modNb << " " << xCur << " " << yCur << endl;
                    if (ix == Zdc_NColumns_NICA - 1)
                        xCur = Zdc_X_size_NICA + Zdc_module_X_size_NICA;
                    // FillModule_NICA_simple(zdcModuleV_NICA[iMod]);

                }   // for(Int_t ix==0; ix<7; ix++)
            }   // for(Int_t iy==0; iy<7; iy++)

            itInit_G10 = Scint_edge_Z_coord[it - 1] + 2. * Zdc_Sc_Z_size + Zdc_G10_Z_size;
            ZDCV->AddNode(G10V_NICA, it + 1, new TGeoTranslation(0, 0, itInit_G10));
            cout << "NICA it,G10 " << itInit_G10 << " ==== " << it + 1 << endl;
        }   // it>5 //Cu part
    }   // for(Int_t it=0; it<Zdc_NSlices_in_module_withVeto; it++)

    // return;

    // Adding the zdc mother volume to the global TOP Volume
    // DET_1
    TGeoRotation* rot = new TGeoRotation("rot");
    rot->RotateX(rotX_1);
    rot->RotateY(rotY_1);

    Double_t ZDC_center_Xpos;
    Double_t ZDC_center_Ypos;
    Double_t ZDC_center_Zpos;
    /*
    Double_t master[3] = { 5.9, 2.4, 0 };
    Double_t local[3];
    local[0] = 0; local[1] = 0.518; local[2] = nDet_dist_1+Zdc_Z_size;
    rot->LocalToMaster(local, master);
    cout <<"master_1 " <<master[0] <<" " <<master[1] <<" " <<master[2] <<endl;
    cout <<"local_1 " <<local[0] <<" " <<local[1] <<" " <<local[2] <<endl;
    */
    /*
    Double_t master[3] = { 0, 0, 0 };
    Double_t local[3];
    //local[0] = 0; local[1] = 0; local[2] = nDet_dist_1+Zdc_Z_size;
    local[0] = Xshift_1; local[1] = Yshift_1; local[2] = nDet_dist_1+Zdc_Z_size;
    rot->LocalToMaster(local, master);
    */

    Double_t master[3] = {0, 0, 0};
    Double_t local[3];
    local[0] = 0;
    local[1] = 0;
    local[2] = nDet_dist_1 + Zdc_Z_size;
    rot->LocalToMaster(local, master);
    master[1] += Yshift_1;

    ZDC_center_Xpos = master[0];
    ZDC_center_Ypos = master[1];
    ZDC_center_Zpos = master[2];
    cout << "ZDC center " << ZDC_center_Xpos << " " << ZDC_center_Ypos << " " << ZDC_center_Zpos << endl;
    TGeoCombiTrans* zdcZDC_1 = new TGeoCombiTrans("zdcZDC_1", ZDC_center_Xpos, ZDC_center_Ypos, ZDC_center_Zpos, rot);
    NDET_common->AddNode(NDET_envelope, 0, zdcZDC_1);   // NDET
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
int FillModule_NICA_simple(TGeoVolume* mother_volume_NICA)
{

    // Detector's construct parameters ===================

    // Global dimensions for ZDC in cm

    // 12x12cm 9mods //not used
    Double_t Zdc_X_size = 6;      // 12/2.
    Double_t Zdc_Y_size = 6;      // 12/2.
    Double_t Zdc_Z_size = 45.1;   // 90.2/2.

    // Modules
    Double_t Zdc_module_X_size = 2;      // 4./2.
    Double_t Zdc_module_Y_size = 2;      // 4./2.
    Double_t Zdc_module_Z_size = 45.1;   // 90.2/2.

    // Pb slices //Fe slices, do not change names //5slices Pb 8mm
    Double_t Zdc_NPb_in_module_5mm = 5;
    Double_t Zdc_Pb_X_size_5mm = 2;     // 4/2
    Double_t Zdc_Pb_Y_size_5mm = 2;     // 4/2.
    Double_t Zdc_Pb_Z_size_5mm = 0.4;   // 0.8/2.

    // PLA slices //5slices PLA 2mm before PB slices
    Double_t Zdc_PLA_X_size_5mm = 2;     // 4/2
    Double_t Zdc_PLA_Y_size_5mm = 2;     // 4/2.
    Double_t Zdc_PLA_Z_size_5mm = 0.1;   // 0.2/2.

    // Pb slices //Fe slices, do not change names //5slices Pb 8mm
    Double_t Zdc_NPb_in_module_10mm = 9;   // 9 slices Cu 3cm
    Double_t Zdc_Pb_X_size_10mm = 2;       // 4/2.
    Double_t Zdc_Pb_Y_size_10mm = 2;       // 4/2.
    Double_t Zdc_Pb_Z_size_10mm = 1.5;     // 3./2.

    Double_t Zdc_Sc_X_size = 2;      // as Pb
    Double_t Zdc_Sc_Y_size = 2;      // as Pb
    Double_t Zdc_Sc_Z_size = 1.25;   // 2.5/2.

    Double_t Zdc_Veto_X_size = 2;      // as Pb
    Double_t Zdc_Veto_Y_size = 2;      // as Pb
    Double_t Zdc_Veto_Z_size = 1.25;   // 2.5/2.

    // PCB
    Double_t Zdc_G10_X_size = 2;     // as Pb
    Double_t Zdc_G10_Y_size = 2;     // as Pb
    Double_t Zdc_G10_Z_size = 0.1;   // 0.2/2.

    Int_t Zdc_NSlises_in_module;
    Zdc_NSlises_in_module = Zdc_NPb_in_module_5mm + Zdc_NPb_in_module_10mm;
    Double_t Scint_edge_Z_coord[Zdc_NSlises_in_module];   // 5Pb,9Cu

    // Shapes
    TGeoShape* PbS_NICA_5mm =
        new TGeoBBox(BmnNdetGeoPar::Pb_name_NICA_5mm, Zdc_Pb_X_size_5mm, Zdc_Pb_Y_size_5mm, Zdc_Pb_Z_size_5mm);
    TGeoVolume* PbV_NICA_5mm = new TGeoVolume(BmnNdetGeoPar::Pb_name_NICA_5mm, PbS_NICA_5mm);
    PbV_NICA_5mm->SetLineColor(kBlue);
    PbV_NICA_5mm->SetFillColor(kBlue);
    TGeoMedium* Pb_medium_NICA_5mm = pMedLead;   // set medium
    if (Pb_medium_NICA_5mm)
        PbV_NICA_5mm->SetMedium(Pb_medium_NICA_5mm);
    else
        Fatal("Main", "Invalid medium for Pb_NICA_5mm!");

    TGeoShape* PbS_NICA_10mm =
        new TGeoBBox(BmnNdetGeoPar::Pb_name_NICA_10mm, Zdc_Pb_X_size_10mm, Zdc_Pb_Y_size_10mm, Zdc_Pb_Z_size_10mm);
    TGeoVolume* PbV_NICA_10mm = new TGeoVolume(BmnNdetGeoPar::Pb_name_NICA_10mm, PbS_NICA_10mm);
    PbV_NICA_10mm->SetLineColor(kMagenta);
    PbV_NICA_10mm->SetFillColor(kMagenta);
    TGeoMedium* Pb_medium_NICA_10mm = pMedCopper;   // set medium
    if (Pb_medium_NICA_10mm)
        PbV_NICA_10mm->SetMedium(Pb_medium_NICA_10mm);
    else
        Fatal("Main", "Invalid medium for Pb_NICA_10mm!");

    TGeoShape* zdcSensitiveVolumeS_NICA =
        new TGeoBBox(BmnNdetGeoPar::SensitiveVolume_name_NICA, Zdc_Sc_X_size, Zdc_Sc_Y_size, Zdc_Sc_Z_size);
    TGeoVolume* zdcSensitiveVolumeV_NICA =
        new TGeoVolume(BmnNdetGeoPar::SensitiveVolume_name_NICA, zdcSensitiveVolumeS_NICA);
    zdcSensitiveVolumeV_NICA->SetLineColor(kRed);
    zdcSensitiveVolumeV_NICA->SetFillColor(kRed);
    TGeoMedium* Sc_medium_NICA = pMedScint;   // set medium
    if (Sc_medium_NICA)
        zdcSensitiveVolumeV_NICA->SetMedium(Sc_medium_NICA);
    else
        Fatal("Main", "Invalid medium for Sc_NICA!");

    TGeoShape* zdcSensitiveVolumeS_VETO =
        new TGeoBBox(BmnNdetGeoPar::SensitiveVolume_name_VETO, Zdc_Veto_X_size, Zdc_Veto_Y_size, Zdc_Veto_Z_size);
    TGeoVolume* zdcSensitiveVolumeV_VETO =
        new TGeoVolume(BmnNdetGeoPar::SensitiveVolume_name_VETO, zdcSensitiveVolumeS_VETO);
    zdcSensitiveVolumeV_VETO->SetLineColor(kGray);
    zdcSensitiveVolumeV_VETO->SetFillColor(kGray);
    TGeoMedium* Sc_medium_VETO = pMedScint;   // set medium
    if (Sc_medium_VETO)
        zdcSensitiveVolumeV_VETO->SetMedium(Sc_medium_VETO);
    else
        Fatal("Main", "Invalid medium for VETO!");

    TGeoShape* G10S_NICA = new TGeoBBox(BmnNdetGeoPar::G10_name_NICA, Zdc_G10_X_size, Zdc_G10_Y_size, Zdc_G10_Z_size);
    TGeoVolume* G10V_NICA = new TGeoVolume(BmnNdetGeoPar::G10_name_NICA, G10S_NICA);
    G10V_NICA->SetLineColor(kOrange - 7);
    G10V_NICA->SetFillColor(kOrange - 7);
    TGeoMedium* G10_medium_NICA = pMedG10;   // set medium
    if (G10_medium_NICA)
        G10V_NICA->SetMedium(G10_medium_NICA);
    else
        Fatal("Main", "Invalid medium for G10_NICA!");

    TGeoShape* PLAS_NICA =
        new TGeoBBox(BmnNdetGeoPar::PLA_name_NICA, Zdc_PLA_X_size_5mm, Zdc_PLA_Y_size_5mm, Zdc_PLA_Z_size_5mm);
    TGeoVolume* PLAV_NICA = new TGeoVolume(BmnNdetGeoPar::PLA_name_NICA, PLAS_NICA);
    PLAV_NICA->SetLineColor(kViolet);
    PLAV_NICA->SetFillColor(kViolet);
    TGeoMedium* PLA_medium_NICA = pMedPLA;   // set medium
    if (PLA_medium_NICA)
        PLAV_NICA->SetMedium(PLA_medium_NICA);
    else
        Fatal("Main", "Invalid medium for PLA_NICA!");

    Double_t itDelta = 0;
    Double_t itInit_Pb, itCur_Pb = 0;
    Double_t itInit_Sc, itCur_Sc = 0;
    Double_t itInit_G10, itCur_G10 = 0;
    Double_t itInit_Veto;
    Double_t itInit_PLA;

    // insert Veto slice
    itInit_Veto = -(Zdc_module_Z_size - Zdc_Veto_Z_size);
    mother_volume_NICA->AddNode(zdcSensitiveVolumeV_VETO, 1, new TGeoTranslation(0, 0, itInit_Veto));
    cout << "NICA Veto " << Zdc_module_Z_size << " " << Zdc_Veto_Z_size << " " << itInit_Veto << endl;

    // insert G10 slice after Veto
    itInit_G10 = -(Zdc_module_Z_size - 2. * Zdc_Veto_Z_size - Zdc_G10_Z_size);

    mother_volume_NICA->AddNode(G10V_NICA, 1, new TGeoTranslation(0, 0, itInit_G10));
    cout << "NICA G10_1 " << itInit_G10 << endl;

    for (Int_t it = 0; it < Zdc_NSlises_in_module; it++) {   // measured
        if (it == 0)
            Scint_edge_Z_coord[it] = -40.1;
        if (it == 1)
            Scint_edge_Z_coord[it] = -34.7;
        if (it == 2)
            Scint_edge_Z_coord[it] = -30.8;
        if (it == 3)
            Scint_edge_Z_coord[it] = -26.6;
        if (it == 4)
            Scint_edge_Z_coord[it] = -21.7;
        if (it == 5)
            Scint_edge_Z_coord[it] = -14.6;
        if (it == 6)
            Scint_edge_Z_coord[it] = -7.4;
        if (it == 7)
            Scint_edge_Z_coord[it] = 0;
        if (it == 8)
            Scint_edge_Z_coord[it] = 6.4;
        if (it == 9)
            Scint_edge_Z_coord[it] = 13.3;
        if (it == 10)
            Scint_edge_Z_coord[it] = 20.4;
        if (it == 11)
            Scint_edge_Z_coord[it] = 27.7;
        if (it == 12)
            Scint_edge_Z_coord[it] = 34.6;
        if (it == 13)
            Scint_edge_Z_coord[it] = 42.2;
    }
    for (Int_t it = 0; it < Zdc_NSlises_in_module; it++) {   // 0-13
        if (it <= 4) {                                       // Pb part
            itInit_PLA = Scint_edge_Z_coord[it] - 2. * Zdc_Pb_Z_size_5mm - Zdc_PLA_Z_size_5mm;
            mother_volume_NICA->AddNode(PLAV_NICA, it + 2, new TGeoTranslation(0, 0, itInit_PLA));
            cout << "NICA it,PLA " << Scint_edge_Z_coord[it] << " " << 2. * Zdc_Pb_Z_size_5mm << " "
                 << Zdc_PLA_Z_size_5mm << " " << itInit_PLA << " " << it + 2 << endl;

            itInit_Pb = Scint_edge_Z_coord[it] - Zdc_Pb_Z_size_5mm;
            mother_volume_NICA->AddNode(PbV_NICA_5mm, it + 2, new TGeoTranslation(0, 0, itInit_Pb));
            cout << "NICA it,Pb " << itInit_Pb << " " << it + 2 << endl;

            itInit_Sc = Scint_edge_Z_coord[it] + Zdc_Sc_Z_size;
            mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA, it + 2, new TGeoTranslation(0, 0, itInit_Sc));
            cout << "NICA it,Sc " << itInit_Sc << " " << it + 2 << endl;

            itInit_G10 = Scint_edge_Z_coord[it] + 2. * Zdc_Sc_Z_size + Zdc_G10_Z_size;
            mother_volume_NICA->AddNode(
                G10V_NICA, it + 2, new TGeoTranslation(0, 0, itInit_G10));   // G10 - 1 -> after Veto
            cout << "NICA it,G10 " << itInit_G10 << " ===== "
                 << " " << it + 2 << endl;
        }   // Pb

        else {   // Cu part//it=5 - 13
            itInit_Pb = Scint_edge_Z_coord[it] - Zdc_Pb_Z_size_10mm;
            mother_volume_NICA->AddNode(PbV_NICA_10mm, it + 2, new TGeoTranslation(0, 0, itInit_Pb));
            cout << "NICA it,Cu " << itInit_Pb << " " << it + 2 << endl;

            itInit_Sc = Scint_edge_Z_coord[it] + Zdc_Sc_Z_size;
            mother_volume_NICA->AddNode(zdcSensitiveVolumeV_NICA, it + 2, new TGeoTranslation(0, 0, itInit_Sc));
            cout << "NICA it,Sc " << itInit_Sc << " " << it + 2 << endl;

            itInit_G10 = Scint_edge_Z_coord[it] + 2. * Zdc_Sc_Z_size + Zdc_G10_Z_size;
            mother_volume_NICA->AddNode(
                G10V_NICA, it + 2, new TGeoTranslation(0, 0, itInit_G10));   // G10 - 1 -> after Veto
            cout << "NICA it,G10 " << itInit_G10 << " ==== " << it + 2 << endl;

        }   // Cu part
    }       // for(Int_t it=0; it<Zdc_NSlises_in_module; it++)

    return 1;

}   // FillModule_NICA_simple
*/
