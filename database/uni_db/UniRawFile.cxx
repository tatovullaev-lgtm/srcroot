// ----------------------------------------------------------------------
//                    UniRawFile cxx file 
//                      Generated 27-09-2022 
// ----------------------------------------------------------------------

#include "UniRawFile.h"

#include "TSQLServer.h"
#include "TSQLStatement.h"
#include "TMD5.h"

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniRawFile::UniRawFile(UniConnection* db_connect, int file_id, int period_number, int run_number, int start_event, int end_event, TDatime start_datetime, TDatime end_datetime, TString file_path, int event_count, int64_t file_size, TString* file_hash)
{
    connectionDB = db_connect;

    i_file_id = file_id;
    i_period_number = period_number;
    i_run_number = run_number;
    i_start_event = start_event;
    i_end_event = end_event;
    ts_start_datetime = start_datetime;
    ts_end_datetime = end_datetime;
    str_file_path = file_path;
    i_event_count = event_count;
    i64_file_size = file_size;
    str_file_hash = file_hash;
}

// -----   Destructor   -------------------------------------------------
UniRawFile::~UniRawFile()
{
    if (connectionDB)
        delete connectionDB;
    if (str_file_hash)
        delete str_file_hash;
}

// -----   Creating new raw file in the database  ---------------------------
UniRawFile* UniRawFile::CreateRawFile(int period_number, int run_number, int start_event, int end_event, TDatime start_datetime, TDatime end_datetime, TString file_path, int event_count, int64_t file_size, TString* file_hash)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "insert into raw_file(period_number, run_number, start_event, end_event, start_datetime, end_datetime, file_path, event_count, file_size, file_hash) "
        "values ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, period_number);
    stmt->SetInt(1, run_number);
    stmt->SetInt(2, start_event);
    stmt->SetInt(3, end_event);
    stmt->SetDatime(4, start_datetime);
    stmt->SetDatime(5, end_datetime);
    stmt->SetString(6, file_path);
    stmt->SetInt(7, event_count);
    stmt->SetLong64(8, file_size);
    if (file_hash == nullptr)
        stmt->SetNull(9);
    else
        stmt->SetString(9, *file_hash);

    // inserting new raw file to the Database
    if (!stmt->Process())
    {
        cout<<"ERROR: inserting new raw file to the Database has been failed"<<endl;
        delete stmt;
        delete connDb;
        return nullptr;
    }

    delete stmt;

    // getting last inserted ID
    int file_id;
    TSQLStatement* stmt_last = db_server->Statement("SELECT currval(pg_get_serial_sequence('raw_file','file_id'))");

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
            file_id = stmt_last->GetInt(0);
            delete stmt_last;
        }
    }
    else
    {
        cout<<"ERROR: getting last ID has been failed!"<<endl;
        delete stmt_last;
        return nullptr;
    }

    int tmp_file_id;
    tmp_file_id = file_id;
    int tmp_period_number;
    tmp_period_number = period_number;
    int tmp_run_number;
    tmp_run_number = run_number;
    int tmp_start_event;
    tmp_start_event = start_event;
    int tmp_end_event;
    tmp_end_event = end_event;
    TDatime tmp_start_datetime;
    tmp_start_datetime = start_datetime;
    TDatime tmp_end_datetime;
    tmp_end_datetime = end_datetime;
    TString tmp_file_path;
    tmp_file_path = file_path;
    int tmp_event_count;
    tmp_event_count = event_count;
    int64_t tmp_file_size;
    tmp_file_size = file_size;
    TString* tmp_file_hash;
    if (file_hash == nullptr) tmp_file_hash = nullptr;
    else
        tmp_file_hash = new TString(*file_hash);

    return new UniRawFile(connDb, tmp_file_id, tmp_period_number, tmp_run_number, tmp_start_event, tmp_end_event, tmp_start_datetime, tmp_end_datetime, tmp_file_path, tmp_event_count, tmp_file_size, tmp_file_hash);
}

