/*
 * Description:
 * Macro for calculating parameters of GEM detector and writing them
 * into the XML-file needed for simulation and reconstruction procedures.
 * The obtained XML-file should be moved into the 'parameters/gem/XMLConfigs' directory
 *
 * GEM configuration: Run8 (8 stations)
 *
 * Created: 12.05.2020
 * 02.03.2021 (Updated): radius of hole in active zone (4.0 -> 6.0 cm)
 * 09.03.2021 (Updated): the form of a 'hole' dead zone changed (square -> 12 sided polygon)
 * 25.05.2021 (Updated): Z-distance from the target to the first sens. vol. has been increased
 * 08.12.2022 (Updated): Positions were corrected in accordance with the measurement scheme
 * 24.01.2023 (updated): half-plane shifts have been added according to the measurement scheme
 * Author: Baranov D.
 */

#include "Rtypes.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)

#include "TString.h"

#include <fstream>

using namespace TMath;
using namespace std;

//Set Parameters of GEMs -------------------------------------------------------
const Int_t NStations = 8;      //stations in the detector
const Int_t NMaxModules = 4;    //max. number of modules in a station

const Int_t NModulesInStations[NStations] = {4, 4, 4, 4, 4, 4, 4, 1};

//(X-Y-Z)Positions of stations (sensitive volumes)
const Double_t XStationPositions[NStations] = { +1.476/*0*/, +2.614/*1*/, +3.342/*2*/, +4.674/*3*/, +6.303/*4*/, +8.140/*5*/, +9.871/*6*/, +50.494/*7*/}; //center of hole for a half-plane nearest to the target
const Double_t YStationPositions[NStations] = { -0.148/*0*/, -0.107/*1*/, -0.064/*2*/, -0.120/*3*/, -0.071/*4*/, +0.123/*5*/, -0.050/*6*/, -0.186/*7*/}; //center of hole for a half-plane nearest to the target
const Double_t ZStationPositions[NStations] = { +61.229/*0*/, +92.2505/*1*/, +123.502/*2*/, +154.679/*3*/, +185.9235/*4*/, +217.194/*5*/, +248.344/*6*/, +792.532/*7*/}; //half-plane nearest to the target

const Double_t xShift[NStations] = {-0.004/*0*/, 0.0/*1*/, +0.102/*2*/, +0.05/*3*/, +0.067/*4*/, +0.098/*5*/, +0.045/*6*/, 0.0/*7*/};
const Double_t yShift[NStations] = {+0.125/*0*/, 0.0/*1*/, +0.131/*2*/, +0.046/*3*/, +0.136/*4*/, -0.197/*5*/, +0.122/*6*/, 0.0/*7*/};
const Double_t zShift[NStations] = {+3.998/*0*/, +4.101/*1*/, +4.0785/*2*/, +4.055/*3*/, +4.0985/*4*/, +3.903/*5*/, +4.1075/*6*/, 0.0/*7*/};

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {163.2*0.25 + xShift[0], -163.2*0.25 + xShift[0],  //st. 0 (upper h. plane)
     163.2*0.25, -163.2*0.25},                         //st. 0 (lower h. plane)

    {163.2*0.25, -163.2*0.25,                          //st. 1 (upper h. plane)
     163.2*0.25 + xShift[1], -163.2*0.25 + xShift[1]}, //st. 1 (lower h. plane)

    {163.2*0.25 + xShift[2], -163.2*0.25 + xShift[2],  //st. 2 (upper h. plane)
     163.2*0.25, -163.2*0.25},                         //st. 2 (lower h. plane)

    {163.2*0.25, -163.2*0.25,                          //st. 3 (upper h. plane)
     163.2*0.25 + xShift[3], -163.2*0.25 + xShift[3]}, //st. 3 (lower h. plane)

    {163.2*0.25 + xShift[4], -163.2*0.25 + xShift[4],  //st. 4 (upper h. plane)
     163.2*0.25, -163.2*0.25},                         //st. 4 (lower h. plane)

    {163.2*0.25, -163.2*0.25,                          //st. 5 (upper h. plane)
     163.2*0.25 + xShift[5], -163.2*0.25 + xShift[5]}, //st. 5 (lower h. plane)

    {163.2*0.25 + xShift[6], -163.2*0.25 + xShift[6],  //st. 6 (upper h. plane)
    163.2*0.25, -163.2*0.25},                          //st. 6 (lower h. plane)

    {0.0}                                              //st. 7
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {+22.5 + yShift[0], +22.5 + yShift[0],      //st. 0 (upper h. plane)
     -19.5, -19.5},                             //st. 0 (lower h. plane)

    {+22.5, +22.5,                              //st. 1 (upper h. plane)
     -19.5 + yShift[1], -19.5 + yShift[1]},     //st. 1 (lower h. plane)

    {+22.5 + yShift[2], +22.5 + yShift[2],      //st. 2 (upper h. plane)
     -19.5, -19.5},                             //st. 2 (lower h. plane)

    {+22.5, +22.5,                              //st. 3 (upper h. plane)
     -19.5 + yShift[3], -19.5 + yShift[3]},     //st. 3 (lower h. plane)

    {+22.5 + yShift[4], +22.5 + yShift[4],      //st. 4 (upper h. plane)
     -19.5, -19.5},                             //st. 4 (lower h. plane)

    {+22.5, +22.5,                              //st. 5 (upper h. plane)
     -19.5 + yShift[5], -19.5 + yShift[5]},     //st. 5 (lower h. plane)

    {+22.5 + yShift[6], +22.5 + yShift[6],      //st. 6 (upper h. plane)
     -19.5, -19.5},                             //st. 6 (lower h. plane)

    {0.0}                                       //st. 7
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {zShift[0], zShift[0], 0.0, 0.0}, //st. 0
    {0.0, 0.0, zShift[1], zShift[1]}, //st. 1
    {zShift[2], zShift[2], 0.0, 0.0}, //st. 2
    {0.0, 0.0, zShift[3], zShift[3]}, //st. 3
    {zShift[4], zShift[4], 0.0, 0.0}, //st. 4
    {0.0, 0.0, zShift[5], zShift[5]}, //st. 5
    {zShift[6], zShift[6], 0.0, 0.0}, //st. 6
    {0.0}                   //st. 7
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

