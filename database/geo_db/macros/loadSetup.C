#include <cstring>
#include <fcntl.h>   // open
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/sendfile.h>   // sendfile
#include <sys/stat.h>       // fstat
#include <sys/types.h>      // fstat
#include <time.h>           /* time */
#include <unistd.h>         // close
#include <unistd.h>
typedef std::pair<int, TString> moduleP;

#include "TXMLEngine.h"

struct SETUPMODULE
{
    TString type;
    double xMove;
    double yMove;
    double zMove;
    bool enable;
} setup_module, *setup_module_ptr;

struct MODULEREVISION
{
    TString class_name;
    int idmr;
    int idm;
    int rev;
    bool active;
    TString args;
} module_r, *module_r_ptr;

std::map<int, TString> m_modules_table;
std::map<int, MODULEREVISION> m_modules_revision_table;

std::string getRootPath()
{
    // return getenv("DBL_ROOT_PATH");
    std::string rootPath = getenv("DBL_FILE_PATH");
    struct stat st;
    if (stat(rootPath.c_str(), &st) == 0)
        if ((st.st_mode & S_IFDIR) != 0) {
            std::cerr << "Error: DBL_FILE_PATH variable points to the folder. Please, set DBL_FILE_PATH to file path "
                         "with local geometry database"
                      << std::endl;
            std::exit(-1);
        }
    int last = -1;
    last = rootPath.find_last_of("/\\");
    std::string fileName;
    if (last == -1) {
        fileName = rootPath;
        rootPath = ".";
    } else {
        fileName = rootPath.substr(last + 1);
        rootPath = rootPath.substr(0, last);
    }
    rootPath += "/storage/";
    // std::cerr<<"rootPath="<<rootPath<<std::endl;
    return rootPath;
}

bool exists_file(const char* name)
{
    return (access(name, F_OK) != -1);
}

TString getModuleName(int dbNoduleUndex)
{
    std::map<int, TString>::iterator it = m_modules_table.find(dbNoduleUndex);
    if (it != m_modules_table.end())
        return it->second;
    else {
        std::cerr << "Unknown module type with id=" << dbNoduleUndex << std::endl;
        return TString("Unknown");
    }
}

int getModuleIndex(TString& name)
{
    std::map<int, TString>::iterator it;
    for (it = m_modules_table.begin(); it != m_modules_table.end(); ++it)
        if (it->second == name)
            return it->first;

    return -1;
}

MODULEREVISION getModuleRevision(int dbModuleRev)
{
    std::map<int, MODULEREVISION>::iterator it = m_modules_revision_table.find(dbModuleRev);
    if (it != m_modules_revision_table.end())
        return it->second;   //.class_name;

    MODULEREVISION mr;
    return mr;
}

int getModuleId(int dbModuleRev)
{
    std::map<int, MODULEREVISION>::iterator it = m_modules_revision_table.find(dbModuleRev);
    if (it != m_modules_revision_table.end())
        return it->second.idm;

    return -1;
}

TString getModuleType(int dbModuleRev)
{
    return getModuleName(getModuleId(dbModuleRev));
}

bool loadFairModule(FairRunSim* fRun,
                    int dbModuleRev,
                    const char* path,
                    double psdx,
                    double psdz,
                    int iVerbose = 0)   // KG psdx и psdz не используются?
{
    MODULEREVISION mr = getModuleRevision(dbModuleRev);
    int dbNoduleUndex = mr.idm;
    TString mName = getModuleType(dbModuleRev);
    mName.ToUpper();

    TString type_name;
    TClass* cl = TClass::GetClass(mr.class_name);
    // std::cout<<"TClass:"<<cl<<std::endl;
    FairModule* f_cl = (FairModule*)cl->New();
    if (mr.args.Length() > 0) {
        type_name = mr.args;
        // f_cl->SetName(mr.args);
    } else {
        // f_cl->SetName(getModuleType(dbModuleRev));
        type_name = getModuleType(dbModuleRev);
    }
    TString command = "new " + mr.class_name + "(\"" + type_name + "\"";   //,kTrue)";
    if (mr.active)
        command += ",kTRUE)";
    else
        command += ")";
    if (iVerbose > 0)
        std::cerr << "command:" << command << std::endl;
    FairModule* obj = (FairModule*)gInterpreter->Calc(command);

    // std::cerr<<"OBJ="<<obj<<std::endl;
    obj->SetGeometryFileName(path);
    fRun->AddModule(obj);

    // obj->Dump();
    // std::cerr<<"dbModuleRev="<<dbModuleRev<<" idmr="<<mr.idmr<<" idm="<<mr.idm<<"
    // class_name="<<typeid(f_cl).name()<<std::endl; f_cl->Dump();
    return true;
}

