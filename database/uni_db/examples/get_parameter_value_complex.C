// macro for getting parameter value (if parameter exist - you could check existing parameters by 'UniParameter::PrintAll()' function)
void get_parameter_value_complex()
{
    bool return_error = false;

    // get noise parameter values presented by IIStructure: Int+Int (slot:channel)
    UniDetectorParameter* pDetectorParameter = UniDetectorParameter::GetDetectorParameter("DCH1", "noise", 1, 133);
    if (pDetectorParameter != nullptr)
    {
        vector<UniValue*> pValues;
        pDetectorParameter->GetValue(pValues);

        // YOUR CODE (e.g print values)
        for (int i = 0; i < pValues.size(); i++)
            cout<<"Slot:Channel "<<((IIValue*)pValues[i])->value1<<":"<<((IIValue*)pValues[i])->value2<<endl;

        // clean memory after work
        for (int i = 0; i < pValues.size(); i++) delete pValues[i];
        if (pDetectorParameter)
            delete pDetectorParameter;
    }
    else
        return_error = true;

    if (return_error)
        cout<<"\nMacro finished with errors"<<endl;
    else
        cout<<"\nMacro finished successfully"<<endl;
}

// macro for getting alignment value for the given period and run number
void get_parameter_alignment(int period_number = 7, int run_number = 4668)
{
    bool return_error = false;

    // get noise parameter values presented by IIStructure: Int+Int (slot:channel)
    UniDetectorParameter* pDetectorParameter = UniDetectorParameter::GetDetectorParameter("gem", "alignment_shift", period_number, run_number);
    if (pDetectorParameter != nullptr)
    {
        vector<UniValue*> pValues;
        pDetectorParameter->GetValue(pValues);

        for (int i = 0; i < pValues.size(); i++)
        {
            AlignmentValue* pValue = (AlignmentValue*) pValues[i];
            cout<<"Station:Module = "<<(unsigned)pValue->station<<":"<<(unsigned)pValue->module<<endl;
            cout<<"shiftX = "<<pValue->value[0]<<", shiftY = "<<pValue->value[1]<<", shiftZ = "<<pValue->value[2]<<endl<<endl;
        }

        // clean memory after work
        for (int i = 0; i < pValues.size(); i++) delete pValues[i];
        if (pDetectorParameter)
            delete pDetectorParameter;
    }
    else
        return_error = true;

    if (return_error)
        cout<<"Macro finished with errors"<<endl;
    else
        cout<<"Macro finished successfully"<<endl;
}
