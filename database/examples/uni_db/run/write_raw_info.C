R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/run_raw_bmn.C"

int append_log(TString log_name, TString message);

// Macro to extract information while converting raw files
// and writing the raw file metadata to the Unified Condition Database
// uses run_raw_bmn.C macro
//
// Parameters:
// input_raw_file: full path to the raw file
// period - period number, default: 9
// isRewriteMeta - rewrite raw file metadata in the Condition Database if exists. default: false
// error_log_name: full path to the log file with errors, default: "" - no log will be created
//
// example:
// root 'write_raw_info.C("/ceph/bmn/run/run9/beam/run_Top_9086_ev1_p0.data", "", 9, false, "error.log")'
void write_raw_info(TString input_raw_file,
                    UInt_t period = 9,
                    Bool_t isRewriteMeta = false,
                    TString error_log_file = "")
{
    // expand paths of input and output files to substitute env variables
    gSystem->ExpandPathName(input_raw_file);
    bool isLog = false;
    if (error_log_file != "") {
        isLog = true;
        gSystem->ExpandPathName(error_log_file);
    }

    // check input raw file exist and has no zero size
    if (gSystem->AccessPathName(input_raw_file.Data()) == true) {
        TString message = TString::Format("ERROR: no input file was found: %s", input_raw_file.Data());
        cout << endl << message << endl;
        if (isLog)
            append_log(error_log_file, message);
        return;
    }
    Long_t raw_id, raw_flags, raw_modtime;
    Long64_t raw_file_size = -1;
    gSystem->GetPathInfo(input_raw_file.Data(), &raw_id, &raw_file_size, &raw_flags, &raw_modtime);
    if (raw_file_size <= 0) {
        TString message = TString::Format("ERROR: input file has zero size: %s", input_raw_file.Data());
        cout << endl << message << endl;
        if (isLog)
            append_log(error_log_file, message);
        return;
    }

    // calculate raw root file name
    /*Int_t lastSlash = output_digi_file.Last('/');
    TString digiPath = (lastSlash == TString::kNPOS) ? "" : TString(output_digi_file(0, lastSlash + 1));
    TString subName = input_raw_file;
    TPRegexp re_data(".+_(\\d+)([^/]*)\\.data");
    if (re_data.MatchB(subName))
        re_data.Substitute(subName, "$2");
    else
        subName = "";
    TString raw_root_file = Form("%sbmn_run%d%s_raw.root", digiPath.Data(), run_number, subName.Data());*/

    // 2: RAW -> RAW_ROOT converting
    run_raw_bmn(input_raw_file, "", 0, period, 2, kTRUE);

    // if need to rewrite raw file metadata (created while converting RAW -> RAW_ROOT)
    if (isRewriteMeta) {
        // get name of the created RAW ROOT file
        TString raw_root_file = gRawRootFilePath;
        if (gRawRootFilePath == "") {
            TString message =
                TString::Format("ERROR: decoder failed while creating a RAW ROOT file for: %s", input_raw_file.Data());
            cout << endl << message << endl;
            if (isLog)
                append_log(error_log_file, message);
            return;
        }

        // check raw root file exist and has no zero size
        Long_t root_id, root_flags, root_modtime;
        Long64_t root_file_size = -1;
        gSystem->GetPathInfo(raw_root_file.Data(), &root_id, &root_file_size, &root_flags, &root_modtime);
        if (root_file_size <= 0) {
            TString message = TString::Format("ERROR: output file has zero size: %s", raw_root_file.Data());
            cout << endl << message << endl;
            if (isLog)
                append_log(error_log_file, message);
            return;
        }

        // open RAW ROOT file, then print and write to the Unified Condition Database
        TChain* tree = new TChain("BMN_RAW");
        tree->Add(raw_root_file);

        // link event header to get information on the raw file
        BmnEventHeader* fEventHeader = NULL;
        tree->SetBranchAddress("BmnEventHeader.", &fEventHeader);

        // get event count (== tree records) from the RAW ROOT file
        Int_t event_count = tree->GetEntries();
        if (event_count < 1) {
            cout << endl << "ERROR: File '" << raw_root_file << "' has no events" << endl;
            return;
        } else {
            // get start time and start event from the RAW ROOT file
            tree->GetEntry(0);
            // get period and run number from the event header
            Int_t period_number = fEventHeader->GetPeriodId();
            Int_t run_number = fEventHeader->GetRunId();
            // get start and end time from the event header
            TTimeStamp startTime = fEventHeader->GetEventTimeTS();
            UInt_t start_event = fEventHeader->GetEventId();

            // get end time and end event from the RAW ROOT file
            tree->GetEntry(event_count - 1);
            TTimeStamp endTime = fEventHeader->GetEventTimeTS();
            UInt_t end_event = fEventHeader->GetEventId();

            cout << "Event number interval: " << start_event << " - " << end_event << endl;
            cout << "Event time interval: " << startTime.AsString() << " - " << endTime.AsString() << endl;
            cout << "Event count: " << event_count << endl;

            // searching for the raw file with the period_number, run_number, start_event and end_event
            TObjArray* arrRawFiles =
                UniRawFile::GetRawFiles(int period_number, int run_number, int start_event, int end_event);
            if (arrRawFiles == nullptr) {
                cout << endl << "ERROR: no raw files found to rewrite the file metadata" << endl;
                return;
            }
            if (arrRawFiles->GetEntries() > 1) {
                cout << endl << "ERROR: more than one raw files found to rewrite the file metadata" << endl;
                delete arrRawFiles;
                return;
            }

            UniRawFile* pRawFile = (UniRawFile*)arrRawFiles->At(0);

            bool isErrors = false;
            // setting new metadata for the input raw file
            TDatime startDate(Int_t(startTime.GetDate(kFALSE)), Int_t(startTime.GetTime(kFALSE)));
            TDatime endDate(Int_t(endTime.GetDate(kFALSE)), Int_t(endTime.GetTime(kFALSE)));
            if (pRawFile->SetFilePath(input_raw_file) != 0)
                isErrors = true;
            if (pRawFile->SetStartDatetime(startDate) != 0)
                isErrors = true;
            if (pRawFile->SetEndDatetime(endDate) != 0)
                isErrors = true;
            if (pRawFile->SetStartEvent(start_event) != 0)
                isErrors = true;
            if (pRawFile->SetEndEvent(end_event) != 0)
                isErrors = true;
            if (pRawFile->SetEventCount(event_count) != 0)
                isErrors = true;
            if (pRawFile->SetFileSize(raw_file_size) != 0)
                isErrors = true;
            TString strFileHash = UniRawFile::CalculateFileHash(input_raw_file);
            if (strFileHash != "") {
                if (pRawFile->SetFileHash(&strFileHash) != 0)
                    isErrors = true;
            } else
                cout << "WARNING: a problem occured while file checksum calculation: " << raw_root_file << endl;

            delete arrRawFiles;

            if (isErrors)
                cout << "The errors occured during raw file " << run_number << ":" << run_number << " " << start_event
                     << "-" << end_event << " info update" << endl;
            else
                cout << "Info for raw file " << run_number << ":" << run_number << " " << start_event << "-"
                     << end_event << " updated" << endl;
        }   // calculating and writing raw file info if event_count > 0
    }   // if (isRewriteMeta)
}

/**
 * @brief Appends a message to a log file
 * @param log_name Full path to the log file
 * @param message Message to be appended to the log file
 * @return 0 if successful, -1 if the log file could not be opened
 */
int append_log(TString log_name, TString message)
{
    // open log file
    ofstream logFile;
    logFile.open(log_name, ofstream::out | ofstream::app);
    if (!logFile.is_open()) {
        cout << "ERROR: log file could not be opened: " << log_name << endl;
        return -1;
    }

    logFile << message << endl;

    logFile.close();
    return 0;
}
