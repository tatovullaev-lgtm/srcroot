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

const Double_t FD_Xpos = 0.0;
const Double_t FD_Ypos = 0.0;
const Double_t FD_Zpos = 453.8;   // cm

const Double_t MaxRadFD = 7.0;
const Double_t MinRadFD = 0.;
const Double_t Zthick_FD = 0.025;

const Double_t MaxRadtubeFD = 12.5;
const Double_t MinRadtubeFD = 12.35;
const Double_t Z_tube_FD = 20.;

const Double_t MaxRadtubeFD2 = 10.5;
const Double_t MinRadtubeFD2 = 10.35;
const Double_t Z_tube_FD2 = 30.;

const Double_t MaxR = 7.6;
const Double_t MinR = 7.3;
const Double_t Zl = 11.;

void create_fd_root_run9()
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
    const TString geoDetectorName = "FD";
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

    FairGeoMedium* msteel = geoMedia->getMedium("steel");
    if (!msteel)
        Fatal("Main", "FairMedium steel not found");
    geoBuild->createMedium(msteel);
    TGeoMedium* pMedsteel = gGeoMan->GetMedium("steel");
    if (!pMedsteel)
        Fatal("Main", "Medium steel  not found");

    FairGeoMedium* maluminium = geoMedia->getMedium("aluminium");
    if (!maluminium)
        Fatal("Main", "FairMedium aluminium   not found");
    geoBuild->createMedium(maluminium);
    TGeoMedium* pMedaluminium = gGeoMan->GetMedium("aluminium");
    if (!pMedaluminium)
        Fatal("Main", "Medium aluminium  not found");

    FairGeoMedium* mVac = geoMedia->getMedium("vacuum");
    if (!mVac)
        Fatal("Main", "FairMedium vacuum not found");
    geoBuild->createMedium(mVac);
    TGeoMedium* pMedVac = gGeoMan->GetMedium("vacuum");
    if (!pMedVac)
        Fatal("Main", "Medium vacuum not found");

    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);

    TGeoTranslation* FD_Position = new TGeoTranslation("FD_Position", FD_Xpos, FD_Ypos, FD_Zpos);

    TGeoTranslation* crystal_cord = new TGeoTranslation("crystal_cord", 0, 0, 0);
    crystal_cord->RegisterYourself();

    TGeoTranslation* sec_tube_cord = new TGeoTranslation("sec_tube_cord", 0, 0, 0);
    sec_tube_cord->RegisterYourself();

    TGeoCombiTrans* tube_cord = new TGeoCombiTrans("tube_cord", 0, -17, 0, new TGeoRotation("rot0", 0, 90, 0));
    tube_cord->RegisterYourself();

    TGeoCombiTrans* tube_cord1 = new TGeoCombiTrans("tube_cord1", 0, -5, 0, new TGeoRotation("rot0", 0, 90, 0));
    tube_cord1->RegisterYourself();

    TGeoCombiTrans* pmt_tube_cord = new TGeoCombiTrans("pmt_tube_cord", 0, -38, 0, new TGeoRotation("rot0", 0, 90, 0));
    pmt_tube_cord->RegisterYourself();

    TGeoTranslation* ring_cord1 = new TGeoTranslation("ring_cord1", 0, 0, -5.5);
    ring_cord1->RegisterYourself();

    TGeoTranslation* ring_cord2 = new TGeoTranslation("ring_cord2", 0, 0, 5.5);
    ring_cord2->RegisterYourself();

    // Define TOP Geometry
    TGeoVolume* FD = new TGeoVolumeAssembly("FD");
    FD->SetMedium(pMedVac);

    TGeoTube* fd_crystal = new TGeoTube("fd_crystal", MinRadFD / 2, MaxRadFD / 2, Zthick_FD / 2);
    TGeoTube* fd_section_tube = new TGeoTube("fd_section_tube", MinRadtubeFD / 2, MaxRadtubeFD / 2, Z_tube_FD / 2);
    TGeoTube* fd_tube = new TGeoTube("fd_tube", MinRadtubeFD2 / 2, MaxRadtubeFD2 / 2, Z_tube_FD2 / 2);

    TGeoTube* P_tube = new TGeoTube("P_tube", 0., MaxRadtubeFD / 2, Z_tube_FD / 2);
    TGeoTube* P_tube1 = new TGeoTube("P_tube1", 0., MaxRadtubeFD2 / 2, 10 / 2);

    TGeoTube* pmt_tube = new TGeoTube("pmt_tube", MinR / 2, MaxR / 2, Zl / 2);

    TGeoTube* ring1 = new TGeoTube("ring1", 5. / 2, 13. / 2, 1. / 2);
    TGeoTube* ring2 = new TGeoTube("ring2", 7.6 / 2, 12. / 2, 1. / 2);

    TGeoCompositeShape* FD_tube_sh = new TGeoCompositeShape("FD_tube_sh", "fd_tube:tube_cord- P_tube:sec_tube_cord");
    TGeoCompositeShape* FD_tube_sh1 =
        new TGeoCompositeShape("FD_tube_sh", "fd_section_tube:sec_tube_cord- P_tube1:tube_cord1");

    TGeoCompositeShape* pmt_tube_comp =
        new TGeoCompositeShape("pmt_tube_comp", "pmt_tube:sec_tube_cord + ring1:ring_cord1 + ring2:ring_cord2");

    TGeoVolume* FDActiveVolumeV = new TGeoVolume("FDActiveVolumeV", fd_crystal);
    FDActiveVolumeV->SetMedium(pMedBC408);
    FDActiveVolumeV->SetLineColor(kCyan - 2);

    TGeoVolume* FD_sec_tube_mat = new TGeoVolume("FD_sec_tube_mat", FD_tube_sh1);
    FD_sec_tube_mat->SetMedium(pMedaluminium);
    FD_sec_tube_mat->SetLineColor(kGray);

    TGeoVolume* FD_sec_tube_mat2 = new TGeoVolume("FD_sec_tube_mat2", FD_tube_sh);
    FD_sec_tube_mat2->SetMedium(pMedaluminium);
    FD_sec_tube_mat2->SetLineColor(kGray);

    TGeoVolume* FD_pmt_tube_mat = new TGeoVolume("FD_pmt_tube_mat", pmt_tube_comp);
    FD_pmt_tube_mat->SetMedium(pMedsteel);
    FD_pmt_tube_mat->SetLineColor(kGray + 3);

    FD->AddNode(FDActiveVolumeV, 1, crystal_cord);
    FD->AddNode(FD_sec_tube_mat, 2, sec_tube_cord);
    FD->AddNode(FD_sec_tube_mat2, 3, sec_tube_cord);
    FD->AddNode(FD_pmt_tube_mat, 4, pmt_tube_cord);

    // Adding volumes to the TOP Volume
    top->AddNode(FD, 1, FD_Position);
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
