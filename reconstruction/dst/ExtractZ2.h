#ifndef EXTRACTZ2_H
#define EXTRACTZ2_H 1

#include "BmnTrigWaveDigit.h"
#include "BmnTrigDigit.h"
#include "TClonesArray.h"

// Define some constants for analysis
//const double pedBC1 = 69.2885;
//const double pedBC2 = -11.7212;
//const double pedBC3 = -25.4808;
//const double pedBC4 = 126.067;
//const double a_out = 0.00173144;
//const double b_out = 0.0384856;
//const double c_out = 0.000015362;
//const double a_in = 0.020542;
//const double b_in = 0.0305108;
//const double c_in = 0.0000114953;

// Define constants for PID
const double meanBC1_Cin = 2300;
const double meanBC2_Cin = 2500;
const double sigmaBC1_Cin = 350;
const double sigmaBC2_Cin = 450;
const double sigmaMultiBC1_Cin = 2.0;
const double sigmaMultiBC2_Cin = 2.0;
const double meanBC3_Cout = 4600;
const double meanBC4_Cout = 4600;
const double meanBC5_Cout = 4600;
const double sigmaBC3_Cout = 300;
const double sigmaBC4_Cout = 300;
const double sigmaBC5_Cout = 300;
const double sigmaMultiBC3_Cout = 2.0;
const double sigmaMultiBC4_Cout = 2.0;
const double sigmaMultiBC5_Cout = 2.0;
const double meanBC3_Bout = 3350;
const double meanBC4_Bout = 3350;
const double meanBC5_Bout = 3350;
const double sigmaBC3_Bout = 230;
const double sigmaBC4_Bout = 230;
const double sigmaBC5_Bout = 230;
const double sigmaMultiBC3_Bout = 2.0;
const double sigmaMultiBC4_Bout = 2.0;
const double sigmaMultiBC5_Bout = 2.0;
//Be out
const double meanBC3_Beout = 2283;
const double meanBC4_Beout = 2242;
const double meanBC5_Beout = 2206;
const double sigmaBC3_Beout = 272;
const double sigmaBC4_Beout = 286;
const double sigmaBC5_Beout = 235;
const double sigmaMultiBC3_Beout = 1.0;
const double sigmaMultiBC4_Beout = 1.0;
const double sigmaMultiBC5_Beout = 1.0;
//Li out
const double meanBC3_Liout = 1295;
const double meanBC4_Liout = 1283;
const double meanBC5_Liout = 1211;
const double sigmaBC3_Liout = 206;
const double sigmaBC4_Liout = 194;
const double sigmaBC5_Liout = 206;
const double sigmaMultiBC3_Liout = 1.0;
const double sigmaMultiBC4_Liout = 1.0;
const double sigmaMultiBC5_Liout = 1.0;

// const double meanBC3_Liout = 12470;//1288
// const double meanBC4_Liout = 11960;//1253
// const double meanBC5_Liout = 1211;
// const double sigmaBC3_Liout = 1660;//169
// const double sigmaBC4_Liout = 1590;//176
// const double sigmaBC5_Liout = 206;
// const double sigmaMultiBC3_Liout = 1.0;
// const double sigmaMultiBC4_Liout = 1.0;
// const double sigmaMultiBC5_Liout = 1.0;

const double meanBC3S_Cout = 5600;
const double meanBC4S_Cout = 5400;
const double meanBC5S_Cout = 5750;
const double sigmaBC3S_Cout = 400;
const double sigmaBC4S_Cout = 350;
const double sigmaBC5S_Cout = 330;
const double sigmaMultiBC3S_Cout = 2.0;
const double sigmaMultiBC4S_Cout = 2.0;
const double sigmaMultiBC5S_Cout = 2.0;
const double meanBC3S_Bout = 4000;
const double meanBC4S_Bout = 4100;
const double meanBC5S_Bout = 410;
const double sigmaBC3S_Bout = 250;
const double sigmaBC4S_Bout = 250;
const double sigmaBC5S_Bout = 250;
const double sigmaMultiBC3S_Bout = 2.0;
const double sigmaMultiBC4S_Bout = 2.0;
const double sigmaMultiBC5S_Bout = 2.0;

