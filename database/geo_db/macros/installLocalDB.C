#if !defined(__CLING__) || defined(__MAKECLING__)
#include "TString.h"
#include "TSystem.h"

#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
#endif

bool exists_file(const char* name)
{
    return (access(name, F_OK) != -1);
}

unsigned long int getServerRevision(const char* url)
{
    unsigned long int serverRev = 0;
    gSystem->Exec(TString::Format("wget -O server_revision.txt \"%s/get_cur_timestamp.php\" > /dev/null 2>&1", url));
    ifstream myFile_Handler;
    string myLine;
    myFile_Handler.open("server_revision.txt");
    if (myFile_Handler.is_open()) {
        char* ptr;
        while (getline(myFile_Handler, myLine)) {
            serverRev = strtoul(myLine.c_str(), &ptr, 10);
            break;
        }
        myFile_Handler.close();
        std::remove("server_revision.txt");
    }
    return serverRev;
}

unsigned long int getLocalRevision(TString& localDbPath)
{
    unsigned long int localRev = 0;
    char type;
    if (exists_file(localDbPath)) {
        ostringstream stringStream;
        stringStream << "sqlite://" << localDbPath;
        TSQLiteServer db(stringStream.str().c_str());
        stringStream.str("");
        if (db.IsConnected()) {
            stringStream << "select value_field from config where key_field='config_time';";
            cout << stringStream.str().c_str() << std::endl;
            TSQLResult* resQ = db.Query(stringStream.str().c_str());
            if (resQ != 0) {
                TSQLRow* row = resQ->Next();
                if (row != NULL) {
                    const char* ch_time = row->GetField(0);
                    char* ptr;
                    if (ch_time != NULL)
                        localRev = strtoul(ch_time, &ptr, 10);
                }
            }
        }
    }
    return localRev;
}

int installLocalDB(const char* tag = nullptr, const char* url = "https://bmn-geometry.jinr.int")
{
    // get DBL_FILE_PATH variable with a local database filepath
    TString localDbPath = gSystem->ExpandPathName(getenv("DBL_FILE_PATH"));
    if (localDbPath == "") {
        cerr << "DBL_FILE_PATH is not set. Please, set DBL_FILE_PATH as a path to the local database file (default, "
                "'local.db')"
             << endl;
        return -1;
    }

    if (tag == nullptr)
    {
        ostringstream stringStream;
        unsigned long int localRev = 0, serverRev = 0;
        char type;
        if (exists_file(localDbPath)) {
            localRev = getLocalRevision(localDbPath);
            /*      std::ostringstream stringStream;
                  stringStream<<"sqlite://"<<localDbPath;
                  TSQLiteServer db(stringStream.str().c_str());
                  stringStream.str("");
                  unsigned long int localRev = 0,serverRev=0;
                  if(db.IsConnected()){
                    stringStream<<"select value_field from config where key_field='config_time';";
                    cout<<stringStream.str().c_str()<<std::endl;
                    TSQLResult* resQ = db.Query(stringStream.str().c_str());
                    if (resQ != 0)
                    {
                  TSQLRow* row = resQ->Next();
                  if (row != NULL){
                    const char* ch_time = row->GetField(0);
                        char *ptr;
                    if(ch_time!=NULL)
                    localRev = strtoul(ch_time, &ptr, 10);
                  }
                    }
                  }*/
            stringStream.str("");
            stringStream << " Local revision: ";
            if (localRev == 0)
                stringStream << "Undifined.";
            else
                stringStream << localRev;
            cout << "\033[31m" << "The local database with path " << localDbPath << " already exists."
                 << stringStream.str().c_str() << "\033[0m" << endl;
            //      if(exists_file("server_revision.txt"))
            //	std::remove("server_revision.txt");
            /*      gSystem->Exec(TString::Format("wget -O server_revision.txt \"%s/get_cur_timestamp.php\" > /dev/null
               2>&1", url)); std::ifstream myFile_Handler; std::string myLine; myFile_Handler.open("server_revision.txt");
                  if(myFile_Handler.is_open())
                  {
                char *ptr;
                    while(getline(myFile_Handler, myLine))
                    {
                      serverRev=strtoul(myLine.c_str(), &ptr, 10);
                  break;
                    }
                    myFile_Handler.close();
                  }*/
            serverRev = getServerRevision(url);
            if (serverRev == 0) {
                cerr << "The server revision is not detected. Please contact with administrator for check the problem.";
                return -2;
            }
            if (serverRev == localRev) {
                cout << "\033[32m" << "The current local version of geometry database is last. Instalation is stopped."
                     << "\033[0m" << std::endl;
                return 0;
            }

            cout << "\033[31m" << "The server revision: " << serverRev << "\033[0m" << endl;
            do {
                cout << "Do you want to update it[y/n]?" << "\033[0m" << endl;
                cin >> type;
            } while (!cin.fail() && type != 'y' && type != 'n');
            if (type == 'n') {
                cout << "Break install because local database already exist" << "\033[0m";
                return -1;
            } else if (type == 'y') {
            }
        }
    }

    // check whether the local database filepath is a directory
    FileStat_t file_stat;
    gSystem->GetPathInfo(localDbPath.Data(), file_stat);
    if (R_ISDIR(file_stat.fMode)) {
        cerr << "DBL_FILE_PATH is a path to a folder. Please, set DBL_FILE_PATH as a path to the local database file "
                "(default, 'local.db')"
             << endl;
        return -2;
    }

    // extract directory and file name from the path
    TString dirName(gSystem->DirName(localDbPath.Data()));
    TString fileName(gSystem->BaseName(localDbPath.Data()));
    // create the directory if not exists
    if (gSystem->AccessPathName(dirName.Data(), kFileExists))
    {
        if (gSystem->mkdir(dirName, true) != 0) {
            cerr << "ERROR: cannot create folder: " << dirName << endl;
            return -3;
        }
    }

    // get archive with the database file
    TString st = TString::Format("wget -O localdb.tar \"%s/download_file.php", url);
    if (tag == nullptr) st += "?type=all&tag=all\"";
    else st += TString::Format("?type=s&tag=%s\"", tag);
    st += "> /dev/null 2>&1";
    // cerr<<st<<endl;
    gSystem->Exec(st);

    if (gSystem->AccessPathName("localdb.tar", kFileExists)) {
        cerr << "ERROR: downloading the geometry database file failed, please, check url: " << url << endl;
        return -4;
    }
    gSystem->GetPathInfo("localdb.tar", file_stat);
    if (file_stat.fSize == 0) {
        cerr << "ERROR: downloaded geometry database file has 0 size, please, check url: " << url << endl;
        return -5;
    }

    // untar archive with the database file
    gSystem->Exec(TString::Format("tar -xvf localdb.tar -C \"%s/\" > /dev/null 2>&1", dirName.Data()));
    if (gSystem->AccessPathName(gSystem->ConcatFileName(dirName.Data(), "local.db"), kFileExists)) {
        cerr << "ERROR: unarchiving the geometry database failed, please, check url: " << url << endl;
        return -6;
    }

    // remove temporary archive
    gSystem->Exec("rm -rf localdb.tar");

    // move local.db file to the given path
    if (fileName != "local.db") {
        gSystem->CopyFile(gSystem->ConcatFileName(dirName.Data(), "local.db"),
                          gSystem->ConcatFileName(dirName.Data(), fileName.Data()), kTRUE);
        gSystem->Unlink(gSystem->ConcatFileName(dirName.Data(), "local.db"));
    }

    cout << "The Local Geometry Database was successfully installed to the directory: " << dirName << endl;
    return 0;
}
