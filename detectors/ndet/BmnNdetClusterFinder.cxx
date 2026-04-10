/** @file BmnNdetClusterFinder.cxx
 ** @author Arseniy Shabanov <arseniy.shabanov@phystech.edu>
 ** @date 19.01.2024
 **
 ** Code of Class for algorithm of recognition of clusters in Bmn Ndet detector
 **/
#include "BmnNdetClusterFinder.h"

#include "TMath.h"

#include <iostream>

using namespace std;

BmnNdetClusterFinder::BmnNdetClusterFinder(const Int_t kNColumns, const Int_t kNRows, const Int_t kNLayers)
    : TObject()
    , fClusters()
    , fIndexXYZ(kNColumns, vector<vector<Int_t>>(kNRows, vector<Int_t>(kNLayers, -1)))
{
    fNCells = 0;
    fNClusters = 0;
}

BmnNdetClusterFinder::~BmnNdetClusterFinder()
{
    for (Int_t c = 0; c < GetNColumns(); ++c) {
        for (Int_t r = 0; r < GetNRows(); ++r) {
            fIndexXYZ[c][r].clear();
        }
        fIndexXYZ[c].clear();
    }
    fIndexXYZ.clear();
}

void BmnNdetClusterFinder::Fill(Int_t cell,
                                Int_t column,
                                Int_t row,
                                Int_t layer,
                                Double_t x,
                                Double_t y,
                                Double_t z,
                                Double_t t,
                                Double_t e_dep)
{
    Double_t e2 = e_dep;
    Double_t t2 = t;
    if (t2 <= 0.)
        return;
    if (e2 <= 0.)
        return;
    Int_t index = fIndexXYZ[column][row][layer];
    if (index >= 0) {
        Double_t t1 = fTime[index];
        Double_t e1 = fEdep[index];
        t2 = (t2 > t1 ? t1 : t2);
        e2 += e1;
    }
    if (column < 0 || column >= GetNColumns())
        return;
    if (row < 0 || row >= GetNRows())
        return;
    if (layer < 0 || layer >= GetNLayers())
        return;
    fCell.push_back(cell);
    fColumn.push_back(column);
    fRow.push_back(row);
    fLayer.push_back(layer);
    fXcoord.push_back(x);
    fYcoord.push_back(y);
    fZcoord.push_back(z);
    fTime.push_back(t2);
    fEdep.push_back(e2);

    Double_t distance = TMath::Sqrt(TMath::Sq(x) + TMath::Sq(y) + TMath::Sq(z));
    Double_t beta = (distance * 1.E-2) / (t2 * 1.E-9) / TMath::C();
    fBeta.push_back(beta);
    fIndexTime.push_back(fNCells);
    fIndexEdep.push_back(fNCells);
    fIndexBeta.push_back(fNCells);
    fCellStatus.push_back(kIsNotUsed);

    fIndexXYZ[column][row][layer] = fNCells;

    Int_t temp;
    // Sort ascending Time
    for (Int_t i = fNCells; i > 0; --i) {
        if (fTime[fIndexTime[i - 1]] < fTime[fIndexTime[i]])
            break;
        temp = fIndexTime[i - 1];
        fIndexTime[i - 1] = fIndexTime[i];
        fIndexTime[i] = temp;
    }
    for (Int_t i = fNCells; i > 0; --i) {
        if (fEdep[fIndexEdep[i - 1]] > fEdep[fIndexEdep[i]])
            break;
        temp = fIndexEdep[i - 1];
        fIndexEdep[i - 1] = fIndexEdep[i];
        fIndexEdep[i] = temp;
    }
    for (Int_t i = fNCells; i > 0; --i) {
        if (fBeta[fIndexBeta[i - 1]] > fBeta[fIndexBeta[i]])
            break;
        temp = fIndexBeta[i - 1];
        fIndexBeta[i - 1] = fIndexBeta[i];
        fIndexBeta[i] = temp;
    }
    ++fNCells;
}

