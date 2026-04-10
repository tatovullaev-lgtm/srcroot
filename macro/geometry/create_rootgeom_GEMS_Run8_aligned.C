/*
 * Baranov D.
 * 25.02.2020 (created)
 * 18.01.2021 (updated)
 * 02.03.2021 (updated): radius of hole in active zone (4.0 -> 6.0 cm)
 * 12.12.2022 (updated): small GEM added
 * Use this macro to create ROOT geometry for the FutureConfig2020 GEM configuration
 *
 */

//Set Parameters of GEMS -------------------------------------------------------
const Int_t NStations = 8;      //stations in the detector
const Int_t NMaxModules = 4;    //max. number of modules in a station

//(X-Y-Z)Positions of stations (sensitive volumes)
const Double_t XStationPositions[NStations] = { +1.474/*0*/, +2.614/*1*/, +3.393/*2*/, +4.699/*3*/, +6.337/*4*/, +8.189/*5*/, +9.893/*6*/, +50.494/*7*/};
const Double_t YStationPositions[NStations] = { -0.086/*0*/, -0.107/*1*/, +0.003/*2*/, -0.097/*3*/, -0.003/*4*/, +0.0245/*5*/, +0.022/*6*/, -0.186/*7*/};
const Double_t ZStationPositions[NStations] = { +60.76/*0*/, +92.242/*1*/, +123.512/*2*/, +154.692/*3*/, +185.989/*4*/, +217.287/*5*/, +248.401/*6*/, +792.532/*7*/};

//(X-Y-Z)Shifts of modules in each station
Double_t XModuleShifts[NStations][NMaxModules] = {
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 0
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 1
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 2
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 3
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 4
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 5
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 6
    {0.0}                                               //st. 7
};
Double_t YModuleShifts[NStations][NMaxModules] = {
    {+22.5, +22.5, -19.5, -19.5},  //st. 0
    {+22.5, +22.5, -19.5, -19.5},  //st. 1
    {+22.5, +22.5, -19.5, -19.5},  //st. 2
    {+22.5, +22.5, -19.5, -19.5},  //st. 3
    {+22.5, +22.5, -19.5, -19.5},  //st. 4
    {+22.5, +22.5, -19.5, -19.5},  //st. 5
    {+22.5, +22.5, -19.5, -19.5},  //st. 6
    {0.0}                          //st. 7
};
Double_t ZModuleShifts[NStations][NMaxModules] = {
    {+4.1, +4.1, 0.0, 0.0}, //st. 0
    {0.0, 0.0, +4.1, +4.1}, //st. 1
    {+4.1, +4.1, 0.0, 0.0}, //st. 2
    {0.0, 0.0, +4.1, +4.1}, //st. 3
    {+4.1, +4.1, 0.0, 0.0}, //st. 4
    {0.0, 0.0, +4.1, +4.1}, //st. 5
    {+4.1, +4.1, 0.0, 0.0}, //st. 6
    {0.0}                   //st. 7
};

//alignment corrections --------------------------------------------------------
const Double_t StsDX[NStations][8] = {
    {2.28613e-01,2.34606e-01,3.70449e-01,4.39267e-01, 2.67664e-01,2.84859e-01,3.41005e-01,4.16753e-01},
    {3.17030e-01,2.45618e-01,4.29878e-01,4.29169e-01, 2.60443e-01,1.79925e-01,3.76252e-01,3.71674e-01},
    {1.25543e-01,1.37935e-01,2.77096e-01,3.51704e-01, 2.16012e-01,2.12139e-01,2.88912e-01,3.57699e-01 },
    {3.17647e-01,2.36295e-01,3.75431e-01,3.80575e-01, 1.68884e-01,8.80524e-02,2.51805e-01,2.56513e-01},
    {-3.01883e-02,-2.87880e-02,1.09744e-01,1.90429e-01, 1.78162e-02,1.32671e-02,9.23760e-02,1.67779e-01},
    {1.43966e-01,5.53303e-02,2.48555e-01,2.49193e-01, 3.88544e-02,-3.31393e-02,7.96085e-02,7.41273e-02},
    {0.,-5.63645e-04,0.05,8.74027e-04, 0.,3.60763e-03,0.074,1.49797e-01}
};
const Double_t StsDY[NStations][8] = {
    {-2.40830e-01,-2.27117e-01,-4.41393e-01,-2.98662e-01, -1.47376e-01,-5.64919e-01,3.60250e-02,-2.15290e-01},
    {-4.83443e-01,-3.63780e-01,-3.52015e-01,-3.36903e-01, -1.14008e-01,-3.49136e-01,-2.14579e-01,-6.23524e-01},
    {-1.91062e-01,-2.04515e-01,-3.79972e-01,-2.57092e-01, -1.01400e-01, -5.18137e-01, 5.39497e-02, -1.85679e-01},
    {-4.55871e-01,-3.48696e-01, -2.33191e-01, -2.40044e-01, -5.37329e-02,-3.00596e-01, -1.98740e-01, -6.04983e-01},
    {-2.31062e-01, -2.58276e-01, -4.03759e-01, -2.70196e-01, -8.53213e-02, -5.14891e-01, 3.15495e-02, -2.08464e-01},
    {-2.58821e-01, -1.58621e-01, -9.84743e-02, -1.05756e-01, 1.54258e-01, -1.04935e-01, 1.83280e-02, -3.86622e-01},
    {0.,-5.86937e-03,-0.23,-1.45342e-01, 0.,-3.98117e-01,0.15,-1.04839e-01}
};

