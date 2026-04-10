// ----------------------------------------------------------------------
//                    ElogAttachment cxx file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "ElogAttachment.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
ElogAttachment::ElogAttachment(ElogConnection* db_connect, int record_id, int attachment_number, TString file_path)
{
    connectionDB = db_connect;

    i_record_id = record_id;
    i_attachment_number = attachment_number;
    str_file_path = file_path;
}

// -----   Destructor   -------------------------------------------------
ElogAttachment::~ElogAttachment()
{
    if (connectionDB)
        delete connectionDB;
}

// -----   Creating new attachment in the database  ---------------------------
ElogAttachment* ElogAttachment::CreateAttachment(int record_id, int attachment_number, TString file_path)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "insert into attachment_(record_id, attachment_number, file_path) "
        "values ($1, $2, $3)");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, record_id);
    stmt->SetInt(1, attachment_number);
    stmt->SetString(2, file_path);

    // inserting new attachment to the Database
    if (!stmt->Process())
    {
        cout<<"ERROR: inserting new attachment to the Database has been failed"<<endl;
        delete stmt;
        delete connDb;
        return nullptr;
    }

    delete stmt;

    int tmp_record_id;
    tmp_record_id = record_id;
    int tmp_attachment_number;
    tmp_attachment_number = attachment_number;
    TString tmp_file_path;
    tmp_file_path = file_path;

    return new ElogAttachment(connDb, tmp_record_id, tmp_attachment_number, tmp_file_path);
}

// -----  Get attachment from the database  ---------------------------
ElogAttachment* ElogAttachment::GetAttachment(int record_id, int attachment_number)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select record_id, attachment_number, file_path "
        "from attachment_ "
        "where record_id = %d and attachment_number = %d", record_id, attachment_number);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get attachment from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting attachment from the database has been failed"<<endl;

        delete stmt;
        delete connDb;
        return nullptr;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract row
    if (!stmt->NextResultRow())
    {
        cout<<"ERROR: attachment was not found in the database"<<endl;

        delete stmt;
        delete connDb;
        return nullptr;
    }

    int tmp_record_id;
    tmp_record_id = stmt->GetInt(0);
    int tmp_attachment_number;
    tmp_attachment_number = stmt->GetInt(1);
    TString tmp_file_path;
    tmp_file_path = stmt->GetString(2);

    delete stmt;

    return new ElogAttachment(connDb, tmp_record_id, tmp_attachment_number, tmp_file_path);
}

// -----  Get attachment from the database by unique key  --------------
ElogAttachment* ElogAttachment::GetAttachment(TString file_path)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select record_id, attachment_number, file_path "
        "from attachment_ "
        "where lower(file_path) = lower('%s')", file_path.Data());
    TSQLStatement* stmt = db_server->Statement(sql);

    // get attachment from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting attachment from the database has been failed"<<endl;

        delete stmt;
        delete connDb;
        return nullptr;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract row
    if (!stmt->NextResultRow())
    {
        cout<<"ERROR: attachment was not found in the database"<<endl;

        delete stmt;
        delete connDb;
        return nullptr;
    }

    int tmp_record_id;
    tmp_record_id = stmt->GetInt(0);
    int tmp_attachment_number;
    tmp_attachment_number = stmt->GetInt(1);
    TString tmp_file_path;
    tmp_file_path = stmt->GetString(2);

    delete stmt;

    return new ElogAttachment(connDb, tmp_record_id, tmp_attachment_number, tmp_file_path);
}

// -----  Check attachment exists in the database  ---------------------------
int ElogAttachment::CheckAttachmentExists(int record_id, int attachment_number)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select 1 "
        "from attachment_ "
        "where record_id = %d and attachment_number = %d", record_id, attachment_number);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get attachment from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting attachment from the database has been failed"<<endl;

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

// -----  Check attachment exists in the database by unique key  --------------
int ElogAttachment::CheckAttachmentExists(TString file_path)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select 1 "
        "from attachment_ "
        "where lower(file_path) = lower('%s')", file_path.Data());
    TSQLStatement* stmt = db_server->Statement(sql);

    // get attachment from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting attachment from the database has been failed"<<endl;

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

// -----  Delete attachment from the database  ---------------------------
int ElogAttachment::DeleteAttachment(int record_id, int attachment_number)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "delete from attachment_ "
        "where record_id = $1 and attachment_number = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, record_id);
    stmt->SetInt(1, attachment_number);

    // delete attachment from the dataBase
    if (!stmt->Process())
    {
        cout<<"ERROR: deleting attachment from the dataBase has been failed"<<endl;

        delete stmt;
        delete connDb;
        return -2;
    }

    delete stmt;
    delete connDb;
    return 0;
}

// -----  Delete attachment from the database by unique key  --------------
int ElogAttachment::DeleteAttachment(TString file_path)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "delete from attachment_ "
        "where lower(file_path) = lower($1)");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, file_path);

    // delete attachment from the dataBase
    if (!stmt->Process())
    {
        cout<<"ERROR: deleting attachment from the DataBase has been failed"<<endl;

        delete stmt;
        delete connDb;
        return -2;
    }

    delete stmt;
    delete connDb;
    return 0;
}

// -----  Print all 'attachments'  ---------------------------------
int ElogAttachment::PrintAll()
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select record_id, attachment_number, file_path "
        "from attachment_");
    TSQLStatement* stmt = db_server->Statement(sql);

    // get all 'attachments' from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting all 'attachments' from the dataBase has been failed"<<endl;

        delete stmt;
        delete connDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // print rows
    cout<<"Table 'attachment_':"<<endl;
    while (stmt->NextResultRow())
    {
        cout<<"record_id: ";
        cout<<(stmt->GetInt(0));
        cout<<", attachment_number: ";
        cout<<(stmt->GetInt(1));
        cout<<", file_path: ";
        cout<<(stmt->GetString(2));
        cout<<"."<<endl;
    }

    delete stmt;
    delete connDb;

    return 0;
}


// Setters functions
int ElogAttachment::SetRecordId(int record_id)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update attachment_ "
        "set record_id = $1 "
        "where record_id = $2 and attachment_number = $3");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, record_id);
    stmt->SetInt(1, i_record_id);
    stmt->SetInt(2, i_attachment_number);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about attachment has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i_record_id = record_id;

    delete stmt;
    return 0;
}

int ElogAttachment::SetAttachmentNumber(int attachment_number)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update attachment_ "
        "set attachment_number = $1 "
        "where record_id = $2 and attachment_number = $3");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, attachment_number);
    stmt->SetInt(1, i_record_id);
    stmt->SetInt(2, i_attachment_number);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about attachment has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i_attachment_number = attachment_number;

    delete stmt;
    return 0;
}

int ElogAttachment::SetFilePath(TString file_path)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update attachment_ "
        "set file_path = $1 "
        "where record_id = $2 and attachment_number = $3");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, file_path);
    stmt->SetInt(1, i_record_id);
    stmt->SetInt(2, i_attachment_number);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about attachment has been failed"<<endl;

        delete stmt;
        return -2;
    }

    str_file_path = file_path;

    delete stmt;
    return 0;
}

// -----  Print current attachment  ---------------------------------------
void ElogAttachment::Print()
{
    cout<<"Table 'attachment_'";
    cout<<". record_id: "<<i_record_id<<". attachment_number: "<<i_attachment_number<<". file_path: "<<str_file_path<<endl;

    return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */
