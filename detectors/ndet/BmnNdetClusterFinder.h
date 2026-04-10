/** \file BmnNdetClusterFinder.h
 ** \author Arseniy Shabanov <arseniy.shabanov@phystech.edu>
 ** \date 19.01.2024
 **/

#ifndef BMNNDETCLUSTERFINDER_H
#define BMNNDETCLUSTERFINDER_H

#include "BmnNdetCluster.h"
#include "TObject.h"

#include <vector>

using namespace std;

/*! \class BmnNdetClusterFinder
 * \brief Class for description of algorithm of recognition of clusters in Bmn Ndet detector
 * \details BmnNdetClusterFinder must be run in the following order: \n
 * -# create BmnNdetClusterFinder object
 * -# initialize data with BmnNdetClusterFinder::Fill() method
 * -# search for clusters with BmnNdetClusterFinder::FindClusters() method \n
 * The recognized clusters can be obtained with BmnNdetClusterFinder::GetCluster() method
 * \version 1.0
 */
class BmnNdetClusterFinder : public TObject
{
  public:
    //! Flag if the cell belongs to any cluster
    enum eCellStatus
    {
        kIsNotUsed = 0, /*!< the cell doesn't belong to any cluster */
        kIsUsed         /*!< the cell belongs to some cluster */
    };

    //! Status of the cluster
    enum eClusterStatus
    {
        kNoCluster = -1, /*!< there is no cluster                      */
        kUnchecked = 0,  /*!< the cluster is not checked yet           */
        kIsNeutron,      /*!< the cluster is produced by neutron       */
        kNotNeutron      /*!< the cluster can't be produced by neutron */
    };

  private:
    //! Recognized clusters
    vector<BmnNdetCluster*> fClusters;
    //! Flag if cell belongs to some cluster
    vector<eClusterStatus> fClusterStatus;

    //! Number of fired cells in event
    Int_t fNCells;
    //! Number of recognized clusters
    Int_t fNClusters;
    //! Number of cells not belonging to any  cluster
    Int_t fNUncheckedCellsInEvent;

    //! Index of cells sorted by ascending time
    vector<Int_t> fIndexTime;
    //! Index of cells sorted by decending deposited energy
    vector<Int_t> fIndexEdep;
    //! Index of cells sorted by decending beta
    vector<Int_t> fIndexBeta;

    //! Flag if cell belongs to some cluster
    vector<eCellStatus> fCellStatus;

    //! Index of a cell in BmnNdetCell array in BmnNdetEvent object.
    vector<Int_t> fCell;
    //! Column index of a cell. It must be proportional to X coordinate in self coordinate system of a detector
    vector<Int_t> fColumn;
    //! Row index of a cell. It must be proportional to Y coordinate in self coordinate system of a detector
    vector<Int_t> fRow;
    //! Layer index of a cell. It must be proportional to Z coordinate in self coordinate system of a detector
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
    //! Beta = v/c, v = d/t
    vector<Double_t> fBeta;

    vector<vector<vector<Int_t>>> fIndexXYZ;

  public:
    //! Default constructor
    BmnNdetClusterFinder(const Int_t kNColumns = 11, const Int_t kNRows = 11, const Int_t kNLayers = 16);
    //! Default destructor
    ~BmnNdetClusterFinder();

    //! @returns Number of recognized clusters
    Int_t GetNClusters();

    //! @returns Number of cells in event
    Int_t GetNCells();

    /*! Number of cells in event with deposited energy higher than threshold
     * @param threshold minimal deposited energy in counted cells
     * @returns number of cells with edep > threshold
     */
    Int_t GetNCells(Double_t threshold);

    //! @returns Number of columns in detector. Used for iteration of cells along X coordinate.
    Int_t GetNColumns();

    //! @returns Number of rows in detector. Used for iteration of cells along Y coordinate.
    Int_t GetNRows();

    //! @returns Number of Layers in detector. Used for iteration of cells along Z coordinate.
    Int_t GetNLayers();

