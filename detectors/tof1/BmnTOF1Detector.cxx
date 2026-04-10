#include "BmnTOF1Detector.h"

#include <iomanip>

BmnTOF1Detector::BmnTOF1Detector() {}

BmnTOF1Detector::BmnTOF1Detector(Int_t tofId, Int_t NPlane, Int_t fill_hist = 0, Int_t Verbose = 0)
{
    // cout << "BmnTOF1Detector::BmnTOF1Detector  tofId = " << tofId << "; NPlane = " << NPlane << endl;
    fTofId = (DetectorId)tofId;
    fVerbose = Verbose;
    memset(fKilled, 0, sizeof(fKilled));
    memset(fCorrLR, 0, sizeof(fCorrLR));
    memset(fCorrTimeShift, 0, sizeof(fCorrTimeShift));
    fKillSide = -1;
    fNEvents = 0;
    // KillStrip(0);
    // KillStrip(47);
    fFillHist = fill_hist;
    fNPlane = NPlane;
    fNStr = fNStrMax;
    fStripLength = 30;        // cm
    fSignalVelosity = 0.06;   // 0.06 ns/cm
    if (fNPlane >= 41) {
        fStripLength = 56;
        fSignalVelosity = 0.0585;
    }
    fMaxDelta = (fStripLength * 0.5 + 3.0) * fSignalVelosity;   // + 20 mm on the strip edge
    fCommonTimeShift = 0;

    for (Int_t i = 0; i < fNStrMax; i++) {
        fSignalVelosityStr[i] = fSignalVelosity;
        gSlew[i] = NULL;
        funT0[i] = NULL;
        funRPC[i] = NULL;
    }

    if (fTofId == kTOF1)
        fName = Form("TOF400_Plane_%d", NPlane);
    if (fTofId == kTOF701)
        fName = Form("TOF700_Plane_%d", NPlane);
    TString Name;

    if (fFillHist > 0) {
        fHistListStat = new TList();
        fHistListdt = new TList();

        Name.Clear();
        Name = Form("Hist_HitByCh_%s", fName.Data());
        hHitByCh = new TH1I(Name, Name, fNStrMax + 1, -0.5, fNStrMax + 0.5);
        fHistListStat->Add(hHitByCh);

        Name.Clear();
        Name = Form("Hist_HitPerEv_%s", fName.Data());
        hHitPerEv = new TH1I(Name, Name, fNStrMax + 1, -0.5, fNStrMax + 0.5);
        fHistListStat->Add(hHitPerEv);

        Name.Clear();
        Name = Form("Hist_HitLR_%s", fName.Data());
        hHitLR = new TH2I(Name, Name, fNStrMax, 0, fNStrMax, fNStrMax, 0, fNStrMax);
        fHistListStat->Add(hHitLR);

        Name.Clear();
        Name = Form("Hist_XY_%s", fName.Data());
        hXY = new TH2I(Name, Name, 400, -250, 250, 120, -75, 75);
        fHistListStat->Add(hXY);

        hDy_near = new TH1S(Form("hDy_near_%s", fName.Data()), Form("hDy_near_%s", fName.Data()), 400, -20, 20);
        hDtime_near =
            new TH1S(Form("hDtime_near_%s", fName.Data()), Form("hDtime_near_%s", fName.Data()), 400, -10, 10);
        hDWidth_near =
            new TH1S(Form("hDWidth_near_%s", fName.Data()), Form("hDWidth_near_%s", fName.Data()), 256, -28., 28.);
        hTempDtimeDy_near = new TH2S(Form("hTempDtimeDy_near_%s", fName.Data()),
                                     Form("hTempDtimeDy_near_%s", fName.Data()), 400, -10, 10, 200, -10, 10);
        hDy_acros = new TH1S(Form("hDy_acros_%s", fName.Data()), Form("hDy_acros_%s", fName.Data()), 400, -20, 20);
        hDtime_acros =
            new TH1S(Form("hDtime_acros_%s", fName.Data()), Form("hDtime_acros_%s", fName.Data()), 400, -10, 10);
        hDWidth_acros =
            new TH1S(Form("hDWidth_acros_%s", fName.Data()), Form("hDWidth_acros_%s", fName.Data()), 256, -28., 28.);
        hTempDtimeDy_acros = new TH2S(Form("hTempDtimeDy_acros_%s", fName.Data()),
                                      Form("hTempDtimeDy_acros_%s", fName.Data()), 400, -10, 10, 200, -10, 10);
        fHistListStat->Add(hDy_near);
        fHistListStat->Add(hDtime_near);
        fHistListStat->Add(hDWidth_near);
        fHistListStat->Add(hTempDtimeDy_near);
        fHistListStat->Add(hDy_acros);
        fHistListStat->Add(hDtime_acros);
        fHistListStat->Add(hDWidth_acros);
        fHistListStat->Add(hTempDtimeDy_acros);

        for (Int_t i = 0; i < fNStrMax + 1; i++) {
            hdT_vs_WidthDet[i] =
                new TH2S(Form("dt_vs_WidthDet_str_%d_%s", i, fName.Data()),
                         Form("dt_vs_WidthDet_str_%d_%s", i, fName.Data()), 1024, 0, 50, 1024, -12, 12);
            fHistListdt->Add(hdT_vs_WidthDet[i]);
        }
        for (Int_t i = 0; i < fNStrMax + 1; i++) {
            hdT_vs_WidthT0[i] = new TH2S(Form("dt_vs_WidthT0_str_%d_%s", i, fName.Data()),
                                         Form("dt_vs_WidthT0_str_%d_%s", i, fName.Data()), 1024, 0, 50, 1024, -12, 12);
            fHistListdt->Add(hdT_vs_WidthT0[i]);
        }
        for (Int_t i = 0; i < fNStrMax + 1; i++) {
            hdT[i] =
                new TH1I(Form("dt_str_%d_%s", i, fName.Data()), Form("dt_str_%d_%s", i, fName.Data()), 1024, -12, 12);
            fHistListdt->Add(hdT[i]);
        }

    } else {

        hHitByCh = NULL;
        hHitPerEv = NULL;
        hHitLR = NULL;
        hXY = NULL;

        hDy_near = NULL;
        hDtime_near = NULL;
        hDWidth_near = NULL;
        hTempDtimeDy_near = NULL;
        hDy_acros = NULL;
        hDtime_acros = NULL;
        hDWidth_acros = NULL;
        hTempDtimeDy_acros = NULL;

        for (Int_t i = 0; i < fNStrMax + 1; i++) {
            hdT_vs_WidthDet[i] = NULL;
            hdT_vs_WidthT0[i] = NULL;
            hdT[i] = NULL;
        }
    }
    Clear();
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::Clear()
{
    memset(fTimeL, 0, sizeof(fTimeL));
    memset(fTimeR, 0, sizeof(fTimeR));
    memset(fTimeLtemp, 0, sizeof(fTimeLtemp));
    memset(fTimeRtemp, 0, sizeof(fTimeRtemp));
    memset(fTime, 0, sizeof(fTime));
    memset(fWidthL, 0, sizeof(fWidthL));
    memset(fWidthR, 0, sizeof(fWidthR));
    memset(fWidthLtemp, 0, sizeof(fWidthLtemp));
    memset(fWidthRtemp, 0, sizeof(fWidthRtemp));
    memset(fWidth, 0, sizeof(fWidth));
    memset(fFlagHit, 0, sizeof(fFlagHit));
    memset(fTof, 0, sizeof(fTof));
    memset(fDigitL, 0, sizeof(fDigitL));
    memset(fDigitR, 0, sizeof(fDigitR));
    memset(fHit, 0, sizeof(fHit));
    memset(fIndexL, 0, sizeof(fIndexL));
    memset(fIndexR, 0, sizeof(fIndexR));
    fHit_Per_Ev = 0;

    vHitsL.clear();
    vHitsL.reserve(fNStrMax);
    vHitsL.resize(fNStrMax);

    vHitsR.clear();
    vHitsR.reserve(fNStrMax);
    vHitsR.resize(fNStrMax);

    for (Int_t i = 0; i < fNStrMax; i++) {
        fCrossPoint[i].SetXYZ(0., 0., 0.);
        fIndexLtemp[i] = -1;
        fIndexRtemp[i] = -1;
    }
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetDigitNew(BmnTof1Digit* TofDigit, Int_t ind)
{
    fStrip = TofDigit->GetStrip();

    if (fStrip < 0 || fStrip > fNStr || fKilled[fStrip])
        return kFALSE;

    if (fVerbose > 3)
        cout << endl
             << " Plane = " << TofDigit->GetPlane() << "; Strip " << TofDigit->GetStrip() << "; Side "
             << TofDigit->GetSide() << "; Time " << TofDigit->GetTime() << "; Amp " << TofDigit->GetAmplitude() << endl;

    if (TofDigit->GetSide() == 0 && fKillSide != 0)
        fTimeLtemp[fStrip] = TofDigit->GetTime() - 2. * fCorrLR[fStrip];
    else if (TofDigit->GetSide() == 1 && fKillSide != 1)
        fTimeLtemp[fStrip] = TofDigit->GetTime();
    else
        return kFALSE;

    if (fVerbose > 3)
        cout << "Setting Shift: strip # " << fStrip << " shift val " << fCorrLR[fStrip] << "; shifted timeL "
             << fTimeLtemp[fStrip] << "\n";

    fWidthLtemp[fStrip] = TofDigit->GetAmplitude();

    fIndexLtemp[fStrip] = ind;

    std::vector<Double_t> tempVector;
    tempVector.reserve(3);
    tempVector.push_back(fTimeLtemp[fStrip]);
    tempVector.push_back(fWidthLtemp[fStrip]);
    tempVector.push_back(ind);

    if (TofDigit->GetSide() == 0) {
        vHitsL.at(fStrip).push_back(tempVector);
        fDigitL[fStrip]++;

        if (fKillSide == 1)
            vHitsR.at(fStrip).push_back(tempVector);

        return kTRUE;
    } else {
        vHitsR.at(fStrip).push_back(tempVector);
        fDigitR[fStrip]++;

        if (fKillSide == 0)
            vHitsL.at(fStrip).push_back(tempVector);

        return kTRUE;
    }

    return kFALSE;
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::KillStrip(Int_t NumberOfStrip)
{
    fKilled[NumberOfStrip] = kTRUE;
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::KillSide(Int_t NumberOfSide)
{
    fKillSide = NumberOfSide;
}

//----------------------------------------------------------------------------------------

Int_t BmnTOF1Detector::FindHitsNew(BmnTrigDigit* T0, TClonesArray* TofHit, int printnow)
{
    fT0 = T0;
    if (fT0 == NULL)
        fT0 = new BmnTrigDigit(-1, 0, 20, -1);

    fNEvents++;

    if (printnow != 0)
        cout << endl << "   Plane " << fNPlane << endl;
    for (Int_t i = 0; i < fNStr; i++) {
        if (vHitsL[i].empty() || vHitsR[i].empty())
            continue;
        if (printnow != 0)
            cout << "   Strip " << i << endl;

        for (Int_t iL = 0; iL < (int)vHitsL.at(i).size(); iL++)
            for (Int_t iR = 0; iR < (int)vHitsR.at(i).size(); iR++) {

                fTimeL[i] = vHitsL.at(i).at(iL).at(0);
                fTimeR[i] = vHitsR.at(i).at(iR).at(0);

                fWidthL[i] = vHitsL.at(i).at(iL).at(1);
                fWidthR[i] = vHitsR.at(i).at(iR).at(1);

                fIndexL[i] = vHitsL.at(i).at(iL).at(2);
                fIndexR[i] = vHitsR.at(i).at(iR).at(2);

                if (printnow != 0) {
                    cout << "iL = " << iL << ": time = " << fTimeL[i] << "; amp = " << fWidthL[i]
                         << "; ind = " << fIndexL[i] << endl;
                    cout << "iR = " << iR << ": time = " << fTimeR[i] << "; amp = " << fWidthR[i]
                         << "; ind = " << fIndexR[i] << endl;
                }
                if (!GetCrossPoint(i, fTimeL[i], fTimeR[i]))
                    continue;

                fWidth[i] = fWidthL[i] + fWidthR[i];
                fTime[i] = (fTimeL[i] + fTimeR[i]) * 0.5;
                fTof[i] = CalculateDt(i);

                fHit_Per_Ev++;

                if (fVerbose > 2 || printnow != 0)
                    printf("Hit on Plane#%d  Strip#%d\n\tTime = %.3f; (X,Y,Z) = %.3f, %.3f, %.3f\n", fNPlane, i,
                           fTof[i], fCrossPoint[i].X(), fCrossPoint[i].Y(), fCrossPoint[i].Z());
                if (fFillHist > 0) {
                    hdT_vs_WidthDet[i]->Fill(fWidth[i], fTof[i]);
                    hdT_vs_WidthT0[i]->Fill(fT0->GetAmp(), fTof[i]);
                    hdT[i]->Fill(fTof[i]);
                    hdT_vs_WidthDet[fNStr]->Fill(fWidth[i], fTof[i]);
                    hdT_vs_WidthT0[fNStr]->Fill(fT0->GetAmp(), fTof[i]);
                    hdT[fNStr]->Fill(fTof[i]);
                    if (i > 2)
                        if (fFlagHit[i - 1] == kTRUE) {
                            hDy_near->Fill(fCrossPoint[i].Y() - fCrossPoint[i - 1].Y());
                            hDtime_near->Fill(fTof[i] - fTof[i - 1]);
                            hDWidth_near->Fill(fWidth[i] - fWidth[i - 1]);
                            hTempDtimeDy_near->Fill(fTof[i] - fTof[i - 1], fCrossPoint[i].Y() - fCrossPoint[i - 1].Y());
                        }
                    if (i > 3)
                        if (fFlagHit[i - 2] == kTRUE) {
                            hDy_acros->Fill(fCrossPoint[i].Y() - fCrossPoint[i - 2].Y());
                            hDtime_acros->Fill(fTof[i] - fTof[i - 2]);
                            hDWidth_acros->Fill(fWidth[i] - fWidth[i - 2]);
                            hTempDtimeDy_acros->Fill(fTof[i] - fTof[i - 2],
                                                     fCrossPoint[i].Y() - fCrossPoint[i - 2].Y());
                        }
                }
                TString Name = TofHit->GetClass()->GetName();
                if (Name == "BmnTofHit") {
                    //            cout << " Fill BmnTofHit" << endl;
                    AddHit(i, TofHit);
                } else if (Name == "BmnTOF1Conteiner") {
                    //           cout << " Fill BmnTOF1Conteiner" << endl;
                    AddConteiner(i, TofHit);
                }
            }   // end loop over Left/Right sides of the strip
    }   // end loop over strips for (Int_t i = 0; i < fNStrMax; i++)

    if (printnow != 0)
        getchar();
    if (fFillHist > 0)
        FillHist();

    if (fVerbose > 2)
        printf("Total number of hits on Plane#%d is %d\n\n", fNPlane, fHit_Per_Ev);
    return fHit_Per_Ev;
}

//------------------------------------------------------------------------------------------------------------------------

void BmnTOF1Detector::AddHit(Int_t Str, TClonesArray* TofHit)
{

    fVectorTemp.SetXYZ(0.5, 0.36, 1.);   // error for point dx = 0.5 cm; dy = 1.25/SQRT(12) = 0.36 cm; dz = 1(?)cm
    Int_t UID = BmnTOF1Point::GetVolumeUID(0, fNPlane + 1, Str + 1);   // strip [0,47] -> [1, 48]

    BmnTofHit* pHit = new ((*TofHit)[TofHit->GetEntriesFast()])
        BmnTofHit(UID, fCrossPoint[Str], fVectorTemp, FormIndex(fIndexL[Str], fIndexR[Str]));

    pHit->SetModule(fNPlane);
    pHit->SetStation(Str);
    pHit->SetTimeStamp(fTof[Str]);
    pHit->AddLink(FairLink(0x1, -1));
    pHit->AddLink(FairLink(0x2, -1));
    pHit->AddLink(FairLink(0x4, UID));
    pHit->SetUpperClusterIndex(fIndexL[Str]);
    pHit->SetLowerClusterIndex(fIndexR[Str]);
    pHit->SetDetId(fTofId);

    //  cout << "Add hit to Array# " << TofHit->GetEntriesFast() << " : fNPlane = " << fNPlane << "; Str = " << Str
    //       << "; iR = " << fIndexL[Str] << "; iR = " << fIndexR[Str] << "; pointIndex";
}

//------------------------------------------------------------------------------------------------------------------------

void BmnTOF1Detector::AddConteiner(Int_t Str, TClonesArray* TofHit)
{

    new ((*TofHit)[TofHit->GetEntriesFast()])
        BmnTOF1Conteiner(fNPlane, Str, fTimeL[Str], fTimeR[Str], fTime[Str], fWidthL[Str], fWidthR[Str], fWidth[Str],
                         fCrossPoint[Str].x(), fCrossPoint[Str].y(), fCrossPoint[Str].z(), fT0->GetTime(),
                         fT0->GetAmp(), fTof[Str], fIndexL[Str], fIndexR[Str]);
}

//----------------------------------------------------------------------------------------

void BmnTOF1Detector::FillHist()
{
    hHitPerEv->Fill(fHit_Per_Ev);
    for (Int_t i = 0; i < fNStr; i++)
        for (Int_t j = 0; j < fNStr; j++) {
            if (fWidthL[i] != 0 && fWidthR[j] != 0) {
                hHitLR->Fill(i, j);
                if (i == j) {
                    hHitByCh->Fill(i);
                    hXY->Fill(fCrossPoint[i].x(), fCrossPoint[i].y());
                }
            }
        }
}

//----------------------------------------------------------------------------------------

Double_t BmnTOF1Detector::CalculateDt(Int_t Str = 0)
{

    Double_t dt = 0;
    Double_t T0Amp;
    dt = fTime[Str] - fT0->GetTime() + fCommonTimeShift;
    if (fVerbose > 3)
        printf("Calculate dt\n     raw      dt = %.3f\n", dt);
    T0Amp = fT0->GetAmp();

    /*dt = dt - (1.947 - 0.5363 * T0Amp
            + 0.03428 * T0Amp * T0Amp
            - 0.0005853 * T0Amp * T0Amp * T0Amp);// RUN6 */

    if (funT0[Str] != NULL)
        dt = dt - funT0[Str]->Eval(T0Amp);
    if (fVerbose > 3)
        printf("After T0 corr dt = %.3f\t", dt);

    if (gSlew[Str] != NULL) {
        dt = dt - gSlew[Str]->Eval(fWidth[Str]);
        if (fVerbose > 3)
            printf("After RPC corr dt = %.3f\t", dt);
    }

    if (funRPC[Str] != NULL) {
        dt = dt - funRPC[Str]->Eval(fWidth[Str]);
        if (fVerbose > 3)
            printf("After RPC corr dt = %.3f\t", dt);
    }

    dt = dt + fCorrTimeShift[Str];   // CorrTimeShift is ToF for Gamma
    if (fVerbose > 3)
        printf("After Shift on Proton mass dt = %.3f\n", dt);

    // cout << dt << endl;
    return dt;
}

//----------------------------------------------------------------------------------------

TList* BmnTOF1Detector::GetList(Int_t n = 0)
{
    if (fFillHist > 0) {
        if (n == 0)
            return fHistListStat;
    } else
        return NULL;

    return NULL;
}

//----------------------------------------------------------------------------------------

TString BmnTOF1Detector::GetName()
{
    return fName;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetCorrLR(TString NameFile)
{
    // cout << "BmnTOF1Detector::SetCorrLR" << endl;
    char line[256];
    Int_t Pl, St;
    Double_t CorrFit, CorrMean;
    ifstream f_corr;
    TString dir = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", NameFile.Data());
    // cout << "Open file " << dir << endl;
    f_corr.open(dir);
    f_corr.getline(line, 256);
    f_corr.getline(line, 256);
    if (f_corr.is_open() == kTRUE) {
        while (!f_corr.eof()) {
            if (fTofId == kTOF1)
                f_corr >> Pl >> St >> CorrFit >> CorrMean;
            if (fTofId == kTOF701)
                f_corr >> Pl >> St >> CorrMean;
            if (Pl == fNPlane) {
                fCorrLR[St] = CorrMean;
                // If diff between my shift and old shift is greater than the actual cable, throw
                // strip out
                // if (TMath::Abs(Temp - fCorrLR[St]) > 2.) fCorrLR[St] = -11.9766;
                // cout << Pl << " " << St << " " << fCorrLR[St] << " " << CorrMean << "\n";
            }
        }
    } else {
        cout << "File " << NameFile.Data() << " for LR correction is not found" << endl;
        cout << "Check " << dir.Data() << " folder for file" << endl;
        return kFALSE;
    }
    f_corr.close();
    return kTRUE;
}
//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetCorrSlewing(TString NameFile)
{
    TString PathToFile = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", NameFile.Data());
    TString name, dirname;
    TDirectory* Dir;
    TFile* f_corr = new TFile(PathToFile.Data(), "READ");
    if (f_corr->IsOpen()) {
        dirname = Form("Plane_%d", fNPlane);
        f_corr->cd(dirname.Data());
        Dir = f_corr->CurrentDirectory();
        for (Int_t i = 0; i < fNStr; i++) {
            name = Form("T0_TA_Plane%d_Str%d", fNPlane, i);
            funT0[i] = (TF1*)Dir->Get(name.Data());
            if (funT0[i] == NULL && fVerbose >= 1)
                printf("funT0[%d] is NULL\n", i);
            name = Form("Rpc_TA_Plane%d_Str%d", fNPlane, i);
            gSlew[i] = (TGraphErrors*)Dir->Get(name.Data());
            if (gSlew[i] == NULL && fVerbose >= 1)
                printf("gSlew[%d] is NULL\n", i);
        }
    } else {
        cout << "File " << NameFile.Data() << " for Slewing correction is not found" << endl;
        cout << "Check " << PathToFile.Data() << " folder for file" << endl;
        return kFALSE;
    }
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetCorrTimeShift(TString NameFile)
{
    char line[256];
    Int_t Pl, St;
    Double_t Temp;
    ifstream f_corr;
    TString dir = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", NameFile.Data());
    f_corr.open(dir);
    f_corr.getline(line, 256);
    // cout << line << endl;

    f_corr.getline(line, 256);
    // cout << line << endl;
    fCommonTimeShift = atof(line);
    // cout << fCommonTimeShift << endl;

    f_corr.getline(line, 256);
    // cout << line << endl;

    // char ccc = getchar();
    // if (ccc == 'q') return kFALSE;

    if (f_corr.is_open() == kTRUE) {
        while (!f_corr.eof()) {
            f_corr >> Pl >> St >> Temp;
            if (Pl == fNPlane) {
                fCorrTimeShift[St] = Temp;
                // cout << Pl << " " << St << " " << fCorrTimeShift[St] << "\n";
            }
        }
    } else {
        cout << "File " << NameFile.Data() << " for TimeShift correction is not found" << endl;
        cout << "Check " << dir.Data() << " folder for file" << endl;
        return kFALSE;
    }
    f_corr.close();
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::GetCrossPoint(Int_t NStrip, Double_t tL, Double_t tR)
{

    fVectorTemp.SetXYZ(0., 0., 0.);

    double dtt = (tL - tR) * 0.5;
    double dL = dtt / fSignalVelosityStr[NStrip];
    // cout << "fSignalVelosityStr[NStrip] = " << fSignalVelosityStr[NStrip] << endl;
    // cout << "dl = " << dL << endl;
    if (TMath::Abs(dtt) > fMaxDelta)
        return kFALSE;
    // cout << "accept this hit" << endl;
    // should be checked
    fVectorTemp(0) = dL * TMath::Cos(fStripAngle[NStrip].X());
    fVectorTemp(1) = dL * TMath::Cos(fStripAngle[NStrip].Y());
    fVectorTemp(2) = dL * TMath::Cos(fStripAngle[NStrip].Z());

    fCrossPoint[NStrip].SetXYZ(0., 0., 0.);
    fCrossPoint[NStrip] = fCentrStrip[NStrip] + fVectorTemp;

    return kTRUE;
    //    cout << "Z = " << fCrossPoint[NStrip].Z() << endl;
    //    return kTRUE;
}
//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetGeoFile(TString NameFile)
{

    // get gGeoManager from ROOT file
    TString PathToFile = Form("%s%s%s", getenv("VMCWORKDIR"), "/macro/run/geometry_run/", NameFile.Data());
    TFile* geoFile = new TFile(PathToFile, "READ");
    if (!geoFile->IsOpen()) {
        cout << "Error: could not open ROOT file with geometry: " + NameFile << endl;
        return kFALSE;
    }
    TList* keyList = geoFile->GetListOfKeys();
    TIter next(keyList);
    TKey* key = (TKey*)next();
    TString className(key->GetClassName());
    if (className.BeginsWith("TGeoManager"))
        key->ReadObj();
    else {
        cout << "Error: TGeoManager isn't top element in geometry file " + NameFile << endl;
        return kFALSE;
    }

    BmnTof1GeoUtils* pGeoUtils = new BmnTof1GeoUtils();
    if (fTofId == kTOF1)
        pGeoUtils->ParseTGeoManager(false, NULL, true);
    else if (fTofId == kTOF701)
        pGeoUtils->ParseTGeoManager701(false, NULL, true);
    else
        return kFALSE;

    SetGeo(pGeoUtils);

    geoFile->Close();
    pGeoUtils->~BmnTof1GeoUtils();
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetGeo(BmnTof1GeoUtils* pGeoUtils)
{
    Int_t UID;
    TVector3 x(1, 0, 0);
    TVector3 y(0, 1, 0);
    TVector3 z(0, 0, 1);
    for (Int_t i = 0; i < fNStrMax; i++) {
        UID = BmnTOF1Point::GetVolumeUID(0, fNPlane + 1, i + 1);   // strip [0,47] -> [1, 48]
        // cout << "Plane = " << fNPlane << "; Strip = " << i << "; UID = " << UID << endl;
        const LStrip1* pStrip = pGeoUtils->FindStrip(UID);
        if (pStrip == NULL) {
            fNStr = i;
            break;
        }
        if (fTofId == kTOF701)
            SetStripLength(pStrip->GetLength());
        fCentrStrip[i] = pStrip->center;

        if (fTofId == kTOF1)
            fVectorTemp = (pStrip->C + pStrip->D) * 0.5 - (pStrip->A + pStrip->B) * 0.5;
        else if (fTofId == kTOF701)
            fVectorTemp = (pStrip->B + pStrip->C) * 0.5 - (pStrip->A + pStrip->D) * 0.5;
        else
            return kFALSE;

        fStripAngle[i](0) = fVectorTemp.Angle(x);
        fStripAngle[i](1) = fVectorTemp.Angle(y);
        fStripAngle[i](2) = fVectorTemp.Angle(z);

        // printf("Plane%d, Strip%d\t A=%.3f\t B=%.3f\t C=%.3f\n", fNPlane, i, fStripAngle[i](0), fStripAngle[i](1),
        //        fStripAngle[i](2));

        //        if (fNPlane >= 5) fCentrStrip[i].SetX(fCentrStrip[i].X()+5.5); // for field run only
        //        else fCentrStrip[i].SetX(fCentrStrip[i].X()+2.5);
    }
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::GetXYZTime(Int_t Str, TVector3* XYZ, Double_t* ToF)
{

    if (Str < 0 || Str > fNStr)
        return kFALSE;
    if (NULL != XYZ)
        XYZ->SetXYZ(fCrossPoint[Str].x(), fCrossPoint[Str].y(), fCrossPoint[Str].z());
    if (NULL != ToF)
        *ToF = fTof[Str];

    return kTRUE;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::GetLRTime(Int_t Str, Double_t* LMinusRTime)
{

    //   if (fTof[Str] == 0) return kFALSE;
    if (NULL == LMinusRTime)
        return kFALSE;
    *LMinusRTime = (fTimeL[Str] - fTimeR[Str]) * 0.5;
    return kTRUE;
}

//----------------------------------------------------------------------------------------

Double_t BmnTOF1Detector::GetWidth(Int_t Str = 1)
{
    return fWidth[Str];
}

//----------------------------------------------------------------------------------------

Double_t BmnTOF1Detector::GetWidthL(Int_t Str = 1)
{
    return fWidthL[Str];
}

//----------------------------------------------------------------------------------------

Double_t BmnTOF1Detector::GetWidthR(Int_t Str = 1)
{
    return fWidthR[Str];
}

//----------------------------------------------------------------------------------------

Double_t BmnTOF1Detector::GetTime(Int_t Str = 1)
{
    return fTime[Str];
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SaveHistToFile(TString NameFile)
{

    if (fFillHist > 0) {
        TFile* fileout = new TFile(NameFile.Data(), "UPDATE");
        // Int_t ResWrite;

        TDirectory* Dir;
        TString Name;
        Name = Form("Tof400_%s", fName.Data());
        Dir = fileout->mkdir(Name.Data());
        Dir->cd();
        // Dir->pwd();

        TDirectory* DirStat;
        DirStat = Dir->mkdir("Statistic");
        DirStat->cd();
        // DirStat->pwd();
        // ResWrite = 0;
        /*ResWrite = */ fHistListStat->Write();
        // cout << "Res write = " << ResWrite << endl;

        TDirectory* DirdT;
        DirdT = Dir->mkdir("dt");
        DirdT->cd();
        // DirStat->pwd();
        // ResWrite = 0;
        /*ResWrite = */ fHistListdt->Write();
        // cout << "Res write = " << ResWrite << endl;

        fileout->Close();   //*/
        return kTRUE;       //*/

    } else
        return kFALSE;
}

//----------------------------------------------------------------------------------------

Int_t BmnTOF1Detector::FormIndex(Int_t IndL, Int_t IndR)
{
    if (IndL > fHalfMaxInt || IndR > fHalfMaxInt)
        return -1;
    Int_t Index = 0;
    Index = (IndL << 16) + IndR;
    return Index;
}

//----------------------------------------------------------------------------------------

Bool_t BmnTOF1Detector::SetSpeedOfSignal(TString NameFile)   // Should be used after BmnTOF1Detector::SetGeo
{
    char line[256];
    Int_t Pl, St;
    Double_t Temp;
    ifstream f_corr;
    TString dir = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", NameFile.Data());
    f_corr.open(dir);
    f_corr.getline(line, 256);
    // cout << line << endl;

    if (f_corr.is_open() == kTRUE) {
        while (!f_corr.eof()) {
            f_corr >> Pl >> St >> Temp;
            if (Pl == fNPlane && St == fNStr) {
                fSignalVelosity = Temp;
                fMaxDelta = (fStripLength * 0.5 + 3.0) * fSignalVelosity;
                // cout << Pl << " " << St << "fStripLength = " << fStripLength << "; Speed = " << fSignalVelosity
                //      << "  MaxDelta = " << fMaxDelta << "\n";
            }
            if (Pl == fNPlane && St < fNStr) {
                fSignalVelosityStr[St] = Temp;
            }
        }
    } else {
        cout << "File " << NameFile.Data() << " for TimeShift correction is not found" << endl;
        cout << "Check " << dir.Data() << " folder for file" << endl;
        return kFALSE;
    }
    f_corr.close();
    return kTRUE;
}

//----------------------------------------------------------------------------------------
