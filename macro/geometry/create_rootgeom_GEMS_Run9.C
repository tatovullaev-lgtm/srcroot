/*
 * Basic ROOT geometry for the configuration of GEM detector in RUN-9
 * (only sensor volumes)
 *
 * Author: Baranov D.
 * Date: 07.04.2025 (created)
 *       22.04.2025 (st.3 z-pos. corrected)
 */

// Set Parameters of GEMS -------------------------------------------------------
const Int_t NStations = 8;     // stations in the detector
const Int_t NMaxModules = 4;   // max. number of modules in a station

// X-position of the hole center for the plane closest to the target (in a station)
const Double_t XStationPositions[NStations] = {
    +1.419,   // 0 st. (M15)
    +2.529,   // 1 st. (M10)
    +3.408,   // 2 st. (M14)
    +4.697,   // 3 st. (M8)
    +6.335,   // 4 st. (M16)
    +8.213,   // 5 st. (M7)
    +9.784,   // 6 st. (M12)
    +50.494   // 7 st. (small GEM)
};

// Y-position of the hole center for the plane closest to the target (in a station)
const Double_t YStationPositions[NStations] = {
    -0.032,   // 0 st. (M15)
    +0.123,   // 1 st. (M10)
    +0.026,   // 2 st. (M14)
    -0.013,   // 3 st. (M8),
    -0.026,   // 4 st. (M16)
    +0.058,   // 5 st. (M7)
    -0.048,   // 6 st. (M12)
    -0.186    // 7 st. (small GEM)
};

// Z-position of gas volume for the plane closest to the target (in a station)
const Double_t ZStationPositions[NStations] = {
    +61.107,    // 0 st. (M15)
    +92.313,    // 1 st. (M10)
    +123.541,   // 2 st. (M14)
    +154.778,   // 3 st. (M8)
    +185.458,   // 4 st. (M16)
    +217.141,   // 5 st. (M7)
    +248.367,   // 6 st. (M12)
    +792.532    // 7 st. (small GEM)
};

