R__ADD_INCLUDE_PATH($VMCWORKDIR)
R__LOAD_LIBRARY(libcurl)
#include "database/event_db/event_db_settings.h"
#include <curl/curl.h>
#include <services/nlohmann/json.hpp>
using json = nlohmann::ordered_json;
//namespace json = nlohmann::json;

int write_emd_2db(TString json_path);
// function for recursively searching for DST ROOT files and reading them to write their event metadata to the Event Catalogue
// returns the success code: 0 - without errors, error_code - otherwise
int write_emd(TString dir_name, TString storage_name, TString software_version, bool isWriteEventDB = false)
{
    gSystem->ExpandPathName(dir_name);

    TSystemDirectory dir(dir_name.Data(), dir_name.Data());
    TList* file_list = dir.GetListOfFiles();
    if (file_list == nullptr)
    {
        cout<<"WARNING: There are no files in the directory: "<<dir_name<<endl;
        return 0;
    }

    int total_parsed_events = 0, added_events = 0, total_files = 0, parsed_files = 0, added_files = 0;
    TSystemFile* cur_file;
    TIter iterFile(file_list);
    while ((cur_file = (TSystemFile*)iterFile.Next()))
    {
        TString file_name = cur_file->GetName();
        if ((file_name == ".") || (file_name == "..")) continue;

        TString file_dir = cur_file->GetTitle();
        if (cur_file->IsDirectory())    // if directory
        {
            write_emd(file_dir, storage_name, software_version);
        }
        else
        {
            total_files++;
            TString file_path = file_dir;
            if (file_path[file_path.Length()-1] != '/') file_path += "/";
            file_path += file_name;
            if (file_name.EndsWith(".root"))
            {
                // open DST ROOT file, then print and write event metadata to the Event Catalogue
                TFile dst_file(file_path);
                TTree* dst_tree = (TTree*) dst_file.Get("bmndata");
                if (!dst_tree)
                {
                    cout<<"WARNING: the current file has no 'bmndata' tree: "<<file_path<<endl;
                    continue;
                }
                cout<<"Current file: "<<dst_tree->GetCurrentFile()->GetName()<<endl;

                // read Run Header if present
                DstRunHeader* run_header = (DstRunHeader*) dst_tree->GetCurrentFile()->Get("DstRunHeader");
                int period_number = -1, run_number = -1, track_number = -1;
                if (run_header)
                {
                    period_number = run_header->GetPeriodNumber();
                    run_number = run_header->GetRunNumber();
                    delete run_header;
                }
                else
                {
                    cout<<"WARNING: the current file has not DST format: "<<file_path<<endl;
                    continue;
                }

                // link Event Header to get event metadata
                DstEventHeader* fEventHeader = nullptr;
                dst_tree->SetBranchAddress("DstEventHeader.", &fEventHeader);

                Int_t event_count = dst_tree->GetEntries(); // events = records - 1; (-RunHeader)
                if (event_count < 1)
                {
                    cout<<endl<<"ERROR: the current file (Run "<<period_number<<":"<<run_number<<") has 0 events: "<<file_path<<endl;
                    continue;
                }
                parsed_files++;

                json emd_json = json::array();
                // assign TClonesArray to 'BmnGlobalTrack' branch
                TClonesArray* globTracks = nullptr;
                dst_tree->SetBranchAddress("BmnGlobalTrack", &globTracks);
                dst_tree->SetBranchStatus("*", 0);
                dst_tree->SetBranchStatus("DstEventHeader.", 1);
                dst_tree->SetBranchStatus("BmnGlobalTrack", 1);
                // event loop
                int parsed_events = 0;
                for (Int_t curEvent = 0; curEvent < dst_tree->GetEntries(); curEvent++)
                {
                    dst_tree->GetEntry(curEvent);
                    track_number = globTracks->GetEntriesFast();

                    // write event metadata
                    json emd_element;
                    emd_element["reference"]["storage_name"] = storage_name.Data();
                    emd_element["reference"]["file_path"] = file_path.Data();
                    emd_element["reference"]["event_number"] = curEvent;
                    emd_element["software_version"] = software_version.Data();
                    emd_element["period_number"] = period_number;
                    emd_element["run_number"] = run_number;
                    emd_element["parameters"]["track_number"] = track_number;

                    emd_json.push_back(emd_element);
                    parsed_events++;
                }
                total_parsed_events += parsed_events;

                // write JSON to a file
                size_t lastindex = file_path.Last('.');
                TString json_name = file_path(0, lastindex);
                json_name.ReplaceAll('/', '_');
                json_name.Append(".json");
                ofstream json_file(json_name.Data());
                json_file << setw(4) << emd_json << endl;
                json_file.close();
                //string json_string = emd_json.dump();   // emd_json.get<string>(); // emd_json.dump();
                //cout<<"json_string = "<<json_string<<endl;

                if (isWriteEventDB)
                {
                    gSystem->ExpandPathName(json_name);
                    EventCatalogue ec;
                    int ret_code = ec.AddEventInfo(json_name.Data());
                    if (ret_code == 0)
                    {
                        added_files++;
                        added_events += parsed_events;
                    }
                    cout<<endl;
                    // remove JSON file
                    gSystem->Unlink(json_name);
                }
            }//if (file_name.EndsWith(".root"))
        }//if not a directory
    }//while file list

    if (total_parsed_events > 0)
    {
        cout<<endl<<total_parsed_events<<" events have been parsed in "<<parsed_files<<" files";
        if (isWriteEventDB) cout<<" and "<<added_events<<" events were successfully added from "<<added_files<<" files";
        cout<<" (dir: "<<dir_name<<" with "<<total_files<<" files)"<<endl<<endl;
    }
    else
        cout<<endl<<"There are no DST ROOT files in the directory (dir: "<<dir_name<<")"<<endl<<endl;

    return 0;
}
