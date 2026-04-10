#include "BmnCSCHitMaker.h"

#include "BmnEventHeader.h"
#include "FairRunAna.h"
#include "TSystem.h"

#include <TChain.h>
#include <TRandom.h>
#include <TStopwatch.h>
#include <zlib.h>

static Double_t workTime = 0.0;

BmnCSCHitMaker::BmnCSCHitMaker()
    : fHitMatching(kTRUE)
{

    fInputPointsBranchName = "CSCPoint";
    fInputDigitsBranchName = "BmnCSCDigit";
    fInputDigitMatchesBranchName = "BmnCSCDigitMatch";

    fOutputHitsBranchName = "BmnCSCHit";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnCSCConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    fBmnCSCHitsArray = nullptr;
    fBmnCSCUpperClustersArray = nullptr;
    fBmnCSCLowerClustersArray = nullptr;
}

BmnCSCHitMaker::BmnCSCHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, TString alignFile, Bool_t isSrc)
    : fHitMatching(kTRUE)
{

    fInputPointsBranchName = "CSCPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnCSCDigit" : "CSC";
    fIsExp = isExp;
    fIsSrc = isSrc;

    fInputDigitMatchesBranchName = "BmnCSCDigitMatch";

    fOutputHitsBranchName = "BmnCSCHit";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnCSCConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    fBmnCSCHitsArray = nullptr;
    fBmnCSCUpperClustersArray = nullptr;
    fBmnCSCLowerClustersArray = nullptr;

    switch (run_period) {
        case 7:   // BM@N RUN-7 (and SRC)
            fCurrentConfig = BmnCSCConfiguration::RunSpring2018;
            if (run_number >= 2041 && run_number <= 3588) {
                fCurrentConfig = BmnCSCConfiguration::RunSRCSpring2018;
            }
            break;
        case 8:
            if (fIsSrc) {
                fCurrentConfig = BmnCSCConfiguration::RunSRC2021;
            } else {
                fCurrentConfig = BmnCSCConfiguration::FullCSC_Run8;
            }
            break;
        case 9:
            fCurrentConfig = BmnCSCConfiguration::Run9;
            break;
    }
}

BmnCSCHitMaker::~BmnCSCHitMaker()
{
    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }

    if (fBmnCSCHitsArray) {
        fBmnCSCHitsArray->Delete();
        delete fBmnCSCHitsArray;
    }
    if (fBmnCSCUpperClustersArray) {
        fBmnCSCUpperClustersArray->Delete();
        delete fBmnCSCUpperClustersArray;
    }
    if (fBmnCSCLowerClustersArray) {
        fBmnCSCLowerClustersArray->Delete();
        delete fBmnCSCLowerClustersArray;
    }
}

void BmnCSCHitMaker::LoadDetectorConfiguration()
{
    TString gPathCSCConfig = gSystem->Getenv("VMCWORKDIR");
    gPathCSCConfig += "/parameters/csc/XMLConfigs/";

    switch (fCurrentConfig) {
        case BmnCSCConfiguration::RunSpring2018:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRunSpring2018.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRunSpring2018.xml");
            if (fVerbose > 1)
                cout << "   Current CSC Configuration : RunSpring2018" << "\n";
            break;

        case BmnCSCConfiguration::RunSRCSpring2018:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRunSRCSpring2018.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRunSRCSpring2018.xml");
            if (fVerbose > 1)
                cout << "   Current CSC Configuration : RunSRCSpring2018" << "\n";
            break;

        case BmnCSCConfiguration::Run8:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRun8.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRun8.xml");
            if (fVerbose)
                cout << "   Current CSC Configuration : Run8" << "\n";
            break;

        case BmnCSCConfiguration::RunSRC2021:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRunSRC2021.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRunSRC2021.xml");
            if (fVerbose)
                cout << "   Current CSC Configuration : RunSRC2021" << "\n";
            break;

        case BmnCSCConfiguration::LargeCSC_Run8:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "LargeCSCRun8.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "LargeCSCRun8.xml");
            if (fVerbose)
                cout << "   Current CSC Configuration : LargeCSCRun8" << "\n";
            break;

        case BmnCSCConfiguration::FullCSC_Run8:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "FullCSCRun8.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "FullCSCRun8.xml");
            if (fVerbose)
                cout << "   Current CSC Configuration : FullCSCRun8 (4 small + 1 large chambers)" << "\n";
            break;

        case BmnCSCConfiguration::Run9:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRun9.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRun9.xml");
            if (fVerbose)
                cout << "   Current CSC Configuration : Run9 (4 small + 2 large chambers)" << "\n";
            break;

        default:
            StationSet = nullptr;
    }
}

