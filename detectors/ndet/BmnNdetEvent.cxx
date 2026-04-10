#include "BmnNdetEvent.h"

#include "FairLogger.h"

#include <iostream>

BmnNdetEvent::BmnNdetEvent()
    : TNamed("BmnNdetEvent", "NdetEvent")
    , fCells(new TClonesArray("BmnNdetCell"))
{}

BmnNdetEvent::~BmnNdetEvent()
{
    if (fCells) {
        fCells->Delete();
        delete fCells;
        fCells = nullptr;
    }
}

BmnNdetEvent::BmnNdetEvent(const BmnNdetEvent& other)
    : TNamed(other)
    , fCells(new TClonesArray("BmnNdetCell", other.fCells->GetSize()))
{
    for (int i = 0; i < other.fCells->GetEntriesFast(); ++i) {
        new ((*fCells)[i]) BmnNdetCell(*(BmnNdetCell*)other.fCells->At(i));
    }
}

BmnNdetEvent::BmnNdetEvent(BmnNdetEvent&& other) noexcept
    : TNamed(std::move(other))
    , fCells(other.fCells)
{
    other.fCells = nullptr;
}

BmnNdetEvent& BmnNdetEvent::operator=(const BmnNdetEvent& other)
{
    if (this != &other) {
        TNamed::operator=(other);

        if (fCells) {
            fCells->Delete();
            delete fCells;
        }

        fCells = new TClonesArray("BmnNdetCell", other.fCells->GetSize());
        for (int i = 0; i < other.fCells->GetEntriesFast(); ++i) {
            new ((*fCells)[i]) BmnNdetCell(*(BmnNdetCell*)other.fCells->At(i));
        }
    }
    return *this;
}

BmnNdetEvent& BmnNdetEvent::operator=(BmnNdetEvent&& other) noexcept
{
    if (this != &other) {
        TNamed::operator=(std::move(other));

        if (fCells) {
            fCells->Delete();
            delete fCells;
        }

        fCells = other.fCells;
        other.fCells = nullptr;
    }
    return *this;
}

void BmnNdetEvent::AddCell(BmnNdetCell* cell)
{
    if (!GetCell(cell->GetAddress())) {
        new ((*fCells)[fCells->GetEntriesFast()]) BmnNdetCell(*cell);
    } else {
        LOG(error) << "BmnNdetEvent::AddCell. Tried to duplicate address.";
        return;
    }
}

BmnNdetCell* BmnNdetEvent::GetCell(uint32_t address)
{
    for (int i = 0; i < fCells->GetEntriesFast(); ++i) {
        BmnNdetCell* cell = (BmnNdetCell*)fCells->At(i);
        if (cell->GetAddress() == address) {
            return cell;
        }
    }
    return nullptr;
}

BmnNdetCell* BmnNdetEvent::GetCell(uint32_t ArmId, uint32_t RowId, uint32_t ColumnId, uint32_t LayerId)
{
    uint32_t address = BmnNdetAddress::GetAddress(ArmId, RowId, ColumnId, LayerId);
    return GetCell(address);
}

void BmnNdetEvent::reset()
{
    if (fCells) {
        fCells->Delete();
    }
}

void BmnNdetEvent::ResetCells()
{
    for (int i = 0; i < fCells->GetEntriesFast(); ++i) {
        BmnNdetCell* cell = (BmnNdetCell*)fCells->At(i);
        cell->ResetTimeEnergy();
    }
}