//rotations of stations around y-axis by 180 deg.
const Bool_t YStationRotations[NStations] = {true/*0*/, false/*1*/, true/*2*/, false/*3*/, true/*4*/, false/*5*/, true/*6*/, false/*7*/};

//rotations of stations around x-axis by 180 deg.
const Bool_t XStationRotations[NStations] = {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, false/*7*/};

//drift direction in each module ("true" is "forward", "false" is "backward")
const Bool_t DriftDirectionInModules[NStations][NMaxModules] = {
    {false, false, true, true},  //st. 0
    {true, true, false, false},  //st. 1
    {false, false, true, true},  //st. 2
    {true, true, false, false},  //st. 3
    {false, false, true, true},  //st. 4
    {true, true, false, false},  //st. 5
    {false, false, true, true},  //st. 6
    {true}                       //st. 7
};

//thicknesses of gaps inside a GEM chamber (for all modules)
const Double_t Gap_Drift_Thickness = 0.3; //cm
const Double_t Gap_FirstTransfer_Thickness = 0.25; //cm
const Double_t Gap_SecondTransfer_Thickness = 0.2; //cm
const Double_t Gap_Induction_Thickness = 0.15; //cm


//upper half-plane (type UHP with sizes of 163.2 x 45.0 cm) --------------------
const Double_t StripPitch_UHP = 0.08; //cm

const Double_t XMainZoneLayerSize_UHP =  81.6; //cm
const Double_t YMainZoneLayerSize_UHP = 45.0; //cm

const Double_t XHotZoneLayerSize_StraightStrips_UHP = 40.0; //cm
const Double_t YHotZoneLayerSize_StraightStrips_UHP  = 15.0; //cm

const Double_t XHotZoneLayerSize_InclinedStrips_UHP[2] = {37.99, 42.01}; // [0] - min, [1] - max
const Double_t YHotZoneLayerSize_InclinedStrips_UHP = 15.0; //deg. (plus - clockwise, minus - anticlockwise)
//------------------------------------------------------------------------------

//lower half-plane (type LHP with sizes of 163.2 x 39.0 cm) --------------------
const Double_t StripPitch_LHP = 0.08; //cm

const Double_t XMainZoneLayerSize_LHP =  81.6; //cm
const Double_t YMainZoneLayerSize_LHP = 39.0; //cm

const Double_t XHotZoneLayerSize_StraightStrips_LHP = 40.0; //cm
const Double_t YHotZoneLayerSize_StraightStrips_LHP  = 15.0; //cm

const Double_t XHotZoneLayerSize_InclinedStrips_LHP[2] = {37.99, 42.01}; // [0] - min, [1] - max
const Double_t YHotZoneLayerSize_InclinedStrips_LHP = 15.0; //deg. (plus - clockwise, minus - anticlockwise)
//------------------------------------------------------------------------------

//small GEM (type SMALL with sizes of 10.0 x 10.0 cm) --------------------------
const Double_t StripPitch_SMALL = 0.04; //cm

const Double_t XMainZoneLayerSize_SMALL =  10.0; //cm
const Double_t YMainZoneLayerSize_SMALL = 10.0; //cm
//------------------------------------------------------------------------------

const Int_t NMaxLayersInModule = 4;
const Int_t NLayersInModule[NStations] = {4, 4, 4, 4, 4, 4, 4, 2/*smallGEM*/};

const Int_t ZoneTypeOfLayers[NStations][NMaxLayersInModule] = { // 0 - main, 1 - hot zone
    {0, 0, 1, 1}, //st. 0
    {0, 0, 1, 1}, //st. 1
    {0, 0, 1, 1}, //st. 2
    {0, 0, 1, 1}, //st. 3
    {0, 0, 1, 1}, //st. 4
    {0, 0, 1, 1}, //st. 5
    {0, 0, 1, 1}, //st. 6
    {0, 0}        //st. 7 (small GEM)
};

const char* StripTypeOfLayes[NStations][NMaxLayersInModule] = { // "lower" is straight strips, "upper " is inclined strips
    {"lower", "upper", "lower", "upper"}, //st. 0
    {"lower", "upper", "lower", "upper"}, //st. 1
    {"lower", "upper", "lower", "upper"}, //st. 2
    {"lower", "upper", "lower", "upper"}, //st. 3
    {"lower", "upper", "lower", "upper"}, //st. 4
    {"lower", "upper", "lower", "upper"}, //st. 5
    {"lower", "upper", "lower", "upper"}, //st. 6
    {"lower", "upper"}                    //st. 7 (small GEM)
};

