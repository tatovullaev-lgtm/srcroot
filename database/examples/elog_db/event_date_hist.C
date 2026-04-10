// macro for visualizing event acquisition during the experiemnt

/* SQL queries for event statistics by shift leaders
   select max(period_number)
   from record_

   select (date(min(record_date) - interval '1 DAY'))::timestamp + INTERVAL '00:00:00', (date(max(record_date) +
   interval '1 DAY'))::timestamp + INTERVAL '23:59:59' from record_ where period_number = 9 and
   ((replace(replace(lower(record_comment), ' ', ''), 'mv', '')) ~ '\d+((\.|,)\d+)?(k|m)')

   select date(record_date), person_name,
      (coalesce(sum(rtrim(substring(lower(regexp_replace(record_comment, ' |mv', '', 'gi')), '(\d+((\.|,)\d+)?)k'), 'k')
   :: float),0) + (coalesce(sum(rtrim(substring(lower(regexp_replace(record_comment, ' |mv', '', 'gi')),
   '(\d+((\.|,)\d+)?)m'), 'm') :: float),0)*1000)) as event_count from record_ r join person_ p on r.shift_leader_id  =
   p.person_id where period_number = 9 and run_number is not null and record_date >= '2026-01-18 00:00:00' and
   record_date <= '2026-02-14 23:59:59' and
      ((lower(regexp_replace(record_comment, ' |mv', '', 'gi'))) ~ '\d+((\.|,)\d+)?(k|m)')
   group by date(record_date), person_name
   order by date(record_date)
*/

template<class T>
string FormatWithCommas(T value)
{
    stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << value;
    return ss.str();
}

