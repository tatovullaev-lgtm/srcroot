// Macro for printing TANGO data for GEM high voltage

// mod_2 top GEMs: vmon_1 - М6, vmon_2 - М10, vmon_3 - М7, vmon_4 - М9, vmon_5 - М5, vmon_6 - М8, vmon_7 - М11 (vmon_8 -
// small GEM) mod_0 bottom GEMs: vmon_1 - М16, vmon_2 - М15, vmon_3 - М14, vmon_4 - М12, vmon_5 - М18, vmon_6 - М13,
// vmon_7 - М17
vector<pair<const char*, const char*>> map_channel_run8 = {{"mod_2", "vmon_1"},
                                                           {"mod_2", "vmon_2"},
                                                           {"mod_2", "vmon_3"},
                                                           {"mod_2", "vmon_4"},
                                                           {"mod_2", "vmon_5"},
                                                           {"mod_2", "vmon_6"},
                                                           {"mod_2", "vmon_7"},
                                                           {"mod_0", "vmon_1"},
                                                           {"mod_0", "vmon_2"},
                                                           {"mod_0", "vmon_3"},
                                                           {"mod_0", "vmon_4"},
                                                           {"mod_0", "vmon_5"},
                                                           {"mod_0", "vmon_6"},
                                                           {"mod_0", "vmon_7"}};

// for datetime range
void tango_gem_u(const char* start_date = "2016-12-21 12:20:00", const char* end_date = "2016-12-21 12:40:00")
{
    int start_period = UniRunPeriod::GetPeriodNumber(start_date);
    if (start_period < 1) {
        cout << "ERROR: start date is not in any run period of the experiment" << endl;
        exit(-1);
    }
    int end_period = UniRunPeriod::GetPeriodNumber(end_date);
    if (end_period < 1) {
        cout << "ERROR: end date is not in any run period of the experiment" << endl;
        exit(-2);
    }
    if (start_period != end_period) {
        cout << "ERROR: the time interval must be in one run period of the experiment" << endl;
        exit(-3);
    }

    if (start_period >= 8) {
        switch (start_period) {
            case 8: {
                const char* detector_name = "gem";
                vector<TangoData*> vecTangoData;
                for (const auto& current_channel : map_channel_run8) {
                    // cout<<"detector_name = "<<detector_name<<", parameter name = "<<current_channel.second<<", member
                    // name = "<<current_channel.first<<endl;
                    //  TangoData stores array of TangoTimeParameter* (parameter's values) for all time points
                    TangoData* db_tango = new TangoData(
                        start_date, end_date, detector_name, current_channel.second, current_channel.first);
                    vecTangoData.push_back(db_tango);
                    if (db_tango->GetTangoData() == nullptr) {
                        cout << "Macro finished with errors" << endl;
                        exit(-4);
                    }
                }

                // console output
                TangoData::PrintTangoArrayConsole(vecTangoData);
                // graph output
                TangoData::PrintTangoArrayMultiGraph(vecTangoData);

                for (const auto& tango_data : vecTangoData)
                    delete tango_data;
                vecTangoData.clear();

                break;
            }
            default: {
                cout << "Period " << start_period << " is not supported" << endl;
                exit(-5);
            }
        }
    } else {
        const char* detector_name = "gem";
        const char* parameter_name = "u";

        // TangoData stores array of TangoTimeParameter* (parameter's values) for all time points
        TangoData db_tango(start_date, end_date, detector_name, parameter_name);
        if (db_tango.GetTangoData() == nullptr) {
            cout << "Macro finished with errors" << endl;
            exit(-6);
        }

        // console output
        db_tango.PrintTangoDataConsole();

        // graph output
        // db_tango.PrintTangoDataSurface(tango_data);
        db_tango.PrintTangoDataMultiGraph();   //, "U", true);
    }

    cout << "Macro finished successfully" << endl;
}

// for a given run
void tango_gem_u(int period, int run)
{
    // get run time
    UniRun* pRun = UniRun::GetRun(period, run);
    if (pRun == nullptr) {
        cout << "Macro finished with errors: no experimental run was found for period = " << period << ", run = " << run
             << endl;
        exit(-1);
    }

    TString strStartDate = pRun->GetStartDatetime().AsSQLString();
    TDatime* endDate = pRun->GetEndDatetime();
    if (endDate == nullptr) {
        cout << "Macro finished with errors: no end datetime in the database for period = " << period
             << ", run = " << run << endl;
        delete pRun;
        exit(-2);
    }
    TString strEndDate = endDate->AsSQLString();
    delete pRun;

    if (period >= 8) {
        switch (period) {
            case 8: {
                const char* detector_name = "gem";
                vector<TangoData*> vecTangoData;
                for (const auto& current_channel : map_channel_run8) {
                    // cout<<"detector_name = "<<detector_name<<", parameter name = "<<current_channel.second<<", member
                    // name = "<<current_channel.first<<endl;
                    //  TangoData stores array of TangoTimeParameter* (parameter's values) for all time points
                    TangoData* db_tango = new TangoData(strStartDate.Data(),
                                                        strEndDate.Data(),
                                                        detector_name,
                                                        current_channel.second,
                                                        current_channel.first);
                    vecTangoData.push_back(db_tango);
                    if (db_tango->GetTangoData() == nullptr) {
                        cout << "Macro finished with errors" << endl;
                        exit(-3);
                    }
                }

                // console output
                TangoData::PrintTangoArrayConsole(vecTangoData);
                // graph output
                TangoData::PrintTangoArrayMultiGraph(vecTangoData);

                for (const auto& tango_data : vecTangoData)
                    delete tango_data;
                vecTangoData.clear();

                break;
            }
            default: {
                cout << "Period " << period << " is not supported" << endl;
                exit(-4);
            }
        }
    } else {
        const char* detector_name = "gem";
        const char* parameter_name = "u";

        // TangoData stores array of TangoTimeParameter* (parameter's values) for all time points
        TangoData db_tango(strStartDate.Data(), strEndDate.Data(), detector_name, parameter_name);
        if (db_tango.GetTangoData() == nullptr) {
            cout << "Macro finished with errors" << endl;
            exit(-5);
        }

        // console output
        db_tango.PrintTangoDataConsole();

        // graph output
        // db_tango.PrintTangoDataSurface();
        db_tango.PrintTangoDataMultiGraph();
    }

    cout << "Macro finished successfully" << endl;
}
