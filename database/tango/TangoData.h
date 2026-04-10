/****************************************************************************
           This interface can be used to get hardware detector data
             from Tango (Slow Control System) database
****************************************************************************/
#ifndef TANGODATA_H
#define TANGODATA_H 1

#include "TDatime.h"
#include "TObjArray.h"
#include "TString.h"

#include <vector>

struct CSVElement
{
    TString varName;   // variable name (e.g. ZDC_X or ZDC_Y)
    TDatime runTime;   // corresponding time
    int varValue;      // value of the parameter
};

enum Tango_Parameter_Type
{
    Tango_Bool = 1,
    Tango_Double = 2,
    Tango_Bool_Array = 11,
    Tango_Double_Array = 12
};

class TangoTimeParameter : public TObject
{
  public:
    // parameter time
    TDatime parameter_time;
    // parameter type
    Tango_Parameter_Type parameter_type;
    // parameter values of one of the corresponding parameter_type
    std::vector<bool> bool_parameter_value;
    std::vector<double> double_parameter_value;

    /** Default constructor **/
    TangoTimeParameter();
    /** Initialization constructor **/
    TangoTimeParameter(TDatime par_time, Tango_Parameter_Type par_type);
    /** Copy constructor **/
    TangoTimeParameter(const TangoTimeParameter& ttp);

    /** Destructor **/
    virtual ~TangoTimeParameter();

    ClassDef(TangoTimeParameter, 1);
};

class TangoTimeInterval : public TObject
{
  public:
    TDatime start_time;
    TDatime end_time;

    /** Default constructor **/
    TangoTimeInterval();

    /** Initialization constructor **/
    TangoTimeInterval(TDatime start_time_interval, TDatime end_time_interval);

    /** Destructor **/
    virtual ~TangoTimeInterval();

    ClassDef(TangoTimeInterval, 1);
};

// class used to get hardware detector data from Tango database and to display results in console or graphs
class TangoData
{
  public:
    enum enumConditions
    {
        conditionLess,
        conditionLessOrEqual,
        conditionEqual,
        conditionNotEqual,
        conditionGreater,
        conditionGreaterOrEqual,
        conditionLike,
        conditionNull,
        conditionNotNull
    };

    /** Single Constructor: fill Tango data array according to the given parameters
     *  Tango database connection is defined in the 'tango_db_settings.h' file
     *  Parameters:
     *  system_name - name of the system (e.g. ""magnet, "zdc" or "gem")
     *  parameter_name - name of hardware parameter stored in Tango (e.g. "uset" for ZDC or "u" for GEM)
     *  start_date - time from which to start reading the parameter, format: "YYYY-MM-DD HH:MM:SS" (e.g. "2015-03-13
     *23:00:00") end_date - end time of reading the parameter, the same format (e.g. "2015-03-13 24:00:00")
     **/
    TangoData(const char* start_date,
              const char* end_date,
              const char* system_name,
              const char* parameter_name,
              const char* member_name = "",
              int verbose = 0);
    // empty destructor
    virtual ~TangoData();

    TObjArray* GetTangoData() { return arrTangoData; }

    TString GetStartDate() { return strStartDate; }
    TString GetEndDate() { return strEndDate; }
    TString GetSystemName() { return strSystemName; }
    TString GetParameterName() { return strParameterName; }
    TString GetMemberName() { return strMemberName; }
    TString GetUserName() { return strUserName; }
    void SetUserName(TString user_name) { strUserName = user_name; }
    TString GetFullName();

    // Function displays hardware data obtained from Tango, e.g. ZDC voltage in time interval, in console
    void PrintTangoDataConsole();
    // Function displays a set of "simple" parameters obtained from Tango, e.g. GEMs high voltages voltage, in console
    static void PrintTangoArrayConsole(std::vector<TangoData*> vecTangoData);
    // Function displays hardware vector data obtained from Tango, e.g. ZDC voltage in time interval, graphically as 2D
    // Surface Graph Parameter: y_axis - label of Y axis
    void PrintTangoDataSurface(const char* y_axis = "tango parameter");
    // Function displays hardware vector data obtained from Tango, e.g. ZDC voltage in time interval, graphically as a
    // set of Line Graphs Parameters: y_axis - label of Y axis, is3D - whether show in 3D view Returns: 0 - in case of
    // the success, < 0 - if an error occured
    int PrintTangoDataMultiGraph(const char* y_axis = "parameter", bool is3D = false);
    // Function displays a set of "simple" parameters obtained from Tango, e.g. GEMs high voltages voltage, graphically
    // as a set of Line Graphs
    static int PrintTangoArrayMultiGraph(std::vector<TangoData*> vecTangoData,
                                         const char* y_axis = "parameter",
                                         bool is3D = false);

    // Function calculates and returns average value for Tango data array (result vector with size > 1 is used in case
    // of many channels)
    std::vector<double> GetAverageTangoData();

    /** Function SearchTangoIntervals gets time intervals for defined condition on the parameter
     *  NOTE: now it works only if channel count is constant during given time period
     *  Parameters:
     *  condition - condition of time interval sampling, default: conditionEqual (the possible list is presented above
     *in 'enumConditions') value - boolean value for the condition with which the comparison is performed, default: true
     *  mapChannel - vector of integer values (map) to change the order of result TObjArray-s in the common result
     *array, if, for example, channels go in a different sequence; nullptr - if not used Returns common TObjArray with
     *TObjArray objects containing TangoTimeInterval (i.e. conditionally TObjArray<TObjArray<TangoTimeInterval*>>), If
     *there are no intervals found, it returns an empty TObjArray with zero elements In case of errors it returns
     *nullptr
     **/
    TObjArray* SearchTangoIntervals(enumConditions condition = conditionEqual,
                                    bool value = true,
                                    std::vector<int>* mapChannel = nullptr);
    /** Function displays (in console) time intervals obtained from Tango for a defined condition
     *  Parameters:
     *  tango_intervals - TObjArray with TObjArray objects containing TangoTimeInterval objects obtained from
     *'SearchTangoIntervals' function channel_name - name of the dimension to display on the screen, default: Channel
     *  isShowOnlyExists - if true, print only intervals which satisfy the condition (skip empty intervals)
     **/
    void PrintTangoIntervalConsole(TObjArray* tango_intervals,
                                   TString channel_name = "Channel",
                                   bool isShowOnlyExists = false);

  private:
    bool IsOldMySQL = false;
    TString strSystemName = "", strParameterName = "", strMemberName = "", strUserName = "", strStartDate = "",
            strEndDate = "";
    TObjArray* arrTangoData = nullptr;
    int iVerbose;

    // non-user function GetTangoParameter gets hardware data from the Tango database and write to the arrTangoData
    // First, name of the target table with required parameter is extracted from the Tango Base Table,
    // then required data are selected from the target table corresponding the given time interval.
    // Returns: 0 - in case of the success, < 0 - if an error occured
    int GetTangoParameter();

    // delete Tango parameter
    void DeleteTangoParameter();

    ClassDef(TangoData, 2)
};

#endif
