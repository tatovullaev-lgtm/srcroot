/** @file BmnNdetCluster.cxx
 ** @author Arseniy Shabanov <arseniy.shabanov@phystech.edu>
 ** @date 19.01.2024
 **
 ** Code of Class for description of recognized cluster in Bmn Ndet detector
 **/

#include "BmnNdetCluster.h"

#include "TMath.h"

BmnNdetCluster::BmnNdetCluster()
    : TObject()
{
    fNCells = 0;
}

BmnNdetCluster::~BmnNdetCluster() {}

BmnNdetCluster::BmnNdetCluster(size_t size)
    : TObject()
{
    fCell.reserve(size);
    fColumn.reserve(size);
    fRow.reserve(size);
    fLayer.reserve(size);
    fXcoord.reserve(size);
    fYcoord.reserve(size);
    fZcoord.reserve(size);
    fTime.reserve(size);
    fEdep.reserve(size);
    fBeta.reserve(size);
    fNCells = 0;
}

void BmnNdetCluster::AddCell(Int_t cell,
                             Int_t column,
                             Int_t row,
                             Int_t layer,
                             Double_t x,
                             Double_t y,
                             Double_t z,
                             Double_t t,
                             Double_t e_dep,
                             Double_t beta)
{
    fCell.push_back(cell);
    fColumn.push_back(column);
    fRow.push_back(row);
    fLayer.push_back(layer);
    fXcoord.push_back(x);
    fYcoord.push_back(y);
    fZcoord.push_back(z);
    fTime.push_back(t);
    fEdep.push_back(e_dep);

    if (beta < 0.) {
        Double_t distance = TMath::Sqrt(TMath::Sq(x) + TMath::Sq(y) + TMath::Sq(z));
        Double_t _beta = t >= 0 ? distance * 1.E-2 / (t * 1E-9) / TMath::C() : -1.;
        fBeta.push_back(_beta);
    } else {
        fBeta.push_back(beta);
    }
    ++fNCells;
}

Int_t BmnNdetCluster::GetNCells()
{
    return fNCells;
}

Int_t BmnNdetCluster::GetNCells(Double_t threshold)
{
    Int_t nCells = 0.;
    for (Int_t iCell = 0; iCell < fNCells; ++iCell) {
        if (fEdep[iCell] > threshold)
            ++nCells;
    }
    return nCells;
}

vector<Int_t> BmnNdetCluster::GetCells()
{
    return fCell;
}
vector<Int_t> BmnNdetCluster::GetColumns()
{
    return fColumn;
}
vector<Int_t> BmnNdetCluster::GetRows()
{
    return fRow;
}
vector<Int_t> BmnNdetCluster::GetLayers()
{
    return fLayer;
}
vector<Double_t> BmnNdetCluster::GetX()
{
    return fXcoord;
}
vector<Double_t> BmnNdetCluster::GetY()
{
    return fYcoord;
}
vector<Double_t> BmnNdetCluster::GetZ()
{
    return fZcoord;
}
vector<Double_t> BmnNdetCluster::GetTime()
{
    return fTime;
}
vector<Double_t> BmnNdetCluster::GetEdep()
{
    return fEdep;
}
vector<Double_t> BmnNdetCluster::GetBeta()
{
    return fBeta;
}

Bool_t BmnNdetCluster::ContainsVetoCells(Int_t veto_layer)
{
    for (Int_t iCell = 0; iCell < fNCells; ++iCell) {
        if (fLayer[iCell] == veto_layer)
            return kTRUE;
    }
    return kFALSE;
}

Bool_t BmnNdetCluster::StartsOnBorder(Int_t size_x, Int_t size_y, Int_t size_z)
{
    Int_t iCell = FirstCell();
    if (fLayer[iCell] == 0)
        return kTRUE;
    if (fLayer[iCell] == size_z - 1)
        return kTRUE;
    if (fColumn[iCell] == 0)
        return kTRUE;
    if (fColumn[iCell] == size_x - 1)
        return kTRUE;
    if (fRow[iCell] == 0)
        return kTRUE;
    if (fRow[iCell] == size_y - 1)
        return kTRUE;

    return kFALSE;
}

