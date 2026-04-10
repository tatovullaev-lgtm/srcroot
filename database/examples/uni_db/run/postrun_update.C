// changing raw file path according to the real directory, where they are located
int update_raw_file_path(const char* raw_file_directory = "/eos/nica/bmn/exp/raw/run9/beam/",
                         int period_number = 9,
                         int start_run = 1,
                         int end_run = 100000)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) {
        cout << "ERROR: connection to the UniConDa database failed" << endl;
        return -1;
    }

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format("update raw_file "
                                  "set file_path = $1 || regexp_replace(file_path, '^.+[/\\\\]', '') "
                                  "where period_number = $2 and run_number >= $3 and run_number <= $4");
    TSQLStatement* stmt = db_server->Statement(sql);

    // to remove incorrect raw files with duplicate paths (inserted before with incorrect parameters)
    // delete from raw_file where file_id in (select rf1.file_id  from raw_file rf1 cross join raw_file rf2
    //				                          where (rf1.file_id < rf2.file_id) and
    //(regexp_replace(rf1.file_path, '^.+[/\\]', '') = regexp_replace(rf2.file_path, '^.+[/\\]', '')))

    stmt->NextIteration();
    stmt->SetString(0, raw_file_directory);
    stmt->SetInt(1, period_number);
    stmt->SetInt(2, start_run);
    stmt->SetInt(3, end_run);

    // write correct value to the database
    if (!stmt->Process()) {
        cout << "ERROR: updating information on raw file paths failed" << endl;
        delete stmt;
        delete connDb;
        return -2;
    }
    delete stmt;
    delete connDb;

    return 0;
}

// correcting start and end times of the period/session according to the real first and last run
int update_run_period_time(int period_number, int start_run, int end_run)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) {
        cout << "ERROR: connection to the UniConDa database failed" << endl;
        return -1;
    }

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "update run_period set "
        "start_datetime = (select start_datetime from run_ where period_number = $1 and run_number = $2), "
        "end_datetime = (select end_datetime from run_ where period_number = $3 and run_number = $4) "
        "where period_number = $5");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, period_number);
    stmt->SetInt(1, start_run);
    stmt->SetInt(2, period_number);
    stmt->SetInt(3, end_run);
    stmt->SetInt(4, period_number);

    // write correct value to the database
    if (!stmt->Process()) {
        cout << "ERROR: updating information on the start and end time of the run period failed" << endl;
        delete stmt;
        delete connDb;
        return -2;
    }
    delete stmt;
    delete connDb;

    return 0;
}

// correcting detector parameter ranges according to the real first and last run numbers
// it separately changes both:
// start_period == period_number, old_start_run -> start_run (if prev_start_run != -1, then changes only old_start_run
// == prev_start_run) end_period == period_number old_end_run -> end_run (if prev_end_run != -1, then changes only
// old_end_run == prev_end_run)
int update_detector_paramater_range(int period_number,
                                    int start_run,
                                    int end_run,
                                    int prev_start_run = -1,
                                    int prev_end_run = -1)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) {
        cout << "ERROR: connection to the UniConDa database failed" << endl;
        return -1;
    }

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format("update detector_parameter "
                                  "set start_run = $1 "
                                  "where start_period = $2 and ($3 = -1 or $3 = start_run)");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, start_run);
    stmt->SetInt(1, period_number);
    stmt->SetInt(2, prev_start_run);

    // write correct value to the database
    if (!stmt->Process()) {
        cout << "ERROR: updating information on the start number of the detector parameters failed" << endl;
        delete stmt;
        delete connDb;
        return -2;
    }
    delete stmt;

    sql = TString::Format("update detector_parameter "
                          "set end_run = $1 "
                          "where end_period = $2 and ($3 = -1 or $3 = end_run)");

    stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, end_run);
    stmt->SetInt(1, period_number);
    stmt->SetInt(2, prev_end_run);

    // write correct value to the database
    if (!stmt->Process()) {
        cout << "ERROR: updating information on the end number of the detector parameters failed" << endl;
        delete stmt;
        delete connDb;
        return -3;
    }
    delete stmt;
    delete connDb;

    return 0;
}