Int_t BmnNdetClusterFinder::GetNClusters()
{
    return fNClusters;
}

Int_t BmnNdetClusterFinder::GetNCells()
{
    return fNCells;
}

Int_t BmnNdetClusterFinder::GetNCells(Double_t threshold)
{
    Int_t nCells = 0.;
    for (Int_t iCell = 0; iCell < fNCells; ++iCell) {
        if (fEdep[iCell] > threshold)
            ++nCells;
    }
    return nCells;
}

Int_t BmnNdetClusterFinder::GetNColumns()
{
    return fIndexXYZ.size();
}

Int_t BmnNdetClusterFinder::GetNRows()
{
    return fIndexXYZ[0].size();
}

Int_t BmnNdetClusterFinder::GetNLayers()
{
    return fIndexXYZ[0][0].size();
}

vector<Int_t> BmnNdetClusterFinder::GetCells()
{
    return fCell;
}
vector<Int_t> BmnNdetClusterFinder::GetColumns()
{
    return fColumn;
}
vector<Int_t> BmnNdetClusterFinder::GetRows()
{
    return fRow;
}
vector<Int_t> BmnNdetClusterFinder::GetLayers()
{
    return fLayer;
}
vector<Double_t> BmnNdetClusterFinder::GetX()
{
    return fXcoord;
}
vector<Double_t> BmnNdetClusterFinder::GetY()
{
    return fYcoord;
}
vector<Double_t> BmnNdetClusterFinder::GetZ()
{
    return fZcoord;
}
vector<Double_t> BmnNdetClusterFinder::GetTime()
{
    return fTime;
}
vector<Double_t> BmnNdetClusterFinder::GetEdep()
{
    return fEdep;
}
vector<Double_t> BmnNdetClusterFinder::GetBeta()
{
    return fBeta;
}

BmnNdetCluster* BmnNdetClusterFinder::GetCluster(Int_t i_cluster)
{
    if (i_cluster < 0 || i_cluster >= fNClusters)
        return nullptr;
    return fClusters[i_cluster];
}

Int_t BmnNdetClusterFinder::GetClusterStatus(Int_t i_cluster)
{
    if (i_cluster >= fNClusters || i_cluster < 0)
        return kNoCluster;
    return fClusterStatus[i_cluster];
}

void BmnNdetClusterFinder::SetClusterStatus(Int_t i_cluster, eClusterStatus status)
{
    if (i_cluster >= fNClusters || i_cluster < 0)
        return;

    fClusterStatus[i_cluster] = status;
}

BmnNdetCluster* BmnNdetClusterFinder::AddCluster()
{
    BmnNdetCluster* cl = new BmnNdetCluster();
    fClusters.push_back(cl);
    fClusterStatus.push_back(kUnchecked);
    ++fNClusters;
    return cl;
}

void BmnNdetClusterFinder::AddCellToCluster(Int_t i_cell, Int_t i_cluster = -1)
{
    if (i_cluster == -1)
        i_cluster = fNClusters - 1;
    if (i_cell < 0 || i_cell >= fNCells)
        return;
    if (i_cluster < 0 || i_cluster >= fNClusters)
        return;
    BmnNdetCluster* cl = GetCluster(i_cluster);
    Int_t i = fCell[i_cell];
    Int_t c = fColumn[i_cell];
    Int_t r = fRow[i_cell];
    Int_t l = fLayer[i_cell];
    Double_t x = fXcoord[i_cell];
    Double_t y = fYcoord[i_cell];
    Double_t z = fZcoord[i_cell];
    Double_t t = fTime[i_cell];
    Double_t e = fEdep[i_cell];
    Double_t b = fBeta[i_cell];
    cl->AddCell(i, c, r, l, x, y, z, t, e, b);
    fCellStatus[i_cell] = kIsUsed;
    --fNUncheckedCellsInEvent;
}

