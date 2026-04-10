/*
 * Baranov D.
 * 28.01.2025 (created)
 *
 * Use this macro to create ROOT geometry for the Gas Beam Profilometer
 *
 */

// Detector position (x, y, z) --------------------------------------------------
const Double_t XDetectorPosition = +52.963;
const Double_t YDetectorPosition = -0.105;
const Double_t ZDetectorPosition = +830.880;
//------------------------------------------------------------------------------

// GeoManager
TGeoManager* gGeoMan = NULL;

// media
TGeoMedium* pMedAir = 0;
TGeoMedium* pMedMylar = 0;
TGeoMedium* pMedFiberGlass = 0;
TGeoMedium* pMedAluminium = 0;
TGeoMedium* pMedArCO27030 = 0;
TGeoMedium* pMedArC4H10_80_20 = 0;

class FairGeoMedia;
class FairGeoBuilder;

// Function declarations
TGeoVolume* CreateDetector(TString detector_name);

void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild)
{

    // air medium
    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if (!mAir)
        Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    pMedAir = gGeoManager->GetMedium("air");
    if (!pMedAir)
        Fatal("Main", "Medium air not found");

    // mylar medium
    FairGeoMedium* mMylar = geoMedia->getMedium("mylar");
    if (!mMylar)
        Fatal("Main", "FairMedium mylar not found");
    geoBuild->createMedium(mMylar);
    pMedMylar = gGeoManager->GetMedium("mylar");
    if (!pMedMylar)
        Fatal("Main", "Medium mylar not found");

    // fiberglass medium
    FairGeoMedium* mFiberGlass = geoMedia->getMedium("fiberglass");
    if (!mFiberGlass)
        Fatal("Main", "FairMedium fiberglass not found");
    geoBuild->createMedium(mFiberGlass);
    pMedFiberGlass = gGeoManager->GetMedium("fiberglass");
    if (!pMedFiberGlass)
        Fatal("Main", "Medium fiberglass not found");

    // aluminium medium
    FairGeoMedium* mAluminium = geoMedia->getMedium("aluminium");
    if (!mAluminium)
        Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAluminium);
    pMedAluminium = gGeoManager->GetMedium("aluminium");
    if (!pMedAluminium)
        Fatal("Main", "Medium aluminium not found");

    // arco27020 medium
    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if (!mArCO27030)
        Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    pMedArCO27030 = gGeoManager->GetMedium("arco27030");
    if (!pMedArCO27030)
        Fatal("Main", "Medium arco27030 not found");

    // ArC4H10_80_20 medium
    FairGeoMedium* mArC4H10_80_20 = geoMedia->getMedium("ArC4H10_80_20");
    if (!mArC4H10_80_20)
        Fatal("Main", "FairMedium ArC4H10_80_20 not found");
    geoBuild->createMedium(mArC4H10_80_20);
    pMedArC4H10_80_20 = gGeoManager->GetMedium("ArC4H10_80_20");
    if (!pMedArC4H10_80_20)
        Fatal("Main", "Medium ArC4H10_80_20 not found");
}

void create_rootgeom_GasBeamProf_Run8()
{

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ---------------------------------
    const TString geoDetectorName = "GasBeamProf";
    const TString geoDetectorVersion = "Run8";
    const TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + geoDetectorVersion + ".root";

    // ----  global geometry parameters  ---------------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();

    // -------   Load media from media file   ----------------------------------
    TString medFile = gPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    DefineRequiredMedia(geoMedia, geoBuild);
    // -------------------------------------------------------------------------

    // --------------   Create geometry and top volume  ------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    // gGeoMan->SetTopVisible(1);
    //  -------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* GASBEAMPROF = new TGeoVolumeAssembly(geoDetectorName);
    GASBEAMPROF->SetMedium(pMedAir);

    TGeoVolume* detector = CreateDetector(TString("detector_GEMBeamProf"));

    TGeoCombiTrans* detectorTransforms = new TGeoCombiTrans();
    detectorTransforms->SetTranslation(XDetectorPosition, YDetectorPosition, ZDetectorPosition);

    GASBEAMPROF->AddNode(detector, 0, detectorTransforms);

    top->AddNode(GASBEAMPROF, 0);
    top->SetVisContainers(kTRUE);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.0001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();
    // gGeoMan->RestoreMasterVolume();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
}
//------------------------------------------------------------------------------

