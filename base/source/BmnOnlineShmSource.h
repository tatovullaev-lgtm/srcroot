#ifndef BMN_ONLINE_SHM_SOURCE_H
#define BMN_ONLINE_SHM_SOURCE_H

#include <FairOnlineSource.h>
#include <FairSource.h>
#include <TClonesArray.h>
#include <TTree.h>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <string>

namespace bIpc = boost::interprocess;

class BmnOnlineShmSource : public FairOnlineSource {
   public:
    BmnOnlineShmSource();
    virtual ~BmnOnlineShmSource();

    virtual Bool_t Init();
    virtual Int_t ReadEvent(UInt_t = 0);
    virtual void Close();

    BmnOnlineShmSource(const BmnOnlineShmSource &) = delete;
    BmnOnlineShmSource &operator=(const BmnOnlineShmSource &) = delete;

   private:
    Bool_t fIsRegisterBranches;

    std::string fShmName;
    std::string fWriteSemName;
    std::string fReadSemName;
    std::string fMsgName;

    std::unique_ptr<bIpc::managed_shared_memory> fShmSegment;
    std::unique_ptr<bIpc::named_semaphore> fWriteSem;
    std::unique_ptr<bIpc::named_semaphore> fReadSem;

    std::unordered_map<std::string, std::unique_ptr<TClonesArray>> fRegisteredBranches;

    void RegisterBranches(std::unique_ptr<TTree> &inputTree);
    void UploadData(std::unique_ptr<TTree> &inputTree);

    ClassDef(BmnOnlineShmSource, 0);
};

#endif  // BMN_ONLINE_SHM_SOURCE_H
