/*
 * Description:
 * Macro for calculating parameters of VSP detector and writing them
 * into the XML-file needed for simulation and reconstruction procedures.
 *
 * !!! IMPORTANT !!!
 * The obtained XML-file SHOULD BE MOVED into the 'parameters/vsp/XMLConfigs' directory
 *
 * Author: D. Baranov
 * Created: 08.07.2024
 * Updated: ***
 *
 */

#include "Rtypes.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)

#include "TString.h"

#include <fstream>

using namespace TMath;
using namespace std;

//Configuration of VSP detector ------------------------------------------------

//Set Parameters of VSP detector -----------------------------------------------
const Int_t NStations = 1;      //stations in the detector
const Int_t NMaxModules = 6;    //max. number of modules in a station

const Int_t NModulesInStations[NStations] = {6};

//(X-Y-Z)Positions of stations (sensitive volumes)
const Double_t XStationPositions[NStations] = { 0.0 };
const Double_t YStationPositions[NStations] = { 0.0 };
const Double_t ZStationPositions[NStations] = { 6.5 }; //the closest side to the target (frame or sensor)

//shift of a module is its geometric center position
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {+6.0/*0*/,
     +0.0/*1*/,
     -6.0/*2*/,

     +6.0/*3*/,
     +0.0/*4*/,
     -6.0/*5*/},
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {+3.0/*0*/,
     +5.8/*1*/,
     +3.0/*2*/,

     -3.0/*3*/,
     -5.8/*4*/,
     -3.0/*5*/},
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {2.48/*0*/,
     2.3/*1*/,
     2.48/*2*/,

     0.06/*3*/,
     0.24/*4*/,
     0.06/*5*/},
};

enum ModuleFrameType {SIDE, MIDDLE};

const ModuleFrameType ModulesFrameTypes[NStations][NMaxModules] = {
    {SIDE, MIDDLE, SIDE,    SIDE, MIDDLE, SIDE},
};

//Rotations of modules around y-axis by 180 deg.
const Bool_t ModulesYRotations[NStations][NMaxModules] = {
    {false/*0*/, false/*1*/, false/*2*/,     false/*3*/, false/*4*/, false/*5*/},
};

//Rotations of modules around x-axis by 180 deg.
const Bool_t ModulesXRotations[NStations][NMaxModules] = {
    {false/*0*/, false/*1*/, false/*2*/,    true/*3*/, true/*4*/, true/*5*/},
 };
//------------------------------------------------------------------------------

//Configuration of modules -----------------------------------------------------

//Module parameters ------------------------------------------------------------
Double_t XModuleSize = 6.2;
Double_t YModuleSize = 6.2;
Double_t ZModuleSize = 0.03; //300 mkm

Double_t XLeftBorderFrame = 0.1304;
Double_t XRightBorderFrame = 0.1304;
Double_t YTopBorderFrame = 0.1304;
Double_t YBottomBorderFrame = 0.1304;

//Sensor zones -----------------------------------------------------------------
Double_t XSensorZoneSize = XModuleSize - (XLeftBorderFrame + XRightBorderFrame);
Double_t YSensorZoneSize = YModuleSize - (YTopBorderFrame + YBottomBorderFrame);
Double_t ZSensorZoneSize = ZModuleSize;
//------------------------------------------------------------------------------

//The next layers have the indices:
// 0 - inclined strips of main zone (front side),
// 1 - inclined strips of duplicate corner zone (front side)
// 2 - straight strips (back side)

Double_t XOrigLayer[3] = {
    0.0 + XSensorZoneSize*0.5,
    0.0 + XSensorZoneSize*0.5,
    0.0 + XSensorZoneSize*0.5
};

Double_t YOrigLayer[3] = {
    0.0 - YSensorZoneSize*0.5,
    0.0 - YSensorZoneSize*0.5,
    0.0 - YSensorZoneSize*0.5 };

Double_t XSizeLayer[3] = {
    XSensorZoneSize,
    XSensorZoneSize,
    XSensorZoneSize
};

Double_t YSizeLayer[3] = {
    YSensorZoneSize,
    YSensorZoneSize,
    YSensorZoneSize
};

Double_t XLeftBorderLayer[3] = {
    XOrigLayer[0],
    XOrigLayer[1] - XSizeLayer[1],
    XOrigLayer[2]
};

Double_t YLeftBorderLayer[3] = {
    YOrigLayer[0] + YSizeLayer[0],
    YOrigLayer[1] + YSizeLayer[1],
    YOrigLayer[2] + YSizeLayer[2]
};

Double_t XRightBorderLayer[3] = {
    XOrigLayer[0] - XSizeLayer[0],
    XOrigLayer[1] - XSizeLayer[1],
    XOrigLayer[2] - XSizeLayer[2]
};

