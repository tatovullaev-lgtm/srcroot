TString get_trigger(int period_number = 7, int run_number = 4608)
{
    TString strTrigger = "";
    TObjArray* pRecords = ElogRecord::GetRecords(period_number, run_number);

    TIter next(pRecords);
    ElogRecord* curRecord;
    while ((curRecord = (ElogRecord*)next()) != nullptr)
    {
        if (curRecord->GetTriggerId() != nullptr)
            strTrigger = ElogTrigger::GetTrigger(*(curRecord->GetTriggerId()))->GetTriggerInfo();
    }

    if (strTrigger == "") cout<<"Trigger information was not found"<<endl;
    else cout<<"Trigger for run "<<period_number<<":"<<run_number<<" = "<<strTrigger<<endl;

    return strTrigger;
}
