// # define RUN_1_7
//  for RUN 1 to 7
#ifdef RUN_1_7
const char* system_name = "bmn";
const char* parameter_name = "ch1";
// for RUN greater than 7
#else
const char* system_name = "magnet";
const char* parameter_name = "ch_1";
#endif

/**
 *  Function to calculate and show average magnetic field values (voltage, mV) from Tango for runs of the given
 *period/session to UniConDa Parameters: period_number - period/session number start_run - number of the first run of
 *the interval end_run - number of the end run of the interval isShowElog - whether show also corresponding e-Log values
 *  isOnlyDifferent - show average magnetic field values from Tango only for different e-Log values (current, A)
 **/
int show_avg_field(int period_number = 9,
                   int start_run = 8977,
                   int end_run = 9094,
                   bool isShowElog = true,
                   bool isOnlyDifferent = true)
{
    // get the given period
    UniRunPeriod* pCurrentPeriod = UniRunPeriod::GetRunPeriod(period_number);
    if (pCurrentPeriod == nullptr) {
        cout << "The function encountered with errors: no experimental period was found with number " << period_number
             << endl;
        return -1;
    }
    const string red("\033[0;91m"), orange("\033[0;33m"), cyan("\033[0;96m"), reset("\033[0m");

    TDatime period_start_time = pCurrentPeriod->GetStartDatetime();
    TDatime* period_end_time = pCurrentPeriod->GetEndDatetime();
    delete pCurrentPeriod;
    if (period_end_time == nullptr) {
        cout << "ERROR: there is no end time for Period " << period_number << "Exiting..." << endl;
        return -2;
    }

    UniqueRunNumber* run_numbers;
    int run_count = UniRun::GetRunNumbers(run_numbers, period_number, start_run, period_number, end_run);
    if (run_count <= 0)
        return -1;

    int out_start_run_time = 0, no_end_time_run = 0, out_end_run_time = 0, no_field_elog = 0, diff_count = 0;
    for (int i = 0; i < run_count; i++) {
        // get run time
        UniRun* pRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == nullptr) {
            cout << "The function encountered with errors: no experimental run was found ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl
                 << endl;
            continue;
        }

        TDatime startDate = pRun->GetStartDatetime();
        TString strStartDate = startDate.AsSQLString();
        if ((startDate < period_start_time) || (startDate > (*period_end_time))) {
            out_start_run_time++;
            cout << "ERROR: the start time of the run is out of range: " << startDate.AsSQLString() << endl;
            delete pRun;
            continue;
        }

        TDatime* endDate = pRun->GetEndDatetime();
        if (endDate == nullptr) {
            no_end_time_run++;
            cout << "The function encountered with errors: no end datetime in the database for the run ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl
                 << endl;
            delete pRun;
            continue;
        }
        if (((*endDate) < period_start_time) || ((*endDate) > (*period_end_time))) {
            out_end_run_time++;
            cout << "ERROR: the end time of the run is out of range: " << endDate->AsSQLString() << endl;
            delete endDate;
            delete pRun;
            continue;
        }
        TString strEndDate = endDate->AsSQLString();
        delete endDate;
        delete pRun;

        // calculate average magnetic field (voltage, mV) from the Tango database
        TangoData db_tango(strStartDate.Data(), strEndDate.Data(), system_name, parameter_name);   //, "", 1);
        if (db_tango.GetTangoData() == nullptr) {
            cout << "The function encountered with errors: result Tango data is null (" << run_numbers[i].period_number
                 << ":" << run_numbers[i].run_number << "). This run will be skipped!" << endl;
            continue;
        }

        vector<double> vec_average = db_tango.GetAverageTangoData();
        if (vec_average.empty()) {
            cout << "The function encountered with errors: Tango data is empty or average value is wrong ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl
                 << endl;
            continue;
        }
        size_t data_count = db_tango.GetTangoData()->GetEntriesFast();

        double average_field = vec_average[0];
        double average_current = average_field * 900 / 55.87;
        double average_coeff = average_field / 55.87;

        if (!isShowElog) {
            cout << "Run " << run_numbers[i].period_number << "-" << run_numbers[i].run_number
                 << " (number of time counts: " << data_count << ")" << endl;
            cout << "Average magnetic field from Tango: " << average_field << " mV" << endl;
            cout << "Calculated current: " << average_current << " A (ratio: " << average_coeff << ")" << endl << endl;
            continue;
        }

        // get e-Log info
        TObjArray* arrayRecords = ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number);
        if (arrayRecords == nullptr) {
            cout << "The function encountered with errors: no e-Log record was found (" << run_numbers[i].period_number
                 << ":" << run_numbers[i].run_number << "). This run will be skipped!" << endl
                 << endl;
            continue;
        }
        int* pField = nullptr;
        for (int j = 0; j < arrayRecords->GetEntriesFast(); j++) {
            ElogRecord* pRecord = (ElogRecord*)arrayRecords->At(j);
            pField = pRecord->GetSp41();
            if (pField != nullptr)
                break;
        }

        if (pField == nullptr) {
            no_field_elog++;
            cout << "The function encountered with errors: no SP-41 field set in e-Log ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl
                 << endl;
            delete arrayRecords;
            continue;
        }

        double elog_voltage = (*pField) * 55.87 / 900;
        delete arrayRecords;

        // compare ELog magnet field and average field from Tango database
        bool isDifferent = false;
        if (fabs(average_field - elog_voltage) > 5)
            isDifferent = true;
        if ((isOnlyDifferent) && (!isDifferent)) {
            delete pField;
            continue;
        }

        cout << "Run " << run_numbers[i].period_number << "-" << run_numbers[i].run_number << endl;
        cout << "Average Tango magnetic field: " << average_field << " mV (" << data_count << " counts)." << endl;
        cout << "e-Log magnetic field: " << elog_voltage << " mV (" << (*pField) << " A)" << endl;
        if (fabs(average_field - elog_voltage) > 5) {
            diff_count++;
            cout << "ERROR: e-Log and Tango magnetic fields differ by more than 5 mV!" << endl;
        }
        cout << endl;

        delete pField;
    }

    cout << endl << red;
    if (out_start_run_time > 0)
        cout << "The start time of the run is out of range in " << out_start_run_time << " run(s)" << endl;
    if (no_end_time_run > 0)
        cout << "There is no end time in " << no_end_time_run << " run(s)" << endl;
    if (out_end_run_time > 0)
        cout << "The end time of the run is out of range in " << out_end_run_time << " run(s)" << endl;
    if (no_field_elog > 0)
        cout << "There is no field value (e-Log) in " << no_field_elog << " run(s)" << endl;
    if (diff_count > 0)
        cout << "e-Log and Tango magnetic fields differ in " << diff_count << " run(s)" << endl;
    cout << reset;

    delete run_numbers;
    cout << "Macro finished successfully" << endl;
    return 0;
}

