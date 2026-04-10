// limits to define out-of-range values and to exclude them while calculating the averages
#define MIN_EVENT_SIZE_KB 30
#define MAX_EVENT_SIZE_KB 1100
#define MIN_FILE_SIZE_MB  1
#define MAX_FILE_SIZE_GB  60
#define MIN_FILE_DUR_SEC  6
#define MAX_FILE_DUR_MIN  8
#define MIN_EVENT_RATE   100
#define MAX_EVENT_RATE   3600

class UniqueRunNumber;
/**
 * Function for creating statistics histograms on raw file sizes, event sizes, raw file durations
 * for runs stored in the UniConDa Database for a specified Run/Period (or run interval)
 * 'instead_show_stream' flag switches to showing average stream (event/sec) for raw files
*/
void show_raw_stats(int period = 8, int start_run = 6587, int end_run = -1, bool show_only_wrong = false)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRun::GetRunNumbers(run_numbers, period, start_run, period, end_run);
    if (run_count <= 0)
        return;

    //bool isTerminal = isatty(fileno(stdout));
    const string red("\033[0;91m"), orange("\033[0;33m"), cyan("\033[0;96m"), reset("\033[0m");

    TH1D* hist_file_size = new TH1D("hist_file_size", TString::Format("Raw file sizes for Period %d", period),
                                    2*MAX_FILE_SIZE_GB, 0, MAX_FILE_SIZE_GB);
    TAxis* xaxis_file_size = hist_file_size->GetXaxis();
    xaxis_file_size->SetTitle("Raw File Size, GB");

    TH1D* hist_file_duration = new TH1D("hist_file_duration", TString::Format("Raw file duration for Period %d", period),
                                        2*MAX_FILE_DUR_MIN*60, 0, MAX_FILE_DUR_MIN*60);
    TAxis* xaxis_file_duration = hist_file_duration->GetXaxis();
    xaxis_file_duration->SetTitle("Raw File Duration, sec");

    TH1D* hist_event_size = new TH1D("hist_event_size", TString::Format("Event sizes for Period %d", period),
                                     MAX_EVENT_SIZE_KB/5, 0, MAX_EVENT_SIZE_KB);
    TAxis* xaxis_event_size = hist_event_size->GetXaxis();
    xaxis_event_size->SetTitle("Event Sizes, KB");

    TH1D* hist_stream_ev = new TH1D("hist_avg_stream_ev", TString::Format("Event count per second for Period %d",period),
                                        500, MIN_EVENT_RATE, MAX_EVENT_RATE);
    TAxis* xaxis_stream_ev = hist_stream_ev->GetXaxis();
    xaxis_stream_ev->SetTitle("Event stream, event/sec");

    TH1D* hist_stream_mb = new TH1D("hist_avg_stream_mb", TString::Format("Megabytes per second for Period %d",period),
                                        500, MIN_EVENT_RATE, MAX_EVENT_RATE);
    TAxis* xaxis_stream_mb = hist_stream_mb->GetXaxis();
    xaxis_stream_mb->SetTitle("Binary stream, MB/sec");

    int correct_file_count = 0;
    int64_t sum_file_sec = 0, sum_event_count = 0;
    double sum_file_size_mb = 0, sum_event_size_kb = 0;
    map<TString, double> wrong_files;
    map<TString, double> wrong_events;
    map<TString, int> wrong_time;
    // cycle by all runs for the given period
    for (int i = 0; i < run_count; i++)
    {
        // get the current run
        UniRun* pRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == nullptr)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        // get all raw files for the current run
        TObjArray* raw_files = pRun->GetRawFiles();
        delete pRun;
        if (raw_files == nullptr) continue;

        // cycle by all raw files for the current run
        bool is_run_wrong = false;
        double sum_run_size_mb = 0;
        for (const auto& j : *raw_files)
        {
            int is_file_wrong = 0;
            UniRawFile* pRawFile = (UniRawFile*)j;

            // get file size, file duration in seconds and event count
            TDatime file_start_time = pRawFile->GetStartDatetime();
            TDatime file_end_time = pRawFile->GetEndDatetime();
            UInt_t file_duration_sec = 0, end_seconds = file_end_time.Convert(), start_seconds = file_start_time.Convert();
            if (end_seconds < start_seconds)
            {
                is_file_wrong = 1, is_run_wrong = true;
                file_duration_sec = start_seconds - end_seconds;
                wrong_time.insert(pair<TString, int>(pRawFile->GetFilePath(), -1*file_duration_sec));
            }
            else
                file_duration_sec = end_seconds - start_seconds;

            int64_t file_size_byte = pRawFile->GetFileSize();
            double file_size_mb = file_size_byte / 1024.0 / 1024.0;
            double file_size_gb = file_size_mb / 1024.0;
            int event_count = pRawFile->GetEventCount();
            double event_size_kb = file_size_byte / 1024.0 / event_count;
            if (file_size_gb > MAX_FILE_SIZE_GB)
            {
                is_file_wrong = 1, is_run_wrong = true;
                wrong_files.insert(pair<TString, double>(pRawFile->GetFilePath(), file_size_mb));
            }

            double events_per_second = event_count / (double) file_duration_sec;
            double mb_per_second = file_size_mb / (double) file_duration_sec;

            // do not take into account files with size < MIN_FILE_SIZE_MB
            if (file_size_mb < MIN_FILE_SIZE_MB)
            {
                is_file_wrong = 2, is_run_wrong = true;
                wrong_files.insert(pair<TString, double>(pRawFile->GetFilePath(), file_size_mb));
            }
            // do not take into account files with duration < MIN_FILE_DUR_SEC or > MIN_FILE_DUR_SEC
            else if ((file_duration_sec < MIN_FILE_DUR_SEC) || (file_duration_sec/60 > MAX_FILE_DUR_MIN))
            {
                is_file_wrong = 2, is_run_wrong = true;
                wrong_time.insert(pair<TString, double>(pRawFile->GetFilePath(), file_duration_sec));
            }
            // do not take into account files with event size < MIN_EVENT_SIZE_KB or > MAX_EVENT_SIZE_KB
            else if (((event_size_kb < MIN_EVENT_SIZE_KB)) || (event_size_kb > MAX_EVENT_SIZE_KB))
            {
                is_file_wrong = 2, is_run_wrong = true;
                wrong_events.insert(pair<TString, double>(pRawFile->GetFilePath(), event_size_kb));
            }
            else
            {
                sum_run_size_mb += file_size_mb;
                sum_file_size_mb += file_size_mb;
                sum_file_sec += file_duration_sec;
                sum_event_size_kb += event_size_kb;
                sum_event_count += event_count;
                correct_file_count++;
            }

            if (is_file_wrong == 1) cout<<orange;
            else if (is_file_wrong == 2) cout<<red;
            if (!(show_only_wrong && (is_file_wrong == 0)))
            {
                cout<<"Current file '"<<pRawFile->GetFilePath()<<"':"<<endl;
                cout<<"File size: "<<file_size_gb<<" GB (event size = "<<event_size_kb<<" KB)"<<endl;
                cout<<"Duration: "<<file_duration_sec<<" sec"<<endl;
                cout<<"Events/second: "<<events_per_second<<" ("<<mb_per_second<<" MB/s)"<<endl<<endl;
            }
            if (is_file_wrong > 0) cout<<reset;

            hist_file_size->Fill(file_size_gb);
            hist_file_duration->Fill(file_duration_sec);
            hist_event_size->Fill(event_size_kb);
            hist_stream_ev->Fill(events_per_second);
            hist_stream_mb->Fill(mb_per_second);
        }
        if (!(show_only_wrong && !is_run_wrong))
            cout<<"Run size for "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<": "<<sum_run_size_mb<<" MB"<<endl<<endl;
    }

    double average_file_size = sum_file_size_mb / correct_file_count;
    double average_file_duration = sum_file_sec / (double) correct_file_count;
    double average_event_size = sum_event_size_kb / correct_file_count;
    double average_event_stream = sum_event_count / (double) sum_file_sec;
    double average_mb_stream = sum_file_size_mb / sum_file_sec;
    cout<<cyan<<"-------------------------------------------------------"<<endl;
    cout<<"Average file size for period "<<period<<": "<<round(average_file_size)<<" MB"<<endl;
    cout<<"Average file duration for period "<<period<<": "<<round(average_file_duration)<<" sec"<<endl;
    cout<<"Average event size for period "<<period<<": "<<round(average_event_size)<<" KB"<<endl;
    cout<<"Average event stream for period "<<period<<": "<<round(average_event_stream)<<" event/sec"<<endl;
    cout<<"Average binary stream for period "<<period<<": "<<round(average_mb_stream)<<" MB/sec"<<endl;
    cout<<"-------------------------------------------------------"<<endl<<reset;

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
    if (wrong_time.size() > 0)
    {
        cout<<endl<<"Wrong file duration: (< "<<MIN_FILE_DUR_SEC<<" sec or > "<<MAX_FILE_DUR_MIN<<" minutes):"<<endl;
        for (auto it = wrong_time.begin(); it != wrong_time.end(); ++it)
            cout<<"File '"<<(*it).first<<": "<<(*it).second<<" seconds"<<endl;
        cout<<red;
        cout<<"Total wrong file duration: "<<wrong_time.size()<<endl;
        cout<<reset;
    }
    cout<<endl;

    delete run_numbers;

    TCanvas* canvas_size = new TCanvas("canvas_size","canvas_size",1200, 640);
    canvas_size->Divide(2, 1);
    canvas_size->cd(1);
    hist_file_size->Draw();
    canvas_size->cd(2);
    hist_event_size->Draw();
    TCanvas* canvas_file_duration = new TCanvas("canvas_file_duration","canvas_file_duration");
    canvas_file_duration->cd();
    hist_file_duration->Draw();
    TCanvas* canvas_stream = new TCanvas("canvas_stream","canvas_stream",1200, 640);
    canvas_stream->Divide(2, 1);
    canvas_stream->cd(1);
    hist_stream_ev->Draw();
    canvas_stream->cd(2);
    hist_stream_mb->Draw();

    cout<<"Macro finished successfully"<<endl;
}