//stereo angle for all layers with inclined strips in each module of a basic station
const Double_t StripStereoAngleInModules[NStations][NMaxModules] = { // deg. (plus - clockwise, minus - anticlockwise)
    {+15.0, +15.0, -15.0, -15.0}, //st. 0
    {+15.0, +15.0, -15.0, -15.0}, //st. 1
    {+15.0, +15.0, -15.0, -15.0}, //st. 2
    {+15.0, +15.0, -15.0, -15.0}, //st. 3
    {+15.0, +15.0, -15.0, -15.0}, //st. 4
    {+15.0, +15.0, -15.0, -15.0}, //st. 5
    {+15.0, +15.0, -15.0, -15.0}, //st. 6
    {+90.0}                       //st. 7 (small GEM)
};

enum StripNumberingDirection {LeftToRight, RightToLeft};
StripNumberingDirection StripDirection[2] = { LeftToRight, RightToLeft };

Double_t HoleRadius = 4.0+2.0;

//dead zones (upper half-plane) ------------------------------------------------
Double_t XDeadZone_Rect_UHP[4] = {
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_StraightStrips_UHP,
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_StraightStrips_UHP,
    0.0 - XMainZoneLayerSize_UHP*0.5,
    0.0 - XMainZoneLayerSize_UHP*0.5
};
Double_t YDeadZone_Rect_UHP[4] = {
    0.0 - YMainZoneLayerSize_UHP*0.5,
    0.0 - YMainZoneLayerSize_UHP*0.5 + YHotZoneLayerSize_StraightStrips_UHP,
    0.0 - YMainZoneLayerSize_UHP*0.5 + YHotZoneLayerSize_StraightStrips_UHP,
    0.0 - YMainZoneLayerSize_UHP*0.5
};

Double_t XDeadZone_TrapLeft_UHP[4] = {
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[1],
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[0],
    0.0 - XMainZoneLayerSize_UHP*0.5,
    0.0 - XMainZoneLayerSize_UHP*0.5
};
Double_t YDeadZone_TrapLeft_UHP[4] = {
    0.0 - YMainZoneLayerSize_UHP*0.5,
    0.0 - YMainZoneLayerSize_UHP*0.5 + YHotZoneLayerSize_InclinedStrips_UHP,
    0.0 - YMainZoneLayerSize_UHP*0.5 + YHotZoneLayerSize_InclinedStrips_UHP,
    0.0 - YMainZoneLayerSize_UHP*0.5
};

Double_t XDeadZone_TrapRight_UHP[4] = {
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[0],
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[1],
    0.0 - XMainZoneLayerSize_UHP*0.5,
    0.0 - XMainZoneLayerSize_UHP*0.5
};
Double_t YDeadZone_TrapRight_UHP[4] = {
    0.0 - YMainZoneLayerSize_UHP*0.5,
    0.0 - YMainZoneLayerSize_UHP*0.5 + YHotZoneLayerSize_InclinedStrips_UHP,
    0.0 - YMainZoneLayerSize_UHP*0.5 + YHotZoneLayerSize_InclinedStrips_UHP,
    0.0 - YMainZoneLayerSize_UHP*0.5
};

Double_t XDeadZone_TrianLeft_UHP[3] = {
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[1],
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[1],
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[0]
};
Double_t YDeadZone_TrianLeft_UHP[3] = {
    0.0 - YMainZoneLayerSize_UHP*0.5,
    0.0 - YMainZoneLayerSize_UHP*0.5 + YHotZoneLayerSize_InclinedStrips_UHP,
    0.0 - YMainZoneLayerSize_UHP*0.5 + YHotZoneLayerSize_InclinedStrips_UHP
};

Double_t XDeadZone_TrianRight_UHP[3] = {
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[1],
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[1],
    0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[0]
};
Double_t YDeadZone_TrianRight_UHP[3] = {
    0.0 - YMainZoneLayerSize_UHP*0.5,
    0.0 - YMainZoneLayerSize_UHP*0.5 + YHotZoneLayerSize_InclinedStrips_UHP,
    0.0 - YMainZoneLayerSize_UHP*0.5
};

const UInt_t NPoints_DeadZone_Hole_UHP = 5;
Double_t XDeadZone_Hole_UHP[NPoints_DeadZone_Hole_UHP] = {
    0.0 - XMainZoneLayerSize_UHP*0.5 + HoleRadius,
    0.0 - XMainZoneLayerSize_UHP*0.5 + HoleRadius*TMath::Cos(30.0*TMath::DegToRad()),
    0.0 - XMainZoneLayerSize_UHP*0.5 + HoleRadius*TMath::Cos(60.0*TMath::DegToRad()),
    0.0 - XMainZoneLayerSize_UHP*0.5,
    0.0 - XMainZoneLayerSize_UHP*0.5
};
Double_t YDeadZone_Hole_UHP[NPoints_DeadZone_Hole_UHP] = {
    0.0 - YMainZoneLayerSize_UHP*0.5,
    0.0 - YMainZoneLayerSize_UHP*0.5 + TMath::Sqrt(HoleRadius*HoleRadius - (HoleRadius*TMath::Cos(30.0*TMath::DegToRad()))*(HoleRadius*TMath::Cos(30.0*TMath::DegToRad()))),
    0.0 - YMainZoneLayerSize_UHP*0.5 + TMath::Sqrt(HoleRadius*HoleRadius - (HoleRadius*TMath::Cos(60.0*TMath::DegToRad()))*(HoleRadius*TMath::Cos(60.0*TMath::DegToRad()))),
    0.0 - YMainZoneLayerSize_UHP*0.5 + HoleRadius,
    0.0 - YMainZoneLayerSize_UHP*0.5
};

//------------------------------------------------------------------------------

