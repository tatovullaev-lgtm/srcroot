//Creating Silicon 2022 SRC RUN geometry. Macro writes geometry to 'geometry/Silicon_RunSRCSpring2022.root'
#include "TGeoManager.h"
#include "TFile.h"
#include "TGeoMedium.h"
#include "TGeoXtru.h"
#include "TMath.h"

using namespace TMath;
//---------------------------

TGeoManager* gGeoMan = NULL;

//-------- Unity [cm] --------------------------------------------

//-------- Detector's position -----------------------------------
const Double_t Silicon_Xpos[4] = { 0.500000, 0.500000, 0.500000, 0.500000};
const Double_t Silicon_Ypos[4] = { -4.7, -4.7, -4.7, -4.7};
const Double_t Silicon_Zpos[4] = { -576.2 + 120.779 + 3.9 , //Si1 Plane
 -576.2 + 120.779 + 5.9,  //Si2 Plane
 -576.2 + 172.779 + 3.9,  //Si3 plane
 -576.2 + 173.779 + 5.9}; //Si4 plane
const Double_t Silicon_Zshift[4][4] = { {-1.14, -1.13, -1.9, -1.88},
{1.15, 1.14, 1.87, 1.87},
{-1.07, -1.12, -1.84, -1.86},
{1.14, 1.14, 1.88, 1.87}
};


//-------- Detector's parameters ---------------------------------
const Double_t X_Size = 6.300000;
const Double_t Y_Size = 6.300000;
const Double_t Z_Size = 0.028000;
const Double_t Z_Size1 = 0.018000;
const Double_t a = 6.042334;
const Double_t b = 0.774465;
Double_t c, X_Shift, Z_Shift;

