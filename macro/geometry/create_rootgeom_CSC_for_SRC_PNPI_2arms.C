/*
 * Baranov D., modified by Yu. Naryshkin
 * 17.05.2019
 * Use this macro to create ROOT geometry for the RunSRCSpring2018 CSC (Cathode
 * Strip Chamber)
 * configuration (BM@N SRC RUN-7)
 *
 * WARNING: all units is in cm!!!
 *
 */

#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include <string>

//Set Parameters of CSC --------------------------------------------------------
const Int_t NStations = 4;      //stations in the detector
const Int_t NMaxModules = 4;    //max. number of modules in a station

//(X-Y-Z)Positions of stations

const Double_t XStationPositions[NStations] = { 180. * sin(30.), 300. * sin(30.), 180. * sin(-30.), 300. * sin(-30.) }; //geometry center  (original)
const Double_t YStationPositions[NStations] = { 0., 0., 0., 0. }; //geometry center (original)
const Double_t ZStationPositions[NStations] = { 180., 300., 180., 300. }; //outer side nearest to the target (original)

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = { //centers of gas volume
    {0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0}
};
const Double_t YModuleShifts[NStations][NMaxModules] = { //centers of gas volume
    {0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0}
};
const Double_t ZModuleShifts[NStations][NMaxModules] = { //distances from st.side to mod.side
    {-6.0, -2.0, 2.0, 6.0},
    {-6.0, -2.0, 2.0, 6.0},
    {-6.0, -2.0, 2.0, 6.0},
    {-6.0, -2.0, 2.0, 6.0}
//    {-4.5, -1.5, 1.5, 4.5},
//    {-4.5, -1.5, 1.5, 4.5}
};

//Sizes of elements (cm) -------------------------------------------------------

//Gas volume sizes (module: half of station)
const Double_t XGasSize[NStations] = {50., 100., 50., 100.};
const Double_t YGasSize[NStations] = {50., 100., 50., 100.};
const Double_t ZGasSize[NStations] = {0.76, 0.76, 0.76, 0.76};

//Sensitive volume sizes (module: half of station)
const Double_t XSensSize[NStations] = {50., 100., 50., 100.};
const Double_t YSensSize[NStations] = {50., 100., 50., 100.}; 
const Double_t ZSensSize[NStations] = {0.075, 0.075, 0.075, 0.075};

//Sensitive volume Cu cover sizes (module: half of station)
const Double_t XCuSize[NStations] = {50., 100., 50., 100.};
const Double_t YCuSize[NStations] = {50., 100., 50., 100.}; 
const Double_t ZCuSize[NStations] = {0.0017, 0.0017, 0.0017, 0.0017};

//Frame sizes
const Double_t dXFrameSize = 4.5;
const Double_t dYFrameSize = 4.5;
const Double_t dZFrameSize = 3.26;

//Side panel sizes
const Double_t dZFiberglassPanel = 0.1;
const Double_t dZHoneycombPanel = 1.05;


//------------------------------------------------------------------------------

//GeoManager
TGeoManager* gGeoMan = NULL;

//media
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedG10 = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedCopper = 0;
TGeoMedium *pMedArCO27030 = 0;
TGeoMedium *pMedArgonIsobutane7525 = 0;
TGeoMedium *pMedRohacell = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateStation(TString station_name);
TGeoVolume *CreateModule(TString module_name);

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

//YN
    //G10 medium
    FairGeoMedium* mG10 = geoMedia->getMedium("G10");
    if ( ! mG10  ) Fatal("Main", "FairMedium G10 not found");
    geoBuild->createMedium(mG10);
    pMedG10 = gGeoManager->GetMedium("G10");
    if ( ! pMedG10 ) Fatal("Main", "Medium G10 not found");
