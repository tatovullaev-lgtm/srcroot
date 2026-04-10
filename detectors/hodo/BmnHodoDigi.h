/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** \file BmnHodoDigi.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 21.01.2022
 **/

/** \class BmnHodoDigi
 ** \brief Data class for BmnHodo digital signal processing
 ** \version 1.0
 **/

#ifndef BmnHodoDigi_H
#define BmnHodoDigi_H 1

#include "BmnDigiContainerTemplate.h"   // for BmnDigiContainerTemplate
#include "BmnHodoAddress.h"             // for BmnHodoAddress
#include "BmnHodoDigit.h"               // for BmnHodoDigit

class BmnHodoDigi
    : public BmnHodoDigit
    , public BmnDigiContainerTemplate
{

  public:
    /**@brief Default constructor.
     **/
    BmnHodoDigi()
        : BmnHodoDigit()
        , BmnDigiContainerTemplate()
        , fNpeaks(0)
        , fR2history{}
        , fSignalIntegral{0.0}
        , fCrosstalk{0.0}
    {}

    /** Destructor **/
    ~BmnHodoDigi() {};

    void reset() override final
    {
        BmnHodoDigit::reset();
        BmnDigiContainerTemplate::reset();
        fNpeaks = 0;
        fR2history.clear();
        fSignalIntegral = 0.0;
        fCrosstalk = 0.0;
    }

    size_t fNpeaks = 0;
    std::vector<float> fR2history;
    float fSignalIntegral;
    float fCrosstalk;

    /** @brief Class name
     ** @return BmnHodoDigi
     **/
    virtual const char* GetClassName() override final { return "BmnHodoDigi"; }

    /** @brief Strip Side
     ** @return Strip Side from Unique channel address (see BmnHodoAddress)
     **/
    uint32_t GetStripSide() const { return BmnHodoAddress::GetStripSide(fAddress); };

    /** @brief Gain
     ** @return Gain from Unique channel address (see BmnHodoAddress)
     **/
    uint32_t GetGain() const { return BmnHodoAddress::GetGain(fAddress); };

    const int DrawWfm()
    {
        TString hist_name =
            (fNpeaks > 1)
                ? Form("Strip%u. From deconv. Amplitude %.0f ZL %.0f TimeMax %.0f FitR2 %.4f; time [sample]; ampl "
                       "[a.u.]",
                       GetStripId(), fFitAmpl, fFitZL, fFitTimeMax, GetFitR2())
                : Form("Strip%u. From raw. Amplitude %d ZL %d TimeMax %d FitR2 %.4f; time [sample]; ampl [a.u.]",
                       GetStripId(), fAmpl, fZL, fTimeMax, GetFitR2());
        TCanvas* canvas = new TCanvas();
        canvas->Divide(1, 2);
        DrawWfmWithTitle(canvas, hist_name);
        DrawR2history(canvas);

        return 1;
    }

    void DrawR2history(TCanvas* canvas)
    {
        if (fR2history.empty())
            return;
        canvas->cd(2);

        std::vector<float> points(fR2history.size());
        std::iota(std::begin(points), std::end(points), 0);   // Fill with 0, 1, ..., wfm.back().
        TGraph* tgr_ptr = new TGraph(fR2history.size(), &points[0], &fR2history[0]);
        tgr_ptr->SetTitle(Form("R2 history; iteration; R2"));
        tgr_ptr->Draw();
    }

    ClassDefOverride(BmnHodoDigi, 4);
};

#endif   // BmnHodoDigi_H
