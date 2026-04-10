#include "BmnGemStripDigitizer.h"

#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripStationSet_RunSummer2016.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "FairRootManager.h"
#include "TSystem.h"

#include <TRandom.h>   //AZ-240823

static Float_t workTime = 0.0;
static int entrys = 0;

BmnGemStripDigitizer::BmnGemStripDigitizer()
    : fOnlyPrimary(kFALSE)
    , fStripMatching(kTRUE)
    , fUseRealEffects(kFALSE)
{

    fInputBranchName = "StsPoint";
    fOutputDigitsBranchName = "BmnGemStripDigit";
    fOutputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
    fField = nullptr;

    fBmnGemStripDigitsArray = nullptr;
    fBmnGemStripDigitMatchesArray = nullptr;

    fSigma = -1;               // AZ-240823
    fThresh = 12;              // AZ-240823 - initial value
    SetName("GemDigitizer");   // AZ-110124
}

BmnGemStripDigitizer::~BmnGemStripDigitizer()
{
    if (StationSet)
        delete StationSet;

    if (TransfSet)
        delete TransfSet;

    if (fBmnGemStripDigitsArray) {
        fBmnGemStripDigitsArray->Delete();
        delete fBmnGemStripDigitsArray;
    }
    if (fBmnGemStripDigitMatchesArray) {
        fBmnGemStripDigitMatchesArray->Delete();
        delete fBmnGemStripDigitMatchesArray;
    }
}

InitStatus BmnGemStripDigitizer::Init()
{

    if (fVerbose)
        cout << "\nBmnGemStripDigitizer::Init()\n ";

    // if GEM configuration is not set -> return a fatal error
    if (!fCurrentConfig) {
        Fatal("BmnGemStripDigitizer::Init()", " !!! Current GEM config is not set !!! ");
    }

    if (fVerbose && fOnlyPrimary)
        cout << "  Only primary particles are processed!!! " << endl;

    if (fVerbose && fStripMatching)
        cout << "  Strip Matching is activated!!! " << endl;
    else
        cout << "  Strip Matching is deactivated!!! " << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripPointsArray = (TClonesArray*)ioman->GetObject(fInputBranchName);
    if (!fBmnGemStripPointsArray) {
        cout << "BmnGemStripDigitizer::Init(): branch StsPoint not found! Task "
                "will be deactivated"
             << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fMCTracksArray = (TClonesArray*)ioman->GetObject("MCTrack");

    fBmnGemStripDigitsArray = new TClonesArray(fOutputDigitsBranchName);
    ioman->Register(fOutputDigitsBranchName, "GEM_DIGIT", fBmnGemStripDigitsArray, kTRUE);

    if (fStripMatching) {
        fBmnGemStripDigitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputDigitMatchesBranchName, "GEM_DIGIT", fBmnGemStripDigitMatchesArray, kTRUE);
    }

    TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");
    gPathGemConfig += "/parameters/gem/XMLConfigs/";

    // Create GEM detector ------------------------------------------------------
    switch (fCurrentConfig) {
        case BmnGemStripConfiguration::RunSummer2016:
            StationSet = new BmnGemStripStationSet_RunSummer2016(fCurrentConfig);
            if (fVerbose)
                cout << "   Current GEM Configuration : RunSummer2016"
                     << "\n";
            break;

        case BmnGemStripConfiguration::RunWinter2016:
            StationSet = new BmnGemStripStationSet_RunWinter2016(fCurrentConfig);
            if (fVerbose)
                cout << "   Current GEM Configuration : RunWinter2016"
                     << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2017:
            StationSet = new BmnGemStripStationSet_RunSpring2017(fCurrentConfig);
            // StationSet = new BmnGemStripStationSet(gPathGemConfig +
            // "GemRunSpring2017.xml");
            if (fVerbose)
                cout << "   Current GEM Configuration : RunSpring2017"
                     << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2018:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2018.xml");
            if (fVerbose)
                cout << "   Current GEM Configuration : RunSpring2018"
                     << "\n";
            break;

        case BmnGemStripConfiguration::RunSRCSpring2018:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSRCSpring2018.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemRunSRCSpring2018.xml");
            if (fVerbose)
                cout << "   Current GEM Configuration : GemRunSRCSpring2018"
                     << "\n";
            break;

        case BmnGemStripConfiguration::RunSRC2021:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSRC2021.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemRunSRC2021.xml");
            if (fVerbose)
                cout << "   Current GEM Configuration : RunSRC2021"
                     << "\n";
            break;

        case BmnGemStripConfiguration::Run8:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRun8.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemRun8.xml");
            if (fVerbose)
                cout << "   Current GEM Configuration : Run8"
                     << "\n";
            break;

        case BmnGemStripConfiguration::Run9:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRun9.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemRun9.xml");
            if (fVerbose)
                cout << "   Current GEM Configuration : Run9"
                     << "\n";
            break;

        default:
            StationSet = nullptr;
    }
    if (fUseRealEffects)
        ;

    //--------------------------------------------------------------------------

    if (fVerbose)
        cout << "BmnGemStripDigitizer::Init() finished\n\n";
    return kSUCCESS;
}