int putMudulesRevision4SQLite(TSQLiteServer& db)
{
    std::ostringstream stringStream;
    stringStream << "select idmr,idm,class_name,revision,active,args from module_revision;";
    TSQLResult* resQ2 = db.Query(stringStream.str().c_str());
    if (resQ2 == 0) {
        std::cout << "Error: SQL query was failed. "
                     "Please, check connection to the geometry database and that DBL_FILE_PATH variable is correct"
                  << std::endl;
        return -1;
    }
    if (resQ2->GetRowCount() == 0) {
        std::cout << "Error: no module revisions in the geometry database" << std::endl;
        return -2;
    }

    TSQLRow* row;
    while ((row = resQ2->Next()) != 0) {
        int idmr = atoi(row->GetField(0));
        int idm = atoi(row->GetField(1));
        int rev = atoi(row->GetField(3));
        int act = atoi(row->GetField(4));
        TString class_name(row->GetField(2));
        TString args(row->GetField(5));

        MODULEREVISION mr;
        mr.idmr = idmr;
        mr.rev = rev;
        mr.idm = idm;
        mr.class_name = class_name;
        mr.args = args;
        if (act == 0)
            mr.active = false;
        else
            mr.active = true;
        m_modules_revision_table.insert(make_pair(idmr, mr));
    }

    return 0;
}

int putMudules4SQLite(TSQLiteServer& db)
{
    std::ostringstream stringStream;
    stringStream << "select idm,mname from module;";
    TSQLResult* resQ2 = db.Query(stringStream.str().c_str());
    if (resQ2 == 0) {
        std::cout << "Error: SQL query was failed. "
                     "Please, check connection to the geometry database and that DBL_FILE_PATH variable is correct"
                  << std::endl;
        return -1;
    }
    if (resQ2->GetRowCount() == 0) {
        std::cout << "Error: no modules in the geometry database" << std::endl;
        return -2;
    }

    TSQLRow* row;
    while ((row = resQ2->Next()) != 0) {
        int id = atoi(row->GetField(0));
        TString name(row->GetField(1));
        m_modules_table.insert(make_pair(id, name));
    }

    return 0;
}

/*
CbmFieldMap* CreateFieldMap(TString& fieldTag) {
  TString mapName = "";//"field_";
  mapName += fieldTag;
  TString fileName = gSystem->Getenv("VMCWORKDIR");
  fileName = fileName + "/input/" + mapName + ".root";
  TFile mapFile(fileName);
  if ( ! mapFile.IsOpen() ) {
    LOG(ERROR) << "Could not open field map file " << fileName
               << FairLogger::endl;
    return NULL;
  }
  CbmFieldMapData* data = NULL;
  mapFile.GetObject(mapName, data);
  if ( ! data ) {
    LOG(ERROR) << "Could not find CbmFieldMapData object " << mapName << " in file "
               << fileName << FairLogger::endl;
    return NULL;
  }
  Int_t fieldType = data->GetType();

  CbmFieldMap* field = NULL;
  switch (fieldType) {
    case 2: field = new CbmFieldMapSym2(mapName); break;
    case 3: field = new CbmFieldMapSym3(mapName); break;
    default: LOG(ERROR) << "Unknown field type " << fieldType
                        << FairLogger::endl; break;
  }
  if ( field ) {
        field->SetScale(fFieldScale);
        field->SetPosition(fFieldPosition.X(),
                                       fFieldPosition.Y(),
                                       fFieldPosition.Z());
  }

  return field;
}
*/