void findIdx(TClonesArray *data, int &index, double refT) {
    // Function to find the waveform that is closest in time to T0 waveform
    // and returns the index where the closest waveform is in the data array.
    double minT = 1e4;
    for (int m = 0; m < data->GetEntriesFast(); m++) {
        BmnTrigWaveDigit *signal = (BmnTrigWaveDigit *)data->At(m);
        double time = fabs(signal->GetTime() - refT);
        if (time < minT) {
            minT = time;
            index = m;
        }
    }
}

void grabZ2(TClonesArray *TQDC_BC1_1, TClonesArray *TQDC_BC1_2, TClonesArray *TQDC_BC2_1, TClonesArray *TQDC_BC2_2, double t0Time1, double t0Time2, double &x1, double &x2, double BC1calib, double BC2calib, short &Z, int pair) {
    double adcBC1_1, adcBC1_2, adcBC2_1, adcBC2_2;
    int bc1_1Idx, bc1_2Idx, bc2_1Idx, bc2_2Idx;

    // Require that BC1 and T0 had TQDC digits
    if (TQDC_BC1_1->GetEntriesFast() && TQDC_BC1_1->GetEntriesFast())  {
        // Find the waveform closest in time to T0 time
        findIdx(TQDC_BC1_1, bc1_1Idx, t0Time1);
        findIdx(TQDC_BC1_2, bc1_2Idx, t0Time2);
        
        
        BmnTrigWaveDigit *signal1_1 = (BmnTrigWaveDigit *)TQDC_BC1_1->At(bc1_1Idx);
        adcBC1_1 = signal1_1->GetPeak(); // - pedBC1 + BC1calib;
  
        BmnTrigWaveDigit *signal1_2 = (BmnTrigWaveDigit *)TQDC_BC1_2->At(bc1_2Idx);
        adcBC1_2 = signal1_2->GetPeak(); // - pedBC1 + BC1calib;
        
       

        // Take geometric mean adc
        x1 = sqrt(adcBC1_1 * adcBC1_2);
     }   
     if (TQDC_BC2_1->GetEntriesFast() && TQDC_BC2_1->GetEntriesFast()) {  
        findIdx(TQDC_BC2_1, bc2_1Idx, t0Time1);
        findIdx(TQDC_BC2_2, bc2_2Idx, t0Time2);
        
        BmnTrigWaveDigit *signal2_1 = (BmnTrigWaveDigit *)TQDC_BC2_1->At(bc2_1Idx);
        adcBC2_1 = signal2_1->GetPeak(); // - pedBC1 + BC1calib;
        
        BmnTrigWaveDigit *signal2_2 = (BmnTrigWaveDigit *)TQDC_BC2_2->At(bc2_2Idx);
        adcBC2_2 = signal2_2->GetPeak(); // - pedBC1 + BC1calib;
   
        x2 = sqrt(adcBC2_1 * adcBC2_2);
    }
        // Convert to Z2:
        // Ask if this is for before target or after target
        // because the calibration constants are different for the
        // different PMTs
        
    if (TQDC_BC1_1->GetEntriesFast() && TQDC_BC1_1->GetEntriesFast() && TQDC_BC2_1->GetEntriesFast() && TQDC_BC2_1->GetEntriesFast())  {
        if (pair == 12) {  
            // Ellipse sigmaMulti cut for Carbon on 2D plot
            if (x2 > meanBC2_Cin - sigmaMultiBC2_Cin * sigmaBC2_Cin * sqrt(1 - (x1 - meanBC1_Cin) * (x1 - meanBC1_Cin) / (sigmaMultiBC1_Cin * sigmaBC1_Cin * sigmaMultiBC1_Cin * sigmaBC1_Cin)) && x2 < meanBC2_Cin + sigmaMultiBC2_Cin * sigmaBC2_Cin * sqrt(1 - (x1 - meanBC1_Cin) * (x1 - meanBC1_Cin) / (sigmaMultiBC1_Cin * sigmaBC1_Cin * sigmaMultiBC1_Cin * sigmaBC1_Cin)))
                Z = 6;
        }
        
        if (pair == 34) {  
            // Ellipse sigmaMulti cut for Carbon on 2D plot
            if (x2 > meanBC4_Cout - sigmaMultiBC4_Cout * sigmaBC4_Cout * sqrt(1 - (x1 - meanBC3_Cout) * (x1 - meanBC3_Cout) / (sigmaMultiBC3_Cout * sigmaBC3_Cout * sigmaMultiBC3_Cout * sigmaBC3_Cout)) && x2 < meanBC4_Cout + sigmaMultiBC4_Cout * sigmaBC4_Cout * sqrt(1 - (x1 - meanBC3_Cout) * (x1 - meanBC3_Cout) / (sigmaMultiBC3_Cout * sigmaBC3_Cout * sigmaMultiBC3_Cout * sigmaBC3_Cout)))
                Z = 6;
                
            if (x2 > meanBC4_Bout - sigmaMultiBC4_Bout * sigmaBC4_Bout * sqrt(1 - (x1 - meanBC3_Bout) * (x1 - meanBC3_Bout) / (sigmaMultiBC3_Bout * sigmaBC3_Bout * sigmaMultiBC3_Bout * sigmaBC3_Bout)) && x2 < meanBC4_Bout + sigmaMultiBC4_Bout * sigmaBC4_Bout * sqrt(1 - (x1 - meanBC3_Bout) * (x1 - meanBC3_Bout) / (sigmaMultiBC3_Bout * sigmaBC3_Bout * sigmaMultiBC3_Bout * sigmaBC3_Bout)))
                Z = 5;

            if (x2 > meanBC4_Beout - sigmaMultiBC4_Beout * sigmaBC4_Beout * sqrt(1 - (x1 - meanBC3_Beout) * (x1 - meanBC3_Beout) / (sigmaMultiBC3_Beout * sigmaBC3_Beout * sigmaMultiBC3_Beout * sigmaBC3_Beout)) && x2 < meanBC4_Beout + sigmaMultiBC4_Beout * sigmaBC4_Beout * sqrt(1 - (x1 - meanBC3_Beout) * (x1 - meanBC3_Beout) / (sigmaMultiBC3_Beout * sigmaBC3_Beout * sigmaMultiBC3_Beout * sigmaBC3_Beout)))
                Z = 4;

            if (x2 > meanBC4_Liout - sigmaMultiBC4_Liout * sigmaBC4_Liout * sqrt(1 - (x1 - meanBC3_Liout) * (x1 - meanBC3_Liout) / (sigmaMultiBC3_Liout * sigmaBC3_Liout * sigmaMultiBC3_Liout * sigmaBC3_Liout)) && x2 < meanBC4_Liout + sigmaMultiBC4_Liout * sigmaBC4_Liout * sqrt(1 - (x1 - meanBC3_Liout) * (x1 - meanBC3_Liout) / (sigmaMultiBC3_Liout * sigmaBC3_Liout * sigmaMultiBC3_Liout * sigmaBC3_Liout)))
                Z = 3;
        }
        
        
        if (pair == 35) {  
            // Ellipse sigmaMulti cut for Carbon on 2D plot
            if (x2 > meanBC5_Cout - sigmaMultiBC5_Cout * sigmaBC5_Cout * sqrt(1 - (x1 - meanBC3_Cout) * (x1 - meanBC3_Cout) / (sigmaMultiBC3_Cout * sigmaBC3_Cout * sigmaMultiBC3_Cout * sigmaBC3_Cout)) && x2 < meanBC5_Cout + sigmaMultiBC5_Cout * sigmaBC5_Cout * sqrt(1 - (x1 - meanBC3_Cout) * (x1 - meanBC3_Cout) / (sigmaMultiBC3_Cout * sigmaBC3_Cout * sigmaMultiBC3_Cout * sigmaBC3_Cout)))
                Z = 6;
                
            if (x2 > meanBC5_Bout - sigmaMultiBC5_Bout * sigmaBC5_Bout * sqrt(1 - (x1 - meanBC3_Bout) * (x1 - meanBC3_Bout) / (sigmaMultiBC3_Bout * sigmaBC3_Bout * sigmaMultiBC3_Bout * sigmaBC3_Bout)) && x2 < meanBC5_Bout + sigmaMultiBC5_Bout * sigmaBC5_Bout * sqrt(1 - (x1 - meanBC3_Bout) * (x1 - meanBC3_Bout) / (sigmaMultiBC3_Bout * sigmaBC3_Bout * sigmaMultiBC3_Bout * sigmaBC3_Bout)))
                Z = 5;

            if (x2 > meanBC5_Beout - sigmaMultiBC5_Beout * sigmaBC5_Beout * sqrt(1 - (x1 - meanBC3_Beout) * (x1 - meanBC3_Beout) / (sigmaMultiBC3_Beout * sigmaBC3_Beout * sigmaMultiBC3_Beout * sigmaBC3_Beout)) && x2 < meanBC5_Beout + sigmaMultiBC5_Beout * sigmaBC5_Beout * sqrt(1 - (x1 - meanBC3_Beout) * (x1 - meanBC3_Beout) / (sigmaMultiBC3_Beout * sigmaBC3_Beout * sigmaMultiBC3_Beout * sigmaBC3_Beout)))
                Z = 4;

            if (x2 > meanBC5_Liout - sigmaMultiBC5_Liout * sigmaBC5_Liout * sqrt(1 - (x1 - meanBC3_Liout) * (x1 - meanBC3_Liout) / (sigmaMultiBC3_Liout * sigmaBC3_Liout * sigmaMultiBC3_Liout * sigmaBC3_Liout)) && x2 < meanBC5_Liout + sigmaMultiBC5_Liout * sigmaBC5_Liout * sqrt(1 - (x1 - meanBC3_Liout) * (x1 - meanBC3_Liout) / (sigmaMultiBC3_Liout * sigmaBC3_Liout * sigmaMultiBC3_Liout * sigmaBC3_Liout)))
                Z = 3;
        }
        
        
        if (pair == 45) {  
            // Ellipse sigmaMulti cut for Carbon on 2D plot
            if (x2 > meanBC5_Cout - sigmaMultiBC5_Cout * sigmaBC5_Cout * sqrt(1 - (x1 - meanBC4_Cout) * (x1 - meanBC4_Cout) / (sigmaMultiBC4_Cout * sigmaBC4_Cout * sigmaMultiBC4_Cout * sigmaBC4_Cout)) && x2 < meanBC5_Cout + sigmaMultiBC5_Cout * sigmaBC5_Cout * sqrt(1 - (x1 - meanBC4_Cout) * (x1 - meanBC4_Cout) / (sigmaMultiBC4_Cout * sigmaBC4_Cout * sigmaMultiBC4_Cout * sigmaBC4_Cout)))
                Z = 6;
                
            if (x2 > meanBC5_Bout - sigmaMultiBC5_Bout * sigmaBC5_Bout * sqrt(1 - (x1 - meanBC4_Bout) * (x1 - meanBC4_Bout) / (sigmaMultiBC4_Bout * sigmaBC4_Bout * sigmaMultiBC4_Bout * sigmaBC4_Bout)) && x2 < meanBC5_Bout + sigmaMultiBC5_Bout * sigmaBC5_Bout * sqrt(1 - (x1 - meanBC4_Bout) * (x1 - meanBC4_Bout) / (sigmaMultiBC4_Bout * sigmaBC4_Bout * sigmaMultiBC4_Bout * sigmaBC4_Bout)))
                Z = 5;

            if (x2 > meanBC5_Beout - sigmaMultiBC5_Beout * sigmaBC5_Beout * sqrt(1 - (x1 - meanBC4_Beout) * (x1 - meanBC4_Beout) / (sigmaMultiBC4_Beout * sigmaBC4_Beout * sigmaMultiBC4_Beout * sigmaBC4_Beout)) && x2 < meanBC5_Beout + sigmaMultiBC5_Beout * sigmaBC5_Beout * sqrt(1 - (x1 - meanBC4_Beout) * (x1 - meanBC4_Beout) / (sigmaMultiBC4_Beout * sigmaBC4_Beout * sigmaMultiBC4_Beout * sigmaBC4_Beout)))
                Z = 4;

            if (x2 > meanBC5_Liout - sigmaMultiBC5_Liout * sigmaBC5_Liout * sqrt(1 - (x1 - meanBC4_Liout) * (x1 - meanBC4_Liout) / (sigmaMultiBC4_Liout * sigmaBC4_Liout * sigmaMultiBC4_Liout * sigmaBC4_Liout)) && x2 < meanBC5_Liout + sigmaMultiBC5_Liout * sigmaBC5_Liout * sqrt(1 - (x1 - meanBC4_Liout) * (x1 - meanBC4_Liout) / (sigmaMultiBC4_Liout * sigmaBC4_Liout * sigmaMultiBC4_Liout * sigmaBC4_Liout)))
                Z = 3;
        }
    }
   
}