Int_t BmnNdetClusterFinder::FindClusters1()
{
    fNUncheckedCellsInEvent = fNCells;
    while (fNUncheckedCellsInEvent > 0) {
        // Search for the first unchecked cell
        Int_t iCell1 = -1;
        for (Int_t iTime = 0; iTime < fNCells; ++iTime) {
            Int_t iCell = fIndexTime[iTime];
            if (fCellStatus[iCell] == kIsNotUsed) {
                iCell1 = iCell;
                break;
            }
        }
        if (iCell1 < 0)
            return fNClusters;
        else {
            // Start a new cluster
            BmnNdetCluster* cl = AddCluster();
            AddCellToCluster(iCell1);
            // Chain of checkouts of all neighbours and their neighbours
            Int_t iCellInCluster = 0;
            while (iCellInCluster < cl->GetNCells()) {
                // Loop over all neighbours
                for (Int_t dIx : {-1, 0, 1}) {
                    Int_t iX = cl->GetColumns()[iCellInCluster] + dIx;
                    if (iX < 0 || iX >= GetNColumns())
                        continue;
                    for (Int_t dIy : {-1, 0, 1}) {
                        Int_t iY = cl->GetRows()[iCellInCluster] + dIy;
                        if (iY < 0 || iY >= GetNRows())
                            continue;
                        for (Int_t dIz : {-1, 0, 1}) {
                            Int_t iZ = cl->GetLayers()[iCellInCluster] + dIz;
                            if (iZ < 0 || iZ >= GetNLayers())
                                continue;

                            // Check neighbour
                            Int_t iNeighbour = fIndexXYZ[iX][iY][iZ];
                            if (iNeighbour < 0)
                                continue;
                            if (fCellStatus[iNeighbour] == kIsNotUsed)
                                AddCellToCluster(iNeighbour);
                        }
                    }
                }
                ++iCellInCluster;
            }
        }
    }
    return fNClusters;
}

Int_t BmnNdetClusterFinder::FindClusters1_2()
{
    fNUncheckedCellsInEvent = fNCells;
    while (fNUncheckedCellsInEvent > 0) {
        // Search for the first unchecked cell
        Int_t iCell1 = -1;
        for (Int_t iTime = 0; iTime < fNCells; ++iTime) {
            Int_t iCell = fIndexTime[iTime];
            if (fCellStatus[iCell] == kIsNotUsed) {
                iCell1 = iCell;
                break;
            }
        }
        if (iCell1 < 0)
            return fNClusters;
        else {
            // Start a new cluster
            BmnNdetCluster* cl = AddCluster();
            AddCellToCluster(iCell1);
            Bool_t checkAllLayers = kFALSE;
            if (cl->GetLayers()[0] == 0)
                checkAllLayers = kTRUE;
            // Chain of checkouts of all neighbours and their neighbours
            Int_t iCellInCluster = 0;
            while (iCellInCluster < cl->GetNCells()) {
                // Loop over all neighbours
                for (Int_t dIx : {-1, 0, 1}) {
                    Int_t iX = cl->GetColumns()[iCellInCluster] + dIx;
                    if (iX < 0 || iX >= GetNColumns())
                        continue;
                    for (Int_t dIy : {-1, 0, 1}) {
                        Int_t iY = cl->GetRows()[iCellInCluster] + dIy;
                        if (iY < 0 || iY >= GetNRows())
                            continue;
                        if (checkAllLayers) {
                            for (Int_t iZ = 0; iZ < GetNLayers(); ++iZ) {
                                // Check neighbour
                                Int_t iNeighbour = fIndexXYZ[iX][iY][iZ];
                                if (iNeighbour < 0)
                                    continue;
                                if (fCellStatus[iNeighbour] == kIsNotUsed)
                                    AddCellToCluster(iNeighbour);
                            }
                        } else {
                            for (Int_t dIz : {-1, 0, 1}) {
                                Int_t iZ = cl->GetLayers()[iCellInCluster] + dIz;
                                if (iZ < 0 || iZ >= GetNLayers())
                                    continue;

                                // Check neighbour
                                Int_t iNeighbour = fIndexXYZ[iX][iY][iZ];
                                if (iNeighbour < 0)
                                    continue;
                                if (fCellStatus[iNeighbour] == kIsNotUsed)
                                    AddCellToCluster(iNeighbour);
                            }
                        }
                    }
                }
                ++iCellInCluster;
            }
        }
    }
    return fNClusters;
}

