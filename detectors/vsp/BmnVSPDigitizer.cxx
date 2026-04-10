#include "BmnVSPDigitizer.h"

#include "BmnVSPPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "TSystem.h"

#include <TRandom.h>   // AZ-010725

static Float_t workTime = 0.0;
static int entries = 0;

BmnVSPDigitizer::BmnVSPDigitizer()
    : fOnlyPrimary(kFALSE)
    , fStripMatching(kTRUE)
    , fUseRealEffects(kFALSE)
    , fSigma(-1)
    , fThresh(12)
{   // AZ-010725

    fInputBranchName = "VSPPoint";
    fOutputDigitsBranchName = "BmnVSPDigit";
    fOutputDigitMatchesBranchName = "BmnVSPDigitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnVSPConfiguration::None;
    StationSet = nullptr;
    fField = nullptr;
    TransfSet = nullptr;
}

BmnVSPDigitizer::~BmnVSPDigitizer()
{
    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }
}

InitStatus BmnVSPDigitizer::Init()
{

    if (fVerbose)
        cout << "\nBmnVSPDigitizer::Init()\n ";

    if (fVerbose && fOnlyPrimary)
        cout << "  Only primary particles are processed!!! " << endl;

    if (fVerbose && fStripMatching)
        cout << "  Strip Matching is activated!!! " << endl;
    else
        cout << "  Strip matching is deactivated!!! " << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnVSPPointsArray = (TClonesArray*)ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*)ioman->GetObject("MCTrack");

    fBmnVSPDigitsArray = new TClonesArray(fOutputDigitsBranchName);
    ioman->Register(fOutputDigitsBranchName, "VSP", fBmnVSPDigitsArray, kTRUE);

    if (fStripMatching) {
        fBmnVSPDigitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputDigitMatchesBranchName, "VSP", fBmnVSPDigitMatchesArray, kTRUE);
    }

    TString gPathVSPConfig = gSystem->Getenv("VMCWORKDIR");
    gPathVSPConfig += "/parameters/vsp/XMLConfigs/";

    // Create VSP detector ------------------------------------------------------
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
    //--------------------------------------------------------------------------

    if (fVerbose)
        cout << "BmnVSPDigitizer::Init() finished\n\n";
    return kSUCCESS;
}

