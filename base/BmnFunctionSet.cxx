#include "BmnFunctionSet.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "TFile.h"
#include "TSystem.h"

#include <iomanip>
#include <iostream>
using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

// check whether file exists: 1 - exists, 0 - not exists, -1 - exists but cannot access with the mode (default:
// kFileExists)
int BmnFunctionSet::CheckFileExist(TString& fileName, int iVerbose, EAccessMode mode)
{
    gSystem->ExpandPathName(fileName);
    if (gSystem->AccessPathName(fileName.Data(), kFileExists) == true) {
        if (iVerbose)
            cout << "ERROR: no specified file: " << fileName << endl;
        return 0;
    }
    if ((mode != kFileExists) && (gSystem->AccessPathName(fileName.Data(), mode) == true)) {
        if (iVerbose)
            cout << "ERROR: no required permissions to access the file: " << fileName << endl;
        return -1;
    }

    return 1;
}

// check whether directory exists: 1 - exists, 0 - not exists, -1 - exists but cannot access with the mode (default:
// kFileExists)
int BmnFunctionSet::CheckDirectoryExist(TString& fileName, int iVerbose, EAccessMode mode)
{
    gSystem->ExpandPathName(fileName);
    TString dirName(gSystem->DirName(fileName.Data()));
    if (gSystem->AccessPathName(dirName.Data(), kFileExists) == true) {
        if (iVerbose)
            cout << "ERROR: no specified directory: " << dirName << endl;
        return 0;
    }
    if ((mode != kFileExists) && (gSystem->AccessPathName(dirName.Data(), mode) == true)) {
        if (iVerbose)
            cout << "ERROR: no required permissions to access the directory: " << dirName << endl;
        return -1;
    }

    return 1;
}

// create directory tree for the file name if not exists: 1 - exists, 0 - created
// error codes: -1 - exists but cannot access with the mode (default: kWritePermission), -2 - cannot be created, -3 -
// empty file name
int BmnFunctionSet::CreateDirectoryTree(TString& fileName, int iVerbose, EAccessMode mode)
{
    if (fileName == "") {
        if (iVerbose)
            cout << "ERROR: the specified file name is empty" << endl;
        return -3;
    }

    int status = CheckDirectoryExist(fileName, 0, mode);
    if (status < 0) {
        if (iVerbose)
            cout << "ERROR: the directory of the specified file (\"" << fileName
                 << "\") does not provide required permissions" << endl;
        return status;
    }
    if (status == 0) {
        TString dirName(gSystem->DirName(fileName.Data()));
        int status_parent = CreateDirectoryTree(dirName, 0, mode);
        if (status_parent < 0)
            return status_parent;
        if (gSystem->MakeDirectory(dirName) != 0) {
            if (iVerbose)
                cout << "ERROR: the directory of the specified file (\"" << fileName << "\") could not be created"
                     << endl;
            return -2;
        }
        return 0;
    }

    return 1;
}

int BmnFunctionSet::isSimulationFile(TString fileName)
{
    TFile* fRootFile = nullptr;
    if (fileName == "") {
        FairRootManager* ioman = FairRootManager::Instance();
        if (ioman == nullptr) {
            LOG(error) << "isSimulationFile(): FairRootManager is not instantiated";
            return -1;
        }
        TTree* cur_tree = ioman->GetInTree();
        if (cur_tree == nullptr) {
            LOG(error) << "isSimulationFile(): There is no current tree opened";
            return -2;
        }

        fRootFile = cur_tree->GetCurrentFile();
    } else {
        gSystem->ExpandPathName(fileName);
        if (gSystem->AccessPathName(fileName.Data()) == true) {
            LOG(error) << "isSimulationFile(): no specified file: " << fileName;
            return -3;
        }

        fRootFile = TFile::Open(fileName.Data());
        if (!fRootFile || fRootFile->IsZombie()) {
            LOG(error) << "isSimulationFile(): opening the input file failed: " << fileName;
            if (fRootFile) {
                fRootFile->Close();
                delete fRootFile;
            }
            return -4;
        }
    }

    TObject* branch_list = fRootFile->Get("BranchList");
    if (branch_list != nullptr) {
        TObject* mctrack_find = branch_list->FindObject("MCTrack");
        if (mctrack_find != nullptr) {
            if (fileName != "") {
                fRootFile->Close();
                delete fRootFile;
            }

            return 1;
        }
    }

    if (fileName != "") {
        fRootFile->Close();
        delete fRootFile;
    }
    return 0;
}

// check whether path is a directory
bool BmnFunctionSet::isDirectory(TString path)
{
    FileStat_t file_stat;
    gSystem->GetPathInfo(path.Data(), file_stat);
    if (R_ISDIR(file_stat.fMode))
        return true;
    else
        return false;
}

TTimeStamp BmnFunctionSet::TimePoint2TS(SysPoint p)
{
    Int_t nans = duration_cast<nanoseconds>(p.time_since_epoch()).count() % 1000000000;
    Long64_t sec = duration_cast<nanoseconds>(p.time_since_epoch()).count() / 1000000000;
    return TTimeStamp{time_t(sec), nans};
}

SysPoint BmnFunctionSet::TimeStamp2TP(TTimeStamp t)
{
    Int_t nans = t.GetNanoSec();
    Int_t sec = t.GetSec();
    return SysPoint{sec * 1s + nans * 1ns};
}

void BmnFunctionSet::PrintTimePoint(SysPoint p)
{
    LOG(info) << TimePoint2String(p);
}

std::string BmnFunctionSet::TimePoint2String(SysPoint p)
{
    time_t tt = SysClock::to_time_t(p);
    auto nans = duration_cast<nanoseconds>(p.time_since_epoch()).count() % 1000000000;
    std::stringstream sst;
    sst << std::put_time(std::gmtime(&tt), "%a, %d %b %Y %T %z (%Z)");   // as TTimeStamp
    //    sst << std::put_time(std::localtime(&tt), "%a, %d %b %Y %T %z (%Z)");   // as TTimeStamp
    sst << " +" << std::setw(9) << nans;
    return sst.str();
}

SysPoint BmnFunctionSet::TDatime2GMT(TDatime& tdatime, Int_t timezoneOffsetHours)
{
    std::tm tm = {};
    tm.tm_year = tdatime.GetYear() - 1900;
    tm.tm_mon = tdatime.GetMonth() - 1;
    tm.tm_mday = tdatime.GetDay();
    tm.tm_hour = tdatime.GetHour();
    tm.tm_min = tdatime.GetMinute();
    tm.tm_sec = tdatime.GetSecond();
    time_t t = std::time(nullptr);
    std::tm* local = std::localtime(&t);
    int offset = local->tm_gmtoff;
    return (SysClock::from_time_t(std::mktime(&tm) + offset) - 1h * timezoneOffsetHours);
}
