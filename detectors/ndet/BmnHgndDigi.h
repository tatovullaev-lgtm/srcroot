/**
 * \file BmnHgndDigi.h
 * \author Nikolay Karpushkin <karpushkin@inr.ru>
 * \date 12.04.2025
 * \brief Data class for HGND digis in the BM@N experiment.
 */

#ifndef BmnHgndDigi_H
#define BmnHgndDigi_H 1

#include "BmnNdetDigit.h"   // Base class

/**
 * \class BmnHgndDigi
 * \brief Data structure for a single HGND digit.
 *
 * Inherits from BmnNdetDigit and adds ToA / ToT fields.
 */
class BmnHgndDigi : public BmnNdetDigit
{
  public:
    /// Default constructor
    BmnHgndDigi() = default;

    /// Destructor
    virtual ~BmnHgndDigi() = default;

    /// Reset digit content to default state
    void reset() override final
    {
        BmnNdetDigit::reset();
        fToa = 0.0;
        fTot = 0.0;
    }

    /// Returns the class name
    const char* GetClassName() override final { return "BmnHgndDigi"; }

    /// Set Time-over-Threshold (in ns)
    void SetTot(double tot) { fTot = tot; }

    /// Set Time-of-Arrival (in ns)
    void SetToa(double toa) { fToa = toa; }

    /// Get Time-over-Threshold (in ns)
    double GetTot() const { return fTot; }

    /// Get Time-of-Arrival (in ns)
    double GetToa() const { return fToa; }

  private:
    double fToa = 0.0;   // Time-of-Arrival
    double fTot = 0.0;   // Time-over-Threshold

    ClassDefOverride(BmnHgndDigi, 1);
};

#endif   // BmnHgndDigi_H
