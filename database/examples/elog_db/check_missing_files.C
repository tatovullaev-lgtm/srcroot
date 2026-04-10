#include "../../../services/function_set.h"
#include <TString.h>

// function for checking files from a list in the given text file whether they are missing in the Electronic Logbook
// returns the success code: 0 - without errors, error_code - otherwise
int check_missing_files(TString list_file_path, int period_number = 8)
{
    gSystem->ExpandPathName(list_file_path);

    list<string> file_list;
    ifstream list_file(list_file_path.Data());
    string s;
    while (getline(list_file, s))
        file_list.push_back(s);
    if (file_list.empty())
    {
        cout<<"WARNING: There are no files in the list of the input text file: "<<list_file_path<<endl;
        return 0;
    }

    vector<string> missing_files;
    vector<string> existing_files;
    for (auto const& cur_file_path : file_list)
    {
        string file_name_wo_ext = get_file_name(cur_file_path);
        // parse a run number from the name
        string str_run_number = find_first_number(file_name_wo_ext);
        if (str_run_number == "")
        {
            cout<<"No run number found in the file name: "<<cur_file_path<<", skipping.."<<endl;
            continue;
        }
        int run_number = stoi(str_run_number);

        // search records with the run number in the Electronic Logbook
        TObjArray* pRecords = ElogRecord::GetRecords(period_number, run_number);
        if (pRecords->IsEmpty())
            missing_files.push_back(cur_file_path);
        else
            existing_files.push_back(cur_file_path);
    }//while file list

    if (!missing_files.empty())
    {
        cout<<endl<<"Files not corresponding any run in the logbook:"<<endl;
        for (auto const& cur_file_path: missing_files)
            cout<<cur_file_path<<endl;
        cout<<endl;
    }
    if (!existing_files.empty())
    {
        cout<<endl<<"Files for runs described in the logbook:"<<endl;
        for (auto const& cur_file_path: existing_files)
            cout<<cur_file_path<<endl;
        cout<<endl;
    }

    return 0;
}
