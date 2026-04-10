#ifndef BMNFUNCTIONSET_H
#define BMNFUNCTIONSET_H 1

// STL
#include <chrono>
#include <string>
// ROOT
#include "TDatime.h"
#include "TString.h"
#include "TSystem.h"
#include "TTimeStamp.h"

using SysClock = std::chrono::system_clock;
using SysPoint = std::chrono::time_point<SysClock>;

/** Class with static common functions to simplify experiment code **/
class BmnFunctionSet
{
  public:
    // check whether path is a directory
    static bool isDirectory(TString path);
    // check whether file exists: 1 - exists, 0 - not exists, -1 - exists but cannot access with the mode (default:
    // kFileExists) fileName: file path; iVerbose: 0 - no output, 1 - print errors; mode: check for 'kFileExists', or
    // has 'kExecutePermission', 'kWritePermission' or 'kReadPermission'
    static int CheckFileExist(TString& fileName, int iVerbose = 0, EAccessMode mode = kFileExists);
    // check whether directory exists: 1 - exists, 0 - not exists, -1 - exists but cannot access with the mode (default:
    // kFileExists)
    static int CheckDirectoryExist(TString& fileName, int iVerbose = 0, EAccessMode mode = kFileExists);
    // create directory tree for the file name if not exists: 1 - exists, 0 - created
    // error codes: -1 - exists but cannot access with the mode (default: kWritePermission), -2 - cannot be created, -3
    // - empty file name
    static int CreateDirectoryTree(TString& fileName, int iVerbose = 0, EAccessMode mode = kWritePermission);

    // define whether ROOT file contains simulation data: 1 - true, 0 - false (experimental data), <0 - some errors
    // occured if filename is not set then the function checks the current processing
    static int isSimulationFile(TString fileName = "");

    static TTimeStamp TimePoint2TS(SysPoint p);
    static SysPoint TimeStamp2TP(TTimeStamp p);
    static void PrintTimePoint(SysPoint p);
    static std::string TimePoint2String(SysPoint p);
    static SysPoint TDatime2GMT(TDatime& tdatime, Int_t timezoneOffsetHours = 0);

  private:
    BmnFunctionSet() {}
    ~BmnFunctionSet() {}

    ClassDef(BmnFunctionSet, 1)
};

#endif
