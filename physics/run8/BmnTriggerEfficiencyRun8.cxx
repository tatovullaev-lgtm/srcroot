#include "BmnTriggerEfficiencyRun8.h"

#include "BmnTrigDigit.h"
#include "BmnTrigWaveDigit.h"
#include "FairLogger.h"

#include <TClonesArray.h>
#include <stdlib.h>

BmnTriggerEfficiencyRun8::BmnTriggerEfficiencyRun8()
    : isValid(kTRUE)
    , fNBd(-1)
    , fEffFile1(nullptr)
    , fEffFile2(nullptr)
    , fEffFile3(nullptr)
    , fHBd(nullptr)
    , fHFd(nullptr)
    , fHCctSyst(nullptr)
    , fHFrf(nullptr)
    , fDstFile(nullptr)
    , fDstTree(nullptr)
    , fBTrigInfo(nullptr)
    , fBPrimaryVertex(nullptr)
    , fRunId(-1.)
    , fBC2IntCut(-1.)
    , fFDTrigWindowLeftEdge(-1)
    , fFDPeakLimit(-1)
{
    InitEfficiencyInput();
}

BmnTriggerEfficiencyRun8::BmnTriggerEfficiencyRun8(TString dstFileName)
    : isValid(kTRUE)
    , fNBd(-1)
    , fEffFile1(nullptr)
    , fEffFile2(nullptr)
    , fEffFile3(nullptr)
    , fHBd(nullptr)
    , fHFd(nullptr)
    , fHCctSyst(nullptr)
    , fHFrf(nullptr)
    , fDstFile(nullptr)
    , fDstTree(nullptr)
    , fBTrigInfo(nullptr)
    , fBPrimaryVertex(nullptr)
    , fRunId(-1.)
    , fBC2IntCut(-1.)
    , fFDTrigWindowLeftEdge(-1)
    , fFDPeakLimit(-1)
{
    InitEfficiencyInput();

    fDstFile = TFile::Open(dstFileName, "READONLY");
    // fDstFile = new TFile(dstFileName, "READ");
    if (!fDstFile || !fDstFile->IsOpen() || fDstFile->IsZombie()) {
        LOGF(error, "Cannot open file %s", dstFileName.Data());
        isValid = kFALSE;
        return;
    }
    fDstTree = (TTree*)fDstFile->Get("bmndata");
    if (!fDstTree) {
        LOGF(error, "Cannot find tree bmndata in the file");
        isValid = kFALSE;
        return;
    }

    Int_t ret = fDstTree->SetBranchAddress("BmnTrigInfo.", &fBTrigInfo);
    if ((ret < 0) || (ret > 2)) {
        LOGF(error, "Cannot match branch BmnTrigInfo.");
        isValid = kFALSE;
        return;
    }
    ret = fDstTree->SetBranchAddress("PrimaryVertex.", &fBPrimaryVertex);
    if ((ret < 0) || (ret > 2)) {
        LOGF(error, "Cannot match branch PrimaryVertex.");
        isValid = kFALSE;
        return;
    }

    // Get cuts for this run id
    Ssiz_t startCh = dstFileName.Index("mpd_run_Top_");
    if (startCh == -1) {
        LOGF(error, "Dst file name  must contain \"mpd_run_Top_<RunId>\"");
        isValid = kFALSE;
        return;
    }
    startCh = dstFileName.Index("mpd_run_Top_") + TString("mpd_run_Top_").Sizeof() - 1;
    fRunId = (TString(dstFileName(startCh, 4))).Atoi();
    LOGF(info, "runId = %d", fRunId);

    SetBC2IntCutByRun(fRunId);
    SetFDTrigWindowLeftEdgeByRun(fRunId);
    SetFDPeakLimitByRun(fRunId);

    LOGF(info, "File %s successfully opened", dstFileName.Data());
}

