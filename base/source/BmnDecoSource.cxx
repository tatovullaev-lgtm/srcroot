#include "BmnDecoSource.h"

#include <signal.h>

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

BmnDecoSource::BmnDecoSource(TString addr, Int_t periodId)
    : _addrString(addr)
    ,
    //_addrString(Form("tcp://%s:%d", addr.Data(), 5555)),
    fDigiArrays(nullptr)
    , fFirstEvent(kTRUE)
    , fRunId(0)
    , fPeriodId(periodId)
    , iEventNumber(0)
    , iT0BranchIndex(-1)
    ,
    // fEventHeader(nullptr)
    fGemDigits(nullptr)
    , fVspDigits(nullptr)
    , fSilDigits(nullptr)
    , fSiBTDigits(nullptr)
    , fCscDigits(nullptr)
    , fTof400Digits(nullptr)
    , fTof700Digits(nullptr)
    , fInitDone{false}
{}

BmnDecoSource::~BmnDecoSource() {}

Bool_t BmnDecoSource::Init()
{
    printf("BmnDecoSource::Init()\n");
    if (fInitDone) {
        printf("Twice\n");
        return true;
    }
    gSystem->IgnoreInterrupt();
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    FairRootManager* ioman = FairRootManager::Instance();
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_SUB);

    //    int32_t rcvBufLen = 8 * 1024 * 1024;
    //    int32_t ret = 0;
    //    if (ret=zmq_setsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBufLen, sizeof(rcvBufLen)) == -1)
    //        fprintf(stderr, "Error in zmq_setsockopt of ZMQ_RCVBUF: %s\n", strerror(errno));
    //
    //    int32_t rcvBuf = 0;
    //    size_t vl = sizeof(rcvBuf);
    //    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
    //        fprintf(stderr, "Error getting ZMQ socket RCVBUF size: %s\n", strerror(errno));
    //    printf("ZMQ_RCVBUF %d\n", rcvBuf);

    int32_t hwm = 5;
    if (zmq_setsockopt(_decoSocket, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == -1)
        fprintf(stderr, "Error in zmq_setsockopt of ZMQ_RCVHWM: %s\n", strerror(errno));
    size_t vl = sizeof(hwm);
    if (zmq_getsockopt(_decoSocket, ZMQ_RCVHWM, &hwm, &vl) == -1)
        fprintf(stderr, "Error getting ZMQ socket RCVHWM size: %s\n", strerror(errno));
    printf("ZMQ_RCV HWM %d\n", hwm);

    if (zmq_setsockopt(_decoSocket, ZMQ_SUBSCRIBE, nullptr, 0) == -1) {
        // DBGERR("zmq subscribe")
        fprintf(stderr, "Error subscribing to ZMQ socket: %s\n", strerror(errno));
        return kFALSE;
    }
    if (zmq_connect(_decoSocket, _addrString.Data()) != 0) {
        // DBGERR("zmq connect")
        fprintf(stderr, "Error connecting to ZMQ socket: %s\n", strerror(errno));
        return kFALSE;
    } else {
        printf("connected to %s\n", _addrString.Data());
    }

    _tBuf = new TBufferFile(TBuffer::kRead);
    zmq_msg_init(&_msg);
    Int_t frame_size = -1;
    do {
        if (ReceivedSignal)
            return false;
        gSystem->ProcessEvents();
        frame_size = zmq_msg_recv(&_msg, _decoSocket, ZMQ_DONTWAIT);
        if (frame_size == -1) {
            //            printf("Receive error # %d #%s\n", errno, zmq_strerror(errno));
            switch (errno) {
                case EAGAIN:
                    usleep(TimeDelta);
                    break;
                case EINTR:
                    printf("EINTR\n");
                    printf("Exit!\n");
                    //                    return kFALSE;
                    break;
                case EFAULT:
                    fprintf(stderr, "Receive error № %d #%s\n", errno, zmq_strerror(errno));
                    //                    return kFALSE;
                    break;
                default:
                    break;
            }
            gSystem->ProcessEvents();
        }
    } while ((frame_size <= 0));

    if (fDigiArrays) {
        fDigiArrays->Clear();
        delete fDigiArrays;
        fDigiArrays = nullptr;
    }
    _tBuf->Reset();
    _tBuf->SetBuffer(zmq_msg_data(&_msg), zmq_msg_size(&_msg));
    fDigiArrays = (DigiArrays*)(_tBuf->ReadObject(DigiArrays::Class()));
    _tBuf->DetachBuffer();
    zmq_msg_close(&_msg);

    for (TClonesArray* ar : *(fDigiArrays->trigAr)) {
        TClonesArray* newAr = new TClonesArray(ar->GetClass());
        newAr->SetName(ar->GetName());
        ioman->RegisterInputObject(newAr->GetName(), newAr);
        fT0Digits.push_back(newAr);
    }

    fEventHeader = new BmnEventHeader();
    ioman->RegisterInputObject("BmnEventHeader.", fEventHeader);

    fGemDigits = new TClonesArray("BmnGemStripDigit");   //::Class());
    ioman->RegisterInputObject("GEM", fGemDigits);

    fVspDigits = new TClonesArray("BmnVSPDigit");
    ioman->RegisterInputObject("VSP", fVspDigits);

    fSilDigits = new TClonesArray("BmnSiliconDigit");   //::Class());
    ioman->RegisterInputObject("SILICON", fSilDigits);

    fSiBTDigits = new TClonesArray("BmnSiBTDigit");
    ioman->RegisterInputObject("SiBT", fSiBTDigits);

    fCscDigits = new TClonesArray("BmnCSCDigit");   //::Class());
    ioman->RegisterInputObject("CSC", fCscDigits);

    fTof400Digits = new TClonesArray("BmnTof1Digit");   //::Class());
    ioman->RegisterInputObject("TOF400", fTof400Digits);

    fTof700Digits = new TClonesArray("BmnTof701Digit");   //::Class());
    ioman->RegisterInputObject("TOF701", fTof700Digits);
    fInitDone = true;
    return true;
}

