/* Macro for creation of ROOT geometry for VSP (Vertex Silicon Plane) for RUN-9;
 * Basic configuration of VSP (one plane including 6 silicon modules);
 *
 * Author: Baranov D.
 * Data: 25.04.2024 (created)
 * Data: 09.06.2025 (updated) : positions according to measurement schemes
 *
 * WARNING: all units are in cm!!!
 */

using namespace TMath;

// Set Parameters of VSP detector -----------------------------------------------
const Int_t NStations = 1;     // stations in the detector
const Int_t NMaxModules = 6;   // max. number of modules in a station

const Int_t NModulesInStations[NStations] = {6};

//(X-Y-Z)Positions of stations (sensitive volumes)
const Double_t XStationPositions[NStations] = {0.0};
const Double_t YStationPositions[NStations] = {0.0};
const Double_t ZStationPositions[NStations] = {0.0};   // the closest side to the target (frame or sensor)

// shift of a module is its geometric center position
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {+6.259 /*0*/, +0.312 /*1*/, -5.692 /*2*/,

     +6.536 /*3*/, +0.516 /*4*/, -5.414 /*5*/},
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {+3.1385 /*0*/, +6.0655 /*1*/, +3.2225 /*2*/,

     -3.056 /*3*/, -5.801 /*4*/, -2.976 /*5*/},
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {+8.4085 /*0*/, +8.298 /*1*/, +8.4435 /*2*/,

     +6.991 /*3*/, +7.118 /*4*/, +6.979 /*5*/},
};

enum ModuleFrameType
{
    SIDE,
    MIDDLE
};

const ModuleFrameType ModulesFrameTypes[NStations][NMaxModules] = {
    {SIDE, MIDDLE, SIDE, SIDE, MIDDLE, SIDE},
};

// Rotations of modules around y-axis by 180 deg.
const Bool_t ModulesYRotations[NStations][NMaxModules] = {
    {false /*0*/, false /*1*/, false /*2*/, false /*3*/, false /*4*/, false /*5*/},
};

// Rotations of modules around x-axis by 180 deg.
const Bool_t ModulesXRotations[NStations][NMaxModules] = {
    {false /*0*/, false /*1*/, false /*2*/, true /*3*/, true /*4*/, true /*5*/},
};
//------------------------------------------------------------------------------

// Module parameters ------------------------------------------------------------
Double_t XModuleSize = 6.2;
Double_t YModuleSize = 6.2;
Double_t ZModuleSize = 0.03;   // 300 mkm

Double_t XLeftBorder = 0.1304;
Double_t XRightBorder = 0.1304;
Double_t YTopBorder = 0.1304;
Double_t YBottomBorder = 0.1304;

// Sensor zones ------------------------------------
Double_t XSensorZoneSize = XModuleSize - (XLeftBorder + XRightBorder);
Double_t YSensorZoneSize = YModuleSize - (YTopBorder + YBottomBorder);
Double_t ZSensorZoneSize = ZModuleSize;
//-------------------------------------------------

// Module frames -----------------------------------
Double_t ZModuleFrameSize = 0.06;
//-------------------------------------------------
//------------------------------------------------------------------------------

// GeoManager --------------------------------------
TGeoManager* gGeoMan = 0;
//-------------------------------------------------

// media
TGeoMedium* pMedAir = 0;
TGeoMedium* pMedCarbon = 0;
TGeoMedium* pMedFiberGlass = 0;
TGeoMedium* pMedCopper = 0;
TGeoMedium* pMedSilicon = 0;

class FairGeoMedia;
class FairGeoBuilder;

