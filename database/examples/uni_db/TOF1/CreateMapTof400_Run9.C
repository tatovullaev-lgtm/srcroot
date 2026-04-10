/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/main.c to edit this template
 */

/*
 * File:   CreateMapTof400_Run9.C
 * Author: mikhailr
 *
 * Created on September 22, 2025, 3:02 PM
 */

#include "Riostream.h"
#include "TString.h"

#include <TMath.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

fstream ff, ffStrip;
static const Int_t kNIt = 4;
static const Int_t kNDet = 10;
static const Int_t kNSlot = 23;

UInt_t crate;   // TTVXS Left side slot 12
UInt_t slot[kNSlot] = {0};

void FillPlacementMap(std::string mappp)
{
    // 2. Fill Placement map
    ff.open(mappp.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    if (ff.fail()) {
        cerr << "2. Cannot open the file " << mappp << endl;
        return;
    }
    for (Int_t i = 0; i < kNSlot; i++)
        if (slot[i] != 0) {
            ff << std::hex << std::setfill('0') << std::setw(8) << crate << '\t';
            ff << std::dec << i << '\t';
            ff << std::hex << std::setfill('0') << std::setw(8) << slot[i] << endl;
        }
    ff.close();
}

Int_t GetSlot(Int_t number)
{
    Int_t temp = static_cast<int>(number / 1000.);
    return temp;
}

Int_t GetCon(Int_t number)
{
    Int_t temp = static_cast<int>((number - GetSlot(number) * 1000.) / 100.);
    return temp;
}

Int_t GetDet(Int_t number)
{
    Int_t temp = static_cast<int>((number - GetSlot(number) * 1000. - GetCon(number) * 100.) / 10.);
    return temp;
}

Int_t GetSide(Int_t number)
{
    Int_t temp = static_cast<int>((number - GetSlot(number) * 1000. - GetCon(number) * 100. - GetDet(number) * 10.));
    return temp;
}

void CreateMapTof400_Run9(std::string placementMapFile = "TOF400_PlaceMap_RUN9.txt",
                          std::string mapFile = "TOF400_StripMap_RUN9.txt")
{

    Int_t MapR[24] = {3, 0, 9, 6, 4, 1, 10, 7, 5, 2, 11, 8, 18, 21, 12, 15, 19, 22, 13, 16, 20, 23, 14, 17};
    Int_t MapL[24] = {20, 23, 14, 17, 19, 22, 13, 16, 18, 21, 12, 15, 5, 2, 11, 8, 4, 1, 10, 7, 3, 0, 9, 6};
    // Int_t MapL[24] = {4, 1, 10, 7, 5, 2, 11, 8, 6, 3, 12, 9, 19, 22, 13, 16, 20, 23, 14, 17, 21, 0, 15, 18};
    // Int_t MapR[24] = {21, 0, 15, 18, 20, 23, 14, 17, 19, 22, 13, 16, 6, 3, 12, 9, 5, 2, 11, 8, 4, 1, 10, 7};

    // Crate 1 Left Side
    crate = 0x0cd9141f;   // TTVXS Left side slot 12
    memset(slot, 0, sizeof(slot));

    // Left crete #1
    //  box1 det 5-9 +++++
    slot[4] = 0x06e9404b;
    slot[5] = 0x06e9da9e;
    slot[6] = 0x06e940b1;
    slot[7] = 0x07a8deb2;
    slot[8] = 0x06e9403e;
    slot[9] = 0x06e9da86;
    slot[10] = 0x076cae7a;
    // box1 det 0-4 +++++
    slot[14] = 0x06e9e4b0;
    slot[15] = 0x076cae7b;   // new
    slot[16] = 0x076cad53;
    slot[17] = 0x06e940be;
    slot[18] = 0x076ccf5f;
    slot[19] = 0x076ccf56;
    slot[20] = 0x06e9da91;

    // 1. Placement map
    FillPlacementMap(placementMapFile);

    Int_t Num = 14516;
    cout << "Num = " << Num << endl;
    cout << "GetSlot(Num) = " << GetSlot(Num) << " GetCon(Num) = " << GetCon(Num) << endl;
    cout << "GetDet(Num) = " << GetDet(Num) << " GetSide(Num) = " << GetSide(Num) << endl;

    // 1. Fill Strip Map
    ffStrip.open(mapFile.c_str(), std::fstream::out);
    if (ffStrip.fail()) {
        cerr << "Cannot open the file " << mapFile << endl;
        return;
    }

    // Left crete #1
    // first number - slot 1-22, second number - connector (0 - 0..23, 1 - 24..47, 2 - 48..71
    // third - detector , fourth - side (1,2 - L (Down) , 3,4 - R (Up))
    Int_t MapDetL[kNDet][kNIt] = {
        // coll1+++++++
        {18203, 15104, 20101, 15202},   // det 0
        {18013, 14014, 19011, 16012},   // det 1
        {18123, 14224, 20021, 16122},   // det 2
        {17233, 14134, 19131, 17032},   // det 3
        {17143, 15044, 19241, 16242},   // det 4
        // coll2+++++++
        {9053, 5254, 8151, 5152},    // det 5
        {9163, 6064, 8261, 5062},    // det 6
        {9273, 6174, 8071, 4272},    // det 7
        {10083, 6284, 7281, 4182},   // det 8
        {10193, 7094, 7191, 4092},   // det 9
    };

    for (Int_t det = 0; det < kNDet; det++) {
        for (Int_t i = 0; i < kNIt; i++) {
            for (Int_t j = 0; j < 24; j++) {

                /*                  cout << std::dec << "MapDet = " << MapDet[det][i] << "; Det = " << det << "; i = "
                   << i << "; j = " << j << "; slot = " << slot[GetSlot(MapDet[det][i])]
                                          << "; chTDC = " << j + 24 * GetCon(MapDet[det][i]) << "; GetDet = " <<
                   GetDet(MapDet[det][i])
                                          << "; GetSide = " << GetSide(MapDet[det][i]) << endl;
                                  getchar();
                 */

                ffStrip << std::hex << std::setfill('0') << std::setw(8) << slot[GetSlot(MapDetL[det][i])]
                        << std::setfill(' ') << '\t';
                ffStrip << std::dec << j + 24 * GetCon(MapDetL[det][i]) << '\t';
                ffStrip << GetDet(MapDetL[det][i]) << '\t';
                if (GetSide(MapDetL[det][i]) == 1 || GetSide(MapDetL[det][i]) == 2) {
                    ffStrip << 47 - (MapL[j] + (GetSide(MapDetL[det][i]) - 1) * 24) << '\t';
                    // if (det < 5) ffStrip << 47 - (MapL[j] + (GetSide(MapDetL[det][i]) - 1)*24) << '\t';
                    // else ffStrip << MapL[j] + (GetSide(MapDetL[det][i]) - 1)*24 << '\t';
                    // ffStrip << MapL[j] + (GetSide(MapDet[det][i]) - 1)*24 << '\t';
                    ffStrip << 'L';
                    ffStrip << endl;
                } else if (GetSide(MapDetL[det][i]) == 3 || GetSide(MapDetL[det][i]) == 4) {
                    ffStrip << 47 - (MapR[j] + (GetSide(MapDetL[det][i]) - 3) * 24) << '\t';
                    // if (det < 5) ffStrip << 47 - (MapR[j] + (GetSide(MapDetL[det][i]) - 3)*24) << '\t';
                    // else ffStrip << MapR[j] + (GetSide(MapDetL[det][i]) - 3)*24 << '\t';
                    // ffStrip << MapR[j] + (GetSide(MapDet[det][i]) - 3)*24 << '\t';
                    ffStrip << 'R';
                    ffStrip << endl;
                } else {
                    ffStrip << -1 << '\t';
                    // ffStrip << 'L';
                    ffStrip << endl;
                }
            }
        }
    }
    ffStrip.close();

    // ------------------------------------------------------------------------------------------------
    // Crate #3 Box1 det 10-14, Box2 det 15-19
    memset(slot, 0, sizeof(slot));
    crate = 0x0a9ff045;   // TTVXS slot 12

    // box4 det 15-19 +++++
    slot[4] = 0x0e7619ce;
    slot[5] = 0x0e765cfa;
    slot[6] = 0x0e75504a;
    slot[7] = 0x0e7619cf;
    slot[8] = 0x0e755014;
    slot[9] = 0x0e755026;
    slot[10] = 0x0e7619bc;
    // box3 det 10-14 +++++
    slot[14] = 0x0e75e9e1;
    slot[15] = 0x0e765ced;
    slot[16] = 0x0e7619bb;
    slot[17] = 0x0e755038;
    slot[18] = 0x0e75502f;
    slot[19] = 0x0e755053;
    slot[20] = 0x0e755041;

    // 2. Fill Placement map
    FillPlacementMap(placementMapFile);

    // first number - slot 1-22, second number - connector (0 - 0..23, 1 - 24..47, 2 - 48..71
    // third - detector , fourth - side (1,2 - L, 3,4 - R)
    Int_t MapDetL2[kNDet][kNIt] = {
        // Box1 det 10-14,
        {20103, 16104, 18001, 14102},   // det 10
        {20013, 16024, 18111, 14012},   // det 11
        {19223, 17024, 17121, 15122},   // det 12
        {19133, 16234, 17231, 15032},   // det 13
        {19043, 15244, 18241, 14242},   // det 14
        // box2 det 15-19
        {9253, 10254, 8051, 5052},   // det 15
        {9163, 6264, 7161, 4262},    // det 16
        {10073, 6174, 7271, 5172},   // det 17
        {10183, 6084, 8171, 4082},   // det 18
        {9093, 5294, 8291, 4192},    // det 19
    };

    // 2. Fill Strip Map
    ffStrip.open(mapFile.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    if (ffStrip.fail()) {
        cerr << "2. Cannot open the file " << mapFile << endl;
        return;
    }

    for (Int_t det = 0; det < kNDet; det++) {
        for (Int_t i = 0; i < kNIt; i++) {
            for (Int_t j = 0; j < 24; j++) {
                /*
                                cout << std::dec << "MapDet = " << MapDet[det][i] << "; Det = " << det << "; i = " << i
                   << "; j = " << j << "; slot = " << slot[GetSlot(MapDet[det][i])]
                                        << "; chTDC = " << j + 24 * GetCon(MapDet[det][i]) << "; GetDet = " <<
                   GetDet(MapDet[det][i])
                                        << "; GetSide = " << GetSide(MapDet[det][i]) << endl;
                                getchar();
                 */

                ffStrip << std::hex << std::setfill('0') << std::setw(8) << slot[GetSlot(MapDetL2[det][i])]
                        << std::setfill(' ') << '\t';
                ffStrip << std::dec << j + 24 * GetCon(MapDetL2[det][i]) << '\t';
                ffStrip << GetDet(MapDetL2[det][i]) + 10 << '\t';
                if (GetSide(MapDetL2[det][i]) == 1 || GetSide(MapDetL2[det][i]) == 2) {
                    ffStrip << 47 - (MapL[j] + (GetSide(MapDetL2[det][i]) - 1) * 24) << '\t';
                    // if (det >= 5) ffStrip << 47 - (MapL[j] + (GetSide(MapDetL2[det][i]) - 1)*24) << '\t';
                    // else ffStrip << MapL[j] + (GetSide(MapDetL2[det][i]) - 1)*24 << '\t';
                    // ffStrip << MapL[j] + (GetSide(MapDetL2[det][i]) - 1)*24 << '\t';
                    ffStrip << 'L';
                    ffStrip << endl;
                } else if (GetSide(MapDetL2[det][i]) == 3 || GetSide(MapDetL2[det][i]) == 4) {
                    ffStrip << 47 - (MapR[j] + (GetSide(MapDetL2[det][i]) - 3) * 24) << '\t';
                    // if (det >= 5) ffStrip << 47 - (MapR[j] + (GetSide(MapDetL2[det][i]) - 3)*24) << '\t';
                    // else ffStrip << MapR[j] + (GetSide(MapDetL2[det][i]) - 3)*24 << '\t';
                    // ffStrip << MapR[j] + (GetSide(MapDetL2[det][i]) - 3)*24 << '\t';
                    ffStrip << 'R';
                    ffStrip << endl;
                } else {
                    ffStrip << -1 << '\t';
                    // ffStrip << 'L';
                    ffStrip << endl;
                }
            }
        }
    }
    ffStrip.close();

    // ------------------------------------------------------------------------------------------------
    // Crate #1 Box2 det 20-29
    memset(slot, 0, sizeof(slot));
    crate = 0xcd9687a;   // TTVXS slot 12

    // box2 det 24-29
    slot[4] = 0x076caef6;
    slot[5] = 0x06e9404a;
    slot[6] = 0x076d180a;
    slot[7] = 0x06e940b2;
    slot[8] = 0x080c05e4;
    slot[9] = 0x076d445b;
    slot[10] = 0x07a96e8a;
    // box2 det 20-24
    slot[14] = 0x07a9fc6f;
    slot[15] = 0x07a9b249;
    slot[16] = 0x080bcc14;
    slot[17] = 0x07a9b1d6;
    slot[18] = 0x07a9e7d0;
    slot[19] = 0x07a9fbf8;
    slot[20] = 0x07a9eef2;

    // 2. Fill Placement map
    FillPlacementMap(placementMapFile);

    // first number - slot 1-22, second number - connector (0 - 0..23, 1 - 24..47, 2 - 48..71
    // third - detector , fourth - side (1,2 - L, 3,4 - R)
    Int_t MapDetL3[kNDet][kNIt] = {
        // Box1 det 20-24,
        {20103, 18204, 17001, 14102},   // det 20
        {20013, 18114, 16211, 15112},   // det 21
        {19223, 18024, 16021, 14022},   // det 22
        {19133, 17234, 16131, 14132},   // det 23
        {19043, 17144, 15241, 15042},   // det 24
        // box2 det 25-29
        {10153, 6054, 8051, 4252},   // det 25
        {10063, 5264, 8061, 4162},   // det 26
        {9273, 6174, 7271, 4072},    // det 27
        {9083, 6284, 8181, 5082},    // det 28
        {9193, 7094, 7191, 5192},    // det 29
    };

    // 2. Fill Strip Map
    ffStrip.open(mapFile.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    if (ffStrip.fail()) {
        cerr << "2. Cannot open the file " << mapFile << endl;
        return;
    }

    for (Int_t det = 0; det < kNDet; det++) {
        for (Int_t i = 0; i < kNIt; i++) {
            for (Int_t j = 0; j < 24; j++) {
                /*
                                cout << std::dec << "MapDet = " << MapDet[det][i] << "; Det = " << det << "; i = " << i
                   << "; j = " << j << "; slot = " << slot[GetSlot(MapDet[det][i])]
                                        << "; chTDC = " << j + 24 * GetCon(MapDet[det][i]) << "; GetDet = " <<
                   GetDet(MapDet[det][i])
                                        << "; GetSide = " << GetSide(MapDet[det][i]) << endl;
                                getchar();
                 */

                ffStrip << std::hex << std::setfill('0') << std::setw(8) << slot[GetSlot(MapDetL3[det][i])]
                        << std::setfill(' ') << '\t';
                ffStrip << std::dec << j + 24 * GetCon(MapDetL3[det][i]) << '\t';
                ffStrip << GetDet(MapDetL3[det][i]) + 20 << '\t';
                if (GetSide(MapDetL3[det][i]) == 1 || GetSide(MapDetL3[det][i]) == 2) {
                    ffStrip << 47 - (MapL[j] + (GetSide(MapDetL3[det][i]) - 1) * 24) << '\t';
                    // if (det >= 5) ffStrip << 47 - (MapL[j] + (GetSide(MapDetL3[det][i]) - 1)*24) << '\t';
                    // else ffStrip << MapL[j] + (GetSide(MapDetL3[det][i]) - 1)*24 << '\t';
                    // ffStrip << MapL[j] + (GetSide(MapDetL3[det][i]) - 1)*24 << '\t';
                    ffStrip << 'L';
                    ffStrip << endl;
                } else if (GetSide(MapDetL3[det][i]) == 3 || GetSide(MapDetL3[det][i]) == 4) {
                    ffStrip << 47 - (MapR[j] + (GetSide(MapDetL3[det][i]) - 3) * 24) << '\t';
                    // if (det >= 5) ffStrip << 47 - (MapR[j] + (GetSide(MapDetL3[det][i]) - 3)*24) << '\t';
                    // else ffStrip << MapR[j] + (GetSide(MapDetL3[det][i]) - 3)*24 << '\t';
                    // ffStrip << MapR[j] + (GetSide(MapDetL3[det][i]) - 3)*24 << '\t';
                    ffStrip << 'R';
                    ffStrip << endl;
                } else {
                    ffStrip << -1 << '\t';
                    // ffStrip << 'L';
                    ffStrip << endl;
                }
            }
        }
    }
    ffStrip.close();
}