const Double_t XAlignmentCorrections[NStations][NMaxModules] = {
    {StsDX[0][0], StsDX[0][2], StsDX[0][4], StsDX[0][6]}, //st. 0
    {StsDX[1][0], StsDX[1][2], StsDX[1][4], StsDX[1][6]}, //st. 1
    {StsDX[2][0], StsDX[2][2], StsDX[2][4], StsDX[2][6]}, //st. 2
    {StsDX[3][0], StsDX[3][2], StsDX[3][4], StsDX[3][6]}, //st. 3
    {StsDX[4][0], StsDX[4][2], StsDX[4][4], StsDX[4][6]}, //st. 4
    {StsDX[5][0], StsDX[5][2], StsDX[5][4], StsDX[5][6]}, //st. 5
    {StsDX[6][0], StsDX[6][2], StsDX[6][4], StsDX[6][6]}, //st. 6
    {0.0}                                                 //st. 7
};

const Double_t YAlignmentCorrections[NStations][NMaxModules] = {
    {StsDY[0][0], StsDY[0][2], StsDY[0][4], StsDY[0][6]}, //st. 0
    {StsDY[1][0], StsDY[1][2], StsDY[1][4], StsDY[1][6]}, //st. 1
    {StsDY[2][0], StsDY[2][2], StsDY[2][4], StsDY[2][6]}, //st. 2
    {StsDY[3][0], StsDY[3][2], StsDY[3][4], StsDY[3][6]}, //st. 3
    {StsDY[4][0], StsDY[4][2], StsDY[4][4], StsDY[4][6]}, //st. 4
    {StsDY[5][0], StsDY[5][2], StsDY[5][4], StsDY[5][6]}, //st. 5
    {StsDY[6][0], StsDY[6][2], StsDY[6][4], StsDY[6][6]}, //st. 6
    {0.0}                                                 //st. 7
};

const Double_t ZAlignmentCorrections[NStations][NMaxModules] = {
    {0.243654,0.250775,0.173749,0.161366},            //st. 0
    {-0.143876,-0.150758,0.109959,0.156433},          //st. 1
    {-0.110145,-0.165981,-0.213889,-0.229543},        //st. 2
    {0.103517,0.036342,0.086135,0.0941369},           //st. 3
    {-0.037648,-0.0941807,-0.180758,-0.176327},       //st. 4
    {-0.0674491,-0.0367844,-0.0972473,-0.0584354},    //st. 5
    {0,0,0,0},                                        //st. 6
    {0.0}                                             //st. 7
};
//------------------------------------------------------------------------------

//GEM plane sizes (66x41 type) -------------------------------------------------
const Double_t XModuleSize_Station66x41 = 66.0;
const Double_t YModuleSize_Station66x41 = 41.0;
const Double_t ZModuleSize_Station66x41 = 0.9;

const Double_t dXFrame_Station66x41 = 2.0;
const Double_t dYFrame_Station66x41 = 2.0;
const Double_t dZFrame_Station66x41 = 4.1;
//------------------------------------------------------------------------------

//GEM plane sizes (163x45 type) ------------------------------------------------
const Double_t XModuleSize_Station163x45 = 163.2*0.5; //81.6
const Double_t YModuleSize_Station163x45 = 45.0;
const Double_t ZModuleSize_Station163x45 = 0.9;