// function declarations
TGeoVolume* CreateStation(TString station_name);
TGeoVolume* CreateModule(TString module_name);
TGeoVolume* CreateModuleFrame(TString moduleFrame_name, ModuleFrameType type);

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

    // copper medium
    FairGeoMedium* mCopper = geoMedia->getMedium("copper");
    if (!mCopper)
        Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    pMedCopper = gGeoManager->GetMedium("copper");
    if (!pMedCopper)
        Fatal("Main", "Medium copper not found");

    // silicon medium
    FairGeoMedium* mSilicon = geoMedia->getMedium("silicon");
    if (!mSilicon)
        Fatal("Main", "FairMedium silicon not found");
    geoBuild->createMedium(mSilicon);
    pMedSilicon = gGeoManager->GetMedium("silicon");
    if (!pMedSilicon)
        Fatal("Main", "Medium silicon not found");
}

void PrintInfoAboutModuleParameters()
{
    cout << "\n";
    cout << "Module parameters information : -----------------------------"
         << "\n";

    cout << "-------------------------------------------------------------\n";
}

void PrintInformationAboutStations()
{
    cout << "\n";
    for (Int_t istation = 0; istation < NStations; ++istation) {
        cout << "station " << istation << ":\n";
        cout << "   XStationPositions[" << istation << "] = " << XStationPositions[istation] << " cm\n";
        cout << "   YStationPositions[" << istation << "] = " << YStationPositions[istation] << " cm\n";
        cout << "   ZStationPositions[" << istation << "] = " << ZStationPositions[istation] << " cm\n";
        cout << "\n";

        for (Int_t imodule = 0; imodule < NModulesInStations[istation]; ++imodule) {
            cout << "   module " << imodule << ":\n";
            cout << "      XModuleShifts[" << istation << "][" << imodule << "] = " << XModuleShifts[istation][imodule]
                 << " cm\n";
            cout << "      YModuleShifts[" << istation << "][" << imodule << "] = " << YModuleShifts[istation][imodule]
                 << " cm\n";
            cout << "      ZModuleShifts[" << istation << "][" << imodule << "] = " << ZModuleShifts[istation][imodule]
                 << " cm\n";
            cout << "\n";
            cout << "      ModulesYRotations[" << istation << "][" << imodule << "] = ";
            if (ModulesYRotations[istation][imodule] == false)
                cout << "not rotated around Y\n";
            if (ModulesYRotations[istation][imodule] == true)
                cout << "rotated around Y\n";
            cout << "      ModulesXRotations[" << istation << "][" << imodule << "] = ";
            if (ModulesXRotations[istation][imodule] == false)
                cout << "not rotated around X\n";
            if (ModulesXRotations[istation][imodule] == true)
                cout << "rotated around X\n";
            cout << "\n";
        }
        cout << "-------------------------------------------------------------\n";
    }
    cout << "-------------------------------------------------------------\n";
}
//------------------------------------------------------------------------------