// TFile* f=new TFile(fileName);
TGeoVolume* constructRootGeometry(TFile* f)
{
    // TGeoManager* OldGeo=geoManeger;
    TGeoManager* NewGeo = 0;
    TGeoVolume* volume = 0;
    ;
    // TFile* f=new TFile(fileName);
    TList* l = f->GetListOfKeys();
    TKey* key;
    TIter next(l);
    TGeoNode* n = 0;
    TGeoVolume* v1 = 0;
    while ((key = (TKey*)next())) {
        if (strcmp(key->GetClassName(), "TGeoManager") != 0)
            continue;
        // gGeoManager = 0;
        NewGeo = (TGeoManager*)key->ReadObj();
        break;
    }

    if (NewGeo != 0) {
        NewGeo->cd();
        // return NewGeo->GetNode(0);
        return (TGeoVolume*)NewGeo->GetNode(0)->GetDaughter(0)->GetVolume();
        // v1=volume->MakeCopyVolume(volume->GetShape());
        // n=v1->GetNode(0);
    } else {
        key = (TKey*)l->At(0);   // Get the first key in the list
        volume = dynamic_cast<TGeoVolume*>(key->ReadObj());
        return volume;
        // if(volume!=0) { return volume->GetNode(0);}//n=volume->GetNode(0); }
        // if(n!=0) { v1=n->GetVolume(); }
        // return NULL;
    }

    // if(v1==0) {
    //   std::cerr<<"ConstructRootGeometry:  could not find any geometry in File!!"<<fileName<<std::endl;
    // }
    // gGeoManager=OldGeo;
    // gGeoManager->cd();
    // TGeoVolume* Cave=NULL;
    // if(NULL == Cave)
    //       return v1;
    // else
    //       return Cave;
}

void copyFile(TString& in, TString& out)
{
    int source = open(in.Data(), O_RDONLY, 0);
    int dest = open(out.Data(), O_WRONLY | O_CREAT /*| O_TRUNC*/, 0644);

    // struct required, rationale: function stat() exists also
    struct stat stat_source;
    fstat(source, &stat_source);
    sendfile(dest, source, 0, stat_source.st_size);
    close(source);
    close(dest);
}

bool hasEnding(std::string const& fullString, std::string const& ending)
{
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    else
        return false;
}

int downloadModule(std::map<int, TString>* loadedModule,
                   TString& rootDir,
                   TSQLiteServer& db,
                   int idsm,
                   FairRunSim* fRun,
                   std::map<int, SETUPMODULE>& moduleMove,
                   int iVerbose = 0)
{
    int res = -1;
    std::map<int, TString>::iterator it = loadedModule->find(idsm);
    if (it != loadedModule->end())
        return 0;

    std::ostringstream stringStream;
    stringStream << "select f.idm,f.url,sm.idp,sm.tx,sm.ty,sm.tz from setupmodule sm INNER JOIN file f ON f.idf=sm.idf "
                    "where sm.idsm="
                 << idsm << ";";
    // std::cerr<<stringStream<<std::endl;
    TSQLResult* resQ2 = db.Query(stringStream.str().c_str());
    if (resQ2 == 0 || resQ2->GetRowCount() == 0) {
        std::cout << "Error: no file for the module was found" << std::endl;
        return res;
    }

    // std::cerr<<"rows="<<resQ2->GetRowCount()<<std::endl;
    TSQLRow* row2 = resQ2->Next();
    // const char* ccc = row2->GetField(1);
    int idP = atoi(row2->GetField(2));
    if (iVerbose > 1)
        std::cerr << "idP=" << idP << std::endl;
    if (idP != -1) {
        res = downloadModule(loadedModule, rootDir, db, idP, fRun, moduleMove, iVerbose);
        if (res < 0)
            return res;
    }
    if (iVerbose > 1)
        std::cerr << row2->GetField(1) << std::endl;
    int dbModuleIndex = atoi(row2->GetField(0));
    stringStream.str("");
    stringStream << getRootPath().c_str() << row2->GetField(1);
    std::string ends = ".geo";
    //	std::cerr<<"getRootPath()="<<getRootPath().c_str()<<std::endl;
    //	std::cerr<<"File path="<<stringStream.str().c_str()<<std::endl;

    TString mName = getModuleType(dbModuleIndex), out;
    mName.ToUpper();
    if (mName == TString("CAVE") || hasEnding(stringStream.str(), ends))
        loadFairModule(fRun, dbModuleIndex, stringStream.str().c_str(), atof(row2->GetField(3)),
                       atof(row2->GetField(5)));
    else {
        double mx = 0, my = 0, mz = 0;
        std::map<int, SETUPMODULE>::iterator itr = moduleMove.find(dbModuleIndex);
        if (iVerbose > 1)
            std::cerr << "detector " << dbModuleIndex << std::endl;
        if (itr != moduleMove.end()) {
            if (iVerbose > 1)
                std::cerr << "detector was found" << std::endl;
            if (itr->second.enable) {
                mx = itr->second.xMove;
                my = itr->second.yMove;
                mz = itr->second.zMove;
                if (iVerbose > 1)
                    std::cerr << "Move detector " << dbModuleIndex << " to x:" << mx << " to y:" << my << " to z:" << mz
                              << std::endl;
            }
        }

        TGeoManager* geom = new TGeoManager(mName, TString::Format("Geometry of the %s module", mName.Data()));
        //		std::cerr<<"import file"<<stringStream.str().c_str()<<std::endl;
        // TGeoManager::Import(stringStream.str().c_str());
        TFile* f = new TFile(stringStream.str().c_str());
        TGeoVolume* node = constructRootGeometry(f);
        double tx = (double)atoi(row2->GetField(3)) + mx;
        double ty = (double)atoi(row2->GetField(4)) + my;
        double tz = (double)atoi(row2->GetField(5)) + mz;
        TGeoRotation rotM;
        TGeoGenTrans* newMatrix = new TGeoGenTrans(tx, ty, tz, 1, 1, 1, &rotM);
        if (std::strcmp(node->GetNode(0)->ClassName(), "TGeoNodeMatrix") == 0) {
            TGeoNodeMatrix* v = (TGeoNodeMatrix*)(node->GetNode(0));
            // std::cerr<<"Matrix changed"<<std::endl;
            v->SetMatrix(newMatrix);
        }
        // geom->CloseGeometry();
        out = rootDir + getModuleName(getModuleId(dbModuleIndex)) + ".root";
        node->Export(out.Data());
        // gGeoManager->Export(out.Data());
        loadFairModule(fRun, dbModuleIndex, out.Data(), atof(row2->GetField(3)), atof(row2->GetField(5)), iVerbose);
        if (iVerbose > 0)
            std::cerr << "Exporting geometry to ROOT file: " << out << std::endl;
        delete geom;
        f->Close();
        delete f;
        // gGeoManager->Export(out.Data());
    }
    loadedModule->insert(std::pair<int, TString>(idsm, out));

    row2->Delete();
    resQ2->Delete();
    return 0;
}

