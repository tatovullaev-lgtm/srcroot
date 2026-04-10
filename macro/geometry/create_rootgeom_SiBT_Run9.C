/*
 * Use this macro to create basic ROOT geometry of Silicon Beam Tracker (SiBT)
 * for the RUN-9 configuration (3 stations)
 *
 * Author: Baranov D.
 * Created: 25.04.2025
 *
 * WARNING: all units is in cm!!!
 */

// Set Parameters of SiBT --------------------------------------------------------
const Int_t NStations = 3;     // stations in the detector
const Int_t NMaxModules = 1;   // max. number of modules in a station

// X-position of geometrical center of sensor
const Double_t XStationPositions[NStations] = {
    +0.002,   // st. 0 (+0.002 + 0.0)
    -0.258,   // st. 1 (+0.012 + (-0.27))
    +0.097    // st. 2 (+0.137 + (-0.04))
};

// Y-position of geometrical center of sensor
const Double_t YStationPositions[NStations] = {
    +0.084,   // st. 0 (-0.006 + 0.09)
    -0.003,   // st. 1 (+0.027 + (-0.03))
    +0.105    // st. 2 (+0.095 + 0.01)
};

// Z-position of sensor side, closest to the beam
const Double_t ZStationPositions[NStations] = {
    -283.39,    // st. 0 (-283.060 - 0.33)
    -183.386,   // st. 1 (-183.276 - 0.11)
    -84.397     // st. 2 (-84.087 - 0.31)
};

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {0.0},   // st. 0
    {0.0},   // st. 1
    {0.0}    // st. 2
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {0.0},   // st. 0
    {0.0},   // st. 1
    {0.0}    // st. 2
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {0.0},   // st. 0
    {0.0},   // st. 1
    {0.0}    // st. 2
};

// Rotations of stations around Z (deg.)
const Double_t ZStationRotations[NStations][NMaxModules] = {
    {+0.1020},   // st. 0 (-(90-90.102))
    {-30.005},   // st. 1 (-(90-59.995))
    {-59.944}    // st. 2 (-(90-30.560))
};

// Sizes of elements (cm) -------------------------------------------------------

// Sensor sizes
const Double_t XSensSize = 6.08;    // cm
const Double_t YSensSize = 6.08;    // cm
const Double_t ZSensSize = 0.018;   // cm
//------------------------------------------------------------------------------

// GeoManager
TGeoManager* gGeoMan = NULL;

// media
TGeoMedium* pMedAir = 0;
TGeoMedium* pMedCarbon = 0;
TGeoMedium* pMedFiberGlass = 0;
TGeoMedium* pMedSilicon = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume* CreateStation(TString station_name);
TGeoVolume* CreateModule(TString module_name);

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

    // carbon medium
    FairGeoMedium* mCarbon = geoMedia->getMedium("carbon");
    if (!mCarbon)
        Fatal("Main", "FairMedium carbon not found");
    geoBuild->createMedium(mCarbon);
    pMedCarbon = gGeoManager->GetMedium("carbon");
    if (!pMedCarbon)
        Fatal("Main", "Medium carbon not found");

    // fiberglass medium
    FairGeoMedium* mFiberGlass = geoMedia->getMedium("fiberglass");
    if (!mFiberGlass)
        Fatal("Main", "FairMedium fiberglass not found");
    geoBuild->createMedium(mFiberGlass);
    pMedFiberGlass = gGeoManager->GetMedium("fiberglass");
    if (!pMedFiberGlass)
        Fatal("Main", "Medium fiberglass not found");

    // silicon medium
    FairGeoMedium* mSilicon = geoMedia->getMedium("silicon");
    if (!mSilicon)
        Fatal("Main", "FairMedium silicon not found");
    geoBuild->createMedium(mSilicon);
    pMedSilicon = gGeoManager->GetMedium("silicon");
    if (!pMedSilicon)
        Fatal("Main", "Medium silicon not found");
}

void create_rootgeom_SiBT_Run9()
{

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "SiBT";
    const TString geoDetectorVersion = "Run9";
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
    TGeoVolume* SiBT = new TGeoVolumeAssembly(geoDetectorName);
    SiBT->SetMedium(pMedAir);

    for (Int_t istation = 0; istation < NStations; ++istation) {

        Int_t stationNum = istation;   // station number

        TGeoVolume* station = CreateStation(TString("station") + TString::Itoa(stationNum, 10));

        TGeoVolume* module0 = CreateModule(TString("module0_") + station->GetName());

        TGeoCombiTrans* module0_transform = new TGeoCombiTrans();
        module0_transform->RotateZ(ZStationRotations[stationNum][0]);
        module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0],
                                          ZModuleShifts[stationNum][0] + 0.5 * ZSensSize);

        TGeoCombiTrans* station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum],
                                          ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));

        SiBT->AddNode(station, 0, station_transform);
    }

    top->AddNode(SiBT, 0);
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

TGeoVolume* CreateStation(TString station_name)
{

    TGeoVolume* stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    return stationA;
}

TGeoVolume* CreateModule(TString module_name)
{

    // shapes
    TGeoShape* moduleS = new TGeoBBox(TString("moduleS") += TString("_") + module_name, XSensSize * 0.5,
                                      YSensSize * 0.5, ZSensSize * 0.5);

    // volumes
    TGeoVolume* moduleV = new TGeoVolume(TString("SiBTSensor") += TString("_") + module_name, moduleS);

    // medium
    if (pMedSilicon) {
        moduleV->SetMedium(pMedSilicon);
    } else
        Fatal("Main", "Invalid medium for moduleV volumes!");

    // volume visual property
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(30);

    return moduleV;
}
//------------------------------------------------------------------------------