Double_t YRightBorderLayer[3] = {
    YOrigLayer[0] + YSizeLayer[0],
    YOrigLayer[1],
    YOrigLayer[2]
};

enum StripNumberingDirection {LeftToRight, RightToLeft};
StripNumberingDirection StripDirection[3] = { LeftToRight, LeftToRight, LeftToRight };
//Double_t PitchValue[3] = { 0.0058, 0.0058, 0.0058 };
Double_t PitchValue[3] = { 0.00575038, 0.00575038, 0.0058 };
Double_t StripAngle[3] = { +7.5, +7.5, +0.0 }; // plus - clockwise, minus - anticlockwise
Int_t FirstStripNumberLayer[3] = { 0, 0, 0 };


//Dead zones (if you need) -----------------------------------------------------

Double_t XShift_DuplicateCornerLayer = 0.78191; // X-width of duplicate corner layer for inclined strips (134 inclined strips of 62x62 mm^2 module)
//------------------------------------------------------------------------------


//Function declarations --------------------------------------------------------
void PrintInfo();
void CalculateParameters(TString xml_file_name);
//------------------------------------------------------------------------------


void VSPConfigCalculate_Run9() {

    PrintInfo();
    CalculateParameters("VSP_Run9.xml");

    cout << "It's the end" << "\n";
    //gApplication->Terminate();
}

//------------------------------------------------------------------------------

void PrintInfo() {

    cout << "----------------------------------------------------------------\n";
    cout << "Parameters of the VSP module 62x62 mm^2:\n";
    cout << "\n";
    cout << " Layer[0] (inclined strips):\n";
    cout << "    orig(x:y) = ( " << XOrigLayer[0] << " : " << YOrigLayer[0] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeLayer[0] << " : " << YSizeLayer[0] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderLayer[0] << " : " << YLeftBorderLayer[0] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderLayer[0] << " : " << YRightBorderLayer[0] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[0] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[0] << "\n";
    cout << "    angle = " << StripAngle[0] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberLayer[0] << "\n";
    cout << "\n";

    cout << " Layer[1] (inclined strips):\n";
    cout << "    orig(x:y) = ( " << XOrigLayer[1] << " : " << YOrigLayer[1] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeLayer[1] << " : " << YSizeLayer[1] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderLayer[1] << " : " << YLeftBorderLayer[1] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderLayer[1] << " : " << YRightBorderLayer[1] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[1] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[1] << "\n";
    cout << "    angle = " << StripAngle[1] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberLayer[1] << "\n";
    cout << "\n";

    cout << " Layer[2] (straight strips):\n";
    cout << "    orig(x:y) = ( " << XOrigLayer[2] << " : " << YOrigLayer[2] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeLayer[2] << " : " << YSizeLayer[2] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderLayer[2] << " : " << YLeftBorderLayer[2] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderLayer[2] << " : " << YRightBorderLayer[2] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[2] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[2] << "\n";
    cout << "    angle = " << StripAngle[2] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberLayer[2] << "\n";
    cout << "\n";
    cout << "----------------------------------------------------------------\n";
}
//------------------------------------------------------------------------------

