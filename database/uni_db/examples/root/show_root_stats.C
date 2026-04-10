// limits to define out-of-range values and to exclude them while calculating the averages
#define MIN_FILE_SIZE_MB  1
#define MAX_FILE_SIZE_GB  4
#define MIN_EVENT_SIZE_KB 1
#define MAX_EVENT_SIZE_KB 200

//bool isTerminal = isatty(fileno(stdout));
const string red("\033[0;91m"), orange("\033[0;33m"), cyan("\033[0;96m"), reset("\033[0m");

class UniqueRunNumber;
int calculate_size(TString dir_path, const char* root_tree_name, TH1F* hist_file_size, TH1F* hist_event_size, double& sum_file_size_mb,
                   double& sum_event_size_kb, int& correct_file_count, map<TString, double>& wrong_files, map<TString, double>& wrong_events);

/**
 * Function for creating histograms on sizes of ROOT (digit) files stored in a specified directory amd events in the files
*/
int show_root_stats(TString dir_path, const char* root_tree_name = "bmndata")
{
    if (dir_path == "")
    {
        cout<<"ERROR: Directory path was not specified"<<endl;
        return -1;
    }
    gSystem->ExpandPathName(dir_path);

    TH1F* hist_file_size = new TH1F("hist_file_size", "ROOT file sizes", MAX_FILE_SIZE_GB*1024/50, 0, MAX_FILE_SIZE_GB*1024);
    TAxis* xaxis1 = hist_file_size->GetXaxis();
    xaxis1->SetTitle("ROOT FIle Size, MB");

    TH1F* hist_event_size = new TH1F("hist_event_size", "Event sizes", MAX_EVENT_SIZE_KB/5, 0, MAX_EVENT_SIZE_KB);
    TAxis* xaxis2 = hist_event_size->GetXaxis();
    xaxis2->SetTitle("Event Size, KB");

    int correct_file_count = 0;
    double sum_file_size_mb = 0, sum_event_size_kb = 0;
    map<TString, double> wrong_files;
    map<TString, double> wrong_events;
    calculate_size(dir_path, root_tree_name, hist_file_size, hist_event_size, sum_file_size_mb, sum_event_size_kb,
                   correct_file_count, wrong_files, wrong_events);

    if (correct_file_count > 0)
    {
        double average_file_size = sum_file_size_mb / correct_file_count;
        double average_event_size = sum_event_size_kb / correct_file_count;

        cout<<cyan<<"-------------------------------------------------------"<<endl;
        cout<<correct_file_count<<" ROOT files have been successfully processed (dir: "<<dir_path<<")"<<endl;
        cout<<"Average ROOT file size: "<<average_file_size<<" MB"<<endl;
        cout<<"Average event size: "<<average_event_size<<" KB"<<endl;
        cout<<"-------------------------------------------------------"<<endl<<reset;

        TCanvas* canvas_size = new TCanvas("canvas_size","canvas_size",1200, 640);
        canvas_size->Divide(2, 1);
        canvas_size->cd(1);
        hist_file_size->Draw();
        canvas_size->cd(2);
        hist_event_size->Draw();
    }

    if (wrong_files.size() > 0)
    {
        cout<<endl<<"Wrong file size: (< "<<MIN_FILE_SIZE_MB<<" MB or > "<<MAX_FILE_SIZE_GB<<" GB):"<<endl;
        for (auto it = wrong_files.begin(); it != wrong_files.end(); ++it)
            cout<<"File '"<<(*it).first<<"': "<<(*it).second<<" MB"<<endl;
        cout<<red;
        cout<<"Total wrong file size: "<<wrong_files.size()<<endl;
        cout<<reset;
    }
    cout<<endl;
    if (wrong_events.size() > 0)
    {
        cout<<endl<<"Wrong event size: (< "<<MIN_EVENT_SIZE_KB<<" KB or > "<<MAX_EVENT_SIZE_KB<<" KB):"<<endl;
        for (auto it = wrong_events.begin(); it != wrong_events.end(); ++it)
            cout<<"File '"<<(*it).first<<": "<<(*it).second<<" KB"<<endl;
        cout<<red;
        cout<<"Total wrong event size: "<<wrong_events.size()<<endl;
        cout<<reset;
    }
    cout<<endl;

    return 0;
}

