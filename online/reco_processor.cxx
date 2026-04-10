#include "BmnCSCHitMaker.h"
#include "BmnFHCalReconstructor.h"
#include "BmnFillTrigInfoDst.h"
#include "BmnGemStripHitMaker.h"
#include "BmnGlobalTracking.h"
#include "BmnNewFieldMap.h"
#include "BmnSiBTHitMaker.h"
#include "BmnSiliconHitMaker.h"
#include "BmnStsVectorFinder.h"
#include "BmnTask.h"
#include "BmnToCbmHitConverter.h"
#include "BmnTof1HitProducer.h"
// #include "BmnTof701HitProducer.h"
// #include "BmnTofHitProducer.h"
#include "CbmKF.h"
#include "CbmL1.h"
#include "UniRun.h"

#include <FairRootFileSink.h>
#include <FairRunAna.h>
#include <RootSerializer.h>
#include <TBranchElement.h>
#include <TFile.h>
#include <TKey.h>
#include <TRandom2.h>
#include <TString.h>
#include <TTree.h>
#include <fairmq/Device.h>
#include <fairmq/runDevice.h>
#include <string>

namespace bpo = boost::program_options;

namespace bmn::online
{
struct ReconstructionProcessor : fair::mq::Device
{
  protected:
    static constexpr auto INPUT_CHANNEL_NAME = "in-channel";
    static constexpr auto OUTPUT_CHANNEL_NAME = "out-channel";

    void Init() override { OnData(INPUT_CHANNEL_NAME, &ReconstructionProcessor::HandleData); }

    void InitTask() override
    {
        fRunAna = std::make_unique<FairRunAna>();
        fRunAna->SetGenerateRunInfo(kFALSE);

        fRunAna->SetSink(new FairRootFileSink("help.root"));
        FairRootManager::Instance()->InitSink();

        fRunPeriod = fConfig->GetValue<Int_t>("run-period");

        fRegisteredBranches.clear();
        fTasks.clear();
    }

    void ResetTask() override { fTasks.clear(); }

    Bool_t SetupExperimentalRun(std::unique_ptr<TTree>& inputTree, Int_t& runNumber)
    {
        LOG(info) << "Setup experimental run...";

        auto geoFileName = Form("current_geo_file_%d.root", UInt_t(TRandom2(0).Integer(UINT32_MAX)));
        Int_t nearest_geometry_run = runNumber + 1;
        if (UniRun::ReadPreviousGeometryFile(fRunPeriod, nearest_geometry_run, geoFileName)) {
            LOG(error) << "Could not read geometry file from the Unified Condition Database";
            return kFALSE;
        }

        auto geoFile = std::make_unique<TFile>(geoFileName, "READ");
        if (!geoFile->IsOpen()) {
            LOG(error) << "Could not open ROOT file with geometry: " << geoFileName;
            return kFALSE;
        }

        TIter next(geoFile->GetListOfKeys());
        auto key = std::unique_ptr<TKey>(static_cast<TKey*>(next()));
        TString className(key->GetClassName());
        if (className.BeginsWith("TGeoManager"))
            key->ReadObj();
        else {
            LOG(error) << "TGeoManager is not top element in geometry file " << geoFileName;
            return kFALSE;
        }

        auto currentRun = std::unique_ptr<UniRun>(UniRun::GetRun(fRunPeriod, runNumber));
        if (!currentRun)
            return kFALSE;

        auto magneticFieldVoltage = currentRun->GetFieldVoltage();
        if (!magneticFieldVoltage) {
            LOG(error) << "No field voltage was found for run " << fRunPeriod << ":" << runNumber;
            return kFALSE;
        }

        Double_t mapCurrent = (fRunPeriod == 8) ? 112.0 : (fRunPeriod == 7) ? 55.87 : 0.0;
        Double_t magneticFieldScale = 0.;
        if (*magneticFieldVoltage < 10) {
            magneticFieldScale = 0;
        } else {
            magneticFieldScale = (*magneticFieldVoltage) / mapCurrent;
        }

        auto magneticField = new BmnNewFieldMap("FieldMap_1900_extrap_noPed.root");
        magneticField->SetScale(magneticFieldScale);
        magneticField->Init();
        fRunAna->SetField(magneticField);

        TString target;
        if (!currentRun->GetTargetParticle()) {
            target = "-";
        } else {
            target = (currentRun->GetTargetParticle())[0];
        }

        auto beam = currentRun->GetBeamParticle();

        LOG(info) << "Complete!";

        LOG(info) << "Experimental run summary:";
        LOG(info) << "\tPeriod: run" << fRunPeriod;
        LOG(info) << "\tNumber: " << runNumber;
        LOG(info) << "\tBeam: " << beam;
        LOG(info) << "\tTarget: " << target;
        LOG(info) << "\tMagnetic field scale: " << setprecision(4) << magneticFieldScale;

        remove(geoFileName);

        return kTRUE;
    }