void CalculateParameters(TString xml_file_name) {

    ofstream SiFileOut(xml_file_name.Data(), ios::out);

    SiFileOut << "<StationSet>\n";
    SiFileOut << "\n";

    for(Int_t istation = 0; istation < NStations; ++istation) {

        cout << "istation = " << istation << ": -----------------------------\n";

        SiFileOut << "<!-- Station " << istation << " -->\n";

        SiFileOut << "\t<Station";
        SiFileOut << " xPosition=\"" << XStationPositions[istation] << "\"";
        SiFileOut << " yPosition=\"" << YStationPositions[istation] << "\"";
        SiFileOut << " zPosition=\"" << ZStationPositions[istation] << "\"";
        SiFileOut << " rotAngleDeg=\"" << "0.0" << "\"";
        SiFileOut << " rotCenterX=\"" << "0.0" << "\"";
        SiFileOut << " rotCenterY=\"" << "0.0" << "\"";
        SiFileOut << ">\n";
        SiFileOut << "\n";

        for(Int_t imodule = 0; imodule < NModulesInStations[istation]; ++imodule) {

            cout << "  imodule = " << imodule << "\n";

            SiFileOut << "\t\t<!-- module " << imodule << " -->\n";

            SiFileOut << "\t\t<Module";
            SiFileOut << " xShift=\"" << XModuleShifts[istation][imodule] << "\"";
            SiFileOut << " yShift=\"" << YModuleShifts[istation][imodule] << "\"";
            //SiFileOut << " xShift=\"" << 0.0 << "\"";
            //SiFileOut << " yShift=\"" << 0.0 << "\"";
            SiFileOut << " zShift=\"" << ZModuleShifts[istation][imodule] << "\"";
            SiFileOut << ">\n";

            //------------------------------------------------------------------

            //Notice!!!: here:
            // 0 - inclined strips of main zone (front side),
            // 1 - inclined strips of duplicate corner zone (front side)
            // 2 - straight strips (back side)

            Double_t xOrig_Layer[3];
            Double_t yOrig_Layer[3];

            Double_t xSize_Layer[3];
            Double_t ySize_Layer[3];

            Double_t xLBorder_Layer[3];
            Double_t yLBorder_Layer[3];
            Double_t xRBorder_Layer[3];
            Double_t yRBorder_Layer[3];

            StripNumberingDirection stripDirection[3];
            Double_t pitchValue[3] = {PitchValue[0], PitchValue[1], PitchValue[2]};
            Double_t stripAngle[3];
            Int_t firstStripNumber_Layer[3];

            Double_t XDeadZoneInMainInclinedLayer[3]; //3-point dead zone
            Double_t YDeadZoneInMainInclinedLayer[3]; //3-point dead zone

            Double_t XDeadZoneInCornerInclinedLayer[4]; //4-point dead zone
            Double_t YDeadZoneInCornerInclinedLayer[4]; //4-point dead zone


            if( ModulesYRotations[istation][imodule] == false && ModulesXRotations[istation][imodule] == false ) {

                xOrig_Layer[0] = XOrigLayer[0];
                yOrig_Layer[0] = YOrigLayer[0];

                xOrig_Layer[1] = XOrigLayer[1];
                yOrig_Layer[1] = YOrigLayer[1];

                xOrig_Layer[2] = XOrigLayer[2];
                yOrig_Layer[2] = YOrigLayer[2];

                xSize_Layer[0] = XSizeLayer[0];
                ySize_Layer[0] = YSizeLayer[0];

                xSize_Layer[1] = XSizeLayer[1];
                ySize_Layer[1] = YSizeLayer[1];

                xSize_Layer[2] = XSizeLayer[2];
                ySize_Layer[2] = YSizeLayer[2];

                xLBorder_Layer[0] = XLeftBorderLayer[0];
                xLBorder_Layer[1] = XLeftBorderLayer[1];
                xLBorder_Layer[2] = XLeftBorderLayer[2];

                yLBorder_Layer[0] = YLeftBorderLayer[0];
                yLBorder_Layer[1] = YLeftBorderLayer[1];
                yLBorder_Layer[2] = YLeftBorderLayer[2];

                xRBorder_Layer[0] = XRightBorderLayer[0];
                xRBorder_Layer[1] = XRightBorderLayer[1];
                xRBorder_Layer[2] = XRightBorderLayer[2];

                yRBorder_Layer[0] = YRightBorderLayer[0];
                yRBorder_Layer[1] = YRightBorderLayer[1];
                yRBorder_Layer[2] = YRightBorderLayer[2];

                if(StripDirection[0] == LeftToRight) stripDirection[0] = LeftToRight; else stripDirection[0] = RightToLeft;
                if(StripDirection[1] == LeftToRight) stripDirection[1] = LeftToRight; else stripDirection[1] = RightToLeft;
                if(StripDirection[2] == LeftToRight) stripDirection[2] = LeftToRight; else stripDirection[2] = RightToLeft;

                stripAngle[0] = StripAngle[0];
                stripAngle[1] = StripAngle[1];
                stripAngle[2] = StripAngle[2];

                firstStripNumber_Layer[0] = FirstStripNumberLayer[0];
                firstStripNumber_Layer[1] = FirstStripNumberLayer[1];
                firstStripNumber_Layer[2] = FirstStripNumberLayer[2];

                //Dead zone (in a main inclined strip layer) -------------------
                XDeadZoneInMainInclinedLayer[0] = XOrigLayer[0] - XSizeLayer[0];
                YDeadZoneInMainInclinedLayer[0] = YOrigLayer[0];

                XDeadZoneInMainInclinedLayer[1] = XOrigLayer[0] - XSizeLayer[0] + TMath::Tan(StripAngle[0]*TMath::DegToRad())*YSizeLayer[0];
                YDeadZoneInMainInclinedLayer[1] = YOrigLayer[0];

                XDeadZoneInMainInclinedLayer[2] = XOrigLayer[0] - XSizeLayer[0];
                YDeadZoneInMainInclinedLayer[2] = YOrigLayer[0] + YSizeLayer[0];
                //--------------------------------------------------------------

                //Dead zone (in a corner inclined strip layer) -----------------
                XDeadZoneInCornerInclinedLayer[0] = XOrigLayer[0];
                YDeadZoneInCornerInclinedLayer[0] = YOrigLayer[0];

                XDeadZoneInCornerInclinedLayer[1] = XOrigLayer[0];
                YDeadZoneInCornerInclinedLayer[1] = YOrigLayer[0] + YSizeLayer[1];

                XDeadZoneInCornerInclinedLayer[2] = XOrigLayer[0] - XSizeLayer[0];
                YDeadZoneInCornerInclinedLayer[2] = YOrigLayer[0] + YSizeLayer[0];

                XDeadZoneInCornerInclinedLayer[3] = XOrigLayer[0] - XSizeLayer[0] + TMath::Tan(StripAngle[0]*TMath::DegToRad())*YSizeLayer[1];
                YDeadZoneInCornerInclinedLayer[3] = YOrigLayer[0];
                //--------------------------------------------------------------
            }

            if( ModulesYRotations[istation][imodule] == true && ModulesXRotations[istation][imodule] == false ) {

                xOrig_Layer[0] = XOrigLayer[0];
                yOrig_Layer[0] = YOrigLayer[0];

                xOrig_Layer[1] = XOrigLayer[1];
                yOrig_Layer[1] = YOrigLayer[1];

                xOrig_Layer[2] = XOrigLayer[2];
                yOrig_Layer[2] = YOrigLayer[2];

                xSize_Layer[0] = XSizeLayer[0];
                ySize_Layer[0] = YSizeLayer[0];

                xSize_Layer[1] = XSizeLayer[1];
                ySize_Layer[1] = YSizeLayer[1];

                xSize_Layer[2] = XSizeLayer[2];
                ySize_Layer[2] = YSizeLayer[2];

                xLBorder_Layer[0] = XLeftBorderLayer[0];
                xLBorder_Layer[1] = XLeftBorderLayer[1] + XSizeLayer[1];
                xLBorder_Layer[2] = XLeftBorderLayer[2];

                yLBorder_Layer[0] = YLeftBorderLayer[0];
                yLBorder_Layer[1] = YLeftBorderLayer[1] - YSizeLayer[1];
                yLBorder_Layer[2] = YLeftBorderLayer[1] - YSizeLayer[2];

                xRBorder_Layer[0] = XRightBorderLayer[0];
                xRBorder_Layer[1] = XRightBorderLayer[1] + XSizeLayer[1];
                xRBorder_Layer[2] = XRightBorderLayer[2];

                yRBorder_Layer[0] = YRightBorderLayer[0];
                yRBorder_Layer[1] = YRightBorderLayer[1] + YSizeLayer[1];
                yRBorder_Layer[2] = YRightBorderLayer[2] + YSizeLayer[2];

                if(StripDirection[0] == LeftToRight) stripDirection[0] = RightToLeft; else stripDirection[0] = LeftToRight;
                if(StripDirection[1] == LeftToRight) stripDirection[1] = RightToLeft; else stripDirection[1] = LeftToRight;
                if(StripDirection[2] == LeftToRight) stripDirection[2] = RightToLeft; else stripDirection[2] = LeftToRight;

                stripAngle[0] = -StripAngle[0];
                stripAngle[1] = -StripAngle[1];
                stripAngle[2] = -StripAngle[2];

                firstStripNumber_Layer[0] = FirstStripNumberLayer[0];
                firstStripNumber_Layer[1] = FirstStripNumberLayer[1];
                firstStripNumber_Layer[2] = FirstStripNumberLayer[2];

                //Dead zone (in a main inclined strip layer) -------------------
                XDeadZoneInMainInclinedLayer[0] = XOrigLayer[0];
                YDeadZoneInMainInclinedLayer[0] = YOrigLayer[0];

                XDeadZoneInMainInclinedLayer[1] = XOrigLayer[0] - TMath::Tan(StripAngle[0]*TMath::DegToRad())*YSizeLayer[0];
                YDeadZoneInMainInclinedLayer[1] = YOrigLayer[0];

                XDeadZoneInMainInclinedLayer[2] = XOrigLayer[0];
                YDeadZoneInMainInclinedLayer[2] = YOrigLayer[0] + YSizeLayer[0];
                //--------------------------------------------------------------

                //Dead zone (in a corner inclined strip layer) -----------------
                XDeadZoneInCornerInclinedLayer[0] = XOrigLayer[0] - XSizeLayer[1];
                YDeadZoneInCornerInclinedLayer[0] = YOrigLayer[0];

                XDeadZoneInCornerInclinedLayer[1] = XOrigLayer[0] - XSizeLayer[1];
                YDeadZoneInCornerInclinedLayer[1] = YOrigLayer[0] + YSizeLayer[1];

                XDeadZoneInCornerInclinedLayer[2] = XOrigLayer[0];
                YDeadZoneInCornerInclinedLayer[2] = YOrigLayer[0] + YSizeLayer[1];

                XDeadZoneInCornerInclinedLayer[3] = XOrigLayer[0] - TMath::Tan(StripAngle[0]*TMath::DegToRad())*YSizeLayer[1];
                YDeadZoneInCornerInclinedLayer[3] = YOrigLayer[0];
                //--------------------------------------------------------------
            }

            if( ModulesYRotations[istation][imodule] == false && ModulesXRotations[istation][imodule] == true ) {

                xOrig_Layer[0] = XOrigLayer[0];
                yOrig_Layer[0] = YOrigLayer[0];

                xOrig_Layer[1] = XOrigLayer[1];
                yOrig_Layer[1] = YOrigLayer[1];

                xOrig_Layer[2] = XOrigLayer[2];
                yOrig_Layer[2] = YOrigLayer[2];

                xSize_Layer[0] = XSizeLayer[0];
                ySize_Layer[0] = YSizeLayer[0];

                xSize_Layer[1] = XSizeLayer[1];
                ySize_Layer[1] = YSizeLayer[1];

                xSize_Layer[2] = XSizeLayer[2];
                ySize_Layer[2] = YSizeLayer[2];

                xLBorder_Layer[0] = XLeftBorderLayer[0];
                xLBorder_Layer[1] = XLeftBorderLayer[1];
                xLBorder_Layer[2] = XLeftBorderLayer[2];

                yLBorder_Layer[0] = YLeftBorderLayer[0] - YSizeLayer[0];
                yLBorder_Layer[1] = YLeftBorderLayer[1] - YSizeLayer[1];
                yLBorder_Layer[2] = YLeftBorderLayer[2] - YSizeLayer[2];

                xRBorder_Layer[0] = XRightBorderLayer[0];
                xRBorder_Layer[1] = XRightBorderLayer[1];
                xRBorder_Layer[2] = XRightBorderLayer[2];

                yRBorder_Layer[0] = YRightBorderLayer[0] - YSizeLayer[0];
                yRBorder_Layer[1] = YRightBorderLayer[1] + YSizeLayer[1];
                yRBorder_Layer[2] = YRightBorderLayer[2] + YSizeLayer[2];

                if(StripDirection[0] == LeftToRight) stripDirection[0] = LeftToRight; else stripDirection[0] = RightToLeft;
                if(StripDirection[1] == LeftToRight) stripDirection[1] = LeftToRight; else stripDirection[1] = RightToLeft;
                if(StripDirection[2] == LeftToRight) stripDirection[2] = LeftToRight; else stripDirection[2] = RightToLeft;

                stripAngle[0] = -StripAngle[0];
                stripAngle[1] = -StripAngle[1];
                stripAngle[2] = -StripAngle[2];

                firstStripNumber_Layer[0] = FirstStripNumberLayer[0];
                firstStripNumber_Layer[1] = FirstStripNumberLayer[1];
                firstStripNumber_Layer[2] = FirstStripNumberLayer[2];

                //Dead zone (in a main inclined strip layer) -------------------
                XDeadZoneInMainInclinedLayer[0] = XOrigLayer[0] - XSizeLayer[0];
                YDeadZoneInMainInclinedLayer[0] = YOrigLayer[0] + YSizeLayer[0];

                XDeadZoneInMainInclinedLayer[1] = XOrigLayer[0] - XSizeLayer[0] + TMath::Tan(StripAngle[0]*TMath::DegToRad())*YSizeLayer[0];
                YDeadZoneInMainInclinedLayer[1] = YOrigLayer[0] + YSizeLayer[0];

                XDeadZoneInMainInclinedLayer[2] = XOrigLayer[0] - XSizeLayer[0];
                YDeadZoneInMainInclinedLayer[2] = YOrigLayer[0];
                //--------------------------------------------------------------

                //Dead zone (in a corner inclined strip layer) -----------------
                XDeadZoneInCornerInclinedLayer[0] = XOrigLayer[0];
                YDeadZoneInCornerInclinedLayer[0] = YOrigLayer[0] + YSizeLayer[1];

                XDeadZoneInCornerInclinedLayer[1] = XOrigLayer[0];
                YDeadZoneInCornerInclinedLayer[1] = YOrigLayer[0];

                XDeadZoneInCornerInclinedLayer[2] = XOrigLayer[0] - XSizeLayer[1];
                YDeadZoneInCornerInclinedLayer[2] = YOrigLayer[0];

                XDeadZoneInCornerInclinedLayer[3] = XOrigLayer[0] - XSizeLayer[1] + TMath::Tan(StripAngle[0]*TMath::DegToRad())*YSizeLayer[1];
                YDeadZoneInCornerInclinedLayer[3] = YOrigLayer[0] + YSizeLayer[1];
                //--------------------------------------------------------------
            }

            if( ModulesYRotations[istation][imodule] == true && ModulesXRotations[istation][imodule] == true ) {

                xOrig_Layer[0] = XOrigLayer[0];
                yOrig_Layer[0] = YOrigLayer[0];

                xOrig_Layer[1] = XOrigLayer[1];
                yOrig_Layer[1] = YOrigLayer[1];

                xOrig_Layer[2] = XOrigLayer[2];
                yOrig_Layer[2] = YOrigLayer[2];

                xSize_Layer[0] = XSizeLayer[0];
                ySize_Layer[0] = YSizeLayer[0];

                xSize_Layer[1] = XSizeLayer[1];
                ySize_Layer[1] = YSizeLayer[1];

                xSize_Layer[2] = XSizeLayer[2];
                ySize_Layer[2] = YSizeLayer[2];

                xLBorder_Layer[0] = XLeftBorderLayer[0];
                xLBorder_Layer[1] = XLeftBorderLayer[1] + XSizeLayer[1];
                xLBorder_Layer[2] = XLeftBorderLayer[2];

                yLBorder_Layer[0] = YLeftBorderLayer[0] - YSizeLayer[0];
                yLBorder_Layer[1] = YLeftBorderLayer[1];
                yLBorder_Layer[2] = YLeftBorderLayer[2];

                xRBorder_Layer[0] = XRightBorderLayer[0];
                xRBorder_Layer[1] = XRightBorderLayer[1] + XSizeLayer[1];
                xRBorder_Layer[2] = XRightBorderLayer[2];

                yRBorder_Layer[0] = YRightBorderLayer[0] - YSizeLayer[0];
                yRBorder_Layer[1] = YRightBorderLayer[1];
                yRBorder_Layer[2] = YRightBorderLayer[2];

                if(StripDirection[0] == LeftToRight) stripDirection[0] = RightToLeft; else stripDirection[0] = LeftToRight;
                if(StripDirection[1] == LeftToRight) stripDirection[1] = RightToLeft; else stripDirection[1] = LeftToRight;
                if(StripDirection[2] == LeftToRight) stripDirection[2] = RightToLeft; else stripDirection[2] = LeftToRight;

                stripAngle[0] = StripAngle[0];
                stripAngle[1] = StripAngle[1];
                stripAngle[2] = StripAngle[2];

                firstStripNumber_Layer[0] = FirstStripNumberLayer[0];
                firstStripNumber_Layer[1] = FirstStripNumberLayer[1];
                firstStripNumber_Layer[2] = FirstStripNumberLayer[2];

                //Dead zone (in a main inclined strip layer) -------------------
                XDeadZoneInMainInclinedLayer[0] = XOrigLayer[0];
                YDeadZoneInMainInclinedLayer[0] = YOrigLayer[0] + YSizeLayer[0];

                XDeadZoneInMainInclinedLayer[1] = XOrigLayer[0] - TMath::Tan(StripAngle[0]*TMath::DegToRad())*YSizeLayer[0];
                YDeadZoneInMainInclinedLayer[1] = YOrigLayer[0] + YSizeLayer[0];

                XDeadZoneInMainInclinedLayer[2] = XOrigLayer[0];
                YDeadZoneInMainInclinedLayer[2] = YOrigLayer[0];
                //--------------------------------------------------------------

                //Dead zone (in a corner inclined strip layer) -----------------
                XDeadZoneInCornerInclinedLayer[0] = XOrigLayer[0] - XSizeLayer[1];
                YDeadZoneInCornerInclinedLayer[0] = YOrigLayer[0] + YSizeLayer[1];

                XDeadZoneInCornerInclinedLayer[1] = XOrigLayer[0] - XSizeLayer[1];
                YDeadZoneInCornerInclinedLayer[1] = YOrigLayer[0];

                XDeadZoneInCornerInclinedLayer[2] = XOrigLayer[0];
                YDeadZoneInCornerInclinedLayer[2] = YOrigLayer[0];

                XDeadZoneInCornerInclinedLayer[3] = XOrigLayer[0] - TMath::Tan(StripAngle[0]*TMath::DegToRad())*YSizeLayer[1];
                YDeadZoneInCornerInclinedLayer[3] = YOrigLayer[0] + YSizeLayer[1];
                //--------------------------------------------------------------
            }

            cout << "   ---\n";
            cout << "   Orig_Layer[0](x:y) = ( " << xOrig_Layer[0] << " : " << yOrig_Layer[0] << " )\n";
            cout << "   Orig_Layer[1](x:y) = ( " << xOrig_Layer[1] << " : " << yOrig_Layer[1] << " )\n";
            cout << "   Orig_Layer[2](x:y) = ( " << xOrig_Layer[2] << " : " << yOrig_Layer[2] << " )\n";
            cout << "\n\n";

            cout << "   ---\n";
            cout << "   Size_Layer[0](dx:dy) = ( " << xSize_Layer[0] << " : " << ySize_Layer[0] << " )\n";
            cout << "   Size_Layer[1](dx:dy) = ( " << xSize_Layer[1] << " : " << ySize_Layer[1] << " )\n";
            cout << "   Size_Layer[2](dx:dy) = ( " << xSize_Layer[2] << " : " << ySize_Layer[2] << " )\n";
            cout << "\n\n";

            cout << "   ---\n";
            cout << "   LBorder_Layer[0](x:y) = ( " << xLBorder_Layer[0] << " : " << yLBorder_Layer[0] << " )\n";
            cout << "   RBorder_Layer[0](x:y) = ( " << xRBorder_Layer[0] << " : " << yRBorder_Layer[0] << " )\n";
            cout << "\n";

            cout << "   LBorder_Layer[1](x:y) = ( " << xLBorder_Layer[1] << " : " << yLBorder_Layer[1] << " )\n";
            cout << "   RBorder_Layer[1](x:y) = ( " << xRBorder_Layer[1] << " : " << yRBorder_Layer[1] << " )\n";
            cout << "\n";

            cout << "   LBorder_Layer[2](x:y) = ( " << xLBorder_Layer[2] << " : " << yLBorder_Layer[2] << " )\n";
            cout << "   RBorder_Layer[2](x:y) = ( " << xRBorder_Layer[2] << " : " << yRBorder_Layer[2] << " )\n";
            cout << "\n\n";

            cout << "   ---\n";
            cout << "   stripDirection[0] = ";
            if(stripDirection[0] == LeftToRight) cout << "LeftToRight\n"; else cout << "RightToLeft\n";
            cout << "   stripDirection[1] = ";
            if(stripDirection[1] == LeftToRight) cout << "LeftToRight\n"; else cout << "RightToLeft\n";
            cout << "   stripDirection[2] = ";
            if(stripDirection[2] == LeftToRight) cout << "LeftToRight\n"; else cout << "RightToLeft\n";
            cout << "\n";

            cout << "   ---\n";
            cout << "   pitchValue[0] = " << pitchValue[0] << "\n";
            cout << "   pitchValue[1] = " << pitchValue[1] << "\n";
            cout << "   pitchValue[2] = " << pitchValue[2] << "\n";
            cout << "\n";

            cout << "---\n";
            cout << "   stripAngle[0] = " << stripAngle[0] << "\n";
            cout << "   stripAngle[1] = " << stripAngle[1] << "\n";
            cout << "   stripAngle[2] = " << stripAngle[2] << "\n";
            cout << "\n";

            cout << "---\n";
            cout << "   firstStripNumber_Layer[0] = " << firstStripNumber_Layer[0] << "\n";
            cout << "   firstStripNumber_Layer[1] = " << firstStripNumber_Layer[1] << "\n";
            cout << "   firstStripNumber_Layer[2] = " << firstStripNumber_Layer[2] << "\n";
            cout << "\n";


            //file out for straight strips = back side -------------------------
            SiFileOut << "\t\t\t<Layer";
            SiFileOut << " zoneID=\"" << "0" << "\"";
            SiFileOut << " type=\"" << "lower" << "\"";
            SiFileOut << " firstStripNumber=\"" << firstStripNumber_Layer[2] << "\"";
            SiFileOut << " stripAngle=\"" << stripAngle[2] << "\"";
            SiFileOut << " pitch=\"" << pitchValue[2] << "\"";
            SiFileOut << " xorig=\"" << xOrig_Layer[2] << "\"";
            SiFileOut << " yorig=\"" << yOrig_Layer[2] << "\"";
            SiFileOut << " xsize=\"" << xSize_Layer[2] << "\"";
            SiFileOut << " ysize=\"" << ySize_Layer[2] << "\"";
            SiFileOut << " stripDirection=\"";
                if(stripDirection[2] == LeftToRight) SiFileOut << "LeftToRight\""; else SiFileOut << "RightToLeft\"";
            SiFileOut << " lxborder=\"" << xLBorder_Layer[2] << "\"";
            SiFileOut << " lyborder=\"" << yLBorder_Layer[2] << "\"";
            SiFileOut << " rxborder=\"" << xRBorder_Layer[2] << "\"";
            SiFileOut << " ryborder=\"" << yRBorder_Layer[2] << "\"";
            SiFileOut << ">";

            SiFileOut << "</Layer>\n";
            //------------------------------------------------------------------

            //file out for inclined strips = front side (main zone) ------------
            SiFileOut << "\t\t\t<Layer";
            SiFileOut << " zoneID=\"" << "1" << "\"";
            SiFileOut << " type=\"" << "upper" << "\"";
            SiFileOut << " firstStripNumber=\"" << firstStripNumber_Layer[0] << "\"";
            SiFileOut << " stripAngle=\"" << stripAngle[0] << "\"";
            SiFileOut << " pitch=\"" << pitchValue[0] << "\"";
            SiFileOut << " xorig=\"" << xOrig_Layer[0] << "\"";
            SiFileOut << " yorig=\"" << yOrig_Layer[0] << "\"";
            SiFileOut << " xsize=\"" << xSize_Layer[0] << "\"";
            SiFileOut << " ysize=\"" << ySize_Layer[0] << "\"";
            SiFileOut << " stripDirection=\"";
                if(stripDirection[0] == LeftToRight) SiFileOut << "LeftToRight\""; else SiFileOut << "RightToLeft\"";
            SiFileOut << " lxborder=\"" << xLBorder_Layer[0] << "\"";
            SiFileOut << " lyborder=\"" << yLBorder_Layer[0] << "\"";
            SiFileOut << " rxborder=\"" << xRBorder_Layer[0] << "\"";
            SiFileOut << " ryborder=\"" << yRBorder_Layer[0] << "\"";
            SiFileOut << ">";

            SiFileOut << "\n";
            SiFileOut << "\t\t\t\t<DeadZone>\n";
            for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                SiFileOut << "\t\t\t\t\t<DeadPoint";
                SiFileOut << " x=\"" << XDeadZoneInMainInclinedLayer[ipoint] << "\"";
                SiFileOut << " y=\"" << YDeadZoneInMainInclinedLayer[ipoint] << "\"";
                SiFileOut << " />\n";
            }
            SiFileOut << "\t\t\t\t</DeadZone>\n\t\t\t";

            SiFileOut << "</Layer>\n";
            //------------------------------------------------------------------

            //file out for inclined strips = front side (corner zone1) ---------
            SiFileOut << "\t\t\t<Layer";
            SiFileOut << " zoneID=\"" << "1" << "\"";
            SiFileOut << " type=\"" << "upper" << "\"";
            SiFileOut << " firstStripNumber=\"" << firstStripNumber_Layer[1] << "\"";
            SiFileOut << " stripAngle=\"" << stripAngle[1] << "\"";
            SiFileOut << " pitch=\"" << pitchValue[1] << "\"";
            SiFileOut << " xorig=\"" << xOrig_Layer[1] << "\"";
            SiFileOut << " yorig=\"" << yOrig_Layer[1] << "\"";
            SiFileOut << " xsize=\"" << xSize_Layer[1] << "\"";
            SiFileOut << " ysize=\"" << ySize_Layer[1] << "\"";
            SiFileOut << " stripDirection=\"";
                if(stripDirection[1] == LeftToRight) SiFileOut << "LeftToRight\""; else SiFileOut << "RightToLeft\"";
            SiFileOut << " lxborder=\"" << xLBorder_Layer[1] << "\"";
            SiFileOut << " lyborder=\"" << yLBorder_Layer[1] << "\"";
            SiFileOut << " rxborder=\"" << xRBorder_Layer[1] << "\"";
            SiFileOut << " ryborder=\"" << yRBorder_Layer[1] << "\"";
            SiFileOut << ">";

            SiFileOut << "\n";
            SiFileOut << "\t\t\t\t<DeadZone>\n";
            for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                SiFileOut << "\t\t\t\t\t<DeadPoint";
                SiFileOut << " x=\"" << XDeadZoneInCornerInclinedLayer[ipoint] << "\"";
                SiFileOut << " y=\"" << YDeadZoneInCornerInclinedLayer[ipoint] << "\"";
                SiFileOut << " />\n";
            }
            SiFileOut << "\t\t\t\t</DeadZone>\n\t\t\t";

            SiFileOut << "</Layer>\n";
            //------------------------------------------------------------------

            SiFileOut << "\t\t</Module>\n";
            SiFileOut << "\n";
        }

        SiFileOut << "\t</Station>\n";
        SiFileOut << "\n";

        cout << "------------------------------------------------------------\n";
    }

    SiFileOut << "</StationSet>\n";

    SiFileOut.close();
}