#include "BmnMQSource.h"

#include <signal.h>
// BmnRoot
#include "BmnMath.h"

using namespace std;

namespace
{
int ReceivedSignal = 0;

void signal_handler(int sig)
{
    ReceivedSignal = sig;
    //    fKeepWorking = false;
    printf("Received signal %d Exiting\n", sig);
}
}   // namespace

BmnMQSource::BmnMQSource(std::string addr, Bool_t toFile)
    : _tBuf(nullptr)
    , _addrString(addr)
    , fFirstEvent(kTRUE)
    , fToFile(toFile)
    , fRunId(0)
    , fPeriodId(7)
    , iEventNumber(0)
    , fEventHeader(nullptr)
{}

BmnMQSource::~BmnMQSource() {}

Bool_t BmnMQSource::Init()
{
    printf("BmnMQSource::Init()\n");
    if (!InitZMQ())
        return kFALSE;
    zmq_msg_init(&_msg);
    Int_t frame_size = 0;
    do {
        if (ReceivedSignal)
            return kFALSE;
        gSystem->ProcessEvents();
        frame_size = zmq_msg_recv(&_msg, _decoSocket, ZMQ_DONTWAIT);
        if (frame_size == -1) {
            //                printf("Receive error # %d #%s\n", errno, zmq_strerror(errno));
            switch (errno) {
                case EAGAIN:
                    usleep(TimeDelta);
                    break;
                case EINTR:
                    printf("EINTR\n");
                    printf("Exit!\n");
                    return kFALSE;
                    break;
                case EFAULT:
                    fprintf(stderr, "Receive error № %d #%s\n", errno, zmq_strerror(errno));
                    return kFALSE;
                    break;
                default:
                    break;
            }
            gSystem->ProcessEvents();
        }
    } while ((frame_size <= 0));
    //        printf("Received frame_size = %d\n", frame_size);
    BmnParts* parts = nullptr;
    _tBuf->Reset();
    _tBuf->SetBuffer(zmq_msg_data(&_msg), zmq_msg_size(&_msg));
    parts = static_cast<BmnParts*>(_tBuf->ReadObjectAny(BmnParts::Class()));
    cout << "TCA len : " << parts->GetArrays().size() << endl;
    cout << "Obj len : " << parts->GetObjects().size() << endl;

    FairRootManager* ioman = FairRootManager::Instance();
    for (TClonesArray* ar : parts->GetArrays()) {
        TClonesArray* newAr = new TClonesArray(ar->GetClass());
        newAr->SetName(ar->GetName());
        //            printf("Register %30s  %30s\n", ar->GetName(), newAr->GetName());
        ioman->RegisterInputObject(newAr->GetName(), newAr);
        //            ioman->Register(newAr->GetName(), ".", newAr, fToFile);
        fArrVec.push_back(newAr);
    }
    for (TNamed* tn : parts->GetObjects()) {
        //            printf("Register %20s\n", tn->GetName());
        //            printf("ClassName %20s  Class_Name %20s  GetName %20s\n", tn->ClassName(), tn->Class_Name(),
        //            tn->GetName());
        // TClass * cl = tn->Class();
        TNamed* ob = static_cast<TNamed*>(tn->Clone());   // cl->New());
        ioman->RegisterInputObject(tn->GetName(), ob);
        fNamVec.push_back(ob);
    }
    delete parts;
    _tBuf->DetachBuffer();
    zmq_msg_close(&_msg);
    return kTRUE;
}

void BmnMQSource::Close()
{
    for (TClonesArray*& ar : fArrVec)
        if (ar) {
            ar->Delete();
            delete ar;
        }
    for (TNamed*& ar : fNamVec)
        if (ar)
            delete ar;
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    delete _tBuf;
}