    void RegisterBranches(std::unique_ptr<TTree>& inputTree)
    {
        fRegisteredBranches.clear();

        auto branches = inputTree->GetListOfBranches();
        for (Long64_t branchIndex = 0; branchIndex < branches->GetEntriesFast(); branchIndex++) {
            auto branch = static_cast<TBranchElement*>(branches->At(branchIndex));
            if (strcmp(branch->GetClassName(), "TClonesArray") == 0) {
                fRegisteredBranches[branch->GetName()] = std::make_unique<TClonesArray>(branch->GetClonesName());
            }
        }

        for (auto& branch : fRegisteredBranches) {
            FairRootManager::Instance()->RegisterInputObject(branch.first.c_str(), branch.second.get());
        }
    }

    void UploadData(std::unique_ptr<TTree>& inputTree)
    {
        for (auto& branch : fRegisteredBranches) {
            branch.second->Delete();
            auto tmp = branch.second.get();
            if (inputTree->SetBranchAddress(branch.first.c_str(), &tmp)) {
                LOG(warning) << "Registered branch '" << branch.first << "' not found in input tree!";
            }
        }

        inputTree->GetEntry(0);
    }

    void InitializeReconstructionTasks(Int_t runNumber)
    {
        LOG(info) << "Initializing reconstruction tasks...";

        fTasks.clear();

        // Pileup Event Classification
        //! no inheritance from BmnTask
        // auto eventSelector = std::make_unique<BmnEventSelector>();
        // eventSelector->SetRunId(runNumber);
        // eventSelector->SetInputFileName("$VMCWORKDIR/input/eventSelector_calib_run8.root");
        // fTasks.push_back(std::move(eventSelector));

        // Fill basic trigger information
        auto triggerInfo = std::make_unique<BmnFillTrigInfoDst>(kTRUE);
        fTasks.push_back(std::move(triggerInfo));

        // Hits in front of the target
        auto sibtHM = std::make_unique<BmnSiBTHitMaker>(fRunPeriod, runNumber, kTRUE);
        sibtHM->SetHitMatching(kFALSE);
        fTasks.push_back(std::move(sibtHM));

        // Silicon hit finder
        auto siliconHM = std::make_unique<BmnSiliconHitMaker>(fRunPeriod, runNumber, kTRUE);
        siliconHM->SetHitMatching(kFALSE);
        fTasks.push_back(std::move(siliconHM));

        // GEM hit finder
        auto gemHM = std::make_unique<BmnGemStripHitMaker>(fRunPeriod, runNumber, kTRUE);
        gemHM->SetHitMatching(kFALSE);
        fTasks.push_back(std::move(gemHM));

        // CSC hit finder
        auto cscHM = std::make_unique<BmnCSCHitMaker>(fRunPeriod, runNumber, kTRUE);
        cscHM->SetHitMatching(kFALSE);
        fTasks.push_back(std::move(cscHM));

        // TOF1 hit finder
        auto tof1HP = std::make_unique<BmnTof1HitProducer>("TOF400", kTOF1, kFALSE, 0, kFALSE);
        tof1HP->SetPeriodRun(fRunPeriod, runNumber);
        fTasks.push_back(std::move(tof1HP));

        // TOF701 hit finder
        auto tof700HP = std::make_unique<BmnTof1HitProducer>("TOF700", kTOF701, kFALSE, 0, kFALSE);
        tof700HP->SetPeriodRun(fRunPeriod, runNumber);
        fTasks.push_back(std::move(tof700HP));

        // Legacy
        // // TOF2 hit finder
        // auto tof2HP = std::make_unique<BmnTofHitProducer>("TOF", "TOF700_geometry_run8.txt", kFALSE, 0, kFALSE);
        // tof2HP->SetTimeResolution(0.115);
        // tof2HP->SetProtonTimeCorrectionFile("bmn_run9687_digi_calibration.root");
        // tof2HP->SetMCTimeFile("TOF700_MC_xenon_dcmsmm_time_run8.txt");
        // // 0 - minimal time, 1 - maximal amplitude
        // tof2HP->SetMainStripSelection(0);
        // // 0 - Petukhov, 1 - Panin SRC, 2 - Petukhov Argon (default), 3 - Petukhov Xenon (preliminary)
        // tof2HP->SetSelectXYCalibration(3);
        // // minimal digit time
        // tof2HP->SetTimeMin(-2.f);
        // // Maximal digit time
        // tof2HP->SetTimeMax(+50.f);
        // // Abs maximal difference for small chambers
        // tof2HP->SetDiffTimeMaxSmall(1.2f);
        // // Abs maximal difference for big chambers
        // tof2HP->SetDiffTimeMaxBig(3.5f);
        // fTasks.push_back(std::move(tof2HP));

        auto fhcalReco = std::make_unique<BmnFHCalReconstructor>(kTRUE, kTRUE);
        fhcalReco->SetRecoCutsFile("FHCAL_reco_cuts_period8.txt");
        fTasks.push_back(std::move(fhcalReco));

        // auto scwallReco = std::make_unique<BmnScWallReconstructor>(kTRUE, kTRUE);
        // scwallReco->SetRecoCutsFile("SCWALL_reco_cuts_period8.txt");
        // fTasks.push_back(std::move(scwallReco));

        // auto hodoReco = std::make_unique<BmnHodoReconstructor>("", kTRUE);
        // hodoReco->SetRecoCutsFile("HODO_reco_cuts_period8.txt");
        // fTasks.push_back(std::move(hodoReco));

        // auto ndetReco = std::make_unique<BmnNdetReconstructor>(kTRUE, kTRUE);
        // ndetReco->SetRecoCutsFile("NDET_reco_cuts_period8.txt");
        // fTasks.push_back(std::move(ndetReco));

        // auto vacReco = std::make_unique<BmnVacWallReconstructor>();
        // fTasks.push_back(std::move(vacReco));

        // Inner Tracking
        auto innerTrackBranchName = "StsVector";
        auto hitConverter = std::make_unique<BmnToCbmHitConverter>(0, kTRUE);
        hitConverter->EnableAlignment(kFALSE);
        hitConverter->SetFixedErrors(0.08 / sqrt(12) * 1.2, 0.1234, 0.01 / sqrt(12), 0.1234);
        fTasks.push_back(std::move(hitConverter));

        auto kalmanFilter = std::make_unique<CbmKF>();
        fTasks.push_back(std::move(kalmanFilter));

        auto l1Tracking = std::make_unique<CbmL1>();
        l1Tracking->SetMaterialBudgetFileName("");
        fTasks.push_back(std::move(l1Tracking));

        auto vectorFinder = std::make_unique<BmnStsVectorFinder>();
        vectorFinder->SetMatBudgetFileName("$VMCWORKDIR/parameters/sts_matbudget_run8.root");

        //! no inheritance from BmnTask
        // auto stsMatchTracks = std::make_unique<BmnStsMatchTracks>(0);
        // stsMatchTracks->SetTrackBranch(innerTrackBranchName);
        // fTasks.push_back(std::move(stsMatchTracks));

        //! no inheritance from BmnTask
        // auto beamTF = std::make_unique<BmnBeamTracking>(fRunPeriod);
        // fTasks.push_back(std::move(beamTF));

        // auto pvFinder = std::make_unique<CbmPVFinderKF>();
        // auto findVertex = std::make_unique<CbmFindPrimaryVertex>(pvFinder);
        // findVertex->SetTrackBranch(innerTrackBranchName);
        // fTasks.push_back(std::move(findVertex));

        // auto pvMPD = std::make_unique<BmnKFPrimaryVertexFinder>();
        // fTasks.push_back(std::move(pvMPD));

        // auto combPV = std::make_unique<BmnCombineVertexFinder>();
        // fTasks.push_back(std::move(combPV));

        //! no inheritance from BmnTask
        // auto pidAnalyzer = std::make_unique<BmnPid>();
        // fTasks.push_back(std::move(pidAnalyzer));

        // auto centralityClustering =
        // std::make_unique<CentralityClusterizer>("XeCsI@3.8AGeV_MBT_7clusters_period8.root");
        // fTasks.push_back(std::move(centralityClustering));

        auto glTF = std::make_unique<BmnGlobalTracking>(kTRUE, kFALSE);
        glTF->SetInnerTracksBranchName(innerTrackBranchName);
        fTasks.push_back(std::move(glTF));

        for (auto&& task : fTasks) {
            task->SetVerbose(0);
            task->InitTask();
        };

        LOG(info) << "Complete!";
    }

