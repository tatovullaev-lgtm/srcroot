#include "TangoData.h"

#include "TColor.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TangoConnection.h"

#include <TAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TSQLServer.h>
#include <TSQLStatement.h>
#include <TStyle.h>
#include <fstream>
#include <iostream>
using namespace std;

// Tango Database connection based on MySQL before 2020-03-24
const char* const TANGO_DB_USERNAME_b2020 = "tango";
const char* const TANGO_DB_PASSWORD_b2020 = "tangompd";
const char* const TANGO_DB_HOST_b2020 = "10.18.11.66";
const char* const TANGO_DB_NAME_b2020 = "hdbpp";

TangoTimeParameter::TangoTimeParameter() {}

TangoTimeParameter::TangoTimeParameter(TDatime par_time, Tango_Parameter_Type par_type)
{
    parameter_time = par_time;
    parameter_type = par_type;
}

// Copy constructor
TangoTimeParameter::TangoTimeParameter(const TangoTimeParameter& ttp)
    : TObject(ttp)
    , parameter_time(ttp.parameter_time)
    , parameter_type(ttp.parameter_type)
{
    bool_parameter_value = ttp.bool_parameter_value;
    double_parameter_value = ttp.double_parameter_value;
}

TangoTimeParameter::~TangoTimeParameter() {}

TangoTimeInterval::TangoTimeInterval() {}

TangoTimeInterval::TangoTimeInterval(TDatime start_time_interval, TDatime end_time_interval)
{
    start_time = start_time_interval;
    end_time = end_time_interval;
}

TangoTimeInterval::~TangoTimeInterval() {}

/**
 *  TangoData CLASS IMPLEMENTATION
 **/

/** Single Constructor: fill Tango data array according to the given parameters
 *  Tango database connection is defined in the 'tango_db_settings.h' file
 *  Parameters:
 *  system_name - name of the system (e.g. "zdc" or "gem")
 *  parameter_name - name of hardware parameter stored in Tango (e.g. "uset" for ZDC or "u" for GEM)
 *  date_start - time from which to start reading the parameter, format: "YYYY-MM-DD HH:MM:SS" (e.g. "2015-03-13
 *23:00:00") date_end - end time of reading the parameter, the same format (e.g. "2015-03-13 24:00:00")
 **/
TangoData::TangoData(const char* start_date,
                     const char* end_date,
                     const char* system_name,
                     const char* parameter_name,
                     const char* member_name,
                     int verbose)
    : strSystemName(system_name)
    , strParameterName(parameter_name)
    , strMemberName(member_name)
    , strUserName("")
    , strStartDate(start_date)
    , strEndDate(end_date)
    , iVerbose(verbose)
{
    if (strcmp(start_date, "2020-03-25") < 0) {
        IsOldMySQL = true;
        if (iVerbose)
            cout << "Old MySQL Database is used to get Tango data" << endl;
    } else if (iVerbose)
        cout << "PostgreSQL Database is used to get Tango data" << endl;

    GetTangoParameter();
}

TangoData::~TangoData()
{
    if (arrTangoData != nullptr) {
        arrTangoData->Delete();
        delete arrTangoData;
    }
}

TString TangoData::GetFullName()
{
    TString strFullName = strSystemName + ":" + strParameterName;
    if (strMemberName != "")
        strFullName += " (" + strMemberName + ")";

    return strFullName;
}