void create_rootgeom_VSP_Run9()
{

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "VSP";
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

    // Print information about parameters of VSP
    PrintInfoAboutModuleParameters();
    PrintInformationAboutStations();

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    // gGeoMan->SetTopVisible(1);
    //  --------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* VSP = new TGeoVolumeAssembly(geoDetectorName);
    VSP->SetMedium(pMedAir);

    // Let's create VSP stations filled with modules
    for (Int_t istation = 0; istation < NStations; ++istation) {
        // for(Int_t istation = 0; istation < 1; ++istation) {

        TGeoVolume* station = CreateStation(TString("station") + TString::Itoa(istation, 10) + TString("_VSP"));

        Int_t NModules = NModulesInStations[istation];

        TGeoVolume* modules[NModules];
        TGeoVolume* moduleFrames[NModules];

        TGeoCombiTrans* modTransforms[NModules];
        TGeoCombiTrans* modFrameTransform[NModules];

        for (Int_t imodule = 0; imodule < NModules; ++imodule) {
            // for(Int_t imodule = 0; imodule < 1; ++imodule) {

            TString module_name = TString("module") + TString::Itoa(imodule, 10) + TString("_") + station->GetName();
            modules[imodule] = CreateModule(module_name);

            TString moduleFrame_name =
                TString("moduleFrame") + TString::Itoa(imodule, 10) + TString("_") + station->GetName();
            moduleFrames[imodule] = CreateModuleFrame(moduleFrame_name, ModulesFrameTypes[istation][imodule]);

            modTransforms[imodule] = new TGeoCombiTrans();

            if (ModulesYRotations[istation][imodule]) {
                modTransforms[imodule]->RotateY(180.0);
            }
            if (ModulesXRotations[istation][imodule]) {
                modTransforms[imodule]->RotateX(180.0);
            }

            modTransforms[imodule]->SetDx(XModuleShifts[istation][imodule]);
            modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule]);
            Double_t z_module_shift =
                ZModuleSize * 0.5;   // To z-module position is at the beginning of the sensitive volume
            modTransforms[imodule]->SetDz(ZModuleShifts[istation][imodule] + z_module_shift);

            modFrameTransform[imodule] = new TGeoCombiTrans(*modTransforms[imodule]);
            if (imodule < NModulesInStations[istation] / 2) {   // modules of upper half-plane
                modFrameTransform[imodule]->SetDz(modFrameTransform[imodule]->GetTranslation()[2]
                                                  + (ZModuleSize * 0.5 + ZModuleFrameSize * 0.5));
            } else {   // modules of lower half-plane
                modFrameTransform[imodule]->SetDz(modFrameTransform[imodule]->GetTranslation()[2]
                                                  - (ZModuleSize * 0.5 + ZModuleFrameSize * 0.5));
            }

            station->AddNode(modules[imodule], 0, modTransforms[imodule]);
            station->AddNode(moduleFrames[imodule], 0, modFrameTransform[imodule]);
        }

        TGeoCombiTrans* station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[istation], YStationPositions[istation],
                                          ZStationPositions[istation]);

        VSP->AddNode(station, 0, station_transform);
    }

    top->AddNode(VSP, 0);
    top->SetVisContainers(kTRUE);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.0001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

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
//------------------------------------------------------------------------------

TGeoVolume* CreateModule(TString module_name)
{

    // Shapes -------------------------------------------------------------------
    TGeoShape* moduleS = new TGeoBBox(TString("moduleS") += TString("_") + module_name, XModuleSize * 0.5,
                                      YModuleSize * 0.5, ZModuleSize * 0.5);
    TGeoShape* sensorS = new TGeoBBox(TString("sensorS") += TString("_") + module_name, XSensorZoneSize * 0.5,
                                      YSensorZoneSize * 0.5, ZSensorZoneSize * 0.5);
    //--------------------------------------------------------------------------

    // Volumes ------------------------------------------------------------------
    TGeoVolume* moduleV = new TGeoVolume(TString("moduleV") += TString("_") + module_name, moduleS);
    TGeoVolume* sensorV = new TGeoVolume(TString("sensorV") += TString("_") + module_name, sensorS);
    //--------------------------------------------------------------------------

    // Media for volumes --------------------------------------------------------
    TGeoMedium* silicon_medium = pMedSilicon;
    if (silicon_medium) {
        moduleV->SetMedium(silicon_medium);
        sensorV->SetMedium(silicon_medium);
    } else
        Fatal("Main", "Invalid medium for modules and sensors!");
    //--------------------------------------------------------------------------

    // Visual properties for volumes --------------------------------------------
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    sensorV->SetLineColor(TColor::GetColor("#e0b0ff"));
    moduleV->SetTransparency(50);
    //--------------------------------------------------------------------------

    moduleV->AddNode(sensorV, 0, new TGeoTranslation());

    return moduleV;
}
//------------------------------------------------------------------------------

