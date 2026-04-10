/**
 * Class BmnStripDigit
 * Base class for strip detector digits
 */

#ifndef BMNSTRIPDIGIT_H
#define BMNSTRIPDIGIT_H

#include "TNamed.h"

#include <iostream>

class BmnStripDigit : public TNamed
{
  public:
    BmnStripDigit();
    BmnStripDigit(BmnStripDigit* digit);
    BmnStripDigit(Int_t iStation,
                  Int_t iModule,
                  Int_t iStripLayer,
                  Int_t iStripNumber,
                  Double_t iStripSignal,
                  Double_t iStripNoise = -1);
    virtual ~BmnStripDigit();

    Int_t GetStation() { return fStation; }
    Int_t GetModule() { return fModule; }
    Int_t GetStripLayer() { return fStripLayer; }
    Int_t GetStripNumber() { return fStripNumber; }
    Double_t GetStripSignal() { return fStripSignal; }
    Double_t GetStripNoise() { return fStripNoise; }
    Bool_t IsGoodDigit() { return fIsGoodDigit; }

    void SetStation(Int_t station) { fStation = station; }
    void SetModule(Int_t module) { fModule = module; }
    void SetStripLayer(Int_t layer) { fStripLayer = layer; }
    void SetStripNumber(Int_t num) { fStripNumber = num; }
    void SetStripSignal(Double_t signal) { fStripSignal = signal; }
    void SetStripNoise(Double_t noise) { fStripNoise = noise; }
    void SetIsGoodDigit(Bool_t tmp) { fIsGoodDigit = tmp; }

    void Print()
    {
        std::cout << "Digi info# " << std::endl;
        std::cout << "Stat# " << std::dec << fStation << std::endl;
        std::cout << "Mod# " << std::dec << fModule << std::endl;
        std::cout << "Layer# " << std::dec << fStripLayer << std::endl;
        std::cout << "Strip# " << std::dec << fStripNumber << std::endl;
        std::cout << "Signal# " << std::dec << fStripSignal << std::endl;
        std::cout << "Noise# " << std::dec << fStripNoise << std::endl;
    }

  protected:
    Int_t fStation;
    Int_t fModule;
    Int_t fStripLayer;
    Int_t fStripNumber;
    Double_t fStripSignal;
    Double_t fStripNoise;
    Bool_t fIsGoodDigit;

    ClassDef(BmnStripDigit, 2);
};

#endif /* BMNSTRIPDIGIT_H */