// -----  Get raw file from the database  ---------------------------
UniRawFile* UniRawFile::GetRawFile(int file_id)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select file_id, period_number, run_number, start_event, end_event, start_datetime, end_datetime, file_path, event_count, file_size, file_hash "
        "from raw_file "
        "where file_id = %d", file_id);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get raw file from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting raw file from the database has been failed"<<endl;

        delete stmt;
        delete connDb;
        return nullptr;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract row
    if (!stmt->NextResultRow())
    {
        cout<<"ERROR: raw file was not found in the database"<<endl;

        delete stmt;
        delete connDb;
        return nullptr;
    }

    int tmp_file_id;
    tmp_file_id = stmt->GetInt(0);
    int tmp_period_number;
    tmp_period_number = stmt->GetInt(1);
    int tmp_run_number;
    tmp_run_number = stmt->GetInt(2);
    int tmp_start_event;
    tmp_start_event = stmt->GetInt(3);
    int tmp_end_event;
    tmp_end_event = stmt->GetInt(4);
    TDatime tmp_start_datetime;
    tmp_start_datetime = stmt->GetDatime(5);
    TDatime tmp_end_datetime;
    tmp_end_datetime = stmt->GetDatime(6);
    TString tmp_file_path;
    tmp_file_path = stmt->GetString(7);
    int tmp_event_count;
    tmp_event_count = stmt->GetInt(8);
    int64_t tmp_file_size;
    tmp_file_size = stmt->GetLong64(9);
    TString* tmp_file_hash;
    if (stmt->IsNull(10)) tmp_file_hash = nullptr;
    else
        tmp_file_hash = new TString(stmt->GetString(10));

    delete stmt;

    return new UniRawFile(connDb, tmp_file_id, tmp_period_number, tmp_run_number, tmp_start_event, tmp_end_event, tmp_start_datetime, tmp_end_datetime, tmp_file_path, tmp_event_count, tmp_file_size, tmp_file_hash);
}

// -----  Get raw file from the database by unique key  --------------
UniRawFile* UniRawFile::GetRawFile(TString file_path)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select file_id, period_number, run_number, start_event, end_event, start_datetime, end_datetime, file_path, event_count, file_size, file_hash "
        "from raw_file "
        "where lower(file_path) = lower('%s')", file_path.Data());
    TSQLStatement* stmt = db_server->Statement(sql);

    // get raw file from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting raw file from the database has been failed"<<endl;

        delete stmt;
        delete connDb;
        return nullptr;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract row
    if (!stmt->NextResultRow())
    {
        cout<<"ERROR: raw file was not found in the database"<<endl;

        delete stmt;
        delete connDb;
        return nullptr;
    }

    int tmp_file_id;
    tmp_file_id = stmt->GetInt(0);
    int tmp_period_number;
    tmp_period_number = stmt->GetInt(1);
    int tmp_run_number;
    tmp_run_number = stmt->GetInt(2);
    int tmp_start_event;
    tmp_start_event = stmt->GetInt(3);
    int tmp_end_event;
    tmp_end_event = stmt->GetInt(4);
    TDatime tmp_start_datetime;
    tmp_start_datetime = stmt->GetDatime(5);
    TDatime tmp_end_datetime;
    tmp_end_datetime = stmt->GetDatime(6);
    TString tmp_file_path;
    tmp_file_path = stmt->GetString(7);
    int tmp_event_count;
    tmp_event_count = stmt->GetInt(8);
    int64_t tmp_file_size;
    tmp_file_size = stmt->GetLong64(9);
    TString* tmp_file_hash;
    if (stmt->IsNull(10)) tmp_file_hash = nullptr;
    else
        tmp_file_hash = new TString(stmt->GetString(10));

    delete stmt;

    return new UniRawFile(connDb, tmp_file_id, tmp_period_number, tmp_run_number, tmp_start_event, tmp_end_event, tmp_start_datetime, tmp_end_datetime, tmp_file_path, tmp_event_count, tmp_file_size, tmp_file_hash);
}

// -----  Check raw file exists in the database  ---------------------------
int UniRawFile::CheckRawFileExists(int file_id)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select 1 "
        "from raw_file "
        "where file_id = %d", file_id);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get raw file from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting raw file from the database has been failed"<<endl;

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

