// ----------------------------------------------------------------------
//                    UniParameterType header file
//                      Generated 05-06-2023
// ----------------------------------------------------------------------

/** /home/soul/bmnroot/database/uni_db/temp/UniParameterType.h
 ** Class for the table: parameter_type
 **/

#ifndef UNIPARAMETERTYPE_H
#define UNIPARAMETERTYPE_H 1

#include "TString.h"
#include "TDatime.h"
#include "TTimeStamp.h"

#include "UniConnection.h"

class UniParameterType
{
 private:
    /* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    /// connection to the database
    UniConnection* connectionDB;

    /// type id
    int i_type_id;
    /// type name
    TString str_type_name;

    //Constructor
    UniParameterType(UniConnection* db_connect, int type_id, TString type_name);
    /* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
    /* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    virtual ~UniParameterType(); // Destructor

    // static class functions
    /// add new parameter type to the database
    static UniParameterType* CreateParameterType(TString type_name);
    /// get parameter type from the database
    static UniParameterType* GetParameterType(int type_id);
    /// check parameter type exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckParameterTypeExists(int type_id);
    /// delete parameter type from the database
    static int DeleteParameterType(int type_id);
    /// print all parameter types
    static int PrintAll();

    // Getters
    /// get type id of the current parameter type
    int GetTypeId() {return i_type_id;}
    /// get type name of the current parameter type
    TString GetTypeName() {return str_type_name;}

    // Setters
    /// set type name of the current parameter type
    int SetTypeName(TString type_name);

    /// print information about current parameter type
    void Print();
    /* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 ClassDef(UniParameterType,1);
};

#endif
