#include "BmnGemStripHitMaker.h"

#include "BmnEventHeader.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripStationSet_RunSummer2016.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "FairLinkManager.h"
#include "FairRunAna.h"
#include "TSystem.h"
#include "UniDetectorParameter.h"

#include <TChain.h>
#include <TRandom.h>
#include <TStopwatch.h>
#include <zlib.h>

static Double_t workTime = 0.0;

BmnGemStripHitMaker::BmnGemStripHitMaker()
    : fHitMatching(kTRUE)
{

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = "BmnGemStripDigit";
    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";

    fField = NULL;
    fFieldScale = 0.0;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    fBmnGemStripHitsArray = nullptr;
    fBmnGemUpperClustersArray = nullptr;
    fBmnGemLowerClustersArray = nullptr;
}

BmnGemStripHitMaker::BmnGemStripHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, Bool_t isSrc)
    : fHitMatching(kTRUE)
{

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnGemStripDigit" : "GEM";

    fRunPeriod = run_period;
    fRunNumber = run_number;

    fIsExp = isExp;
    fIsSrc = isSrc;

    fSignalLow = 0.;
    fSignalUp = DBL_MAX;

    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";

    fField = NULL;
    fFieldScale = 0.0;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    fBmnGemStripHitsArray = nullptr;
    fBmnGemUpperClustersArray = nullptr;
    fBmnGemLowerClustersArray = nullptr;

    switch (fRunPeriod) {
        case 5:   // BM@N RUN-5
            fCurrentConfig = BmnGemStripConfiguration::RunWinter2016;
            break;
        case 6:   // BM@N RUN-6
            fCurrentConfig = BmnGemStripConfiguration::RunSpring2017;
            break;
        case 7:   // BM@N RUN-7 (and SRC)
            if (fIsSrc) {
                fCurrentConfig = BmnGemStripConfiguration::RunSRCSpring2018;
            } else {
                fCurrentConfig = BmnGemStripConfiguration::RunSpring2018;
            }
            break;
        case 8:   // BM@N RUN-8
            if (fIsSrc) {
                fCurrentConfig = BmnGemStripConfiguration::RunSRC2021;
            } else {
                fCurrentConfig = BmnGemStripConfiguration::Run8;
            }
            break;
        case 9:   // BM@N RUN-9
            fCurrentConfig = BmnGemStripConfiguration::Run9;
            break;
    }
}

