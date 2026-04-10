//========================================================================================================
// Name        : nica-scheduler.cpp
// Author      : Konstantin Gertsenberger
// Description : NICA-Scheduler to run processing tasks of the ROOT-based frameworks through batch systems
//========================================================================================================

// own headers
#include "function_set.h"
#include "batch_command.h"
#include "container_settings.h"

// ROOT headers for work with the Unified Condition Database (UniConDa)
#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include "TString.h"

// C++ headers
#include <thread>
#include <mutex>
#include <condition_variable>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <fstream>
#include <algorithm>
#include <array>
#include <map>
#include <glob.h>
#include <unistd.h>
#include <getopt.h>

/* Predefined settings for the Unified Condition Database*/
struct struct_database_settings
{
    string db_host = "";
    string db_name = "";
    string db_username = "";
    string db_password = "";

    struct_database_settings() {}
    struct_database_settings(const struct_database_settings& arg_db_settings)
    { db_host = arg_db_settings.db_host; db_name = arg_db_settings.db_name; db_username = arg_db_settings.db_username; db_password = arg_db_settings.db_password;}
    struct_database_settings(string arg_db_host, string arg_db_name, string arg_db_username, string arg_db_password)
    { db_host = arg_db_host; db_name = arg_db_name; db_username = arg_db_username; db_password = arg_db_password;}
    struct_database_settings& operator=(const struct_database_settings& arg_db_settings) {
        if (this != &arg_db_settings) {
            db_host = arg_db_settings.db_host; db_name = arg_db_settings.db_name;
            db_username = arg_db_settings.db_username; db_password = arg_db_settings.db_password;
        }
        return *this;
    }
};
#include "../settings.h"

// debug flag for extended output
bool isDebugMode = false;

// C++11 does not include semaphore implementation (only in C++20)
// therefore there is a manual implementation
class Semaphore
{
  public:
    Semaphore(unsigned int resource_count = 0) : count_(resource_count) {}

    /**
    * @brief Acquires a resource from the semaphore.
    *
    * Decrements the internal resource count. If the count is zero, the
    * calling thread will block until another thread releases a resource by
    * calling release().
    */
    void acquire()
    {
        unique_lock<mutex> lock(mutex_);
        condition_.wait(lock, [this] { return count_ > 0; });
        count_--;
    }

    /**
    * @brief Releases a resource from the semaphore.
    *
    * Increments the internal resource count. If a thread is waiting for a
    * resource to be released, it will be notified.
    */
    void release() {
        unique_lock<mutex> lock(mutex_);
        count_++;
        condition_.notify_one();
    }

private:
    mutex mutex_;
    condition_variable condition_;
    unsigned int count_;
};

// string structure to support null (no) value
class nullString
{
  protected:
    // string value
    string strValue;
    // whether the string is null (no value)
    bool isNullString;

  public:
    nullString() : strValue(""), isNullString(true) {}
    nullString(nullptr_t) : strValue(""), isNullString(true) {}
    nullString(char* pchar) { SetValue(pchar); }
    nullString(string str) : strValue(str), isNullString(false) {}
    nullString(const nullString& str) : strValue(str.strValue), isNullString(str.isNullString) {}

    nullString& operator=(const nullString& str)
    {
        strValue = str.strValue;
        isNullString = str.isNullString;
        return *this;
    }

    bool operator==(nullptr_t) const { return isNullString; }
    bool operator!=(nullptr_t) const { return !isNullString; }

    void SetNull() { strValue = ""; isNullString = true;}
    void SetValue(string str) { strValue = str; isNullString = false; }
    void SetValue(nullptr_t) { SetNull(); }
    void SetValue(char* pchar)
    {
        if (pchar == nullptr) SetNull();
        else SetValue((string) pchar);
    }

    string GetValue() { return strValue; }
    bool isNull() { return isNullString; }
};

// structure with information on files to be processed
// iParallelMode = processor count to parallel event processing of the input file ('-1' - was not set)
// iMerge: 0 (chain) - save only result TChain with the data tree to the first file name (pointing to other files)
//         1 (true) - merge to the first file name and delete partial files
//         2 (preserve) - merge to the first file name and not delete partial files
//        -1 (false) - not to merge (default if 'merge' attribute is not set
struct structFilePar
{
    // input file path
    string strFileIn;
    // output file path or null if no output file set
    nullString nstrFileOut;
    // start event and event number to be processed (higher priority than for <macro>)
    unique_ptr<int> ptrStartEvent, ptrEventCount;
    // ???
    string strParallelMode;
    // described upper, before the structure
    int iParallelMode, iMerge;
    // command and path for copying to a temporary storage
    string strPutCommand, strPutPath;
    // command and path for copying from a temporary storage
    string strGetCommand, strGetPath;
    // paths of temporary files to be deleted
    vector<string> vecCleanPath;

    structFilePar(): strFileIn(""), nstrFileOut(nullptr), ptrStartEvent(nullptr), ptrEventCount(nullptr),
                     strParallelMode(""), iParallelMode(-1), iMerge(-1),
                     strPutCommand(""), strPutPath(""), strGetCommand(""), strGetPath("") {}
    ~structFilePar() { vecCleanPath.clear(); }
};

// structure with information on macro to be executed
// macro must have the following sequence of parameters: input_file, output_file, start event, event count...
struct structMacroPar
{
    // ROOT macro name
    nullString nstrMacroName;
    // ROOT macro path
    nullString nstrMacroPath;
    // start event and event number to be processed
    unique_ptr<int> ptrStartEvent, ptrEventCount;
    // additional arguments of the macro after 'event count'
    string strAddArgs;
    // file list to be processed
    vector<unique_ptr<structFilePar>> vecFiles;
    structMacroPar(): nstrMacroName(nullptr), nstrMacroPath(nullptr), ptrStartEvent(nullptr), ptrEventCount(nullptr), strAddArgs("") {}
    ~structMacroPar()
    {
        vecFiles.clear();
    }
};

// structure with information on job to be run
struct structJobPar
{
    // job name
    string strJobName;
    // job name of the parent job, from which the current one is dependent
    string strDdependencyName;
    // command line in case of a manual command
    nullString nstrCommandline;
    // run mode: 0 - local (on a multi-core machine), 1 - global (on a cluster), 2 - in container on a cluster
    int iRunMode;
    // container type
    string strContainerType;
    // string with processor count to be used
    nullString nstrProcCount;
    // configuration file path
    string strConfigPath;
    // priority of the job in a batch system
    nullString nstrPriority;
    // string with RAM size requested for one process
    string strOperativeMemory1;
    // whether write a core dump if the task is crashed
    bool isCoreDump;
    // log file path
    nullString nstrLogs;
    // queue name of a batch system
    nullString nstrQueue;
    // Quality of Service name of a batch system
    nullString nstrQOS;
    // a list of hosts, on which the job is performed
    nullString nstrHosts;
    // working directory path
    nullString nstrWorkDir;
    // setting of the Unified Condition Database
    struct_database_settings dbSettings;
    vector<shared_ptr<structMacroPar>> vecMacros;
    structJobPar(): strJobName("scheduler_job"), strDdependencyName(""), nstrCommandline(nullptr),
            iRunMode(0), strContainerType(""), nstrProcCount(nullptr), strConfigPath(""),
            nstrPriority(nullptr), strOperativeMemory1(""), isCoreDump(false),
            nstrLogs(nullptr), nstrQueue(nullptr), nstrQOS(nullptr), nstrHosts(nullptr), nstrWorkDir(nullptr) {}
    ~structJobPar() { vecMacros.clear(); }
};

// structure for parameters to transfer to the main thread in case of the LOCAL mode
struct structThreadPar
{
    // job parameters to use in the main thread
    shared_ptr<structJobPar> sptrJobParameter;
    // semaphore to synchronize all the threads
    shared_ptr<Semaphore> semJobSemaphore;
    // mutex to synchronize all the threads
    shared_ptr<mutex> mutJobMutex;
    // thread number starting from 1
    int iThreadCounter;
};
// structure for parameters to transfer from the main thread to sub-threads
// sub-threads are used to parallelize by events, where threads process individual files
struct structSubThreadPar
{
    // move parameters from the main thread to child ones
    shared_ptr<structThreadPar> sptrThreadParameter;
    // macro parameters to use in the child threads
    shared_ptr<structMacroPar> sptrMacroParameter;
    // start event for the current sub-thread
    int iStartEvent;
    // event count to be processed by the current sub-thread
    int iEventCount;
    // real input file path for the sub-thread
    string strFileIn;
    // real output file path for the sub-thread
    nullString nstrFileOut;
    // sub-thread number
    int iSubthreadCounter;
};

// declaration of function to parse criteria to select data from the Unified Condition Database
void ParseDatabaseParameters(string input, TString& sql, bool is_simulation);
// declaration of function to parse file list for the macro of the job
bool ParseMacroFiles(xmlNodePtr sub_node, shared_ptr<structJobPar> job_par, shared_ptr<structMacroPar> macro_par,
        multimap< string,unique_ptr<vector<nullString>> >& map_job_output, batch_commands batch_com);

// special function to color main info and error messages (cyan or red color)
void info_message(const string msg, bool is_error = 0)
{
    bool is_terminal = isatty(fileno(stdout));

    string color_off("\033[0m");
    string msg_color;	// message color
    if (is_error)
        msg_color = "\033[0;91m";	// light red color for errors
    else
        msg_color = "\033[0;36m";	// cyan color for info message

    if (is_terminal) cout<<msg_color<<msg<<color_off<<endl;
    else cout<<msg<<endl;
}

// replace string in the input text with the following options after ':'
// '~N' - exclude first N-'count' symbols of the new_substring
//  e.g. {file_name_with_ext:~3} - remove first 3 symbols
// '-N' - exclude last N-'count' symbols of the new_substring
//  e.g. {file_name:-4}- remove last 4 symbols
void replace_string_in_text_ext(string &text, string old_substring, string new_substring)
{
    int start = -1, end, count, last_char = old_substring.length()-1;
    do
    {
        start = text.find(old_substring.substr(0, last_char), start + 1);
        if (start > -1)
        {
            if (text[start+last_char] == ':')
            {
                if (text[start+last_char+1] == '~')
                {
                    end = text.find("}", start+last_char+2);
                    if (end > -1)
                    {
                        count = atoi(text.substr(start+last_char+2, end-start-last_char-2).c_str());
                        if (((unsigned int)count) < new_substring.length())
                            text.replace(start, end+1-start, new_substring.substr(count).c_str());
                        else
                            cout<<"Warning: deletion of the first chars ('~' command symbol) was failed."<<endl;
                    }
                }
                if (text[start+last_char+1] == '-')
                {
                    end = text.find("}", start+last_char+2);
                    if (end > -1)
                    {
                        count = atoi(text.substr(start+last_char+2, end-start-last_char-2).c_str());
                        if (((unsigned int)count) < new_substring.length())
                            text.replace(start, end+1-start, new_substring.substr(0, new_substring.length()-count).c_str());
                        else
                            cout<<"Warning: deletion of the last chars ('-' command symbol) was failed."<<endl;
                    }
                }
            }
            else
                text.replace(start, last_char+1, new_substring.c_str());
        }
    }
    while (start > -1);
}

// form output file name containing special (${}) variables
string form_file_name(string out_name, string in_name, int counter, string batch_temp_dir)
{
    replace_string_in_text(out_name, "${counter}", counter);
    replace_string_in_text(out_name, "${batch_temp_dir}", batch_temp_dir);

    replace_string_in_text_ext(out_name, "${input}", in_name);
    replace_string_in_text_ext(out_name, "${file_dir_name}", get_directory_name(in_name));
    // ${file_name_with_ext} must be BEFORE ${file_name} because of searching without the last '}'
    replace_string_in_text_ext(out_name, "${file_name_with_ext}", get_file_name_with_ext(in_name));
    replace_string_in_text_ext(out_name, "${file_name}", get_file_name(in_name));

    // ${first_number_fn} must be BEFORE ${first_number} because of searching without the last '}'
    replace_string_in_text_ext(out_name, "${first_number_fn}", find_first_number(get_file_name(in_name)));
    replace_string_in_text_ext(out_name, "${first_number}", find_first_number(in_name));
    replace_string_in_text_ext(out_name, "${last_number}", find_last_number(in_name));

    if (out_name.find("${user}") != string::npos)
    {
        char* login_name = getlogin();
        if (login_name != nullptr) replace_string_in_text(out_name, "${user}", login_name);
    }

    return out_name;
}

// parse regular expression in the path and return a set of result file paths (for UNIX-systems)
inline vector<string> glob(const string& path)
{
    glob_t glob_result;
    glob(path.c_str(), GLOB_TILDE, nullptr, &glob_result);

    vector<string> ret;
    for (unsigned int i = 0; i < glob_result.gl_pathc; i++)
        ret.push_back(string(glob_result.gl_pathv[i]));

    globfree(&glob_result);
    return ret;
}

// generate output file name concatenating with 'counter' value (for partial results)
string GenerateOutputFilePath(string path, int counter)
{
    size_t last_point_idx = path.find_last_of(".");

    string add_string = "_";
    add_string += int_to_string(counter);

    if (string::npos != last_point_idx)
        return path.insert(last_point_idx, add_string);
    else
        return path.append(add_string);
}

// sub-thread in working thread to process single file
void SubThreadProcessFile(shared_ptr<structSubThreadPar> subthread_par)
{
    shared_ptr<structThreadPar> thread_par = subthread_par->sptrThreadParameter;

    // get necessary parameters transferred to the sub-thread
    string config_file = thread_par->sptrJobParameter->strConfigPath;
    string macro_path = subthread_par->sptrMacroParameter->nstrMacroPath.GetValue();
    string in_file = subthread_par->strFileIn;
    string add_args = subthread_par->sptrMacroParameter->strAddArgs;
    string logs = thread_par->sptrJobParameter->nstrLogs.GetValue();

    int start_event = subthread_par->iStartEvent;
    int event_count = subthread_par->iEventCount;
    nullString out_file = subthread_par->nstrFileOut;

    int thread_counter = thread_par->iThreadCounter;
    int subthread_counter = subthread_par->iSubthreadCounter;

    // using the parameters to generate job string to be executed
    stringstream ssROOTCommand;
    ssROOTCommand << config_file << "root -b -q '" << macro_path <<"(";

    // first macro argument - input file
    bool is_first = true;
    if (in_file != "")
    {
        ssROOTCommand << "\"" << in_file << "\"";
        is_first = false;
    }
    // second macro argument - output file
    if (!out_file.isNull())
    {
        if (!is_first) ssROOTCommand << ", ";
        else is_first = false;
        ssROOTCommand << "\"" << out_file.GetValue() << "\"";
    }
    // third macro argument - start event number
    if (!is_first) ssROOTCommand << ", ";
    else is_first = false;
    ssROOTCommand << to_string(start_event);
    // fourth macro argument - event count
    if (!is_first) ssROOTCommand << ", ";
    else is_first = false;
    ssROOTCommand << to_string(event_count);
    // then add other arguments to the macro
    if (add_args != "")
    {
        if (!is_first) ssROOTCommand << ", ";
        else is_first = false;
        ssROOTCommand << add_args;
    }

    // end of the command with writing to the log or not
    if (logs != "")
        ssROOTCommand << ")' > " << logs << "_" << to_string(thread_counter) << "_" << to_string(subthread_counter) << " 2>&1";
    else
        ssROOTCommand << ")'";

    // write and execute temporary bash file
    ofstream myfile;
    long int t = time(nullptr);
    // generate output name for temporary bash file
    stringstream ssBashFile;
    ssBashFile << "temp_" << to_string(thread_counter) << "_" << to_string(subthread_counter)
               << "_" << to_string(t) <<".sh";
    // open bash file and run the ROOT macro command
    myfile.open(ssBashFile.str().c_str());
    myfile<<"export ROOT_HIST=0 "<<endl;
    myfile<<ssROOTCommand.str();
    myfile.close();
    stringstream ssRunCommand;
    ssRunCommand << "bash " << ssBashFile.str();

    // lock thread for sequential output
    thread_par->mutJobMutex->lock();
    cout<<"nica-scheduler$ Subtask "<<thread_counter<<":"<<subthread_counter<<" is running:"<<endl<<"input - "<<in_file<<endl
        <<"output - "<<(out_file.isNull() ? "<no output>" : out_file.GetValue())<<endl<<"start event - "<<start_event<<endl
        <<"event count - "<<event_count<<endl<<endl;
    thread_par->mutJobMutex->unlock();

    // run bash file with the job in the sub-thread
    int system_return = system(ssRunCommand.str().c_str());
    if (system_return != 0)
        cout<<"nica-scheduler$ WARNING: System call (in SubThreadProcessFile) returned non-zero code: "<<system_return<<endl;

    //delete temporary bash file
    if (!isDebugMode)
    {
        stringstream ssDeleteCommand;
        ssDeleteCommand << "rm -rf " << ssBashFile.str();
        system_return = system(ssDeleteCommand.str().c_str());
        if (system_return != 0)
            cout<<"nica-scheduler$ WARNING: System call (in SubThreadProcessFile) returned non-zero code: "<<system_return<<endl;
    }
    else
        cout<<"DEBUG nica-scheduler$ '"<<ssBashFile.str()<<"' bash file was used "<<endl;

    thread_par->semJobSemaphore->release();
}

