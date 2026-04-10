#pragma once

#include <string>
// ROOT
#include <TObject.h>

using std::string;

class BmnFileProp : public TObject
{
  public:
    BmnFileProp(uint32_t runId = 0, string runIndex = "", uint32_t eventOrder = 0, uint32_t fileId = 0);

    virtual ~BmnFileProp() = default;

    void SetRunId(uint32_t v) { run_id = v; }
    void SetRunIndex(string index) { run_index = index; }
    void SetEventOrder(uint32_t order) { event_order = order; }
    void SetFileId(uint32_t fid) { file_id = fid; }

    uint32_t GetRunId() const { return run_id; }
    string GetRunIndex() const { return run_index; }
    uint32_t GetEventOrder() const { return event_order; }
    uint32_t GetFileId() const { return file_id; }

  protected:
    uint32_t run_id;
    string run_index;
    uint32_t event_order;
    uint32_t file_id;

    ClassDef(BmnFileProp, 1);
};