BmnTriggerEfficiencyRun8::BmnTriggerEfficiencyRun8(Int_t runId, BmnTrigInfoDst* trigInfo, CbmVertex* primaryVertex)
    : isValid(kTRUE)
    , fNBd(-1)
    , fEffFile1(nullptr)
    , fEffFile2(nullptr)
    , fEffFile3(nullptr)
    , fHBd(nullptr)
    , fHFd(nullptr)
    , fHCctSyst(nullptr)
    , fHFrf(nullptr)
    , fDstFile(nullptr)
    , fDstTree(nullptr)
    , fBTrigInfo(trigInfo)
    , fBPrimaryVertex(primaryVertex)
    , fRunId(runId)
    , fBC2IntCut(-1.)
    , fFDTrigWindowLeftEdge(-1)
    , fFDPeakLimit(-1)
{
    InitEfficiencyInput();

    LOGF(info, "runId = %d", fRunId);

    SetBC2IntCutByRun(fRunId);
    SetFDTrigWindowLeftEdgeByRun(fRunId);
    SetFDPeakLimitByRun(fRunId);
}

Double_t BmnTriggerEfficiencyRun8::GetBDEfficiency(Int_t runId, Int_t ntrPV)
{
    LOGF(debug, "BmnTriggerEfficiencyRun8::GetBDEfficiency(): Start");
    return GetEfficiencyOrError(fHBd, runId, ntrPV, kTRUE);
}

Double_t BmnTriggerEfficiencyRun8::GetFDEfficiency(Int_t runId, Int_t ntrPV)
{
    LOGF(debug, "BmnTriggerEfficiencyRun8::GetFDEfficiency(): Start");
    return GetEfficiencyOrError(fHFd, runId, ntrPV, kTRUE);
}

Double_t BmnTriggerEfficiencyRun8::GetTriggerEfficiency(Int_t runId, Int_t ntrPV)
{
    LOGF(debug, "BmnTriggerEfficiencyRun8::GetTriggerEfficiency(): Start");

    Double_t bdEff = GetEfficiencyOrError(fHBd, runId, ntrPV, kTRUE);
    if (bdEff < 0)
        return -1.;

    return bdEff * GetEfficiencyOrError(fHFd, runId, ntrPV, kTRUE);
}

Bool_t BmnTriggerEfficiencyRun8::GetTriggerEfficiencyWithErrors(Int_t runId,
                                                                Int_t ntrPV,
                                                                Double_t& eff,
                                                                Double_t& statErr,
                                                                Double_t& systErr,
                                                                Double_t& errSystErr)
{
    statErr = systErr = errSystErr = -1.;

    eff = GetTriggerEfficiency(runId, ntrPV);
    if (eff < 0.)
        return kFALSE;
    else if (eff == 0.) {
        statErr = systErr = errSystErr = 0.;
        return kTRUE;
    }

    Double_t relStatErrBD =
        GetEfficiencyOrError(fHBd, runId, ntrPV, kFALSE) / GetEfficiencyOrError(fHBd, runId, ntrPV, kTRUE);
    Double_t relStatErrFD =
        GetEfficiencyOrError(fHFd, runId, ntrPV, kFALSE) / GetEfficiencyOrError(fHFd, runId, ntrPV, kTRUE);
    statErr = eff * sqrt(relStatErrBD * relStatErrBD + relStatErrFD * relStatErrFD);

    systErr = abs(GetEfficiencyOrError(fHCctSyst, runId, ntrPV, kTRUE));

    errSystErr = GetEfficiencyOrError(fHCctSyst, runId, ntrPV, kFALSE);

    return kTRUE;
}

Bool_t BmnTriggerEfficiencyRun8::GetTriggerEfficiencyWithErrors(Double_t& eff,
                                                                Double_t& statErr,
                                                                Double_t& systErr,
                                                                Double_t& errSystErr)
{
    if (!fBPrimaryVertex) {
        LOGF(debug, "Primary Vertex branch should be defined");
        return kFALSE;
    }
    Int_t pvntr = fBPrimaryVertex->GetNTracks();

    return GetTriggerEfficiencyWithErrors(fRunId, pvntr, eff, statErr, systErr, errSystErr);
}