/**
 *  Function to display magnetic field for a given time interval corresponding to the specified run range
 *  end_period == -1 means "end_period = start_period"
 *  end_run == -1 means "end_run = start_run"
 **/
int show_field_graph(int start_period = 8, int start_run = 6866, int end_period = -1, int end_run = -1)
{
    // get time of the begin run
    UniRun* pStartRun = UniRun::GetRun(start_period, start_run);
    if (pStartRun == nullptr) {
        cout << "Macro finished with errors: no experimental run was found - " << start_period << ":" << start_run
             << " (period:run)" << endl;
        return -1;
    }

    TDatime startDate = pStartRun->GetStartDatetime();
    TString strStartDate = startDate.AsSQLString();

    TDatime* endDate = nullptr;
    if ((end_period == -1) && (end_run == -1))
        endDate = pStartRun->GetEndDatetime();
    else {
        if (end_period == -1)
            end_period = start_period;

        // get time of the end run
        UniRun* pEndRun = UniRun::GetRun(end_period, end_run);
        if (pEndRun == nullptr) {
            cout << "Macro finished with errors: no experimental run was found - " << end_period << ":" << end_run
                 << " (period:run)" << endl;
            delete pStartRun;
            return -1;
        }
        endDate = pEndRun->GetEndDatetime();
        delete pEndRun;
    }

    if (endDate == nullptr) {
        cout << "Macro finished with errors: no end datetime in the database for this run" << endl;
        delete pStartRun;
        return -2;
    }
    TString strEndDate = endDate->AsSQLString();
    cout << endl << "Time interval: " << strStartDate << " - " << strEndDate << endl;
    delete pStartRun;

    // get Tango data
    TangoData db_tango(strStartDate.Data(), strEndDate.Data(), system_name, parameter_name);   //, "", 1);
    if (db_tango.GetTangoData() == nullptr) {
        cout << "Macro finished with errors: return Tango data is null" << endl;
        return -3;
    }

    vector<double> vec_average = db_tango.GetAverageTangoData();
    if (!vec_average.empty()) {
        size_t data_count = db_tango.GetTangoData()->GetEntriesFast();

        double average_field = vec_average[0];
        double average_current = average_field * 900 / 55.87;
        double average_coeff = average_field / 55.87;

        cout << "Average magnetic field from Tango for the interval: " << average_field << " mV"
             << " (number of counts: " << data_count << ")" << endl;
        cout << "Calculated current: " << average_current << " A (ratio: " << average_coeff << ")" << endl << endl;
    }

    // print
    db_tango.PrintTangoDataMultiGraph("hall sensor voltage, mV");

    return 0;
}

