/*
 * Use this macro to create ROOT geometry of Vacuum Beam Pipe (section 2: arter
 * target) for the RUN-8 configuration, according to the measurement scheme
 *
 * Author: Baranov D.
 * Created: 05.12.2022
 *
 *
 * WARNING: all units is in cm!!!
 *
 */

#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"

//Parameters of Vacuum Beam Pipe

const Int_t NModules = 4;

//Positions
const Double_t XModulePositions[NModules] = {
    +0.372,                     // [0]
    +0.372 + 0.65,              // [1]
    +0.372 + 0.65 + 3.14,       // [2]
    +0.372 + 0.65 + 3.14 + 6.97 // [3]
};
const Double_t YModulePositions[NModules] = {
    -0.079,    // [0]
    -0.079,    // [1]
    -0.079,    // [2]
    -0.079     // [3]
};
const Double_t ZModulePositions[NModules] = {
    -34.0,                                       // [0]
    -34.0 + 82.26,                               // [1]
    -34.0 + 82.26 + 101.54-1.5-0.05,             // [2]
    -34.0 + 82.26 + 101.54-1.5 + 125.58-1.5-0.35 // [3]
};

//Rotations of modules around Z (deg.)
const Double_t ZStationRotations[NModules] = {
    +0.0,  // [0]
    +0.0,  // [1]
    +0.0,  // [2]
    +0.0   // [3]
};


//------------------------------------------------------------------------------

//GeoManager
TGeoManager* gGeoMan = NULL;

//media
TGeoMedium *pMedVacuum = 0;
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedAluminium = 0;
TGeoMedium *pMedSteel = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedSilicon = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateModule1(TString module_name);
TGeoVolume *CreateModule2(TString module_name);
TGeoVolume *CreateModule3(TString module_name);
TGeoVolume *CreateModule4(TString module_name);

void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild) {

    //vacuum medium
    FairGeoMedium* mVacuum = geoMedia->getMedium("vacuum");
    if ( ! mVacuum ) Fatal("Main", "FairMedium vacuum not found");
    geoBuild->createMedium(mVacuum);
    pMedVacuum = gGeoManager->GetMedium("vacuum");
    if ( ! pMedVacuum ) Fatal("Main", "Medium vacuum not found");

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

}

void create_rootgeom_VacuumPipe_section2() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "VacuumPipe_section2";
    const TString geoDetectorVersion = "Run8";
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
    TGeoVolume* VacuumPipe_section2 = new TGeoVolumeAssembly(geoDetectorName);
    VacuumPipe_section2->SetMedium(pMedVacuum);

    TGeoVolume *module1 = CreateModule1(TString("VacuumPipeSec2_Module1"));
    TGeoVolume *module2 = CreateModule2(TString("VacuumPipeSec2_Module2"));
    TGeoVolume *module3 = CreateModule3(TString("VacuumPipeSec2_Module3"));
    TGeoVolume *module4 = CreateModule4(TString("VacuumPipeSec2_Module4"));

    TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
    module1_transform->RotateZ(ZStationRotations[0]);
    module1_transform->SetTranslation(XModulePositions[0], YModulePositions[0], ZModulePositions[0]);

    TGeoCombiTrans *module2_transform = new TGeoCombiTrans();
    module2_transform->RotateZ(ZStationRotations[1]);
    module2_transform->SetTranslation(XModulePositions[1], YModulePositions[1], ZModulePositions[1]);
    //module2_transform->SetTranslation(XModulePositions[1] + 0.65/*shift*/, YModulePositions[1], ZModulePositions[1] + 82.26/*shift*/);

    TGeoCombiTrans *module3_transform = new TGeoCombiTrans();
    module3_transform->RotateZ(ZStationRotations[2]);
    module3_transform->SetTranslation(XModulePositions[2], YModulePositions[2], ZModulePositions[2]);
    //module3_transform->SetTranslation(XModulePositions[2] + 0.65 + 3.14/*shift*/, YModulePositions[2], ZModulePositions[2] + 82.26 + 101.54-1.5-0.05/*shift*/);

    TGeoCombiTrans *module4_transform = new TGeoCombiTrans();
    module4_transform->RotateZ(ZStationRotations[3]);
    module4_transform->SetTranslation(XModulePositions[3], YModulePositions[3], ZModulePositions[3]);
    //module4_transform->SetTranslation(XModulePositions[3] + 0.65 + 3.14 + 6.97/*shift*/, YModulePositions[3], ZModulePositions[3] + 82.26 + 101.54-1.5 + 125.58-1.5-0.35/*shift*/);

    VacuumPipe_section2->AddNode(module1, 0, module1_transform);
    VacuumPipe_section2->AddNode(module2, 0, module2_transform);
    VacuumPipe_section2->AddNode(module3, 0, module3_transform);
    VacuumPipe_section2->AddNode(module4, 0, module4_transform);

    top->AddNode(VacuumPipe_section2, 0);
    top->SetVisContainers(kTRUE);

    // ---------------   Finish   ----------------------------------------------
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