int loadSetup(const char* setupTag, int subSetup, const char* fileName = NULL, int iVerbose = 0)
{
    std::map<int, SETUPMODULE> moduleMove;
    TString tag(setupTag);
    if (fileName != NULL) {
        TXMLEngine* xml = new TXMLEngine;
        XMLDocPointer_t xmldoc = xml->ParseFile(fileName);
        if (xmldoc == 0) {
            std::cerr << "Selected file has not XML format: " << fileName << std::endl;
            delete xml;
            return -1;
        }

        XMLNodePointer_t mainnode = xml->DocGetRootElement(xmldoc);
        // display attributes
        /*		XMLAttrPointer_t attr = xml->GetFirstAttr(mainnode);
                while (attr!=0) {
                        printf(" attr: %s value: %s\n", xml->GetAttrName(attr), xml->GetAttrValue(attr));
                    TString aName = xml->GetAttrName(attr);
                    TString aValue = xml->GetAttrValue(attr);
                        if(aName.CompareTo("tag")==0){
                        tag=aValue;
                        break;
                    }
                    attr = xml->GetNextAttr(attr);
                }
                if(tag.Length()==0){
                    std::cerr<<"No tag attribute in root element in selected file"<<std::endl;
                    xml->FreeDoc(xmldoc);
                    delete xml;
                            return -1;
                }
        */
        XMLAttrPointer_t attr;
        XMLNodePointer_t child = xml->GetChild(mainnode);
        while (child != 0) {
            TString chName = xml->GetNodeName(child);
            if (chName.CompareTo("setupModule") == 0) {
                attr = xml->GetFirstAttr(child);

                SETUPMODULE sNode;
                sNode.xMove = 0;
                sNode.yMove = 0;
                sNode.zMove = 0;
                sNode.enable = true;
                sNode.type = TString("");
                while (attr != 0) {
                    TString aName = xml->GetAttrName(attr);
                    TString aValue = xml->GetAttrValue(attr);

                    if (aName.CompareTo("type") == 0)
                        sNode.type = aValue;
                    else if (aName.CompareTo("moveX") == 0) {
                        sNode.xMove = aValue.Atof();
                    } else if (aName.CompareTo("moveY") == 0) {
                        sNode.yMove = aValue.Atof();
                    } else if (aName.CompareTo("moveZ") == 0) {
                        sNode.zMove = aValue.Atof();
                    }
                    if ((aName.CompareTo("enable") == 0)
                        && (aValue.CompareTo("false") == 0 || aValue.CompareTo("False") == 0
                            || aValue.CompareTo("FALSE") == 0))
                        sNode.enable = false;
                    printf(" attr: %s value: %s\n", xml->GetAttrName(attr), xml->GetAttrValue(attr));
                    attr = xml->GetNextAttr(attr);
                }
                int ii = getModuleIndex(sNode.type);
                if (ii != -1)
                    moduleMove.insert(std::pair<int, SETUPMODULE>(ii, sNode));
            }
            child = xml->GetNext(child);
        }
        xml->FreeDoc(xmldoc);
        delete xml;
    }

    FairRunSim* fRun = FairRunSim::Instance();
    if (fRun == NULL) {
        fRun = new FairRunSim();
        std::cout << "Created new FairRunSim" << std::endl;
    }
    TString rootPath = getenv("TMP_PATH");
    if (rootPath.Length() == 0) {
        UserGroup_t* user = gSystem->GetUserInfo();
        string login(user->fUser.Data());
        rootPath = "/tmp/" + login + "/";
        system("mkdir -p " + rootPath);
    }

    std::ostringstream stringStream;
    stringStream << "sqlite://"
                 << getenv("DBL_FILE_PATH");   // KG если переменная пустая, то все отрабатывает, но неверно?
    TSQLiteServer db(stringStream.str().c_str());
    if (iVerbose > 0)
        std::cerr << "Geometry Database Path: " << stringStream.str().c_str() << std::endl;
    stringStream.str("");

    if (putMudules4SQLite(db) != 0)
        return -1;
    if (putMudulesRevision4SQLite(db) != 0)
        return -1;

    stringStream
        << "select idsd,status,idma,idfi from setup where stag='" << tag
        << "' order by revision desc;";   // KG если я правильно понял, то нужно брать по-умолчанию, самый новый релиз?
    TSQLResult* resQ = db.Query(stringStream.str().c_str());
    if (resQ == 0) {
        std::cerr << "SQL query was failed while searching the given tag: `" << tag << "`" << std::endl;
        return -1;
    }
    TSQLRow* row = resQ->Next();
    if (row == NULL) {
        std::cerr << "No geometry setup found for the given tag: `" << tag << "`" << std::endl;
        return -1;
    }

    const char* idsd = row->GetField(0);
    int idma = atoi(row->GetField(2));
    int idfi = atoi(row->GetField(3));
    // cout<<"idsd = "<<idsd<<"; idma = "<<idma<<"; idfi = "<<idfi<<endl;
    stringStream.str("");
    stringStream << "select idsm from sms where idsd=" << idsd << ";";
    row->Delete();
    resQ->Delete();

    resQ = db.Query(stringStream.str().c_str());
    if (resQ == 0 || resQ->GetRowCount() == 0) {
        std::cout << "Error: no modules found" << std::endl;
        return -1;
    }
    std::map<int, TString>* loadedModule = new std::map<int, TString>();
    while ((row = resQ->Next()) != 0) {
        stringStream.str("");
        const char* idsm = row->GetField(0);
        int idsmI = atoi(idsm);
        if (subSetup < 0 || subSetup == idsmI) {   // check subset
            int res = downloadModule(loadedModule, rootPath, db, idsmI, fRun, moduleMove, iVerbose);
            if (res == -1) {
                std::cout << "Error: no file for the module was found " << std::endl;
                return -1;
            }
        }
        row->Delete();
    }
    /*	if(idfi>0){
            stringStream.str("");
            //stringStream.clear();
            stringStream<<"select tag,scale,z from field where idfi="<<idfi<<";";
            resQ = db.Query(stringStream.str().c_str());
            if(resQ==0 || resQ->GetRowCount()==0){
                std::cout <<"Field did not find"<<std::endl;
            }else{
                row= resQ->Next();
                double fieldZ = atof(row->GetField(2));
                double fieldScale = atof(row->GetField(1));
                TString tagF="field_";
                tagF+=(row->GetField(0));
                //std::ofstream dest(rootPath+"field.txt", std::ofstream::out|std::ofstream::trunc);
                //dest<<"field="<<row->GetField(0)<<std::endl<<"Position=(0.,
       0.,"<<fieldZ<<")"<<std::endl<<"scale="<<fieldScale;
                //dest.close();
                CbmFieldMap* magField = CreateFieldMap(tagF);//new CbmFieldMapSym3(row->GetField(0));
                magField->SetPosition(0., 0., fieldZ);
                magField->SetScale(fieldScale);
                fRun->SetField(magField);
                //CbmFieldMap* CreateFieldMap
                row->Delete();
                resQ->Delete();
            }
        }*/

    if (idma > 0) {
        stringStream.str("");
        stringStream << "select matag,url from material where idma=" << idma << ";";
        resQ = db.Query(stringStream.str().c_str());
        if (resQ == 0 || resQ->GetRowCount() == 0)
            std::cout << "Materials did not find" << std::endl;
        else {
            row = resQ->Next();
            const char* url = row->GetField(1);
            stringStream.str("");
            stringStream << url;
            TString in("");   //("/../../..");
            TString work = getenv("GEOMPATH");
            TString Mat = "";
            work.ReplaceAll("//", "/");
            if (work.IsNull()) {
                work = getenv("VMCWORKDIR");
                Mat = work + "/geometry/";
                Mat.ReplaceAll("//", "/");
            } else {
                Mat = work;
                if (!Mat.EndsWith("/"))
                    Mat += "/";
            }
            std::string workP(Mat.Data());
            if (iVerbose > 0)
                std::cout << "Working geometry directory: " << workP << std::endl;
            std::size_t found = workP.find('/');
            if (found != std::string::npos) {
                while ((found = workP.find('/', found + 1)) != std::string::npos) {
                    if (iVerbose > 1)
                        std::cout << "found:" << found << std::endl;
                    in += "../";
                }
            }
            in += getRootPath() + stringStream.str();
            fRun->SetMaterials(
                in.Data());   // KG
                              // /home/soul/bmnroot/geometry/../../../..//home/soul/bmnroot/database/geo_db/storage/07022020_085550_media.geo?
            resQ->Delete();
            row->Delete();
        }
    }

    db.Close();
    if (iVerbose > 0)
        std::cout << "The setup geometry was successfully loaded" << std::endl;
    return 0;
}