void BmnVSPDigitizer::Exec(Option_t* opt)
{
    clock_t tStart = clock();
    fBmnVSPDigitsArray->Delete();

    if (fStripMatching) {
        fBmnVSPDigitMatchesArray->Delete();
    }

    if (!fBmnVSPPointsArray) {
        Error("BmnVSPDigitizer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if (fVerbose) {
        cout << " BmnVSPDigitizer::Exec(), Number of BmnVSPPoints = " << fBmnVSPPointsArray->GetEntriesFast() << "\n";
    }

    ProcessMCPoints();

    if (fVerbose)
        cout << " BmnVSPDigitizer::Exec() finished\n\n";
    entries++;
    clock_t tFinish = clock();
    workTime += ((Float_t)(tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnVSPDigitizer::ProcessMCPoints()
{

    FairMCPoint* VSPPoint;
    Int_t NNotPrimaries = 0;

    for (Int_t ipoint = 0; ipoint < fBmnVSPPointsArray->GetEntriesFast(); ipoint++) {
        VSPPoint = (FairMCPoint*)fBmnVSPPointsArray->At(ipoint);

        if (fOnlyPrimary) {
            CbmMCTrack* track = (CbmMCTrack*)fMCTracksArray->UncheckedAt(VSPPoint->GetTrackID());
            if (!track)
                continue;
            if (track->GetMotherId() != -1) {
                NNotPrimaries++;
                continue;
            }
        }

        Double_t x = -VSPPoint->GetX();   // invert because in current geometry +x is left, -x is right
        Double_t y = VSPPoint->GetY();
        Double_t z = VSPPoint->GetZ();

        Double_t px = -VSPPoint->GetPx();   // invert because in current geometry +x is left, -x is right
        Double_t py = VSPPoint->GetPy();
        Double_t pz = VSPPoint->GetPz();

        Double_t dEloss = VSPPoint->GetEnergyLoss() * 1e6;   // in keV
        Int_t refId = ipoint;

        // Information from MC-points
        Int_t mc_station_num = ((BmnVSPPoint*)VSPPoint)->GetStation();
        Int_t mc_module_num = ((BmnVSPPoint*)VSPPoint)->GetModule();

        // test output
        // cout << "mc_station_num = " << mc_station_num << "\n";
        // cout << "mc_module_num = " << mc_module_num << "\n";

        // Transform MC-point coordinates to local coordinate system
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

        // AZ-010725 - gain fluctuations
        Double_t gain = 1;
        // gain += gRandom->Gaus(0, 0.5);
        if (gain < 0)
            gain = 0;
        dEloss *= gain;
        //
        StationSet->AddPointToDetector(x, y, z, px, py, pz, dEloss, refId);
    }

    Int_t NAddedPoints = StationSet->CountNAddedToDetectorPoints();
    if (fVerbose && fOnlyPrimary)
        cout << "   Number of not primaries points : " << NNotPrimaries << "\n";
    if (fVerbose)
        cout << "   Processed MC points  : " << NAddedPoints << "\n";

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnVSPStation* station = StationSet->GetVSPStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnVSPModule* module = station->GetModule(iModule);
            vector<Int_t> processed_zones;

            for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                BmnVSPLayer layer = module->GetStripLayer(iLayer);

                Int_t zone_id = layer.GetZoneID();

                Bool_t is_processed_zone = false;
                for (size_t iz = 0; iz < processed_zones.size(); ++iz) {
                    if (zone_id == processed_zones[iz])
                        is_processed_zone = true;
                }

                if (!is_processed_zone) {
                    Int_t first_strip_in_zone = module->GetFirstStripInZone(zone_id);
                    Int_t last_strip_in_zone = module->GetLastStripInZone(zone_id);
                    map<int, int> noiseMap;   // AZ-010725 - add noise to strips (including left neighbour

                    for (Int_t iStrip = first_strip_in_zone; iStrip < last_strip_in_zone + 1; ++iStrip) {
                        Double_t signal = module->GetStripSignalInZone(zone_id, iStrip);

                        if (signal > 0.0) {
                            signal = signal / (120. / 500.0);   // el to ADC
                            signal *= 1.17;                     // AZ-110923 - for beam data
                            // AZ-130923 - gain fluctuations
                            //*
                            Double_t gain = 1;
                            // AZ-191223 gain += gRandom->Gaus(0, 0.5);
                            if (gain < 0)
                                gain = 0;
                            signal *= gain;
                            // AZ-240823 if (signal <= 12) continue;
                            // AZ-240823--->
                            if (fSigma > 0) {
                                // Add noise
                                signal += gRandom->Gaus(0, fSigma);
                            }
                            // AZ-130923 - gain fluctuations
                            /*
                              Double_t gain = 1;
                              //AZ-161223 gain += gRandom->Gaus(0, 0.2); //20%
                              gain += gRandom->Gaus(0, 0.5); //50%
                              if (gain < 0) gain = 0;
                              signal *= gain;
                            */
                            noiseMap[iStrip] = 1;   // noise flag
                            if (signal <= fThresh)
                                continue;
                            if (signal >= 2048)
                                signal = 2047;
                            // <--- AZ

                            new ((*fBmnVSPDigitsArray)[fBmnVSPDigitsArray->GetEntriesFast()]) BmnVSPDigit(
                                iStation, iModule, zone_id, iStrip, signal);   // zone_id == layer_num !!!!!!

                            if (fStripMatching) {
                                new ((*fBmnVSPDigitMatchesArray)[fBmnVSPDigitMatchesArray->GetEntriesFast()])
                                    BmnMatch(module->GetStripMatchInZone(zone_id, iStrip));
                            }
                        } else if (fSigma > 0) {   // AZ-010725
                            signal += gRandom->Gaus(0, fSigma);
                            // AZ-130923 - gain fluctuations
                            /*
                              Double_t gain = 1;
                              //AZ-161223 gain += gRandom->Gaus(0, 0.2);
                              gain += gRandom->Gaus(0, 0.5); //50%
                              if (gain < 0) gain = 0;
                              signal *= gain;
                            */
                            if (signal <= fThresh)
                                continue;
                            if (signal >= 2048)
                                signal = 2047;
                            new ((*fBmnVSPDigitsArray)[fBmnVSPDigitsArray->GetEntriesFast()]) BmnVSPDigit(
                                iStation, iModule, zone_id, iStrip, signal);   // zone_id == layer_num !!!!!!

                            if (fStripMatching) {
                                new ((*fBmnVSPDigitMatchesArray)[fBmnVSPDigitMatchesArray->GetEntriesFast()])
                                    BmnMatch(module->GetStripMatchInZone(zone_id, iStrip));
                            }
                        }   // else if (fSigma > 0)
                    }
                    processed_zones.push_back(zone_id);
                }
            }
        }
    }

    StationSet->Reset();
}

void BmnVSPDigitizer::Finish()
{
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if (TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    cout << "Work time of the VSP digitizer: " << workTime << endl;
}
