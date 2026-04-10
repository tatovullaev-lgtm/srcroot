/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** \file BmnFHCalDigi.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 16.01.2022
 **/

/** \class BmnFHCalDigi
 ** \brief Data class for Bmn FHCal digital signal processing
 ** \version 1.0
 **/

#ifndef BmnFHCalDigi_H
#define BmnFHCalDigi_H 1

#include "BmnDigiContainerTemplate.h"   // for BmnDigiContainerTemplate
#include "BmnFHCalDigit.h"              // for BmnFHCalDigit base class

class BmnFHCalDigi
    : public BmnFHCalDigit
    , public BmnDigiContainerTemplate
{

  public:
    /**@brief Default constructor.
     **/
    BmnFHCalDigi()
        : BmnFHCalDigit()
        , BmnDigiContainerTemplate()
    {}

    /** Destructor **/
    ~BmnFHCalDigi(){};

    void reset() override final
    {
        BmnFHCalDigit::reset();
        BmnDigiContainerTemplate::reset();
    }

    /** @brief Class name
     ** @return BmnFHCalDigi
     **/
    virtual const char* GetClassName() override final { return "BmnFHCalDigi"; }

    const int DrawWfm()
    {
        TString hist_name =
            Form("Mod%u Sec%u. Signal %.2f FitR2 %.2f", GetModuleId(), GetSectionId(), GetSignal(), GetFitR2());
        TCanvas* canvas = new TCanvas();
        DrawWfmWithTitle(canvas, hist_name);
        return 1;
    }

    ClassDefOverride(BmnFHCalDigi, 2);
};

#endif   // BmnFHCalDigi_H
