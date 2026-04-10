#include "../../function_set.h"

// function for checking existence of the files whose paths stored in the Database
// returns the success code: 0 - without errors, error_code - otherwise
int check_db_paths_exist(int period_number = 8)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRunPeriod::GetRunNumbers(run_numbers, period_number);
    if (run_count <= 0)
    {
        cout<<"ERROR: There are no runs in the UniConDa Database for period "<<period_number<<endl;
        return -1;
    }

    bool isAllIn = true;
    for (int i = 0; i < run_count; i++)
    {
        // get run time
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
        for (const auto& j : *raw_files)
        {
            UniRawFile* pRawFile = (UniRawFile*)j;

            TString strFilePath = pRawFile->GetFilePath();
            gSystem->ExpandPathName(strFilePath);

            if (gSystem->AccessPathName(strFilePath.Data()))
            {
                isAllIn = false;
                cout<<"ATTENTION: The current raw file does not exist: "<<strFilePath<<endl;
            }
        }
    }

    if (isAllIn) cout<<endl<<"All run paths exist"<<endl;
    else cout<<endl<<"There are absent run paths!"<<endl;

    return 0;
}