// additional function for displaying  magnetic field for the given time interval
int show_field_graph(const char* start_time, const char* end_time)
{
    TDatime startDate(start_time);
    TString strStartDate = startDate.AsSQLString();
    TDatime endDate(end_time);
    TString strEndDate = endDate.AsSQLString();
    // cout<<"strStartDate = "<<strStartDate<<", strEndDate = "<<strEndDate<<endl;

    // get Tango data
    TangoData db_tango(strStartDate.Data(), strEndDate.Data(), system_name, parameter_name);
    if (db_tango.GetTangoData() == nullptr) {
        cout << "Macro finished with errors: return Tango data is null" << endl;
        return -3;
    }

    vector<double> vec_average = db_tango.GetAverageTangoData();
    if (!vec_average.empty()) {
        size_t data_count = db_tango.GetTangoData()->GetEntriesFast();

        double average_field = vec_average[0];
        double average_current = average_field * 900 / 55.87;
        double average_coeff = average_field / 55.87;

        cout << "Average magnetic field from TangoDB for the interval: " << average_field
             << " mV. Calculated current: " << average_current << " A. Ratio: " << average_coeff
             << ". Number of counts: " << data_count << endl;
    }

    // print
    db_tango.PrintTangoDataMultiGraph("hall sensor voltage, mV");

    return 0;
}

/**
 * Function to calculate and write Tango average magnetic field values for runs of the given period/session to UniConDa
 **/