/* MAIN THREAD for LOCAL execution */
void ThreadLocalProcess(shared_ptr<structThreadPar> thread_par)
{
    // initialization of variables
    shared_ptr<structJobPar> job_par = thread_par->sptrJobParameter;
    int thread_counter = thread_par->iThreadCounter;

    // cycle for all macros in the job
    for (size_t ind_macro = 0; ind_macro < job_par->vecMacros.size(); ind_macro++)
    {
        shared_ptr<structMacroPar> cur_macro = job_par->vecMacros[ind_macro];

        // get current file for the current thread
        structFilePar* cur_file = nullptr;
        if (cur_macro->vecFiles.size() > 0)
            cur_file = cur_macro->vecFiles[thread_counter-1].get();

        // if macro use no input files, then add only additional arguments if exist
        if (cur_file == nullptr)
        {
            // generate ROOT command from the job parameters without input and output files
            stringstream ssROOTCommand;
            ssROOTCommand << job_par->strConfigPath << "root -b -q '" << cur_macro->nstrMacroPath.GetValue() << "(";
            if (cur_macro->strAddArgs != "")
                ssROOTCommand << cur_macro->strAddArgs;
            if (job_par->nstrLogs != nullptr)
                ssROOTCommand << ")' > " << job_par->nstrLogs.GetValue() << "_" << to_string(thread_counter) << " 2>&1";
            else
                ssROOTCommand << ")'";

            // display local command in DEBUG mode for processing without input files
            if (isDebugMode)
                cout<<"DEBUG nica-scheduler$ Local ROOT command = "<<ssROOTCommand.str()<<endl;

            // write and execute temporary bash file
            ofstream myfile;
            long int t = time(nullptr);
            // generate output name for temporary bash file
            stringstream ssBashFile;
            ssBashFile << "temp_" << to_string(thread_counter) << "_" << to_string(t) <<".sh";
            // open bash file and run the ROOT macro command
            myfile.open(ssBashFile.str().c_str());
            myfile<<"export ROOT_HIST=0 "<<endl;
            myfile<<ssROOTCommand.str();
            myfile.close();
            stringstream ssRunCommand;
            ssRunCommand << "bash " << ssBashFile.str();

            // lock thread for sequential output
            thread_par->mutJobMutex->lock();
            cout<<"nica-scheduler$ Task "<<ind_macro+1<<" is running..."<<endl<<endl;
            thread_par->mutJobMutex->unlock();

            // run bash file with the job in the sub-thread
            int system_return = system(ssRunCommand.str().c_str());
            if (system_return != 0)
                cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;

            //delete temporary bash file
            if (!isDebugMode)
            {
                stringstream ssDeleteCommand;
                ssDeleteCommand << "rm -rf " << ssBashFile.str();
                system_return = system(ssDeleteCommand.str().c_str());
                if (system_return != 0)
                    cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;
            }
            else
                cout<<"DEBUG nica-scheduler$ '"<<ssBashFile.str()<<"' bash file was used "<<endl;

        }// if (cur_file == nullptr)
        // if there are input files for the macro
        else
        {
            // strRealInputFile - input file path for ROOT macro (for intermediate copy case)
            string strRealInputFile = cur_file->strFileIn;
            // strRealOutputFile - output file path for ROOT macro (for intermediate copy case)
            nullString nstrRealOutputFile = cur_file->nstrFileOut;

            // copy file to the temporary location if it is set
            if (cur_file->strPutPath != "")
            {
                stringstream ssCopyCommand;
                ssCopyCommand << cur_file->strPutCommand << " " << cur_file->strFileIn << " " << cur_file->strPutPath;
                int system_return = system(ssCopyCommand.str().c_str());
                if (system_return != 0)
                    cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;

                strRealInputFile = cur_file->strPutPath;
            }
            // assign real output file on the intermediate storage
            if (cur_file->strGetPath != "")
                nstrRealOutputFile.SetValue(cur_file->strGetPath);

            // get start event for the current file
            int* cur_start_event = cur_macro->ptrStartEvent.get();
            if (cur_file->ptrStartEvent != nullptr)
                cur_start_event = cur_file->ptrStartEvent.get();

            // get event number for the current file
            int* cur_event_count = cur_macro->ptrEventCount.get();
            if (cur_file->ptrEventCount != nullptr)
                cur_event_count = cur_file->ptrEventCount.get();

            // if parallel mode and start event is skipped then assign it to 0
            if ((cur_file->iParallelMode > 1) && (cur_start_event == nullptr))
            {
                cur_file->ptrStartEvent = make_unique<int>(0);
                cur_start_event = cur_file->ptrStartEvent.get();
                cout<<"nica-scheduler$ WARNING: parallel_mode is used, but start event is not set, so it is assigned to 0"<<endl;
            }
            // if parallel mode and event coun is skipped then assign it to 0
            if ((cur_file->iParallelMode > 1) && (cur_event_count == nullptr))
            {
                cur_file->ptrEventCount = make_unique<int>(0);
                cur_event_count = cur_file->ptrEventCount.get();
                cout<<"nica-scheduler$ WARNING: parallel_mode is used, but event count is not set, so it is assigned to 0"<<endl;
            }

            // reset parallel mode if start event or event count is not set
            if ((cur_file->iParallelMode > 1) && ((cur_start_event == nullptr) || (cur_event_count == nullptr)))
            {
                cur_file->iParallelMode = 1;
                cout<<"nica-scheduler$ WARNING: parallel_mode must be used with 'start_event' and 'event_count' parameters, parallel_mode is ignored!"<<endl;
            }

            int real_thread_count = 1;
            // concatenation of output file paths to wait for
            string strOutputUnion = nstrRealOutputFile.GetValue();

            // variables to combine parallel mode > 1 amd = 1 in one cycle
            int start_event = -1, event_count = -1;
            if (cur_start_event != nullptr)
                start_event = *cur_start_event;
            if (cur_event_count != nullptr)
                event_count = *cur_event_count;

            // if parallel mode and event count is 0, then trying to get event number from the input file
            if ((cur_file->iParallelMode > 1) && (event_count == 0))
            {
                string config_paths = job_par->strConfigPath;
                // send output of the configuration files to /dev/null if no log file is set
                if (job_par->nstrLogs == nullptr)
                    replace_string_in_text(config_paths, ";", " > /dev/null 2>&1;");
                stringstream ssEventNumberCommand;
                ssEventNumberCommand << config_paths << "show_event_count auto \""
                                     << strRealInputFile << "\" 2> /dev/null";
                if (isDebugMode)
                    cout<<"DEBUG nica-scheduler$ trying to get event count from the input file: " << ssEventNumberCommand.str() <<endl;

                array<char, 128> bufferCommand;
                string str_event_number = "";
                FILE* stream = popen(ssEventNumberCommand.str().c_str(), "r");
                while (fgets(bufferCommand.data(), static_cast<int>(bufferCommand.size()), stream) != nullptr)
                    str_event_number += bufferCommand.data();
                pclose(stream);
                str_event_number = find_first_number(str_event_number);
                if (str_event_number == "")
                {
                    info_message(TString::Format("ERROR nica-scheduler$ parallel_mode is used, but event_count is not set and cannot be automatically defined. "
                                 "'%s' file will be skipped!", strRealInputFile.c_str()).Data(), 1);
                    break;
                }
                event_count = atoi(str_event_number.c_str());
                if (isDebugMode)
                    cout<<"DEBUG nica-scheduler$ "<< event_count << " event(s) found in the input file" <<endl;
            }

            // if parallel mode and start event is 0, then trying to get start event from the input file
            vector<nullString> vecPartialOutputFiles;
            /***********************************************************************/
            /* if parallel mode > 1 - parallel local processing for one input file */
            /***********************************************************************/
            if (cur_file->iParallelMode > 1)
            {
                // create sub-threads to additionally parallelize by events in case of 'parallel_mode'
                vector<thread> vecSubThreads;

                // fill sub-threads parameters and run them to parallelize by events
                for (int i = 0; i < cur_file->iParallelMode; i++)
                {
                    int event_per_thread = (event_count+i)/cur_file->iParallelMode;
                    if (event_per_thread != 0)
                    {
                        nullString nstr_output_file = nstrRealOutputFile;
                        // generate output file name for particular processing by events
                        if (nstr_output_file != nullptr)
                            nstr_output_file.SetValue(GenerateOutputFilePath(nstr_output_file.GetValue(), real_thread_count));

                        // fill parameters for the current sub-thread (with the current event portion)
                        shared_ptr<structSubThreadPar> subthread_par = make_shared<structSubThreadPar>();
                        subthread_par->sptrThreadParameter = thread_par;
                        subthread_par->sptrMacroParameter = cur_macro;
                        subthread_par->iStartEvent = start_event;
                        subthread_par->iEventCount = event_per_thread;
                        subthread_par->strFileIn = strRealInputFile;
                        subthread_par->nstrFileOut = nstr_output_file;
                        subthread_par->iSubthreadCounter = i+1;

                        // release the semaphore for the current thread to organize parallelization in sub-threads
                        if (i == 0)
                            thread_par->semJobSemaphore->release();

                        thread_par->semJobSemaphore->acquire();

                        // create sub-threads to parallelize one file processing by events
                        try
                        {
                            vecSubThreads.emplace_back(SubThreadProcessFile, subthread_par);
                        }
                        catch (const system_error& e)
                        {
                            info_message((TString::Format("ERROR nica-scheduler$ error while creating sub-thread with code = %d and message = %s",
                                          e.code().value(), e.what())).Data(), 1);
                            exit(-1);
                        }

                        if (nstr_output_file != nullptr)
                        {
                            strOutputUnion += " " + nstr_output_file.GetValue();
                            vecPartialOutputFiles.push_back(nstr_output_file);
                        }

                        real_thread_count++;
                    }//if (event_per_thread != 0){

                    start_event = start_event + event_per_thread;
                }// for (int i = 0; i < cur_file->iParallelMode; i++)

                // waiting for finishing of the child threads
                if (isDebugMode)
                    cout<<"nica-scheduler$ Waiting for "<<real_thread_count<<" subtask(s) to finish..."<<endl;
                for (auto& sub_thread : vecSubThreads)
                {
                    try
                    {
                        sub_thread.join();
                    }
                    catch (const system_error& e)
                    {
                        info_message((TString::Format("ERROR nica-scheduler$ Sub-thread failed with code = %d and message = %s",
                                      e.code().value(), e.what())).Data(), 1);
                    }
                }
                cout<<"nica-scheduler$ Subtasks "<<ind_macro+1<<":[] has been finished"<<endl;
                thread_par->semJobSemaphore->acquire();

                // merge result files or write to single TChain object
                // iMerge: 0 - save result TChain; 1 - merge to the origin file name and delete partial files;
                // 2 - merge to the origin file name and NOT delete partial files
                if ((cur_file->iMerge >= 0) && (strOutputUnion != ""))
                {
                    string UNIONc_path = get_app_dir_linux() + "merge_result.C";

                    // form command for merging result files
                    stringstream ssMergeCommand;
                    ssMergeCommand << job_par->strConfigPath << "root -b -q '" << UNIONc_path << "(\""
                          << strOutputUnion << "\", " << to_string(cur_file->iMerge) << ")'";

                    // save merging log to file if it is set
                    if (job_par->nstrLogs != nullptr)
                        ssMergeCommand << " >> " << job_par->nstrLogs.GetValue() << " 2>&1";

                    // run merge_result.C macro in bash
                    ofstream myfile;
                    long int t = time(nullptr);
                    stringstream ssBashFile;
                    ssBashFile << "temp_union_" << to_string(t) << ".sh";
                    myfile.open(ssBashFile.str().c_str());
                    myfile << ssMergeCommand.str();
                    myfile.close();
                    stringstream ssBashCommand;
                    ssBashCommand << "bash "<< ssBashFile.str().c_str();

                    cout<<endl<<"nica-scheduler$ Merging the result of subtasks "<<ind_macro+1<<":[]..."<<endl<<endl;
                    int system_return = system(ssBashCommand.str().c_str());
                    if (system_return != 0)
                        cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;

                    //delete temporary bash file
                    if (!isDebugMode)
                    {
                        stringstream ssRemoveCommand;
                        ssRemoveCommand << "rm -rf " << ssBashFile.str();
                        system_return =  system(ssRemoveCommand.str().c_str());
                        if (system_return != 0)
                            cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;
                    }
                    else
                        cout<<"DEBUG nica-scheduler$ '"<<ssBashFile.str()<<"' bash file was used "<<endl;
                }//if - merge result files
            }//if (parallel_mode > 1)
            /************************/
            /* if parallel mode = 1 */
            /************************/
            else
            {
                // using the parameters to generate job string to be executed
                stringstream ssROOTCommand;
                ssROOTCommand << job_par->strConfigPath << "root -b -q '" << cur_macro->nstrMacroPath.GetValue() <<"(";

                // first macro argument - input file
                bool is_first = true;
                if (strRealInputFile != "")
                {
                    ssROOTCommand << "\"" << strRealInputFile << "\"";
                    is_first = false;
                }
                // second macro argument - output file
                if (nstrRealOutputFile != nullptr)
                {
                    if (!is_first) ssROOTCommand << ", ";
                    else is_first = false;
                    ssROOTCommand << "\"" << nstrRealOutputFile.GetValue() << "\"";
                }
                // third macro argument - start event number
                if (cur_start_event != nullptr)
                {
                    if (!is_first) ssROOTCommand << ", ";
                    else is_first = false;
                    ssROOTCommand << to_string(*cur_start_event);
                }
                // fourth macro argument - event count
                if (cur_event_count != nullptr)
                {
                    if (!is_first) ssROOTCommand << ", ";
                    else is_first = false;
                   ssROOTCommand << to_string(*cur_event_count);
                }
                // then add other arguments to the macro
                if (cur_macro->strAddArgs != "")
                {
                    if (!is_first) ssROOTCommand << ", ";
                    else is_first = false;
                    ssROOTCommand << cur_macro->strAddArgs;
                }

                // end of the command with writing to the log or not
                if (job_par->nstrLogs != nullptr)
                    ssROOTCommand << ")' > " << job_par->nstrLogs.GetValue() << "_" << to_string(thread_counter) << " 2>&1";
                else
                    ssROOTCommand << ")'";

                // display local ROOT command in debug mode for processing with input files
                if (isDebugMode)
                    cout<<"DEBUG nica-scheduler$ Local command = "<<ssROOTCommand.str()<<endl;

                // write and execute temporary bash file
                ofstream myfile;
                long int t = time(nullptr);
                // generate output name for temporary bash file
                stringstream ssBashFile;
                ssBashFile << "temp_" << to_string(thread_counter) << "_" << "_" << to_string(t) <<".sh";
                // open bash file and run the ROOT macro command
                myfile.open(ssBashFile.str().c_str());
                myfile<<"export ROOT_HIST=0 "<<endl;
                myfile<<ssROOTCommand.str();
                myfile.close();
                stringstream ssRunCommand;
                ssRunCommand << "bash " << ssBashFile.str();

                // lock thread for sequential output
                thread_par->mutJobMutex->lock();
                cout<<"nica-scheduler$ Task "<<thread_counter<<" is running..."<<endl;
                if (strRealInputFile != "") cout<<"input data - "<<strRealInputFile<<endl;
                if (nstrRealOutputFile != nullptr) cout<<"output data - "<<nstrRealOutputFile.GetValue()<<endl;
                cout<<endl;
                thread_par->mutJobMutex->unlock();

                // run bash file with the job in the sub-thread
                int system_return = system(ssRunCommand.str().c_str());
                if (system_return != 0)
                    cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;

                // delete temporary bash file
                if (!isDebugMode)
                {
                    stringstream ssRemoveCommand;
                    ssRemoveCommand << "rm -rf " << ssBashFile.str();
                    system_return =  system(ssRemoveCommand.str().c_str());
                    if (system_return != 0)
                        cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;
                }
                else
                    cout<<"DEBUG nica-scheduler$ '"<<ssBashFile.str()<<"' bash file was used "<<endl;
            }// else (parallel_mode = 1)

            // copy file (or files in case of parallel_mode > 1) from the temporary location to the final output path
            if ((cur_file->strGetPath != "") && (cur_file->nstrFileOut != nullptr))
            {
                stringstream ssCopyCommand;
                ssCopyCommand << cur_file->strGetCommand <<" " << cur_file->strGetPath << " " << cur_file->nstrFileOut.GetValue();
                int system_return = system(ssCopyCommand.str().c_str());
                if (system_return != 0)
                    cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;

                // also copy partial result files for parallel_mode > 1
                if ((cur_file->iParallelMode > 1) && ((cur_file->iMerge == 0) || (cur_file->iMerge == 2)))
                {
                    for (nullString& cur_partial_file : vecPartialOutputFiles)
                    {
                        stringstream ssCopyPartialsCommand;
                        ssCopyPartialsCommand << cur_file->strGetCommand << " " << cur_partial_file.GetValue() << " "
                                << get_directory_path(cur_file->nstrFileOut.GetValue()) + string("/") +
                                   get_file_name_with_ext(cur_partial_file.GetValue());
                        system_return = system(ssCopyPartialsCommand.str().c_str());
                        if (system_return != 0)
                            cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;
                     }
                } // if iParallelMode > 1 && iMerge = 0,2 then copy all partial result files
            }// copy file from the temporary location to the output path

            // remove the temporary input files from <put>
            if (cur_file->strPutPath != "")
            {
                stringstream ssRemoveCommand;
                ssRemoveCommand << "rm -rf " << cur_file->strPutPath;
                int system_return = system(ssRemoveCommand.str().c_str());
                if (system_return != 0)
                    cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;
            }
            // remove the temporary output files from <get>
            if (cur_file->strGetPath != "")
            {
                stringstream ssRemoveCommand;
                ssRemoveCommand << "rm -rf " << cur_file->strGetPath;
                int system_return = system(ssRemoveCommand.str().c_str());
                if (system_return != 0)
                    cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;

                // remove partial result files in case of the parallel mode for by-event processing
                if ((cur_file->iParallelMode > 1) && ((cur_file->iMerge == 0) || (cur_file->iMerge == 2)))
                {
                    for (nullString& cur_partial_file : vecPartialOutputFiles)
                    {
                        stringstream ssRemovePartialCommand;
                        ssRemovePartialCommand << "rm -rf " << cur_partial_file.GetValue();
                        system_return = system(ssRemovePartialCommand.str().c_str());
                        if (system_return != 0)
                            cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;
                    }
                }// if iParallelMode > 1 && iMerge = 0,2 then delete all partial result files
            }// if (cur_file->strGetPath != "")

            // remove the temporary files from <clean>
            if (!cur_file->vecCleanPath.empty())
            {
                stringstream ssRemoveCommand;
                for (const string& cur_clean_path : cur_file->vecCleanPath)
                    ssRemoveCommand << "rm -rf " << cur_clean_path << "; ";
                int system_return = system(ssRemoveCommand.str().c_str());
                if (system_return != 0)
                    cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;
            }

            if (cur_file->iParallelMode == 1)
                cout<<"nica-scheduler$ Task "<<thread_counter<<" has been finished"<<endl;
        }// if (cur_file != nullptr)
    }// for (size_t ind_macro = 0; ind_macro < job_par->vecMacros.size(); ind_macro++)

    thread_par->semJobSemaphore->release();
}

