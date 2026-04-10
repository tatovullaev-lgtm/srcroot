// root -b -q CheckTriggerEfficiencyInterfaceRun8.C

void CheckTriggerEfficiencyInterfaceRun8(
    TString fileName = "/eos/lpp/nica/bmn/exp/dst/run8/25.04.0/mpd_run_Top_7842_ev0_p0.root")
{
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("info");   // for info write "INFO"

    // Check event for analysis

#if 1
    /////////////////////////////
    cout << "Use case 1" << endl;
    /////////////////////////////

    // BmnTriggerEfficiencyRun8 *pTE1 = new
    // BmnTriggerEfficiencyRun8("/eos/lpp/nica/bmn/exp/dst/run8/25.04.0/mpd_run_Top_7842_ev0_p0.root");
    // BmnTriggerEfficiencyRun8 *pTE1 = new
    // BmnTriggerEfficiencyRun8("/eos/lpp/nica/bmn/exp/dst/run8/25.04.0/mpd_run_Top_7850_ev0_p10.root");
    BmnTriggerEfficiencyRun8* pTE1 = new BmnTriggerEfficiencyRun8(fileName);
    if (!pTE1->IsValid())
        return;
    Int_t nAnalysableEvents1 = 0;
    for (Int_t i = 0; i < 25004; i++) {
        if (i % 1000 == 0)
            cout << i << "/" << 25004 << endl;
        if (pTE1->IsEventAnalysable(i))
            nAnalysableEvents1++;
    }
    cout << "nAnalysableEvents1 = " << nAnalysableEvents1 << endl;
    delete pTE1;
#endif

#if 1
    /////////////////////////////
    cout << "Use case 2" << endl;
    /////////////////////////////

    TFile* file = TFile::Open(fileName, "READONLY");
    TTree* tree = (TTree*)file->Get("bmndata");

    BmnTrigInfoDst* trigInfo = nullptr;
    tree->SetBranchAddress("BmnTrigInfo.", &trigInfo);
    CbmVertex* primaryVertex = nullptr;
    tree->SetBranchAddress("PrimaryVertex.", &primaryVertex);

    Ssiz_t startCh = fileName.Index("mpd_run_Top_") + TString("mpd_run_Top_").Sizeof() - 1;
    Int_t run = (Short_t)((TString(fileName(startCh, 4))).Atoi());
    cout << "run = " << run << endl;

    BmnTriggerEfficiencyRun8* pTE2 = new BmnTriggerEfficiencyRun8(run, trigInfo, primaryVertex);
    if (!pTE2->IsValid())
        return;
    Int_t nAnalysableEvents2 = 0;
    Double_t sumEff, sumStatErr, sumSystErr, sumErrSystErr;
    sumEff = sumStatErr = sumSystErr = sumErrSystErr = 0.;

    Int_t nEvents = tree->GetEntries();
    for (Int_t i = 0; i < nEvents; i++) {
        if (i % 1000 == 0)
            cout << i << "/" << nEvents << endl;
        tree->GetEntry(i);

        if (pTE2->IsEventAnalysable()) {
            nAnalysableEvents2++;
            Double_t eff0, statErr0, systErr0, errSystErr0;
            if (pTE2->GetTriggerEfficiencyWithErrors(eff0, statErr0, systErr0, errSystErr0)) {
                // cout << "ev" << i << ": " << eff0 << " " << statErr0 << " " << systErr0 << " " << errSystErr0 <<
                // endl;
                sumEff += eff0;
                sumStatErr += statErr0 * statErr0;
                sumSystErr += systErr0 * systErr0;
                sumErrSystErr += errSystErr0 * errSystErr0;
            } else {
                cout << "Can not get efficiency for event " << i << endl;
            }
        }
    }
    sumStatErr = sqrt(sumStatErr);
    sumSystErr = sqrt(sumSystErr);
    sumErrSystErr = sqrt(sumErrSystErr);

    cout << "nAnalysableEvents2 = " << nAnalysableEvents2 << endl;
    cout << "sumEff = " << sumEff << "+/-" << sumStatErr << "+/-" << sumSystErr << "+/-" << sumErrSystErr << endl;
    delete pTE2;
#endif

#if 0
    // BmnTriggerEfficiencyRun8();
    BmnTriggerEfficiencyRun8 te;
    if (!te.IsValid())
        return;

    // BD
    cout << "EffBD(7842, 15) = " << te.GetBDEfficiency(7842, 15) << endl;
    cout << "EffBD(5000, 15) = " << te.GetBDEfficiency(5000, 15) << endl;
    cout << "EffBD(8000, 75) = " << te.GetBDEfficiency(8000, 75) << endl;

    // Check edges
    cout << "EffBD(6967, 40) = " << te.GetBDEfficiency(6967, 40) << endl;
    cout << "EffBD(6968, 40) = " << te.GetBDEfficiency(6968, 40) << endl;
    cout << "EffBD(8306, 40) = " << te.GetBDEfficiency(8306, 40) << endl;
    cout << "EffBD(8307, 40) = " << te.GetBDEfficiency(8307, 40) << endl;

    cout << "EffBD(8000, 1) = " << te.GetBDEfficiency(8000, 1) << endl;
    cout << "EffBD(8000, 2) = " << te.GetBDEfficiency(8000, 2) << endl;
    cout << "EffBD(8000, 69) = " << te.GetBDEfficiency(8000, 69) << endl;

    // BD
    cout << "EffFD(7842, 15) = " << te.GetFDEfficiency(7842, 15) << endl;
    cout << "EffFD(5000, 15) = " << te.GetFDEfficiency(5000, 15) << endl;
    cout << "EffFD(8000, 75) = " << te.GetFDEfficiency(8000, 75) << endl;

    // Check edges
    cout << "EffFD(6967, 40) = " << te.GetFDEfficiency(6967, 40) << endl;
    cout << "EffFD(6968, 40) = " << te.GetFDEfficiency(6968, 40) << endl;
    cout << "EffFD(8306, 40) = " << te.GetFDEfficiency(8306, 40) << endl;
    cout << "EffFD(8307, 40) = " << te.GetFDEfficiency(8307, 40) << endl;

    cout << "EffFD(8000, 1) = " << te.GetFDEfficiency(8000, 1) << endl;
    cout << "EffFD(8000, 2) = " << te.GetFDEfficiency(8000, 2) << endl;
    cout << "EffFD(8000, 69) = " << te.GetFDEfficiency(8000, 69) << endl;

    // BDxFD
    cout << "EffBDxFD(7842, 15) = " << te.GetTriggerEfficiency(7842, 15) << endl;
    cout << "EffBDxFD(5000, 15) = " << te.GetTriggerEfficiency(5000, 15) << endl;
    cout << "EffBDxFD(8000, 75) = " << te.GetTriggerEfficiency(8000, 75) << endl;

    // Check edges
    cout << "EffBDxFD(6967, 40) = " << te.GetTriggerEfficiency(6967, 40) << endl;
    cout << "EffBDxFD(6968, 40) = " << te.GetTriggerEfficiency(6968, 40) << endl;
    cout << "EffBDxFD(8306, 40) = " << te.GetTriggerEfficiency(8306, 40) << endl;
    cout << "EffBDxFD(8307, 40) = " << te.GetTriggerEfficiency(8307, 40) << endl;

    cout << "EffBDxFD(8000, 1) = " << te.GetTriggerEfficiency(8000, 1) << endl;
    cout << "EffBDxFD(8000, 2) = " << te.GetTriggerEfficiency(8000, 2) << endl;
    cout << "EffBDxFD(8000, 69) = " << te.GetTriggerEfficiency(8000, 69) << endl;

    // GetTriggerEfficiencyWithErrors()
    Double_t eff, statErr, systErr, errSystErr;
    if (te.GetTriggerEfficiencyWithErrors(7842, 15, eff, statErr, systErr, errSystErr)) {
        cout << "EffAndErr(7842, 15): eff = " << eff << ", " << "statErr = " << statErr << ", systErr = " << systErr
             << ", errSystErr = " << errSystErr << endl;
    }
    if (te.GetTriggerEfficiencyWithErrors(5000, 15, eff, statErr, systErr, errSystErr)) {
        cout << "EffAndErr(5000, 15): eff = " << eff << ", " << "statErr = " << statErr << ", systErr = " << systErr
             << ", errSystErr = " << errSystErr << endl;
    }
    if (te.GetTriggerEfficiencyWithErrors(8000, 75, eff, statErr, systErr, errSystErr)) {
        cout << "EffAndErr(8000, 75): eff = " << eff << ", " << "statErr = " << statErr << ", systErr = " << systErr
             << ", errSystErr = " << errSystErr << endl;
    }

    // Check edges
    if (te.GetTriggerEfficiencyWithErrors(6967, 40, eff, statErr, systErr, errSystErr)) {
        cout << "EffAndErr(6967, 40): eff = " << eff << ", " << "statErr = " << statErr << ", systErr = " << systErr
             << ", errSystErr = " << errSystErr << endl;
    }
    if (te.GetTriggerEfficiencyWithErrors(6968, 40, eff, statErr, systErr, errSystErr)) {
        cout << "EffAndErr(6968, 40): eff = " << eff << ", " << "statErr = " << statErr << ", systErr = " << systErr
             << ", errSystErr = " << errSystErr << endl;
    }
    if (te.GetTriggerEfficiencyWithErrors(8306, 40, eff, statErr, systErr, errSystErr)) {
        cout << "EffAndErr(8306, 40): eff = " << eff << ", " << "statErr = " << statErr << ", systErr = " << systErr
             << ", errSystErr = " << errSystErr << endl;
    }
    if (te.GetTriggerEfficiencyWithErrors(8307, 40, eff, statErr, systErr, errSystErr)) {
        cout << "EffAndErr(8307, 40): eff = " << eff << ", " << "statErr = " << statErr << ", systErr = " << systErr
             << ", errSystErr = " << errSystErr << endl;
    }

    if (te.GetTriggerEfficiencyWithErrors(8000, 1, eff, statErr, systErr, errSystErr)) {
        cout << "EffAndErr(8000, 1): eff = " << eff << ", " << "statErr = " << statErr << ", systErr = " << systErr
             << ", errSystErr = " << errSystErr << endl;
    }
    if (te.GetTriggerEfficiencyWithErrors(8000, 2, eff, statErr, systErr, errSystErr)) {
        cout << "EffAndErr(8000, 2): eff = " << eff << ", " << "statErr = " << statErr << ", systErr = " << systErr
             << ", errSystErr = " << errSystErr << endl;
    }
    if (te.GetTriggerEfficiencyWithErrors(8000, 69, eff, statErr, systErr, errSystErr)) {
        cout << "EffAndErr(8000, 69): eff = " << eff << ", " << "statErr = " << statErr << ", systErr = " << systErr
             << ", errSystErr = " << errSystErr << endl;
    }
#endif

#if 1
    /////////////////////////////
    cout << "Use case 3" << endl;
    /////////////////////////////

    BmnTriggerEfficiencyRun8* pTE3 = new BmnTriggerEfficiencyRun8();
    if (!pTE3->IsValid())
        return;
    Int_t runId = 7830;
    Double_t factor, err;
    if (pTE3->GetFluxRejectionFactorWithError(runId, factor, err)) {
        cout << "FluxRejAndErr(7830): factor = " << factor << ", " << "err = " << err << endl;
    } else {
        cout << "Can not get flux rejection factor for run " << runId << endl;
    }

#if 1
    if (pTE3->GetFluxRejectionFactorWithError(6968, factor, err)) {
        cout << "FluxRejAndErr(6968): factor = " << factor << ", " << "err = " << err << endl;
    }
    if (pTE3->GetFluxRejectionFactorWithError(7115, factor, err)) {
        cout << "FluxRejAndErr(7115): factor = " << factor << ", " << "err = " << err << endl;
    }
    if (pTE3->GetFluxRejectionFactorWithError(8306, factor, err)) {
        cout << "FluxRejAndErr(8306): factor = " << factor << ", " << "err = " << err << endl;
    }
#endif

    delete pTE3;
#endif

    return;
}
