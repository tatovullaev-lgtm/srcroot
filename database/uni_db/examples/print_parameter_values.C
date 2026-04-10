// macro for printing parameter values for selected detector
void print_parameter_values(const char* pcDetectorName="")
{
    if (pcDetectorName[0] == '\0')
    {
        UniDetectorParameter::PrintAll();
        return;
    }

    TObjArray arrayConditions;
    UniSearchCondition* searchCondition = new UniSearchCondition(
                UniSearchCondition::columnDetectorName, UniSearchCondition::conditionEqual, TString(pcDetectorName));
    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* pParameterValueArray = UniDetectorParameter::Search(arrayConditions);

    // clean memory for conditions after search
    arrayConditions.Delete();

    // print run numbers and file path with raw data
    for (int i = 0; i < pParameterValueArray->GetEntriesFast(); i++)
    {
        UniDetectorParameter* pParameterValue = (UniDetectorParameter*) pParameterValueArray->At(i);

        cout<<"Parameter: name - "<<pParameterValue->GetParameterName()<<", parameter value - "<<pParameterValue->GetParameterValue()
            <<" (run "<<pParameterValue->GetStartRun()<<"-"<<pParameterValue->GetEndRun()<<")"<<endl;
    }

    // clean memory after work
    for (int i = 0; i < pParameterValueArray->GetEntriesFast(); i++)
        delete (UniRun*)pParameterValueArray->At(i);
    delete pParameterValueArray;

    cout << "\nMacro finished successfully" << endl;
}
