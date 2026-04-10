#include "BmnFileProp.h"

BmnFileProp::BmnFileProp(uint32_t runId, string runIndex, uint32_t eventOrder, uint32_t fileId)
    : TObject()
    , run_id(runId)
    , run_index(runIndex)
    , event_order(eventOrder)
    , file_id(fileId)
{}