//YN

    //fiberglass medium
    FairGeoMedium* mFiberGlass = geoMedia->getMedium("fiberglass");
    if ( ! mFiberGlass  ) Fatal("Main", "FairMedium fiberglass not found");
    geoBuild->createMedium(mFiberGlass);
    pMedFiberGlass = gGeoManager->GetMedium("fiberglass");
    if ( ! pMedFiberGlass  ) Fatal("Main", "Medium fiberglass not found");

    //arco27020 medium
    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if ( ! mArCO27030  ) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    pMedArCO27030= gGeoManager->GetMedium("arco27030");
    if ( ! pMedArCO27030  ) Fatal("Main", "Medium arco27030 not found");

    //ArgonIsobutane (75/25): Ar-iC4H10 medium
    FairGeoMedium* mArgonIsobutane7525 = geoMedia->getMedium("ArgonIsobutane7525");
    if ( ! mArgonIsobutane7525  ) Fatal("Main", "FairMedium ArgonIsobutane7525 not found");
    geoBuild->createMedium(mArgonIsobutane7525);
    pMedArgonIsobutane7525= gGeoManager->GetMedium("ArgonIsobutane7525");
    if ( ! pMedArgonIsobutane7525  ) Fatal("Main", "Medium ArgonIsobutane7525 not found");

    //Rohacell medium
    FairGeoMedium* mRohacell = geoMedia->getMedium("rohacellhf71");
    if ( ! mRohacell  ) Fatal("Main", "FairMedium rohacellhf71 not found");
    geoBuild->createMedium(mRohacell);
    pMedRohacell= gGeoManager->GetMedium("rohacellhf71");
    if ( ! pMedRohacell  ) Fatal("Main", "Medium rohacellhf71 not found");


    //Copper: Cu medium
    FairGeoMedium* mCopper = geoMedia->getMedium("copper");
    if (!mCopper) Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    pMedCopper = gGeoManager->GetMedium("copper");
    if (!pMedCopper)
        Fatal("Main", "Medium copper not found");

}