//dead zones (lower half-plane) ------------------------------------------------
Double_t XDeadZone_Rect_LHP[4] = {
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_StraightStrips_LHP,
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_StraightStrips_LHP,
    0.0 - XMainZoneLayerSize_LHP*0.5,
    0.0 - XMainZoneLayerSize_LHP*0.5
};
Double_t YDeadZone_Rect_LHP[4] = {
    0.0 - YMainZoneLayerSize_LHP*0.5,
    0.0 - YMainZoneLayerSize_LHP*0.5 + YHotZoneLayerSize_StraightStrips_LHP,
    0.0 - YMainZoneLayerSize_LHP*0.5 + YHotZoneLayerSize_StraightStrips_LHP,
    0.0 - YMainZoneLayerSize_LHP*0.5
};

Double_t XDeadZone_TrapLeft_LHP[4] = {
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[1],
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[0],
    0.0 - XMainZoneLayerSize_LHP*0.5,
    0.0 - XMainZoneLayerSize_LHP*0.5
};
Double_t YDeadZone_TrapLeft_LHP[4] = {
    0.0 - YMainZoneLayerSize_LHP*0.5,
    0.0 - YMainZoneLayerSize_LHP*0.5 + YHotZoneLayerSize_InclinedStrips_LHP,
    0.0 - YMainZoneLayerSize_LHP*0.5 + YHotZoneLayerSize_InclinedStrips_LHP,
    0.0 - YMainZoneLayerSize_LHP*0.5
};

Double_t XDeadZone_TrapRight_LHP[4] = {
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[0],
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[1],
    0.0 - XMainZoneLayerSize_LHP*0.5,
    0.0 - XMainZoneLayerSize_LHP*0.5
};
Double_t YDeadZone_TrapRight_LHP[4] = {
    0.0 - YMainZoneLayerSize_LHP*0.5,
    0.0 - YMainZoneLayerSize_LHP*0.5 + YHotZoneLayerSize_InclinedStrips_LHP,
    0.0 - YMainZoneLayerSize_LHP*0.5 + YHotZoneLayerSize_InclinedStrips_LHP,
    0.0 - YMainZoneLayerSize_LHP*0.5
};

Double_t XDeadZone_TrianLeft_LHP[3] = {
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[1],
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[1],
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[0]
};
Double_t YDeadZone_TrianLeft_LHP[3] = {
    0.0 - YMainZoneLayerSize_LHP*0.5,
    0.0 - YMainZoneLayerSize_LHP*0.5 + YHotZoneLayerSize_InclinedStrips_LHP,
    0.0 - YMainZoneLayerSize_LHP*0.5 + YHotZoneLayerSize_InclinedStrips_LHP
};

Double_t XDeadZone_TrianRight_LHP[3] = {
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[1],
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[1],
    0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[0]
};
Double_t YDeadZone_TrianRight_LHP[3] = {
    0.0 - YMainZoneLayerSize_LHP*0.5,
    0.0 - YMainZoneLayerSize_LHP*0.5 + YHotZoneLayerSize_InclinedStrips_LHP,
    0.0 - YMainZoneLayerSize_LHP*0.5
};

const UInt_t NPoints_DeadZone_Hole_LHP = 5;
Double_t XDeadZone_Hole_LHP[NPoints_DeadZone_Hole_LHP] = {
    0.0 - XMainZoneLayerSize_LHP*0.5 + HoleRadius,
    0.0 - XMainZoneLayerSize_LHP*0.5 + HoleRadius*TMath::Cos(30.0*TMath::DegToRad()),
    0.0 - XMainZoneLayerSize_LHP*0.5 + HoleRadius*TMath::Cos(60.0*TMath::DegToRad()),
    0.0 - XMainZoneLayerSize_LHP*0.5,
    0.0 - XMainZoneLayerSize_LHP*0.5
};
Double_t YDeadZone_Hole_LHP[NPoints_DeadZone_Hole_LHP] = {
    0.0 - YMainZoneLayerSize_LHP*0.5,
    0.0 - YMainZoneLayerSize_LHP*0.5 + TMath::Sqrt(HoleRadius*HoleRadius - (HoleRadius*TMath::Cos(30.0*TMath::DegToRad()))*(HoleRadius*TMath::Cos(30.0*TMath::DegToRad()))),
    0.0 - YMainZoneLayerSize_LHP*0.5 + TMath::Sqrt(HoleRadius*HoleRadius - (HoleRadius*TMath::Cos(60.0*TMath::DegToRad()))*(HoleRadius*TMath::Cos(60.0*TMath::DegToRad()))),
    0.0 - YMainZoneLayerSize_LHP*0.5 + HoleRadius,
    0.0 - YMainZoneLayerSize_LHP*0.5
};


//------------------------------------------------------------------------------

//Function declarations
//void PrintInfo();
void CalculateParameters(TString config_name, TString author, TString date);
//------------------------------------------------------------------------------


void GemConfigCalculate_Run8_8stations() {

    TString ConfigName = "GemRun8";
    TString AuthorName = "Baranov D.";
    TString DateCreated = "18.01.2021";

    //PrintInfo();
    CalculateParameters(ConfigName, AuthorName, DateCreated);

    cout << "It's the end" << "\n";
    //gApplication->Terminate();
}
//------------------------------------------------------------------------------