Bool_t BmnTriggerEfficiencyRun8::GetFluxRejectionFactorWithError(Int_t runId, Double_t& factor, Double_t& err)
{
    factor = err = -1.;

    if (!IsValid()) {
        LOGF(error, "This BmnTriggerEfficiencyRun8 object is not valid");
        return kFALSE;
    }

    Int_t bin = fHFrf->FindBin(runId);
    LOGF(debug, "BmnTriggerEfficiencyRun8::GetFluxRejectionFactorWithError(): bin = %d", bin);
    Int_t binx, biny, binz;
    fHFrf->GetBinXYZ(bin, binx, biny, binz);
    LOGF(debug, "BmnTriggerEfficiencyRun8::GetFluxRejectionFactorWithError(): binx = %d, biny = %d, binz = %d", binx,
         biny, binz);

    if ((binx < 1) || (binx > fHFrf->GetNbinsX())) {
        LOGF(error, "Run id is out of analysis range");
        return kFALSE;
    }

    factor = fHFrf->GetBinContent(bin);
    if (factor <= 0.)
        return kFALSE;

    err = fHFrf->GetBinError(bin);

    return kTRUE;
}

Bool_t BmnTriggerEfficiencyRun8::GetFluxRejectionFactorWithError(Double_t& factor, Double_t& err)
{
    return GetFluxRejectionFactorWithError(fRunId, factor, err);
}

Bool_t BmnTriggerEfficiencyRun8::IsEventAnalysable()
{
    Double_t pvx, pvy, pvz;
    Int_t pvntr;
    pvx = fBPrimaryVertex->GetX();
    pvy = fBPrimaryVertex->GetY();
    pvz = fBPrimaryVertex->GetZ();
    pvntr = fBPrimaryVertex->GetNTracks();

    LOGF(debug, "(pvx,pvy,pvz,pvntr) = (%f,%f,%f,%d)", pvx, pvy, pvz, pvntr);

    TVector3 vtxPos;
    vtxPos.SetXYZ(0.3179, 0., -0.0169);
    if ((abs(pvz - vtxPos.Z()) > 1.) || (pow(pvx - vtxPos.X(), 2) + pow(pvy - vtxPos.Y(), 2) > pow(1.5, 2))
        || (pvntr < 2))
    {
        LOGF(debug, "Event primary vertex does not pass analysis conditions");
        return kFALSE;
    }

    Double_t pv_chi2tondf = fBPrimaryVertex->GetChi2() / fBPrimaryVertex->GetNDF();
    if ((pv_chi2tondf < 0.1) || (pv_chi2tondf > 10)) {
        LOGF(debug, "Event primary vertex does not pass analysis conditions");
        return kFALSE;
    }

    Int_t bc2inttw40 = GetBC2Int(258, 267);
    if ((bc2inttw40 > fBC2IntCut) || (fBC2IntCut < 0)) {
        LOGF(debug, "This event does not pass BC2 analysis conditions for pile up");
        return kFALSE;
    }

    auto TrigPattern = fBTrigInfo->GetInputSignalsAR();
    auto bitCCT2 = (TrigPattern >> 7) & 1;
    if ((bitCCT2 != 1) || !IsBDTriggered() || !IsFDTriggered()) {
        LOGF(debug, "This event does not pass trigger conditions");
        return kFALSE;
    }

    return kTRUE;
}

Bool_t BmnTriggerEfficiencyRun8::IsEventAnalysable(Int_t evId)
{
    if ((evId >= fDstTree->GetEntries()) || (evId < 0)) {
        LOGF(debug, "Event id is out of range for this file");
        return kFALSE;
    }

    fDstTree->GetEntry(evId);

    return IsEventAnalysable();
}

Double_t BmnTriggerEfficiencyRun8::GetEfficiencyOrError(TH2D* hist, Int_t runId, Int_t ntrPV, Bool_t isEff)
{
    if (ntrPV < 2) {
        LOGF(error, "Number of tracks in PV is out of analysis range");
        return -1.;
    }
    if (!IsValid()) {
        LOGF(error, "This BmnTriggerEfficiencyRun8 object is not valid");
        return -1.;
    }

    Int_t bin = hist->FindBin(runId, ntrPV);
    LOGF(debug, "BmnTriggerEfficiencyRun8::GetEfficiencyOrError(): bin = %d", bin);
    Int_t binx, biny, binz;
    hist->GetBinXYZ(bin, binx, biny, binz);
    LOGF(debug, "BmnTriggerEfficiencyRun8::GetEfficiencyOrError(): binx = %d, biny = %d, binz = %d", binx, biny, binz);

    if ((binx < 1) || (binx > hist->GetNbinsX())) {
        LOGF(error, "Run id is out of analysis range");
        return -1.;
    }

    if (biny > hist->GetNbinsY()) {
        if (isEff)
            return hist->GetBinContent(binx, hist->GetNbinsY());
        else
            return hist->GetBinError(binx, hist->GetNbinsY());
    }

    Double_t ret;

    if (isEff)
        ret = hist->GetBinContent(bin);
    else
        ret = hist->GetBinError(bin);

    return ret;
}

