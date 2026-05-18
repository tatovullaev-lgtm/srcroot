//Creating MAGNET SP57 geometry. Macro writes geometry to 'geometry/magnet_sp57.root'
#include "TGeoManager.h"
#include "TFile.h"
#include "TGeoMedium.h"
#include "TGeoXtru.h"
#include "TMath.h"

#include <iomanip>
#include <iostream>

R__ADD_INCLUDE_PATH($VMCWORKDIR)

using namespace TMath;
//---------------------------

TGeoManager* gGeoMan = NULL;

//--------Magnet position-------------------------------------------------------
    const double XMagnetPos = 0.5055 - 0.167 + 7.; // cm   x=0.5055 cm - Z SRC relative to Z BMN in X direction (D. Baranov),  x=-0.167 - coordinate of the mark in SRC coordinate system (A. Kolesnikov), x = 7 cm - center of the magnet relative to the mark in X direction (M. Patsyuk)
const double YMagnetPos = -4.62 - 20.53 + 5. + 19.; // cm  x=-4.62  cm - Z SRC relative to Z BMN in Y direction (D. Baranov),  x=-20.53 - coordinate of the mark in SRC coordinate system (A. Kolesnikov), x = 5+19 cm - center of the magnet relative to the mark in Y direction (S. Piyadin, technical drawing)
    const double ZMagnetPos = -576.2; //cm

//--------Magnet construct parameters-------------------------------------------

    //Yoke parameters:
    const double XSizeYoke = 300.0; //cm
    const double YSizeYoke = 211.0;
    const double ZSizeYoke = 120.0;

    const double XSizeInnerSpaceInYoke = 189.0; // x-inner size of empty space in yoke
    const double YSizeInnerSpaceInYoke = 112.0; // y-inner size of empty space in yoke
    const double ZSizeInnerSpaceInYoke = ZSizeYoke + 0.01; // z-inner size of empty space in yoke

    //Coil parameters:
    const double HCoil = 32.0;
    const double RCoil = 155.0/2.;
    const double YDistanceBetweenCoils = 48.0;

    //Pole parameters
    const double HPole = 5.0;
    const double RPole = 105.0/2.;
    const double YDistanceBetweenPoles = 38.0;

