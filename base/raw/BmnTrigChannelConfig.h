/**
 * Class BmnTrigChannelConfig
 * Trigger channel configuration (count reduction, time protection)
 */

#ifndef TRIGCHANNELCONFIG_H
#define TRIGCHANNELCONFIG_H

#include "TObject.h"

struct BmnTrigChannelConfig : public TObject
{
    BmnTrigChannelConfig()
        : after_protect_en{}
        , after_protect_time{}
        , before_protect_en{}
        , before_protect_time{}
        , delay{}
        , reduction_en{}
        , reduction_factor{}
    {}

    bool after_protect_en;
    uint32_t after_protect_time;
    bool before_protect_en;
    uint32_t before_protect_time;
    uint32_t delay;
    bool reduction_en;
    uint32_t reduction_factor;

    void print()
    {
        printf(" after_prot en %4u\n", after_protect_en);
        printf(" after_prot  t %4u\n", after_protect_time);
        printf("before_prot en %4u\n", before_protect_en);
        printf("before_prot  t %4u\n", before_protect_time);
        printf("         delay %4u\n", delay);
        printf("  reduction en %4u\n", reduction_en);
        printf("reduct  factor %4u\n", reduction_factor);
    }

    ClassDef(BmnTrigChannelConfig, 1)
};

#endif /* TRIGCHANNELCONFIG_H */
