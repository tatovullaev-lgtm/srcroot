#ifndef DSTRUNHEADER_H
#define DSTRUNHEADER_H

#include "BmnEnums.h"
#include "TNamed.h"
#include "TTimeStamp.h"

class DstRunHeader : public TNamed
{
  public:
    /** Default constructor */
    DstRunHeader();

    /** Constructor (short) */
    DstRunHeader(UInt_t period_number, UInt_t run_number, TTimeStamp start_time, TTimeStamp end_time);

    /** Constructor (full) */
    DstRunHeader(UInt_t period_number,
                 UInt_t run_number,
                 TTimeStamp start_time,
                 TTimeStamp end_time,
                 Int_t beam_a,
                 Int_t beam_z,
                 Float_t beam_energy,
                 Int_t target_a,
                 Int_t target_z,
                 Int_t field_type,
                 Double_t field_scale,
                 TString field_name,
                 TString geo_name);

    /** Destructor */
    virtual ~DstRunHeader();

    /** Get period number for this run */
    UInt_t GetPeriodId() { return fPeriodNumber; }
    /** Get run number for this run */
    UInt_t GetRunId() { return fRunNumber; }
    TTimeStamp GetStartTime() { return fStartTime; }
    TTimeStamp GetFinishTime() { return fFinishTime; }
    Int_t GetBeamA() { return fBeamA; }
    Int_t GetBeamZ() { return fBeamZ; }
    Float_t GetBeamEnergy() { return fBeamEnergy; }
    Int_t GetTargetA() { return fTargetA; }
    Int_t GetTargetZ() { return fTargetZ; }
    Int_t GetFieldType() { return fFieldType; }
    Double_t GetFieldScale() { return fFieldScale; }
    TString GetFieldName() { return fFieldName; }
    TString GetGeometryName() { return fGeometryName; }

    /** Set the period number for this run **/
    void SetPeriodId(UInt_t period_number) { fPeriodNumber = period_number; }
    /** Set the run number for this run **/
    void SetRunId(UInt_t run_number) { fRunNumber = run_number; }
    /** Set the period and number numbers for this run **/
    void SetPeriodRun(UInt_t period_number, UInt_t run_number)
    {
        fPeriodNumber = period_number;
        fRunNumber = run_number;
    }
    void SetStartTime(TTimeStamp start_time) { fStartTime = start_time; }
    void SetFinishTime(TTimeStamp end_time) { fFinishTime = end_time; }
    void SetBeamA(Int_t beam_a) { fBeamA = beam_a; }
    void SetBeamZ(Int_t beam_z) { fBeamZ = beam_z; }
    void SetBeamEnergy(Float_t beam_energy) { fBeamEnergy = beam_energy; }
    void SetTargetA(Int_t target_a) { fTargetA = target_a; }
    void SetTargetZ(Int_t target_z) { fTargetZ = target_z; }
    void SetFieldType(Int_t field_type) { fFieldType = field_type; }
    void SetFieldScale(Double_t field_scale) { fFieldScale = field_scale; }
    void SetFieldName(TString field_name) { fFieldName = field_name; }
    void SetGeometryName(TString geo_name) { fGeometryName = geo_name; }

  private:
    /** Period Number **/
    UInt_t fPeriodNumber;
    /** Run Number **/
    UInt_t fRunNumber;
    /** Start time of run **/
    TTimeStamp fStartTime;
    /** Finish time of run **/
    TTimeStamp fFinishTime;

    /** Beam, A (atomic weight) **/
    Int_t fBeamA;
    /** Beam, Z (charge) **/
    Int_t fBeamZ;
    /** Beam energy, GeV **/
    Float_t fBeamEnergy;
    /** Target, A (atomic weight) **/
    Int_t fTargetA;
    /** Target, Z (charge) **/
    Int_t fTargetZ;

    /** Magnetic field type **/
    Int_t fFieldType;
    /** Magnetic field scale **/
    Double_t fFieldScale;
    /** File name with magnetic field **/
    TString fFieldName;
    /** Geometry name **/
    TString fGeometryName;

    ClassDef(DstRunHeader, 3)
};

#endif /* DSTRUNHEADER_H */