Int_t BmnNdetClusterFinder::FindClusters2()
{
    fNUncheckedCellsInEvent = fNCells;
    while (fNUncheckedCellsInEvent > 0) {
        // Search for the first unchecked cell
        Int_t iCell1 = -1;
        for (Int_t iBeta = 0; iBeta < fNCells; ++iBeta) {
            Int_t iCell = fIndexBeta[iBeta];
            if (fCellStatus[iCell] == kIsNotUsed) {
                iCell1 = iCell;
                break;
            }
        }
        if (iCell1 < 0)
            return fNClusters;
        else {
            // Start a new cluster
            BmnNdetCluster* cl = AddCluster();
            AddCellToCluster(iCell1);
            // Chain of checkouts of all neighbours and their neighbours
            Int_t iCellInCluster = 0;
            while (iCellInCluster < cl->GetNCells()) {
                // Loop over all neighbours
                for (Int_t dIx : {-1, 0, 1}) {
                    Int_t iX = cl->GetColumns()[iCellInCluster] + dIx;
                    if (iX < 0 || iX >= GetNColumns())
                        continue;
                    for (Int_t dIy : {-1, 0, 1}) {
                        Int_t iY = cl->GetRows()[iCellInCluster] + dIy;
                        if (iY < 0 || iY >= GetNRows())
                            continue;
                        for (Int_t dIz : {-1, 0, 1}) {
                            Int_t iZ = cl->GetLayers()[iCellInCluster] + dIz;
                            if (iZ < 0 || iZ >= GetNLayers())
                                continue;

                            // Check neighbour
                            Int_t iNeighbour = fIndexXYZ[iX][iY][iZ];
                            if (iNeighbour < 0)
                                continue;
                            if (fCellStatus[iNeighbour] == kIsNotUsed)
                                AddCellToCluster(iNeighbour);
                        }
                    }
                }
                ++iCellInCluster;
            }
        }
    }
    return fNClusters;
}

Int_t BmnNdetClusterFinder::FindClusters2_2()
{
    fNUncheckedCellsInEvent = fNCells;
    while (fNUncheckedCellsInEvent > 0) {
        // Search for the first unchecked cell
        Int_t iCell1 = -1;
        for (Int_t iBeta = 0; iBeta < fNCells; ++iBeta) {
            Int_t iCell = fIndexBeta[iBeta];
            if (fCellStatus[iCell] == kIsNotUsed) {
                iCell1 = iCell;
                break;
            }
        }
        if (iCell1 < 0)
            return fNClusters;
        else {
            // Start a new cluster
            BmnNdetCluster* cl = AddCluster();
            AddCellToCluster(iCell1);
            // Chain of checkouts of all neighbours and their neighbours
            Int_t iCellInCluster = 0;
            while (iCellInCluster < cl->GetNCells()) {
                // Loop over all neighbours
                for (Int_t dIx : {-1, 0, 1}) {
                    Int_t iX = cl->GetColumns()[iCellInCluster] + dIx;
                    if (iX < 0 || iX >= GetNColumns())
                        continue;
                    for (Int_t dIy : {-1, 0, 1}) {
                        Int_t iY = cl->GetRows()[iCellInCluster] + dIy;
                        if (iY < 0 || iY >= GetNRows())
                            continue;
                        for (Int_t dIz : {-1, 0, 1, 2}) {
                            Int_t iZ = cl->GetLayers()[iCellInCluster] + dIz;
                            if (iZ < 0 || iZ >= GetNLayers())
                                continue;

                            // Check neighbour
                            Int_t iNeighbour = fIndexXYZ[iX][iY][iZ];
                            if (iNeighbour < 0)
                                continue;
                            if (fCellStatus[iNeighbour] == kIsNotUsed)
                                AddCellToCluster(iNeighbour);
                        }
                    }
                }
                ++iCellInCluster;
            }
        }
    }
    return fNClusters;
}

