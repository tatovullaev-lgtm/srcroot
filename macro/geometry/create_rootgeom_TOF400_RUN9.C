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

void create_rootgeom_TOF400_RUN9()
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
    const TString geoDetectorVersion = "RUN9";
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

    TGeoBBox* sPartСhannel1 = new TGeoBBox(3.6 / 2., 158. / 2., 0.8 / 2.);
    TGeoVolume* vPartСhannel1 = new TGeoVolume("vPartСhannel1", sPartСhannel1, medAl);
    vPartСhannel1->SetLineColor(kRed);

    TGeoBBox* sPartСhannel2 = new TGeoBBox(0.4 / 2., 158 / 2., 20. / 2.);
    TGeoVolume* vPartСhannel2 = new TGeoVolume("vPartСhannel2", sPartСhannel2, medAl);
    vPartСhannel2->SetLineColor(kGray);

    vChannel_lr->AddNode(vPartСhannel1, 1, new TGeoTranslation("", 2.2, 0., 1.));
    vChannel_lr->AddNode(vPartСhannel1, 2, new TGeoTranslation("", 2.2, 0., 19.2 + 1.));
    vChannel_lr->AddNode(vPartСhannel2, 1, new TGeoTranslation("", 0.2, 0., 10.6));

    vBox->AddNode(vChannel_lr, 1, new TGeoTranslation("", 93., 0., 0));
    vBox->AddNode(vChannel_lr, 2, new TGeoCombiTrans("", -93., 0., 0, new TGeoRotation("", 180, 0, 0)));

    //***

    TGeoVolumeAssembly* vChannel_ud = new TGeoVolumeAssembly("tof1ChannelV_ud");

    TGeoBBox* sPartСhannel3 = new TGeoBBox(194. / 2., 3.2 / 2., 0.8 / 2.);
    TGeoVolume* vPartСhannel3 = new TGeoVolume("vPartСhannel3", sPartСhannel3, medAl);
    vPartСhannel3->SetLineColor(kRed);

    TGeoBBox* sPartСhannel4 = new TGeoBBox(194. / 2., 0.8 / 2., 20. / 2.);
    TGeoVolume* vPartСhannel4 = new TGeoVolume("vPartСhannel4", sPartСhannel4, medAl);
    vPartСhannel4->SetLineColor(kGray);

    vChannel_ud->AddNode(vPartСhannel3, 1, new TGeoTranslation("", 0, 2.4, 1.));
    vChannel_ud->AddNode(vPartСhannel3, 1, new TGeoTranslation("", 0, 2.4, 19.2 + 1.));
    vChannel_ud->AddNode(vPartСhannel4, 2, new TGeoTranslation("", 0, 0.4, 10.6));

    // vBox->AddNode(vChannel_ud, 1, new TGeoTranslation("", 0, 0, 0));
    vBox->AddNode(vChannel_ud, 1, new TGeoTranslation("", 0, 158. / 2., 0));
    vBox->AddNode(vChannel_ud, 2, new TGeoCombiTrans("", 0, -158. / 2., 0, new TGeoRotation("", 180, 0, 0)));

    //*** covers

    TGeoBBox* sCover = new TGeoBBox(194. / 2., 166. / 2., 0.1 / 2.);
    TGeoVolume* vCover = new TGeoVolume("vCover", sCover, medAl);
    vCover->SetLineColor(kGreen);

    vBox->AddNode(vCover, 1, new TGeoTranslation("", 0, 0, 20 + 0.6 + 0.1 / 2.));
    vBox->AddNode(vCover, 2, new TGeoTranslation("", 0, 0, 0.6 - 0.1 / 2.));

    // --------------------- Create strip ------------------
    const float stripGasX = 1.25, stripGasY = DetYsize, stripGasZ = 0.3;   // [cm]
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

    // --------------------------------------- Assemble a detector

    // Add strips
    const int NStrips = 48;          //	24 strips group
    const double StripStep = 1.25;   // [cm]
    for (int i = 0; i < NStrips; i++) {
        vDetector->AddNode(vStripGas, i + 1,
                           new TGeoTranslation("", -DetXsize / 2. + StripStep / 2. + i * StripStep, 0., 0.));
    }

    // Add  Glass plates
    vDetector->AddNode(vDetGlass, 1, new TGeoTranslation("", 0., 0., -stripGasZ / 2. - detGlassZ / 2.));
    vDetector->AddNode(vDetGlass, 2, new TGeoTranslation("", 0., 0., +stripGasZ / 2. + detGlassZ / 2.));

    // Add  G10 plates
    vDetector->AddNode(vDetPlate, 1, new TGeoTranslation("", 0., 0., -DetZsize / 2. + detPlateZ / 2.));
    vDetector->AddNode(vDetPlate, 2, new TGeoTranslation("", 0., 0., +DetZsize / 2. - detPlateZ / 2.));

    // -------------------------------------- Install Detectors
    const double tofZgap = 0.4;       // 4 mm between detectors
    const double tofYoverlap = 3.0;   // 30 mm detectors overlap at Y direction
    const double tofZshift = (tofZgap + DetZsize) * -1.;

    // Box1, left side, X>0
    const double box1_x = 164.193;   // [cm]
    const double box1_y = -2.128;    // [cm]
    const double box1_z = 422.167;   // [cm]

    // column 1: det #0-4
    const double ofsetDetRelBox1_col1_x = +3.0 + 55.;
    const double ofsetDetRelBox1_col1_y = -2.25;
    const double ofsetDetRelBox1_col1_z = 15.0 + 2. * tofZshift;
    TGeoRotation* geoRot1 = new TGeoRotation();
    geoRot1->RotateX(0);    // 0.005 Deg
    geoRot1->RotateY(0);    //-0.055 Deg
    geoRot1->RotateZ(0.);   // 0.09 Deg
    const Double_t corr_X_col1[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Y_col1[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Z_col1[5] = {0, 0, 0, 0, 0};

    TGeoCombiTrans* cR1 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col1_x + corr_X_col1[0],
                           2. * DetYsize - 2 * tofYoverlap + box1_y + ofsetDetRelBox1_col1_y + corr_Y_col1[0],
                           box1_z + ofsetDetRelBox1_col1_z + corr_Z_col1[0], geoRot1);
    topTof->AddNode(vDetector, 1, cR1);

    TGeoCombiTrans* cR2 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col1_x + corr_X_col1[1],
                           1. * DetYsize - tofYoverlap + box1_y + ofsetDetRelBox1_col1_y + corr_Y_col1[1],
                           box1_z + ofsetDetRelBox1_col1_z + corr_Z_col1[1] + tofZshift, geoRot1);
    topTof->AddNode(vDetector, 2, cR2);

    TGeoCombiTrans* cR3 = new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col1_x + corr_X_col1[2],
                                             0. + box1_y + ofsetDetRelBox1_col1_y + corr_Y_col1[2],
                                             box1_z + ofsetDetRelBox1_col1_z + corr_Z_col1[2], geoRot1);
    topTof->AddNode(vDetector, 3, cR3);

    TGeoCombiTrans* cR4 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col1_x + corr_X_col1[3],
                           -1. * DetYsize + tofYoverlap + box1_y + ofsetDetRelBox1_col1_y + corr_Y_col1[3],
                           box1_z + ofsetDetRelBox1_col1_z + corr_Z_col1[3] + tofZshift, geoRot1);
    topTof->AddNode(vDetector, 4, cR4);

    TGeoCombiTrans* cR5 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col1_x + corr_X_col1[4],
                           -2. * DetYsize + 2 * tofYoverlap + box1_y + ofsetDetRelBox1_col1_y + corr_Y_col1[4],
                           box1_z + ofsetDetRelBox1_col1_z + corr_Z_col1[4], geoRot1);
    topTof->AddNode(vDetector, 5, cR5);

    // column 2: det #5-9
    const double ofsetDetRelBox1_col2_x = +3.0;
    const double ofsetDetRelBox1_col2_y = -2.25;
    const double ofsetDetRelBox1_col2_z = 15.0;
    TGeoRotation* geoRot2 = new TGeoRotation();
    geoRot2->RotateX(0);    // 0.005 Deg
    geoRot2->RotateY(0);    //-0.055 Deg
    geoRot2->RotateZ(0.);   // 0.09 Deg
    const Double_t corr_X_col2[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Y_col2[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Z_col2[5] = {0, 0, 0, 0, 0};

    TGeoCombiTrans* cR6 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col2_x + corr_X_col2[0],
                           2. * DetYsize - 2 * tofYoverlap + box1_y + ofsetDetRelBox1_col2_y + corr_Y_col2[0],
                           box1_z + ofsetDetRelBox1_col2_z + corr_Z_col2[0], geoRot1);
    topTof->AddNode(vDetector, 6, cR6);

    TGeoCombiTrans* cR7 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col2_x + corr_X_col2[1],
                           1. * DetYsize - tofYoverlap + box1_y + ofsetDetRelBox1_col2_y + corr_Y_col2[1],
                           box1_z + ofsetDetRelBox1_col2_z + corr_Z_col2[1] + tofZshift, geoRot1);
    topTof->AddNode(vDetector, 7, cR7);

    TGeoCombiTrans* cR8 = new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col2_x + corr_X_col2[2],
                                             0. + box1_y + ofsetDetRelBox1_col2_y + corr_Y_col2[2],
                                             box1_z + ofsetDetRelBox1_col2_z + corr_Z_col2[2], geoRot1);
    topTof->AddNode(vDetector, 8, cR8);

    TGeoCombiTrans* cR9 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col2_x + corr_X_col2[3],
                           -1. * DetYsize + tofYoverlap + box1_y + ofsetDetRelBox1_col2_y + corr_Y_col2[3],
                           box1_z + ofsetDetRelBox1_col2_z + corr_Z_col2[3] + tofZshift, geoRot1);
    topTof->AddNode(vDetector, 9, cR9);

    TGeoCombiTrans* cR10 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col2_x + corr_X_col2[4],
                           -2. * DetYsize + 2 * tofYoverlap + box1_y + ofsetDetRelBox1_col2_y + corr_Y_col2[4],
                           box1_z + ofsetDetRelBox1_col2_z + corr_Z_col2[4], geoRot1);
    topTof->AddNode(vDetector, 10, cR10);

    // column 3: det #10-14
    const double ofsetDetRelBox1_col3_x = +3.0 - 55.;
    const double ofsetDetRelBox1_col3_y = -2.25;
    const double ofsetDetRelBox1_col3_z = 15.0 + 2. * tofZshift;
    TGeoRotation* geoRot3 = new TGeoRotation();
    geoRot3->RotateX(0);    // 0.005 Deg
    geoRot3->RotateY(0);    //-0.055 Deg
    geoRot3->RotateZ(0.);   // 0.09 Deg
    const Double_t corr_X_col3[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Y_col3[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Z_col3[5] = {0, 0, 0, 0, 0};

    TGeoCombiTrans* cR11 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col3_x + corr_X_col3[0],
                           2. * DetYsize - 2 * tofYoverlap + box1_y + ofsetDetRelBox1_col3_y + corr_Y_col3[0],
                           box1_z + ofsetDetRelBox1_col3_z + corr_Z_col3[0], geoRot1);
    topTof->AddNode(vDetector, 11, cR11);

    TGeoCombiTrans* cR12 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col3_x + corr_X_col3[1],
                           1. * DetYsize - tofYoverlap + box1_y + ofsetDetRelBox1_col3_y + corr_Y_col3[1],
                           box1_z + ofsetDetRelBox1_col3_z + corr_Z_col3[1] + tofZshift, geoRot1);
    topTof->AddNode(vDetector, 12, cR12);

    TGeoCombiTrans* cR13 = new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col3_x + corr_X_col3[2],
                                              0. + box1_y + ofsetDetRelBox1_col3_y + corr_Y_col3[2],
                                              box1_z + ofsetDetRelBox1_col3_z + corr_Z_col3[2], geoRot1);
    topTof->AddNode(vDetector, 13, cR13);

    TGeoCombiTrans* cR14 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col3_x + corr_X_col3[3],
                           -1. * DetYsize + tofYoverlap + box1_y + ofsetDetRelBox1_col3_y + corr_Y_col3[3],
                           box1_z + ofsetDetRelBox1_col3_z + corr_Z_col3[3] + tofZshift, geoRot1);
    topTof->AddNode(vDetector, 14, cR14);

    TGeoCombiTrans* cR15 =
        new TGeoCombiTrans("", box1_x + ofsetDetRelBox1_col3_x + corr_X_col3[4],
                           -2. * DetYsize + 2 * tofYoverlap + box1_y + ofsetDetRelBox1_col3_y + corr_Y_col3[4],
                           box1_z + ofsetDetRelBox1_col3_z + corr_Z_col3[4], geoRot1);
    topTof->AddNode(vDetector, 15, cR15);

    // Box
    TGeoCombiTrans* cBox1 = new TGeoCombiTrans("", box1_x, box1_y, box1_z, geoRot1);
    topTof->AddNode(vBox, 1, cBox1);

    //-----------------------------
    // Box2, right side, X<0
    const double box2_x = -119.799;   // [cm]
    const double box2_y = -2.055;     // [cm]
    const double box2_z = 422.238;    // [cm]

    // column 4: det #15-19
    const double ofsetDetRelBox2_col4_x = -5.0 + 55.;
    const double ofsetDetRelBox2_col4_y = -2.25;
    const double ofsetDetRelBox2_col4_z = 15.0 + 2. * tofZshift;
    TGeoRotation* geoRot4 = new TGeoRotation();
    geoRot4->RotateX(0);    // 0.005 Deg
    geoRot4->RotateY(0);    //-0.055 Deg
    geoRot4->RotateZ(0.);   // 0.09 Deg
    const Double_t corr_X_col4[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Y_col4[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Z_col4[5] = {0, 0, 0, 0, 0};

    TGeoCombiTrans* cR16 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col4_x + corr_X_col4[0],
                           2. * DetYsize - 2 * tofYoverlap + box2_y + ofsetDetRelBox2_col4_y + corr_Y_col4[0],
                           box2_z + ofsetDetRelBox2_col4_z + corr_Z_col4[0], geoRot4);
    topTof->AddNode(vDetector, 16, cR16);

    TGeoCombiTrans* cR17 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col4_x + corr_X_col4[1],
                           1. * DetYsize - tofYoverlap + box2_y + ofsetDetRelBox2_col4_y + corr_Y_col4[1],
                           box2_z + ofsetDetRelBox2_col4_z + corr_Z_col4[1] + tofZshift, geoRot4);
    topTof->AddNode(vDetector, 17, cR17);

    TGeoCombiTrans* cR18 = new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col4_x + corr_X_col4[2],
                                              0. + box2_y + ofsetDetRelBox2_col4_y + corr_Y_col4[2],
                                              box2_z + ofsetDetRelBox2_col4_z + corr_Z_col4[2], geoRot4);
    topTof->AddNode(vDetector, 18, cR18);

    TGeoCombiTrans* cR19 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col4_x + corr_X_col4[3],
                           -1. * DetYsize + tofYoverlap + box2_y + ofsetDetRelBox2_col4_y + corr_Y_col4[3],
                           box2_z + ofsetDetRelBox2_col4_z + corr_Z_col4[3] + tofZshift, geoRot4);
    topTof->AddNode(vDetector, 19, cR19);

    TGeoCombiTrans* cR20 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col4_x + corr_X_col4[4],
                           -2. * DetYsize + 2 * tofYoverlap + box2_y + ofsetDetRelBox2_col4_y + corr_Y_col4[4],
                           box2_z + ofsetDetRelBox2_col4_z + corr_Z_col4[4], geoRot4);
    topTof->AddNode(vDetector, 20, cR20);

    // column 5: det #20-24
    const double ofsetDetRelBox2_col5_x = -5.0;
    const double ofsetDetRelBox2_col5_y = -2.25;
    const double ofsetDetRelBox2_col5_z = 15.0;
    TGeoRotation* geoRot5 = new TGeoRotation();
    geoRot5->RotateX(0);    // 0.005 Deg
    geoRot5->RotateY(0);    //-0.055 Deg
    geoRot5->RotateZ(0.);   // 0.09 Deg
    const Double_t corr_X_col5[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Y_col5[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Z_col5[5] = {0, 0, 0, 0, 0};

    TGeoCombiTrans* cR21 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col5_x + corr_X_col5[0],
                           2. * DetYsize - 2 * tofYoverlap + box2_y + ofsetDetRelBox2_col5_y + corr_Y_col5[0],
                           box2_z + ofsetDetRelBox2_col5_z + corr_Z_col5[0], geoRot5);
    topTof->AddNode(vDetector, 21, cR21);

    TGeoCombiTrans* cR22 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col5_x + corr_X_col5[1],
                           1. * DetYsize - tofYoverlap + box2_y + ofsetDetRelBox2_col5_y + corr_Y_col5[1],
                           box2_z + ofsetDetRelBox2_col5_z + corr_Z_col5[1] + tofZshift, geoRot5);
    topTof->AddNode(vDetector, 22, cR22);

    TGeoCombiTrans* cR23 = new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col5_x + corr_X_col5[2],
                                              0. + box2_y + ofsetDetRelBox2_col5_y + corr_Y_col5[2],
                                              box2_z + ofsetDetRelBox2_col5_z + corr_Z_col5[2], geoRot5);
    topTof->AddNode(vDetector, 23, cR23);

    TGeoCombiTrans* cR24 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col5_x + corr_X_col5[3],
                           -1. * DetYsize + tofYoverlap + box2_y + ofsetDetRelBox2_col5_y + corr_Y_col5[3],
                           box2_z + ofsetDetRelBox2_col5_z + corr_Z_col5[3] + tofZshift, geoRot5);
    topTof->AddNode(vDetector, 24, cR24);

    TGeoCombiTrans* cR25 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col5_x + corr_X_col5[4],
                           -2. * DetYsize + 2 * tofYoverlap + box2_y + ofsetDetRelBox2_col5_y + corr_Y_col5[4],
                           box2_z + ofsetDetRelBox2_col5_z + corr_Z_col5[4], geoRot5);
    topTof->AddNode(vDetector, 25, cR25);

    // column 6: det #25-29
    const double ofsetDetRelBox2_col6_x = -5.0 - 55.;
    const double ofsetDetRelBox2_col6_y = -2.25;
    const double ofsetDetRelBox2_col6_z = 15.0 + 2. * tofZshift;
    TGeoRotation* geoRot6 = new TGeoRotation();
    geoRot6->RotateX(0);    // 0.005 Deg
    geoRot6->RotateY(0);    //-0.055 Deg
    geoRot6->RotateZ(0.);   // 0.09 Deg
    const Double_t corr_X_col6[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Y_col6[5] = {0, 0, 0, 0, 0};
    const Double_t corr_Z_col6[5] = {0, 0, 0, 0, 0};

    TGeoCombiTrans* cR26 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col6_x + corr_X_col6[0],
                           2. * DetYsize - 2 * tofYoverlap + box2_y + ofsetDetRelBox2_col6_y + corr_Y_col6[0],
                           box2_z + ofsetDetRelBox2_col6_z + corr_Z_col6[0], geoRot6);
    topTof->AddNode(vDetector, 26, cR26);

    TGeoCombiTrans* cR27 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col6_x + corr_X_col6[1],
                           1. * DetYsize - tofYoverlap + box2_y + ofsetDetRelBox2_col6_y + corr_Y_col6[1],
                           box2_z + ofsetDetRelBox2_col6_z + corr_Z_col6[1] + tofZshift, geoRot6);
    topTof->AddNode(vDetector, 27, cR27);

    TGeoCombiTrans* cR28 = new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col6_x + corr_X_col6[2],
                                              0. + box2_y + ofsetDetRelBox2_col6_y + corr_Y_col6[2],
                                              box2_z + ofsetDetRelBox2_col6_z + corr_Z_col6[2], geoRot6);
    topTof->AddNode(vDetector, 28, cR28);

    TGeoCombiTrans* cR29 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col6_x + corr_X_col6[3],
                           -1. * DetYsize + tofYoverlap + box2_y + ofsetDetRelBox2_col6_y + corr_Y_col6[3],
                           box2_z + ofsetDetRelBox2_col6_z + corr_Z_col6[3] + tofZshift, geoRot6);
    topTof->AddNode(vDetector, 29, cR29);

    TGeoCombiTrans* cR30 =
        new TGeoCombiTrans("", box2_x + ofsetDetRelBox2_col6_x + corr_X_col6[4],
                           -2. * DetYsize + 2 * tofYoverlap + box2_y + ofsetDetRelBox2_col6_y + corr_Y_col6[4],
                           box2_z + ofsetDetRelBox2_col6_z + corr_Z_col6[4], geoRot6);
    topTof->AddNode(vDetector, 30, cR30);

    // Box
    TGeoCombiTrans* cBox2 = new TGeoCombiTrans("", box2_x, box2_y, box2_z, geoRot3);
    topTof->AddNode(vBox, 2, cBox2);

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
