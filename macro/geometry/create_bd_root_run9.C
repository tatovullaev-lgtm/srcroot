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
const Double_t strip_BD_Zpos = -2.5;   // cm
// const Double_t strip_BD_Zpos = -3.5; //cm

const Double_t tubeFe_Xpos = 0.0;
const Double_t tubeFe_Ypos = 0.0;
const Double_t tubeFe_Zpos = -8.45;   // cm
// const Double_t tubeFe_Zpos = -9.45; //cm

const Double_t tubeAl_Xpos = 0.0;
const Double_t tubeAl_Ypos = 0.0;
const Double_t tubeAl_Zpos = -14.3;   // cm

const Double_t roh_Xpos = 0.0;
const Double_t roh_Ypos = 0.0;
const Double_t roh_Zpos = -2.65;   // cm

const Double_t ZLength_BDActiveVolume = 15.;
const Double_t ZLength_tubeFe = 29.5;
const Double_t ZLength_tubeAl = 18.;
const Double_t ZLength_roh = 5.3;

const Double_t dphi1 = 360. / 40;
const Double_t rr = 4.95;

const Double_t MaxRadiusFe = 8.1;
const Double_t MinRadiusFe = 8.;

const Double_t MaxRadiusAl = 7.0;
const Double_t MinRadiusAl = 6.8;

const Double_t MaxRadiusRohacel = 8.0;
const Double_t MinRadiusRohacel = 7.0;

const Double_t Seg_tube_Min_dim = 10.7;
const Double_t Seg_tube_Max_dim = 50.0;

const Double_t Seg_tube_Max_phi = 270.0;
const Double_t Seg_tube_Min_phi = 90.0;

// const Double_t Seg_tube_Thick = 1.0;
const Double_t Seg_tube_Thick = 0.6;

const Double_t MaxRadiusOfDiv = 10.699;
const Double_t MinRadiusOfDiv = 8.101;

