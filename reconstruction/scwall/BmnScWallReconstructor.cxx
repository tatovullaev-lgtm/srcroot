/** @file BmnScWallReconstructor.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 03.04.2021
 **
 ** Code for BmnScWallEvent reconstruction (creation) from BmnScWallDigi {Data} or BmnScWallDigit {Sim}
 **/

#include "BmnScWallReconstructor.h"

#include "TTreeFormula.h"

BmnScWallReconstructor::BmnScWallReconstructor(bool isExp, bool isGlobalCoordinates)
{
    fIsExp = isExp;
    fIsGlobal = isGlobalCoordinates;
    LOG(detail) << Form("BmnScWallReconstructor: isExp %i; isGlobal %i", fIsExp, fIsGlobal);
}

void BmnScWallReconstructor::SetRecoCutsFile(TString reco_cuts_file)
{
    fRecoCutsFile = reco_cuts_file;
    ParseCuts();
    LOG(detail) << Form("BmnScWallReconstructor: Reco cuts %s", fRecoCutsFile.Data());
}

BmnScWallReconstructor::~BmnScWallReconstructor()
{
    if (fBmnScWallEvent)
        delete fBmnScWallEvent;
}

InitStatus BmnScWallReconstructor::Init()
{
    fworkTime = 0.;
    fpFairRootMgr = FairRootManager::Instance();
    (fIsExp) ? fArrayOfDigits = (TClonesArray*)fpFairRootMgr->GetObject("ScWallDigi")
             : fArrayOfDigits = (TClonesArray*)fpFairRootMgr->GetObject("ScWallDigit");

    if (!fArrayOfDigits) {
        LOG(error) << "BmnScWallReconstructor::Init(): branch with Digits not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnScWallEvent = new BmnScWallEvent();
    fBmnScWallEvent->reset();
    ParseGeometry();
    fpFairRootMgr->RegisterAny("ScWallEvent", fBmnScWallEvent, kTRUE);

    LOG(debug) << "ScWall Reconstructor ready";
    return kSUCCESS;
}

void BmnScWallReconstructor::ParseCuts()
{
    namespace po = boost::program_options;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/parameters/scwall/";

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
        LOG(error) << Form("BmnScWallReconstructor: Loading Cuts from file: %s - file open error!",
                           fRecoCutsFile.Data());
        return;
    }
    LOG(debug) << Form("BmnScWallReconstructor: Loading Cuts from file: %s", fRecoCutsFile.Data());
    po::store(po::parse_config_file(config_file, desc), vm);
    config_file.close();
    po::notify(vm);

    if (fIsExp)
        fSelectionString = cutExp;
    else
        fSelectionString = cutSim;
}

void BmnScWallReconstructor::ParseGeometry()
{
    BmnScWallGeo* GeoHandler = new BmnScWallGeo();
    GeoHandler->ReadGeometryFromGeoManager(fIsGlobal);
    const auto& positionMap = GeoHandler->GetPositionMap();
    for (auto it : positionMap) {
        auto unique_address = it.first;
        if (unique_address == 0)
            continue;
        auto cell_id = BmnScWallAddress::GetCellId(unique_address);
        auto this_cell = fBmnScWallEvent->GetCell(cell_id);
        this_cell->SetCellId(cell_id);
        this_cell->SetSignal(0.);

        this_cell->SetPosition(it.second.first);
        this_cell->SetPositionError(it.second.second);
    }
    delete GeoHandler;
}

void BmnScWallReconstructor::Exec(Option_t* opt)
{

    if (!IsActive())
        return;

    TStopwatch sw;
    sw.Start();
    fBmnScWallEvent->ResetCells();
    auto digiVector = GetSelectedDigiVector(fSelectionString);

    for (auto it : digiVector) {
        BmnScWallDigit* ThisDigi = (BmnScWallDigit*)it;
        auto cell_id = ThisDigi->GetCellId();   // 1 to 174
        if (cell_id <= 0 || cell_id > BmnScWallEvent::fgkMaxCells) {
            if (fVerbose)
                LOG(warning) << Form("ScWall digi ignored. Cell %d ", cell_id);
            continue;
        }
        fBmnScWallEvent->GetCell(cell_id)->SetSignal(ThisDigi->GetSignal());
    }

    fBmnScWallEvent->SummarizeEvent();
    sw.Stop();
    fworkTime += sw.RealTime();
}

std::vector<TObject*> BmnScWallReconstructor::GetSelectedDigiVector(TString formulaString)
{
    TTree* temp_tree = new TTree("temp_tree", "temp_tree");
    TString temp_br_name = (fIsExp) ? "ScWallDigi" : "ScWallDigit";
    fSelectedDigiVector.clear();

    if (fIsExp) {
        BmnScWallDigi* ptr = nullptr;
        temp_tree->Branch(temp_br_name.Data(), &ptr);
        for (int i = 0; i < fArrayOfDigits->GetEntriesFast(); i++) {
            ptr = (BmnScWallDigi*)fArrayOfDigits->At(i);
            temp_tree->Fill();
        }
    } else {
        BmnScWallDigit* ptr = nullptr;
        temp_tree->Branch(temp_br_name.Data(), &ptr);
        for (int i = 0; i < fArrayOfDigits->GetEntriesFast(); i++) {
            ptr = (BmnScWallDigit*)fArrayOfDigits->At(i);
            temp_tree->Fill();
        }
    }

    TTreeFormula* formula = new TTreeFormula("TTreeformula", formulaString.Data(), temp_tree);
    if (!formula || formula->GetNdim() == 0)
        LOG(error) << Form("BmnScWallReconstructor::GetSelectedDigiVector. Bad cuts. Check formula %s from file %s",
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

void BmnScWallReconstructor::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch("ScWallEvent", &fBmnScWallEvent);
    resultTree->Fill();
}

void BmnScWallReconstructor::Finish()
{
    printf("Work time of BmnScWallReconstructor: %4.2f sec.\n", fworkTime);
}