TGeoVolume *CreateModule1(TString module_name) {

    Double_t length1 = 34.5;  // wide tube
    Double_t length2 = 5.8;   // cone
    Double_t length3 = 41.96; // narrow tube

    Double_t innerRadius1 = 6.4*0.5;
    Double_t outerRadius1 = 6.6*0.5;

    Double_t innerRadius2 = 4.8*0.5;
    Double_t outerRadius2 = 5.0*0.5;

    Double_t angleInclined3 = 0.88; //deg

    //shapes
    TGeoShape *vacTube1S = new TGeoTube(TString("vacTube1S")+=TString("_") + module_name, innerRadius1, outerRadius1, length1*0.5);
    TGeoShape *vacCone2S = new TGeoCone(TString("vacCone2S")+=TString("_") + module_name, length2*0.5, innerRadius1, outerRadius1, innerRadius2, outerRadius2);
    //TGeoShape *vacTube3S = new TGeoTube(TString("vacTube3S")+=TString("_") + module_name, innerRadius2, outerRadius2, length3*0.5);
    TGeoShape *vacTube3S = new TGeoCtub(TString("vacTube3S")+=TString("_") + module_name, innerRadius2, outerRadius2, length3*0.5, 0.0, 360.0,  0.015/*!!!*/, 0, -1,   0, 0, 1);

    TGeoShape *vacTube1VacuumS = new TGeoTube(TString("vacTube1VacuumS")+=TString("_") + module_name, 0.0, innerRadius1, length1*0.5);
    TGeoShape *vacCone2VacuumS = new TGeoCone(TString("vacCone2VacuumS")+=TString("_") + module_name, length2*0.5, 0.0, innerRadius1, 0.0, innerRadius2);
    TGeoShape *vacTube3VacuumS = new TGeoCtub(TString("vacTube3VacuumS")+=TString("_") + module_name, 0.0, innerRadius2, length3*0.5, 0.0, 360.0,  0.015/*!!!*/, 0, -1,   0, 0, 1);

    TGeoCombiTrans *vacTube3S_transf = new TGeoCombiTrans();
        vacTube3S_transf->SetName(TString("vacTube3S_transf")+=TString("_") + module_name);
        vacTube3S_transf->RotateY(angleInclined3);
        vacTube3S_transf->RegisterYourself();

    TGeoCombiTrans *vacTube3S_transf1 = new TGeoCombiTrans();
        vacTube3S_transf1->SetName(TString("vacTube3S_transf1")+=TString("_") + module_name);
        vacTube3S_transf1->SetTranslation(-1000.0, 0.0, 0.0);
        vacTube3S_transf1->RegisterYourself();

    TGeoCompositeShape *vacTube3RotS = new TGeoCompositeShape();
    vacTube3RotS->SetName(TString("vacTube3RotS")+=TString("_") + module_name);
    {
        TString expression = "vacTube3S"; expression += TString("_") + module_name;
        expression += ":vacTube3S_transf"; expression += TString("_") + module_name;
        expression += "-vacTube3S"; expression += TString("_") + module_name;
        expression += ":vacTube3S_transf1"; expression += TString("_") + module_name;

        vacTube3RotS->MakeNode(expression);
        vacTube3RotS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCompositeShape *vacTube3RotVacuumS = new TGeoCompositeShape();
    vacTube3RotVacuumS->SetName(TString("vacTube3RotVacuumS")+=TString("_") + module_name);
    {
        TString expression = "vacTube3VacuumS"; expression += TString("_") + module_name;
        expression += ":vacTube3S_transf"; expression += TString("_") + module_name;
        expression += "-vacTube3VacuumS"; expression += TString("_") + module_name;
        expression += ":vacTube3S_transf1"; expression += TString("_") + module_name;

        vacTube3RotVacuumS->MakeNode(expression);
        vacTube3RotVacuumS->ComputeBBox(); //need to compute a bounding box
    }

    //volumes
    TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
    TGeoVolume *vacTube1V = new TGeoVolume(TString("vacTube1V")+=TString("_") + module_name, vacTube1S);
    TGeoVolume *vacCone2V = new TGeoVolume(TString("vacCone2V")+=TString("_") + module_name, vacCone2S);
    //TGeoVolume *vacTube3V = new TGeoVolume(TString("vacTube3V")+=TString("_") + module_name, vacTube3S);
    TGeoVolume *vacTube3V = new TGeoVolume(TString("vacTube3V")+=TString("_") + module_name, vacTube3RotS);

    TGeoVolume *vacTube1VacuumV = new TGeoVolume(TString("vacTube1VacuumV")+=TString("_") + module_name, vacTube1VacuumS);
    TGeoVolume *vacCone2VacuumV = new TGeoVolume(TString("vacCone2VacuumV")+=TString("_") + module_name, vacCone2VacuumS);
    TGeoVolume *vacTube3VacuumV = new TGeoVolume(TString("vacTube3VacuumV")+=TString("_") + module_name, vacTube3RotVacuumS);

    //volume medium
    TGeoMedium *vacTubeV_medium = pMedCarbon;
    if(vacTubeV_medium) {
        vacTube1V->SetMedium(vacTubeV_medium);
        vacCone2V->SetMedium(vacTubeV_medium);
        vacTube3V->SetMedium(vacTubeV_medium);
    }
    else Fatal("Main", "Invalid medium for vacTubeV!");

    vacTube1VacuumV->SetMedium(pMedVacuum);
    vacCone2VacuumV->SetMedium(pMedVacuum);
    vacTube3VacuumV->SetMedium(pMedVacuum);

    //volume visual property (transparency)
    vacTube1V->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTube1V->SetTransparency(0);
    vacCone2V->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTube3V->SetLineColor(TColor::GetColor("#ffcccc"));

    //part positions in the vacuum tube box
    TGeoCombiTrans *vacTube1V_transf[1];
    vacTube1V_transf[0] = new TGeoCombiTrans();
    vacTube1V_transf[0]->SetTranslation(0.0, 0.0, +length1*0.5);

    TGeoCombiTrans *vacCone2V_transf[1];
    vacCone2V_transf[0] = new TGeoCombiTrans();
    vacCone2V_transf[0]->SetTranslation(0.0, 0.0, +length1 + length2*0.5);

    TGeoCombiTrans *vacTube3V_transf[1];
    vacTube3V_transf[0] = new TGeoCombiTrans();
    vacTube3V_transf[0]->SetTranslation(TMath::Sin(angleInclined3*TMath::DegToRad())*length3*0.5, 0.0, +length1 + length2 + length3*0.5);


    vacTubeBoxV->AddNode(vacTube1V, 0, vacTube1V_transf[0]);
    vacTubeBoxV->AddNode(vacCone2V, 0, vacCone2V_transf[0]);
    vacTubeBoxV->AddNode(vacTube3V, 0, vacTube3V_transf[0]);

    vacTubeBoxV->AddNode(vacTube1VacuumV, 0, vacTube1V_transf[0]);
    vacTubeBoxV->AddNode(vacCone2VacuumV, 0, vacCone2V_transf[0]);
    vacTubeBoxV->AddNode(vacTube3VacuumV, 0, vacTube3V_transf[0]);

    return vacTubeBoxV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule2(TString module_name) {

    Double_t length1 = 101.54-1.5;  // cm

    Double_t innerRadius1 = 4.8*0.5;
    Double_t outerRadius1 = 5.0*0.5;
    Double_t angleInclined1 = 1.78; //deg

    //shapes
    //TGeoShape *vacTube1S = new TGeoTube(TString("vacTube1S")+=TString("_") + module_name, innerRadius1, outerRadius1, length1*0.5);
    TGeoShape *vacTube1S = new TGeoCtub(TString("vacTube1S")+=TString("_") + module_name, innerRadius1, outerRadius1, length1*0.5, 0.0, 360.0,  0.015/*!!!*/, 0, -1,   0.015/*!!!*/, 0, 1);

    TGeoShape *vacTube1VacuumS = new TGeoCtub(TString("vacTube1VacuumS")+=TString("_") + module_name, 0.0, innerRadius1, length1*0.5, 0.0, 360.0,  0.015/*!!!*/, 0, -1,   0.015/*!!!*/, 0, 1);

    TGeoCombiTrans *vacTube1S_transf = new TGeoCombiTrans();
        vacTube1S_transf->SetName(TString("vacTube1S_transf")+=TString("_") + module_name);
        vacTube1S_transf->RotateY(angleInclined1);
        vacTube1S_transf->RegisterYourself();

    TGeoCombiTrans *vacTube1S_transf1 = new TGeoCombiTrans();
        vacTube1S_transf1->SetName(TString("vacTube1S_transf1")+=TString("_") + module_name);
        vacTube1S_transf1->SetTranslation(-1000.0, 0.0, 0.0);
        vacTube1S_transf1->RegisterYourself();

    TGeoCompositeShape *vacTube1RotS = new TGeoCompositeShape();
    vacTube1RotS->SetName(TString("vacTube1RotS")+=TString("_") + module_name);
    {
        TString expression = "vacTube1S"; expression += TString("_") + module_name;
        expression += ":vacTube1S_transf"; expression += TString("_") + module_name;
        expression += "-vacTube1S"; expression += TString("_") + module_name;
        expression += ":vacTube1S_transf1"; expression += TString("_") + module_name;

        vacTube1RotS->MakeNode(expression);
        vacTube1RotS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCompositeShape *vacTube1RotVacuumS = new TGeoCompositeShape();
    vacTube1RotVacuumS->SetName(TString("vacTube1RotVacuumS")+=TString("_") + module_name);
    {
        TString expression = "vacTube1VacuumS"; expression += TString("_") + module_name;
        expression += ":vacTube1S_transf"; expression += TString("_") + module_name;
        expression += "-vacTube1VacuumS"; expression += TString("_") + module_name;
        expression += ":vacTube1S_transf1"; expression += TString("_") + module_name;

        vacTube1RotVacuumS->MakeNode(expression);
        vacTube1RotVacuumS->ComputeBBox(); //need to compute a bounding box
    }

    //volumes
    TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
    //TGeoVolume *vacTube1V = new TGeoVolume(TString("vacTube1V")+=TString("_") + module_name, vacTube1S);
    TGeoVolume *vacTube1V = new TGeoVolume(TString("vacTube3V")+=TString("_") + module_name, vacTube1RotS);

    TGeoVolume *vacTube1VacuumV = new TGeoVolume(TString("vacTube1VacuumV")+=TString("_") + module_name, vacTube1RotVacuumS);

    //volume medium
    TGeoMedium *vacTubeV_medium = pMedCarbon;
    if(vacTubeV_medium) {
        vacTube1V->SetMedium(vacTubeV_medium);
    }
    else Fatal("Main", "Invalid medium for vacTubeV!");

    vacTube1VacuumV->SetMedium(pMedVacuum);

    //volume visual property (transparency)
    vacTube1V->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTube1V->SetTransparency(0);

    //part positions in the vacuum tube box
    TGeoCombiTrans *vacTube1V_transf[1];
    vacTube1V_transf[0] = new TGeoCombiTrans();
    vacTube1V_transf[0]->SetTranslation(TMath::Sin(angleInclined1*TMath::DegToRad())*length1*0.5, 0.0, length1*0.5);


    vacTubeBoxV->AddNode(vacTube1V, 0, vacTube1V_transf[0]);
    vacTubeBoxV->AddNode(vacTube1VacuumV, 0, vacTube1V_transf[0]);

    return vacTubeBoxV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule3(TString module_name) {

    Double_t length1 = 125.58-1.5;  // cm

    Double_t innerRadius1 = 4.8*0.5;
    Double_t outerRadius1 = 5.0*0.5;

    Double_t innerRadius2 = 7.0*0.5;
    Double_t outerRadius2 = 7.2*0.5;

    Double_t angleInclined1 = 3.22; //deg

    //shapes
    TGeoShape *vacCone1S = new TGeoCone(TString("vacCone1S")+=TString("_") + module_name, length1*0.5, innerRadius1, outerRadius1, innerRadius2, outerRadius2);

    TGeoShape *vacCone1VacuumS = new TGeoCone(TString("vacCone1VacuumS")+=TString("_") + module_name, length1*0.5, 0.0, innerRadius1, 0.0, innerRadius2);

    TGeoCombiTrans *vacCone1S_transf = new TGeoCombiTrans();
        vacCone1S_transf->SetName(TString("vacCone1S_transf")+=TString("_") + module_name);
        vacCone1S_transf->RotateY(angleInclined1);
        vacCone1S_transf->RegisterYourself();

    TGeoCombiTrans *vacCone1S_transf1 = new TGeoCombiTrans();
        vacCone1S_transf1->SetName(TString("vacCone1S_transf1")+=TString("_") + module_name);
        vacCone1S_transf1->SetTranslation(-1000.0, 0.0, 0.0);
        vacCone1S_transf1->RegisterYourself();

    TGeoCompositeShape *vacCone1RotS = new TGeoCompositeShape();
    vacCone1RotS->SetName(TString("vacCone1RotS")+=TString("_") + module_name);
    {
        TString expression = "vacCone1S"; expression += TString("_") + module_name;
        expression += ":vacCone1S_transf"; expression += TString("_") + module_name;
        expression += "-vacCone1S"; expression += TString("_") + module_name;
        expression += ":vacCone1S_transf1"; expression += TString("_") + module_name;

        vacCone1RotS->MakeNode(expression);
        vacCone1RotS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCompositeShape *vacCone1RotVacuumS = new TGeoCompositeShape();
    vacCone1RotVacuumS->SetName(TString("vacCone1RotVacuumS")+=TString("_") + module_name);
    {
        TString expression = "vacCone1VacuumS"; expression += TString("_") + module_name;
        expression += ":vacCone1S_transf"; expression += TString("_") + module_name;
        expression += "-vacCone1VacuumS"; expression += TString("_") + module_name;
        expression += ":vacCone1S_transf1"; expression += TString("_") + module_name;

        vacCone1RotVacuumS->MakeNode(expression);
        vacCone1RotVacuumS->ComputeBBox(); //need to compute a bounding box
    }

    //volumes
    TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
    TGeoVolume *vacCone1V = new TGeoVolume(TString("vacCone1V")+=TString("_") + module_name, vacCone1RotS);

    TGeoVolume *vacCone1VacuumV = new TGeoVolume(TString("vacCone1VacuumV")+=TString("_") + module_name, vacCone1RotVacuumS);

    //volume medium
    TGeoMedium *vacTubeV_medium = pMedCarbon;
    if(vacTubeV_medium) {
        vacCone1V->SetMedium(vacTubeV_medium);
    }
    else Fatal("Main", "Invalid medium for vacCone1V!");

    vacCone1VacuumV->SetMedium(pMedVacuum);

    //volume visual property (transparency)
    vacCone1V->SetLineColor(TColor::GetColor("#ffcccc"));
    vacCone1V->SetTransparency(0);

    //part positions in the vacuum tube box
    TGeoCombiTrans *vacCone1V_transf[1];
    vacCone1V_transf[0] = new TGeoCombiTrans();
    vacCone1V_transf[0]->SetTranslation(TMath::Sin(angleInclined1*TMath::DegToRad())*length1*0.5, 0.0, +length1*0.5);

    vacTubeBoxV->AddNode(vacCone1V, 0, vacCone1V_transf[0]);
    vacTubeBoxV->AddNode(vacCone1VacuumV, 0, vacCone1V_transf[0]);

    return vacTubeBoxV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule4(TString module_name) {

    Double_t length1 = 158.28-1.5;  // cm

    Double_t innerRadius1 = 7.0*0.5;
    Double_t outerRadius1 = 7.2*0.5;
    Double_t angleInclined1 = 4.25; //deg

    //shapes
    //TGeoShape *vacTube1S = new TGeoTube(TString("vacTube1S")+=TString("_") + module_name, innerRadius1, outerRadius1, length1*0.5);
    TGeoShape *vacTube1S = new TGeoCtub(TString("vacTube1S")+=TString("_") + module_name, innerRadius1, outerRadius1, length1*0.5, 0.0, 360.0,  0.015/*!!!*/, 0, -1,   0.0/*!!!*/, 0, 1);

    TGeoShape *vacTube1VacuumS = new TGeoCtub(TString("vacTube1VacuumS")+=TString("_") + module_name, 0.0, innerRadius1, length1*0.5, 0.0, 360.0,  0.015/*!!!*/, 0, -1,   0.0/*!!!*/, 0, 1);

    TGeoCombiTrans *vacTube1S_transf = new TGeoCombiTrans();
        vacTube1S_transf->SetName(TString("vacTube1S_transf")+=TString("_") + module_name);
        vacTube1S_transf->RotateY(angleInclined1);
        vacTube1S_transf->RegisterYourself();

    TGeoCombiTrans *vacTube1S_transf1 = new TGeoCombiTrans();
        vacTube1S_transf1->SetName(TString("vacTube1S_transf1")+=TString("_") + module_name);
        vacTube1S_transf1->SetTranslation(-1000.0, 0.0, 0.0);
        vacTube1S_transf1->RegisterYourself();

    TGeoCompositeShape *vacTube1RotS = new TGeoCompositeShape();
    vacTube1RotS->SetName(TString("vacTube1RotS")+=TString("_") + module_name);
    {
        TString expression = "vacTube1S"; expression += TString("_") + module_name;
        expression += ":vacTube1S_transf"; expression += TString("_") + module_name;
        expression += "-vacTube1S"; expression += TString("_") + module_name;
        expression += ":vacTube1S_transf1"; expression += TString("_") + module_name;

        vacTube1RotS->MakeNode(expression);
        vacTube1RotS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCompositeShape *vacTube1RotVacuumS = new TGeoCompositeShape();
    vacTube1RotVacuumS->SetName(TString("vacTube1RotVacuumS")+=TString("_") + module_name);
    {
        TString expression = "vacTube1VacuumS"; expression += TString("_") + module_name;
        expression += ":vacTube1S_transf"; expression += TString("_") + module_name;
        expression += "-vacTube1VacuumS"; expression += TString("_") + module_name;
        expression += ":vacTube1S_transf1"; expression += TString("_") + module_name;

        vacTube1RotVacuumS->MakeNode(expression);
        vacTube1RotVacuumS->ComputeBBox(); //need to compute a bounding box
    }

    //volumes
    TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
    //TGeoVolume *vacTube1V = new TGeoVolume(TString("vacTube1V")+=TString("_") + module_name, vacTube1S);
    TGeoVolume *vacTube1V = new TGeoVolume(TString("vacTube3V")+=TString("_") + module_name, vacTube1RotS);

    TGeoVolume *vacTube1VacuumV = new TGeoVolume(TString("vacTube1VacuumV")+=TString("_") + module_name, vacTube1RotVacuumS);

    //volume medium
    TGeoMedium *vacTubeV_medium = pMedCarbon;
    if(vacTubeV_medium) {
        vacTube1V->SetMedium(vacTubeV_medium);
    }
    else Fatal("Main", "Invalid medium for vacTubeV!");

    vacTube1VacuumV->SetMedium(pMedVacuum);

    //volume visual property (transparency)
    vacTube1V->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTube1V->SetTransparency(0);

    //part positions in the vacuum tube box
    TGeoCombiTrans *vacTube1V_transf[1];
    vacTube1V_transf[0] = new TGeoCombiTrans();
    vacTube1V_transf[0]->SetTranslation(TMath::Sin(angleInclined1*TMath::DegToRad())*length1*0.5, 0.0, length1*0.5);


    vacTubeBoxV->AddNode(vacTube1V, 0, vacTube1V_transf[0]);
    vacTubeBoxV->AddNode(vacTube1VacuumV, 0, vacTube1V_transf[0]);

    return vacTubeBoxV;
}
//------------------------------------------------------------------------------