void BmnTriggerEfficiencyRun8::InitEfficiencyInput()
{
    TString fileName1 = TString(getenv("VMCWORKDIR")) + TString("/parameters/triggerefficiency/EffBdFd_Run8_v2.root");
    fEffFile1 = new TFile(fileName1, "READ");
    if (!fEffFile1->IsOpen() || fEffFile1->IsZombie()) {
        LOGF(error, "Cannot open file %s", fileName1.Data());
        isValid = kFALSE;
        return;
    }

    fHBd = (TH2D*)fEffFile1->Get("hbd");
    if (!fHBd) {
        LOGF(error, "Cannot find histogram hbd in the file");
        isValid = kFALSE;
        return;
    }
    fHFd = (TH2D*)fEffFile1->Get("hfd");
    if (!fHFd) {
        LOGF(error, "Cannot find histogram hfd in the file");
        isValid = kFALSE;
        return;
    }

    LOGF(info, "File %s successfully opened", fileName1.Data());

    TString fileName2 = TString(getenv("VMCWORKDIR")) + TString("/parameters/triggerefficiency/EffCctSyst_Run8.root");
    fEffFile2 = new TFile(fileName2, "READ");
    if (!fEffFile2->IsOpen() || fEffFile2->IsZombie()) {
        LOGF(error, "Cannot open file %s", fileName2.Data());
        isValid = kFALSE;
        return;
    }

    fHCctSyst = (TH2D*)fEffFile2->Get("herr");
    if (!fHCctSyst) {
        LOGF(error, "Cannot find histogram herr in the file");
        isValid = kFALSE;
        return;
    }

    LOGF(info, "File %s successfully opened", fileName2.Data());

    TString fileName3 =
        TString(getenv("VMCWORKDIR")) + TString("/parameters/triggerefficiency/FluxRejectionFactor_Run8.root");
    fEffFile3 = new TFile(fileName3, "READ");
    if (!fEffFile3->IsOpen() || fEffFile3->IsZombie()) {
        LOGF(error, "Cannot open file %s", fileName3.Data());
        isValid = kFALSE;
        return;
    }

    fHFrf = (TH1D*)fEffFile3->Get("hfrc_bt");
    if (!fHFrf) {
        LOGF(error, "Cannot find histogram hfrc_bt in the file");
        isValid = kFALSE;
        return;
    }

    LOGF(info, "File %s successfully opened", fileName3.Data());

    return;
}

void BmnTriggerEfficiencyRun8::SetBC2IntCutByRun(Int_t runId)
{
    if (runId < 7840)
        fBC2IntCut = 9000.;
    else if (runId < 7984)
        fBC2IntCut = 11000.;
    else if (runId < 8005)
        fBC2IntCut = 14000.;
    else if (runId < 8011)
        fBC2IntCut = 13000.;
    else if (runId < 8030)
        fBC2IntCut = 12000.;
    else if (runId < 8060)
        fBC2IntCut = 10000.;
    else
        fBC2IntCut = 11000.;

    return;
}

void BmnTriggerEfficiencyRun8::SetFDTrigWindowLeftEdgeByRun(Int_t runId)
{
    fFDTrigWindowLeftEdge = 279 - FDTrigWindow;
    if (runId < 7069)
        fFDTrigWindowLeftEdge += 2;
    else if (runId < 7114)
        fFDTrigWindowLeftEdge += 3;
    else if (runId < 7237)
        fFDTrigWindowLeftEdge += 2;
    else if (runId < 7560)
        fFDTrigWindowLeftEdge += 0;
    else if (runId < 7594)
        fFDTrigWindowLeftEdge += 3;

    return;
}

