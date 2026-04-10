#include "EventCatalogue.h"
#include "event_db_settings.h"

#include "nlohmann/json.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

using json = nlohmann::ordered_json;

map<int, string> const Errors {{ 401, "Unauthorized Access (check user login and password)" },
                               { 403, "Token invalid or expired" },
                               { 404, "Requested Data not found" },
                               { 409, "Incorrect Database Request (e.g. violating constraints)" },
                               { 422, "Incorrect Input Request" },
                               { 500, "Internal Server Error" },
                               { 503, "Service Unavaialble" },
                               { 504, "Server Timeout" }};

string const GetErrorString(int error_id)
{
    auto it = Errors.find(error_id);
    return it != Errors.end() ? it->second : "";
}

EventCondition::EventCondition(searchAttributes attribute, matchCriteria condition) :
    EventAttribute(attribute),
    EventMatchCriteria(condition),
    iValueType(0)
{}

EventCondition::EventCondition(searchAttributes attribute, matchCriteria condition, int value) :
    EventAttribute(attribute),
    EventMatchCriteria(condition),
    iValueType(1),
    iValue(value)
{}

EventCondition::EventCondition(searchAttributes attribute, matchCriteria condition, unsigned int value) :
    EventAttribute(attribute),
    EventMatchCriteria(condition),
    iValueType(2),
    uiValue(value)
{}

EventCondition::EventCondition(searchAttributes attribute, matchCriteria condition, double value) :
    EventAttribute(attribute),
    EventMatchCriteria(condition),
    iValueType(3),
    dValue(value)
{}

EventCondition::EventCondition(searchAttributes attribute, matchCriteria condition, string value) :
    EventAttribute(attribute),
    EventMatchCriteria(condition),
    iValueType(4),
    strValue(value)
{}

// private default constructor
EventCondition::EventCondition() :
    EventAttribute(kError),
    EventMatchCriteria(kEqual),
    iValueType(0)
{}


// Event Catalogue Constructor
EventCatalogue::EventCatalogue()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curlInstance = curl_easy_init();
    if (!curlInstance)
    {
        //cout<<"ERROR: CURL initialisation failed"<<endl;
        throw runtime_error("CURL initialisation failed");
        return;
    }

    ServiceUrl = EVENT_API_HOST + string("/") + EVENT_API_NAME + string("/v") + to_string(EventInfo::Version) +string("/");
    ServiceAccount = EVENT_API_USERNAME + string(":") + EVENT_API_PASSWORD;
    if (VerboseLevel > 2) cout<<"DEBUG: ServiceUrl = "<<ServiceUrl<<", ServiceAccount = "<<ServiceAccount<<endl;
}

// Event Catalogue Destructor
EventCatalogue::~EventCatalogue()
{
    curl_easy_cleanup(curlInstance);
    curl_global_cleanup();
}

/*!
 * Function for searching for event metadata in the Event Catalogue using specified conditions
 * \param[in] event_conditions A set of conditions to specify criteria for event metadata search
 * \return Array of the 'EventInfo' objects containing selected event metadata (enpty if not found or some errors occured)
 */
vector<unique_ptr<EventInfo>> EventCatalogue::GetEventList(vector<unique_ptr<EventCondition>> event_conditions)
{
    vector<unique_ptr<EventInfo>> event_array;
    cout<<"ERROR: EventCatalogue::GetEventList is not implemented yet"<<endl;

    return event_array;
}

/*!
 * Function tp get metadata of the event stored at the given location
 * \param[in] storage_name Storage, where the file with the event is located
 * \param[in] file_path Path of the file containing the event
 * \param[in] event_number Number of the event in the file
 * \return 'EventInfo' object containing event metadata for the event (nullptr if not found or some errors occured)
 */
unique_ptr<EventInfo> EventCatalogue::GetEventInfo(string storage_name, string file_path, int event_number)
{
    unique_ptr<EventInfo> event_info;
    cout<<"ERROR: EventCatalogue::GetEventInfo is not implemented yet"<<endl;

    return event_info;
}

/*!
 * Function to add metadata for the event stored at the given location
 * \param[in] storage_name Storage, where the file with the event is located
 * \param[in] file_path Path of the file containing the event
 * \param[in] event_number Number of the event in the file
 * \param[in] event_metadata 'EventInfo' object containing event metadata for the event
 * \return 0 - if successfully added, -2 - if event was not found at the location, -3 - connection problem with the Event Catalogue
 */
int EventCatalogue::AddEventInfo(string storage_name, string file_path, int event_number, EventInfo event_metadata)
{
    cout<<"ERROR: EventCatalogue::AddEventInfo is not implemented yet"<<endl;

    return 0;
}

size_t response_callback(void* contents, size_t size, size_t nmemb, string* data)
{
    size_t realsize = size * nmemb;
    try
    {
        data->append((char*)contents, realsize);
    }
    catch (std::bad_alloc& e)
    {
        //handle memory problem
        return 0;
    }

    return realsize;
}

