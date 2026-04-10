/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/main.c to edit this template
 */

/* 
 * File:   CreateMapTof400_Run9.C
 * Author: mikhailr
 *
 * Created on November 25, 2022, 3:02 PM
 */


#include <TMath.h>
#include "TString.h"
#include <iostream>
#include "Riostream.h"
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

Int_t GetSlot(Int_t number) {
    Int_t temp = static_cast<int> (number / 1000.);
    return temp;
}

Int_t GetCon(Int_t number) {
    Int_t temp = static_cast<int> ((number - GetSlot(number)*1000.) / 100.);
    return temp;
}

Int_t GetDet(Int_t number) {
    Int_t temp = static_cast<int> ((number - GetSlot(number)*1000. - GetCon(number)*100.) / 10.);
    return temp;
}

Int_t GetSide(Int_t number) {
    Int_t temp = static_cast<int> ((number - GetSlot(number)*1000. - GetCon(number)*100. - GetDet(number)*10.));
    return temp;
}

void CreateMapTof400_Run9(std::string placementMapFile = "../../../../input/TOF400_PlaceMap_RUN9.txt",
        std::string mapFile = "../../../../input/TOF400_StripMap_RUN9.txt") {


    Int_t MapR[24] = {3, 0, 9, 6, 4, 1, 10, 7, 5, 2, 11, 8, 18, 21, 12, 15, 19, 22, 13, 16, 20, 23, 14, 17};
    Int_t MapL[24] = {20, 23, 14, 17, 19, 22, 13, 16, 18, 21, 12, 15, 5, 2, 11, 8, 4, 1, 10, 7, 3, 0, 9, 6};
    //Int_t MapL[24] = {4, 1, 10, 7, 5, 2, 11, 8, 6, 3, 12, 9, 19, 22, 13, 16, 20, 23, 14, 17, 21, 0, 15, 18};
    //Int_t MapR[24] = {21, 0, 15, 18, 20, 23, 14, 17, 19, 22, 13, 16, 6, 3, 12, 9, 5, 2, 11, 8, 4, 1, 10, 7};

    //Left Side
    UInt_t crate = 0x0cd91420; // TTVXS Left side slot 12
    static const Int_t kNSlot = 23;
    UInt_t slot[kNSlot] = {0};
    memset(slot, 0, sizeof (slot));

    // box2 det 5-9 +++++
    slot[4] = 0x06e9404b;
    slot[5] = 0x06e9da9e;
    slot[6] = 0x06e940b1;
    slot[7] = 0x07a8deb2;
    slot[8] = 0x06e9403e;
    slot[9] = 0x06e9da86;
    slot[10] = 0x076cae7a;
    // box1 det 0-4 +++++
    slot[14] = 0x06e9e4b0;
    slot[15] = 0x06e9da9d;
    slot[16] = 0x076cad53;
    slot[17] = 0x06e940be;
    slot[18] = 0x076ccf5f;
    slot[19] = 0x076ccf56;
    slot[20] = 0x06e9da91;

    fstream ff, ffStrip;

    //1. Placement map
    ff.open(placementMapFile.c_str(), std::fstream::out);
    if (ff.fail()) {
        cerr << "Cannot open the file " << placementMapFile << endl;
        return;
    }
    for (Int_t i = 0; i < kNSlot; i++)
        if (slot[i] != 0) {
            ff << std::hex << std::setfill('0') << std::setw(8) << crate << '\t';
            ff << std::dec << i << '\t';
            ff << std::hex << std::setfill('0') << std::setw(8) << slot[i] << endl;
        }
    ff.close();


    Int_t Num = 14516;
    cout << "Num = " << Num << endl;
    cout << "GetSlot(Num) = " << GetSlot(Num) << " GetCon(Num) = " << GetCon(Num) << endl;
    cout << "GetDet(Num) = " << GetDet(Num) << " GetSide(Num) = " << GetSide(Num) << endl;


    //1. Fill Strip Map
    ffStrip.open(mapFile.c_str(), std::fstream::out);
    if (ffStrip.fail()) {
        cerr << "Cannot open the file " << mapFile << endl;
        return;
    }

    static const Int_t kNIt = 4;
    static const Int_t kNDet = 10;

    // Right arm
    Int_t MapDetL[kNDet][kNIt] = {
        //box1
        {14003, 14101, 14204, 15002}, //det 0
        {15113, 15211, 16014, 16112}, //det 1
        {16223, 17021, 17124, 17222}, //det 2
        {18033, 18131, 18234, 19032}, //det 3
        {19143, 19241, 20044, 20142}, //det 4
        //box2
        {4053, 4151, 4254, 5052}, //det 5
        {5163, 5261, 6064, 6162}, //det 6
        {6273, 7071, 7174, 7272}, //det 7
        {8083, 8181, 8284, 9082}, //det 8
        {9193, 9291, 10094, 10192}, //det 9
    };

    //first number - slot 1-22, second number - connector (0 - 0..23, 1 - 24..47, 2 - 48..71
    //third - detector , fourth - side (1,2 - L (Down) , 3,4 - R (Up))


    for (Int_t det = 0; det < kNDet; det++) {
        for (Int_t i = 0; i < kNIt; i++) {
            for (Int_t j = 0; j < 24; j++) {

                /*                  cout << std::dec << "MapDet = " << MapDet[det][i] << "; Det = " << det << "; i = " << i << "; j = " << j << "; slot = " << slot[GetSlot(MapDet[det][i])]
                                          << "; chTDC = " << j + 24 * GetCon(MapDet[det][i]) << "; GetDet = " << GetDet(MapDet[det][i])
                                          << "; GetSide = " << GetSide(MapDet[det][i]) << endl;
                                  getchar();
                 */

                ffStrip << std::hex << std::setfill('0') << std::setw(8) << slot[GetSlot(MapDetL[det][i])] << std::setfill(' ') << '\t';
                ffStrip << std::dec << j + 24 * GetCon(MapDetL[det][i]) << '\t';
                ffStrip << GetDet(MapDetL[det][i]) << '\t';
                if (GetSide(MapDetL[det][i]) == 1 || GetSide(MapDetL[det][i]) == 2) {
                    if (det < 5) ffStrip << 47 - (MapL[j] + (GetSide(MapDetL[det][i]) - 1)*24) << '\t';
                    else ffStrip << MapL[j] + (GetSide(MapDetL[det][i]) - 1)*24 << '\t';
                    //ffStrip << MapL[j] + (GetSide(MapDet[det][i]) - 1)*24 << '\t';
                    ffStrip << 'L';
                    ffStrip << endl;
                } else if (GetSide(MapDetL[det][i]) == 3 || GetSide(MapDetL[det][i]) == 4) {
                    if (det < 5) ffStrip << 47 - (MapR[j] + (GetSide(MapDetL[det][i]) - 3)*24) << '\t';
                    else ffStrip << MapR[j] + (GetSide(MapDetL[det][i]) - 3)*24 << '\t';
                    //ffStrip << MapR[j] + (GetSide(MapDet[det][i]) - 3)*24 << '\t';
                    ffStrip << 'R';
                    ffStrip << endl;
                } else {
                    ffStrip << -1 << '\t';
//                    ffStrip << 'L';
                    ffStrip << endl;
                }
            }
        }
    }
    ffStrip.close();
    // 

    //Right side
    memset(slot, 0, sizeof (slot));
    crate = 0x0cd9687a; // TTVXS slot 12

    //box4 det 15-19 +++++
    slot[4] = 0x076caef6;
    slot[5] = 0x06e9404a;
    slot[6] = 0x076d180a;
    slot[7] = 0x06e940b2;
    slot[8] = 0x080c05e4;
    slot[9] = 0x076d445b;
    slot[10] = 0x07a96e8a;
    //box3 det 10-14 +++++
    slot[14] = 0x07a9fc6f;
    slot[15] = 0x07a9b249;
    slot[16] = 0x080bcc14;
    slot[17] = 0x07a9b1d6;
    slot[18] = 0x07a9e7d0;
    slot[19] = 0x07a9fbf8;
    slot[20] = 0x07a9eef2;


    //2. Fill Placement map
    ff.open(placementMapFile.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    if (ff.fail()) {
        cerr << "2. Cannot open the file " << placementMapFile << endl;
        return;
    }
    for (Int_t i = 0; i < kNSlot; i++)
        if (slot[i] != 0) {
            ff << std::hex << std::setfill('0') << std::setw(8) << crate << '\t';
            ff << std::dec << i << '\t';
            ff << std::hex << std::setfill('0') << std::setw(8) << slot[i] << endl;
        }
    ff.close();

    //fill it !!!!!!!!!!!!!!!!!

    Int_t MapDetR[kNDet][kNIt] = {
        //box3
        {14003, 14101, 14204, 15002}, //det 10
        {15113, 15211, 16014, 16112}, //det 11
        {16223, 17021, 17124, 17222}, //det 12
        {18033, 18131, 18234, 19032}, //det 13
        {19143, 19241, 20044, 20142}, //det 14
        //box4
        {4053, 4151, 4254, 5052}, //det 15
        {5163, 5261, 6064, 6162}, //det 16
        {6273, 7071, 7174, 7272}, //det 17
        {8083, 8181, 8284, 9082}, //det 18
        {9193, 9291, 10094, 10192}, //det 19
    };

    //2. Fill Strip Map
    ffStrip.open(mapFile.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    if (ffStrip.fail()) {
        cerr << "2. Cannot open the file " << mapFile << endl;
        return;
    }

    //first number - slot 1-22, second number - connector (0 - 0..23, 1 - 24..47, 2 - 48..71
    //third - detector , fourth - side (1,2 - L, 3,4 - R)


    for (Int_t det = 0; det < kNDet; det++) {
        for (Int_t i = 0; i < kNIt; i++) {
            for (Int_t j = 0; j < 24; j++) {
                /*
                                cout << std::dec << "MapDet = " << MapDet[det][i] << "; Det = " << det << "; i = " << i << "; j = " << j << "; slot = " << slot[GetSlot(MapDet[det][i])]
                                        << "; chTDC = " << j + 24 * GetCon(MapDet[det][i]) << "; GetDet = " << GetDet(MapDet[det][i])
                                        << "; GetSide = " << GetSide(MapDet[det][i]) << endl;
                                getchar();
                 */

               ffStrip << std::hex << std::setfill('0') << std::setw(8) << slot[GetSlot(MapDetL[det][i])] << std::setfill(' ') << '\t';
                ffStrip << std::dec << j + 24 * GetCon(MapDetL[det][i]) << '\t';
                ffStrip << GetDet(MapDetL[det][i]) + 10 << '\t';
                if (GetSide(MapDetL[det][i]) == 1 || GetSide(MapDetL[det][i]) == 2) {
                    if (det >= 5) ffStrip << 47 - (MapL[j] + (GetSide(MapDetL[det][i]) - 1)*24) << '\t';
                    else ffStrip << MapL[j] + (GetSide(MapDetL[det][i]) - 1)*24 << '\t';
                    //ffStrip << MapL[j] + (GetSide(MapDet[det][i]) - 1)*24 << '\t';
                    ffStrip << 'L';
                    ffStrip << endl;
                } else if (GetSide(MapDetL[det][i]) == 3 || GetSide(MapDetL[det][i]) == 4) {
                    if (det >= 5) ffStrip << 47 - (MapR[j] + (GetSide(MapDetL[det][i]) - 3)*24) << '\t';
                    else ffStrip << MapR[j] + (GetSide(MapDetL[det][i]) - 3)*24 << '\t';
                    //ffStrip << MapR[j] + (GetSide(MapDet[det][i]) - 3)*24 << '\t';
                    ffStrip << 'R';
                    ffStrip << endl;
                } else {
                    ffStrip << -1 << '\t';
//                    ffStrip << 'L';
                    ffStrip << endl;
                }
            }
        }
    }
    ffStrip.close();
}


