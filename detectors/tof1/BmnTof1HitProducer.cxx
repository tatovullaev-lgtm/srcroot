#include "BmnTof1HitProducer.h"

#include "BmnTOF1Point.h"
#include "BmnTof1Digit.h"
#include "BmnTofHit.h"
#include "CbmMCTrack.h"
#include "FairLogger.h"
#include "FairRootManager.h"

#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TStopwatch.h>
#include <TVector3.h>
#include <iostream>

using namespace std;

static Double_t workTime = 0.0;

//--------------------------------------------------------------------------------------------------------------------------------------

BmnTof1HitProducer::BmnTof1HitProducer(const char* name, DetectorId detId, Bool_t useMCdata, Int_t verbose, Bool_t test)
    : BmnTof1HitProducerIdeal(name, useMCdata, verbose, test)
    , fVerbose(verbose)
    , fTimeSigma(0.100)
    , fErrX(1. / sqrt(12.))
    , fErrY(0.5)
    , fEffFunc(nullptr)
    , pRandom(nullptr)
    , pGeoUtils(nullptr)
    , pDetector(nullptr)
    , fNDetectors(0)
    , fDetectorId(detId)
{
    pGeoUtils = new BmnTof1GeoUtils;
    fEffFunc = new TF1("eff_func", "[0]*x+[1]", 0, 3);
    if (fDetectorId == kTOF1) {
        fDetName = "TOF400";
        fPointsName = "TOF400Point";
        fDigitsName = "TOF400";
        fHitsName = "BmnTof400Hit";
        fErrX = 1. / sqrt(12.);
        fErrY = 0.5;
    }
    if (fDetectorId == kTOF701) {
        fDetName = "TOF700";
        fPointsName = "TOF700Point";
        fDigitsName = "TOF701";
        fHitsName = "BmnTof700Hit";
        fErrX = 0.8;
        fErrY = 0.5;
    }

    if (fDoTest) {
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------

BmnTof1HitProducer::~BmnTof1HitProducer()
{
    if (!fUseMCData) {
        if (pDetector) {
            for (Int_t i = 0; i < fNDetectors; i++) {
                delete pDetector[i];
            }
            delete[] pDetector;
        }
    }
    delete fEffFunc;
    delete pRandom;
    delete pGeoUtils;
}
//--------------------------------------------------------------------------------------------------------------------------------------

InitStatus BmnTof1HitProducer::LoadDetectorConfiguration()
{
    // Parsing geometry
    fNDetectors = -1;
    if (fDetectorId == kTOF1) {
        fNDetectors = pGeoUtils->ParseTGeoManager(fUseMCData, true, fVerbose);
        pGeoUtils->FindNeighborStrips(fVerbose);
    }
    if (fDetectorId == kTOF701) {
        fNDetectors = pGeoUtils->ParseTGeoManager701(fUseMCData, true, fVerbose);
        pGeoUtils->FindNeighborStrips701(fVerbose);
    }

    if (fNDetectors <= 0) {
        cout << this->GetName() << "::LoadDetectorConfiguration(): No " << fDetName
             << " detectors in geometry file for the current run! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    if (fVerbose)
        cout << this->GetName() << "::LoadDetectorConfiguration(): number of " << fDetName
             << " Detectors from geometry file = " << fNDetectors << endl;

    // Init BmnTOF1Detectors
    if (!fUseMCData) {
        if (!SetCorrFiles()) {
            cout << this->GetName()
                 << "::LoadDetectorConfiguration(): No corrections for the current run! Task will be deactivated"
                 << endl;
            SetActive(kFALSE);
            return kERROR;
        }

        pDetector = new BmnTOF1Detector*[fNDetectors];
        for (Int_t i = 0; i < fNDetectors; i++) {
            Int_t DoTestForDetector =
                0;   // For developers only. Level of Histograms filling (0-don't fill, 1-low, 2-high).
            // cout << "Detector # " << i << endl;
            pDetector[i] = new BmnTOF1Detector(fDetectorId, i, DoTestForDetector, fVerbose);
            if (FlagFileLRcorrection)
                pDetector[i]->SetCorrLR(NameFileLRcorrection);
            if (FlagFileSlewingCorrection)
                pDetector[i]->SetCorrSlewing(NameFileSlewingCorrection);
            if (FlagFileTimeShiftCorrection)
                pDetector[i]->SetCorrTimeShift(NameFileTimeShiftCorrection);

            pDetector[i]->SetGeo(pGeoUtils);

            if (fDetectorId == kTOF701)
                pDetector[i]->SetSpeedOfSignal("CorreSpeed.txt");

            if (fPeriod == 6 && fDetectorId == kTOF1) {
                // cout << "!!!!!!!!!!!!!!!! Kill Strip !!!!!!!!!!!!!!!!!!" << endl;
                pDetector[i]->KillStrip(0);
                pDetector[i]->KillStrip(47);
            }
        }
    }

    return kSUCCESS;
}

InitStatus BmnTof1HitProducer::Init()
{
    if (fVerbose)
        printf("\n=================== %s::Init(): Start================\n", this->GetName());

    pRandom = new TRandom2();

    if (fOnlyPrimary)
        cout << " Only primary particles are processed!!! \n";   // FIXME NOT used now ADDD

    if (fUseMCData) {
        aMcPoints = (TClonesArray*)FairRootManager::Instance()->GetObject(fPointsName.Data());
        if (!aMcPoints) {
            cout << this->GetName() << "::Init(): branch " << fPointsName.Data()
                 << " not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
        aMcTracks = (TClonesArray*)FairRootManager::Instance()->GetObject("MCTrack");
        if (!aMcTracks) {
            cout << this->GetName() << "::Init(): branch MCTrack not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
    } else {
        if (fDetectorId != kTOF1 && fDetectorId != kTOF701) {
            cout << this->GetName() << "::Init(): DetectorId is incorrect. Use kTOF1 or kTOF701." << endl;
            cout << "   kTOF1 = " << kTOF1 << "; kTOF701 = " << kTOF701 << "; fDetectorId = " << fDetectorId << endl;
            SetActive(kFALSE);
            return kERROR;
        }
        if (fDetectorId == kTOF701 && fPeriod < 8) {
            std::printf("\x1b[91m"
                        "\n\t!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        "\t!!!!!!!!!!!!!!!   %s   !!!!!!!!!!!!!\n"
                        "\tThis task is not suitable for this period/run (Period #%d, Run #%d).\n"
                        "\tPlease use the legacy code for the Tof700HitProduser task.\n"
                        "\tFor this purpose, use some old(?) version of bmnroot.\n"
                        "\t!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        "\x1b[0m",
                        this->GetName(), fPeriod, fRun);
            SetActive(kFALSE);
            return kERROR;
        }

        fEventHeader = (BmnEventHeader*)FairRootManager::Instance()->GetObject("BmnEventHeader.");

        aExpDigits = (TClonesArray*)FairRootManager::Instance()->GetObject(fDigitsName.Data());
        if (!aExpDigits) {
            cout << this->GetName() << "::Init(): branch" << fDigitsName.Data()
                 << " is not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }

        // looking for T0 branch
        TString NameT0Branch, NameT0Branch2;
        if (fPeriod == 6)
            NameT0Branch = "T0";
        if (fPeriod == 7)
            NameT0Branch = "BC2";
        if (fPeriod == 8) {
            if (fRun >= 968 && fRun <= 1546) {
                NameT0Branch = "T0_1_A";
                NameT0Branch2 = "T0_2_A";
            }
            if (fRun >= 6587 && fRun <= 8427) {
                NameT0Branch = "BC2AT";
                NameT0Branch2 = "BC2AB";
            }
        }
        if (fPeriod == 9) {
            //            if (fRun >= 9289) {
            NameT0Branch = "TQDC_BC2T";
            NameT0Branch2 = "TQDC_BC2B";
            //            }
        }

        LOG(info) << this->GetName() << "::Init(): looking for branch " << NameT0Branch << " for start" << endl;
        aExpDigitsT0 = (TClonesArray*)FairRootManager::Instance()->GetObject(NameT0Branch.Data());
        if (!aExpDigitsT0) {
            printf("%s::Init(): branch %s is not found! Task will be deactivated\n", this->GetName(),
                   NameT0Branch.Data());
            SetActive(kFALSE);
            return kERROR;
        }

        if (fPeriod >= 8) {
            if (fVerbose)
                cout << this->GetName() << "::Init(): looking for branch " << NameT0Branch2 << " for start" << endl;
            aExpDigitsT0_2 = (TClonesArray*)FairRootManager::Instance()->GetObject(NameT0Branch2.Data());
            if (!aExpDigitsT0_2) {
                printf("%s::Init(): branch %s is not found! Task will be deactivated\n", this->GetName(),
                       NameT0Branch2.Data());
                SetActive(kFALSE);
                return kERROR;
            }
        }
    }

    // Create and register output array
    aTofHits = new TClonesArray("BmnTofHit");
    FairRootManager::Instance()->Register(fHitsName.Data(), fDetName.Data(), aTofHits, kTRUE);

    if (LoadDetectorConfiguration() != kSUCCESS) {
        SetActive(kFALSE);
        return kERROR;
    }

    if (fVerbose)
        printf("\n=================== %s::Init(): Initialization finished succesfully. ==\n\n", this->GetName());
    return kSUCCESS;
}
//--------------------------------------------------------------------------------------------------------------------------------------

Bool_t BmnTof1HitProducer::HitExist(const Double_t& val,
                                    const Bool_t& isWide_strip)   // val - distance to the pad edge [cm]
{

    const static Double_t slope = (0.98 - 0.95) / 0.2;
    Double_t efficiency = (val > 0.2) ? 0.98 : (0.95 + slope * val);

    //-------------------------------------
    // 99% ---------
    //              /
    //               /
    //                /
    // 95%             /
    //  <-----------|--|
    //            0.2  0.
    //-------------------------------------

    if (pRandom->Rndm() < efficiency)
        return true;
    return false;
}
//------------------------------------------------------------------------------------------------------------------------

Bool_t BmnTof1HitProducer::DoubleHitExist(const Double_t& val,
                                          const Bool_t& isWide_strip,
                                          const DetectorId& flag)   // val - distance to the pad edge  [cm]
{
    // const static Double_t slope = (0.3 - 0.0) / 0.5;
    // Double_t efficiency = (val > 0.5) ? 0. : (0.3 - slope * val);

    //-------------------------------------
    // 30%                   /|
    //                     /  |
    //                   /    |
    //                 /      |
    // 5%            /        |
    //  <-----------|---------|
    //            width        0.
    //             /2
    //-------------------------------------
    // if (efficiency == 0.)
    // return false;
    if (flag == kTOF701 && isWide_strip) {
        fEffFunc->SetParameters((0.05 - 0.409) / 0.9375, 0.409);
    }

    else if (flag == kTOF701 && !isWide_strip)
        fEffFunc->SetParameters((0.05 - 0.44) / 0.5492, 0.44);

    else
        fEffFunc->SetParameters((0.05 - 0.339) / 0.625, 0.339);

    Double_t efficiency = fEffFunc->Eval(val);

    if (pRandom->Rndm() < efficiency)
        return HitExist(val, isWide_strip);
    return false;
}

Bool_t BmnTof1HitProducer::TripleHitExist(const Double_t& val,
                                          const Bool_t& isWide_strip,
                                          const DetectorId& flag)   // val - distance to the pad edge  [cm]
{

    if (flag == kTOF701 && isWide_strip)
        fEffFunc->SetParameters((0.078 - 0.01) / 0.9375, 0.078);

    else if (flag == kTOF701 && !isWide_strip)
        fEffFunc->SetParameters((0.095 - 0.01) / 0.5492, 0.095);

    else
        fEffFunc->SetParameters((0.075 - 0.01) / 0.625, 0.075);

    Double_t efficiency = fEffFunc->Eval(val);

    if (pRandom->Rndm() < efficiency)
        return HitExist(val, isWide_strip);
    return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::Exec(Option_t* opt)
{
    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;

    if (fVerbose)
        printf("\n=================== %s::Exec(): Started ====================\n", this->GetName());

    aTofHits->Clear();

    Int_t UID, trackID;
    DetectorId detID;
    TVector3 pos, XYZ_smeared;
    int nSingleHits = 0, nDoubleHits = 0, nTripleHits = 0;

    if (fUseMCData) {
        for (Int_t pointIndex = 0, nTofPoint = aMcPoints->GetEntriesFast(); pointIndex < nTofPoint;
             pointIndex++)   // cycle by TOF points
        {
            BmnTOF1Point* pPoint = (BmnTOF1Point*)aMcPoints->UncheckedAt(pointIndex);
            if (fVerbose > 2)
                pPoint->Print("");

            trackID = pPoint->GetTrackID();
            UID = pPoint->GetVolumeUID();
            detID = (DetectorId)pPoint->GetDetectorID();
            Double_t time = pRandom->Gaus(pPoint->GetTime(), fTimeSigma);   // 100 ps
            pPoint->Position(pos);

            const LStrip1* pStrip = pGeoUtils->FindStrip(UID);
            const LStrip1* const pcStrip = pStrip;
            if (pStrip == NULL) {
                printf("UID is incorrect: UID = %d", UID);
                break;
            }

            Bool_t isWide_strip = false;
            if (detID == kTOF701) {
                isWide_strip = BmnTOF1Point::GetModule(UID) > 41 ? true : false;
            }
            switch (detID) {
                case kTOF1:
                    XYZ_smeared.SetXYZ(pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY), pStrip->center.Z());
                    break;
                case kTOF701:
                    if (isWide_strip)
                        fErrY = 1.875 / sqrt(12.);
                    else {
                        fErrY = 1.09 / sqrt(12.);
                    }
                    XYZ_smeared.SetXYZ(pRandom->Gaus(pos.X(), fErrX), pStrip->center.Y(), pStrip->center.Z());
                    break;
                default:
                    printf("!!!!!   WRONG DetectorID for %s reconstruction\n", fDetName.Data());
                    continue;
                    break;
            }

            static const TVector3 XYZ_err(fErrX, fErrY, 0.);

            LStrip1::Side_t side;
            Double_t distance = 1000;
            if (detID == kTOF1)
                distance = pStrip->MinDistanceToEdge(&pos, side);   // [cm]
            if (detID == kTOF701)
                distance = pStrip->MinDistanceToEdge701(&pos, side);   // [cm]

            // pStrip->Dump();
            // cout << " neighboring[0] = " << pStrip->neighboring[0]<< " neighboring[1] = " << pStrip->neighboring[1]
            // << endl;
            //     cout << "side = " << side << endl;

            bool passed;
            if ((passed = HitExist(distance, isWide_strip)) == true)   // check efficiency
            {
                if (pStrip->GetIsKilled() == false) {
                    // printf("\nadding hit\tMChitNo = %d,\tmod = %d,\tstr = %d\n", pointIndex,
                    // BmnTOF1Point::GetModule(UID), BmnTOF1Point::GetStrip(UID) );
                    AddHit(UID, XYZ_smeared, XYZ_err, pointIndex, trackID, time, detID);
                    nSingleHits++;
                }
            }

            // printf("UID:\t\t\tside = %d\tmod = %d,\tstrip = %d\n", side, BmnTOF1Point::GetModule(UID),
            // BmnTOF1Point::GetStrip(UID) ); printf("CrossUID:\t\tside = %d\tmod = %d,\tstrip = %d\n", side,
            // BmnTOF1Point::GetModule(CrossUID), BmnTOF1Point::GetStrip(CrossUID) ); printf("CrossUID_opp:\tside =
            // %d\tmod = %d,\tstrip = %d\n\n", side, BmnTOF1Point::GetModule(CrossUID_opposite),
            // BmnTOF1Point::GetStrip(CrossUID_opposite) );

            if (passed && DoubleHitExist(distance, isWide_strip, detID))   // check cross hit
            {
                // cout << "DoubleHit = true pStripUID = " << endl;
                Int_t CrossUID = -1;
                CrossUID = (side == LStrip1::kUpper) ? pcStrip->neighboring[LStrip1::kUpper]
                                                     : pcStrip->neighboring[LStrip1::kLower];
                CrossUID = (side == LStrip1::kRight) ? pcStrip->neighboring[LStrip1::kRight]
                                                     : pcStrip->neighboring[LStrip1::kLeft];

                // cout << "Check neighbording strip: CrossUID = " << CrossUID << endl;

                // if (LStrip1::kInvalid == CrossUID)
                // continue;   // last strip on module

                if (CrossUID != LStrip1::kInvalid) {
                    pStrip = pGeoUtils->FindStrip(CrossUID);
                    if (detID == kTOF1)
                        XYZ_smeared.SetXYZ(pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY), pStrip->center.Z());
                    else if (detID == kTOF701)
                        XYZ_smeared.SetXYZ(pRandom->Gaus(pos.X(), fErrX), pStrip->center.Y(), pStrip->center.Z());
                    else {   // unexpected case
                        printf("!!!!!   WRONG DetectorID for %s reconstruction\n", fDetName.Data());
                        continue;
                    }
                    // cout << " Save DoubleHit " << endl;
                    if (pStrip->GetIsKilled() == false
                        && BmnTOF1Point::GetModule(CrossUID) == BmnTOF1Point::GetModule(UID))
                    {
                        // printf("adding dit\tMChitNo = %d,\tmod = %d,\tstr = %d\n", pointIndex,
                        // BmnTOF1Point::GetModule(CrossUID), BmnTOF1Point::GetStrip(CrossUID) );
                        AddHit(CrossUID, XYZ_smeared, XYZ_err, pointIndex, trackID, time, detID);
                        nDoubleHits++;
                    }
                }
            } else {
                passed = false;
            }

            if ((passed && TripleHitExist(distance, isWide_strip, detID)) == true) {
                Int_t CrossUID = -1;

                CrossUID = (side == LStrip1::kUpper) ? pcStrip->neighboring[LStrip1::kLower]
                                                     : pcStrip->neighboring[LStrip1::kUpper];
                CrossUID = (side == LStrip1::kRight) ? pcStrip->neighboring[LStrip1::kLeft]
                                                     : pcStrip->neighboring[LStrip1::kRight];

                if (CrossUID != LStrip1::kInvalid) {
                    pStrip = pGeoUtils->FindStrip(CrossUID);
                    if (detID == kTOF1)
                        XYZ_smeared.SetXYZ(pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY), pStrip->center.Z());
                    else if (detID == kTOF701)
                        XYZ_smeared.SetXYZ(pRandom->Gaus(pos.X(), fErrX), pStrip->center.Y(), pStrip->center.Z());
                    else {
                        printf("!!!!!   WRONG DetectorID for %s reconstruction\n", fDetName.Data());
                        continue;
                    }
                    // printf("adding tit\tMChitNo = %d,\tmod = %d,\tstr = %d\n", pointIndex,
                    // BmnTOF1Point::GetModule(CrossUID), BmnTOF1Point::GetStrip(CrossUID) );
                    AddHit(CrossUID, XYZ_smeared, XYZ_err, pointIndex, trackID, time, detID);
                    nTripleHits++;
                }
            }

        }   // cycle by the TOF points
    } else {
        BmnTrigDigit* digT0 = FingT0Digit();

        for (Int_t i = 0; i < fNDetectors; i++)
            pDetector[i]->Clear();

        for (Int_t iDig = 0; iDig < aExpDigits->GetEntriesFast(); ++iDig) {

            BmnTof1Digit* digTof = (BmnTof1Digit*)aExpDigits->At(iDig);
            // cout << "SETTING PLANE " << digTof->GetPlane() << "\n";
            if (!OutOfRange(digTof->GetPlane())) {
                //                pDetector[digTof->GetPlane()]->SetDigit(digTof, iDig);
                pDetector[digTof->GetPlane()]->SetDigitNew(digTof, iDig);
            }
        }

        for (Int_t i = 0; i < fNDetectors; i++) {
            // if (fEventHeader->GetEventId() == 10576)
            //     nSingleHits += pDetector[i]->FindHitsNew(digT0, aTofHits, 1);
            // else
            nSingleHits += pDetector[i]->FindHitsNew(digT0, aTofHits, 0);
        }
    }

    if (fUseMCData)
        MergeHitsOnStrip();   // save only the fastest hit in the strip. Used for MC only

    int nFinally = CompressHits();   // remove blank slotes

    sw.Stop();
    workTime += sw.RealTime();

    if (fVerbose) {
        cout << fDetName << ": single hits= " << nSingleHits << ", double hits= " << nDoubleHits
             << ", triple hits= " << nTripleHits << ", final hits= " << nFinally << endl;
        printf("\n=================== %s::Exec(): Finished ===================\n", this->GetName());
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::Finish()
{
    if (fDoTest) {
        TFile file(fTestFlnm.Data(), "RECREATE");
        file.Close();
        if (!fUseMCData)
            for (Int_t i = 0; i < fNDetectors; i++)
                pDetector[i]->SaveHistToFile(fTestFlnm.Data());
    }

    printf("Work time of %s: %4.2f sec.\n", this->GetName(), workTime);
}

//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::SetSeed(UInt_t seed)
{
    pRandom->SetSeed(seed);
}
//--------------------------------------------------------------------------------------------------------------------------------------

BmnTrigDigit* BmnTof1HitProducer::FingT0Digit()
{
    BmnTrigDigit* digT0 = NULL;
    BmnTrigDigit* digT0_1 = NULL;
    BmnTrigDigit* digT0_2 = NULL;

    BmnTrigWaveDigit* wdigT0_1 = NULL;
    BmnTrigWaveDigit* wdigT0_2 = NULL;

    if (fPeriod != 8) {
        for (Int_t i = 0; i < aExpDigitsT0->GetEntriesFast(); i++) {
            digT0 = (BmnTrigDigit*)aExpDigitsT0->At(i);
            if (digT0->GetMod() == 0) {
                // if (fVerbose)
                //     cout << "BmnTof1HitProducer::FingT0Digit(): T0 digit is found, Time = " << digT0->GetTime() <<
                //     endl;
                return digT0;   // take first T0 digit with Mod == 0. needed for ToF calculation.
            }
        }
    } else if (fPeriod == 8) {
        digT0 = new BmnTrigDigit();
        if (aExpDigitsT0->GetEntriesFast() == 0 || aExpDigitsT0_2->GetEntriesFast() == 0)
            return NULL;
        // SRC 2022
        if (fRun >= 968 && fRun <= 1546) {
            digT0_1 = (BmnTrigDigit*)aExpDigitsT0->At(0);
            digT0_2 = (BmnTrigDigit*)aExpDigitsT0_2->At(0);
            digT0->SetAmp(digT0_1->GetAmp() + digT0_2->GetAmp());
            digT0->SetTime((digT0_1->GetTime() + digT0_2->GetTime()) * 0.5);
        }

        // BMN 2022-2023
        if (fRun >= 6587 && fRun <= 8427) {
            Bool_t FlagHitTop = kFALSE, FlagHitBotom = kFALSE;

            for (Int_t iDig = 0; iDig < aExpDigitsT0->GetEntriesFast(); ++iDig) {
                digT0_1 = (BmnTrigDigit*)aExpDigitsT0->At(iDig);
                // cout << "digT0_1 = " << digT0_1->GetTime() << endl;
                if (digT0_1->GetTime() >= 1900 && digT0_1->GetTime() <= 1960) {   // cut for trigger particle time
                    FlagHitTop = kTRUE;
                    break;
                }
            }

            for (Int_t iDig = 0; iDig < aExpDigitsT0_2->GetEntriesFast(); ++iDig) {
                digT0_2 = (BmnTrigDigit*)aExpDigitsT0_2->At(iDig);
                // cout << "digT0_2 = " << digT0_2->GetTime() << endl;
                if (digT0_2->GetTime() >= 1900 && digT0_2->GetTime() <= 1960) {   // cut for trigger particle time
                    FlagHitBotom = kTRUE;
                    break;
                }
            }

            if (FlagHitTop && FlagHitBotom) {
                digT0->SetAmp(digT0_1->GetAmp() + digT0_2->GetAmp());
                digT0->SetTime((digT0_1->GetTime() + digT0_2->GetTime()) * 0.5);
            }
        }

        return digT0;   // take Time and Amp (two PMT on one Scintillator)
    } else if (fPeriod == 9) {
        Bool_t f1 = false;
        Bool_t f2 = false;
        Double_t time1, time2;

        if (aExpDigitsT0->GetEntriesFast() == 0 || aExpDigitsT0_2->GetEntriesFast() == 0)
            return NULL;

        if (fRun >= 9289) {
            wdigT0_1 = (BmnTrigWaveDigit*)aExpDigitsT0->At(0);
            wdigT0_2 = (BmnTrigWaveDigit*)aExpDigitsT0_2->At(0);

            vector<Double_t> bc2t_time = wdigT0_1->TdcVector();
            vector<Double_t> bc2b_time = wdigT0_2->TdcVector();

            for (Int_t iv = 0; iv < (int)bc2t_time.size(); iv++) {
                time1 = bc2t_time.at(iv);
                if (time1 > 2600 && time1 < 2660) {
                    f1 = true;
                    break;
                }
            }

            for (Int_t iv = 0; iv < (int)bc2b_time.size(); iv++) {
                time2 = bc2b_time.at(iv);
                if (time2 > 2600 && time2 < 2660) {
                    f2 = true;
                    break;
                }
            }

            if (!f1 || !f2)
                return NULL;

            digT0 = new BmnTrigDigit();
            digT0->SetAmp(10);
            digT0->SetTime((time1 + time2) * 0.5);
        }
    }

    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------

Bool_t BmnTof1HitProducer::IsFile(TString NameFile = "")
{
    NameFile = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", NameFile.Data());
    ifstream Temp;
    Temp.open(NameFile, ios::in /*| ios::nocreate*/);
    return Temp.is_open();
}
//--------------------------------------------------------------------------------------------------------------------------------------

Bool_t BmnTof1HitProducer::SetCorrFiles()
{
    FlagFileLRcorrection = true;
    FlagFileSlewingCorrection = true;
    FlagFileTimeShiftCorrection = true;

    NameFileLRcorrection = "";
    NameFileSlewingCorrection = "";
    NameFileTimeShiftCorrection = "";

    // Run 6 (03.2017)
    if (fPeriod == 6) {
        NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i.dat", fPeriod);
        NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i.root", fPeriod);
        NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i.dat", fPeriod);
    }

    // Run 7 (03.2018 - 04.2018)
    if (fPeriod == 7) {
        // SRC
        if (fRun >= 2013 && fRun <= 3588) {
            // for first time will be used correction from BM@N
            NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_BMN.dat", fPeriod);
            NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_BMN.root", fPeriod);
            NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_SRC.dat", fPeriod);
        }

        // BM@N Ar beam
        if (fRun >= 3589 && fRun <= 4707) {
            NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_BMN.dat", fPeriod);
            NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_BMN.root", fPeriod);
            NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_BMN_Ar.dat", fPeriod);
        }

        // BM@N Kr beam
        if (fRun >= 4747 && fRun <= 5185) {
            NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_BMN.dat", fPeriod);
            NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_BMN.root", fPeriod);
            NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_BMN_Kr.dat", fPeriod);
        }
    }

    // Run 8
    if (fPeriod == 8) {

        // SRC (2022)
        if (fRun >= 968 && fRun <= 1546) {
            NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_SRC.dat", fPeriod);
            NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_SRC.root", fPeriod);
            NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_SRC.dat", fPeriod);
        }

        // BM@N (2022-2023)
        if (fRun >= 6587 && fRun <= 8427) {
            if (fDetectorId == kTOF1) {
                NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i.dat", fPeriod);
                NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_v2.root", fPeriod);
                NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_v4.dat", fPeriod);
            }
            if (fDetectorId == kTOF701) {
                NameFileLRcorrection = Form("TOF701_LRCorr_RUN%i.dat", fPeriod);
                NameFileSlewingCorrection = Form("TOF701_SlewingCorr_RUN%i.root", fPeriod);
                NameFileTimeShiftCorrection = Form("TOF701_TimeShiftCorr_RUN%i_v2.dat", fPeriod);
            }
        }
    }
    if (fPeriod == 9) {

        if (fRun >= 9289) {
            if (fDetectorId == kTOF1) {
                NameFileLRcorrection = "";
                NameFileSlewingCorrection = "";
                NameFileTimeShiftCorrection = "";
                FlagFileLRcorrection = false;
                FlagFileSlewingCorrection = false;
                FlagFileTimeShiftCorrection = false;
            }
            if (fDetectorId == kTOF701) {
                NameFileLRcorrection = Form("TOF701_LRCorr_RUN%i.dat", 8);
                NameFileSlewingCorrection = "";
                NameFileTimeShiftCorrection = "";
                FlagFileLRcorrection = true;
                FlagFileSlewingCorrection = false;
                FlagFileTimeShiftCorrection = false;
            }
            // temporary solution
            return kTRUE;
        }
    }

    // check all files exist
    if (!IsFile(NameFileLRcorrection)) {
        FlagFileLRcorrection = false;
        if (fVerbose) {
            cout << endl
                 << this->GetName() << "::Init(): File " << NameFileLRcorrection.Data()
                 << " for LR correction is not found" << endl;
            cout << "Check /input folder for file" << endl;
        }
    }

    if (!IsFile(NameFileSlewingCorrection)) {
        FlagFileSlewingCorrection = false;
        if (fVerbose) {
            cout << endl
                 << this->GetName() << "::Init(): File " << NameFileSlewingCorrection.Data()
                 << " for Slewing correction is not found" << endl;
            cout << "Check /input folder for file" << endl;
        }
    }

    if (!IsFile(NameFileTimeShiftCorrection)) {
        FlagFileTimeShiftCorrection = false;
        if (fVerbose) {
            cout << endl
                 << this->GetName() << "::Init(): File " << NameFileTimeShiftCorrection.Data()
                 << " for TimeShift correction is not found" << endl;
            cout << "Check /input folder for file" << endl;
        }
    }

    // cout << "NameFileLRcorrection = " << NameFileLRcorrection << endl;
    // cout << "NameFileSlewingCorrection = " << NameFileSlewingCorrection << endl;
    // cout << "NameFileTimeShiftCorrection = " << NameFileTimeShiftCorrection << endl;

    // return "true" in case the run is physical and correction files are found.
    if (FlagFileLRcorrection && FlagFileSlewingCorrection && FlagFileTimeShiftCorrection)
        return kTRUE;

    // return "false" in case the run is outside physical runs or correction files are not found.
    return kFALSE;
}
//--------------------------------------------------------------------------------------------------------------------------------------

Bool_t BmnTof1HitProducer::OutOfRange(Int_t iPlane = -1)
{
    if (iPlane < 0 || iPlane >= fNDetectors)
        return kTRUE;
    return kFALSE;
}

void BmnTof1HitProducer::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch(fHitsName.Data(), &aTofHits);
    resultTree->Fill();
}