static size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata)
{
    string* headers = (string*) userdata;
    try
    {
        headers->append(buffer, nitems * size);
    }
    catch (std::bad_alloc& e)
    {
        //handle memory problem
        return 0;
    }

    return nitems * size;
}

/*!
 * Function to add metadata of events to the Event Catalogue using input JSON file path
 * \param[in] json_file_path path of the JSON file containing a set of event metadata to be stored
 * \return number (>=0) of successfully added event metadata records or error_code (<0) in case of failures
 */
int EventCatalogue::AddEventInfo(string json_file_path)
{
    ifstream json_file(json_file_path);
    ostringstream sstr;
    sstr<<json_file.rdbuf();
    string json_string = sstr.str();
    if (VerboseLevel > 2) cout<<"DEBUG: json_string = "<<json_string<<endl;

    string success_message = "Event metadata have been written from JSON file '" + json_file_path + "' to the Event Catalogue";
    int return_code = PostRequest(ServiceUrl + "event", json_string, success_message);

    return return_code;
}

/*!
 * Function to update metadata of the event stored at the given location
 * \param[in] storage_name Storage, where the file with the event is located
 * \param[in] file_path Path of the file containing the event
 * \param[in] event_number Number of the event in the file
 * \param[in] event_metadata 'EventInfo' object containing event metadata for the event
 * \return 0 - if successfully updated, -2 - if event was not found at the location, -3 - connection problem with the Event Catalogue
 */
int EventCatalogue::UpdateEventInfo(string storage_name, string file_path, int event_number, EventInfo event_metadata)
{
    cout<<"ERROR: EventCatalogue::UpdateEventInfo is not implemented yet"<<endl;

    return 0;
}

/*!
 * Function to delete metadata for the event stored at the given location
 * \param[in] storage_name Storage, where the file with the event is located
 * \param[in] file_path Path of the file containing the event
 * \param[in] event_number Number of the event in the file
 * \return 0 - if successfully deleted, -2 - if event was not found at the location, -3 - connection problem with the Event Catalogue
 */
int EventCatalogue::DeleteEventInfo(string storage_name, string file_path, int event_number)
{
    cout<<"ERROR: EventCatalogue::DeleteEventInfo is not implemented yet"<<endl;

    return 0;
}

/*!
 * Function to check metadata for the event stored at the given location
 * \param[in] storage_name Storage, where the file with the event is located
 * \param[in] file_path Path of the file containing the event
 * \param[in] event_number Number of the event in the file
 * \return 0 - if event metadata is avaialble, -1 - if event metadata is not set, -2 - if event was not found at the location, -3 - connection problem with the Event Catalogue
 */
int EventCatalogue::CheckEventInfo(string storage_name, string file_path, int event_number)
{
    cout<<"ERROR: EventCatalogue::CheckEventInfo is not implemented yet"<<endl;

    return 0;
}

/*!
 * Function to print metadata for the event stored at the given location
 * \param[in] storage_name Storage, where the file with the event is located
 * \param[in] file_path Path of the file containing the event
 * \param[in] event_number Number of the event in the file
 * \return 0 - if successfull, -2 - if event was not found at the location, -3 - connection problem with the Event Catalogue
 */
int EventCatalogue::PrintEventInfo(string storage_name, string file_path, int event_number)
{
    cout<<"ERROR: EventCatalogue::PrintEventInfo is not implemented yet"<<endl;

    return 0;
}

/*!
 * Function to add storage to the dictionary used by the Event Catalogue
 * \param[in] storage_name Name of the storage be added to the dictionary
 * \return 0 - if successfull, <0 - an error occured
 */
int EventCatalogue::AddStorageInfo(string storage_name)
{
    // write storage metadata to JSON element
    //json storage_element;
    //storage_element["storage_id"] = -1;
    //storage_element["storage_name"] = storage_name;
    string json_string = "{\"storage_id\": -1, \"storage_name\": \"" + storage_name + "\"}";
    if (VerboseLevel > 2) cout<<"DEBUG: json_string = "<<json_string<<endl;

    string success_message = "Storage information has been added for '" + storage_name + "' to the Event Catalogue";
    int return_code = PostRequest( ServiceUrl + "storage", json_string, success_message);

    return return_code;
}

/*!
 * Function to add software version to the dictionary used by the Event Catalogue
 * \param[in] software_version Version of the software, which was used to obtain processed events
 * \return 0 - if successfull, <0 - an error occured
 */
int EventCatalogue::AddSoftwareInfo(string software_version)
{
    // write software version to JSON element
    //json storage_element;
    //storage_element["software_id"] = -1;
    //storage_element["software_version"] = software_version;
    string json_string = "{\"software_id\": -1, \"software_version\": \"" + software_version + "\"}";
    if (VerboseLevel > 2) cout<<"ERROR: json_string = "<<json_string<<endl;

    string success_message = "Software version has been added for '" + software_version + "' version to the Event Catalogue";
    int return_code = PostRequest( ServiceUrl + "software", json_string, success_message);

    return return_code;
}