void write_avg_field_db(int period_number = 9,
                        int start_run = 8977,
                        int end_run = 9094,
                        bool is_rewrite = true,
                        float allowed_tolerance_pct = 10)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRun::GetRunNumbers(run_numbers, period_number, start_run, period_number, end_run);
    if (run_count <= 0)
        return;

    int error_count = 0;
    for (int i = 0; i < run_count; i++) {
        // get run time
        UniRun* pRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == nullptr) {
            cout << "The function encountered with errors: no experimental run was found ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl;
            continue;
        }

        // get start and end time of the run
        TDatime startDate = pRun->GetStartDatetime();
        TString strStartDate = startDate.AsSQLString();
        TDatime* endDate = pRun->GetEndDatetime();
        if (endDate == nullptr) {
            cout << "The function encountered with errors: no end datetime in the database ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl;
            delete pRun;
            continue;
        }
        TString strEndDate = endDate->AsSQLString();
        // if difference between start and end time is less than 2 seconds than skip
        if ((endDate->Convert() - startDate.Convert()) < 2) {
            cout << "The function encountered with errors: run duration <= 1 seconds (" << run_numbers[i].period_number
                 << ":" << run_numbers[i].run_number << "). This run will be skipped!" << endl;
            delete pRun;
            continue;
        }

        TangoData db_tango(strStartDate.Data(), strEndDate.Data(), system_name, parameter_name);
        if (db_tango.GetTangoData() == nullptr) {
            cout << "The function encountered with errors: return data is null (" << run_numbers[i].period_number << ":"
                 << run_numbers[i].run_number << "). This run will be skipped!" << endl;
            continue;
        }

        vector<double> vec_average = db_tango.GetAverageTangoData();
        if (vec_average.empty()) {
            cout << "The function encountered with errors: Tango data is empty or average value is wrong ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl;
            continue;
        }

        double average_field = vec_average[0];
        size_t data_count = db_tango.GetTangoData()->GetEntriesFast();

        cout << "Average magnetic field for run " << run_numbers[i].period_number << "-" << run_numbers[i].run_number
             << ": " << average_field << " mV (" << data_count << " counts)." << endl;

        // write average magnetic field value to the UniConDa run
        if (pRun->GetFieldVoltage() == nullptr) {
            pRun->SetFieldVoltage(&average_field);
            cout << "Average magnetic field has been written to UniConDa" << endl << endl;
        } else if (is_rewrite) {
            double cur_voltage_value = *pRun->GetFieldVoltage();
            // if cur_voltage value == 0 then check that average_field is really small than they corresponds
            if ((cur_voltage_value == 0) and (average_field < 0.6))
                cur_voltage_value = average_field;
            double cur_tolerance_pct = std::abs(cur_voltage_value - average_field) / cur_voltage_value * 100;   // in %
            if (cur_tolerance_pct > allowed_tolerance_pct) {
                cout << "ERROR: admissible tolerance (" << allowed_tolerance_pct
                     << "%) is exceeded, the current voltage value = " << cur_voltage_value << " mV, run period from "
                     << strStartDate.Data() << " to " << strEndDate.Data() << endl
                     << endl;
                error_count++;
            } else {
                if (cur_tolerance_pct > 0.001)
                    cout << "ATTENTION: the current voltage value (" << cur_voltage_value
                         << " mV) will be rewritten with the new one within acceptable tolerance ("
                         << allowed_tolerance_pct << "%)" << endl;
                pRun->SetFieldVoltage(&average_field);
                cout << "Average magnetic field has been written to UniConDa" << endl << endl;
            }
        }

        delete pRun;
    }

    if (error_count)
        cout << "The count of exceeding the tolerance (skipped runs): " << error_count << endl;

    delete run_numbers;
    cout << "Macro finished successfully" << endl;
}