TGeoVolume* CreateDetector(TString detector_name)
{

    TGeoVolume* detectorV = new TGeoVolumeAssembly(detector_name);
    detectorV->SetMedium(pMedAir);

    // gas  ---------------------------------------------------------------------
    Double_t gas_XSize = 20.0;   // cm
    Double_t gas_YSize = 20.0;   // cm
    Double_t gas_ZSize = 2.95;   // cm

    TGeoShape* gasS = new TGeoBBox("gasS", gas_XSize * 0.5, gas_YSize * 0.5, gas_ZSize * 0.5);

    TGeoVolume* gasV = new TGeoVolume(TString("gasV") += TString("_") + detectorV->GetName(), gasS);

    // volume medium
    TGeoMedium* gasV_medium = pMedArC4H10_80_20;
    if (gasV_medium) {
        gasV->SetMedium(gasV_medium);
    } else
        Fatal("Main", "Invalid medium for gasV!");

    // volume visual property (transparency)
    gasV->SetLineColor(TColor::GetColor("#ff47ca"));
    gasV->SetTransparency(50);

    TGeoCombiTrans* gasV_transf[1];

    gasV_transf[0] = new TGeoCombiTrans();
    gasV_transf[0]->SetDx(0.0);
    gasV_transf[0]->SetDy(0.0);
    gasV_transf[0]->SetDz(0.0);

    detectorV->AddNode(gasV, 0, gasV_transf[0]);
    //--------------------------------------------------------------------------

    // horizontal part of fiberglass frame --------------------------------------
    Double_t horizFGFrame_XSize = 25.0;   // cm
    Double_t horizFGFrame_YSize = 2.5;    // cm
    Double_t horizFGFrame_ZSize = 2.95;   // cm

    TGeoShape* horizFGFrameS =
        new TGeoBBox("horizFGFrameS", horizFGFrame_XSize * 0.5, horizFGFrame_YSize * 0.5, horizFGFrame_ZSize * 0.5);

    TGeoVolume* horizFGFrameV =
        new TGeoVolume(TString("horizFGFrameV") += TString("_") + detectorV->GetName(), horizFGFrameS);

    // volume medium
    TGeoMedium* horizFGFrameV_medium = pMedArC4H10_80_20;
    if (horizFGFrameV_medium) {
        horizFGFrameV->SetMedium(horizFGFrameV_medium);
    } else
        Fatal("Main", "Invalid medium for horizFGFrameV!");

    // volume visual property (transparency)
    horizFGFrameV->SetLineColor(TColor::GetColor("#ccffcc"));
    horizFGFrameV->SetTransparency(50);

    TGeoCombiTrans* horizFGFrameV_transf[2];

    horizFGFrameV_transf[0] = new TGeoCombiTrans();
    horizFGFrameV_transf[0]->SetDx(0.0);
    horizFGFrameV_transf[0]->SetDy(gas_YSize * 0.5 + horizFGFrame_YSize * 0.5);
    horizFGFrameV_transf[0]->SetDz(0.0);

    horizFGFrameV_transf[1] = new TGeoCombiTrans();
    horizFGFrameV_transf[1]->SetDx(0.0);
    horizFGFrameV_transf[1]->SetDy(-(gas_YSize * 0.5 + horizFGFrame_YSize * 0.5));
    horizFGFrameV_transf[1]->SetDz(0.0);

    detectorV->AddNode(horizFGFrameV, 0, horizFGFrameV_transf[0]);
    detectorV->AddNode(horizFGFrameV, 1, horizFGFrameV_transf[1]);
    //--------------------------------------------------------------------------

    // side part of fiberglass frame --------------------------------------------
    Double_t sideFGFrame_XSize = 2.5;    // cm
    Double_t sideFGFrame_YSize = 20.0;   // cm
    Double_t sideFGFrame_ZSize = 2.95;   // cm

    TGeoShape* sideFGFrameS =
        new TGeoBBox("sideFGFrameS", sideFGFrame_XSize * 0.5, sideFGFrame_YSize * 0.5, sideFGFrame_ZSize * 0.5);

    TGeoVolume* sideFGFrameV =
        new TGeoVolume(TString("sideFGFrameV") += TString("_") + detectorV->GetName(), sideFGFrameS);

    // volume medium
    TGeoMedium* sideFGFrameV_medium = pMedArC4H10_80_20;
    if (sideFGFrameV_medium) {
        sideFGFrameV->SetMedium(sideFGFrameV_medium);
    } else
        Fatal("Main", "Invalid medium for sideFGFrameV!");

    // volume visual property (transparency)
    sideFGFrameV->SetLineColor(TColor::GetColor("#ccffcc"));
    sideFGFrameV->SetTransparency(50);

    TGeoCombiTrans* sideFGFrameV_transf[2];

    sideFGFrameV_transf[0] = new TGeoCombiTrans();
    sideFGFrameV_transf[0]->SetDx(gas_XSize * 0.5 + sideFGFrame_XSize * 0.5);
    sideFGFrameV_transf[0]->SetDy(0.0);
    sideFGFrameV_transf[0]->SetDz(0.0);

    sideFGFrameV_transf[1] = new TGeoCombiTrans();
    sideFGFrameV_transf[1]->SetDx(-(gas_XSize * 0.5 + sideFGFrame_XSize * 0.5));
    sideFGFrameV_transf[1]->SetDy(0.0);
    sideFGFrameV_transf[1]->SetDz(0.0);

    detectorV->AddNode(sideFGFrameV, 0, sideFGFrameV_transf[0]);
    detectorV->AddNode(sideFGFrameV, 1, sideFGFrameV_transf[1]);
    //--------------------------------------------------------------------------

    // mylar film ---------------------------------------------------------------
    Double_t mylarFilm_XSize = 24.0;    // cm
    Double_t mylarFilm_YSize = 24.0;    // cm
    Double_t mylarFilm_ZSize = 0.005;   // cm

    TGeoShape* mylarFilmS =
        new TGeoBBox("mylarFilmS", mylarFilm_XSize * 0.5, mylarFilm_YSize * 0.5, mylarFilm_ZSize * 0.5);

    TGeoVolume* mylarFilmV = new TGeoVolume(TString("mylarFilmV") += TString("_") + detectorV->GetName(), mylarFilmS);

    // volume medium
    TGeoMedium* mylarFilmV_medium = pMedMylar;
    if (mylarFilmV_medium) {
        mylarFilmV->SetMedium(mylarFilmV_medium);
    } else
        Fatal("Main", "Invalid medium for mylarFilmV!");

    // volume visual property (transparency)
    mylarFilmV->SetLineColor(TColor::GetColor("#00e6e6"));
    mylarFilmV->SetTransparency(50);

    TGeoCombiTrans* mylarFilmV_transf[2];

    mylarFilmV_transf[0] = new TGeoCombiTrans();
    mylarFilmV_transf[0]->SetDx(0.0);
    mylarFilmV_transf[0]->SetDy(0.0);
    mylarFilmV_transf[0]->SetDz(gas_ZSize * 0.5 + mylarFilm_ZSize * 0.5);

    mylarFilmV_transf[1] = new TGeoCombiTrans();
    mylarFilmV_transf[1]->SetDx(0.0);
    mylarFilmV_transf[1]->SetDy(0.0);
    mylarFilmV_transf[1]->SetDz(-(gas_ZSize * 0.5 + mylarFilm_ZSize * 0.5));

    detectorV->AddNode(mylarFilmV, 0, mylarFilmV_transf[0]);
    detectorV->AddNode(mylarFilmV, 1, mylarFilmV_transf[1]);
    //--------------------------------------------------------------------------

    // horizontal part of aluminium frame ---------------------------------------
    Double_t horizAlumFrame_XSize = 27.0;   // cm
    Double_t horizAlumFrame_YSize = 2.0;    // cm
    Double_t horizAlumFrame_ZSize = 1.4;    // cm

    TGeoShape* horizAlumFrameS = new TGeoBBox("horizAlumFrameS", horizAlumFrame_XSize * 0.5, horizAlumFrame_YSize * 0.5,
                                              horizAlumFrame_ZSize * 0.5);

    TGeoVolume* horizAlumFrameV =
        new TGeoVolume(TString("horizAlumFrameV") += TString("_") + detectorV->GetName(), horizAlumFrameS);

    // volume medium
    TGeoMedium* horizAlumFrameV_medium = pMedAluminium;
    if (horizAlumFrameV_medium) {
        horizAlumFrameV->SetMedium(horizAlumFrameV_medium);
    } else
        Fatal("Main", "Invalid medium for horizAlumFrameV!");

    // volume visual property (transparency)
    horizAlumFrameV->SetLineColor(TColor::GetColor("#99ff99"));
    horizAlumFrameV->SetTransparency(50);

    TGeoCombiTrans* horizAlumFrameV_transf[4];

    horizAlumFrameV_transf[0] = new TGeoCombiTrans();
    horizAlumFrameV_transf[0]->SetDx(-(horizAlumFrame_XSize - horizFGFrame_XSize) * 0.5);
    horizAlumFrameV_transf[0]->SetDy((gas_YSize * 0.5 + horizFGFrame_YSize) - horizAlumFrame_YSize * 0.5);
    horizAlumFrameV_transf[0]->SetDz(gas_ZSize * 0.5 + mylarFilm_ZSize + horizAlumFrame_ZSize * 0.5);

    horizAlumFrameV_transf[1] = new TGeoCombiTrans();
    horizAlumFrameV_transf[1]->SetDx(-(horizAlumFrame_XSize - horizFGFrame_XSize) * 0.5);
    horizAlumFrameV_transf[1]->SetDy(-((gas_YSize * 0.5 + horizFGFrame_YSize) - horizAlumFrame_YSize * 0.5));
    horizAlumFrameV_transf[1]->SetDz(gas_ZSize * 0.5 + mylarFilm_ZSize + horizAlumFrame_ZSize * 0.5);

    horizAlumFrameV_transf[2] = new TGeoCombiTrans();
    horizAlumFrameV_transf[2]->SetDx(-(horizAlumFrame_XSize - horizFGFrame_XSize) * 0.5);
    horizAlumFrameV_transf[2]->SetDy((gas_YSize * 0.5 + horizFGFrame_YSize) - horizAlumFrame_YSize * 0.5);
    horizAlumFrameV_transf[2]->SetDz(-(gas_ZSize * 0.5 + mylarFilm_ZSize + horizAlumFrame_ZSize * 0.5));

    horizAlumFrameV_transf[3] = new TGeoCombiTrans();
    horizAlumFrameV_transf[3]->SetDx(-(horizAlumFrame_XSize - horizFGFrame_XSize) * 0.5);
    horizAlumFrameV_transf[3]->SetDy(-((gas_YSize * 0.5 + horizFGFrame_YSize) - horizAlumFrame_YSize * 0.5));
    horizAlumFrameV_transf[3]->SetDz(-(gas_ZSize * 0.5 + mylarFilm_ZSize + horizAlumFrame_ZSize * 0.5));

    detectorV->AddNode(horizAlumFrameV, 0, horizAlumFrameV_transf[0]);
    detectorV->AddNode(horizAlumFrameV, 1, horizAlumFrameV_transf[1]);
    detectorV->AddNode(horizAlumFrameV, 2, horizAlumFrameV_transf[2]);
    detectorV->AddNode(horizAlumFrameV, 3, horizAlumFrameV_transf[3]);
    //--------------------------------------------------------------------------

    // side part of aluminium frame ---------------------------------------------
    Double_t sideAlumFrame_XSize = 2.0;    // cm
    Double_t sideAlumFrame_YSize = 21.0;   // cm
    Double_t sideAlumFrame_ZSize = 1.4;    // cm

    TGeoShape* sideAlumFrameS =
        new TGeoBBox("sideAlumFrameS", sideAlumFrame_XSize * 0.5, sideAlumFrame_YSize * 0.5, sideAlumFrame_ZSize * 0.5);

    TGeoVolume* sideAlumFrameV =
        new TGeoVolume(TString("sideAlumFrameV") += TString("_") + detectorV->GetName(), sideAlumFrameS);

    // volume medium
    TGeoMedium* sideAlumFrameV_medium = pMedAluminium;
    if (sideAlumFrameV_medium) {
        sideAlumFrameV->SetMedium(sideAlumFrameV_medium);
    } else
        Fatal("Main", "Invalid medium for sideAlumFrameV!");

    // volume visual property (transparency)
    sideAlumFrameV->SetLineColor(TColor::GetColor("#99ff99"));
    sideAlumFrameV->SetTransparency(50);

    TGeoCombiTrans* sideAlumFrameV_transf[4];

    sideAlumFrameV_transf[0] = new TGeoCombiTrans();
    sideAlumFrameV_transf[0]->SetDx((gas_XSize * 0.5 + sideFGFrame_XSize) - sideAlumFrame_XSize * 0.5);
    sideAlumFrameV_transf[0]->SetDy(0.0);
    sideAlumFrameV_transf[0]->SetDz(gas_ZSize * 0.5 + mylarFilm_ZSize + sideAlumFrame_ZSize * 0.5);

    sideAlumFrameV_transf[1] = new TGeoCombiTrans();
    sideAlumFrameV_transf[1]->SetDx(-((gas_XSize * 0.5 + sideFGFrame_XSize) - sideAlumFrame_XSize * 0.5));
    sideAlumFrameV_transf[1]->SetDy(0.0);
    sideAlumFrameV_transf[1]->SetDz(gas_ZSize * 0.5 + mylarFilm_ZSize + sideAlumFrame_ZSize * 0.5);

    sideAlumFrameV_transf[2] = new TGeoCombiTrans();
    sideAlumFrameV_transf[2]->SetDx((gas_XSize * 0.5 + sideFGFrame_XSize) - sideAlumFrame_XSize * 0.5);
    sideAlumFrameV_transf[2]->SetDy(0.0);
    sideAlumFrameV_transf[2]->SetDz(-(gas_ZSize * 0.5 + mylarFilm_ZSize + sideAlumFrame_ZSize * 0.5));

    sideAlumFrameV_transf[3] = new TGeoCombiTrans();
    sideAlumFrameV_transf[3]->SetDx(-((gas_XSize * 0.5 + sideFGFrame_XSize) - sideAlumFrame_XSize * 0.5));
    sideAlumFrameV_transf[3]->SetDy(0.0);
    sideAlumFrameV_transf[3]->SetDz(-(gas_ZSize * 0.5 + mylarFilm_ZSize + sideAlumFrame_ZSize * 0.5));

    detectorV->AddNode(sideAlumFrameV, 0, sideAlumFrameV_transf[0]);
    detectorV->AddNode(sideAlumFrameV, 1, sideAlumFrameV_transf[1]);
    detectorV->AddNode(sideAlumFrameV, 2, sideAlumFrameV_transf[2]);
    detectorV->AddNode(sideAlumFrameV, 3, sideAlumFrameV_transf[3]);
    //--------------------------------------------------------------------------

    // aluminium PCB plank ------------------------------------------------------
    Double_t pcbPlank_XSize = 0.4;    // cm
    Double_t pcbPlank_YSize = 25.0;   // cm
    Double_t pcbPlank_ZSize = 5.8;    // cm

    TGeoShape* pcbPlankS = new TGeoBBox("pcbPlankS", pcbPlank_XSize * 0.5, pcbPlank_YSize * 0.5, pcbPlank_ZSize * 0.5);

    TGeoVolume* pcbPlankV = new TGeoVolume(TString("pcbPlankV") += TString("_") + detectorV->GetName(), pcbPlankS);

    // volume medium
    TGeoMedium* pcbPlankV_medium = pMedAluminium;
    if (pcbPlankV_medium) {
        pcbPlankV->SetMedium(pcbPlankV_medium);
    } else
        Fatal("Main", "Invalid medium for pcbPlankV!");

    // volume visual property (transparency)
    pcbPlankV->SetLineColor(TColor::GetColor("#99ff99"));
    pcbPlankV->SetTransparency(50);

    TGeoCombiTrans* pcbPlankV_transf[1];

    pcbPlankV_transf[0] = new TGeoCombiTrans();
    pcbPlankV_transf[0]->SetDx(
        -(horizAlumFrame_XSize * 0.5 + (horizAlumFrame_XSize - horizFGFrame_XSize) * 0.5 + pcbPlank_XSize * 0.5));
    pcbPlankV_transf[0]->SetDy(0.0);
    pcbPlankV_transf[0]->SetDz(0.0);

    detectorV->AddNode(pcbPlankV, 0, pcbPlankV_transf[0]);
    //--------------------------------------------------------------------------

    return detectorV;
}
//------------------------------------------------------------------------------