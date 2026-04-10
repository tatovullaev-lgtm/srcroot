// -------------------------------------------------------------------------
// -----                BmnFieldParser header file                     -----
// -----               Created 28/06/22 by M.Mamaev, S.Merts           -----
// -------------------------------------------------------------------------

#ifndef BMN_FIELD_PARSER_H_
#define BMN_FIELD_PARSER_H_

// #include <string>
// #include <vector>
// #include <algorithm>
// #include <stdexcept>
// #include <limits>
// #include <cassert>

#include "BmnFieldPoint.h"
#include "TString.h"
#include <vector>
#include <map>
#include <iostream>    // std::cout
#include <sstream>     // std::istringstream
#include <fstream>

using namespace std;

class BmnFieldParser {
public:
    // The method reads the field points from the data
    //vector<BmnFieldPoint> ParseData(TString file_name);
    vector<BmnFieldPoint> ParseData(TString file_name, Bool_t shift = kTRUE);
    map<pair<Int_t, Int_t>, BmnFieldPoint> ParseDataMap(TString file_name, Bool_t shift = kTRUE);

    BmnFieldParser() {}
    ~BmnFieldParser() {}



    // The method sorts the large heap of field points into smaller ones so that
    // each heap will be unique on this coordinate.
    // { x1, x1, x1, x2, x2, x2, ... } ->
    // {x1, x2, ... }
    // {x1, x2, ... }
    // {x1, x2, ... }
    // template <typename T>
    // static vector<vector<BmnFieldPoint>> SortPoints(const vector<BmnFieldPoint>& points, T coordinate) {
    //     vector<vector<BmnFieldPoint>> sorted_points;
    //     if (points.empty())
    //         throw runtime_error("Field Points are empty");
    //     auto copy_field_points = move(sorted_points);
    //     vector<vector<double>> sort_coordinates;
    //     sorted_points.emplace_back();
    //     sort_coordinates.emplace_back();

    //     for (auto& point : points) {
    //         auto x = coordinate(point);
    //         vector<int> number_of_inclusions;
    //         for (auto data : sorted_points) {
    //             auto n_ins = count_if(data.begin(), data.end(), [x, coordinate](BmnFieldPoint p) {
    //                 return fabs(coordinate(p) - x) < numeric_limits<float>::min();
    //                 });
    //             number_of_inclusions.push_back(n_ins);
    //         }
    //         bool is_filled = false;
    //         for (size_t i = 0; i < number_of_inclusions.size(); ++i) {
    //             if (number_of_inclusions.at(i) == 0) {
    //                 sorted_points.at(i).push_back(point);
    //                 is_filled = true;
    //             }
    //         }
    //         if (!is_filled) {
    //             sorted_points.emplace_back();
    //             sorted_points.back().push_back(point);
    //         }
    //     }
    //     for (auto& ds : sorted_points) {
    //         sort(ds.begin(), ds.end(), [coordinate](BmnFieldPoint a, BmnFieldPoint b) {
    //             return coordinate(a) < coordinate(b);
    //             });
    //     }
    //     return sorted_points;
    // }


};

#endif // BMN_FIELD_PARSER_H_