void BmnGemStripHitMaker::createGemDetector()
{
    TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");
    gPathGemConfig += "/parameters/gem/XMLConfigs/";

    // Create GEM detector ------------------------------------------------------
    switch (fCurrentConfig) {
        case BmnGemStripConfiguration::RunSummer2016:
            StationSet = new BmnGemStripStationSet_RunSummer2016(fCurrentConfig);
            if (fVerbose > 1)
                cout << "   Current GEM Configuration : RunSummer2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunWinter2016:
            StationSet = new BmnGemStripStationSet_RunWinter2016(fCurrentConfig);
            if (fVerbose > 1)
                cout << "   Current GEM Configuration : RunWinter2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2017:
            StationSet = new BmnGemStripStationSet_RunSpring2017(fCurrentConfig);
            // StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2017.xml");
            if (fVerbose > 1)
                cout << "   Current GEM Configuration : RunSpring2017" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2018:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2018.xml");
            if (fVerbose > 1)
                cout << "   Current GEM Configuration : RunSpring2018" << "\n";
            break;

        case BmnGemStripConfiguration::RunSRCSpring2018:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSRCSpring2018.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemRunSRCSpring2018.xml");
            if (fVerbose > 1)
                cout << "   Current GEM Configuration : GemRunSRCSpring2018" << "\n";
            break;

        case BmnGemStripConfiguration::RunSRC2021:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSRC2021.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemRunSRC2021.xml");
            if (fVerbose)
                cout << "   Current GEM Configuration : RunSRC2021" << "\n";
            break;

        case BmnGemStripConfiguration::Run8:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRun8.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemRun8.xml");
            if (fVerbose)
                cout << "   Current GEM Configuration : Run8" << "\n";
            break;

        case BmnGemStripConfiguration::Run9:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRun9.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemRun9.xml");
            if (fVerbose)
                cout << "   Current GEM Configuration : Run9" << "\n";
            break;

        default:
            StationSet = NULL;
    }

    const Int_t nStat = StationSet->GetNStations();
    fLorCor = new Double_t*[nStat];
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        const Int_t nParams = 3;
        fLorCor[iStat] = new Double_t[nParams];
        for (Int_t iPar = 0; iPar < nParams; iPar++) {
            fLorCor[iStat][iPar] = 0.0;
        }
    }

    if (fRunPeriod == 7) {
        if (fIsExp) {
            UniDetectorParameter* coeffLorCorrs =
                UniDetectorParameter::GetDetectorParameter("GEM", "lorentz_shift", fRunPeriod, fRunNumber);
            vector<UniValue*> shifts;
            if (coeffLorCorrs)
                coeffLorCorrs->GetValue(shifts);
            for (Int_t iStat = 0; iStat < nStat; iStat++) {
                const Int_t nParams = 3;
                for (Int_t iPar = 0; iPar < nParams; iPar++) {
                    fLorCor[iStat][iPar] = (coeffLorCorrs) ? ((LorentzShiftValue*)shifts[iStat])->ls[iPar] : 0.0;
                    // printf("fLorCor[%d][%d] = %f\n", iStat, iPar, fLorCor[iStat][iPar]);
                }
            }
        }
        // AZ-110625 } else if (fRunPeriod == 8) {
    } else if (fRunPeriod >= 8) {   // AZ-110625
        for (Int_t iStat = 0; iStat < nStat; iStat++) {
            // Pol2 approximation of the next configuration:
            // BmnGemStripMediumConfiguration::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_8T
            fLorCor[iStat][0] = -0.01710;
            fLorCor[iStat][1] = 0.01730;
            fLorCor[iStat][2] = 0.00024;
        }
    }

    if (fIsExp) {
        UniDetectorParameter* coeffAlignCorrs =
            UniDetectorParameter::GetDetectorParameter("GEM", "alignment_shift", fRunPeriod, fRunNumber);
        vector<UniValue*> algnShifts;
        if (coeffAlignCorrs)
            coeffAlignCorrs->GetValue(algnShifts);
        fAlignCor = new Double_t**[nStat];
        for (Int_t iStat = 0; iStat < nStat; iStat++) {
            const Int_t nParams = 3;
            Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
            fAlignCor[iStat] = new Double_t*[nModul];
            for (Int_t iMod = 0; iMod < nModul; iMod++) {
                fAlignCor[iStat][iMod] = new Double_t[nParams];
                for (Int_t iPar = 0; iPar < nParams; iPar++) {
                    // cout << iStat << " " << iMod << ": " << ((AlignmentValue*)algnShifts[iStat * nModul +
                    // iMod])->value[iPar] << endl;
                    fAlignCor[iStat][iMod][iPar] =
                        (coeffAlignCorrs) ? ((AlignmentValue*)algnShifts[iStat * nModul + iMod])->value[iPar] : 0.0;
                }
            }
        }
    }
}

BmnGemStripHitMaker::~BmnGemStripHitMaker()
{
    if (fIsExp) {
        for (Int_t iStat = 0; iStat < StationSet->GetNStations(); iStat++) {
            for (Int_t iMod = 0; iMod < StationSet->GetGemStation(iStat)->GetNModules(); iMod++) {
                delete[] fAlignCor[iStat][iMod];
            }
            delete[] fAlignCor[iStat];
        }
        delete[] fAlignCor;
    }
    for (Int_t iStat = 0; iStat < StationSet->GetNStations(); iStat++) {
        delete[] fLorCor[iStat];
    }
    delete[] fLorCor;

    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }

    if (fBmnGemStripHitsArray) {
        fBmnGemStripHitsArray->Delete();
        delete fBmnGemStripHitsArray;
    }
    if (fBmnGemUpperClustersArray) {
        fBmnGemUpperClustersArray->Delete();
        delete fBmnGemUpperClustersArray;
    }
    if (fBmnGemLowerClustersArray) {
        fBmnGemLowerClustersArray->Delete();
        delete fBmnGemLowerClustersArray;
    }
}

InitStatus BmnGemStripHitMaker::Init()
{

    if (fVerbose > 1)
        cout << "=================== BmnGemStripHitMaker::Init() started ===============" << endl;

    createGemDetector();

    // if GEM configuration is not set -> return a fatal error
    if (!fCurrentConfig)
        Fatal("BmnGemStripHitMaker::Init()", " !!! Current GEM config is not set !!! ");

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripDigitsArray = (TClonesArray*)ioman->GetObject(fInputDigitsBranchName);
    if (!fBmnGemStripDigitsArray) {
        cout << "BmnGemStripHitMaker::Init(): branch " << fInputDigitsBranchName
             << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnGemStripDigitMatchesArray = (TClonesArray*)ioman->GetObject(fInputDigitMatchesBranchName);

    if (fVerbose > 1) {
        if (fBmnGemStripDigitMatchesArray)
            cout << "  Strip matching information exists!\n";
        else
            cout << "  Strip matching information doesn`t exist!\n";
    }

    fBmnGemStripHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "GEM", fBmnGemStripHitsArray, kTRUE);
    fBmnGemUpperClustersArray = new TClonesArray("StripCluster");
    ioman->Register("BmnGemUpperCluster", "GEM", fBmnGemUpperClustersArray, kTRUE);
    fBmnGemLowerClustersArray = new TClonesArray("StripCluster");
    ioman->Register("BmnGemLowerCluster", "GEM", fBmnGemLowerClustersArray, kTRUE);

    fField = FairRunAna::Instance()->GetField();
    if (!fField)
        // Fatal("Init", "No Magnetic Field found");
        Warning("Init", "No Magnetic Field found");

    //--------------------------------------------------------------------------

    if (fVerbose > 1)
        cout << "=================== BmnGemStripHitMaker::Init() finished ==============" << endl;

    return kSUCCESS;
}

void BmnGemStripHitMaker::Exec(Option_t* opt)
{

    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;

    fBmnGemStripHitsArray->Delete();
    fBmnGemUpperClustersArray->Delete();
    fBmnGemLowerClustersArray->Delete();

    BmnGemStripLayer::SetLowerUniqueID(0);
    BmnGemStripLayer::SetUpperUniqueID(0);

    if (fVerbose > 1)
        cout << "=================== BmnGemStripHitMaker::Exec() started ===============" << endl;

    if (fVerbose > 1)
        cout << " BmnGemStripHitMaker::Exec(), Number of BmnGemStripDigits = "
             << fBmnGemStripDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1)
        cout << "=================== BmnGemStripHitMaker::Exec() finished ==============" << endl;

    sw.Stop();
    workTime += sw.RealTime();
}

