/** @file BmnFHCalModule.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 27.03.2022
 **
 ** Code of Class for Bmn FHCal module data container in event
 **/

#include "BmnFHCalModule.h"

#include "BmnDetectorList.h"   // for kFHCAL

#include <iostream>
using namespace std;

BmnFHCalModule::BmnFHCalModule()
    : FairHit()
    , fModId(0)
    , fModuleEnergy(0.0)
    , fSectVec()
{}

BmnFHCalModule::BmnFHCalModule(int ModuleId, const TVector3& posHit, const TVector3& posHitErr, int pointIndx)
    : FairHit(kFHCAL, posHit, posHitErr, pointIndx)
    , fModId(ModuleId)
    , fModuleEnergy(0.0)
{
    fSectVec.clear();
}

BmnFHCalModule::BmnFHCalModule(const BmnFHCalModule& other)
    : FairHit(other)
    , fModId(other.fModId)
    , fModuleEnergy(other.fModuleEnergy)
    , fSectVec(other.fSectVec)
{}

BmnFHCalModule::BmnFHCalModule(BmnFHCalModule&& other) noexcept
    : FairHit(std::move(other))
    , fModId(other.fModId)
    , fModuleEnergy(other.fModuleEnergy)
    , fSectVec(std::move(other.fSectVec))
{}

BmnFHCalModule& BmnFHCalModule::operator=(const BmnFHCalModule& other)
{
    if (this != &other) {
        FairHit::operator=(other);
        fModId = other.fModId;
        fModuleEnergy = other.fModuleEnergy;
        fSectVec = other.fSectVec;
    }
    return *this;
}

BmnFHCalModule& BmnFHCalModule::operator=(BmnFHCalModule&& other) noexcept
{
    if (this != &other) {
        FairHit::operator=(std::move(other));
        fModId = other.fModId;
        fModuleEnergy = other.fModuleEnergy;
        fSectVec = std::move(other.fSectVec);
    }
    return *this;
}

void BmnFHCalModule::Print(Option_t* option) const
{
    cout << "Module ID: " << fModId << "\n"
         << "Energy: " << fModuleEnergy << "\n"
         << "Sections: " << GetNsections() << "\n"
         << "Position: (" << GetX() << ", " << GetY() << ", " << GetZ() << ")" << endl;
}

void BmnFHCalModule::SummarizeModule(double section_threshold)
{
    double energy = 0.0;
    for (const auto& section_energy : fSectVec) {
        if (section_energy > section_threshold) {
            energy += section_energy;
        }
    }
    SetEnergy(energy);
}

void BmnFHCalModule::ResetSections()
{
    fModuleEnergy = 0.;
    std::fill(fSectVec.begin(), fSectVec.end(), 0.0);
}

void BmnFHCalModule::reset()
{
    fModId = 0;
    ResetSections();
    fSectVec.clear();

    TVector3 zero;
    SetPosition(zero);
    SetPositionError(zero);
    SetRefIndex(-1);
}
