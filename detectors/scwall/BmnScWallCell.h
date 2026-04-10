/** \file BmnScWallCell.h
 ** \author Valerii Troshin <valerytrosh@gmail.com>
 ** \date 09.09.2022
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 27.06.2023
 **/

/** \class BmnScWallCell
 ** \brief Class for Bmn ScWall module data container in event
 ** \version 0.1
 **/

#ifndef BMNSCWALLCELL_H
#define BMNSCWALLCELL_H

#include <FairHit.h>
#include <TClonesArray.h>
#include <array>
#include <numeric>

class BmnScWallCell : public FairHit
{

  public:
    /** @brief Default constructor.
     **/
    BmnScWallCell();

    /** @brief Constructor to use.
     **/
    BmnScWallCell(int CellId, float CellSignal, TVector3 posHit, TVector3 posHitErr, int pointIndx);

    /**  Copy constructor **/
    BmnScWallCell(const BmnScWallCell&);

    /** Move constructor  **/
    BmnScWallCell(BmnScWallCell&&) noexcept;

    /** Assignment operator  **/
    BmnScWallCell& operator=(const BmnScWallCell&);

    /** Move Assignment operator  **/
    BmnScWallCell& operator=(BmnScWallCell&&) noexcept;

    /** Destructor **/
    virtual ~BmnScWallCell() {};

    /** @brief Class name
     ** @return BmnScWallCell
     **/
    virtual const char* GetClassName() { return "BmnScWallCell"; }

    /** @brief Cell id
     ** @return BmnScWallCell index
     **/
    int GetCellId() const { return fCellId; };

    /** @brief Deposited Signal
     ** @return Deposited Signal in cell in event [~Z^2]
     **/
    float GetSignal() const { return fCellSignal; };

    /** @brief Position
     ** @return unherited from FairHit
     ** @brief use GetX() GetY() GetZ() syntaxis
     **/

    void SetCellId(int CellId) { fCellId = CellId; }
    void SetSignal(float Signal) { fCellSignal = Signal; }
    void reset();

    virtual void Print(Option_t* option = "") const;

  private:
    int fCellId;
    float fCellSignal;

    ClassDef(BmnScWallCell, 2);
};

#endif /* BMNSCWALLCELL_H */