// non-user function for creating histograms on root file sizes and stored event size for a given directory (recursively)
// returns the success code: 0 - without errors, error code - otherwise
int calculate_size(TString dir_path, const char* root_tree_name, TH1F* hist_file_size, TH1F* hist_event_size, double& sum_file_size_mb,
                   double& sum_event_size_kb, int& correct_file_count, map<TString, double>& wrong_files, map<TString, double>& wrong_events)
{
    TSystemDirectory dir(dir_path.Data(), dir_path.Data());
    TList* file_list = dir.GetListOfFiles();
    if (file_list == nullptr)
    {
        cout<<"WARNING: There are no files in the directory: "<<dir_path<<endl;
        return 0;
    }

    TSystemFile* cur_file;
    TIter iterFile(file_list);
    while ((cur_file = (TSystemFile*)iterFile.Next()))
    {
        TString file_name = cur_file->GetName();
        if ((file_name == ".") || (file_name == "..")) continue;

        TString file_dir = cur_file->GetTitle();
        // if directory
        if (cur_file->IsDirectory())
        {
            calculate_size(file_dir, root_tree_name, hist_file_size, hist_event_size, sum_file_size_mb, sum_event_size_kb,
                           correct_file_count, wrong_files, wrong_events);
        }
        else
        {
            TString file_path = file_dir + "/" + file_name;
            if (file_name.EndsWith(".root"))
            {
                Long64_t file_size_bytes = 0;
                Long_t id = 0, flags = 0, modtime = 0;
                gSystem->GetPathInfo(file_path.Data(), &id, &file_size_bytes, &flags, &modtime);

                TFile root_file(file_path);
                TTree* root_tree = (TTree*) root_file.Get(root_tree_name);
                if (root_tree == nullptr)
                {
                    cout<<"ERROR: there are no tree '"<<root_tree_name<<"' in file: "<<file_path<<endl;
                    root_file.Close();
                    continue;
                }

                double file_size_mb = file_size_bytes / 1024.0 / 1024.0;
                double file_size_gb = file_size_mb / 1024.0;
                int event_count = root_tree->GetEntries();
                double event_size_kb = file_size_bytes / 1024.0 / event_count;

                int is_file_wrong = 0;
                if (file_size_gb > MAX_FILE_SIZE_GB)
                {
                    is_file_wrong = 1;
                    wrong_files.insert(pair<TString, double>(file_path, file_size_mb));
                }
                // do not take into account files with size < MIN_FILE_SIZE_MB
                if (file_size_mb < MIN_FILE_SIZE_MB)
                {
                    is_file_wrong = 2;
                    wrong_files.insert(pair<TString, double>(file_path, file_size_mb));
                }
                // do not take into account files with event size < MIN_EVENT_SIZE_KB or > MAX_EVENT_SIZE_KB
                else if (((event_size_kb < MIN_EVENT_SIZE_KB)) || (event_size_kb > MAX_EVENT_SIZE_KB))
                {
                    is_file_wrong = 2;
                    wrong_events.insert(pair<TString, double>(file_path, event_size_kb));
                }
                else {
                    sum_file_size_mb += file_size_mb;
                    sum_event_size_kb += event_size_kb;
                    correct_file_count++;
                }

                if (is_file_wrong == 1) cout<<orange;
                else if (is_file_wrong == 2) cout<<red;
                cout<<file_path<<" size: "<<file_size_mb<<" MB (event size = "<<event_size_kb<<" KB)"<<endl;
                if (is_file_wrong > 0) cout<<reset;

                hist_file_size->Fill(file_size_mb);
                hist_event_size->Fill(event_size_kb);

                root_file.Close();
            }//if (file_name.EndsWith(".root"))
        }//if not a directory
    }//while file list

    return 0;
}
