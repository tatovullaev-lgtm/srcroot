/** @file BmnFHCalReconstructor.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 03.04.2021
 **
 ** Code for BmnFHCalEvent reconstruction (creation) from BmnFHCalDigi {Data} or BmnFHCalDigit {Sim}
 **/

#include "BmnFHCalReconstructor.h"

#include "BmnDetectorList.h"
#include "BmnFHCalGeo.h"
#include "BmnFHCalPoint.h"
#include "TTreeFormula.h"

BmnFHCalReconstructor::BmnFHCalReconstructor(bool isExp, bool isGlobalCoordinates)
{
    fIsExp = isExp;
    fDoMatchMC = (fIsExp) ? false : true;
    fIsGlobal = isGlobalCoordinates;
    LOG(detail) << Form("BmnFHCalReconstructor: isExp %i; isGlobal %i", fIsExp, fIsGlobal);
}

void BmnFHCalReconstructor::SetRecoCutsFile(TString reco_cuts_file)
{
    fRecoCutsFile = reco_cuts_file;
    ParseCuts();
    LOG(detail) << Form("BmnFHCalReconstructor: Reco cuts %s", fRecoCutsFile.Data());
}

BmnFHCalReconstructor::~BmnFHCalReconstructor()
{
    if (fBmnFHCalEvent)
        delete fBmnFHCalEvent;
}

InitStatus BmnFHCalReconstructor::Init()
{
    fworkTime = 0.;
    fpFairRootMgr = FairRootManager::Instance();

    if (fIsExp) {
        fArrayOfDigits = (TClonesArray*)fpFairRootMgr->GetObject("FHCalDigi");
        if (!fArrayOfDigits) {
            LOG(error) << "BmnFHCalReconstructor::Init(): branch with Digits not found! Task will be deactivated";
            SetActive(kFALSE);
            return kERROR;
        }
    } else {
        fArrayOfDigits = (TClonesArray*)fpFairRootMgr->GetObject("FHCalDigit");
        if (!fArrayOfDigits) {
            LOG(error) << "BmnFHCalReconstructor::Init(): branch with Digits not found! Task will be deactivated";
            SetActive(kFALSE);
            return kERROR;
        }

        fArrayOfPoints = (TClonesArray*)fpFairRootMgr->GetObject("FHCalPoint");
        if (!fArrayOfPoints) {
            LOG(error) << "BmnFHCalReconstructor::Init(): branch with FHCalPoints not found! No MC match will be done";
            fDoMatchMC = false;
        }
    }

    fBmnFHCalEvent = new BmnFHCalEvent();
    fBmnFHCalEvent->reset();
    ParseGeometry();
    fpFairRootMgr->RegisterAny("FHCalEvent", fBmnFHCalEvent, kTRUE);
    fpFairRootMgr->Register("FHCalEvent", "FHCal", fBmnFHCalEvent, kFALSE);

    LOG(debug) << "FHCal Reconstructor ready";
    return kSUCCESS;
}

void BmnFHCalReconstructor::ParseCuts()
{
    namespace po = boost::program_options;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/parameters/fhcal/";

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
        LOG(error) << Form("BmnFHCalReconstructor: Loading Cuts from file: %s - file open error!",
                           fRecoCutsFile.Data());
        return;
    }
    LOG(debug) << Form("BmnFHCalReconstructor: Loading Cuts from file: %s", fRecoCutsFile.Data());
    po::store(po::parse_config_file(config_file, desc), vm);
    config_file.close();
    po::notify(vm);

    if (fIsExp)
        fSelectionString = cutExp;
    else
        fSelectionString = cutSim;
}

void BmnFHCalReconstructor::ParseGeometry()
{
    BmnFHCalGeo* GeoHandler = new BmnFHCalGeo();
    GeoHandler->ReadGeometryFromGeoManager(fIsGlobal);
    if (GeoHandler->GetMaxModuleId() > 65)
        LOG(error) << Form("BmnFHCalReconstructor::Check geometry file. Trying to set number of FHCal modules %d",
                           GeoHandler->GetMaxModuleId());
    fBmnFHCalEvent->SetTotalModules(GeoHandler->GetMaxModuleId());
    const auto& positionMap = GeoHandler->GetPositionMap();
    for (auto it = positionMap.begin(); it != positionMap.end(); ++it) {
        auto unique_address = it->first;
        auto position = it->second.first;
        if (BmnFHCalAddress::GetSectionId(unique_address) != 1)
            continue;
        auto mod_id = BmnFHCalAddress::GetModuleId(unique_address);
        auto this_mod = fBmnFHCalEvent->GetModule(mod_id);
        this_mod->SetModuleId(mod_id);
        this_mod->SetPosition(position);
        if (BmnFHCalAddress::GetModuleType(unique_address) == 1)
            this_mod->SetNsections(7);
        if (BmnFHCalAddress::GetModuleType(unique_address) == 2)
            this_mod->SetNsections(10);

        auto err = it->second.second;
        this_mod->SetDxyz(err.X(), err.Y(), err.Z() * this_mod->GetNsections());
    }
    delete GeoHandler;
}

