#include "BmnProfilometerSource.h"

BmnProfilometerSource::BmnProfilometerSource(vector<string> addr, vector<string> boardIds)
: fFirstEvent(kTRUE),
  keepWorking(kTRUE),
  isReceiving(kTRUE),
  fRunId(0),
  fPeriodId(8),
  fVerbosity(0),
  iEventNumber(0),
  isSpillStart(true),
  fArr(nullptr),
  fArrTemp(nullptr),
  fAddrs(move(addr)),
  fBoardIds(move(boardIds))
{}

BmnProfilometerSource::~BmnProfilometerSource() {
    for (auto &it : fBoardsMap)
        delete it.second;
}

Bool_t BmnProfilometerSource::Init() {
    printf("BmnProfilometerSource::Init()\n");
    _ctx = zmq_ctx_new();
    //    _rawSocket = zmq_socket(_ctx, ZMQ_SUB);
    for (string &addr : fAddrs) {
        void * raw_socket = zmq_socket(_ctx, ZMQ_SUB);
        fRawSockets.push_back(raw_socket);
        if (zmq_connect(raw_socket, addr.c_str()) != 0) {
            //DBGERR("zmq connect")
            fprintf(stderr, "Error connecting to ZMQ socket: %s\n", strerror(errno));
            return kFALSE;
        } else {
            printf("connected to %s\n", addr.c_str());
            if (zmq_setsockopt(raw_socket, ZMQ_SUBSCRIBE, NULL, 0) == -1) {
                //DBGERR("zmq subscribe")
                fprintf(stderr, "Error subscribing to ZMQ socket: %s\n", strerror(errno));
                return kFALSE;
            }
        }
    }

    FairRootManager* ioman = FairRootManager::Instance();
    fArr = new TClonesArray(BmnADCDigit::Class());
    fArrTemp = new TClonesArray(BmnADCDigit::Class());
    ioman->RegisterInputObject("ADC192ASIC", fArr);
    fEventHead = new BmnEventHeader();
    ioman->RegisterInputObject("BmnEventHeader.", fEventHead);
    TPRegexp re("\\w+(\\d+)");
    for (string &id : fBoardIds) {
        ProfBoard* b = new ProfBoard();
        TString str(id);
        re.Substitute(str, "$1");
        b->board_id = str.Atoi();
        fBoardsMap.insert(make_pair(id, b));
    }
    return kTRUE;
}

void BmnProfilometerSource::Close() {
    for (void * rs : fRawSockets)
        zmq_close(rs);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
}

Int_t BmnProfilometerSource::ReadEvent(UInt_t i) {
    fArr->Delete();
    if (fArrTemp->GetEntriesFast() == 0) {
        if (fVerbosity)
            printf("Empty\n");
        ReceiveSpill();
        isSpillStart = true;
        iEventNumber = 0;

    }
    //    printf("entries before %d\n", fArrTemp->GetEntriesFast());
    fArr->AbsorbObjects(fArrTemp, 0, 0);
    ++iEventNumber;
    fEventHead->SetEventId(iEventNumber);
    fEventHead->SetSpillStart(isSpillStart);
    if (isSpillStart)
        isSpillStart = false;
    //    printf("entries after  %d\n", fArrTemp->GetEntriesFast());
    return 0;
}

void BmnProfilometerSource::FillEventHeader(FairEventHeader* feh) {
    return;
}

