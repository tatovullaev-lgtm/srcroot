/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   create_rootgeom_TOF400_RUN8.C
 * Author: mikhailr
 *
 * Created on January 10, 2023, 11:06 AM
 */

R__ADD_INCLUDE_PATH($VMCWORKDIR)
// #include "macro/run/bmnloadlibs.C"

using namespace std;

void create_rootgeom_TOF400_RUN8()
{
    // Load the necessary FairRoot libraries
    //    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    //    bmnloadlibs(); // load libraries

    // -------   Load media from media file   --------------------------------
    TGeoManager* gGeoMan;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;

    // -------   Geometry file name (output)   -------------------------------
    const TString geoDetectorName = "TOF400";
    const TString geoDetectorVersion = "RUN8_v3";
    //   const TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_" + geoDetectorVersion + ".root";
    const TString geoFileName = geoDetectorName + "_" + geoDetectorVersion + ".root";

    // -----------------   Get and create the required media    --------------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    // Air
    FairGeoMedium* matAir = geoMedia->getMedium("air");
    if (!matAir)
        Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(matAir);
    TGeoMedium* medAir = gGeoMan->GetMedium("air");
    if (!medAir)
        Fatal("Main", "Medium air not found");

    // Polypropylene
    FairGeoMedium* matPprop = geoMedia->getMedium("polypropylene");
    if (!matPprop)
        Fatal("Main", "FairMedium polypropylene not found");
    geoBuild->createMedium(matPprop);
    TGeoMedium* medPprop = gGeoMan->GetMedium("polypropylene");
    if (!medPprop)
        Fatal("Main", "Medium polypropylene not found");

    // lead
    FairGeoMedium* matPb = geoMedia->getMedium("lead");
    if (!matPb)
        Fatal("Main", "FairMedium lead not found");
    geoBuild->createMedium(matPb);
    TGeoMedium* medPb = gGeoMan->GetMedium("lead");
    if (!medPb)
        Fatal("Main", "Medium lead not found");

    // aluminium
    FairGeoMedium* matAl = geoMedia->getMedium("aluminium");
    if (!matAl)
        Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(matAl);
    TGeoMedium* medAl = gGeoMan->GetMedium("aluminium");
    if (!medAl)
        Fatal("Main", "Medium aluminium not found");

    // RPCglass
    FairGeoMedium* matGlass = geoMedia->getMedium("RPCglass");
    if (!matGlass)
        Fatal("Main", "FairMedium RPCglass not found");
    geoBuild->createMedium(matGlass);
    TGeoMedium* medGlass = gGeoMan->GetMedium("RPCglass");
    if (!medGlass)
        Fatal("Main", "Medium RPCglass not found");

    // RPCgas
    FairGeoMedium* matGas = geoMedia->getMedium("RPCgas");
    if (!matGas)
        Fatal("Main", "FairMedium RPCgas not found");
    geoBuild->createMedium(matGas);
    TGeoMedium* medGas = gGeoMan->GetMedium("RPCgas");
    if (!medGas)
        Fatal("Main", "Medium RPCgas not found");

    // G10
    FairGeoMedium* matG10 = geoMedia->getMedium("G10");
    if (!matG10)
        Fatal("Main", "FairMedium G10 not found");
    geoBuild->createMedium(matG10);
    TGeoMedium* medG10 = gGeoMan->GetMedium("G10");
    if (!medG10)
        Fatal("Main", "Medium G10 not found");

    // -----------------------------------------   Create geometry and top volume
    // -----------------------------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");   // TOP VOLUME
    top->SetMedium(medAir);
    gGeoMan->SetTopVolume(top);
    gGeoMan->SetTopVisible(1);
    gGeoMan->SetVisLevel(7);

    // ---------------------------------------  Define TOP Geometry ------------------------------------
    TGeoVolume* topTof = new TGeoVolumeAssembly(geoDetectorName);
    topTof->SetMedium(medAir);

    TGeoVolume* vDetector = new TGeoVolumeAssembly("tof1Detector");
    vDetector->SetMedium(medAir);

    const double DetXsize = 60.;   // [cm]
    const double DetYsize = 30.;   // [cm]
    const double DetZsize = 3.0;   // [cm]

    TGeoVolume* vBox = new TGeoVolumeAssembly("tof1Box");
    vDetector->SetMedium(medAir);

    // --------------------- Create Box Frame --------------
    TGeoVolumeAssembly* vChannel_lr = new TGeoVolumeAssembly("tof1ChannelV_lr");

    TGeoBBox* sPartСhannel1 = new TGeoBBox(4. / 2., 160 / 2., 0.4 / 2.);
    TGeoVolume* vPartСhannel1 = new TGeoVolume("vPartСhannel1", sPartСhannel1, medAl);
    vPartСhannel1->SetLineColor(kRed);

    TGeoBBox* sPartСhannel2 = new TGeoBBox(0.4 / 2., 160 / 2., 8.4 / 2.);
    TGeoVolume* vPartСhannel2 = new TGeoVolume("vPartСhannel2", sPartСhannel2, medAl);
    vPartСhannel2->SetLineColor(kGray);

    vChannel_lr->AddNode(vPartСhannel2, 1, new TGeoTranslation("", 1.8, 0., 0.));
    vChannel_lr->AddNode(vPartСhannel2, 2, new TGeoTranslation("", -1.8, 0., 0.));
    vChannel_lr->AddNode(vPartСhannel1, 1, new TGeoTranslation("", 0, 0., -5.));
    vChannel_lr->AddNode(vPartСhannel1, 2, new TGeoTranslation("", 0, 0., 5.));

    vBox->AddNode(vChannel_lr, 1, new TGeoTranslation("", 33.5, 0., 0));
    vBox->AddNode(vChannel_lr, 2, new TGeoTranslation("", -33.5, 0., 0));

    //***

    TGeoVolumeAssembly* vChannel_ud = new TGeoVolumeAssembly("tof1ChannelV_ud");

    TGeoBBox* sPartСhannel3 = new TGeoBBox(71 / 2., 0.4 / 2., 10. / 2.);
    TGeoVolume* vPartСhannel3 = new TGeoVolume("vPartСhannel3", sPartСhannel3, medAl);
    vPartСhannel3->SetLineColor(kGreen);

    TGeoBBox* sPartСhannel4 = new TGeoBBox(71 / 2., 3.6 / 2., 0.4 / 2.);
    TGeoVolume* vPartСhannel4 = new TGeoVolume("vPartСhannel4", sPartСhannel4, medAl);
    vPartСhannel4->SetLineColor(kGreen);

    vChannel_ud->AddNode(vPartСhannel3, 1, new TGeoTranslation("", 0, -1.8, 0.));
    vChannel_ud->AddNode(vPartСhannel4, 1, new TGeoTranslation("", 0., 0.2, 5.));
    vChannel_ud->AddNode(vPartСhannel4, 2, new TGeoTranslation("", 0., 0.2, -5.));

    vBox->AddNode(vChannel_ud, 1, new TGeoTranslation("", 0, 160. / 2 + 2., 0));

    vBox->AddNode(vChannel_ud, 2, new TGeoCombiTrans("", 0, -160. / 2 - 2., 0, new TGeoRotation("", 180, 0, 0)));

    //*** covers

    TGeoBBox* sCover = new TGeoBBox(71 / 2., 160 / 2., 0.1 / 2.);
    TGeoVolume* vCover = new TGeoVolume("vCover", sCover, medAl);
    vCover->SetLineColor(kGray);

    vBox->AddNode(vCover, 1, new TGeoTranslation("", 0, 0, 5.1));
    vBox->AddNode(vCover, 2, new TGeoTranslation("", 0, 0, -5.1));

    // --------------------- Create strip ------------------
    const float stripGasX = 1., stripGasY = DetYsize, stripGasZ = 0.3;   // [cm]
    TGeoBBox* sStripGas = new TGeoBBox(stripGasX / 2., stripGasY / 2., stripGasZ / 2.);
    sStripGas->SetName("StripGas");
    TGeoVolume* vStripGas = new TGeoVolume("tof1StripActiveGasV", sStripGas, medGas);
    vStripGas->SetLineColor(kRed);

    // --------------------- Create Glass plates ------------------
    const float detGlassX = DetXsize, detGlassY = DetYsize, detGlassZ = 0.618 / 2.;   // [cm]  6.18 mm / 2 plate
    TGeoBBox* sDetGlass = new TGeoBBox(detGlassX / 2., detGlassY / 2., detGlassZ / 2.);
    sDetGlass->SetName("DetectorGlass");
    TGeoVolume* vDetGlass = new TGeoVolume("tof1DetGlassV", sDetGlass, medGlass);
    vDetGlass->SetLineColor(kYellow);

    // --------------------- Create G10 plates ------------------
    const float detPlateX = DetXsize, detPlateY = DetYsize, detPlateZ = 0.64 / 2.;   // [cm]  6.4 mm / 2 plate
    TGeoBBox* sDetPlate = new TGeoBBox(detPlateX / 2., detPlateY / 2., detPlateZ / 2.);
    sDetPlate->SetName("DetectorPlate");
    TGeoVolume* vDetPlate = new TGeoVolume("tof1DetPlateV", sDetPlate, medG10);
    vDetPlate->SetLineColor(kBlue);

    // --------------------------------------- Assemble detector

    // Add strips
    const int NStrips = 48;          //	24 strips group
    const double StripStep = 1.25;   // [cm]
    for (int i = 0; i < NStrips; i++) {
        vDetector->AddNode(
            vStripGas, i + 1, new TGeoTranslation("", -DetXsize / 2. + StripStep / 2. + i * StripStep, 0., 0.));
    }

    // Add  Glass plates
    vDetector->AddNode(vDetGlass, 1, new TGeoTranslation("", 0., 0., -stripGasZ / 2. - detGlassZ / 2.));
    vDetector->AddNode(vDetGlass, 2, new TGeoTranslation("", 0., 0., +stripGasZ / 2. + detGlassZ / 2.));

    // Add  G10 plates
    vDetector->AddNode(vDetPlate, 1, new TGeoTranslation("", 0., 0., -DetZsize / 2. + detPlateZ / 2.));
    vDetector->AddNode(vDetPlate, 2, new TGeoTranslation("", 0., 0., +DetZsize / 2. - detPlateZ / 2.));

    // -------------------------------------- Install Detectors
    const double tofZgap = 0.86;      // 8.6 mm between detectors
    const double tofYoverlap = 3.0;   // 30 mm detectors overlap at Y direction

    const double tofXgap = 0.;   // [cm] RUN5, part1
    // const double tofXgap = 2. * 24.4; // [cm] RUN5, part2

    double tofX = tofXgap / 2. + DetXsize / 2.;
    double tofZshift = tofZgap + DetZsize;

    // 1 box left side, 0 < X
    //    const double box1_x = 0; // [cm]
    //    const double box1_y = 0; // [cm]
    //    const double box1_z = 0; // [cm] :

    const double box1_x = 207.405;               // [cm]
    const double box1_y = 2.478 - 3.;            // [cm]
    const double box1_z = 434.286 - 2.5 + 6.5;   // [cm] :
    TGeoRotation* geoRot1 = new TGeoRotation();
    geoRot1->RotateX(0);    // 0.005 Deg
    geoRot1->RotateY(0);    //-0.055 Deg
    geoRot1->RotateZ(0.);   // 0.09 Deg
    const Double_t corr_X_box1[5] = {-0.43, -0.115, -0.267, -0.323, -1.427};
    const Double_t corr_Y_box1[5] = {0.123, 0.269, 0.26, 0.521, 1.107};
    const Double_t corr_Z_box1[5] = {-1.203, 0.057, 0.447, 0.58, -0.788};

    TGeoCombiTrans* cR1 = new TGeoCombiTrans("",
                                             box1_x + corr_X_box1[0],
                                             2. * DetYsize - 2 * tofYoverlap + box1_y + corr_Y_box1[0],
                                             box1_z + corr_Z_box1[0],
                                             geoRot1);
    topTof->AddNode(vDetector, 1, cR1);

    TGeoCombiTrans* cR2 = new TGeoCombiTrans("",
                                             box1_x + corr_X_box1[1],
                                             1. * DetYsize - tofYoverlap + box1_y + corr_Y_box1[1],
                                             box1_z + tofZshift + corr_Z_box1[1],
                                             geoRot1);
    topTof->AddNode(vDetector, 2, cR2);

    TGeoCombiTrans* cR3 =
        new TGeoCombiTrans("", box1_x + corr_X_box1[2], 0 + box1_y + corr_Y_box1[2], box1_z + corr_Z_box1[2], geoRot1);
    topTof->AddNode(vDetector, 3, cR3);

    TGeoCombiTrans* cR4 = new TGeoCombiTrans("",
                                             box1_x + corr_X_box1[3],
                                             -1. * DetYsize + tofYoverlap + box1_y + corr_Y_box1[3],
                                             box1_z + tofZshift + corr_Z_box1[3],
                                             geoRot1);
    topTof->AddNode(vDetector, 4, cR4);

    TGeoCombiTrans* cR5 = new TGeoCombiTrans("",
                                             box1_x + corr_X_box1[4],
                                             -2. * DetYsize + 2 * tofYoverlap + box1_y + corr_Y_box1[4],
                                             box1_z + corr_Z_box1[4],
                                             geoRot1);
    topTof->AddNode(vDetector, 5, cR5);

    TGeoCombiTrans* cBox1 = new TGeoCombiTrans("", box1_x, box1_y + 3., box1_z + 2.9 - 1.3, geoRot1);
    topTof->AddNode(vBox, 1, cBox1);

    // 2 box left side, 0 < x < box1_x
    const double box2_x = 150.04;                // [cm]
    const double box2_y = 3.163 - 3.;            // [cm]
    const double box2_z = 421.161 - 2.5 + 6.5;   // [cm] :
    TGeoRotation* geoRot2 = new TGeoRotation;
    geoRot2->RotateX(0);
    geoRot2->RotateY(0);
    geoRot2->RotateZ(0);
    const Double_t corr_X_box2[5] = {0.926, 0.717, 0.067, -0.579, -0.178};
    const Double_t corr_Y_box2[5] = {-0.293, -0.299, -0.204, -0.143, 0.948};
    const Double_t corr_Z_box2[5] = {1.61, 1.22, 0.57, -0.083, 0.922};

    TGeoCombiTrans* cR6 = new TGeoCombiTrans("",
                                             box2_x + corr_X_box2[0],
                                             2. * DetYsize - 2 * tofYoverlap + box2_y + corr_Y_box2[0],
                                             box2_z + corr_Z_box2[0],
                                             geoRot2);
    topTof->AddNode(vDetector, 6, cR6);

    TGeoCombiTrans* cR7 = new TGeoCombiTrans("",
                                             box2_x + corr_X_box2[1],
                                             1. * DetYsize - tofYoverlap + box2_y + corr_Y_box2[1],
                                             box2_z + tofZshift + corr_Z_box2[1],
                                             geoRot2);
    topTof->AddNode(vDetector, 7, cR7);

    TGeoCombiTrans* cR8 =
        new TGeoCombiTrans("", box2_x + corr_X_box2[2], 0 + box2_y + corr_Y_box2[2], box2_z + corr_Z_box2[2], geoRot2);
    topTof->AddNode(vDetector, 8, cR8);

    TGeoCombiTrans* cR9 = new TGeoCombiTrans("",
                                             box2_x + corr_X_box2[3],
                                             -1. * DetYsize + tofYoverlap + box2_y + corr_Y_box2[3],
                                             box2_z + tofZshift + corr_Z_box2[3],
                                             geoRot2);
    topTof->AddNode(vDetector, 9, cR9);

    TGeoCombiTrans* cR10 = new TGeoCombiTrans("",
                                              box2_x + corr_X_box2[4],
                                              -2. * DetYsize + 2 * tofYoverlap + box2_y + corr_Y_box2[4],
                                              box2_z + corr_Z_box2[4],
                                              geoRot2);
    topTof->AddNode(vDetector, 10, cR10);

    TGeoCombiTrans* cBox2 = new TGeoCombiTrans("", box2_x, box2_y + 3., box2_z + 2.9 - 1.3 + 0.2, geoRot2);
    topTof->AddNode(vBox, 2, cBox2);

    // 3 box left side, x < 0
    const double box3_x = -94.244;               // [cm]
    const double box3_y = 3.583 - 3.;            // [cm]
    const double box3_z = 421.065 - 2.5 + 6.5;   // [cm] :
    TGeoRotation* geoRot3 = new TGeoRotation;
    geoRot3->RotateX(0);
    geoRot3->RotateY(0);
    geoRot3->RotateZ(0.);   // 0.07 deg
    const Double_t corr_X_box3[5] = {-0.078, 0.016, -0.043, 0.124, -0.211};
    const Double_t corr_Y_box3[5] = {1.673, 1.11, 1.815, 1.481, 2.184};
    const Double_t corr_Z_box3[5] = {1.04, 0.967, 1.427, 0.34, 0.748};

    TGeoCombiTrans* cR11 = new TGeoCombiTrans("",
                                              box3_x + corr_X_box3[0],
                                              2. * DetYsize - 2 * tofYoverlap + box3_y + corr_Y_box3[0],
                                              box3_z + corr_Z_box3[0],
                                              geoRot3);
    topTof->AddNode(vDetector, 11, cR11);

    TGeoCombiTrans* cR12 = new TGeoCombiTrans("",
                                              box3_x + corr_X_box3[1],
                                              1. * DetYsize - tofYoverlap + box3_y + corr_Y_box3[1],
                                              box3_z + tofZshift + corr_Z_box3[1],
                                              geoRot3);
    topTof->AddNode(vDetector, 12, cR12);

    TGeoCombiTrans* cR13 =
        new TGeoCombiTrans("", box3_x + corr_X_box3[2], 0 + box3_y + corr_Y_box3[2], box3_z + corr_Z_box3[2], geoRot3);
    topTof->AddNode(vDetector, 13, cR13);

    TGeoCombiTrans* cR14 = new TGeoCombiTrans("",
                                              box3_x + corr_X_box3[3],
                                              -1. * DetYsize + tofYoverlap + box3_y + corr_Y_box3[3],
                                              box3_z + tofZshift + corr_Z_box3[3],
                                              geoRot3);
    topTof->AddNode(vDetector, 14, cR14);

    TGeoCombiTrans* cR15 = new TGeoCombiTrans("",
                                              box3_x + corr_X_box3[4],
                                              -2. * DetYsize + 2 * tofYoverlap + box3_y + corr_Y_box3[4],
                                              box3_z + corr_Z_box3[4],
                                              geoRot3);
    topTof->AddNode(vDetector, 15, cR15);

    TGeoCombiTrans* cBox3 = new TGeoCombiTrans("", box3_x, box3_y + 3., box3_z + 2.9 - 1.3 + 0.2, geoRot3);
    topTof->AddNode(vBox, 3, cBox3);

    // 4 box left side, box3 < x < 0
    const double box4_x = -150.210;              // [cm]
    const double box4_y = 2.588 - 3. + 1.24;     // [cm]
    const double box4_z = 434.650 - 2.5 + 6.5;   // [cm] :
    TGeoRotation* geoRot4 = new TGeoRotation;
    geoRot4->RotateX(0);
    geoRot4->RotateY(0);    // 0.016
    geoRot4->RotateZ(0.);   // 0.0755
    const Double_t corr_X_box4[5] = {0.025, 0.189, 0.222, 0.200, 0.123};
    const Double_t corr_Y_box4[5] = {
        1.167 + 1.35 - 1.24, 1.262 + 1.27 - 1.24, 1.258 + 1.1 - 1.24, 1.793 + 1.17 - 1.24, 1.376 + 1.32 - 1.24};
    const Double_t corr_Z_box4[5] = {-0.916, -1.048, -1.096, -1.096, -1.657};

    TGeoCombiTrans* cR16 = new TGeoCombiTrans("",
                                              box4_x + corr_X_box4[0],
                                              2. * DetYsize - 2 * tofYoverlap + box4_y + corr_Y_box4[0],
                                              box4_z + corr_Z_box4[0],
                                              geoRot4);
    topTof->AddNode(vDetector, 16, cR16);

    TGeoCombiTrans* cR17 = new TGeoCombiTrans("",
                                              box4_x + corr_X_box4[1],
                                              1. * DetYsize - tofYoverlap + box4_y + corr_Y_box4[1],
                                              box4_z + tofZshift + corr_Z_box4[1],
                                              geoRot4);
    topTof->AddNode(vDetector, 17, cR17);

    TGeoCombiTrans* cR18 =
        new TGeoCombiTrans("", box4_x + corr_X_box4[2], 0 + box4_y + corr_Y_box4[2], box4_z + corr_Z_box4[2], geoRot4);
    topTof->AddNode(vDetector, 18, cR18);

    TGeoCombiTrans* cR19 = new TGeoCombiTrans("",
                                              box4_x + corr_X_box4[3],
                                              -1. * DetYsize + tofYoverlap + box4_y + corr_Y_box4[3],
                                              box4_z + tofZshift + corr_Z_box4[3],
                                              geoRot4);
    topTof->AddNode(vDetector, 19, cR19);

    TGeoCombiTrans* cR20 = new TGeoCombiTrans("",
                                              box4_x + corr_X_box4[4],
                                              -2. * DetYsize + 2 * tofYoverlap + box4_y + corr_Y_box4[4],
                                              box4_z + corr_Z_box4[4],
                                              geoRot4);
    topTof->AddNode(vDetector, 20, cR20);

    TGeoCombiTrans* cBox4 = new TGeoCombiTrans("", box4_x, box4_y + 3., box4_z + 2.9 - 1.3, geoRot4);
    topTof->AddNode(vBox, 4, cBox4);

    //*/
    // ------------------  Adding  the TOP Volume --------------------------
    top->AddNode(topTof, 1, new TGeoTranslation("tw", 0., 0., 0.));

    // ---------------   Finish   --------------------------------------------
    top->SetVisContainers(kTRUE);

    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();

    top->Draw("ogl");
    gGeoMan->Browse(new TBrowser);
}
