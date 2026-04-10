#ifndef BMN_ONLINE_SOURCE_H
#define BMN_ONLINE_SOURCE_H

#include <FairOnlineSource.h>
#include <TClonesArray.h>
#include <TMessage.h>
#include <TString.h>
#include <TTree.h>
#include <zmq.h>

class _CustomTMessage : public TMessage {
   public:
    _CustomTMessage(void* buf, Int_t len) : TMessage(buf, len) {
        ResetBit(kIsOwner);
    }
};

class BmnOnlineSource : public FairOnlineSource {
   public:
    BmnOnlineSource(TString endpoint = "tcp://localhost:5555");
    virtual ~BmnOnlineSource();

    virtual Bool_t Init();
    virtual Int_t ReadEvent(UInt_t = 0);
    virtual void Close();

    BmnOnlineSource(const BmnOnlineSource&) = delete;
    BmnOnlineSource& operator=(const BmnOnlineSource&) = delete;

    template <typename T>
    static void DeserializeZmqMessage(zmq_msg_t* zmq_message,
                                      std::unique_ptr<T>& output) {
        _CustomTMessage _message(zmq_msg_data(zmq_message),
                                 zmq_msg_size(zmq_message));

        output.reset(
            static_cast<T*>(_message.ReadObjectAny(_message.GetClass())));
    };

   protected:
    TString fEndpoint;

    std::shared_ptr<void> fZmqContext;
    std::shared_ptr<void> fZmqSubSocket;

    std::unordered_map<std::string, std::unique_ptr<TClonesArray>>
        fRegisteredBranches;

    void RegisterBranches(std::unique_ptr<TTree>& tree);
    void UploadDataToRegisteredBranches(std::unique_ptr<TTree>& tree);

   private:
    Bool_t fAreBranchesRegistered;

    ClassDef(BmnOnlineSource, 0)
};

#endif  // BMN_ONLINE_SOURCE_H