void create_bd_root_run9()
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
    const TString geoDetectorName = "BD";
    const TString geoDetectorVersion = "run9";
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

    TGeoTranslation* BD_Position = new TGeoTranslation("BD_Position", 0, 0, 0);

    // Define TOP Geometry
    TGeoVolume* BD = new TGeoVolumeAssembly("BD");
    BD->SetMedium(pMedVac);

    TGeoTranslation* strips_BD1Pos_trans =
        new TGeoTranslation("strips_BD1Pos_trans", strip_BD_Xpos, strip_BD_Ypos, strip_BD_Zpos);
    TGeoTranslation* tubefePos_trans = new TGeoTranslation("tubefePos_trans", tubeFe_Xpos, tubeFe_Ypos, tubeFe_Zpos);
    TGeoTranslation* tubealPos_trans = new TGeoTranslation("tubealPos_trans", tubeAl_Xpos, tubeAl_Ypos, tubeAl_Zpos);
    TGeoTranslation* Postubepb_trans = new TGeoTranslation("Postubepb_trans", 0.0, 0.0, -4);
    // TGeoTranslation *Seg_tube_trans = new TGeoTranslation("Seg_tube_trans", 0, 0, 4.6);

    TGeoTranslation* trans_Pbring = new TGeoTranslation("trans_Pbring", 0, 0, 5.3);

    TGeoTranslation* Seg_tube_trans = new TGeoTranslation("Seg_tube_trans", -12.5, 0, 5.6);
    Seg_tube_trans->RegisterYourself();

    TGeoTranslation* pos_trans1 = new TGeoTranslation("pos_trans1", 0, 0, 5.6);
    pos_trans1->RegisterYourself();

    TGeoTranslation* Postube1 = new TGeoTranslation("Postube1", 0.0, 0.0, -13);

    TGeoBBox* strip = new TGeoBBox("srtip", 0.7 / 2., 0.7 / 2., 15. / 2.);
    TGeoTube* tubefeContainerS = new TGeoTube("tubefeContainerS", MinRadiusFe / 2, MaxRadiusFe / 2, ZLength_tubeFe / 2);
    TGeoTube* tubealContainerS = new TGeoTube("tubealContainerS", MinRadiusAl / 2, MaxRadiusAl / 2, ZLength_tubeAl / 2);
    TGeoTube* tubepbContainerS = new TGeoTube("tubepbContainerS", 7.4 / 2, 8 / 2, 18.8 / 2);
    // TGeoTubeSeg *Seg_tube = new TGeoTubeSeg("Seg_tube", Seg_tube_Min_dim/2, Seg_tube_Max_dim/2, Seg_tube_Thick/2,
    // Seg_tube_Min_phi, Seg_tube_Max_phi);

    TGeoBBox* Seg_tube = new TGeoBBox("Seg_tube", 25 / 2., 50 / 2., 0.5 / 2.);
    TGeoTube* tube1 = new TGeoTube("tube1", MinRadiusFe / 2, 18. / 2, 1. / 2);
    TGeoTube* tube22 = new TGeoTube("tube22", 0, 10.699 / 2, 5. / 2);

    TGeoTube* PbRing = new TGeoTube("PbRing", 8.101 / 2, 10.69 / 2, 0.5 / 2);

    TGeoCompositeShape* prot_form = new TGeoCompositeShape("prot_form", "Seg_tube:Seg_tube_trans - tube22:pos_trans1");

    Double_t phi = 90 * TMath::DegToRad(), delta_phi = 360. / 40., dphi = delta_phi * TMath::DegToRad();

    TGeoCombiTrans* combi[41];

    for (Int_t i = 1; i < 41; i++) {
        combi[i] = new TGeoCombiTrans(rr * TMath::Cos(dphi * (i + 0.5)), rr * TMath::Sin(dphi * (i + 0.5)),
                                      strip_BD_Zpos, new TGeoRotation("rot", dphi1 * (i + 0.5), 0., 0.));
    }

    TGeoVolume* BD1ActiveVolumeV1 = new TGeoVolume("BD1ActiveVolumeV1", strip);
    BD1ActiveVolumeV1->SetMedium(pMedBC408);
    BD1ActiveVolumeV1->SetLineColor(kYellow - 10);

    TGeoVolume* tubefeMat = new TGeoVolume("tubefeMat", tubefeContainerS);
    tubefeMat->SetMedium(pMediron);
    tubefeMat->SetLineColor(kOrange - 1);

    TGeoVolume* tubealMat = new TGeoVolume("tubealMat", tubealContainerS);
    tubealMat->SetMedium(pMedaluminium);
    tubealMat->SetLineColor(kCyan - 10);

    TGeoVolume* tubepb_prot = new TGeoVolume("tubepb_prot", tubepbContainerS);
    tubepb_prot->SetMedium(pMedlead);
    // tubepb_prot->SetMedium(pMedcopper);
    tubepb_prot->SetLineColor(kGray);

    TGeoVolume* seg_tube_prot = new TGeoVolume("seg_tube_prot", prot_form);
    seg_tube_prot->SetMedium(pMedlead);
    // seg_tube_prot->SetMedium(pMedcopper);
    seg_tube_prot->SetLineColor(kGray);

    TGeoVolume* tube1Mat = new TGeoVolume("tube1Mat", tube1);
    tube1Mat->SetMedium(pMediron);
    tube1Mat->SetLineColor(kOrange - 1);

    TGeoVolume* Pbring_prot = new TGeoVolume("Pbring_prot", PbRing);
    Pbring_prot->SetMedium(pMedlead);
    // Pbring_prot->SetMedium(pMedcopper);
    Pbring_prot->SetLineColor(kGray);

    for (Int_t i = 1; i < 41; i++) {
        BD->AddNode(BD1ActiveVolumeV1, i, combi[i]);
    }

    // BD->AddNode(tubefeMat, 41, tubefePos_trans);
    // BD->AddNode(tubealMat, 42, tubealPos_trans);

    BD->AddNode(Pbring_prot, 42, trans_Pbring);
    BD->AddNode(tubepb_prot, 43, Postubepb_trans);
    BD->AddNode(seg_tube_prot, 44);
    BD->AddNode(tube1Mat, 45, Postube1);

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