// date_start, date_end format: "yyyy-mm-dd" (if empty, define maximum+1 and minimum-1 date from e-Log records)
void event_date_hist(int period_number = 9,
                     TString str_date_start = "",
                     TString str_date_end = "",
                     TString save_hist = "")
{
    // split save_hist with file names to save two histograms (';' and ',' are delimiters)
    TObjArray* arr_save_hist = save_hist.Tokenize(";,");
    TString save_hist1 = "", save_hist2 = "";
    if (arr_save_hist->GetEntries() > 0)
        save_hist1 = ((TObjString*)arr_save_hist[0][0])->GetString().Strip(TString::kBoth);
    if (arr_save_hist->GetEntries() > 1)
        save_hist2 = ((TObjString*)arr_save_hist[0][1])->GetString().Strip(TString::kBoth);

    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr)
        return;

    TSQLServer* db_server = connDb->GetSQLServer();
    // if start or end date is not set, then define one day before the first record for this run and one day after
    if ((str_date_start == "") || (str_date_end == "")) {
        int start_run_number = 0;
        if (period_number == 8)
            start_run_number = 5301;
        // get one day before and one day after for the current records of the run (where comments contain a number)
        TString sql_date = TString::Format(
            "select date(min(record_date) - interval '1 DAY'), date(max(record_date) + interval '1 DAY') "
            "from record_ "
            "where period_number = %d and run_number >= %d and ((lower(replace(record_comment, ' ', ''))) ~ '\\d+k')",
            period_number, start_run_number);

        TSQLStatement* stmt_date = db_server->Statement(sql_date);
        // get records from the database
        if (!stmt_date->Process()) {
            cout << "ERROR: getting record from the database has been failed" << endl;
            delete stmt_date;
            delete connDb;
            return;
        }
        // store result of statement in buffer
        stmt_date->StoreResult();

        if (!stmt_date->NextResultRow()) {
            cout << "ERROR: max and min record date was not found in the database" << endl;
            delete stmt_date;
            delete connDb;
            return;
        }
        // save the found dates (one day before and one day after)
        if (str_date_start == "")
            str_date_start = stmt_date->GetString(0);
        if (str_date_end == "")
            str_date_end = stmt_date->GetString(1);

        delete stmt_date;
    }
    // check the start and end dates for the correct format
    TPRegexp sql_date_format("^\\d{4}\\-(0[1-9]|1[012])\\-(0[1-9]|[12][0-9]|3[01])$");
    if ((!str_date_start.Contains(sql_date_format)) || (!str_date_end.Contains(sql_date_format))) {
        cout << "ERROR: 'date_start' or 'date_end' format is not correct, please, set them as \"YYYY-MM-DD\"" << endl;
        delete connDb;
        return;
    }

    // add time part to the start and end dates
    str_date_start += " 00:00:00";
    str_date_end += " 23:59:59";
    TDatime date_start(str_date_start), date_end(str_date_end);
    if (date_start >= date_end) {
        cout << "ERROR: 'date_end' must be greater than 'date_end'" << endl;
        delete connDb;
        return;
    }

    TString sql =
        TString::Format("select date(record_date), type_text, "
                        "       (coalesce(sum(rtrim(substring(lower(regexp_replace(record_comment, ' |mv', '', 'gi')), "
                        "'(\\d+((\\.|,)\\d+)?)k'), 'k') :: float),0) + "
                        "       (coalesce(sum(rtrim(substring(lower(regexp_replace(record_comment, ' |mv', '', 'gi')), "
                        "'(\\d+((\\.|,)\\d+)?)m'), 'm') :: float),0)*1000)) "
                        "       as event_count "
                        "from record_ r join type_ t on r.type_id = t.type_id "
                        "where period_number = %d and run_number is not null and "
                        "      record_date >= '%s' and record_date <= '%s' and "
                        "      ((lower(regexp_replace(record_comment, ' |mv', '', 'gi'))) ~ '\\d+((\\.|,)\\d+)?(k|m)') "
                        "group by date(record_date), type_text "
                        "order by date(record_date)",
                        period_number, str_date_start.Data(), str_date_end.Data());
    // cout<<"SQL query: \""<<sql<<"\""<<endl;
    TSQLStatement* stmt = db_server->Statement(sql);

    // get records from the database
    if (!stmt->Process()) {
        cout << "ERROR: getting record from the database has been failed" << endl;

        delete stmt;
        delete connDb;
        return;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // create histogram with bars
    TCanvas* c1 = new TCanvas("c1", "Event statistics by different types", 1000, 500);
    gPad->SetGrid();
    if (save_hist1.EndsWith("svg", TString::kIgnoreCase))
        gStyle->SetGridColor(17);
    int bin_count = (date_end.Convert() - date_start.Convert() + 1) / 24 / 3600;
    // cout<<"bin count = "<<bin_count<<endl;

    TH1I* hEventByDatePhys = new TH1I("hEventByDatePhys", "", bin_count, date_start.Convert(), date_end.Convert() + 1);
    // gStyle->SetTitleFontSize(0.03);    // only one approach to change histogram title size
    hEventByDatePhys->SetStats(kFALSE);
    hEventByDatePhys->LabelsDeflate();
    hEventByDatePhys->SetLabelFont(62);
    hEventByDatePhys->SetMarkerSize(0.65);
    // hEventByDatePhys->LabelsOption("v");
    hEventByDatePhys->GetXaxis()->SetTitle("run date");
    hEventByDatePhys->GetXaxis()->SetTitleSize(0.03);
    hEventByDatePhys->GetXaxis()->CenterTitle();
    hEventByDatePhys->GetXaxis()->SetTimeDisplay(1);   // The X axis is a time axis
    hEventByDatePhys->GetXaxis()->SetLabelSize(0.02 - 0.00017 * bin_count);
    hEventByDatePhys->GetXaxis()->CenterLabels();
    hEventByDatePhys->GetXaxis()->SetTimeFormat("%d.%m ");
    hEventByDatePhys->GetXaxis()->SetNdivisions(bin_count);
    hEventByDatePhys->GetYaxis()->SetTitle("event count");
    // hEventByDatePhys->GetYaxis()->SetTitleSize(0.03);
    hEventByDatePhys->GetYaxis()->SetNdivisions(1007);
    hEventByDatePhys->GetYaxis()->SetLabelSize(0.02);
    TH1I* hEventByDateTest = (TH1I*)hEventByDatePhys->Clone("hEventByDateTest");
    TH1I* hEventByDateCalib = (TH1I*)hEventByDatePhys->Clone("hEventByDateCalib");
    hEventByDatePhys->SetBarWidth(0.28);
    hEventByDatePhys->SetBarOffset(0.1);
    hEventByDatePhys->SetFillColor(49);
    hEventByDateTest->SetBarWidth(0.28);
    hEventByDateTest->SetBarOffset(0.38);
    hEventByDateTest->SetFillColor(50);
    hEventByDateCalib->SetBarWidth(0.28);
    hEventByDateCalib->SetBarOffset(0.66);
    hEventByDateCalib->SetFillColor(42);

    int total_phys_events = 0, total_test_events = 0, total_calib_events = 0;
    cout << endl << "|date       |type            |event_number|" << endl;
    cout << "|-----------|----------------|------------|" << endl;
    // extract rows from the database query
    while (stmt->NextResultRow()) {
        TString str_record_date = stmt->GetString(0);
        TDatime record_date(str_record_date + " 00:00:00");
        TString type_text = stmt->GetString(1);
        int event_count = stmt->GetInt(2);
        TString str_event_count = FormatWithCommas(event_count * 1000);
        for (int i = str_event_count.Length(); i < 12; i++)
            str_event_count += " ";

        cout << "|" << str_record_date << " |" << type_text.Data();
        if (type_text == "Physics Run") {
            hEventByDatePhys->Fill(record_date.Convert(), event_count * 1000);
            cout << "    " << " |" << str_event_count << "|" << endl;
            total_phys_events += event_count * 1000;
        } else if (type_text == "Test Run") {
            hEventByDateTest->Fill(record_date.Convert(), event_count * 1000);
            cout << "       " << " |" << str_event_count << "|" << endl;
            total_test_events += event_count * 1000;
        } else if (type_text == "Calibration Run") {
            hEventByDateCalib->Fill(record_date.Convert(), event_count * 1000);
            cout << " |" << str_event_count << "|" << endl;
            total_calib_events += event_count * 1000;
        } else {
            for (int i = type_text.Length(); i < 15; i++)
                cout << " ";
            cout << " |" << str_event_count << "|" << endl;
            cout << "WARNING: Incorrect type of the record with event number is \"" << type_text << "\"" << endl;
        }
    }
    cout << endl
         << "Total physics events - " << FormatWithCommas(total_phys_events) << endl
         << "Total test events - " << FormatWithCommas(total_test_events) << endl
         << "Total calibration events - " << FormatWithCommas(total_calib_events) << endl;

    TString hist_options = "bar2,hist,text";   // 'text' option for vertical captions with event count
    TH1* h1 = hEventByDatePhys->DrawCopy(hist_options);
    TH1* h2 = hEventByDateTest->DrawCopy(hist_options + ",same");
    TH1* h3 = hEventByDateCalib->DrawCopy(hist_options + ",same");

    TLegend* legend = new TLegend(0.13, 0.74, 0.30, 0.88);
    legend->AddEntry(h1, "Physics Events", "f");
    legend->AddEntry(h2, "Test Events", "f");
    legend->AddEntry(h3, "Calibration Events", "f");
    legend->Draw();

    if (save_hist1 != "")
        c1->SaveAs(save_hist1.Data());

    // SECOND CANVAS WITH INTEGRAL HISTOGRAM
    TH1F* hIntegralPhys = new TH1F("hIntegralPhys", "", bin_count, date_start.Convert(), date_end.Convert() + 1);
    hIntegralPhys->SetBinContent(0, hEventByDatePhys->GetBinContent(0) / 1000000.0);
    for (Int_t i = 1; i < hEventByDatePhys->GetNbinsX(); ++i) {
        hIntegralPhys->SetBinContent(i, hEventByDatePhys->GetBinContent(i) / 1000000.0
                                            + hIntegralPhys->GetBinContent(i - 1));
    }

    TCanvas* c2 = new TCanvas("c2", "Integral statistics of physics events", 1000, 500);
    c2->cd();
    gPad->SetGridy();
    hIntegralPhys->SetStats(kFALSE);
    hIntegralPhys->LabelsDeflate();
    hIntegralPhys->SetLineWidth(2);
    hIntegralPhys->SetLineColor(49);
    hIntegralPhys->SetLabelFont(62);
    hIntegralPhys->SetTitleFont(62);
    hIntegralPhys->SetMarkerSize(0.65);
    hIntegralPhys->GetXaxis()->SetTitle("run date");
    hIntegralPhys->GetXaxis()->SetTitleFont(62);
    // hIntegralPhys->GetXaxis()->LabelsOption("v");
    hIntegralPhys->GetYaxis()->SetTitleFont(62);
    hIntegralPhys->GetYaxis()->SetTitle("Integral statistics, events #times 10^{6}");
    hIntegralPhys->GetXaxis()->SetTitleSize(0.03);
    hIntegralPhys->GetXaxis()->CenterTitle();
    hIntegralPhys->GetXaxis()->SetTimeDisplay(1);   // The X axis is a time axis
    hIntegralPhys->GetXaxis()->SetLabelSize(0.02 - 0.00017 * bin_count);
    hIntegralPhys->GetXaxis()->CenterLabels();
    hIntegralPhys->GetXaxis()->SetNdivisions(bin_count);
    hIntegralPhys->SetFillColor(49);
    hIntegralPhys->SetFillStyle(3001);
    hIntegralPhys->Draw("");

    if (save_hist2 != "")
        c2->SaveAs(save_hist2.Data());
}

