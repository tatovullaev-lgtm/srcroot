// ----------------------------------------------------------------------
//                    ElogPerson header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** ElogPerson.h
 ** Class for the table: person_ 
 **/ 

#ifndef ElogPerson_H 
#define ElogPerson_H 1 

#include "TString.h"
#include "TDatime.h"

#include "ElogConnection.h"

class ElogPerson
{
 private:
    /* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    /// connection to the database
    ElogConnection* connectionDB;

    /// person id
    int i_person_id;
    /// person name
    TString str_person_name;
    /// is active
    bool b_is_active;
    /// email
    TString* str_email;

    //Constructor
    ElogPerson(ElogConnection* db_connect, int person_id, TString person_name, bool is_active, TString* email);
    /* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
    /* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    virtual ~ElogPerson(); // Destructor

    // static class functions
    /// add new person to the database
    static ElogPerson* CreatePerson(TString person_name, bool is_active, TString* email);
    /// get person from the database
    static ElogPerson* GetPerson(int person_id);
    /// get person from the database
    static ElogPerson* GetPerson(TString person_name);
    /// check person exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckPersonExists(int person_id);
    /// check person exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckPersonExists(TString person_name);
    /// delete person from the database
    static int DeletePerson(int person_id);
    /// delete person from the database
    static int DeletePerson(TString person_name);
    /// print all persons
    static int PrintAll();

    // Getters
    /// get person id of the current person
    int GetPersonId() {return i_person_id;}
    /// get person name of the current person
    TString GetPersonName() {return str_person_name;}
    /// get is active of the current person
    bool GetIsActive() {return b_is_active;}
    /// get email of the current person
    TString* GetEmail() {if (str_email == nullptr) return nullptr; else return new TString(*str_email);}

    // Setters
    /// set person name of the current person
    int SetPersonName(TString person_name);
    /// set is active of the current person
    int SetIsActive(bool is_active);
    /// set email of the current person
    int SetEmail(TString* email);

    /// print information about current person
    void Print();
    /* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 ClassDef(ElogPerson,1);
};

#endif