const Double_t dXFrame_Station163x45 = 2.0;
const Double_t dYFrame_Station163x45 = 2.0;
const Double_t dZFrame_Station163x45 = 4.1;
//------------------------------------------------------------------------------

//GEM plane sizes (163x39 type) ------------------------------------------------
const Double_t XModuleSize_Station163x39 = 163.2*0.5; //81.6
const Double_t YModuleSize_Station163x39 = 39.0;
const Double_t ZModuleSize_Station163x39 = 0.9; //sensitive volume
const Double_t ZGEMSize_Station163x39 = 4.1; //common thickness of GEM (including sens. vol)

const Double_t dXFrame_Station163x39 = 2.0;
const Double_t dYFrame_Station163x39 = 2.0;
const Double_t dZFrame_Station163x39 = 4.1;
//------------------------------------------------------------------------------

//GEM plane sizes (10x10 type) -------------------------------------------------
const Double_t XModuleSize_Station10x10 = 10.0; //81.6
const Double_t YModuleSize_Station10x10 = 10.0;
const Double_t ZModuleSize_Station10x10 = 0.9; //sensitive volume
const Double_t ZGEMSize_Station10x10 = 4.1; //common thickness of GEM (including sens. vol)

const Double_t dXFrame_Station10x10 = 2.0;
const Double_t dYFrame_Station10x10 = 2.0;
const Double_t dZFrame_Station10x10 = 4.1;
//------------------------------------------------------------------------------

//GeoManager
TGeoManager* gGeoMan = NULL;

//media
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedCopper = 0;
TGeoMedium *pMedArCO27030 = 0;
TGeoMedium *pMedArC4H10_80_20 = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateStation(TString station_name);

TGeoVolume *CreateModule_Station66x41(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize);
TGeoVolume *CreateFrameForModule_Station66x41(TString frame_name, Double_t dx, Double_t dy, Double_t dz);

TGeoVolume *CreateModule_Station163x45(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius);
TGeoVolume *CreateFrameForModule_Station163x45(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius);

TGeoVolume *CreateModule_Station163x39(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius);
TGeoVolume *CreateFrameForModule_Station163x39(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius);

TGeoVolume *CreateModule_Station10x10(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize);

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

    //arco27020 medium
    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if ( ! mArCO27030  ) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    pMedArCO27030= gGeoManager->GetMedium("arco27030");
    if ( ! pMedArCO27030  ) Fatal("Main", "Medium arco27030 not found");

    //ArC4H10_80_20 medium
    FairGeoMedium* mArC4H10_80_20 = geoMedia->getMedium("ArC4H10_80_20");
    if ( ! mArC4H10_80_20  ) Fatal("Main", "FairMedium ArC4H10_80_20 not found");
    geoBuild->createMedium(mArC4H10_80_20);
    pMedArC4H10_80_20 = gGeoManager->GetMedium("ArC4H10_80_20");
    if ( ! pMedArC4H10_80_20  ) Fatal("Main", "Medium ArC4H10_80_20 not found");
}

