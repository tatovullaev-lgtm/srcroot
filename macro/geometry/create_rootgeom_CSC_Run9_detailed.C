/*
 * Use this macro to create detailed ROOT geometry of CSC (Cathode
 * Strip Chamber) detector for the RUN-9 configuration (4 small chambers +
 * 2 large chambers)
 *
 * Author: Baranov D.
 * Created: 23.04.2025
 *
 * WARNING: all units are in cm!!!
 */

// Set Parameters of CSC --------------------------------------------------------
const Int_t NStations = 6;     // number of stations in the detector
const Int_t NMaxModules = 8;   // max. number of modules in a station

// X-positions of stations
const Double_t XStationPositions[NStations] = {
    // geometrical center
    +130.688,   // st. 0 (small CSC)
    -89.761,    // st. 1 (small CSC)
    +196.159,   // st. 2 (small CSC)
    -144.430,   // st. 3 (small CSC)
    +27.126,    // st. 4 (large CSC)
    +30.503     // st. 5 (large CSC)
};

// Y-positions of stations
const Double_t YStationPositions[NStations] = {
    // geometrical center
    +8.571,   // st. 0 (small CSC)
    +8.530,   // st. 1 (small CSC)
    -0.025,   // st. 2 (small CSC)
    -0.087,   // st. 3 (small CSC)
    -0.032,   // st. 4 (large CSC)
    -0.046    // st. 5 (large CSC)
};

// Z-positions of stations
const Double_t ZStationPositions[NStations] = {
    // outer side, closest to the target
    +399.250,    // st. 0 (small CSC)
    +399.246,    // st. 1 (small CSC)
    +471.557,    // st. 2 (small CSC)
    +471.589,    // st. 3 (small CSC)
    +491.4198,   // st. 4 (large CSC)
    +536.7465    // st. 5 (large CSC)
};

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    // centers of gas volumes
    {0.0, 0.0},   // st. 0
    {0.0, 0.0},   // st. 1
    {0.0, 0.0},   // st. 2
    {0.0, 0.0},   // st. 3

    {+81.375 /*0*/, +27.125 /*1*/, -27.125 /*2*/, -81.375 /*3*/, +81.375 /*4*/, +27.125 /*5*/, -27.125 /*6*/,
     -81.375 /*7*/},   // st. 4

    {+81.375 /*0*/, +27.125 /*1*/, -27.125 /*2*/, -81.375 /*3*/, +81.375 /*4*/, +27.125 /*5*/, -27.125 /*6*/,
     -81.375 /*7*/}   // st. 5
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    // centers of gas volumes
    {27.1875, -27.1875},   // st. 0
    {27.1875, -27.1875},   // st. 1
    {27.1875, -27.1875},   // st. 2
    {27.1875, -27.1875},   // st. 3

    {+36.5 /*0*/, +36.5 /*1*/, +36.5 /*2*/, +36.5 /*3*/, -36.5 /*4*/, -36.5 /*5*/, -36.5 /*6*/, -36.5 /*7*/},   // st. 4

    {+36.5 /*0*/, +36.5 /*1*/, +36.5 /*2*/, +36.5 /*3*/, -36.5 /*4*/, -36.5 /*5*/, -36.5 /*6*/, -36.5 /*7*/}   // st. 5
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    // distances from st.side to mod.side
    {1.25, 1.25},   // st. 0
    {1.25, 1.25},   // st. 1
    {1.25, 1.25},   // st. 2
    {1.25, 1.25},   // st. 3

    {2.5 /*0*/, 2.5 /*1*/, 2.5 /*2*/, 2.5 /*3*/, 2.5 /*4*/, 2.5 /*5*/, 2.5 /*6*/, 2.5 /*7*/},   // st. 4

    {2.5 /*0*/, 2.5 /*1*/, 2.5 /*2*/, 2.5 /*3*/, 2.5 /*4*/, 2.5 /*5*/, 2.5 /*6*/, 2.5 /*7*/}   // st. 5
};

enum ModuleType
{
    WHOLE_MOD,
    HOLED_LEFT_MOD,
    HOLED_RIGHT_MOD
};

const ModuleType ModuleTypes[NStations][NMaxModules] = {
    {WHOLE_MOD, WHOLE_MOD},   // st. 0
    {WHOLE_MOD, WHOLE_MOD},   // st. 1
    {WHOLE_MOD, WHOLE_MOD},   // st. 2
    {WHOLE_MOD, WHOLE_MOD},   // st. 3

    {WHOLE_MOD /*0*/, HOLED_LEFT_MOD /*1*/, HOLED_RIGHT_MOD /*2*/, WHOLE_MOD /*3*/, WHOLE_MOD /*4*/,
     HOLED_RIGHT_MOD /*5*/, HOLED_LEFT_MOD /*6*/, WHOLE_MOD /*7*/},   // st. 4

    {WHOLE_MOD /*0*/, HOLED_LEFT_MOD /*1*/, HOLED_RIGHT_MOD /*2*/, WHOLE_MOD /*3*/, WHOLE_MOD /*4*/,
     HOLED_RIGHT_MOD /*5*/, HOLED_LEFT_MOD /*6*/, WHOLE_MOD /*7*/}   // st. 5
};

const Bool_t ModulesYRotations[NStations][NMaxModules] = {
    {false, false},   // st. 0
    {false, false},   // st. 1
    {false, false},   // st. 2
    {false, false},   // st. 3

    {false /*0*/, false /*1*/, false /*2*/, false /*3*/, true /*4*/, true /*5*/, true /*6*/, true /*7*/},   // st. 4

    {false /*0*/, false /*1*/, false /*2*/, false /*3*/, true /*4*/, true /*5*/, true /*6*/, true /*7*/}   // st. 5
};

const Bool_t ModulesXRotations[NStations][NMaxModules] = {
    {false, false},   // st. 0
    {false, false},   // st. 1
    {false, false},   // st. 2
    {false, false},   // st. 3

    {false /*0*/, false /*1*/, false /*2*/, false /*3*/, true /*4*/, true /*5*/, true /*6*/, true /*7*/},   // st. 4

    {false /*0*/, false /*1*/, false /*2*/, false /*3*/, true /*4*/, true /*5*/, true /*6*/, true /*7*/}   // st. 5
};

// Sizes of elements (cm) -------------------------------------------------------

// Gas volume sizes (small module: half of station)
const Double_t XGasSize_small = 113.0;
const Double_t YGasSize_small = 54.375;
const Double_t ZGasSize_small = 0.76;

// Sensitive volume sizes (module: half of station)
const Double_t XSensSize_small = 112.9;
const Double_t YSensSize_small = 53.25;   // 106.5*0.5
const Double_t ZSensSize_small = 0.76;

// Gas volume sizes (large module)
const Double_t XGasSize_large = 54.25;
const Double_t YGasSize_large = 73.0;
const Double_t ZGasSize_large = 0.6;

const Double_t BeamHole_XSize_large = 11.125;
const Double_t BeamHole_YSize_large = 11.625;
//------------------------------------------------------------------------------

// GeoManager
TGeoManager* gGeoMan = NULL;

