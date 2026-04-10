/*
 * Use this macro to create detailed ROOT geometry of Silicon Beam Tracker (SiBT)
 * for the RUN-8 configuration (3 stations)
 *
 * Author: Baranov D.
 * Created: 10.11.2022
 *
 *
 * WARNING: all units is in cm!!!
 *
 */

#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"

//Set Parameters of SiBT --------------------------------------------------------
const Int_t NStations = 3;      //stations in the detector
const Int_t NMaxModules = 1;    //max. number of modules in a station

const Double_t XStationPositions[NStations] = { 0.115/*0*/, -0.295/*1*/, 0.177/*2*/}; //geometry center
const Double_t YStationPositions[NStations] = { +0.089/*0*/, -0.046/*1*/, -0.012/*2*/}; //geometry center
const Double_t ZStationPositions[NStations] = { -283.427/*0*/, -183.225/*1*/, -84.378/*2*/}; //side which is the nearest to the beam
//where -283.427 = -283.097 - 0.33; -183.225 = -183.115 - 0.11; -84.378 = -84.068 - 0.31; (S.Pyaydin's scheme)

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {0.0}, //station 0
    {0.0}, //station 1
    {0.0}  //station 2
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {0.0}, //station 0
    {0.0}, //station 1
    {0.0}  //station 2
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {0.0}, //station 0
    {0.0}, //station 1
    {0.0}  //station 2
};

//Rotations of stations around Z (deg.)
const Double_t ZStationRotations[NStations][NMaxModules] = {
    {-0.028}, //station 0
    {-29.817}, //station 1
    {-59.949}  //station 2
};

//Sizes of elements (cm) -------------------------------------------------------

//Sensor sizes
const Double_t XSensSize = 6.08; //cm
const Double_t YSensSize = 6.08; //cm
const Double_t ZSensSize = 0.018; //cm
//------------------------------------------------------------------------------

//GeoManager
TGeoManager* gGeoMan = NULL;

//media
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedSilicon = 0;
TGeoMedium *pMedAluminium = 0;
TGeoMedium *pMedSteel = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateStation(TString station_name);
TGeoVolume *CreateModule(TString module_name);
TGeoVolume *CreateVacuumTube(TString vacTube_name);

void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild) {

    //air medium
    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    pMedAir = gGeoManager->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");

    //carbon medium
    FairGeoMedium* mCarbon = geoMedia->getMedium("carbon");
    if ( ! mCarbon  ) Fatal("Main", "FairMedium carbon not found");
    geoBuild->createMedium(mCarbon);
    pMedCarbon = gGeoManager->GetMedium("carbon");
    if ( ! pMedCarbon ) Fatal("Main", "Medium carbon not found");

    //fiberglass medium
    FairGeoMedium* mFiberGlass = geoMedia->getMedium("fiberglass");
    if ( ! mFiberGlass  ) Fatal("Main", "FairMedium fiberglass not found");
    geoBuild->createMedium(mFiberGlass);
    pMedFiberGlass = gGeoManager->GetMedium("fiberglass");
    if ( ! pMedFiberGlass  ) Fatal("Main", "Medium fiberglass not found");

    //silicon medium
    FairGeoMedium* mSilicon = geoMedia->getMedium("silicon");
    if ( ! mSilicon  ) Fatal("Main", "FairMedium silicon not found");
    geoBuild->createMedium(mSilicon);
    pMedSilicon= gGeoManager->GetMedium("silicon");
    if ( ! pMedSilicon  ) Fatal("Main", "Medium silicon not found");

    //aluminium medium
    FairGeoMedium* mAluminium = geoMedia->getMedium("aluminium");
    if ( ! mAluminium  ) Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAluminium);
    pMedAluminium= gGeoManager->GetMedium("aluminium");
    if ( ! pMedAluminium  ) Fatal("Main", "Medium aluminium not found");

    //steel medium
    FairGeoMedium* mSteel = geoMedia->getMedium("steel");
    if ( ! mSteel  ) Fatal("Main", "FairMedium steel not found");
    geoBuild->createMedium(mSteel);
    pMedSteel= gGeoManager->GetMedium("steel");
    if ( ! pMedSteel  ) Fatal("Main", "Medium steel not found");
}

