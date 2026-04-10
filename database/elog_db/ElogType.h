// ----------------------------------------------------------------------
//                    ElogType header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** ElogType.h
 ** Class for the table: type_ 
 **/ 

#ifndef ElogType_H 
#define ElogType_H 1 

#include "TString.h"
#include "TDatime.h"

#include "ElogConnection.h"

class ElogType
{
 private:
    /* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    /// connection to the database
    ElogConnection* connectionDB;

    /// type id
    int i_type_id;
    /// type text
    TString str_type_text;

    //Constructor
    ElogType(ElogConnection* db_connect, int type_id, TString type_text);
    /* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
    /* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    virtual ~ElogType(); // Destructor

    // static class functions
    /// add new type to the database
    static ElogType* CreateType(TString type_text);
    /// get type from the database
    static ElogType* GetType(int type_id);
    /// get type from the database
    static ElogType* GetType(TString type_text);
    /// check type exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckTypeExists(int type_id);
    /// check type exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckTypeExists(TString type_text);
    /// delete type from the database
    static int DeleteType(int type_id);
    /// delete type from the database
    static int DeleteType(TString type_text);
    /// print all types
    static int PrintAll();

    // Getters
    /// get type id of the current type
    int GetTypeId() {return i_type_id;}
    /// get type text of the current type
    TString GetTypeText() {return str_type_text;}

    // Setters
    /// set type text of the current type
    int SetTypeText(TString type_text);

    /// print information about current type
    void Print();
    /* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 ClassDef(ElogType,1);
};

#endif