// -----  Check raw file exists in the database by unique key  --------------
int UniRawFile::CheckRawFileExists(TString file_path)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select 1 "
        "from raw_file "
        "where lower(file_path) = lower('%s')", file_path.Data());
    TSQLStatement* stmt = db_server->Statement(sql);

    // get raw file from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting raw file from the database has been failed"<<endl;

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

// -----  Delete raw file from the database  ---------------------------
int UniRawFile::DeleteRawFile(int file_id)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "delete from raw_file "
        "where file_id = $1");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, file_id);

    // delete raw file from the dataBase
    if (!stmt->Process())
    {
        cout<<"ERROR: deleting raw file from the dataBase has been failed"<<endl;

        delete stmt;
        delete connDb;
        return -2;
    }

    delete stmt;
    delete connDb;
    return 0;
}

// -----  Delete raw file from the database by unique key  --------------
int UniRawFile::DeleteRawFile(TString file_path)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "delete from raw_file "
        "where lower(file_path) = lower($1)");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, file_path);

    // delete raw file from the dataBase
    if (!stmt->Process())
    {
        cout<<"ERROR: deleting raw file from the DataBase has been failed"<<endl;

        delete stmt;
        delete connDb;
        return -2;
    }

    delete stmt;
    delete connDb;
    return 0;
}

// -----  Print all 'raw files'  ---------------------------------
int UniRawFile::PrintAll()
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select file_id, period_number, run_number, start_event, end_event, start_datetime, end_datetime, file_path, event_count, file_size, file_hash "
        "from raw_file");
    TSQLStatement* stmt = db_server->Statement(sql);

    // get all 'raw files' from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting all 'raw files' from the dataBase has been failed"<<endl;

        delete stmt;
        delete connDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // print rows
    cout<<"Table 'raw_file':"<<endl;
    while (stmt->NextResultRow())
    {
        cout<<"file_id: ";
        cout<<(stmt->GetInt(0));
        cout<<", period_number: ";
        cout<<(stmt->GetInt(1));
        cout<<", run_number: ";
        cout<<(stmt->GetInt(2));
        cout<<", start_event: ";
        cout<<(stmt->GetInt(3));
        cout<<", end_event: ";
        cout<<(stmt->GetInt(4));
        cout<<", start_datetime: ";
        cout<<(stmt->GetDatime(5)).AsSQLString();
        cout<<", end_datetime: ";
        cout<<(stmt->GetDatime(6)).AsSQLString();
        cout<<", file_path: ";
        cout<<(stmt->GetString(7));
        cout<<", event_count: ";
        cout<<(stmt->GetInt(8));
        cout<<", file_size: ";
        cout<<(stmt->GetLong64(9));
        cout<<", file_hash: ";
        if (stmt->IsNull(10)) cout<<"nullptr";
        else
            cout<<stmt->GetString(10);
        cout<<"."<<endl;
    }

    delete stmt;
    delete connDb;

    return 0;
}


// Setters functions
int UniRawFile::SetPeriodNumber(int period_number)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update raw_file "
        "set period_number = $1 "
        "where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, period_number);
    stmt->SetInt(1, i_file_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about raw file has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i_period_number = period_number;

    delete stmt;
    return 0;
}

int UniRawFile::SetRunNumber(int run_number)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update raw_file "
        "set run_number = $1 "
        "where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, run_number);
    stmt->SetInt(1, i_file_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about raw file has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i_run_number = run_number;

    delete stmt;
    return 0;
}

int UniRawFile::SetStartEvent(int start_event)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update raw_file "
        "set start_event = $1 "
        "where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, start_event);
    stmt->SetInt(1, i_file_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about raw file has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i_start_event = start_event;

    delete stmt;
    return 0;
}

int UniRawFile::SetEndEvent(int end_event)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update raw_file "
        "set end_event = $1 "
        "where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, end_event);
    stmt->SetInt(1, i_file_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about raw file has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i_end_event = end_event;

    delete stmt;
    return 0;
}