InitStatus BmnCSCHitMaker::Init()
{

    if (fVerbose > 1)
        cout << "=================== BmnCSCHitMaker::Init() started ====================" << endl;

    // if the configuration is not set -> return a fatal error
    if (!fCurrentConfig)
        Fatal("BmnCSCHitMaker::Init()", " !!! Current CSC config is not set !!! ");

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnCSCDigitsArray = (TClonesArray*)ioman->GetObject(fInputDigitsBranchName);
    if (!fBmnCSCDigitsArray) {
        cout << "BmnCSCHitMaker::Init(): branch " << fInputDigitsBranchName << " not found! Task will be deactivated"
             << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnCSCDigitMatchesArray = (TClonesArray*)ioman->GetObject(fInputDigitMatchesBranchName);

    if (fVerbose > 1) {
        if (fBmnCSCDigitMatchesArray)
            cout << "  Strip matching information exists!\n";
        else
            cout << "  Strip matching information doesn`t exist!\n";
    }

    fBmnCSCHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "CSC", fBmnCSCHitsArray, kTRUE);
    fBmnCSCUpperClustersArray = new TClonesArray("StripCluster");
    ioman->Register("BmnCSCUpperCluster", "CSC", fBmnCSCUpperClustersArray, kTRUE);
    fBmnCSCLowerClustersArray = new TClonesArray("StripCluster");
    ioman->Register("BmnCSCLowerCluster", "CSC", fBmnCSCLowerClustersArray, kTRUE);

    // Create CSC detector ------------------------------------------------------

    LoadDetectorConfiguration();

    fField = FairRunAna::Instance()->GetField();
    if (!fField)
        Fatal("Init", "No Magnetic Field found");

    //--------------------------------------------------------------------------

    if (fVerbose > 1)
        cout << "=================== BmnCSCHitMaker::Init() finished ===================" << endl;

    return kSUCCESS;
}

void BmnCSCHitMaker::Exec(Option_t* opt)
{

    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;

    fBmnCSCHitsArray->Delete();
    fBmnCSCUpperClustersArray->Delete();
    fBmnCSCLowerClustersArray->Delete();

    BmnCSCLayer::SetLowerUniqueID(0);
    BmnCSCLayer::SetUpperUniqueID(0);

    if (fVerbose > 1)
        cout << "=================== BmnCSCHitMaker::Exec() started ====================" << endl;

    if (fVerbose > 1)
        cout << " BmnCSCHitMaker::Exec(), Number of BmnCSCDigits = " << fBmnCSCDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1)
        cout << "=================== BmnCSCHitMaker::Exec() finished ===================" << endl;

    sw.Stop();
    workTime += sw.RealTime();
}