Double_t BmnNdetCluster::EnergyDepositedAverage(Double_t threshold)
{
    Double_t edep = 0.;
    Double_t nCells = 0.;
    for (Int_t iCell = 0; iCell < fNCells; ++iCell) {
        if (fEdep[iCell] > threshold) {
            edep += fEdep[iCell];
            nCells += 1.;
        }
    }
    if (nCells > 0.)
        return edep / nCells;
    return 0.;
}

Double_t BmnNdetCluster::EnergyDeposited(Double_t threshold)
{
    Double_t edep = 0.;
    for (Int_t iCell = 0; iCell < fNCells; ++iCell) {
        if (fEdep[iCell] > threshold) {
            edep += fEdep[iCell];
        }
    }
    return edep;
}

Int_t BmnNdetCluster::FirstCell()
{
    Double_t t = 100.;
    Int_t iFirst = 0;
    for (Int_t iCell = 0; iCell < fNCells; ++iCell) {
        if (fTime[iCell] < t) {
            t = fTime[iCell];
            iFirst = iCell;
        }
    }
    return iFirst;
}

Int_t BmnNdetCluster::FastestCell()
{
    Double_t beta = 0.;
    Int_t iFastest = 0;
    for (Int_t iCell = 0; iCell < fNCells; ++iCell) {
        if (fBeta[iCell] > beta) {
            beta = fBeta[iCell];
            iFastest = iCell;
        }
    }
    return iFastest;
}

Double_t BmnNdetCluster::BetaFirst()
{
    Int_t iFirst = FirstCell();
    return fBeta[iFirst];
}

Double_t BmnNdetCluster::BetaFastest()
{
    Int_t iFastest = FastestCell();
    return fBeta[iFastest];
}

Double_t BmnNdetCluster::EnergyTofFirst(Double_t mass)
{
    Double_t beta = BetaFirst();
    if (beta >= 1.)
        return -1.;
    Double_t gamma = 1. / TMath::Sqrt(1. - TMath::Sq(beta));
    return (gamma - 1.) * mass;
}

Double_t BmnNdetCluster::EnergyTofFastest(Double_t mass)
{
    Double_t beta = BetaFastest();
    if (beta >= 1.)
        return -1.;
    Double_t gamma = 1. / TMath::Sqrt(1. - TMath::Sq(beta));
    return (gamma - 1.) * mass;
}

Double_t BmnNdetCluster::EnergyTofAverage(Double_t mass)
{
    Double_t t = 0.;
    Double_t x = 0.;
    Double_t y = 0.;
    Double_t z = 0.;
    for (Int_t iCell = 0; iCell < fNCells; ++iCell) {
        t += fTime[iCell];
        x += fXcoord[iCell];
        y += fYcoord[iCell];
        z += fZcoord[iCell];
    }
    Double_t distance = TMath::Sqrt(TMath::Sq(x) + TMath::Sq(y) + TMath::Sq(z));
    if (t <= 0.)
        return -1.;
    Double_t beta = (distance * 1.E-2 / (t * 1.E-9)) / TMath::C();
    if (beta >= 1.)
        return -1.;
    Double_t gamma = 1. / TMath::Sqrt(1. - TMath::Sq(beta));
    return (gamma - 1.) * mass;
}

