/** \file BmnScWallEvent.h
 ** \author
 ** \date 09.09.2022
 **/

/** \class BmnScWallEvent
 ** \brief Class for Bmn ScWall data container in event
 ** \version 0.1
 **/

#ifndef BMNSCWALLEVENT_H
#define BMNSCWALLEVENT_H

#include "BmnScWallCell.h"

#include <TClonesArray.h>
#include <TNamed.h>
#include <iostream>
#include <vector>

class BmnScWallEvent : public TNamed
{
  public:
    /** @brief Default constructor.
     **/
    BmnScWallEvent()
        : TNamed()
        , fCellsArr()
    {
        reset();
    }

    /** Destructor **/
    ~BmnScWallEvent() { reset(); }

    /** @brief Class name
     ** @return BmnScWallEvent
     **/
    virtual const char* GetClassName() { return "BmnScWallEvent"; }

    /** @brief Cell info
     ** @return BmnScWallCell object with index cell_id = {1 to 174}
     **/
    BmnScWallCell* GetCell(uint8_t cell_id) { return &fCellsArr.at(cell_id - 1); };

    /** @brief Set CellsArrtor
     ** @return Set Vector of BmnScWallCell objects
     **/
    void SetCell(uint8_t cell_id, BmnScWallCell Cell) { fCellsArr.at(cell_id - 1) = Cell; };

    /** @brief Zero all fields
     ** @return Set All fields to zero
     **/
    void reset() { fCellsArr.fill({}); }
    void ResetCells()
    {
        int sci = 1;
        for (auto& it : fCellsArr) {
            it.SetCellId(sci);
            it.SetSignal(0.);
            sci++;
        }
    }

    void SummarizeEvent();
    virtual void Print(Option_t* option = "") const {};

    static const int fgkMaxCells = 174;   // 174 modules numbered from 1 to 174 inclusively

  private:
    std::array<BmnScWallCell, fgkMaxCells> fCellsArr;

    ClassDef(BmnScWallEvent, 2);
};

#endif /* BMNSCWALLEVENT_H */