void create_rootgeom_Silicon_RunSRCSpring2022() {

    //c = X_Size; X_Shift = (b * b + c * c - a * a) / (2. * c); 
    //Z_Shift = (TMath::Power(a, 4) + 2. * b * b * c * c - TMath::Power(a * a - b * b, 2) - TMath::Power(a * a - c * c, 2)) / (4. * c * c); 
    //Z_Shift = TMath::Sqrt(Z_Shift);
    
    TGeoRotation *geoRot = new TGeoRotation; 
    geoRot -> RotateZ(90.);
    
    // -------   Load media from media file   -----------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad -> getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace -> setMediaFile(medFile);
    geoFace -> readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "Silicon";
    const TString geoDetectorVersion = "RunSRCSpring2022";
    const TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia*   geoMedia = geoFace -> getMedia();
    FairGeoBuilder* geoBuild = geoLoad -> getGeoBuilder();

    FairGeoMedium* mAir = geoMedia -> getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild -> createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan -> GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");

    FairGeoMedium* mSilicon = geoMedia -> getMedium("silicon");
    if ( ! mSilicon ) Fatal("Main", "FairMedium silicon not found");
    geoBuild -> createMedium(mSilicon);
    TGeoMedium* pMedSilicon = gGeoMan -> GetMedium("silicon");
    if ( ! pMedSilicon ) Fatal("Main", "Medium silicon not found");

    // --------------------------------------------------------------------------
    
    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT -> FindObject("FAIRGeom");
    gGeoMan -> SetName(geoDetectorName + "_geom");
    //gGeoMan->SetTopVisible(1);
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top -> SetMedium(pMedAir);
    gGeoMan -> SetTopVolume(top);

    //----------- Active zone ---------------------------------------------------
    TGeoBBox *SensorActive_ZoneS = new TGeoBBox("SensorActive_ZoneS", X_Size / 2., 2. * Y_Size / 2.,  Z_Size / 2.);
    TGeoBBox *SensorActive_Zone1S = new TGeoBBox("SensorActive_Zone1S", X_Size / 2., Y_Size / 2.,  Z_Size1 / 2.);
    
    
    //----------- Volumes -------------------------------------------------------
    TGeoVolume *DetectorV = new TGeoVolumeAssembly("Detector");
    DetectorV -> SetMedium(pMedAir);
    
    TGeoVolume *SiliconV = new TGeoVolumeAssembly(geoDetectorName);
    SiliconV -> SetMedium(pMedAir);
    
    TGeoVolume *SensorActive_ZoneV = new TGeoVolume("SensorActive_ZoneV", SensorActive_ZoneS);
    SensorActive_ZoneV -> SetMedium(pMedSilicon);
    SensorActive_ZoneV -> SetLineColor(kGreen);
    SensorActive_ZoneV -> SetTransparency(50);

    TGeoVolume *SensorActive_Zone1V = new TGeoVolume("SensorActive_Zone1V", SensorActive_Zone1S);
    SensorActive_Zone1V -> SetMedium(pMedSilicon);
    SensorActive_Zone1V -> SetLineColor(kRed);
    SensorActive_Zone1V -> SetTransparency(50);
    
    //-------------- Adding volumes to the TOP Volume ---------------------------
    // DetectorV -> AddNode(SensorActive_ZoneV, 0, new TGeoTranslation(0, 0.,Silicon_Zshift[0][0]));
    // DetectorV -> AddNode(SensorActive_ZoneV, 1, new TGeoTranslation(0, 0.,Silicon_Zshift[0][0]);
    // DetectorV -> AddNode(SensorActive_ZoneV, 1, new TGeoTranslation(0, 0.,Silicon_Zshift[0][0]);
    // DetectorV -> AddNode(SensorActive_ZoneV, 1, new TGeoTranslation(0, 0.,Silicon_Zshift[0][0]);
    
    SiliconV -> AddNode(SensorActive_ZoneV, 0, new TGeoTranslation(Silicon_Xpos[0]+X_Size/2, Silicon_Ypos[0], Silicon_Zpos[0]+Silicon_Zshift[0][0]));
    SiliconV -> AddNode(SensorActive_ZoneV, 0, new TGeoTranslation(Silicon_Xpos[0]-X_Size/2, Silicon_Ypos[0], Silicon_Zpos[0]+Silicon_Zshift[0][1]));
    SiliconV -> AddNode(SensorActive_ZoneV, 0, new TGeoCombiTrans(Silicon_Xpos[0], Silicon_Ypos[0]+Y_Size/2, Silicon_Zpos[0]+Silicon_Zshift[0][2], geoRot));
    SiliconV -> AddNode(SensorActive_ZoneV, 0, new TGeoCombiTrans(Silicon_Xpos[0], Silicon_Ypos[0]-Y_Size/2, Silicon_Zpos[0]+Silicon_Zshift[0][3], geoRot));
    SiliconV -> AddNode(SensorActive_ZoneV, 1, new TGeoTranslation(Silicon_Xpos[1]+X_Size/2, Silicon_Ypos[1], Silicon_Zpos[1]+Silicon_Zshift[1][0]));
    SiliconV -> AddNode(SensorActive_ZoneV, 1, new TGeoTranslation(Silicon_Xpos[1]-X_Size/2, Silicon_Ypos[1], Silicon_Zpos[1]+Silicon_Zshift[1][1]));
    SiliconV -> AddNode(SensorActive_ZoneV, 1, new TGeoCombiTrans(Silicon_Xpos[1], Silicon_Ypos[1]+Y_Size/2, Silicon_Zpos[1]+Silicon_Zshift[1][2], geoRot));
    SiliconV -> AddNode(SensorActive_ZoneV, 1, new TGeoCombiTrans(Silicon_Xpos[1], Silicon_Ypos[1]-Y_Size/2, Silicon_Zpos[1]+Silicon_Zshift[1][3], geoRot));
    SiliconV -> AddNode(SensorActive_ZoneV, 2, new TGeoTranslation(Silicon_Xpos[2]+X_Size/2, Silicon_Ypos[2], Silicon_Zpos[2]+Silicon_Zshift[2][0]));
    SiliconV -> AddNode(SensorActive_ZoneV, 2, new TGeoTranslation(Silicon_Xpos[2]-X_Size/2, Silicon_Ypos[2], Silicon_Zpos[2]+Silicon_Zshift[2][1]));
    SiliconV -> AddNode(SensorActive_ZoneV, 2, new TGeoCombiTrans(Silicon_Xpos[2], Silicon_Ypos[2]+Y_Size/2, Silicon_Zpos[2]+Silicon_Zshift[2][2], geoRot));
    SiliconV -> AddNode(SensorActive_ZoneV, 2, new TGeoCombiTrans(Silicon_Xpos[2], Silicon_Ypos[2]-Y_Size/2, Silicon_Zpos[2]+Silicon_Zshift[2][3], geoRot));
    SiliconV -> AddNode(SensorActive_ZoneV, 3, new TGeoTranslation(Silicon_Xpos[3]+X_Size/2, Silicon_Ypos[3], Silicon_Zpos[3]+Silicon_Zshift[3][0]));
    SiliconV -> AddNode(SensorActive_ZoneV, 3, new TGeoTranslation(Silicon_Xpos[3]-X_Size/2, Silicon_Ypos[3], Silicon_Zpos[3]+Silicon_Zshift[3][1]));
    SiliconV -> AddNode(SensorActive_ZoneV, 3, new TGeoCombiTrans(Silicon_Xpos[3], Silicon_Ypos[3]+Y_Size/2, Silicon_Zpos[3]+Silicon_Zshift[3][2], geoRot));
    SiliconV -> AddNode(SensorActive_ZoneV, 3, new TGeoCombiTrans(Silicon_Xpos[3], Silicon_Ypos[3]-Y_Size/2, Silicon_Zpos[3]+Silicon_Zshift[3][3], geoRot));
    SiliconV -> AddNode(SensorActive_Zone1V, 4, new TGeoTranslation(Silicon_Xpos[3], Silicon_Ypos[3], -576.2-200));
    
    top -> AddNode(SiliconV, 0);
    
    
    //---------------- Finish ---------------------------------------------------
    gGeoMan -> CloseGeometry();
    gGeoMan -> CheckOverlaps(0.000001);
    gGeoMan -> PrintOverlaps();
    gGeoMan -> Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top -> Write();
    geoFile -> Close();
    top -> Draw("ogl");
    
}