int UniRawFile::SetStartDatetime(TDatime start_datetime)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update raw_file "
        "set start_datetime = $1 "
        "where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetDatime(0, start_datetime);
    stmt->SetInt(1, i_file_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about raw file has been failed"<<endl;

        delete stmt;
        return -2;
    }

    ts_start_datetime = start_datetime;

    delete stmt;
    return 0;
}

int UniRawFile::SetEndDatetime(TDatime end_datetime)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update raw_file "
        "set end_datetime = $1 "
        "where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetDatime(0, end_datetime);
    stmt->SetInt(1, i_file_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about raw file has been failed"<<endl;

        delete stmt;
        return -2;
    }

    ts_end_datetime = end_datetime;

    delete stmt;
    return 0;
}

int UniRawFile::SetFilePath(TString file_path)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update raw_file "
        "set file_path = $1 "
        "where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, file_path);
    stmt->SetInt(1, i_file_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about raw file has been failed"<<endl;

        delete stmt;
        return -2;
    }

    str_file_path = file_path;

    delete stmt;
    return 0;
}

int UniRawFile::SetEventCount(int event_count)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update raw_file "
        "set event_count = $1 "
        "where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, event_count);
    stmt->SetInt(1, i_file_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about raw file has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i_event_count = event_count;

    delete stmt;
    return 0;
}

int UniRawFile::SetFileSize(int64_t file_size)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update raw_file "
        "set file_size = $1 "
        "where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetLong64(0, file_size);
    stmt->SetInt(1, i_file_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about raw file has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i64_file_size = file_size;

    delete stmt;
    return 0;
}

int UniRawFile::SetFileHash(TString* file_hash)
{
    if (!connectionDB)
    {
        cout<<"CRITICAL ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionDB->GetSQLServer();

    TString sql = TString::Format(
        "update raw_file "
        "set file_hash = $1 "
        "where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    if (file_hash == nullptr)
        stmt->SetNull(0);
    else
        stmt->SetString(0, *file_hash);
    stmt->SetInt(1, i_file_id);

    // write new value to the database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating information about raw file has been failed"<<endl;

        delete stmt;
        return -2;
    }

    if (str_file_hash)
        delete str_file_hash;
    if (file_hash == nullptr) str_file_hash = nullptr;
    else
        str_file_hash = new TString(*file_hash);

    delete stmt;
    return 0;
}

// -----  Print current raw file  ---------------------------------------
void UniRawFile::Print()
{
    cout<<"Table 'raw_file'";
    cout<<". file_id: "<<i_file_id<<". period_number: "<<i_period_number<<". run_number: "<<i_run_number<<". start_event: "<<i_start_event<<". end_event: "<<i_end_event<<". start_datetime: "<<ts_start_datetime.AsSQLString()<<". end_datetime: "<<ts_end_datetime.AsSQLString()<<". file_path: "<<str_file_path<<". event_count: "<<i_event_count<<". file_size: "<<i64_file_size<<". file_hash: "<<(str_file_hash == nullptr? "nullptr": *str_file_hash)<<endl;

    return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */

// check raw file exists in the database
// for the given period_number:run_number and start_event-end_event values
// returns 1 - true, 0 - false, <0 - database operation error
int UniRawFile::CheckRawFileExists(int period_number, int run_number, int start_event, int end_event)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select 1 "
        "from raw_file "
        "where period_number = %d AND run_number = %d "
        "AND %d >= start_event AND %d <= end_event AND %d >= start_event AND %d <= end_event "
        "AND NOT (((%d - start_event) = 1) AND ((%d - end_event) = -1))",
        period_number, run_number, start_event, start_event, end_event, end_event, start_event, end_event);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get raw file from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting raw file from the database has been failed"<<endl;

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

// check that raw file exists in the database
// for the given period_number:run_number and start_event value
// returns maximum 'end_event' value if some exist, 0 - not exists, <0 - database operation error
int UniRawFile::CheckRawFileExists(int period_number, int run_number, int start_event)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select end_event "
        "from raw_file "
        "where period_number = %d AND run_number = %d "
        "AND %d = start_event "
        "order by end_event desc limit 1",
        period_number, run_number, start_event);
    TSQLStatement* stmt = db_server->Statement(sql);

             // get raw file from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting raw file from the database has been failed"<<endl;

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

    int i_end_event = stmt->GetInt(0);
    if (i_end_event == 0)
    {
        cout<<"ERROR: while checking existense of raw files in UniConDa one raw file is found with 0 end event value ("
             <<period_number<<":"<<run_number<<":"<<start_event<<")"<<endl
             <<"The function returns 1 end event value to exclude uncertancies"<<endl;
        i_end_event++;
    }

    delete stmt;
    delete connDb;

    return i_end_event;
}

