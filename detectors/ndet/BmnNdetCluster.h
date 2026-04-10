/** \file BmnNdetCluster.h
 ** \author Arseniy Shabanov <arseniy.shabanov@phystech.edu>
 ** \date 19.01.2024
 **/

/** \class BmnNdetCluster
 ** \brief Class for description of recognized cluster in Bmn Ndet detector
 ** \details BmnNdetCluster contains information about position, time and deposited energies
 ** of all cells belonging to the cluster.
 ** this information is filled when BmnClusterFinder::FindClusters() method is run.
 ** \version 1.0
 **/

#ifndef BMNNDETCLUSTER_H
#define BMNNDETCLUSTER_H

#include "TObject.h"

#include <iostream>

using namespace std;

// #include "TClonesArray.h"

class BmnNdetCluster : public TObject
{
  private:
    //! number of cells in this cluster
    Int_t fNCells;
    //! Index of a cell in BmnNdetCell array in BmnNdetEvent object.
    vector<Int_t> fCell;
    //! Column number of a cell. It must be proportional to X coordinate in self coordinate system of a detector
    vector<Int_t> fColumn;
    //! Row number of a cell. It must be proportional to Y coordinate in self coordinate system of a detector
    vector<Int_t> fRow;
    //! Layer number of a cell. It must be proportional to Z coordinate in self coordinate system of a detector
    vector<Int_t> fLayer;
    //! X coordinate [cm]
    vector<Double_t> fXcoord;
    //! Y coordinate [cm]
    vector<Double_t> fYcoord;
    //! Z coordinate [cm]
    vector<Double_t> fZcoord;
    //! Time of actuation of the cell [ns]
    vector<Double_t> fTime;
    //! Energy deposited in the cell [GeV]
    vector<Double_t> fEdep;
    //! beta=v/c calculated from the time of flight to this cell
    vector<Double_t> fBeta;

  public:
    //! Default constructor
    BmnNdetCluster();
    //! Default destructor
    ~BmnNdetCluster();
    /*! Сonstructor
     * @param size expected number of cells in cluster
     */
    BmnNdetCluster(size_t size);

    /*! Add cell to the cluster
     * @param cell Index of a cell in BmnNdetCell array in BmnNdetEvent object.
     * @param column Column index of a cell
     * @param row Row index of a cell
     * @param layer Layer index of a cell
     * @param x X coordinate of a cell [cm]
     * @param y Y coordinate of a cell [cm]
     * @param z Z coordinate of a cell [cm]
     * @param t Time of actuation of the cell [ns]
     * @param e_dep Deposited energy in scintillator of a cell [GeV]
     * @param beta Beta calculated from distance to target and time of flight. If beta < 0 it is calculated while
     * filling from x,y,z,t.
     */
    void AddCell(Int_t cell,
                 Int_t column,
                 Int_t row,
                 Int_t layer,
                 Double_t x,
                 Double_t y,
                 Double_t z,
                 Double_t t,
                 Double_t e_dep,
                 Double_t beta = -1.);

    //! @returns Number of cells in cluster
    Int_t GetNCells();
    /*! Number of cells in cluster with deposited energy > threshold
     * @param threshold Energy threshold [GeV]
     * @returns Number of cells in cluster with energy higher than threshold
     */
    Int_t GetNCells(Double_t threshold);

    //! @returns vector of cell indices
    vector<Int_t> GetCells();
    //! @returns vector of column indices of cells
    vector<Int_t> GetColumns();
    //! @returns vector of row indices of cells
    vector<Int_t> GetRows();
    //! @returns vector of layer indices of cells
    vector<Int_t> GetLayers();
    //! @returns vector of X coordinates of cells [cm]
    vector<Double_t> GetX();
    //! @returns vector of Y coordinates of cells [cm]
    vector<Double_t> GetY();
    //! @returns vector of Z coordinates of cells [cm]
    vector<Double_t> GetZ();
    //! @returns vector of time of actuation of cells [ns]
    vector<Double_t> GetTime();
    //! @returns vector of energies deposited in cells [GeV]
    vector<Double_t> GetEdep();
    //! @returns vector of beta calculated for every cell
    vector<Double_t> GetBeta();

    /*! Function for selection of charged particles. If the particle is charged, it has hit in veto layer.
     * @param veto_layer Index of veto layer. It depends on enumeration of layers while BmnNdetClusterFinder::Fill() is
     * called. By default veto_layer=0.
     * @returns kTRUE if the cluster contains any cells of veto layer
     */
    Bool_t ContainsVetoCells(Int_t veto_layer = 0);

