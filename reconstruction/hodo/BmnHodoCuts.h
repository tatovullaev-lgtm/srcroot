#ifndef BMN_HODO_CUTS_H
#define BMN_HODO_CUTS_H

#include "BmnHodoDigi.h"
#include "BmnHodoDigit.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct BmnHodoCuts
{

    // --- internal helper ---
    struct Range
    {
        double min = -1e9, max = 1e9;
        bool hasMin = false, hasMax = false;
        inline bool Pass(double v) const
        {
            if (hasMin && v <= min)
                return false;
            if (hasMax && v >= max)
                return false;
            return true;
        }
    };

    // --- physics fields ---
    Range CrosstalkOverIntegral;   // fCrosstalk / fIntegral
    Range TimeMax;                 // fTimeMax

    // --- methods ---
    inline bool PassSim(const BmnHodoDigit* d) const { return true; }

    inline bool PassExp(const BmnHodoDigi* d) const
    {
        if (d->fIntegral != 0 && !CrosstalkOverIntegral.Pass(d->fCrosstalk / d->fIntegral))
            return false;
        if (!TimeMax.Pass(d->fTimeMax))
            return false;
        return true;
    }

    static BmnHodoCuts Load(const std::string& file, bool isExp)
    {
        std::ifstream f(file);
        if (!f.is_open()) {
            std::cerr << "[BmnHodoCuts] Cannot open config: " << file << std::endl;
            return {};
        }

        json j;
        try {
            f >> j;
        } catch (const std::exception& e) {
            std::cerr << "[BmnHodoCuts] JSON parse error in " << file << ": " << e.what() << std::endl;
            return {};
        }

        const json& sec = isExp ? j["experiment"] : j["simulation"];
        BmnHodoCuts c;

        auto readRange = [](Range& r, const json& js, const char* key) {
            const std::string kmin = std::string(key) + "_min";
            const std::string kmax = std::string(key) + "_max";
            if (js.contains(kmin)) {
                r.min = js[kmin].get<double>();
                r.hasMin = true;
            }
            if (js.contains(kmax)) {
                r.max = js[kmax].get<double>();
                r.hasMax = true;
            }
        };

        if (isExp) {
            readRange(c.CrosstalkOverIntegral, sec, "fCrosstalk_over_fIntegral");
            readRange(c.TimeMax, sec, "fTimeMax");
        }

        std::cout << "[BmnHodoCuts] Loaded cuts for " << (isExp ? "experiment" : "simulation") << " from " << file
                  << std::endl;

        return c;
    }
};

#endif   // BMN_HODO_CUTS_H