// non-user function GetTangoParameter gets hardware data from the Tango database and write to the arrTangoData
// First, name of the target table with required parameter is extracted from the Tango Base Table,
// then required data are selected from the target table corresponding the given time interval.
// Returns: 0 - in case of the success, < 0 - if an error occured
int TangoData::GetTangoParameter()
{
    if (iVerbose)
        cout << "Input Start Date = " << strStartDate.Data() << endl;
    if (iVerbose)
        cout << "Input End Date = " << strEndDate.Data() << endl;
    if ((strStartDate == "") || (strEndDate == "")) {
        cout << "ERROR: start datetime and end datetime should not be empty!" << endl;
        return -1;
    }

    // new start and end datetime with time zone if PgSQL
    TString start_date_new(strStartDate), end_date_new(strEndDate);
    if (!IsOldMySQL)
        start_date_new += " Europe/Moscow", end_date_new += " Europe/Moscow";

    // TANGO database connection (MySQL before 2020-03-24 and PgSQL after)
    TangoConnection* connDb = nullptr;
    if (IsOldMySQL)
        connDb = TangoConnection::Open(TangoConnection::MySQL,
                                       TANGO_DB_HOST_b2020,
                                       TANGO_DB_NAME_b2020,
                                       TANGO_DB_USERNAME_b2020,
                                       TANGO_DB_PASSWORD_b2020);
    else
        connDb = TangoConnection::Open();
    if (connDb == nullptr)
        return -2;
    TSQLServer* db_server = connDb->GetSQLServer();
    if (db_server == nullptr)
        return -3;

    if (db_server->GetTables(db_server->GetDB()) == nullptr)
        return -4;

    // searching for a table name with corresponding parametter type, where the parameter with given system and
    // parameter names is stored Statement returns parameter ID and parameter table
    TString strStatement;
    if (IsOldMySQL)
        strStatement = TString::Format(
            "SELECT att_conf_id, data_type "
            "FROM att_conf ac join att_conf_data_type acdt on ac.att_conf_data_type_id = acdt.att_conf_data_type_id "
            "WHERE family='%s' and name='%s' ",
            strSystemName.Data(),
            strParameterName.Data());
    else {
        strStatement = TString::Format("SELECT att_conf_id, table_name "
                                       "FROM att_conf "
                                       "WHERE domain='%s' and name='%s' ",
                                       strSystemName.Data(),
                                       strParameterName.Data());
        if (strMemberName != "")
            strStatement += TString::Format("and member = '%s' ", strMemberName.Data());
    }

    if (iVerbose)
        cout << "Processing the following SQL request to get info on the Tango parameter:" << endl
             << strStatement << endl;
    TSQLStatement* stmt_select = db_server->Statement(strStatement);
    if (!stmt_select->Process()) {
        cout << "ERROR: getting info on parameter from Tango has been failed: " << GetFullName() << endl;
        delete stmt_select;
        return -5;
    }

    stmt_select->StoreResult();
    if (!stmt_select->NextResultRow()) {
        cout << "ERROR: There is no Tango parameter '" << GetFullName() << endl;
        delete stmt_select;
        return -6;
    }
    // Tango team duplicated (old MySQL solution) parameters for every Nuclotron session
    if ((IsOldMySQL) && (stmt_select->GetNumAffectedRows() > 1) && (strcmp(strEndDate.Data(), "2018-01-01") > 0)) {
        if (!stmt_select->NextResultRow()) {
            cout << "ERROR: There is no second row for parameter '" << strParameterName << "' for " << strSystemName
                 << " detector" << endl;
            delete stmt_select;
            return -7;
        }
    }

    int parameter_id = stmt_select->GetInt(0);
    TString table_name(stmt_select->GetString(1));
    if (IsOldMySQL)
        table_name = TString::Format("att_%s", table_name.Data());
    if (iVerbose)
        cout << "Found Data Table name = " << table_name << ". Parameter ID = " << parameter_id << endl;
    delete stmt_select;

    // parse parameter type. current support for att_array_devdouble, att_array_devboolean, att_scalar_devdouble
    Tango_Parameter_Type par_type;
    if (table_name.Contains("array_devdouble"))
        par_type = Tango_Double_Array;
    else {
        if (table_name.Contains("array_devboolean"))
            par_type = Tango_Bool_Array;
        else {
            if (table_name.Contains("scalar_devdouble"))
                par_type = Tango_Double;
            else {
                cout << "ERROR: This Tango type is not supported: '" << table_name << "'" << endl;
                return -8;
            }
        }
    }

    // getting data from the table found by the attribute type
    TString query_data;
    if (IsOldMySQL)
        query_data = "SELECT date_format(data_time, '%Y-%m-%d %H:%i:%S'), value_r ";
    else
        query_data = "SELECT to_char(data_time, 'YYYY-MM-DD HH24:MI:SS.MS'), value_r ";

    if (par_type > 10)   // if the parameter is stored as array of values
        query_data += ", dim_x_r, dim_x_w, idx ";

    // add a new point with the previous value (<=date_start)
    query_data += TString::Format("FROM %s "
                                  "WHERE att_conf_id='%d' and ((data_time > '%s' and data_time <= '%s') "
                                  "or (data_time = (SELECT max(data_time) FROM %s WHERE att_conf_id='%d' and data_time "
                                  "<= '%s' and value_r is not null))) "
                                  "and value_r is not null ",
                                  table_name.Data(),
                                  parameter_id,
                                  start_date_new.Data(),
                                  end_date_new.Data(),
                                  table_name.Data(),
                                  parameter_id,
                                  start_date_new.Data());

    if (par_type > 10)   // if the parameter is stored as array of values
        query_data += "ORDER BY data_time,idx";
    else
        query_data += "ORDER BY data_time";

    if (iVerbose)
        cout << "Query data with the following SQL request:" << endl << query_data << endl;
    stmt_select = db_server->Statement(query_data);
    if (!stmt_select->Process()) {
        cout << "ERROR: getting parameter values from Tango has been failed" << endl;
        delete stmt_select;
        return -9;
    }

    stmt_select->StoreResult();

    Int_t cur_num = 0;
    TangoTimeParameter* par = nullptr;
    // create Tango data array and write data to it
    arrTangoData = new TObjArray();
    arrTangoData->SetName(
        TString::Format("'%s' parameter for %s detector", strParameterName.Data(), strSystemName.Data()));
    arrTangoData->SetOwner(kTRUE);
    while (stmt_select->NextResultRow()) {
        const char* pc_datetime = stmt_select->GetString(0);
        TDatime datetime(pc_datetime);
        // shift the previous time va;ue to the date_start position
        if ((cur_num == 0) && (strcmp(pc_datetime, strStartDate) < 0))
            datetime.Set(strStartDate);

        int i_par_len = 1;
        if (par_type > 10)
            i_par_len = stmt_select->GetInt(2);
        if (i_par_len == 0) {
            cout << "ERROR: Parameter length cannot be equal 0" << endl;
            delete stmt_select;
            DeleteTangoParameter();
            return -10;
        }

        int i_real_par_len = 1;
        if (par_type > 10)
            i_real_par_len = stmt_select->GetInt(3);
        if (i_real_par_len == 0) {
            cout << "ERROR: Real parameter length cannot be equal 0" << endl;
            delete stmt_select;
            DeleteTangoParameter();
            return -11;
        }

        par = new TangoTimeParameter(datetime, par_type);
        // cout<<par.parameter_time.AsString()<<endl;
        for (int ind = 0; ind < i_par_len; ind++) {
            if (ind > 0)
                stmt_select->NextResultRow();

            int i_idx = 0;
            if (par_type > 10)
                i_idx = stmt_select->GetInt(4);
            // cout<<"idx:ind - "<<idx<<":"<<ind<<endl;
            if (i_idx != ind) {
                cout << "ERROR: idx should be equal index of the parameter array" << endl;
                delete stmt_select;
                DeleteTangoParameter();
                return -12;
            }

            if (ind >= i_real_par_len)
                continue;

            if ((par_type == Tango_Bool_Array) || (par_type == Tango_Bool)) {
                bool b_val = (bool)stmt_select->GetInt(1);
                par->bool_parameter_value.push_back(b_val);
                par->double_parameter_value.push_back(b_val);
                // cout<<par.bool_parameter_value[par.bool_parameter_value.size()-1]<<endl;
            } else {
                double d_val = stmt_select->GetDouble(1);
                par->double_parameter_value.push_back(d_val);
                // cout<<par.double_parameter_value[par.double_parameter_value.size()-1]<<endl;
            }
        }
        if (iVerbose)
            cout << "Boolean parameter value size = " << par->bool_parameter_value.size() << endl;
        if (iVerbose)
            cout << "Double parameter value size = " << par->double_parameter_value.size() << endl;

        arrTangoData->Add(par);
        cur_num++;
    }
    delete stmt_select;

    // add date_end point (if not exists) equal to the the previous value
    if ((par != nullptr) && (strcmp(par->parameter_time.AsSQLString(), strEndDate.Data()) < 0)) {
        TangoTimeParameter* par_last = new TangoTimeParameter(*par);
        par_last->parameter_time.Set(strEndDate);
        arrTangoData->Add(par_last);
    }

    return 0;
}

