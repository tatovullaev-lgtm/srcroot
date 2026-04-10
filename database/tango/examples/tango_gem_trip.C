// macro for printing TANGO data for GEM stations tripped in runs

// map: № Tango channel -> GEMs Z-order (starts from 0)
vector<int> map_channel_run5{1, 3, 0, 5, 2, 6, 4, 7};
vector<int> map_channel_run6_b1529{1, 3, 0, 5, 2, 6, 4};
vector<int> map_channel_run6_a1529{1, 3, 0, 5, 6, 4, 2};
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

void tango_gem_trip(int period = 6, int run = 1529, bool isShowOnlyTrip = false)
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
        const char* parameter_name = "trip";
        vector<int>* map_channel = nullptr;
        switch (period) {
            case 5:
                map_channel = &map_channel_run5;
                break;
            case 6: {
                if (run < 1529)
                    map_channel = &map_channel_run6_b1529;
                else
                    map_channel = &map_channel_run6_a1529;
                break;
            }
            default:
                cout << "Macro finished with errors: no channel map was found for the period " << period << endl;
                exit(-3);
        }

        TangoData::enumConditions condition = TangoData::conditionEqual;
        bool condition_value = 1;

        TangoData db_tango(strStartDate.Data(), strEndDate.Data(), detector_name, parameter_name);
        if (db_tango.GetTangoData() == nullptr) {
            cout << "Macro finished with errors" << endl;
            exit(-1);
        }

        // TObjArray stores array of TangoTimeIntervals ((for all channels)): time intervals satisfying the condition
        // ((TObjArray*)tango_data->At(0))->At(2) -> TangoTimeInterval*: third time interval satisfying the condition
        // for the first channel 0
        TObjArray* tango_intervals = db_tango.SearchTangoIntervals(condition, condition_value, map_channel);
        if (tango_intervals == nullptr) {
            cout << "Macro finished with errors" << endl;
            exit(-4);
        }

        // console output
        cout << "Information on " << parameter_name << " parameter for " << detector_name << " (period = " << period
             << ", run = " << run << "):" << endl;
        db_tango.PrintTangoIntervalConsole(tango_intervals, "GEM Z-order", isShowOnlyTrip);
        cout << endl;

        delete tango_intervals;
    }
}

void tango_gem_trip(int period, bool isShowOnlyTrip = false)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRunPeriod::GetRunNumbers(run_numbers, period);
    if (run_count <= 0)
        return;

    for (int i = 0; i < run_count; i++)
        tango_gem_trip(run_numbers[i].period_number, run_numbers[i].run_number, isShowOnlyTrip);

    cout << "Macro finished successfully" << endl;
}