// media
TGeoMedium* pMedAir = 0;
TGeoMedium* pMedCarbon = 0;
TGeoMedium* pMedFiberGlass = 0;
TGeoMedium* pMedCopper = 0;
TGeoMedium* pMedArCO27030 = 0;
TGeoMedium* pMedArgonIsobutane7525 = 0;
TGeoMedium* pMedHoneyComb = 0;
TGeoMedium* pMedEpoxideCompound = 0;
TGeoMedium* pMedBrass = 0;
TGeoMedium* pMedTeflon = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume* CreateStation(TString station_name);
TGeoVolume* CreateSmallModule(TString module_name);
TGeoVolume* CreateFrameForSmallModule(TString frame_name);

TGeoVolume* CreateLargeModule(TString module_name, ModuleType module_type);

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

    // arco27020 medium
    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if (!mArCO27030)
        Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    pMedArCO27030 = gGeoManager->GetMedium("arco27030");
    if (!pMedArCO27030)
        Fatal("Main", "Medium arco27030 not found");

    // ArgonIsobutane (75/25): Ar-iC4H10 medium
    FairGeoMedium* mArgonIsobutane7525 = geoMedia->getMedium("ArgonIsobutane7525");
    if (!mArgonIsobutane7525)
        Fatal("Main", "FairMedium ArgonIsobutane7525 not found");
    geoBuild->createMedium(mArgonIsobutane7525);
    pMedArgonIsobutane7525 = gGeoManager->GetMedium("ArgonIsobutane7525");
    if (!pMedArgonIsobutane7525)
        Fatal("Main", "Medium ArgonIsobutane7525 not found");

    // HoneyComb medium
    FairGeoMedium* mHoneyComb = geoMedia->getMedium("AramidHoneyComb");
    if (!mHoneyComb)
        Fatal("Main", "FairMedium AramidHoneyComb not found");
    geoBuild->createMedium(mHoneyComb);
    pMedHoneyComb = gGeoManager->GetMedium("AramidHoneyComb");
    if (!pMedHoneyComb)
        Fatal("Main", "Medium AramidHoneyComb not found");

    // epoxide compound (as Araldit glue based on solid epoxide resin)
    FairGeoMedium* mEpoxideCompound = geoMedia->getMedium("polyurethane_hd");
    if (!mEpoxideCompound)
        Fatal("Main", "FairMedium epoxideCompound not found");
    geoBuild->createMedium(mEpoxideCompound);
    pMedEpoxideCompound = gGeoManager->GetMedium("polyurethane_hd");
    if (!pMedEpoxideCompound)
        Fatal("Main", "Medium epoxideCompound not found");

    // brass medium
    FairGeoMedium* mBrass = geoMedia->getMedium("brass");
    if (!mBrass)
        Fatal("Main", "FairMedium brass not found");
    geoBuild->createMedium(mBrass);
    pMedBrass = gGeoManager->GetMedium("brass");
    if (!pMedBrass)
        Fatal("Main", "Medium brass not found");

    // teflon medium
    FairGeoMedium* mTeflon = geoMedia->getMedium("teflon");
    if (!mTeflon)
        Fatal("Main", "FairMedium teflon not found");
    geoBuild->createMedium(mTeflon);
    pMedTeflon = gGeoManager->GetMedium("teflon");
    if (!pMedTeflon)
        Fatal("Main", "Medium teflon not found");
}

void create_rootgeom_CSC_Run9_detailed()
{

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "CSC";
    const TString geoDetectorVersion = "Run9_detailed";
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
    TGeoVolume* CSC = new TGeoVolumeAssembly(geoDetectorName);
    CSC->SetMedium(pMedAir);

    for (Int_t istation = 0; istation < NStations; ++istation) {

        Int_t stationNum = istation;   // station number

        TGeoVolume* station = CreateStation(TString("station") + TString::Itoa(stationNum, 10));

        if (stationNum < 4) {   // small CSC

            TGeoVolume* module0 = CreateSmallModule(TString("module0_") + station->GetName());
            TGeoVolume* module1 = CreateSmallModule(TString("module1_") + station->GetName());

            TGeoVolume* frame0 = CreateFrameForSmallModule(TString("frame0_") + station->GetName());
            TGeoVolume* frame1 = CreateFrameForSmallModule(TString("frame1_") + station->GetName());

            TGeoCombiTrans* module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0],
                                              ZModuleShifts[stationNum][0] + 0.5 * ZGasSize_small);

            TGeoCombiTrans* module1_transform = new TGeoCombiTrans();
            module1_transform->RotateZ(180);
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1],
                                              ZModuleShifts[stationNum][1] + 0.5 * ZGasSize_small);

            TGeoCombiTrans* frame0_transform = new TGeoCombiTrans();
            frame0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0],
                                             ZModuleShifts[stationNum][0] + 0.5 * ZGasSize_small);

            TGeoCombiTrans* frame1_transform = new TGeoCombiTrans();
            frame1_transform->RotateZ(180);
            frame1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1],
                                             ZModuleShifts[stationNum][1] + 0.5 * ZGasSize_small);

            station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
            station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform));

            station->AddNode(frame0, 0, new TGeoCombiTrans(*frame0_transform));
            station->AddNode(frame1, 0, new TGeoCombiTrans(*frame1_transform));
        } else {
            if (stationNum >= 4) {   // large CSC
                for (Int_t imod = 0; imod < NMaxModules; ++imod) {

                    TGeoVolume* module =
                        CreateLargeModule(Form("module%d_%s", imod, station->GetName()), ModuleTypes[istation][imod]);

                    TGeoCombiTrans* module_transform =
                        new TGeoCombiTrans(Form("transr_module%d_%s", imod, station->GetName()));

                    if (ModulesYRotations[istation][imod])
                        module_transform->RotateY(180.0);
                    if (ModulesXRotations[istation][imod])
                        module_transform->RotateX(180.0);

                    module_transform->SetTranslation(XModuleShifts[stationNum][imod], YModuleShifts[stationNum][imod],
                                                     ZModuleShifts[stationNum][imod] + 0.5 * ZGasSize_large);

                    station->AddNode(module, 0, module_transform);
                }
            }
        }

        TGeoCombiTrans* station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum],
                                          ZStationPositions[stationNum]);

        CSC->AddNode(station, 0, station_transform);
    }

    top->AddNode(CSC, 0);
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

