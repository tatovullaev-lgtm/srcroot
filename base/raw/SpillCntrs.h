#ifndef SPILLCNTRS_H
#define SPILLCNTRS_H

#include <array>
#include <vector>

template<UInt_t counters_count = 16>
struct SpillCntrs : public TObject
{
    inline const static size_t counters_cnt = counters_count;
    std::array<ULong64_t, counters_cnt> cntr_after_err = {};
    std::array<ULong64_t, counters_cnt> cntr_befor_err = {};
    std::vector<std::pair<std::string, std::array<ULong64_t, counters_cnt>>> cntr_input;
    std::vector<std::pair<std::string, std::array<ULong64_t, counters_cnt>>> cntr_input_after_red;

    void ResizeTo(const SpillCntrs& example)
    {
        cntr_input.resize(example.cntr_input.size());
        for (size_t i = 0; i < example.cntr_input.size(); i++)
            cntr_input[i].first = example.cntr_input[i].first;
        cntr_input_after_red.resize(example.cntr_input_after_red.size());
        for (size_t i = 0; i < example.cntr_input_after_red.size(); i++)
            cntr_input_after_red[i].first = example.cntr_input_after_red[i].first;
    }

    ClassDef(SpillCntrs, 1)
};

#endif /* SPILLCNTRS_H */