BmnStatus BmnProfilometerSource::ProcessBuffer(uint32_t *word, size_t len, ProfBoard * board) {
    uint32_t holdb_temp = 0;
    uint32_t holdb = 0;
    //    string str(reinterpret_cast<char*> (word), 6);
    //    printf("str %s\n", str.c_str());
    for (uint32_t i = 0; i < len; i++) {
        uint32_t data = word[i];
        //                                                printf("data %08X  i = %05u\n",data, i);
        // Check is it data or a trigger:
        if (BmnProfRawTools::data_or_trig(data)) {
            holdb = BmnProfRawTools::holdb_cntr(data); //get holdb counter
            if (holdb_temp == holdb) {
                //                printf("data  %08X\n", data);
                // Divide ADC0 and ADC1 data
                if (!BmnProfRawTools::adc_num(data)) { // 1st ADC
                    board->adc1_word.push_back(data);
                } else if (BmnProfRawTools::adc_num(data)) { // 2nd ADC
                    board->adc2_word.push_back(data);
                }
            } else {
                if (board->adc1_word.size() == BmnProfRawTools::ChannelDigitCnt())
                    new((*fArrTemp)[fArrTemp->GetEntriesFast()]) BmnADCDigit(board->board_id, 0, BmnProfRawTools::ChannelDigitCnt(), board->adc1_word);
                //                if (board->adc2_word.size() == BmnProfRawTools::ChannelDigitCnt())
                //                    new((*fArr)[fArr->GetEntriesFast()]) BmnADCDigit(board->board_id, 1, BmnProfRawTools::ChannelDigitCnt(), board->adc2_word);
                //                printf("adc1_word  %lu\n", adc1_word.size());
                //                printf("adc1  %lu\n", adc1.size());
                board->adc1_word.clear();
                board->adc2_word.clear();
                // Divide ADC0 and ADC1 data
                if (!BmnProfRawTools::adc_num(data)) { // 1st ADC
                    board->adc1_word.push_back(data);
                } else { // 2nd ADC
                    board->adc2_word.push_back(data);
                }
                holdb_temp = holdb;
            }
        } else {
            //            if (BmnProfRawTools::trig_psd(data)) {
            //                trigger_pside.push_back(data);
            //            } else if (BmnProfRawTools::trig_nsd(data)) {
            //                trigger_nside.push_back(data);
            //            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnProfilometerSource::ReceiveSpill() {
    const Int_t MaxStrLen = 100;
    bool isIdFound = kFALSE;
    bool isHeaderFound = kFALSE;
    bool isTrailerFound = kFALSE;
    string BoardName;
    ProfBoard * Board = nullptr;
    do {
        Int_t recv_more = 0;
        isIdFound = kFALSE;
        do {
            //            gSystem->ProcessEvents();
            //            fServer->ProcessRequests();
            for (void *rs : fRawSockets) {
                zmq_msg_t msg;
                zmq_msg_init(&msg);
                Int_t frame_size = zmq_msg_recv(&msg, rs, ZMQ_DONTWAIT); //  ZMQ_DONTWAIT
                //                printf("recv %d\n", frame_size);
                if (frame_size == -1) {
                    //                printf("Receive error # %d #%s\n", errno, zmq_strerror(errno));
                    switch (errno) {
                        case EAGAIN:
                            //                            printf("EAGAIN\n");
                            usleep(50000);
                            break;
                        case EINTR:
                            if (fVerbosity)
                                printf("EINTR\n");
                            isReceiving = kFALSE;
                            keepWorking = kFALSE;
                            if (fVerbosity)
                                printf("Exit!\n");
                            break;
                        case EFAULT:
                            if (fVerbosity)
                                printf("EFAULT\n");
                            zmq_close(rs);
                            isReceiving = kFALSE;
                            keepWorking = kFALSE;
                            break;
                        default:
                            break;
                    }
                } else {
                    if (frame_size < MaxStrLen) {
                        string str(static_cast<char*> (zmq_msg_data(&msg)), zmq_msg_size(&msg));
                        //                    printf("str %s\n", str.c_str());
                        if (isIdFound) {
                            if (str == DataTrailer) {
                                isTrailerFound = kTRUE;
                                if (fVerbosity)
                                    printf("trailer\n");
                                keepWorking = kFALSE;
                            }
                            if (str == DataHeader) {
                                isHeaderFound = kTRUE;
                                if (fVerbosity)
                                    printf("header\n");
                            }
                        } else {
                            //                        if (str == TargetBoardId) {
                            BoardName = str;
                            auto it = fBoardsMap.find(BoardName);
                            if (it != fBoardsMap.end())
                                Board = it->second;
                            isIdFound = kTRUE;
                            if (fVerbosity)
                                printf("id %s\n", str.c_str());
                            //                        }
                        }
                    } else {
                        if (isHeaderFound && Board) {
                            ProcessBuffer(static_cast<uint32_t*> (zmq_msg_data(&msg)), zmq_msg_size(&msg) / 4, Board);
                        }
                    }
                }
                size_t opt_size = sizeof (recv_more);
                if (zmq_getsockopt(rs, ZMQ_RCVMORE, &recv_more, &opt_size) == -1) {
                    printf("ZMQ socket options error #%s\n", zmq_strerror(errno));
                    return kBMNERROR;
                }
                //            printf("ZMQ rcvmore = %d\n", recv_more);
                zmq_msg_close(&msg);
            }
        } while (recv_more && isReceiving && (!isTrailerFound));
    } while (keepWorking);
    if (fVerbosity)
        printf("FullReceive\n");
    if (isTrailerFound) {
        if (fVerbosity)
            printf("SPILLEND for %s\n", BoardName.c_str());
        isHeaderFound = kFALSE;
        isTrailerFound = kFALSE;
    }
    return kBMNSUCCESS;
}
