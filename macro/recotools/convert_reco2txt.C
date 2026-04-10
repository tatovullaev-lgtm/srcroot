R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include <services/nlohmann/json.hpp>

using json = nlohmann::json;
using namespace ROOT;

int convert_reco2txt(TString input_list_file,
                     TString output_file = "reco_tracks.txt",
                     TString configFileName = "recotxt_config.json")
{
    TStopwatch timer;
    timer.Start();

    gSystem->ExpandPathName(input_list_file);

    list<string> file_list;
    ifstream list_file(input_list_file.Data());
    string s;
    while (getline(list_file, s))
        file_list.push_back(s);
    if (file_list.empty()) {
        cout << "WARNING: There are no files in the list of the input text file: " << input_list_file << endl;
        return 0;
    }

    TChain chain("bmndata");
    for (auto const& cur_file_path : file_list) {
        TString strFilePath(cur_file_path);
        gSystem->ExpandPathName(strFilePath);

        chain.Add(strFilePath);
    }

    /*
    // read configuration variables
    configFileName = gSystem->ExpandPathName("$VMCWORKDIR/config/" + configFileName);
    ifstream configFile(configFileName.Data());
    if (!configFile.is_open()) {
        cout << "[ERROR] Cannot open config file " << configFileName << endl;
        return -2;
    }
    json config;
    try {
        configFile >> config;
    }
    catch (const json::parse_error& ex) {
        cout << "[ERROR] Wrong config file format: " << ex.what() << endl;
        return -3;
    }
    Int_t minNTracks = config.value("minNTracks", 2);
    Double_t minZ = config.value("minZ", -TMath::Infinity());
    Double_t maxZ = config.value("maxZ", TMath::Infinity());
    vector<string> excludedBranches = config.value("excludedBranches", vector<string>());

    if (outFormat != "root") {
        cout << "[ERROR] Only \"root\" format is supported at the moment" << endl;
        return -4;
    }
    */
    /*
        TFile* inFile = TFile::Open(inFilePath.Data());
        if (inFile == nullptr) {
            cout << "[ERROR] Cannot open input DST file " << inFilePath << endl;
            return -5;
        }
        TTree* inTree = (TTree*) inFile->Get("bmndata");
        if (!inTree) {
            cout << "[ERROR] Input file " << inFilePath << " does not contain \"bmndata\" tree" << endl;
            return -6;
        }
    *//*
    // exclude branches
    vector<string> branches;
    for (TObject* branch: *inTree->GetListOfBranches()) {
        string branchName = ((TBranch*)branch)->GetName();
        if (find(excludedBranches.begin(), excludedBranches.end(), branchName) == excludedBranches.end())
            branches.push_back(branchName);
    }
*/
    ofstream file(output_file.Data());
    file << "event# firstVx firstVy firstVz lastVx lastVy lastVz firstTx firstTy lastTx lastTy firstQp lastQp "
            "firstCov[0] firstCov[1] firstCov[2] firstCov[3] firstCov[4] firstCov[5] firstCov[6] firstCov[7] "
            "firstCov[8] firstCov[9] firstCov[10] firstCov[11] firstCov[12] firstCov[13] firstCov[14] "
            "lastCov[0] lastCov[1] lastCov[2] lastCov[3] lastCov[4] lastCov[5] lastCov[6] lastCov[7] "
            "lastCov[8] lastCov[9] lastCov[10] lastCov[11] lastCov[12] lastCov[13] lastCov[14] "
            "Chi2 NDF Length Nhits Beta400 Beta700 fDCAInVertex PDGCode"
         << endl;

    auto save_to_txt = [&file](const TClonesArray& global_tracks, const TClonesArray& sts_tracks,
                               const TClonesArray& sts_match, const BmnMCInfoDst& mc_info, int col_iter) {
        for (int i = 0; i < global_tracks.GetEntries(); i++) {
            BmnGlobalTrack* cur_global_track = static_cast<BmnGlobalTrack*>(global_tracks.UncheckedAt(i));

            // check that there is information on Beta from TOF-400 or TOF-700
            if ((cur_global_track->GetBeta(1) == -1000) and (cur_global_track->GetBeta(2) == -1000))
                continue;

            // get PDG code
            CbmTrackMatch* sts_match_track =
                static_cast<CbmTrackMatch*>(sts_match.UncheckedAt(cur_global_track->GetGemTrackIndex()));
            if (sts_match_track->GetMCTrackId() < 0)
                continue;
            CbmMCTrack* mc_track =
                static_cast<CbmMCTrack*>(mc_info.GetMCTracks()->UncheckedAt(sts_match_track->GetMCTrackId()));
            // cout<<"PDG code = "<<mc_track->GetPdgCode()<<endl;

            FairTrackParam* param_first = cur_global_track->GetParamFirst();
            FairTrackParam* param_last = cur_global_track->GetParamLast();

            file << col_iter << " " << param_first->GetX() << " " << param_first->GetY() << " " << param_first->GetZ()
                 << " " << param_last->GetX() << " " << param_last->GetY() << " " << param_last->GetZ() << " "
                 << param_first->GetTx() << " " << param_first->GetTy() << " " << param_last->GetTx() << " "
                 << param_last->GetTy() << " " << param_first->GetQp() << " " << param_last->GetQp() << " ";
            Double_t cov_matrix1[15];
            param_first->CovMatrix(cov_matrix1);
            for (int j = 0; j < 15; j++)
                file << cov_matrix1[j] << " ";
            Double_t cov_matrix2[15];
            param_first->CovMatrix(cov_matrix2);
            for (int j = 0; j < 15; j++)
                file << cov_matrix2[j] << " ";
            file << cur_global_track->GetChi2() << " " << cur_global_track->GetNDF() << " "
                 << cur_global_track->GetLength() << " " << cur_global_track->GetNHits() << " "
                 << cur_global_track->GetBeta(1) << " " << cur_global_track->GetBeta(2) << " "
                 << cur_global_track->GetDCAInVertex() << " " << mc_track->GetPdgCode() << endl;
        }
        return;
    };

    // function to cut events without vertex
    auto cutVertex = [](const CbmVertex& vtx) { return (vtx.GetNTracks() > 1); };

    int col_iter = -1;
    RDataFrame data(chain);
    auto w_iterator =
        data.Filter(cutVertex, {"PrimaryVertex."})
            //.Define("pdg_code", definePDG, {"BmnGlobalTrack", "StsVector", "StsTrackMatch", "BmnMCInfo."})
            .Define("col_iter", [&col_iter] { return ++col_iter; });

    ROOT::RDF::Experimental::AddProgressBar(data);
    w_iterator.Foreach(save_to_txt, {"BmnGlobalTrack", "StsVector", "StsTrackMatch", "BmnMCInfo.", "col_iter"});

    file.close();
    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    return 0;
}