    /*!Function for selection of particles coming from side. If the first hit in cluster is a cell on side border of the
     * detector, it's probably secondary scattered charged particle.
     * @param size_x number of cells along x axis. It is assumed, enumeration of cells starts from 0.
     * @param size_y number of cells along y axis. It is assumed, enumeration of cells starts from 0.
     * @param size_z number of cells along z axis. It is assumed, enumeration of cells starts from 0.
     * @returns kTRUE if the first hit in cluster is a cell on border.
     */
    Bool_t StartsOnBorder(Int_t size_x = 11, Int_t size_y = 11, Int_t size_z = 8);

    /*! Average deposited energy per cell in cluster [GeV]. Only the cells with energy > threshold are taken into
     * account.
     * @param threshold Energy threshold [GeV]
     * @returns Average deposited energy per cell [GeV]. The expected value for neutrons is 0.01 GeV.
     */
    Double_t EnergyDepositedAverage(Double_t threshold = 0.003);

    /*! Energy deposited in cluster [GeV]. Only the cells with energy > threshold are taken into account.
     * @param threshold Energy threshold [GeV]
     * @returns deposited energy [GeV]. The expected value for neutrons is nCells * 0.01 GeV.
     */
    Double_t EnergyDeposited(Double_t threshold = 0.003);

    /*! Returns index of the first fired cell in cluster
     */
    Int_t FirstCell();

    /*! Returns index of the fastest fired cell in cluster
     */
    Int_t FastestCell();

    /*! Beta calculated for first fired cell in cluster\n
     * beta = v / c \n
     * v = d / t \n
     * d = sqrt(x^2+y^2+z^2)\n
     * @returns v/c. In case of negative time return -1. \n
     * NOTE: In case beta>1 return beta
     */
    Double_t BetaFirst();

    /*! Beta calculated for the cell with highest beta\n
     * beta = v / c \n
     * v = d / t \n
     * d = sqrt(x^2+y^2+z^2)\n
     * @returns v/c. In case of negative time return -1. \n
     * NOTE: In case beta>1 return beta
     */
    Double_t BetaFastest();

    /*! Kinetic energy calculated by the time of flight.
     * Distance from target and time of flight are taken from the first fired cell.
     * @param mass Mass of a particle [GeV/c]. Default value is mass of neutron.
     * @returns Kinetic energy [GeV]
     */
    Double_t EnergyTofFirst(Double_t mass = 0.939565420);

    /*! Kinetic energy calculated by the time of flight.
     * Distance from target and time of flight are taken from the fastest fired cell (with biggest d/t).
     * @param mass Mass of a particle [GeV/c]. Default value is mass of neutron.
     * @returns Kinetic energy [GeV]
     */
    Double_t EnergyTofFastest(Double_t mass = 0.939565420);

    /*! Kinetic energy calculated by the time of flight.
     * Distance from target and time of flight are taken as average values in cluster.
     * @param mass Mass of a particle [GeV/c]. Default value is mass of neutron.
     * @returns Kinetic energy [GeV]
     */
    Double_t EnergyTofAverage(Double_t mass = 0.939565420);

    /*! Angle between direction to first cell and average direction of cluster\n
     * Direction of cluster is sum of vectors, which connect first cell and other cells.
     * @returns angle in degrees in the range 0 - 180
     */
    Double_t AngleToTarget();

    /*! Angle between vector (x,y,z) and average direction of cluster\n
     * Direction of cluster is sum of vectors, which connect first cell and other cells.
     * @param x X coordinate of vector
     * @param y Y coordinate of vector
     * @param z Z coordinate of vector
     * @returns angle in degrees in the range 0 - 180
     */
    Double_t AngleToDirection(Double_t x = 0, Double_t y = 0, Double_t z = 1);

    //! Merging of clusters
    BmnNdetCluster* AddCluster(BmnNdetCluster* other);

    //! Copy constructor
    BmnNdetCluster(const BmnNdetCluster&);

    //! Move constructor
    BmnNdetCluster(BmnNdetCluster&&) noexcept;

    //! Clear all cell contents, set fNCells=0
    void Clear();

    //! Print
    virtual void Print(const Option_t* option = "");

    virtual const char* GetClassName() { return "BmnNdetCluster"; }
    ClassDef(BmnNdetCluster, 1)
};

#endif /* BMNNDETCLUSTER_H */
