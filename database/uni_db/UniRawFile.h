// ----------------------------------------------------------------------
//                    UniRawFile header file 
//                      Generated 27-09-2022 
// ----------------------------------------------------------------------

/** /home/soul/bmnroot/database/uni_db1/UniRawFile.h 
 ** Class for the table: raw_file 
 **/ 

#ifndef UNIRAWFILE_H 
#define UNIRAWFILE_H 1 

#include "TString.h"
#include "TDatime.h"
#include "TObjArray.h"

#include "UniConnection.h"

class UniRawFile
{
 private:
    /* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    /// connection to the database
    UniConnection* connectionDB;

    /// file id
    int i_file_id;
    /// period number
    int i_period_number;
    /// run number
    int i_run_number;
    /// start event
    int i_start_event;
    /// end event
    int i_end_event;
    /// start datetime
    TDatime ts_start_datetime;
    /// end datetime
    TDatime ts_end_datetime;
    /// file path
    TString str_file_path;
    /// event count
    int i_event_count;
    /// file size
    int64_t i64_file_size;
    /// file hash
    TString* str_file_hash;

    //Constructor
    UniRawFile(UniConnection* db_connect, int file_id, int period_number, int run_number, int start_event, int end_event, TDatime start_datetime, TDatime end_datetime, TString file_path, int event_count, int64_t file_size, TString* file_hash);
    /* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
    /* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
    virtual ~UniRawFile(); // Destructor

    // static class functions
    /// add new raw file to the database
    static UniRawFile* CreateRawFile(int period_number, int run_number, int start_event, int end_event, TDatime start_datetime, TDatime end_datetime, TString file_path, int event_count, int64_t file_size, TString* file_hash);
    /// get raw file from the database
    static UniRawFile* GetRawFile(int file_id);
    /// get raw file from the database
    static UniRawFile* GetRawFile(TString file_path);
    /// check raw file exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckRawFileExists(int file_id);
    /// check raw file exists in the database: 1- true, 0 - false, <0 - database operation error
    static int CheckRawFileExists(TString file_path);
    /// delete raw file from the database
    static int DeleteRawFile(int file_id);
    /// delete raw file from the database
    static int DeleteRawFile(TString file_path);
    /// print all raw files
    static int PrintAll();

    // Getters
    /// get file id of the current raw file
    int GetFileId() {return i_file_id;}
    /// get period number of the current raw file
    int GetPeriodNumber() {return i_period_number;}
    /// get run number of the current raw file
    int GetRunNumber() {return i_run_number;}
    /// get start event of the current raw file
    int GetStartEvent() {return i_start_event;}
    /// get end event of the current raw file
    int GetEndEvent() {return i_end_event;}
    /// get start datetime of the current raw file
    TDatime GetStartDatetime() {return ts_start_datetime;}
    /// get end datetime of the current raw file
    TDatime GetEndDatetime() {return ts_end_datetime;}
    /// get file path of the current raw file
    TString GetFilePath() {return str_file_path;}
    /// get event count of the current raw file
    int GetEventCount() {return i_event_count;}
    /// get file size of the current raw file
    int64_t GetFileSize() {return i64_file_size;}
    /// get file hash of the current raw file
    TString* GetFileHash() {if (str_file_hash == nullptr) return nullptr; else return new TString(*str_file_hash);}

    // Setters
    /// set period number of the current raw file
    int SetPeriodNumber(int period_number);
    /// set run number of the current raw file
    int SetRunNumber(int run_number);
    /// set start event of the current raw file
    int SetStartEvent(int start_event);
    /// set end event of the current raw file
    int SetEndEvent(int end_event);
    /// set start datetime of the current raw file
    int SetStartDatetime(TDatime start_datetime);
    /// set end datetime of the current raw file
    int SetEndDatetime(TDatime end_datetime);
    /// set file path of the current raw file
    int SetFilePath(TString file_path);
    /// set event count of the current raw file
    int SetEventCount(int event_count);
    /// set file size of the current raw file
    int SetFileSize(int64_t file_size);
    /// set file hash of the current raw file
    int SetFileHash(TString* file_hash);

    /// print information about current raw file
    void Print();
    /* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

    /// check raw file exists in the database
    /// for the given period_number:run_number and start_event-end_event values
    /// returns 1 - true, 0 - false, <0 - database operation error
    static int CheckRawFileExists(int period_number, int run_number, int start_event, int end_event);

    /// check that raw file exists in the database
    /// for the given period_number:run_number and start_event value
    /// returns maximum 'end_event' value, 0 - not exists, <0 - database operation error
    static int CheckRawFileExists(int period_number, int run_number, int start_event);

    /// get event count for a given run as a sum for all raw files: if <0 - database operation error
    static int GetSumEventCount(int period_number, int run_number);

    /// get array of raw files (UniRawFile*) corresponding to the period:run number
    /// ordered by start_event ascending
    /// return nullptr if an error occured)
    static TObjArray* GetRawFiles(int period_number, int run_number);

    /// get array of raw files (UniRawFile*) corresponding to the period_number:run_number and start_event - end_event
    /// ordered by file_id descending
    /// returns nullptr if an error occured
    static TObjArray* GetRawFiles(int period_number, int run_number, int start_event, int end_event);

    /// calculate and return (in hex string format) cheсksum for a given file
    /// use Adler32 algorithm if zlib library is found
    /// or use MD5 algorithm if zlib library is not found
    static TString CalculateFileHash(TString file_path);

 ClassDef(UniRawFile,1);
};

#endif
