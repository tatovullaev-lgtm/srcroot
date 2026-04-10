/** \file BmnHodoStrip.h
 ** \author
 ** \date 09.09.2022
 **/

/** \class BmnHodoStrip
 ** \brief Class for Bmn Hodo strip data container in event
 ** \version 0.1
 **/

#ifndef BMNHODOSTRIP_H
#define BMNHODOSTRIP_H

#include <TClonesArray.h>
#include <array>
#include <numeric>

class BmnHodoStrip
{
  public:
    // Enumeration to represent pile-up status
    enum class PileUp
    {
        No,
        Resolved,
        Unresolved
    };

    /** @brief Default constructor.
     **/
    BmnHodoStrip();

    /**  Copy constructor **/
    BmnHodoStrip(const BmnHodoStrip&);

    /** Move constructor  **/
    BmnHodoStrip(BmnHodoStrip&&);

    /** Assignment operator  **/
    BmnHodoStrip& operator=(const BmnHodoStrip&);

    /** Move Assignment operator  **/
    BmnHodoStrip& operator=(BmnHodoStrip&&);

    /** Destructor **/
    virtual ~BmnHodoStrip(){};

    /** @brief Class name
     ** @return BmnHodoStrip
     **/
    virtual const char* GetClassName() { return "BmnHodoStrip"; }

    /** @brief Strip id
     ** @return BmnHodoStrip index
     **/
    int GetStripId() const { return fStripId; };

    /** @brief Deposited Signal
     ** @return Deposited Signal in strip in event [~Z^2]
     **/
    float GetSignal() const { return fStripSignal; };

    /** @brief Deposited Signal by integral
     ** @return Deposited Signal by integral over the whole waveform in strip in event [~Z^2]
     **/
    float GetSignalIntegral() const { return fStripSignalIntegral; };

    /** @brief Get pile-up status
     ** @return Pile-up status
     **/
    PileUp GetPileUpStatus() const { return fPileUpStatus; }

    void SetStripId(int StripId) { fStripId = StripId; }
    void SetSignal(float Signal) { fStripSignal = Signal; }
    void SetSignalIntegral(float Signal) { fStripSignalIntegral = Signal; }
    void SetPileUpStatus(PileUp pileUp) { fPileUpStatus = pileUp; }
    void reset();

    virtual void Print(Option_t* option = "") const;

  private:
    int fStripId;
    float fStripSignal;
    float fStripSignalIntegral;
    PileUp fPileUpStatus;

    ClassDef(BmnHodoStrip, 2);
};

#endif /* BMNHODOSTRIP_H */
