// function to calculate hash values for files whose paths stored in the Database
// returns the success code: 0 - without errors, error_code - otherwise
int write_checksum_db(int period_number, bool isSkipExisted = true)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRunPeriod::GetRunNumbers(run_numbers, period_number), iter_count = 0;
    if (run_count <= 0)
    {
        cout<<"ERROR: runs do not exist in the Database for period "<<period_number<<endl;
        return -1;
    }

    bool isError = false;
    for (int i = 0; i < run_count; i++)
    {
        // get run time
        UniRun* pRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == nullptr)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            isError = true;
            continue;
        }
        if ((isSkipExisted) && (pRun->GetFileHash() != nullptr)) continue;

        TString strFilePath = pRun->GetFilePath();
        gSystem->ExpandPathName(strFilePath);

        if (gSystem->AccessPathName(strFilePath.Data()))
        {
            isError = true;
            cout<<"ERROR: raw file does not exist: "<<strFilePath<<endl;
        }
        else
        {
            TString strFileHash = UniRawFile::CalculateFileHash(raw_root_file);
            if (strFileHash == "")
            {
                isError = true;
                cout<<"ERROR: a problem occured while opening file: "<<strFilePath<<endl;
                continue;
            }
            cout<<"File: "<<strFilePath<<". File checksum = "<<strFileHash<<endl;
            int res_code = pRun->SetFileHash(&strFileHash);
            if (res_code != 0) isError = true;
            else iter_count++;
        }
    }

    if (isError) cout<<endl<<"There are some errors occured!"<<endl;
    cout<<"Checksums were written for "<<iter_count<<" of "<<run_count<<" files"<<endl;

    return 0;
}

int write_sim_checksum_db(bool isSkipExisted = true)
{
    TObjArray* simulation_files = UniSimulationFile::GetSimulationFiles();

    bool isError = false;
    int file_count = simulation_files->GetEntriesFast(), iter_count = 0;
    for (int i = 0; i < file_count; i++)
    {
        UniSimulationFile* pFile = (UniSimulationFile*) simulation_files->At(i);
        if ((isSkipExisted) && (pFile->GetFileHash() != nullptr)) continue;

        TString strFilePath = pFile->GetFilePath();
        gSystem->ExpandPathName(strFilePath);

        if (gSystem->AccessPathName(strFilePath.Data()))
        {
            isError = true;
            cout<<"ERROR: simulation file does not exist: "<<strFilePath<<endl;
        }
        else
        {
            TString strFileHash = UniRawFile::CalculateFileHash(raw_root_file);
            if (strFileHash == "")
            {
                isError = true;
                cout<<"ERROR: a problem occured while opening file: "<<strFilePath<<endl;
                continue;
            }
            cout<<"File: "<<strFilePath<<". File checksum = "<<strFileHash<<endl;
            int res_code = pRun->SetFileHash(&strFileHash);
            if (res_code != 0) isError = true;
            else iter_count++;
        }
    }

    // clean memory after work - deleting the array of simulation files
    simulation_files->Delete();
    delete simulation_files;

    if (isError) cout<<endl<<"There are some errors occured!"<<endl;
    cout<<"Checksums were written for "<<iter_count<<" of "<<file_count<<" files"<<endl;

    return 0;
}
