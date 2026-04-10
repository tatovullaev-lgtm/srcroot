// macro for run selection by specified conditions
void selecting_parameters()
{
    TObjArray conditions;
    conditions.Add((TObject*) (new UniSearchCondition(UniSearchCondition::columnStartPeriod, UniSearchCondition::conditionLessOrEqual, 4)));
    conditions.Add((TObject*) (new UniSearchCondition(UniSearchCondition::columnStartRun, UniSearchCondition::conditionLessOrEqual, 83)));
    conditions.Add((TObject*) (new UniSearchCondition(UniSearchCondition::columnEndPeriod, UniSearchCondition::conditionGreaterOrEqual, 4)));
    conditions.Add((TObject*) (new UniSearchCondition(UniSearchCondition::columnEndRun, UniSearchCondition::conditionGreaterOrEqual, 83)));
    conditions.Add((TObject*) (new UniSearchCondition(UniSearchCondition::columnDetectorName, UniSearchCondition::conditionEqual, TString("TOF1"))));

    TObjArray* pValuesArray = UniDetectorParameter::Search(conditions);

    // clean memory for conditions after search
    conditions.Delete();

    cout<<"The number of parameter values is "<<pValuesArray->GetEntries()<<endl;

    TIter res_it(pValuesArray);
    UniDetectorParameter* curRow;
    while ((curRow = (UniDetectorParameter*)res_it()) != nullptr)
        curRow->Print();

    // clean memory after work - deleting the array of parameter values
    delete pValuesArray;

    cout << "\nMacro finished successfully" << endl;
}