void create_rootgeom_CSC_for_SRC_PNPI_2arms() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "CSC";
//    const TString geoDetectorVersion = "RunSRCSpring2018";
    const TString geoDetectorVersion = "for_SRC_PNPI_2arms";
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
    TGeoVolume* CSC = new TGeoVolumeAssembly(geoDetectorName);
    CSC->SetMedium(pMedAir);




    //station 0 (consisting of four modules)
    if(1) {
        Int_t stationNum = 0; //station number

        TGeoVolume *station = CreateStation(TString("station")+TString::Itoa(stationNum, 10));

        TGeoVolume *module0 = CreateModule(TString("module0_")+station->GetName());
        TGeoVolume *module1 = CreateModule(TString("module1_")+station->GetName());
        TGeoVolume *module2 = CreateModule(TString("module2_")+station->GetName());
        TGeoVolume *module3 = CreateModule(TString("module3_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
//            module0_transform->RotateY(-30);
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
//            module1_transform->RotateY(-30);
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]);

        TGeoCombiTrans *module2_transform = new TGeoCombiTrans();
//            module2_transform->RotateY(-30);
            module2_transform->SetTranslation(XModuleShifts[stationNum][2], YModuleShifts[stationNum][2], ZModuleShifts[stationNum][2]);

        TGeoCombiTrans *module3_transform = new TGeoCombiTrans();
//            module3_transform->RotateY(-30);
            module3_transform->SetTranslation(XModuleShifts[stationNum][3], YModuleShifts[stationNum][3], ZModuleShifts[stationNum][3]);

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->RotateY(-30);
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
        station->AddNode(module1, 1, new TGeoCombiTrans(*module1_transform));
        station->AddNode(module2, 2, new TGeoCombiTrans(*module2_transform));
        station->AddNode(module3, 3, new TGeoCombiTrans(*module3_transform));

        CSC->AddNode(station, 0, station_transform);
    }




    //station 1 (consisting of four modules)
    if(2) {
        Int_t stationNum = 1; //station number

        TGeoVolume *station1 = CreateStation(TString("station1")+TString::Itoa(stationNum, 10));

        TGeoVolume *module0 = CreateModule(TString("module0_")+station1->GetName());
        TGeoVolume *module1 = CreateModule(TString("module1_")+station1->GetName());
        TGeoVolume *module2 = CreateModule(TString("module2_")+station1->GetName());
        TGeoVolume *module3 = CreateModule(TString("module3_")+station1->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]);

        TGeoCombiTrans *module2_transform = new TGeoCombiTrans();
            module2_transform->SetTranslation(XModuleShifts[stationNum][2], YModuleShifts[stationNum][2], ZModuleShifts[stationNum][2]);

        TGeoCombiTrans *module3_transform = new TGeoCombiTrans();
            module3_transform->SetTranslation(XModuleShifts[stationNum][3], YModuleShifts[stationNum][3], ZModuleShifts[stationNum][3]);

        TGeoCombiTrans *station_transform1 = new TGeoCombiTrans();
        station_transform1->RotateY(-30);
        station_transform1->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station1->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
        station1->AddNode(module1, 1, new TGeoCombiTrans(*module1_transform));
        station1->AddNode(module2, 2, new TGeoCombiTrans(*module2_transform));
        station1->AddNode(module3, 3, new TGeoCombiTrans(*module3_transform));

        CSC->AddNode(station1, 0, station_transform1);
    }







    //station 2 (consisting of four modules)
    if(3) {
        Int_t stationNum = 2; //station number

        TGeoVolume *station2 = CreateStation(TString("station2")+TString::Itoa(stationNum, 10));

        TGeoVolume *module0 = CreateModule(TString("module0_")+station2->GetName());
        TGeoVolume *module1 = CreateModule(TString("module1_")+station2->GetName());
        TGeoVolume *module2 = CreateModule(TString("module2_")+station2->GetName());
        TGeoVolume *module3 = CreateModule(TString("module3_")+station2->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]);

        TGeoCombiTrans *module2_transform = new TGeoCombiTrans();
            module2_transform->SetTranslation(XModuleShifts[stationNum][2], YModuleShifts[stationNum][2], ZModuleShifts[stationNum][2]);

        TGeoCombiTrans *module3_transform = new TGeoCombiTrans();
            module3_transform->SetTranslation(XModuleShifts[stationNum][3], YModuleShifts[stationNum][3], ZModuleShifts[stationNum][3]);

        TGeoCombiTrans *station_transform2 = new TGeoCombiTrans();
        station_transform2->RotateY(30);
        station_transform2->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station2->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
        station2->AddNode(module1, 1, new TGeoCombiTrans(*module1_transform));
        station2->AddNode(module2, 2, new TGeoCombiTrans(*module2_transform));
        station2->AddNode(module3, 3, new TGeoCombiTrans(*module3_transform));

        CSC->AddNode(station2, 0, station_transform2);
    }






    //station 3 (consisting of four modules)
    if(4) {
        Int_t stationNum = 3; //station number

        TGeoVolume *station3 = CreateStation(TString("station3")+TString::Itoa(stationNum, 10));

        TGeoVolume *module0 = CreateModule(TString("module0_")+station3->GetName());
        TGeoVolume *module1 = CreateModule(TString("module1_")+station3->GetName());
        TGeoVolume *module2 = CreateModule(TString("module2_")+station3->GetName());
        TGeoVolume *module3 = CreateModule(TString("module3_")+station3->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]);

        TGeoCombiTrans *module2_transform = new TGeoCombiTrans();
            module2_transform->SetTranslation(XModuleShifts[stationNum][2], YModuleShifts[stationNum][2], ZModuleShifts[stationNum][2]);

        TGeoCombiTrans *module3_transform = new TGeoCombiTrans();
            module3_transform->SetTranslation(XModuleShifts[stationNum][3], YModuleShifts[stationNum][3], ZModuleShifts[stationNum][3]);

        TGeoCombiTrans *station_transform3 = new TGeoCombiTrans();
        station_transform3->RotateY(30);
        station_transform3->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station3->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
        station3->AddNode(module1, 1, new TGeoCombiTrans(*module1_transform));
        station3->AddNode(module2, 2, new TGeoCombiTrans(*module2_transform));
        station3->AddNode(module3, 3, new TGeoCombiTrans(*module3_transform));

        CSC->AddNode(station3, 0, station_transform3);
    }






    top->AddNode(CSC, 0);
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
//    top->Draw("ogl");
    top->Draw("");
}

TGeoVolume *CreateStation(TString station_name) {

    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    return stationA;
}