    Bool_t HandleData(fair::mq::MessagePtr& inputMessage, Int_t)
    {
        std::unique_ptr<TTree> inputTree(nullptr);
        RootSerializer().Deserialize(*inputMessage, inputTree);

        if (fCurrentEvent == 0) {
            Int_t runNumber = -1;
            if (!SetupExperimentalRun(inputTree, runNumber))
                return kFALSE;
            RegisterBranches(inputTree);
            InitializeReconstructionTasks(runNumber);
        }

        UploadData(inputTree);

        LOG(info) << "Event #" << fCurrentEvent;

        auto resultTree = std::make_unique<TTree>();
        for (auto&& task : fTasks) {
            task->Exec("0");
            task->OnlineWrite(resultTree);
        }

        auto message = NewMessage();
        RootSerializer().Serialize(*message, resultTree);

        if (Send(message, OUTPUT_CHANNEL_NAME) < 0) {
            LOG(error) << "Sending message failed!";
            return kFALSE;
        }

        fCurrentEvent++;

        return kTRUE;
    }

  private:
    Long64_t fCurrentEvent = 0;
    Int_t fRunPeriod = 0;

    std::vector<std::unique_ptr<BmnTask>> fTasks;
    std::unordered_map<std::string, std::unique_ptr<TClonesArray>> fRegisteredBranches;

    std::unique_ptr<FairRunAna> fRunAna;
};
}   // namespace bmn::online

void addCustomOptions(bpo::options_description& options)
{
    auto op = options.add_options();

    op("run-period", bpo::value<Int_t>()->default_value(8), "BM@N Run period");
}

std::unique_ptr<fair::mq::Device> getDevice(fair::mq::ProgOptions&)
{
    return std::make_unique<bmn::online::ReconstructionProcessor>();
}
