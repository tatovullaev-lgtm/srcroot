#ifndef BMN_ONLINE_TASK_H
#define BMN_ONLINE_TASK_H

#include <FairField.h>
#include <FairTask.h>

#include <TTree.h>

/**
 * @brief BmnTask
 *
 */
class BmnTask : public FairTask {
  public:
    /** Default constructor **/
    BmnTask();

    /** Standard constructor
     *@param name Name of task
     *@param iVerbose Verbosity level
     **/
    BmnTask(const char *name, Int_t iVerbose = 1);

    /**
     * @brief Destroy the BmnTask object
     *
     */
    virtual ~BmnTask();

    /**
     * @brief Write task resul to tree
     *
     * @param resultTree
     */
    virtual void OnlineWrite(const std::unique_ptr<TTree> &resultTree) {}

    BmnTask(const BmnTask &) = delete;
    BmnTask &operator=(const BmnTask &) = delete;

  ClassDef(BmnTask, 1);
};

#endif  // BMN_ONLINE_TASK_H