// Shifts for the second plane in a station relative to the closest half-plane
const Double_t xShift[NStations] = {
    +0.111,   // 0 st. (M6)
    +0.142,   // 1 st. (M13)
    +0.081,   // 2 st. (M11)
    +0.100,   // 3 st. (M18)
    +0.092,   // 4 st. (M5)
    +0.113,   // 5 st. (M17)
    +0.155,   // 6 st. (M9)
    +0.0      // 7 st. (small GEM)
};
const Double_t yShift[NStations] = {
    +0.008,   // 0 st. (M6)
    -0.100,   // 1 st. (M13)
    +0.048,   // 2 st. (M11)
    +0.042,   // 3 st. (M18)
    +0.109,   // 4 st. (M5)
    -0.089,   // 5 st. (M17)
    +0.168,   // 6 st. (M9)
    +0.0      // 7 st. (small GEM)
};
const Double_t zShift[NStations] = {
    +4.691,   // 0 st. (M6)
    +4.158,   // 1 st. (M13)
    +4.672,   // 2 st. (M11)
    +4.124,   // 3 st. (M18)
    +4.683,   // 4 st. (M5)
    +3.995,   // 5 st. (M17)
    +4.063,   // 6 st. (M9)
    +0.0      // 7 st. (small GEM)
};

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {163.2 * 0.25 + xShift[0], -163.2 * 0.25 + xShift[0],   // 0 st. (upper h. plane)
     163.2 * 0.25, -163.2 * 0.25},                          // 0 st. (lower h. plane)

    {163.2 * 0.25, -163.2 * 0.25,                            // 1 st. (upper h. plane)
     163.2 * 0.25 + xShift[1], -163.2 * 0.25 + xShift[1]},   // 1 st. (lower h. plane)

    {163.2 * 0.25 + xShift[2], -163.2 * 0.25 + xShift[2],   // 2 st. (upper h. plane)
     163.2 * 0.25, -163.2 * 0.25},                          // 2 st. (lower h. plane)

    {163.2 * 0.25, -163.2 * 0.25,                            // 3 st. (upper h. plane)
     163.2 * 0.25 + xShift[3], -163.2 * 0.25 + xShift[3]},   // 3 st. (lower h. plane)

    {163.2 * 0.25 + xShift[4], -163.2 * 0.25 + xShift[4],   // 4 st. (upper h. plane)
     163.2 * 0.25, -163.2 * 0.25},                          // 4 st. (lower h. plane)

    {163.2 * 0.25, -163.2 * 0.25,                            // 5 st. (upper h. plane)
     163.2 * 0.25 + xShift[5], -163.2 * 0.25 + xShift[5]},   // 5 st. (lower h. plane)

    {163.2 * 0.25 + xShift[6], -163.2 * 0.25 + xShift[6],   // 6 st. (upper h. plane)
     163.2 * 0.25, -163.2 * 0.25},                          // 6 st. (lower h. plane)

    {0.0}   // 7 st.
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {+22.5 + yShift[0], +22.5 + yShift[0],   // 0 st. (upper h. plane)
     -19.5, -19.5},                          // 0 st. (lower h. plane)

    {+22.5, +22.5,                            // 1 st. (upper h. plane)
     -19.5 + yShift[1], -19.5 + yShift[1]},   // 1 st. (lower h. plane)

    {+22.5 + yShift[2], +22.5 + yShift[2],   // 2 st. (upper h. plane)
     -19.5, -19.5},                          // 2 st. (lower h. plane)

    {+22.5, +22.5,                            // 3 st. (upper h. plane)
     -19.5 + yShift[3], -19.5 + yShift[3]},   // 3 st. (lower h. plane)

    {+22.5 + yShift[4], +22.5 + yShift[4],   // 4 st. (upper h. plane)
     -19.5, -19.5},                          // 4 st. (lower h. plane)

    {+22.5, +22.5,                            // 5 st. (upper h. plane)
     -19.5 + yShift[5], -19.5 + yShift[5]},   // 5 st. (lower h. plane)

    {+22.5 + yShift[6], +22.5 + yShift[6],   // 6 st. (upper h. plane)
     -19.5, -19.5},                          // 6 st. (lower h. plane)

    {0.0}   // 7 st.
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {zShift[0], zShift[0], 0.0, 0.0},   // 0 st.
    {0.0, 0.0, zShift[1], zShift[1]},   // 1 st.
    {zShift[2], zShift[2], 0.0, 0.0},   // 2 st.
    {0.0, 0.0, zShift[3], zShift[3]},   // 3 st.
    {zShift[4], zShift[4], 0.0, 0.0},   // 4 st.
    {0.0, 0.0, zShift[5], zShift[5]},   // 5 st.
    {zShift[6], zShift[6], 0.0, 0.0},   // 6 st.
    {0.0}                               // 7 st.
};

/*
 * Basic station:
 *
 *  -------------------- --------------------
 * |                    |                    |
 * |  module 0          |  module 1          |    ^ beam direction (Z)
 * |                    |                    |   /
 * |        ------------|--------------      |  /
 * |       /            |             /      | /
 * |      /             |            /       |/
 *  -----------------------------------------
 * |      \             |            \       |
 * |       \            |             \      |
 * |        \___________|______________\     |
 * |                    |                    |
 * |                    |                    |
 * |  module 3          |  module 4          |
 *  -----------------------------------------
 *
 */

// rotations of stations around y-axis by 180 deg.
const Bool_t YStationRotations[NStations] = {true /*0*/, false /*1*/, true /*2*/, false /*3*/,
                                             true /*4*/, false /*5*/, true /*6*/, false /*7*/};

// rotations of stations around x-axis by 180 deg.
const Bool_t XStationRotations[NStations] = {false /*0*/, false /*1*/, false /*2*/, false /*3*/,
                                             false /*4*/, false /*5*/, false /*6*/, false /*7*/};