// writing absent run metadata (beam, target, energy, field_voltage)
// from e-Log to the UniConDa database for the given run range
int update_run_metadata_from_elog(int period, int start_run, int end_run)
{
    // get all run numbers for the given period
    UniqueRunNumber* run_numbers;
    int run_count = UniRun::GetRunNumbers(run_numbers, period, start_run, period, end_run);
    if (run_count <= 0)
        return -1;
    cout << "run count = " << run_count << endl;

    // cycle by all runs for the given period
    for (int i = 0; i < run_count; i++) {
        // get the current run
        UniRun* pCurrentRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pCurrentRun == nullptr) {
            cout << "The function encountered with errors: no experimental run was found ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl;
            continue;
        }

        // get existing values (beam, target, energy, field, run_type) in UniConDa for the record
        TString run_beam = (pCurrentRun->GetBeamParticle()).Strip(TString::kBoth);
        TString* run_target = pCurrentRun->GetTargetParticle();
        double* run_energy = pCurrentRun->GetEnergy();
        double* run_field = pCurrentRun->GetFieldVoltage();
        int run_type = pCurrentRun->GetRunType();

        // if some of the values are empty then get them from e-Log
        if ((run_beam == "") || (run_energy == nullptr) || (run_field == nullptr)) {
            // cout<<endl<<"Period:Run   "<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<endl;
            TString elog_beam = "";
            TString* elog_target = nullptr;
            double* elog_energy = nullptr;
            int* elog_current = nullptr;
            int elog_type = -1;

            // get the corresonding e-Log records
            TObjArray* pRecords = ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number);
            // if not found then get e-Log information from the previous run
            if ((pRecords == nullptr) || (pRecords->GetEntriesFast() == 0))
                pRecords = ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number, true);
            TIter next(pRecords);
            ElogRecord* curRecord;
            while ((curRecord = (ElogRecord*)next()) != nullptr) {
                if (curRecord->GetBeam() != nullptr) {
                    TString tmp_elog_beam = (*(curRecord->GetBeam())).Strip(TString::kBoth);
                    if (tmp_elog_beam != "")
                        elog_beam = tmp_elog_beam;
                }
                if (curRecord->GetTarget() != nullptr) {
                    TString tmp_elog_target = (*(curRecord->GetTarget())).Strip(TString::kBoth);
                    if (tmp_elog_target != "")
                        elog_target = curRecord->GetTarget();
                }
                if (curRecord->GetEnergy() != nullptr)
                    elog_energy = curRecord->GetEnergy();
                if (curRecord->GetSp41() != nullptr)
                    elog_current = curRecord->GetSp41();
                int tmp_elog_type = curRecord->GetTypeId();
                if ((elog_type == -1) && (tmp_elog_type >= 1) && (tmp_elog_type <= 3))
                    elog_type = curRecord->GetTypeId();
            }

            // FILL DATA FROM E-LOG IF ABSENT
            // cout<<"run_beam = "<<run_beam<<" && elog_beam = "<<elog_beam<<endl
            //    <<"run_target = "<<(run_target == nullptr ? "null" : (*run_target))<<" && elog_target =
            //    "<<(elog_target == nullptr ? "null" : (*elog_target))<<endl
            //    <<"run_energy = "<<(run_energy == nullptr ? 0 : (*run_energy))<<" && elog_energy = "<<(elog_energy ==
            //    nullptr ? 0 : (*elog_energy))<<endl
            //    <<"run_field = "<<(run_field == nullptr ? 0 : (*run_field))<<" && elog_field = "<<(elog_current ==
            //    nullptr ? 0 : ((*elog_current)/900.0*55.87))<<endl;
            if ((run_beam == "") && (elog_beam != "")) {
                cout << "Period:Run " << run_numbers[i].period_number << ":" << run_numbers[i].run_number
                     << ". Recording beam particle... new value = " << elog_beam << " (old value = " << run_beam << ")"
                     << endl;
                pCurrentRun->SetBeamParticle(elog_beam);
                cout << "Period:Run " << run_numbers[i].period_number << ":" << run_numbers[i].run_number
                     << ". Recording target particle... new value = "
                     << (elog_target == nullptr ? "null" : (*elog_target))
                     << " (old value = " << (run_target == nullptr ? "null" : (*run_target)) << ")" << endl;
                pCurrentRun->SetTargetParticle(elog_target);
            }
            if ((run_energy == nullptr) && (elog_energy != nullptr)) {
                cout << "Period:Run " << run_numbers[i].period_number << ":" << run_numbers[i].run_number
                     << ". Recording beam energy... new value = " << *elog_energy
                     << " (old value = " << (run_energy == nullptr ? 0 : (*run_energy)) << ")" << endl;
                pCurrentRun->SetEnergy(elog_energy);
            }
            if ((run_field == nullptr) && (elog_current != nullptr)) {
                cout << "Period:Run " << run_numbers[i].period_number << ":" << run_numbers[i].run_number
                     << ". Recording field voltage... new value = " << ((*elog_current) / 900.0 * 55.87)
                     << " (old value = " << (run_field == nullptr ? 0 : (*run_field)) << ")" << endl;
                pCurrentRun->SetFieldVoltage(new double((*elog_current) / 900.0 * 55.87));
            }
            if ((elog_type != -1) && (run_type != elog_type)) {
                cout << "Period:Run " << run_numbers[i].period_number << ":" << run_numbers[i].run_number
                     << ". Recording run type... new value = " << elog_type << " (old value = " << run_type << ")"
                     << endl;
                pCurrentRun->SetRunType(elog_type);
            }
        }   // if ((run_beam == "") || (run_target == nullptr) || (run_energy == nullptr) || (run_field == nullptr))
    }   // for (int i = 0; i < run_count; i++)

    return 0;
}

