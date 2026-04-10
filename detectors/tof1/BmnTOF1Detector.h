/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BmnTOF1Detector.h
 * Author: mikhail
 *
 * Created on March 13, 2017, 4:52 PM
 */

#ifndef BMNTOF1DETECTOR_H
#define BMNTOF1DETECTOR_H 1

#include "BmnEnums.h"
#include "BmnEventHeader.h"
#include "BmnTOF1Conteiner.h"
#include "BmnTOF1Point.h"
#include "BmnTof1Digit.h"
#include "BmnTof1GeoUtils.h"
#include "BmnTof701Digit.h"
#include "BmnTofHit.h"
#include "BmnTrigDigit.h"
#include "TClonesArray.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TList.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"
#include "TVector3.h"

#include <RtypesCore.h>
#include <TGeoManager.h>
#include <TKey.h>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class BmnTOF1Detector
{
  private:
    static const Int_t fHalfMaxInt = kMaxInt * 0.5 - 1;
    static const Int_t fNStrMax = 48;
    DetectorId fTofId;
    Int_t fVerbose;
    Double_t fStripLength, fSignalVelosity, fSignalVelosityStr[fNStrMax];
    TString fName;
    Int_t fNPlane, fNStr;
    Int_t fFillHist;

    std::vector<std::vector<std::vector<Double_t>>> vHitsL;
    std::vector<std::vector<std::vector<Double_t>>> vHitsR;
    // std::vector<std::vector<std::vector<Double_t>>> vHits;

    Short_t fIndexL[fNStrMax], fIndexR[fNStrMax], fIndexLtemp[fNStrMax], fIndexRtemp[fNStrMax];
    Double_t fTimeL[fNStrMax], fTimeR[fNStrMax], fTimeLtemp[fNStrMax], fTimeRtemp[fNStrMax], fTime[fNStrMax];
    Double_t fWidthL[fNStrMax], fWidthR[fNStrMax], fWidthLtemp[fNStrMax], fWidthRtemp[fNStrMax], fWidth[fNStrMax];
    Double_t fTof[fNStrMax];
    Double_t fDoubleTemp, fMaxDelta;
    Int_t fHit_Per_Ev, fNEvents, fStrip;
    Bool_t fFlagHit[fNStrMax], fKilled[fNStrMax];
    Int_t fKillSide;
    Double_t fCorrLR[fNStrMax], fCorrTimeShift[fNStrMax];
    Int_t fDigitL[fNStrMax], fDigitR[fNStrMax], fHit[fNStrMax];
    Double_t fCommonTimeShift;
    TVector3 fCentrStrip[fNStrMax], fStripAngle[fNStrMax], fCrossPoint[fNStrMax], fVectorTemp;
    BmnTrigDigit* fT0;

    TList* fHistListStat;
    TList* fHistListdt;

    TH2S *hdT_vs_WidthDet[fNStrMax + 1], *hdT_vs_WidthT0[fNStrMax + 1];
    TH1I* hdT[fNStrMax + 1];
    TH1I *hHitByCh, *hHitPerEv;
    TH2I *hHitLR, *hXY;
    TH1S *hDy_near, *hDtime_near, *hDWidth_near;
    TH1S *hDy_acros, *hDtime_acros, *hDWidth_acros;
    TH2S *hTempDtimeDy_near, *hTempDtimeDy_acros;

    TGraphErrors* gSlew[fNStrMax];
    TF1 *funT0[fNStrMax], *funRPC[fNStrMax];

    void FillHist();
    Double_t CalculateDt(Int_t Str);
    Bool_t GetCrossPoint(Int_t NStrip, Double_t tL, Double_t tR);
    void AddHit(Int_t Str, TClonesArray* TofHit);
    void AddConteiner(Int_t Str, TClonesArray* TofHit);
    Int_t FormIndex(Int_t IndL, Int_t IndR);

  public:
    BmnTOF1Detector();

    BmnTOF1Detector(
        Int_t tofId,
        Int_t NPlane,
        Int_t FillHistLevel,
        Int_t Verbose);   // FillHistLevel=0-don"t fill, FillHistLevel=1-fill statistic, FillHistLevel>1-fill all

    virtual ~BmnTOF1Detector() {};

    void Clear();
    Bool_t SetDigitNew(BmnTof1Digit* TofDigit, Int_t ind = -1);
    void KillStrip(Int_t NumberOfStrip);
    void KillSide(Int_t NumberOfSide);
    void SetStripLength(Double_t l)
    {
        fStripLength = l;
        fMaxDelta = (fStripLength * 0.5 + 3.0) * fSignalVelosity;
    };
    Int_t FindHitsNew(BmnTrigDigit* T0, TClonesArray* TofHit, int printnaw);
    TList* GetList(Int_t n);
    TString GetName();
    Bool_t SetCorrLR(TString NameFile);
    Bool_t SetCorrSlewing(TString NameFile);
    Bool_t SetCorrTimeShift(TString NameFile);
    Bool_t SetGeoFile(TString NameFile);
    Bool_t SetGeo(BmnTof1GeoUtils* pGeoUtils);
    Bool_t SetSpeedOfSignal(TString NameFile);
    Bool_t GetXYZTime(Int_t Str, TVector3* XYZ, Double_t* ToF);
    Bool_t GetLRTime(Int_t Str, Double_t* LMinusRTime);
    Double_t GetWidth(Int_t Str);
    Double_t GetWidthL(Int_t Str);
    Double_t GetWidthR(Int_t Str);
    Double_t GetTime(Int_t Str);
    Bool_t SaveHistToFile(TString NameFile);

    Int_t GetFillHistLevel() { return fFillHist; };

    ClassDef(BmnTOF1Detector, 4);
};

#endif
