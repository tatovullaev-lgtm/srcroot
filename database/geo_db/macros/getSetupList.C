#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <string>

// #define SQLITE_OPEN_URI              0x00000040
// #define SQLITE_VERSION_NUMBER        3005000
int getSetupList(const char* dbPath = NULL)
{
    /*  TClass* cl = TClass::GetClass("FairCave");
        std::cout<<"TClass:"<<cl<<std::endl;
        FairModule* f_cl = (FairModule*) cl->New();
        f_cl->SetName("Cave");
        std::cout<<"FairCave:"<<f_cl->GetName()<<std::endl;
    */
    std::ostringstream stringStream;
    if (dbPath != NULL)
        stringStream << "sqlite://" << dbPath;
    else
        stringStream << "sqlite://" << getenv("DBL_FILE_PATH");
    std::cout << stringStream.str().c_str() << std::endl;

    TSQLiteServer db(stringStream.str().c_str());   // TSQLiteServer db("sqlite://test.db");
    stringStream.str("");
    stringStream << "select setup_tag,creation_date,author,desc from setup";
    TSQLResult* resQ = db.Query(stringStream.str().c_str());
    if (resQ == 0 || resQ->GetRowCount() == 0) {
        std::cerr << "No setups in selected DB" << std::endl;
        return -1;
    }

    std::cout << "Setup list:" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "Tag                   Date           Author              Description" << std::endl;
    TSQLRow* row;
    while ((row = resQ->Next()) != 0) {
        const char* tag = row->GetField(0);
        std::cout << tag;
        int i = 0;
        for (i = strlen(tag); i < 22; i++)
            std::cout << " ";
        const char* date = row->GetField(1);
        std::cout << date;
        for (i = strlen(date); i < 15; i++)
            std::cout << " ";
        const char* author = row->GetField(2);
        std::cout << author;
        for (i = strlen(author); i < 20; i++)
            std::cout << " ";
        // if(row->IsValid(3)){
        try {
            const char* desc = row->GetField(3);
            if (desc != 0)
                std::cout << desc;
        } catch (...) {
        }
        std::cout << std::endl;
    }

    std::cout << "-----------------------------------------------------------------------------------" << std::endl;
    db.Close();
    return 0;
}