Int_t BmnNdetClusterFinder::FindClusters3(Double_t cut_beta)
{
    fNUncheckedCellsInEvent = fNCells;
    while (fNUncheckedCellsInEvent > 0) {
        // Search for the first unchecked cell
        Int_t iCell1 = -1;
        for (Int_t iBeta = 0; iBeta < fNCells; ++iBeta) {
            Int_t iCell = fIndexBeta[iBeta];
            if (fCellStatus[iCell] == kIsNotUsed) {
                iCell1 = iCell;
                break;
            }
        }
        if (iCell1 < 0)
            return fNClusters;
        else {
            // Start a new cluster
            BmnNdetCluster* cl = AddCluster();
            AddCellToCluster(iCell1);
            Double_t beta0 = fBeta[iCell1];
            // Chain of checkouts of all neighbours and their neighbours
            Int_t iCellInCluster = 0;
            while (iCellInCluster < cl->GetNCells()) {
                // Loop over all neighbours
                for (Int_t dIx : {-1, 0, 1}) {
                    Int_t iX = cl->GetColumns()[iCellInCluster] + dIx;
                    if (iX < 0 || iX >= GetNColumns())
                        continue;
                    for (Int_t dIy : {-1, 0, 1}) {
                        Int_t iY = cl->GetRows()[iCellInCluster] + dIy;
                        if (iY < 0 || iY >= GetNRows())
                            continue;
                        for (Int_t dIz : {-1, 0, 1}) {
                            Int_t iZ = cl->GetLayers()[iCellInCluster] + dIz;
                            if (iZ < 0 || iZ >= GetNLayers())
                                continue;

                            // Check neighbour
                            Int_t iNeighbour = fIndexXYZ[iX][iY][iZ];
                            if (iNeighbour < 0)
                                continue;
                            if (fCellStatus[iNeighbour] == kIsUsed)
                                continue;
                            if (TMath::Abs(fBeta[iNeighbour] - beta0) > cut_beta)
                                continue;
                            AddCellToCluster(iNeighbour);
                        }
                    }
                }
                ++iCellInCluster;
            }
        }
    }
    return fNClusters;
}

