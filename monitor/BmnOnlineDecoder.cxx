/*
 * File:   BmnOnlineDecoder.cxx
 * Author: ilnur
 *
 * Created on January 11, 2017, 11:08 AM
 */

#include "BmnOnlineDecoder.h"

#include <dirent.h>
#include <regex>
#include <signal.h>
#include <sys/inotify.h>
#include <thread>
#include <zmq.h>
// BmnRoot
#include <BmnConverterTools.h>
#include <BmnFileProp.h>
#include <BmnGemStripDigit.h>
#include <BmnTof1Digit.h>
#include <BmnTrigDigit.h>
#include <BmnVSPDigit.h>
#include <RawTypes.h>

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

BmnOnlineDecoder::BmnOnlineDecoder()
    : _ctx(nullptr)
    , fBmnSetup(kBMNSETUP)
    , rawDataDecoder(nullptr)
    , fRunID(UNKNOWN_RUNID)
    , fKeepWorking(false)
{}

BmnOnlineDecoder::~BmnOnlineDecoder()
{
    signal(SIGINT, SIG_DFL);
    if (rawDataDecoder) {
        rawDataDecoder->GetDecoder()->Finalize();
        delete rawDataDecoder;
        rawDataDecoder = nullptr;
    }
    if (_ctx) {
        zmq_ctx_destroy(_ctx);
        _ctx = NULL;
    }
}

BmnStatus BmnOnlineDecoder::InitDecoder(string fRawFileName)
{
    return InitDecoder(TString(fRawFileName.data()));
}

BmnStatus BmnOnlineDecoder::InitDecoder(TString fRawFileName)
{
    DBG("started")
    rawDataDecoder = new BmnRawDataDecoder(fRawFileName);
    //    rawDataDecoder->SetApplyThreshold();
    Int_t runID = rawDataDecoder->GetRunId();
    if (runID < 1) {
        runID = rawDataDecoder->GetRunIdFromFile(_curFile);
        //        regex re(".*mpd_run_.*_(\\d+).data");
        //        string idstr = regex_replace(fRawFileName.Data(), re, "$1");
        //        runID = atoi(idstr.c_str());
        if (runID < 0) {
            printf("!!! Error Could not detect runID\n");
            return kBMNERROR;
        }
    }
    InitDecoder(runID);

    DBG("fin")
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::InitDecoder(uint32_t runID)
{
    LOGF(info, "InitDecoder runID = %d", runID);
    if (!rawDataDecoder)
        rawDataDecoder = new BmnRawDataDecoder();
    rawDataDecoder->SetRunId(runID);
    rawDataDecoder->SetPeriodId(fPeriodID);
    rawDataDecoder->SetDetectorSetup(fDetectorSetup);
    rawDataDecoder->SetBmnSetup(fBmnSetup);
    rawDataDecoder->SetVerbose(1);
    rawDataDecoder->CreateConverter();
    rawDataDecoder->CreateDecoder();
    rawDataDecoder->SetForwardMode();
    BmnDecoder* decoder = rawDataDecoder->GetDecoder();
    TString PeriodSetupExt = Form("%d%s.txt", fPeriodID, ((fBmnSetup == kBMNSETUP) ? "" : "_SRC"));
    decoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt);
    decoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt);
    decoder->SetSiliconMapping(TString("SILICON_map_run") + PeriodSetupExt);
    decoder->SetSiBTMapping(TString("SiBT_map_period") + PeriodSetupExt);
    decoder->SetGemMapping(TString("GEM_map_run") + PeriodSetupExt);
    decoder->SetCSCMapping(TString("CSC_map_period")
                           + Form("%d%s%s.txt", fPeriodID, ((fBmnSetup == kBMNSETUP) ? "" : "_SRC"),
                                  (((decoder->GetRunId() < 7013) && (fPeriodID == 8)) ? "_before_run_7013" : "")));
    decoder->SetVspMapping(TString("VSP_map_period") + PeriodSetupExt);
    decoder->SetMSCMapping(TString("MSC_map_Run") + PeriodSetupExt);
    // in case comment out the line decoder->SetTof400Mapping("...")
    // the maps of TOF400 will be read from DB (only for JINR network)
    decoder->SetTOF700ReferenceRun(7012);
    decoder->SetTof700Geom(TString("TOF700_geometry_run") + PeriodSetupExt);
    decoder->SetTof400Mapping(TString("TOF400_PlaceMap_RUN") + PeriodSetupExt,
                              TString("TOF400_StripMap_RUN") + PeriodSetupExt);
    decoder->SetTof701Mapping(TString("TOF701_PlaceMap_RUN") + PeriodSetupExt,
                              TString("TOF701_StripMap_RUN") + PeriodSetupExt);
    if (decoder->GetRunId() >= 4278 && decoder->GetPeriodId() == 7)
        decoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d_from_run_4278.txt", fPeriodID));
    else
        decoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d.txt", fPeriodID));
    decoder->SetScWallMapping(TString("SCWALL_map_period") + PeriodSetupExt);
    decoder->SetScWallCalibration(TString("SCWALL_calibration_period") + PeriodSetupExt);
    decoder->SetFHCalMapping(TString("FHCAL_map_period") + PeriodSetupExt);
    decoder->SetFHCalCalibration(TString("FHCAL_calibration_period") + PeriodSetupExt);
    decoder->SetHodoMapping(Form("HODO_map_period%d%s.json", fPeriodID, ((fBmnSetup == kBMNSETUP) ? "" : "_SRC")));
    decoder->SetHodoCalibration(
        Form("HODO_Q_calibration_period%d%s.json", fPeriodID, ((fBmnSetup == kBMNSETUP) ? "" : "_SRC")));
    decoder->SetNdetMapping(TString("NDET_map_period") + PeriodSetupExt);
    if ((decoder->GetRunId() >= 7506 && decoder->GetRunId() <= 7534)
        || (decoder->GetRunId() >= 8203 && decoder->GetRunId() <= 8258))
        decoder->SetNdetCalibration(TString("NDET_calibration_period")
                                    + Form("%d_run_7506_7534_or_8203_8258.txt", fPeriodID));
    else
        decoder->SetNdetCalibration(TString("NDET_calibration_period") + PeriodSetupExt);
    decoder->SetUseCalibFile(true);
    if (_curFile.Length() > 0)
        rawDataDecoder->InitConverter(_curDir + _curFile);
    else
        rawDataDecoder->InitConverter();
    rawDataDecoder->ConnectForward();
    BmnStatus iniStatus = rawDataDecoder->InitDecoder();
    return iniStatus;
}