// Function displays hardware data obtained from Tango, e.g. ZDC voltage in time interval, in console
void TangoData::PrintTangoDataConsole()
{
    if (arrTangoData == nullptr) {
        cout << "ERROR: Tango parameter has not been obtained" << endl;
        return;
    }

    int def_precision = cout.precision();
    cout.precision(17);

    int par_length = 0;
    for (int i = 0; i < arrTangoData->GetEntriesFast(); i++) {
        TangoTimeParameter* pParameter = (TangoTimeParameter*)arrTangoData->At(i);
        cout << pParameter->parameter_time.AsSQLString() << endl;

        // cout<<TD->dataArray[i].parameter_length<<endl;
        par_length = pParameter->double_parameter_value.size();
        for (int j = 0; j < par_length; j++)
            cout << pParameter->double_parameter_value[j] << "  ";
        cout << "" << endl << endl;
    }

    cout.precision(def_precision);
    return;
}

// Function displays a set of "simple" parameters obtained from Tango, e.g. GEMs high voltages voltage, in console
void TangoData::PrintTangoArrayConsole(vector<TangoData*> vecTangoData)
{
    for (const auto& tango_data : vecTangoData) {
        cout << "The current parameter: " << tango_data->GetFullName() << endl;
        tango_data->PrintTangoDataConsole();
    }
}