void create_rootgeom_SiBT_Run8_detailed() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "SiBT";
    const TString geoDetectorVersion = "Run8_detailed";
    const TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";

    // ----  global geometry parameters  ---------------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();

    // -------   Load media from media file   ----------------------------------
    TString medFile = gPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    DefineRequiredMedia(geoMedia, geoBuild);
    // -------------------------------------------------------------------------

    // --------------   Create geometry and top volume  ------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // -------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* SiBT = new TGeoVolumeAssembly(geoDetectorName);
    SiBT->SetMedium(pMedAir);

    for(Int_t istation = 0; istation < NStations; ++istation) {
    //for(Int_t istation = 0; istation < 1; ++istation) {

        Int_t stationNum = istation; //station number

        TGeoVolume *station = CreateStation(TString("station")+TString::Itoa(stationNum, 10));

        TGeoVolume *module0 = CreateModule(TString("module0_")+station->GetName());

        TGeoVolume *vacuumTube = CreateVacuumTube(TString("vacuumTube_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->RotateZ(ZStationRotations[stationNum][0]);
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0] + 0.5*ZSensSize);

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
        station->AddNode(vacuumTube, 0, new TGeoCombiTrans(*module0_transform));

        SiBT->AddNode(station, 0, station_transform);
    }

    top->AddNode(SiBT, 0);
    top->SetVisContainers(kTRUE);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.0001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();
    //gGeoMan->RestoreMasterVolume();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
}

TGeoVolume *CreateStation(TString station_name) {

    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    return stationA;
}

TGeoVolume *CreateModule(TString module_name) {

    //shapes
    TGeoShape *moduleS = new TGeoBBox(TString("moduleS")+=TString("_") + module_name, XSensSize*0.5, YSensSize*0.5, ZSensSize*0.5);

    //volumes
    TGeoVolume *moduleV = new TGeoVolume(TString("SiBTSensor")+=TString("_") + module_name, moduleS);

    //medium
    if(pMedSilicon) {
        moduleV->SetMedium(pMedSilicon);
    }
    else Fatal("Main", "Invalid medium for moduleV volumes!");

    //volume visual property
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(30);

    return moduleV;
}

