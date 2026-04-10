// -------------------------------------------------------------------------
// -----                       UniConnection source file               -----
// -------------------------------------------------------------------------
#include "UniConnection.h"

#include "TRandom.h"
#include "TSystem.h"

#include <regex>
#include <chrono>
#include <thread>

mapSQLServer* UniConnection::mapConnection = nullptr;

// -----   Constructor with connection   ----------------------
UniConnection::UniConnection(TSQLServer* pSQLServer)
{
    db_server = pSQLServer;
}

UniConnection* UniConnection::OpenConnection(const TString& connect_string, const char* db_host, const char* db_user, const char* db_password)
{
    if (UniConnection::mapConnection == nullptr)
        UniConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = nullptr;
    itSQLServer it = UniConnection::mapConnection->find(connect_string.Data());
    if (it != UniConnection::mapConnection->end())
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

        UniConnection::mapConnection->insert(pairSQLServer(connect_string.Data(), pSQLServer));
    }

    return new UniConnection(pSQLServer);
}

// -------------------------------------------------------------------
UniConnection* UniConnection::Open()
{
    // get database host and name from global variables in C++ header (uni_db_settings.h)
    TString conString = TString::Format("pgsql://%s/%s", UNI_DB_HOST, UNI_DB_NAME);

    // try to get dynamic database username and password from bash environment
    const char *db_username = gSystem->Getenv("UNICONDA_USERNAME"),
               *db_password = gSystem->Getenv("UNICONDA_PASSWORD");
    // if no bash variables then get from global variables in C++ header
    if ((db_username == nullptr) || (db_password == nullptr))
    {
        db_username = UNI_DB_USERNAME;
        db_password = UNI_DB_PASSWORD;
    }

    return OpenConnection(conString, UNI_DB_HOST, db_username, db_password);
}

// -------------------------------------------------------------------
UniConnection* UniConnection::Open(enumDBMS database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword)
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
UniConnection::~UniConnection()
{
    //if (db_server)
    //    delete db_server;
}