void BmnTriggerEfficiencyRun8::SetFDPeakLimitByRun(Int_t runId)
{
    fFDPeakLimit = 4250;
    if (runId < 7069)
        fFDPeakLimit = 3500;
    else if (runId < 7114)
        fFDPeakLimit = 4250;
    else if (runId < 7148)
        fFDPeakLimit = 3500;
    else if (runId < 7237)
        fFDPeakLimit = 2500;
    else if (runId < 7258)
        fFDPeakLimit = 3500;
    else if (runId < 7429)
        fFDPeakLimit = 4000;
    else if (runId < 7486)
        fFDPeakLimit = 3750;
    else if (runId < 7560)
        fFDPeakLimit = 4000;
    else if (runId < 7594)
        fFDPeakLimit = 3500;

    return;
}

Int_t BmnTriggerEfficiencyRun8::GetBC2Int(Int_t leftEl, Int_t rightEl)
{
    Int_t bcIntTW = 0;

    // Search for a number of BC1 signals between leftEdge and rightEdge
    TClonesArray* bc2Digits = fBTrigInfo->GetBC2Digits();
    if (bc2Digits->GetEntriesFast() != 1) {
        LOGF(error, "Unexpected behaviour. bc2Digits->GetEntriesFast() = %d", bc2Digits->GetEntriesFast());
        return -1;
    }
    BmnTrigWaveDigit* bc2Digit = (BmnTrigWaveDigit*)bc2Digits->UncheckedAt(0);
    for (Int_t i = leftEl; i <= rightEl; ++i) {
        if (bc2Digit->GetShortValue()[i] > 100)
            bcIntTW += bc2Digit->GetShortValue()[i];
    }

    return bcIntTW;
}

Bool_t BmnTriggerEfficiencyRun8::IsBDTriggered()
{
    Bool_t bdTrig = kFALSE;

    // Search for a number of BD signals inside the trigger window (+-50 ns)
    TClonesArray* bdDigits = fBTrigInfo->GetBDDigits();
    Int_t nBd = 0;
    // Int_t aBd[40] = {0};
    for (Int_t iBd = 0; iBd < bdDigits->GetEntriesFast(); iBd++) {
        BmnTrigDigit* bdDigit = (BmnTrigDigit*)bdDigits->UncheckedAt(iBd);
        if ((bdDigit->GetMod() < 0) || (bdDigit->GetMod() > 39)) {
            LOGF(error, "Unexpected BD module number %d", bdDigit->GetMod());
            return kFALSE;
        }
        // 7988 - mean=1950., delta=50.
        // 7842 - mean=1930., delta=30.
        if (abs(bdDigit->GetTime() - 1930.) < 30.) {
            // aBd[bdDigit->GetMod()]++;
            nBd++;
        }
    }
    if (nBd >= 2)
        bdTrig = kTRUE;

    fNBd = nBd;

    return bdTrig;
}

Bool_t BmnTriggerEfficiencyRun8::IsFDTriggered()
{
    Bool_t fdTrig = kFALSE;

    // Search for a number of FD signals inside the trigger window (+-35 ns)
    TClonesArray* fdDigits = fBTrigInfo->GetFDDigits();
    // Double_t fdamp = 0.;
    Int_t start = fFDTrigWindowLeftEdge;
    Int_t stop = start + 2 * FDTrigWindow;
    Int_t peak = -100000;
    // fdamp = (Double_t)trInfo->GetFDAmp(); // Could be out of trig window
    BmnTrigWaveDigit* fdDigit0 = (BmnTrigWaveDigit*)fdDigits->UncheckedAt(0);
    for (Int_t i = start; i <= stop; ++i)
        if (fdDigit0->GetShortValue()[i] > peak)
            peak = fdDigit0->GetShortValue()[i];

    // cout << "peak=" << peak << endl;
    // 7988 (same for 7842) - 4500, 4250 - half the threshold height
    if (peak < fFDPeakLimit)
        fdTrig = kTRUE;

    return fdTrig;
}