Int_t BmnNdetClusterFinder::FindClusters4(Double_t cut_beta)
{
    fNUncheckedCellsInEvent = fNCells;
    while (fNUncheckedCellsInEvent > 0) {
        // Search for the first unchecked cell
        Int_t iCell1 = -1;
        for (Int_t iTime = 0; iTime < fNCells; ++iTime) {
            Int_t iCell = fIndexTime[iTime];
            if (fCellStatus[iCell] == kIsNotUsed) {
                iCell1 = iCell;
                break;
            }
        }
        if (iCell1 < 0)
            return fNClusters;
        else {
            // Start a new cluster
            BmnNdetCluster* cl = AddCluster();
            AddCellToCluster(iCell1);

            Double_t x0 = cl->GetX()[0];
            Double_t y0 = cl->GetY()[0];
            Double_t z0 = cl->GetZ()[0];
            Double_t t0 = cl->GetTime()[0];
            Double_t beta0 = cl->BetaFirst();
            // Chain of checkouts of all neighbours and their neighbours
            Int_t iCellInCluster = 0;
            while (iCellInCluster < cl->GetNCells()) {
                Double_t x1 = cl->GetX()[iCellInCluster];
                Double_t y1 = cl->GetY()[iCellInCluster];
                Double_t z1 = cl->GetZ()[iCellInCluster];
                Double_t t1 = cl->GetTime()[iCellInCluster];
                // Loop over all neighbours
                for (Int_t dIx : {-1, 0, 1}) {
                    Int_t iX = cl->GetColumns()[iCellInCluster] + dIx;
                    if (iX < 0 || iX >= GetNColumns())
                        continue;
                    for (Int_t dIy : {-1, 0, 1}) {
                        Int_t iY = cl->GetRows()[iCellInCluster] + dIy;
                        if (iY < 0 || iY >= GetNRows())
                            continue;
                        for (Int_t dIz : {-1, 0, 1}) {
                            Int_t iZ = cl->GetLayers()[iCellInCluster] + dIz;
                            if (iZ < 0 || iZ >= GetNLayers())
                                continue;

                            // Check neighbour
                            Int_t iNeighbour = fIndexXYZ[iX][iY][iZ];
                            if (iNeighbour < 0)
                                continue;
                            Double_t x2 = cl->GetX()[iNeighbour];
                            Double_t y2 = cl->GetY()[iNeighbour];
                            Double_t z2 = cl->GetZ()[iNeighbour];
                            Double_t t2 = cl->GetTime()[iNeighbour];
                            Double_t distance21 =
                                TMath::Sqrt(TMath::Sq(x2 - x1) + TMath::Sq(y2 - y1) + TMath::Sq(z2 - z1));
                            Double_t distance20 =
                                TMath::Sqrt(TMath::Sq(x2 - x0) + TMath::Sq(y2 - y0) + TMath::Sq(z2 - z0));
                            Double_t d_time20 = t2 - t0;
                            Double_t d_time21 = t2 - t1;
                            Double_t beta20 =
                                d_time20 <= 0 ? -1. : distance20 * 1.E-2 / (d_time20 * 1.E-9) / TMath::C();
                            Double_t beta21 =
                                d_time21 <= 0 ? -1. : distance21 * 1.E-2 / (d_time21 * 1.E-9) / TMath::C();
                            if (beta20 > 1. || beta20 < 0.)
                                continue;
                            if (beta21 > 1. || beta21 < 0.)
                                continue;
                            if (TMath::Abs(beta21 - beta0) > cut_beta)
                                continue;
                            if (fCellStatus[iNeighbour] == kIsUsed)
                                continue;
                            else
                                AddCellToCluster(iNeighbour);
                        }
                    }
                }
                ++iCellInCluster;
            }
        }
    }
    return fNClusters;
}

Int_t BmnNdetClusterFinder::MergeClusters(Int_t i_cluster_1, Int_t i_cluster_2)
{
    if (i_cluster_1 == i_cluster_2)
        return fNClusters;
    Int_t i1 = min(i_cluster_1, i_cluster_2);
    Int_t i2 = max(i_cluster_1, i_cluster_2);

    BmnNdetCluster* cl1 = GetCluster(i1);
    BmnNdetCluster* cl2 = GetCluster(i2);

    for (Int_t iCell2 = 0; iCell2 < cl2->GetNCells(); ++iCell2) {

        cl1->AddCell(cl2->GetColumns()[iCell2], cl2->GetRows()[iCell2], cl2->GetLayers()[iCell2], cl2->GetX()[iCell2],
                     cl2->GetY()[iCell2], cl2->GetZ()[iCell2], cl2->GetTime()[iCell2], cl2->GetEdep()[iCell2],
                     cl2->GetBeta()[iCell2]);
    }
    fClusters.erase(fClusters.begin() + i2);
    fClusterStatus.erase(fClusterStatus.begin() + i2);
    --fNClusters;
    return fNClusters;
}

Int_t BmnNdetClusterFinder::SelectNeutrons(Double_t cut_angle, Int_t cut_ncells, Bool_t check_border)
{
    Int_t nNeutrons = 0;
    BmnNdetCluster* cl = nullptr;
    for (Int_t iCluster = 0; iCluster < fNClusters; ++iCluster) {
        // if(fClusterStatus[iCluster]!=kUnchecked)continue;
        cl = GetCluster(iCluster);
        if (cl->AngleToTarget() < cut_angle && cl->GetNCells() >= cut_ncells
            && (!cl->StartsOnBorder(GetNColumns(), GetNRows(), GetNLayers()) && check_border))
        {
            fClusterStatus[iCluster] = kIsNeutron;
            ++nNeutrons;
        } else {
            fClusterStatus[iCluster] = kNotNeutron;
        }
    }
    cl = nullptr;
    return nNeutrons;
}

