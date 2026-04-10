/************************************************************
 * Copyright: JINR
 * Authors: V.Leontyev
 * Goal: to setup the STS constants, calculations
 * Date: 07.03.2026
 ************************************************************/

#ifndef STSCONST_H
#define STSCONST_H

#include <TMath.h>

namespace StsConst
{
static constexpr uint16_t kuHitNbAdcBins = 32;   // 5 bit
static constexpr uint16_t kuHitNbTsBins = 512;   // 9 bit
static constexpr double kdClockCycleNs = 12.5;
static constexpr uint16_t kuStripsPerSide = 1024;

static constexpr Double_t kdSensorsSzX = 60.;   // [mm], active is 59.570 mm (kiNbStrips*kdPitchMm)
static constexpr Double_t kdSensorsSzY = 60.;   // [mm], jun 22

static constexpr Double_t kdPitchMm = 0.058;   // [mm]

static constexpr Double_t kdStereoAngle = 7.5;   // [deg], angle-2023

// static constexpr uint16_t kuMaxELinks = 96;
static constexpr uint16_t kuNbElinksPerDpb = 48;
static constexpr uint16_t kuTotalNbOfSensors = 6;

static constexpr uint16_t kuTotalNbOfMAPS = 3;

enum SideType
{
    POS = 0,
    NEG = 1
};
static const TString cSideName[2] = {"POS", "NEG"};   // naming, MUST coinside with SideType
// static constexpr char cSideName[1024][2] = {"POS", "NEG"}; // naming, MUST coinside with SideType

static constexpr uint16_t kuELinkToAsicMap[kuNbElinksPerDpb]{
    // mar23 mapping 2AFCK
    0x0000, 0x0002, 0x0004, 0x0006, 0x0001, 0x0003, 0x0005, 0x0007, 0x0009, 0x000B, 0x000D, 0x000F,
    0x0008, 0x000A, 0x000C, 0x000E, 0x0010, 0x0012, 0x0014, 0x0016, 0x0011, 0x0013, 0x0015, 0x0017,   // p-side, sens01
    0x0019, 0x001B, 0x001D, 0x001F, 0x0018, 0x001A, 0x001C, 0x001E,                                   // n-side. sens01
    0x0020, 0x0022, 0x0024, 0x0026, 0x0021, 0x0023, 0x0025, 0x0027,                                   // p-side, sens02
    0x0029, 0x002B, 0x002D, 0x002F, 0x0028, 0x002A, 0x002C, 0x002E,                                   // n-side. sens02
};
static constexpr uint16_t kuDpbArray[2]{0x18D1, 0x1940};   // apr25
//  static constexpr uint16_t kuDpbArray[2] {0x18D1, 0x10DB}; // 6353, 4315 - run323(generator)-feb24
//    static constexpr uint16_t kuDpbArray[2] {0x10DB, 0x18D1}; // 4315, 6353

//  enum SensMID{B011 = 0, B010 = 1, B004 = 2, B035 = 3}; // Manufacturer Id, beam arranged in Gatchina march23
// enum SensMID{B011 = 3, B010 = 2, B004 = 1, B008 = 0}; // cosmics-feb24
// enum SensMID{B011 = 0, B010 = 1, B004 = 2, B008 = 3, B034 = 4, B032 = 5}; // Gatchina march24, details
//  enum SensMID{ B011, B010, B004, B008, B034, B032 }; // Gatchina march24
// enum SensMID{B008 = 0, B032 = 1, B004 = 2, B010 = 3, B034 = 4, B011 = 5}; // Dubna dec24-err?, details
enum SensMID
{
    B008 = 0,
    B032 = 1,
    B004 = 2,
    B011 = 3,
    B034 = 4,
    B010 = 5
};   // Dubna mar26, details
static const TString sMID[kuTotalNbOfSensors]{"B008", "B032", "B004",
                                              "B011", "B034", "B010"};   // naming, MUST coinside with SensMID, mar26

static const Double_t kdStereoAngleTan = TMath::Tan(StsConst::kdStereoAngle * TMath::DegToRad());

UInt_t GetIdx_SideFromAsic(UInt_t iAsicIdx)
{
    if (((iAsicIdx / 8) % 2) != 0)
        return StsConst::NEG;   // every odd 8th-modulo, "8-15", etc
    return StsConst::POS;
};

uint16_t GetIdx_AsicFromELink(uint16_t uELinkIdx)
{
    // from CbmCern2017UnpackParSts
    if (uELinkIdx < StsConst::kuNbElinksPerDpb)
        return StsConst::kuELinkToAsicMap[uELinkIdx];
    else {
        cout << " GetIdx_AsicFromELink  => ELink Index is out of bound: " << uELinkIdx;
        cout << "\treturning dummy value!(0)" << endl;
        return 0;
    }
};

UInt_t GetIdx_SensorFromAsic(uint16_t iDpbIdx, uint16_t iAsicIdx)
{                                               // call for the multi DPB analysis
    if (iDpbIdx == StsConst::kuDpbArray[0]) {   // the 1st one, 4315 , 0x10DB
        if (iAsicIdx <= 0x000F) {
            return 0;
        }   // the stright forward order for the BeamBox, the closest module
        else if (iAsicIdx >= 0x0010 && iAsicIdx <= 0x001F)
        {
            return 1;
        }   // the 2nd module
        else if (iAsicIdx >= 0x0020 && iAsicIdx <= 0x002F)
        {
            return 2;
        }   // the 3rd module
    }
    if (iDpbIdx == StsConst::kuDpbArray[1]) {   // the 2nd one, 46353 , 0x18D1
        if (iAsicIdx <= 0x000F) {
            return 3;
        }
        // if (iAsicIdx >= 0x0010 &&  iAsicIdx <= 0x001F) { return 3; }
        else if (iAsicIdx >= 0x0010 && iAsicIdx <= 0x001F)
        {
            return 4;
        } else if (iAsicIdx >= 0x0020 && iAsicIdx <= 0x002F) {
            return 5;
        }
        cout << "GetIdx_SensorFromAsic: WARNING, iAsicIdx= " << iAsicIdx;
        cout << "\tat iDpbIdx= " << iDpbIdx << endl;
        return 3;
        //      else if (iAsicIdx >= 0x0010 &&  iAsicIdx <= 0x001F) { return 3; }
        //      else if (iAsicIdx >= 0x0020 &&  iAsicIdx <= 0x002F) { return 3; }
        //      else if (iAsicIdx >= 0x0030 &&  iAsicIdx <= 0x003F) { return 3; }
    }
    cout << "GetIdx_SensorFromAsic: unknown DPB index, using Asic 0" << endl;
    return 0;
};

uint16_t GetIdx_Strip(uint16_t iSensIdx, uint16_t iAsicIdx, uint16_t iChannel)
{
    uint16_t st = 0;
    uint16_t iAsIdx = iAsicIdx & 0xF;   // take only lower 4 bits (inside one Sensor)
    uint16_t iF8Idx = iAsIdx % 8;       // FEB8 chip index inside one Side

    // MappingPatch apr23, v2.1, (_work51)
    SensMID midSensIdx = static_cast<SensMID>(iSensIdx);

    if (iAsIdx > 7) {                         // N side, begin
        st = 128 * iF8Idx + 127 - iChannel;   // apr23-II, eq.(3)
        // if ( iSensIdx==1 ) { // Patch Sens01Chip00, N side, apr23-II
        if (midSensIdx == B010) {   // Patch Sens05 Chip00, N side, apr23-II, MID-mod-mar26
            if (iAsIdx == 0x8 && iChannel % 2 == 0 && iChannel != 126) {
                st = 128 * iF8Idx + 127 - iChannel - 2;   // eq.(4)
                // cout << "StsConst.h: eq.4 patch,  iSensIdx= " << iSensIdx << "\tiF8Idx=" << iF8Idx
                //<< "\tiChannel=" << iChannel << "\tStrip=" << st << endl;
            }
        }
    }   // N side, end
    else
    {   // P side, begin
        if (iChannel % 2 == 0)
            st = 128 * iF8Idx + iChannel;   // Patch of the mapping in apr23, v2.0, eq.(1)
        else {
            st = 128 * iF8Idx + iChannel + 2;   // eq.(2)
            if (iF8Idx == 7 && iChannel == 127)
                st = 1;
        }
        if (midSensIdx == B010 && iAsIdx == 0x3   // Sens05, chip03, MID-mod-mar26
                                                  // midSensIdx == B035 && (iAsIdx == 0x2 || iAsIdx == 0x4)
                                                  // SensNO,
                                                  // chip02, chip04, MID-mod-mar26
        )
            st = 128 * iF8Idx + iChannel + 2;   // eq.(-2)
    }   // P side, end

    return st;
};

struct fsdHitXY
{
    Double_t X;
    Double_t Y;
};

//  fsdHitXY fsdSensLocCoord[kuTotalNbOfSensors] = {-60., 30.,   0., 60.,   60., 30.,
//						  60., -30.,   0., -60.,    -60., -30}; // VSPdec24 coordinates

// const Double_t dSp = 58. * 1.024; // for fsdSensLocCoord calculation - the case of a sharp alignment
const Double_t dSp = 58. * 0.990;   // for fsdSensLocCoord calculation - 34 strips shift
const Double_t dSpHalf = 0.5 * dSp;

fsdHitXY fsdSensLocCoord[kuTotalNbOfSensors] = {-dSp, dSpHalf, 0., dSp,  dSp,  30.,
                                                dSp,  -30.,    0., -dSp, -dSp, -30};   // VSPfeb25 coordinates

fsdHitXY ComputeHitXYfromStrip(Double_t dStripN, Double_t dStripP)
{
    fsdHitXY f1;
    Double_t dMapSizeMmX = StsConst::kdSensorsSzX;
    Double_t dMapSizeMmY = StsConst::kdSensorsSzY;

    Double_t dCoordN = dStripN;
    Double_t dCoordP = dStripP;

    const Double_t dZgap = 134.;

    /* // Gatchina24 coordinates, begin
    f1.X = dMapSizeMmX / 2.;
    f1.Y = dMapSizeMmY / 2.;

    f1.Y -= StsConst::kdPitchMm * dCoordN;   // work46

    // Z-strips, the lower half
    if( dCoordP <= (dZgap-1.) && dCoordN >= (StsConst::kuStripsPerSide-dZgap-1.) ) {
      f1.X -=  ( (dCoordP + StsConst::kuStripsPerSide) - dCoordN ) *
    StsConst::kdPitchMm / kdStereoAngleTan;
    }
    else { // non-Z-strips, or the upper half with the Z-strips
      f1.X -= ( dCoordP - dCoordN ) * StsConst::kdPitchMm / kdStereoAngleTan; // work46
    }
// Gatchina24 coordinates, end
    */

    // VSPdec24 coordinates, begin
    f1.X = -dMapSizeMmX / 2.;
    f1.Y = -dMapSizeMmY / 2.;

    f1.X += StsConst::kdPitchMm * dCoordN;

    // Z-strips, the lower half
    if (dCoordP <= (dZgap - 1.) && dCoordN >= (StsConst::kuStripsPerSide - dZgap - 1.)) {
        f1.Y += ((dCoordP + StsConst::kuStripsPerSide) - dCoordN) * StsConst::kdPitchMm / kdStereoAngleTan;
    } else {   // non-Z-strips, or the upper half with the Z-strips
        f1.Y += (dCoordP - dCoordN) * StsConst::kdPitchMm / kdStereoAngleTan;
    }

    // VSPdec24 coordinates, end

    return f1;
};

fsdHitXY ComputeVspGeomFromSsXY(UInt_t uSensIdx, Double_t dSsX, Double_t dSsY)
{
    fsdHitXY f2;

    // f2.X = static_cast<Double_t> (uSensIdx);
    // f2.Y = static_cast<Double_t> (uSensIdx);
    if (uSensIdx < 3) {   // the TOP halfstation
        f2.X = fsdSensLocCoord[uSensIdx].X - dSsX;
        f2.Y = fsdSensLocCoord[uSensIdx].Y - dSsY;
        // f2.X = fsdSensLocCoord[uSensIdx].X - dSsY;
        // f2.Y = fsdSensLocCoord[uSensIdx].Y - dSsX;
    } else if (uSensIdx < 6) {   // the BOTTOM halfstation
        f2.X = fsdSensLocCoord[uSensIdx].X + dSsX;
        f2.Y = fsdSensLocCoord[uSensIdx].Y + dSsY;
    } else {   // dummy case
        f2.X = -1.;
        f2.Y = -1.;
    }
    return f2;
};

};   // namespace StsConst
#endif   // STSCONST_H