void CalculateParameters(TString config_name, TString author, TString date) {

    TString FileName = config_name + TString(".xml");

    cout << "\nCreating XML configuration file...\n";
    cout << "  Configuration name: " << config_name << "\n";
    cout << "  Author: " << author << "\n";
    cout << "  Date created: " << date << "\n";
    cout << "  02.03.2021 (Updated): radius of hole in active zone (4.0 -> 6.0 cm)\n";
    cout << "  09.03.2021 (Updated): the form of a 'hole' dead zone changed (square -> 12 sided polygon) \n";
    cout << "  25.05.2021 (Updated): Z-distance from the target to the first sens. vol. has been increased \n";
    cout << "  08.12.2022 (Updated): Positions were corrected in accordance with the measurement scheme \n";
    cout << "  12.12.2022 (updated): small GEM added \n";
    cout << "  24.01.2023 (updated): half-plane shifts have been added according to the measurement scheme \n";
    cout << "  File path: " << FileName << "\n";
    cout << "\n";

    //WARNING(!!!): the file will be written in the current directory
    ofstream FileOut(FileName.Data(), ios::out);

    FileOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    //reference frame usage comment
    FileOut << "<!--\
        \r\tConfig: " << config_name <<  "\
        \r\tAuthor: " << author <<  "\
        \r\tDate: " << date <<  "\
        \r\t02.03.2021 (Updated): radius of hole in active zone (4.0 -> 6.0 cm)\
        \r\t09.03.2021 (Updated): the form of a 'hole' dead zone changed (square -> 12 sided polygon)\
        \r\t25.05.2021 (Updated): Z-distance from the target to the first sens. vol. has been increased\
        \r\t08.12.2022 (Updated): Positions were corrected in accordance with the measurement scheme\
        \r\t12.12.2022 (updated): small GEM added\
        \r\t24.01.2023 (updated): half-plane shifts have been added according to the measurement scheme\n\
        \r\tA right-handed coordinates are used.\
        \r\tIf you look at the GEMs from the target:\
        \r\t\tx-axis is directed to the left,\
        \r\t\ty-axis - to the top,\
        \r\t\tz-axis - to the GEMs.\
        \r-->\n\n";

    FileOut << "<StationSet>\n";
    FileOut << "\n";

    for(Int_t istation = 0; istation < NStations; ++istation) {

        //cout << "istation = " << istation << ": -----------------------------\n";

        FileOut << "<!-- Station " << istation << " -->\n"; //comment

        FileOut << "\t<Station";
        FileOut << " xPosition=\"" << XStationPositions[istation] << "\"";
        FileOut << " yPosition=\"" << YStationPositions[istation] << "\"";
        FileOut << " zPosition=\"" << ZStationPositions[istation] << "\"";
        if(istation != 7) FileOut << " beamHole=\"" << HoleRadius << "\""; // small GEM
        FileOut << ">\n";
        FileOut << "\n";

        for(Int_t imodule = 0; imodule < NModulesInStations[istation]; ++imodule) {

            //cout << "  imodule = " << imodule << "\n";

            FileOut << "\t\t<!-- module " << imodule << " -->\n"; //comment

            FileOut << "\t\t<Module";
            FileOut << " xShift=\"" << XModuleShifts[istation][imodule] << "\"";
            FileOut << " yShift=\"" << YModuleShifts[istation][imodule] << "\"";
            FileOut << " zShift=\"" << ZModuleShifts[istation][imodule] << "\"";
            FileOut << " driftDirection=\"" << (DriftDirectionInModules[istation][imodule] ? "forward" : "backward") << "\"";
            FileOut << " gap_drift_thick=\"" << Gap_Drift_Thickness << "\"";
            FileOut << " gap_ftransf_thick=\"" << Gap_FirstTransfer_Thickness << "\"";
            FileOut << " gap_stransf_thick=\"" << Gap_SecondTransfer_Thickness << "\"";
            FileOut << " gap_induct_thick=\"" << Gap_Induction_Thickness << "\"";
            FileOut << ">\n";

            for(Int_t ilayer = 0; ilayer < NLayersInModule[istation]; ++ilayer) {


                //cout << "    ilayer = "  <<  ilayer  << "\n";

                FileOut << "\t\t\t<Layer";

                FileOut << " zoneNumber=\"" << ZoneTypeOfLayers[istation][ilayer] << "\"";
                FileOut << " type=\"" << StripTypeOfLayes[istation][ilayer] << "\"";

                //define a strip angle for the current layer
                Double_t layer_angle;
                if(TString(StripTypeOfLayes[istation][ilayer]) == TString("lower")) {
                    layer_angle = 0.0;
                }
                else {
                    layer_angle = StripStereoAngleInModules[istation][imodule];
                    if(YStationRotations[istation]^XStationRotations[istation]) layer_angle *= -1;

                }

                FileOut << " stripAngle=\"" << layer_angle << "\"";

                if ( istation == 7 ) {  //small GEM
                    FileOut << " pitch=\"" << StripPitch_SMALL << "\"";
                }
                else {
                    if( imodule == 0 || imodule == 1 ) FileOut << " pitch=\"" << StripPitch_UHP << "\"";
                    if( imodule == 2 || imodule == 3 ) FileOut << " pitch=\"" << StripPitch_LHP << "\"";
                }

                //define 'xorig' and 'yorig' coordinates (left bottom point), 'xsize' and 'ysize'
                Double_t xorig = 0.0; //default
                Double_t yorig = 0.0; //default
                Double_t xsize = 0.0; //default
                Double_t ysize = 0.0; //default

                if(ZoneTypeOfLayers[istation][ilayer] == 0) {  //main zone
                    if ( istation == 7 ) {  //small GEM
                        xorig = 0.0 + XMainZoneLayerSize_SMALL*0.5;
                        yorig = 0.0 - YMainZoneLayerSize_SMALL*0.5;
                        xsize = XMainZoneLayerSize_SMALL;
                        ysize = YMainZoneLayerSize_SMALL;
                    }
                    else {
                        if( imodule == 0 || imodule == 1 ) {
                            xorig = 0.0 + XMainZoneLayerSize_UHP*0.5;
                            yorig = 0.0 - YMainZoneLayerSize_UHP*0.5;
                            xsize = XMainZoneLayerSize_UHP;
                            ysize = YMainZoneLayerSize_UHP;
                        }
                        if( imodule == 2 || imodule == 3 ) {
                            xorig = 0.0 + XMainZoneLayerSize_LHP*0.5;
                            yorig = 0.0 - YMainZoneLayerSize_LHP*0.5;
                            xsize = XMainZoneLayerSize_LHP;
                            ysize = YMainZoneLayerSize_LHP;
                        }
                    }
                }
                else {  //hot zone
                    switch(imodule) {
                        case 0:
                            if(TString(StripTypeOfLayes[istation][ilayer]) == TString("lower")) {
                                xorig = 0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_StraightStrips_UHP;
                                yorig = 0.0 - YMainZoneLayerSize_UHP*0.5;
                                xsize = XHotZoneLayerSize_StraightStrips_UHP;
                                ysize = YHotZoneLayerSize_StraightStrips_UHP;
                            }
                            else {
                                xorig = 0.0 - XMainZoneLayerSize_UHP*0.5 + XHotZoneLayerSize_InclinedStrips_UHP[1];
                                yorig = 0.0 - YMainZoneLayerSize_UHP*0.5;
                                xsize = XHotZoneLayerSize_InclinedStrips_UHP[1];
                                ysize = YHotZoneLayerSize_InclinedStrips_UHP;
                            }
                            break;

                        case 1:
                            if(TString(StripTypeOfLayes[istation][ilayer]) == TString("lower")) {
                                xorig = 0.0 + XMainZoneLayerSize_UHP*0.5;
                                yorig = 0.0 - YMainZoneLayerSize_UHP*0.5;
                                xsize = XHotZoneLayerSize_StraightStrips_UHP;
                                ysize = YHotZoneLayerSize_StraightStrips_UHP;
                            }
                            else {
                                xorig = 0.0 + XMainZoneLayerSize_UHP*0.5;
                                yorig = 0.0 - YMainZoneLayerSize_UHP*0.5;
                                xsize = XHotZoneLayerSize_InclinedStrips_UHP[1];
                                ysize = YHotZoneLayerSize_InclinedStrips_UHP;
                            }
                            break;

                        case 2:
                            if(TString(StripTypeOfLayes[istation][ilayer]) == TString("lower")) {
                                xorig = 0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_StraightStrips_LHP;
                                yorig = 0.0 + YMainZoneLayerSize_LHP*0.5 - YHotZoneLayerSize_StraightStrips_LHP;
                                xsize = XHotZoneLayerSize_StraightStrips_LHP;
                                ysize = YHotZoneLayerSize_StraightStrips_LHP;
                            }
                            else {
                                xorig = 0.0 - XMainZoneLayerSize_LHP*0.5 + XHotZoneLayerSize_InclinedStrips_LHP[1];
                                yorig = 0.0 + YMainZoneLayerSize_LHP*0.5 - YHotZoneLayerSize_StraightStrips_LHP;
                                xsize = XHotZoneLayerSize_InclinedStrips_LHP[1];
                                ysize = YHotZoneLayerSize_InclinedStrips_LHP;
                            }
                            break;

                        case 3:
                            if(TString(StripTypeOfLayes[istation][ilayer]) == TString("lower")) {
                                xorig = 0.0 + XMainZoneLayerSize_LHP*0.5;
                                yorig = 0.0 + YMainZoneLayerSize_LHP*0.5 - YHotZoneLayerSize_StraightStrips_LHP;
                                xsize = XHotZoneLayerSize_StraightStrips_LHP;
                                ysize = YHotZoneLayerSize_StraightStrips_LHP;
                            }
                            else {
                                xorig = 0.0 + XMainZoneLayerSize_LHP*0.5;
                                yorig = 0.0 + YMainZoneLayerSize_LHP*0.5 - YHotZoneLayerSize_StraightStrips_LHP;
                                xsize = XHotZoneLayerSize_InclinedStrips_LHP[1];
                                ysize = YHotZoneLayerSize_InclinedStrips_LHP;
                            }
                            break;
                    }
                }

                FileOut << " xorig=\"" << xorig << "\"";
                FileOut << " yorig=\"" << yorig << "\"";
                FileOut << " xsize=\"" << xsize << "\"";
                FileOut << " ysize=\"" << ysize << "\"";

                if (YStationRotations[istation] == false) FileOut << " stripDirection=\"" << "LeftToRight" << "\"";
                else FileOut << " stripDirection=\"" << "RightToLeft" << "\"";

                //define 'lborder' and 'rborder'
                TString lborder = "";
                TString rborder = "";

                switch(imodule) {
                    case 0:
                        if(YStationRotations[istation] == false) {
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                lborder = "LeftTop";
                                rborder = "RightBottom";
                            }
                            else {  //hot zone
                                lborder = "LeftBottom";
                                rborder = "RightBottom";
                            }
                        }
                        else {
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                lborder = "LeftBottom";
                                rborder = "RightTop";
                            }
                            else {  //hot zone
                                lborder = "LeftTop";
                                rborder = "RightTop";
                            }
                        }
                        break;

                    case 1:
                        if(YStationRotations[istation] == false) {
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                lborder = "LeftTop";
                                rborder = "RightBottom";
                            }
                            else {  //hot zone
                                lborder = "LeftTop";
                                rborder = "RightTop";
                            }
                        }
                        else {
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                lborder = "LeftBottom";
                                rborder = "RightTop";
                            }
                            else {  //hot zone
                                lborder = "LeftBottom";
                                rborder = "RightBottom";
                            }
                        }
                        break;

                    case 2:
                        if(YStationRotations[istation] == false) {
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                lborder = "LeftBottom";
                                rborder = "RightTop";
                            }
                            else {  //hot zone
                                lborder = "LeftTop";
                                rborder = "RightTop";
                            }
                        }
                        else {
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                lborder = "LeftTop";
                                rborder = "RightBottom";
                            }
                            else {  //hot zone
                                lborder = "LeftBottom";
                                rborder = "RightBottom";
                            }
                        }
                        break;

                    case 3:
                        if(YStationRotations[istation] == false) {
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                lborder = "LeftBottom";
                                rborder = "RightTop";
                            }
                            else {  //hot zone
                                lborder = "LeftBottom";
                                rborder = "RightBottom";
                            }
                        }
                        else {
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                lborder = "LeftTop";
                                rborder = "RightBottom";
                            }
                            else {  //hot zone
                                lborder = "LeftTop";
                                rborder = "RightTop";
                            }
                        }
                        break;
                }

                FileOut << " lborder=\"" << lborder << "\"";
                FileOut << " rborder=\"" << rborder << "\"";
                FileOut << ">\n";

                //add dead zones
                if(istation != 7)  {//small GEM
                    switch(imodule) {
                        //module 0 -------------------------------------------------
                        case 0:
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                if(TString(StripTypeOfLayes[istation][ilayer]) == TString("lower")) { //straight strips
                                    FileOut << "\t\t\t\t<DeadZone>\n";
                                    for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        FileOut << " x=\"" << XDeadZone_Rect_UHP[ipoint] << "\"";
                                        FileOut << " y=\"" << YDeadZone_Rect_UHP[ipoint] << "\"";
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                                else { //inclined strips
                                    FileOut << "\t\t\t\t<DeadZone>\n";
                                    for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        if (YStationRotations[istation] == false) {
                                            FileOut << " x=\"" << XDeadZone_TrapLeft_UHP[ipoint] << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrapLeft_UHP[ipoint] << "\"";
                                        }
                                        else {
                                            FileOut << " x=\"" << XDeadZone_TrapRight_UHP[ipoint] << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrapRight_UHP[ipoint] << "\"";
                                        }
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                            }
                            else { //hot zone
                                FileOut << "\t\t\t\t<DeadZone>"; //hole
                                FileOut << " <!-- hole -->\n";
                                for(Int_t ipoint = 0; ipoint < NPoints_DeadZone_Hole_UHP; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    FileOut << " x=\"" << XDeadZone_Hole_UHP[ipoint] << "\"";
                                    FileOut << " y=\"" << YDeadZone_Hole_UHP[ipoint] << "\"";
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";

                                if(TString(StripTypeOfLayes[istation][ilayer]) == TString("upper")) { //inclined strips
                                    FileOut << "\t\t\t\t<DeadZone>"; //triangle
                                    FileOut << " <!-- triangle -->\n";
                                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        if (YStationRotations[istation] == false) {
                                            FileOut << " x=\"" << XDeadZone_TrianLeft_UHP[ipoint] << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrianLeft_UHP[ipoint] << "\"";
                                        }
                                        else {
                                            FileOut << " x=\"" << XDeadZone_TrianRight_UHP[ipoint] << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrianRight_UHP[ipoint] << "\"";
                                        }
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                            }
                            break;
                        //----------------------------------------------------------
                        //module 1 -------------------------------------------------
                        case 1:
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                if(TString(StripTypeOfLayes[istation][ilayer]) == TString("lower")) { //straight strips
                                    FileOut << "\t\t\t\t<DeadZone>\n";
                                    for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        FileOut << " x=\"" << XDeadZone_Rect_UHP[ipoint]*(-1) << "\"";
                                        FileOut << " y=\"" << YDeadZone_Rect_UHP[ipoint] << "\"";
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                                else { //inclined strips
                                    FileOut << "\t\t\t\t<DeadZone>\n";
                                    for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        if (YStationRotations[istation] == false) {
                                            FileOut << " x=\"" << XDeadZone_TrapRight_UHP[ipoint]*(-1) << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrapRight_UHP[ipoint] << "\"";
                                        }
                                        else {
                                            FileOut << " x=\"" << XDeadZone_TrapLeft_UHP[ipoint]*(-1) << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrapLeft_UHP[ipoint] << "\"";
                                        }
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                            }
                            else { //hot zone
                                FileOut << "\t\t\t\t<DeadZone>"; //hole
                                FileOut << " <!-- hole -->\n";
                                for(Int_t ipoint = 0; ipoint < NPoints_DeadZone_Hole_UHP; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    FileOut << " x=\"" << XDeadZone_Hole_UHP[ipoint]*(-1) << "\"";
                                    FileOut << " y=\"" << YDeadZone_Hole_UHP[ipoint] << "\"";
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";

                                if(TString(StripTypeOfLayes[istation][ilayer]) == TString("upper")) { //inclined strips
                                    FileOut << "\t\t\t\t<DeadZone>"; //triangle
                                    FileOut << " <!-- triangle -->\n";
                                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        if (YStationRotations[istation] == false) {
                                            FileOut << " x=\"" << XDeadZone_TrianRight_UHP[ipoint]*(-1) << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrianRight_UHP[ipoint] << "\"";
                                        }
                                        else {
                                            FileOut << " x=\"" << XDeadZone_TrianLeft_UHP[ipoint]*(-1) << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrianLeft_UHP[ipoint] << "\"";
                                        }
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                            }
                            break;
                        //----------------------------------------------------------
                        //module 2 -------------------------------------------------
                        case 2:
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                if(TString(StripTypeOfLayes[istation][ilayer]) == TString("lower")) { //straight strips
                                    FileOut << "\t\t\t\t<DeadZone>\n";
                                    for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        FileOut << " x=\"" << XDeadZone_Rect_LHP[ipoint] << "\"";
                                        FileOut << " y=\"" << YDeadZone_Rect_LHP[ipoint]*(-1) << "\"";
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                                else { //inclined strips
                                    FileOut << "\t\t\t\t<DeadZone>\n";
                                    for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        if (YStationRotations[istation] == false) {
                                            FileOut << " x=\"" << XDeadZone_TrapLeft_LHP[ipoint] << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrapLeft_LHP[ipoint]*(-1) << "\"";
                                        }
                                        else {
                                            FileOut << " x=\"" << XDeadZone_TrapRight_LHP[ipoint] << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrapRight_LHP[ipoint]*(-1) << "\"";
                                        }
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                            }
                            else { //hot zone
                                FileOut << "\t\t\t\t<DeadZone>"; //hole
                                FileOut << " <!-- hole -->\n";
                                for(Int_t ipoint = 0; ipoint < NPoints_DeadZone_Hole_LHP; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    FileOut << " x=\"" << XDeadZone_Hole_LHP[ipoint] << "\"";
                                    FileOut << " y=\"" << YDeadZone_Hole_LHP[ipoint]*(-1) << "\"";
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";

                                if(TString(StripTypeOfLayes[istation][ilayer]) == TString("upper")) { //inclined strips
                                    FileOut << "\t\t\t\t<DeadZone>"; //triangle
                                    FileOut << " <!-- triangle -->\n";
                                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        if (YStationRotations[istation] == false) {
                                            FileOut << " x=\"" << XDeadZone_TrianLeft_LHP[ipoint] << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrianLeft_LHP[ipoint]*(-1) << "\"";
                                        }
                                        else {
                                            FileOut << " x=\"" << XDeadZone_TrianRight_LHP[ipoint] << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrianRight_LHP[ipoint]*(-1) << "\"";
                                        }
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                            }
                            break;
                        //----------------------------------------------------------
                        //module 3 -------------------------------------------------
                        case 3:
                            if(ZoneTypeOfLayers[istation][ilayer] == 0) { //main zone
                                if(TString(StripTypeOfLayes[istation][ilayer]) == TString("lower")) { //straight strips
                                    FileOut << "\t\t\t\t<DeadZone>\n";
                                    for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        FileOut << " x=\"" << XDeadZone_Rect_LHP[ipoint]*(-1) << "\"";
                                        FileOut << " y=\"" << YDeadZone_Rect_LHP[ipoint]*(-1) << "\"";
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                                else { //inclined strips
                                    FileOut << "\t\t\t\t<DeadZone>\n";
                                    for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        if (YStationRotations[istation] == false) {
                                            FileOut << " x=\"" << XDeadZone_TrapRight_LHP[ipoint]*(-1) << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrapRight_LHP[ipoint]*(-1) << "\"";
                                        }
                                        else {
                                            FileOut << " x=\"" << XDeadZone_TrapLeft_LHP[ipoint]*(-1) << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrapLeft_LHP[ipoint]*(-1) << "\"";
                                        }
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                            }
                            else { //hot zone
                                FileOut << "\t\t\t\t<DeadZone>"; //hole
                                FileOut << " <!-- hole -->\n";
                                for(Int_t ipoint = 0; ipoint < NPoints_DeadZone_Hole_LHP; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    FileOut << " x=\"" << XDeadZone_Hole_LHP[ipoint]*(-1) << "\"";
                                    FileOut << " y=\"" << YDeadZone_Hole_LHP[ipoint]*(-1) << "\"";
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";

                                if(TString(StripTypeOfLayes[istation][ilayer]) == TString("upper")) { //inclined strips
                                    FileOut << "\t\t\t\t<DeadZone>"; //triangle
                                    FileOut << " <!-- triangle -->\n";
                                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                                        FileOut << "\t\t\t\t\t<DeadPoint";
                                        if (YStationRotations[istation] == false) {
                                            FileOut << " x=\"" << XDeadZone_TrianRight_LHP[ipoint]*(-1) << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrianRight_LHP[ipoint]*(-1) << "\"";
                                        }
                                        else {
                                            FileOut << " x=\"" << XDeadZone_TrianLeft_LHP[ipoint]*(-1) << "\"";
                                            FileOut << " y=\"" << YDeadZone_TrianLeft_LHP[ipoint]*(-1) << "\"";
                                        }
                                        FileOut << "/>\n";
                                    }
                                    FileOut << "\t\t\t\t</DeadZone>\n";
                                }
                            }
                            break;
                        //----------------------------------------------------------
                    }
                }
                FileOut << "\t\t\t</Layer>\n";
            }
            FileOut << "\t\t</Module>\n";
            FileOut << "\n";
        }
        FileOut << "\t</Station>\n";
        FileOut << "\n";

        //cout << "------------------------------------------------------------\n";
    }

    FileOut << "</StationSet>\n";

    FileOut.close();
}