// mine 

#include "BmnTof1HitProducer.h"
#include "BmnTOF1Point.h"
#include "BmnTof1Digit.h"
#include "BmnTofHit.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include <TGeoManager.h>
#include <TVector3.h>
#include <TStopwatch.h>
#include <TClonesArray.h>

#include <iostream>

using namespace std;

static Double_t workTime = 0.0;

//--------------------------------------------------------------------------------------------------------------------------------------

BmnTof1HitProducer::BmnTof1HitProducer(const char *name, Bool_t useMCdata, Int_t verbose, Bool_t test)
    : BmnTof1HitProducerIdeal(name, useMCdata, verbose, test),
      fTimeSigma(0.100),
      fErrX(1. / sqrt(12.)),
      fErrY(0.5),
      pRandom(nullptr),
      h2TestStrips(nullptr),
      h1TestDistance(nullptr),
      h2TestNeighborPair(nullptr),
      fSignalVelosity(0.060)
{
    pGeoUtils = new BmnTof1GeoUtils;

    if (fDoTest)
    {
        fTestFlnm = "test.BmnTof400HitProducer.root";
        effTestEfficiencySingleHit = new TEfficiency("effSingleHit", "Efficiency single hit;R, cm;Side", 10000, -0.1, 1.);
        fList.Add(effTestEfficiencySingleHit);
        effTestEfficiencyDoubleHit = new TEfficiency("effDoubleHit", "Efficiency double hit;R, cm;Side", 10000, -0.1, 1.);
        fList.Add(effTestEfficiencyDoubleHit);

        h1TestDistance = new TH1D("TestDistance", "Distance between strips;M, cm;Side", 1000, 0., 100.);
        fList.Add(h1TestDistance);
        h2TestStrips = new TH2D("TestStrips", ";Z, cm;#phi, rads", 2000, -300., 300., 500, -3.5, 3.5);
        fList.Add(h2TestStrips);

        h2TestNeighborPair = new TH2D("TestNeighborPair", "Neighbor strip pairs test;stripID1;stripID2", 100, -0.5, 49.5, 100, -0.5, 49.5);
        fList.Add(h2TestNeighborPair);
        h2TestXYSmeared = new TH2D("TestXYSmeared", "Smeared XY (single hit) test;#DeltaX, cm;#DeltaY, cm", 1000, -1., 1., 1000, -2., 2.);
        fList.Add(h2TestXYSmeared);
        h2TestXYSmeared2 = new TH2D("TestXYSmeared2", "Smeared XY (single hit) test;X, cm;Y, cm", 1000, -180., 180., 1000, -180., 180.);
        fList.Add(h2TestXYSmeared2);
        h2TestXYSmearedDouble = new TH2D("TestXYSmearedDouble", "Smeared XY (double hit) test;#DeltaX, cm;#DeltaY, cm", 1000, -2., 2., 1000, -2., 2.);
        fList.Add(h2TestXYSmearedDouble);
        h2TestXYSmearedDouble2 = new TH2D("TestXYSmearedDouble2", "Smeared XY (double hit) test;X, cm;Y, cm", 1000, -180., 180., 1000, -180., 180.);
        fList.Add(h2TestXYSmearedDouble2);

        h2TestEtaPhi = new TH2D("TestEtaPhi", ";#eta;#phi, degree", 1000, -1.6, 1.6, 1000, -181., 181.);
        fList.Add(h2TestEtaPhi);
        h2TestRZ = new TH2D("TestRZ", ";X, cm;Y, cm", 1000, -300., 300., 1000, -200., 200.);
        fList.Add(h2TestRZ);
        h2TdetIdStripId = new TH2D("TdetIdStripId", ";stripId;detId", 100, -0.5, 99.5, 21, -0.5, 20.5);
        fList.Add(h2TdetIdStripId);
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------

BmnTof1HitProducer::~BmnTof1HitProducer()
{
    if (!fUseMCData)
    {
        for (Int_t i = 0; i < fNDetectors; i++)
        {
            delete pDetector[i];
        }
        delete[] pDetector;
    }
    delete pRandom;
    delete pGeoUtils;
}
//--------------------------------------------------------------------------------------------------------------------------------------

InitStatus BmnTof1HitProducer::LoadDetectorConfiguration()
{
    // Parsing geometry
    fNDetectors = -1;
    fNDetectors = pGeoUtils->ParseTGeoManager(fUseMCData, h2TestStrips, true);
    if (fNDetectors <= 0)
    {
        cout << "BmnTof1HitProducer::LoadDetectorConfiguration(): No TOF400 detectors in geometry file for the current run! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    if (fVerbose)
        cout << "BmnTof1HitProducer::CreateDetector(): number of TOF400 Detectors from geometry file = " << fNDetectors << endl;
    pGeoUtils->FindNeighborStrips(h1TestDistance, h2TestNeighborPair, fDoTest);

    // Init BmnTOF1Detectors
    if (!fUseMCData)
    {
        if (!SetCorrFiles())
        {
            cout << "BmnTof1HitProducer::LoadDetectorConfiguration(): No corrections for the current run! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }

        pDetector = new BmnTOF1Detector *[fNDetectors];
        for (Int_t i = 0; i < fNDetectors; i++)
        {
            Int_t DoTestForDetector = 0; // For developers only. Level of Histograms filling (0-don't fill, 1-low, 2-high).
            pDetector[i] = new BmnTOF1Detector(i, DoTestForDetector, fVerbose); // fVerbose = 4

            if (FlagFileLRcorrection)
                pDetector[i]->SetCorrLR(NameFileLRcorrection);
            if (FlagFileSlewingCorrection)
                pDetector[i]->SetCorrSlewing(NameFileSlewingCorrection);
             if (FlagFileTimeShiftCorrection)
                 pDetector[i]->SetCorrTimeShift(NameFileTimeShiftCorrection);
            pDetector[i]->SetGeo(pGeoUtils);

            if (fPeriod == 6)
            {
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
        cout << endl
             << "BmnTof400HitProducer::Init(): Start  Verbose level = " << fVerbose << endl;

    pRandom = new TRandom2();

    if (fOnlyPrimary)
        cout << " Only primary particles are processed!!! \n"; // FIXME NOT used now ADDD

    if (fUseMCData)
    {
        aMcPoints = (TClonesArray *)FairRootManager::Instance()->GetObject("TOF400Point");
        if (!aMcPoints)
        {
            cout << "BmnTof400HitProducer::Init(): branch TOF1Point not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
        aMcTracks = (TClonesArray *)FairRootManager::Instance()->GetObject("MCTrack");
        if (!aMcTracks)
        {
            cout << "BmnTof400HitProducer::Init(): branch MCTrack not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
    }
    else
    {
        aExpDigits = (TClonesArray *)FairRootManager::Instance()->GetObject("TOF400");
        if (!aExpDigits)
        {
            cout << "BmnTof400HitProducer::Init(): branch TOF400 not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }

        // looking for T0 branch
        TString NameT0Branch, NameT0Branch2, NameT0Branch3, NameT0Branch4;
        TString NameT0Branch5, NameT0Branch6, NameT0Branch7, NameT0Branch8;
        if (fPeriod == 6)
            NameT0Branch = "T0";
        if (fPeriod == 7)
            NameT0Branch = "BC2";
        if (fPeriod == 8)
        {
            NameT0Branch = "TQDC_T0_1_A";
            NameT0Branch2 = "TQDC_T0_2_A";
            NameT0Branch3 = "TQDC_BC2_1_A";
            NameT0Branch4 = "TQDC_BC2_2_A";
            NameT0Branch5 = "T0_1_A"; // ?
            NameT0Branch6 = "T0_2_A";
            NameT0Branch7 = "BC2_1_A";
            NameT0Branch8 = "BC2_2_A";
        }

        if (fVerbose)
            cout << "BmnTof400HitProducer::Init(): looking for branch " << NameT0Branch << " for start" << endl;
        aExpDigitsT0 = (TClonesArray *)FairRootManager::Instance()->GetObject(NameT0Branch.Data());
        if (!aExpDigitsT0)
        {
            printf("BmnTof400HitProducer::Init(): branch %s not found! Task will be deactivated\n", NameT0Branch.Data());
            SetActive(kFALSE);
            return kERROR;
        }
        else if (fVerbose)
            cout << "................ ok" << endl;

        if (fPeriod == 8)
        {
            if (fVerbose)
                cout << "BmnTof400HitProducer::Init(): looking for branch " << NameT0Branch2 << " for start" << endl;
            aExpDigitsT0_2 = (TClonesArray *)FairRootManager::Instance()->GetObject(NameT0Branch2.Data());
            if (!aExpDigitsT0_2)
            {
                printf("BmnTof400HitProducer::Init(): branch %s not found! Task will be deactivated\n", NameT0Branch2.Data());
                SetActive(kFALSE);
                return kERROR;
            }
            else if (fVerbose)
                cout << "................ ok" << endl;

            if (fVerbose)
                cout << "BmnTof400HitProducer::Init(): looking for branch " << NameT0Branch3 << " for start" << endl;
            aExpDigitsT0_3 = (TClonesArray *)FairRootManager::Instance()->GetObject(NameT0Branch3.Data());
            if (!aExpDigitsT0_3)
            {
                printf("BmnTof400HitProducer::Init(): branch %s not found! Task will be deactivated\n", NameT0Branch3.Data());
                SetActive(kFALSE);
                return kERROR;
            }
            else if (fVerbose)
                cout << "................ ok" << endl;

            if (fVerbose)
                cout << "BmnTof400HitProducer::Init(): looking for branch " << NameT0Branch4 << " for start" << endl;
            aExpDigitsT0_4 = (TClonesArray *)FairRootManager::Instance()->GetObject(NameT0Branch4.Data());
            if (!aExpDigitsT0_4)
            {
                printf("BmnTof400HitProducer::Init(): branch %s not found! Task will be deactivated\n", NameT0Branch4.Data());
                SetActive(kFALSE);
                return kERROR;
            }
            else if (fVerbose)
                cout << "................ ok" << endl;

            if (fVerbose)
                cout << "BmnTof400HitProducer::Init(): looking for branch " << NameT0Branch5 << " for start" << endl;
            aExpDigitsT0_5 = (TClonesArray *)FairRootManager::Instance()->GetObject(NameT0Branch5.Data());
            if (!aExpDigitsT0_5)
            {
                printf("BmnTof400HitProducer::Init(): branch %s not found! Task will be deactivated\n", NameT0Branch5.Data());
                SetActive(kFALSE);
                return kERROR;
            }
            else if (fVerbose)
                cout << "................ ok" << endl;

            if (fVerbose)
                cout << "BmnTof400HitProducer::Init(): looking for branch " << NameT0Branch6 << " for start" << endl;
            aExpDigitsT0_6 = (TClonesArray *)FairRootManager::Instance()->GetObject(NameT0Branch6.Data());
            if (!aExpDigitsT0_6)
            {
                printf("BmnTof400HitProducer::Init(): branch %s not found! Task will be deactivated\n", NameT0Branch6.Data());
                SetActive(kFALSE);
                return kERROR;
            }
            else if (fVerbose)
                cout << "................ ok" << endl;

            if (fVerbose)
                cout << "BmnTof400HitProducer::Init(): looking for branch " << NameT0Branch7 << " for start" << endl;
            aExpDigitsT0_7 = (TClonesArray *)FairRootManager::Instance()->GetObject(NameT0Branch7.Data());
            if (!aExpDigitsT0_7)
            {
                printf("BmnTof400HitProducer::Init(): branch %s not found! Task will be deactivated\n", NameT0Branch7.Data());
                SetActive(kFALSE);
                return kERROR;
            }
            else if (fVerbose)
                cout << "................ ok" << endl;

            if (fVerbose)
                cout << "BmnTof400HitProducer::Init(): looking for branch " << NameT0Branch8 << " for start" << endl;
            aExpDigitsT0_8 = (TClonesArray *)FairRootManager::Instance()->GetObject(NameT0Branch8.Data());
            if (!aExpDigitsT0_8)
            {
                printf("BmnTof400HitProducer::Init(): branch %s not found! Task will be deactivated\n", NameT0Branch8.Data());
                SetActive(kFALSE);
                return kERROR;
            }
            else if (fVerbose)
                cout << "................ ok" << endl;

            f_walkC_t01_0 = new TF1("f_walkC_t01_0", Obj4WalkCorr, 1000, 5000, 3);
            f_walkC_t01_1 = new TF1("f_walkC_t01_1", Obj4WalkCorr, 1000, 5000, 3);
            f_walkC_t02_0 = new TF1("f_walkC_t02_0", Obj4WalkCorr, 1000, 5000, 3);
            f_walkC_t02_1 = new TF1("f_walkC_t02_1", Obj4WalkCorr, 1000, 5000, 3);

            f_walkC_t01_0->SetParameters(par_walk_t01_0);
            f_walkC_t01_1->SetParameters(par_walk_t01_1);
            f_walkC_t02_0->SetParameters(par_walk_t02_0);
            f_walkC_t02_1->SetParameters(par_walk_t02_1);

            // cout << "f_walkC_t01_0->Eval(3000) = " << f_walkC_t01_0->Eval(3000) << endl;
            // cout << "f_walkC_t01_1->Eval(3000) = " << f_walkC_t01_1->Eval(3000) << endl;
            // cout << "f_walkC_t02_0->Eval(3000) = " << f_walkC_t02_0->Eval(3000) << endl;
            // cout << "f_walkC_t02_1->Eval(3000) = " << f_walkC_t02_1->Eval(3000) << endl;
        }
    }

    // Create and register output array
    aTofHits = new TClonesArray("BmnTofHit");
    FairRootManager::Instance()->Register("BmnTof400Hit", "TOF1", aTofHits, kTRUE);

    if (LoadDetectorConfiguration() != kSUCCESS)
    {
        cout << "BmnTof400HitProducer::Init(): Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    if (fVerbose)
        cout << "BmnTof400HitProducer::Init(): Initialization finished succesfully." << endl;

    return kSUCCESS;
}
//--------------------------------------------------------------------------------------------------------------------------------------

InitStatus BmnTof1HitProducer::OnlineInit()
{
    aExpDigits = new TClonesArray("BmnTof1Digit");
    aExpDigitsT0 = new TClonesArray("BmnTrigDigit");
    aTofHits = new TClonesArray("BmnTofHit");

    return LoadDetectorConfiguration();
}

InitStatus BmnTof1HitProducer::OnlineRead(const std::unique_ptr<TTree> &dataTree,
                                          const std::unique_ptr<TTree> &resultTree)
{
    if (!IsActive())
        return kERROR;

    SetOnlineActive();

    aExpDigits->Delete();
    if (dataTree->SetBranchAddress("TOF400", &aExpDigits))
    {
        LOG(error) << "BmnTof1HitProducer::OnlineRead(): branch TOF400 not found! Task will be deactivated";
        SetOnlineActive(kFALSE);
        return kERROR;
    }

    TString nameT0Branch = "";
    if (fPeriod == 6)
        nameT0Branch = "T0";
    if (fPeriod == 7)
        nameT0Branch = "BC2";
    if (fPeriod == 8)
        nameT0Branch = "T0_1_A";

    aExpDigitsT0->Delete();
    if (nameT0Branch == "" || dataTree->SetBranchAddress(nameT0Branch.Data(), &aExpDigitsT0))
    {
        LOG(error) << "BmnTof1HitProducer::OnlineRead(): branch" << nameT0Branch.Data()
                   << " not found! Task will be deactivated";
        SetOnlineActive(kFALSE);
        return kERROR;
    }

    aTofHits->Delete();

    return kSUCCESS;
}

Bool_t BmnTof1HitProducer::HitExist(Double_t val) // val - distance to the pad edge [cm]
{
    const static Double_t slope = (0.98 - 0.95) / 0.2;
    Double_t efficiency = (val > 0.2) ? 0.98 : (0.95 + slope * val);

    //-------------------------------------
    // 99% ---------
    //              \
    //               \
    //                \
    // 95%             \
    //  <-----------|--|
    //            0.2  0.
    //-------------------------------------

    if (pRandom->Rndm() < efficiency)
        return true;
    return false;
}
//------------------------------------------------------------------------------------------------------------------------

Bool_t BmnTof1HitProducer::DoubleHitExist(Double_t val) // val - distance to the pad edge  [cm]
{
    const static Double_t slope = (0.3 - 0.0) / 0.5;
    Double_t efficiency = (val > 0.5) ? 0. : (0.3 - slope * val);

    //-------------------------------------
    // 30%               /
    //                  /
    //                 /
    //                /
    // 0%            /
    //  <-----------|----|
    //            0.5    0.
    //-------------------------------------

    if (efficiency == 0.)
        return false;

    if (pRandom->Rndm() < efficiency)
        return HitExist(val);
    return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::Exec(Option_t *opt)
{
    TStopwatch sw;
    sw.Start();

    if (!IsActive() || !IsOnlineActive())
        return;

    if (fVerbose)
        cout << endl
             << "======================== TOF400 exec started ====================" << endl;
    static const TVector3 XYZ_err(fErrX, fErrY, 0.);

    aTofHits->Clear();

    Int_t UID, trackID;
    TVector3 pos, XYZ_smeared;
    int nSingleHits = 0, nDoubleHits = 0;

    if (fUseMCData)
    {
        for (Int_t pointIndex = 0, nTofPoint = aMcPoints->GetEntriesFast(); pointIndex < nTofPoint; pointIndex++) // cycle by TOF points
        {
            BmnTOF1Point *pPoint = (BmnTOF1Point *)aMcPoints->UncheckedAt(pointIndex);

            if (fVerbose > 2)
                pPoint->Print("");

            trackID = pPoint->GetTrackID();
            UID = pPoint->GetDetectorID();
            Double_t time = pRandom->Gaus(pPoint->GetTime(), fTimeSigma); // 100 ps
            pPoint->Position(pos);

            const LStrip1 *pStrip = pGeoUtils->FindStrip(UID);

            XYZ_smeared.SetXYZ(pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY), pStrip->center.Z());

            LStrip1::Side_t side;
            Double_t distance = pStrip->MinDistanceToEdge(&pos, side); // [cm]

            bool passed;
            if (passed = HitExist(distance)) // check efficiency
            {
                AddHit(UID, XYZ_smeared, XYZ_err, pointIndex, trackID, time);
                nSingleHits++;

                if (fDoTest)
                {
                    h2TestXYSmeared->Fill(pos.X() - XYZ_smeared.X(), pos.Y() - XYZ_smeared.Y());
                    h2TestXYSmeared2->Fill(XYZ_smeared.X(), XYZ_smeared.Y());
                    h2TestEtaPhi->Fill(pos.Eta(), pos.Phi() * TMath::RadToDeg());
                    h2TestRZ->Fill(pos.X(), pos.Y());
                }
            }

            if (fDoTest)
                effTestEfficiencySingleHit->Fill(passed, distance);

            if (passed = DoubleHitExist(distance)) // check cross hit
            {
                Int_t CrossUID = (side == LStrip1::kRight) ? pStrip->neighboring[LStrip1::kRight]
                                                           : pStrip->neighboring[LStrip1::kLeft];

                if (LStrip1::kInvalid == CrossUID)
                    continue; // last strip on module

                pStrip = pGeoUtils->FindStrip(CrossUID);
                XYZ_smeared.SetXYZ(pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY), pStrip->center.Z());

                AddHit(CrossUID, XYZ_smeared, XYZ_err, pointIndex, trackID, time);
                nDoubleHits++;

                if (fDoTest)
                {
                    h2TestXYSmearedDouble->Fill((pos - XYZ_smeared).Mag(), pos.Z() - XYZ_smeared.Z());
                    h2TestXYSmearedDouble2->Fill(XYZ_smeared.X(), XYZ_smeared.Y());
                }
            }

            if (fDoTest)
                effTestEfficiencyDoubleHit->Fill(passed, distance);

        } // cycle by the TOF points
    }
    else
    {
        BmnTrigDigit *digT0 = FingT0Digit();

        for (Int_t i = 0; i < fNDetectors; i++)
            pDetector[i]->Clear();

        for (Int_t iDig = 0; iDig < aExpDigits->GetEntriesFast(); ++iDig)
        {
            BmnTof1Digit *digTof = (BmnTof1Digit *)aExpDigits->At(iDig);
            // cout << "SETTING PLANE " << digTof->GetPlane() << "\n";
            if (!OutOfRange(digTof->GetPlane()))
                pDetector[digTof->GetPlane()]->SetDigit(digTof);
        }

        for (Int_t i = 0; i < fNDetectors; i++)
            nSingleHits += pDetector[i]->FindHits(digT0, aTofHits);
    }

    MergeHitsOnStrip(); // save only the fastest hit in the strip. Used for MC only

    int nFinally = CompressHits(); // remove blank slotes

    sw.Stop();
    workTime += sw.RealTime();

    if (fVerbose)
        cout << "Tof400  single hits= " << nSingleHits << ", double hits= " << nDoubleHits
             << ", final hits= " << nFinally << endl;
    if (fVerbose)
        cout << "======================== TOF400 exec finished ====================" << endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::Finish()
{
    if (fDoTest)
    {
        TFile *ptr = gFile;
        TFile file(fTestFlnm.Data(), "RECREATE");
        fList.Write();
        file.Close();
        gFile = ptr;
        if (!fUseMCData)
            for (Int_t i = 0; i < fNDetectors; i++)
                pDetector[i]->SaveHistToFile(fTestFlnm.Data());
    }

    printf("Work time of BmnTof1HitProducer: %4.2f sec.\n", workTime);
}

//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::SetSeed(UInt_t seed)
{
    pRandom->SetSeed(seed);
}
//--------------------------------------------------------------------------------------------------------------------------------------

BmnTrigDigit *BmnTof1HitProducer::FingT0Digit()
{
    BmnTrigDigit *digT0 = NULL;

    BmnTrigDigit *digT0_0 = NULL;
    BmnTrigDigit *digT0_1 = NULL;
    BmnTrigDigit *digT0_2 = NULL;
    BmnTrigDigit *digT0_3 = NULL;

    BmnTrigWaveDigit *newT0_0 = NULL; //// !!!!!!!!!!!!  problem with  BmnTrigWaveDigit
    BmnTrigWaveDigit *newT0_1 = NULL;
    BmnTrigWaveDigit *newT0_2 = NULL;
    BmnTrigWaveDigit *newT0_3 = NULL;
    // BmnTrigDigit *digT0_1 = NULL;
    // BmnTrigDigit *digT0_2 = NULL;


    if (fPeriod != 8)
    {
        for (Int_t i = 0; i < aExpDigitsT0->GetEntriesFast(); i++)
        {
            digT0 = (BmnTrigDigit *)aExpDigitsT0->At(i);
            if (digT0->GetMod() == 0)
            {
                // if (fVerbose)
                //     cout << "BmnTof1HitProducer::FingT0Digit(): T0 digit is found, Time = " << digT0->GetTime() <<
                //     endl;
                return digT0; // take first T0 digit with Mod == 0. needed for ToF calculation.
            }
        }
    }
    else if (fPeriod == 8)
    {
         digT0 = new BmnTrigDigit();
        // if (aExpDigitsT0->GetEntriesFast() == 0 || aExpDigitsT0_2->GetEntriesFast() == 0) return NULL;
        // digT0_1 = (BmnTrigDigit*) aExpDigitsT0->At(0);
        // digT0_2 = (BmnTrigDigit*) aExpDigitsT0_2->At(0);
        // digT0->SetAmp(digT0_1->GetAmp() + digT0_2->GetAmp());
        // digT0->SetTime((digT0_1->GetTime() + digT0_2->GetTime()) * 0.5);
        // return digT0; // take Time and Amp from BC2 detector (two PMT on one Scintillator)

if (aExpDigitsT0->GetEntriesFast() != 1 || aExpDigitsT0_2->GetEntriesFast() != 1 || aExpDigitsT0_3->GetEntriesFast() != 1 || aExpDigitsT0_4->GetEntriesFast() != 1 ||
aExpDigitsT0_5->GetEntriesFast() != 1 || aExpDigitsT0_6->GetEntriesFast() != 1 || aExpDigitsT0_7->GetEntriesFast() != 1 || aExpDigitsT0_8->GetEntriesFast() != 1)
            return NULL;

        newT0_0 = (BmnTrigWaveDigit *) aExpDigitsT0->At(0); // TQDC
        newT0_1 = (BmnTrigWaveDigit *) aExpDigitsT0_2->At(0);
        newT0_2 = (BmnTrigWaveDigit *) aExpDigitsT0_3->At(0);
        newT0_3 = (BmnTrigWaveDigit *) aExpDigitsT0_4->At(0);

        digT0_0 = (BmnTrigDigit *) aExpDigitsT0_5->At(0); // not TQDC
        digT0_1 = (BmnTrigDigit *) aExpDigitsT0_6->At(0);
        digT0_2 = (BmnTrigDigit *) aExpDigitsT0_7->At(0);
        digT0_3 = (BmnTrigDigit *) aExpDigitsT0_8->At(0);


        Double_t t01Time0 = digT0_0->GetTime();
        Double_t t01Time1 = digT0_1->GetTime();
        Double_t t02Time0 = digT0_2->GetTime();
        Double_t t02Time1 = digT0_3->GetTime();

        Double_t t01dE0, testtime10, t01dE1, testtime11, t02dE0, testtime20, t02dE1, testtime21;

        if (t01Time0 > 0)
            getEnergyLoss(aExpDigitsT0, 0, t01Time0, t01dE0, testtime10);
        if (t01Time1 > 0)
            getEnergyLoss(aExpDigitsT0_2, 0, t01Time1, t01dE1, testtime11);
        if (t02Time0 > 0)
            getEnergyLoss(aExpDigitsT0_3, 0, t02Time0, t02dE0, testtime20);
        if (t02Time1 > 0)
            getEnergyLoss(aExpDigitsT0_4, 0, t02Time1, t02dE1, testtime21);

        Double_t t01Time0Walk = f_walkC_t01_0->Eval(t01dE0);
        Double_t t01Time1Walk = f_walkC_t01_1->Eval(t01dE1);
        Double_t t02Time0Walk = f_walkC_t02_0->Eval(t02dE0);
        Double_t t02Time1Walk = f_walkC_t02_1->Eval(t02dE1);

        t01Time0 = t01Time0 - t01Time0Walk;
        t01Time1 = t01Time1 - t01Time1Walk;
        t02Time0 = t02Time0 - t02Time0Walk;
        t02Time1 = t02Time1 - t02Time1Walk;

        digT0->SetAmp((newT0_0->GetPeak() + newT0_1->GetPeak() + newT0_2->GetPeak() + newT0_3->GetPeak()) * 0.25); // new Amplitude
        digT0->SetTime(0.25 * (t01Time0 + t01Time1 + t02Time0 + t02Time1));                                        // new Time

        return digT0;
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
    Bool_t temp = false;
    FlagFileLRcorrection = false;
    FlagFileSlewingCorrection = false;
    FlagFileTimeShiftCorrection = false;

    // Run 6 (03.2017)
    if (fPeriod == 6)
    {
        NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i.dat", fPeriod);
        NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i.root", fPeriod);
        NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i.dat", fPeriod);

        FlagFileLRcorrection = true;
        FlagFileSlewingCorrection = true;
        FlagFileTimeShiftCorrection = true;

        temp = true;
    }

    // Run 7 (03.2018 - 04.2018)
    if (fPeriod == 7)
    {
        // SRC
        if (fRun >= 2013 && fRun <= 3588)
        {
            // for first time will be used correction from BM@N
            NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_BMN.dat", fPeriod);
            NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_BMN.root", fPeriod);

            // NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_SRC.dat", fPeriod);
            // NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_SRC.root", fPeriod);

            NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_SRC.dat", fPeriod);

            FlagFileLRcorrection = true;
            FlagFileSlewingCorrection = true;
            FlagFileTimeShiftCorrection = true;

            temp = true;
        }

        // BM@N Ar beam
        if (fRun >= 3589 && fRun <= 4707)
        {
            NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_BMN.dat", fPeriod);
            NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_BMN.root", fPeriod);
            NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_BMN_Ar.dat", fPeriod);

            FlagFileLRcorrection = true;
            FlagFileSlewingCorrection = true;
            FlagFileTimeShiftCorrection = true;

            temp = true;
        }

        // BM@N Kr beam
        if (fRun >= 4747 && fRun <= 5185)
        {
            NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_BMN.dat", fPeriod);
            NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_BMN.root", fPeriod);
            NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_BMN_Kr.dat", fPeriod);

            FlagFileLRcorrection = true;
            FlagFileSlewingCorrection = true;
            FlagFileTimeShiftCorrection = true;

            temp = true;
        }
    }

    // Run 8 (2022)
    if (fPeriod == 8)
    {
        NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_SRC.dat", fPeriod);
        NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_SRC_Pb1_recovery_v2.root", fPeriod);
        NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_SRC_Pb1_v2.dat", fPeriod); // shifted = center - dt
        //"TOF400_TimeShiftCorr_RUN%i_SRC_corrections.dat"  center of strip
        FlagFileLRcorrection = true;
        FlagFileSlewingCorrection = true;
        FlagFileTimeShiftCorrection = true;
        //
        temp = true;
        // return kTRUE;
    }

    if (temp)
    {
        // check all files exist
        if (!IsFile(NameFileLRcorrection))
        {
            FlagFileLRcorrection = false;
            if (fVerbose)
            {
                cout << endl
                     << "BmnTof1HitProducer::Init(): File " << NameFileLRcorrection.Data() << " for LR correction is not found" << endl;
                cout << "Check /input folder for file" << endl;
            }
        }

        if (!IsFile(NameFileSlewingCorrection))
        {
            FlagFileSlewingCorrection = false;
            if (fVerbose)
            {
                cout << endl
                     << "BmnTof1HitProducer::Init(): File " << NameFileSlewingCorrection.Data() << " for Slewing correction is not found" << endl;
                cout << "Check /input folder for file" << endl;
            }
        }

        if (!IsFile(NameFileTimeShiftCorrection))
        {
            FlagFileTimeShiftCorrection = false; // !! false
            if (fVerbose)
            {
                cout << endl
                     << "BmnTof1HitProducer::Init(): File " << NameFileTimeShiftCorrection.Data() << " for TimeShift correction is not found" << endl;
                cout << "Check /input folder for file" << endl;
            }
        }
    }

    // return "true" in case the run is physical and correction files are found.
    if (FlagFileLRcorrection && FlagFileSlewingCorrection && FlagFileTimeShiftCorrection)
    {
        cout << "LR correction file         : " << NameFileLRcorrection.Data() << endl;
        cout << "Slewing correction file    : " << NameFileSlewingCorrection.Data() << endl;
        cout << "Time Shift correction file : " << NameFileTimeShiftCorrection.Data() << endl;

        return kTRUE;
    }

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

void BmnTof1HitProducer::OnlineWrite(const std::unique_ptr<TTree> &dataTree)
{
    if (!IsActive() || !IsOnlineActive())
        return;

    dataTree->Branch("BmnTof400Hit", &aTofHits);
    dataTree->Fill();
}

//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::findIdx(TClonesArray *data, Int_t side, Int_t &index, Double_t refT)
{
    Double_t minT = 1e4;
    for (Int_t m = 0; m < data->GetEntriesFast(); m++)
    {
        BmnTrigWaveDigit *signal = (BmnTrigWaveDigit *)data->At(m);
        if (signal->GetMod() != side)
            continue; // only look at PMT sides
        Double_t time = fabs(signal->GetTime() - refT);
        // cout << time << endl;
        if (time < minT)
        {
            minT = time;
            index = m;
        }
    }
    // cout << "-------------" << endl;
}

void BmnTof1HitProducer::getEnergyLoss(TClonesArray *bcData, Int_t side, Double_t t0Time, Double_t &dE, Double_t &time)
{

    Double_t adcBC = -1.;

    if (bcData->GetEntriesFast())
    {

        Int_t bcIdx;
        findIdx(bcData, side, bcIdx, t0Time);

        if (bcIdx < bcData->GetEntries())
        {
            BmnTrigWaveDigit *signal = (BmnTrigWaveDigit *)bcData->At(bcIdx);
            if (signal->GetMod() == side)
            {
                dE = signal->GetPeak(); // - pedBC1;
                time = signal->GetTime();
            }
            else
                cout << "--- Wrong PMT side ---" << endl;
        }
        else
            return;
    }
}

ClassImp(BmnTof1HitProducer)
