/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 10.01.2024
 **
 ** \brief Class for experimental data at digi level
 ** \version 2.0
 **/

#ifndef BmnScWallDigi_H
#define BmnScWallDigi_H 1

#include "BmnDigiContainerTemplate.h"   // for BmnDigiContainerTemplate
#include "BmnScWallDigit.h"             // for BmnScWallDigit

class BmnScWallDigi
    : public BmnScWallDigit
    , public BmnDigiContainerTemplate
{

  public:
    /**@brief Default constructor.
     **/
    BmnScWallDigi()
        : BmnScWallDigit()
        , BmnDigiContainerTemplate()
    {}

    /** Destructor **/
    ~BmnScWallDigi(){};

    void reset() override final
    {
        BmnScWallDigit::reset();
        BmnDigiContainerTemplate::reset();
    }

    /** @brief Class name
     ** @return BmnScWallDigi
     **/
    virtual const char* GetClassName() override final { return "BmnScWallDigi"; }

    const int DrawWfm()
    {
        TString hist_name = Form("Cell%u. Signal %.2f FitR2 %.2f", GetCellId(), GetSignal(), GetFitR2());
        TCanvas* canvas = new TCanvas();
        DrawWfmWithTitle(canvas, hist_name);
        return 1;
    }

    ClassDefOverride(BmnScWallDigi, 2);
};

#endif   // BmnScWallDigi_H
