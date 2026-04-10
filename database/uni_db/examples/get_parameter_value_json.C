// macro for getting parameter value in JSON format
void get_parameter_value_json()
{
    // get 'on' parameter value (boolean value)
    UniDetectorParameter* pDetectorParameter = UniDetectorParameter::GetDetectorParameter("DCH1", "my_json", 1, 77); //(detector_name, parameter_name, period_number, run_number)
    if (pDetectorParameter == nullptr)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    JsonValue* json_value = (JsonValue*) pDetectorParameter->GetValue();
    cout<<"JSON value for DCH1:my_json"<<endl<<json_value->value.dump(4)<<endl;

    // clean memory after work
    delete json_value;
    delete pDetectorParameter;

    cout << "\nMacro finished successfully" << endl;
}
