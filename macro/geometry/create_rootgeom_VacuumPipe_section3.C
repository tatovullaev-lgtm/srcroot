/*
 * Use thtis macro to create ROOT geometry of Vacuum Beam Pipe (section 3: arter
 * the magnet) for the RUN-8 configuration, according to the measurement scheme
 *
 * Author: Baranov D.
 * Created: 06.12.2022
 *
 *
 * WARNING: all units is in cm!!!
 *
 */

#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"

//Parameters of Vacuum Beam Pipe

const Int_t NModules = 1;

//Positions
const Double_t XModulePositions[NModules] = {
    +22.998 // [0]
};
const Double_t YModulePositions[NModules] = {
    -0.005 // [0]
};
const Double_t ZModulePositions[NModules] = {
    +432.019 // [0]
};

//Rotations of modules around Z (deg.)
const Double_t ZStationRotations[NModules] = {
    +0.0  // [0]
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

TGeoVolume *CreateTube(TString module_name);


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

void create_rootgeom_VacuumPipe_section3() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "VacuumPipe_section3";
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
    TGeoVolume* VacuumPipe_section3 = new TGeoVolumeAssembly(geoDetectorName);
    VacuumPipe_section3->SetMedium(pMedVacuum);

    TGeoVolume *tube = CreateTube(TString("VacuumPipeSec3_Tube"));

    TGeoCombiTrans *tube_transform = new TGeoCombiTrans();
    tube_transform->RotateY(4.2392);
    tube_transform->SetTranslation(XModulePositions[0], YModulePositions[0], ZModulePositions[0]);

    VacuumPipe_section3->AddNode(tube, 0, tube_transform);

    top->AddNode(VacuumPipe_section3, 0);
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

TGeoVolume *CreateTube(TString module_name) {

    Double_t length0 = 3.4; //cm

    Double_t length1 = 125.0; //cm
    Double_t length2 = 100.0; //cm
    Double_t length3 = 96.0; //cm
    Double_t length4 = 150.0; //cm
    Double_t adapter_length1 = 1.5;
    Double_t adapter_length2 = 5.9;
    Double_t adapter_length_all = adapter_length1 + adapter_length2;

    Double_t innerRadius1 = 12.0*0.5;
    Double_t outerRadius1 = 12.4*0.5;
    Double_t flangeRadius1 = 13.8*0.5;

    Double_t innerRadius2 = 15.0*0.5;
    Double_t outerRadius2 = 15.4*0.5;
    Double_t flangeRadius2 = 16.9*0.5;

    Double_t flangeThickness = 0.5;

    //shapes
    TGeoShape *vacTube0S = new TGeoTube(TString("vacTube0S")+=TString("_") + module_name, innerRadius1, outerRadius1, length0*0.5);
    TGeoShape *vacTube1S = new TGeoTube(TString("vacTube1S")+=TString("_") + module_name, innerRadius1, outerRadius1, length1*0.5);
    TGeoShape *vacTube2S = new TGeoTube(TString("vacTube2S")+=TString("_") + module_name, innerRadius1, outerRadius1, length2*0.5);
    TGeoShape *vacTube3S = new TGeoTube(TString("vacTube3S")+=TString("_") + module_name, innerRadius1, outerRadius1, length3*0.5);
    TGeoShape *vacTube4S = new TGeoTube(TString("vacTube4S")+=TString("_") + module_name, innerRadius2, outerRadius2, length4*0.5);

    TGeoShape *vacTube0VacuumS = new TGeoTube(TString("vacTube0VacuumS")+=TString("_") + module_name, 0.0, innerRadius1, length0*0.5);
    TGeoShape *vacTube1VacuumS = new TGeoTube(TString("vacTube1VacuumS")+=TString("_") + module_name, 0.0, innerRadius1, length1*0.5);
    TGeoShape *vacTube2VacuumS = new TGeoTube(TString("vacTube2VacuumS")+=TString("_") + module_name, 0.0, innerRadius1, length2*0.5);
    TGeoShape *vacTube3VacuumS = new TGeoTube(TString("vacTube3VacuumS")+=TString("_") + module_name, 0.0, innerRadius1, length3*0.5);
    TGeoShape *vacTube4VacuumS = new TGeoTube(TString("vacTube4VacuumS")+=TString("_") + module_name, 0.0, innerRadius2, length4*0.5);

    TGeoShape *vacAdapterPart1S = new TGeoTube(TString("vacAdapterPart1S")+=TString("_") + module_name, innerRadius1, outerRadius1, adapter_length1*0.5);
    TGeoShape *vacAdapterPart2S = new TGeoTube(TString("vacAdapterPart2S")+=TString("_") + module_name, innerRadius2, outerRadius2, adapter_length2*0.5);
    TGeoShape *vacAdapterPart3S = new TGeoTube(TString("vacAdapterPart3S")+=TString("_") + module_name, innerRadius1, innerRadius2, flangeThickness*0.5);

    TGeoShape *vacAdapterPart1VacuumS = new TGeoTube(TString("vacAdapterPart1VacuumS")+=TString("_") + module_name, 0.0, innerRadius1, adapter_length1*0.5);
    TGeoShape *vacAdapterPart2VacuumS = new TGeoTube(TString("vacAdapterPart2VacuumS")+=TString("_") + module_name, 0.0, innerRadius2, adapter_length2*0.5);
    TGeoShape *vacAdapterPart3VacuumS = new TGeoTube(TString("vacAdapterPart3VacuumS")+=TString("_") + module_name, 0.0, innerRadius2, flangeThickness*0.5);

    TGeoShape *vacTubeFlangeS = new TGeoTube(TString("vacTubeFlangeS")+=TString("_") + module_name, outerRadius1, flangeRadius1, flangeThickness*0.5);
    TGeoShape *vacTubeBigFlangeS = new TGeoTube(TString("vacTubeBigFlangeS")+=TString("_") + module_name, outerRadius2, flangeRadius2, flangeThickness*0.5);

    TGeoCombiTrans *vacAdapterPart1_transf = new TGeoCombiTrans();
        vacAdapterPart1_transf->SetName(TString("vacAdapterPart1_transf")+=TString("_") + module_name);
        vacAdapterPart1_transf->SetTranslation(0.0, 0.0, +adapter_length1*0.5);
        vacAdapterPart1_transf->RegisterYourself();

    TGeoCombiTrans *vacAdapterPart2_transf = new TGeoCombiTrans();
        vacAdapterPart2_transf->SetName(TString("vacAdapterPart2_transf")+=TString("_") + module_name);
        vacAdapterPart2_transf->SetTranslation(0.0, 0.0, +adapter_length1 + adapter_length2*0.5);
        vacAdapterPart2_transf->RegisterYourself();

    TGeoCombiTrans *vacAdapterPart3_transf = new TGeoCombiTrans();
        vacAdapterPart3_transf->SetName(TString("vacAdapterPart3_transf")+=TString("_") + module_name);
        vacAdapterPart3_transf->SetTranslation(0.0, 0.0, +adapter_length1 + flangeThickness*0.5);
        vacAdapterPart3_transf->RegisterYourself();

    TGeoCompositeShape *vacAdapterS = new TGeoCompositeShape();
    vacAdapterS->SetName(TString("vacAdapterS")+=TString("_") + module_name);
    {
        TString expression = "vacAdapterPart1S"; expression += TString("_") + module_name;
        expression += ":vacAdapterPart1_transf"; expression += TString("_") + module_name;
        expression += "+vacAdapterPart2S"; expression += TString("_") + module_name;
        expression += ":vacAdapterPart2_transf"; expression += TString("_") + module_name;
        expression += "+vacAdapterPart3S"; expression += TString("_") + module_name;
        expression += ":vacAdapterPart3_transf"; expression += TString("_") + module_name;
        vacAdapterS->MakeNode(expression);
        vacAdapterS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCompositeShape *vacAdapterVacuumS = new TGeoCompositeShape();
    vacAdapterVacuumS->SetName(TString("vacAdapterVacuumS")+=TString("_") + module_name);
    {
        TString expression = "vacAdapterPart1VacuumS"; expression += TString("_") + module_name;
        expression += ":vacAdapterPart1_transf"; expression += TString("_") + module_name;
        expression += "+vacAdapterPart2VacuumS"; expression += TString("_") + module_name;
        expression += ":vacAdapterPart2_transf"; expression += TString("_") + module_name;
        expression += "+vacAdapterPart3VacuumS"; expression += TString("_") + module_name;
        expression += ":vacAdapterPart3_transf"; expression += TString("_") + module_name;
        vacAdapterVacuumS->MakeNode(expression);
        vacAdapterVacuumS->ComputeBBox(); //need to compute a bounding box
    }

    //volumes
    TGeoVolumeAssembly *vacTubeBoxV = new TGeoVolumeAssembly(module_name);
    TGeoVolume *vacTube0V = new TGeoVolume(TString("vacTube0V")+=TString("_") + module_name, vacTube0S);
    TGeoVolume *vacTube1V = new TGeoVolume(TString("vacTube1V")+=TString("_") + module_name, vacTube1S);
    TGeoVolume *vacTube2V = new TGeoVolume(TString("vacTube2V")+=TString("_") + module_name, vacTube2S);
    TGeoVolume *vacTube3V = new TGeoVolume(TString("vacTube3V")+=TString("_") + module_name, vacTube3S);
    TGeoVolume *vacTube4V = new TGeoVolume(TString("vacTube4V")+=TString("_") + module_name, vacTube4S);
    TGeoVolume *vacAdapterV = new TGeoVolume(TString("vacAdapterV")+=TString("_") + module_name, vacAdapterS);

    TGeoVolume *vacTube0VacuumV = new TGeoVolume(TString("vacTube0VacuumV")+=TString("_") + module_name, vacTube0VacuumS);
    TGeoVolume *vacTube1VacuumV = new TGeoVolume(TString("vacTube1VacuumV")+=TString("_") + module_name, vacTube1VacuumS);
    TGeoVolume *vacTube2VacuumV = new TGeoVolume(TString("vacTube2VacuumV")+=TString("_") + module_name, vacTube2VacuumS);
    TGeoVolume *vacTube3VacuumV = new TGeoVolume(TString("vacTube3VacuumV")+=TString("_") + module_name, vacTube3VacuumS);
    TGeoVolume *vacTube4VacuumV = new TGeoVolume(TString("vacTube4VacuumV")+=TString("_") + module_name, vacTube4VacuumS);
    TGeoVolume *vacAdapterVacuumV = new TGeoVolume(TString("vacAdapterVacuumV")+=TString("_") + module_name, vacAdapterVacuumS);

    TGeoVolume *vacTubeFlangeV = new TGeoVolume(TString("vacTubeFlangeV")+=TString("_") + module_name, vacTubeFlangeS);
    TGeoVolume *vacTubeBigFlangeV = new TGeoVolume(TString("vacTubeBigFlangeV")+=TString("_") + module_name, vacTubeBigFlangeS);

    //volume medium
    TGeoMedium *vacTubeV_medium = pMedAluminium;
    if(vacTubeV_medium) {
        vacTube0V->SetMedium(vacTubeV_medium);
        vacTube1V->SetMedium(vacTubeV_medium);
        vacTube2V->SetMedium(vacTubeV_medium);
        vacTube3V->SetMedium(vacTubeV_medium);
        vacTube4V->SetMedium(vacTubeV_medium);
        vacAdapterV->SetMedium(vacTubeV_medium);
        vacTubeFlangeV->SetMedium(vacTubeV_medium);
        vacTubeBigFlangeV->SetMedium(vacTubeV_medium);
    }
    else Fatal("Main", "Invalid medium for vacTubeV!");

    TGeoMedium *vacTubeVacuumV_medium = pMedVacuum;
    if(vacTubeVacuumV_medium) {
        vacTube0VacuumV->SetMedium(vacTubeVacuumV_medium);
        vacTube1VacuumV->SetMedium(vacTubeVacuumV_medium);
        vacTube2VacuumV->SetMedium(vacTubeVacuumV_medium);
        vacTube3VacuumV->SetMedium(vacTubeVacuumV_medium);
        vacTube4VacuumV->SetMedium(vacTubeVacuumV_medium);
        vacAdapterVacuumV->SetMedium(vacTubeVacuumV_medium);
    }
    else Fatal("Main", "Invalid medium for vacTubeVacuumV!");

    //volume visual property (transparency)
    vacTube0V->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTube1V->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTube1V->SetTransparency(0);
    vacTube2V->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTube3V->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTube4V->SetLineColor(TColor::GetColor("#ffcccc"));
    vacAdapterV->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTubeFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));
    vacTubeBigFlangeV->SetLineColor(TColor::GetColor("#ffcccc"));

    //part positions in the vacuum tube box
    TGeoCombiTrans *vacTube0V_transf[1];
    vacTube0V_transf[0] = new TGeoCombiTrans();
    vacTube0V_transf[0]->SetTranslation(0.0, 0.0, -length0*0.5);

    TGeoCombiTrans *vacTube1V_transf[1];
    vacTube1V_transf[0] = new TGeoCombiTrans();
    vacTube1V_transf[0]->SetTranslation(0.0, 0.0, +length1*0.5);

    TGeoCombiTrans *vacTube2V_transf[1];
    vacTube2V_transf[0] = new TGeoCombiTrans();
    vacTube2V_transf[0]->SetTranslation(0.0, 0.0, +length1 + length2*0.5);

    TGeoCombiTrans *vacTube3V_transf[1];
    vacTube3V_transf[0] = new TGeoCombiTrans();
    vacTube3V_transf[0]->SetTranslation(0.0, 0.0, +length1 + length2 + length3*0.5);

    TGeoCombiTrans *vacAdapterV_transf[1];
    vacAdapterV_transf[0] = new TGeoCombiTrans();
    vacAdapterV_transf[0]->SetTranslation(0.0, 0.0, +length1 + length2 + length3);

    TGeoCombiTrans *vacTube4V_transf[1];
    vacTube4V_transf[0] = new TGeoCombiTrans();
    vacTube4V_transf[0]->SetTranslation(0.0, 0.0, +length1 + length2 + length3 + adapter_length_all + length4*0.5);

    TGeoCombiTrans *vacTube1FlangeV_transf[2];
    vacTube1FlangeV_transf[0] = new TGeoCombiTrans();
    vacTube1FlangeV_transf[0]->SetTranslation(0.0, 0.0, +flangeThickness*0.5);
    vacTube1FlangeV_transf[1] = new TGeoCombiTrans();
    vacTube1FlangeV_transf[1]->SetTranslation(0.0, 0.0, +length1 - flangeThickness*0.5);

    TGeoCombiTrans *vacTube2FlangeV_transf[2];
    vacTube2FlangeV_transf[0] = new TGeoCombiTrans();
    vacTube2FlangeV_transf[0]->SetTranslation(0.0, 0.0, +length1 + flangeThickness*0.5);
    vacTube2FlangeV_transf[1] = new TGeoCombiTrans();
    vacTube2FlangeV_transf[1]->SetTranslation(0.0, 0.0, +length1 + length2 - flangeThickness*0.5);

    TGeoCombiTrans *vacTube3FlangeV_transf[2];
    vacTube3FlangeV_transf[0] = new TGeoCombiTrans();
    vacTube3FlangeV_transf[0]->SetTranslation(0.0, 0.0, +length1 + length2 + flangeThickness*0.5);
    vacTube3FlangeV_transf[1] = new TGeoCombiTrans();
    vacTube3FlangeV_transf[1]->SetTranslation(0.0, 0.0, +length1 + length2 + length3 - flangeThickness*0.5);

    TGeoCombiTrans *vacAdapterFlangeV_transf[1];
    vacAdapterFlangeV_transf[0] = new TGeoCombiTrans();
    vacAdapterFlangeV_transf[0]->SetTranslation(0.0, 0.0, +length1 + length2 + length3 + flangeThickness*0.5);

    TGeoCombiTrans *vacAdapterBigFlangeV_transf[1];
    vacAdapterBigFlangeV_transf[0] = new TGeoCombiTrans();
    vacAdapterBigFlangeV_transf[0]->SetTranslation(0.0, 0.0, +length1 + length2 + length3 + adapter_length1+adapter_length2 - flangeThickness*0.5);

    TGeoCombiTrans *vacTube4FlangeV_transf[2];
    vacTube4FlangeV_transf[0] = new TGeoCombiTrans();
    vacTube4FlangeV_transf[0]->SetTranslation(0.0, 0.0, +length1 + length2 + + length3 + adapter_length_all + flangeThickness*0.5);
    vacTube4FlangeV_transf[1] = new TGeoCombiTrans();
    vacTube4FlangeV_transf[1]->SetTranslation(0.0, 0.0, +length1 + length2 + length3 + adapter_length_all + length4 - flangeThickness*0.5);

    vacTubeBoxV->AddNode(vacTube0V, 0, vacTube0V_transf[0]);
    vacTubeBoxV->AddNode(vacTube1V, 0, vacTube1V_transf[0]);
    vacTubeBoxV->AddNode(vacTubeFlangeV, 0, vacTube1FlangeV_transf[0]);
    vacTubeBoxV->AddNode(vacTubeFlangeV, 1, vacTube1FlangeV_transf[1]);
    vacTubeBoxV->AddNode(vacTube2V, 0, vacTube2V_transf[0]);
    vacTubeBoxV->AddNode(vacTubeFlangeV, 0, vacTube2FlangeV_transf[0]);
    vacTubeBoxV->AddNode(vacTubeFlangeV, 1, vacTube2FlangeV_transf[1]);
    vacTubeBoxV->AddNode(vacTube3V, 0, vacTube3V_transf[0]);
    vacTubeBoxV->AddNode(vacTubeFlangeV, 0, vacTube3FlangeV_transf[0]);
    vacTubeBoxV->AddNode(vacTubeFlangeV, 1, vacTube3FlangeV_transf[1]);
    vacTubeBoxV->AddNode(vacAdapterV, 0, vacAdapterV_transf[0]);
    vacTubeBoxV->AddNode(vacTubeFlangeV, 0, vacAdapterFlangeV_transf[0]);
    vacTubeBoxV->AddNode(vacTubeBigFlangeV, 0, vacAdapterBigFlangeV_transf[0]);

    vacTubeBoxV->AddNode(vacTube0VacuumV, 0, vacTube0V_transf[0]);
    vacTubeBoxV->AddNode(vacTube1VacuumV, 0, vacTube1V_transf[0]);
    vacTubeBoxV->AddNode(vacTube2VacuumV, 0, vacTube2V_transf[0]);
    vacTubeBoxV->AddNode(vacTube3VacuumV, 0, vacTube3V_transf[0]);
    vacTubeBoxV->AddNode(vacAdapterVacuumV, 0, vacAdapterV_transf[0]);

    //this section is not for RUN-8
    //vacTubeBoxV->AddNode(vacTube4V, 0, vacTube4V_transf[0]);
    //vacTubeBoxV->AddNode(vacTubeBigFlangeV, 0, vacTube4FlangeV_transf[0]);
    //vacTubeBoxV->AddNode(vacTubeBigFlangeV, 1, vacTube4FlangeV_transf[1]);

    return vacTubeBoxV;
}
//------------------------------------------------------------------------------