// Function displays hardware vector data obtained from Tango, e.g. ZDC voltage in time interval, graphically as 2D
// Surface Graph Parameter: y_axis - label of Y axis
void TangoData::PrintTangoDataSurface(const char* y_axis)
{
    if (arrTangoData == nullptr) {
        cout << "ERROR: Tango parameter has not been obtained" << endl;
        return;
    }

    TCanvas* c1 = new TCanvas("c1", "Tango Data", 800, 600);
    TGraph2D* tango_graph = new TGraph2D();
    tango_graph->SetTitle(arrTangoData->GetName());

    int par_length = 0;
    for (int i = 0; i < arrTangoData->GetEntriesFast(); i++) {
        TangoTimeParameter* pParameter = (TangoTimeParameter*)arrTangoData->At(i);

        par_length = pParameter->double_parameter_value.size();
        // cout<<"Parameter length = "<<par_length<<endl;
        for (int j = 0; j < par_length; j++) {
            int cur_time = pParameter->parameter_time.Convert();
            double value = pParameter->double_parameter_value[j];
            // cout<<x<<" "<<y<<" "<<z<<endl;
            tango_graph->SetPoint(i * par_length + j, cur_time, j + 1, value);
        }
    }

    tango_graph->Draw("SURF1");
    gPad->Update();

    tango_graph->GetXaxis()->SetTitle("time");
    tango_graph->GetXaxis()->CenterTitle();
    tango_graph->GetXaxis()->SetTitleOffset(1.9);
    tango_graph->GetXaxis()->SetLabelSize(0.025);
    tango_graph->GetXaxis()->SetLabelOffset(0.009);
    tango_graph->GetXaxis()->SetNdivisions(-503);

    tango_graph->GetYaxis()->SetTitle(y_axis);
    tango_graph->GetYaxis()->CenterTitle();
    tango_graph->GetYaxis()->SetTitleOffset(1.9);
    tango_graph->GetYaxis()->SetLabelSize(0.025);
    tango_graph->GetYaxis()->SetLabelOffset(0.001);
    tango_graph->GetYaxis()->CenterLabels();
    tango_graph->GetYaxis()->SetNdivisions(par_length + 1, 0, 0);

    tango_graph->GetZaxis()->SetLabelSize(0.025);

    // form X-axis with time ticks
    tango_graph->GetXaxis()->SetTimeDisplay(1);
    tango_graph->GetXaxis()->SetTimeFormat("%Y.%m.%d %H:%M");
    tango_graph->GetXaxis()->SetTimeOffset(0, "local");

    c1->Modified();

    return;
}

// Function PrintTangoDataMulti3D display hardware vector data obtained from Tango, e.g. ZDC voltage in time interval,
// graphically as a set of Line Graphs Parameter: tango_data - TObjArray with TangoTimeParameter objects obtained from
// 'GetTangoParameter' function
int TangoData::PrintTangoDataMultiGraph(const char* y_axis, bool is3D)
{
    if (arrTangoData == nullptr) {
        cout << "ERROR: Tango parameter has not been obtained" << endl;
        return -1;
    }

    if (arrTangoData->GetEntries() < 1) {
        cout << "WARNING: There are no elements in the Tango array" << endl;
        return -2;
    }

    TCanvas* c1 = new TCanvas("c1", "Tango Data", 800, 600);

    // cout<<"Tango data array size = "<<arrTangoData->GetEntries()<<endl;
    int par_length = ((TangoTimeParameter*)arrTangoData->At(0))->double_parameter_value.size();
    // cout<<"par_length = "<<par_length<<endl;
    TGraph* tango_graphs = new TGraph[par_length]();
    gRandom->SetSeed();
    for (int j = 0; j < par_length; j++) {
        Int_t color = gRandom->Integer(50);
        if ((color == 0) || (color == 10) || ((color > 16) && (color < 20)))
            color += 20;
        tango_graphs[j].SetLineColor(color);
    }

    for (int i = 0; i < arrTangoData->GetEntriesFast(); i++) {
        TangoTimeParameter* pParameter = (TangoTimeParameter*)arrTangoData->At(i);
        par_length = pParameter->double_parameter_value.size();
        for (int j = 0; j < par_length; j++) {
            int cur_time = pParameter->parameter_time.Convert();
            double value = pParameter->double_parameter_value[j];
            tango_graphs[j].SetPoint(i, cur_time, value);
        }
    }

    TMultiGraph* tango_multi = new TMultiGraph();
    tango_multi->SetTitle(arrTangoData->GetName());
    for (int j = 0; j < par_length; j++) {
        tango_multi->Add(&tango_graphs[j]);
        tango_graphs[j].SetTitle(TString::Format("%d", j));
        tango_graphs[j].SetLineWidth(3);
    }

    TString draw_par = "AL";
    if (is3D)
        draw_par += "3D";
    tango_multi->Draw(draw_par.Data());
    gPad->Update();

    if (!is3D) {
        tango_multi->GetXaxis()->SetTitle("time");
        tango_multi->GetXaxis()->CenterTitle();
        tango_multi->GetXaxis()->SetTitleOffset(1.3);
        tango_multi->GetXaxis()->SetLabelSize(0.025);
        tango_multi->GetXaxis()->SetLabelOffset(0.009);
        tango_multi->GetXaxis()->SetNdivisions(-503);
        // form X-axis with time ticks
        tango_multi->GetXaxis()->SetTimeDisplay(1);
        tango_multi->GetXaxis()->SetTimeFormat("%Y.%m.%d %H:%M");
        tango_multi->GetXaxis()->SetTimeOffset(0, "local");

        tango_multi->GetYaxis()->SetTitle(y_axis);
        tango_multi->GetYaxis()->CenterTitle();
        tango_multi->GetYaxis()->SetTitleOffset(1.3);
        tango_multi->GetYaxis()->SetLabelSize(0.025);
        tango_multi->GetYaxis()->SetLabelOffset(0.001);
        tango_multi->GetYaxis()->CenterLabels();
        // gr->GetYaxis()->SetNdivisions(10, 0, 0);
    }

    TLegend* pLegend = c1->BuildLegend(0.92, 0.77, 0.99, 0.98, "");
    pLegend->SetMargin(0.80);

    c1->Modified();
    return 0;
}

