#include "TGeoCompositeShape.h"
#include "TGeoCone.h"
#include "TGeoManager.h"
#include "TGeoPara.h"
#include "TGeoTrd2.h"
#include "TGeoTube.h"

#include <iomanip>
#include <iostream>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
//---------------------------
//---------------------------

TGeoManager* gGeoMan = NULL;

const Double_t strip_BD_Xpos = 0.0;
const Double_t strip_BD_Ypos = 0.0;
const Double_t strip_BD_Zpos = 0.0;   // cm
// const Double_t strip_BD_Zpos = -3.5; //cm

// const Double_t dphi1 = 360./40;
const Double_t rr = 7.55;

void create_bd64_root()
{

    // -------   Load media from media file   -----------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "root_geom_BD64";
    const TString geoDetectorVersion = "v1";
    TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_" + geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if (!mAir)
        Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if (!pMedAir)
        Fatal("Main", "Medium air not found");

    FairGeoMedium* mBC408 = geoMedia->getMedium("BC408");
    if (!mBC408)
        Fatal("Main", "FairMedium BC408 not found");
    geoBuild->createMedium(mBC408);
    TGeoMedium* pMedBC408 = gGeoMan->GetMedium("BC408");
    if (!pMedBC408)
        Fatal("Main", "Medium BC408 not found");

    FairGeoMedium* miron = geoMedia->getMedium("iron");
    if (!miron)
        Fatal("Main", "FairMedium iron   not found");
    geoBuild->createMedium(miron);
    TGeoMedium* pMediron = gGeoMan->GetMedium("iron");
    if (!pMediron)
        Fatal("Main", "Medium iron  not found");

    FairGeoMedium* maluminium = geoMedia->getMedium("aluminium");
    if (!maluminium)
        Fatal("Main", "FairMedium aluminium   not found");
    geoBuild->createMedium(maluminium);
    TGeoMedium* pMedaluminium = gGeoMan->GetMedium("aluminium");
    if (!pMedaluminium)
        Fatal("Main", "Medium aluminium  not found");

    FairGeoMedium* mpolypropylene = geoMedia->getMedium("polypropylene");
    if (!mpolypropylene)
        Fatal("Main", "FairMedium polypropylene   not found");
    geoBuild->createMedium(mpolypropylene);
    TGeoMedium* pMedpolypropylene = gGeoMan->GetMedium("polypropylene");
    if (!pMedpolypropylene)
        Fatal("Main", "Medium polypropylene  not found");

    FairGeoMedium* mlead = geoMedia->getMedium("lead");
    if (!mlead)
        Fatal("Main", "FairMedium lead   not found");
    geoBuild->createMedium(mlead);
    TGeoMedium* pMedlead = gGeoMan->GetMedium("lead");
    if (!pMedlead)
        Fatal("Main", "Medium lead  not found");

    FairGeoMedium* mVac = geoMedia->getMedium("vacuum");
    if (!mVac)
        Fatal("Main", "FairMedium vacuum not found");
    geoBuild->createMedium(mVac);
    TGeoMedium* pMedVac = gGeoMan->GetMedium("vacuum");
    if (!pMedVac)
        Fatal("Main", "Medium vacuum not found");

    FairGeoMedium* mcopper = geoMedia->getMedium("copper");
    if (!mcopper)
        Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mcopper);
    TGeoMedium* pMedcopper = gGeoMan->GetMedium("copper");
    if (!pMedcopper)
        Fatal("Main", "Medium copper not found");

    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);

    // --------------------------------------------------------------------------

    TGeoTranslation* BD_Position = new TGeoTranslation("BD_Position", 0, 0, 2.75);

    // Define TOP Geometry
    TGeoVolume* BD = new TGeoVolumeAssembly("BD");
    BD->SetMedium(pMedVac);

    TGeoTranslation* strips_BD1Pos_trans = new TGeoTranslation("strips_BD1Pos_trans", 0, 0, 0);
    TGeoTranslation* tubeCu_trans = new TGeoTranslation("tubeCu_trans", 0, 0, -0.8);
    TGeoTranslation* tubeCu_2_trans = new TGeoTranslation("tubeCu_2_trans", 0, 0, -2);
    // TGeoTranslation *tubeCu_2_trans = new TGeoTranslation("tubeCu_2_trans", 0, 0, -6);
    TGeoTranslation* tubeCu_3_trans = new TGeoTranslation("tubeCu_3_trans", 0, 0, -5.2);
    TGeoTranslation* tubeCu_4_trans = new TGeoTranslation("tubeCu_4_trans", 0, 0, 3.2);

    TGeoTranslation* Seg_tube_trans = new TGeoTranslation("Seg_tube_trans", -14.5, 0, 3.2);
    // TGeoTranslation *Seg_tube_trans = new TGeoTranslation("Seg_tube_trans", -25, 0, 2.5);
    Seg_tube_trans->RegisterYourself();

    TGeoTranslation* pos_trans1 = new TGeoTranslation("pos_trans1", 0, 0, 5);
    pos_trans1->RegisterYourself();

    TGeoTranslation* poss_trans = new TGeoTranslation("poss_trans", 14.5, 0, -4);

    TGeoBBox* strip = new TGeoBBox("srtip", 0.505 / 2., 0.705 / 2., 5.7 / 2.);
    TGeoTube* tubeCu = new TGeoTube("tubeCu", 12 / 2., 14 / 2., 8 / 2.);
    // TGeoTube *tubeCu = new TGeoTube("tubeCu", 13.4/2., 14.4/2., 6.5/2.);

    // TGeoTube *tubeCu_2 = new TGeoTube("tubeCu_2", 10.4/2., 11/2., 5/2.);
    TGeoTube* tubeCu_2 = new TGeoTube("tubeCu_2", 12.4 / 2., 13.4 / 2., 6 / 2.);
    // TGeoTube *tubeCu_2 = new TGeoTube("tubeCu_2", 10.4/2., 11/2., 10/2.);

    TGeoTube* tubeCu_3 = new TGeoTube("tubeCu_3", 11 / 2., 16 / 2., 0.5 / 2.);
    TGeoTube* tubeCu_4 = new TGeoTube("tubeCu_4", 14.4 / 2., 16 / 2., 0.5 / 2.);

    // TGeoTube *tube22 = new TGeoTube("tube22", 0, 15.8/2, 10./2);
    // TGeoBBox *Seg_tube = new TGeoBBox("Seg_tube", 20/2., 25/2., 0.5/2.);

    TGeoBBox* tube22 = new TGeoBBox("tube22", 30 / 2., 30 / 2., 30 / 2.);
    TGeoTube* Seg_tube = new TGeoTube("Seg_tube", 11 / 2., 12 / 2., 8 / 2.);

    TGeoCompositeShape* prot_form = new TGeoCompositeShape("prot_form", "Seg_tube:Seg_tube_trans - tube22:pos_trans1");

    Double_t phi = 90 * TMath::DegToRad(), delta_phi = 360. / 64., dphi = delta_phi * TMath::DegToRad();

    TGeoCombiTrans* combi[65];

    for (Int_t i = 1; i < 65; i++) {
        combi[i] = new TGeoCombiTrans(rr * TMath::Cos(dphi * (i + 0.5)), rr * TMath::Sin(dphi * (i + 0.5)),
                                      strip_BD_Zpos, new TGeoRotation("rot", delta_phi * (i + 0.5), 0., 0.));
    }

    TGeoVolume* BD1ActiveVolumeV1 = new TGeoVolume("BD1ActiveVolumeV1", strip);
    BD1ActiveVolumeV1->SetMedium(pMedBC408);
    BD1ActiveVolumeV1->SetLineColor(kCyan - 7);

    TGeoVolume* tubeCuMat = new TGeoVolume("tubeCuMat", tubeCu);
    tubeCuMat->SetMedium(pMedcopper);
    tubeCuMat->SetLineColor(kOrange + 1);

    TGeoVolume* tubeCuMat_2 = new TGeoVolume("tubeCuMat_2", tubeCu_2);
    tubeCuMat_2->SetMedium(pMedcopper);
    tubeCuMat_2->SetLineColor(kOrange + 1);

    TGeoVolume* tubeCuMat_3 = new TGeoVolume("tubeCuMat_3", tubeCu_3);
    tubeCuMat_3->SetMedium(pMedcopper);
    tubeCuMat_3->SetLineColor(kRed);

    TGeoVolume* tubeCuMat_4 = new TGeoVolume("tubeCuMat_4", tubeCu_4);
    tubeCuMat_4->SetMedium(pMedcopper);
    tubeCuMat_4->SetLineColor(kOrange + 1);

    TGeoVolume* seg_tube_prot = new TGeoVolume("seg_tube_prot", prot_form);
    seg_tube_prot->SetMedium(pMedcopper);
    // seg_tube_prot->SetLineColor(kOrange+1);
    seg_tube_prot->SetLineColor(kBlue);

    for (Int_t i = 1; i < 65; i++) {
        BD->AddNode(BD1ActiveVolumeV1, i, combi[i]);
    }

    BD->AddNode(tubeCuMat, 65, tubeCu_trans);
    // BD->AddNode(tubeCuMat_2, 66, tubeCu_2_trans);
    BD->AddNode(tubeCuMat_3, 67, tubeCu_3_trans);
    BD->AddNode(tubeCuMat_4, 68, tubeCu_4_trans);
    BD->AddNode(seg_tube_prot, 69, poss_trans);

    // Adding volumes to the TOP Volume

    top->AddNode(BD, 1, BD_Position);
    top->SetVisContainers(kTRUE);

    // ---------------   Finish   -----------------------------------------------
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
