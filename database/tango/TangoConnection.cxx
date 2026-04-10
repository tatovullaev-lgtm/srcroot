// -------------------------------------------------------------------------
// -----                       TangoConnection source file               -----
// -------------------------------------------------------------------------
#include "TangoConnection.h"

#include "TRandom.h"

#include <chrono>
#include <thread>

mapSQLServer* TangoConnection::mapConnection = nullptr;

// -----   Constructor with connection   ----------------------
TangoConnection::TangoConnection(TSQLServer* pSQLServer)
{
    tango_db_server = pSQLServer;
}

TangoConnection* TangoConnection::OpenConnection(const TString& connect_string,
                                                 const char* db_host,
                                                 const char* db_user,
                                                 const char* db_password)
{
    if (TangoConnection::mapConnection == nullptr)
        TangoConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = nullptr;
    itSQLServer it = TangoConnection::mapConnection->find(connect_string.Data());
    if (it != TangoConnection::mapConnection->end())
        pSQLServer = it->second;
    else {
        // try to connect 3 times
        for (int i = 0; i < 4; i++) {
            pSQLServer = TSQLServer::Connect(connect_string, db_user, db_password);
            if (pSQLServer)
                break;
            if (i == 3) {
                cout << "ERROR: database connection was not established (" << db_host << ") for '" << db_user
                     << "' user" << endl;
                return nullptr;
            }
            unsigned int random_integer = pow(10, i) + gRandom->Integer(pow(10, i) * 2 + 1);
            this_thread::sleep_for(chrono::seconds(random_integer));
        }
        // cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

        TangoConnection::mapConnection->insert(pairSQLServer(connect_string.Data(), pSQLServer));
    }

    return new TangoConnection(pSQLServer);
}

// -------------------------------------------------------------------
TangoConnection* TangoConnection::Open()
{
    TString conString = TString::Format("pgsql://%s/%s", TANGO_DB_HOST, TANGO_DB_NAME);
    return OpenConnection(conString, TANGO_DB_HOST, TANGO_DB_USERNAME, TANGO_DB_PASSWORD);
}

// -------------------------------------------------------------------
TangoConnection* TangoConnection::Open(enumDBMS database_type,
                                       const char* strDBHost,
                                       const char* strDBName,
                                       const char* strUID,
                                       const char* strPassword)
{
    char* db_type;
    switch (database_type) {
        case MySQL:
            db_type = (char*)"mysql";
            break;
        case PgSQL:
            db_type = (char*)"pgsql";
            break;
        default: {
            cout << "ERROR: incorrect database type!" << endl;
            return nullptr;
        }
    }
    TString conString = TString::Format("%s://%s/%s", db_type, strDBHost, strDBName);
    return OpenConnection(conString, strDBHost, strUID, strPassword);
}

// -------------------------------------------------------------------
TangoConnection::~TangoConnection()
{
    // if (tango_db_server)
    //     delete tango_db_server;
}