void create_rootgeom_GEMS_Run8_aligned() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ---------------------------------
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "Run8_aligned";
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
    TGeoVolume* GEMS = new TGeoVolumeAssembly(geoDetectorName);
    GEMS->SetMedium(pMedAir);

    for(Int_t istation = 0; istation < NStations; ++istation) {

        Int_t stationNum = istation;
        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));

        //Applying alignment corrections to module shifts !!!!!!!!!!
        for(Int_t imodule = 0; imodule < NMaxModules; ++imodule) {
            XModuleShifts[istation][imodule] -= XAlignmentCorrections[istation][imodule];
            YModuleShifts[istation][imodule] -= YAlignmentCorrections[istation][imodule];
            ZModuleShifts[istation][imodule] += ZAlignmentCorrections[istation][imodule];
        }

        if(istation < 7)  { //big GEMs

            //upper part
            TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0+2.0);
            TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_")+station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0+2.0);
            TGeoVolume *frame0 = CreateFrameForModule_Station163x45(TString("frame0_")+station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0+2.0);
            TGeoVolume *frame1 = CreateFrameForModule_Station163x45(TString("frame1_")+station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0+2.0);

            //lower part
            TGeoVolume *module2 = CreateModule_Station163x39(TString("Sensor_module2_")+station->GetName(), XModuleSize_Station163x39, YModuleSize_Station163x39, ZModuleSize_Station163x39, 4.0+2.0);
            TGeoVolume *module3 = CreateModule_Station163x39(TString("Sensor_module3_")+station->GetName(), XModuleSize_Station163x39, YModuleSize_Station163x39, ZModuleSize_Station163x39, 4.0+2.0);
            TGeoVolume *frame2 = CreateFrameForModule_Station163x39(TString("frame2_")+station->GetName(), dXFrame_Station163x39, dYFrame_Station163x39, dZFrame_Station163x39, 4.0+2.0);
            TGeoVolume *frame3 = CreateFrameForModule_Station163x39(TString("frame3_")+station->GetName(), dXFrame_Station163x39, dYFrame_Station163x39, dZFrame_Station163x39, 4.0+2.0);

            TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
                module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station163x45*0.5);

            TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
                module1_transform->ReflectX(true);
                module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+ZModuleSize_Station163x45*0.5);

            TGeoCombiTrans *module2_transform = new TGeoCombiTrans();
                module2_transform->ReflectY(true);
                module2_transform->SetTranslation(XModuleShifts[stationNum][2], YModuleShifts[stationNum][2], ZModuleShifts[stationNum][2]+ZModuleSize_Station163x45*0.5);

            TGeoCombiTrans *module3_transform = new TGeoCombiTrans();
                module3_transform->ReflectY(true);
                module3_transform->ReflectX(true);
                module3_transform->SetTranslation(XModuleShifts[stationNum][3], YModuleShifts[stationNum][3], ZModuleShifts[stationNum][3]+ZModuleSize_Station163x45*0.5);

            TGeoCombiTrans *station_transform = new TGeoCombiTrans();
            station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

            station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
            station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform)); //module
            station->AddNode(module2, 0, new TGeoCombiTrans(*module2_transform)); //module
            station->AddNode(module3, 0, new TGeoCombiTrans(*module3_transform)); //module

            //station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame
            //station->AddNode(frame1, 0, new TGeoCombiTrans(*module1_transform)); //frame
            //station->AddNode(frame2, 0, new TGeoCombiTrans(*module2_transform)); //frame
            //station->AddNode(frame3, 0, new TGeoCombiTrans(*module3_transform)); //frame
        }

        if (istation == 7) { //small GEM
            TGeoVolume *module0 = CreateModule_Station10x10(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station10x10, YModuleSize_Station10x10, ZModuleSize_Station10x10);

            TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
                module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station10x10*0.5);

            station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        }

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
            station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        GEMS->AddNode(station, 0, station_transform);
    }

    top->AddNode(GEMS, 0);
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

//------------------------------------------------------------------------------

TGeoVolume *CreateStation(TString station_name) {

    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    return stationA;
}

TGeoVolume *CreateModule_Station66x41(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize) {

    //module shape
    TGeoShape *moduleS = new TGeoBBox("moduleS", xsize*0.5, ysize*0.5, zsize*0.5);

    //module volume
    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //medium
    if(pMedArC4H10_80_20) moduleV->SetMedium(pMedArC4H10_80_20);
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //visual parameters
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(20);

    return moduleV;
}