Int_t BmnNdetClusterFinder::CalculateNumberOfNeutrons(Double_t weight_edep,
                                                      Double_t weight_ncells,
                                                      Double_t weight_nclusters,
                                                      Double_t threshold12)
{
    Double_t edep = 0.;
    Int_t n_cells = 0;
    Int_t n_clusters = 0;
    BmnNdetCluster* cl = nullptr;
    for (Int_t iCluster = 0; iCluster < fNClusters; ++iCluster) {
        if (fClusterStatus[iCluster] != kIsNeutron)
            continue;
        cl = GetCluster(iCluster);
        edep += cl->EnergyDeposited();
        n_cells += Double_t(cl->GetNCells());
        n_clusters += 1.;
    }
    cl = nullptr;
    if (n_clusters < 1)
        return 0;
    Double_t separation_parameter = weight_edep * edep + weight_ncells * n_cells + weight_nclusters * n_clusters;
    if (separation_parameter > threshold12)
        return 2;
    return 1;
}

void BmnNdetClusterFinder::Clear()
{
    for (Int_t i = 0; i < fNCells; ++i) {
        fCell.pop_back();
        fColumn.pop_back();
        fRow.pop_back();
        fLayer.pop_back();
        fXcoord.pop_back();
        fYcoord.pop_back();
        fZcoord.pop_back();
        fTime.pop_back();
        fEdep.pop_back();
        fBeta.pop_back();
        fIndexTime.pop_back();
        fIndexEdep.pop_back();
        fIndexBeta.pop_back();
        fCellStatus.pop_back();
    }
    fNCells = 0;

    for (Int_t c = 0; c < GetNColumns(); ++c) {
        for (Int_t r = 0; r < GetNRows(); ++r) {
            for (Int_t l = 0; l < GetNLayers(); ++l) {
                fIndexXYZ[c][r][l] = -1;
            }
        }
    }

    for (Int_t i = 0; i < fNClusters; ++i) {
        fClusters.pop_back();
        fClusterStatus.pop_back();
    }
    fNClusters = 0;
}

void BmnNdetClusterFinder::Print()
{
    cout << "-----------------------------   BmnNdetClusterFinder   -----------------------------" << endl;
    for (Int_t i = 0; i < fNCells; ++i) {
        cout << fCell[i] << " cell (" << fColumn[i] << ", " << fRow[i] << ", " << fLayer[i] << "): ";
        cout << " i: " << fIndexXYZ[fColumn[i]][fRow[i]][fLayer[i]];
        cout << "\tX: " << fXcoord[i] << "[cm], Y: " << fYcoord[i] << "[cm], Z: " << fZcoord[i] << "[cm]";
        cout << "\tt: " << fTime[i] << "[ns], Edep: " << 1.E3 * fEdep[i] << "[MeV]" << endl;
    }
    cout << endl;

    cout << "Sorted time" << endl;
    for (Int_t i = 0; i < fNCells; ++i) {
        cout << fIndexTime[i] << "(" << fTime[fIndexTime[i]] << "),   ";
    }
    cout << endl;

    cout << "Sorted edep" << endl;
    for (Int_t i = 0; i < fNCells; ++i) {
        cout << fIndexEdep[i] << "(" << fEdep[fIndexEdep[i]] << "),   ";
    }
    cout << endl;

    cout << "Sorted beta" << endl;
    for (Int_t i = 0; i < fNCells; ++i) {
        cout << fIndexBeta[i] << "(" << fBeta[fIndexBeta[i]] << "),   ";
    }
    cout << endl;
    cout << "------------------------------------------------------------------------------------" << endl;
}