void BmnFHCalReconstructor::Exec(Option_t* opt)
{
    if (!IsActive())
        return;
    TStopwatch sw;
    sw.Start();

    fBmnFHCalEvent->ResetEnergies();
    auto digiVector = GetSelectedDigiVector(fSelectionString);

    for (auto it : digiVector) {
        BmnFHCalDigit* ThisDigi = (BmnFHCalDigit*)it;
        auto mod_id = ThisDigi->GetModuleId();
        auto sec_id = ThisDigi->GetSectionId();
        if (mod_id <= 0 || (int)mod_id > fBmnFHCalEvent->GetTotalModules() || sec_id <= 0
            || (int)sec_id > fBmnFHCalEvent->GetModule(mod_id)->GetNsections())
        {
            if (fVerbose)
                LOG(warning) << Form("FHCal digi ignored. Mod %d Sec %d", mod_id, sec_id);
            continue;
        }
        fBmnFHCalEvent->GetModule(mod_id)->SetSectionEnergy(sec_id, ThisDigi->GetSignal());
    }

    if (!fIsExp && fDoMatchMC)
        MatchMCTracks();

    fBmnFHCalEvent->SummarizeEvent();
    sw.Stop();
    fworkTime += sw.RealTime();
}

std::vector<TObject*> BmnFHCalReconstructor::GetSelectedDigiVector(TString formulaString)
{
    TTree* temp_tree = new TTree("temp_tree", "temp_tree");
    TString temp_br_name = (fIsExp) ? "FHCalDigi" : "FHCalDigit";
    fSelectedDigiVector.clear();

    if (fIsExp) {
        BmnFHCalDigi* ptr = nullptr;
        temp_tree->Branch(temp_br_name.Data(), &ptr);
        for (int i = 0; i < fArrayOfDigits->GetEntriesFast(); i++) {
            ptr = (BmnFHCalDigi*)fArrayOfDigits->At(i);
            temp_tree->Fill();
        }
    } else {
        BmnFHCalDigit* ptr = nullptr;
        temp_tree->Branch(temp_br_name.Data(), &ptr);
        for (int i = 0; i < fArrayOfDigits->GetEntriesFast(); i++) {
            ptr = (BmnFHCalDigit*)fArrayOfDigits->At(i);
            temp_tree->Fill();
        }
    }

    TTreeFormula* formula = new TTreeFormula("TTreeformula", formulaString.Data(), temp_tree);
    if (!formula || formula->GetNdim() == 0)
        LOG(error) << Form("BmnFHCalReconstructor::GetSelectedDigiVector. Bad cuts. Check formula %s from file %s",
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

void BmnFHCalReconstructor::MatchMCTracks()
{
    // Enable the use of FairLinks in FairRootManager
    fpFairRootMgr->SetUseFairLinks(kTRUE);

    // Iterate over all modules in the event
    for (uint imod = 1; imod < (uint)fBmnFHCalEvent->GetTotalModules(); ++imod) {
        auto* mod = fBmnFHCalEvent->GetModule(imod);
        assert(mod);

        // Iterate over all points
        for (int iPt = 0; iPt < fArrayOfPoints->GetEntriesFast(); ++iPt) {
            auto* point = static_cast<BmnFHCalPoint*>(fArrayOfPoints->At(iPt));
            assert(point);
            if (point->GetModuleId() != imod)
                continue;   // Skip if point is from another module

            if (!point->GetPointerToLinks()) {
                LOG(debug4) << "BmnFHCalReconstructor::MatchMCTracks. Link nullptr";
                continue;
            }

            mod->AddLinks(*point->GetPointerToLinks());
            LOG(debug4) << Form("BmnFHCalReconstructor::MatchMCTracks. Updated module with id "
                                "%d. Now it has %d links",
                                mod->GetModuleId(), mod->GetNLinks());
        }
    }

    // Disable the use of FairLinks in FairRootManager
    fpFairRootMgr->SetUseFairLinks(kFALSE);
}

void BmnFHCalReconstructor::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch("FHCalEvent", &fBmnFHCalEvent);
    resultTree->Fill();
}

void BmnFHCalReconstructor::Finish()
{
    printf("Work time of BmnFHCalReconstructor: %4.2f sec.\n", fworkTime);
}