TGeoVolume *CreateFrameForModule_Station66x41(TString frame_name, Double_t dx, Double_t dy, Double_t dz) {

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox("vertical_frameS", dx*0.5, YModuleSize_Station66x41*0.5+dy, dz*0.5);
    TGeoShape *horizontal_frameS = new TGeoBBox("horizontal_frameS", XModuleSize_Station66x41*0.5, dy*0.5, dz*0.5);

    //frame volumes
    TGeoVolume *composite_frameV = new TGeoVolumeAssembly(frame_name);
    TGeoVolume *vertical_frameV = new TGeoVolume(frame_name+"_vertical_frameV", vertical_frameS);
    TGeoVolume *horizontal_frameV = new TGeoVolume(frame_name+"_horizontal_frameV", horizontal_frameS);

    //media
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if(pMedCarbon) {
        vertical_frameV->SetMedium(frame_medium);
        horizontal_frameV->SetMedium(frame_medium);
    }
    else Fatal("Main", "Invalid medium for frames!");
    if(pMedAir) {
        composite_frameV->SetMedium(pMedAir);
    }
    else Fatal("Main", "Invalid medium for frames!");

    //visual parameters
    vertical_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //frame part positions in the frame
    TGeoCombiTrans *left_vertical_frame_position = new TGeoCombiTrans();
    left_vertical_frame_position->SetTranslation(XModuleSize_Station66x41*0.5+dXFrame_Station66x41*0.5, 0.0, 0.0);

    TGeoCombiTrans *right_vertical_frame_position = new TGeoCombiTrans();
    right_vertical_frame_position->SetTranslation(-XModuleSize_Station66x41*0.5-dXFrame_Station66x41*0.5, 0.0, 0.0);

    TGeoCombiTrans *bottom_horizontal_frame_position = new TGeoCombiTrans();
    bottom_horizontal_frame_position->SetTranslation(0.0, YModuleSize_Station66x41*0.5+dYFrame_Station66x41*0.5, 0.0);

    TGeoCombiTrans *lower_horizontal_frame_position = new TGeoCombiTrans();
    lower_horizontal_frame_position->SetTranslation(0.0, -YModuleSize_Station66x41*0.5-dYFrame_Station66x41*0.5, 0.0);

    composite_frameV->AddNode(vertical_frameV, 0, left_vertical_frame_position);
    composite_frameV->AddNode(vertical_frameV, 1, right_vertical_frame_position);
    composite_frameV->AddNode(horizontal_frameV, 0, bottom_horizontal_frame_position);
    composite_frameV->AddNode(horizontal_frameV, 1, lower_horizontal_frame_position);

    return composite_frameV;
}

