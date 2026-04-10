#include "BmnSiProfHitMaker.h"

#include "BmnEventHeader.h"
#include "FairRunAna.h"
#include "TSystem.h"

#include <TChain.h>
#include <TRandom.h>
#include <TStopwatch.h>
#include <zlib.h>

static Double_t workTime = 0.0;

BmnSiProfHitMaker::BmnSiProfHitMaker()
    : fHitMatching(kTRUE)
{

    fInputPointsBranchName = "SiProfPoint";
    fInputDigitsBranchName = "BmnSiProfDigit";
    fInputDigitMatchesBranchName = "BmnSiProfDigitMatch";

    fOutputHitsBranchName = "BmnSiProfHit";
    fOutputHitMatchesBranchName = "BmnSiProfHitMatch";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnSiProfConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
}

BmnSiProfHitMaker::BmnSiProfHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, TString alignFile)
    : fHitMatching(kTRUE)
{

    fInputPointsBranchName = "SiProfPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnSiProfDigit" : "SiProf";
    fIsExp = isExp;

    fInputDigitMatchesBranchName = "BmnSiProfDigitMatch";

    fOutputHitsBranchName = "BmnSiProfHit";
    fOutputHitMatchesBranchName = "BmnSiProfHitMatch";

    fBmnEvQualityBranchName = "BmnEventQuality";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnSiProfConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    switch (run_period) {
        case 8:   // BM@N RUN-8
            fCurrentConfig = BmnSiProfConfiguration::Run8;
            break;
        case 9:   // BM@N RUN-9
            fCurrentConfig = BmnSiProfConfiguration::Run9;
            break;
    }
}

BmnSiProfHitMaker::~BmnSiProfHitMaker()
{
    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }
}

void BmnSiProfHitMaker::LoadDetectorConfiguration()
{
    TString gPathSiProfConfig = gSystem->Getenv("VMCWORKDIR");
    gPathSiProfConfig += "/parameters/profilometer/XMLConfigs/";

    switch (fCurrentConfig) {
        case BmnSiProfConfiguration::Run8:
            StationSet = new BmnSiProfStationSet(gPathSiProfConfig + "ProfRun8.xml");
            TransfSet = new BmnSiProfTransform();
            TransfSet->LoadFromXMLFile(gPathSiProfConfig + "ProfRun8.xml");
            if (fVerbose)
                cout << "   Current SiProf Configuration : Run8"
                     << "\n";
            break;

        case BmnSiProfConfiguration::Run9:
            StationSet = new BmnSiProfStationSet(gPathSiProfConfig + "ProfRun9.xml");
            TransfSet = new BmnSiProfTransform();
            TransfSet->LoadFromXMLFile(gPathSiProfConfig + "ProfRun9.xml");
            if (fVerbose)
                cout << "   Current SiProf Configuration : Run9"
                     << "\n";
            break;

        default:
            StationSet = nullptr;
    }
}

InitStatus BmnSiProfHitMaker::Init()
{

    if (fVerbose > 1)
        cout << "=================== BmnSiProfHitMaker::Init() started ====================" << endl;

    // if the configuration is not set -> return a fatal error
    if (!fCurrentConfig)
        Fatal("BmnSiProfHitMaker::Init()", " !!! Current SiProf config is not set !!! ");

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnSiProfDigitsArray = (TClonesArray*)ioman->GetObject(fInputDigitsBranchName);
    if (!fBmnSiProfDigitsArray) {
        cout << "BmnSiProfHitMaker::Init(): branch " << fInputDigitsBranchName << " not found! Task will be deactivated"
             << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnSiProfDigitMatchesArray = (TClonesArray*)ioman->GetObject(fInputDigitMatchesBranchName);

    if (fVerbose > 1) {
        if (fBmnSiProfDigitMatchesArray)
            cout << "  Strip matching information exists!\n";
        else
            cout << "  Strip matching information doesn`t exist!\n";
    }

    fBmnSiProfHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "SiProf", fBmnSiProfHitsArray, kTRUE);

    if (fHitMatching && fBmnSiProfDigitMatchesArray) {
        fBmnSiProfHitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputHitMatchesBranchName, "SiProf", fBmnSiProfHitMatchesArray, kTRUE);
    } else {
        fBmnSiProfHitMatchesArray = 0;
    }

    // Create SiProf detector ------------------------------------------------------
    LoadDetectorConfiguration();

    fField = FairRunAna::Instance()->GetField();
    if (!fField)
        Fatal("Init", "No Magnetic Field found");

    //--------------------------------------------------------------------------

    fBmnEvQuality = (TClonesArray*)ioman->GetObject(fBmnEvQualityBranchName);

    if (fVerbose > 1)
        cout << "=================== BmnSiProfHitMaker::Init() finished ===================" << endl;

    return kSUCCESS;
}

void BmnSiProfHitMaker::Exec(Option_t* opt)
{

    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;

    // Event separation by triggers ...
    if (fIsExp && fBmnEvQuality) {
        BmnEventQuality* evQual = (BmnEventQuality*)fBmnEvQuality->UncheckedAt(0);
        if (!evQual->GetIsGoodEvent())
            return;
    }
    fBmnSiProfHitsArray->Delete();

    if (fHitMatching && fBmnSiProfHitMatchesArray) {
        fBmnSiProfHitMatchesArray->Delete();
    }

    if (fVerbose > 1)
        cout << "=================== BmnSiProfHitMaker::Exec() started ====================" << endl;

    fField = FairRunAna::Instance()->GetField();

    if (fVerbose > 1)
        cout << " BmnSiProfHitMaker::Exec(), Number of BmnSiProfDigits = " << fBmnSiProfDigitsArray->GetEntriesFast()
             << "\n";

    ProcessDigits();

    if (fVerbose > 1)
        cout << "=================== BmnSiProfHitMaker::Exec() finished ===================" << endl;

    sw.Stop();
    workTime += sw.RealTime();
}

