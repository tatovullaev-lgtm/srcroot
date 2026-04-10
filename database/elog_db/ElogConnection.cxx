// -------------------------------------------------------------------------
// -----                       ElogConnection source file               -----
// -------------------------------------------------------------------------
#include "ElogConnection.h"

#include "TRandom.h"
#include "TSystem.h"

#include <chrono>
#include <thread>

mapSQLServer* ElogConnection::mapConnection = nullptr;

// -----   Constructor with connection   ----------------------
ElogConnection::ElogConnection(TSQLServer* pSQLServer)
{
    elog_db_server = pSQLServer;
}

ElogConnection* ElogConnection::OpenConnection(const TString& connect_string, const char* db_host, const char* db_user, const char* db_password)
{
    if (ElogConnection::mapConnection == nullptr)
        ElogConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = nullptr;
    itSQLServer it = ElogConnection::mapConnection->find(connect_string.Data());
    if (it != ElogConnection::mapConnection->end())
        pSQLServer = it->second;
    else
    {
        // try to connect 3 times
        for (int i = 0; i < 4; i++)
        {
            pSQLServer = TSQLServer::Connect(connect_string, db_user, db_password);
            if (pSQLServer)
                break;
            if (i == 3)
            {
                cout<<"ERROR: database connection was not established ("<<db_host<<") for '"<<db_user<<"' user"<<endl;
                return nullptr;
            }
            unsigned int random_integer = pow(10, i) + gRandom->Integer(pow(10, i)*2 + 1);
            this_thread::sleep_for(chrono::seconds(random_integer));
        }
        //cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

        ElogConnection::mapConnection->insert(pairSQLServer(connect_string.Data(), pSQLServer));
    }

    return new ElogConnection(pSQLServer);
}

// -------------------------------------------------------------------
ElogConnection* ElogConnection::Open()
{
    // get database host and name from global variables in C++ header (elog_db_settings.h)
    TString conString = TString::Format("pgsql://%s/%s", ELOG_DB_HOST, ELOG_DB_NAME);

    // try to get dynamic database username and password from bash environment
    const char *db_username = gSystem->Getenv("ELOG_USERNAME"),
               *db_password = gSystem->Getenv("ELOG_PASSWORD");
    // if no bash variables then get from global variables in C++ header
    if ((db_username == nullptr) || (db_password == nullptr))
    {
        db_username = ELOG_DB_USERNAME;
        db_password = ELOG_DB_PASSWORD;
    }

    return OpenConnection(conString, ELOG_DB_HOST, db_username, db_password);
}

// -------------------------------------------------------------------
ElogConnection* ElogConnection::Open(enumDBMS database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword)
{
    char* db_type;
    switch (database_type)
    {
        case MySQL:
            db_type = (char*)"mysql";
            break;
        case PgSQL:
            db_type = (char*)"pgsql";
            break;
        default:
            {
                cout<<"ERROR: incorrect database type!"<<endl;
                return nullptr;
            }
    }
    TString conString = TString::Format("%s://%s/%s", db_type, strDBHost, strDBName);
    return OpenConnection(conString, strDBHost, strUID, strPassword);
}

// -------------------------------------------------------------------
ElogConnection::~ElogConnection()
{
    //if (elog_db_server)
    //    delete elog_db_server;
}
