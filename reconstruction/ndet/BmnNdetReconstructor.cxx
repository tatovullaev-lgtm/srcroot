/** @file BmnNdetReconstructor.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 03.04.2021
 **
 ** Code for BmnNdetEvent reconstruction (creation) from BmnNdetDigi {Data} or BmnNdetDigit {Sim}
 **/

#include "BmnNdetReconstructor.h"

#include "BmnNdetDigi.h"
#include "BmnNdetDigit.h"
#include "BmnNdetGeo.h"
#include "BmnNdetPoint.h"
#include "BmnNdetSurfacePoint.h"
#include "FairLogger.h"
#include "TTreeFormula.h"

#include <TStopwatch.h>

BmnNdetReconstructor::BmnNdetReconstructor(bool isExp, bool isGlobalCoordinates)
{
    fIsExp = isExp;
    fIsExp ? fDoMatchMC = false : fDoMatchMC = true;
    fIsGlobal = isGlobalCoordinates;
    LOG(detail) << Form("BmnNdetReconstructor: isExp %i; isGlobal %i", fIsExp, fIsGlobal);
}

void BmnNdetReconstructor::SetRecoCutsFile(TString reco_cuts_file)
{
    fRecoCutsFile = reco_cuts_file;
    ParseCuts();
    LOG(detail) << Form("BmnNdetReconstructor: Reco cuts %s", fRecoCutsFile.Data());
}

BmnNdetReconstructor::~BmnNdetReconstructor()
{
    if (fBmnNdetEvent)
        delete fBmnNdetEvent;
    if (fGeoHandler)
        delete fGeoHandler;
}

InitStatus BmnNdetReconstructor::Init()
{
    fworkTime = 0.;
    fpFairRootMgr = FairRootManager::Instance();

    if (fIsExp) {
        fArrayOfDigits = (TClonesArray*)fpFairRootMgr->GetObject("NdetDigi");
        if (!fArrayOfDigits) {
            LOG(error) << "BmnNdetReconstructor::Init(): branch with Digits not found! Task will be deactivated";
            SetActive(kFALSE);
            return kERROR;
        }
    } else {
        fArrayOfDigits = (TClonesArray*)fpFairRootMgr->GetObject("NdetDigit");
        if (!fArrayOfDigits) {
            LOG(error) << "BmnNdetReconstructor::Init(): branch with Digits not found! Task will be deactivated";
            SetActive(kFALSE);
            return kERROR;
        }

        fArrayOfPoints = (TClonesArray*)fpFairRootMgr->GetObject("NdetPoint");
        if (!fArrayOfPoints) {
            LOG(error) << "BmnNdetReconstructor::Init(): branch with NdetPoints not found! Task will be deactivated";
            SetActive(kFALSE);
            return kERROR;
        }

        fArrayOfSurfacePoints = (TClonesArray*)fpFairRootMgr->GetObject("NdetSurfacePoint");
        if (!fArrayOfSurfacePoints) {
            LOG(error)
                << "BmnNdetReconstructor::Init(): branch with NdetSurfacePoint not found! Task will be deactivated";
            SetActive(kFALSE);
            return kERROR;
        }
        fpFairRootMgr->Register("NdetSurfacePoint", "NDET", fArrayOfSurfacePoints, kTRUE);
    }

    fBmnNdetEvent = new BmnNdetEvent();
    fBmnNdetEvent->reset();
    ParseGeometry();
    fpFairRootMgr->RegisterAny("NdetEvent", fBmnNdetEvent, kTRUE);
    fpFairRootMgr->Register("NdetEvent", "NDET", fBmnNdetEvent, kFALSE);

    LOG(debug) << "Ndet Reconstructor ready";
    return kSUCCESS;
}

void BmnNdetReconstructor::ParseCuts()
{
    namespace po = boost::program_options;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/parameters/ndet/";

    float version;
    std::string comment;
    std::string cutExp;
    std::string cutSim;

    // Setup options.
    po::options_description desc("Options");
    desc.add_options()("VERSION.id", po::value<float>(&version),
                       "version identificator")("COMMENT.str", po::value<std::string>(&comment), "comment")(
        "EXPERIMENT.cut", po::value<std::string>(&cutExp),
        "cut for Experimental data")("SIMULATION.cut", po::value<std::string>(&cutSim), "cut for Simulated data");

    // Load config file.
    po::variables_map vm;
    std::ifstream config_file((path + fRecoCutsFile).Data(), std::ifstream::in);
    if (!config_file.is_open()) {
        LOG(error) << Form("BmnNdetReconstructor : Loading Cuts from file: %s - file open error!",
                           fRecoCutsFile.Data());
        return;
    }
    LOG(debug) << Form("BmnNdetReconstructor : Loading Cuts from file: %s", fRecoCutsFile.Data());
    po::store(po::parse_config_file(config_file, desc), vm);
    config_file.close();
    po::notify(vm);

    fSelectionString = (fIsExp) ? cutExp : cutSim;
}

void BmnNdetReconstructor::ParseGeometry()
{
    fGeoHandler = new BmnNdetGeo();
    fGeoHandler->ReadGeometryFromGeoManager(fIsGlobal);
}

