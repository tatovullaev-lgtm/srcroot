#include "BmnVSPHitMaker.h"

#include "BmnVSPModule.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TSystem.h"
#include "UniDetectorParameter.h"

#include <TStopwatch.h>

static Double_t workTime = 0.0;

BmnVSPHitMaker::BmnVSPHitMaker()
    : fHitMatching(kTRUE)
{

    fInputPointsBranchName = "VSPPoint";
    fInputDigitsBranchName = "BmnVSPDigit";
    fInputDigitMatchesBranchName = "BmnVSPDigitMatch";

    fOutputHitsBranchName = "BmnVSPHit";

    fCurrentConfig = BmnVSPConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
}

BmnVSPHitMaker::BmnVSPHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp)
    : fHitMatching(kTRUE)
{
    fIsExp = isExp;

    fRunPeriod = run_period;
    fRunNumber = run_number;

    fInputPointsBranchName = "VSPPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnVSPDigit" : "VSP";
    fInputDigitMatchesBranchName = "BmnVSPDigitMatch";

    fOutputHitsBranchName = "BmnVSPHit";

    fCurrentConfig = BmnVSPConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    fSignalLow = 0.;
    fSignalUp = DBL_MAX;

    switch (fRunPeriod) {

        case 9:   // BM@N RUN-9
            fCurrentConfig = BmnVSPConfiguration::Run9;
            break;
        case 777:   // test purpose
            fCurrentConfig = BmnVSPConfiguration::Run9;
            break;
    }
}

void BmnVSPHitMaker::createVSPDetector()
{
    TString gPathVSPConfig = gSystem->Getenv("VMCWORKDIR");
    gPathVSPConfig += "/parameters/vsp/XMLConfigs/";

    // Create VSP detector --------------------------------------------------
    switch (fCurrentConfig) {

        case BmnVSPConfiguration::Run9:
            StationSet = new BmnVSPStationSet(gPathVSPConfig + "VSP_Run9.xml");
            TransfSet = new BmnVSPTransform();
            TransfSet->LoadFromXMLFile(gPathVSPConfig + "VSP_Run9.xml");
            if (fVerbose)
                cout << "   Current VSP Configuration : Run9" << "\n";
            break;

        default:
            StationSet = nullptr;
    }

    if (fIsExp) {
        const Int_t nStat = StationSet->GetNStations();
        UniDetectorParameter* coeffAlignCorrs =
            UniDetectorParameter::GetDetectorParameter("VSP", "alignment_shift", fRunPeriod, fRunNumber);
        vector<UniValue*> algnShifts;
        if (coeffAlignCorrs)
            coeffAlignCorrs->GetValue(algnShifts);
        fAlignCor = new Double_t**[nStat];
        for (Int_t iStat = 0; iStat < nStat; iStat++) {
            const Int_t nParams = 3;
            Int_t nModul = StationSet->GetVSPStation(iStat)->GetNModules();
            fAlignCor[iStat] = new Double_t*[nModul];
            for (Int_t iMod = 0; iMod < nModul; iMod++) {
                fAlignCor[iStat][iMod] = new Double_t[nParams];
                for (Int_t iPar = 0; iPar < nParams; iPar++) {
                    fAlignCor[iStat][iMod][iPar] = 0.0;
                }
            }
        }

        for (size_t i = 0; i < algnShifts.size(); ++i) {
            // cout << ((AlignmentValue*)algnShifts[i])->value[0] << endl;
            // cout << ((AlignmentValue*)algnShifts[i])->value[1] << endl;
            // cout << ((AlignmentValue*)algnShifts[i])->value[2] << endl;
            Int_t st = ((AlignmentValue*)algnShifts[i])->station;
            Int_t mod = ((AlignmentValue*)algnShifts[i])->module;
            fAlignCor[st][mod][0] = ((AlignmentValue*)algnShifts[i])->value[0];
            fAlignCor[st][mod][1] = ((AlignmentValue*)algnShifts[i])->value[1];
            fAlignCor[st][mod][2] = ((AlignmentValue*)algnShifts[i])->value[2];
        }
    }
}