void BmnSiProfHitMaker::ProcessDigits()
{

    // FairMCPoint* MCPoint;
    BmnSiProfDigit* digit;
    BmnMatch* strip_match;

    BmnSiProfStation* station;
    BmnSiProfModule* module;

    // Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (Int_t idigit = 0; idigit < fBmnSiProfDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnSiProfDigit*)fBmnSiProfDigitsArray->At(idigit);
        if (!digit->IsGoodDigit())
            continue;
        station = StationSet->GetStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());

        if (module->SetStripSignalInLayer(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal()))
            AddedDigits++;

        if (fBmnSiProfDigitMatchesArray) {
            strip_match = (BmnMatch*)fBmnSiProfDigitMatchesArray->At(idigit);
            if (module->SetStripMatchInLayer(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match))
                AddedStripDigitMatches++;
        }
    }

    if (fVerbose > 1)
        cout << "   Processed strip digits  : " << AddedDigits << "\n";
    if (fVerbose > 1 && fBmnSiProfDigitMatchesArray)
        cout << "   Added strip digit matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    // Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose > 1)
        cout << "   Calculated points  : " << NCalculatedPoints << "\n";
    if (fVerbose == 1)
        cout << "BmnSiProfHitMaker: " << NCalculatedPoints << " hits\n";

    Int_t clear_matched_points_cnt = 0;   // points with the only one match-index

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        station = StationSet->GetStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            module = station->GetModule(iModule);

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {

                Double_t sigL = module->GetIntersectionPoint_LowerLayerSripTotalSignal(iPoint);
                Double_t sigU = module->GetIntersectionPoint_UpperLayerSripTotalSignal(iPoint);

                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);
                Double_t z = module->GetZPositionRegistered();

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                // Transform hit coordinates from local coordinate system of GEM-planes to global
                if (TransfSet) {
                    Plane3D::Point glob_point = TransfSet->ApplyTransforms(Plane3D::Point(-x, y, z), iStation, iModule);
                    x = -glob_point.X();
                    y = glob_point.Y();
                    z = glob_point.Z();
                }

                Int_t RefMCIndex = 0;

                // hit matching (define RefMCIndex)) ----------------------------
                BmnMatch match = module->GetIntersectionPointMatch(iPoint);

                Int_t most_probably_index = -1;
                Double_t max_weight = 0;

                Int_t n_links = match.GetNofLinks();
                if (n_links == 1)
                    clear_matched_points_cnt++;
                for (Int_t ilink = 0; ilink < n_links; ilink++) {
                    Int_t index = match.GetLink(ilink).GetIndex();
                    Double_t weight = match.GetLink(ilink).GetWeight();
                    if (weight > max_weight) {
                        max_weight = weight;
                        most_probably_index = index;
                    }
                }

                RefMCIndex = most_probably_index;
                //--------------------------------------------------------------

                // Add hit ------------------------------------------------------
                x *= -1;   // invert to global X //Temporary switched off

                new ((*fBmnSiProfHitsArray)[fBmnSiProfHitsArray->GetEntriesFast()])
                    BmnSiProfHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnSiProfHit* hit = (BmnSiProfHit*)fBmnSiProfHitsArray->At(fBmnSiProfHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnSiProfHitsArray->GetEntriesFast() - 1);
                hit->SetClusterSizeInLowerLayer(
                    module->GetIntersectionPoint_LowerLayerClusterSize(iPoint));   // cluster size (lower layer |||)
                hit->SetClusterSizeInUpperLayer(module->GetIntersectionPoint_UpperLayerClusterSize(
                    iPoint));   // cluster size (upper layer ///or\\\)
                hit->SetStripPositionInLowerLayer(
                    module->GetIntersectionPoint_LowerLayerSripPosition(iPoint));   // strip position (lower layer |||)
                hit->SetStripPositionInUpperLayer(module->GetIntersectionPoint_UpperLayerSripPosition(
                    iPoint));   // strip position (upper layer ///or\\\)
                hit->SetStripTotalSignalInLowerLayer(sigL);
                hit->SetStripTotalSignalInUpperLayer(sigU);

                if (fVerbose) {
                    cout << "  glob(x:y:z) = ( " << x << " : " << y << " : " << z << "\n";
                    cout << "  hit(x:y:z) = ( " << hit->GetX() << " : " << hit->GetY() << " : " << hit->GetZ() << "\n";
                    cout << "\n";
                }
                //--------------------------------------------------------------

                // hit matching -------------------------------------------------
                if (fHitMatching && fBmnSiProfHitMatchesArray) {
                    new ((*fBmnSiProfHitMatchesArray)[fBmnSiProfHitMatchesArray->GetEntriesFast()])
                        BmnMatch(module->GetIntersectionPointMatch(iPoint));
                }
                //--------------------------------------------------------------
            }
        }
    }
    if (fVerbose > 1)
        cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnSiProfHitMaker::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch(fOutputHitsBranchName, &fBmnSiProfHitsArray);
    resultTree->Fill();
}

void BmnSiProfHitMaker::Finish()
{
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if (TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    printf("Work time of BmnSiProfHitMaker: %4.2f sec.\n", workTime);
}
