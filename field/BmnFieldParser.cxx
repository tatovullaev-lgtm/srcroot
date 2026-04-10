
// -------------------------------------------------------------------------
// -----                BmnFieldParser source file                     -----
// -----               Created 28/06/22 by M.Mamaev, S.Merts           -----
// -------------------------------------------------------------------------


#include "BmnFieldParser.h"

vector<BmnFieldPoint> BmnFieldParser::ParseData(TString file_name, Bool_t doShift) {
    cout << "Reading file: " << file_name << endl;
    Int_t idxStart = file_name.Index("file_") + 5;
    Int_t idxEnd = file_name.Index("_CW.");
    TString shift = file_name(idxStart, idxEnd - idxStart);
    TString fileIdx = shift(0, shift.Index("_"));
    Int_t yShift = (doShift) ? TString(shift(shift.Index("_") + 1, shift.Length() - shift.Index("_") - 1)).Atoi() : 0;
    cout << "File index: " << fileIdx << " Y shift = " << yShift << endl;
    ifstream in_file(file_name);
    if (!in_file.is_open())
        throw runtime_error("Unable to read file " + file_name);
    string line;
    size_t line_number = 0;
    vector<BmnFieldPoint> field_points;
    while (getline(in_file, line)) {
        istringstream stream(line);
        // Skipping the title of the table
        if (line_number == 0) {
            line_number++;
            continue;
        }
        Double_t enc_pos = 0;
        Double_t x = 0, y = 0, z = 0;
        Double_t mon = 0;
        Double_t bx = 0, by = 0, bz = 0;
        stream >> enc_pos >> x >> y >> z >> mon >> bx >> by >> bz;
        field_points.push_back(BmnFieldPoint(x, y + yShift, z, bx, by, bz));
        line_number++;
    }
    cout << field_points.size() << " points are read" << endl;
    return field_points;
}

map<pair<Int_t, Int_t>, BmnFieldPoint> BmnFieldParser::ParseDataMap(TString file_name, Bool_t doShift) {
    cout << "Reading file: " << file_name << endl;
    Int_t idxStart = file_name.Index("file_") + 5;
    Int_t idxEnd = file_name.Index("_CW.");
    TString shift = file_name(idxStart, idxEnd - idxStart);
    TString fileIdx = shift(0, shift.Index("_"));
    Int_t yShift = (doShift) ? TString(shift(shift.Index("_") + 1, shift.Length() - shift.Index("_") - 1)).Atoi() : 0;
    cout << "File index: " << fileIdx << " Y shift = " << yShift << endl;
    ifstream in_file(file_name);
    if (!in_file.is_open())
        throw runtime_error("Unable to read file " + file_name);
    string line;
    size_t line_number = 0;
    map<pair<Int_t, Int_t>, BmnFieldPoint> field_points;
    while (getline(in_file, line)) {
        istringstream stream(line);
        // Skipping the title of the table
        if (line_number == 0) {
            line_number++;
            continue;
        }
        Double_t enc_pos = 0;
        Double_t x = 0, y = 0, z = 0;
        Double_t mon = 0;
        Double_t bx = 0, by = 0, bz = 0;
        stream >> enc_pos >> x >> y >> z >> mon >> bx >> by >> bz;
        if (Int_t(y + yShift) == 3075) cout << Int_t(x) << " " << Int_t(y + yShift) << endl;
        field_points.insert(pair<pair<Int_t, Int_t>, BmnFieldPoint>(pair<Int_t, Int_t>(Int_t(x), Int_t(y + yShift)), BmnFieldPoint(x, y + yShift, z, bx, by, bz)));
        line_number++;
    }
    cout << field_points.size() << endl;
    cout << field_points.size() << " points are read" << endl;
    return field_points;
}