    //! @returns Vector of indices of all fired cells in event
    vector<Int_t> GetCells();

    //! @returns Vector of column numbers of all fired cells in event
    vector<Int_t> GetColumns();

    //! @returns Vector of row numbers of all fired cells in event
    vector<Int_t> GetRows();

    //! @returns Vector of layer numbers of all fired cells in event
    vector<Int_t> GetLayers();

    //! @returns Vector of X coordinates of all fired cells in event [cm]
    vector<Double_t> GetX();

    //! @returns Vector of Y coordinates of all fired cells in event [cm]
    vector<Double_t> GetY();

    //! @returns Vector of Z coordinates of all fired cells in event [cm]
    vector<Double_t> GetZ();

    //! @returns Vector of time of actuation of all fired cells in event [ns]
    vector<Double_t> GetTime();

    //! @returns Vector of energies deposited in all fired cells in event [GeV]
    vector<Double_t> GetEdep();

    //! @returns Vector of beta calculated as Beta = v/c, v = d/t
    vector<Double_t> GetBeta();

    /*! Status of a cluster. It is assigned to the cluster when applying BmnNdetClusterFinder::SelectNeutrons() method.
     * @param i_cluster index of BmnNdetCluster object
     * @returns status of a cluster (see eClusterStatus for details)
     */
    Int_t GetClusterStatus(Int_t i_cluster);

    /*! Set status of a cluster.
     * @param i_cluster index of BmnNdetCluster object
     * @param status status of a cluster (see eClusterStatus for details)
     */
    void SetClusterStatus(Int_t i_cluster, eClusterStatus status);

    /*! Add cell to the list of fired cells
     * For correct work of the code it is needed that columns, rows and layers were enumerated from 0, not 1 !
     * @param cell index of cell in BmnNdetCell array in BmnNdetEvent object.
     * @param column index of column. It must be enumerated starting from 0. The column number must increase in the same
     * direction as X axis.
     * @param row index of row. It must be enumerated starting from 0. The row number must increase in the same
     * direction as Y axis.
     * @param layer index of layer. It must be enumerated starting from 0. The layer number must increase in the same
     * direction as Z axis.
     * @param x X coordinate of the fired cell
     * @param y Y coordinate of the fired cell
     * @param z Z coordinate of the fired cell
     * @param t Time of actuation of the fired cell
     * @param e_dep Energy deposited in cell [GeV]
     */
    void Fill(Int_t cell,
              Int_t column,
              Int_t row,
              Int_t layer,
              Double_t x,
              Double_t y,
              Double_t z,
              Double_t t,
              Double_t e_dep);

    //! Clear all vectors, set fNCells=0
    void Clear();

    //! Print
    void Print();

    /*! Get BmnNdetCluster object
     * @param i_cluster index of recognized cluster
     * @returns pointer to BmnNdetCluster object
     */
    BmnNdetCluster* GetCluster(Int_t i_cluster);

    /*! Add empty BmnNdetCluster to the list of clusters
     * @returns pointer  to the newly added cluster
     */
    BmnNdetCluster* AddCluster();

    /*! Mark a cell i_cell as belonging to cluster i_cluster
     * @param i_cell index of cell in order of its filling with BmnNdetClusterFinder::Fill() method
     * @param i_cluster index of cluster in order of its addition with BmnNdetClusterFinder::AddCluster() method. The
     * last cluster is taken by default.
     */
    void AddCellToCluster(Int_t i_cell, Int_t i_cluster);

    /*! Main method of a class.\n
     * The clusters are groups of neighbouring cells. Time and deposited energies are not taken into account.\n
     * Before you use it, you must fill data with BmnNdetClusterFinder::Fill() method.\n
     * This algorithm loops over cells in order of ascending time.\n
     * @returns number of recognized clusters
     */
    Int_t FindClusters1();