TGeoVolume* CreateModuleFrame(TString moduleFrame_name, ModuleFrameType type)
{

    // Frame sizes --------------------------------------------------------------
    Double_t longVSide = 0.0;
    if (type == SIDE)
        longVSide = 11.5;   // side module
    if (type == MIDDLE)
        longVSide = 8.7;   // middle module
    Double_t slopeVSide = 0.95;
    Double_t slopeHSide = 0.95;
    Double_t shortVSide = 3.05;
    Double_t longHSide = 8.3;
    Double_t shortHSide = 6.4;
    Double_t holeHSide = 6.0;
    Double_t holeVSide = 6.9;
    Double_t holeHMargin = 0.2;
    Double_t holeVMargin = 0.2;
    //--------------------------------------------------------------------------

    // Shapes -------------------------------------------------------------------
    TGeoShape* moduleFrameS = new TGeoXtru(2);
    moduleFrameS->SetName(TString("moduleFrameS") += TString("_") + moduleFrame_name);
    Double_t x_pos[16] = {
        +shortHSide * 0.5 /*0*/,
        +shortHSide * 0.5 /*1*/,
        +shortHSide * 0.5 + slopeHSide /*2*/,
        +shortHSide * 0.5 + slopeHSide /*3*/,
        -(shortHSide * 0.5 + slopeHSide) /*4*/,
        -(shortHSide * 0.5 + slopeHSide) /*5*/,
        -shortHSide * 0.5 /*6*/,
        -shortHSide * 0.5 /*7*/,
        -0.0 /*8*/,
        -0.0 /*9*/,
        -holeHSide * 0.5 /*10*/,
        -holeHSide * 0.5 /*11*/,
        +holeHSide * 0.5 /*12*/,
        +holeHSide * 0.5 /*13*/,
        +0.0 /*14*/,
        +0.0 /*15*/
    };
    Double_t y_pos[16] = {
        -(YModuleSize * 0.5 + holeVMargin - 0.1) /*0*/,
        -(YModuleSize * 0.5 + holeVMargin - 0.1) + longVSide /*1*/,
        -(YModuleSize * 0.5 + holeVMargin - 0.1) + longVSide + slopeVSide /*2*/,
        -(YModuleSize * 0.5 + holeVMargin - 0.1) + longVSide + slopeVSide + shortVSide /*3*/,
        -(YModuleSize * 0.5 + holeVMargin - 0.1) + longVSide + slopeVSide + shortVSide /*4*/,
        -(YModuleSize * 0.5 + holeVMargin - 0.1) + longVSide + slopeVSide /*5*/,
        -(YModuleSize * 0.5 + holeVMargin - 0.1) + longVSide /*6*/,
        -(YModuleSize * 0.5 + holeVMargin - 0.1) /*7*/,
        -(YModuleSize * 0.5 + holeVMargin - 0.1) /*8*/,
        -YModuleSize * 0.5 + 0.1 /*9*/,
        -YModuleSize * 0.5 + 0.1 /*10*/,
        +holeVSide * 0.5 /*11*/,
        +holeVSide * 0.5 /*12*/,
        -YModuleSize * 0.5 + 0.1 /*13*/,
        -YModuleSize * 0.5 + 0.1 /*14*/,
        -(YModuleSize * 0.5 + holeVMargin - 0.1) /*15*/
    };
    ((TGeoXtru*)moduleFrameS)->DefinePolygon(16, x_pos, y_pos);
    ((TGeoXtru*)moduleFrameS)->DefineSection(0, -ZModuleFrameSize * 0.5);
    ((TGeoXtru*)moduleFrameS)->DefineSection(1, +ZModuleFrameSize * 0.5);
    //--------------------------------------------------------------------------

    // Volumes ------------------------------------------------------------------
    TGeoVolume* moduleFrameV = new TGeoVolume(TString("moduleFrameV") += TString("_") + moduleFrame_name, moduleFrameS);
    //--------------------------------------------------------------------------

    // Media for volumes --------------------------------------------------------
    // TGeoMedium *carbon_medium = pMedCarbon;
    TGeoMedium* fiberGlass_medium = pMedFiberGlass;
    if (fiberGlass_medium) {
        moduleFrameV->SetMedium(fiberGlass_medium);
    } else
        Fatal("Main", "Invalid medium for moduleFrameV!");
    //--------------------------------------------------------------------------

    // Visual properties for volumes --------------------------------------------
    moduleFrameV->SetLineColor(TColor::GetColor("#cccccc"));
    moduleFrameV->SetTransparency(0);
    //--------------------------------------------------------------------------

    return moduleFrameV;
}