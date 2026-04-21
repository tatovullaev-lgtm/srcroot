/* 
 * File:   BmnScWallReconstructor.cxx
 * Author: Sergey Nepochatykh
 *
 */

#include "BmnScWallReconstructor.h"

BmnScWallReconstructor::BmnScWallReconstructor() {

  fELosses = 0.;
    
}


BmnScWallReconstructor::~BmnScWallReconstructor() {
}


void BmnScWallReconstructor::SetELosses(Double_t ELosses) { 

  fELosses = ELosses; 

}


Double_t BmnScWallReconstructor::GetELosses() {
 
  return fELosses;
 
}


void BmnScWallReconstructor::SetCellId(UInt_t CellId) { 

  fCellId = CellId; 

}


UInt_t BmnScWallReconstructor::GetCellId() {
 
  return fCellId;
 
}


void BmnScWallReconstructor::SetX1(Double_t X1) { 

  fX1 = X1; 

}


Double_t BmnScWallReconstructor::GetX1() {
 
  return fX1;
 
}


void BmnScWallReconstructor::SetX2(Double_t X2) { 

  fX2 = X2; 

}


Double_t BmnScWallReconstructor::GetX2() {
 
  return fX2;
 
}


void BmnScWallReconstructor::SetY1(Double_t Y1) { 

  fY1 = Y1; 

}


Double_t BmnScWallReconstructor::GetY1() {
 
  return fY1;
 
}


void BmnScWallReconstructor::SetY2(Double_t Y2) { 

  fY2 = Y2; 

}


Double_t BmnScWallReconstructor::GetY2() {
 
  return fY2;

}


ClassImp(BmnScWallReconstructor)

