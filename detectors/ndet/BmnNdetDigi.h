/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** \file BmnNdetDigi.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 16.01.2022
 **/

/** \class BmnNdetDigi
 ** \brief Data class for Bmn Ndet digital signal processing
 ** \version 1.0
 **/

#ifndef BmnNdetDigi_H
#define BmnNdetDigi_H 1

#include "BmnDigiContainerTemplate.h"   // for BmnDigiContainerTemplate
#include "BmnNdetDigit.h"               // for BmnNdetDigit base class

class BmnNdetDigi
    : public BmnNdetDigit
    , public BmnDigiContainerTemplate
{

  public:
    /**@brief Default constructor.
     **/
    BmnNdetDigi()
        : BmnNdetDigit()
        , BmnDigiContainerTemplate()
    {}

    /** Destructor **/
    ~BmnNdetDigi(){};

    void reset() override final
    {
        BmnNdetDigit::reset();
        BmnDigiContainerTemplate::reset();
    }

    /** @brief Class name
     ** @return BmnFHCalDigi
     **/
    virtual const char* GetClassName() override final { return "BmnNdetDigi"; }

    const int DrawWfm()
    {
        TString hist_name = Form("%s. Signal %.2f FitR2 %.2f", BmnNdetAddress::GetInfoString(GetAddress()).c_str(),
                                 GetSignal(), GetFitR2());
        TCanvas* canvas = new TCanvas();
        DrawWfmWithTitle(canvas, hist_name);
        return 1;
    }

    ClassDefOverride(BmnNdetDigi, 2);
};

#endif   // BmnNdetDigi_H