// Function displays a set of "simple" parameters obtained from Tango, e.g. GEMs high voltages voltage, graphically as a
// set of Line Graphs
int TangoData::PrintTangoArrayMultiGraph(vector<TangoData*> vecTangoData, const char* y_axis, bool is3D)
{
    if (vecTangoData.size() < 1) {
        cout << "WARNING: There are no parameters in the specified array" << endl;
        return -1;
    }

    // cout<<"Tango data array size = "<<vecTangoData.size()<<endl;
    int par_length = vecTangoData.size();
    // cout<<"par_length = "<<par_length<<endl;

    TCanvas* c1 = new TCanvas("c1", "Tango Data", 800, 600);
    TGraph* tango_graphs = new TGraph[par_length]();
    gRandom->SetSeed();
    int j;
    for (j = 0; j < par_length; j++) {
        Int_t color = gRandom->Integer(50);
        if ((color == 0) || (color == 10) || ((color > 16) && (color < 20)))
            color += 20;
        tango_graphs[j].SetLineColor(color);
    }

    j = 0;
    for (const auto& current_parameter : vecTangoData) {
        if (current_parameter->GetTangoData()->IsEmpty()) {
            cout << "ERROR: There are no elements in the parameter (" << current_parameter->GetFullName() << ")"
                 << endl;
            return -2;
        }

        if ((((TangoTimeParameter*)current_parameter->GetTangoData()->At(0))->double_parameter_value.size()) != 1) {
            cout << "ERROR: the parameter " << current_parameter->GetFullName() << " must have single value" << endl;
            return -3;
        }

        TObjArray* arrTangoData = current_parameter->GetTangoData();
        for (int i = 0; i < arrTangoData->GetEntriesFast(); i++) {
            int cur_time = ((TangoTimeParameter*)arrTangoData->At(i))->parameter_time.Convert();
            double value = ((TangoTimeParameter*)arrTangoData->At(i))->double_parameter_value[0];
            tango_graphs[j].SetPoint(i, cur_time, value);
        }
        j++;
    }

    TMultiGraph* tango_multi = new TMultiGraph();
    tango_multi->SetTitle(vecTangoData[0]->GetFullName());
    for (j = 0; j < par_length; j++) {
        tango_multi->Add(&tango_graphs[j]);
        tango_graphs[j].SetTitle(TString::Format("%d", j));
        tango_graphs[j].SetLineWidth(3);
    }

    TString draw_par = "AL";
    if (is3D)
        draw_par += "3D";
    tango_multi->Draw(draw_par.Data());
    gPad->Update();

    if (!is3D) {
        tango_multi->GetXaxis()->SetTitle("time");
        tango_multi->GetXaxis()->CenterTitle();
        tango_multi->GetXaxis()->SetTitleOffset(1.3);
        tango_multi->GetXaxis()->SetLabelSize(0.025);
        tango_multi->GetXaxis()->SetLabelOffset(0.009);
        tango_multi->GetXaxis()->SetNdivisions(-503);
        // form X-axis with time ticks
        tango_multi->GetXaxis()->SetTimeDisplay(1);
        tango_multi->GetXaxis()->SetTimeFormat("%Y.%m.%d %H:%M");
        tango_multi->GetXaxis()->SetTimeOffset(0, "local");

        tango_multi->GetYaxis()->SetTitle(y_axis);
        tango_multi->GetYaxis()->CenterTitle();
        tango_multi->GetYaxis()->SetTitleOffset(1.3);
        tango_multi->GetYaxis()->SetLabelSize(0.025);
        tango_multi->GetYaxis()->SetLabelOffset(0.001);
        tango_multi->GetYaxis()->CenterLabels();
        // gr->GetYaxis()->SetNdivisions(10, 0, 0);
    }

    TLegend* pLegend = c1->BuildLegend(0.92, 0.77, 0.99, 0.98, "");
    pLegend->SetMargin(0.80);

    c1->Modified();
    return 0;
}

