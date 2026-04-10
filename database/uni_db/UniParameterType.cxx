// ----------------------------------------------------------------------
//                    UniParameterType cxx file
//                      Generated 05-06-2023
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniParameterType.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniParameterType::UniParameterType(UniConnection* db_connect, int type_id, TString type_name)
{
    connectionDB = db_connect;

    i_type_id = type_id;
    str_type_name = type_name;
}

// -----   Destructor   -------------------------------------------------
UniParameterType::~UniParameterType()
{
    if (connectionDB)
        delete connectionDB;
}

// -----   Creating new parameter type in the database  ---------------------------
UniParameterType* UniParameterType::CreateParameterType(TString type_name)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "insert into parameter_type(type_name) "
        "values ($1)");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, type_name);

    // inserting new parameter type to the Database
    if (!stmt->Process())
    {
        cout<<"ERROR: inserting new parameter type to the Database has been failed"<<endl;
        delete stmt;
        delete connDb;
        return nullptr;
    }

    delete stmt;

    // getting last inserted ID
    int type_id;
    TSQLStatement* stmt_last = db_server->Statement("SELECT currval(pg_get_serial_sequence('parameter_type','type_id'))");

    // process getting last id
    if (stmt_last->Process())
    {
        // store result of statement in buffer
        stmt_last->StoreResult();

        // if there is no last id then exit with error
        if (!stmt_last->NextResultRow())
        {
            cout<<"ERROR: no last ID in DB!"<<endl;
            delete stmt_last;
            return nullptr;
        }
        else
        {
            type_id = stmt_last->GetInt(0);
            delete stmt_last;
        }
    }
    else
    {
        cout<<"ERROR: getting last ID has been failed!"<<endl;
        delete stmt_last;
        return nullptr;
    }

    int tmp_type_id;
    tmp_type_id = type_id;
    TString tmp_type_name;
    tmp_type_name = type_name;

    return new UniParameterType(connDb, tmp_type_id, tmp_type_name);
}

// -----  Get parameter type from the database  ---------------------------
UniParameterType* UniParameterType::GetParameterType(int type_id)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select type_id, type_name "
        "from parameter_type "
        "where type_id = %d", type_id);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get parameter type from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting parameter type from the database has been failed"<<endl;

        delete stmt;
        delete connDb;
        return nullptr;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract row
    if (!stmt->NextResultRow())
    {
        cout<<"ERROR: parameter type was not found in the database"<<endl;

        delete stmt;
        delete connDb;
        return nullptr;
    }

    int tmp_type_id;
    tmp_type_id = stmt->GetInt(0);
    TString tmp_type_name;
    tmp_type_name = stmt->GetString(1);

    delete stmt;

    return new UniParameterType(connDb, tmp_type_id, tmp_type_name);
}

// -----  Check parameter type exists in the database  ---------------------------
int UniParameterType::CheckParameterTypeExists(int type_id)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select 1 "
        "from parameter_type "
        "where type_id = %d", type_id);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get parameter type from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting parameter type from the database has been failed"<<endl;

        delete stmt;
        delete connDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract row
    if (!stmt->NextResultRow())
    {
        delete stmt;
        delete connDb;
        return 0;
    }

    delete stmt;
    delete connDb;

    return 1;
}

// -----  Delete parameter type from the database  ---------------------------
int UniParameterType::DeleteParameterType(int type_id)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "delete from parameter_type "
        "where type_id = $1");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, type_id);

    // delete parameter type from the dataBase
    if (!stmt->Process())
    {
        cout<<"ERROR: deleting parameter type from the dataBase has been failed"<<endl;

        delete stmt;
        delete connDb;
        return -2;
    }

    delete stmt;
    delete connDb;
    return 0;
}

// -----  Print all 'parameter types'  ---------------------------------
int UniParameterType::PrintAll()
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select type_id, type_name "
        "from parameter_type");
    TSQLStatement* stmt = db_server->Statement(sql);

    // get all 'parameter types' from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting all 'parameter types' from the dataBase has been failed"<<endl;

        delete stmt;
        delete connDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // print rows
    cout<<"Table 'parameter_type':"<<endl;
    while (stmt->NextResultRow())
    {
        cout<<"type_id: ";
        cout<<(stmt->GetInt(0));
        cout<<", type_name: ";
        cout<<(stmt->GetString(1));
        cout<<"."<<endl;
    }

    delete stmt;
    delete connDb;

    return 0;
}


// Setters functions
int UniParameterType::SetTypeName(TString type_name)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update parameter_type "
        "set type_name = $1 "
        "where type_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, type_name);
    stmt->SetInt(1, i_type_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about parameter type has been failed"<<endl;

        delete stmt;
        return -2;
    }

    str_type_name = type_name;

    delete stmt;
    return 0;
}

// -----  Print current parameter type  ---------------------------------------
void UniParameterType::Print()
{
    cout<<"Table 'parameter_type'";
    cout<<". type_id: "<<i_type_id<<". type_name: "<<str_type_name<<endl;

    return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */
