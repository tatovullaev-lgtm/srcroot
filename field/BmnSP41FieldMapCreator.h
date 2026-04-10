#ifndef BMNSP41FIELDMAPCREATOR_H
#define BMNSP41FIELDMAPCREATOR_H 1

#include "BmnFieldMap.h"

#include <TFile.h>
#include <TClonesArray.h>
#include <TF1.h>
#include <TH1.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TString.h>
#include <TMath.h>

#include <vector>
#include <utility>

using namespace TMath;

class fitParMonitor : public TObject
{
  public:
    fitParMonitor() {}
    fitParMonitor(TString out);
    virtual ~fitParMonitor();

    TH1F*** C;
    TH1F*** M;
    TH1F*** S;

  private:
    TFile* f;

  ClassDef(fitParMonitor, 1)
};


class extrapolationConditions : public TObject
{
  public:
    extrapolationConditions()
    {
        direction = "Z";
        min = -DBL_MAX;
        max = DBL_MAX;
    }

    virtual ~extrapolationConditions() {}

    TString direction;
    Double_t min;
    Double_t max;
    std::pair <Double_t, Double_t> tailRangeLeft;
    std::pair <Double_t, Double_t> tailRangeRight;

  ClassDef(extrapolationConditions, 1)
};


class smoothedValues : public TObject
{
  public:
    smoothedValues()
    {
        X = 0.;
        Y = 0.;
        Z = 0.;
        fieldValue = 0.;
    }

    virtual ~smoothedValues() {}

    Double_t X;
    Double_t Y;
    Double_t Z;
    Double_t fieldValue;

  ClassDef(smoothedValues, 1)
};


class BmnSP41FieldMapCreator : public TNamed
{
  public:
    BmnSP41FieldMapCreator() {}
    BmnSP41FieldMapCreator(TString, TString);
    virtual ~BmnSP41FieldMapCreator();

    void CreateExtrapolatedMap();
    void SmoothMap(TClonesArray*);

    void SetOutParamFileName(TString name) {
        fParamFileName = name;
    }

    void SetDebug(Bool_t flag) {
        fDebug = flag;
    }

    void SetNodeNumber(Int_t nNodes) {
        dimGrid = nNodes;
    }

    // Extrapolation new ranges ...

    void SetNewRanges(Double_t min, Double_t max) {
        fConditions->min = min;
        fConditions->max = max;
    }

    void SetExtrapolationAxis(TString axis) {
        fConditions->direction = axis;
    }

    // Main method to be called ...
    void Extrapolation();

  private:
    Bool_t fDebug;

    TString nameOutput;

    Int_t dimGrid; // dimGrid x dimGrid corresponds to number of nodes to build a grid along X(Y)-axis 
    Int_t dimNhist; // dimNhist corresponds to number of profiles drawn

    Double_t factor; // Transition from T to kG

    BmnFieldMap* fMap;
    extrapolationConditions* fConditions;
    fitParMonitor* fMonitor;

    Double_t WinXmin;
    Double_t WinXmax;
    Double_t WinYmin;
    Double_t WinYmax;
    Double_t WinZmin;
    Double_t WinZmax;
    Double_t winXstep;
    Double_t winYstep;
    Double_t winZstep;

    TGraph2D*** constant;
    TGraph2D*** mean;
    TGraph2D*** sigma;

    TString fParamFileName;

    void DrawProfileExtrapZ(Int_t, TGraph***, TF1***, TString, TString, Double_t, Double_t, Double_t, Double_t);
    void DrawProfileExtrapX(Int_t, TGraph***, TF1***, TString, TString, Double_t, Double_t, Double_t, Double_t);
    Double_t FieldExtrapolate(Double_t, Double_t, Double_t, TGraph2D*, TGraph2D*, TGraph2D*);
    void Print(BmnFieldMap*);

    void DoExtrapolationAlongZ(Int_t, Int_t);
    void DoExtrapolationAlongX(Int_t, Int_t);

    std::vector <Double_t> checkedFieldValue(Double_t bx, Double_t by, Double_t bz) {
        // To be not more than 5kG at scale = 1
        const Double_t min = 0.001;
        const Double_t max = 7.;

        std::vector <Double_t> out;

        out.push_back((TMath::Abs(bx) > min && TMath::Abs(bx) < max) ? bx : 0.);
        out.push_back((TMath::Abs(by) > min && TMath::Abs(by) < max) ? by : 0.);
        out.push_back((TMath::Abs(bz) > min && TMath::Abs(bz) < max) ? bz : 0.);

        return out;
    }

  ClassDef(BmnSP41FieldMapCreator, 1)
};

#endif