void create_rootgeom_MAGNET_SP57() {

    // -------   Load media from media file   -----------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoObjectName = "magnetSP57";
    const TString geoObjectVersion = "1";
    const TString geoFileName = geoPath + "/geometry/" + geoObjectName + "_"+ geoObjectVersion + ".root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");

    FairGeoMedium* mIron = geoMedia->getMedium("iron");
    if ( ! mIron ) Fatal("Main", "FairMedium iron not found");
    geoBuild->createMedium(mIron);
    TGeoMedium* pMedIron = gGeoMan->GetMedium("iron");
    if ( ! pMedIron ) Fatal("Main", "Medium iron not found");

    FairGeoMedium* mSteel = geoMedia->getMedium("steel");
    if ( ! mSteel ) Fatal("Main", "FairMedium steel not found");
    geoBuild->createMedium(mSteel);
    TGeoMedium* pMedSteel = gGeoMan->GetMedium("steel");
    if ( ! pMedSteel ) Fatal("Main", "Medium steel not found");

    //the structure of the SP57 coil includes copper and iron with pores for water cooling.
    // To simplify that we assume there is a copper with 0.75 density of real copper, which is called copperSP57.
    FairGeoMedium* mCopper = geoMedia->getMedium("copperSP75");
    if ( ! mCopper ) Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    TGeoMedium* pMedCopper = gGeoMan->GetMedium("copperSP75");
    if ( ! pMedCopper ) Fatal("Main", "Medium copper not found");

    // liquid hydrogen material for the target
    FairGeoMedium* mLH = geoMedia->getMedium("LiqHyd");
    if ( ! mLH ) Fatal("Main", "FairMedium liquid hydrogen not found");
    geoBuild->createMedium(mLH);
    TGeoMedium* pLiqHyd = gGeoMan->GetMedium("LiqHyd");
    if ( ! pLiqHyd ) Fatal("Main", "Medium liquid hydrogen not found");

    FairGeoMedium* mPolyurethane_ld = geoMedia->getMedium("polyurethane_ld");
    if ( ! mPolyurethane_ld ) Fatal("Main", "FairMedium polyurethane_ld not found");
    geoBuild->createMedium(mPolyurethane_ld);
    TGeoMedium* pPolyurethane_ld = gGeoMan->GetMedium("polyurethane_ld");
    if ( ! pPolyurethane_ld ) Fatal("Main", "Medium polyurethane_ld not found");

    FairGeoMedium* mMylar = geoMedia->getMedium("mylar");
    if ( ! mMylar ) Fatal("Main", "FairMedium mylar not found");
    geoBuild->createMedium(mMylar);
    TGeoMedium* pMylar = gGeoMan->GetMedium("mylar");
    if ( ! pMylar ) Fatal("Main", "Medium mylar not found");
    
    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoObjectName + "_geom");
    // gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);

    //---------------------------- TARGET VOLUME---------------------------------
    TGeoTranslation *t01 = new TGeoTranslation("t01", 0., 0., 11.25);
    TGeoTranslation *t02 = new TGeoTranslation("t02", 0., 0., -11.25);
    t01->RegisterYourself();
    t02->RegisterYourself();
    TGeoTube *TargetCylinder = new TGeoTube("TargetCylinder", 0., 2.9875, 13.5); // r = 30 - 0.125 mm
    
    TGeoSphere *TargetCup1 = new TGeoSphere("TargetCup1", 0, 3.7375, 0., 90., 0., 360.); // r = 37.5 - 0.125 mm
    TGeoSphere *TargetCup2 = new TGeoSphere("TargetCup2", 0, 3.7375, 90., 180., 0., 360.); // r = 37.5 - 0.125 mm
    TGeoTube *TargetCutCupBody = new TGeoTube("TargetCutCupBody", 0., 4. , 13.5); // r = 30 - 0.125 mm
    TargetCup1->SetName("TargetCup1"); 
    TargetCup2->SetName("TargetCup2"); 
    //TGeoCompositeShape *cupS = new TGeoCompositeShape("cupS", "(TargetS1:t01-TargetS2)");
    TGeoCompositeShape *targetS = new TGeoCompositeShape("targetS", "TargetCylinder+(TargetCup1:t01-TargetCutCupBody)+(TargetCup2:t02-TargetCutCupBody)");

    TGeoVolume *targetV = new TGeoVolume("targetV", targetS);
    targetV->SetMedium(pLiqHyd);
    //targetV->SetMedium(pMedSteel);
    targetV->SetLineColor(kRed);
    targetV->SetTransparency(60);

    //---------------------------- TARGET mylar shell---------------------------------
    
    TGeoTube *TargetMylarCylinder = new TGeoTube("TargetMylarCylinder", 2.9875, 3, 13.5); 
    
    TGeoSphere *TargetMylarCup1 = new TGeoSphere("TargetMylarCup1", 3.7375, 3.75, 0., 90., 0., 360.); // r = 37.5 - 0.125 mm
    TGeoSphere *TargetMylarCup2 = new TGeoSphere("TargetMylarCup2", 3.7375, 3.75, 90., 180., 0., 360.); // r = 37.5 - 0.125 mm
    
    TargetMylarCup1->SetName("TargetMylarCup1"); 
    TargetMylarCup2->SetName("TargetMylarCup2"); 
    //TGeoCompositeShape *cupS = new TGeoCompositeShape("cupS", "(TargetS1:t01-TargetS2)");
    TGeoCompositeShape *targetMylarS = new TGeoCompositeShape("targetMylarS", "TargetMylarCylinder+(TargetMylarCup1:t01-TargetCutCupBody)+(TargetMylarCup2:t02-TargetCutCupBody)");

    TGeoVolume *targetMylarV = new TGeoVolume("targetMylarV", targetMylarS);
    targetMylarV->SetMedium(pMylar);
    //targetV->SetMedium(pMedSteel);
    targetMylarV->SetLineColor(kBlue);
    targetMylarV->SetTransparency(60);

    //---------------------------- TARGET mylar window1---------------------------------
    TGeoTube *MylarWindow1S = new TGeoTube("MylarWindow1S", 0., 1, 0.005);
    TGeoVolume *MylarWindow1V = new TGeoVolume("MylarWindow1V", MylarWindow1S);
    MylarWindow1V->SetMedium(pMylar);
    //targetV->SetMedium(pMedSteel);
    MylarWindow1V->SetLineColor(kBlack);
    MylarWindow1V->SetTransparency(60);

    //---------------------------- TARGET mylar window2---------------------------------
    TGeoSphere *MylarWindow2S = new TGeoSphere("MylarWindow2S", 3.74, 3.75, 0., 90., 0., 360.);
    TGeoVolume *MylarWindow2V = new TGeoVolume("MylarWindow2V", MylarWindow2S);
    MylarWindow2V->SetMedium(pMylar);
    //targetV->SetMedium(pMedSteel);
    MylarWindow2V->SetLineColor(kBlack);
    MylarWindow2V->SetTransparency(60);

    //---------------------------- TARGET polyuretane shell---------------------------------

    t01 = new TGeoTranslation("t01", 0., 0., 6);
    t02 = new TGeoTranslation("t02", 0., 0., 6.7);
    t01->RegisterYourself();
    t02->RegisterYourself();
    TGeoSphere *TargetShell0S = new TGeoSphere("TargetShell0S", 5., 6.25, 0., 90., 0., 360.);
    TGeoTube *TargetShell3S = new TGeoTube("TargetShell3S", 5., 6.25, 12.5);
    TGeoTube *TargetShell1S = new TGeoTube("TargetShell1S", 0., 0.5, 7.);
    TGeoTube *TargetShell2S = new TGeoTube("TargetShell2S", 0.5, 1., 1.);
    
    TargetShell2S->SetName("TargetShell2S");   

    TGeoCompositeShape *TargetShellS = new TGeoCompositeShape("TargetShellS", "(TargetShell0S-TargetShell1S-(TargetShell2S:t02))");


    TGeoVolume *TargetShellV = new TGeoVolume("TargetShell", TargetShellS);
    TargetShellV->SetMedium(pPolyurethane_ld);
    //targetV->SetMedium(pMedSteel);
    TargetShellV->SetLineColor(kGreen);
    TargetShellV->SetTransparency(60);

    TGeoVolume *TargetShell3V = new TGeoVolume("TargetShell3V", TargetShell3S);
    TargetShell3V->SetMedium(pPolyurethane_ld);
    //targetV->SetMedium(pMedSteel);
    TargetShell3V->SetLineColor(kGreen);
    TargetShell3V->SetTransparency(60);

    

