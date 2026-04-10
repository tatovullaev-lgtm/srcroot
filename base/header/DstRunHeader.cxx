#include "DstRunHeader.h"

// -----   Default constructor   ------------------------------------------
DstRunHeader::DstRunHeader()
: TNamed("DstRunHeader",""),
  fPeriodNumber(0),
  fRunNumber(0),
  fStartTime(TTimeStamp()),
  fFinishTime(TTimeStamp()),
  fBeamA(-1),
  fBeamZ(-1),
  fBeamEnergy(0),
  fTargetA(-1),
  fTargetZ(-1),
  fFieldType(-1),
  fFieldScale(-1),
  fFieldName(""),
  fGeometryName("")
{}

DstRunHeader::DstRunHeader(UInt_t period_number, UInt_t run_number, TTimeStamp start_time, TTimeStamp end_time)
: TNamed("DstRunHeader",""),
  fPeriodNumber(period_number),
  fRunNumber(run_number),
  fStartTime(start_time),
  fFinishTime(end_time),
  fBeamA(-1),
  fBeamZ(-1),
  fBeamEnergy(0),
  fTargetA(-1),
  fTargetZ(-1),
  fFieldType(-1),
  fFieldScale(-1),
  fFieldName(""),
  fGeometryName("")
{}

DstRunHeader::DstRunHeader(UInt_t period_number, UInt_t run_number, TTimeStamp start_time, TTimeStamp end_time,
                           Int_t beam_a, Int_t beam_z, Float_t beam_energy, Int_t target_a, Int_t target_z,
                           Int_t field_type, Double_t field_scale, TString field_name, TString geo_name)
: TNamed("DstRunHeader",""),
  fPeriodNumber(period_number),
  fRunNumber(run_number),
  fStartTime(start_time),
  fFinishTime(end_time),
  fBeamA(beam_a),
  fBeamZ(beam_z),
  fBeamEnergy(beam_energy),
  fTargetA(target_a),
  fTargetZ(target_z),
  fFieldType(field_type),
  fFieldScale(field_scale),
  fFieldName(field_name),
  fGeometryName(geo_name)
{}

// -----   Destructor   ----------------------------------------------------
DstRunHeader::~DstRunHeader()
{}