// date_start, date_end format: "yyyy-mm-dd" (if empty, define maximum+1 and minimum-1 date from e-Log records)
void event_shift_hist(int period_number = 8,
                      TString str_date_start = "",
                      TString str_date_end = "",
                      TString save_hist = "")
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr)
        return;

    TSQLServer* db_server = connDb->GetSQLServer();
    if ((str_date_start == "") || (str_date_end == "")) {
        int start_run_number = 0;
        if (period_number == 8)
            start_run_number = 5301;
        TString sql_date = TString::Format(
            "select date(min(record_date) - interval '1 DAY'), date(max(record_date) + interval '1 DAY') "
            "from record_ "
            "where period_number = %d and run_number >= %d and ((lower(replace(record_comment, ' ', ''))) ~ '\\d+k')",
            period_number, start_run_number);

        TSQLStatement* stmt_date = db_server->Statement(sql_date);
        // get records from the database
        if (!stmt_date->Process()) {
            cout << "ERROR: getting record from the database has been failed" << endl;
            delete stmt_date;
            delete connDb;
            return;
        }
        // store result of statement in buffer
        stmt_date->StoreResult();

        if (!stmt_date->NextResultRow()) {
            cout << "ERROR: max and min record date was not found in the database" << endl;
            delete stmt_date;
            delete connDb;
            return;
        }
        if (str_date_start == "")
            str_date_start = stmt_date->GetString(0);
        if (str_date_end == "")
            str_date_end = stmt_date->GetString(1);

        delete stmt_date;
    }
    TPRegexp sql_date_format("^\\d{4}\\-(0[1-9]|1[012])\\-(0[1-9]|[12][0-9]|3[01])$");
    if ((!str_date_start.Contains(sql_date_format)) || (!str_date_end.Contains(sql_date_format))) {
        cout << "ERROR: 'date_start' or 'date_end' format is not correct, please, set them as \"YYYY-MM-DD\"" << endl;
        delete connDb;
        return;
    }

    str_date_start += " 00:00:00";
    str_date_end += " 23:59:59";
    TDatime date_start(str_date_start), date_end(str_date_end);
    if (date_start >= date_end) {
        cout << "ERROR: 'date_end' must be greater than 'date_end'" << endl;
        delete connDb;
        return;
    }

    TString sql =
        TString::Format("select date(record_date), trunc((date_part('hour', record_date) / 8)), type_text, "
                        "       (coalesce(sum(rtrim(substring(lower(regexp_replace(record_comment, ' |mv', '', 'gi')), "
                        "'(\\d+((\\.|,)\\d+)?)k'), 'k') :: float),0) + "
                        "       (coalesce(sum(rtrim(substring(lower(regexp_replace(record_comment, ' |mv', '', 'gi')), "
                        "'(\\d+((\\.|,)\\d+)?)m'), 'm') :: float),0)*1000)) "
                        "       as event_count "
                        "from record_ r join type_ t on r.type_id = t.type_id "
                        "where period_number = %d and run_number is not null and "
                        "      record_date >= '%s' and record_date <= '%s' and "
                        "      ((lower(regexp_replace(record_comment, ' |mv', '', 'gi'))) ~ '\\d+((\\.|,)\\d+)?(k|m)') "
                        "group by date(record_date), trunc((date_part('hour', record_date) / 8)), type_text "
                        "order by date(record_date)",
                        period_number, str_date_start.Data(), str_date_end.Data());
    // cout<<"SQL query: \""<<sql<<"\""<<endl;
    TSQLStatement* stmt = db_server->Statement(sql);

    // get records from the database
    if (!stmt->Process()) {
        cout << "ERROR: getting record from the database has been failed" << endl;

        delete stmt;
        delete connDb;
        return;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // create histogram with bars
    TCanvas* c1 = new TCanvas("c1", "Event Acquisition Histogram", 1000, 600);
    gPad->SetGrid();
    if (save_hist.EndsWith("svg", TString::kIgnoreCase))
        gStyle->SetGridColor(17);
    int bin_count = (date_end.Convert() - date_start.Convert() + 1) / 8 / 3600;
    // cout<<"bin count = "<<bin_count<<endl;

    TH1I* hEventByDatePhys = new TH1I("hEventByDatePhys", "", bin_count, date_start.Convert(), date_end.Convert() + 1);
    // gStyle->SetTitleFontSize(0.03);    // only one approach to change histogram title size
    hEventByDatePhys->SetStats(kFALSE);
    hEventByDatePhys->LabelsDeflate();
    hEventByDatePhys->SetLabelFont(62);
    hEventByDatePhys->SetMarkerSize(0.65);
    hEventByDatePhys->GetXaxis()->SetTitle("run date");
    hEventByDatePhys->GetXaxis()->SetTitleSize(0.03);
    hEventByDatePhys->GetXaxis()->CenterTitle();
    hEventByDatePhys->GetXaxis()->SetTimeDisplay(1);   // The X axis is a time axis
    hEventByDatePhys->GetXaxis()->SetLabelSize(0.012);
    hEventByDatePhys->GetXaxis()->CenterLabels();
    // hEventByDatePhys->GetXaxis()->SetTimeFormat("  %d/%m");
    hEventByDatePhys->GetXaxis()->SetNdivisions(bin_count / 3);   // divisions by days
    hEventByDatePhys->GetYaxis()->SetTitle("event count");
    hEventByDatePhys->GetYaxis()->SetTitleSize(0.03);
    hEventByDatePhys->GetYaxis()->SetNdivisions(1007);
    hEventByDatePhys->GetYaxis()->SetLabelSize(0.02);
    TH1I* hEventByDateTest = (TH1I*)hEventByDatePhys->Clone("hEventByDateTest");
    TH1I* hEventByDateCalib = (TH1I*)hEventByDatePhys->Clone("hEventByDateCalib");
    hEventByDatePhys->SetBarWidth(0.28);
    hEventByDatePhys->SetBarOffset(0.1);
    hEventByDatePhys->SetFillColor(49);
    hEventByDateTest->SetBarWidth(0.28);
    hEventByDateTest->SetBarOffset(0.38);
    hEventByDateTest->SetFillColor(50);
    hEventByDateCalib->SetBarWidth(0.28);
    hEventByDateCalib->SetBarOffset(0.66);
    hEventByDateCalib->SetFillColor(42);

    int total_phys_events = 0, total_test_events = 0, total_calib_events = 0;
    cout << endl << "|shift              |type            |event_number|" << endl;
    cout << "|-------------------|----------------|------------|" << endl;
    // extract rows from the database query
    while (stmt->NextResultRow()) {
        TString str_record_date = stmt->GetString(0), str_shift_time = "";
        int shift_num = stmt->GetInt(1);
        TDatime record_date;
        switch (shift_num) {
            case 0: {
                record_date.Set(str_record_date + " 04:00:00");
                str_shift_time = " 00h-08h";
                break;
            }
            case 1: {
                record_date.Set(str_record_date + " 12:00:00");
                str_shift_time = " 08h-16h";
                break;
            }
            case 2: {
                record_date.Set(str_record_date + " 20:00:00");
                str_shift_time = " 16h-24h";
                break;
            }
        }
        TString type_text = stmt->GetString(2);
        int event_count = stmt->GetInt(3);
        TString str_event_count = FormatWithCommas(event_count * 1000);
        for (int i = str_event_count.Length(); i < 12; i++)
            str_event_count += " ";

        cout << "|" << str_record_date << str_shift_time << " |" << type_text.Data();
        if (type_text == "Physics Run") {
            hEventByDatePhys->Fill(record_date.Convert(), event_count * 1000);
            cout << "    " << " |" << str_event_count << "|" << endl;
            total_phys_events += event_count * 1000;
        } else if (type_text == "Test Run") {
            hEventByDateTest->Fill(record_date.Convert(), event_count * 1000);
            cout << "       " << " |" << str_event_count << "|" << endl;
            total_test_events += event_count * 1000;
        } else if (type_text == "Calibration Run") {
            hEventByDateCalib->Fill(record_date.Convert(), event_count * 1000);
            cout << " |" << str_event_count << "|" << endl;
            total_calib_events += event_count * 1000;
        } else {
            for (int i = type_text.Length(); i < 15; i++)
                cout << " ";
            cout << " |" << str_event_count << "|" << endl;
            cout << "WARNING: Incorrect type of the record with event number is \"" << type_text << "\"" << endl;
        }
    }
    cout << endl
         << "Total physics events - " << FormatWithCommas(total_phys_events) << endl
         << "Total test events - " << FormatWithCommas(total_test_events) << endl
         << "Total calibration events - " << FormatWithCommas(total_calib_events) << endl;

    TString hist_options = "bar2,hist,text";   // 'text' option for vertical captions with event count
    TH1* h1 = hEventByDatePhys->DrawCopy(hist_options);
    TH1* h2 = hEventByDateTest->DrawCopy(hist_options + ",same");
    TH1* h3 = hEventByDateCalib->DrawCopy(hist_options + ",same");

    TLegend* legend = new TLegend(0.13, 0.79, 0.30, 0.88);
    legend->AddEntry(h1, "Physics Events", "f");
    legend->AddEntry(h2, "Test Events", "f");
    legend->AddEntry(h3, "Calibration Events", "f");
    legend->Draw();

    if (save_hist != "")
        c1->SaveAs(save_hist.Data());
}
