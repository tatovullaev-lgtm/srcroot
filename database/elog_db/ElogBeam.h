// ----------------------------------------------------------------------
//                    ElogBeam header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** ElogBeam.h
 ** Class for the table: beam_ 
 **/ 

#ifndef ElogBeam_H 
#define ElogBeam_H 1 

#include "TString.h"
#include "TDatime.h"

#include "ElogConnection.h"

class ElogBeam
{
 private:
    /* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    /// connection to the database
    ElogConnection* connectionDB;

    /// beam
    TString str_beam;

    //Constructor
    ElogBeam(ElogConnection* db_connect, TString beam);
    /* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
    /* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    virtual ~ElogBeam(); // Destructor

    // static class functions
    /// add new beam to the database
    static ElogBeam* CreateBeam(TString beam);
    /// get beam from the database
    static ElogBeam* GetBeam(TString beam);
    /// check beam exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckBeamExists(TString beam);
    /// delete beam from the database
    static int DeleteBeam(TString beam);
    /// print all beams
    static int PrintAll();

    // Getters
    /// get beam of the current beam
    TString GetBeam() {return str_beam;}

    // Setters
    /// set beam of the current beam
    int SetBeam(TString beam);

    /// print information about current beam
    void Print();
    /* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 ClassDef(ElogBeam,1);
};

#endif
