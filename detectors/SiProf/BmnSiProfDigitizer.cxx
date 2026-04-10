#include "BmnSiProfDigitizer.h"

#include "BmnSiProfPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "TSystem.h"

static Float_t workTime = 0.0;
static int entrys = 0;

BmnSiProfDigitizer::BmnSiProfDigitizer()
    : fOnlyPrimary(kFALSE)
    , fStripMatching(kTRUE)
{

    fInputBranchName = "SiProfPoint";
    fOutputDigitsBranchName = "BmnSiProfDigit";
    fOutputDigitMatchesBranchName = "BmnSiProfDigitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnSiProfConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
}

BmnSiProfDigitizer::~BmnSiProfDigitizer()
{
    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }
}

InitStatus BmnSiProfDigitizer::Init()
{

    if (fVerbose)
        cout << "\nBmnSiProfDigitizer::Init()\n ";

    // if the configuration is not set -> return a fatal error
    if (!fCurrentConfig)
        Fatal("BmnSiProfDigitizer::Init()", " !!! Current SiProf config is not set !!! ");

    if (fVerbose && fOnlyPrimary)
        cout << "  Only primary particles are processed!!! " << endl;

    if (fVerbose && fStripMatching)
        cout << "  Strip Matching is activated!!! " << endl;
    else
        cout << "  Strip Matching is deactivated!!! " << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnSiProfPointsArray = (TClonesArray*)ioman->GetObject(fInputBranchName);
    if (!fBmnSiProfPointsArray) {
        cout << "BmnSiProfDigitizer::Init(): branch " << fInputBranchName << " not found! Task will be deactivated"
             << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fMCTracksArray = (TClonesArray*)ioman->GetObject("MCTrack");

    fBmnSiProfDigitsArray = new TClonesArray(fOutputDigitsBranchName);
    ioman->Register(fOutputDigitsBranchName, "SiProf_DIGIT", fBmnSiProfDigitsArray, kTRUE);

    if (fStripMatching) {
        fBmnSiProfDigitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputDigitMatchesBranchName, "SiProf_DIGIT", fBmnSiProfDigitMatchesArray, kTRUE);
    }

    TString gPathSiProfConfig = gSystem->Getenv("VMCWORKDIR");
    gPathSiProfConfig += "/parameters/profilometer/XMLConfigs/";

    // Create SiProf detector ------------------------------------------------------
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
    //--------------------------------------------------------------------------

    if (fVerbose)
        cout << "BmnSiProfDigitizer::Init() finished\n\n";
    return kSUCCESS;
}

void BmnSiProfDigitizer::Exec(Option_t* opt)
{

    if (!IsActive())
        return;

    clock_t tStart = clock();
    fBmnSiProfDigitsArray->Delete();

    if (fStripMatching) {
        fBmnSiProfDigitMatchesArray->Delete();
    }

    if (!fBmnSiProfPointsArray) {
        Error("BmnSiProfDigitizer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if (fVerbose) {
        cout << " BmnSiProfDigitizer::Exec(), Number of BmnSiProfPoints = " << fBmnSiProfPointsArray->GetEntriesFast()
             << "\n";
    }

    ProcessMCPoints();

    if (fVerbose)
        cout << " BmnSiProfDigitizer::Exec() finished\n\n";
    entrys++;
    clock_t tFinish = clock();
    workTime += ((Float_t)(tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnSiProfDigitizer::ProcessMCPoints()
{

    FairMCPoint* SiProfPoint;
    Int_t NNotPrimaries = 0;

    for (Int_t ipoint = 0; ipoint < fBmnSiProfPointsArray->GetEntriesFast(); ipoint++) {
        SiProfPoint = (FairMCPoint*)fBmnSiProfPointsArray->At(ipoint);

        if (fOnlyPrimary) {
            CbmMCTrack* track = (CbmMCTrack*)fMCTracksArray->UncheckedAt(SiProfPoint->GetTrackID());
            if (!track)
                continue;
            if (track->GetMotherId() != -1) {
                NNotPrimaries++;
                continue;
            }
        }

        Double_t x = -SiProfPoint->GetX();   // invert because in the current geometry +x is left, -x is right
        Double_t y = SiProfPoint->GetY();
        Double_t z = SiProfPoint->GetZ();

        Double_t px = -SiProfPoint->GetPx();   // invert because in the current geometry +x is left, -x is right
        Double_t py = SiProfPoint->GetPy();
        Double_t pz = SiProfPoint->GetPz();

        Double_t dEloss = SiProfPoint->GetEnergyLoss() * 1e6;   // in keV
        Int_t refId = ipoint;

        // Information from MC-points
        Int_t mc_station_num = ((BmnSiProfPoint*)SiProfPoint)->GetStation();
        Int_t mc_module_num = ((BmnSiProfPoint*)SiProfPoint)->GetModule();

        // test output
        // cout << "mc_station_num = " << mc_station_num << "\n";
        // cout << "mc_module_num = " << mc_module_num << "\n";

        // Transform mc-point coordinates to local coordinate system of SiProf-planes
        if (TransfSet && mc_station_num < StationSet->GetNStations()) {
            if (mc_module_num < StationSet->GetStation(mc_station_num)->GetNModules()) {
                Plane3D::Point loc_point =
                    TransfSet->ApplyInverseTransforms(Plane3D::Point(-x, y, z), mc_station_num, mc_module_num);
                Plane3D::Point loc_direct = TransfSet->ApplyInverseTransforms(
                    Plane3D::Point(-(px + x), (py + y), (pz + z)), mc_station_num, mc_module_num);
                x = -loc_point.X();
                y = loc_point.Y();
                z = loc_point.Z();

                px = -(loc_direct.X() - loc_point.X());
                py = loc_direct.Y() - loc_point.Y();
                pz = loc_direct.Z() - loc_point.Z();
            }
        }

        StationSet->AddPointToDetector(x, y, z, px, py, pz, dEloss, refId);
    }

    Int_t NAddedPoints = StationSet->CountNAddedToDetectorPoints();
    if (fVerbose && fOnlyPrimary)
        cout << "   Number of not primaries points : " << NNotPrimaries << "\n";
    if (fVerbose)
        cout << "   Processed MC points  : " << NAddedPoints << "\n";

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnSiProfStation* station = StationSet->GetStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnSiProfModule* module = station->GetModule(iModule);

            for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                BmnSiProfLayer layer = module->GetStripLayer(iLayer);

                for (Int_t iStrip = 0; iStrip < layer.GetNStrips(); ++iStrip) {
                    Double_t signal = layer.GetStripSignal(iStrip);
                    if (signal > 0.0) {
                        new ((*fBmnSiProfDigitsArray)[fBmnSiProfDigitsArray->GetEntriesFast()])
                            BmnSiProfDigit(iStation, iModule, iLayer, iStrip, signal);

                        if (fStripMatching) {
                            new ((*fBmnSiProfDigitMatchesArray)[fBmnSiProfDigitMatchesArray->GetEntriesFast()])
                                BmnMatch(layer.GetStripMatch(iStrip));
                        }
                    }
                }
            }
        }
    }
    StationSet->Reset();
}

void BmnSiProfDigitizer::Finish()
{
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if (TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    cout << "Work time of the SiProf digitizer: " << workTime << endl;
}