// Function calculates and returns average value for Tango data array (result vector with size > 1 is used in case of
// many channels)
vector<double> TangoData::GetAverageTangoData()
{
    vector<double> result;
    if (arrTangoData == nullptr) {
        cout << "ERROR: Tango parameter has not been obtained" << endl;
        return result;
    }

    if (iVerbose)
        cout << "strStartDateAvg = " << strStartDate.Data() << endl;
    if (iVerbose)
        cout << "strEndDateAvg = " << strEndDate.Data() << endl;
    int time_count = arrTangoData->GetEntriesFast();
    TDatime start_date(strStartDate), end_date(strEndDate);

    // get arithmetic average value
    if ((IsOldMySQL) || (time_count < 2)) {
        for (int i = 0; i < time_count; i++) {
            TangoTimeParameter* pParameter = (TangoTimeParameter*)arrTangoData->At(i);
            // cycle for Tango parameter array (from 1 to array size)
            for (size_t j = 0; j < pParameter->double_parameter_value.size(); j++) {
                if (i == 0)
                    result.push_back(pParameter->double_parameter_value[j]);
                else
                    result[j] += pParameter->double_parameter_value[j];
            }
        }

        for (size_t j = 0; j < result.size(); j++)
            result[j] /= time_count;
    }
    // get interval average value (for the new architecture of the Tango database)
    else {
        TDatime previousTime;
        TangoTimeParameter* pParameter;
        for (int i = 0; i < time_count; previousTime = pParameter->parameter_time, i++) {
            pParameter = (TangoTimeParameter*)arrTangoData->At(i);

            if (i == 0) {
                if (start_date < pParameter->parameter_time) {
                    UInt_t sec = pParameter->parameter_time.Convert() - start_date.Convert();
                    // cycle for Tango parameter array (from 1 to array size)
                    for (size_t j = 0; j < pParameter->double_parameter_value.size(); j++)
                        result.push_back(pParameter->double_parameter_value[j] * sec);
                }
                continue;
            }

            UInt_t sec = pParameter->parameter_time.Convert() - previousTime.Convert();
            // cycle for Tango parameter array (from 1 to array size)
            for (size_t j = 0; j < pParameter->double_parameter_value.size(); j++) {
                if (result.empty())
                    result.push_back(pParameter->double_parameter_value[j] * sec);
                else
                    result[j] += pParameter->double_parameter_value[j] * sec;
            }

        }   // for (int i = 0; i < time_count; previousTime = pParameter->parameter_time, i++)

        if (end_date > pParameter->parameter_time) {
            UInt_t sec = end_date.Convert() - pParameter->parameter_time.Convert();
            // cycle for Tango parameter array (from 1 to array size)
            for (size_t j = 0; j < pParameter->double_parameter_value.size(); j++)
                result[j] += pParameter->double_parameter_value[j] * sec;
        }

        UInt_t sec = end_date.Convert() - start_date.Convert();
        for (size_t j = 0; j < result.size(); j++)
            result[j] /= sec;
    }   // else - get interval average value (for the new architecture of the Tango database)

    return result;
}

/** Function SearchTangoIntervals gets time intervals for defined condition on the parameter
 *  NOTE: now it works only if channel count is constant during given time period
 *  Parameters:
 *  condition - condition of time interval sampling, default: conditionEqual (the possible list is presented above in
 *'enumConditions') value - boolean value for the condition with which the comparison is performed, default: true
 *  mapChannel - vector of integer values (map) to change the order of result TObjArray-s in the common result array,
 *if, for example, channels go in a different sequence; nullptr - if not used Returns common TObjArray with TObjArray
 *objects containing TangoTimeInterval (i.e. conditionally TObjArray<TObjArray<TangoTimeInterval*>>), If there are no
 *intervals found, it returns an empty TObjArray with zero elements In case of errors it returns nullptr
 **/
