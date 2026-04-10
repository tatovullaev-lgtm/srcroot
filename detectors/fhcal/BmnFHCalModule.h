/** \file BmnFHCalModule.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 22.01.2025
 **/

/** \class BmnFHCalModule
 ** \brief Class for Bmn FHCal module data container in event
 ** \version 3.0
 **/

#ifndef BMNFHCALMODULE_H
#define BMNFHCALMODULE_H

#include <FairHit.h>
#include <numeric>
#include <vector>

class BmnFHCalModule : public FairHit
{
  public:
    /** @brief Default constructor. **/
    BmnFHCalModule();

    /** @brief Constructor with parameters. **/
    BmnFHCalModule(int ModuleId, const TVector3& posHit, const TVector3& posHitErr, int pointIndx);

    /** Copy constructor. **/
    BmnFHCalModule(const BmnFHCalModule&);

    /** Move constructor. **/
    BmnFHCalModule(BmnFHCalModule&&) noexcept;

    /** Assignment operator. **/
    BmnFHCalModule& operator=(const BmnFHCalModule&);

    /** Move Assignment operator. **/
    BmnFHCalModule& operator=(BmnFHCalModule&&) noexcept;

    /** Destructor. **/
    virtual ~BmnFHCalModule() = default;

    /** @brief Module id. **/
    int GetModuleId() const { return fModId; };

    /** @brief Deposited Energy. **/
    float GetEnergy() const { return fModuleEnergy; };

    /** @brief Sections number. **/
    int GetNsections() const { return fSectVec.size(); };

    /** @brief Section energy. **/
    float GetSectionEnergy(int sec_id) const { return fSectVec.at(sec_id - 1); }

    void SetModuleId(int ModId) { fModId = ModId; }
    void SetEnergy(float Energy) { fModuleEnergy = Energy; }
    void SetNsections(int Nsections) { fSectVec.resize(Nsections); }
    void SetSectionEnergy(int sec_id, float Energy) { fSectVec.at(sec_id - 1) = Energy; }

    void SummarizeModule(double section_threshold = 0.0);
    void reset();
    void ResetSections();

    virtual void Print(Option_t* option = "") const;

  private:
    int fModId;
    float fModuleEnergy;
    std::vector<float> fSectVec;

    ClassDef(BmnFHCalModule, 3);
};

#endif /* BMNFHCALMODULE_H */
