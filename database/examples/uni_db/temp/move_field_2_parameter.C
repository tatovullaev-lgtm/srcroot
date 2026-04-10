// macro for moving magnetic field voltage from table RUN_ to parameter values
class UniqueRunNumber;
void move_field_2_parameter(int period)
{
    UniqueRunNumber* run_numbers = nullptr;
    int run_count = UniRun::GetRunNumbers(run_numbers, period, 1, period, 100000);
    if (run_count <= 0)
        return;

    const char* detector_name = "magnet";
    const char* parameter_name = "field_voltage";

    for (int i = 0; i < run_count; i++)
    {
        // get run time
        UniRun* pRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == nullptr)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        double* field_voltage = pRun->GetFieldVoltage();
        if (field_voltage == nullptr)
        {
            cout<<"No field current in the database for this run ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            delete pRun;
            continue;
        }
        delete pRun;

        // check if magnet exists in the database
        if (!UniDetector::CheckDetectorExists(detector_name))
        {
            UniDetector* pDetector = UniDetector::CreateDetector(detector_name, nullptr);
            if (pDetector == nullptr)
            {
                cout << "\nMacro finished with errors" << endl;
                return;
            }

            cout<<"The magnet was added to detectors in the database"<<endl;
            delete pDetector;
        }

        // check if 'field_current' parameter exists in the database
        if (!UniParameter::CheckParameterExists(parameter_name))
        {
            UniParameter* pParameter = UniParameter::CreateParameter(parameter_name, DoubleType);
            if (pParameter == nullptr)
            {
                cout << "\nMacro finished with errors" << endl;
                return;
            }

            cout<<"The 'average_field_mv' parameter was added to the database"<<endl;
            delete pParameter;
        }

        // writing magnetic field current as parameter to the Unified Condition Database
        DoubleValue dValue; dValue.value = *field_voltage;
        UniDetectorParameter* pDetectorParameter = UniDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,
                                                     run_numbers[i].period_number, run_numbers[i].run_number, run_numbers[i].period_number, run_numbers[i].run_number, &dValue);
        if (pDetectorParameter == nullptr)
        {
            cout << "\nMacro finished with errors" << endl;
            return;
        }

        cout<<"Field current was written to the database ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<")"<<endl;
        delete pDetectorParameter;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}

void print_parameter(int period)
{
    UniqueRunNumber* run_numbers = nullptr;
    int run_count = UniRun::GetRunNumbers(run_numbers, period, 1, period, 100000);
    if (run_count <= 0)
        return;

    const char* detector_name = "magnet";
    const char* parameter_name = "field_current";

    for (int i = 0; i < run_count; i++)
    {
        UniDetectorParameter* pDetectorParameter = UniDetectorParameter::GetDetectorParameter(detector_name, parameter_name,
                                                     run_numbers[i].period_number, run_numbers[i].run_number);
        if (pDetectorParameter == nullptr)
        {
            cout<<"No field current was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<")"<<endl;
            continue;
        }

        cout<<"Field current for run "<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<" "<<((DoubleValue*)pDetectorParameter->GetValue())->value<<endl;
        delete pDetectorParameter;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}
