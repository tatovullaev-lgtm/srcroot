// macro for ELOG record selection by specified conditions
void selecting_records()
{
    TObjArray arrayConditions;
    ElogSearchCondition* searchCondition = new ElogSearchCondition(ElogSearchCondition::columnPeriodNumber, ElogSearchCondition::conditionEqual, 6);
    arrayConditions.Add((TObject*)searchCondition);
    searchCondition = new ElogSearchCondition(ElogSearchCondition::columnTargetParticle, ElogSearchCondition::conditionEqual, TString("Cu"));
    arrayConditions.Add((TObject*)searchCondition);
    searchCondition = new ElogSearchCondition(ElogSearchCondition::columnSp41, ElogSearchCondition::conditionGreaterOrEqual, 1200);
    arrayConditions.Add((TObject*)searchCondition);
    searchCondition = new ElogSearchCondition(ElogSearchCondition::columnSp41, ElogSearchCondition::conditionLessOrEqual, 1800);
    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* pRecordArray = ElogRecord::Search(arrayConditions);

    // clean memory for conditions after search
    arrayConditions.Delete();

    // print run numbers and file path with raw data
    for (int i = 0; i < pRecordArray->GetEntriesFast(); i++)
    {
        ElogRecord* pRecord = (ElogRecord*) pRecordArray->At(i);
        cout<<"Record: time - "<<pRecord->GetRecordDate().AsSQLString()<<", period - "<<(pRecord->GetPeriodNumber() == nullptr ? -1 : *pRecord->GetPeriodNumber())
            <<", run - "<<(pRecord->GetRunNumber() == nullptr ? -1 : *pRecord->GetRunNumber())<<", SP-41 field - "<<(pRecord->GetSp41() == nullptr ? -1 : *pRecord->GetSp41())<<"A"<<endl;
    }

    // clean memory after work - deleting the array of runs
    delete pRecordArray;

    cout << "\nMacro finished successfully" << endl;
}