//-----------Solids (shapes)----------------------------------------------------

    TGeoBBox *YokeContainerS = new TGeoBBox("YokeContainerS", XSizeYoke/2., YSizeYoke/2., ZSizeYoke/2.);
    TGeoShape *InnerSpaceInYokeS = new TGeoBBox("InnerSpaceInYokeS", XSizeInnerSpaceInYoke/2, YSizeInnerSpaceInYoke/2, ZSizeInnerSpaceInYoke/2);
    TGeoCompositeShape *YokeS = new TGeoCompositeShape("YokeS", "YokeContainerS-InnerSpaceInYokeS");
    
    TGeoShape* CoilS = new TGeoTube("CoilS", 0., RCoil, HCoil/2.);
    TGeoRotation *rotCoil = new TGeoRotation("rotCoil", 180.0, 90.0, 0.0);

    TGeoShape* PoleS = new TGeoTube("PoleS", 0., RPole, HPole/2.);

   

//------------Volumes-----------------------------------------------------------
    TGeoVolume *MagnetContainerV = new TGeoVolumeAssembly("Magnet");
    MagnetContainerV->SetMedium(pMedAir);

    TGeoVolume *YokeV = new TGeoVolume("Yoke", YokeS);
    YokeV->SetMedium(pMedSteel);
    YokeV->SetLineColor(kGreen);
    //YokeV->SetVisLeaves(kTRUE);
    YokeV->SetTransparency(60);
    
    TGeoVolume *CoilV = new TGeoVolume("Coil", CoilS); // copper + steel, the coil has pores for water cooling. density = 0.75!!!!
    CoilV->SetMedium(pMedCopper);
    CoilV->SetLineColor(kRed);
    //CoilV->SetVisLeaves(kTRUE);
    CoilV->SetTransparency(60);
    
    TGeoVolume *PoleV = new TGeoVolume("Pole", PoleS);
    PoleV->SetMedium(pMedIron);
    PoleV->SetLineColor(kBlue);
    //PoleV->SetVisLeaves(kTRUE);
    PoleV->SetTransparency(60);
    
//--------------Adding volumes to the TOP Volume--------------------------------
    MagnetContainerV->AddNode(CoilV, 1, new TGeoCombiTrans(0, -(YDistanceBetweenCoils/2 + HCoil/2), 0, rotCoil));
    MagnetContainerV->AddNode(CoilV, 2, new TGeoCombiTrans(0,   YDistanceBetweenCoils/2 + HCoil/2,  0, rotCoil));

    MagnetContainerV->AddNode(PoleV, 1, new TGeoCombiTrans(0, -(YDistanceBetweenPoles/2 + HPole/2), 0, rotCoil));
    MagnetContainerV->AddNode(PoleV, 2, new TGeoCombiTrans(0,   YDistanceBetweenPoles/2 + HPole/2,  0, rotCoil));

    MagnetContainerV->AddNode(YokeV, 0);

    MagnetContainerV->AddNode(targetV, 0, new TGeoTranslation(0.5 - XMagnetPos, -4.6 - YMagnetPos, 0));
    MagnetContainerV->AddNode(TargetShellV, 0, new TGeoTranslation(0.5 - XMagnetPos, -4.6 - YMagnetPos, 0+14.15));
    MagnetContainerV->AddNode(TargetShell3V, 0, new TGeoTranslation(0.5 - XMagnetPos, -4.6 - YMagnetPos, 0+14.15-12.5));
    MagnetContainerV->AddNode(targetMylarV, 0, new TGeoTranslation(0.5 - XMagnetPos, -4.6 - YMagnetPos, 0));
    MagnetContainerV->AddNode(MylarWindow1V, 0, new TGeoTranslation(0.5 - XMagnetPos, -4.6 - YMagnetPos, 0+14.15+6.7));
    MagnetContainerV->AddNode(MylarWindow2V, 0, new TGeoTranslation(0.5 - XMagnetPos, -4.6 - YMagnetPos, 0-40.));
    
    top->AddNode(MagnetContainerV, 0, new TGeoTranslation(XMagnetPos, YMagnetPos, ZMagnetPos));

    //    top->AddNode(targetV, 0, new TGeoTranslation(0.5 ,-4.6, -647.));
    
    top->SetVisContainers(kTRUE);


    
 //---------------- Finish------------------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
    
    // top->Draw("");
}
