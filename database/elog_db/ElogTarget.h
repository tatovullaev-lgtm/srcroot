// ----------------------------------------------------------------------
//                    ElogTarget header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** ElogTarget.h
 ** Class for the table: target_ 
 **/ 

#ifndef ElogTarget_H 
#define ElogTarget_H 1 

#include "TString.h"
#include "TDatime.h"

#include "ElogConnection.h"

class ElogTarget
{
 private:
    /* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    /// connection to the database
    ElogConnection* connectionDB;

    /// target
    TString str_target;

    //Constructor
    ElogTarget(ElogConnection* db_connect, TString target);
    /* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
    /* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    virtual ~ElogTarget(); // Destructor

    // static class functions
    /// add new target to the database
    static ElogTarget* CreateTarget(TString target);
    /// get target from the database
    static ElogTarget* GetTarget(TString target);
    /// check target exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckTargetExists(TString target);
    /// delete target from the database
    static int DeleteTarget(TString target);
    /// print all targets
    static int PrintAll();

    // Getters
    /// get target of the current target
    TString GetTarget() {return str_target;}

    // Setters
    /// set target of the current target
    int SetTarget(TString target);

    /// print information about current target
    void Print();
    /* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 ClassDef(ElogTarget,1);
};

#endif
