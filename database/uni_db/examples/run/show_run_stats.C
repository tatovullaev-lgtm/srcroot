// limits to define out-of-range values and to exclude them while calculating the averages
#define MIN_RUN_DUR_SEC   6
#define MAX_RUN_DUR_HOUR  3
#define MIN_EVENT_RATE   100
#define MAX_EVENT_RATE   3600

class UniqueRunNumber;
int fill_run_diff_hist(TH1D*& hist_run_diff, int period, int start_run, int end_run);

/**
 * Function for creating statistics histograms on run duration and average stream (event/sec)
 * for runs stored in the UniConDa Database for a specified Run/Period (or run interval)
*/
void show_run_stats(int period = 8, int start_run = 6587, int end_run = -1, bool show_only_wrong = false)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRun::GetRunNumbers(run_numbers, period, start_run, period, end_run);
    if (run_count <= 0)
        return;

    //bool isTerminal = isatty(fileno(stdout));
    const string red("\033[0;91m"), orange("\033[0;33m"), cyan("\033[0;96m"), reset("\033[0m");

    TH1D* hist_run_duration = new TH1D("hist_run_duration",  TString::Format("Run duration for Period %d", period),
                                       MAX_RUN_DUR_HOUR*60, 0, MAX_RUN_DUR_HOUR*60);
    TAxis* xaxis_run_duration = hist_run_duration->GetXaxis();
    xaxis_run_duration->SetTitle("Run Duration, min");

    TH1D* hist_stream_ev = new TH1D("hist_stream_ev", TString::Format("Event count per second for Period %d",period),
                                        500, MIN_EVENT_RATE, MAX_EVENT_RATE);
    TAxis* xaxis_stream_ev = hist_stream_ev->GetXaxis();
    xaxis_stream_ev->SetTitle("Event stream, event/sec");

    int correct_run_count = 0;
    int64_t sum_run_sec = 0, sum_event_count = 0;
    map<int, int> wrong_time;
    for (int i = 0; i < run_count; i++)
    {
        // get the current run
        UniRun* pRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == nullptr)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        bool is_run_wrong = false;
        // get run duration and event count
        TDatime run_start_time = pRun->GetStartDatetime();
        TDatime* run_end_time = pRun->GetEndDatetime();
        if (run_end_time == nullptr)
        {
            cout<<"The function encountered with errors: end time of the run was not defined for "<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<endl;
            delete pRun;
            continue;
        }
        UInt_t run_duration_sec = 0, end_seconds = (*run_end_time).Convert(), start_seconds = run_start_time.Convert();
        if (end_seconds < start_seconds)
        {
            is_run_wrong = true;
            run_duration_sec = start_seconds - end_seconds;
            wrong_time.insert(pair<int, int>(run_numbers[i].run_number, -1*run_duration_sec));
        }
        else
            run_duration_sec = end_seconds - start_seconds;

        int* p_event_count = pRun->GetEventCount();
        delete pRun;
        if (p_event_count == nullptr)
        {
            cout<<"The function encountered with errors: event count was not defined for "<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<endl;
            continue;
        }
        int event_count = *p_event_count;

        double events_per_second = event_count / (double) run_duration_sec;

        // do not take into account runs with duration < MIN_RUN_DUR_SEC or > MAX_RUN_DUR_HOUR
        if ((run_duration_sec < MIN_RUN_DUR_SEC) || (run_duration_sec/3600 > MAX_RUN_DUR_HOUR))
        {
            is_run_wrong = true;
            wrong_time.insert(pair<int, int>(run_numbers[i].run_number, run_duration_sec));
        }
        else
        {
            sum_run_sec += run_duration_sec;
            sum_event_count += event_count;
            correct_run_count++;
        }

        if (is_run_wrong) cout<<red;
        if (!(show_only_wrong && !is_run_wrong))
        {
            cout<<"Current run "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<":"<<endl;
            cout<<"Duration: "<<run_duration_sec<<" sec"<<endl;
            cout<<"Events/second: "<<events_per_second<<endl<<endl;
        }
        if (is_run_wrong) cout<<reset;

        hist_run_duration->Fill(run_duration_sec/60.0);
        hist_stream_ev->Fill(events_per_second);
    }

    double average_run_duration = sum_run_sec / (double) correct_run_count;
    double average_event_stream = sum_event_count / (double) sum_run_sec;
    cout<<cyan<<"-------------------------------------------------------"<<endl;
    cout<<"Average run duration for period "<<period<<": "<<round(average_run_duration)<<" sec ("<<TString::Format("%.2f",average_run_duration/60)<<" min)"<<endl;
    cout<<"Average event stream for period "<<period<<": "<<round(average_event_stream)<<" event/sec"<<endl;
    cout<<"-------------------------------------------------------"<<endl<<reset;

    if (wrong_time.size() > 0)
    {
        cout<<endl<<"Wrong run duration:"<<endl;
        for (auto it = wrong_time.begin(); it != wrong_time.end(); ++it)
            cout<<"Run "<<period<<"-"<<(*it).first<<": "<<(*it).second<<" seconds"<<endl;
        cout<<red;
        cout<<"Total wrong run duration: "<<wrong_time.size()<<endl;
        cout<<reset;
    }
    cout<<endl;

    delete run_numbers;

    TCanvas* canvas_run = new TCanvas("canvas_run","canvas_run",1200, 640);
    canvas_run->Divide(2, 1);
    canvas_run->cd(1);
    hist_run_duration->Draw();
    canvas_run->cd(2);
    hist_stream_ev->Draw();

    TCanvas* canvas_run_diff = new TCanvas("canvas_run_diff","canvas_run_diff");
    TH1D* hist_run_diff = new TH1D("hist_run_diff", TString::Format("Run time gap for Period %d",period),
                                   1000, 0, 3*3600);
    TAxis* xaxis_run_diff = hist_run_diff->GetXaxis();
    xaxis_run_diff->SetTitle("Run Time Gaps, sec");
    fill_run_diff_hist(hist_run_diff, period, start_run, end_run);
    canvas_run_diff->cd();
    hist_run_diff->Draw();

    cout<<"Macro finished successfully"<<endl;
}

int fill_run_diff_hist(TH1D*& hist_run_diff, int period, int start_run, int end_run)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select r1.run_number, EXTRACT(EPOCH FROM (r2.start_datetime - r1.end_datetime)) "
        "from run_ r1 cross join lateral (select start_datetime from run_ rin where r1.period_number = rin.period_number and r1.run_number+1 = rin.run_number) r2 "
        "where period_number = %d AND run_number >= %d AND (run_number <= %d OR -1 = %d)",
        period, start_run, end_run, end_run);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get time differences for the closest runs (between runs with difference in run_numbers being equal to 1)
    if (!stmt->Process())
    {
        cout<<"ERROR: getting time differences for the closest runs from the database has been failed"<<endl;
        delete stmt;
        delete connDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();
    while (stmt->NextResultRow())
    {
        UniConnection* connRun = UniConnection::Open();
        if (connRun == nullptr)
        {
            cout<<"ERROR: the connection to the UniConDa Database failed"<<endl;
            delete stmt;
            delete connDb;
            return -3;
        }

        int diff_sec = stmt->GetInt(1);
        if (diff_sec > 0) hist_run_diff->Fill(diff_sec);
    }

    delete stmt;
    delete connDb;

    return 0;
}
