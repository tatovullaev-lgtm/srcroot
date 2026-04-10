#ifndef BMNCALIBDATA_H
#define	BMNCALIBDATA_H

#include <vector>

#include "TNamed.h"

using std::vector;
using std::move;

class BmnCalibData : public TNamed
{
    typedef vector<vector<vector<bool> > > BitVecADC;
    typedef vector<vector<vector<Float_t> > > FloatVecADC;
  public:
    /** Default constructor **/
    BmnCalibData();

    /** Main constructor **/
    BmnCalibData(FloatVecADC calib, BitVecADC noise);
//    BmnCalibData(Float_t*** calib, bool*** noise);

    /** Destructor **/
    virtual ~BmnCalibData() {}

    void SetCalibration(FloatVecADC v) { calib_obj = v;}
    void SetNoise(BitVecADC v) { noise_obj = v; }

//    void SetCalibration(FloatVecADC &&v) { calib_obj = move(v);}
//    void SetNoise(BitVecADC &&v) { noise_obj = move(v); }
    
    FloatVecADC& GetCalibration() { return calib_obj; }
    BitVecADC& GetNoise() { return noise_obj; }

  private:
    FloatVecADC  calib_obj;
    BitVecADC  noise_obj;

  ClassDef(BmnCalibData, 1);
};

#endif	/* BMNCALIBDATA_H */