TGeoVolume *CreateModule_Station163x45(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius) {

    gRandom->SetSeed(0);
    Int_t uniqueId = gRandom->Integer(100000);

    //module shapes
    TGeoShape *module_partS = new TGeoBBox(TString("module_partS")+TString::Itoa(uniqueId,10), xsize*0.5, ysize*0.5, zsize*0.5);
    TGeoShape *holeS = new TGeoTube(TString("holeS")+TString::Itoa(uniqueId,10), 0.0, hole_radius, zsize*0.5+0.01);

    TGeoTranslation *hole_module_trans = new TGeoTranslation(TString("hole_module_trans_")+TString::Itoa(uniqueId,10), -xsize*0.5, -ysize*0.5, 0.0);

    hole_module_trans->RegisterYourself();

    TString bool_expression_module = TString(module_partS->GetName()) + TString(" - ") + TString(holeS->GetName()) + TString(":") + TString(hole_module_trans->GetName());
    TGeoCompositeShape *moduleS = new TGeoCompositeShape(bool_expression_module);

    //module volume
    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //medium
    if(pMedArC4H10_80_20) moduleV->SetMedium(pMedArC4H10_80_20);
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //visual parameters
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(20);

    return moduleV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateFrameForModule_Station163x45(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius) {

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox("vertical_frameS", dx*0.5, YModuleSize_Station163x45*0.5+dy, dz*0.5);
    TGeoShape *horizontal_upper_frameS = new TGeoBBox("horizontal_upper_frameS", XModuleSize_Station163x45*0.5, dy*0.5, dz*0.5);
    TGeoShape *horizontal_bottom_frameS = new TGeoBBox("horizontal_upper_frameS", XModuleSize_Station163x45*0.5-hole_radius*0.5, dy*0.5, dz*0.5);

    //frame volumes
    TGeoVolume *composite_frameV = new TGeoVolumeAssembly(frame_name);
    TGeoVolume *vertical_frameV = new TGeoVolume(frame_name+"_vertical_frameV", vertical_frameS);
    TGeoVolume *horizontal_upper_frameV = new TGeoVolume(frame_name+"_horizontal_upper_frameV", horizontal_upper_frameS);
    TGeoVolume *horizontal_bottom_frameV = new TGeoVolume(frame_name+"_horizontal_bottom_frameV", horizontal_bottom_frameS);

    //media
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if(pMedCarbon) {
        vertical_frameV->SetMedium(frame_medium);
        horizontal_upper_frameV->SetMedium(frame_medium);
        horizontal_bottom_frameV->SetMedium(frame_medium);
    }
    else Fatal("Main", "Invalid medium for frames!");
    if(pMedAir) {
        composite_frameV->SetMedium(pMedAir);
    }
    else Fatal("Main", "Invalid medium for frames!");

    //visual parameters
    vertical_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_upper_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_bottom_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //frame part positions in the frame
    TGeoCombiTrans *left_vertical_frame_position = new TGeoCombiTrans();
    left_vertical_frame_position->SetTranslation(XModuleSize_Station163x45*0.5+dXFrame_Station163x45*0.5, 0.0, 0.0);

    TGeoCombiTrans *upper_horizontal_frame_position = new TGeoCombiTrans();
    upper_horizontal_frame_position->SetTranslation(0.0, YModuleSize_Station163x45*0.5+dYFrame_Station163x45*0.5, 0.0);

    TGeoCombiTrans *bottom_horizontal_frame_position = new TGeoCombiTrans();
    bottom_horizontal_frame_position->SetTranslation(hole_radius*0.5, -YModuleSize_Station163x45*0.5-dYFrame_Station163x45*0.5, 0.0);

    composite_frameV->AddNode(vertical_frameV, 0, left_vertical_frame_position);
    composite_frameV->AddNode(horizontal_upper_frameV, 0, upper_horizontal_frame_position);
    composite_frameV->AddNode(horizontal_bottom_frameV, 0, bottom_horizontal_frame_position);

    return composite_frameV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule_Station163x39(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius) {

    return CreateModule_Station163x45(module_name, xsize, ysize, zsize, hole_radius);
}
//------------------------------------------------------------------------------

TGeoVolume *CreateFrameForModule_Station163x39(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius) {

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox(TString("vertical_frameS")+=TString("_") + frame_name, dx*0.5, YModuleSize_Station163x39*0.5+dy, dz*0.5);
    TGeoShape *horizontal_upper_frameS = new TGeoBBox(TString("horizontal_upper_frameS")+=TString("_") + frame_name, XModuleSize_Station163x39*0.5, dy*0.5, dz*0.5);
    TGeoShape *horizontal_bottom_frameS = new TGeoBBox(TString("horizontal_upper_frameS")+=TString("_") + frame_name, XModuleSize_Station163x39*0.5-hole_radius*0.5, dy*0.5, dz*0.5);

    //frame volumes
    TGeoVolume *composite_frameV = new TGeoVolumeAssembly(frame_name);
    TGeoVolume *vertical_frameV = new TGeoVolume(frame_name+"_vertical_frameV", vertical_frameS);
    TGeoVolume *horizontal_upper_frameV = new TGeoVolume(frame_name+"_horizontal_upper_frameV", horizontal_upper_frameS);
    TGeoVolume *horizontal_bottom_frameV = new TGeoVolume(frame_name+"_horizontal_bottom_frameV", horizontal_bottom_frameS);

    //media
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if(pMedCarbon) {
        vertical_frameV->SetMedium(frame_medium);
        horizontal_upper_frameV->SetMedium(frame_medium);
        horizontal_bottom_frameV->SetMedium(frame_medium);
    }
    else Fatal("Main", "Invalid medium for frames!");
    if(pMedAir) {
        composite_frameV->SetMedium(pMedAir);
    }
    else Fatal("Main", "Invalid medium for frames!");

    //visual parameters
    vertical_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_upper_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_bottom_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //frame part positions in the frame
    TGeoCombiTrans *left_vertical_frame_position = new TGeoCombiTrans();
    left_vertical_frame_position->SetTranslation(XModuleSize_Station163x39*0.5+dXFrame_Station163x39*0.5, 0.0, 0.0);

    TGeoCombiTrans *upper_horizontal_frame_position = new TGeoCombiTrans();
    upper_horizontal_frame_position->SetTranslation(0.0, YModuleSize_Station163x39*0.5+dYFrame_Station163x39*0.5, 0.0);

    TGeoCombiTrans *bottom_horizontal_frame_position = new TGeoCombiTrans();
    bottom_horizontal_frame_position->SetTranslation(hole_radius*0.5, -YModuleSize_Station163x39*0.5-dYFrame_Station163x39*0.5, 0.0);

    composite_frameV->AddNode(vertical_frameV, 0, left_vertical_frame_position);
    composite_frameV->AddNode(horizontal_upper_frameV, 0, upper_horizontal_frame_position);
    composite_frameV->AddNode(horizontal_bottom_frameV, 0, bottom_horizontal_frame_position);

    return composite_frameV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule_Station10x10(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize) {

    //module shape
    TGeoShape *moduleS = new TGeoBBox("moduleS", xsize*0.5, ysize*0.5, zsize*0.5);

    //module volume
    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //medium
    if(pMedArC4H10_80_20) moduleV->SetMedium(pMedArC4H10_80_20);
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //visual parameters
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(20);

    return moduleV;
}
//------------------------------------------------------------------------------