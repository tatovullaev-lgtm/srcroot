// macro for adding parameter value in JSON format
void put_parameter_value_json()
{
    /*
    UniParameter* pParameter = UniParameter::CreateParameter("my_json", JsonType, false);
    if (pParameter == nullptr)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }
    // clean memory after work
    delete pParameter;
    */

    JsonValue json_value; json_value.value = json::parse(R"({"simulation": true, "pi": 3.141})");
    UniDetectorParameter* pDetectorParameter = UniDetectorParameter::CreateDetectorParameter("DCH1", "my_json", 1, 77, 1, 77, &json_value);
    if (pDetectorParameter == nullptr)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }
    // clean memory after work
    delete pDetectorParameter;

    cout << "\nMacro finished successfully" << endl;
}
