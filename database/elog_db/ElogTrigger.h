// ----------------------------------------------------------------------
//                    ElogTrigger header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** ElogTrigger.h
 ** Class for the table: trigger_ 
 **/ 

#ifndef ElogTrigger_H 
#define ElogTrigger_H 1 

#include "TString.h"
#include "TDatime.h"

#include "ElogConnection.h"

class ElogTrigger
{
 private:
    /* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    /// connection to the database
    ElogConnection* connectionDB;

    /// trigger id
    int i_trigger_id;
    /// trigger info
    TString str_trigger_info;

    //Constructor
    ElogTrigger(ElogConnection* db_connect, int trigger_id, TString trigger_info);
    /* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
    /* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    virtual ~ElogTrigger(); // Destructor

    // static class functions
    /// add new trigger to the database
    static ElogTrigger* CreateTrigger(TString trigger_info);
    /// get trigger from the database
    static ElogTrigger* GetTrigger(int trigger_id);
    /// get trigger from the database
    static ElogTrigger* GetTrigger(TString trigger_info);
    /// check trigger exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckTriggerExists(int trigger_id);
    /// check trigger exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckTriggerExists(TString trigger_info);
    /// delete trigger from the database
    static int DeleteTrigger(int trigger_id);
    /// delete trigger from the database
    static int DeleteTrigger(TString trigger_info);
    /// print all triggers
    static int PrintAll();

    // Getters
    /// get trigger id of the current trigger
    int GetTriggerId() {return i_trigger_id;}
    /// get trigger info of the current trigger
    TString GetTriggerInfo() {return str_trigger_info;}

    // Setters
    /// set trigger info of the current trigger
    int SetTriggerInfo(TString trigger_info);

    /// print information about current trigger
    void Print();
    /* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 ClassDef(ElogTrigger,1);
};

#endif