void BmnNdetReconstructor::Exec(Option_t* opt)
{

    if (!IsActive())
        return;
    fpFairRootMgr->SetUseFairLinks(kTRUE);
    TStopwatch sw;
    sw.Start();
    fBmnNdetEvent->reset();
    auto digiVector = GetSelectedDigiVector(fSelectionString);
    for (auto it : digiVector) {
        BmnNdetDigit* ThisDigi = (BmnNdetDigit*)it;
        auto address = ThisDigi->GetAddress();
        const auto& posMap = fGeoHandler->GetPositionMap();
        auto pos = posMap.find(address);
        if (pos == posMap.end()) {
            LOG(debug2) << Form("BmnNdetReconstructor : Exec. Digi address %d not found in geometry. Skipping",
                                address);
            if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug2))
                ThisDigi->Print();
            continue;
        }
        BmnNdetCell* this_cell = new BmnNdetCell();
        this_cell->SetAddress(address);
        this_cell->SetPosition(pos->second.first);
        this_cell->SetPositionError(pos->second.second);
        this_cell->SetSignal(ThisDigi->GetSignal());
        this_cell->SetTime(ThisDigi->GetTime());
        fBmnNdetEvent->AddCell(this_cell);
    }

    if (!fIsExp && fDoMatchMC)
        MatchMCTracks();

    sw.Stop();
    fworkTime += sw.RealTime();
    fpFairRootMgr->SetUseFairLinks(kFALSE);
}

std::vector<TObject*> BmnNdetReconstructor::GetSelectedDigiVector(TString formulaString)
{
    TTree* temp_tree = new TTree("temp_tree", "temp_tree");
    TString temp_br_name = (fIsExp) ? "NdetDigi" : "NdetDigit";
    fSelectedDigiVector.clear();

    if (fIsExp) {
        BmnNdetDigi* ptr = nullptr;
        temp_tree->Branch(temp_br_name.Data(), &ptr);
        for (int i = 0; i < fArrayOfDigits->GetEntriesFast(); i++) {
            ptr = (BmnNdetDigi*)fArrayOfDigits->At(i);
            temp_tree->Fill();
        }
    } else {
        BmnNdetDigit* ptr = nullptr;
        temp_tree->Branch(temp_br_name.Data(), &ptr);
        for (int i = 0; i < fArrayOfDigits->GetEntriesFast(); i++) {
            ptr = (BmnNdetDigit*)fArrayOfDigits->At(i);
            temp_tree->Fill();
        }
    }

    TTreeFormula* formula = new TTreeFormula("TTreeformula", formulaString.Data(), temp_tree);
    if (!formula || formula->GetNdim() == 0)
        LOG(error) << Form("BmnNdetReconstructor::GetSelectedDigiVector. Bad cuts. Check formula %s from file %s",
                           formulaString.Data(), fRecoCutsFile.Data());
    // next line is needed. Reason unclear. without it will lose some digis.
    TObject* ThisDigi = nullptr;
    temp_tree->SetBranchAddress(temp_br_name.Data(), &ThisDigi);
    for (int i = 0; i < temp_tree->GetEntries(); i++) {
        temp_tree->GetEntry(i);
        formula->GetNdata();
        if (formula->EvalInstance() > 0)   // passed selection
            fSelectedDigiVector.push_back(fArrayOfDigits->At(i));
    }
    formula->Delete();
    temp_tree->Delete();
    return fSelectedDigiVector;
}

void BmnNdetReconstructor::MatchMCTracks()
{
    // Enable the use of FairLinks in FairRootManager
    fpFairRootMgr->SetUseFairLinks(kTRUE);

    // Iterate over all cells in the event
    auto* cells = fBmnNdetEvent->GetModifiableCells();
    for (int ice = 0, nce = cells->GetEntriesFast(); ice < nce; ++ice) {
        auto* cell = static_cast<BmnNdetCell*>(cells->At(ice));
        if (!cell)
            continue;

        // Iterate over all points
        for (int iPt = 0, nPt = fArrayOfPoints->GetEntriesFast(); iPt < nPt; ++iPt) {
            auto* point = static_cast<BmnNdetPoint*>(fArrayOfPoints->At(iPt));
            if (!point)
                continue;
            if (point->GetAddress() != cell->GetAddress())
                continue;   // Skip if point is from another cell
            cell->AddLink(FairLink(static_cast<int>(BmnNdetCell::LinkType::MCPoint), iPt, point->GetEnergyLoss()));
            for (auto link : point->GetLinks()) {
                link.SetWeight(point->GetEnergyLoss());
                cell->AddLink(link);
            }
            LOG(debug) << Form("BmnNdetReconstructor::MatchMCTracks. Matched MC point and MC track. Updated cell with "
                               "address %d. Now it has %d links",
                               cell->GetAddress(), cell->GetNLinks());
        }
    }

    // Disable the use of FairLinks in FairRootManager
    fpFairRootMgr->SetUseFairLinks(kFALSE);
}

void BmnNdetReconstructor::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch("NdetEvent", &fBmnNdetEvent);
    if (!fIsExp)
        resultTree->Branch("NdetSurfacePoint", &fArrayOfSurfacePoints);
    resultTree->Fill();
}

void BmnNdetReconstructor::Finish()
{
    printf("Work time of BmnNdetReconstructor: %4.2f sec.\n", fworkTime);
}