TGeoVolume* CreateSmallModule(TString module_name)
{

    // shapes
    TGeoShape* moduleS = new TGeoBBox(TString("moduleS") += TString("_") + module_name, XGasSize_small * 0.5,
                                      YGasSize_small * 0.5, ZGasSize_small * 0.5);
    TGeoShape* sensS = new TGeoBBox(TString("sensS") += TString("_") + module_name, XSensSize_small * 0.5,
                                    YSensSize_small * 0.5, ZSensSize_small * 0.5);

    // volumes
    TGeoVolume* moduleV = new TGeoVolume(TString("moduleV") += TString("_") + module_name, moduleS);
    TGeoVolume* sensV = new TGeoVolume(TString("CSCSensor") += TString("_") + module_name, sensS);

    // medium
    if (pMedArgonIsobutane7525) {
        moduleV->SetMedium(pMedArgonIsobutane7525);
        sensV->SetMedium(pMedArgonIsobutane7525);
    } else
        Fatal("Main", "Invalid medium for moduleV and sensV volumes!");

    // volume visual property (transparency)
    moduleV->SetLineColor(TColor::GetColor("#47ffca"));
    moduleV->SetTransparency(50);
    sensV->SetLineColor(TColor::GetColor("#ff47ca"));
    sensV->SetTransparency(50);

    // inner spacer (the spacer located inside a sensitive volume (gas)) --------
    Double_t innerSpacerPart1_Radius = 1.65 * 0.5;   // cm
    Double_t innerSpacerPart1_ZSize = 0.12;          // cm

    Double_t innerSpacerPart2_Radius = 1.15 * 0.5;   // cm
    Double_t innerSpacerPart2_ZSize = 0.52;          // cm

    TGeoShape* innerSpacerPart1S =
        new TGeoTubeSeg(TString("innerSpacerPart1S") += TString("_") + moduleV->GetName(), 0.0, innerSpacerPart1_Radius,
                        innerSpacerPart1_ZSize * 0.5, 0.0, 180.0);
    TGeoShape* innerSpacerPart2S =
        new TGeoTubeSeg(TString("innerSpacerPart2S") += TString("_") + moduleV->GetName(), 0.0, innerSpacerPart2_Radius,
                        innerSpacerPart2_ZSize * 0.5, 0.0, 180.0);

    TGeoTranslation* innerSpacerPart1_pos1 = new TGeoTranslation();
    innerSpacerPart1_pos1->SetName(TString("innerSpacerPart1_pos1") += TString("_") + moduleV->GetName());
    innerSpacerPart1_pos1->SetDx(0.0);
    innerSpacerPart1_pos1->SetDy(0.0);
    innerSpacerPart1_pos1->SetDz(-(ZGasSize_small * 0.5 - innerSpacerPart1_ZSize * 0.5));
    innerSpacerPart1_pos1->RegisterYourself();

    TGeoTranslation* innerSpacerPart1_pos2 = new TGeoTranslation();
    innerSpacerPart1_pos2->SetName(TString("innerSpacerPart1_pos2") += TString("_") + moduleV->GetName());
    innerSpacerPart1_pos2->SetDx(0.0);
    innerSpacerPart1_pos2->SetDy(0.0);
    innerSpacerPart1_pos2->SetDz(+(ZGasSize_small * 0.5 - innerSpacerPart1_ZSize * 0.5));
    innerSpacerPart1_pos2->RegisterYourself();

    TGeoTranslation* innerSpacerPart2_pos = new TGeoTranslation();
    innerSpacerPart2_pos->SetName(TString("innerSpacerPart2_pos") += TString("_") + moduleV->GetName());
    innerSpacerPart2_pos->SetDx(0.0);
    innerSpacerPart2_pos->SetDy(0.0);
    innerSpacerPart2_pos->SetDz(0.0);
    innerSpacerPart2_pos->RegisterYourself();

    TGeoCompositeShape* innerSpacerS = new TGeoCompositeShape();
    innerSpacerS->SetName(TString("innerSpacerS") += TString("_") + moduleV->GetName());
    {
        TString expression = "innerSpacerPart1S";
        expression += TString("_") + moduleV->GetName();
        expression += ":innerSpacerPart1_pos1";
        expression += TString("_") + moduleV->GetName();
        expression += "+innerSpacerPart1S";
        expression += TString("_") + moduleV->GetName();
        expression += ":innerSpacerPart1_pos2";
        expression += TString("_") + moduleV->GetName();
        expression += "+innerSpacerPart2S";
        expression += TString("_") + moduleV->GetName();
        expression += ":innerSpacerPart2_pos";
        expression += TString("_") + moduleV->GetName();

        innerSpacerS->MakeNode(expression);
        innerSpacerS->ComputeBBox();   // need to compute a bounding box
    }

    TGeoVolume* innerSpacerV =
        new TGeoVolume(TString("innerSpacerV") += TString("_") + moduleV->GetName(), innerSpacerS);

    // volume medium
    TGeoMedium* innerSpacerV_medium = pMedEpoxideCompound;
    if (innerSpacerV_medium) {
        innerSpacerV->SetMedium(innerSpacerV_medium);
    } else
        Fatal("Main", "Invalid medium for innerSpacerV!");

    // volume visual property (transparency)
    innerSpacerV->SetLineColor(TColor::GetColor("#ffcf40"));
    innerSpacerV->SetTransparency(0);

    TGeoCombiTrans* innerSpacerV_transf[1];

    innerSpacerV_transf[0] = new TGeoCombiTrans();
    innerSpacerV_transf[0]->SetDx(0.0);
    innerSpacerV_transf[0]->SetDy(-(YSensSize_small * 0.5));
    innerSpacerV_transf[0]->SetDz(0.0);

    sensV->AddNode(innerSpacerV, 0, innerSpacerV_transf[0]);
    //--------------------------------------------------------------------------

    // support wire -------------------------------------------------------------
    Double_t supportWire_YSize = YSensSize_small - 1.15 * 0.5 /*cut element*/;   // cm
    Double_t supportWire_Radius = 0.08 * 0.5;                                    // cm

    TGeoShape* supportWireS = new TGeoTube(TString("supportWireS") += TString("_") + moduleV->GetName(), 0.0,
                                           supportWire_Radius, supportWire_YSize * 0.5);

    TGeoVolume* supportWireV =
        new TGeoVolume(TString("supportWireV") += TString("_") + moduleV->GetName(), supportWireS);

    // volume medium
    TGeoMedium* supportWireV_medium = pMedTeflon;
    if (supportWireV_medium) {
        supportWireV->SetMedium(supportWireV_medium);
    } else
        Fatal("Main", "Invalid medium for supportWireV!");

    // volume visual property (transparency)
    supportWireV->SetLineColor(TColor::GetColor("#5294ff"));
    supportWireV->SetTransparency(0);

    TGeoCombiTrans* supportWireV_transf[2];

    supportWireV_transf[0] = new TGeoCombiTrans();
    supportWireV_transf[0]->RotateX(90.0);
    supportWireV_transf[0]->SetDx(+0.4 /*shift*/);
    supportWireV_transf[0]->SetDy(+(YSensSize_small - supportWire_YSize) * 0.5);
    supportWireV_transf[0]->SetDz(-(supportWire_Radius + 0.005 /*shift*/));

    supportWireV_transf[1] = new TGeoCombiTrans();
    supportWireV_transf[1]->RotateX(90.0);
    supportWireV_transf[1]->SetDx(+0.4 /*shift*/);
    supportWireV_transf[1]->SetDy(+(YSensSize_small - supportWire_YSize) * 0.5);
    supportWireV_transf[1]->SetDz(+(supportWire_Radius + 0.005 /*shift*/));

    sensV->AddNode(supportWireV, 0, supportWireV_transf[0]);
    sensV->AddNode(supportWireV, 1, supportWireV_transf[1]);
    //--------------------------------------------------------------------------

    TGeoCombiTrans* sensV_transf[1];
    sensV_transf[0] = new TGeoCombiTrans();
    sensV_transf[0]->SetDx(0.0);
    sensV_transf[0]->SetDy(-(YGasSize_small - YSensSize_small) * 0.5);
    sensV_transf[0]->SetDz(0.0);

    moduleV->AddNode(sensV, 0, sensV_transf[0]);

    return moduleV;
}
//------------------------------------------------------------------------------