// options for running the NICA-Scheduler
static struct option long_options[] =
{
        {"help",    no_argument,        0, 'h'},
        {"file",    required_argument,  0, 'f'},
        {"debug",   no_argument,        0, 'd'},
        {"db",      required_argument,  0,  0 },
        {"database",required_argument,  0,  0 },
        {0, 0, 0, 0}
};

// help message for the NICA-Scheduler
void ShowHelpMessage()
{
    cout<<"Usage: nica-scheduler job_description.xml [options]"<<endl<<endl<<
          "Options:"<<endl<<
          "  -h/--help              : produce help message"<<endl<<
          "  -f/--file arg          : set XML file path with a job description"<<endl<<
          "  -d/--debug             : debug mode with detailed output"<<endl<<
          "  --db/--database arg    : predefined settings for the Condition Database (now 'bmn' supported only)"<<endl<<endl;
}

/* Main Function of the NICA Scheduler*/
int main(int argc, char** argv)
{
    // if no necessary arguments are given for the NICA-Scheduler
    if (argc < 2)
    {
        ShowHelpMessage();
        info_message("ERROR nica-scheduler$ XML file path (containing job description) must be set", 1);
        return 1;
    }

    // parsing command line arguments
    string xml_file_path = "", exp_database = "";
    int option_index = 0, c;
    while ((c = getopt_long(argc, argv, "hf:d", long_options, &option_index)) != -1)
    {
        switch (c)
        {
            case 0:
            {
                if (long_options[option_index].flag != 0)
                    break;
                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf("with arg %s", optarg);
                printf ("\n");

                if (strcmp(long_options[option_index].name, "db") == 0)
                    exp_database = optarg;
                if (strcmp(long_options[option_index].name, "database") == 0)
                    exp_database = optarg;
                break;
            }
            case 'h':
                ShowHelpMessage();
                return 0;
            case 'f':
                xml_file_path = optarg;
                break;
            case 'd':
                isDebugMode = true;
                break;
            case '?':
                /* getopt_long already printed an error message. */
                break;
        }
    }
    // remaining command line arguments (not options)
    if (optind < argc)
    {
        if (((argc - optind) == 1) && (xml_file_path == ""))
            xml_file_path = argv[optind];
        else
        {
            info_message("ERROR nica-scheduler$ Argument list is incorrect.\nPlease, check information on command line arguments in the 'README.md' file", 1);
            return 1;
        }
    }

    // check that XML job file is not empty
    if (xml_file_path == "")
    {
        info_message("ERROR nica-scheduler$ XML file path (containing job description) must be set", 1);
        return 1;
    }

    // high means the high priority value of the parameter (a console parameter has the highest priority for the settings)
    bool isDBSettingsHigh = false;
    struct_database_settings dbSettingsPre;
    if (exp_database != "")
    {
        auto iter_db_settings = predefined_database_settings.find(exp_database);
        if (iter_db_settings == predefined_database_settings.end())
        {
            info_message((TString::Format("ERROR nica-scheduler$ Predefined database settings were not found for '%s'", exp_database.c_str())).Data(), 1);
            return 1;
        } else {
            dbSettingsPre = iter_db_settings->second;
            isDBSettingsHigh = true;
        }
    }

    // obtain pointer to XML document for a given input file path
    xmlDocPtr doc = xmlReadFile(xml_file_path.c_str(), nullptr, 0);
    if (doc == nullptr)
    {
        info_message((TString::Format("ERROR nica-scheduler$ Failed to open input XML file: '%s'", xml_file_path.c_str())).Data(), 1);
        return 2;
    }

    // get root element of the XML job description
    xmlNodePtr root = xmlDocGetRootElement(doc);

    // map with Job Name -> ID
    multimap<string, string> mapJobName2ID;
    // map with Job Name -> Output List
    multimap< string, unique_ptr<vector<nullString>> > mapJobName2FileOut;

    // path to executable's directory ('/' - last char)
    string exe_dir = get_app_dir_linux();

    // initialization before job cycle
    srand(time(nullptr));
    BATCH_SYSTEM_NAME batch_system = SLURM_BATCH_SYSTEM;	// default value: SLURM batch system

    // cycle for all jobs in the XML description
    xmlNodePtr cur_node = root;
    while (cur_node)
    {
        // if tag is XML_ELEMENT_NODE then continue
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            char *pc_temp; int i_temp;
            // JOBS TAG (array of jobs) then step down and continue
            if (strcmp((char*)cur_node->name, "jobs") == 0)
            {
                // read BATCH system name in the XML description
                pc_temp = (char*) xmlGetProp(cur_node, (unsigned char*)"batch");
                if (pc_temp != nullptr)
                {
                    char* pc_batch_system = convert_pchar_to_lowercase_new(pc_temp);
                    if (strcmp(pc_batch_system, "sge") == 0)
                        batch_system = SGE_BATCH_SYSTEM;
                    else
                    {
                        if (strcmp(pc_batch_system, "torque") == 0)
                            batch_system = TORQUE_BATCH_SYSTEM;
                    }
                    free(pc_temp);
                    free(pc_batch_system);
                }
                // read UniConDa database settings in the XML description
                pc_temp = (char*) xmlGetProp(cur_node, (unsigned char*)"database");
                if ((pc_temp != nullptr) && (!isDBSettingsHigh))
                {
                    auto iter_db_settings = predefined_database_settings.find(pc_temp);
                    if (iter_db_settings == predefined_database_settings.end())
                        info_message((TString::Format("WARNING nica-scheduler$ Predefined database settings were not found for '%s'. The parameter will be ignored.", pc_temp)).Data(), 1);
                    else
                        dbSettingsPre = iter_db_settings->second;
                    free(pc_temp);
                }

                cur_node = cur_node->children;
            }

            // JOB TAG
            if (strcmp((char*)cur_node->name, "job") == 0)
            {
                bool isError = false;
                //bool isPuts = false, isGets = false, isCleans = false;
                shared_ptr<structJobPar> job_par = make_shared<structJobPar>();

                // read Job Name in the XML description
                pc_temp = (char*) xmlGetProp(cur_node, (unsigned char*)"name");
                if (pc_temp != nullptr)
                {
                    job_par->strJobName = pc_temp;
                    free(pc_temp);
                }

                struct_database_settings dbSettings(dbSettingsPre);
                // read UniConDa database settings in the XML description
                pc_temp = (char*) xmlGetProp(cur_node, (unsigned char*)"database");
                if ((pc_temp != nullptr) && (!isDBSettingsHigh))
                {
                    auto iter_db_settings = predefined_database_settings.find(pc_temp);
                    if (iter_db_settings == predefined_database_settings.end())
                        info_message((TString::Format("WARNING nica-scheduler$ Predefined database settings were not found for '%s'. The parameter will be ignored.", pc_temp)).Data(), 1);
                    else
                        dbSettings = iter_db_settings->second;
                    free(pc_temp);
                }
                if (dbSettings.db_name == "")
                {
                    dbSettings.db_host = UNI_DB_HOST; dbSettings.db_name = UNI_DB_NAME;
                    dbSettings.db_username = UNI_DB_USERNAME; dbSettings.db_password = UNI_DB_PASSWORD;
                }
                job_par->dbSettings = dbSettings;

                // read BATCH system name in the XML description
                pc_temp = (char*) xmlGetProp(cur_node, (unsigned char*)"batch");
                if (pc_temp != nullptr)
                {
                    char* pc_batch_system = convert_pchar_to_lowercase_new(pc_temp);
                    if (strcmp(pc_batch_system, "slurm") == 0)
                        batch_system = SLURM_BATCH_SYSTEM;
                    else
                    {
                        if (strcmp(pc_batch_system, "torque") == 0)
                            batch_system = TORQUE_BATCH_SYSTEM;
                    }
                    free(pc_temp);
                    free(pc_batch_system);
                }
                batch_commands batch_com = mapBS[batch_system];

                // fill BASH file content to be executed in the BATCH system
                // transferring individual parameters to the JOB ARRAY using DECLARE bash variable
                // DECLARE method can lead to the overflow of the content of the variables (mb better "CAT << EOF" files method)
                string strBatchFile = batch_com.job_begin,
                strBatchInitInputFile = "declare -a InputFileArray=(", strBatchInitOutputFile = "declare -a OutputFileArray=(",
                strBatchInitStartEvent = "declare -a StartEventArray=(", strBatchInitEventCount = "declare -a EventCountArray=(",
                strBatchInitAddArgs = "declare -a AddArgsArray=(", strBatchInitPut = "declare -a PutArray=(",
                strBatchInitGet = "declare -a GetArray=(", strBatchInitClean = "declare -a CleanArray=(";

                // read Job Dependency in the XML description
                pc_temp = (char*) xmlGetProp(cur_node, (unsigned char*)"dependency");
                if (pc_temp != nullptr)
                {
                    multimap<string,string>::iterator it = mapJobName2ID.find(pc_temp);
                    if (it == mapJobName2ID.end())
                        cout<<"nica-scheduler$ WARNING: the following dependency for '"<<job_par->strJobName
                            <<"' job was not found: "<<pc_temp<<endl;
                    else
                        job_par->strDdependencyName = batch_com.array_dependency_option + it->second;
                    free(pc_temp);
                }

                /* PARSING TAGS of the JOB */
                xmlNodePtr sub_node = cur_node->children;
                while (sub_node)
                {
                    if (sub_node->type == XML_ELEMENT_NODE)
                    {
                        // COMMAND LINE TAG
                        if (strcmp((char*)sub_node->name, "command") == 0)
                        {
                            if (job_par->nstrCommandline != nullptr)
                                cout<<"nica-scheduler$ WARNING: job command line is reassigned"<<endl;

                            job_par->nstrCommandline.SetValue((char*) xmlGetProp(sub_node, (unsigned char*)"line"));
                            if (job_par->nstrCommandline == nullptr)
                            {
                                isError = true;
                                info_message("ERROR nica-scheduler$ 'line' attribute was not set for the command!", 1);
                                break;
                            }

                            sub_node = sub_node->next;
                            continue;
                        }

                        // MACRO NAME TAG
                        if (strcmp((char*)sub_node->name, "macro") == 0)
                        {
                            shared_ptr<structMacroPar> macro_par = make_shared<structMacroPar>();

                            // read Macro Name in the XML description (for dependency from macro)
                            // macro_par->nstrMacroName.SetValue((char*) xmlGetProp(sub_node, (unsigned char*)"name"));
                            // read Macro Path in the XML description
                            macro_par->nstrMacroPath.SetValue((char*) xmlGetProp(sub_node, (unsigned char*)"path"));
                            if (macro_par->nstrMacroPath == nullptr)
                            {
                                isError = true;
                                info_message("ERROR nica-scheduler$ Macro path was not set (please, add 'path' attribute)!", 1);
                                break;
                            }

                            // read Global Start Event value in the XML description
                            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"start_event");
                            if (pc_temp != nullptr)
                            {
                                macro_par->ptrStartEvent = make_unique<int>(atoi(pc_temp));
                                free(pc_temp);
                            }

                            // read Global Event Count value in the XML description
                            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"event_count");
                            if (pc_temp != nullptr)
                            {
                                if (!is_string_number(pc_temp))
                                {
                                    isError = true;
                                    info_message("ERROR nica-scheduler$ Event count must be a number!", 1);
                                    free(pc_temp);
                                    break;
                                }
                                macro_par->ptrEventCount = make_unique<int>(atoi(pc_temp));
                                if (*macro_par->ptrEventCount < 0)
                                {
                                    isError = true;
                                    info_message("ERROR nica-scheduler$ Event count must be a positive number or 0 (for all events)!", 1);
                                    free(pc_temp);
                                    break;
                                }
                                free(pc_temp);
                            }

                            // read Additional Arguments of the macro in the XML description
                            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"add_args");
                            if (pc_temp != nullptr)
                            {
                                macro_par->strAddArgs = pc_temp;
                                free(pc_temp);
                                // replace ''' symbols for '"'
                                replace_string_in_text(macro_par->strAddArgs, "'", "\"");
                            }

                            // read File List to process by the macro in the XML description
                            if (ParseMacroFiles(sub_node->children, job_par, macro_par, mapJobName2FileOut, batch_com) == true)
                                isError = true;

                            job_par->vecMacros.push_back(macro_par);

                            sub_node = sub_node->next;
                            continue;
                        }

                        // RUN TYPE TAG
                        if (strcmp((char*)sub_node->name, "run") == 0)
                        {
                            // read Run Mode in the XML description: local, global or container
                            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"mode");
                            if (pc_temp != nullptr)
                            {
                                if (strcmp(pc_temp, "global") == 0)
                                    job_par->iRunMode = 1;
                                else if (strcmp(pc_temp, "container") == 0)
                                    job_par->iRunMode = 2;

                                free(pc_temp);
                            }

                            // read Processor Count to use in the XML description
                            // it will be corrected according to count of subtasks
                            job_par->nstrProcCount.SetValue((char*) xmlGetProp(sub_node, (unsigned char*)"count"));
                            // read Operative Memory required for 1 task
                            // the RAM size is converted to MegaBytes
                            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"memory1");
                            if (pc_temp != nullptr)
                            {
                                i_temp = floor(byte_size_to_double(pc_temp, 'M') + 0.5);
                                if (i_temp > 0)
                                    job_par->strOperativeMemory1 = int_to_string(i_temp);
                                else
                                    cout<<"nica-scheduler$ WARNING: 'memory1' field contains incorrect format, then it is ignored"<<endl;

                                   free(pc_temp);
                            }
                            // read Configuration File in the XML description
                            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"config");
                            if (pc_temp != nullptr)
                            {
                                job_par->strConfigPath = pc_temp;
                                free(pc_temp);
                            }
                            // read Job Priority in the XML description
                            job_par->nstrPriority.SetValue((char*) xmlGetProp(sub_node, (unsigned char*)"priority"));
                            // read Log File Path in the XML description
                            job_par->nstrLogs.SetValue((char*) xmlGetProp(sub_node, (unsigned char*)"logs"));
                            // read Queue Name of the batch system in the XML description
                            job_par->nstrQueue.SetValue((char*) xmlGetProp(sub_node, (unsigned char*)"queue"));
                            // read Quality-Of-Service Name of the batch system in the XML description
                            job_par->nstrQOS.SetValue((char*) xmlGetProp(sub_node, (unsigned char*)"qos"));
                            // read Working Directory of the batch system in the XML description
                            job_par->nstrWorkDir.SetValue((char*) xmlGetProp(sub_node, (unsigned char*)"work_dir"));
                            // read Container OS in the XML description
                            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"container_os");
                            if (pc_temp != nullptr)
                            {
                                job_par->strContainerType = pc_temp;
                                free(pc_temp);
                            }

                            // read Host List, on which the job will be executed
                            job_par->nstrHosts.SetValue((char*) xmlGetProp(sub_node, (unsigned char*)"hosts"));
                            // change comma separator with a specific symbol corresponding to the batch system
                            if ((job_par->nstrHosts != nullptr) && (batch_com.sheduler_hosts_separation != ','))
                                replace(job_par->nstrHosts.GetValue().begin(), job_par->nstrHosts.GetValue().end(),
                                        ',', batch_com.sheduler_hosts_separation);

                            // read Core Dump Flag (whether write a core dump in case of errors)
                            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"core_dump");
                            if (pc_temp != nullptr)
                            {
                                   if (strcmp(pc_temp, "true") == 0)
                                       job_par->isCoreDump = true;
                                free(pc_temp);
                            }

                            sub_node = sub_node->next;
                            continue;
                        }
                    }

                    sub_node = sub_node->next;
                }//while PARSING TAGS

                /*****************************************************/
                /* START MAIN EXECUTION AFTER PARSING XML PARAMETERS */
                /*****************************************************/
                // check that there are macros to execute and it is not a command line
                if ((job_par->nstrCommandline == nullptr) && (job_par->vecMacros.size() == 0))
                {
                    info_message("ERROR nica-scheduler$ No macros in the job!", 1);
                    isError = true;
                }
                // if errors are present while parsing parameters then continue
                if (isError)
                {
                    info_message("ERROR nica-scheduler$ This job will be skipped because of errors above!", 1);
                    cur_node = cur_node->next;
                    continue;
                }

                // store list with output file paths for the whole job (the last macro)
                unique_ptr<vector<nullString>> vecJobFileOut = make_unique<vector<nullString>>();
                // 1. define total processor count and fill 'parallel_mode' parameter for each file of the job
                // 2. save list of output files of the whole job (output of the last macro) to mapJobName2FileOut
                int globalParallelCount = 0;
                if (job_par->nstrCommandline != nullptr) globalParallelCount = 1;
                // cycle by macros of the job
                for (size_t ind_macro = 0; ind_macro < job_par->vecMacros.size(); ind_macro++)
                {
                    int iTotalParallelCount;
                    // if the job contain a command line then no parallelization
                    if (job_par->nstrCommandline != nullptr)
                        iTotalParallelCount = 1;
                    else
                        iTotalParallelCount = 0;

                    shared_ptr<structMacroPar> cur_macro = job_par->vecMacros[ind_macro];
                    // cycle by files of the current macro
                    for (const auto& iter : cur_macro->vecFiles)
                    {
                        structFilePar* filePar = iter.get();
                        if (filePar->strParallelMode != "")
                        {
                            // if parallel mode is set to 0, than all cores are used
                            if (filePar->strParallelMode == "0")
                            {
                                // in the local mode, all CPU cores are used
                                if (job_par->iRunMode == 0)
                                    filePar->iParallelMode = get_linux_processor_count();
                                else
                                {
                                    // in the global mode, all queue slots are used
                                    if (job_par->nstrQueue == nullptr)
                                        filePar->iParallelMode = get_batch_processor_count(batch_system);
                                    else
                                        filePar->iParallelMode = get_batch_processor_count(batch_system, job_par->nstrQueue.GetValue());
                                }

                                // parallel mode is 1 by default (if incorrect values set)
                                if (filePar->iParallelMode < 1)
                                    filePar->iParallelMode = 1;
                            }
                            else
                            {
                                filePar->iParallelMode = atoi(filePar->strParallelMode.c_str());
                                // parallel mode is 1 by default (if incorrect values set)
                                if (filePar->iParallelMode < 1)
                                    filePar->iParallelMode = 1;
                            }
                        }
                        else
                            filePar->iParallelMode = 1;

                        // total processor count for all files of the macro
                        iTotalParallelCount += filePar->iParallelMode;

                        // fill list with output file paths for the whole job (the last macro)
                        if (ind_macro == (job_par->vecMacros.size() - 1))
                            vecJobFileOut->push_back(filePar->nstrFileOut);
                    }// for all files of the current macro

                    if ((ind_macro > 0) && (globalParallelCount != iTotalParallelCount))
                    {
                        info_message("ERROR nica-scheduler$ The file count in macros of the job is not the same. This job will be skipped!", 1);
                        cur_node = cur_node->next;
                        continue;
                    }
                    globalParallelCount = iTotalParallelCount;
                }// for job macros

                // deleting vecJobFileOut -> do not use later
                mapJobName2FileOut.insert(pair< string, unique_ptr<vector<nullString>> >
                                         (job_par->strJobName, std::move(vecJobFileOut)));

                // form command to source ('.' is alias) all configuration files separated by ',' or ';'
                // the result form: ". config1; . config2; "
                job_par->strConfigPath = trim(job_par->strConfigPath);
                if (job_par->strConfigPath != "")
                {
                    replace_string_in_text(job_par->strConfigPath, ",", ";");
                    if (job_par->strConfigPath[job_par->strConfigPath.length() - 1] == ';')
                        job_par->strConfigPath = job_par->strConfigPath.substr(0, job_par->strConfigPath.length() - 1);
                    replace_string_in_text(job_par->strConfigPath, ";", ";. ");
                    job_par->strConfigPath = ". " + job_par->strConfigPath + "; ";
                    // if log file is set then add redirection of the config output to the log file
                    if (job_par->nstrLogs != nullptr)
                        replace_string_in_text(job_par->strConfigPath, ";", 
                                (string)" > " + job_par->nstrLogs.GetValue() + (string)" 2>&1;");
                }

                /*****************************************/
                /* GLOBAL SCHEDULING W/WO CONTAINER MODE */
                /*****************************************/
                FILE* stream = nullptr;
                // if not local run mode
                if (job_par->iRunMode > 0)
                {
                    // if container mode, define container batch file
                    string strApptainerRunFile = "#!/bin/bash\n\n";
                    if (job_par->iRunMode == 2)
                    {
                        strApptainerRunFile += "echo \"Starting Apptainer Job\"\n";
                        if (job_par->strConfigPath != "")
                            strApptainerRunFile += job_par->strConfigPath + "\n\n";
                        strApptainerRunFile += "RUN_CMD=\"root -q -b \"\n";
                        strApptainerRunFile += "RUN_CMD+=\"${@}\"\n\n";
                        if (isDebugMode)
                            strApptainerRunFile += "echo \"DEBUG nica-scheduler: ROOT command in container = $RUN_CMD\"\n";
                        strApptainerRunFile += "eval $RUN_CMD\n";
                        strApptainerRunFile += "exit 0\n";
                    }

                    // define process count for the job
                    int proc_count = 1;
                    if (job_par->nstrProcCount != nullptr)
                    {
                        // if processor count was set to 0 then use all the available processors
                        if ((job_par->nstrProcCount.GetValue()[0] == '0') && (job_par->nstrProcCount.GetValue().length() == 1))
                        {
                            if (job_par->nstrQueue == nullptr)
                                proc_count = get_batch_processor_count(batch_system);
                            else
                                proc_count = get_batch_processor_count(batch_system, job_par->nstrQueue.GetValue());

                            if (proc_count == 0)
                            {
                                info_message("ERROR nica-scheduler$ Batch processors were not defined. This job will be skipped!", 1);
                                cur_node = cur_node->next;
                                continue;
                            }

                            cout<<"nica-scheduler$ The batch queue has "<<proc_count<<" processor cores being available for user jobs."<<endl;
                        }
                        else
                            proc_count = atoi(job_par->nstrProcCount.GetValue().c_str());
                    }
                    else
                        proc_count = globalParallelCount;

                    if (proc_count == 0)
                    {
                        info_message("ERROR nica-scheduler$ Processor count can not be equal 0. This job will be skipped!", 1);
                        cur_node = cur_node->next;
                        continue;
                    }

                    // disable dumping core in case of failures
                    if (job_par->isCoreDump)
                           strBatchFile += "ulimit -c 0\n\n";  //"set ulimit -c 0\n\n";

                    // load configuration file for environment
                    if ((job_par->iRunMode != 2) && (job_par->strConfigPath != ""))
                        strBatchFile += job_par->strConfigPath + "\n\n";

                    // define priority of the job (default: 0)
                    int iPriority = 0;
                    if (job_par->nstrPriority != nullptr)
                        iPriority = atoi(job_par->nstrPriority.GetValue().c_str());

                    // process count is assigned to input file count (taking parallel_mode in account) if greater
                    if ((globalParallelCount > 0) && (proc_count > globalParallelCount))
                        proc_count = globalParallelCount;

                    // store merge modes and partial outputs to organize merging for result files
                    vector<int> vecMergeMode;
                    vector<nullString> vecPartialOutputFiles;

                    // if there are files to process then write processing info to the special variables
                    if (globalParallelCount > 0)
                    {
                        // cycle by macros of the job
                        for (const auto& cur_macro : job_par->vecMacros)
                        {
                            // cycle by files of the current macro
                            for (const auto& cur_file : cur_macro->vecFiles)
                            {
                                // input file path for the ROOT macro
                                string strRealInputFile = cur_file->strFileIn;
                                // output file path for the ROOT macro
                                nullString nstrRealOutputFile = cur_file->nstrFileOut;

                                // in case of using a fast storage
                                // set real input file path for the ROOT macro
                                if (cur_file->strPutPath != "")
                                    strRealInputFile = cur_file->strPutPath;
                                // in case of using a fast storage
                                // set real output file path for the ROOT macro
                                if (cur_file->strGetPath != "")
                                    nstrRealOutputFile.SetValue(cur_file->strGetPath);

                                // get start event for the current file
                                int* cur_start_event = cur_macro->ptrStartEvent.get();
                                if (cur_file->ptrStartEvent != nullptr)
                                    cur_start_event = cur_file->ptrStartEvent.get();

                                // get event count for the current file
                                int* cur_event_count = cur_macro->ptrEventCount.get();
                                if (cur_file->ptrEventCount != nullptr)
                                    cur_event_count = cur_file->ptrEventCount.get();

                                // if parallel mode and start event is skipped then assign it to 0
                                if ((cur_file->iParallelMode > 1) && (cur_start_event == nullptr))
                                {
                                    cur_file->ptrStartEvent = make_unique<int>(0);
                                    cur_start_event = cur_file->ptrStartEvent.get();
                                    cout<<"nica-scheduler$ WARNING: parallel_mode is used, but start event is not set, so it is assigned to 0"<<endl;
                                }
                                // if parallel mode and event count is skipped then assign it to 0
                                if ((cur_file->iParallelMode > 1) && (cur_event_count == nullptr))
                                {
                                    cur_file->ptrEventCount = make_unique<int>(0);
                                    cur_event_count = cur_file->ptrEventCount.get();
                                    cout<<"nica-scheduler$ WARNING: parallel_mode is used, but event count is not set, so it is assigned to 0"<<endl;
                                }

                                // generate output name for output paths
                                string strOutputUnion = nstrRealOutputFile.GetValue();

                                // variables to combine parallel mode > 1 amd = 1 in one cycle
                                int start_event = -1, event_count = -1;
                                if (cur_start_event != nullptr)
                                    start_event = *cur_start_event;
                                if (cur_event_count != nullptr)
                                    event_count = *cur_event_count;

                                // if parallel mode and event count is 0
                                // then trying to get event number from the input file
                                if ((cur_file->iParallelMode > 1) && (event_count == 0))
                                {
                                    string config_paths = job_par->strConfigPath;
                                    // send output of the configuration files to /dev/null if no log file is set
                                    if (job_par->nstrLogs == nullptr)
                                        replace_string_in_text(config_paths, ";", 
                                                " > /dev/null 2>&1;");
                                    stringstream ssEventNumberCommand;
                                    ssEventNumberCommand << config_paths << "show_event_count auto \"" 
                                                         << strRealInputFile << "\" 2> /dev/null";
                                    if (isDebugMode)
                                        cout<<"DEBUG nica-scheduler$ trying to get event count from the input file: " << ssEventNumberCommand.str() <<endl;

                                    array<char, 128> bufferCommand;
                                    string str_event_number = "";
                                    stream = popen(ssEventNumberCommand.str().c_str(), "r");
                                    while (fgets(bufferCommand.data(), static_cast<int>(bufferCommand.size()), stream) != nullptr)
                                        str_event_number += bufferCommand.data();
                                    pclose(stream);
                                    str_event_number = find_first_number(str_event_number);
                                    if (str_event_number == "")
                                    {
                                        info_message(TString::Format("ERROR nica-scheduler$ parallel_mode is used, but event_count is not set and cannot be automatically defined. "
                                                     "'%s' file will be skipped!", strRealInputFile.c_str()).Data(), 1);
                                        continue;
                                    }
                                    event_count = atoi(str_event_number.c_str());
                                    if (isDebugMode)
                                        cout<<"DEBUG nica-scheduler$ "<< event_count << " event(s) found in the input file" <<endl;
                                }

                                int merge_proc_count = 0;
                                // fill parameters and parallelize the current file by events (if parallel_mode)
                                for (int j = 0; j < cur_file->iParallelMode; j++)
                                {
                                    int event_per_proc = event_count;
                                    if (cur_file->iParallelMode > 1)
                                    {
                                        event_per_proc = (event_count+j)/proc_count;
                                        if (event_per_proc == 0)
                                        {
                                            start_event = start_event + event_per_proc;
                                            continue;
                                        }
                                    }

                                    merge_proc_count++;

                                    nullString nstr_output_file = nstrRealOutputFile;

                                    // generate variable with input file array
                                    strBatchInitInputFile += "\""; strBatchInitInputFile += strRealInputFile;
                                    strBatchInitInputFile += "\" ";

                                    // generate variable with output file array
                                    if (nstr_output_file != nullptr)
                                    {
                                        // in case of parallel_mode - for particular processing by events
                                        if (cur_file->iParallelMode > 1)
                                            nstr_output_file.SetValue(GenerateOutputFilePath(nstr_output_file.GetValue(), merge_proc_count));
                                        strBatchInitOutputFile += "\""; strBatchInitOutputFile += nstr_output_file.GetValue();
                                        strBatchInitOutputFile += "\" ";
                                    }
                                    else strBatchInitOutputFile += "\">\" ";

                                    // generate variable with start event array
                                    if (cur_start_event != nullptr)
                                    {
                                        string strInt = int_to_string(start_event);
                                        strBatchInitStartEvent += "\""; strBatchInitStartEvent += strInt;
                                        strBatchInitStartEvent += "\" ";
                                    }
                                    else strBatchInitStartEvent += "\">\" ";

                                    // generate variable with event number array
                                    if (cur_event_count != nullptr)
                                    {
                                        string strInt = int_to_string(event_per_proc);
                                        strBatchInitEventCount += "\""; strBatchInitEventCount += strInt;
                                        strBatchInitEventCount += "\" ";
                                    }
                                    else strBatchInitEventCount += "\">\" ";

                                    // generate variable with additional_argiments array
                                    if (cur_macro->strAddArgs != "")
                                    {
                                        strBatchInitAddArgs += "\"";
                                        string mask_string = cur_macro->strAddArgs;
                                        replace_string_in_text(mask_string, "\"", "\\\"");
                                        strBatchInitAddArgs += mask_string;
                                        strBatchInitAddArgs += "\" ";
                                    }
                                    else strBatchInitAddArgs += "\">\" ";

                                    bool is_clean = false;      // flag if there are objects to clean
                                    // in case of using a fast storage
                                    // add commands to put files to the intermediate storage
                                    if (cur_file->strPutPath != "")
                                    {
                                        if (j == 0)
                                        {
                                            strBatchInitPut += "\""; strBatchInitPut += cur_file->strPutCommand; strBatchInitPut += " ";
                                            strBatchInitPut += cur_file->strFileIn; strBatchInitPut += " ";
                                            strBatchInitPut += cur_file->strPutPath;
                                            // create input_file_name.lock to synchronize with other tasks
                                            if (cur_file->iParallelMode > 1)
                                            {
                                                strBatchInitPut += "; touch "; 
                                                strBatchInitPut += cur_file->strPutPath + (string)".lock\" ";

                                                if (is_clean == false) strBatchInitClean += "\"";
                                                else strBatchInitClean += " ";
                                                strBatchInitClean += cur_file->strPutPath + (string)".lock\" ";
                                                is_clean = true;
                                            }
                                            strBatchInitPut += "\" ";

                                        }
                                        // wait for the first task will put input file to the fast storage 
                                        else {
                                            strBatchInitPut += "\""; strBatchInitPut += "wait "; 
                                            strBatchInitPut += cur_file->strPutPath; strBatchInitPut += ".lock\" ";
                                        }
                                    }
                                    else strBatchInitPut += "\">\" ";

                                    // generate variable to copy file from the temporary location to the output path
                                    if (cur_file->strGetPath != "")
                                    {
                                        strBatchInitGet += "\""; strBatchInitGet += cur_file->strGetCommand; strBatchInitGet += " ";
                                        if (cur_file->iParallelMode > 1)
                                        {
                                            strBatchInitGet += nstr_output_file.GetValue(); strBatchInitGet += " ";
                                            strBatchInitGet += get_directory_path(cur_file->nstrFileOut.GetValue()) + string("/") +
                                                               get_file_name_with_ext(nstr_output_file.GetValue());
                                        }
                                        else
                                        {
                                            strBatchInitGet += cur_file->strGetPath; strBatchInitGet += " ";
                                            strBatchInitGet += cur_file->nstrFileOut.GetValue();
                                        }
                                        strBatchInitGet += "\" ";
                                    }
                                    else strBatchInitGet += "\">\" ";

                                    // fill variable to clean the temporary files
                                    // from <put>, <get> (always now) and from <clean>
                                    if (cur_file->strPutPath != "")
                                    {
                                        if (is_clean == false) strBatchInitClean += "\"";
                                        else strBatchInitClean += " ";
                                        strBatchInitClean += cur_file->strPutPath;

                                        is_clean = true;
                                    }
                                    if (cur_file->strGetPath != "")
                                    {
                                        if (is_clean == false) strBatchInitClean += "\"";
                                        else strBatchInitClean += " ";
                                        strBatchInitClean += cur_file->strGetPath;

                                        is_clean = true;
                                    }
                                    for (const auto& str_clean_path : cur_file->vecCleanPath)
                                    {
                                        if (is_clean == false) strBatchInitClean += "\"";
                                        else strBatchInitClean += " ";
                                        strBatchInitClean += str_clean_path;

                                        is_clean = true;
                                    }
                                    if (is_clean)
                                        strBatchInitClean += "\" ";

                                    // form output file list (in one string) to merge if necessary
                                    if (nstr_output_file != nullptr)
                                        strOutputUnion += " " + nstr_output_file.GetValue();

                                    start_event = start_event + event_per_proc;
                                }//for (int j = 0; j < i_parallel_mode; j++)

                                if ((merge_proc_count > 1) && (cur_file->iMerge >= 0))
                                {
                                    //if (isDebugMode)
                                    //    cout<<"DEBUG nica-scheduler$ Adding "<< strOutputUnion << " to the merge list" <<endl;
                                    vecPartialOutputFiles.push_back(strOutputUnion);
                                    vecMergeMode.push_back(cur_file->iMerge);
                                }
                            }//for files of the current macro
                        }// for macros of the current job
                    }// if there are files to process then write processing info to the special variables

                    // GENERATE JOB: *.sh file for a batch system
                    if (job_par->iRunMode == 2)     // global container mode
                    {
                        strBatchFile += "export CONTAINER_PATH=";
                        auto it = mapContainerType.find(job_par->strContainerType);
                        if (it != mapContainerType.end())
                            strBatchFile += it->second;
                        else
                        {
                            info_message((TString::Format("ERROR nica-scheduler$ Undefined container type: '%s'",
                                          job_par->strContainerType.c_str())).Data(), 1);
                            exit(-1);
                        }
                        strBatchFile += "\n\n";
                    }

                    // if just a command line
                    if (job_par->nstrCommandline != nullptr)
                    {
                        strBatchFile += job_par->nstrCommandline.GetValue();
                        strBatchFile += "\n";
                    }
                    // if a job with ROOT macros
                    else
                    {
                        // add all generated variables to to the bash file
                        strBatchFile += strBatchInitInputFile; strBatchFile += ")\n\n";
                        strBatchFile += strBatchInitOutputFile; strBatchFile += ")\n\n";
                        strBatchFile += strBatchInitStartEvent; strBatchFile += ")\n\n";
                        strBatchFile += strBatchInitEventCount; strBatchFile += ")\n\n";
                        strBatchFile += strBatchInitAddArgs; strBatchFile += ")\n\n";
                        strBatchFile += strBatchInitPut, strBatchFile += ")\n\n";
                        strBatchFile += strBatchInitGet, strBatchFile += ")\n\n";
                        strBatchFile += strBatchInitClean, strBatchFile += ")\n\n";
                        strBatchFile += "\n";

                        // cycle by all macros of the job
                        for (size_t ind_macro = 0; ind_macro < job_par->vecMacros.size(); ind_macro++)
                        {
                            structMacroPar* cur_macro = job_par->vecMacros[ind_macro].get();

                            // get line number in the job script to obtain variables corresponding the task
                            int start_line_number = ind_macro * cur_macro->vecFiles.size();
                            strBatchFile += "let \"array_index = \""; strBatchFile += batch_com.batch_task_id;
                            strBatchFile += "\" - 1 + "; strBatchFile += to_string(start_line_number); strBatchFile += "\"\n\n";

                            // copy file to the temporary storage if the current element is not equal to '>'
                            // in case of parallelizing by events - other waits when the first put the input file 
                            strBatchFile += "element=${PutArray[${array_index}]}\n"
                                            "if [ \"${element}\" != \">\" ]\nthen\n"
                                            " # Check if PutElement contains \"wait <file_path>\"\n"
                                            " if [[ \"${element}\" == *\"wait \"* ]]; then\n"
                                            "   # Extract file path after \"wait \"\n"
                                            "   lock_file_path=\"${1#wait }\"\n"
                                            "   # Wait until file appears\n"
                                            "   while [ ! -f \"$lock_file_path\" ]; do\n";
                            if (isDebugMode)
                                strBatchFile += "    echo \"DEBUG nica-scheduler: wait while putting file\"\n";
                            strBatchFile += "    sleep 5\n"
                                            "   done\n"
                                            " else\n"
                                            "  set -f # avoid globbing (expansion of *)\n"
                                            "  # arr=(${element// / })\n"
                                            "  # comm=\"${arr[0]} ${arr[1]} ${arr[2]}\"\n"
                                            "  # eval $comm\n";
                            if (isDebugMode)
                                strBatchFile += "  echo \"DEBUG nica-scheduler: PUT command = ${element}\"\n";
                            strBatchFile += "  eval ${element}\n"
                                            " fi\n"
                                            "fi\n\n";

                            // run macro by ROOT
                            strBatchFile += "element=${InputFileArray[${array_index}]}\n";
                            // global run mode on a cluster
                            if (job_par->iRunMode == 1)
                            {
                                strBatchFile += "export ROOT_HIST=0\n";
                                strBatchFile += "comm=\"root -b -q '"; strBatchFile += cur_macro->nstrMacroPath.GetValue();
                                strBatchFile += "(\\\"${element}\\\"\"\n"
                                               "element=${OutputFileArray[${array_index}]}\n"
                                               "if [ \"${element}\" != \">\" ]\nthen\n"
                                               " comm=\"${comm},\\\"${element}\\\"\"\n";
                            }
                            // run mode^ 2 - global container/apptainer mode
                            else
                            {
                                strBatchFile += "comm=\"'"; strBatchFile += cur_macro->nstrMacroPath.GetValue();
                                // using triple escaped quotes to pass it to apptainer_run.sh file
                                strBatchFile += "(\\\\\\\"${element}\\\\\\\"\"\n"
                                                "element=${OutputFileArray[${array_index}]}\n"
                                                "if [ \"${element}\" != \">\" ]\nthen\n"
                                                " comm=\"${comm},\\\\\\\"${element}\\\\\\\"\"\n";
                            }
                            // add start event, event count and additional arguments to ROOT macro
                            strBatchFile += "fi\n"
                                            "element=${StartEventArray[${array_index}]}\n"
                                            "if [ \"${element}\" != \">\" ]\nthen\n"
                                            " comm=\"${comm},${element}\"\n"
                                            "fi\n"
                                            "element=${EventCountArray[${array_index}]}\n"
                                            "if [ \"${element}\" != \">\" ]\nthen\n"
                                            " comm=\"${comm},${element}\"\n"
                                            "fi\n"
                                            "element=${AddArgsArray[${array_index}]}\n"
                                            "if [ \"${element}\" != \">\" ]\nthen\n"
                                            " comm=\"${comm},${element}\"\n"
                                            "fi\n"
                                            "comm=\"$comm)'\"\n\n";
                            if (isDebugMode)
                                strBatchFile += " echo \"DEBUG nica-scheduler: Run Command = $comm\"\n";
                            if (job_par->iRunMode == 1)     // global mode, not container
                                strBatchFile += "eval $comm\n\n";

                            // copy file from the temporary location to the output path
                            strBatchFile += "element=${GetArray[${array_index}]}\n"
                                            "if [ \"${element}\" != \">\" ]\nthen\n"
                                            " set -f # avoid globbing (expansion of *)\n"
                                            "# arr=(${element// / })\n"
                                            "# comm=\"${arr[0]} ${arr[1]} ${arr[2]}\"\n"
                                            "# eval $comm\n";
                            if (isDebugMode) strBatchFile +=
                                    " echo \"DEBUG nica-scheduler: GET command = ${element}\"\n";
                            strBatchFile += " eval ${element}\n"
                                            "fi\n\n";

                            // clean the temporary files from <put>, <get> and from <clean>
                            strBatchFile += "element=${CleanArray[${array_index}]}\n"
                                            "if [ \"${element}\" != \">\" ]\nthen\n"
                                            " set -f # avoid globbing (expansion of *)\n"
                                            " arr=(${element// / })\n"
                                            " comm=\"rm -rf \"\n"
                                            " for i in \"${arr[@]}\"\n"
                                            " do\n"
                                            "  comm=\"${comm}${i} \"\n"
                                            " done\n";
                            if (isDebugMode)
                                strBatchFile += " echo \"DEBUG nica-scheduler: CLEAN command = $comm\"\n";
                            strBatchFile += " eval $comm\n"
                                            "fi\n\n";
                        }// for macros in the job
                    }

                    // if global mode, echo the current datetime
                    if (job_par->iRunMode == 1)
                        //if (isDebugMode) strBatchFile += "echo \"  Current DIR ls: `ls -alh`\"\n";
                        strBatchFile += "echo \"  End date: `date`\"\nexit 0\n";

                    // write the job a to temporary file
                    if (job_par->iRunMode == 2) // for container/apptainer mode
                    {
                        strBatchFile += batch_com.container_run_command;
                        strBatchFile += " apptainer run -B \"/eos,/cvmfs\" --fakeroot \"$CONTAINER_PATH\" ";
                        strBatchFile += exe_dir + "apptainer_run.sh " +  "\"$@\" \"$comm\" \n\n";
                        strBatchFile += "echo \"  End date: `date`\"\nexit 0\n";

                        string apptainer_run_file_path = exe_dir + (string)"apptainer_run" + (string)".sh";
                        FILE* pJobContFile = fopen(apptainer_run_file_path.c_str(), "wt");
                        if (pJobContFile == nullptr)
                        {
                            info_message(TString::Format("ERROR nica-scheduler$ Container file was not created: %s. "
                                                         "This job will be skipped!", apptainer_run_file_path.c_str()).Data(), 1);
                            cur_node = cur_node->next;
                            continue;
                        }

                        fwrite(strApptainerRunFile.c_str(), strApptainerRunFile.length(), sizeof(char), pJobContFile);
                        fclose(pJobContFile);
                        // Set executable permission
                        chmod(apptainer_run_file_path.c_str(), S_IRWXU); // 0700
                    }

                    //string input_file_path = exe_dir + (string)"input_" + buffer + (string)".txt";
                    //string output_file_path = exe_dir + (string)"output" + buffer + (string)".txt";

                    // create main shell file with all the commands to be executed (default path: 'nica-scheduler' location)
                    stringstream ssMainScriptFile;
                    ssMainScriptFile << "job_" << rand() << rand() << ".sh";
                    stringstream ssMainScriptPath;
                    ssMainScriptPath << exe_dir << ssMainScriptFile.str();
                    FILE* pJobFile = fopen(ssMainScriptPath.str().c_str(), "wt");
                    if (pJobFile == nullptr)
                    {
                        string error_path = ssMainScriptPath.str();
                        // if not created, change to the current location
                        ssMainScriptPath.str("");   // Clear the content
                        ssMainScriptPath.clear();   // Clear the state flags
                        ssMainScriptPath << ssMainScriptFile.str();

                        pJobFile = fopen(ssMainScriptPath.str().c_str(), "wt");
                        if (pJobFile == nullptr)
                        {
                            error_path += (string)" || " + ssMainScriptPath.str();
                            // if not created, change to the XML-file location
                            ssMainScriptPath.str("");   // Clear the content
                            ssMainScriptPath.clear();   // Clear the state flags
                            ssMainScriptPath << get_directory_path(xml_file_path) << ssMainScriptFile.str();
                            pJobFile = fopen(ssMainScriptPath.str().c_str(), "wt");
                            if (pJobFile == nullptr)
                            {
                                error_path += (string)" || " + ssMainScriptPath.str();
                                info_message(TString::Format("ERROR nica-scheduler$ Job file was not created at: %s. "
                                                             "This job will be skipped!", error_path.c_str()).Data(), 1);
                                cur_node = cur_node->next;
                                continue;
                            }// if (pJobFile == nullptr)
                        }// if (pJobFile == nullptr)
                    }// if (pJobFile == nullptr)
                    fwrite(strBatchFile.c_str(), strBatchFile.length(), sizeof(char), pJobFile);
                    fclose(pJobFile);

                    // output qsub file content in debug mode
                    //if (isDebugMode) cout<<"DEBUG nica-scheduler$ QSUB file content:"<<endl<<strBatchFile<<endl;

                    // form batch command to run our jobs
                    stringstream ssBatchCommand;
                    // fill batch command, job name, dependency, task count, batch slot count
                    ssBatchCommand << TString::Format(batch_com.scheduler_run_job.c_str(),
                            job_par->strJobName.c_str(), job_par->strDdependencyName.c_str(), globalParallelCount, proc_count).Data();
                    // add priority of the job if given
                    if (iPriority != 0)
                        ssBatchCommand << TString::Format(batch_com.sheduler_priority_option.c_str(), iPriority).Data();
                    // add queue name if given
                    if (job_par->nstrQueue != nullptr)
                        ssBatchCommand << TString::Format(batch_com.sheduler_queue_option.c_str(), job_par->nstrQueue.GetValue().c_str()).Data();
                    // add Quality-Of-Service if given
                    if (job_par->nstrQOS != nullptr)
                        ssBatchCommand << TString::Format(batch_com.sheduler_qos_option.c_str(), job_par->nstrQOS.GetValue().c_str()).Data();
                    // add host list if given
                    if (job_par->nstrHosts != nullptr)
                    {
                        // host list to exclude from processing
                        if (job_par->nstrHosts.GetValue()[0] == '~')
                            ssBatchCommand << TString::Format(batch_com.sheduler_exclude_hosts_option.c_str(), job_par->nstrHosts.GetValue().substr(1).c_str()).Data();
                        // host list for processing
                        else
                            ssBatchCommand << TString::Format(batch_com.sheduler_hosts_option.c_str(), job_par->nstrHosts.GetValue().c_str()).Data();
                    }
                    // add working directory if given
                    if (job_par->nstrWorkDir != nullptr)
                        ssBatchCommand << TString::Format(batch_com.sheduler_workdir_option.c_str(), job_par->nstrWorkDir.GetValue().c_str()).Data();
                    // add maximum operative memory size per one process
                    if (job_par->strOperativeMemory1 != "")
                        ssBatchCommand << TString::Format(batch_com.operative_memory1_option.c_str(), job_par->strOperativeMemory1.c_str()).Data();

                    // add script path to be executed
                    ssBatchCommand << " " << ssMainScriptPath.str();
                    if (isDebugMode)
                        // main command
                        cout<<"DEBUG nica-scheduler$ Batch command: "<<ssBatchCommand.str()<<endl;

                    array<char, 128> bufferBatch;
                    string ID = "";
                    stream = popen(ssBatchCommand.str().c_str(), "r");
                    while (fgets(bufferBatch.data(), static_cast<int>(bufferBatch.size()), stream) != nullptr)
                        ID += bufferBatch.data();
                    pclose(stream);

                    if (isDebugMode)
                        // batch command result
                        cout<<"DEBUG nica-scheduler$ Batch command output = "<<ID<<endl;

                    ID = find_first_number(ID);
                    if (ID == "")
                    {
                        info_message("ERROR nica-scheduler$ Job ID was not found: "
                                "an error occurred or there is no batch system. This job will be skipped!", 1);
                        cur_node = cur_node->next;
                        continue;
                    }

                    // current time
                    //time_t t = time(nullptr);
                    //struct tm tm = *localtime(&t);
                    //printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

                    info_message(TString::Format("nica-scheduler$ The job '%s' has been submitted with ID: %s. "
                                                 "Enter '%s' command to check status.", job_par->strJobName.c_str(), ID.c_str(),
                                                 batch_com.check_status_command.c_str()).Data());
                    mapJobName2ID.insert(pair<string,string>(job_par->strJobName, ID));

                    // add jobs to merge partial files if vecPartialOutputFiles is not empty, i.e. parallel_mode is set
                    if (!vecPartialOutputFiles.empty())
                    {
                        // cycle: one vector record contains output file list separated by space
                        // where first element of the list is the result output file
                        for (size_t i = 0; i < vecPartialOutputFiles.size(); i++)
                        {
                            // merge.sh - special batch job to merge files
                            // !!! need to rewrite - create merge.sh dynamically
                            string MERGEjob_path = exe_dir + "merge.sh";
                            stringstream ssMergeCommand;
                            ssMergeCommand << batch_com.run_command << " " << batch_com.job_dependency_option << ID
                                           << batch_com.set_job_variables << "\"MergeFiles" << vecPartialOutputFiles.at(i).GetValue()
                                           << "\",MergeMode=\"" << vecMergeMode.at(i) << "\",ConfigFiles=\""
                                           << job_par->strConfigPath << "\" " << MERGEjob_path;

                            array<char, 128> bufferMerge;
                            string localID = "";
                            stream = popen(ssMergeCommand.str().c_str(), "r");
                            while (fgets(bufferMerge.data(), static_cast<int>(bufferMerge.size()), stream) != nullptr)
                                localID += bufferMerge.data();
                            pclose(stream);

                            localID = find_first_number(localID);
                            if (localID == "")
                            {
                                info_message("ERROR nica-scheduler$ Union job ID was not found: an error occurred or there is no batch system.", 1);
                                continue;
                            }

                            cout<<"nica-scheduler$ Task for merging parallel files has ID: "<<localID<<endl;
                        }// for vecPartialOutputFiles elements
                    }// if (!vecPartialOutputFiles.empty())
                } 
                /********************/
                /* LOCAL SCHEDULING */
                /********************/
                else
                {
                    // dummy - no dependency property for local jobs because it waits every time for all child threads
                    mapJobName2ID.insert(pair<string,string>(job_par->strJobName, "dummy"));

                    // define process count for the job
                    int proc_count = 1;
                    if (job_par->nstrProcCount != nullptr)
                    {
                        if ((job_par->nstrProcCount.GetValue()[0] == '0') &&
                            (job_par->nstrProcCount.GetValue().length()) == 1)
                        {
                            proc_count = get_linux_processor_count();

                            if (proc_count == 0)
                            {
                                info_message("ERROR nica-scheduler$ Core number was not defined. This job will be skipped!", 1);
                                cur_node = cur_node->next;
                                continue;
                            }
                        }
                        else
                            proc_count = atoi(job_par->nstrProcCount.GetValue().c_str());
                    }
                    else
                        proc_count = globalParallelCount == 0 ? 1 : globalParallelCount;

                    if (proc_count == 0)
                    {
                        info_message("ERROR nica-scheduler$ Process count can not be equal 0. This job will be skipped!", 1);
                        cur_node = cur_node->next;
                        continue;
                    }

                    // process count is equal count of input files if more
                    if (proc_count > globalParallelCount)
                        proc_count = globalParallelCount;

                    vector<thread> vecThreads;
                    // semaphore and mutex to synchronize all parallel threads and sub-threads
                    shared_ptr<Semaphore> job_semaphore = make_shared<Semaphore>(proc_count);
                    shared_ptr<mutex> job_mutex = make_shared<mutex>(); // PTHREAD_MUTEX_RECURSIVE_NP

                    // console command - not ROOT macro execution
                    if (job_par->nstrCommandline != nullptr)
                    {
                        // generate bash command string
                        stringstream ssBashCommand;
                        ssBashCommand << job_par->strConfigPath << job_par->nstrCommandline.GetValue();
                        if (job_par->nstrLogs != nullptr)
                            ssBashCommand << " > " << job_par->nstrLogs.GetValue() << " 2>&1";

                        // generate output name for temporary bash file
                        stringstream ssBashFile;
                        long int t = time(nullptr);
                        ssBashFile << "temp_" << t << ".sh";
                        // create temporary bash file and write the command into it
                        ofstream bash_file;
                        bash_file.open (ssBashFile.str().c_str());
                        bash_file<<ssBashCommand.str();
                        bash_file.close();

                        cout<<"nica-scheduler$ Task 1 is running: command line - "
                            <<job_par->nstrCommandline.GetValue()<<endl;

                        // run bash command
                        stringstream ssBashRun;
                        ssBashRun << "bash " << ssBashFile.str();
                        int system_return = system(ssBashRun.str().c_str());
                        if (system_return != 0)
                            cout<<"nica-scheduler$ WARNING: System call (in main) returned non-zero code: "<<system_return<<endl;

                        //delete temporary bash file
                        if (!isDebugMode)
                        {
                            stringstream ssRemoveCommand;
                            ssRemoveCommand << "rm -rf " << ssBashFile.str();
                            system_return = system(ssRemoveCommand.str().c_str());
                            if (system_return != 0)
                                cout<<"nica-scheduler$ WARNING: System call (in ThreadLocalProcess) returned non-zero code: "<<system_return<<endl;
                        }
                        else
                            cout<<"DEBUG nica-scheduler$ '"<<ssBashFile.str()<<"' bash file was used "<<endl;
                    }
                    // parallelizing of file processing in ROOT macro on one multi-core machine
                    else
                    {
                        unsigned int thread_count = 1;
                        if (job_par->vecMacros[0]->vecFiles.size() > 1)
                            thread_count = job_par->vecMacros[0]->vecFiles.size();

                        // fill parameters for processing threads
                        for (unsigned int i = 0; i < thread_count; i++)
                        {
                            shared_ptr<structThreadPar> thread_par = make_shared<structThreadPar>();
                            thread_par->sptrJobParameter = job_par;
                            thread_par->semJobSemaphore = job_semaphore;
                            thread_par->mutJobMutex = job_mutex;
                            thread_par->iThreadCounter = i+1;

                            job_semaphore->acquire();

                            // create and run threads to local processing - function ThreadLocalProcess
                            try
                            {
                                vecThreads.emplace_back(ThreadLocalProcess, thread_par);
                            }
                            catch (const system_error& e)
                            {
                                info_message((TString::Format("ERROR nica-scheduler$ error while creating thread with code = %d and message = %s",
                                        e.code().value(), e.what())).Data(), 1);
                                exit(-1);
                            }
                        }

                        // waiting for finishing threads
                        // cout<<"Waiting for finishing threads "<<vecFiles.size()<<endl;
                        for (auto& cur_thread : vecThreads)
                        {
                            try
                            {
                                cur_thread.join();
                            }
                            catch (const system_error& e)
                            {
                                info_message((TString::Format("ERROR nica-scheduler$ Thread failed with code = %d and message = %s",
                                        e.code().value(), e.what())).Data(), 1);
                            }
                        }
                    }// else job_par->nstrCommandline != nullptr

                    cout<<"nica-scheduler$ Local job has been finished"<<endl;
                }//else local scheduling
            }// JOB tag was processed
        }// if tag means JOB

        cur_node = cur_node->next;
    }// while - cycle for all jobs

    mapJobName2ID.clear();
    mapJobName2FileOut.clear();
    xmlFreeDoc(doc);
    return 0;
}