/*!
 * Function for searching for event metadata in the Event Catalogue using REST API request as an HTTP(s) string
 * \param[in] rest_api_request String HTTP command representing a request using REST API format
 * \return Array of the 'EventInfo' objects containing selected event metadata (enpty if not found or some errors occured)
 */
vector<unique_ptr<EventInfo>> EventCatalogue::EventCatalogueRequest(string rest_api_request)
{
    vector<unique_ptr<EventInfo>> event_array;
    cout<<"ERROR: EventCatalogue::EventCatalogueRequest is not implemented yet"<<endl;

    return event_array;
}

/*!
 * Function to perform a set of specific checks in the Event Catalogue
 * \param[out] file_not_found Number of not found files for existing metadata in the Catalogue
 * \param[out] event_list List of not found files for existing metadata in the Catalogue
 * \return 0 - if successfull, -3 - connection problem with the Event Catalogue
 */
int EventCatalogue::EventCatalogueChecks(int& file_not_found, vector<unique_ptr<EventInfo>>& event_list)
{
    cout<<"ERROR: EventCatalogue::EventCatalogueChecks is not implemented yet"<<endl;

    return 0;
}

/*!
 * Function to collect summary statistics on the Event Catalogue
 * \param[out] record_count Number of records in the Event Catalogue
 * \return 0 - if successfull, -3 - connection problem with the Event Catalogue
 */
int EventCatalogue::EventCatalogueStats(int& record_count)
{
    cout<<"ERROR: EventCatalogue::EventCatalogueStats is not implemented yet"<<endl;

    return 0;
}

int EventCatalogue::PostRequest(string RequestUrl, string json_string, string success_message)
{
    struct curl_slist* http_header = nullptr;
    http_header = curl_slist_append(http_header, "Accept: */*");
    http_header = curl_slist_append(http_header, "Content-Type: application/json");
    http_header = curl_slist_append(http_header, "charset: utf-8");
    curl_easy_setopt(curlInstance, CURLOPT_HTTPHEADER, http_header);
    curl_easy_setopt(curlInstance, CURLOPT_URL, RequestUrl.c_str());
    curl_easy_setopt(curlInstance, CURLOPT_USERPWD, ServiceAccount.c_str());

    //curl_easy_setopt(curlInstance, CURLOPT_TCP_KEEPALIVE, 1);
    //curl_easy_setopt(curlInstance, CURLOPT_TCP_KEEPIDLE, 30);
    //curl_easy_setopt(curlInstance, CURLOPT_TCP_KEEPINTVL, 30);
    //curl_easy_setopt(curlInstance, CURLOPT_TCP_KEEPCNT, 720);

    curl_easy_setopt(curlInstance, CURLOPT_POST, 1);
    curl_easy_setopt(curlInstance, CURLOPT_POSTFIELDS, json_string.c_str());

    string response_string, header_string;
    curl_easy_setopt(curlInstance, CURLOPT_WRITEFUNCTION, response_callback);
    curl_easy_setopt(curlInstance, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curlInstance, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curlInstance, CURLOPT_HEADERDATA, &header_string);

    // write new event metadata to the Event Catalogue
    int res = curl_easy_perform(curlInstance);
    if (res)
    {
        cout<<"ERROR: sending command to '"<<RequestUrl<<"' failed with code = "<<res<<endl;
        curl_slist_free_all(http_header);
        return -1;
    }
    curl_slist_free_all(http_header);

    long response_code;
    curl_easy_getinfo(curlInstance, CURLINFO_RESPONSE_CODE, &response_code);
    if (VerboseLevel > 2)
    {
        double elapsed;
        curl_easy_getinfo(curlInstance, CURLINFO_TOTAL_TIME, &elapsed);
        char* url;
        curl_easy_getinfo(curlInstance, CURLINFO_EFFECTIVE_URL, &url);
        cout<<"DEBUG: CURLINFO_TOTAL_TIME = "<<elapsed<<", CURLINFO_EFFECTIVE_URL = "<<string(url)<<endl;
    }
    // check the result of the request
    if ((response_code >= 200) && (response_code < 205))
    {
        if (VerboseLevel > 0) cout<<success_message<<endl;
        if (VerboseLevel > 1) cout<<"Request has been completed with the following response: "<<response_string<<endl;
    }
    else
    {
        string error_message = GetErrorString(response_code);
        cout<<"ERROR: Sending request to '"<<RequestUrl<<"' has been failed";
        if (error_message != "") cout<<": "<<error_message<<endl;
        else cout<<" with code = "<<response_code<<endl;
        if (VerboseLevel > 1) cout<<"Error Message:"<<endl<<response_string<<endl<<"Error Output Header:"<<endl<<header_string<<endl;
        return -1*response_code;
    }

    return 0;
}
