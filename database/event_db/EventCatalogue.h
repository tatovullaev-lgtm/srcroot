/** Interface to the Event Catalogue (through REST API service) to add, update, delete and search for event metadata
 *
 * @author  K. Gertsenberger
 * @version 0.2
 * @since   10.12.24
 *
 */
#ifndef EVENTCATALOGUE_H
#define EVENTCATALOGUE_H

//#include "TObject.h"

#include <curl/curl.h>

#include <string>
#include <vector>
#include <memory>
using namespace std;

/// Class presenting Event Info (Metadata) Structure
class EventInfo
{
  public:
    static const int Version = 1; ///< structure (API) version

    string StorageName = ""; ///< storage name, where the file with the event is located
    string FilePath = ""; ///< path of the file containing the event
    int    EventNumber = -1; ///< event number in the file

    string SoftwareVersion = ""; ///< version of the software used for processing the event
    int    PeriodNumber = -1; ///< number of experiment session, when the event was collected
    int    RunNumber = -1; ///< number of experiment run, when the event was collected

    int    TrackNumber = -1; ///< number of tracks in the event
};

/// Class for describing a condition to search for necessary event metadata
class EventCondition
{
  public:
    /// enumeration for attributes, which can be used in searching event metadata
    enum searchAttributes{kStorageName, kFilePath, kEventNumber, kSoftwareVersion, kPeriodNumber, kRunNumber, kTrackNumber, kError};
    /// enumeration for conditions, which can be used for matching with a specified value
    enum matchCriteria{kLess, kLessOrEqual, kEqual, kNotEqual, kGreater, kGreaterOrEqual, kLike, kNull, kNotNull};

    /*!
     * Create condition for searching event metadata
     * \param[in] attribute Used for searching event metadata
     * \param[in] condition Defining the operation of comparison with the value
     * \param[in] value Used in the condition for the comparison (if required)
     */
    EventCondition(searchAttributes attribute, matchCriteria condition);
    EventCondition(searchAttributes attribute, matchCriteria condition, int value);
    EventCondition(searchAttributes attribute, matchCriteria condition, unsigned int value);
    EventCondition(searchAttributes attribute, matchCriteria condition, double value);
    EventCondition(searchAttributes attribute, matchCriteria condition, string value);
    virtual ~EventCondition(){}

  private:
    EventCondition();

    searchAttributes EventAttribute;
    matchCriteria EventMatchCriteria;

    // 1 - int, 2 - unsigned int, 3 - double, 4 - TString, 5 - TDatime
    int iValueType;
    int iValue;
    unsigned int uiValue;
    double dValue;
    string strValue;
};

/// Main Interface Class to work with the Event Catalogue of the experiment
/// Throws runtime_exception if a critical error occurrs while the constructor
/// Error Codes: -1 - sending command via CURL failed, -401 - unauthorized request, -403 - token invalid or expired, -404 - data not found,
/// -409 - incorrect database request (e.g. violating constraints), -422 - incorrect input request, -500 - internal server error,
/// -503 - service unavaialble, -504 - gateway timeout
class EventCatalogue
{
  public:
    EventCatalogue();
    virtual ~EventCatalogue();

    /*!
     * Function for searching for event metadata in the Event Catalogue using specified conditions
     * \param[in] event_conditions A set of conditions to specify criteria for event metadata search
     * \return Array of the 'EventInfo' objects containing selected event metadata (enpty if not found or some errors occured)
     */
    vector<unique_ptr<EventInfo>> GetEventList(vector<unique_ptr<EventCondition>> event_conditions);

    /*!
     * Function tp get metadata of the event stored at the given location
     * \param[in] storage_name Storage, where the file with the event is located
     * \param[in] file_path Path of the file containing the event
     * \param[in] event_number Number of the event in the file
     * \return 'EventInfo' object containing event metadata for the event (nullptr if not found or some errors occured)
     */
    unique_ptr<EventInfo> GetEventInfo(string storage_name, string file_path, int event_number);

    /*!
     * Function to add metadata for the event stored at the given location
     * \param[in] storage_name Storage, where the file with the event is located
     * \param[in] file_path Path of the file containing the event
     * \param[in] event_number Number of the event in the file
     * \param[in] event_metadata 'EventInfo' object containing event metadata for the event
     * \return 0 - if successfully added, <0 - an error occured
     */
    int AddEventInfo(string storage_name, string file_path, int event_number, EventInfo event_metadata);

