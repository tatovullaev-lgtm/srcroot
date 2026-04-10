#ifndef BMNTRIGWAVEDIGIT_H
#define BMNTRIGWAVEDIGIT_H

#include <iostream>
using namespace std;

#include <TNamed.h>
//#include "BmnTrigDigit.h"

class BmnTrigWaveDigit : public TNamed {
public:
    BmnTrigWaveDigit();
    BmnTrigWaveDigit(const BmnTrigWaveDigit&);
    BmnTrigWaveDigit(UShort_t iMod, Short_t *iValue, UInt_t nVals, Double_t trigTimestamp, Double_t adcTimestamp, Double_t time);

    UShort_t GetMod() const {
        return fMod;
    }

    UInt_t GetNSamples() const {
        return fNsmpl;
    }

    int GetIntegral() const {
        int spectra = 0;
        for (Int_t i = 0; i < fNsmpl; ++i)
            spectra += fValueI[i];
        return spectra;
    }

    int GetPeak() const {
        int peak = -100000;
        for (Int_t i = 0; i < fNsmpl; ++i)
            if (fValueI[i] > peak) peak = fValueI[i];
        return peak;
    }

    // int GetIntegral(UInt_t start = 0, UInt_t stop = 1e9) const {
    //     stop = min(stop, fNsmpl);
    //     start = min(start, stop);
    //     int spectra = 0;
    //     for (UInt_t i = start; i < stop; ++i)
    //         spectra += fValueI[i];
    //     return spectra;
    // }

    // int GetPeak(UInt_t start = 0, UInt_t stop = 1e9) const {
    //     stop = min(stop, fNsmpl);
    //     start = min(start, stop);
    //     int peak = -100000;
    //     for (UInt_t i = start; i < stop; ++i)
    //         if (fValueI[i] > peak) peak = fValueI[i];
    //     return peak;
    // }

    Short_t *GetShortValue() const {
        return (Short_t *) fValueI;
    }

    void SetShortValue(Short_t *iValue) const {
        for (UInt_t i = 0; i < fNsmpl; ++i)
            fValueI[i] = iValue[i];
    }

    Double_t GetAdcTimestamp() const {
        return fAdcTimestamp;
    }

    Double_t GetTrigTimestamp() const {
        return fTrigTimestamp;
    }

    Double_t GetTime() const {
        return fTime;
    }
    
    vector<Double_t>& TdcVector(){
        return fTdcTimes;
    }

    virtual ~BmnTrigWaveDigit();
protected:
    UShort_t fMod;
    UInt_t fNsmpl;
    Short_t* fValueI; //[fNsmpl]
    Double_t fTrigTimestamp;
    Double_t fAdcTimestamp;
    Double_t fTime;
    vector<Double_t> fTdcTimes;

    ClassDef(BmnTrigWaveDigit, 4);
};

#endif /* BMNTRIGWAVEDIGIT_H */

