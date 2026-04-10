#include "GeoSetup.h"

#include "BmnNewFieldMap.h"
#include "FairModule.h"
#include "FairRunAna.h"
#include "TGeoBBox.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoCone.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoTrd2.h"
#include "TGeoTube.h"
#include "TKey.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include "TSQLiteServer.h"
#include "TSystem.h"
#include "TXMLEngine.h"

#include <cctype>
#include <cstring>
#include <iostream>
#include <sys/stat.h> /* stat */
#include <time.h>     /* time */

bool GeoSetup::exists_file(const char* name)
{
    return (access(name, F_OK) != -1);
}

bool ichar_equals(char a, char b)
{
    return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
}

unsigned long int GeoSetup::GetServerRevision(const char* url)
{
    unsigned long int serverRev = 0;
    gSystem->Exec(TString::Format("wget -O server_revision.txt \"%s/get_cur_timestamp.php\" > /dev/null 2>&1", url));
    std::ifstream myFile_Handler;
    std::string myLine;
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

int GeoSetup::CheckTagOnServer(string_view setup_tag, const char* url)
{
    int retval = -1;
    gSystem->Exec(TString::Format("wget -O server_setup_list.txt \"%s/get_setup_list.php\" > /dev/null 2>&1", url));
    std::ifstream myFile_Handler;
    std::string myLine;
    std::string sTag(setup_tag);
    myFile_Handler.open("server_setup_list.txt");
    if (myFile_Handler.is_open()) {
        while (getline(myFile_Handler, myLine)) {
            vector<string> strings;
            istringstream f(myLine);
            string s;
            while (getline(f, s, ';')) {
                std::string::size_type pos = 0;
                if ((pos = s.find(':', pos)) != std::string::npos) {
                    std::string tag(s.substr(0, pos));
                    std::string status(s.substr(pos + 1));
                    bool bb = std::equal(tag.begin(), tag.end(), sTag.begin(), sTag.end(), ichar_equals);
                    if (tag.size() == sTag.size() && bb)
                        return std::stoi(status);
                }
            }
            break;
        }
        myFile_Handler.close();
        std::remove("server_setup_list.txt");
    }
    return retval;
}

unsigned long int GeoSetup::GetLocalRevision(TString& localDbPath)
{
    unsigned long int localRev = 0;
    // char type;
    if (exists_file(localDbPath)) {
        std::ostringstream stringStream;
        stringStream << "sqlite://" << localDbPath;
        TSQLiteServer db(stringStream.str().c_str());
        stringStream.str("");
        if (db.IsConnected()) {
            stringStream << "select value_field from config where key_field='config_time';";
            //cout << stringStream.str().c_str() << std::endl;
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

int GeoSetup::installLocalDB(string_view setup_tag, const char* url)
{
    CloseDB();
    TString localDbPath = gSystem->ExpandPathName(getenv("DBL_FILE_PATH"));
    if (localDbPath == "") {
        LOG(error) << "DBL_FILE_PATH is not set. Please, set DBL_FILE_PATH as a path to the local database file (default, "
                "'local.db')"
             << endl;
        return -1;
    }
    FileStat_t file_stat;
    gSystem->GetPathInfo(localDbPath.Data(), file_stat);
    if (R_ISDIR(file_stat.fMode)) {
        LOG(error) << "DBL_FILE_PATH is a path to a folder. Please, set DBL_FILE_PATH as a path to the local database file "
                "(default, 'local.db')"
             << endl;
        return -2;
    }

    TString dirName(gSystem->DirName(localDbPath.Data()));
    TString fileName(gSystem->BaseName(localDbPath.Data()));
    if (!exists_file(dirName.Data()))
        if (gSystem->mkdir(dirName, true) != 0) {
            LOG(error) << "ERROR: cannot create folder: " << dirName << endl;
            return -3;
        }
    TString command;
    if (setup_tag.size() == 0)
        command = TString::Format("wget -O testdb.tar \"%s/download_file.php?type=all&tag=all\" > /dev/null 2>&1", url);
    else {
        std::string tag(setup_tag);
        command = TString::Format("wget -O testdb.tar \"%s/download_file.php?type=s&tag=%s&rev=1\" > /dev/null 2>&1",
                                  url, tag.c_str());
    }
    gSystem->Exec(command);
    if (!exists_file("testdb.tar")) {
        LOG(error) << "ERROR: downloading the geometry database file failed, please, check url: " << url << endl;
        return -4;
    }
    gSystem->GetPathInfo("testdb.tar", file_stat);
    if (file_stat.fSize == 0) {
        LOG(error) << "ERROR: downloaded geometry database file has 0 size, please, check url: " << url << endl;
        return -5;
    }
    // untar archive with the database file
    gSystem->Exec(TString::Format("tar -xvf testdb.tar -C \"%s/\" > /dev/null 2>&1", dirName.Data()));
    // remove temporary archive
    gSystem->Exec("rm -rf testdb.tar");
    if (fileName != "local.db") {
        gSystem->CopyFile(gSystem->ConcatFileName(dirName.Data(), "local.db"),
                          gSystem->ConcatFileName(dirName.Data(), fileName.Data()), kTRUE);
        gSystem->Unlink(gSystem->ConcatFileName(dirName.Data(), "local.db"));
    }
    if (!OpenLocalGeoDatabase())
        return -6;
    LOG(info) << "The Local Geometry Database was successfully installed to the directory: " << dirName << endl;
    return 0;
}

void GeoSetup::CloseDB()
{
    m_modules_table.clear();
    m_modules_revision_table.clear();
    if (m_db != nullptr) {
        m_db->Close();
        delete m_db;
        m_db = nullptr;
    }
}

// GeoSetup destructor
GeoSetup::~GeoSetup()
{
    // CloseDB();
}

// create connection to the Local Geometry Database and fill the tables with modules and their revisions
bool GeoSetup::OpenLocalGeoDatabase()
{
    const char* env_p = getenv("DBL_FILE_PATH");
    if (env_p == nullptr) {
        LOG(error) << "Please, set DBL_FILE_PATH environment variable before using the Geometry Database";
        return false;
    }
    if (!exists_file(gSystem->ExpandPathName(env_p))) {
        LOG(error) << "No Database found in " << gSystem->ExpandPathName(env_p);
        return false;
    }
    ostringstream stringStream;
    stringStream << "sqlite://" << gSystem->ExpandPathName(env_p);

    LOG(debug1) << "Connecting the Local Geometry Database at path: " << stringStream.str().c_str();
    // std::cerr<<"Connecting the Local Geometry Database at path: "<<stringStream.str().c_str()<<std::endl;
    m_db = new TSQLiteServer(stringStream.str().c_str());
    if (PutModules() != 0) {
        LOG(error) << "Cannot load detector geometry, please, check DBL_FILE_PATH environment variable";
        return false;
    }
    if (PutModuleRevisions() != 0) {
        LOG(error) << "Cannot load detector geometry, please, check DBL_FILE_PATH environment variable";
        return false;
    }

    return true;
}

//-1 problem to load DB. Require to check DBL_FILE_PATH environment variable.
//-2 No geometry setup found for the given tag
//-3 Setup is damaged.
// load selected geometry setup from the Local Geometry Database and create gGeoManager instance
int GeoSetup::LoadSetupToGeoManager(string_view setup_tag)
{
    const char* env_p = getenv("DBL_FILE_PATH");
    if (env_p == nullptr) {
        LOG(error) << "Please, set DBL_FILE_PATH environment variable before using the Geometry Database";
        return -1;
    }
    bool isOpen = false;
    if (OpenLocalGeoDatabase())
        isOpen = true;
    TSQLResult* resQ = nullptr;
    TSQLRow* row = nullptr;
    ostringstream stringStream;
    // if (revision == nullptr)
    stringStream << "select setup_id,status,material_id,field_id,file_name from setup where lower(setup_tag)=lower('"
                 << setup_tag << "') order by creation_date desc;";
    // else
    //     stringStream<<"select idsd,status,idma,idfi,rfile from setup where lower(stag)=lower('"<<setupTag<<"') and
    //     TRIM(revision)=TRIM('"<<revision<<"') order by sdate desc;";
    if (isOpen) {
        resQ = m_db->Query(stringStream.str().c_str());
        if (resQ == nullptr) {
            LOG(error) << "SQL query was failed while searching the given tag: `" << setup_tag << "`";
            return -2;
        }
        row = resQ->Next();
    }
    if (row == nullptr) {
        LOG(error) << "No geometry setup found in the local database for `" << setup_tag << "` tag.";
        delete resQ;
        resQ = nullptr;
        int status = CheckTagOnServer(setup_tag);
        if (status >= 0) {
            int isUpload = -1;
            if (status == 1) {
                LOG(info) << "Approved geometry setup found on the geometry server:`" << GEO_SERVER_HOST
                          << "`. Downloading...";
                isUpload = installLocalDB("");
            } else {
                LOG(info) << "Not approved geometry setup found on the geometry server:`" << GEO_SERVER_HOST
                          << "`. Downloading";
                isUpload = installLocalDB(setup_tag);
            }
            if (isUpload >= 0) {
                resQ = m_db->Query(stringStream.str().c_str());
                row = resQ->Next();
            } else {
                LOG(error) << "Cannot download the geometry setup from the server:`" << GEO_SERVER_HOST << "`.";
                return -2;
            }
        } else
            LOG(error) << "No geometry setup found on the geometry server:`" << GEO_SERVER_HOST << "`.";
        if (row == nullptr)
            return -2;
    }

    m_sid = atoi(row->GetField(0));
    // int idma = atoi(row->GetField(2));
    m_fid = atoi(row->GetField(3));
    TString geoFileName = GetRootPath() + row->GetField(4);
    delete row;
    delete resQ;

    TFile* geoFile = new TFile(geoFileName, "READ");
    if (!geoFile->IsOpen()) {
        LOG(error) << "ERROR: could not open ROOT file with geometry: " + geoFileName;
        return -3;
    }
    TList* keyList = geoFile->GetListOfKeys();
    TIter next(keyList);
    TKey* key = (TKey*)next();
    TString className(key->GetClassName());
    if (className.BeginsWith("TGeoManager"))
        key->ReadObj();
    else {
        LOG(error) << "ERROR: TGeoManager is not top element in geometry file " + geoFileName;
        return -3;
    }

    return 0;
}

// load selected geometry for simulation application from the Geometry Database.
int GeoSetup::LoadSetupToFairRunSim(string_view setup_tag, const char* localSettings)
{
    if (localSettings != nullptr)
        LoadLocalSettings(localSettings);

    // int random_variable;
    /*    if (url != nullptr)
        {
            srand(time(NULL));
            random_variable = rand();
            //cout<<random_variable<<endl;
            ostringstream stringStream;
            //  stringStream<<"wget -O test2.tar \""<<url<<"?type=s&tag="<<setupTag<<"&dirname="<<random_variable<<"\"";
       http://cbmdb.jinr.ru/geometry8/download_file.php?type=all&tag=all stringStream<<"wget -O test2.tar
       \""<<url<<"/download_file.php?type=all&tag=all&dirname="<<random_variable<<"\" >/dev/null ";
            system(stringStream.str().c_str());

            stringStream.str("");
            stringStream<<"tar -xf test2.tar >/dev/null";
            system(stringStream.str().c_str());

            stringStream.str("");
            stringStream<<"rm test2.tar";
            system(stringStream.str().c_str());

            stringStream.str("");
            stringStream<<gSystem->pwd()<<"/"<<random_variable<<"/storage/";
            gSystem->Setenv("DBL_ROOT_PATH", stringStream.str().c_str());

            stringStream.str("");
            stringStream<<gSystem->pwd()<<"/"<<random_variable<<"/local.db";
            gSystem->Setenv("DBL_FILE_PATH", stringStream.str().c_str());
        }*/
    const char* env_p = getenv("DBL_FILE_PATH");
    if (env_p == nullptr) {
        LOG(error) << "Please, set DBL_FILE_PATH environment variable before using the Geometry Database";
        return -1;
    }
    bool isOpen = false;
    if (OpenLocalGeoDatabase())
        isOpen = true;
    TSQLResult* resQ = nullptr;
    TSQLRow* row = nullptr;

    ostringstream stringStream;
    //    if (revision == nullptr)
    stringStream << "select setup_id,status,material_id,field_id from setup where lower(setup_tag)=lower('" << setup_tag
                 << "') order by creation_date desc;";   // KG если я правильно понял, то нужно брать по-умолчанию,
                                                         // самый новый релиз?
    //    else
    //        stringStream<<"select idsd,status,idma,idfi from setup where stag='"<<setupTag<<"' and
    //        TRIM(revision)=TRIM('"<<revision<<"') order by sdate desc;";
    LOG(debug1) << stringStream.str().c_str();
    if (isOpen) {
        resQ = m_db->Query(stringStream.str().c_str());
        if (resQ == nullptr) {
            LOG(error) << "SQL query was failed while searching the given tag: `" << setup_tag << "`";
            return -2;
        }
        row = resQ->Next();
    }
    if (row == nullptr) {
        LOG(error) << "No geometry setup found in the local database for `" << setup_tag << "` tag.";
        delete resQ;
        resQ = nullptr;
        int status = CheckTagOnServer(setup_tag);
        if (status >= 0) {
            int isUpload = -1;
            if (status == 1) {
                LOG(info) << "Approved geometry setup found on the geometry server:`" << GEO_SERVER_HOST
                          << "`. Downloading...";
                isUpload = installLocalDB("");
            } else {
                LOG(info) << "Not approved geometry setup found on the geometry server:`" << GEO_SERVER_HOST
                          << "`. Downloading...";
                isUpload = installLocalDB(setup_tag);
            }
            if (isUpload >= 0) {
                resQ = m_db->Query(stringStream.str().c_str());
                row = resQ->Next();
            } else {
                LOG(error) << "Cannot download the geometry setup from the server:`" << GEO_SERVER_HOST << "`.";
                return -2;
            }
        } else
            LOG(error) << "No geometry setup found on the geometry server:`" << GEO_SERVER_HOST << "`.";
        if (row == nullptr)
            return -2;
    }

    FairRunSim* fRun = FairRunSim::Instance();
    if (fRun == nullptr) {
        fRun = new FairRunSim();
        LOG(info) << "New FairRunSim instance has been created";
    }

    m_sid = atoi(row->GetField(0));
    int idma = atoi(row->GetField(2));
    m_fid = atoi(row->GetField(3));
    if (idma > 0) {
        delete row;
        delete resQ;

        stringStream.str("");
        stringStream << "select matag,file_name from material where material_id=" << idma << ";";
        resQ = m_db->Query(stringStream.str().c_str());

        if (resQ == 0 || resQ->GetRowCount() == 0)
            LOG(info) << "Materials were not found";
        else {
            row = resQ->Next();
            const char* media_path = row->GetField(1);

            TString Mat = getenv("GEOMPATH");
            if (Mat.IsNull()) {
                Mat = ((TString)getenv("VMCWORKDIR")) + "/geometry/";
                Mat.ReplaceAll("//", "/");
            } else {
                Mat.ReplaceAll("//", "/");
                if (!Mat.EndsWith("/"))
                    Mat += "/";
            }

            string workP(Mat.Data());
            LOG(debug1) << "Working geometry directory: " << workP;

            size_t found = workP.find('/');
            TString in("");
            if (found != string::npos) {
                while ((found = workP.find('/', found + 1)) != string::npos) {
                    LOG(debug1) << "found:" << found;
                    in += "../";
                }
            }
            in += GetRootPath() + media_path;
            // in= GetRootPath() + media_path;
            fRun->SetMaterials(
                in.Data());   // KG
                              // /home/soul/bmnroot/geometry/../../../..//home/soul/bmnroot/database/geo_db/storage/07022020_085550_media.geo?

            delete row;
            delete resQ;
        }
    } else {
        delete row;
        delete resQ;
    }

    LOG(debug1) << "setup_id = " << m_sid << "; material_id = " << idma << "; filed_id = " << m_fid << endl;
    stringStream.str("");
    stringStream << "select setup_module_id from sms where setup_id=" << m_sid << ";";
    resQ = m_db->Query(stringStream.str().c_str());
    if (resQ == 0 || resQ->GetRowCount() == 0) {
        LOG(error) << "ERROR: no modules found";
        return -3;
    }
    while ((row = resQ->Next()) != nullptr) {
        stringStream.str("");
        const char* idsm = row->GetField(0);
        int idsmI = atoi(idsm);
        LOG(debug1) << "module=" << idsmI << std::endl;
        // if (subSetup < 0 || subSetup == idsmI){ //check subset
        int res = LoadModuleToFairRunSim(idsmI, fRun);
        if (res == -1) {
            LOG(error) << "ERROR: no file for the setup module " << idsmI << " was found";
            return -3;
        }
        delete row;
    }
    delete resQ;

    /*   if (url != nullptr)
       {
           stringStream.str("");
           stringStream<<"rm -rf "<<random_variable<<"/local.db";
           system(stringStream.str().c_str());

           stringStream.str("");
           stringStream<<"rm -rf "<<random_variable;
           system(stringStream.str().c_str());

           cout<<"DBL_ROOT_PATH="<<getRootPath()<<endl; //KG should be DBL_PATH variables restored?
       }*/

    return 0;
}

// get field map of selected geometry
FairField* GeoSetup::GetMagneticField(double scale)
{
    // return magnetic field if exists
    if (m_field != nullptr)
        return m_field;

    if (m_db == nullptr) {
        LOG(error) << "Geometry Database is not loaded";
        return nullptr;
    }
    if (m_fid == -1) {
        LOG(debug1) << "Selected Geometry Database has no magnetic field";
        return nullptr;
    }

    // get field map for the current id from the Geometry Database
    ostringstream stringStream;
    stringStream << "select file_name,scale,x,y,z from field where field_id=" << m_fid << ";";
    TSQLResult* resQ = m_db->Query(stringStream.str().c_str());
    if (resQ == nullptr) {
        LOG(error) << "SQL query was failed while searching magnetic field for id: `" << m_fid << "`";
        return nullptr;
    }
    if (resQ->GetRowCount() == 0) {
        LOG(error) << "ERROR: no selected magnetic field in the Geometry Database for id=" << m_fid;
        delete resQ;
        return nullptr;
    }

    TSQLRow* row = resQ->Next();
    if (row == nullptr) {
        LOG(error) << "ERROR: no selected magnetic field in the Geometry Database for id=" << m_fid;
        delete resQ;
        return nullptr;
    }

    // const char* url = row->GetField(0);
    stringStream.str("");
    stringStream << GetRootPath().c_str() << row->GetField(0);
    if (access(stringStream.str().c_str(), F_OK) == -1) {
        LOG(error) << "ERROR: File with the magnetic field was not found. The url=" << stringStream.str().c_str();
        delete row;
        delete resQ;
        return nullptr;
    }
    double sc = atof(row->GetField(1));
    delete row;
    delete resQ;

    // to be changed to dynamic magnetic class got by the Geometry Database
    BmnNewFieldMap* pBmnNewFieldMap = new BmnNewFieldMap();
    pBmnNewFieldMap->SetFileName(stringStream.str().c_str());
    // scale the magnetic field
    if (scale > 0)
        pBmnNewFieldMap->SetScale(scale);
    else
        pBmnNewFieldMap->SetScale(sc);

    m_field = pBmnNewFieldMap;
    return m_field;
}

// get parameter file by given detector name
string_view GeoSetup::GetParameterFile(string_view name)
{
    if (name == "") {
        LOG(error) << "Detector name for the parameter file is not set";
        return "";
    }
    if (m_db == nullptr) {
        LOG(error) << "The Geometry Database is not connected";
        return "";
    }

    TString sname(name);
    int index = GetModuleIndex(sname);
    if (index < 0) {
        LOG(error) << "The detector name \'" << sname << "\'does not exist in the Geometry Database";
        return "";
    }
    LOG(debug1) << "The detector name \'" << sname << "\'exists in the Geometry Database";

    ostringstream stringStream;
    stringStream
        << "select sm.fpar from sms s INNER JOIN setup_module sm on s.idsm=sm.idsm INNER JOIN file f on sm.idf=f.idf "
           "INNER JOIN module_revision mr on f.idm=mr.idmr where idsd="
        << m_sid << " and mr.idm=" << index << ";";
    LOG(debug1) << "Request \'" << stringStream.str().c_str() << "\'.";
    TSQLResult* resQ = m_db->Query(stringStream.str().c_str());
    if (resQ == nullptr) {
        LOG(info) << "SQL query was failed while searching parameter file for detector `" << sname << "`";
        return "";
    }
    TSQLRow* row = resQ->Next();
    if (row == nullptr) {
        LOG(info) << "Parameter file for detector `" << sname << "` not found";
        return "";
    }

    const char* file = row->GetField(0);
    delete row;
    delete resQ;

    if (file == nullptr) {
        LOG(info) << "Parameter file for detector `" << sname << "` does not exist in the Geometry Database";
        return "";
    }

    return file;
}

// get path to the location of the Local Geometry Database
string GeoSetup::GetRootPath()
{
    string rootPath = gSystem->ExpandPathName(getenv("DBL_FILE_PATH"));

    struct stat st;
    if (stat(rootPath.c_str(), &st) == 0)
        if ((st.st_mode & S_IFDIR) != 0) {
            LOG(error) << "ERROR: DBL_FILE_PATH variable points to the folder. Please, set DBL_FILE_PATH to file path "
                          "with local geometry database";
            return "";
        }

    int last = rootPath.find_last_of("/\\");
    if (last == -1)
        rootPath = ".";
    else
        rootPath = rootPath.substr(0, last);
    rootPath += "/storage/";

    return rootPath;
}

// get path to a temporary directory (/tmp/$USER/), and create if absent
TString& GeoSetup::GetTmpPath()
{
    if (m_tmpFolder.Length() != 0)
        return m_tmpFolder;

    UserGroup_t* user = gSystem->GetUserInfo();
    string login(user->fUser.Data());
    m_tmpFolder = "/tmp/" + login + "/";
    system("mkdir -p " + m_tmpFolder);

    return m_tmpFolder;
}

// load local settings from the file for all setup modules
bool GeoSetup::LoadLocalSettings(const char* fileName)
{
    TXMLEngine* xml = new TXMLEngine;
    XMLDocPointer_t xmldoc = xml->ParseFile(fileName);
    if (xmldoc == 0) {
        LOG(error) << "Selected file has not XML format: " << fileName;
        delete xml;
        return false;
    }

    m_moduleEdit.clear();

    XMLNodePointer_t mainnode = xml->DocGetRootElement(xmldoc);
    XMLNodePointer_t child = xml->GetChild(mainnode);
    while (child != 0) {
        TString chName = xml->GetNodeName(child);
        if (chName.CompareTo("setupModule") == 0) {
            setup_module_ptr sNode = new setup_module();
            sNode->xMove = 0;
            sNode->yMove = 0;
            sNode->zMove = 0;
            sNode->enable = true;
            sNode->type = TString("");

            XMLAttrPointer_t attr = xml->GetFirstAttr(child);
            while (attr != 0) {
                TString aName = xml->GetAttrName(attr);
                TString aValue = xml->GetAttrValue(attr);

                if (aName.CompareTo("type") == 0)
                    sNode->type = aValue;
                else if (aName.CompareTo("moveX") == 0) {
                    sNode->xMove = aValue.Atof();
                } else if (aName.CompareTo("moveY") == 0) {
                    sNode->yMove = aValue.Atof();
                } else if (aName.CompareTo("moveZ") == 0) {
                    sNode->zMove = aValue.Atof();
                }
                if ((aName.CompareTo("enable") == 0)
                    && (aValue.CompareTo("false") == 0 || aValue.CompareTo("False") == 0
                        || aValue.CompareTo("FALSE") == 0))
                    sNode->enable = false;

                LOG(debug1) << "attr: " << xml->GetAttrName(attr) << " value: " << xml->GetAttrValue(attr);

                attr = xml->GetNextAttr(attr);
            }

            int ii = GetModuleIndex(sNode->type);
            if (ii != -1)
                m_moduleEdit.insert(pair<int, setup_module_ptr>(ii, sNode));
        }
        child = xml->GetNext(child);
    }

    xml->FreeDoc(xmldoc);
    delete xml;
    return true;
}

// get module name by index; return "Unknown" if not found
TString GeoSetup::GetModuleName(int dbModuleUndex)
{
    map<int, TString>::iterator it = m_modules_table.find(dbModuleUndex);
    if (it != m_modules_table.end())
        return it->second;
    else {
        LOG(error) << "Unknown module type with id=" << dbModuleUndex;
        return TString("Unknown");
    }
}

// get module index by name; return -1 if not found
int GeoSetup::GetModuleIndex(TString& name)
{
    map<int, TString>::iterator it;
    for (it = m_modules_table.begin(); it != m_modules_table.end(); ++it)
        if ((it->second).CompareTo(name, TString::ECaseCompare::kIgnoreCase) == 0)
            return it->first;

    return -1;
}

// get module_r_ptr (pointer to the module revision) by revision number
module_r_ptr GeoSetup::GetModuleRevision(int dbModuleRev)
{
    map<int, module_r_ptr>::iterator it = m_modules_revision_table.find(dbModuleRev);
    if (it != m_modules_revision_table.end())
        return it->second;   //.class_name;

    module_r_ptr mr = nullptr;
    return mr;
}

// get module ID by revision number; return -1 if not found
int GeoSetup::GetModuleId(int dbModuleRev)
{
    map<int, module_r_ptr>::iterator it = m_modules_revision_table.find(dbModuleRev);
    if (it != m_modules_revision_table.end())
        return it->second->idm;

    return -1;
}

// get module type by revision number
TString GeoSetup::GetModuleType(int dbModuleRev)
{
    return GetModuleName(GetModuleId(dbModuleRev));
}

// get root volume in the TFile object (first search for TGeoManager)
TGeoVolume* GeoSetup::GetRootVolume(TFile* f)
{
    TKey* key;
    TGeoManager* geoManager = nullptr;
    TList* l = f->GetListOfKeys();
    TIter next(l);
    while ((key = (TKey*)next())) {
        if (strcmp(key->GetClassName(), "TGeoManager") != 0)
            continue;
        geoManager = (TGeoManager*)key->ReadObj();
        geoManager->cd();
        return (TGeoVolume*)geoManager->GetNode(0)->GetDaughter(0)->GetVolume();
    }

    // if TGeoManager was not found in the file
    key = (TKey*)l->At(0);   // Get the first key in the list
    TGeoVolume* volume = dynamic_cast<TGeoVolume*>(key->ReadObj());

    return volume;
}

// add FairModule defined by revision number and geometry file path to the FairRunSim instance
bool GeoSetup::AddModuleToFairRunSim(int dbModuleRev, const char* path, FairRunSim* fRun)
{
    module_r_ptr mr = GetModuleRevision(dbModuleRev);

    // TClass* cl = TClass::GetClass(mr->class_name);
    // FairModule* f_cl = (FairModule*) cl->New();
    // cout<<"TClass:"<<cl<<endl; f_cl->Dump();

    TString type_name;
    if (mr->args.Length() > 0) {
        type_name = mr->args;
        // f_cl->SetName(mr.args);
    } else {
        type_name = GetModuleType(dbModuleRev);
        // f_cl->SetName(getModuleType(dbModuleRev));
    }

    TString command = "new " + mr->class_name + "(\"" + type_name + "\"";
    if (mr->active)
        command += ",kTRUE)";
    else
        command += ")";
    LOG(debug1) << "command:" << command;

    FairModule* obj = (FairModule*)gInterpreter->Calc(command);
    // cerr<<"OBJ="<<obj<<endl; obj->Dump();
    obj->SetGeometryFileName(path);
    fRun->AddModule(obj);

    // cerr<<"dbModuleRev="<<dbModuleRev<<" idmr="<<mr.idmr<<" idm="<<mr.idm<<" class_name="<<typeid(f_cl).name()<<endl;
    return true;
}

TGeoNode* GeoSetup::ConstructRootGeometry(const char* fileName)
{
    TGeoManager* NewGeo = 0;
    TGeoVolume* volume = 0;
    ;
    TFile* f = new TFile(fileName);
    TList* l = f->GetListOfKeys();
    TKey* key;
    TIter next(l);
    TGeoNode* n = 0;
    // TGeoVolume* v1=0;
    while ((key = (TKey*)next())) {
        if (strcmp(key->GetClassName(), "TGeoManager") != 0) {
            continue;
        }
        NewGeo = (TGeoManager*)key->ReadObj();
        break;
    }
    if (NewGeo != 0) {
        NewGeo->cd();
        n = NewGeo->GetNode(0);
        f->Close();
        delete f;
        return n;
    } else {
        key = (TKey*)l->At(0);   // Get the first key in the list
        volume = dynamic_cast<TGeoVolume*>(key->ReadObj());
        if (volume != 0) {
            f->Close();
            delete f;
            return volume->GetNode(0);
        }   // n=volume->GetNode(0); }
        f->Close();
        delete f;
        return NULL;
    }
}

bool GeoSetup::IsEqlMatrix(const char* fileName, TSQLRow* row)
{
    if (row == NULL || fileName == NULL)
        return false;
    bool res = true;
    try {
        // cerr<<"Start compare fileName:"<<fileName<<endl;
        TGeoNode* node = ConstructRootGeometry(fileName);
        if (node == NULL)
            return false;
        const Double_t* tran = node->GetMatrix()->GetTranslation();   // 3
        // const Double_t * sc = node->GetMatrix()->GetScale();//3
        const Double_t* rot = node->GetMatrix()->GetRotationMatrix();   // 9
        unsigned int i;
        for (i = 0; i < 3; i++) {
            double t = (double)atof(row->GetField(3 + i));
            if (tran[i] != t) {
                LOG(debug1) << " for i=" << i << " tran[i]=" << tran[i] << " t=" << t << endl;
                res = false;
            }
        }
        for (i = 0; i < 9; i++) {
            double t = (double)atof(row->GetField(6 + i));
            if (rot[i] != t) {
                LOG(debug1) << " for i=" << i << " rot[i]=" << rot[i] << " t=" << t << endl;
                res = false;
            }
        }
    } catch (...) {
        return false;
    }
    return res;
}
// load one module from the Geometry Database and add to the FairRunSim instance
int GeoSetup::LoadModuleToFairRunSim(int idsm, FairRunSim* fRun)
{
    map<int, TString>::iterator it = m_loadedModule.find(idsm);
    if (it != m_loadedModule.end())
        return 0;

    LOG(debug1) << "Start load idsm=" << idsm;
    LOG(debug1) << "Start load setup_module_id=" << idsm;
    ostringstream stringStream;
    stringStream << "select f.module_id,f.file_name,sm.parent_id,sm.tx,sm.ty,sm.tz,r11,r12,r13,r21,r22,r23,r31,r32,r33 "
                    "from setup_module sm INNER JOIN geometry_file f ON f.file_id=sm.file_id where sm.setup_module_id="
                 << idsm << ";";
    //cerr << stringStream.str() << endl;

    TSQLResult* resQ = m_db->Query(stringStream.str().c_str());
    if (resQ == 0 || resQ->GetRowCount() == 0) {
        LOG(error) << "ERROR: no file for the setup module (" << idsm << ") was found";
        return -1;
    }
    // cerr<<"rows="<<resQ->GetRowCount()<<endl;

    TSQLRow* row = resQ->Next();
    int dbModuleIndex = atoi(row->GetField(0));
    int idP = atoi(row->GetField(2));
    LOG(debug1) << "idP=" << idP;
    if (idP != -1) {
        int res = LoadModuleToFairRunSim(idP, fRun);
        if (res < 0)
            return res;
    }

    stringStream.str("");
    LOG(debug1) << row->GetField(1);
    stringStream << GetRootPath().c_str() << row->GetField(1);
    // cerr<<"getRootPath()="<<GetRootPath().c_str()<<endl<<"File path="<<stringStream.str().c_str()<<endl;

    TString mName = GetModuleType(dbModuleIndex), out = "";
    mName.ToUpper();
    if (mName == TString("CAVE") || ((TString)stringStream.str()).EndsWith(".geo"))
        AddModuleToFairRunSim(dbModuleIndex, stringStream.str().c_str(), fRun);
    else {
        double mx = 0, my = 0, mz = 0;
        map<int, setup_module_ptr>::iterator itr = m_moduleEdit.find(dbModuleIndex);
        LOG(debug1) << "detector " << dbModuleIndex;
        if (itr != m_moduleEdit.end()) {
            LOG(debug1) << "detector was found";
            if (itr->second->enable) {
                mx = itr->second->xMove;
                my = itr->second->yMove;
                mz = itr->second->zMove;
                LOG(debug1) << "Move detector " << dbModuleIndex << " to x:" << mx << " to y:" << my << " to z:" << mz;
            }
        }
        if (mx == 0 && my == 0 && mz == 0 && IsEqlMatrix(stringStream.str().c_str(), row)) {
            LOG(debug1) << "The same matrix for file=" << stringStream.str().c_str() << endl;
            AddModuleToFairRunSim(dbModuleIndex, stringStream.str().c_str(), fRun);
        } else {
            TGeoManager* geom = new TGeoManager(mName, TString::Format("Geometry of the %s module", mName.Data()));
            LOG(debug1) << "import file" << stringStream.str().c_str() << endl;
            // TGeoManager::Import(stringStream.str().c_str());
            TFile* f = new TFile(stringStream.str().c_str());
            TGeoVolume* root_volume = GetRootVolume(f);
            double tx = (double)atof(row->GetField(3)) + mx;
            double ty = (double)atof(row->GetField(4)) + my;
            double tz = (double)atof(row->GetField(5)) + mz;
            TGeoRotation rotM;
            TGeoGenTrans* newMatrix = new TGeoGenTrans(tx, ty, tz, 1, 1, 1, &rotM);
            if (strcmp(root_volume->GetNode(0)->ClassName(), "TGeoNodeMatrix") == 0) {
                TGeoNodeMatrix* v = (TGeoNodeMatrix*)root_volume->GetNode(0);
                // cerr<<"Matrix changed"<<endl;
                v->SetMatrix(newMatrix);
            }

            // geom->CloseGeometry();
            out = GetTmpPath() + GetModuleName(GetModuleId(dbModuleIndex)) + ".root";
            root_volume->Export(out.Data());
            // gGeoManager->Export(out.Data());
            AddModuleToFairRunSim(dbModuleIndex, out.Data(), fRun);
            LOG(debug1) << "Exporting geometry to ROOT file: " << out;

            delete geom;
            f->Close();
            delete f;
        }
    }
    m_loadedModule.insert(pair<int, TString>(idsm, out));

    delete row;
    delete resQ;
    return 0;
}

// fill table with modules (m_modules_table) from the Geometry Database
int GeoSetup::PutModules()
{
    TSQLResult* resQ = m_db->Query("select module_id,module_name from module;");
    if (resQ == nullptr) {
        LOG(error) << "ERROR: SQL query was failed. Please, check connection to the Geometry Database and "
                      "DBL_FILE_PATH variable is correct";
        return -1;
    }
    if (resQ->GetRowCount() == 0) {
        LOG(error) << "ERROR: no modules in the Geometry Database";
        delete resQ;
        return -2;
    }

    TSQLRow* row;
    while ((row = resQ->Next()) != 0) {
        int id = atoi(row->GetField(0));
        TString name(row->GetField(1));
        m_modules_table.insert(make_pair(id, name));
        delete row;
    }

    delete resQ;
    return 0;
}

// fill table with module revisions (m_modules_revision_table) from the Geometry Database
int GeoSetup::PutModuleRevisions()
{
    TSQLResult* resQ = m_db->Query("select module_class_id,mc.module_id,class_name,revision,active,object_name from "
                                   "module_class as mc INNER JOIN module as m ON  mc.module_id=m.module_id;");
    if (resQ == nullptr) {
        LOG(error) << "ERROR: SQL query was failed. Please, check connection to the Geometry Database and "
                      "DBL_FILE_PATH variable is correct";
        return -1;
    }
    if (resQ->GetRowCount() == 0) {
        LOG(error) << "ERROR: no module revisions in the geometry database";
        delete resQ;
        return -2;
    }

    TSQLRow* row;
    while ((row = resQ->Next()) != 0) {
        module_r_ptr mr = new module_r();
        mr->idmr = atoi(row->GetField(0));
        mr->idm = atoi(row->GetField(1));
        mr->class_name = row->GetField(2);
        mr->rev = atoi(row->GetField(3));
        if (atoi(row->GetField(4)) == 0)
            mr->active = false;
        else
            mr->active = true;
        mr->args = row->GetField(5);

        m_modules_revision_table.insert(make_pair(mr->idmr, mr));
        delete row;
    }

    delete resQ;
    return 0;
}