TGeoVolume* CreateFrameForSmallModule(TString frame_name)
{

    // frame
    TGeoVolume* frameA = new TGeoVolumeAssembly(frame_name);
    frameA->SetMedium(pMedAir);

    // vertical frames ----------------------------------------------------------
    Double_t verticalFrame_XSize = 4.5;              // cm
    Double_t verticalFrame_YSize = YGasSize_small;   // cm
    Double_t verticalFrame_ZSize = 3.25;             // cm

    Double_t verticalFrameHole_Radius = 1.6 * 0.5;   // cm
    Double_t verticalFrameHole_ZSize = 0.6;          // cm

    TGeoShape* verticalFrameS =
        new TGeoBBox(TString("verticalFrameS") += TString("_") + frameA->GetName(), verticalFrame_XSize * 0.5,
                     verticalFrame_YSize * 0.5, verticalFrame_ZSize * 0.5);
    TGeoShape* verticalFrameHoleS = new TGeoTube(TString("verticalFrameHoleS") += TString("_") + frameA->GetName(), 0.0,
                                                 verticalFrameHole_Radius, verticalFrameHole_ZSize * 0.5);

    TGeoVolume* verticalFrameV =
        new TGeoVolume(TString("verticalFrameV") += TString("_") + frameA->GetName(), verticalFrameS);
    TGeoVolume* verticalFrameHoleV =
        new TGeoVolume(TString("verticalFrameHoleV") += TString("_") + frameA->GetName(), verticalFrameHoleS);

    // volume medium
    TGeoMedium* verticalFrameV_medium = pMedFiberGlass;
    if (verticalFrameV_medium) {
        verticalFrameV->SetMedium(verticalFrameV_medium);
    } else
        Fatal("Main", "Invalid medium for verticalFrameV!");

    TGeoMedium* verticalFrameHoleV_medium = pMedAir;
    if (verticalFrameHoleV_medium) {
        verticalFrameHoleV->SetMedium(verticalFrameHoleV_medium);
    } else
        Fatal("Main", "Invalid medium for verticalFrameHoleV!");

    // volume visual property (transparency)
    verticalFrameV->SetLineColor(TColor::GetColor("#9999ff"));
    verticalFrameV->SetTransparency(0);
    verticalFrameHoleV->SetLineColor(TColor::GetColor("#ffffff"));
    verticalFrameHoleV->SetTransparency(0);

    TGeoCombiTrans* verticalFrameHoleV_transf[8];
    {
        Double_t y_pos = verticalFrame_YSize * 0.5 - 9.175;
        for (Int_t i = 0; i < 4; ++i) {
            verticalFrameHoleV_transf[i] = new TGeoCombiTrans();
            verticalFrameHoleV_transf[i]->SetDx(+0.65 /*shift*/);
            verticalFrameHoleV_transf[i]->SetDy(y_pos);
            verticalFrameHoleV_transf[i]->SetDz(-verticalFrame_ZSize * 0.5 + verticalFrameHole_ZSize * 0.5
                                                - 0.0001 /*shift*/);

            verticalFrameHoleV_transf[i + 4] = new TGeoCombiTrans();
            verticalFrameHoleV_transf[i + 4]->SetDx(+0.65 /*shift*/);
            verticalFrameHoleV_transf[i + 4]->SetDy(y_pos);
            verticalFrameHoleV_transf[i + 4]->SetDz(
                -(-verticalFrame_ZSize * 0.5 + verticalFrameHole_ZSize * 0.5 - 0.0001 /*shift*/));

            verticalFrameV->AddNode(verticalFrameHoleV, i, verticalFrameHoleV_transf[i]);
            verticalFrameV->AddNode(verticalFrameHoleV, i + 4, verticalFrameHoleV_transf[i + 4]);

            y_pos -= 12.5;
        }
    }

    TGeoCombiTrans* verticalFrameV_transf[2];

    verticalFrameV_transf[0] = new TGeoCombiTrans();
    verticalFrameV_transf[0]->SetDx(+(XGasSize_small * 0.5 + verticalFrame_XSize * 0.5));
    verticalFrameV_transf[0]->SetDy(0.0);
    verticalFrameV_transf[0]->SetDz(0.0);

    verticalFrameV_transf[1] = new TGeoCombiTrans();
    verticalFrameV_transf[1]->ReflectX(true);
    verticalFrameV_transf[1]->SetDx(-(XGasSize_small * 0.5 + verticalFrame_XSize * 0.5));
    verticalFrameV_transf[1]->SetDy(0.0);
    verticalFrameV_transf[1]->SetDz(0.0);

    frameA->AddNode(verticalFrameV, 0, verticalFrameV_transf[0]);
    frameA->AddNode(verticalFrameV, 1, verticalFrameV_transf[1]);

    cout << "CreateFrameForSmallModule:: ass1 \n";
    //--------------------------------------------------------------------------

    // horizontal frame ---------------------------------------------------------
    Double_t horizontalFrame_XSize = XGasSize_small + verticalFrame_XSize * 2;   // cm
    Double_t horizontalFrame_YSize = 4.5;                                        // cm
    Double_t horizontalFrame_ZSize = 3.25;                                       // cm

    Double_t horizontalFrameHole_Radius = 1.6 * 0.5;   // cm
    Double_t horizontalFrameHole_ZSize = 0.6;          // cm

    TGeoShape* horizontalFrameS =
        new TGeoBBox(TString("horizontalFrameS") += TString("_") + frameA->GetName(), horizontalFrame_XSize * 0.5,
                     horizontalFrame_YSize * 0.5, horizontalFrame_ZSize * 0.5);
    TGeoShape* horizontalFrameHoleS = new TGeoTube(TString("horizontalFrameHoleS") += TString("_") + frameA->GetName(),
                                                   0.0, horizontalFrameHole_Radius, horizontalFrameHole_ZSize * 0.5);

    TGeoVolume* horizontalFrameV =
        new TGeoVolume(TString("horizontalFrameV") += TString("_") + frameA->GetName(), horizontalFrameS);
    TGeoVolume* horizontalFrameHoleV =
        new TGeoVolume(TString("horizontalFrameHoleV") += TString("_") + frameA->GetName(), horizontalFrameHoleS);

    // volume medium
    TGeoMedium* horizontalFrameV_medium = pMedFiberGlass;
    if (horizontalFrameV_medium) {
        horizontalFrameV->SetMedium(horizontalFrameV_medium);
    } else
        Fatal("Main", "Invalid medium for horizontalFrameV!");

    TGeoMedium* horizontalFrameHoleV_medium = pMedAir;
    if (horizontalFrameHoleV_medium) {
        horizontalFrameHoleV->SetMedium(horizontalFrameHoleV_medium);
    } else
        Fatal("Main", "Invalid medium for horizontalFrameHoleV!");

    // volume visual property (transparency)
    horizontalFrameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontalFrameV->SetTransparency(0);
    horizontalFrameHoleV->SetLineColor(TColor::GetColor("#ffffff"));
    horizontalFrameHoleV->SetTransparency(0);

    TGeoCombiTrans* horizontalFrameHoleV_transf[20];
    {
        Double_t x_pos = horizontalFrame_XSize * 0.5 - 1.6;
        for (Int_t i = 0; i < 10; ++i) {
            horizontalFrameHoleV_transf[i] = new TGeoCombiTrans();
            horizontalFrameHoleV_transf[i]->SetDx(x_pos);
            horizontalFrameHoleV_transf[i]->SetDy(+0.65 /*shift*/);
            horizontalFrameHoleV_transf[i]->SetDz(-horizontalFrame_ZSize * 0.5 + horizontalFrameHole_ZSize * 0.5
                                                  - 0.0001 /*shift*/);

            horizontalFrameHoleV_transf[i + 10] = new TGeoCombiTrans();
            horizontalFrameHoleV_transf[i + 10]->SetDx(x_pos);
            horizontalFrameHoleV_transf[i + 10]->SetDy(+0.65 /*shift*/);
            horizontalFrameHoleV_transf[i + 10]->SetDz(
                -(-horizontalFrame_ZSize * 0.5 + horizontalFrameHole_ZSize * 0.5 - 0.0001 /*shift*/));

            horizontalFrameV->AddNode(horizontalFrameHoleV, i, horizontalFrameHoleV_transf[i]);
            horizontalFrameV->AddNode(horizontalFrameHoleV, i + 10, horizontalFrameHoleV_transf[i + 10]);

            if (i == 0 || i == 8)
                x_pos -= 15.6;
            else
                x_pos -= 12.5;
        }
    }

    TGeoCombiTrans* horizontalFrameV_transf[1];

    horizontalFrameV_transf[0] = new TGeoCombiTrans();
    horizontalFrameV_transf[0]->SetDx(0.0);
    horizontalFrameV_transf[0]->SetDy(+(YGasSize_small * 0.5 + horizontalFrame_YSize * 0.5));
    horizontalFrameV_transf[0]->SetDz(0.0);

    frameA->AddNode(horizontalFrameV, 0, horizontalFrameV_transf[0]);
    //--------------------------------------------------------------------------

    // panels -------------------------------------------------------------------
    Double_t fiberGlassPanel_XSize = XGasSize_small;   // cm
    Double_t fiberGlassPanel_YSize = YGasSize_small;   // cm
    Double_t fiberGlassPanel_ZSize = 0.1;              // cm
    Double_t fiberGlassPanel_HoleRadius = 1.75;        // cm

    Double_t honeyCombPanel_XSize = XGasSize_small;   // cm
    Double_t honeyCombPanel_YSize = YGasSize_small;   // cm
    Double_t honeyCombPanel_ZSize = 1.05;             // cm
    Double_t honeyCombPanel_HoleRadius = 1.75;        // cm

    // TGeoShape *fiberGlassPanelS = new TGeoBBox(TString("fiberGlassPanelS")+=TString("_") + frameA->GetName(),
    // fiberGlassPanel_XSize*0.5, fiberGlassPanel_YSize*0.5, fiberGlassPanel_ZSize*0.5);
    TGeoShape* fiberGlassPanelBigPartS = new TGeoBBox(
        TString("fiberGlassPanelBigPartS") += TString("_") + frameA->GetName(), fiberGlassPanel_XSize * 0.5,
        (fiberGlassPanel_YSize - fiberGlassPanel_HoleRadius) * 0.5, fiberGlassPanel_ZSize * 0.5);
    TGeoShape* fiberGlassPanelSmallPartS =
        new TGeoBBox(TString("fiberGlassPanelSmallPartS") += TString("_") + frameA->GetName(),
                     (fiberGlassPanel_XSize * 0.5 - fiberGlassPanel_HoleRadius) * 0.5, fiberGlassPanel_HoleRadius * 0.5,
                     fiberGlassPanel_ZSize * 0.5);

    TGeoTranslation* fiberGlassPanelBigPart_pos = new TGeoTranslation();
    fiberGlassPanelBigPart_pos->SetName(TString("fiberGlassPanelBigPart_pos") += TString("_") + frameA->GetName());
    fiberGlassPanelBigPart_pos->SetDx(0.0);
    fiberGlassPanelBigPart_pos->SetDy(+fiberGlassPanel_HoleRadius * 0.5 + 0.0001);
    fiberGlassPanelBigPart_pos->SetDz(0.0);
    fiberGlassPanelBigPart_pos->RegisterYourself();

    TGeoTranslation* fiberGlassPanelSmallPart_pos1 = new TGeoTranslation();
    fiberGlassPanelSmallPart_pos1->SetName(TString("fiberGlassPanelSmallPart_pos1") +=
                                           TString("_") + frameA->GetName());
    fiberGlassPanelSmallPart_pos1->SetDx(+(fiberGlassPanel_XSize * 0.5 + fiberGlassPanel_HoleRadius) * 0.5);
    fiberGlassPanelSmallPart_pos1->SetDy(-(fiberGlassPanel_YSize - fiberGlassPanel_HoleRadius) * 0.5);
    fiberGlassPanelSmallPart_pos1->SetDz(0.0);
    fiberGlassPanelSmallPart_pos1->RegisterYourself();

    TGeoTranslation* fiberGlassPanelSmallPart_pos2 = new TGeoTranslation();
    fiberGlassPanelSmallPart_pos2->SetName(TString("fiberGlassPanelSmallPart_pos2") +=
                                           TString("_") + frameA->GetName());
    fiberGlassPanelSmallPart_pos2->SetDx(-(fiberGlassPanel_XSize * 0.5 + fiberGlassPanel_HoleRadius) * 0.5);
    fiberGlassPanelSmallPart_pos2->SetDy(-(fiberGlassPanel_YSize - fiberGlassPanel_HoleRadius) * 0.5);
    fiberGlassPanelSmallPart_pos2->SetDz(0.0);
    fiberGlassPanelSmallPart_pos2->RegisterYourself();

    TGeoCompositeShape* fiberGlassPanelS = new TGeoCompositeShape();
    fiberGlassPanelS->SetName(TString("fiberGlassPanelS") += TString("_") + frameA->GetName());
    {
        TString expression = "fiberGlassPanelBigPartS";
        expression += TString("_") + frameA->GetName();
        expression += ":fiberGlassPanelBigPart_pos";
        expression += TString("_") + frameA->GetName();
        expression += "+fiberGlassPanelSmallPartS";
        expression += TString("_") + frameA->GetName();
        expression += ":fiberGlassPanelSmallPart_pos1";
        expression += TString("_") + frameA->GetName();
        expression += "+fiberGlassPanelSmallPartS";
        expression += TString("_") + frameA->GetName();
        expression += ":fiberGlassPanelSmallPart_pos2";
        expression += TString("_") + frameA->GetName();

        fiberGlassPanelS->MakeNode(expression);
        fiberGlassPanelS->ComputeBBox();   // need to compute a bounding box
    }

    // TGeoShape *honeyCombPanelS = new TGeoBBox(TString("honeyCombPanelS")+=TString("_") + frameA->GetName(),
    // honeyCombPanel_XSize*0.5, honeyCombPanel_YSize*0.5, honeyCombPanel_ZSize*0.5);
    TGeoShape* honeyCombPanelBigPartS =
        new TGeoBBox(TString("honeyCombPanelBigPartS") += TString("_") + frameA->GetName(), honeyCombPanel_XSize * 0.5,
                     (honeyCombPanel_YSize - honeyCombPanel_HoleRadius) * 0.5, honeyCombPanel_ZSize * 0.5);
    TGeoShape* honeyCombPanelSmallPartS =
        new TGeoBBox(TString("honeyCombPanelSmallPartS") += TString("_") + frameA->GetName(),
                     (honeyCombPanel_XSize * 0.5 - honeyCombPanel_HoleRadius) * 0.5, honeyCombPanel_HoleRadius * 0.5,
                     honeyCombPanel_ZSize * 0.5);

    TGeoTranslation* honeyCombPanelBigPart_pos = new TGeoTranslation();
    honeyCombPanelBigPart_pos->SetName(TString("honeyCombPanelBigPart_pos") += TString("_") + frameA->GetName());
    honeyCombPanelBigPart_pos->SetDx(0.0);
    honeyCombPanelBigPart_pos->SetDy(+honeyCombPanel_HoleRadius * 0.5 + 0.0001);
    honeyCombPanelBigPart_pos->SetDz(0.0);
    honeyCombPanelBigPart_pos->RegisterYourself();

    TGeoTranslation* honeyCombPanelSmallPart_pos1 = new TGeoTranslation();
    honeyCombPanelSmallPart_pos1->SetName(TString("honeyCombPanelSmallPart_pos1") += TString("_") + frameA->GetName());
    honeyCombPanelSmallPart_pos1->SetDx(+(honeyCombPanel_XSize * 0.5 + honeyCombPanel_HoleRadius) * 0.5);
    honeyCombPanelSmallPart_pos1->SetDy(-(honeyCombPanel_YSize - honeyCombPanel_HoleRadius) * 0.5);
    honeyCombPanelSmallPart_pos1->SetDz(0.0);
    honeyCombPanelSmallPart_pos1->RegisterYourself();

    TGeoTranslation* honeyCombPanelSmallPart_pos2 = new TGeoTranslation();
    honeyCombPanelSmallPart_pos2->SetName(TString("honeyCombPanelSmallPart_pos2") += TString("_") + frameA->GetName());
    honeyCombPanelSmallPart_pos2->SetDx(-(honeyCombPanel_XSize * 0.5 + honeyCombPanel_HoleRadius) * 0.5);
    honeyCombPanelSmallPart_pos2->SetDy(-(honeyCombPanel_YSize - honeyCombPanel_HoleRadius) * 0.5);
    honeyCombPanelSmallPart_pos2->SetDz(0.0);
    honeyCombPanelSmallPart_pos2->RegisterYourself();

    TGeoCompositeShape* honeyCombPanelS = new TGeoCompositeShape();
    honeyCombPanelS->SetName(TString("honeyCombPanelS") += TString("_") + frameA->GetName());
    {
        TString expression = "honeyCombPanelBigPartS";
        expression += TString("_") + frameA->GetName();
        expression += ":honeyCombPanelBigPart_pos";
        expression += TString("_") + frameA->GetName();
        expression += "+honeyCombPanelSmallPartS";
        expression += TString("_") + frameA->GetName();
        expression += ":honeyCombPanelSmallPart_pos1";
        expression += TString("_") + frameA->GetName();
        expression += "+honeyCombPanelSmallPartS";
        expression += TString("_") + frameA->GetName();
        expression += ":honeyCombPanelSmallPart_pos2";
        expression += TString("_") + frameA->GetName();

        honeyCombPanelS->MakeNode(expression);
        honeyCombPanelS->ComputeBBox();   // need to compute a bounding box
    }

    TGeoVolume* fiberGlassPanelV =
        new TGeoVolume(TString("fiberGlassPanelV") += TString("_") + frameA->GetName(), fiberGlassPanelS);
    TGeoVolume* honeyCombPanelV =
        new TGeoVolume(TString("honeyCombPanelV") += TString("_") + frameA->GetName(), honeyCombPanelS);

    // volume medium
    TGeoMedium* fiberGlassPanelV_medium = pMedFiberGlass;
    if (fiberGlassPanelV_medium) {
        fiberGlassPanelV->SetMedium(fiberGlassPanelV_medium);
    } else
        Fatal("Main", "Invalid medium for fiberGlassPanelV!");

    TGeoMedium* honeyCombPanelV_medium = pMedHoneyComb;
    if (honeyCombPanelV_medium) {
        honeyCombPanelV->SetMedium(honeyCombPanelV_medium);
    } else
        Fatal("Main", "Invalid medium for honeyCombPanelV!");

    // volume visual property (transparency)
    fiberGlassPanelV->SetLineColor(TColor::GetColor("#ffcf40"));
    fiberGlassPanelV->SetTransparency(0);
    honeyCombPanelV->SetLineColor(TColor::GetColor("#ffff00"));
    honeyCombPanelV->SetTransparency(0);

    TGeoCombiTrans* fiberGlassPanelV_transf[4];

    fiberGlassPanelV_transf[0] = new TGeoCombiTrans();
    fiberGlassPanelV_transf[0]->SetDx(0.0);
    fiberGlassPanelV_transf[0]->SetDy(0.0);
    fiberGlassPanelV_transf[0]->SetDz(-(ZGasSize_small * 0.5 + fiberGlassPanel_ZSize * 0.5));

    fiberGlassPanelV_transf[1] = new TGeoCombiTrans();
    fiberGlassPanelV_transf[1]->SetDx(0.0);
    fiberGlassPanelV_transf[1]->SetDy(0.0);
    fiberGlassPanelV_transf[1]->SetDz(+(ZGasSize_small * 0.5 + fiberGlassPanel_ZSize * 0.5));

    fiberGlassPanelV_transf[2] = new TGeoCombiTrans();
    fiberGlassPanelV_transf[2]->SetDx(0.0);
    fiberGlassPanelV_transf[2]->SetDy(0.0);
    fiberGlassPanelV_transf[2]->SetDz(
        -(ZGasSize_small * 0.5 + fiberGlassPanel_ZSize + honeyCombPanel_ZSize + fiberGlassPanel_ZSize * 0.5));

    fiberGlassPanelV_transf[3] = new TGeoCombiTrans();
    fiberGlassPanelV_transf[3]->SetDx(0.0);
    fiberGlassPanelV_transf[3]->SetDy(0.0);
    fiberGlassPanelV_transf[3]->SetDz(
        +(ZGasSize_small * 0.5 + fiberGlassPanel_ZSize + honeyCombPanel_ZSize + fiberGlassPanel_ZSize * 0.5));

    TGeoCombiTrans* honeyCombPanelV_transf[4];

    honeyCombPanelV_transf[0] = new TGeoCombiTrans();
    honeyCombPanelV_transf[0]->SetDx(0.0);
    honeyCombPanelV_transf[0]->SetDy(0.0);
    honeyCombPanelV_transf[0]->SetDz(-(ZGasSize_small * 0.5 + fiberGlassPanel_ZSize + honeyCombPanel_ZSize * 0.5));

    honeyCombPanelV_transf[1] = new TGeoCombiTrans();
    honeyCombPanelV_transf[1]->SetDx(0.0);
    honeyCombPanelV_transf[1]->SetDy(0.0);
    honeyCombPanelV_transf[1]->SetDz(+(ZGasSize_small * 0.5 + fiberGlassPanel_ZSize + honeyCombPanel_ZSize * 0.5));

    frameA->AddNode(fiberGlassPanelV, 0, fiberGlassPanelV_transf[0]);
    frameA->AddNode(fiberGlassPanelV, 1, fiberGlassPanelV_transf[1]);
    frameA->AddNode(fiberGlassPanelV, 2, fiberGlassPanelV_transf[2]);
    frameA->AddNode(fiberGlassPanelV, 3, fiberGlassPanelV_transf[3]);

    frameA->AddNode(honeyCombPanelV, 0, honeyCombPanelV_transf[0]);
    frameA->AddNode(honeyCombPanelV, 1, honeyCombPanelV_transf[1]);
    //--------------------------------------------------------------------------

    // outer spacer (the spacer located inside a sensitive volume (gas)) --------
    Double_t outerSpacerPart1_Radius = 0.8 * 0.5;   // cm
    Double_t outerSpacerPart1_ZSize = 0.4;          // cm

    TGeoShape* outerSpacerPart1S =
        new TGeoTubeSeg(TString("outerSpacerPart1S") += TString("_") + frameA->GetName(), 0.0, outerSpacerPart1_Radius,
                        outerSpacerPart1_ZSize * 0.5, 0.0, 360.0);

    TGeoTranslation* outerSpacerPart1_pos1 = new TGeoTranslation();
    outerSpacerPart1_pos1->SetName(TString("outerSpacerPart1_pos1") += TString("_") + frameA->GetName());
    outerSpacerPart1_pos1->SetDx(0.0);
    outerSpacerPart1_pos1->SetDy(0.0);
    outerSpacerPart1_pos1->SetDz(-(ZGasSize_small * 0.5 + outerSpacerPart1_ZSize * 0.5));
    outerSpacerPart1_pos1->RegisterYourself();

    TGeoTranslation* outerSpacerPart1_pos2 = new TGeoTranslation();
    outerSpacerPart1_pos2->SetName(TString("outerSpacerPart1_pos2") += TString("_") + frameA->GetName());
    outerSpacerPart1_pos2->SetDx(0.0);
    outerSpacerPart1_pos2->SetDy(0.0);
    outerSpacerPart1_pos2->SetDz(+(ZGasSize_small * 0.5 + outerSpacerPart1_ZSize * 0.5));
    outerSpacerPart1_pos2->RegisterYourself();

    TGeoCompositeShape* outerSpacerS = new TGeoCompositeShape();
    outerSpacerS->SetName(TString("outerSpacerS") += TString("_") + frameA->GetName());
    {
        TString expression = "outerSpacerPart1S";
        expression += TString("_") + frameA->GetName();
        expression += ":outerSpacerPart1_pos1";
        expression += TString("_") + frameA->GetName();
        expression += "+outerSpacerPart1S";
        expression += TString("_") + frameA->GetName();
        expression += ":outerSpacerPart1_pos2";
        expression += TString("_") + frameA->GetName();

        outerSpacerS->MakeNode(expression);
        outerSpacerS->ComputeBBox();   // need to compute a bounding box
    }

    TGeoVolume* outerSpacerV =
        new TGeoVolume(TString("outerSpacerV") += TString("_") + frameA->GetName(), outerSpacerS);

    // volume medium
    TGeoMedium* outerSpacerV_medium = pMedEpoxideCompound;
    if (outerSpacerV_medium) {
        outerSpacerV->SetMedium(outerSpacerV_medium);
    } else
        Fatal("Main", "Invalid medium for outerSpacerV!");

    // volume visual property (transparency)
    outerSpacerV->SetLineColor(TColor::GetColor("#ffcf40"));
    outerSpacerV->SetTransparency(0);

    TGeoCombiTrans* outerSpacerV_transf[1];

    outerSpacerV_transf[0] = new TGeoCombiTrans();
    outerSpacerV_transf[0]->SetDx(0.0);
    outerSpacerV_transf[0]->SetDy(-(YGasSize_small * 0.5));
    outerSpacerV_transf[0]->SetDz(0.0);

    if (!TString(frameA->GetName()).Contains("frame1")) {
        frameA->AddNode(outerSpacerV, 0, outerSpacerV_transf[0]);
    }
    //--------------------------------------------------------------------------

    // spacer bed ---------------------------------------------------------------
    Double_t spacerBed_XSize = 3.5;    // cm
    Double_t spacerBed_YSize = 3.5;    // cm
    Double_t spacerBed_ZSize = 1.25;   // cm

    Double_t spacerBedInnerHole_Radius = 0.8 * 0.5 + 0.01;   // cm
    Double_t spacerBedInnerHole_ZSize = 0.4;                 // cm

    Double_t spacerBedOuterHole_Radius = 2.0 * 0.5;   // cm
    Double_t spacerBedOuterHole_ZSize = 0.85;         // cm

    TGeoShape* spacerBedBlankS = new TGeoBBox(TString("spacerBedBlankS") += TString("_") + frameA->GetName(),
                                              spacerBed_XSize * 0.5, spacerBed_YSize * 0.5, spacerBed_ZSize * 0.5);
    TGeoShape* spacerBedInnerHoleS = new TGeoTube(TString("spacerBedInnerHoleS") += TString("_") + frameA->GetName(),
                                                  0.0, spacerBedInnerHole_Radius, (spacerBed_ZSize + 0.01) * 0.5);
    TGeoShape* spacerBedOuterHoleS =
        new TGeoTube(TString("spacerBedOuterHoleS") += TString("_") + frameA->GetName(), 0.0, spacerBedOuterHole_Radius,
                     (spacerBedOuterHole_ZSize + 0.01) * 0.5);

    TGeoTranslation* spacerBedBlank_pos = new TGeoTranslation();
    spacerBedBlank_pos->SetName(TString("spacerBedBlank_pos") += TString("_") + frameA->GetName());
    spacerBedBlank_pos->SetDx(0.0);
    spacerBedBlank_pos->SetDy(0.0);
    spacerBedBlank_pos->SetDz(0.0);
    spacerBedBlank_pos->RegisterYourself();

    TGeoTranslation* spacerBedInnerHole_pos = new TGeoTranslation();
    spacerBedInnerHole_pos->SetName(TString("spacerBedInnerHole_pos") += TString("_") + frameA->GetName());
    spacerBedInnerHole_pos->SetDx(0.0);
    spacerBedInnerHole_pos->SetDy(0.0);
    spacerBedInnerHole_pos->SetDz(0.0);
    spacerBedInnerHole_pos->RegisterYourself();

    TGeoTranslation* spacerBedOuterHole_pos = new TGeoTranslation();
    spacerBedOuterHole_pos->SetName(TString("spacerBedOuterHole_pos") += TString("_") + frameA->GetName());
    spacerBedOuterHole_pos->SetDx(0.0);
    spacerBedOuterHole_pos->SetDy(0.0);
    spacerBedOuterHole_pos->SetDz(-(spacerBed_ZSize * 0.5 - spacerBedOuterHole_ZSize * 0.5 + 0.01));
    spacerBedOuterHole_pos->RegisterYourself();

    TGeoCompositeShape* spacerBedS = new TGeoCompositeShape();
    spacerBedS->SetName(TString("spacerBedS") += TString("_") + frameA->GetName());
    {
        TString expression = "spacerBedBlankS";
        expression += TString("_") + frameA->GetName();
        expression += ":spacerBedBlank_pos";
        expression += TString("_") + frameA->GetName();
        expression += "-spacerBedOuterHoleS";
        expression += TString("_") + frameA->GetName();
        expression += ":spacerBedOuterHole_pos";
        expression += TString("_") + frameA->GetName();
        expression += "-spacerBedInnerHoleS";
        expression += TString("_") + frameA->GetName();
        expression += ":spacerBedInnerHole_pos";
        expression += TString("_") + frameA->GetName();

        spacerBedS->MakeNode(expression);
        spacerBedS->ComputeBBox();   // need to compute a bounding box
    }

    TGeoVolume* spacerBedV = new TGeoVolume(TString("spacerBedV") += TString("_") + frameA->GetName(), spacerBedS);

    // volume medium
    TGeoMedium* spacerBedV_medium = pMedFiberGlass;
    if (spacerBedV_medium) {
        spacerBedV->SetMedium(spacerBedV_medium);
    } else
        Fatal("Main", "Invalid medium for spacerBedV!");

    // volume visual property (transparency)
    spacerBedV->SetLineColor(TColor::GetColor("#c2ff99"));
    spacerBedV->SetTransparency(0);

    TGeoCombiTrans* spacerBedV_transf[2];

    spacerBedV_transf[0] = new TGeoCombiTrans();
    spacerBedV_transf[0]->SetDx(0.0);
    spacerBedV_transf[0]->SetDy(-(YGasSize_small * 0.5));
    spacerBedV_transf[0]->SetDz(-(ZGasSize_small * 0.5 + spacerBed_ZSize * 0.5));

    spacerBedV_transf[1] = new TGeoCombiTrans();
    spacerBedV_transf[1]->RotateY(180.0);
    spacerBedV_transf[1]->SetDx(0.0);
    spacerBedV_transf[1]->SetDy(-(YGasSize_small * 0.5));
    spacerBedV_transf[1]->SetDz(+(ZGasSize_small * 0.5 + spacerBed_ZSize * 0.5));

    if (!TString(frameA->GetName()).Contains("frame1")) {
        frameA->AddNode(spacerBedV, 0, spacerBedV_transf[0]);
        frameA->AddNode(spacerBedV, 1, spacerBedV_transf[1]);
    }
    //--------------------------------------------------------------------------

    // fixing plate -------------------------------------------------------------
    Double_t fixingPlate_Radius = 1.8 * 0.5;   // cm
    Double_t fixingPlate_ZSize = 0.3;          // cm

    TGeoShape* fixingPlateS = new TGeoTube(TString("fixingPlateS") += TString("_") + frameA->GetName(), 0.0,
                                           fixingPlate_Radius, fixingPlate_ZSize * 0.5);

    TGeoVolume* fixingPlateV =
        new TGeoVolume(TString("fixingPlateV") += TString("_") + frameA->GetName(), fixingPlateS);

    // volume medium
    TGeoMedium* fixingPlateV_medium = pMedBrass;
    if (fixingPlateV_medium) {
        fixingPlateV->SetMedium(fixingPlateV_medium);
    } else
        Fatal("Main", "Invalid medium for fixingPlateV!");

    // volume visual property (transparency)
    fixingPlateV->SetLineColor(TColor::GetColor("#ffcfaa"));
    fixingPlateV->SetTransparency(0);

    TGeoCombiTrans* fixingPlateV_transf[2];

    fixingPlateV_transf[0] = new TGeoCombiTrans();
    fixingPlateV_transf[0]->SetDx(0.0);
    fixingPlateV_transf[0]->SetDy(-(YGasSize_small * 0.5));
    fixingPlateV_transf[0]->SetDz(
        -(ZGasSize_small * 0.5 + fixingPlate_ZSize * 0.5 + spacerBedInnerHole_ZSize + 0.005 /*shift*/));

    fixingPlateV_transf[1] = new TGeoCombiTrans();
    fixingPlateV_transf[1]->SetDx(0.0);
    fixingPlateV_transf[1]->SetDy(-(YGasSize_small * 0.5));
    fixingPlateV_transf[1]->SetDz(
        +(ZGasSize_small * 0.5 + fixingPlate_ZSize * 0.5 + spacerBedInnerHole_ZSize + 0.005 /*shift*/));

    if (!TString(frameA->GetName()).Contains("frame1")) {
        frameA->AddNode(fixingPlateV, 0, fixingPlateV_transf[0]);
        frameA->AddNode(fixingPlateV, 1, fixingPlateV_transf[1]);
    }
    //--------------------------------------------------------------------------

    return frameA;
}