int loadSetup(const char* setupTag, const char* url)
{
    if (url == NULL)
        return loadSetup(setupTag, -1);

    std::srand(time(NULL));
    int random_variable = std::rand();
    std::cout << random_variable << std::endl;
    std::ostringstream stringStream;
    //  stringStream<<"wget -O test2.tar \""<<url<<"?type=s&tag="<<setupTag<<"&dirname="<<random_variable<<"\"";
    //  http://cbmdb.jinr.ru/geometry8/download_file.php?type=all&tag=all
    stringStream << "wget -O test2.tar \"" << url << "/download_file.php?type=all&tag=all&dirname=" << random_variable
                 << "\" >/dev/null ";
    system(stringStream.str().c_str());
    stringStream.str("");
    stringStream << "tar -xf test2.tar >/dev/null";
    system(stringStream.str().c_str());
    stringStream.str("");
    stringStream << "rm test2.tar";
    system(stringStream.str().c_str());
    stringStream.str("");
    stringStream << gSystem->pwd() << "/" << random_variable << "/storage/";
    gSystem->Setenv("DBL_ROOT_PATH", stringStream.str().c_str());
    stringStream.str("");
    stringStream << gSystem->pwd() << "/" << random_variable << "/local.db";
    gSystem->Setenv("DBL_FILE_PATH", stringStream.str().c_str());

    loadSetup(setupTag, -1);

    stringStream.str("");
    stringStream << "rm -rf " << random_variable << "/local.db";
    system(stringStream.str().c_str());
    stringStream.str("");
    stringStream << "rm -rf " << random_variable;
    system(stringStream.str().c_str());
    std::cout << "DBL_ROOT_PATH=" << getRootPath() << std::endl;

    return 0;
}

int loadSetup(const char* tag)
{
    return loadSetup(tag, -1, NULL, 0);
}

/*
int main(int argc, char** argv)
{
    downloadSetup("test2.db","sis100_electron");
}
*/