// check that run metadata in the UniConDa database is the same as in the Electronic Logbook
int compare_uniconda_elog(int period, int start_run, int end_run, bool isChangeRunType = false, bool isShowInfo = false)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRun::GetRunNumbers(run_numbers, period, start_run, period, end_run);
    if (run_count <= 0)
        return -1;
    cout << "run count = " << run_count << endl;

    // cycle by all runs for the given period
    for (int i = 0; i < run_count; i++) {
        // get the current run info from the UniConDa database
        UniRun* pCurrentRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pCurrentRun == nullptr) {
            cout << "The function encountered with errors: no experimental run was found ("
                 << run_numbers[i].period_number << ":" << run_numbers[i].run_number << "). This run will be skipped!"
                 << endl;
            continue;
        }

        TString uniconda_beam = (pCurrentRun->GetBeamParticle()).Strip(TString::kBoth);
        TString* uniconda_target = pCurrentRun->GetTargetParticle();
        double* uniconda_energy = pCurrentRun->GetEnergy();
        // double* uniconda_field = pCurrentRun->GetFieldVoltage(); // it is already compared by another macro
        int uniconda_type = pCurrentRun->GetRunType();

        // get e-Log info
        TObjArray* arrayRecords = ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number);
        if (arrayRecords == nullptr) {
            cout << "The function encountered with errors: no e-Log record was found (" << run_numbers[i].period_number
                 << ":" << run_numbers[i].run_number << "). This run will be skipped!" << endl;
            continue;
        }
        TString *elog_beam = nullptr, *elog_target = nullptr;
        double* elog_energy = nullptr;
        int elog_type = -1;
        for (int j = 0; j < arrayRecords->GetEntriesFast(); j++) {
            ElogRecord* pRecord = (ElogRecord*)arrayRecords->At(j);
            if ((elog_beam == nullptr) && (pRecord->GetBeam() != nullptr)) {
                elog_beam = pRecord->GetBeam();
                elog_target = pRecord->GetTarget();
            }
            if ((elog_energy == nullptr) && (pRecord->GetEnergy() != nullptr))
                elog_energy = pRecord->GetEnergy();
            int tmp_elog_type = pRecord->GetTypeId();
            if ((elog_type == -1) && (tmp_elog_type >= 1) && (tmp_elog_type <= 3))
                elog_type = pRecord->GetTypeId();
        }

        // compare beam information in UniConDa and e-Log
        if (elog_beam != nullptr) {
            if (uniconda_beam.CompareTo(elog_beam->Strip(TString::kBoth), TString::kIgnoreCase) != 0)
                cout << "ERROR: different beam info found for run " << run_numbers[i].period_number << ":"
                     << run_numbers[i].run_number << " (" << uniconda_beam << " <> " << elog_beam << ")" << endl;
        } else {
            // compare beam value of the previous e-Log record with the UniConDa beam
            TString* elog_previous_beam = nullptr;
            TObjArray* arrayPreviousRecords =
                ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number, true);
            if (arrayPreviousRecords != nullptr) {
                for (int j = 0; j < arrayPreviousRecords->GetEntriesFast(); j++) {
                    ElogRecord* pRecord = (ElogRecord*)arrayPreviousRecords->At(j);
                    if (pRecord->GetBeam() != nullptr) {
                        elog_previous_beam = pRecord->GetBeam();
                        break;
                    }
                }
            }
            if ((elog_previous_beam != nullptr)
                and (uniconda_beam.CompareTo(elog_previous_beam->Strip(TString::kBoth), TString::kIgnoreCase) == 0))
            {
                if (isShowInfo)
                    cout << "INFO: the same beam information found in the previous e-Log record for run "
                         << run_numbers[i].period_number << ":" << run_numbers[i].run_number << endl;
            } else
                cout << endl
                     << "ERROR: no beam information found in e-Log for run " << run_numbers[i].period_number << ":"
                     << run_numbers[i].run_number << endl;
        }

        // compare target information in UniConDa and e-Log
        if (elog_beam != nullptr) {
            TString strUnicondaTarget = "no target", strElogTarget = "no target";
            if (uniconda_target != nullptr)
                strUnicondaTarget = *uniconda_target;
            if (elog_target != nullptr)
                strElogTarget = *elog_target;

            if (strUnicondaTarget.CompareTo(strElogTarget.Strip(TString::kBoth), TString::kIgnoreCase) != 0)
                cout << "ERROR: different target info found for run " << run_numbers[i].period_number << ":"
                     << run_numbers[i].run_number << " (" << strUnicondaTarget << " <> " << strElogTarget << ")"
                     << endl;
        } else {
            // compare target value of the previous e-Log record with the UniConDa target
            TString* elog_previous_target = nullptr;
            TObjArray* arrayPreviousRecords =
                ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number, true);
            if (arrayPreviousRecords != nullptr) {
                for (int j = 0; j < arrayPreviousRecords->GetEntriesFast(); j++) {
                    ElogRecord* pRecord = (ElogRecord*)arrayPreviousRecords->At(j);
                    if (pRecord->GetBeam() != nullptr) {
                        elog_previous_target = pRecord->GetTarget();
                        break;
                    }
                }
            }
            TString strUnicondaTarget = "no target", strElogTarget = "no target";
            if (uniconda_target != nullptr)
                strUnicondaTarget = *uniconda_target;
            if (elog_previous_target != nullptr)
                strElogTarget = *elog_previous_target;

            if (strUnicondaTarget.CompareTo(strElogTarget.Strip(TString::kBoth), TString::kIgnoreCase) == 0) {
                if (isShowInfo)
                    cout << "INFO: the same target information found in the previous e-Log record for run "
                         << run_numbers[i].period_number << ":" << run_numbers[i].run_number << endl;
            } else
                cout << "ERROR: different target info found for run " << run_numbers[i].period_number << ":"
                     << run_numbers[i].run_number << " (" << strUnicondaTarget << " <> " << strElogTarget << ")"
                     << endl;
        }

        // compare beam energy information in UniConDa and e-Log
        if (elog_energy != nullptr) {
            if (uniconda_energy == nullptr)
                cout << "ERROR: no beam energy information found in UniConDa for run " << run_numbers[i].period_number
                     << ":" << run_numbers[i].run_number << endl;
            else if (((*uniconda_energy) - (*elog_energy)) > 0.000001)
                cout << "ERROR: different beam energy found for run " << run_numbers[i].period_number << ":"
                     << run_numbers[i].run_number << " (" << *uniconda_energy << " <> " << *elog_energy << ")" << endl;
        } else {
            if (uniconda_energy == nullptr)
                cout << "WARNING: no beam energy information found in e-Log and UniConDa for run "
                     << run_numbers[i].period_number << ":" << run_numbers[i].run_number << endl;
            else {
                // compare beam energy value of the previous e-Log record with the UniConDa energy
                double* elog_previous_energy = nullptr;
                TObjArray* arrayPreviousRecords =
                    ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number, true);
                if (arrayPreviousRecords != nullptr) {
                    for (int j = 0; j < arrayPreviousRecords->GetEntriesFast(); j++) {
                        ElogRecord* pRecord = (ElogRecord*)arrayPreviousRecords->At(j);
                        if (pRecord->GetEnergy() != nullptr) {
                            elog_previous_energy = pRecord->GetEnergy();
                            break;
                        }
                    }
                }
                if ((elog_previous_energy != nullptr) and (((*uniconda_energy) - (*elog_previous_energy)) < 0.000001)) {
                    if (isShowInfo)
                        cout << "INFO: the same beam energy information found in the previous e-Log record for run "
                             << run_numbers[i].period_number << ":" << run_numbers[i].run_number << endl;
                } else
                    cout << "ERROR: no beam energy information found in e-Log for run " << run_numbers[i].period_number
                         << ":" << run_numbers[i].run_number << endl;
            }
        }

        // compare run type in UniConDa and e-Log
        if (elog_type != -1) {
            if (uniconda_type != elog_type) {
                cout << "ERROR: different run type found for run " << run_numbers[i].period_number << ":"
                     << run_numbers[i].run_number << " (" << uniconda_type << " <> " << elog_type << ")" << endl;
                // write value from e-Log to UniConDa
                if (isChangeRunType)
                    pCurrentRun->SetRunType(elog_type);
            }
        } else {
            // compare type value of the previous e-Log record with the UniConDa type
            int elog_previous_type = -1;
            TObjArray* arrayPreviousRecords =
                ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number, true);
            if (arrayPreviousRecords != nullptr) {
                for (int j = 0; j < arrayPreviousRecords->GetEntriesFast(); j++) {
                    ElogRecord* pRecord = (ElogRecord*)arrayPreviousRecords->At(j);
                    int tmp_previous_elog_type = pRecord->GetTypeId();
                    if ((tmp_previous_elog_type >= 1) && (tmp_previous_elog_type <= 3))
                        elog_previous_type = pRecord->GetTypeId();
                }
            }
            if (elog_previous_type != -1)
                if (uniconda_type == elog_previous_type) {
                    if (isShowInfo)
                        cout << "INFO: the run type found in the previous e-Log record for run "
                             << run_numbers[i].period_number << ":" << run_numbers[i].run_number << endl;
                } else {
                    cout << "ERROR: different run type found for run " << run_numbers[i].period_number << ":"
                         << run_numbers[i].run_number << " (" << uniconda_type << " <> " << elog_previous_type << ")"
                         << endl;
                    // write value from e-Log to UniConDa
                    if (isChangeRunType)
                        pCurrentRun->SetRunType(elog_previous_type);
                }
            else
                cout << "ERROR: no run type is defined in e-Log for run " << run_numbers[i].period_number << ":"
                     << run_numbers[i].run_number << endl;
        }
    }   // for (int i = 0; i < run_count; i++)

    return 0;
}

/**
 *  Main post run procedure to correct|update obtained metadata parameters
 **/
int postrun_update(int period_number = 9,
                   int start_run = 8968,
                   int end_run = 9094,
                   int prev_start_run = 1,
                   int prev_end_run = 100000)
{
    update_detector_paramater_range(period_number, start_run, end_run, prev_start_run, prev_end_run);
    update_run_period_time(period_number, start_run, end_run);
    update_run_metadata_from_elog(period_number, start_run, end_run);

    compare_uniconda_elog(period_number, start_run, end_run);

    return 0;
}