Double_t BmnNdetCluster::AngleToTarget()
{
    Int_t iFirst = FirstCell();
    Double_t xTarget = fXcoord[iFirst];
    Double_t yTarget = fYcoord[iFirst];
    Double_t zTarget = fZcoord[iFirst];
    Double_t xCluster = 0;
    Double_t yCluster = 0;
    Double_t zCluster = 0;
    for (Int_t iCell = 0; iCell < fNCells; ++iCell) {
        Double_t x = fXcoord[iCell] - fXcoord[iFirst];
        Double_t y = fYcoord[iCell] - fYcoord[iFirst];
        Double_t z = fZcoord[iCell] - fZcoord[iFirst];

        x *= fEdep[iCell];
        y *= fEdep[iCell];
        z *= fEdep[iCell];

        xCluster += x;
        yCluster += y;
        zCluster += z;
    }
    Double_t scalarProduct = xCluster * xTarget + yCluster * yTarget + zCluster * zTarget;
    Double_t clusterLength = TMath::Sqrt(TMath::Sq(xCluster) + TMath::Sq(yCluster) + TMath::Sq(zCluster));
    Double_t targetLength = TMath::Sqrt(TMath::Sq(xTarget) + TMath::Sq(yTarget) + TMath::Sq(zTarget));
    scalarProduct /= (clusterLength * targetLength);
    return TMath::ACos(scalarProduct) * TMath::RadToDeg();
}

Double_t BmnNdetCluster::AngleToDirection(Double_t x, Double_t y, Double_t z)
{
    Int_t iFirst = FirstCell();
    Double_t xTarget = x;
    Double_t yTarget = y;
    Double_t zTarget = z;
    Double_t xCluster = 0;
    Double_t yCluster = 0;
    Double_t zCluster = 0;
    for (Int_t iCell = 0; iCell < fNCells; ++iCell) {
        Double_t dx = fXcoord[iCell] - fXcoord[iFirst];
        Double_t dy = fYcoord[iCell] - fYcoord[iFirst];
        Double_t dz = fZcoord[iCell] - fZcoord[iFirst];

        dx *= fEdep[iCell];
        dy *= fEdep[iCell];
        dz *= fEdep[iCell];

        xCluster += dx;
        yCluster += dy;
        zCluster += dz;
    }
    Double_t scalarProduct = xCluster * xTarget + yCluster * yTarget + zCluster * zTarget;
    Double_t clusterLength = TMath::Sqrt(TMath::Sq(xCluster) + TMath::Sq(yCluster) + TMath::Sq(zCluster));
    Double_t targetLength = TMath::Sqrt(TMath::Sq(xTarget) + TMath::Sq(yTarget) + TMath::Sq(zTarget));
    scalarProduct /= (clusterLength * targetLength);
    return TMath::ACos(scalarProduct) * TMath::RadToDeg();
}

BmnNdetCluster* BmnNdetCluster::AddCluster(BmnNdetCluster* other)
{

    for (Int_t iCell = 0; iCell < other->GetNCells(); ++iCell) {
        this->AddCell(other->GetCells()[iCell], other->GetColumns()[iCell], other->GetRows()[iCell],
                      other->GetLayers()[iCell], other->GetX()[iCell], other->GetY()[iCell], other->GetZ()[iCell],
                      other->GetTime()[iCell], other->GetEdep()[iCell], other->GetBeta()[iCell]);
    }
    return this;
}

void BmnNdetCluster::Clear()
{
    fCell.clear();
    fColumn.clear();
    fRow.clear();
    fLayer.clear();
    fXcoord.clear();
    fYcoord.clear();
    fZcoord.clear();
    fTime.clear();
    fEdep.clear();
    fBeta.clear();

    fNCells = 0;
}

BmnNdetCluster::BmnNdetCluster(const BmnNdetCluster& other)
    : TObject(other)
{}

BmnNdetCluster::BmnNdetCluster(BmnNdetCluster&& other) noexcept
    : TObject(move(other))
{}

void BmnNdetCluster::Print(const Option_t* option)
{
    cout << "-----------------------------------   BmnNdetCluster   -----------------------------" << endl;
    for (size_t i = 0; i < fEdep.size(); ++i) {
        cout << fCell[i] << " cell (" << fColumn[i] << ", " << fRow[i] << ", " << fLayer[i] << "): ";
        cout << "\tX: " << fXcoord[i] << "[cm], Y: " << fYcoord[i] << "[cm], Z: " << fZcoord[i] << "[cm], ";
        cout << "\tt: " << fTime[i] << "[ns], Edep: " << 1.E3 * fEdep[i] << "[MeV], beta: " << fBeta[i] << endl;
    }
    cout << "------------------------------------------------------------------------------------" << endl;
}