void BmnCSCHitMaker::ProcessDigits()
{

    // FairMCPoint* MCPoint;
    BmnCSCDigit* digit;
    BmnMatch* strip_match;

    BmnCSCStation* station;
    BmnCSCModule* module;

    // Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (Int_t idigit = 0; idigit < fBmnCSCDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnCSCDigit*)fBmnCSCDigitsArray->At(idigit);
        if (!digit->IsGoodDigit())
            continue;
        station = StationSet->GetStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());

        if (module->SetStripSignalInLayer(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal()))
            AddedDigits++;

        if (fBmnCSCDigitMatchesArray) {
            strip_match = (BmnMatch*)fBmnCSCDigitMatchesArray->At(idigit);
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
    if (fVerbose > 1 && fBmnCSCDigitMatchesArray)
        cout << "   Added strip digit matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    // Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose > 1)
        cout << "   Calculated points  : " << NCalculatedPoints << "\n";
    if (fVerbose == 1)
        cout << "BmnCSCHitMaker: " << NCalculatedPoints << " hits\n";

    Int_t clear_matched_points_cnt = 0;   // points with the only one match-index

    map<Int_t, StripCluster> UniqueUpperClusters;
    map<Int_t, StripCluster> UniqueLowerClusters;

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        station = StationSet->GetStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            module = station->GetModule(iModule);

            for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                BmnCSCLayer layer = module->GetStripLayer(iLayer);

                for (size_t iCluster = 0; iCluster < layer.GetStripClusters().size(); ++iCluster) {
                    StripCluster cluster = layer.GetStripClusters()[iCluster];
                    cluster.SetModule(iModule);
                    cluster.SetStation(iStation);

                    // Saving all strip clusters into branches (each cluster have its own unique id in the current
                    // event)
                    if (cluster.GetType() == UpperStripLayer) {
                        new ((*fBmnCSCUpperClustersArray)[fBmnCSCUpperClustersArray->GetEntriesFast()])
                            StripCluster(cluster);
                    } else {
                        new ((*fBmnCSCLowerClustersArray)[fBmnCSCLowerClustersArray->GetEntriesFast()])
                            StripCluster(cluster);
                    }
                }
            }

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            // Int_t NPseudoIntersectionsInModule = module->GetNPseudoIntersections();
            Int_t NPseudoIntersectionsInModule = 0;   // !!! IGNORE PSEUDO INTERSECTIONS !!!

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule + NPseudoIntersectionsInModule; ++iPoint) {

                // Double_t sigL;
                // Double_t sigU;
                Double_t x, y, z;
                Double_t x_err, y_err, z_err;

                if (iPoint < NIntersectionPointsInModule) {
                    // sigL = module->GetIntersectionPoint_LowerLayerSripTotalSignal(iPoint);
                    // sigU = module->GetIntersectionPoint_UpperLayerSripTotalSignal(iPoint);
                    x = module->GetIntersectionPointX(iPoint);
                    y = module->GetIntersectionPointY(iPoint);
                    z = module->GetZPositionRegistered();
                    x_err = module->GetIntersectionPointXError(iPoint);
                    y_err = module->GetIntersectionPointYError(iPoint);
                    z_err = 0.0;
                } else {
                    // sigL =
                    // module->GetPseudoIntersection_LowerLayerSripTotalSignal(iPoint-NIntersectionPointsInModule); sigU
                    // = module->GetPseudoIntersection_UpperLayerSripTotalSignal(iPoint-NIntersectionPointsInModule);
                    x = module->GetPseudoIntersectionX(iPoint - NIntersectionPointsInModule);
                    y = module->GetPseudoIntersectionY(iPoint - NIntersectionPointsInModule);
                    z = module->GetZPositionRegistered();
                    x_err = module->GetPseudoIntersectionXError(iPoint - NIntersectionPointsInModule);
                    y_err = module->GetPseudoIntersectionYError(iPoint - NIntersectionPointsInModule);
                    z_err = 0.0;
                }

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
                x *= -1;   // invert to global X //Temporary switched off

                new ((*fBmnCSCHitsArray)[fBmnCSCHitsArray->GetEntriesFast()])
                    BmnCSCHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnCSCHit* hit = (BmnCSCHit*)fBmnCSCHitsArray->At(fBmnCSCHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnCSCHitsArray->GetEntriesFast() - 1);

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

                if (fVerbose) {
                    cout << "  glob(x:y:z) = ( " << x << " : " << y << " : " << z << "\n";
                    cout << "  hit(x:y:z) = ( " << hit->GetX() << " : " << hit->GetY() << " : " << hit->GetZ() << "\n";
                    cout << "\n";
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
                    // For future update

                    /*BmnMatch digiMatch;
                    Int_t idx0, idx1;
                    if(iPoint < NIntersectionPointsInModule) {
                        digiMatch = module->GetIntersectionPointDigitNumberMatch(iPoint);
                        idx0 = digiMatch.GetLink(0).GetIndex();
                        idx1 = digiMatch.GetLink(1).GetIndex();
                    }
                    else {
                        digiMatch = module->GetPseudoIntersectionDigitNumberMatch(iPoint-NIntersectionPointsInModule);
                        idx0 = digiMatch.GetLink(0).GetIndex();
                        idx1 = digiMatch.GetLink(0).GetIndex();
                    }
                    BmnMatch* digiMatch0 = (BmnMatch*)fBmnCSCDigitMatchesArray->At(idx0);
                    BmnMatch* digiMatch1 = (BmnMatch*)fBmnCSCDigitMatchesArray->At(idx1);

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
                        if (hitOk) break;
                    }

                    hit->SetType(hitOk);
                    if (!hitOk) hit->SetRefIndex(-1);*/
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
            for (Int_t i = 0; i < fBmnCSCHitsArray->GetEntriesFast(); i++) {
                BmnCSCHit* hit = (BmnCSCHit*) fBmnCSCHitsArray->At(i);
                if (hit->GetUpperClusterIndex() != it.first) continue;
                hit->SetUpperClusterIndex(fBmnCSCUpperClustersArray->GetEntriesFast());
            }
            it.second.SetUniqueID(fBmnCSCUpperClustersArray->GetEntriesFast());
            new ((*fBmnCSCUpperClustersArray)[fBmnCSCUpperClustersArray->GetEntriesFast()]) StripCluster(it.second);
        }
        for (auto it : UniqueLowerClusters) {
            for (Int_t i = 0; i < fBmnCSCHitsArray->GetEntriesFast(); i++) {
                BmnCSCHit* hit = (BmnCSCHit*) fBmnCSCHitsArray->At(i);
                if (hit->GetLowerClusterIndex() != it.first) continue;
                hit->SetLowerClusterIndex(fBmnCSCLowerClustersArray->GetEntriesFast());
            }
            it.second.SetUniqueID(fBmnCSCLowerClustersArray->GetEntriesFast());
            new ((*fBmnCSCLowerClustersArray)[fBmnCSCLowerClustersArray->GetEntriesFast()]) StripCluster(it.second);
        }
    */
    if (fVerbose > 1)
        cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnCSCHitMaker::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch(fOutputHitsBranchName, &fBmnCSCHitsArray);
    resultTree->Branch("BmnCSCUpperCluster", &fBmnCSCUpperClustersArray);
    resultTree->Branch("BmnCSCLowerCluster", &fBmnCSCLowerClustersArray);
    resultTree->Fill();
}

void BmnCSCHitMaker::Finish()
{
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if (TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    printf("Work time of BmnCSCHitMaker: %4.2f sec.\n", workTime);
}