BmnVSPHitMaker::~BmnVSPHitMaker()
{

    if (fIsExp && fRunPeriod >= 9) {
        for (Int_t iStat = 0; iStat < StationSet->GetNStations(); iStat++) {
            for (Int_t iMod = 0; iMod < StationSet->GetVSPStation(iStat)->GetNModules(); iMod++)
                delete[] fAlignCor[iStat][iMod];
            delete[] fAlignCor[iStat];
        }
        delete[] fAlignCor;
    }

    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }
}

InitStatus BmnVSPHitMaker::Init()
{

    if (fVerbose > 1)
        cout << "=================== BmnVSPHitMaker::Init() started ================" << endl;

    if (fRunPeriod < 9) {
        SetActive(kFALSE);
        return kERROR;
    }

    createVSPDetector();

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnVSPDigitsArray = (TClonesArray*)ioman->GetObject(fInputDigitsBranchName);
    if (!fBmnVSPDigitsArray) {
        cout << "BmnVSPHitMaker::Init(): branch " << fInputDigitsBranchName << " not found! Task will be deactivated"
             << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fBmnVSPDigitMatchesArray = (TClonesArray*)ioman->GetObject(fInputDigitMatchesBranchName);

    if (fVerbose > 1) {
        if (fBmnVSPDigitMatchesArray)
            cout << "  Strip matching information exists!\n";
        else
            cout << "  Strip matching information doesn`t exist!\n";
    }

    fBmnVSPHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "VSP", fBmnVSPHitsArray, kTRUE);
    fBmnVSPUpperClustersArray = new TClonesArray("StripCluster");
    ioman->Register("BmnVSPUpperCluster", "VSP", fBmnVSPUpperClustersArray, kTRUE);
    fBmnVSPLowerClustersArray = new TClonesArray("StripCluster");
    ioman->Register("BmnVSPLowerCluster", "VSP", fBmnVSPLowerClustersArray, kTRUE);

    if (fVerbose > 1)
        cout << "=================== BmnVSPHitMaker::Init() finished ===============" << endl;

    return kSUCCESS;
}

void BmnVSPHitMaker::Exec(Option_t* opt)
{

    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;

    fBmnVSPHitsArray->Delete();
    fBmnVSPUpperClustersArray->Delete();
    fBmnVSPLowerClustersArray->Delete();

    BmnVSPLayer::SetLowerUniqueID(0);
    BmnVSPLayer::SetUpperUniqueID(0);

    if (fVerbose > 1)
        cout << "=================== BmnVSPHitMaker::Exec() started ================" << endl;
    if (fVerbose > 1)
        cout << " BmnVSPHitMaker::Exec(), Number of BmnVSPDigits = " << fBmnVSPDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1)
        cout << "=================== BmnVSPHitMaker::Exec() finished ===============" << endl;

    sw.Stop();
    workTime += sw.RealTime();
}