// PARSE DATABASE PARAMETERS
void ParseDatabaseParameters(string input, TString& sql, bool isSimulation)
{
    istringstream ss(input);
    string token;

    if (isSimulation)
    {
        // variables for DB
        bool isGen = false, isEnergy = false, isMinEnergy = false, isMaxEnergy = false, isBeam = false, isTarget = false, isDesc = false, isPath = false;
        string strGen, strBeam, strTarget, strDesc, strPath;
        double fEnergy, fMaxEnergy;
        // parse tokens by comma separated
        while(getline(ss, token, ','))
        {
            // to lowercase
            transform(token.begin(), token.end(),token.begin(), ::tolower);

            // generator name parsing
            if ((token.length() > 4) && (token.substr(0,4) == "gen="))
            {
                isGen = true;
                strGen = token.substr(4);
            }
            else
            {
                // energy parsing
                if ((token.length() > 7) && (token.substr(0,7) == "energy="))
                {
                    token = token.substr(7);

                    size_t indDash = token.find_first_of('-');
                    if (indDash != string::npos)
                    {
                        stringstream stream;
                        stream << token.substr(0, indDash);
                        double dVal;
                        if (stream >> dVal)
                        {
                            isEnergy = true;
                            isMinEnergy = true;
                            fEnergy = dVal;
                        }
                        if (token.length() > indDash)
                        {
                            stringstream stream2;
                            stream2 << token.substr(indDash+1);
                            if (stream2 >> dVal)
                            {
                                isEnergy = true;
                                isMaxEnergy = true;
                                fMaxEnergy = dVal;
                            }
                        }
                    }//if (indDash > -1)
                    // if exact energy value
                    else
                    {
                        stringstream stream;
                        stream << token;
                        double dVal;
                        if (stream >> dVal)
                        {
                            isEnergy = true;
                            fEnergy = dVal;
                        }
                    }//else
                }//if ((token.length() > 7) && (token.substr(0,7) == "energy="))
                // particles' names in collision parsing
                else
                {
                    if ((token.length() > 5) && (token.substr(0,5) == "beam="))
                    {
                        isBeam = true;
                        strBeam = token.substr(5);
                    }
                    else
                    {
                        // search text in description string
                        if ((token.length() > 5) && (token.substr(0,5) == "desc="))
                        {
                            isDesc = true;
                            strDesc = token.substr(5);
                        }
                        else
                        {
                            // type of data parsing
                            if ((token.length() > 7) && (token.substr(0,7) == "target="))
                            {
                                isTarget = true;
                                strTarget = token.substr(7);
                            }
                            else
                            {
                                // path parsing
                                if ((token.length() > 5) && (token.substr(0,5) == "path="))
                                {
                                    isPath = true;
                                    strPath = token.substr(5);
                                }
                            }//else TYPE of data
                        }//else DESC
                    }//else PARTICLE
                }//else ENERGY
            }//else GEN
        }//while(getline(ss, token, ','))

        //READ PATH FROM DATABASE
        sql = "select file_path "
                "from simulation_file";

        bool isWhere = false;
        // if event generator selection
        if (isGen == true)
        {
            if (isWhere)
                sql += TString::Format(" AND lower(generator_name) = '%s'", strGen.data());
            else
            {
                isWhere = true;
                sql += TString::Format(" "
                        "where lower(generator_name) = '%s'", strGen.data());
            }
        }
        // if energy selection
        if (isEnergy == true)
        {
            if (isWhere)
                sql += " AND ";
            else
            {
                isWhere = true;
                sql += " "
                        "where ";
            }

            if (isMinEnergy)
            {
                sql += TString::Format("energy >= %f", fEnergy);
                if (isMaxEnergy)
                    sql += TString::Format(" AND energy <= %f", fMaxEnergy);
            }
            else
            {
                if (isMaxEnergy)
                    sql += TString::Format("energy <= %f", fMaxEnergy);
                else
                    sql += TString::Format("energy = %f", fEnergy);
            }
        }
        // if beam particle was selected
        if (isBeam == true)
        {
            if (isWhere)
                sql += TString::Format(" AND lower(beam_particle) = '%s'", strBeam.data());
            else
            {
                isWhere = true;
                sql += TString::Format(" "
                        "where lower(beam_particle) = '%s'", strBeam.data());
            }
        }
        // if beam particle was selected
        if (isTarget == true)
        {
            if (isWhere)
                sql += TString::Format(" AND lower(target_particle) = '%s'", strTarget.data());
            else
            {
                isWhere = true;
                sql += TString::Format(" "
                        "where lower(target_particle) = '%s'", strTarget.data());
            }
        }
        if (isDesc == true)
        {
            if (isWhere)
                sql += TString::Format(" AND lower(file_desc) like '%%%s%%'", strDesc.data());
            else
            {
                isWhere = true;
                sql += TString::Format(" "
                        "where lower(file_desc) like '%%%s%%'", strDesc.data());
            }
        }
        if (isPath == true)
        {
            if (isWhere)
                sql += TString::Format(" AND lower(file_path) like '%%%s%%'", strPath.data());
            else
            {
                isWhere = true;
                sql += TString::Format(" "
                        "where lower(file_path) like '%%%s%%'", strPath.data());
            }
        }
        sql += " order by generator_name,energy";
    }
    // else if experimental data
    else
    {
        bool isPeriod = false, isMinPeriod = false, isMaxPeriod = false, isRun = false, isMinRun = false, isMaxRun = false, isBeam = false, isTarget = false,
             isEnergy = false, isMinEnergy = false, isMaxEnergy = false, isEvents = false, isMinEvents = false, isMaxEvents = false, isTime = false, isMinTime = false, isMaxTime = false,
             isField = false, isMinField = false, isMaxField = false, isSize = false, isMinSize = false, isMaxSize = false, isPath = false;
        string strBeam, strTarget, strTime, strMaxTime, strPath;
        int iPeriod, iMaxPeriod, iRun, iMaxRun, iEvents, iMaxEvents, iField, iMaxField;
        double fEnergy, fMaxEnergy, fSize, fMaxSize;
        // parse tokens by comma separated
        while(getline(ss, token, ','))
        {
            // to lowercase
            transform(token.begin(), token.end(), token.begin(), ::tolower);

            // period number parsing
            if ((token.length() > 7) && (token.substr(0,7) == "period="))
            {
                token = token.substr(7);

                size_t indDash = token.find_first_of('-');
                if (indDash != string::npos)
                {
                    stringstream stream;
                    stream << token.substr(0, indDash);
                    int iVal;
                    if (stream >> iVal)
                    {
                        isPeriod = true;
                        isMinPeriod = true;
                        iPeriod = iVal;
                    }
                    if (token.length() > indDash)
                    {
                        stringstream stream2;
                        stream2 << token.substr(indDash+1);
                        if (stream2 >> iVal)
                        {
                            isPeriod = true;
                            isMaxPeriod = true;
                            iMaxPeriod = iVal;
                        }
                    }
                }//if (indDash > -1)
                // if exact period number
                else
                {
                    stringstream stream;
                    stream << token;
                    int iVal;
                    if (stream >> iVal)
                    {
                        isPeriod = true;
                        iPeriod = iVal;
                    }
                }//else
            }//if ((token.length() > 7) && (token.substr(0,7) == "period="))
            else
            {
                // run number parsing
                if ((token.length() > 4) && (token.substr(0,4) == "run="))
                {
                    token = token.substr(4);

                    size_t indDash = token.find_first_of('-');
                    if (indDash != string::npos)
                    {
                        stringstream stream;
                        stream << token.substr(0, indDash);
                        int iVal;
                        if (stream >> iVal)
                        {
                            isRun = true;
                            isMinRun = true;
                            iRun = iVal;
                        }
                        if (token.length() > indDash)
                        {
                            stringstream stream2;
                            stream2 << token.substr(indDash+1);
                            if (stream2 >> iVal)
                            {
                                isRun = true;
                                isMaxRun = true;
                                iMaxRun = iVal;
                            }
                        }
                    }   // if (indDash > -1)
                    //  if exact run number
                    else
                    {
                        stringstream stream;
                        stream << token;
                        int iVal;
                        if (stream >> iVal) {
                            isRun = true;
                            iRun = iVal;
                        }
                    }   // else
                }//if ((token.length() > 4) && (token.substr(0,4) == "run="))
                else
                {
                    // beam particle's name in collision parsing
                    if ((token.length() > 5) && (token.substr(0,5) == "beam="))
                    {
                        isBeam = true;
                        strBeam = token.substr(5);
                    }
                    else
                    {
                        // target particle's name in collision parsing
                        if ((token.length() > 7) && (token.substr(0,7) == "target="))
                        {
                            isTarget = true;
                            strTarget = token.substr(7);
                        }
                        else
                        {
                            // energy parsing
                            if ((token.length() > 7) && (token.substr(0,7) == "energy="))
                            {
                                token = token.substr(7);

                                size_t indDash = token.find_first_of('-');
                                if (indDash != string::npos)
                                {
                                    stringstream stream;
                                    stream << token.substr(0, indDash);
                                    double dVal;
                                    if (stream >> dVal)
                                    {
                                        isEnergy = true;
                                        isMinEnergy = true;
                                        fEnergy = dVal;
                                    }
                                    if (token.length() > indDash)
                                    {
                                        stringstream stream2;
                                        stream2 << token.substr(indDash+1);
                                        if (stream2 >> dVal)
                                        {
                                            isEnergy = true;
                                            isMaxEnergy = true;
                                            fMaxEnergy = dVal;
                                        }
                                    }
                                }   // if (indDash > -1)
                                //  if exact energy value
                                else
                                {
                                    stringstream stream;
                                    stream << token;
                                    double dVal;
                                    if (stream >> dVal) {
                                        isEnergy = true;
                                        fEnergy = dVal;
                                    }
                                }   // else
                            }//if ((token.length() > 7) && (token.substr(0,7) == "energy="))
                            else
                            {
                                // event count parsing
                                if ((token.length() > 6) && (token.substr(0,6) == "events="))
                                {
                                    token = token.substr(6);

                                    size_t indDash = token.find_first_of('-');
                                    if (indDash != string::npos)
                                    {
                                        stringstream stream;
                                        stream << token.substr(0, indDash);
                                        int iVal;
                                        if (stream >> iVal)
                                        {
                                            isEvents = true;
                                            isMinEvents = true;
                                            iEvents = iVal;
                                        }
                                        if (token.length() > indDash)
                                        {
                                            stringstream stream2;
                                            stream2 << token.substr(indDash+1);
                                            if (stream2 >> iVal)
                                            {
                                                isEvents = true;
                                                isMaxEvents = true;
                                                iMaxEvents = iVal;
                                            }
                                        }
                                    }   // if (indDash > -1)
                                    //  if exact event count
                                    else
                                    {
                                        stringstream stream;
                                        stream << token;
                                        int iVal;
                                        if (stream >> iVal) {
                                            isEvents = true;
                                            iEvents = iVal;
                                        }
                                    }   // else
                                }//if ((token.length() > 6) && (token.substr(0,6) == "events="))
                                else
                                {
                                    // time parsing
                                    if ((token.length() > 5) && (token.substr(0,5) == "time="))
                                    {
                                        token = token.substr(5);

                                        size_t indDash = token.find_first_of('-');
                                        if (indDash != string::npos)
                                        {
                                            if (indDash > 0)
                                            {
                                                isTime = true;
                                                isMinTime = true;
                                                strTime = token.substr(0, indDash);
                                            }
                                            if (token.length() > indDash)
                                            {
                                                isTime = true;
                                                isMaxTime = true;
                                                strMaxTime = token.substr(indDash+1);
                                            }
                                        } // if (indDash > -1)
                                        // if exact time
                                        else
                                        {
                                            isTime = true;
                                            strTime = token;
                                        }   // else
                                    }//if ((token.length() > 5) && (token.substr(0,5) == "time="))
                                    else
                                    {
                                        // field current parsing
                                        if ((token.length() > 6) && (token.substr(0,6) == "field="))
                                        {
                                            token = token.substr(6);

                                            size_t indDash = token.find_first_of('-');
                                            if (indDash != string::npos)
                                            {
                                                stringstream stream;
                                                stream << token.substr(0, indDash);
                                                int iVal;
                                                if (stream >> iVal)
                                                {
                                                    isField = true;
                                                    isMinField = true;
                                                    iField = iVal;
                                                }
                                                if (token.length() > indDash)
                                                {
                                                    stringstream stream2;
                                                    stream2 << token.substr(indDash+1);
                                                    if (stream2 >> iVal)
                                                    {
                                                        isField = true;
                                                        isMaxField = true;
                                                        iMaxField = iVal;
                                                    }
                                                }
                                            }   // if (indDash > -1)
                                            //  if exact field current
                                            else
                                            {
                                                stringstream stream;
                                                stream << token;
                                                int iVal;
                                                if (stream >> iVal) {
                                                    isField = true;
                                                    iField = iVal;
                                                }
                                            }   // else
                                        }//if ((token.length() > 6) && (token.substr(0,6) == "field="))
                                        else
                                        {
                                            // file size parsing
                                            if ((token.length() > 5) && (token.substr(0,5) == "size="))
                                            {
                                                token = token.substr(5);

                                                size_t indDash = token.find_first_of('-');
                                                if (indDash != string::npos)
                                                {
                                                    stringstream stream;
                                                    stream << token.substr(0, indDash);
                                                    double dVal;
                                                    if (stream >> dVal)
                                                    {
                                                        isSize = true;
                                                        isMinSize = true;
                                                        fSize = dVal;
                                                    }
                                                    if (token.length() > indDash)
                                                    {
                                                        stringstream stream2;
                                                        stream2 << token.substr(indDash+1);
                                                        if (stream2 >> dVal)
                                                        {
                                                            isSize = true;
                                                            isMaxSize = true;
                                                            fMaxSize = dVal;
                                                        }
                                                    }
                                                }   // if (indDash > -1)
                                                //  if exact file size
                                                else
                                                {
                                                    stringstream stream;
                                                    stream << token;
                                                    double dVal;
                                                    if (stream >> dVal) {
                                                        isSize = true;
                                                        fSize = dVal;
                                                    }
                                                }   // else
                                            }//if ((token.length() > 5) && (token.substr(0,5) == "size="))
                                            else
                                            {
                                                // path parsing
                                                if ((token.length() > 5) && (token.substr(0,5) == "path="))
                                                {
                                                    isPath = true;
                                                    strPath = token.substr(5);
                                                }
                                            }//else SIZE
                                        }//else FIELD
                                    }//else TIME
                                }//else EVENTS
                            }//else ENERGY
                        }//else TARGER
                    }//else BEAM
                }//else RUN
            }//else PERIOD
        }//while(getline(ss, token, ','))

        // generate query
        sql = "select file_path "
                "from run_";

        bool isWhere = false;
        // if period selection
        if (isPeriod == true)
        {
            if (isWhere)
                sql += " AND ";
            else
            {
                isWhere = true;
                sql += " "
                        "where ";
            }

            if (isMinPeriod)
            {
                sql += TString::Format("period_number >= %d", iPeriod);
                if (isMaxPeriod)
                    sql += TString::Format(" AND period_number <= %d", iMaxPeriod);
            }
            else
            {
                if (isMaxPeriod)
                    sql += TString::Format("period_number <= %d", iMaxPeriod);
                else
                    sql += TString::Format("period_number = %d", iPeriod);
            }
        }
        // if run selection
        if (isRun == true)
        {
            if (isWhere)
                sql += " AND ";
            else
            {
                isWhere = true;
                sql += " "
                        "where ";
            }

            if (isMinRun)
            {
                sql += TString::Format("run_number >= %d", iRun);
                if (isMaxRun)
                    sql += TString::Format(" AND run_number <= %d", iMaxRun);
            }
            else
            {
                if (isMaxRun)
                    sql += TString::Format("run_number <= %d", iMaxRun);
                else
                    sql += TString::Format("run_number = %d", iRun);
            }
        }
        // if beam particle was selected
        if (isBeam == true)
        {
            if (isWhere)
                sql += TString::Format(" AND lower(beam_particle) = '%s'", strBeam.data());
            else
            {
                isWhere = true;
                sql += TString::Format(" "
                        "where lower(beam_particle) = '%s'", strBeam.data());
            }
        }
        // if target particle was selected
        if (isTarget == true)
        {
            if (isWhere)
                sql += TString::Format(" AND lower(target_particle) = '%s'", strTarget.data());
            else
            {
                isWhere = true;
                sql += TString::Format(" "
                        "where lower(target_particle) = '%s'", strTarget.data());
            }
        }
        // if energy selection
        if (isEnergy == true)
        {
            if (isWhere)
                sql += " AND ";
            else
            {
                isWhere = true;
                sql += " "
                        "where ";
            }

            if (isMinEnergy)
            {
                sql += TString::Format("energy >= %f", fEnergy);
                if (isMaxEnergy)
                    sql += TString::Format(" AND energy <= %f", fMaxEnergy);
            }
            else
            {
                if (isMaxEnergy)
                    sql += TString::Format("energy <= %f", fMaxEnergy);
                else
                    sql += TString::Format("energy = %f", fEnergy);
            }
        }
        // if event count selection
        if (isEvents == true)
        {
            if (isWhere)
                sql += " AND ";
            else
            {
                isWhere = true;
                sql += " "
                        "where ";
            }

            if (isMinEvents)
            {
                sql += TString::Format("event_count >= %d", iEvents);
                if (isMaxEvents)
                    sql += TString::Format(" AND event_count <= %d", iMaxEvents);
            }
            else
            {
                if (isMaxEvents)
                    sql += TString::Format("event_count <= %d", iMaxEvents);
                else
                    sql += TString::Format("event_count = %d", iEvents);
            }
        }
        // if time selection
        if (isTime == true)
        {
            if (isWhere)
                sql += " AND ";
            else
            {
                isWhere = true;
                sql += " "
                        "where ";
            }

            // check correct format
            struct tm tm;
            if (!strptime(strMaxTime.c_str(), "%Y-%m-%d %H:%M:%S", &tm))
            {
                isMaxTime = false;
                cout<<"Error: "<<strMaxTime<<" is not correct datetime. DateTime format should be yyyy-mm-dd 24hh:mm:ss."<<endl;
            }
            if (!strptime(strTime.c_str(), "%Y-%m-%d %H:%M:%S", &tm))
            {
                isMinTime = false;
                cout<<"Error: "<<strTime<<" is not correct datetime. DateTime format should be yyyy-mm-dd 24hh:mm:ss."<<endl;
                if (!isMaxTime) isTime = false;
            }

            if (isMinTime)
            {
                sql += TString::Format("end_datetime >= '%s'", strTime.c_str());
                if (isMaxTime)
                    sql += TString::Format(" AND start_datetime <= '%s'", strMaxTime.c_str());
            }
            else
            {
                if (isMaxTime)
                    sql += TString::Format("start_datetime <= '%s'", strMaxTime.c_str());
                else
                {
                    if (isTime)
                        sql += TString::Format("start_datetime <= '%s' AND end_datetime >= '%s'", strTime.c_str(), strTime.c_str());
                }
            }
        }
        // if field current selection
        if (isField == true)
        {
            if (isWhere)
                sql += " AND ";
            else
            {
                isWhere = true;
                sql += " "
                        "where ";
            }

            if (isMinField)
            {
                sql += TString::Format("field_voltage >= %d", iField);
                if (isMaxField)
                    sql += TString::Format(" AND field_voltage <= %d", iMaxField);
            }
            else
            {
                if (isMaxField)
                    sql += TString::Format("field_voltage <= %d", iMaxField);
                else
                    sql += TString::Format("field_voltage = %d", iField);
            }
        }
        // if file size selection
        if (isSize == true)
        {
            if (isWhere)
                sql += " AND ";
            else
            {
                isWhere = true;
                sql += " "
                        "where ";
            }

            if (isMinSize)
            {
                sql += TString::Format("file_size >= %f", fSize);
                if (isMaxSize)
                    sql += TString::Format(" AND file_size <= %f", fMaxSize);
            }
            else
            {
                if (isMaxSize)
                    sql += TString::Format("file_size <= %f", fMaxSize);
                else
                    sql += TString::Format("file_size = %f", fSize);
            }
        }
        if (isPath == true)
        {
            if (isWhere)
                sql += TString::Format(" AND lower(file_path) like '%%%s%%'", strPath.data());
            else
            {
                isWhere = true;
                sql += TString::Format(" "
                        "where lower(file_path) like '%%%s%%'", strPath.data());
            }
        }

        sql += " order by period_number,run_number";
    }

    return;
}