void BmnGemStripDigitizer::Exec(Option_t* opt)
{

    if (!IsActive())
        return;

    clock_t tStart = clock();
    fBmnGemStripDigitsArray->Delete();

    if (fStripMatching)
        fBmnGemStripDigitMatchesArray->Delete();

    if (!fBmnGemStripPointsArray) {
        Error("BmnGemStripDigitizer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if (fVerbose) {
        // cout << " BmnGemStripDigitizer::Exec(), Number of BmnGemStripPoints = "
        // << fBmnGemStripPointsArray->GetEntriesFast() << "\n";
    }

    ProcessMCPoints();

    // if (fVerbose) cout << " BmnGemStripDigitizer::Exec() finished\n\n";
    entrys++;
    clock_t tFinish = clock();
    workTime += ((Float_t)(tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnGemStripDigitizer::ProcessMCPoints()
{

    FairMCPoint* GemStripPoint;
    Int_t NNotPrimaries = 0;

    for (UInt_t ipoint = 0; ipoint < fBmnGemStripPointsArray->GetEntriesFast(); ipoint++) {
        GemStripPoint = (FairMCPoint*)fBmnGemStripPointsArray->At(ipoint);

        if (fOnlyPrimary) {
            CbmMCTrack* track = (CbmMCTrack*)fMCTracksArray->UncheckedAt(GemStripPoint->GetTrackID());
            if (!track)
                continue;
            if (track->GetMotherId() != -1) {
                NNotPrimaries++;
                continue;
            }
        }

        Double_t x = -GemStripPoint->GetX();   // invert because in current geometry
                                               // +x -left, -x - right
        Double_t y = GemStripPoint->GetY();
        Double_t z = GemStripPoint->GetZ();

        Double_t px = -GemStripPoint->GetPx();   // invert because in current geometry
                                                 // +x -left, -x - right
        Double_t py = GemStripPoint->GetPy();
        Double_t pz = GemStripPoint->GetPz();

        Double_t dEloss = GemStripPoint->GetEnergyLoss() * 1e6;   // in keV
        Int_t refId = ipoint;

        Int_t mc_station_num = ((CbmStsPoint*)GemStripPoint)->GetStation();
        Int_t mc_module_num = ((CbmStsPoint*)GemStripPoint)->GetModule();

        // cout << "mc_station_num = " << mc_station_num << "\n";
        // cout << "mc_module_num = " << mc_module_num << "\n";

        // Transform mc-point coordinates to local coordinate system of GEM-planes
        if (TransfSet && mc_station_num < StationSet->GetNStations()) {
            BmnGemStripStation* station = StationSet->GetGemStation(mc_station_num);
            if (mc_module_num < station->GetNModules()) {

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
        if (fUseRealEffects)
            ;

        // AZ-191223 - gain fluctuations
        Double_t gain = 1;
        // gain += gRandom->Gaus(0, 0.5);
        if (gain < 0)
            gain = 0;
        dEloss *= gain;
        //
        StationSet->AddPointToDetector(x, y, z, px, py, pz, dEloss, refId);
    }

    Int_t NAddedPoints = StationSet->CountNAddedToDetectorPoints();
    // if (fVerbose && fOnlyPrimary) cout << "   Number of not primaries points :
    // " << NNotPrimaries << "\n"; if (fVerbose) cout << "   Processed MC points
    // : " << NAddedPoints << "\n";

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnGemStripStation* station = StationSet->GetGemStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnGemStripModule* module = station->GetModule(iModule);

            for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                BmnGemStripLayer layer = module->GetStripLayer(iLayer);

                for (Int_t iStrip = 0; iStrip < layer.GetNStrips(); ++iStrip) {
                    Double_t signal = layer.GetStripSignal(iStrip);
                    if (signal > 0.0) {
                        signal = signal / (120. / 2.0);   // el to ADC
                        // AZ-110124 signal /= 2.5; //AZ-110923 - for beam data
                        signal /= 1.3;   // AZ-110124
                        // AZ-130923 - gain fluctuations
                        Double_t gain = 1;
                        // AZ-191223 gain += gRandom->Gaus(0, 0.5);
                        if (gain < 0)
                            gain = 0;
                        signal *= gain;
                        // AZ-240823 if (signal <= 12) continue;
                        // AZ-240823
                        if (fSigma > 0) {
                            // Add noise
                            signal += gRandom->Gaus(0, fSigma);
                        }
                        if (signal <= fThresh)
                            continue;
                        // AZ
                        if (signal >= 2048)
                            signal = 2047;
                        new ((*fBmnGemStripDigitsArray)[fBmnGemStripDigitsArray->GetEntriesFast()])
                            BmnGemStripDigit(iStation, iModule, iLayer, iStrip, signal);

                        if (fStripMatching) {
                            new ((*fBmnGemStripDigitMatchesArray)[fBmnGemStripDigitMatchesArray->GetEntriesFast()])
                                BmnMatch(layer.GetStripMatch(iStrip));
                        }
                    } else if (fSigma > 0) {
                        // AZ-120923
                        signal += gRandom->Gaus(0, fSigma);
                        // AZ-161223 - gain fluctuations
                        /*
                        Double_t gain = 1;
                        gain += gRandom->Gaus(0, 0.5); //50%
                        if (gain < 0) gain = 0;
                        signal *= gain;
                        */
                        if (signal <= fThresh)
                            continue;
                        if (signal >= 2048)
                            signal = 2047;
                        new ((*fBmnGemStripDigitsArray)[fBmnGemStripDigitsArray->GetEntriesFast()])
                            BmnGemStripDigit(iStation, iModule, iLayer, iStrip, signal);
                        if (fStripMatching) {
                            new ((*fBmnGemStripDigitMatchesArray)[fBmnGemStripDigitMatchesArray->GetEntriesFast()])
                                BmnMatch(layer.GetStripMatch(iStrip));
                        }
                    }
                }
            }
        }
    }
    StationSet->Reset();
}

void BmnGemStripDigitizer::Finish()
{
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if (TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    cout << "Work time of the GEM digitizer: " << workTime << endl;
}
