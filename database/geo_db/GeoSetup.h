#ifndef _GEO_SETUP_INCLUDED
#define _GEO_SETUP_INCLUDED

#include "BmnFieldMap.h"
#include "FairRunSim.h"
#include "TFile.h"
#include "TGeoVolume.h"
#include "TSQLRow.h"
#include "TSQLServer.h"
#include "TString.h"
#include "geo_db_settings.h"

#include <map>
#include <string>
#include <utility>

using namespace std;

typedef struct SETUPMODULE
{
    TString type;
    double xMove, yMove, zMove;
    bool enable;
} setup_module, *setup_module_ptr;

typedef struct MODULEREVISION
{
    TString class_name;
    int idmr, idm, rev;
    bool active;
    TString args;
} module_r, *module_r_ptr;

/**
 *  GeoSetup class. It is used for loading necessary setup geometries from the Geometry Database.
 */
class GeoSetup
{
  public:
    // default constructor
    GeoSetup()
        : m_db(nullptr)
        , m_field(nullptr)
        , m_fid(-1)
        , m_sid(-1)
        , m_tmpFolder("")
    {}

    /**
     * Destructor method
     */
    ~GeoSetup();

    /**
     * Load selected geometry setup from the Local Geometry Database and create gGeoManager instance.
     * Use for Reconstruction and Analysis
     * @param setupTag the name of the setup tag.
     * @return  0, if the setup is loaded. Error code, if the setup is not loaded.
     */
    int LoadSetupToGeoManager(string_view setup_tag);

    /**
     * Load selected geometry setup from the Local or Central (defined by url) Geometry Database and add it to the
     * FairRunSim instance. Use for Simulation
     * @param setupTag the name of the setup tag.
     * @param localSettings path to local setting file that can disable or move some detector in selected geometry.
     * @return  0, if the setup is loaded. Error code, if the setup is not loaded.
     */
    int LoadSetupToFairRunSim(string_view setupTag, const char* localSettings = nullptr);

    /**
     * Get magnetic field of the current setup geometry.
     * @param scale the scale of the field. Default value is 0, which means that when created, scaling is not applied to
     * the magnetic field.
     * @return  Instance of the magnetic field class inherited from FairField. nullptr if no field
     */
    FairField* GetMagneticField(double scale = 0);

    /**
     * Get path to parameter file for the given detector name of the current setup.
     * @param detector_name the name of the detector, for which the parameter file is needed.
     * @return  Path to parameter file. "" if file did not set.
     */
    string_view GetParameterFile(string_view detector_name);

  private:
    map<int, TString> m_modules_table;
    map<int, module_r_ptr> m_modules_revision_table;
    map<int, TString> m_loadedModule;
    map<int, setup_module_ptr> m_moduleEdit;

    // connection to the Geometry Database
    TSQLServer* m_db;
    // magnetic field of the geometry setup
    FairField* m_field;
    // id of the current magnetic field
    int m_fid;
    // id of the current geometry setup
    int m_sid;
    // current tmp directory (/tmp/$USER/)
    TString m_tmpFolder;

    unsigned long int GetServerRevision(const char* url = GEO_SERVER_HOST);
    int CheckTagOnServer(string_view setup_tag, const char* url = GEO_SERVER_HOST);
    unsigned long int GetLocalRevision(TString& localDbPath);
    int installLocalDB(string_view setup_tag, const char* url = GEO_SERVER_HOST);
    void CloseDB();

    // create connection to the Local Geometry Database and fill the tables with modules and their revisions
    bool OpenLocalGeoDatabase();
    bool IsEqlMatrix(const char* fileName, TSQLRow* row);
    TGeoNode* ConstructRootGeometry(const char* fileName);
    static bool exists_file(const char* name);
    // get path to the location of the Local Geometry Database
    string GetRootPath();
    // get path to a temporary directory (/tmp/$USER/), and create if absent
    TString& GetTmpPath();
    // get root volume in the TFile object
    TGeoVolume* GetRootVolume(TFile* f);

    // get module name by index; return "Unknown" if not found
    TString GetModuleName(int dbModuleUndex);
    // get module index by name; return -1 if not found
    int GetModuleIndex(TString& name);
    // get module ID by revision number; return -1 if not found
    int GetModuleId(int dbModuleRev);
    // get module type by revision number
    TString GetModuleType(int dbModuleRev);
    // get module_r_ptr (pointer to the module revision) by revision number
    module_r_ptr GetModuleRevision(int dbModuleRev);

    // load local settings from the file for all setup modules
    bool LoadLocalSettings(const char* fileName);

    // load module from the Geometry Database and add to the FairRunSim instance
    int LoadModuleToFairRunSim(int idsm, FairRunSim* fRun);
    // add FairModule defined by revision number and geometry file path to the FairRunSim instance
    bool AddModuleToFairRunSim(int dbModuleRev, const char* path, FairRunSim* fRun);

    // fill map with modules from the Geometry Database
    int PutModules();
    // fill map with module revisions from the Geometry Database
    int PutModuleRevisions();
};

#endif
