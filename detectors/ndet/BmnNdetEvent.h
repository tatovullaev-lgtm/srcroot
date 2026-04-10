/** \file BmnNdetEvent.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 27.03.2022
 **/

/** \class BmnNdetEvent
 ** \brief Class for Bmn Ndet data container in event
 ** \version 1.0
 **/

#ifndef BMNNDETEVENT_H
#define BMNNDETEVENT_H

#include "BmnNdetCell.h"

#include <TClonesArray.h>
#include <TNamed.h>

class BmnNdetEvent : public TNamed
{
  public:
    /** @brief Default constructor. **/
    BmnNdetEvent();

    /** Destructor **/
    virtual ~BmnNdetEvent();

    /** Copy constructor **/
    BmnNdetEvent(const BmnNdetEvent&);

    /** Move constructor **/
    BmnNdetEvent(BmnNdetEvent&&) noexcept;

    /** Assignment operator **/
    BmnNdetEvent& operator=(const BmnNdetEvent&);

    /** Move assignment operator **/
    BmnNdetEvent& operator=(BmnNdetEvent&&) noexcept;

    /** @brief Class name
     ** @return BmnNdetEvent
     **/
    virtual const char* GetClassName() { return "BmnNdetEvent"; }

    /** @brief Add a cell to the event.
     ** @param cell Pointer to the BmnNdetCell.
     **/
    void AddCell(BmnNdetCell* cell);

    /** @brief Get a cell based on its address.
     ** @param address The unique address of the cell.
     ** @return Pointer to the BmnNdetCell corresponding to the address.
     **/
    BmnNdetCell* GetCell(uint32_t address);

    /** @brief Get a cell based on its arm, row, column, and layer indices.
     ** @param ArmId Arm index.
     ** @param RowId Row index.
     ** @param ColumnId Column index.
     ** @param LayerId Layer index.
     ** @return Pointer to the BmnNdetCell corresponding to the indices.
     **/
    BmnNdetCell* GetCell(uint32_t ArmId, uint32_t RowId, uint32_t ColumnId, uint32_t LayerId);

    /** @brief Get the total number of cells fired in event.
     ** @return Total number of cells fired in event.
     **/
    Long64_t GetTotalCells() const { return fCells->GetEntriesFast(); }

    /** @brief Interface to the cells set.
     ** @return Cells set.
     **/
    TClonesArray* GetCells() const { return fCells; }

    /** @brief Modifiable interface to the cells set.
     ** @return Modifiable cells set.
     **/
    TClonesArray* GetModifiableCells() { return fCells; }

    /** @brief Zero all fields
     **/
    void reset();

    /** @brief Reset time and energy for all cells.
     **/
    void ResetCells();

  private:
    TClonesArray* fCells;

    ClassDef(BmnNdetEvent, 8);
};

#endif /* BMNNDETEVENT_H */