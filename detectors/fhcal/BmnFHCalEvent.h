/** \file BmnFHCalEvent.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 27.03.2022
 **/

/** \class BmnFHCalEvent
 ** \brief Class for Bmn FHCal data container in event
 ** \version 1.0
 **/

#ifndef BMNFHCALEVENT_H
#define BMNFHCALEVENT_H

#include "BmnFHCalModule.h"

#include <TClonesArray.h>
#include <TNamed.h>
#include <vector>

class BmnFHCalEvent : public TNamed
{
  public:
    /** @brief Default constructor.
     **/
    BmnFHCalEvent()
        : TNamed()
        , fTotalEnergy()
        , fModulesVec()
    {}

    /** Destructor **/
    ~BmnFHCalEvent() { reset(); }

    /** @brief Set TotalModules externally
     ** @param MaxModules Total number of modules
     **/
    void SetTotalModules(int TotalModules) { fModulesVec.resize(TotalModules); }

    /** @brief Set Deposited Energy
     ** @return Set Deposited Energy in FHCal in event [mip]
     **/
    void SetTotalEnergy(float Energy) { fTotalEnergy = Energy; };

    /** @brief Set ModulesArrtor
     ** @return Set Vector of BmnFHCalModule objects
     **/
    void SetModule(uint8_t mod_id, BmnFHCalModule Module) { fModulesVec.at(mod_id - 1) = Module; };

    /** @brief Zero all fiels
     ** @return Set All fields to zero
     **/
    void reset()
    {
        fTotalEnergy = 0.;
        fModulesVec.clear();
    }
    void ResetEnergies()
    {
        fTotalEnergy = 0.;
        for (auto& it : fModulesVec) {
            it.ResetSections();
            it.ResetLinks();
        }
    }

    void SummarizeEvent();
    virtual void Print(Option_t* option = "") const {};

    /** @brief Class name
     ** @return BmnFHCalEvent
     **/
    virtual const char* GetClassName() { return "BmnFHCalEvent"; }

    /** @brief Total number of modules
     ** @return Total number of modules
     **/
    int GetTotalModules() const { return fModulesVec.size(); };

    /** @brief Deposited Energy
     ** @return Deposited Energy in FHCal in event [mip]
     **/
    float GetTotalEnergy() const { return fTotalEnergy; };

    /** @brief Module info
     ** @return BmnFHCalModule object with index mod_id = {1 to fgkTotalModules}
     **/
    BmnFHCalModule* GetModule(uint8_t mod_id) { return &fModulesVec.at(mod_id - 1); };

  private:
    float fTotalEnergy;
    std::vector<BmnFHCalModule> fModulesVec;

    ClassDef(BmnFHCalEvent, 4);
};

#endif /* BMNFHCALEVENT_H */
