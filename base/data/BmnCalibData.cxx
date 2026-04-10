#include "BmnCalibData.h"

BmnCalibData::BmnCalibData()
: TNamed() {
}

BmnCalibData::BmnCalibData(FloatVecADC calib, BitVecADC noise)
: TNamed() {
    calib_obj = calib;
    noise_obj = noise;
}