void BmnVSPHitMaker::ProcessDigits()
{

    // FairMCPoint* MCPoint;
    BmnVSPDigit* digit;
    BmnMatch* strip_match;   // MC-information for a strip

    BmnVSPStation* station;
    BmnVSPModule* module;

    // Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (Int_t idigit = 0; idigit < fBmnVSPDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnVSPDigit*)fBmnVSPDigitsArray->At(idigit);
        if (!digit->IsGoodDigit())
            continue;

        if (digit->GetStripSignal() < fSignalLow || digit->GetStripSignal() > fSignalUp)
            continue;

        station = StationSet->GetVSPStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());
        if (!module)
            return;

        if (module->SetStripSignalInLayerByZoneId(digit->GetStripLayer(), digit->GetStripNumber(),
                                                  digit->GetStripSignal()))
            AddedDigits++;

        // Add a MC-match to the current strip if this MC-match array exists
        if (fBmnVSPDigitMatchesArray) {
            strip_match = (BmnMatch*)fBmnVSPDigitMatchesArray->At(idigit);
            if (module->SetStripMatchInLayerByZoneId(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match))
                AddedStripDigitMatches++;
        }

        // Add a digit number match to the current strip
        BmnMatch stripDigitNumberMatch;   // digit number information for the current strip
        stripDigitNumberMatch.AddLink(1.0, idigit);
        module->SetStripDigitNumberMatchInLayerByZoneId(digit->GetStripLayer(), digit->GetStripNumber(),
                                                        stripDigitNumberMatch);
    }

    if (fVerbose > 1)
        cout << "   Processed strip digits  : " << AddedDigits << "\n";
    if (fVerbose > 1 && fBmnVSPDigitMatchesArray)
        cout << "   Added strip digit MC-matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    // Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose > 1)
        cout << "   Calculated points  : " << NCalculatedPoints << "\n";
    if (fVerbose == 1)
        cout << "BmnVSPHitMaker: " << NCalculatedPoints << " hits\n";

    Int_t clear_matched_points_cnt = 0;   // points with the only one match-indexes

    map<Int_t, StripCluster> UniqueUpperClusters;
    map<Int_t, StripCluster> UniqueLowerClusters;

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        station = StationSet->GetVSPStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            module = station->GetModule(iModule);

            for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                BmnVSPLayer layer = module->GetStripLayer(iLayer);

                for (size_t iCluster = 0; iCluster < layer.GetStripClusters().size(); ++iCluster) {
                    StripCluster cluster = layer.GetStripClusters()[iCluster];
                    cluster.SetModule(iModule);
                    cluster.SetStation(iStation);

                    // Saving all strip clusters into branches (each cluster have its own unique id in the current
                    // event)
                    if (cluster.GetType() == UpperStripLayer) {
                        new ((*fBmnVSPUpperClustersArray)[fBmnVSPUpperClustersArray->GetEntriesFast()])
                            StripCluster(cluster);
                    } else {
                        new ((*fBmnVSPLowerClustersArray)[fBmnVSPLowerClustersArray->GetEntriesFast()])
                            StripCluster(cluster);
                    }
                }
            }

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            // Int_t NPseudoIntersectionsInModule = module->GetNPseudoIntersections();
            Int_t NPseudoIntersectionsInModule = 0;   // !!! IGNORE PSEUDO INTERSECTIONS !!!

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule + NPseudoIntersectionsInModule; ++iPoint) {

                // Double_t threshold = 2000000;// ADC

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

                z += fIsExp ? fAlignCor[iStation][iModule][2] : 0.;   // alignment shift

                // Transform hit coordinates from local coordinate system to global
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
                Double_t deltaX = fIsExp ? fAlignCor[iStation][iModule][0] : 0.;
                Double_t deltaY = fIsExp ? fAlignCor[iStation][iModule][1] : 0.;

                x += deltaX;
                y += deltaY;

                new ((*fBmnVSPHitsArray)[fBmnVSPHitsArray->GetEntriesFast()])
                    BmnVSPHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnVSPHit* hit = (BmnVSPHit*)fBmnVSPHitsArray->At(fBmnVSPHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnVSPHitsArray->GetEntriesFast() - 1);

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
                //--------------------------------------------------------------
                /*
                                StripCluster ucls = module->GetUpperCluster(iPoint);
                                StripCluster lcls = module->GetLowerCluster(iPoint);
                                ucls.SetModule(iModule);
                                lcls.SetModule(iModule);
                                ucls.SetStation(iStation);
                                lcls.SetStation(iStation);

                                Int_t upperClusterIndex = ucls.GetUniqueID();
                                Int_t lowerClusterIndex = lcls.GetUniqueID();

                                //Filtration of duplicate clusters in the current module for the same type of strips
                                Bool_t duplicateUpperCluster = false;
                                for(auto it : UniqueUpperClusters) {
                                    StripCluster cluster = it.second;

                                    if(cluster.GetClusterSize() != ucls.GetClusterSize()) continue;

                                    Int_t identic_cnt = 0;
                                    for(Int_t istrip = 0; istrip < cluster.GetClusterSize(); ++istrip) {
                                        Int_t strip_num = cluster.Strips.at(istrip);
                                        Double_t strip_signal = cluster.Signals.at(istrip);

                                        if( (strip_num == ucls.Strips.at(istrip)) && (strip_signal ==
                   ucls.Signals.at(istrip)) ) identic_cnt++; else break;
                                    }
                                    if(identic_cnt == ucls.GetClusterSize()) {
                                        upperClusterIndex = cluster.GetUniqueID();
                                        duplicateUpperCluster = true;
                                        break;
                                    }
                                }

                                Bool_t duplicateLowerCluster = false;
                                for(auto it : UniqueLowerClusters) {
                                    StripCluster cluster = it.second;

                                    if(cluster.GetClusterSize() != lcls.GetClusterSize()) continue;

                                    Int_t identic_cnt = 0;
                                    for(Int_t istrip = 0; istrip < cluster.GetClusterSize(); ++istrip) {
                                        Int_t strip_num = cluster.Strips.at(istrip);
                                        Double_t strip_signal = cluster.Signals.at(istrip);

                                        if( (strip_num == lcls.Strips.at(istrip)) && (strip_signal ==
                   lcls.Signals.at(istrip)) ) identic_cnt++; else break;
                                    }
                                    if(identic_cnt == lcls.GetClusterSize()) {
                                        lowerClusterIndex = cluster.GetUniqueID();
                                        duplicateLowerCluster = true;
                                        break;
                                    }
                                }
                                //--------------------------------------------------------------

                                if(!duplicateUpperCluster) UniqueUpperClusters[upperClusterIndex] = ucls;
                                if(!duplicateLowerCluster) UniqueLowerClusters[lowerClusterIndex] = lcls;
                                hit->SetUpperClusterIndex(upperClusterIndex);
                                hit->SetLowerClusterIndex(lowerClusterIndex);
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
                    BmnMatch* digiMatch0 = (BmnMatch*)fBmnVSPDigitMatchesArray->At(idx0);
                    BmnMatch* digiMatch1 = (BmnMatch*)fBmnVSPDigitMatchesArray->At(idx1);

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
            for (Int_t i = 0; i < fBmnVSPHitsArray->GetEntriesFast(); i++) {
                BmnVSPHit* hit = (BmnVSPHit*)fBmnVSPHitsArray->At(i);
                if (hit->GetUpperClusterIndex() != it.first) continue;
                hit->SetUpperClusterIndex(fBmnVSPUpperClustersArray->GetEntriesFast());
            }
            it.second.SetUniqueID(fBmnVSPUpperClustersArray->GetEntriesFast());
            new ((*fBmnVSPUpperClustersArray)[fBmnVSPUpperClustersArray->GetEntriesFast()]) StripCluster(it.second);
        }
        for (auto it : UniqueLowerClusters) {
            for (Int_t i = 0; i < fBmnVSPHitsArray->GetEntriesFast(); i++) {
                BmnVSPHit* hit = (BmnVSPHit*)fBmnVSPHitsArray->At(i);
                if (hit->GetLowerClusterIndex() != it.first) continue;
                hit->SetLowerClusterIndex(fBmnVSPLowerClustersArray->GetEntriesFast());
            }
            it.second.SetUniqueID(fBmnVSPLowerClustersArray->GetEntriesFast());
            new ((*fBmnVSPLowerClustersArray)[fBmnVSPLowerClustersArray->GetEntriesFast()]) StripCluster(it.second);
        }
     */

    if (fVerbose > 1)
        cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnVSPHitMaker::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch(fOutputHitsBranchName, &fBmnVSPHitsArray);
    resultTree->Branch("BmnVSPUpperCluster", &fBmnVSPUpperClustersArray);
    resultTree->Branch("BmnVSPLowerCluster", &fBmnVSPLowerClustersArray);
    resultTree->Fill();
}

void BmnVSPHitMaker::Finish()
{
    printf("Work time of BmnVSPHitMaker: %4.2f sec.\n", workTime);
}