void BmnDecoSource::Close()
{
    printf("BmnDecoSource::Close\n");
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    if (fEventHeader) {
        //        fEventHeader->Delete();
        delete fEventHeader;
    }
    if (fGemDigits) {
        fGemDigits->Delete();
        delete fGemDigits;
    }
    if (fVspDigits) {
        fVspDigits->Delete();
        delete fVspDigits;
    }
    if (fSilDigits) {
        fSilDigits->Delete();
        delete fSilDigits;
    }
    if (fSiBTDigits) {
        fSiBTDigits->Delete();
        delete fSiBTDigits;
    }
    if (fTof400Digits) {
        fTof400Digits->Delete();
        delete fTof400Digits;
    }
    if (fTof700Digits) {
        fTof700Digits->Delete();
        delete fTof700Digits;
    }
    for (TClonesArray* ar : fT0Digits)
        ar->Delete();

    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = nullptr;
    delete _tBuf;
}

Int_t BmnDecoSource::ReadEvent(UInt_t i)
{
    zmq_msg_init(&_msg);
    Int_t frame_size = zmq_msg_recv(&_msg, _decoSocket, 0);   // ZMQ_DONTWAIT
    if (frame_size == -1) {
        fprintf(stderr, "Receive error № %d #%s\n", errno, zmq_strerror(errno));
    } else {
        //        printf("Received frame_size = %d\n", frame_size);
        if (fDigiArrays) {
            fDigiArrays->Clear();
            delete fDigiArrays;
            fDigiArrays = nullptr;
        }
        _tBuf->Reset();
        _tBuf->SetBuffer(zmq_msg_data(&_msg), zmq_msg_size(&_msg));
        fDigiArrays = (DigiArrays*)(_tBuf->ReadObject(DigiArrays::Class()));

        // move result TClonesArray to registered TClonesArray
        for (TClonesArray* ar : fT0Digits)
            ar->Delete();
        fGemDigits->Delete();
        fVspDigits->Delete();
        fSilDigits->Delete();
        fSiBTDigits->Delete();
        fCscDigits->Delete();
        fTof400Digits->Delete();
        fTof700Digits->Delete();
        BmnEventHeader* header = fDigiArrays->header;
        fEventHeader->SetRunId(header->GetRunId());
        fEventHeader->SetEventId(header->GetEventId());
        fEventHeader->SetEventTimeTS(header->GetEventTimeTS());
        fEventHeader->SetEventTime(header->GetEventTime());
        fEventHeader->SetEventType(header->GetEventType());
        fEventHeader->SetTripWord(kFALSE);
        fEventHeader->SetTrigInfo(header->GetTrigInfo());
        fEventHeader->SetTimeShift(header->GetTimeShift());
        fEventHeader->SetInputSignalsAR(header->GetInputSignalsAR());
        fEventHeader->SetInputSignalsBR(header->GetInputSignalsBR());
        //        fEventHeader->SetInputSignalsVector(move(header->GetInputSignalsVector()));
        fEventHeader->SetStartSignalInfo(header->GetStartSignalTime(), header->GetStartSignalWidth());
        for (size_t iTrig = 0; iTrig < fDigiArrays->trigAr->size(); iTrig++)
            fT0Digits[iTrig]->AbsorbObjects((*fDigiArrays->trigAr)[iTrig]);
        fGemDigits->AbsorbObjects(fDigiArrays->gem);
        fSilDigits->AbsorbObjects(fDigiArrays->silicon);
        fSiBTDigits->AbsorbObjects(fDigiArrays->sibt);
        fCscDigits->AbsorbObjects(fDigiArrays->csc);
        fVspDigits->AbsorbObjects(fDigiArrays->vsp);
        //        if (iT0BranchIndex > -1)
        fTof400Digits->AbsorbObjects(fDigiArrays->tof400);
        fTof700Digits->AbsorbObjects(fDigiArrays->tof701);
        //        fTof700Digits->AbsorbObjects(fDigiArrays->tof700);
        _tBuf->DetachBuffer();
        zmq_msg_close(&_msg);
    }
    return 0;
}
