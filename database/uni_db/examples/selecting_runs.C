// macro for run selection by specified conditions
#include "../UniSearchCondition.h"

void selecting_runs()
{
    TObjArray arrayConditions;
    UniSearchCondition* searchCondition = new UniSearchCondition(UniSearchCondition::columnBeamParticle, UniSearchCondition::conditionEqual, TString("d"));
    arrayConditions.Add((TObject*)searchCondition);
    //searchCondition = new UniSearchCondition(UniSearchCondition::columnTargetParticle, UniSearchCondition::conditionNull);
    searchCondition = new UniSearchCondition(UniSearchCondition::columnTargetParticle, UniSearchCondition::conditionEqual, TString("Cu"));
    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* pRunArray = UniRun::Search(arrayConditions);

    // clean memory for conditions after search
    arrayConditions.Delete();

    // print run numbers and file path with raw data
    for (int i = 0; i < pRunArray->GetEntriesFast(); i++)
    {
        UniRun* pRun = (UniRun*) pRunArray->At(i);
        cout<<"Run (d-Cu): period - "<<pRun->GetPeriodNumber()<<", run - "<<pRun->GetRunNumber()<<endl;
    }

    // clean memory after work - deleting the array of runs
    delete pRunArray;

    cout << "\nMacro finished successfully" << endl;
}