BmnStatus BmnOnlineDecoder::ConnectDataSocket()
{
    _socket_data = zmq_socket(_ctx, ZMQ_STREAM);
    Char_t endpoint_addr[MAX_ADDR_LEN];
    snprintf(endpoint_addr, MAX_ADDR_LEN, "tcp://%s", fDAQAddr.Data());
    if (zmq_connect(_socket_data, endpoint_addr) != 0) {
        DBGERR("zmq connect")
        return kBMNERROR;
    } else {
        printf("Listening to %s\n", endpoint_addr);
    }
    //    UInt_t rcvBufLen = MAX_BUF_LEN;
    //    Int_t rcvBuf = 0;
    //    size_t vl = sizeof(rcvBuf);
    //    if (zmq_setsockopt(_socket_data, ZMQ_RCVBUF, &rcvBufLen, sizeof(rcvBufLen)) == -1)
    //        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
    //    if (zmq_getsockopt(_socket_data, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
    //        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
    //    printf("data rcvbuf = %d\n", rcvBuf);
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::ConnectDigiSocket()
{
    _decoSocket = zmq_socket(_ctx, ZMQ_PUB);
    //    _socket_mcast = zmq_socket(_ctx, ZMQ_XSUB);
    //    UInt_t sndBufLen = 8 * 1024 * 1024;
    //    if (zmq_setsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, sizeof(rcvBuf)) == -1)
    //        DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
    //    rcvBuf = 0;
    //    if (zmq_getsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, &vl) == -1)
    //        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
    //    printf("digi sndbuf = %d\n", rcvBuf);

    int32_t hwm = 5;
    if (zmq_setsockopt(_decoSocket, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == -1)
        fprintf(stderr, "Error in zmq_setsockopt of ZMQ_SNDHWM: %s\n", strerror(errno));
    size_t vl = sizeof(hwm);
    if (zmq_getsockopt(_decoSocket, ZMQ_SNDHWM, &hwm, &vl) == -1)
        fprintf(stderr, "Error getting ZMQ socket ZMQ_SNDHWM size: %s\n", strerror(errno));
    printf("ZMQ_SND HWM %d\n", hwm);

    TString localDecoStr = Form("tcp://*:%d", RAW_DECODER_SOCKET_PORT);
    if (zmq_bind(_decoSocket, localDecoStr.Data()) != 0) {
        DBGERR("zmq bind")
        return kBMNERROR;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::OpenStream()
{
    DBG("started")
    _ctx = zmq_ctx_new();
    if (ConnectDataSocket() != kBMNSUCCESS)
        return kBMNERROR;
    if (ConnectDigiSocket() != kBMNSUCCESS)
        return kBMNERROR;
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::CloseStream()
{
    DBG("started")
    //    zmq_close(_socket_mcast);
    zmq_close(_socket_data);
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    DBG("exit")
    return kBMNFINISH;
}

BmnStatus BmnOnlineDecoder::Decode(TString dirname, TString startFile, Bool_t runCurrent)
{
    DBG("started")
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_PUB);
    Int_t rcvBuf = 0;
    size_t vl = sizeof(rcvBuf);
    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
    printf("rcvbuf = %d\n", rcvBuf);
    //    rcvBuf = 8192; //MAX_BUF_LEN;
    //    if (zmq_setsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
    //        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
    //        if (zmq_setsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
    //            DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
    //            rcvBuf = 0;
    //    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
    //        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
    //        printf("rcvbuf = %d\n", rcvBuf);
    TString localDecoStr = Form("tcp://*:%d", RAW_DECODER_SOCKET_PORT);
    if (zmq_bind(_decoSocket, localDecoStr.Data()) != 0) {
        DBGERR("zmq bind")
        return kBMNERROR;
    }
    _curFile = startFile;
    _curDir = dirname;

    if (!runCurrent) {
        _curFile = "";
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    } else if (_curFile.Length() == 0) {
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    }

    while (kTRUE) {
        if (InitDecoder(_curDir + _curFile) == kBMNERROR)
            continue;
        ProcessFileRun(_curDir + _curFile);
        rawDataDecoder->GetDecoder()->Finalize();
        delete rawDataDecoder;
        rawDataDecoder = NULL;
        //                break; // @TODO Remove
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    }
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    return kBMNSUCCESS;
}

void BmnOnlineDecoder::ProcessStream()
{
    DBG("started")
    OpenStream();
    //    UInt_t *buf = (UInt_t*) malloc(MAX_BUF_LEN);
    Char_t conID[MAX_ADDR_LEN] = {0};
    Int_t conID_size = 0;
    UInt_t msg_len = 0;
    Int_t frame_size = 0;
    // Int_t iEv = 0;
    // Int_t lastEv = -1;
    fEvents = 0;
    BmnStatus convertResult = kBMNSUCCESS;
    Int_t sendRes = 0;
    TBufferFile t(TBuffer::kWrite);
    // UInt_t syncCounter = 0;
    bool isListening = true;
    fKeepWorking = true;
    FILE* fp(nullptr);
    //    fp = fopen("sample.data", "w");
    signal(SIGINT, signal_handler);
    //    signal(SIGTERM, signal_handler);

    while ((!ReceivedSignal) && isListening /* && (msg_len > MAX_BUF_LEN)*/) {

        conID_size = zmq_recv(_socket_data, &conID, sizeof(conID), ZMQ_DONTWAIT);
        //                    printf("ID Recv %u\n", conID_size);
        if (conID_size == -1) {
            //            printf("ID Receive error #%d : %s\n", errno, zmq_strerror(errno));
            switch (errno) {
                case EAGAIN:
                    usleep(MSG_TIMEOUT);
                    //                        else
                    //                            printf("no sleep\n");
                    break;
                case EINTR:
                    printf("Interrupted!\n");
                    isListening = kFALSE;
                    printf("Exit cycle!\n");
                    continue;
                    break;
                default:
                    printf("ID Receive error #%d : %s\n", errno, zmq_strerror(errno));
                    break;
            }
        } else {
            //                printf("ID size =  %d\n Id:%x\n", conID_size, conID);
        }
        //        }
        if (conID_size < 0)
            continue;
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        Int_t recv_more = 0;
        // UInt_t *msgPtr;
        UInt_t* word;
        bool isReceiving = true;
        do {
            frame_size = zmq_msg_recv(&msg, _socket_data, ZMQ_DONTWAIT);   //  ZMQ_DONTWAIT
            //            printf("recv %u\n", frame_size);
            // frame_size = zmq_recv(_socket_data, buf, MAX_BUF_LEN, 0);
            if (frame_size == -1) {
                printf("Receive error # %d #%s\n", errno, zmq_strerror(errno));
                switch (errno) {
                    case EAGAIN:
                        if ((msg_len < MPD_EVENT_HEAD_WORDS_OLD))
                            usleep(MSG_TIMEOUT);
                        break;
                    case EINTR:
                        printf("EINTR\n");
                        isReceiving = kFALSE;
                        isListening = kFALSE;
                        printf("Exit!\n");
                        break;
                    case EFAULT:
                        printf("EFAULT\n");
                        zmq_close(_socket_data);
                        ConnectDataSocket();
                        isReceiving = kFALSE;
                        break;
                    default:
                        break;
                }
            } else {
                if (frame_size) {
                    if (msg_len + frame_size > MAX_BUF_LEN) {
                        printf("buf overflow! msg len %u frame size %u\n", msg_len, frame_size);
                        printf("Something wrong! Exit!\n");
                        isReceiving = kFALSE;
                        isListening = kFALSE;
                        msg_len = 0;
                        //                        if (msg_len % kNBYTESINWORD) {
                        //                            printf("Something wrong! Resetting buffer!\n");
                        //                            msg_len = 0;
                        //                        } else {
                        //                            UInt_t dropped = msg_len + frame_size - MAX_BUF_LEN;
                        //                            msg_len -= dropped;
                        //                            printf("Frame_size %u\t Message length %d, dropped %d bytes\n",
                        //                            frame_size, msg_len, dropped); memmove(buf, &buf[frame_size],
                        //                            msg_len);
                        //                        }
                    }
                    memcpy(buf + msg_len, zmq_msg_data(&msg), frame_size);
                    msg_len += frame_size;
                    //                    fwrite(zmq_msg_data(&msg), 1, zmq_msg_size(&msg), fp);
                    //                    printf("msg_len    = %d\n", msg_len);
                }
            }
            size_t opt_size = sizeof(recv_more);
            if (zmq_getsockopt(_socket_data, ZMQ_RCVMORE, &recv_more, &opt_size) == -1) {
                printf("ZMQ socket options error #%s\n", zmq_strerror(errno));
                isReceiving = false;
            }
            //            printf("ZMQ rcvmore = %d\n", recv_more);
            zmq_msg_close(&msg);
        } while ((!ReceivedSignal) && recv_more && isReceiving);

        if (msg_len < MPD_EVENT_HEAD_WORDS_OLD * sizeof(UInt_t))
            continue;
        UInt_t iWord = 0;
        Bool_t evExit = false;
        UInt_t lenBytes = 0;
        UInt_t lenWords = 0;
        UInt_t runID = 0;
        word = reinterpret_cast<UInt_t*>(buf);
        while (isListening && (!ReceivedSignal) && (iWord < msg_len / kNBYTESINWORD) && (!evExit)
               && (msg_len > MPD_EVENT_HEAD_WORDS_OLD))
        {
            UInt_t val = *(word + iWord);
            switch (val) {
                case SYNC_RUN_START:
                    printf("iWord = %u\n", iWord);
                    printf("start run\n");
                    lenWords = *(word + ++iWord) / sizeof(UInt_t);
                    printf("payLen = %u words\n", lenWords);
                    if (lenWords + iWord > MAX_BUF_LEN) {
                        printf("Wrong payload size!\n");
                        lenBytes = iWord * sizeof(UInt_t);
                        msg_len -= lenBytes;
                        printf(" %u will move by %u bytes. skip wrong header\n", msg_len, lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        iWord = 0;
                        break;
                    }
                    BmnConverterTools::ParseComplexTLV((word + ++iWord), lenWords, runID);
                    printf("runID = %u\n", runID);
                    if (fRunID != runID) {
                        AddRun2DB();
                        if (StartNewRun(runID))
                            break;
                    }
                    iWord += lenWords;
                    lenBytes = iWord * sizeof(UInt_t);
                    printf(" lenBytes %u \n", lenBytes);
                    msg_len -= lenBytes;
                    printf(" %u will move by %u bytes. after sync start\n", msg_len, lenBytes);
                    memmove(&buf[0], &buf[lenBytes], msg_len);
                    iWord = 0;
                    //                    evExit = kTRUE;
                    break;
                case SYNC_RUN_STOP:
                    printf("stop run\n");
                    lenWords = *(word + ++iWord) / sizeof(UInt_t);
                    printf("payLen = %d words\n", lenWords);
                    if (lenWords + iWord > MAX_BUF_LEN) {
                        printf("Wrong payload size!\n");
                        lenBytes = iWord * sizeof(UInt_t);
                        msg_len -= lenBytes;
                        printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        iWord = 0;

                        break;
                    }
                    BmnConverterTools::ParseComplexTLV((word + ++iWord), lenWords, runID);
                    fRunID = runID;
                    AddRun2DB();
                    if (rawDataDecoder) {
                        rawDataDecoder->GetDecoder()->Finalize();
                        delete rawDataDecoder;
                        rawDataDecoder = NULL;
                    }
                    iWord += lenWords;
                    lenBytes = iWord * sizeof(UInt_t);
                    msg_len -= lenBytes;
                    //                    printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                    memmove(&buf[0], &buf[lenBytes], msg_len);
                    iWord = 0;
                    //                    evExit = kTRUE;
                    break;
                case SYNC_EVENT:
                case SYNC_EVENT_OLD:
                    lenBytes = *(word + ++iWord);
                    lenWords = lenBytes / kNBYTESINWORD + (fPeriodID <= 7 ? 1 : 0);
                    if (msg_len / kNBYTESINWORD
                        >= lenWords + iWord + (fPeriodID > 7 ? MPD_EVENT_HEAD_WORDS : MPD_EVENT_HEAD_WORDS_OLD))
                    {
                        // printf(ANSI_COLOR_BLUE "SYNC_EVENT\n" ANSI_COLOR_RESET);
                        ////                                                printf("iWord    == %7u  SIGNAL\n", iWord);
                        //                                                printf("lenBytes == %u\n", lenBytes);
                        //                                                printf("msg_len  == %u\n", msg_len);
                        //                                                printf("lenWords == %u\n", lenWords);
                        //                        //                    printf("captured enough\n");
                        if (!rawDataDecoder) {
                            printf("Init without run header!\n");
                            if (InitDecoder(fRunID) == kBMNERROR) {
                                printf("\n\tError in InitDecoder !!\n\n");
                                //                                evExit = kTRUE;
                                break;
                            }
                        }
                        //                        rawDataDecoder->SetRunId(fRunID);
                        convertResult = rawDataDecoder->ConvertRawToRootIterate(word + ++iWord, lenWords);
                        //                        printf(" convertResult %d \n", convertResult);
                        if (convertResult == kBMNERROR) {
                            printf("convert failed\n");
                            //                            evExit = kTRUE;
                            lenBytes = (iWord - 1) * sizeof(UInt_t);
                            //                            lenBytes = iWord * sizeof (UInt_t);
                            msg_len -= lenBytes;
                            printf(" %u will move by %u bytes. after conv failed\n", msg_len, lenBytes);
                            memmove(&buf[0], &buf[lenBytes], msg_len);
                            iWord = 0;
                            break;
                        }
                        BmnStatus decostat = rawDataDecoder->DecodeDataToDigiIterate();
                        //                        printf(" decostat %d \n", decostat);
                        fEvents++;
                        if (decostat == kBMNSUCCESS) {
                            DigiArrays iterDigi = rawDataDecoder->GetDecoder()->GetDigiArraysObject();
                            BmnEventHeader* head = iterDigi.header;
                            if (head) {
                                if (fRunID != head->GetRunId()) {
                                    if (StartNewRun(head->GetRunId()))
                                        break;
                                }
                                if (head->GetEventType() == kBMNPAYLOAD) {
                                    t.WriteObject(&iterDigi);
                                    sendRes = zmq_send(_decoSocket, t.Buffer(), t.Length(), ZMQ_NOBLOCK);
                                    //  printf("sendRes %d\n", sendRes);
                                    t.Reset();
                                    if (sendRes == -1) {
                                        printf("Send error # %d : %s\n", errno, zmq_strerror(errno));
                                    }
                                }
                            }
                        }
                        iWord += lenWords;
                        lenBytes = iWord * sizeof(UInt_t);
                        //                        printf("msg_len %8u lenBytes %8u bytes. after deco\n", msg_len,
                        //                        lenBytes);
                        msg_len -= lenBytes;
                        //                        printf(" %8u will move by %8u bytes. after deco\n", msg_len,
                        //                        lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        iWord = 0;
                    } else {
                        //                        printf("Not enough data in the buffer!\n");
                        iWord = 0;
                        evExit = kTRUE;
                    }
                    break;
                case SYNC_STAT:   // just skip at this point
                    // printf(ANSI_COLOR_BLUE "STAT\n" ANSI_COLOR_RESET);
                    lenBytes = *(word + ++iWord);
                    lenBytes = lenBytes + (fPeriodID <= 7 ? kNBYTESINWORD : 0);
                    lenWords = lenBytes / kNBYTESINWORD;
                    if (msg_len >= lenBytes + iWord * kNBYTESINWORD + MPD_EVENT_HEAD_WORDS * kNBYTESINWORD) {
                        //                        //                        printf("iWord    == %7u   STAT\n", iWord);
                        //                                                printf("lenBytes == %u\n", lenBytes);
                        //                                                printf("msg_len  == %u\n", msg_len);
                        //                                                printf("lenWords == %u\n", lenWords);
                        //                        ++iWord; // if other is commented
                        // stat decoding will be here
                        //                                                convertResult =
                        //                                                rawDataDecoder->ConvertStatEvent(word +
                        //                                                ++iWord, lenWords); printf("captured
                        //                                                enough\n");
                        if (!rawDataDecoder) {
                            printf("Init without run header!\n");
                            if (InitDecoder(fRunID) == kBMNERROR) {
                                printf("\n\tError in InitDecoder !!\n\n");
                                //                                evExit = kTRUE;
                                break;
                            }
                        }
                        convertResult = rawDataDecoder->ConvertRawToRootIterate(word + ++iWord, lenWords);
                        //                        printf("stat convertResult %d \n", convertResult);
                        rawDataDecoder->GetRawConverter()->SetRawEventStat();
                        if (convertResult == kBMNERROR) {
                            printf("stat convert failed\n");
                            //                            evExit = kTRUE;
                            lenBytes = iWord * sizeof(UInt_t);
                            msg_len -= lenBytes;
                            //                            printf(" %u will move by %u bytes. after conv failed\n",
                            //                            msg_len, lenBytes);
                            memmove(&buf[0], &buf[lenBytes], msg_len);
                            iWord = 0;
                            break;
                        }
                        BmnStatus decostat = rawDataDecoder->DecodeDataToDigiIterate();
                        //                                                printf(" decostat %d \n", decostat);
                        fEvents++;
                        if (decostat == kBMNSUCCESS) {
                            DigiArrays iterDigi = rawDataDecoder->GetDecoder()->GetDigiArraysObject();
                            BmnEventHeader* head = iterDigi.header;
                            if (head) {
                                printf("EventId == %u\n", head->GetEventId());
                                //     if (head->GetEventType() == kBMNPAYLOAD) {
                                t.WriteObject(&iterDigi);
                                sendRes = zmq_send(_decoSocket, t.Buffer(), t.Length(), ZMQ_NOBLOCK);
                                //     printf("sendRes %d\n", sendRes);
                                t.Reset();
                                if (sendRes == -1) {
                                    printf("Send error # %d : %s\n", errno, zmq_strerror(errno));
                                }
                                //                                }
                            }
                        }
                        //                        iWord += lenWords;
                        lenBytes += (iWord - 1) * kNBYTESINWORD;
                        //                        lenBytes += iWord * kNBYTESINWORD;
                        //                        lenBytes = iWord * sizeof (UInt_t);
                        msg_len -= lenBytes;
                        //                        printf(" %u will move by %u bytes. after stat\n", msg_len, lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        iWord = 0;
                    } else {
                        //                        printf("Not enough data in the buffer!\n");
                        iWord = 0;
                        evExit = kTRUE;
                    }
                    break;
                case SYNC_JSON:
                    //                    printf(ANSI_COLOR_BLUE "SYNC_JSON\n" ANSI_COLOR_RESET);
                    lenBytes = *(word + ++iWord);
                    //                    //                    lenWords = lenBytes / kNBYTESINWORD + (fPeriodID <= 7 ?
                    //                    1 : 0);
                    if (msg_len >= lenBytes + iWord * kNBYTESINWORD) {
                        //                        printf("iWord    == %7u  JSON\n", iWord);
                        //                        printf("lenBytes == %u\n", lenBytes);
                        //                        printf("msg_len  == %u\n", msg_len);
                        //                        ++iWord; // if other is commented
                        if (!rawDataDecoder) {
                            printf("Init without run header!\n");
                            if (InitDecoder(fRunID) == kBMNERROR) {
                                printf("\n\tError in InitDecoder !!\n\n");
                                //                                evExit = kTRUE;
                                break;
                            }
                        }
                        rawDataDecoder->GetRawConverter()->ParseJsonTLV(word + ++iWord, lenBytes);
                        //                                                lenBytes += (iWord-1) * kNBYTESINWORD;
                        lenBytes += iWord * kNBYTESINWORD;
                        word = reinterpret_cast<UInt_t*>(buf + lenBytes);
                        msg_len -= lenBytes;
                        //                        printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        iWord = 0;
                    } else {
                        //                        printf("Not enough data in the buffer!\n");
                        iWord = 0;
                        evExit = kTRUE;
                    }
                    break;
                default:
                    //            printf("unrecognized sync %08X\n", *(word + iWord));
                    iWord++;
                    break;
            }
        }
        if (iWord >= MAX_BUF_LEN) {
            printf("Wrong data, resetting array!\n");
            msg_len = 0;
        }
    }
    CloseStream();

    if (fp != NULL) {
        fclose(fp);
        printf("I: file closed\n\r");
    }
    DBG("exit")
    signal(SIGINT, SIG_DFL);
}

BmnStatus BmnOnlineDecoder::StartNewRun(uint32_t runID)
{
    fRunID = runID;
    LOGF(info, "fRunID %u\n", fRunID);
    if (rawDataDecoder) {
        rawDataDecoder->GetDecoder()->Finalize();
        delete rawDataDecoder;
        rawDataDecoder = NULL;
    }
    if (InitDecoder(runID) == kBMNERROR) {
        printf("\n\tError in InitDecoder !!\n\n");
        if (rawDataDecoder) {
            delete rawDataDecoder;
            rawDataDecoder = NULL;
        }
        return kBMNERROR;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::AddRun2DB()
{
    if (fRunID == UNKNOWN_RUNID)
        return kBMNERROR;   // do not store unknown run
    if (!rawDataDecoder)
        return kBMNSUCCESS;
    TTimeStamp fRunStartTime = rawDataDecoder->GetRunStartTime();
    TTimeStamp fRunEndTime = rawDataDecoder->GetRunEndTime();
    TDatime fFileStartDate(Int_t(fRunStartTime.GetDate(kFALSE)), Int_t(fRunStartTime.GetTime(kFALSE)));
    TDatime fFileEndDate(Int_t(fRunEndTime.GetDate(kFALSE)), Int_t(fRunEndTime.GetTime(kFALSE)));
    Int_t event_count = rawDataDecoder->GetEventId();
    printf("\tEvCnt   %d\n", event_count);
    printf("\tStartTime %s\n", fRunStartTime.AsString());
    printf("\t  EndTime %s\n", fRunEndTime.AsString());
    UniRun* pRun = UniRun::GetRun(fPeriodID, fRunID);
    if (!pRun) {
        pRun = UniRun::CreateRun(fPeriodID, fRunID, 0, fFileStartDate, &fFileEndDate, "", nullptr, nullptr, nullptr,
                                 &event_count, nullptr);
        if (pRun)
            cout << "New run has been successfully created in the Condition Database with number " << fPeriodID << ":"
                 << fRunID << endl;
        else
            return kBMNERROR;
    } else {
        printf("Run %8u is already found in the DB.\n", fRunID);
        printf("\tStartTime %s\n", pRun->GetStartDatetime().AsString());
        printf("\t  EndTime %s\n", pRun->GetEndDatetime() ? pRun->GetEndDatetime()->AsString() : "");
        printf("\t   Events %d\n", pRun->GetEventCount() ? *pRun->GetEventCount() : -1);
        //        if ()
        pRun->SetStartDatetime(fFileStartDate);
        printf("writing StartTime %s\n", fFileStartDate.AsString());

        //        if (!pRun->GetEndDatetime())
        pRun->SetEndDatetime(&fFileEndDate);
        printf("writing   EndTime %s\n", fFileEndDate.AsString());
        //        if (!pRun->GetEventCount())
        pRun->SetEventCount(&event_count);
        printf("writing   EvCnt   %d\n", event_count);
    }
    delete pRun;
    return kBMNSUCCESS;
}

void BmnOnlineDecoder::ProcessFileRun(TString rawFileName, UInt_t timeLimit)
{
    DBG("start")
    // UInt_t iEv = 0;
    // UInt_t lastEv = 0;
    BmnStatus convertResult = kBMNSUCCESS;
    Int_t sendRes = 0;
    TBufferFile t(TBuffer::kWrite);
    while (!ReceivedSignal) {
        convertResult = rawDataDecoder->GetRawConverter()->ConvertRawToRootIterateFileRead();
        if (convertResult == kBMNFINISH) {
            printf("finish\n");
            break;
        }
        if (convertResult == kBMNERROR) {
            printf("Converting error\n");
            break;
        }
        // lastEv = iEv;
        // iEv = rawDataDecoder->GetEventId();
        //         if (iEv > lastEv) {
        /*BmnStatus decodeResult = */ rawDataDecoder->DecodeDataToDigiIterate();
        //                    printf("iev %u  convert %d decode %d\n", iEv, convertResult, decodeResult);
        fEvents++;
        DigiArrays iterDigi = rawDataDecoder->GetDecoder()->GetDigiArraysObject();
        if (iterDigi.header == NULL)
            continue;
        // BmnEventHeader* head = (BmnEventHeader*) iterDigi.header;
        //             if (head->GetEventType() != kBMNPAYLOAD)
        //                 continue;
        t.WriteObject(&iterDigi);
        sendRes = zmq_send(_decoSocket, t.Buffer(), t.Length(), ZMQ_NOBLOCK);
        t.Reset();
        if (sendRes == -1) {
            printf("Send error %d #%s\n", errno, zmq_strerror(errno));
        }
        //        }
        if (convertResult == kBMNTIMEOUT) {
            printf("timeout\n");
            break;
        }
        //        break;
    }
}

TString BmnOnlineDecoder::WatchNext(TString dirname, TString filename, Int_t cycleWait)
{
    DBG("started")
    struct dirent** namelist;
    regex re("\\w+\\.data");
    Int_t n;
    TString ret;
    while (kTRUE) {
        n = scandir(dirname.Data(), &namelist, 0, versionsort);
        if (n < 0)
            perror("scandir");
        else {
            for (Int_t i = 0; i < n; ++i) {
                if (regex_match(namelist[i]->d_name, re))
                    ret = namelist[i]->d_name;
                free(namelist[i]);
            }
            free(namelist);
        }
        if (strcmp(filename.Strip().Data(), ret.Strip().Data()) != 0)
            return ret;
        gSystem->ProcessEvents();
        usleep(cycleWait);
    }

    return ret;
}

TString BmnOnlineDecoder::WatchNext(Int_t inotifDir, Int_t cycleWait)
{
    TString fileName = "";
    Char_t evBuf[INOTIF_BUF_LEN];
    while (kTRUE) {
        Int_t len, i = 0;
        len = read(inotifDir, evBuf, INOTIF_BUF_LEN);
        if ((len == -1) && (errno != EAGAIN))
            DBG("inotify read error!")
        else {
            while (i < len) {
                struct inotify_event* event = (struct inotify_event*)&evBuf[i];
                if (event->len) {
                    if ((event->mask & IN_CREATE) && !(event->mask & IN_ISDIR)) {
                        fileName = TString(event->name);
                        printf("File %s was created!\n", fileName.Data());
                        break;
                    }
                }
                i += sizeof(struct inotify_event) + event->len;
            }
            if (cycleWait > 0)
                usleep(cycleWait);
            else
                break;
        }
        gSystem->ProcessEvents();
    }
    return fileName;
}

BmnStatus BmnOnlineDecoder::BatchDirectory(TString dirname)
{
    _curDir = dirname;
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_PUB);
    TString localDecoStr = Form("tcp://*:%d", RAW_DECODER_SOCKET_PORT);
    if (zmq_bind(_decoSocket, localDecoStr.Data()) != 0) {
        DBGERR("zmq bind")
        return kBMNERROR;
    }
    struct dirent** namelist;
    const regex re(".*_run_.*_(\\d+).*.data");

    // vector<TString> fileNames;
    vector<pair<BmnFileProp, string>> fileNames;
    Int_t runCount = 0;
    Int_t n;
    n = scandir(_curDir, &namelist, 0, versionsort);
    LOGF(info, "Found %d files for batch decoding", n);
    if (n < 0) {
        perror("scandir");
        return kBMNERROR;
    } else {
        for (Int_t i = 0; i < n; ++i) {
            if (ReceivedSignal)
                break;
            if (regex_match(namelist[i]->d_name, re)) {
                BmnFileProp prop;
                string file_name(namelist[i]->d_name);
                if (BmnConverterTools::ParseRawFileName(file_name, prop) == kBMNERROR)
                    continue;
                fileNames.emplace_back(make_pair(move(prop), move(file_name)));
            }
        }
        std::sort(fileNames.begin(), fileNames.end(), [](auto& a, auto& b) {
            if (a.first.GetRunId() < b.first.GetRunId())
                return true;
            else if (a.first.GetRunId() == b.first.GetRunId()) {
                if (a.first.GetFileId() < b.first.GetFileId())
                    return true;
                else if ((a.first.GetFileId() == b.first.GetFileId())
                         && (a.first.GetEventOrder() < b.first.GetEventOrder()))
                    return true;
            }
            return false;
        });
        for (auto& pf : fileNames) {
            LOGF(info, "raw file: %s", pf.second.data());
            _curFile = TString(pf.second.data());
            int32_t runID = pf.first.GetRunId();
            if ((runID > 7281) || (runID < 1)) {
                if (runCount == 0) {
                    if (InitDecoder(_curDir + _curFile) == kBMNERROR)
                        continue;
                } else {
                    if (rawDataDecoder) {
                        rawDataDecoder->GetDecoder()->Finalize();
                        delete rawDataDecoder;
                        rawDataDecoder = NULL;
                    }
                    if (InitDecoder(_curDir + _curFile) == kBMNERROR) {
                        printf("\n\tError in InitDecoder !!\n\n");
                        if (rawDataDecoder) {
                            delete rawDataDecoder;
                            rawDataDecoder = NULL;
                        }
                        //                                    evExit = kTRUE;
                        continue;
                    }
                }
                ProcessFileRun(_curFile, 0);
                runCount++;
            } else {
                continue;
            }
        }
        free(namelist);
    }
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::BatchDirectoryToSocket(TString dirname)
{
    uint32_t data_port = 21323;
    signal(SIGINT, signal_handler);
    _curDir = dirname;
    struct dirent** namelist;
    const regex re(".*_run_.*_(\\d+).*.data");

    // vector<TString> fileNames;
    vector<pair<BmnFileProp, string>> fileNames;
    Int_t n;
    n = scandir(_curDir, &namelist, 0, versionsort);
    LOGF(info, "Found %d files for batch decoding", n);
    if (n < 0) {
        perror("scandir");
        return kBMNERROR;
    } else {
        for (Int_t i = 0; i < n; ++i) {
            if (ReceivedSignal)
                break;
            if (regex_match(namelist[i]->d_name, re)) {
                BmnFileProp prop;
                string file_name(namelist[i]->d_name);
                if (BmnConverterTools::ParseRawFileName(file_name, prop) == kBMNERROR)
                    continue;
                fileNames.emplace_back(make_pair(move(prop), move(file_name)));
            }
        }
        std::sort(fileNames.begin(), fileNames.end(), [](auto& a, auto& b) {
            if (a.first.GetRunId() < b.first.GetRunId())
                return true;
            else if (a.first.GetRunId() == b.first.GetRunId()) {
                if (a.first.GetFileId() < b.first.GetFileId())
                    return true;
                else if ((a.first.GetFileId() == b.first.GetFileId())
                         && (a.first.GetEventOrder() < b.first.GetEventOrder()))
                    return true;
            }
            return false;
        });
        for (auto& pf : fileNames) {
            if (ReceivedSignal)
                break;
            if (pf.first.GetRunId() < 7281)
                continue;
            LOGF(info, "raw file: %s", pf.second.data());
            _curFile = TString(pf.second.data());
            int ret = system(Form("socat -u FILE:%s TCP4-LISTEN:%u,reuseaddr", (_curDir + _curFile).Data(), data_port));
            if (ret)
                LOGF(info, "Socat error code: %d", ret);
        }
        free(namelist);
    }
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    signal(SIGINT, SIG_DFL);
    return kBMNSUCCESS;
}

Int_t BmnOnlineDecoder::GetRunIdFromName(TString name)
{
    TPRegexp re(".*\\w+_\\w+_\\w+_(\\d+)[^/]*\\.data");
    //    TPRegexp re(".*mpd_run_.*_(\\d+).data");
    TObjArray* subStr = re.MatchS(name.Data());
    if (subStr->GetEntriesFast() > 1) {
        TString str = ((TObjString*)subStr->At(1))->GetString();
        Int_t runID = atoi(str.Data());
        subStr->Delete();
        delete subStr;
        return runID;
    } else {
        delete subStr;
        return -1;
    }
}

void BmnOnlineDecoder::StripView(TString OrigFileName,
                                 TString TestFileName,
                                 uint32_t periodID,
                                 uint32_t runID,
                                 BmnSetup fSetup,
                                 bool FillOccupancy)
{
    gStyle->SetOptStat(0);
    // Int_t sumMods = 0;
    // Int_t maxLayers = 0;
    bool set_bin_content = !FillOccupancy;

    TString fnames[STRIP_COMP_COLS] = {OrigFileName, TestFileName};
    TString treeNames[STRIP_COMP_COLS] = {"bmndata", "bmndata"};
    TString hdrNames[STRIP_COMP_COLS] = {"BmnEventHeader.", "BmnEventHeader."};
    TString runhdrNames[STRIP_COMP_COLS] = {"DigiRunHeader", "DigiRunHeader"};
    TString silNames[STRIP_COMP_COLS] = {"SILICON", "SILICON"};
    TString gemNames[STRIP_COMP_COLS] = {"GEM", "GEM"};
    TString cscNames[STRIP_COMP_COLS] = {"CSC", "CSC"};
    TString sibtNames[STRIP_COMP_COLS] = {"SiBT", "SiBT"};
    TString vspNames[STRIP_COMP_COLS] = {"VSP", "VSP"};
    TFile* files[STRIP_COMP_COLS] = {NULL};
    TTree* trees[STRIP_COMP_COLS] = {NULL};
    // TClonesArray * hits[STRIP_COMP_COLS] = {NULL};
    // TClonesArray * tracks[STRIP_COMP_COLS] = {NULL};
    DigiRunHeader* runHeaders[STRIP_COMP_COLS] = {NULL, NULL};
    BmnEventHeader* headers[STRIP_COMP_COLS] = {NULL, NULL};
    TClonesArray* silDigit[STRIP_COMP_COLS] = {NULL, NULL};
    TClonesArray* gemDigit[STRIP_COMP_COLS] = {NULL, NULL};
    TClonesArray* cscDigit[STRIP_COMP_COLS] = {NULL, NULL};
    TClonesArray* sibtDigit[STRIP_COMP_COLS] = {NULL, NULL};
    TClonesArray* vspDigit[STRIP_COMP_COLS] = {NULL, NULL};
    vector<vector<vector<TH1F*>>> histStrip[STRIP_COMP_COLS];
    vector<vector<vector<TH1F*>>> histStripGem[STRIP_COMP_COLS];
    vector<vector<vector<TH1F*>>> histStripCsc[STRIP_COMP_COLS];
    vector<vector<vector<TH1F*>>> histStripSiBT[STRIP_COMP_COLS];
    vector<vector<vector<TH1F*>>> histStripVSP[STRIP_COMP_COLS];
    TCanvas* canStrip = nullptr;
    TCanvas* canStripGem = nullptr;
    TCanvas* canStripCsc = nullptr;
    TCanvas* canStripSiBT = nullptr;
    TCanvas* canStripVSP = nullptr;
    vector<PadInfo*> canStripPads;
    vector<PadInfo*> canStripPadsGem;
    vector<PadInfo*> canStripPadsCsc;
    vector<PadInfo*> canStripPadsSiBT;
    vector<PadInfo*> canStripPadsVSP;

    // ********************
    // init pads and canvases
    // ********************
    auto stationSetFSD(BmnSiliconStationSet::Create(periodID, fSetup));
    InitCanvas("Silicon", stationSetFSD, canStrip, canStripPads, histStrip);
    auto stationSetGEM(BmnGemStripStationSet::Create(periodID, fSetup));
    InitCanvas("GEM", stationSetGEM, canStripGem, canStripPadsGem, histStripGem);
    auto stationSetCSC(BmnCSCStationSet::Create(periodID, fSetup));
    InitCanvas("CSC", stationSetCSC, canStripCsc, canStripPadsCsc, histStripCsc);
    unique_ptr<BmnSiBTStationSet> stationSetSiBT(BmnSiBTStationSet::Create(periodID, fSetup));
    InitCanvas("SiBT", stationSetSiBT, canStripSiBT, canStripPadsSiBT, histStripSiBT);
    auto stationSetVSP(BmnVSPStationSet::Create(periodID, fSetup));
    InitCanvas("VSP", stationSetVSP, canStripVSP, canStripPadsVSP, histStripVSP);

    // ********************
    // Trees processing
    // ********************

    Int_t curEv = 0;
    Int_t preEv = 0;
    Long64_t nEvs = LONG_MAX;
    for (Int_t i = 0; i < STRIP_COMP_COLS; i++) {
        if (fnames[i].Length() == 0)
            continue;
        files[i] = new TFile(fnames[i], "READ");
        if (files[i]->IsOpen())
            printf("file %s opened\n", fnames[i].Data());
        else {
            fprintf(stderr, "file %s open error\n", fnames[i].Data());
            return;
        }
        trees[i] = (TTree*)files[i]->Get(treeNames[i]);
        nEvs = Min(nEvs, trees[i]->GetEntries());
        cout << "#recorded entries = " << trees[i]->GetEntries() << endl;
        if (i > 0)
            runHeaders[i] = (DigiRunHeader*)files[i]->Get(runhdrNames[i].Data());
        //                runHeaders[i]);
        trees[i]->SetBranchAddress(hdrNames[i], &headers[i]);
        trees[i]->SetBranchAddress(silNames[i], &silDigit[i]);
        trees[i]->SetBranchAddress(gemNames[i], &gemDigit[i]);
        trees[i]->SetBranchAddress(cscNames[i], &cscDigit[i]);
        trees[i]->SetBranchAddress(sibtNames[i], &sibtDigit[i]);
        trees[i]->SetBranchAddress(vspNames[i], &vspDigit[i]);
    }
    for (Int_t i = 0; i < STRIP_COMP_COLS; i++) {
        if (trees[i] == nullptr)
            continue;
        cout << "tree # " << i << endl;
        if (i > 0 && runHeaders[i]) {
            printf("START (event 1):\t%s\n", runHeaders[i]->GetRunStartTime().AsString());
            printf("FINISH (event %lld):\t%s\n", nEvs, runHeaders[i]->GetRunEndTime().AsString());
        }
        for (Int_t iEv = 0; iEv < nEvs; iEv++) {
            trees[i]->GetEntry(iEv);
            if (iEv % 10000 == 0)
                cout << "iEv = " << iEv << endl;
            BmnEventHeader* hdr = (BmnEventHeader*)headers[i];
            preEv = curEv;
            curEv = hdr->GetEventId();
            if (curEv - preEv > 1)
                //                printf("Events between %d & %d missed\n", preEv, curEv);
                if (silDigit[i])
                    FillHists<BmnSiliconDigit>(histStrip[i], silDigit[i], set_bin_content);
            if (gemDigit[i])
                FillHists<BmnGemStripDigit>(histStripGem[i], gemDigit[i], set_bin_content);
            if (cscDigit[i])
                FillHists<BmnCSCDigit>(histStripCsc[i], cscDigit[i], set_bin_content);
            if (sibtDigit[i])
                FillHists<BmnSiBTDigit>(histStripSiBT[i], sibtDigit[i], set_bin_content);
            if (vspDigit[i])
                FillHists<BmnVSPDigit>(histStripVSP[i], vspDigit[i], set_bin_content);
        }
        printf("Last event %d\n", curEv);
    }
    if (canStrip) {
        BmnHist::DrawRef(canStrip, &canStripPads);
        canStrip->SaveAs(Form("can-run-%d-sil.png", runID));
    }
    if (canStripGem) {
        BmnHist::DrawRef(canStripGem, &canStripPadsGem);
        canStripGem->SaveAs(Form("can-run-%d-gem.png", runID));
    }
    if (canStripCsc) {
        BmnHist::DrawRef(canStripCsc, &canStripPadsCsc);
        canStripCsc->SaveAs(Form("can-run-%d-csc.png", runID));
    }
    if (canStripSiBT) {
        BmnHist::DrawRef(canStripSiBT, &canStripPadsSiBT);
        canStripSiBT->SaveAs(Form("can-run-%d-sibt.png", runID));
    }
    if (canStripVSP) {
        BmnHist::DrawRef(canStripVSP, &canStripPadsVSP);
        canStripVSP->SaveAs(Form("can-run-%d-vsp.png", runID));
    }
}