/// get event count for a given run as a sum for all raw files: if <0 - database operation error
int UniRawFile::GetSumEventCount(int period_number, int run_number)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select sum(event_count) "
        "from raw_file "
        "where period_number = %d AND run_number = %d",
        period_number, run_number);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get sum from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting summary event count for run "<<period_number<<":"<<run_number<<" from the database has been failed"<<endl;
        delete stmt;
        delete connDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract row
    if (!stmt->NextResultRow())
    {
        cout<<"WARNING: there are no raw files for run "<<period_number<<":"<<run_number<<" in the database"<<endl;
        delete stmt;
        delete connDb;
        return 0;
    }

    int sum_event_count = stmt->GetInt(0);

    delete stmt;
    delete connDb;

    return sum_event_count;
}

// get array of raw files (UniRawFile*) corresponding to the period:run number (return nullptr if an error occured)
TObjArray* UniRawFile::GetRawFiles(int period_number, int run_number)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select file_id, period_number, run_number, start_event, end_event, start_datetime, end_datetime, file_path, event_count, file_size, file_hash "
        "from raw_file "
        "where period_number = %d AND run_number = %d "
        "order by start_event asc",
        period_number, run_number);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get raw files for the run from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting raw files for run "<<period_number<<":"<<run_number<<" from the database has been failed"<<endl;
        delete stmt;
        delete connDb;
        return nullptr;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    TObjArray* raw_files = new TObjArray();
    while (stmt->NextResultRow())
    {
        UniConnection* connRaw = UniConnection::Open();
        if (connRaw == nullptr)
        {
            cout<<"ERROR: the connection to the UniConDa Database for the selected raw file info failed"<<endl;
            delete stmt;
            delete connDb;
            return nullptr;
        }

        int tmp_file_id;
        tmp_file_id = stmt->GetInt(0);
        int tmp_period_number;
        tmp_period_number = stmt->GetInt(1);
        int tmp_run_number;
        tmp_run_number = stmt->GetInt(2);
        int tmp_start_event;
        tmp_start_event = stmt->GetInt(3);
        int tmp_end_event;
        tmp_end_event = stmt->GetInt(4);
        TDatime tmp_start_datetime;
        tmp_start_datetime = stmt->GetDatime(5);
        TDatime tmp_end_datetime;
        tmp_end_datetime = stmt->GetDatime(6);
        TString tmp_file_path;
        tmp_file_path = stmt->GetString(7);
        int tmp_event_count;
        tmp_event_count = stmt->GetInt(8);
        int64_t tmp_file_size;
        tmp_file_size = stmt->GetLong64(9);
        TString* tmp_file_hash;
        if (stmt->IsNull(10)) tmp_file_hash = nullptr;
        else
            tmp_file_hash = new TString(stmt->GetString(10));

        raw_files->Add((TObject*) new UniRawFile(connDb, tmp_file_id, tmp_period_number, tmp_run_number,
            tmp_start_event, tmp_end_event, tmp_start_datetime, tmp_end_datetime, tmp_file_path, tmp_event_count, tmp_file_size, tmp_file_hash));
    }

    delete stmt;
    delete connDb;

    return raw_files;
}