// additional function to compare magnetic field value (current, A) from e-Log database
// and average magnetic field (voltage, mV) from UniCOnDa (saved before from Tango)
int compare_avg_field(int period_number = 9, int start_run = 8977, int end_run = 9094)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRun::GetRunNumbers(run_numbers, period_number, start_run, period_number, end_run);
    if (run_count <= 0)
        return -1;
    const string red("\033[0;91m"), orange("\033[0;33m"), cyan("\033[0;96m"), reset("\033[0m");

    int no_elog_record = 0, no_field_elog = 0, no_field_uniconda = 0;
    vector<double> x, y;
    bool* run_error = new bool[run_count];
    for (int i = 0; i < run_count; i++) {
        run_error[i] = 0;

        // get e-Log info
        TObjArray* arrayRecords = ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number);
        if (arrayRecords == nullptr) {
            no_elog_record++;
            run_error[i] = 1;
            cout << "The function encountered with errors: no e-Log record was found (" << run_numbers[i].period_number
                 << ":" << run_numbers[i].run_number << "). This run will be skipped!" << endl
                 << endl;
            continue;
        }

        int* pField = nullptr;
        for (int j = 0; j < arrayRecords->GetEntriesFast(); j++) {
            ElogRecord* pRecord = (ElogRecord*)arrayRecords->At(j);
            pField = pRecord->GetSp41();
            if (pField != nullptr)
                break;
        }
        if (pField == nullptr) {
            no_field_elog++;
            run_error[i] = 1;
            cout << "The function encountered with errors: no SP-41 field was set in e-Log ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl
                 << endl;
            delete arrayRecords;
            continue;
        }
        double elog_voltage = (*pField) * 55.87 / 900;   // 0,062077778
        delete arrayRecords;

        // get UniConDa average field for run
        UniRun* pRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == nullptr) {
            if (run_error[i] == 1)
                run_error[i] = 3;
            else
                run_error[i] = 2;
            cout << "The function encountered with errors: no experimental run was found ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl
                 << endl;
            continue;
        }
        double* avg_field = pRun->GetFieldVoltage();
        delete pRun;
        if (avg_field == nullptr) {
            no_field_uniconda++;
            if (run_error[i] == 1)
                run_error[i] = 3;
            else
                run_error[i] = 2;
            cout << "The function encountered with errors: no magnet field in the BM@N database ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl
                 << endl;
            continue;
        }
        double average_field = *avg_field;

        cout << "Run " << run_numbers[i].period_number << ":" << run_numbers[i].run_number << endl;
        cout << "Magnetic field current from e-Log: " << (*pField) << " A, " << elog_voltage << " mV" << endl;
        cout << "Average magnetic field voltage from Tango: " << average_field << " mV" << endl << endl;

        x.push_back(*pField);
        y.push_back(average_field);
    }

    // canvas includes "U/I dependence" graph containing Utango values vs Ielog values
    TCanvas* c1 = new TCanvas("c1", "U/I dependence", 100, 70, 700, 500);
    TGraph* gr = new TGraph(x.size(), x.data(), y.data());
    gr->SetTitle("");
    gr->GetXaxis()->SetTitle("Ielog, A");
    gr->GetYaxis()->SetTitle("Utango, mV");
    gr->Draw("AP*");

    TFitResultPtr fit_result = gr->Fit("pol1", "S");
    // TMatrixDSym cov = fit_result->GetCovarianceMatrix();
    // Double_t chi2   = fit_result->Chi2();
    Double_t par0 = fit_result->Value(0);
    Double_t par1 = fit_result->Value(1);
    Double_t err0 = fit_result->Error(0);
    Double_t err1 = fit_result->Error(1);
    cout << "p1 = " << par1 << ". p1_error = " << err1 << ". p0 = " << par0 << ". p0_error = " << err0 << endl;

    // calculate residuals for all runs
    double* x2 = new double[run_count];
    double* y2 = new double[run_count];
    int iter = 0;
    for (int i = 0; i < run_count; i++) {
        if (run_error[i] == 0) {
            x2[i] = i;
            y2[i] = y[iter] - par1 * x[iter] - par0;
            iter++;
        } else {
            x2[i] = i;
            y2[i] = 0;
        }
    }

    TCanvas* c2 = new TCanvas("c2", "The search for wrong field values", 200, 250, 1600, 600);
    TGraph* grN = new TGraph(run_count, x2, y2);
    grN->SetTitle("");
    grN->GetXaxis()->SetTitle("run number");
    grN->GetYaxis()->SetTitle("#DeltaU, mV");
    grN->SetMarkerStyle(2);
    grN->GetXaxis()->Set(run_count, 0, run_count - 1);

    for (int i = 0; i < run_count; i++) {
        if (run_error[i] != 0)
            grN->GetXaxis()->SetBinLabel(i + 1, (TString::Format("Error %d", run_numbers[i].run_number).Data()));
        else if ((y2[i] > 7) || (y2[i] < -7))
            grN->GetXaxis()->SetBinLabel(i + 1, (TString::Format("%d", run_numbers[i].run_number).Data()));
    }

    grN->Draw("AP");

    cout << endl << red;
    if (no_elog_record > 0)
        cout << "No e-Log record was found in " << no_elog_record << " run(s)" << endl;
    if (no_field_elog > 0)
        cout << "There is no field value (e-Log) in " << no_field_elog << " run(s)" << endl;
    if (no_field_uniconda > 0)
        cout << "There is no field value (UniConDa) in " << no_field_uniconda << " run(s)" << endl;
    // if (diff_count > 0) cout<<"e-Log and Tango magnetic fields differ in "<<diff_count<<" run(s)"<<endl;
    cout << reset;

    return 0;
}
