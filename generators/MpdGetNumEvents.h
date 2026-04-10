#ifndef __CLING__
#include "TROOT.h"

#include <string>
#include <zlib.h>
#endif

class MpdLibZ
{
  public:
    MpdLibZ(const char* filename);
    virtual ~MpdLibZ();

    int open(const char* mode);
    int eof();
    int close();
#ifndef __CLING__
    int write(voidpc buf, unsigned len);
    int read(voidp buf, unsigned len);
#endif
    char* gets(char* buf, int len);
    int puts(char* s);
#ifndef __CLING__
    off_t tell();
    off_t seek(off_t pos, int whence);
#endif

  private:
    const char* fileName;
#ifndef __CLING__
    gzFile file;
#endif

    ClassDef(MpdLibZ, 1);
};

class MpdGetNumEvents
{
  private:
    MpdGetNumEvents() {}
    virtual ~MpdGetNumEvents() {}

    static bool GetQGSMEventHeader(char* ss, MpdLibZ* libz, Int_t& fQGSM_format_ID, int iVerbose);
    static std::string FindFirstTree(const char* filename, int iVerbose);

  public:
    // Return event count in a given file produced by the PHSD event generator
    // filename - file path to the generator file, iVerbose - output level (0 - no output, 1 - only errors, 2 - full
    // output)
    static Int_t GetNumPHSDEvents(const char* filename, int iVerbose = 2);
    // Return event count in a given file produced by the QGSM/DCM-QGSM event generator
    // filename - file path to the generator file, iVerbose - output level (0 - no output, 1 - only errors, 2 - full
    // output)
    static Int_t GetNumQGSMEvents(const char* fileName, int iVerbose = 2);
    // Return event count in a given file produced by the UrQMD event generator
    // filename - file path to the generator file, iVerbose - output level (0 - no output, 1 - only errors, 2 - full
    // output)
    static Int_t GetNumURQMDEvents(const char* fileName, int iVerbose = 2);
    // Return event count in a given file produced by the DCM-SMM event generator
    // filename - file path to the generator file, iVerbose - output level (0 - no output, 1 - only errors, 2 - full
    // output)
    static Int_t GetNumDCMSMMEvents(const char* fileName, int iVerbose = 2);
    // Return event count in a given ROOT file as a number of records in a specfied tree
    // filename - file path to the ROOT file
    // treename - name of the tree in the ROOT file; "" - get FairRootManager default tree name, nullptr - get first
    // tree from the file iVerbose - output level (0 - no output, 1 - only errors, 2 - full output)
    static Int_t GetNumROOTEvents(const char* filename, const char* treename = "", int iVerbose = 2);
    // Return event count in a given file for an event generator defined automatically
    // filename - file path to the generator file, iVerbose - output level (0 - no output, 1 - only errors, 2 - full
    // output)
    static Int_t GetNumEvents(const char* filename, int iVerbose = 2);

    ClassDef(MpdGetNumEvents, 1);
};