// get array of raw files (UniRawFile*) corresponding to the period_number:run_number and start_event - end_event
// ordered by file_id descending
// returns nullptr if an error occured
TObjArray* UniRawFile::GetRawFiles(int period_number, int run_number, int start_event, int end_event)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select file_id, period_number, run_number, start_event, end_event, start_datetime, end_datetime, file_path, event_count, file_size, file_hash "
        "from raw_file "
        "where period_number = %d AND run_number = %d AND start_event = %d AND end_event = %d "
        "order by file_id desc",
        period_number, run_number, start_event, end_event);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get raw files for the run and events from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting raw files for run "<<period_number<<":"<<run_number<<" and events "
            << start_event << "-" << end_event << " from the database has been failed"<<endl;
        delete stmt;
        delete connDb;
        return nullptr;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    TObjArray* raw_files = new TObjArray();
    while (stmt->NextResultRow())
    {
        UniConnection* connRaw = UniConnection::Open();
        if (connRaw == nullptr)
        {
            cout<<"ERROR: the connection to the UniConDa Database for the selected raw file info failed"<<endl;
            delete stmt;
            delete connDb;
            return nullptr;
        }

        int tmp_file_id;
        tmp_file_id = stmt->GetInt(0);
        int tmp_period_number;
        tmp_period_number = stmt->GetInt(1);
        int tmp_run_number;
        tmp_run_number = stmt->GetInt(2);
        int tmp_start_event;
        tmp_start_event = stmt->GetInt(3);
        int tmp_end_event;
        tmp_end_event = stmt->GetInt(4);
        TDatime tmp_start_datetime;
        tmp_start_datetime = stmt->GetDatime(5);
        TDatime tmp_end_datetime;
        tmp_end_datetime = stmt->GetDatime(6);
        TString tmp_file_path;
        tmp_file_path = stmt->GetString(7);
        int tmp_event_count;
        tmp_event_count = stmt->GetInt(8);
        int64_t tmp_file_size;
        tmp_file_size = stmt->GetLong64(9);
        TString* tmp_file_hash;
        if (stmt->IsNull(10)) tmp_file_hash = nullptr;
        else
            tmp_file_hash = new TString(stmt->GetString(10));

        raw_files->Add((TObject*) new UniRawFile(connDb, tmp_file_id, tmp_period_number, tmp_run_number,
                                                tmp_start_event, tmp_end_event, tmp_start_datetime, tmp_end_datetime, tmp_file_path, tmp_event_count, tmp_file_size, tmp_file_hash));
    }

    delete stmt;
    delete connDb;

    return raw_files;
}

/// calculate and return (in hex string format) cheсksum for a given file
/// use Adler32 algorithm if zlib library is found
/// or use MD5 algorithm if zlib library is not found
TString UniRawFile::CalculateFileHash(TString raw_file_path)
{
#ifdef HAVE_ZLIB
    ifstream raw_file(raw_file_path, ios::binary);
    if (!raw_file)
    {
        cout<<"ERROR: raw file could not be opened to calculate Adler32 checksum: "<<raw_file_path<<endl;
        return "";
    }

    uLong adler = adler32(0L, Z_NULL, 0); // initialize Adler32 checksum

    const size_t bufferSize = 65536;
    char buffer[bufferSize];
    while (raw_file.read(buffer, bufferSize) || raw_file.gcount() > 0)
        adler = adler32(adler, (const Bytef*)buffer, raw_file.gcount());

    raw_file.close();

    //cout << "Adler32 for " << raw_file_path << ": 0x" << std::hex << adler << std::dec << endl;
    std::ostringstream hexStream;
    hexStream << std::hex << std::setw(8) << std::setfill('0') << (adler & 0xFFFFFFFF);

    return hexStream.str();
#else
    TMD5* pMD5 = TMD5::FileChecksum(raw_file_path.Data());
    if (!pMD5)
    {
        cout<<"ERROR: raw file could not be opened to calculate MD5 checksum: "<<raw_file_path<<endl;
        return "";
    }

    //cout << "MD5 for " << raw_file_path << ": 0x" << pMD5->AsString() << endl;
    TString strMD5(pMD5->AsString());

    delete pMD5
    return strMD5;
#endif
}