TGeoVolume *CreateModule(TString module_name) {

    Double_t xmod1;
    Double_t ymod1;
    Double_t zmod1;

    Double_t xsens1;
    Double_t ysens1;
    Double_t zsens1;

    Double_t xCu1;
    Double_t yCu1;
    Double_t zCu1;

    Double_t xShiftCu1;
    Double_t yShiftCu1;
    Double_t zShiftCu1;


//str.find(substr) != std::string::npos

//    if(module_name == "module0_station0") {
    if(module_name.Contains("station0")) {
     xmod1 = XGasSize[0];
     ymod1 = YGasSize[0];
     zmod1 = ZGasSize[0];

     xsens1 = XSensSize[0];
     ysens1 = YSensSize[0];
     zsens1 = ZSensSize[0];

     xCu1 = XCuSize[0];
     yCu1 = YCuSize[0];
     zCu1 = ZCuSize[0];

     zShiftCu1 = ZSensSize[0] * 0.5 + ZCuSize[0] *0.5;

    cout << " Module NAME"  << module_name << endl;

                                          }


//    if(module_name == "module01_station11") {
    if(module_name.Contains("station1")) {
     xmod1 = XGasSize[1];
     ymod1 = YGasSize[1];
     zmod1 = ZGasSize[1];

     xsens1 = XSensSize[1];
     ysens1 = YSensSize[1];
     zsens1 = ZSensSize[1];

     xCu1 = XCuSize[1];
     yCu1 = YCuSize[1];
     zCu1 = ZCuSize[1];

     zShiftCu1 = ZSensSize[1] * 0.5 + ZCuSize[1] * 0.5;

                                            }




//    if(module_name == "module01_station11") {
    if(module_name.Contains("station2")) {
     xmod1 = XGasSize[2];
     ymod1 = YGasSize[2];
     zmod1 = ZGasSize[2];

     xsens1 = XSensSize[2];
     ysens1 = YSensSize[2];
     zsens1 = ZSensSize[2];

     xCu1 = XCuSize[2];
     yCu1 = YCuSize[2];
     zCu1 = ZCuSize[2];

     zShiftCu1 = ZSensSize[3] * 0.5 + ZCuSize[3] * 0.5;

                                            }




//    if(module_name == "module01_station11") {
    if(module_name.Contains("station3")) {
     xmod1 = XGasSize[3];
     ymod1 = YGasSize[3];
     zmod1 = ZGasSize[3];

     xsens1 = XSensSize[3];
     ysens1 = YSensSize[3];
     zsens1 = ZSensSize[3];

     xCu1 = XCuSize[3];
     yCu1 = YCuSize[3];
     zCu1 = ZCuSize[3];

     zShiftCu1 = ZSensSize[3] * 0.5 + ZCuSize[3] * 0.5;

                                            }




//    cout << " Module NAME"  << module_name << endl;

    //shapes
    TGeoShape *moduleS = new TGeoBBox("moduleS", (xmod1 + 2.0*dXFrameSize)*0.5, (ymod1+dYFrameSize)*0.5, (zmod1 + 4.0*dZFiberglassPanel + 2.0*dZHoneycombPanel)*0.5);
    TGeoShape *gasS = new TGeoBBox("gasS", xmod1*0.5, ymod1*0.5, zmod1*0.5);
    TGeoShape *sensS = new TGeoBBox("sensS", xsens1*0.5, ysens1*0.5, zsens1*0.5);
    TGeoShape *sensCuS = new TGeoBBox("sensCuS", xCu1*0.5, yCu1*0.5, zCu1*0.5);
    TGeoShape *fiberglassS = new TGeoBBox("fiberglassS", xmod1*0.5, ymod1*0.5, dZFiberglassPanel*0.5);
    TGeoShape *honeycombS = new TGeoBBox("honeycombS", xmod1*0.5, ymod1*0.5, dZHoneycombPanel*0.5);

//    TGeoShape *verticalframeS = new TGeoBBox("verticalframeS", dXFrameSize*0.5, 0.5*(ymod1+dYFrameSize), (zmod1 + 4.0*dZFiberglassPanel + 2.0*dZHoneycombPanel)*0.5);
//    TGeoShape *horizontalframeS = new TGeoBBox("horizontalframeS", xmod1*0.5, dYFrameSize*0.5, ( + 4.0*dZFiberglassPanel + 2.0*dZHoneycombPanel)*0.5);
                                           
    //volumes
    TGeoVolume *moduleV = new TGeoVolume(TString("moduleV_") + module_name, moduleS);
    TGeoVolume *gasV = new TGeoVolume(TString("gasV_") + module_name, gasS);
    TGeoVolume *sensV = new TGeoVolume(TString("CSCSensor_") + module_name, sensS);
    TGeoVolume *sensCuV = new TGeoVolume(TString("CSCSensorCu_") + module_name, sensCuS);
    TGeoVolume *fiberglassV = new TGeoVolume(TString("fiberglassV_") + module_name, fiberglassS);
    TGeoVolume *honeycombV = new TGeoVolume(TString("honeycombV_") + module_name, honeycombS);
//    TGeoVolume *verticalframeV = new TGeoVolume(TString("verticalframeV_") + module_name, verticalframeS);
//    TGeoVolume *horizontalframeV = new TGeoVolume(TString("horizontalframeV_") + module_name, horizontalframeS);


    //medium
    if(pMedAir) moduleV->SetMedium(pMedAir);
    else Fatal("Main", "Invalid medium for module volume!");

    if(pMedArgonIsobutane7525) {
        gasV->SetMedium(pMedArgonIsobutane7525);
        sensV->SetMedium(pMedG10);
    }
    else Fatal("Main", "Invalid medium for module volumes!");

    if(pMedCopper) {
        sensCuV->SetMedium(pMedCopper);
    }
    else Fatal("Main", "Invalid medium for module volumes!");


//    if(pMedFiberGlass) {
//        fiberglassV->SetMedium(pMedFiberGlass);
//        verticalframeV->SetMedium(pMedFiberGlass);
//        horizontalframeV->SetMedium(pMedFiberGlass);
//    }
//    else Fatal("Main", "Invalid medium for fiberglass volume!");

//    if(pMedRohacell) honeycombV->SetMedium(pMedRohacell);
//    else Fatal("Main", "Invalid medium for honeycomb volume!");

    //visual parameters
    moduleV->SetLineColor(TColor::GetColor("#cccccc"));
    moduleV->SetTransparency(20);

    gasV->SetLineColor(TColor::GetColor("#47ffca"));
    gasV->SetTransparency(20);

//    sensV->SetLineColor(TColor::GetColor("#ff47ca"));
    sensV->SetLineColor(TColor::GetColor("#ff47ca"));
    sensV->SetTransparency(20);

//    sensCuV->SetLineColor(TColor::GetColor("#ff47ca"));
//    sensCuV->SetLineColor(TColor::GetColor("#00b835"));
    sensCuV->SetLineColor(TColor::GetColor("#b87200"));
    sensCuV->SetTransparency(20);

//    fiberglassV->SetLineColor(TColor::GetColor("#ff9933"));
//    fiberglassV->SetTransparency(20);

//    honeycombV->SetLineColor(TColor::GetColor("#ffff00"));
//    honeycombV->SetTransparency(20);

//    verticalframeV->SetLineColor(TColor::GetColor("#9999ff"));
//    verticalframeV->SetTransparency(20);

//    horizontalframeV->SetLineColor(TColor::GetColor("#9999ff"));
//    horizontalframeV->SetTransparency(20);

//    gasV->AddNode(sensV, 0, new TGeoTranslation(0.0, -0.5*(YGasSize-YSensSize), 0.0));

//    moduleV->AddNode(gasV, 0, new TGeoTranslation(0.0, 0.0, 0.0));

    moduleV->AddNode(sensV, 0, new TGeoTranslation(0.,  0., 0.0));
    moduleV->AddNode(sensCuV, 0, new TGeoTranslation(0., 0., zShiftCu1  ));
    moduleV->AddNode(sensCuV, 1, new TGeoTranslation(0., 0., -1.*zShiftCu1  ));

//    moduleV->AddNode(fiberglassV, 0, new TGeoTranslation(0.0, -0.5*dYFrameSize, -(0.5*ZGasSize + 0.5*dZFiberglassPanel)));
//    moduleV->AddNode(fiberglassV, 1, new TGeoTranslation(0.0, -0.5*dYFrameSize, +(0.5*ZGasSize + 0.5*dZFiberglassPanel)));
//    moduleV->AddNode(honeycombV, 0, new TGeoTranslation(0.0, -0.5*dYFrameSize, -(0.5*ZGasSize + dZFiberglassPanel + 0.5*dZHoneycombPanel)));
//    moduleV->AddNode(honeycombV, 1, new TGeoTranslation(0.0, -0.5*dYFrameSize, +(0.5*ZGasSize + dZFiberglassPanel + 0.5*dZHoneycombPanel)));
//    moduleV->AddNode(fiberglassV, 2, new TGeoTranslation(0.0, -0.5*dYFrameSize, -(0.5*ZGasSize + dZFiberglassPanel + dZHoneycombPanel + 0.5*dZFiberglassPanel)));
//    moduleV->AddNode(fiberglassV, 3, new TGeoTranslation(0.0, -0.5*dYFrameSize, +(0.5*ZGasSize + dZFiberglassPanel + dZHoneycombPanel + 0.5*dZFiberglassPanel)));
//    moduleV->AddNode(verticalframeV, 0, new TGeoTranslation(+(0.5*XGasSize + 0.5*dXFrameSize), 0.0, 0.0));
//    moduleV->AddNode(verticalframeV, 1, new TGeoTranslation(-(0.5*XGasSize + 0.5*dXFrameSize), 0.0, 0.0));
//    moduleV->AddNode(horizontalframeV, 0, new TGeoTranslation(0.0, +(0.5*YGasSize), 0.0));

    return moduleV;
}
