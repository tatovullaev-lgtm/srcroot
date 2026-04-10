
#include "BmnToCbmHitConverter.h"

#include "BmnVSPHit.h"        // AZ-060625
#include "CbmMCTrack.h"       //AZ-291223
#include "DstEventHeader.h"   //AZ-010723
#include "FairField.h"        //AZ-030323
#include "FairRunAna.h"       //AZ-030323

#include <TDatabasePDG.h>   //AZ-291223
#include <TFile.h>
#include <TH2.h>
#include <TObjString.h>
#include <TRandom.h>   //AZ-191223
#include <TStopwatch.h>

static Double_t workTime = 0.0;

// ------   Default constructor   -----------------------------------------
BmnToCbmHitConverter::BmnToCbmHitConverter()
    : BmnTask("BMN to CBM Hits Converter", 1)
    , fBmnGemHitsBranchName("BmnGemStripHit")
    , fBmnSilHitsBranchName("BmnSiliconHit")
    , fBmnVspHitsBranchName("BmnVSPHit")
    , fCbmHitsBranchName("StsHit")
    , fBmnGemHitsArray(nullptr)
    , fBmnGemUpperClusters(nullptr)
    , fBmnGemLowerClusters(nullptr)
    , fBmnSilHitsArray(nullptr)
    , fBmnSilUpperClusters(nullptr)
    , fBmnSilLowerClusters(nullptr)
    , fBmnVspHitsArray(nullptr)
    , fBmnVspUpperClusters(nullptr)
    , fBmnVspLowerClusters(nullptr)
    , fCbmHitsArray(nullptr)
    , fGemConfigFile("GemRun8.xml")
    , fSilConfigFile("SiliconRun8.xml")
    , fVspConfigFile("")
    , GemStationSet(nullptr)
    , SilStationSet(nullptr)
    , fVspStationSet(nullptr)
    , fUseFixedErrors(kFALSE)
    , fExp(kFALSE)
    , fMCcorr(kTRUE)
    , fieldFlag(kFALSE)
    , fAlignmentEnabled(kTRUE)
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
// AZ-170323 BmnToCbmHitConverter::BmnToCbmHitConverter(Int_t iVerbose)
BmnToCbmHitConverter::BmnToCbmHitConverter(Int_t iVerbose, Bool_t isExp)
    : BmnTask("BMN to CBM Hits Converter", iVerbose)
    , fBmnGemHitsBranchName("BmnGemStripHit")
    , fBmnSilHitsBranchName("BmnSiliconHit")
    , fBmnVspHitsBranchName("BmnVSPHit")
    , fCbmHitsBranchName("StsHit")
    , fBmnGemHitsArray(nullptr)
    , fBmnGemUpperClusters(nullptr)
    , fBmnGemLowerClusters(nullptr)
    , fBmnSilHitsArray(nullptr)
    , fBmnSilUpperClusters(nullptr)
    , fBmnSilLowerClusters(nullptr)
    , fBmnVspHitsArray(nullptr)
    , fBmnVspUpperClusters(nullptr)
    , fBmnVspLowerClusters(nullptr)
    , fCbmHitsArray(nullptr)
    , fGemConfigFile("GemRun8.xml")
    , fSilConfigFile("SiliconRun8.xml")
    , fVspConfigFile("")
    , GemStationSet(nullptr)
    , SilStationSet(nullptr)
    , fVspStationSet(nullptr)
    , fUseFixedErrors(kFALSE)
    , fExp(isExp)
    , fMCcorr(kTRUE)
    , fieldFlag(kFALSE)
    , fAlignmentEnabled(kTRUE)
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnToCbmHitConverter::~BmnToCbmHitConverter() {}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void BmnToCbmHitConverter::Exec(Option_t* opt)
{

    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;

    fCbmHitsArray->Delete();

    int nVspStat = (fVspStationSet) ? fVspStationSet->GetNStations() : 0;   // AZ-070625

    if (fBmnGemHitsArray) {

        for (Int_t iHit = 0; iHit < fBmnGemHitsArray->GetEntriesFast(); ++iHit) {
            BmnGemStripHit* bmnHit = (BmnGemStripHit*)fBmnGemHitsArray->At(iHit);
            if (bmnHit->IsPseudo())
                continue;   // AZ-180723 - exclude 1-dim hits (one-sided pseudo hit)

            // if (bmnHit->GetRefIndex() == -1) continue;

            // Section for hit filtration by signal asymmetry
            //  StripCluster* uc = (StripCluster*)fBmnGemUpperClusters->At(bmnHit->GetUpperClusterIndex());
            //  StripCluster* lc = (StripCluster*)fBmnGemLowerClusters->At(bmnHit->GetLowerClusterIndex());
            //  Float_t ls = lc->TotalSignal;
            //  Float_t us = uc->TotalSignal;
            //  if (us < ls - 1000 || us > ls + 1000) continue;
            //  if (Abs((us - ls) / (us + ls)) > 0.75) //asymmetry
            //      continue;

            TVector3 pos;
            bmnHit->Position(pos);
            TVector3 dpos;
            if (fUseFixedErrors) {
                // dpos[0] = 0.08/TMath::Sqrt(12); //AZ
                // dpos[1] = 0.1234; //AZ
                dpos[0] = fDXgem;   // 0.015; //AZ - as in cbmroot
                dpos[1] = fDYgem;   // 0.058; //AZ - as in cbmroot
            } else {
                bmnHit->PositionError(dpos);
            }

            Int_t stat = bmnHit->GetStation();
            Int_t mod = bmnHit->GetModule();
            // ElectronDriftDirectionInModule driftDir =
            // GemStationSet->GetStation(stat)->GetModule(mod)->GetElectronDriftDirection(); //AZ-200322
            // if (driftDir == BackwardZAxisEDrift) pos[2] -= 0.9; //AZ-200322
            // if (driftDir == ForwardZAxisEDrift) pos[2] -= 0.9; //AZ-200322

            Int_t lay = 0;
            for (lay = 0; lay < GemStationSet->GetStation(stat)->GetModule(mod)->GetNStripLayers(); lay++) {
                BmnGemStripLayer* layer = &(GemStationSet->GetStation(stat)->GetModule(mod)->GetStripLayer(lay));
                if (layer->IsPointInsideStripLayer(-bmnHit->GetX(), bmnHit->GetY()))
                    break;
            }
            if (lay == GemStationSet->GetStation(stat)->GetModule(mod)->GetNStripLayers())
                continue;   // AZ-230322 - strange case //CHECK IT!!!

            // formula for converting from the bm@n system of modules and layers to the cbm one
            Int_t sect = 2 * mod + 1 + lay / 2;

            Int_t sens = 1;

            Int_t nSilStations = SilStationSet->GetNStations();
            Int_t detId = kGEM << 24 | (stat + 1 + nVspStat + nSilStations) << 16 | sect << 4 | sens << 1;

            // AZ-140226 - remove beam spot in GEMS for mag. field == 0
            if (!fieldFlag) {
                if ((stat == 1 && pos[0] >= -16 && pos[0] < -2) || (stat == 2 && pos[0] >= -18 && pos[0] < -2)
                    || (stat == 3 && pos[0] >= -12 && pos[0] < -2) || (stat == 4 && pos[0] >= -10 && pos[0] < 3)
                    || (stat == 5 && pos[0] >= -10 && pos[0] < 3) || (stat == 6 && pos[0] >= -6 && pos[0] < 4))
                {
                    if (TMath::Abs(pos[1]) < 15)
                        continue;
                }
            }

            new ((*fCbmHitsArray)[fCbmHitsArray->GetEntriesFast()]) CbmStsHit(detId, pos, dpos, 0.0, 0, 0);
            CbmStsHit* hit = (CbmStsHit*)fCbmHitsArray->At(fCbmHitsArray->GetEntriesFast() - 1);

            // AZ-120223 - apply alignment
            if (fExp && fAlignmentEnabled)
                ApplyAlignment(hit);
            else if (fieldFlag && !fExp) {
                // AZ-140824 - shift coordinates
                hit->SetY(hit->GetY() + 0.30);
                if (fMCcorr) {
                    int iq = 0;
                    Double_t pmom = 0;
                    if (bmnHit->GetRefIndex() >= 0) {
                        // Different smearing for negative tracks
                        FairMCPoint* mcp = (FairMCPoint*)fGemPoints->UncheckedAt(bmnHit->GetRefIndex());
                        CbmMCTrack* mctr = (CbmMCTrack*)fMCTracks->UncheckedAt(mcp->GetTrackID());
                        pmom = mctr->GetP();
                        if (TDatabasePDG::Instance()->GetParticle(mctr->GetPdgCode()))
                            iq = TMath::Nint(TDatabasePDG::Instance()->GetParticle(mctr->GetPdgCode())->Charge() / 3);
                    }
                    // Correct MC hit
                    CorrectHitMC(hit, pmom, iq);
                    if (iq == 0)
                        iq = 1;   // for backward compatibility

                    Bool_t ok = CorrectEffic(hit, iq);   // efficiency correction
                    if (!ok) {
                        // Remove hit
                        fCbmHitsArray->RemoveLast();
                        continue;
                    }
                }   // if (fMCcorr)
            }

            FairRootManager::Instance()->SetUseFairLinks(kTRUE);
            hit->ResetLinks();
            hit->SetLinks(bmnHit->GetLinks());
            FairRootManager::Instance()->SetUseFairLinks(kFALSE);
            hit->SetRefIndex(bmnHit->GetRefIndex());
            if (fExp)
                hit->SetRefIndex(iHit);   // AZ-180226 - original hit index (for original coordinates)
            hit->fDigiF = bmnHit->GetLowerClusterIndex() + 0;         // AZ-250322
            hit->fDigiB = bmnHit->GetUpperClusterIndex() + 1000000;   // AZ-250322
            // AZ-221022
            StripCluster* upp = (StripCluster*)fBmnGemUpperClusters->UncheckedAt(hit->fDigiB % 1000000);
            StripCluster* low = (StripCluster*)fBmnGemLowerClusters->UncheckedAt(hit->fDigiF % 1000000);
            Double_t uppq = upp->TotalSignal;
            Double_t lowq = low->TotalSignal;
            Double_t corrq = (uppq - lowq) / (uppq + lowq);
            hit->SetSignalDiv(corrq);
            uppq = upp->Width;   // N of strips
            lowq = low->Width;
            corrq = (uppq - lowq) / (uppq + lowq);
            hit->SetTimeStamp(corrq);                               // just using available space
            hit->SetStrips(low->MeanPosition, upp->MeanPosition);   // AZ-190823
            // hit->SetWidths (low->Width, upp->Width); //AZ-190823
        }   // for (Int_t iHit = 0; iHit < fBmnGemHitsArray->GetEntriesFast();
    }

    if (fBmnSilHitsArray) {
        for (Int_t iHit = 0; iHit < fBmnSilHitsArray->GetEntriesFast(); ++iHit) {
            BmnSiliconHit* bmnHit = (BmnSiliconHit*)fBmnSilHitsArray->At(iHit);
            if (bmnHit->IsPseudo())
                continue;   // AZ-180723 - exclude 1-dim hits (one-sided pseudo hit)
            // if (bmnHit->GetRefIndex() == -1) continue;
            // Section for hit filtration by signal asymmetry
            //  StripCluster* uc = (StripCluster*)fBmnSilUpperClusters->At(bmnHit->GetUpperClusterIndex());
            //  StripCluster* lc = (StripCluster*)fBmnSilLowerClusters->At(bmnHit->GetLowerClusterIndex());
            //  Float_t ls = lc->TotalSignal;
            //  Float_t us = uc->TotalSignal;
            //  if (us < ls - 1000 || us > ls + 1000) continue;
            //  if (Abs((us - ls) / (us + ls)) > 0.75) //asymmetry
            //      continue;

            TVector3 pos;
            bmnHit->Position(pos);
            pos[2] -= 0.0150;   // AZ - shift to the entrance
            TVector3 dpos;
            if (fUseFixedErrors) {
                dpos[0] = fDXsil;   // 0.01 / TMath::Sqrt(12); //AZ
                // dpos[1] = 0.1234; //AZ
                dpos[1] = fDYsil;   // 0.021; //AZ - as in cbmroot
            } else {
                bmnHit->PositionError(dpos);
            }

            Int_t sens = 1;
            Int_t detId = kSILICON << 24 | (bmnHit->GetStation() + nVspStat + 1) << 16 | (bmnHit->GetModule() + 1) << 4
                          | sens << 1;
            new ((*fCbmHitsArray)[fCbmHitsArray->GetEntriesFast()]) CbmStsHit(detId, pos, dpos, 0.0, 0, 0);
            CbmStsHit* hit = (CbmStsHit*)fCbmHitsArray->At(fCbmHitsArray->GetEntriesFast() - 1);
            if (fExp && fAlignmentEnabled)
                ApplyAlignment(hit);   // AZ-120223 - alignment
            else if (!fExp) {
                // AZ-140824 - shift coordinates
                hit->SetY(hit->GetY() + 0.30);
                if (fMCcorr) {
                    int iq = 0;
                    Double_t pmom = 0;
                    if (bmnHit->GetRefIndex() >= 0) {
                        FairMCPoint* mcp = (FairMCPoint*)fSilPoints->UncheckedAt(bmnHit->GetRefIndex());
                        CbmMCTrack* mctr = (CbmMCTrack*)fMCTracks->UncheckedAt(mcp->GetTrackID());
                        pmom = mctr->GetP();
                        if (TDatabasePDG::Instance()->GetParticle(mctr->GetPdgCode()))
                            iq = TMath::Nint(TDatabasePDG::Instance()->GetParticle(mctr->GetPdgCode())->Charge() / 3);
                    }
                    // Correct MC hit
                    CorrectHitMC(hit, pmom, iq);
                    if (iq == 0)
                        iq = 1;   // for backward compatibility

                    Bool_t ok = CorrectEffic(hit, iq);   // efficiency correction
                    if (!ok) {
                        // Remove hit
                        fCbmHitsArray->RemoveLast();
                        continue;
                    }
                }   // if (fMCcorr)
            }
            FairRootManager::Instance()->SetUseFairLinks(kTRUE);
            hit->ResetLinks();
            hit->SetLinks(bmnHit->GetLinks());
            FairRootManager::Instance()->SetUseFairLinks(kFALSE);
            hit->SetRefIndex(bmnHit->GetRefIndex());
            if (fExp)
                hit->SetRefIndex(iHit);   // AZ-180226 - original hit index (for original coordinates)
            hit->fDigiF =
                bmnHit->GetLowerClusterIndex() + 2000000;   //!!!AZ - for VectorFinder (to have unique indices) - 260322
            hit->fDigiB =
                bmnHit->GetUpperClusterIndex() + 3000000;   //!!!AZ - for VectorFinder (to have unique indices) - 260322
            // AZ-221022
            StripCluster* upp = (StripCluster*)fBmnSilUpperClusters->UncheckedAt(hit->fDigiB % 1000000);
            StripCluster* low = (StripCluster*)fBmnSilLowerClusters->UncheckedAt(hit->fDigiF % 1000000);
            Double_t uppq = upp->TotalSignal;
            Double_t lowq = low->TotalSignal;
            Double_t corrq = (uppq - lowq) / (uppq + lowq);
            hit->SetSignalDiv(corrq);
            uppq = upp->Width;   // N of strips
            lowq = low->Width;
            corrq = (uppq - lowq) / (uppq + lowq);
            hit->SetTimeStamp(corrq);                               // just using available space
            hit->SetStrips(low->MeanPosition, upp->MeanPosition);   // AZ-190823
            // hit->SetWidths (low->Width, upp->Width); //AZ-190823
        }   // for (Int_t iHit = 0; iHit < fBmnSilHitsArray->GetEntriesFast();
    }   // if (fBmnSilHitsArray)

    if (fBmnVspHitsArray)
        AddVspHits();   // AZ-060625

    sw.Stop();
    workTime += sw.RealTime();
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus BmnToCbmHitConverter::Init()
{
    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "No FairRootManager");
    fBmnGemHitsArray = (TClonesArray*)ioman->GetObject(fBmnGemHitsBranchName);
    fBmnSilHitsArray = (TClonesArray*)ioman->GetObject(fBmnSilHitsBranchName);
    fBmnVspHitsArray = (TClonesArray*)ioman->GetObject(fBmnVspHitsBranchName);

    fBmnGemLowerClusters = (TClonesArray*)ioman->GetObject("BmnGemLowerCluster");
    fBmnGemUpperClusters = (TClonesArray*)ioman->GetObject("BmnGemUpperCluster");
    fBmnSilLowerClusters = (TClonesArray*)ioman->GetObject("BmnSiliconLowerCluster");
    fBmnSilUpperClusters = (TClonesArray*)ioman->GetObject("BmnSiliconUpperCluster");
    fBmnVspLowerClusters = (TClonesArray*)ioman->GetObject("BmnVSPLowerCluster");
    fBmnVspUpperClusters = (TClonesArray*)ioman->GetObject("BmnVSPUpperCluster");
    if (!fBmnGemHitsArray && !fBmnSilHitsArray) {
        cout << "BmnToCbmHitConverter::Init(): branches with inner hits not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fGemPoints = (TClonesArray*)ioman->GetObject("StsPoint");       // AZ-291223
    fSilPoints = (TClonesArray*)ioman->GetObject("SiliconPoint");   // AZ-291223
    fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");         // AZ-291223

    // Register output array
    fCbmHitsArray = new TClonesArray("CbmStsHit");
    ioman->Register("StsHit", "STSHIT", fCbmHitsArray, kTRUE);

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");

    // Create GEM detector ------------------------------------------------------
    GemStationSet = new BmnGemStripStationSet(gPathConfig + "/parameters/gem/XMLConfigs/" + fGemConfigFile);
    SilStationSet = new BmnSiliconStationSet(gPathConfig + "/parameters/silicon/XMLConfigs/" + fSilConfigFile);
    if (fVspConfigFile != "")
        fVspStationSet = new BmnVSPStationSet(gPathConfig + "/parameters/vsp/XMLConfigs/" + fVspConfigFile);

    FairField* mField = FairRunAna::Instance()->GetField();
    if (mField == nullptr) {
        LOG(error) << "BmnToCbmHitConverter::Init(): mField has null pointer! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }
    fieldFlag = (TMath::Abs(mField->GetBy(0, 0, 135)) > 0.5) ? kTRUE : kFALSE;

    return kSUCCESS;
}
// -------------------------------------------------------------------------

void BmnToCbmHitConverter::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch("StsHit", &fCbmHitsArray);
    resultTree->Fill();
}

void BmnToCbmHitConverter::SetFixedErrors(Float_t dXgem, Float_t dYgem, Float_t dXsil, Float_t dYsil)
{
    fUseFixedErrors = kTRUE;
    fDXgem = dXgem;
    fDYgem = dYgem;
    fDXsil = dXsil;
    fDYsil = dYsil;
}

void BmnToCbmHitConverter::Finish()
{
    printf("Work time of BmnToCbmHitConverter: %4.2f sec.\n", workTime);
}

// -------------------------------------------------------------------------

void BmnToCbmHitConverter::ApplyAlignment(CbmStsHit* hit)
{
    // AZ-120223 - apply alignment

    // AZ-070223 - alignment parameters from Igor
    //  /home/ruf/NICA/bmnroot230107
    //  /data4/ruf/Run8raw/7759/00/runAll.sh
    //  GEM: xhit-=StsDX[station][sector], yhit-=StsDY[station][sector], zhit+=GemDZ10[station-4][sector/2]
    //  Si: xhit-= SiDX[stn][sct]; yhit-= SiDY[stn][sct]; zhit+=SiDZ[stn][sct];

    const Int_t MStn = 11 /*, NSct[MStn] = { 6,10,14,18, 8,8,8,8,8,8,8 }*/;
    /*Double_t StsZ[MStn][18] = {
       {19.3535, 20.5035, 19.2535, 16.2435, 15.0335, 16.4235},
       {29.406, 28.231, 29.401, 28.086, 29.256, 23.807, 24.977, 23.827, 25.112, 23.892},
       {36.9615, 38.1615, 36.8915, 38.1315, 36.8215, 37.9965, 36.8515, 33.534, 32.364, 33.519, 32.344, 33.709, 32.524, 33.719},
       {46.788, 45.583, 46.733, 45.588, 46.773, 45.438, 46.613, 45.433, 46.618, 41.217, 42.342, 41.212, 42.367, 41.222, 42.512,
       41.342, 42.467, 41.312}, {65.61, 65.61, 65.61, 65.61, 60.91, 60.91, 60.91, 60.91},
       {92.392, 92.392, 92.392, 92.392, 97.092, 97.092, 97.092, 97.092},
       {128.362, 128.362, 128.362, 128.362, 123.662, 123.662, 123.662, 123.662},
       {154.842, 154.842, 154.842, 154.842, 159.542, 159.542, 159.542, 159.542},
       {190.839, 190.839, 190.839, 190.839, 186.139, 186.139, 186.139, 186.139},
       {217.437, 217.437, 217.437, 217.437, 222.137, 222.137, 222.137, 222.137},
       {253.251, 253.251, 253.251, 253.251, 248.551, 248.551, 248.551, 248.551} };
    */
    double GemDZ10[7][4] = {{0.243654, 0.250775, 0.173749, 0.161366},     // for [station][Quater]
                            {-0.143876, -0.150758, 0.109959, 0.156433},   // same values from GEM-6 base
                            {-0.110145, -0.165981, -0.213889, -0.229543},
                            {0.103517, 0.036342, 0.086135, 0.0941369},
                            {-0.037648, -0.0941807, -0.180758, -0.176327},
                            {-0.0674491, -0.0367844, -0.0972473, -0.0584354},
                            {0, 0, 0, 0}};

    // CbmStsHit: hit-track. Shifts of intermediate GEM stations with respect of first and last GEMs
    Double_t StsDX[MStn][8] = {
        {0., 0., 0., 0., 0., 0., 0., 0.},
        {0., 0., 0., 0., 0., 0., 0., 0.},
        {0., 0., 0., 0., 0., 0., 0., 0.},
        {0., 0., 0., 0., 0., 0., 0., 0.},
        {2.28613e-01, 2.34606e-01, 3.70449e-01, 4.39267e-01, 2.67664e-01, 2.84859e-01, 3.41005e-01, 4.16753e-01},
        {3.17030e-01, 2.45618e-01, 4.29878e-01, 4.29169e-01, 2.60443e-01, 1.79925e-01, 3.76252e-01, 3.71674e-01},
        {1.25543e-01, 1.37935e-01, 2.77096e-01, 3.51704e-01, 2.16012e-01, 2.12139e-01, 2.88912e-01, 3.57699e-01},
        {3.17647e-01, 2.36295e-01, 3.75431e-01, 3.80575e-01, 1.68884e-01, 8.80524e-02, 2.51805e-01, 2.56513e-01},
        {-3.01883e-02, -2.87880e-02, 1.09744e-01, 1.90429e-01, 1.78162e-02, 1.32671e-02, 9.23760e-02, 1.67779e-01},
        {1.43966e-01, 5.53303e-02, 2.48555e-01, 2.49193e-01, 3.88544e-02, -3.31393e-02, 7.96085e-02, 7.41273e-02},
        {0., -5.63645e-04, 0.05, 8.74027e-04, 0., 3.60763e-03, 0.074, 1.49797e-01}};
    Double_t StsDY[MStn][8] = {
        {0., 0., 0., 0., 0., 0., 0., 0.},
        {0., 0., 0., 0., 0., 0., 0., 0.},
        {0., 0., 0., 0., 0., 0., 0., 0.},
        {0., 0., 0., 0., 0., 0., 0., 0.},
        {-2.40830e-01, -2.27117e-01, -4.41393e-01, -2.98662e-01, -1.47376e-01, -5.64919e-01, 3.60250e-02, -2.15290e-01},
        {-4.83443e-01, -3.63780e-01, -3.52015e-01, -3.36903e-01, -1.14008e-01, -3.49136e-01, -2.14579e-01,
         -6.23524e-01},
        {-1.91062e-01, -2.04515e-01, -3.79972e-01, -2.57092e-01, -1.01400e-01, -5.18137e-01, 5.39497e-02, -1.85679e-01},
        {-4.55871e-01, -3.48696e-01, -2.33191e-01, -2.40044e-01, -5.37329e-02, -3.00596e-01, -1.98740e-01,
         -6.04983e-01},
        {-2.31062e-01, -2.58276e-01, -4.03759e-01, -2.70196e-01, -8.53213e-02, -5.14891e-01, 3.15495e-02, -2.08464e-01},
        {-2.58821e-01, -1.58621e-01, -9.84743e-02, -1.05756e-01, 1.54258e-01, -1.04935e-01, 1.83280e-02, -3.86622e-01},
        {0., -5.86937e-03, -0.23, -1.45342e-01, 0., -3.98117e-01, 0.15, -1.04839e-01}};

    Double_t SiDX[4][18] = {{2.68835e-01, 2.39831e-01, 2.52506e-01, 2.05206e-01, 2.11052e-01, 0.20737190, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0},
                            {3.15120e-01, 2.74671e-01, 2.55372e-01, 2.83446e-01, 2.79317e-01, 3.48623e-01, 3.18654e-01,
                             3.00181e-01, 3.16157e-01, 3.80363e-01, 0, 0, 0, 0, 0, 0, 0, 0},
                            {1.65037e-01, 2.71806e-01, 2.66914e-01, 2.40166e-01, 2.59614e-01, 2.66288e-01, 2.83516e-01,
                             2.88311e-01, 2.61280e-01, 3.02609e-01, 2.96469e-01, 2.98769e-01, 2.66650e-01, 3.72627e-01,
                             0, 0, 0, 0},
                            {3.01013e-01, 3.62381e-01, 3.03249e-01, 3.06502e-01, 2.75747e-01, 2.95530e-01, 3.05152e-01,
                             3.42706e-01, 3.09787e-01, 2.24795e-01, 2.31845e-01, 2.59297e-01, 2.17342e-01, 2.20424e-01,
                             2.06861e-01, 2.49563e-01, 2.64846e-01, 2.02358e-01}};
    Double_t SiDY[4][18] = {
        {-0.272688, -0.300649, -0.343639, -0.313878, -0.331129, -0.299529, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {-0.342847, -0.343267, -0.328436, -0.354188, -0.332445, -0.349645, -0.332025, -0.294098, -0.249696, -0.217585,
         0, 0, 0, 0, 0, 0, 0, 0},
        {-0.398784, -0.342968, -0.325462, -0.325316, -0.338778, -0.338966, -0.324549, -0.339105, -0.314266, -0.301756,
         -0.288295, -0.272864, -0.272913, -0.21294, 0, 0, 0, 0},
        {-0.269827, -0.227419, -0.295412, -0.274608, -0.322856, -0.33417, -0.361013, -0.397192, -0.368905, -0.217688,
         -0.240085, -0.266281, -0.285738, -0.269306, -0.292405, -0.297714, -0.323987, -0.379705}};
    Double_t SiDZ[4][18] = {
        {-0.4, -0.3, -0.23, -0.27, -0.3, -0.23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {-0.5, -0.4, -0.4, -0.28, -0.25, -0.45, -0.45, -0.3, -0.23, -0.2, 0, 0, 0, 0, 0, 0, 0, 0},
        {-0.3, -0.5, -0.45, -0.4, -0.3, -0.2, -0.2, -0.25, -0.2, -0.3, -0.3, -0.4, -0.5, -0.27, 0, 0, 0, 0},
        {-0.5, -0.6, -0.4, -0.5, -0.3, -0.35, -0.2, -0.1, -0.4, -0.6, -0.6, -0.6, -0.5, -0.35, -0.2, -0.1, 0.1, 0.}};

    // AZ-090223 - Si module rotations around OZ (dx = p0 + p1*y)
    const Double_t p0si[4][18] = {{3.865e-2, 6.497e-2, 3.848e-2, -3.535e-2, -2.711e-2, -4.438e-2},   // st1
                                  {-3.011e-2, -6.724e-3, -2.784e-2, -1.761e-2, -1.431e-2, 2.650e-2, 1.818e-2, 1.052e-2,
                                   1.510e-2, 1.977e-2},   // st2
                                  {-1.639e-2, -1.628e-2, -4.100e-2, -2.524e-2, -2.082e-2, -3.096e-3, -1.535e-2,
                                   -1.594e-2, 1.974e-2, 1.212e-2, 1.859e-2, 3.067e-2, 2.477e-2, -3.289e-3},   // st3
                                  {5.774e-2, 5.280e-2, 4.679e-2, 6.866e-2, 6.183e-2, 4.256e-2, 4.247e-2, -5.477e-3,
                                   2.597e-2, 1.403e-2, -2.016e-2, -3.445e-2, -3.345e-2, -3.685e-2, -4.381e-2, -6.655e-2,
                                   -2.791e-2, -4.438e-2}};
    const Double_t p1si[4][18] = {
        {-8.887e-3, -1.128e-2, -6.696e-3, -1.042e-2, -7.374e-3, -1.303e-2},                                     // st1
        {4.338e-3, 8.247e-4, 3.374e-3, 1.151e-3, 1.616e-3, 7.210e-3, 3.103e-3, 1.564e-3, 2.896e-3, 5.346e-3},   // st2
        {3.678e-3, 1.777e-3, 4.869e-3, 2.708e-3, 2.610e-3, 4.911e-4, 2.753e-3, -2.473e-3, 3.427e-3, 1.598e-3, 2.388e-3,
         3.975e-3, 3.602e-3, 1.143e-3},   // st3
        {-4.152e-3, -6.035e-3, -4.377e-3, -6.320e-3, -5.058e-3, -3.510e-3, -3.763e-3, 4.285e-4, -5.000e-3, 1.801e-3,
         -3.673e-3, -3.629e-3, -3.629e-3, -3.660e-3, -4.075e-3, -7.053e-3, -4.439e-3, -6.091e-3}};
    // Second iteration
    const Double_t p10si[4][18] = {{-2.385e-3, 6.372e-4, 8.679e-4, -2.758e-3, -5.495e-3, -4.838e-3},   // st1
                                   {-3.930e-3, 2.765e-3, -3.730e-4, -5.525e-4, 1.433e-3, 1.042e-3, 2.216e-3, 3.619e-3,
                                    1.800e-3, 1.076e-3},   // st2
                                   {2.693e-3, 2.794e-3, -4.447e-3, -3.237e-4, 8.975e-4, 8.401e-4, -2.997e-3, -3.868e-3,
                                    2.667e-3, -6.283e-4, 3.312e-3, 3.420e-3, 5.233e-3, -4.670e-3},   // st3
                                   {7.950e-3, 2.818e-3, -1.178e-3, 3.522e-3, -1.457e-4, -2.013e-3, 3.139e-3, -1.060e-2,
                                    -5.446e-3, 4.349e-3, 9.024e-4, -1.297e-3, -2.652e-3, -7.524e-3, -4.564e-3,
                                    -9.118e-3, 8.307e-3, -5.750e-3}};
    const Double_t p11si[4][18] = {
        {3.525e-4, 4.803e-5, -4.898e-4, -5.709e-4, -1.111e-3, -1.295e-3},                                        // st1
        {5.953e-4, -4.350e-4, 6.053e-5, 4.750e-5, 1.259e-4, 6.429e-4, 2.556e-4, 3.749e-4, 1.500e-4, 7.110e-4},   // st2
        {1.719e-5, -3.435e-4, 6.088e-4, 1.112e-4, 2.683e-4, -6.623e-5, 4.340e-4, -6.263e-4, 4.717e-4, -1.268e-4,
         4.721e-4, 5.582e-4, 6.571e-4, -4.956e-4},   // st3
        {-4.333e-4, -3.287e-4, -7.432e-6, -4.067e-4, -6.684e-5, 3.667e-5, -4.441e-4, 8.977e-4, 7.495e-4, 5.340e-4,
         -1.572e-4, -5.901e-5, -3.272e-4, -6.975e-4, -3.440e-4, -9.403e-4, 5.092e-4, -1.083e-3}};
    // AZ-120223 - extra dZ-shifts of GEMs
    /*Double_t dzAZ[9][4] = {{0.40,0.45,0.00,0.00}, {0.05,0.05,0.35,0.35}, {-0.15,-0.15,-0.30,-0.30},
               {0.18,0.18,0.30,0.30}, {0.05,0.05,0.00,0.00}, {0.05,0.05,0.08,0.08},
               {-0.15,-0.15,0.00,0.00}, {0}, {0}}; */

    static TH2D *hLorCorX[2][17] = {{nullptr}, {nullptr}}, *hLorCorY[2][17] = {{nullptr}, {nullptr}};
    static int irun = -1;
    static Double_t zStat[19] = {0};

    if (irun <= 0)
        irun = ((DstEventHeader*)FairRootManager::Instance()->GetObject("DstEventHeader."))->GetRunId();   // AZ-010723

    // Lorentz shifts
    // AZ-010723 if (hLorCor[0][0] == nullptr) {
    if (irun <= 0)
        return;
    else if (hLorCorX[0][0] == nullptr) {
        // Station Z-positions
        // AZ-160226 - VSP
        int nsta0 = 0;
        if (fVspStationSet)
            nsta0 = fVspStationSet->GetNStations();
        for (int ista = 0; ista < nsta0; ++ista)
            zStat[ista] = fVspStationSet->GetZStationPosition(ista);
        //
        int nsta = SilStationSet->GetNStations();
        for (int ista = 0; ista < nsta; ++ista)
            zStat[ista + nsta0] = SilStationSet->GetZStationPosition(ista);
        int nsta1 = GemStationSet->GetNStations();
        for (int ista = 0; ista < nsta1; ++ista)
            zStat[ista + nsta + nsta0] = GemStationSet->GetZStationPosition(ista);
        if (fVerbose > 0)
            for (int j = 0; j < nsta0 + nsta + nsta1; ++j)
                cout << " Stat: " << j << " " << zStat[j] << endl;

        map<int, vector<Double_t>> lorCorX[2], lorCorY[2];
        int nx, ny;

        for (int igem = 0; igem < 2; ++igem) {
            ReadCorrections(irun, igem, lorCorX, lorCorY, nx, ny);
            Double_t dx = 160.0 / nx, dy = 45.0 / ny;
            // Double_t dx = 20.0, dy = 15.0;

            for (int iupd = 0; iupd < 2; ++iupd) {
                int istb = (igem == 0) ? 0 : nsta + nsta0;
                int iste = (igem == 0) ? nsta + nsta0 : nsta + nsta1 + nsta0;
                if (iste > (int)lorCorX[0].size())
                    --iste;   // exclude last small GEM

                for (int ist = istb; ist < iste; ++ist) {
                    TString hName = "hLorCorX_";
                    hName += iupd;
                    hName += "_";
                    hName += ist;
                    // hLorCor[iupd][ist] = new TH2D(hName, "", 8, -80, 80, 3, 0, 45);
                    int nxfsd = 0;
                    if (igem)
                        hLorCorX[iupd][ist] = new TH2D(hName, "", nx, -80, 80, ny, 0, 45);
                    else {
                        nxfsd = lorCorX[0][ist + 1].size();
                        hLorCorX[iupd][ist] = new TH2D(hName, "", nxfsd, 0, nxfsd, ny, 0, ny);
                    }

                    hName = "hLorCorY_";
                    hName += iupd;
                    hName += "_";
                    hName += ist;
                    if (igem)
                        hLorCorY[iupd][ist] = new TH2D(hName, "", nx, -80, 80, ny, 0, 45);
                    else
                        hLorCorY[iupd][ist] = new TH2D(hName, "", nxfsd, 0, nxfsd, ny, 0, ny);

                    if (igem) {
                        if (lorCorX[iupd].find(ist + 1) == lorCorX[iupd].end())
                            continue;   //   AZ-160226
                        // for (int j = 0; j < nx*ny; ++j) {
                        for (int j = 0; j < 24; ++j) {
                            // int ireg1 = j - 13;
                            // if (ireg1 < 0) ++ireg1;
                            int ireg1 = j - 12;
                            int ireg = TMath::Abs(ireg1);
                            if (ireg1 < 0)
                                --ireg;
                            int iy2 = ireg / 4;
                            int ix2 = ireg % 4;
                            Double_t x = (ix2 + 0.5) * dx;
                            if (ireg1 < 0)
                                x = -x;
                            Double_t y = (iy2 + 0.5) * dy;
                            hLorCorX[iupd][ist]->Fill(x, y, lorCorX[iupd][ist + 1][j]);   // X-corrections
                            if (lorCorY[iupd].find(ist + 1) != lorCorY[iupd].end())
                                hLorCorY[iupd][ist]->Fill(x, y, lorCorY[iupd][ist + 1][j]);   // Y-corrections
                        }   // for (int j = 0;
                    } else {
                        for (int j = 0; j < nxfsd; ++j) {
                            hLorCorX[iupd][ist]->Fill(j + 0.5, 0.5, lorCorX[iupd][ist + 1][j]);   // X-corrections
                            hLorCorY[iupd][ist]->Fill(j + 0.5, 0.5, lorCorY[iupd][ist + 1][j]);   // Y-corrections
                        }
                    }   // if (igem)
                }   // for (int ist = istb;
            }   // for (int iupd = 0;
        }   // for (int igem = 0;
    }   // else if (hLorCorX[0][0] == nullptr)

    int ista = hit->GetStationNr() - 1;
    int isec = hit->GetSectorNr() - 1;

    if (hit->GetSystemId() != kGEM && irun < 9020) {
        // if (0) {
        // AZ-070223 - alignment for run 8
        hit->SetX(hit->GetX() - SiDX[ista][isec]);
        hit->SetY(hit->GetY() - SiDY[ista][isec]);
        hit->SetZ(hit->GetZ() + SiDZ[ista][isec]);
        // AZ-090223 - Si rotation around Z-axis (w/out changing Y-coordinate)
        hit->SetX(hit->GetX()
                  - (p0si[ista][isec] / 2 + p1si[ista][isec] / 2 * hit->GetY() + p10si[ista][isec]
                     + p11si[ista][isec] * hit->GetY()));
    }
    // AZ-110223
    if (hit->GetSystemId() == kGEM && irun < 9020) {
        // if (0) {
        // AZ-070223 - apply alignment (from Igor) for run 8
        hit->SetX(hit->GetX() - StsDX[ista][isec]);
        hit->SetY(hit->GetY() - StsDY[ista][isec]);
        hit->SetZ(hit->GetZ() + GemDZ10[ista - 4][isec / 2]);

        // hit->SetZ(hit->GetZ() + dzAZ[ista - 4][isec / 2]);
        //  2 missed strips
        if (ista == 4 && isec == 3) {
            if (hit->GetX() >= -28.8 && hit->GetX() < -18.5) {
                hit->SetX(hit->GetX() - 0.16);
                hit->SetY(hit->GetY() - 0.16 / TMath::Tan(15.0 * TMath::DegToRad()));
            }
        }

        // AZ-280223 Apply rotation around OY for lower GEMs
        if (isec > 3) {
            // Double_t xshift[9] = {0.32, 0.51, 0.63, 0.89, 1.02, 1.232, 1.368};
            Double_t xshift[9] = {0.024, 0.058, 0.039, 0.050, 0.005, 0.028, 0.000};
            Double_t alpha = 0.0055;
            Double_t xnew = hit->GetX() * TMath::Cos(alpha) - (hit->GetZ() - zStat[ista]) * TMath::Sin(alpha);
            Double_t znew = hit->GetX() * TMath::Sin(alpha) + (hit->GetZ() - zStat[ista]) * TMath::Cos(alpha);
            hit->SetX(xnew + xshift[ista - 4]);
            // if (ista == 6) znew -= 0.05;
            // else if (ista == 7) znew -= 0.1;
            // else if (ista == 8) znew -= 0.1;
            // else if (ista == 10) znew += 0.2; //AZ-010323
            hit->SetZ(znew + zStat[ista]);
        }   // else if (ista == 10) hit->SetZ (hit->GetZ() - 0.15); //AZ-010323

        // AZ-030323 Lorentz shifts per station
        /*
        if (fieldFlag) {
       Double_t lors[2][9] = {{ 0.017, -0.027,  0.021, -0.018,  0.014, 0.009, -0.038},
                  {-0.012,  0.023, -0.030,  0.039, -0.033, 0.013, -0.001}};
       hit->SetX (hit->GetX() - lors[isec/4][ista-4] * 0.7);
        }
        */
    }   // if (hit->GetSystemId() == kGEM)

    if (fieldFlag && hit->GetSystemId() == kGEM) {
        // Lorentz corrections
        int iupd = isec / 4;
        Double_t xh = hit->GetX(), yh = TMath::Abs(hit->GetY()), xh1 = xh, yh1 = yh;
        TH2D* h2 = hLorCorX[iupd][ista];
        if (h2 == nullptr)
            return;
        if (xh1 >= h2->GetXaxis()->GetXmax())
            xh1 = h2->GetXaxis()->GetXmax() - 0.1;
        else if (xh1 < h2->GetXaxis()->GetXmin())
            xh1 = h2->GetXaxis()->GetXmin() + 0.1;
        if (yh1 >= h2->GetYaxis()->GetXmax())
            yh1 = h2->GetYaxis()->GetXmax() - 0.1;
        else if (yh1 < h2->GetYaxis()->GetXmin())
            yh1 = h2->GetYaxis()->GetXmin() + 0.1;
        // if (yh < h2->GetYaxis()->GetXmin() || yh >= h2->GetYaxis()->GetXmax()) cout << " yy " << isec << " " << yh <<
        // endl;
        Double_t dx = 0.0;
        if (irun < 9020)
            dx = h2->Interpolate(xh1, yh1);   // AZ-170226 - for run8
        else {                                // AZ-170226
            int jx = h2->GetXaxis()->FindBin(xh1);
            int jy = h2->GetYaxis()->FindBin(yh1);
            dx = h2->GetBinContent(jx, jy);
        }
        hit->SetX(xh - dx * 0.65);
        // hit->SetX (xh - dx * 0.85);   // AZ-170226 - check

        h2 = hLorCorY[iupd][ista];
        Double_t dy = 0.0;
        if (irun < 9020)
            dy = h2->Interpolate(xh1, yh1);   // AZ-170226 - for run8
        else {                                // AZ-170226
            int jx = h2->GetXaxis()->FindBin(xh1);
            int jy = h2->GetYaxis()->FindBin(yh1);
            dy = h2->GetBinContent(jx, jy);
        }
        if (iupd)
            dy = -dy;
        // cout << iupd << " " << ista << " " << dy << endl;
        Double_t ynew = yh - dy * 0.65;
        // Double_t ynew = yh - dy * 0.85;   // AZ-170226 - check
        if (iupd)
            ynew = -ynew;
        hit->SetY(ynew);
    } else if (hit->GetSystemId() != kGEM) {
        int iupd = isec / hLorCorX[0][ista]->GetNbinsX();
        int ireg = isec % hLorCorX[0][ista]->GetNbinsX();
        TH2D* h2 = hLorCorX[iupd][ista];
        Double_t dx = h2->GetBinContent(ireg + 1, 1);
        hit->SetX(hit->GetX() - dx * 0.65);
        // hit->SetX(hit->GetX() - dx * 0.85);   // AZ-170226 - check
        h2 = hLorCorY[iupd][ista];
        Double_t dy = h2->GetBinContent(ireg + 1, 1);
        hit->SetY(hit->GetY() - dy * 0.65);
        // hit->SetY(hit->GetY() - dy * 0.85);   // AZ-170226 - check
    }
}

// -------------------------------------------------------------------------

void BmnToCbmHitConverter::ReadCorrections(int irun,
                                           int igem,
                                           std::map<int, std::vector<Double_t>>* lorCorX,
                                           std::map<int, std::vector<Double_t>>* lorCorY,
                                           int& nx,
                                           int& ny)
{
    // Read file with "effective" Lorentz-corrections

    ifstream fin;
    TString fileName, chline;

    fileName = gSystem->ExpandPathName("$VMCWORKDIR");
    // AZ-030825 fileName += "/input/";
    fileName += "/parameters/reco/";   // AZ-030825
    if (igem)
        fileName += "LorentzGemCorr.txt";
    else
        fileName += "LorentzFsdCorr.txt";
    fin.open(fileName.Data());
    if (!fin.is_open()) {   // AZ-040825
        cout << " !!! Lorentz correction file " << fileName << " not found !!!" << endl;
        exit(1);
    }

    map<int, std::size_t> runsMap;
    TObjArray* tokens = nullptr;

    while (1) {
        // while (!fin.eof()) {
        size_t pos = fin.tellg();
        chline.ReadLine(fin);
        if (fin.eof())
            break;
        if (chline.Contains("Run")) {
            // new run
            // cout << chline << endl;
            tokens = chline.Tokenize(" ");
            TString run = ((TObjString*)tokens->UncheckedAt(1))->String();
            run.Atoi();
            // runsMap[run.Atoi()] = fin.tellg();
            runsMap[run.Atoi()] = pos;
            // cout << run.Atoi() << " " << pos << endl;
        }
        if (tokens) {
            tokens->Delete();
            delete tokens;
            tokens = nullptr;
        }
    }
    // if (chline[0] == '*') continue; // comment
    // cout << runsMap.size() << endl;

    auto iter = runsMap.upper_bound(irun);
    if (iter != runsMap.begin())
        --iter;
    cout << " ---> Using corrections for run " << iter->first << " " << iter->second << " " << igem << endl;

    fin.clear();
    fin.seekg(iter->second);
    chline.ReadLine(fin);
    // cout << chline << endl;

    // Read detectors
    while (1) {
        chline.ReadLine(fin);
        if (fin.eof())
            break;
        if (chline.Contains("Station")) {
            // cout << chline << endl;
            tokens = chline.Tokenize(" ");
            TString stat = ((TObjString*)tokens->UncheckedAt(1))->String();
            int ista = stat.Atoi();
            int iupd = 0;   // half-station
            if (chline.Contains("lower"))
                iupd = 1;
            nx = ((TObjString*)tokens->UncheckedAt(3))->String().Atoi();
            ny = ((TObjString*)tokens->UncheckedAt(4))->String().Atoi();
            int ixdata = 0;
            if (chline.Contains("X-"))
                ixdata = 1;
            // cout << ista << " " << iupd << " " << nx << " " << ny << endl;
            if (nx * ny < 1)
                cout << " !!! Wrong input line !!! " << chline << " " << nx << " " << ny << endl;
            if (tokens) {
                tokens->Delete();
                delete tokens;
                tokens = nullptr;
            }
            chline.ReadLine(fin);
            tokens = chline.Tokenize(", ");
            int nreg = tokens->GetEntriesFast();
            if (nreg != nx * ny) {
                cout << " !!! Wrong input line !!! " << chline << " " << nreg << " " << nx * ny << endl;
                exit(0);
            }
            auto lorCor = vector<Double_t>(nreg);
            // cout << tokens->GetEntriesFast() << " " << ((TObjString*)tokens->UncheckedAt(0))->String() << endl;
            for (int itok = 0; itok < nreg; ++itok) {
                Double_t corr = ((TObjString*)tokens->UncheckedAt(itok))->String().Atof();
                // lorCor[iupd][ista][itok] = corr;
                lorCor[itok] = corr;
            }
            if (ixdata)
                lorCorX[iupd][ista] = lorCor;
            else
                lorCorY[iupd][ista] = lorCor;
            if (tokens) {
                tokens->Delete();
                delete tokens;
                tokens = nullptr;
            }
        } else if (chline.Contains("*")) {
            continue;   // comment
        } else
            break;
    }   // while (1)
    fin.close();
}

// -------------------------------------------------------------------------

Bool_t BmnToCbmHitConverter::CorrectEffic(CbmStsHit* hit, int iq)
{
    // AZ-211223 Apply efficiency corrections for Monte Carlo

    /*
    if (hit->GetStationNr() == 3) {
       // Apply efficiency for 1 station
       if (gRandom->Rndm() > 0.90) return kFALSE; // 90%
    }
    */
    // return kTRUE; // no inefficiency applied

    static int first = 1;
    const int nstat = 19;
    static TH2D* hEff[2][nstat] = {{nullptr}, {nullptr}};

    if (first) {
        first = 0;
        // Get histograms with station efficiencies
        TString fileName;
        fileName = gSystem->ExpandPathName("$VMCWORKDIR");
        // AZ-220725 fileName += "/input/";
        fileName += "/parameters/reco/";   // AZ-220725
        TString fileEff = "SiGemEffic.root";
        if (FairRun::Instance()->GetTask("STS Vector Finder V9")) {
            fileEff = "SiGemEfficV8.root";
        }
        fileName += fileEff;
        TFile* f = TFile::Open(fileName);
        cout << " *** BmnToCbmHitConverter::CorrectEffic(): Reading Monte Carlo efficiency correction file " << endl;
        cout << " *** " << fileName << endl;

        for (int ipm = 0; ipm < 2; ++ipm) {

            for (int i = 0; i < nstat; ++i) {
                TString hName = (ipm == 0) ? "hEffpos_" : "hEffneg_";
                hName += (1 + i);
                TObject* obj = f->FindObjectAny(hName);
                if (obj == nullptr)
                    break;
                hEff[ipm][i] = (TH2D*)obj;
                // cout << hName << endl;
            }
        }
    }

    int ipm = (iq > 0) ? 0 : 1;
    int ista = hit->GetStationNr() - 1;
    if (hEff[ipm][ista] == nullptr)
        return kTRUE;   // last small GEM
    int ix = hEff[ipm][ista]->GetXaxis()->FindBin(hit->GetX());
    int iy = hEff[ipm][ista]->GetYaxis()->FindBin(hit->GetY());
    if (ix < 1 || ix > hEff[ipm][ista]->GetNbinsX() || iy < 1 || iy > hEff[ipm][ista]->GetNbinsY())
        return kTRUE;
    Double_t eff = hEff[ipm][ista]->GetBinContent(ix, iy);

    // eff = 99; //AZ-170824 - no eff. correction

    if (eff >= 0.999999)
        return kTRUE;
    if (gRandom->Rndm() <= eff)
        return kTRUE;
    return kFALSE;
}

// -------------------------------------------------------------------------

void BmnToCbmHitConverter::CorrectHitMC(CbmStsHit* hit, Double_t pmom0, int iq)
{
    // AZ-270824 - apply coordinate corrections for MC hits
    // AZ-040125 - use 2-component Gaussian description

    Float_t dx = 0;

    // AZ-310823 Extra shift for MC
    if (hit->GetSystemId() == kGEM) {
        dx = 0.0100;
        if (hit->GetStationNr() % 2) {
            if (hit->GetSectorNr() < 5)
                dx = -dx;
        } else if (hit->GetSectorNr() >= 5)
            dx = -dx;
        hit->SetX(hit->GetX() - dx * 0.65);
    }

    // return; // AZ-040125 - no corrections

    static int first = 1;
    // static map<int,Double_t> shiftsPos[2][19], sigmasPos[2][19], shiftsNeg[2][19], sigmasNeg[2][19];
    static map<int, vector<Float_t>> paramsPos[2][19], paramsNeg[2][19];

    // Read file with corrections
    if (first) {
        first = 0;
        ifstream fin;
        TString fileName, chline;

        fileName = gSystem->ExpandPathName("$VMCWORKDIR");
        // AZ-030825 fileName += "/input/SiGemHitCorrMC.txt";
        fileName += "/parameters/reco/SiGemHitCorrMC.txt";   // AZ-030825
        fin.open(fileName.Data());
        if (!fin.is_open()) {   // AZ-030825
            cout << " !!! MC correction file " << fileName << " not found !!!" << endl;
            exit(1);
        }

        TObjArray* tokens = nullptr;

        while (1) {
            chline.ReadLine(fin);
            if (fin.eof())
                break;
            // cout << chline << endl;
            if (chline.Contains("#"))
                continue;   // comment
            tokens = chline.Tokenize(" ");
            TString stations = ((TObjString*)tokens->UncheckedAt(0))->String();
            int stat1 = stations.Atoi(), stat2 = stat1;
            if (stations.Contains("-")) {
                // Several stations
                stat1 = stations.Atoi();
                int ip = stations.First("-");
                TString sstat = stations(ip + 1, stations.Length() - ip - 1);
                stat2 = sstat.Atoi();
            }
            int pmin = (((TObjString*)tokens->UncheckedAt(1))->String()).Atof();   // in MeV
            int pmax = (((TObjString*)tokens->UncheckedAt(2))->String()).Atoi();   // in MeV
            Float_t ampl1 = (((TObjString*)tokens->UncheckedAt(3))->String()).Atof();
            Float_t shift1 = (((TObjString*)tokens->UncheckedAt(4))->String()).Atof() * 1.1;
            Float_t sigma1 = (((TObjString*)tokens->UncheckedAt(5))->String()).Atof() * 1.1;
            Float_t ampl2 = (((TObjString*)tokens->UncheckedAt(6))->String()).Atof();
            Float_t shift2 = (((TObjString*)tokens->UncheckedAt(7))->String()).Atof() * 1.1;
            Float_t sigma2 = (((TObjString*)tokens->UncheckedAt(8))->String()).Atof() * 1.1;
            int ixy = 0;
            if ((((TObjString*)tokens->UncheckedAt(9))->String()).Contains("Y"))
                ixy = 1;
            Float_t ampl = ampl1 + ampl2;
            // cout << stat1 << " " << stat2 << " " << pmax << " " << ampl1 << " " << shift1 << " " << sigma1 << " " <<
            // ixy
            //      << endl;

            for (int ista = stat1; ista <= stat2; ++ista) {
                if (pmin < 0) {
                    paramsNeg[ixy][ista][pmax] = {ampl1 / ampl, shift1, sigma1, ampl2 / ampl, shift2, sigma2};
                } else {
                    paramsPos[ixy][ista][pmax] = {ampl1 / ampl, shift1, sigma1, ampl2 / ampl, shift2, sigma2};
                }
            }
            if (tokens) {
                tokens->Delete();
                delete tokens;
                tokens = nullptr;
            }
        }
    }

    int ista = hit->GetStationNr(), pmom = 0;
    Float_t sigx = 0.03, sigy = 0.05, dy = 0;   // for backward compatibility
    if (hit->GetSystemId() != kGEM) {
        sigx = 0.008;
        sigy = 0.04;
    }
    if (iq != 0) {
        int pmomev = int(pmom0 * 1000 / iq);
        auto par = (iq > 0) ? &paramsPos : &paramsNeg;
        auto params = *par;
        if (params[0][ista].size() != 0) {
            pmom = TMath::Min(pmomev, params[0][ista].rbegin()->first - 1);
            auto iter = params[0][ista].lower_bound(pmom);
            vector<Float_t>& pars = iter->second;
            dx = pars[1];
            sigx = pars[2];
            if (gRandom->Rndm() > pars[0]) {
                dx = pars[4];
                sigx = pars[5];
            }
        }
        if (params[1][ista].size() != 0) {
            pmom = TMath::Min(pmomev, params[1][ista].rbegin()->first - 1);
            auto iter = params[1][ista].lower_bound(pmom);
            vector<Float_t>& pars = iter->second;
            dy = pars[1];
            sigy = pars[2];
            if (gRandom->Rndm() > pars[0]) {
                dy = pars[4];
                sigy = pars[5];
            }
        }
    }

    hit->SetX(hit->GetX() + gRandom->Gaus(dx, sigx));
    hit->SetY(hit->GetY() + gRandom->Gaus(dy, sigy));

    // cout << " zzz " << iq << " " << ista << " " << pmom0 << " " << pmom << " " << dx << " " << sigx << " " << dy << "
    // " << sigy << endl;
}

// AZ-060625 -------------------------------------------------------------------------

void BmnToCbmHitConverter::AddVspHits()
{
    // Add VSP (Vertex Silicon Plane - CBM sensors) hits

    int nhits = fBmnVspHitsArray->GetEntriesFast();

    for (Int_t iHit = 0; iHit < nhits; ++iHit) {
        BmnVSPHit* bmnHit = (BmnVSPHit*)fBmnVspHitsArray->UncheckedAt(iHit);
        TVector3 pos;
        bmnHit->Position(pos);
        pos[2] -= 0.0150;   // AZ - shift to the entrance
        TVector3 dpos;
        if (fUseFixedErrors) {
            dpos[0] = fDXsil;   // 0.01 / TMath::Sqrt(12); //AZ
            // dpos[1] = 0.1234; //AZ
            dpos[1] = fDYsil;   // 0.021; //AZ - as in cbmroot
        } else {
            bmnHit->PositionError(dpos);
        }

        Int_t sens = 1;
        Int_t detId = kVSP << 24 | (bmnHit->GetStation() + 1) << 16 | (bmnHit->GetModule() + 1) << 4 | sens << 1;
        new ((*fCbmHitsArray)[fCbmHitsArray->GetEntriesFast()]) CbmStsHit(detId, pos, dpos, 0.0, 0, 0);
        CbmStsHit* hit = (CbmStsHit*)fCbmHitsArray->At(fCbmHitsArray->GetEntriesFast() - 1);
        if (fExp && fAlignmentEnabled)
            // if (0)
            ApplyAlignment(hit);
        else if (!fExp) {
            /*
            int iq = 0;
            Double_t pmom = 0;
            if (bmnHit->GetRefIndex() >= 0) {
               FairMCPoint* mcp = (FairMCPoint*)fSilPoints->UncheckedAt(bmnHit->GetRefIndex());
               CbmMCTrack* mctr = (CbmMCTrack*)fMCTracks->UncheckedAt(mcp->GetTrackID());
               pmom = mctr->GetP();
               if (TDatabasePDG::Instance()->GetParticle(mctr->GetPdgCode()))
                  iq = TMath::Nint(TDatabasePDG::Instance()->GetParticle(mctr->GetPdgCode())->Charge() / 3);
            }
            // Correct MC hit
            CorrectHitMC(hit, pmom, iq);
            if (iq == 0)
               iq = 1;   // for backward compatibility

            Bool_t ok = CorrectEffic(hit, iq);   // efficiency correction
            if (!ok) {
               // Remove hit
               fCbmHitsArray->RemoveLast();
               continue;
            } */
            // AZ-140824 - shift coordinates
            hit->SetY(hit->GetY() + 0.30);
        }
        FairRootManager::Instance()->SetUseFairLinks(kTRUE);
        hit->ResetLinks();
        hit->SetLinks(bmnHit->GetLinks());
        FairRootManager::Instance()->SetUseFairLinks(kFALSE);
        hit->SetRefIndex(bmnHit->GetRefIndex());
        hit->fDigiF =
            bmnHit->GetLowerClusterIndex() + 4000000;   //!!!AZ - for VectorFinder (to have unique indices) - 260322
        hit->fDigiB =
            bmnHit->GetUpperClusterIndex() + 5000000;   //!!!AZ - for VectorFinder (to have unique indices) - 260322
        // AZ-221022
        StripCluster* upp = (StripCluster*)fBmnVspUpperClusters->UncheckedAt(hit->fDigiB % 1000000);
        StripCluster* low = (StripCluster*)fBmnVspLowerClusters->UncheckedAt(hit->fDigiF % 1000000);
        Double_t uppq = (upp) ? upp->TotalSignal : 0.0;
        Double_t lowq = (low) ? low->TotalSignal : 0.0;
        Double_t corrq = (uppq - lowq) / (uppq + lowq);
        hit->SetSignalDiv(corrq);
        uppq = (upp) ? upp->Width : 0.0;   // N of strips
        lowq = (low) ? low->Width : 0.0;
        corrq = (uppq - lowq) / (uppq + lowq);
        hit->SetTimeStamp(corrq);   // just using available space
        hit->SetStrips((low) ? low->MeanPosition : 0.0,
                       (upp) ? upp->MeanPosition : 0.0);   // AZ-190823
                                                           // hit->SetWidths (low->Width, upp->Width); //AZ-190823
    }   // for (Int_t iHit = 0; iHit < nhits
}
