/**
 *  Macro for writing a link to the existed ROOT setup geometry for a selecting run period to the UniConDa Database
 *  In case of geometry_link == -1 then get geometry from the previous run
 *  Example: root 'write_geometry_to_db.C(45, 7, 1, 7, 10000)'
**/
void write_geometry_to_db(int geometry_link, int start_period, int start_run, int end_period, int end_run)
{
    if (geometry_link < -1)
    {
        cout<<"\nERROR: geometry link should be positive or be equal -1 to get geometry from the previous run"<<endl;
        return;
    }
    if (geometry_link == -1)
    {
        int prev_period = start_period, prev_run = start_run;
        int result_code = UniRun::FindPreviousRun(prev_period, prev_run);
        if (result_code != 0)
            return;

        UniRun* pPreviosRun = UniRun::GetRun(prev_period, prev_run);
        if (pPreviosRun == nullptr)
        {
            cout<<"ERROR: getting of previous run "<<prev_period<<":"<<prev_run<<" (period:number) was failed"<<endl;
            return;
        }

        if (pPreviosRun->GetGeometryId() == nullptr)
        {
            cout<<"ERROR: no geometry id was found in the prevoius run "<<prev_period<<":"<<prev_run<<" (period:number)"<<endl;
            delete pPreviosRun;
            return;
        }
        geometry_link = *(pPreviosRun->GetGeometryId());

        delete pPreviosRun;
    }

    UniqueRunNumber* pUniqueRuns = nullptr;
    int run_count = UniRun::GetRunNumbers(pUniqueRuns, start_period, start_run, end_period, end_run);
    if (run_count < 0)
    {
        cout<<"\nERROR: No runs were found in the given range"<<endl;
        return;
    }
    int res_code = 0;
    for (int i = 0; i < run_count; i++)
    {
        cout<<"Setting link to geometry for run_period:run   "<<pUniqueRuns[i].period_number<<":"<<pUniqueRuns[i].run_number<<"..."<<endl;

        UniRun* pCurRun = UniRun::GetRun(pUniqueRuns[i].period_number, pUniqueRuns[i].run_number);
        if (pCurRun == nullptr)
        {
            cout<<"ERROR: getting of run "<<pUniqueRuns[i].period_number<<":"<<pUniqueRuns[i].run_number<<" (period:number) was failed"<<endl;
            res_code = -1;
            continue;
        }

        pCurRun->SetGeometryId(new int(geometry_link));

        delete pCurRun;
    }

    delete [] pUniqueRuns;
    if (res_code != 0)
    {
        cout<<"\nMacro '"<<gSystem->BaseName(gInterpreter->GetCurrentMacroName())<<"' finished with errors"<<endl;
        return;;
    }

    cout<<"\nMacro '"<<gSystem->BaseName(gInterpreter->GetCurrentMacroName())<<"' finished successfully"<<endl;
}

/**
 *  Macro for creating and writing a ROOT file with a full setup geometry to the UniConDa Database
 *  and writing links for the given runs (from start to end run)
 *  Example: root 'write_geometry_to_db.C("$VMCWORKDIR/macro/run/geometry_run/geometry_src_run7.root", 7, 1, 7, 3600)'
**/
void write_geometry_to_db(const char* root_file_path, int start_period, int start_run, int end_period, int end_run)
{
    // write ROOT file with detector geometry for run range
    int res_code = UniRun::WriteGeometryFile(start_period, start_run, end_period, end_run, root_file_path); //(int start_period, int start_run, int end_period, int end_run, char* geo_file_path)
    if (res_code != 0)
    {
        cout<<"\nMacro '"<<gSystem->BaseName(gInterpreter->GetCurrentMacroName())<<"' finished with errors"<<endl;
        exit(-1);
    }

    cout<<"\nMacro '"<<gSystem->BaseName(gInterpreter->GetCurrentMacroName())<<"' finished successfully"<<endl;
}