// GEM plane sizes (163x45 type) ------------------------------------------------
const Double_t XModuleSize_Station163x45 = 163.2 * 0.5;   // 81.6
const Double_t YModuleSize_Station163x45 = 45.0;
const Double_t ZModuleSize_Station163x45 = 0.9;   // sensitive volume
const Double_t ZGEMSize_Station163x45 = 3.9;      // common thickness of GEM (including sens. vol)
//------------------------------------------------------------------------------

// GEM plane sizes (163x39 type) ------------------------------------------------
const Double_t XModuleSize_Station163x39 = 163.2 * 0.5;   // 81.6
const Double_t YModuleSize_Station163x39 = 39.0;
const Double_t ZModuleSize_Station163x39 = 0.9;   // sensitive volume
const Double_t ZGEMSize_Station163x39 = 3.9;      // common thickness of GEM (including sens. vol)
//------------------------------------------------------------------------------

// GEM plane sizes (10x10 type) -------------------------------------------------
const Double_t XModuleSize_Station10x10 = 10.0;   // cm
const Double_t YModuleSize_Station10x10 = 10.0;   // cm
const Double_t ZModuleSize_Station10x10 = 0.9;    // sensitive volume
//------------------------------------------------------------------------------

// GeoManager
TGeoManager* gGeoMan = NULL;

// media
TGeoMedium* pMedAir = 0;
TGeoMedium* pMedArCO27030 = 0;
TGeoMedium* pMedArC4H10_80_20 = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume* CreateStation(TString station_name);

TGeoVolume* CreateModule_Station163x45(TString module_name,
                                       Double_t xsize,
                                       Double_t ysize,
                                       Double_t zsize,
                                       Double_t hole_radius);

TGeoVolume* CreateModule_Station163x39(TString module_name,
                                       Double_t xsize,
                                       Double_t ysize,
                                       Double_t zsize,
                                       Double_t hole_radius);

TGeoVolume* CreateModule_Station10x10(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize);

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

void create_rootgeom_GEMS_Run9()
{
    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ---------------------------------
    const TString geoDetectorName = "GEMS";
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
    TGeoVolume* GEMS = new TGeoVolumeAssembly(geoDetectorName);
    GEMS->SetMedium(pMedAir);

    for (Int_t istation = 0; istation < NStations; ++istation) {
        // for(Int_t istation = 0; istation < 1; ++istation) {

        Int_t stationNum = istation;
        TGeoVolume* station = CreateStation(TString("station") + TString::Itoa(stationNum, 10) + TString("_GEM"));

        if (istation < 7) {   // big GEMs
            // upper module
            TGeoVolume* module0 =
                CreateModule_Station163x45(TString("Sensor_module0_") + station->GetName(), XModuleSize_Station163x45,
                                           YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 2.0);
            // upper module
            TGeoVolume* module1 =
                CreateModule_Station163x45(TString("Sensor_module1_") + station->GetName(), XModuleSize_Station163x45,
                                           YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 2.0);
            // lower module
            TGeoVolume* module2 =
                CreateModule_Station163x39(TString("Sensor_module2_") + station->GetName(), XModuleSize_Station163x39,
                                           YModuleSize_Station163x39, ZModuleSize_Station163x39, 4.0 + 2.0);
            // lower module
            TGeoVolume* module3 =
                CreateModule_Station163x39(TString("Sensor_module3_") + station->GetName(), XModuleSize_Station163x39,
                                           YModuleSize_Station163x39, ZModuleSize_Station163x39, 4.0 + 2.0);

            TGeoCombiTrans* module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0],
                                              ZModuleShifts[stationNum][0] + ZModuleSize_Station163x45 * 0.5);

            TGeoCombiTrans* module1_transform = new TGeoCombiTrans();
            module1_transform->RotateY(180.0);
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1],
                                              ZModuleShifts[stationNum][1] + ZModuleSize_Station163x45 * 0.5);

            TGeoCombiTrans* module2_transform = new TGeoCombiTrans();
            module2_transform->RotateX(180.0);
            module2_transform->SetTranslation(XModuleShifts[stationNum][2], YModuleShifts[stationNum][2],
                                              ZModuleShifts[stationNum][2] + ZModuleSize_Station163x39 * 0.5);

            TGeoCombiTrans* module3_transform = new TGeoCombiTrans();
            module3_transform->RotateX(180.0);
            module3_transform->RotateY(180.0);
            module3_transform->SetTranslation(XModuleShifts[stationNum][3], YModuleShifts[stationNum][3],
                                              ZModuleShifts[stationNum][3] + ZModuleSize_Station163x39 * 0.5);

            station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
            station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform));
            station->AddNode(module2, 0, new TGeoCombiTrans(*module2_transform));
            station->AddNode(module3, 0, new TGeoCombiTrans(*module3_transform));
        }

        if (istation == 7) {   // small GEM
            TGeoVolume* module0 =
                CreateModule_Station10x10(TString("Sensor_module0_") + station->GetName(), XModuleSize_Station10x10,
                                          YModuleSize_Station10x10, ZModuleSize_Station10x10);

            TGeoCombiTrans* module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0],
                                              ZModuleShifts[stationNum][0] + ZModuleSize_Station10x10 * 0.5);

            station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
        }

        TGeoCombiTrans* station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum],
                                          ZStationPositions[stationNum]);

        GEMS->AddNode(station, 0, station_transform);
    }

    top->AddNode(GEMS, 0);
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

