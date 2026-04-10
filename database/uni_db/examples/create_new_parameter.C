// macro for creating new parameter (if parameter exist - you could check existing parameters by 'UniParameter::PrintAll()' function)
void create_new_parameter()
{
    // add 'voltage' parameter, for example (double value) :(TString parameter_name, int parameter_type, bool is_array)
    UniParameter* pParameter = UniParameter::CreateParameter("BD_global_mapping", TriggerMapType, false);
    if (pParameter == nullptr)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    // clean memory after work
    delete pParameter;

    cout << "\nMacro finished successfully" << endl;
}