TObjArray* TangoData::SearchTangoIntervals(enumConditions condition, bool value, vector<int>* mapChannel)
{
    if (arrTangoData == nullptr) {
        cout << "ERROR: Tango parameter has not been obtained" << endl;
        return nullptr;
    }

    TObjArray* pTimeIntervals = new TObjArray();
    pTimeIntervals->SetOwner(kTRUE);

    // if there is no points in the Tango period then return empty common TObjArray (without any elements)
    if (arrTangoData->GetEntriesFast() == 0) {
        delete arrTangoData;
        return pTimeIntervals;
    }

    // cycle for all Tango Data Array
    vector<int>
        vecStart;   // vector with start indices of the found intervals [size = parameter array size if it is array]
    vector<bool> vecCondition;   // vector with flags indicating whether the current interval satisfies the condition
                                 // [size = parameter array size if it is array]
    TangoTimeParameter* pParameter = nullptr;
    // cout<<"arrTangoData->GetEntriesFast(): "<<arrTangoData->GetEntriesFast()<<endl;
    for (int i = 0; i < arrTangoData->GetEntriesFast(); i++) {
        pParameter = (TangoTimeParameter*)arrTangoData->At(i);
        // initialization of the vectors (vecCondition = false, vecStart = 0)
        if (i == 0) {
            for (size_t j = 0; j < pParameter->bool_parameter_value.size(); j++) {
                vecCondition.push_back(false);
                vecStart.push_back(0);

                TObjArray* pChannel = new TObjArray();
                pChannel->SetOwner(kTRUE);
                pTimeIntervals->Add(pChannel);
            }
        }

        // cycle for array parameter data (if the parameter is an array) and checking the given condition
        for (size_t j = 0; j < pParameter->bool_parameter_value.size(); j++) {
            bool isCondition;
            switch (condition) {
                case conditionLess:
                    isCondition = pParameter->bool_parameter_value[j] < value;
                    break;
                case conditionLessOrEqual:
                    isCondition = pParameter->bool_parameter_value[j] <= value;
                    break;
                case conditionEqual:
                    isCondition = pParameter->bool_parameter_value[j] == value;
                    break;
                case conditionNotEqual:
                    isCondition = pParameter->bool_parameter_value[j] != value;
                    break;
                case conditionGreater:
                    isCondition = pParameter->bool_parameter_value[j] > value;
                    break;
                case conditionGreaterOrEqual:
                    isCondition = pParameter->bool_parameter_value[j] >= value;
                    break;
                default: {
                    cout << "ERROR: comparison operator in the searching of intervals is not appropriable for boolean "
                            "type"
                         << endl;
                    delete arrTangoData;
                    delete pTimeIntervals;
                    return nullptr;
                }
            }

            // if found then set start index in vecStart and set vecCondition to TRUE
            if (isCondition) {
                // save start interval
                if (!vecCondition[j]) {
                    vecCondition[j] = true;
                    vecStart[j] = i;
                    // cout<<"vecCondition "<<j<<": true"<<endl<<"Start: "<<i<<endl<<endl;
                }
            }
            // if the conidition is not satisfied
            else {
                // finally write interval if vecCondition == TRUE
                if (vecCondition[j]) {
                    vecCondition[j] = false;   // reset condition flag (found interval has ended)

                    int start_i = vecStart[j];
                    TDatime startInterval = ((TangoTimeParameter*)arrTangoData->At(start_i))->parameter_time;
                    if ((start_i > 0) && (IsOldMySQL)) {
                        TDatime previousTime = ((TangoTimeParameter*)arrTangoData->At(start_i - 1))->parameter_time;
                        TDatime middleTime((startInterval.Convert() + previousTime.Convert()) / 2);
                        startInterval = middleTime;
                    }
                    UInt_t uiEndTime = ((TangoTimeParameter*)arrTangoData->At(i))->parameter_time.Convert();
                    TDatime endInterval(uiEndTime - 1);
                    if (IsOldMySQL)
                        endInterval.Set(
                            (uiEndTime + ((TangoTimeParameter*)arrTangoData->At(i - 1))->parameter_time.Convert()) / 2);

                    // create TangoTimeInterval for the satisfied condition
                    TangoTimeInterval* pTimeInterval = new TangoTimeInterval(startInterval, endInterval);
                    int real_channel = j;
                    if (mapChannel != nullptr) {
                        if (mapChannel->size() > j) {
                            real_channel = mapChannel->at(j);
                            if (pTimeIntervals->GetEntriesFast() <= real_channel) {
                                cout << "CRITICAL WARNING: Tango array size is less than the corrected channel: "
                                     << real_channel << endl;
                                continue;
                            }
                        } else
                            cout << "CRITICAL WARNING: map channel size is less than the current channel: " << j
                                 << endl;
                    }
                    // cout<<"j = "<<j<<", real_channel = "<<real_channel<<", mapChannel->size() =
                    // "<<mapChannel->size()<<", pTimeIntervals->GetEntriesFast() =
                    // "<<pTimeIntervals->GetEntriesFast()<<endl;
                    ((TObjArray*)pTimeIntervals->At(real_channel))->Add(pTimeInterval);
                }
            }
        }   // for (int j = 0; j < pParameter->bool_parameter_value.size(); j++)
    }       // for (int i = 0; i < arrTangoData->GetEntriesFast(); i++)

    // write the last period if neccessary
    if (IsOldMySQL) {
        for (size_t j = 0; j < pParameter->bool_parameter_value.size(); j++) {
            if (vecCondition[j]) {
                vecCondition[j] = false;

                int start_i = vecStart[j];
                TDatime startInterval = ((TangoTimeParameter*)arrTangoData->At(start_i))->parameter_time;
                if (start_i > 0) {
                    TDatime previousTime = ((TangoTimeParameter*)arrTangoData->At(start_i - 1))->parameter_time;
                    TDatime middleTime((startInterval.Convert() + previousTime.Convert()) / 2);
                    startInterval = middleTime;
                }
                TDatime endInterval =
                    ((TangoTimeParameter*)arrTangoData->At(arrTangoData->GetEntriesFast() - 1))->parameter_time;

                TangoTimeInterval* pTimeInterval = new TangoTimeInterval(startInterval, endInterval);
                int real_channel = j;
                if (mapChannel != nullptr) {
                    if (mapChannel->size() > j) {
                        real_channel = mapChannel->at(j);
                        if (pTimeIntervals->GetEntriesFast() <= real_channel) {
                            cout << "CRITICAL WARNING: Tango array size is less than the corrected channel: "
                                 << real_channel << endl;
                            continue;
                        }
                    } else
                        cout << "CRITICAL WARNING: map channel size is less than the current channel: " << j << endl;
                }
                // cout<<"j = "<<j<<", real_channel = "<<real_channel<<", mapChannel->size() = "<<mapChannel->size()<<",
                // pTimeIntervals->GetEntriesFast() = "<<pTimeIntervals->GetEntriesFast()<<endl;
                ((TObjArray*)pTimeIntervals->At(real_channel))->Add(pTimeInterval);
            }
        }   // write the last period if neccessary
    }

    return pTimeIntervals;
}