void BmnGemStripHitMaker::ProcessDigits()
{

    FairMCPoint* MCPoint;
    BmnGemStripDigit* digit;
    BmnMatch* strip_match;   // MC-information for a strip

    // Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (UInt_t idigit = 0; idigit < fBmnGemStripDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnGemStripDigit*)fBmnGemStripDigitsArray->At(idigit);
        if (!digit->IsGoodDigit())
            continue;

        if (digit->GetStripSignal() < fSignalLow || digit->GetStripSignal() > fSignalUp)
            continue;

        BmnGemStripStation* station = StationSet->GetGemStation(digit->GetStation());
        BmnGemStripModule* module = station->GetModule(digit->GetModule());

        if (module->SetStripSignalInLayer(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal()))
            AddedDigits++;

        // Add a MC-match to the current strip if this MC-match array exists
        if (fBmnGemStripDigitMatchesArray) {
            strip_match = (BmnMatch*)fBmnGemStripDigitMatchesArray->At(idigit);
            if (module->SetStripMatchInLayer(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match))
                AddedStripDigitMatches++;
        }

        // Add a digit number match to the current strip
        BmnMatch stripDigitNumberMatch;   // digit number information for the current strip
        stripDigitNumberMatch.AddLink(1.0, idigit);
        module->SetStripDigitNumberMatchInLayer(digit->GetStripLayer(), digit->GetStripNumber(), stripDigitNumberMatch);
    }

    if (fVerbose > 1)
        cout << "   Processed strip digits  : " << AddedDigits << "\n";
    if (fVerbose > 1 && fBmnGemStripDigitMatchesArray)
        cout << "   Added strip digit MC-matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    // Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose > 1)
        cout << "   Calculated points  : " << NCalculatedPoints << "\n";
    if (fVerbose == 1)
        cout << "BmnGemStripHitMaker: " << NCalculatedPoints << " hits\n";

    Int_t clear_matched_points_cnt = 0;   // points with the only one match-index

    map<Int_t, StripCluster> UniqueUpperClusters;
    map<Int_t, StripCluster> UniqueLowerClusters;

    int fieldFlag = 0;   // AZ-120823
    Double_t Bx = 0.;
    Double_t By = 0.;
    if (fField) {
        if (Abs(fField->GetBy(0., 0., 0.)) > FLT_EPSILON)
            fieldFlag = 1;                          // AZ-120823
        Bx = Abs(fField->GetBx(0.0, 0.0, 135.0));   // AZ-090823 - fixed value (for more stability)
        By = Abs(fField->GetBy(0.0, 0.0, 135.0));   // AZ-090823
    }

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnGemStripStation* station = StationSet->GetGemStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnGemStripModule* module = station->GetModule(iModule);

            for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                BmnGemStripLayer layer = module->GetStripLayer(iLayer);

                for (Int_t iCluster = 0; iCluster < layer.GetStripClusters().size(); ++iCluster) {
                    StripCluster cluster = layer.GetStripClusters()[iCluster];
                    cluster.SetModule(iModule);
                    cluster.SetStation(iStation);

                    // Saving all strip clusters into branches (each cluster have its own unique id in the current
                    // event)
                    if (cluster.GetType() == UpperStripLayer) {
                        new ((*fBmnGemUpperClustersArray)[fBmnGemUpperClustersArray->GetEntriesFast()])
                            StripCluster(cluster);
                    } else {
                        new ((*fBmnGemLowerClustersArray)[fBmnGemLowerClustersArray->GetEntriesFast()])
                            StripCluster(cluster);
                    }
                }
            }

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            // Int_t NPseudoIntersectionsInModule = module->GetNPseudoIntersections();
            Int_t NPseudoIntersectionsInModule = 0;   // !!! IGNORE PSEUDO INTERSECTIONS !!!

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule + NPseudoIntersectionsInModule; ++iPoint) {

                Double_t threshold = 2000000;   // ADC

                Double_t sigL;
                Double_t sigU;
                Double_t x, y, z;
                Double_t x_err, y_err, z_err;

                if (iPoint < NIntersectionPointsInModule) {
                    sigL = module->GetIntersectionPoint_LowerLayerSripTotalSignal(iPoint);
                    sigU = module->GetIntersectionPoint_UpperLayerSripTotalSignal(iPoint);
                    x = module->GetIntersectionPointX(iPoint);
                    y = module->GetIntersectionPointY(iPoint);
                    z = module->GetZPositionRegistered();
                    x_err = module->GetIntersectionPointXError(iPoint);
                    y_err = module->GetIntersectionPointYError(iPoint);
                    z_err = 0.0;
                } else {
                    sigL =
                        module->GetPseudoIntersection_LowerLayerSripTotalSignal(iPoint - NIntersectionPointsInModule);
                    sigU =
                        module->GetPseudoIntersection_UpperLayerSripTotalSignal(iPoint - NIntersectionPointsInModule);
                    x = module->GetPseudoIntersectionX(iPoint - NIntersectionPointsInModule);
                    y = module->GetPseudoIntersectionY(iPoint - NIntersectionPointsInModule);
                    z = module->GetZPositionRegistered();
                    x_err = module->GetPseudoIntersectionXError(iPoint - NIntersectionPointsInModule);
                    y_err = module->GetPseudoIntersectionYError(iPoint - NIntersectionPointsInModule);
                    z_err = 0.0;
                }

                if (fIsExp && !fIsSrc)
                    z += fAlignCor[iStation][iModule][2];   // alignment implementation

                // Transform hit coordinates from local coordinate system of GEM-planes to global
                if (TransfSet) {
                    Plane3D::Point glob_point = TransfSet->ApplyTransforms(Plane3D::Point(-x, y, z), iStation, iModule);
                    x = -glob_point.X();
                    y = glob_point.Y();
                    z = glob_point.Z();
                }

                Int_t RefMCIndex = -1;

                // MC-matching for the current hit (define RefMCIndex)) ---------
                BmnMatch mc_match_hit;
                if (iPoint < NIntersectionPointsInModule) {
                    mc_match_hit = module->GetIntersectionPointMatch(iPoint);
                } else {
                    mc_match_hit = module->GetPseudoIntersectionMatch(iPoint - NIntersectionPointsInModule);
                }

                Int_t most_probably_index = -1;
                Double_t max_weight = 0;

                Int_t n_links = mc_match_hit.GetNofLinks();
                if (n_links == 1)
                    clear_matched_points_cnt++;
                for (Int_t ilink = 0; ilink < n_links; ilink++) {
                    Int_t index = mc_match_hit.GetLink(ilink).GetIndex();
                    Double_t weight = mc_match_hit.GetLink(ilink).GetWeight();
                    if (weight > max_weight) {
                        max_weight = weight;
                        most_probably_index = index;
                    }
                }

                RefMCIndex = most_probably_index;
                //--------------------------------------------------------------

                // Add hit ------------------------------------------------------
                x *= -1;   // invert to global X

                if (fIsExp) {
                    x += fAlignCor[iStation][iModule][0];
                    y += fAlignCor[iStation][iModule][1];
                }

                if (fieldFlag) {   // AZ-120823
                    // AZ-120823 if (Abs(fField->GetBy(0., 0., 0.)) > FLT_EPSILON) {
                    if (!fIsSrc) {   // For SRC lorentz corrections included into fAlignCor
                        // AZ-090823 Double_t Bx = Abs(fField->GetBx(x, y, z));
                        // AZ-090823 Double_t By = Abs(fField->GetBy(x, y, z));
                        // Bx = Abs(fField->GetBx(x, y, z)); //AZ-130823
                        // By = Abs(fField->GetBy(x, y, z)); //AZ-130823
                        Double_t yCor =
                            fLorCor[iStation][0] + fLorCor[iStation][1] * Bx + fLorCor[iStation][2] * Bx * Bx;
                        Double_t xCor =
                            fLorCor[iStation][0] + fLorCor[iStation][1] * By + fLorCor[iStation][2] * By * By;
                        if (!fIsExp)
                            yCor += 0.014;   // AZ-191223
                        if (int(TMath::Abs(By) + 0.5) == 4) {
                            // AZ-180822 - 0.4T
                            xCor += 0.0160;
                            yCor += 0.0140;
                        } else if (int(TMath::Abs(By) + 0.5) == 6) {
                            // AZ-180822 - 0.6T
                            xCor += 0.0180;
                            yCor += 0.0140;
                        } else {
                            // AZ-030823 - new field
                            // xCor += 0.0175;
                            // yCor += 0.0150;
                        }

                        Int_t sign = (module->GetElectronDriftDirection() == ForwardZAxisEDrift) ? +1 : -1;
                        x += xCor * sign;
                        y += yCor * sign;
                    }
                }

                new ((*fBmnGemStripHitsArray)[fBmnGemStripHitsArray->GetEntriesFast()])
                    BmnGemStripHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnGemStripHit* hit =
                    (BmnGemStripHit*)fBmnGemStripHitsArray->At(fBmnGemStripHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnGemStripHitsArray->GetEntriesFast() - 1);

                if (iPoint < NIntersectionPointsInModule) {
                    hit->SetDigitNumberMatch(
                        module->GetIntersectionPointDigitNumberMatch(iPoint));   // digit number match for the hit
                    hit->SetUpperClusterIndex(module->GetUpperCluster(iPoint).GetUniqueID());
                    hit->SetLowerClusterIndex(module->GetLowerCluster(iPoint).GetUniqueID());
                    hit->SetPseudo(false);
                } else {
                    hit->SetDigitNumberMatch(module->GetPseudoIntersectionDigitNumberMatch(
                        iPoint - NIntersectionPointsInModule));   // digit number match for the hit
                    hit->SetUpperClusterIndex(
                        module->GetUpperCluster_PseudoIntersections(iPoint - NIntersectionPointsInModule)
                            .GetUniqueID());
                    hit->SetLowerClusterIndex(
                        module->GetLowerCluster_PseudoIntersections(iPoint - NIntersectionPointsInModule)
                            .GetUniqueID());
                    hit->SetPseudo(true);
                }
                /*
                                StripCluster ucls = module->GetUpperCluster(iPoint);
                                StripCluster lcls = module->GetLowerCluster(iPoint);
                                ucls.SetModule(iModule);
                                lcls.SetModule(iModule);
                                ucls.SetStation(iStation);
                                lcls.SetStation(iStation);
                                UniqueUpperClusters[ucls.GetUniqueID()] = ucls;
                                UniqueLowerClusters[lcls.GetUniqueID()] = lcls;
                                hit->SetUpperClusterIndex(ucls.GetUniqueID());
                                hit->SetLowerClusterIndex(lcls.GetUniqueID());
                */
                if (fHitMatching) {
                    BmnMatch digiMatch;
                    Int_t idx0, idx1;
                    if (iPoint < NIntersectionPointsInModule) {
                        digiMatch = module->GetIntersectionPointDigitNumberMatch(iPoint);
                        idx0 = digiMatch.GetLink(0).GetIndex();
                        idx1 = digiMatch.GetLink(1).GetIndex();
                    } else {
                        digiMatch = module->GetPseudoIntersectionDigitNumberMatch(iPoint - NIntersectionPointsInModule);
                        idx0 = digiMatch.GetLink(0).GetIndex();
                        idx1 = digiMatch.GetLink(0).GetIndex();
                    }
                    BmnMatch* digiMatch0 = (BmnMatch*)fBmnGemStripDigitMatchesArray->At(idx0);
                    BmnMatch* digiMatch1 = (BmnMatch*)fBmnGemStripDigitMatchesArray->At(idx1);

                    Bool_t hitOk = kFALSE;
                    for (Int_t ilink = 0; ilink < digiMatch0->GetNofLinks(); ilink++) {
                        Int_t iindex = digiMatch0->GetLink(ilink).GetIndex();
                        for (Int_t jlink = 0; jlink < digiMatch1->GetNofLinks(); jlink++) {
                            Int_t jindex = digiMatch1->GetLink(jlink).GetIndex();
                            if (iindex == jindex) {
                                hitOk = kTRUE;
                                break;
                            }
                        }
                        if (hitOk)
                            break;
                    }

                    hit->SetType(hitOk);
                    if (!hitOk)
                        hit->SetRefIndex(-1);
                    //--------------------------------------------------------------

                    // hit MC-matching ----------------------------------------------
                    FairRootManager::Instance()->SetUseFairLinks(kTRUE);
                    BmnMatch hitMatch;
                    if (iPoint < NIntersectionPointsInModule) {
                        hitMatch = module->GetIntersectionPointMatch(iPoint);
                    } else {
                        hitMatch = module->GetPseudoIntersectionMatch(iPoint - NIntersectionPointsInModule);
                    }

                    for (BmnLink lnk : hitMatch.GetLinks())
                        hit->AddLink(FairLink(-1, lnk.GetIndex(), lnk.GetWeight()));
                    FairRootManager::Instance()->SetUseFairLinks(kFALSE);
                }
                //--------------------------------------------------------------
            }
        }
    }
    /*
        for (auto it : UniqueUpperClusters) {
            for (Int_t i = 0; i < fBmnGemStripHitsArray->GetEntriesFast(); i++) {
                BmnGemStripHit* hit = (BmnGemStripHit*)fBmnGemStripHitsArray->At(i);
                if (hit->GetUpperClusterIndex() != it.first) continue;
                hit->SetUpperClusterIndex(fBmnGemUpperClustersArray->GetEntriesFast());
            }
            it.second.SetUniqueID(fBmnGemUpperClustersArray->GetEntriesFast());
            new ((*fBmnGemUpperClustersArray)[fBmnGemUpperClustersArray->GetEntriesFast()]) StripCluster(it.second);
        }
        for (auto it : UniqueLowerClusters) {
            for (Int_t i = 0; i < fBmnGemStripHitsArray->GetEntriesFast(); i++) {
                BmnGemStripHit* hit = (BmnGemStripHit*)fBmnGemStripHitsArray->At(i);
                if (hit->GetLowerClusterIndex() != it.first) continue;
                hit->SetLowerClusterIndex(fBmnGemLowerClustersArray->GetEntriesFast());
            }
            it.second.SetUniqueID(fBmnGemLowerClustersArray->GetEntriesFast());
            new ((*fBmnGemLowerClustersArray)[fBmnGemLowerClustersArray->GetEntriesFast()]) StripCluster(it.second);
        }
    */
    if (fVerbose > 1)
        cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnGemStripHitMaker::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch(fOutputHitsBranchName, &fBmnGemStripHitsArray);
    resultTree->Branch("BmnGemUpperCluster", &fBmnGemUpperClustersArray);
    resultTree->Branch("BmnGemLowerCluster", &fBmnGemLowerClustersArray);
    resultTree->Fill();
}

void BmnGemStripHitMaker::Finish()
{
    printf("Work time of BmnGemStripHitMaker: %4.2f sec.\n", workTime);
}
