// ----------------------------------------------------------------------
//                    UniRunGeometry header file
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

/** UniRunGeometry.h
 ** Class for the table: run_geometry 
 **/ 

#ifndef UniRunGEOMETRY_H
#define UniRunGEOMETRY_H 1

#include "TString.h"
#include "TDatime.h"
#include "TObjArray.h"

#include "UniConnection.h"

class UniRunGeometry
{
 private:
    /* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    /// connection to the database
    UniConnection* connectionDB;

    /// geometry id
    int i_geometry_id;
    /// root geometry
    unsigned char* blob_root_geometry;
    /// size of root geometry
    Long_t sz_root_geometry;

    //Constructor
    UniRunGeometry(UniConnection* db_connect, int geometry_id, unsigned char* root_geometry, Long_t size_root_geometry);
    /* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
    /* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    virtual ~UniRunGeometry(); // Destructor

    // static class functions
    /// add new run geometry to the database
    static UniRunGeometry* CreateRunGeometry(unsigned char* root_geometry, Long_t size_root_geometry);
    /// get run geometry from the database
    static UniRunGeometry* GetRunGeometry(int geometry_id);
    /// check run geometry exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckRunGeometryExists(int geometry_id);
    /// delete run geometry from the database
    static int DeleteRunGeometry(int geometry_id);
    /// print all run geometrys
    static int PrintAll();

    // Getters
    /// get geometry id of the current run geometry
    int GetGeometryId() {return i_geometry_id;}
    /// get root geometry of the current run geometry
    unsigned char* GetRootGeometry() {unsigned char* tmp_root_geometry = new unsigned char[sz_root_geometry]; memcpy(tmp_root_geometry, blob_root_geometry, sz_root_geometry); return tmp_root_geometry;}
    /// get size of root geometry of the current run geometry
    Long_t GetRootGeometrySize() {return sz_root_geometry;}

    // Setters
    /// set root geometry of the current run geometry
    int SetRootGeometry(unsigned char* root_geometry, Long_t size_root_geometry);

    /// print information about current run geometry
    void Print();
    /* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

    static UniRunGeometry* CreateRunGeometry(int geometry_id, unsigned char* root_geometry, Long_t size_root_geometry);
    /// get all run geometries
    static TObjArray* GetAll();

 ClassDef(UniRunGeometry,1);
};

#endif