// function to parse file list and parameters for the current macro of the job
bool ParseMacroFiles(xmlNodePtr sub_node, shared_ptr<structJobPar> job_par, shared_ptr<structMacroPar> macro_par,
                     multimap< string, unique_ptr<vector<nullString>> >& mapJobName2FileOut, batch_commands batch_com)
{
    char* pc_temp = nullptr;
    while (sub_node)
    {
        // FILE TAG
        if (strcmp((char*)sub_node->name, "file") == 0)
        {
            bool isStartEvent = false, isEventCount = false;
            int start_event, event_count;
            string sim_db = "", exp_db = "", job_input = "", macro_input = "", file_input = "",
                   put_command = "cp", put_path = "", get_command = "cp", get_path = "", get_output = "", clean_path = "",
                   parallel_mode = "";
            nullString nstr_file_in, nstr_file_out;

            // read Input File Path(s) to process by the macro in the XML description
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"input");
            if (pc_temp != nullptr)
            {
                nstr_file_in.SetValue((string)pc_temp);
                free(pc_temp);
            }
            // read Output File Path(s) to save processing results by the macro
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"output");
            if (pc_temp != nullptr)
            {
                nstr_file_out.SetValue((string)pc_temp);
                free(pc_temp);
            }

            // read Start Event value in the XML description
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"start_event");
            if (pc_temp != nullptr)
            {
                start_event = atoi(pc_temp);
                free(pc_temp);
                isStartEvent = true;
            }

            // read Event Count value in the XML description
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"event_count");
            if (pc_temp != nullptr)
            {
                if (!is_string_number(pc_temp))
                {
                    info_message("ERROR nica-scheduler$ Event count must be a number!", 1);
                    free(pc_temp);
                    return true;
                }
                event_count = atoi(pc_temp);
                free(pc_temp);
                if (event_count < 0)
                {
                    info_message("ERROR nica-scheduler$ Event count must be a positive number or 0 (for all events)!", 1);
                    return true;
                }
                isEventCount = true;
            }

            // read Merge Flag: whether merge files containing partial results
            // possible values: false = -1 (default), merge to the origin file name and delete partial files = 1
            // chain = 0 - store an additional TChain file containing links to all the partial files,
            // preserve = 2 - as' true' value, but not delete partial result files
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"merge");
            int iMerge = -1;	// false - not to merge
            if (pc_temp != nullptr)
            {
                char* lower_merge = convert_pchar_to_lowercase_new(pc_temp);
                if (strcmp(lower_merge, "true") == 0)
                    iMerge = 1;
                if (strcmp(lower_merge, "chain") == 0)
                    iMerge = 0;
                if (strcmp(lower_merge, "preserve") == 0)
                    iMerge = 2;

                free(pc_temp);
                delete lower_merge;
            }
            // read Parallel Mode value: whether parallelize processing of the file by events
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"parallel_mode");
            if (pc_temp != nullptr)
            {
                parallel_mode = pc_temp;
                free(pc_temp);
            }

            // form Input File List from the Simulation Catalogue of the UniConDa database
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"sim_input");
            if (pc_temp != nullptr)
            {
                sim_db = pc_temp;
                free(pc_temp);
            }
            // form Input File List from the Experimental File Catalogue of the UniConDa database
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"exp_input");
            if (pc_temp != nullptr)
            {
                exp_db = pc_temp;
                free(pc_temp);
            }
            // get Input File List from a previously executed job with the given name
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"job_input");
            if (pc_temp != nullptr)
            {
                job_input = pc_temp;
                free(pc_temp);
            }
            // get Input File List from a previously executed macro with the given name
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"macro_input");
            if (pc_temp != nullptr)
            {
                macro_input = pc_temp;
                free(pc_temp);
            }
            // form Input File List from the text file containing input file paths by lines
            pc_temp = (char*) xmlGetProp(sub_node, (unsigned char*)"file_input");
            if (pc_temp != nullptr)
            {
                file_input = pc_temp;
                free(pc_temp);
            }

            // parse information containing preparation of the files to be processed
            // operations: '<put>', '<get>', '<clean>'
            xmlNodePtr sub_file_node = sub_node->children;
            while (sub_file_node)
            {
                // if TAG is XML_ELEMENT_NODE then continue
                if (sub_file_node->type == XML_ELEMENT_NODE)
                {
                    // if TAG means <put>: put the files to the special place before the processing
                    if (strcmp((char*)sub_file_node->name, "put") == 0)
                    {
                        // read Copy Command to put the files on the intermediate storage
                        pc_temp = (char*) xmlGetProp(sub_file_node, (unsigned char*)"command");
                        if (pc_temp != nullptr)
                        {
                            put_command = pc_temp;
                            free(pc_temp);
                        }
                        // read Directory Path, to which put the files on the intermediate storage
                        pc_temp = (char*) xmlGetProp(sub_file_node, (unsigned char*)"directory");
                        if (pc_temp == nullptr)
                        {
                            info_message("ERROR nica-scheduler$ There is no 'directory' property for the <put/> tag!", 1);
                            sub_file_node = sub_file_node->next;
                            continue;
                        }
                        put_path = pc_temp;
                        free(pc_temp);
                    }// if <put/> attribute

                    // if tag means <get>: get the files from the temporary place after the processing
                    if (strcmp((char*)sub_file_node->name, "get") == 0)
                    {
                        // read Copy Command to get result files from the intermediate storage
                        pc_temp = (char*) xmlGetProp(sub_file_node, (unsigned char*)"command");
                        if (pc_temp != nullptr)
                        {
                            get_command = pc_temp;
                            free(pc_temp);
                        }
                        // read Directory Path, to which save the intermediate results and from where copy to the destination storage
                        pc_temp = (char*) xmlGetProp(sub_file_node, (unsigned char*)"directory");
                        if (pc_temp == nullptr)
                        {
                            info_message("ERROR nica-scheduler$ There is no 'directory' property for the <get/> tag!", 1);
                            sub_file_node = sub_file_node->next;
                            continue;
                        }
                        get_path = pc_temp;
                        free(pc_temp);
                    }// if <get/> attribute

                    // if tag means <clean>: clean temporary files after the processing
                    if (strcmp((char*)sub_file_node->name, "clean") == 0)
                    {
                        pc_temp = (char*) xmlGetProp(sub_file_node, (unsigned char*)"path");
                        if (pc_temp != nullptr)
                        {
                            clean_path = pc_temp;
                            free(pc_temp);
                        }
                    }// if <clean> attribute
                }// if sub_file_node->type == XML_ELEMENT_NODE

                sub_file_node = sub_file_node->next;
            }// if file should be pre/post copied

            vector<string> vecInputFiles;
            // PATH TO <INPUT> FILE (one or with a regular expression)
            if (nstr_file_in != nullptr)
            {
                string str_file_in = nstr_file_in.GetValue();
                vecInputFiles.push_back(str_file_in);

                // check whether file contains wildcards (?,*)
                if ((str_file_in.find('?') != std::string::npos) || (str_file_in.find('*') != std::string::npos) || (str_file_in.find('+') != std::string::npos))
                {
                    vecInputFiles = glob(str_file_in.c_str());
                    if (isDebugMode)
                        cout<<"DEBUG nica-scheduler$ File count in the regular expression: "<<vecInputFiles.size()<<endl;
                }
            }// if there is <input> file

            // LIST OF INPUT FILES FROM THE UNIFIED CONDITION DATABASE
            if ((sim_db != "") || (exp_db != ""))
            {
                TString sql, strConnection = "pgsql://" + (TString)job_par->dbSettings.db_host + "/" + (TString)job_par->dbSettings.db_name;
                if (sim_db == "")
                    ParseDatabaseParameters(exp_db, sql, false);
                else
                    ParseDatabaseParameters(sim_db, sql, true);
                TSQLServer* pSQLServer = TSQLServer::Connect(strConnection, job_par->dbSettings.db_username.c_str(),
                                                             job_par->dbSettings.db_password.c_str());
                if (pSQLServer == 0x00)
                {
                    info_message("ERROR nica-scheduler$ Connection to the database was not established!", 1);
                    return true;
                }

                if (isDebugMode)
                    cout<<"DEBUG nica-scheduler$ SQL query: "<<sql<<endl;

                TSQLResult* res = pSQLServer->Query(sql);
                if (res == 0x00)
                {
                    info_message("ERROR nica-scheduler$ An error was occurred during the SQL query!", 1);
                    return true;
                }
                int nrows = res->GetRowCount();
                if (nrows == 0)
                    info_message("nica-scheduler$ WARNING: there are no records for these parameters", 1);
                else
                {
                    TSQLRow* row;
                    while ((row = res->Next()) != nullptr)
                    {
                        string cur_file_path((char*)row->GetField(0));
                        #ifdef MICC_NO_MANAGEMENT
                            if ((simDB != "") && (batch_com.run_command == "sbatch")) replace_string_in_text(cur_file_path, "eos", "eos/eos.jinr.ru");
                        #endif
                        vecInputFiles.push_back(cur_file_path);
                        delete row;
                     }//while (row = res->Next())
                }//else nrows > 0
                delete res;
                if (pSQLServer)
                    delete pSQLServer;
            }// if ((sim_db != nullptr) || (exp_db != nullptr))

            // LIST OF INPUT FILES FROM A PREVIOUS JOB WITH THE GIVEN NAME
            if (job_input != "")
            {
                // search for output list in the map
                auto it = mapJobName2FileOut.find(job_input);
                if (it == mapJobName2FileOut.end())
                    cout<<"nica-scheduler$ WARNING: job (output list) was not found: "<<job_input<<endl;
                else
                {
                    vector<nullString>* vec_string = it->second.get();
                    for (vector<nullString>::iterator iter_output = vec_string->begin(); iter_output != vec_string->end(); iter_output++)
                        vecInputFiles.push_back((*iter_output).GetValue());
                }
            }// if (job_input != nullptr)

            // LIST OF INPUT FILES FROM A PREVIOUS MACRO WITH THE GIVEN NAME
            if (macro_input != "")
            {
                // search for the macro with a given name
                bool is_found = false;
                for (const auto& iter : job_par->vecMacros)
                {
                    structMacroPar* prev_macro = iter.get();
                    if (strcmp(prev_macro->nstrMacroName.GetValue().c_str(), macro_input.c_str()) == 0)
                    {
                        for (const auto& iter_output : prev_macro->vecFiles)
                        {
                            if (iter_output.get()->nstrFileOut.isNull()) continue;
                            vecInputFiles.push_back(iter_output.get()->nstrFileOut.GetValue());
                        }// for

                        is_found = true;
                        break;
                    }//if (strcmp(prev_macro->macro_name, macro_input) == 0)
                }// search for the macro with a given name

                if (!is_found)
                    cout<<"nica-scheduler$ WARNING: macro (output list) was not found: "<<macro_input<<endl;
            }// if (macro_input != nullptr)

            // LIST OF INPUT FILES (separated by newline symbols) FROM A TEXT FILE WITH THE GIVEN PATH
            if (file_input != "")
            {
                // open text file with input list
                string str_file_input = expand_path(file_input);
                ifstream listFile(str_file_input.c_str());
                if (listFile)
                {
                    string list_line;
                    while (getline(listFile, list_line))
                        vecInputFiles.push_back(list_line);
                        //cout<<"Current file: "<<list_line<<endl;
                }
                else
                    info_message("ERROR nica-scheduler$ Can not open text file with file input list!", 1);
            }////if (file_input != "")

            // FORM INFORMATION ON INPUT, OUTPUT and INTERMEDIATE FILES of the macro
            bool isFileFound = false;
            for (unsigned int i = 0; i < vecInputFiles.size(); i++)
            {
                unique_ptr<structFilePar> filePar = make_unique<structFilePar>();
                // add input file
                filePar->strFileIn = trim(vecInputFiles[i]);
                // add output file
                if (nstr_file_out != nullptr)
                    filePar->nstrFileOut.SetValue(form_file_name(nstr_file_out.GetValue(), filePar->strFileIn,
                                                                 i+1, batch_com.batch_temp_directory));
                // add start event number
                if (isStartEvent)
                    filePar->ptrStartEvent = make_unique<int>(start_event);
                // add event count
                if (isEventCount)
                    filePar->ptrEventCount = make_unique<int>(event_count);
                // set parallel mode and the merge option
                if (parallel_mode != "")
                    filePar->strParallelMode = parallel_mode;

                filePar->iMerge = iMerge;
                // form put path
                if (put_path != "")
                {
                    filePar->strPutCommand = put_command;
                    filePar->strPutPath = form_file_name(put_path, filePar->strFileIn, i+1, batch_com.batch_temp_directory);
                    filePar->strPutPath = filePar->strPutPath + string("/") + get_file_name_with_ext(filePar->strFileIn);
                }
                // form get path
                if (get_path != "")
                {
                    filePar->strGetCommand = get_command;
                    filePar->strGetPath = form_file_name(get_path, filePar->strFileIn, i+1, batch_com.batch_temp_directory);
                    if (filePar->nstrFileOut == nullptr)
                    {
                        info_message("ERROR nica-scheduler$ <get> must employ output file path, but it is not set!", 1);
                        return true;
                    }
                    filePar->strGetPath = filePar->strGetPath + string("/") +
                                          get_file_name_with_ext(filePar->nstrFileOut.GetValue());
                }
                // form clean path
                if (clean_path != "")
                {
                    vector<string> vec_clean_path = string_to_vector(clean_path);
                    for (const string& cur_clean_path : vec_clean_path)
                        filePar->vecCleanPath.push_back(form_file_name(cur_clean_path, filePar->strFileIn, i+1, batch_com.batch_temp_directory));
                }

                macro_par->vecFiles.push_back(std::move(filePar));
                isFileFound = true;
            }

            if (!isFileFound)
            {
                info_message("ERROR nica-scheduler$ No input files were found (possible tags: "
                        "<input>, <file_input>, <sim_input>, <exp_input>, <job_input>, <macro_input>!", 1);
                return true;
            }
        }// FILE TAG

        sub_node = sub_node->next;
    }//while (sub_node)

    return false;
}