/** Function displays (in console) time intervals obtained from Tango for a defined condition
 *  Parameters:
 *  tango_intervals - TObjArray with TObjArray objects containing TangoTimeInterval objects obtained from
 *'SearchTangoIntervals' function channel_name - name of the dimension to display on the screen, default: Channel
 *  isShowOnlyExists - if true, print only intervals which satisfy the condition (skip empty intervals)
 **/
void TangoData::PrintTangoIntervalConsole(TObjArray* tango_intervals, TString channel_name, bool isShowOnlyExists)
{
    for (int i = 0; i < tango_intervals->GetEntriesFast(); i++) {
        TObjArray* pChannel = (TObjArray*)tango_intervals->At(i);
        if (pChannel->GetEntriesFast() == 0) {
            if (!isShowOnlyExists)
                cout << channel_name.Data() << " " << i << ":" << endl
                     << "   No intervals correspond to the specified conditions" << endl;
            continue;
        } else
            cout << channel_name.Data() << " " << i << ":" << endl;
        for (int j = 0; j < pChannel->GetEntriesFast(); j++) {
            TangoTimeInterval* pInterval = (TangoTimeInterval*)pChannel->At(j);
            cout << "   " << pInterval->start_time.AsSQLString();
            cout << " - ";
            cout << pInterval->end_time.AsSQLString();
        }
        cout << endl;
    }

    return;
}

// delete Tango parameter
void TangoData::DeleteTangoParameter()
{
    IsOldMySQL = false;
    strSystemName = "", strParameterName = "", strStartDate = "", strEndDate = "";
    if (arrTangoData != nullptr) {
        arrTangoData->Delete();
        delete arrTangoData;
        arrTangoData = nullptr;
    }
}