void grabZ2S(TClonesArray *TQDC_BC1_S, TClonesArray *TQDC_BC2_S, double t0Time1, double t0Time2, double &x1, double &x2, double BC1calib, double BC2calib, short &Z, int pair) {
    //double adcBC1_1, adcBC1_2, adcBC2_1, adcBC2_2;
    int bc1_SIdx, bc2_SIdx;

    // Require that BC1 and T0 had TQDC digits
    if (TQDC_BC1_S->GetEntriesFast()) {
        // Find the waveform closest in time to T0 time
        findIdx(TQDC_BC1_S, bc1_SIdx, t0Time1); 
        
   
        
        BmnTrigWaveDigit *signal1_S = (BmnTrigWaveDigit *)TQDC_BC1_S->At(bc1_SIdx);
        x1 = signal1_S->GetPeak(); // - pedBC1 + BC1calib;
        
    }
     if (TQDC_BC2_S->GetEntriesFast()) {    
        findIdx(TQDC_BC2_S, bc2_SIdx, t0Time1);  
        BmnTrigWaveDigit *signal2_S = (BmnTrigWaveDigit *)TQDC_BC2_S->At(bc2_SIdx);
        x2 = signal2_S->GetPeak(); // - pedBC1 + BC1calib;
    } 
        // Convert to Z2:
        // Ask if this is for before target or after target
        // because the calibration constants are different for the
        // different PMTs
        
        if (pair == 34) {  
            // Ellipse sigmaMulti cut for Carbon on 2D plot
            if (x2 > meanBC4S_Cout - sigmaMultiBC4S_Cout * sigmaBC4S_Cout * sqrt(1 - (x1 - meanBC3S_Cout) * (x1 - meanBC3S_Cout) / (sigmaMultiBC3S_Cout * sigmaBC3S_Cout * sigmaMultiBC3S_Cout * sigmaBC3S_Cout)) && x2 < meanBC4S_Cout + sigmaMultiBC4S_Cout * sigmaBC4S_Cout * sqrt(1 - (x1 - meanBC3S_Cout) * (x1 - meanBC3S_Cout) / (sigmaMultiBC3S_Cout * sigmaBC3S_Cout * sigmaMultiBC3S_Cout * sigmaBC3S_Cout)))
                Z = 6;
                
            if (x2 > meanBC4S_Bout - sigmaMultiBC4S_Bout * sigmaBC4S_Bout * sqrt(1 - (x1 - meanBC3S_Bout) * (x1 - meanBC3S_Bout) / (sigmaMultiBC3S_Bout * sigmaBC3S_Bout * sigmaMultiBC3S_Bout * sigmaBC3S_Bout)) && x2 < meanBC4S_Bout + sigmaMultiBC4S_Bout * sigmaBC4S_Bout * sqrt(1 - (x1 - meanBC3S_Bout) * (x1 - meanBC3S_Bout) / (sigmaMultiBC3S_Bout * sigmaBC3S_Bout * sigmaMultiBC3S_Bout * sigmaBC3S_Bout)))
                Z = 5;
        }
        
        
        if (pair == 35) {  
            // Ellipse sigmaMulti cut for Carbon on 2D plot
            if (x2 > meanBC5S_Cout - sigmaMultiBC5S_Cout * sigmaBC5S_Cout * sqrt(1 - (x1 - meanBC3S_Cout) * (x1 - meanBC3S_Cout) / (sigmaMultiBC3S_Cout * sigmaBC3S_Cout * sigmaMultiBC3S_Cout * sigmaBC3S_Cout)) && x2 < meanBC5S_Cout + sigmaMultiBC5S_Cout * sigmaBC5S_Cout * sqrt(1 - (x1 - meanBC3S_Cout) * (x1 - meanBC3S_Cout) / (sigmaMultiBC3S_Cout * sigmaBC3S_Cout * sigmaMultiBC3S_Cout * sigmaBC3S_Cout)))
                Z = 6;
                
            if (x2 > meanBC5S_Bout - sigmaMultiBC5S_Bout * sigmaBC5S_Bout * sqrt(1 - (x1 - meanBC3S_Bout) * (x1 - meanBC3S_Bout) / (sigmaMultiBC3S_Bout * sigmaBC3S_Bout * sigmaMultiBC3S_Bout * sigmaBC3S_Bout)) && x2 < meanBC5S_Bout + sigmaMultiBC5S_Bout * sigmaBC5S_Bout * sqrt(1 - (x1 - meanBC3S_Bout) * (x1 - meanBC3S_Bout) / (sigmaMultiBC3S_Bout * sigmaBC3S_Bout * sigmaMultiBC3S_Bout * sigmaBC3S_Bout)))
                Z = 5;
        }
        
        
        if (pair == 45) {  
            // Ellipse sigmaMulti cut for Carbon on 2D plot
            if (x2 > meanBC5S_Cout - sigmaMultiBC5S_Cout * sigmaBC5S_Cout * sqrt(1 - (x1 - meanBC4S_Cout) * (x1 - meanBC4S_Cout) / (sigmaMultiBC4S_Cout * sigmaBC4S_Cout * sigmaMultiBC4S_Cout * sigmaBC4S_Cout)) && x2 < meanBC5S_Cout + sigmaMultiBC5S_Cout * sigmaBC5S_Cout * sqrt(1 - (x1 - meanBC4S_Cout) * (x1 - meanBC4S_Cout) / (sigmaMultiBC4S_Cout * sigmaBC4S_Cout * sigmaMultiBC4S_Cout * sigmaBC4S_Cout)))
                Z = 6;
                
            if (x2 > meanBC5S_Bout - sigmaMultiBC5S_Bout * sigmaBC5S_Bout * sqrt(1 - (x1 - meanBC4S_Bout) * (x1 - meanBC4S_Bout) / (sigmaMultiBC4S_Bout * sigmaBC4S_Bout * sigmaMultiBC4S_Bout * sigmaBC4S_Bout)) && x2 < meanBC5S_Bout + sigmaMultiBC5S_Bout * sigmaBC5S_Bout * sqrt(1 - (x1 - meanBC4S_Bout) * (x1 - meanBC4S_Bout) / (sigmaMultiBC4S_Bout * sigmaBC4S_Bout * sigmaMultiBC4S_Bout * sigmaBC4S_Bout)))
                Z = 5;
        }

}


void grabZ2V(TClonesArray *TQDC_BC1_V, double t0Time1, double &x1) {
    //double adcBC1_1, adcBC1_2, adcBC2_1, adcBC2_2;
    int bc1_SIdx;

    // Require that BC1 and T0 had TQDC digits
    if (TQDC_BC1_V->GetEntriesFast()) {
        // Find the waveform closest in time to T0 time
        findIdx(TQDC_BC1_V, bc1_SIdx, t0Time1); 
        
   
        
        BmnTrigWaveDigit *signal1_S = (BmnTrigWaveDigit *)TQDC_BC1_V->At(bc1_SIdx);
        x1 = signal1_S->GetTime(); // - pedBC1 + BC1calib;
        
    }
    
        // Convert to Z2:
        // Ask if this is for before target or after target
        // because the calibration constants are different for the
        // different PMTs
  
}

#endif