/** @file BmnScWallCell.cxx
 ** @author 
 ** @date 09.09.2022
 **
 ** Code of Class for Bmn ScWall module data container in event 
 **/

#include "BmnScWallCell.h"
#include "BmnDetectorList.h"

#include <iostream>
using namespace std;

// --- Default constructor
BmnScWallCell::BmnScWallCell()
  : FairHit()
  , fCellId{0}
  , fCellSignal{0.}
{
  SetDetectorID(kSCWALL);
}


BmnScWallCell::BmnScWallCell(int CellId, float CellSignal, TVector3 posHit, TVector3 posHitErr, int pointIndx)
  : FairHit{kSCWALL, posHit, posHitErr, pointIndx}
  , fCellId{CellId}
  , fCellSignal{CellSignal}
{
}


// --- Copy constructor
BmnScWallCell::BmnScWallCell(const BmnScWallCell& other)
  : FairHit(other)
  , fCellId(other.fCellId)
  , fCellSignal(other.fCellSignal)
{
}


// --- Move constructor
BmnScWallCell::BmnScWallCell(BmnScWallCell&& other) noexcept
  : FairHit(std::move(other)) // Call the base class move constructor
  , fCellId(std::exchange(other.fCellId, 0))
  , fCellSignal(std::exchange(other.fCellSignal, 0.))
{
}


// --- Assignment operator
BmnScWallCell& BmnScWallCell::operator=(const BmnScWallCell& other)
{
  if (this != &other) {
    FairHit::operator=(other); // Call the base class assignment operator
    fCellId = other.fCellId;
    fCellSignal = other.fCellSignal;
  }
  return *this;
}


// --- Move assignment operator
BmnScWallCell& BmnScWallCell::operator=(BmnScWallCell&& other) noexcept
{
  if (this != &other) {
    FairHit::operator=(std::move(other)); // Call the base class move assignment operator
    fCellId = std::exchange(other.fCellId, 0);
    fCellSignal = std::exchange(other.fCellSignal, 0.);
  }
  return *this;
}


void BmnScWallCell::Print(Option_t *option) const
{
}

void BmnScWallCell::reset() {
  fCellSignal = 0.;
  fCellId = 0;
  TVector3 zero;
  SetPosition(zero);
  SetPositionError(zero);
  SetRefIndex(-1);
}