Int_t BmnMQSource::ReadEvent(UInt_t i)
{
    //    printf("ReadEvent(%4u)\n", i);
    gSystem->IgnoreInterrupt();
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    zmq_msg_init(&_msg);
    Int_t frame_size = 0;
    do {
        if (ReceivedSignal)
            return 1;
        gSystem->ProcessEvents();
        frame_size = zmq_msg_recv(&_msg, _decoSocket, ZMQ_DONTWAIT);
        if (frame_size == -1) {
            //            fprintf(stderr, "Receive error zmq# %d #%s\n", zmq_errno(), zmq_strerror(zmq_errno()));
            Int_t err_num = zmq_errno();
            switch (err_num) {
                case EAGAIN:
                    usleep(TimeDelta);
                    return 2;
                    break;
                case EINTR:
                    printf("EINTR\n");
                    printf("Exit!\n");
                    return 1;
                    break;
                case EFAULT:
                    fprintf(stderr, "Receive error # %d #%s\n", err_num, zmq_strerror(err_num));
                    return 1;
                    break;
                default:
                    fprintf(stderr, "Receive error zmq# %d #%s\n", err_num, zmq_strerror(err_num));
                    break;
            }
        }
    } while (frame_size <= 0);

    _tBuf->Reset();
    _tBuf->SetBuffer(zmq_msg_data(&_msg), zmq_msg_size(&_msg));
    BmnParts* parts = static_cast<BmnParts*>(_tBuf->ReadObject(BmnParts::Class()));
    if (parts) {
        //        cout << "TCA len : " << parts->GetArrays().size() << endl;
        //        cout << "Obj len : " << parts->GetObjects().size() << endl;
        //        cout << "Accepted: " << fDigiArrays->header->GetTrigInfo()->GetTrigAccepted() << endl;

        // move result TClonesArray to registered TClonesArray
        for (UInt_t iAr = 0; iAr < fArrVec.size(); iAr++) {
            fArrVec[iAr]->Delete();
            fArrVec[iAr]->AbsorbObjects(parts->GetArrays()[iAr]);
            //            cout << "Count of " << fArrVec[iAr]->GetName() << " digits: " <<
            //            fArrVec[iAr]->GetEntriesFast() << endl;
        }
        for (UInt_t iAr = 0; iAr < fNamVec.size(); iAr++) {
            //            printf("ClassName %20s  Class_Name %20s  GetName %20s\n", fNamVec[iAr]->ClassName(),
            //            fNamVec[iAr]->Class_Name(), fNamVec[iAr]->GetName());
            if (!strcmp(fNamVec[iAr]->ClassName(), "BmnEventHeader")) {
                *static_cast<BmnEventHeader*>(fNamVec[iAr]) = *static_cast<BmnEventHeader*>(parts->GetObjects()[iAr]);
                fEventHeader = static_cast<FairEventHeader*>(fNamVec[iAr]);
                LOG(debug) << "Digi EventID: " << static_cast<BmnEventHeader*>(fNamVec[iAr])->GetEventId() << endl;
            }
            if (!strcmp(fNamVec[iAr]->ClassName(), "DstEventHeader")) {
                // static_cast<DstEventHeader*>(fNamVec[iAr])
                //     ->CopyFrom(static_cast<DstEventHeader*>(parts->GetObjects()[iAr]));
                *static_cast<DstEventHeader*>(fNamVec[iAr]) = *static_cast<DstEventHeader*>(parts->GetObjects()[iAr]);
                fEventHeader = static_cast<FairEventHeader*>(fNamVec[iAr]);
                //                cout << "Object " << fNamVec[iAr]->GetName() << endl;
                LOG(debug) << "DST  EventID: " << static_cast<DstEventHeader*>(fNamVec[iAr])->GetEventId() << endl;
            }
            if (!strcmp(fNamVec[iAr]->ClassName(), "CbmVertex")) {
                static_cast<CbmVertex*>(fNamVec[iAr])->CopyFrom(static_cast<CbmVertex*>(parts->GetObjects()[iAr]));
                //                            cout << "VZ " << static_cast<CbmVertex*> (fNamVec[iAr])->GetZ() << endl;
            }
        }
        delete parts;
    } else
        fprintf(stderr, "Failed to deserialize the incoming object!\n");
    _tBuf->DetachBuffer();
    zmq_msg_close(&_msg);
    return 0;
}

void BmnMQSource::FillEventHeader(FairEventHeader* feh)
{
    if (fEventHeader) {
        feh->SetEventTime(fEventHeader->GetEventTime());
        feh->SetRunId(fEventHeader->GetRunId());
        feh->SetMCEntryNumber(fEventHeader->GetMCEntryNumber());
    }
    LOGF(debug, "feh run id = %d   clname %s\n", feh->GetRunId(), feh->ClassName());
    return;
}

Bool_t BmnMQSource::InitZMQ()
{
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_SUB);
    uint32_t rcvBuf = 0;
    size_t vl = sizeof(rcvBuf);
    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
    printf("ZMQ_RCVBUF %u\n", rcvBuf);

    if (zmq_setsockopt(_decoSocket, ZMQ_SUBSCRIBE, NULL, 0) == -1) {
        // DBGERR("zmq subscribe")
        fprintf(stderr, "Error subscribing to ZMQ socket: %s\n", strerror(errno));
        return kFALSE;
    }
    if (zmq_connect(_decoSocket, _addrString.c_str()) != 0) {
        // DBGERR("zmq connect")
        fprintf(stderr, "Error connecting to ZMQ socket: %s\n", strerror(errno));
        return kFALSE;
    } else {
        printf("Listening to %s\n", _addrString.c_str());
    }
    _tBuf = new TBufferFile(TBuffer::kRead);
    return kTRUE;
}