TGeoVolume* CreateLargeModule(TString module_name, ModuleType module_type)
{

    // shapes
    TGeoShape* moduleS = 0;

    switch (module_type) {
        case ModuleType::WHOLE_MOD: {
            moduleS = new TGeoBBox(Form("moduleS_%s", module_name.Data()), XGasSize_large * 0.5, YGasSize_large * 0.5,
                                   ZGasSize_large * 0.5);
            break;
        }
        case ModuleType::HOLED_LEFT_MOD: {
            moduleS = new TGeoXtru(2);
            moduleS->SetName(Form("moduleS_%s", module_name.Data()));
            Double_t x_pos[6] = {+XGasSize_large * 0.5,
                                 -XGasSize_large * 0.5,
                                 -XGasSize_large * 0.5,
                                 -XGasSize_large * 0.5 + BeamHole_XSize_large,
                                 -XGasSize_large * 0.5 + BeamHole_XSize_large,
                                 +XGasSize_large * 0.5};
            Double_t y_pos[6] = {+YGasSize_large * 0.5,
                                 +YGasSize_large * 0.5,
                                 -YGasSize_large * 0.5 + BeamHole_YSize_large,
                                 -YGasSize_large * 0.5 + BeamHole_YSize_large,
                                 -YGasSize_large * 0.5,
                                 -YGasSize_large * 0.5};
            ((TGeoXtru*)moduleS)->DefinePolygon(6, x_pos, y_pos);
            ((TGeoXtru*)moduleS)->DefineSection(0, -ZGasSize_large * 0.5);
            ((TGeoXtru*)moduleS)->DefineSection(1, +ZGasSize_large * 0.5);
            break;
        }
        case ModuleType::HOLED_RIGHT_MOD: {
            moduleS = new TGeoXtru(2);
            moduleS->SetName(Form("moduleS_%s", module_name.Data()));
            Double_t x_pos[6] = {-XGasSize_large * 0.5,
                                 -XGasSize_large * 0.5,
                                 +XGasSize_large * 0.5,
                                 +XGasSize_large * 0.5,
                                 +XGasSize_large * 0.5 - BeamHole_XSize_large,
                                 +XGasSize_large * 0.5 - BeamHole_XSize_large};
            Double_t y_pos[6] = {-YGasSize_large * 0.5,
                                 +YGasSize_large * 0.5,
                                 +YGasSize_large * 0.5,
                                 -YGasSize_large * 0.5 + BeamHole_YSize_large,
                                 -YGasSize_large * 0.5 + BeamHole_YSize_large,
                                 -YGasSize_large * 0.5};
            ((TGeoXtru*)moduleS)->DefinePolygon(6, x_pos, y_pos);
            ((TGeoXtru*)moduleS)->DefineSection(0, -ZGasSize_large * 0.5);
            ((TGeoXtru*)moduleS)->DefineSection(1, +ZGasSize_large * 0.5);
            break;
        }
        default:
            Error("CreateModule()", "Incorrect module type selected!!!");
    }

    // volumes
    TGeoVolume* moduleV = new TGeoVolume(Form("CSCSensor_%s", module_name.Data()), moduleS);

    // medium
    if (pMedArgonIsobutane7525) {
        moduleV->SetMedium(pMedArgonIsobutane7525);
    } else
        Fatal("Main", "Invalid medium for moduleV volume!");

    // volume visual property (transparency)
    moduleV->SetLineColor(TColor::GetColor("#47ffca"));
    moduleV->SetTransparency(30);

    return moduleV;
}
//------------------------------------------------------------------------------