TGeoVolume *CreateVacuumTube(TString vacTube_name) {

    //shapes
    TGeoShape *vacTubeS = new TGeoTube(TString("vacTubeS")+=TString("_") + vacTube_name, 21.3*0.5, 21.9*0.5, 19.6*0.5);
    TGeoShape *vacTubeHoleS = new TGeoTube(TString("vacTubeHoleS")+=TString("_") + vacTube_name, 0.0, 15.905*0.5, 10.*0.5);

    TGeoShape *sideTubeS = new TGeoTube(TString("sideTubeS")+=TString("_") + vacTube_name, 15.3*0.5, 15.9*0.5, 15.0*0.5);
    TGeoShape *sideTubeCutS = new TGeoTube(TString("sideTubeCutS")+=TString("_") + vacTube_name, 0.0, 21.3*0.5, 19.6*0.5);

    TGeoShape *vacTubeFlangeS = new TGeoTube(TString("vacTubeFlangeS")+=TString("_") + vacTube_name, 21.9*0.5, 24.0*0.5, 1.2*0.5);
    TGeoShape *sideTubeFlangeS = new TGeoTube(TString("sideTubeFlangeS")+=TString("_") + vacTube_name, 15.9*0.5, 18.0*0.5, 1.2*0.5);

    TGeoCombiTrans *vacTubeHole_transf = new TGeoCombiTrans();
        vacTubeHole_transf->SetName(TString("vacTubeHole_transf")+=TString("_") + vacTube_name);
        vacTubeHole_transf->RotateY(90.0);
        vacTubeHole_transf->SetDx(-10.0);
        vacTubeHole_transf->SetDy(0.0);
        vacTubeHole_transf->SetDz(0.0);
        vacTubeHole_transf->RegisterYourself();

    TGeoCompositeShape *vacTubeWithHoleS = new TGeoCompositeShape();
    vacTubeWithHoleS->SetName(TString("vacTubeWithHoleS")+=TString("_") + vacTube_name);
    {
        TString expression = "vacTubeS"; expression += TString("_") + vacTube_name;
        expression += "-vacTubeHoleS"; expression += TString("_") + vacTube_name;
        expression += ":vacTubeHole_transf"; expression += TString("_") + vacTube_name;
        vacTubeWithHoleS->MakeNode(expression);
        vacTubeWithHoleS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCombiTrans *sideTube_transf = new TGeoCombiTrans();
        sideTube_transf->SetName(TString("sideTube_transf")+=TString("_") + vacTube_name);
        sideTube_transf->RotateY(90.0);
        sideTube_transf->SetTranslation(-15.0*0.5, 0.0, 0.0);
        sideTube_transf->RegisterYourself();

    TGeoCompositeShape *sideTubeWithCutS = new TGeoCompositeShape();
    sideTubeWithCutS->SetName(TString("sideTubeWithCutS")+=TString("_") + vacTube_name);
    {
        TString expression = "sideTubeS"; expression += TString("_") + vacTube_name;
        expression += ":sideTube_transf"; expression += TString("_") + vacTube_name;
        expression += "-sideTubeCutS"; expression += TString("_") + vacTube_name;
        sideTubeWithCutS->MakeNode(expression);
        sideTubeWithCutS->ComputeBBox(); //need to compute a bounding box
    }

    //volumes
    //frame volumes
    TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(vacTube_name);
    TGeoVolume *vacTubeWithHoleV = new TGeoVolume(TString("vacTubeWithHoleV")+=TString("_") + vacTube_name, vacTubeWithHoleS);
    TGeoVolume *sideTubeWithCutV = new TGeoVolume(TString("sideTubeWithCutV")+=TString("_") + vacTube_name, sideTubeWithCutS);
    TGeoVolume *vacTubeFlangeV = new TGeoVolume(TString("vacTubeFlangeV")+=TString("_") + vacTube_name, vacTubeFlangeS);
    TGeoVolume *sideTubeFlangeV = new TGeoVolume(TString("sideTubeFlangeV")+=TString("_") + vacTube_name, sideTubeFlangeS);

    //volume medium
    TGeoMedium *vacTubeWithHoleV_medium = nullptr;
    if( vacTube_name.Contains("station0") || vacTube_name.Contains("station1") ) vacTubeWithHoleV_medium = pMedSteel;
    if( vacTube_name.Contains("station2") ) vacTubeWithHoleV_medium = pMedAluminium;
    if(vacTubeWithHoleV_medium) {
        vacTubeWithHoleV->SetMedium(vacTubeWithHoleV_medium);
        sideTubeWithCutV->SetMedium(vacTubeWithHoleV_medium);
        vacTubeFlangeV->SetMedium(vacTubeWithHoleV_medium);
        sideTubeFlangeV->SetMedium(vacTubeWithHoleV_medium);
    }
    else Fatal("Main", "Invalid medium for vacTubeWithHoleV!");

    //volume visual property (transparency)
    vacTubeWithHoleV->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTubeWithHoleV->SetTransparency(0);
    sideTubeWithCutV->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));

    //part positions in the vacuum tube box
    TGeoCombiTrans *vacTubeFlangeV_transf[2];

    vacTubeFlangeV_transf[0] = new TGeoCombiTrans();
    vacTubeFlangeV_transf[0]->SetTranslation(0.0, 0.0, -19.6*0.5 + 1.2*0.5);

    vacTubeFlangeV_transf[1] = new TGeoCombiTrans();
    vacTubeFlangeV_transf[1]->SetTranslation(0.0, 0.0, +19.6*0.5 - 1.2*0.5);

    TGeoCombiTrans *sideTubeFlangeV_transf[1];

    sideTubeFlangeV_transf[0] = new TGeoCombiTrans();
    sideTubeFlangeV_transf[0]->RotateY(90.0);
    sideTubeFlangeV_transf[0]->SetTranslation(-15.0 + 1.2*0.5, 0.0, 0.0);


    vacTubeBoxV->AddNode(vacTubeWithHoleV, 0, new TGeoCombiTrans());
    vacTubeBoxV->AddNode(sideTubeWithCutV, 0, new TGeoCombiTrans());
    vacTubeBoxV->AddNode(vacTubeFlangeV, 0, vacTubeFlangeV_transf[0]);
    vacTubeBoxV->AddNode(vacTubeFlangeV, 1, vacTubeFlangeV_transf[1]);
    vacTubeBoxV->AddNode(sideTubeFlangeV, 0, sideTubeFlangeV_transf[0]);

    return vacTubeBoxV;
}
//------------------------------------------------------------------------------