    /*!
     * Function to add metadata of events to the Event Catalogue using input JSON file path
     * \param[in] json_file_path path of the JSON file containing a set of event metadata to be stored
     * \return number (>=0) of successfully added event metadata records or error_code (<0) in case of failures
     */
    int AddEventInfo(string json_file_path);

    /*!
     * Function to update metadata of the event stored at the given location
     * \param[in] storage_name Storage, where the file with the event is located
     * \param[in] file_path Path of the file containing the event
     * \param[in] event_number Number of the event in the file
     * \param[in] event_metadata 'EventInfo' object containing event metadata for the event
     * \return 0 - if successfully updated, <0 - an error occured
     */
    int UpdateEventInfo(string storage_name, string file_path, int event_number, EventInfo event_metadata);

    /*!
     * Function to delete metadata for the event stored at the given location
     * \param[in] storage_name Storage, where the file with the event is located
     * \param[in] file_path Path of the file containing the event
     * \param[in] event_number Number of the event in the file
     * \return 0 - if successfully deleted, <0 - an error occured
     */
    int DeleteEventInfo(string storage_name, string file_path, int event_number);

    /*!
     * Function to check metadata for the event stored at the given location
     * \param[in] storage_name Storage, where the file with the event is located
     * \param[in] file_path Path of the file containing the event
     * \param[in] event_number Number of the event in the file
     * \return 0 - if event metadata is avaialble, 1 - if event metadata is not set, <0 - an error occured
     */
    int CheckEventInfo(string storage_name, string file_path, int event_number);

    /*!
     * Function to print metadata for the event stored at the given location
     * \param[in] storage_name Storage, where the file with the event is located
     * \param[in] file_path Path of the file containing the event
     * \param[in] event_number Number of the event in the file
     * \return 0 - if successfull, <0 - an error occured
     */
    int PrintEventInfo(string storage_name, string file_path, int event_number);

    /*!
     * Function to add storage to the dictionary used by the Event Catalogue
     * \param[in] storage_name Name of the storage be added to the dictionary
     * \return 0 - if successfull, <0 - an error occured
     */
    int AddStorageInfo(string storage_name);

    /*!
     * Function to add software version to the dictionary used by the Event Catalogue
     * \param[in] software_version Version of the software, which was used to obtain processed events
     * \return 0 - if successfull, <0 - an error occured
     */
    int AddSoftwareInfo(string software_version);

    /*!
     * Function for searching for event metadata in the Event Catalogue using REST API request as an HTTP(s) string
     * \param[in] rest_api_request String HTTP command representing a request using REST API format
     * \return Array of the 'EventInfo' objects containing selected event metadata (enpty if not found or some errors occured)
     */
    vector<unique_ptr<EventInfo>> EventCatalogueRequest(string rest_api_request);

    /*!
     * Function to perform a set of specific checks in the Event Catalogue
     * \param[out] file_not_found Number of not found files for existing metadata in the Catalogue
     * \param[out] event_list List of not found files for existing metadata in the Catalogue
     * \return 0 - if successfull, <0 - an error occured
     */
    int EventCatalogueChecks(int& file_not_found, vector<unique_ptr<EventInfo>>& event_list);

    /*!
     * Function to collect summary statistics on the Event Catalogue
     * \param[out] record_count Number of records in the Event Catalogue
     * \return 0 - if successfull, <0 - an error occured
     */
    int EventCatalogueStats(int& record_count);

    /*!
     * Setting Verbose level: 0 - only errors, 1 - normal output, 2 - detailed output, 2- all with debug info
     * \param[in] verbose_level Verbose level for output information
     */
    void SetVerbose(int verbose_level = 2) { VerboseLevel = verbose_level; }

  private:
    CURL* curlInstance;    // CURL instance created while the constructor
    string ServiceUrl;     // common part of the url for REST API request
    string ServiceAccount; // user account

    int VerboseLevel = 2;  // VerboseLevel: 0 - without messages except errors, 1 - normal output, 2 - detailed output, 3 - debug output

    int PostRequest(string RequestUrl, string json_string, string success_message);

  //ClassDef(EventCatalogue, 1)
};

#endif /* !EVENTCATALOGUE_H */
