#define PARITY 2

// matching event numbers and summary count in 'raw file' records with run metadata, as well as time intervals
// possible errors: starting not from 1, more than 1 event between raw files, sum of the event count of the raw files <> event count of the corresponding run
// time of the first and last raw files are not corresponding start and end time of the run
int check_event_and_time(int period_number, int start_run, int end_run)
{
    // get the given period
    UniRunPeriod* pCurrentPeriod = UniRunPeriod::GetRunPeriod(period_number);
    if (pCurrentPeriod == nullptr)
    {
        cout<<"The function encountered with errors: no experimental period was found with number "<<period_number<<endl;
        return -1;
    }

    TDatime period_start_time = pCurrentPeriod->GetStartDatetime();
    TDatime* period_end_time = pCurrentPeriod->GetEndDatetime();
    delete pCurrentPeriod;
    if (period_end_time == nullptr)
    {
        cout<<"ERROR: there is no end time for Period "<<period_number<<"Exiting..."<<endl;
        return -2;
    }

    UniqueRunNumber* run_numbers;
    int run_count = UniRun::GetRunNumbers(run_numbers, period_number, start_run, period_number, end_run);
    if (run_count <= 0)
        return -1;

    // cycle by all runs for the given period
    int critical_errors = 0, no_event_count_run = 0, out_start_run_time = 0, out_end_run_time = 0, no_end_time_run = 0, no_raw_files_run = 0,
        out_start_raw_time = 0, out_end_raw_time = 0, first_event_gt1 = 0, event_gap = 0, parity_event_gap = 0,
        event_sum_mismatch = 0, time_interval_mismatch = 0;
    for (int i = 0; i < run_count; i++)
    {
        bool is_success = true;

        // get the current run
        UniRun* pCurrentRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pCurrentRun == nullptr)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            critical_errors++;
            continue;
        }
        cout<<endl<<"Period:Run   "<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<endl;

        int* run_event_count = pCurrentRun-> GetEventCount();
        if (run_event_count == nullptr)
        {
            is_success = false;
            no_event_count_run++;
            cout<<"ERROR: there is no event count in the run"<<endl;
        }

        TDatime run_start_time = pCurrentRun->GetStartDatetime();
        if ((run_start_time < period_start_time) || (run_start_time > (*period_end_time)))
        {
            is_success = false;
            out_start_run_time++;
            cout<<"ERROR: the start time of the run is out of range: "<<run_start_time.AsSQLString()<<endl;
        }
        TDatime* run_end_time = pCurrentRun->GetEndDatetime();
        if (run_end_time == nullptr)
        {
            is_success = false;
            no_end_time_run++;
            cout<<"ERROR: there is no end time of the run"<<endl;
        }
        else
        {
            if (((*run_end_time) < period_start_time) || ((*run_end_time) > (*period_end_time)))
            {
                is_success = false;
                out_end_run_time++;
                cout<<"ERROR: the end time of the run is out of range: "<<run_end_time->AsSQLString()<<endl;
            }
        }

        // get all raw files for the current run
        TObjArray* raw_files = pCurrentRun->GetRawFiles();
        delete pCurrentRun;
        if (raw_files == nullptr)
        {
            no_raw_files_run++;
            cout<<"ERROR: there are no raw files in the run"<<endl;
            continue;
        }

        // cycle by all raw files for the current run
        int sum_event_count = 0;
        TDatime min_start_time("2100-01-01 00:00:00"), max_end_time("1995-01-01 00:00:00");
        vector<int> v_start_event, v_end_event;
        for (const auto& j : *raw_files)
        {
            UniRawFile* pRawFile = (UniRawFile*)j;

            int file_start_event = pRawFile->GetStartEvent();
            int file_end_event = pRawFile->GetEndEvent();
            int file_event_count = pRawFile->GetEventCount();
            if (pRawFile->GetStartDatetime() < min_start_time) min_start_time = pRawFile->GetStartDatetime();
            if (pRawFile->GetEndDatetime() > max_end_time) max_end_time = pRawFile->GetEndDatetime();

            // check the start time of the current raw file
            if ((pRawFile->GetStartDatetime() < period_start_time) || (pRawFile->GetStartDatetime() > (*period_end_time)))
            {
                is_success = false;
                out_start_raw_time++;
                cout<<"ERROR: the start time of the raw file is out of range: "<<pRawFile->GetStartDatetime().AsSQLString()<<endl;
            }

            // check the emd time of the current raw file
            if ((pRawFile->GetEndDatetime() < period_start_time) || (pRawFile->GetEndDatetime() > (*period_end_time)))
            {
                is_success = false;
                out_end_raw_time++;
                cout<<"ERROR: the end time of the raw file is out of range: "<<pRawFile->GetEndDatetime().AsSQLString()<<endl;
            }

            // check first event
            if (v_start_event.empty() && (file_start_event != 1))
            {
                is_success = false;
                first_event_gt1++;
                cout<<"ERROR: first event > 1 (first_event = "<<file_start_event<<")"<<endl;
            }

            // check "previous" event: "current start - "parity previous" end == PARITY"
            if (v_start_event.size() > PARITY)
            {
                int prev_end_event = v_end_event[v_end_event.size()-PARITY];
                int event_gap = file_start_event - prev_end_event;
                if (event_gap != PARITY)
                {
                    is_success = false;
                    event_gap++;
                    cout<<"ERROR: event gap > "<<PARITY<<" (current_event = "<<file_start_event<<", previous event = "<<prev_end_event<<endl;
                }
            }

            // check parity event (should be increased by 1)
            if ((PARITY > 1) && (v_start_event.size() % PARITY != 0))
            {
                int parity_start_event = v_start_event[v_start_event.size()-1];
                int parity_gap = file_start_event - parity_start_event;
                if (parity_gap != 1)
                {
                    is_success = false;
                    parity_event_gap++;
                    cout<<"ERROR: parity event gap > 1 (current_event = "<<file_start_event<<", parity start event = "<<parity_start_event<<endl;
                }
            }

            sum_event_count += file_event_count;

            v_start_event.push_back(file_start_event);
            v_end_event.push_back(file_end_event);
        }//for (const auto& j : *raw_files)

        if ((run_event_count != nullptr) && ((*run_event_count) != sum_event_count))
        {
            is_success = false;
            event_sum_mismatch++;
            cout<<"ERROR: raw event sum is different from run event count ("<<sum_event_count<<" <> "<<*run_event_count<<")"<<endl;
        }
        if ((run_end_time != nullptr) && ((run_start_time != min_start_time) || ((*run_end_time) != max_end_time)))
        {
            is_success = false;
            //TString time_format("%d.%m.%Y %H:%M:%S");
            time_interval_mismatch++;
            cout<<"ERROR: run time interval ("<<run_start_time.AsSQLString()<<" - "<<run_end_time->AsSQLString()
                <<") is different from raw file time ("<<min_start_time.AsSQLString()<<" - "<<max_end_time.AsSQLString()<<")"<<endl;
        }

        if (is_success) cout<<"Event numbers and time intervals were successfully matched"<<endl;
    }//for (int i = 0; i < run_count; i++)

    cout<<endl;
    if (critical_errors > 0) cout<<"Critical Errors: "<<critical_errors<<endl;
    if (no_event_count_run > 0) cout<<"No event count in "<<no_event_count_run<<" run(s)"<<endl;
    if (out_start_run_time > 0) cout<<"The start time of the run is out of range in "<<out_start_run_time<<" run(s)"<<endl;
    if (no_end_time_run > 0) cout<<"There is no end time in "<<no_end_time_run<<" run(s)"<<endl;
    if (out_end_run_time > 0) cout<<"The end time of the run is out of range in "<<out_end_run_time<<" run(s)"<<endl;
    if (no_raw_files_run > 0) cout<<"No raw files in "<<no_raw_files_run<<" run(s)"<<endl;
    if (out_start_raw_time > 0) cout<<"The start time of the raw file is out of range in "<<out_start_raw_time<<" run(s)"<<endl;
    if (out_end_raw_time > 0) cout<<"The end time of the raw file is out of range in "<<out_end_raw_time<<" run(s)"<<endl;
    if (first_event_gt1 > 0) cout<<"First event > 1 in "<<first_event_gt1<<" run(s)"<<endl;
    if (event_gap > 0) cout<<"Event gap > 1 in "<<event_gap<<" run(s)"<<endl;
    if (parity_event_gap > 0) cout<<"Parity event gap > 1 in "<<parity_event_gap<<" run(s)"<<endl;
    if (event_sum_mismatch > 0) cout<<"Raw event sum is different from run event count in "<<event_sum_mismatch<<" run(s)"<<endl;
    if (time_interval_mismatch > 0) cout<<"Run time interval is different from raw file time in "<<time_interval_mismatch<<" run(s)"<<endl;

    return 0;
}

/**
 *  Main post run checking procedure to match run metadata and parameters of the raw files
**/
int postrun_check(int period_number = 8, int start_run = 6310, int end_run = 8427)
{
    check_event_and_time(period_number, start_run, end_run);

    return 0;
}