    /*! Main method of a class.\n
     * The clusters are groups of neighbouring cells. Time and deposited energies are not taken into account.\n
     * Before you use it, you must fill data with BmnNdetClusterFinder::Fill() method.\n
     * This algorithm loops over cells in order of ascending time.\n
     * If the cluster starts from veto layer, include to cluster all cells with the same row and column
     * @returns number of recognized clusters
     */
    Int_t FindClusters1_2();

    /*! Main method of a class.\n
     * The clusters are groups of neighbouring cells.\n
     * Time and deposited energies are not taken into account.\n
     * This algorithm loops over cells in order of descending speed.\n
     * Before you use it, you must fill data with BmnNdetClusterFinder::Fill() method.\n
     * @returns number of recognized clusters
     */
    Int_t FindClusters2();

    /*! Main method of a class.\n
     * The clusters are groups of neighbouring cells.\n
     * One more layer is added for checking (the gap of 1 layer doesn't count)\n
     * Time and deposited energies are not taken into account.\n
     * This algorithm loops over cells in order of descending speed.\n
     * Before you use it, you must fill data with BmnNdetClusterFinder::Fill() method.\n
     * @returns number of recognized clusters
     */
    Int_t FindClusters2_2();

    /*! Main method of a class.\n
     * The clusters are groups of neighbouring cells.\n
     * If difference of beta between cells > cut_beta, start a new cluster.
     * Time and deposited energies are not taken into account.\n
     * This algorithm loops over cells in order of descending speed.\n
     * Before you use it, you must fill data with BmnNdetClusterFinder::Fill() method.\n
     * @returns number of recognized clusters
     */
    Int_t FindClusters3(Double_t cut_beta = 0.03);

    /*! Second method of searching for the clusters.\n
     * Before you use it, you must fill data with BmnNdetClusterFinder::Fill() method.\n
     * This algorithm loops over cells in order of ascending time.\n
     * While checking neighbouring cell, the time of flight between cells is taken into account.
     * The cell is added to the cluster, if the speed of propagation of the hadronic shower between the cells is the
     * same as the speed of initial neutron.
     * @param cut_beta maximum difference between beta of propagation of shower and beta calculated from the first cell
     * in cluster.
     * @returns number of recognized clusters
     */
    Int_t FindClusters4(Double_t cut_beta = 0.01);

    /*! Select neutrons
     * @param cut_angle accept, if BmnNdetCluster::AngleToTarget() < cut_angle
     * @param cut_ncells accept, if BmnNdetCluster::GetNCells() >= cut_ncells
     * @param check_border use BmnNdetCluster::StartsOnBorder() check
     * @returns number of found neutrons
     */
    Int_t SelectNeutrons(Double_t cut_angle = 45., Int_t cut_ncells = 3, Bool_t check_border = kFALSE);

    /*! Merge clusters\n
     * The cells of cluster with maximal i_cluster are added to the cluster with minimal i_cluster\n
     * The cluster with maximal i_cluster is destroyed.\n
     * If i_cluster_1==i_cluster_2, cancel merging.
     * @param i_cluster_1 first cluster
     * @param i_cluster_2 second cluster
     * @returns Number of clusters after merging
     */
    Int_t MergeClusters(Int_t i_cluster_1, Int_t i_cluster_2);

    /*! Estimate number of neutrons based on:
     * @param weight_edep * total deposited energy in accepted clusters\n
     * @param weight_nclusters * number of accepted clusters\n
     * @param weight_ncells * total number of cells in accepted clusters.\n
     * If sum of weighed parameters exceeds @param threshold12 there is more than 1 neutron
     * @returns Estimated number of neutrons
     */
    Int_t CalculateNumberOfNeutrons(Double_t weight_edep = 1.,
                                    Double_t weight_ncells = 0.01,
                                    Double_t weight_nclusters = 0.05,
                                    Double_t threshold12 = 0.);

    virtual const char* GetClassName() { return "BmnNdetClusterFinder"; }
    ClassDef(BmnNdetClusterFinder, 1)
};

#endif /* BMNNDETCLUSTERFINDER_H */