TGeoVolume* CreateStation(TString station_name)
{
    TGeoVolume* stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    return stationA;
}

TGeoVolume* CreateModule_Station163x45(TString module_name,
                                       Double_t xsize,
                                       Double_t ysize,
                                       Double_t zsize,
                                       Double_t hole_radius)
{
    gRandom->SetSeed(0);
    Int_t uniqueId = gRandom->Integer(100000);

    // module shapes
    TGeoShape* module_partS =
        new TGeoBBox(TString("module_partS_") + TString::Itoa(uniqueId, 10), xsize * 0.5, ysize * 0.5, zsize * 0.5);
    TGeoShape* holeS =
        new TGeoTube(TString("holeS_") + TString::Itoa(uniqueId, 10), 0.0, hole_radius, zsize * 0.5 + 0.01);

    TGeoTranslation* hole_module_trans = new TGeoTranslation(
        TString("hole_module_trans_") + TString::Itoa(uniqueId, 10), -xsize * 0.5, -ysize * 0.5, 0.0);

    hole_module_trans->RegisterYourself();

    TString bool_expression_module = TString(module_partS->GetName()) + TString(" - ") + TString(holeS->GetName())
                                     + TString(":") + TString(hole_module_trans->GetName());
    TGeoCompositeShape* moduleS = new TGeoCompositeShape(bool_expression_module);

    // module volume
    TGeoVolume* moduleV = new TGeoVolume(module_name, moduleS);

    // medium
    if (pMedArC4H10_80_20)
        moduleV->SetMedium(pMedArC4H10_80_20);
    else
        Fatal("Main", "Invalid medium for sensitive modules!");

    // visual parameters
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(20);

    return moduleV;
}
//------------------------------------------------------------------------------

TGeoVolume* CreateModule_Station163x39(TString module_name,
                                       Double_t xsize,
                                       Double_t ysize,
                                       Double_t zsize,
                                       Double_t hole_radius)
{
    return CreateModule_Station163x45(module_name, xsize, ysize, zsize, hole_radius);
}
//------------------------------------------------------------------------------

TGeoVolume* CreateModule_Station10x10(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize)
{
    // module shape
    TGeoShape* moduleS = new TGeoBBox("moduleS", xsize * 0.5, ysize * 0.5, zsize * 0.5);

    // module volume
    TGeoVolume* moduleV = new TGeoVolume(module_name, moduleS);

    // medium
    if (pMedArC4H10_80_20)
        moduleV->SetMedium(pMedArC4H10_80_20);
    else
        Fatal("Main", "Invalid medium for sensitive modules!");

    // visual parameters
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(20);

    return moduleV;
}
//------------------------------------------------------------------------------
