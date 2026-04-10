#include "BmnConverterThread.h"

// ROOT
#include "TFile.h"
// BmnRoot
#include "BmnADCDigit.h"
#include "BmnAbstractTDCDigit.h"
#include "BmnConverterTools.h"
#include "BmnHRBDigit.h"
#include "BmnHgndRaw.h"
#include "BmnMSCDigit.h"
#include "BmnMSCZSDigit.h"
#include "BmnMath.h"
#include "BmnT0Raw.h"
#include "BmnTDCDigit.h"
#include "BmnTQDCADCDigit.h"
#include "BmnVspRawDigit.h"
// CBM
#include "StorableTimeslice.h"
#include "StsXyterMessage.h"

using namespace std::chrono;

void BmnConverterThread::RegisterBranches()
{   // std::function<void(TString, TObject**) > branch_reg_fun) {
    fRawTree->Branch("BmnEventHeader.", &eventHeaderDAQ);
    //    FairRootManager* frm = FairRootManager::Instance();
    //    frm->Register("BmnEventHeader.", "BmnEventHeader._dir", eventHeaderDAQ, fSaveOutput);
    fRawTree->Branch("SYNC", &sync);
    fRawTree->Branch("ADC32", &adc32);
    fRawTree->Branch("ADC64", &adc64);
    fRawTree->Branch("ADC128", &adc128);
    fRawTree->Branch("ADC", &adc);
    fRawTree->Branch("VSP_RAW", &vsp);
    fRawTree->Branch("TDC", &tdc);
    fRawTree->Branch("TQDC_ADC", &tqdc_adc);
    fRawTree->Branch("TQDC_TDC", &tqdc_tdc);
    fRawTree->Branch("TDC_HGND", &tdc_hgnd);
    fRawTree->Branch("HRB", &hrb);
}

void BmnConverterThread::RegisterSpillBranches()
{
    fRawTreeSpills->Branch("MSC", &msc);
    fRawTreeSpills->Branch("T0Raw", &t0raw);
}

void BmnConverterThread::CreateTrees()
{
    if (merger) {
        mergerFile = merger->GetFile();
        mergerFile->cd();
    }
    TString ext = (threadId == -1) ? "" : to_string(threadId);
    //    TString ext = "";//(threadId == -1) ? "" : to_string(threadId);
    fRawTree = new TTree((TString) "BMN_RAW" + ext, (TString) "BMN_RAW" + ext);
    fRawTree->SetAutoSave(0);   // Very important
    fRawTreeSpills = new TTree((TString) "BMN_RAW_SPILLS" + ext, (TString) "BMN_RAW_SPILLS" + ext);
    fRawTreeSpills->SetAutoSave(0);   // Very important
    eventHeaderDAQ = new BmnEventHeader();
    sync = new TClonesArray(BmnSyncDigit::Class());
    adc32 = new TClonesArray(BmnADCDigit::Class());
    adc64 = new TClonesArray(BmnADCDigit::Class());
    adc128 = new TClonesArray(BmnADCDigit::Class());
    adc = new TClonesArray(BmnADCDigit::Class());
    vsp = new TClonesArray(BmnVspRawDigit::Class());
    tdc = new TClonesArray(BmnTDCDigit::Class());
    tqdc_adc = new TClonesArray(BmnTQDCADCDigit::Class());
    tqdc_tdc = new TClonesArray(BmnTDCDigit::Class());
    tdc_hgnd = new TClonesArray(BmnAbstractTDCDigit::Class());
    hrb = new TClonesArray(BmnHRBDigit::Class());
    msc = new TClonesArray(fPeriodId > 7 ? BmnMSCDigit<UChar_t>::Class() : BmnMSCDigit<UInt_t>::Class());
    t0raw = new TClonesArray(BmnT0Raw<kT0_BIN_BLOCK_WORDS>::Class());
}

void BmnConverterThread::SetTrees(TTree* tree, TTree* treeSpills)
{
    fRawTree = tree;
    fRawTree->SetBranchAddress("BmnEventHeader.", &eventHeaderDAQ);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("ADC32", &adc32);
    fRawTree->SetBranchAddress("ADC64", &adc64);
    fRawTree->SetBranchAddress("ADC128", &adc128);
    fRawTree->SetBranchAddress("ADC", &adc);
    fRawTree->SetBranchAddress("VSP_RAW", &vsp);
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("TQDC_ADC", &tqdc_adc);
    fRawTree->SetBranchAddress("TQDC_TDC", &tqdc_tdc);
    fRawTree->SetBranchAddress("TDC_HGND", &tdc_hgnd);
    fRawTree->SetBranchAddress("HRB", &hrb);
    fRawTreeSpills = treeSpills;
    fRawTreeSpills->SetBranchAddress("MSC", &msc);
    fRawTreeSpills->SetBranchAddress("T0Raw", &t0raw);
}

void BmnConverterThread::WriteSpill(Int_t spillId)
{
    auto f = merger->GetFile();
    TTree* tree = new TTree("BMN_RAW", "BMN_RAW");
    TTree* treeSpills = new TTree("BMN_RAW_SPILLS", "BMN_RAW_SPILLS");
    tree->Branch("BmnEventHeader.", &eventHeaderDAQ);
    tree->Branch("SYNC", &sync);
    tree->Branch("ADC32", &adc32);
    tree->Branch("ADC64", &adc64);
    tree->Branch("ADC128", &adc128);
    tree->Branch("ADC", &adc);
    tree->Branch("VSP_RAW", &vsp);
    tree->Branch("TDC", &tdc);
    tree->Branch("TQDC_ADC", &tqdc_adc);
    tree->Branch("TQDC_TDC", &tqdc_tdc);
    tree->Branch("TDC_HGND", &tdc_hgnd);
    tree->Branch("HRB", &hrb);
    treeSpills->Branch("MSC", &msc);
    treeSpills->Branch("T0Raw", &t0raw);
    for (size_t j = 0; j < vecTree->size(); j++) {
        if ((*vecTree)[j].first == spillId) {
            if ((*vecTree)[j].second.first == threadId) {
                fRawTree->GetEntry((*vecTree)[j].second.second);
                tree->Fill();
            }
        }
    }
    for (size_t j = 0; j < vecTreeSpills->size(); j++) {
        if ((*vecTreeSpills)[j].first == spillId) {
            if ((*vecTreeSpills)[j].second.first == threadId) {
                fRawTreeSpills->GetEntry((*vecTreeSpills)[j].second.second);
                treeSpills->Fill();
            }
        }
    }
    f->Write();
}

void BmnConverterThread::Calculate()
{
    LOGF(debug2, "Calculate task %d in id %d", taskId, threadId);
    if (taskId == 1) {
        ProcessEvent(rawData, dataLen);
        // if(!isTaskMode){
        fRawTree->Fill();
        //}
    } else if (taskId == 2) {
        ConvertStatEvent(rawData, dataLen);
        SpillEnd();
    } else if (taskId == 3) {
        WriteSpill(dataLen);
    }
    LOGF(debug2, "Calculate task %d in id %d done", taskId, threadId);
}

void BmnConverterThread::WriteTree()
{
    fRawTree->Write();
}

void BmnConverterThread::WriteTreeSpills()
{
    fRawTreeSpills->Write();
}

void BmnConverterThread::FinalizeFile()
{
    //    fRawTree->Write();
    //    fRawTreeSpills->Write();
    if (mergerFile) {
        mergerFile->Write();
        mergerFile->Close();
        mergerFile.reset();
        LOGF(info, "thread file closed");
    }
}

void BmnConverterThread::SetData(Int_t id,
                                 Int_t len,
                                 UInt_t* data,
                                 BmnEventType fCurEvType,
                                 UInt_t fRunId,
                                 UInt_t fEventId)
{
    dataLen = len;
    if (data)
        std::memcpy(rawData, data, sizeof(UInt_t) * len);
    fCurEvent.fCurEvType = fCurEvType;
    fCurEvent.fRunId = fRunId;
    //    fCurEvent.fPeriodId = fPeriodId;
    fCurEvent.fEventId = fEventId;
    this->taskId = id;
}

// not actually spill end since 8th period

void BmnConverterThread::SpillEnd()
{
    if (fRawTreeSpills) {
        fRawTreeSpills->Fill();
        ClearRawSpillArrays();
    }
}

BmnStatus BmnConverterThread::ConvertStatEvent(UInt_t* d, UInt_t& len)
{
    eventHeaderDAQ->SetEventType(fCurEvent.fCurEvType);
    ProcessEvent(d, len);
    //    if (fRootFileOut) fRawTree->Fill();
    /*if ((fCurEvType == kBMNEOS) && (fPrevEvType != kBMNEOS)) {
        LOGF(debug1, "\tspill! nSpillEvents %d", nSpillEvents);
        isSpillStart = kTRUE;
        nSpillEvents = 0;
        fSpillCntr++;
        //        if (msc->GetEntriesFast() > 0)

    }*/
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::ProcessEvent(UInt_t* d, UInt_t len)
{
    //            timer.Start();
    ClearRawArrays();
    if ((fCurEvent.fEventId % 5000 <= 1) && (fCurEvent.fCurEvType != kBMNSTAT) && (fCurEvent.fEventId > 0))
        LOG(info) << "Converted event #" << fCurEvent.fEventId;
    //    } else if (fVerbose == 0)
    //        DrawBar(fCurentPositionRawFile, fLengthRawFile);

    UInt_t idx = 1;
    //    BmnEventType evType = kBMNPAYLOAD;
    BmnTrigInfo trigInfo;
    while (idx < len) {
        Bool_t recognized = kTRUE;
        DeviceHeader* dh = reinterpret_cast<DeviceHeader*>(d + idx);
        //        printf("[%u]\n", idx);
        //        if (fCurEvType == kBMNSTAT)
        //            dh->Print();
        UInt_t serial = dh->Serial;
        uint8_t id = dh->DeviceId;
        //        if (serial == 0) id = kGENERIC_DEVICE;
        UInt_t payload = dh->GetNWords();
        idx += sizeof(DeviceHeader) / kNBYTESINWORD;
        if (payload > 2000000) {
            LOGF(warn, "Event %d:\n serial = 0x%06X\n id = Ox%02X\n payload = %d", fCurEvent.fEventId, serial, id,
                 payload);
            break;
        }
        if (fCurEvent.fCurEvType == kBMNSTAT)
            LOGF(debug4, "STAT iev %7d  idx %6u/%6u   idev %02X serial 0x%08X payload %4u", fCurEvent.fEventId, idx,
                 len, id, serial, payload);
        switch (id) {
            case kTQDC16VS_E:
                FillTQDC_E(&d[idx], serial, payload);
                break;
            case kTDC72VXS:      // Trig & ToF400 VXS TDC
            case kTDC64VHLE_E:   // ToF700 VHL TDC
                FillTDC72VXS(&d[idx], serial, payload, id);
                break;
            case kTTVXS:
                FillTTVXS(&d[idx], serial, payload);
                break;
            case kGENERIC_DEVICE:
                ProcessGenericDevice(&d[idx], dh);
                break;
            case kMSC16VE_E:
                //                                printf("MSC16VE-E serial 0x%08X  words %u\n", serial, payload);
                if (msc)
                    FillMSC16VE_E(&d[idx], dh);
                break;
            case kADC64VE_XGE:
            case kADC64VE: {
                BmnConverterTools::Process_ADC64<Short_t>(&d[idx], payload, serial,
                                                          [&](Int_t n_samples) -> TClonesArray* {
                                                              switch (n_samples) {
                                                                  case 32:
                                                                      return adc32;
                                                                      break;
                                                                  case 64:
                                                                      return adc64;
                                                                      break;
                                                                  case 128:
                                                                      return adc128;
                                                                      break;
                                                                  default:
                                                                      return nullptr;
                                                                      break;
                                                              }
                                                          });
                break;
            }
            case kADC64WR: {
                BmnConverterTools::Process_ADC64<UShort_t>(&d[idx], payload, serial,
                                                           [&](Int_t n_samples) -> TClonesArray* { return adc; });
                break;
            }
            case kFVME:
                Process_FVME(&d[idx], payload, serial, trigInfo);
                break;
            case kHRB:
                Process_HRB(&d[idx], payload, serial);
                break;
            case kUT24VE_TRC:
                FillUT24VE_TRC(&d[idx], serial, payload);
                break;
            case kVIRTUAL_DEVICE:
                FillVirtualDevice(&d[idx], dh);
                break;
            default:
                //                printf("Device id %02X not recognized\n", id);
                recognized = kFALSE;
                break;
        }
        if (payload + idx > len) {
            printf(
                "Error in the event #%d: 0x%08X : %2X device payload length mismatch! pauload %5u idx/len: %5u/%5u \n",
                fCurEvent.fEventId, serial, id, payload, idx, len);
            return kBMNERROR;
        } else {
            if (recognized)
                idx += payload;
            else
                idx--;
        }
    }
    if (time_slice_map.size())
        FillVSP(nullptr);
    eventHeaderDAQ->SetRunId(fCurEvent.fRunId);
    eventHeaderDAQ->SetPeriodId(fPeriodId);
    eventHeaderDAQ->SetEventId(fCurEvent.fEventId);
    eventHeaderDAQ->SetEventTimeTS(TTimeStamp(time_t(fCurEvent.fTime_s), fCurEvent.fTime_ns));
    eventHeaderDAQ->SetEventTime(TTimeStamp(time_t(fCurEvent.fTime_s), fCurEvent.fTime_ns).AsDouble());
    eventHeaderDAQ->SetEventType(fCurEvent.fCurEvType);
    eventHeaderDAQ->SetTripWord(kFALSE);
    eventHeaderDAQ->SetTrigInfo(trigInfo);
    // eventHeaderDAQ->SetTimeShift(fTimeShifts);
    //     eventHeaderDAQ->SetStartSignalInfo(fT0Time, fT0Width);
    // eventHeaderDAQ->SetSpillStart(isSpillStart);
    // if ((fCurEvent.fCurEvType == kBMNSTAT) || (fCurEvent.fCurEvType == kBMNEOS))
    //     eventHeaderDAQ->SetEventId(0);
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::Process_FVME(UInt_t* d, UInt_t len, UInt_t serial, BmnTrigInfo& spillInfo)
{
    //                                    printf("FVME serial %08X len %u\n",serial, len);
    if (fCurEvent.fCurEvType == kBMNSTAT) {
        fCurEvent.fCurEvType = kBMNEOS;
        //        printf("stat FVME serial %08X len %u\n", serial, len);
    }
    UInt_t modId = 0;
    UInt_t slot = 0;
    UInt_t type = 0;
    for (UInt_t i = 0; i < len; i++) {
        type = d[i] >> 28;
        //        if (fCurEvType == kBMNSTAT)
        //            printf("     type %02X  i %u\n", type, i);
        switch (type) {
            case kEVHEADER:
                //                if (fCurEvType == kBMNSTAT)
                //                    printf("Ev header \n");
                break;
            case kEVTRAILER:
                //                if (fCurEvType == kBMNSTAT)
                //                    printf("Ev trailer\n");
                return kBMNSUCCESS;
                break;
            case kSTATUS:
            case kPADDING:
                break;
            case kMODHEADER:
                modId = (d[i] >> 16) & 0x7F;
                slot = (d[i] >> 23) & 0x1F;
                //                if (fCurEvType == kBMNSTAT)
                //                    printf("\tmodid 0x%02X slot %d serial 0x%08X\n", modId, slot, serial);
                break;
            case kMODTRAILER:
                //                if (fCurEvType == kBMNSTAT)
                //                    printf("\tmodule trailer\n");
                if (!((d[i] >> 16) & 0x1))
                    printf(ANSI_COLOR_RED "Readout overflow error\n" ANSI_COLOR_RESET);
                if (!((d[i] >> 17) & 0x1))
                    printf(ANSI_COLOR_RED "Readout error\n" ANSI_COLOR_RESET);
                if (!((d[i] >> 18) & 0x1))
                    printf(ANSI_COLOR_RED "TTC error\n" ANSI_COLOR_RESET);
                if (!((d[i] >> 19) & 0x1))
                    printf(ANSI_COLOR_RED "Access error\n" ANSI_COLOR_RESET);
                modId = 0x00;
                slot = 0x00;
                break;
            default:   // data
            {
                // BmnStatus parsing_status(kBMNSUCCESS);
                switch (modId) {
                    case kTDC64V:
                    case kTDC64VHLE:
                    case kTDC72VHL:
                    case kTDC32VL:
                        FillTDC(d, serial, slot, modId, i);
                        break;
                    case kTQDC16:
                    case kTQDC16VS:
                        FillTQDC(d, serial, slot, modId, i);
                        break;
                    case kMSC16V:
                        FillMSC(d, serial, slot, i);
                        break;
                    case kTRIG:
                        /*parsing_status = */ FillFVME2TMWR(d, serial, i, len);
                        break;
                    case kU40VE_RC:
                        FillU40VE(d, slot, i, spillInfo);
                        break;
                }
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::Process_HRB(UInt_t* d, UInt_t len, UInt_t serial)
{
    // UInt_t evId = d[0];
    UInt_t tH = d[1];
    UInt_t tL = d[2];
    UInt_t nWords = 4;                   // 4 words per plane (per 96 channels, why 4 words - 3 is enough???)
    UInt_t nSmpl = (len - 3) / nWords;   // 3 words are read now. Why divide by 4 (To ask Vadim)

    for (UInt_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        for (UInt_t iWord = 0; iWord < nWords; ++iWord) {
            UInt_t word32 = d[3 + iWord + iSmpl * nWords];
            for (Int_t iCh = 0; iCh < 32; ++iCh) {
                if (word32 & BIT(iCh)) {
                    TClonesArray& ar_hrb = *hrb;
                    new (ar_hrb[hrb->GetEntriesFast()]) BmnHRBDigit(serial, iCh + 32 * iWord, iSmpl, tH, tL);
                }
            }
        }
    }

    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillU40VE(UInt_t* d, UInt_t slot, UInt_t& idx, BmnTrigInfo& trigInfo)
{
    UInt_t type = d[idx] >> 28;
    //                printf("\t\tU40VE start type %u slot %u idx %u\n", type, slot, idx);
    Bool_t countersDone = kFALSE;
    while (type == kWORDTAI || type == kWORDTRIG || type == kWORDAUX) {
        if (fPeriodId > 4 && type == kWORDTRIG && slot == kEVENTTYPESLOT) {
            fCurEvent.fCurEvType = ((d[idx] & BIT(3)) >> 3) ? kBMNPEDESTAL : kBMNPAYLOAD;
            // UInt_t trigSrc = ((d[idx] >> 16) & (BIT(8) - 1));
            //             printf("EvId %6u trig source %u  evType %d\n", fEventId, trigSrc, evType);
            //             if (!( ((d[idx]>>10) & 0x1) ^ (fPeriodId >= 7 && fBmnSetup == kBMNSETUP)))
            //                 printf("Ev not Good!\n");
            //             printf("evGood %d\n", (d[idx] & BIT(10)));
        }
        if (type == kWORDAUX && !countersDone) {
            trigInfo.SetTrigCand(d[idx + 0] & 0xFFFFFFF);
            trigInfo.SetTrigAccepted(d[idx + 1] & 0xFFFFFFF);
            trigInfo.SetTrigBefo(d[idx + 2] & 0xFFFFFFF);
            trigInfo.SetTrigAfter(d[idx + 3] & 0xFFFFFFF);
            trigInfo.SetTrigRjct(d[idx + 4] & 0xFFFFFFF);
            trigInfo.SetTrigAll(d[idx + 5] & 0xFFFFFFF);
            trigInfo.SetTrigAvail(d[idx + 6] & 0xFFFFFFF);
            idx += 4;
            countersDone = kTRUE;
        }
        idx++;   // go to the next DATA-word
        type = d[idx] >> 28;
        //                    printf("\t            type %u slot %u idx %u\n", type, slot, idx);
    }

    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillTDC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t& idx)
{
    UInt_t type = d[idx] >> 28;
    while (type != kMODTRAILER) {   // data will be finished when module trailer appears
        if (type == TDC_ERROR) {
            LOGF(warning, "TDC (modID 0x%02X serial 0x%08X slot %d tdcID %d) error flags: 0x%04X", modId, serial, slot,
                 ((d[idx] >> 24) & 0xF), (d[idx] & ((1 << 15) - 1)));
            if ((d[idx] & BIT(12)) || (d[idx] & BIT(13))) {
                LOGF(warning, "TDC data loss in 0x%08X ", serial);
                return kBMNERROR;
            }
        }
        if (type == TDC_LEADING || type == TDC_TRAILING) {
            UInt_t tdcId = (d[idx] >> 24) & 0xF;
            UInt_t time =
                (modId == kTDC64V) ? (d[idx] & 0x7FFFF) : ((d[idx] & 0x7FFFF) << 2) | ((d[idx] & 0x180000) >> 19);
            UInt_t channel = (modId == kTDC64V) ? (d[idx] >> 19) & 0x1F : (d[idx] >> 21) & 0x7;
            // if (modId == kTDC64V && tdcId == 2) channel += 32;
            TClonesArray& ar_tdc = *tdc;
            new (ar_tdc[tdc->GetEntriesFast()])
                BmnTDCDigit(serial, modId, slot, (type == TDC_LEADING), channel, tdcId, time);
            if (fCurEvent.fCurEvType == kBMNSTAT)
                LOGF(debug4, "tdc  %08X : %d channel %d", serial, slot, channel);
        }
        idx++;   // go to the next DATA-word
        type = d[idx] >> 28;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillTQDC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t& idx)
{
    UInt_t type = d[idx] >> 28;
    UShort_t trigTimestamp = 0;
    UShort_t adcTimestamp = 0;
    UShort_t tdcTimestamp = 0;
    UInt_t iSampl = 0;
    UInt_t channel = 0;
    Short_t valI[ADC_SAMPLING_LIMIT];
    Bool_t inADC = kFALSE;
    while (type != kMODTRAILER) {
        if (type == TDC_ERROR) {
            LOGF(warning, "TDC (serial 0x%08X slot %d tdcID %d) error flags: 0x%04X", serial, slot,
                 ((d[idx] >> 24) & ((1 << 4) - 1)), (d[idx] & ((1 << 15) - 1)));
            if ((d[idx] & BIT(12)) || (d[idx] & BIT(13))) {
                LOGF(warning, "TQDC data loss in 0x%08X ", serial);
                return kBMNERROR;
            }
        }
        UInt_t mode = (d[idx] >> 26) & 0x3;
        if (!inADC) {   //       printf("type %d mode %d word %0X\n", type, mode, d[idx]);
            if ((mode == 0) && (type == TDC_LEADING || type == TDC_TRAILING)) {   // TDC time
                channel = (d[idx] >> 19) & 0x1F;
                UInt_t time = ((d[idx] & 0x7FFFF) << 2) | ((d[idx] >> 24) & 0x3);   // in 25 ps
                //               printf("TDC time %d channel %d\n", time, channel);
                new ((*tqdc_tdc)[tqdc_tdc->GetEntriesFast()])
                    BmnTDCDigit(serial, modId, slot, (type == TDC_LEADING), channel, 0, time, tdcTimestamp);
                //                printf("tqdc tdc %08X : %d channel %d\n", serial, slot, channel);
            } else if ((type == 4) && (mode != 0)) {   // Trig | ADC Timestamp
                channel = (d[idx] >> 19) & 0x1F;
                if (d[idx] & BIT(16)) {   // ADC TS
                    adcTimestamp = d[idx] & 0xFFFF;
                    inADC = kTRUE;
                } else {   // Trig TS
                    trigTimestamp = d[idx] & 0xFFFF;
                }
            } else if (type == TDC_EV_HEADER) {
                tdcTimestamp = d[idx] & 0xFFF;
                // UInt_t iEv = (d[idx] >> 12) & 0xFFF;
                //                printf("TDC ev header: %d\n", iEv);
            } else if (type == TDC_EV_TRAILER) {
                // UInt_t iEv = (d[idx] >> 12) & 0xFFF;
                //                printf("TDC ev trailer: %d\n", iEv);
            }
        } else {
            if ((type == 5) && (mode == 2) && (iSampl < ADC_SAMPLING_LIMIT)) {
                Short_t val = (d[idx] & ((1 << 14) - 1)) - (1 << (14 - 1));
                valI[iSampl++] = val;
            } else {
                new ((*tqdc_adc)[tqdc_adc->GetEntriesFast()])
                    BmnTQDCADCDigit(serial, channel, slot, iSampl, valI, trigTimestamp, adcTimestamp);
                inADC = kFALSE;
                iSampl = 0;
                --idx;
                //                printf("tqdc adc %08X : %d channel %d\n", serial, slot, channel);
            }
        }
        type = d[++idx] >> 28;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillBlockADC(UInt_t* d, UInt_t serial, uint8_t channel, uint16_t& len, TClonesArray* ar)
{
    int16_t valI[ADC_SAMPLING_LIMIT];
    const uint8_t NBytesInSample = 2;
    uint16_t iWord = 0;
    while (iWord < len) {
        uint16_t adcTS = d[iWord] & (BIT(16) - 1);
        uint16_t SigLen = d[iWord] >> 16;
        uint16_t NSamples = SigLen / NBytesInSample;
        uint16_t NSampleWords = SigLen / kNBYTESINWORD + ((SigLen % kNBYTESINWORD) ? 1 : 0);
        //        printf("adc len %2u ts %3u NSampleWords %u\n", NSamples, adcTS, NSampleWords);
        if (iWord + NSampleWords + 1 > len) {
            printf("Error! TQDC ADC wrong payload length! iWord %u SigLen %u len %u\n", iWord, SigLen, len);
            return kBMNERROR;
        }
        uint16_t iSampleWord = 0;
        while (iSampleWord++ < NSampleWords) {
            int16_t adcLo = static_cast<int16_t>(d[iWord + iSampleWord] & (BIT(16) - 1));
            int16_t adcHi = static_cast<int16_t>(d[iWord + iSampleWord] >> 16);
            //            printf("\tadcHi %4d  adcLow %4d\n", adcHi, adcLo);
            valI[iSampleWord * 2 - 2] = adcLo;
            valI[iSampleWord * 2 - 1] = adcHi;
        }
        // no slot id for ethernet
        if (NSamples)
            new ((*ar)[ar->GetEntriesFast()]) BmnTQDCADCDigit(serial, channel, 0, NSamples, valI, 0, adcTS);
        iWord += iSampleWord;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillBlockTDC(UInt_t* d, UInt_t serial, uint16_t& len, TClonesArray* ar, UInt_t modid = 0)
{
    // uint16_t tdcTS = 0;
    // uint8_t tdcId = 0;
    uint16_t tdcLine = 0;
    while (tdcLine < len) {
        UInt_t word = d[tdcLine];
        UInt_t bt = word >> 28;
        switch (bt) {
            case TDC_EV_HEADER: {
                // uint16_t evId = (word >> 12) & (BIT(12) - 1);
                // tdcId = (word >> 24) & (BIT(4) - 1);
                // tdcTS = word & (BIT(12) - 1);
                //                 printf("\tTDC header  ev %u TS %u\n", evId, tdcTS);
                break;
            }
            case TDC_EV_TRAILER: {
                // uint16_t evId = (word >> 12) & (BIT(12) - 1);
                // uint16_t tdcWC = word & (BIT(12) - 1);
                //                 printf("\tTDC trailer ev %u WC %u\n", evId, tdcWC);
                break;
            }
            case TDC_LEADING:
            case TDC_TRAILING: {
                uint8_t channel = (word >> 21) & (BIT(7) - 1);
                //                uint16_t time = (word>>2) & (BIT(19) - 1);
                UInt_t time = word & (BIT(21) - 1);
                //                printf("\tTDC %s ch %u id %d time %u\n", (bt == TDC_LEADING) ? "leading" : "trailing",
                //                channel, tdcId, time);
                new ((*ar)[ar->GetEntriesFast()])
                    BmnTDCDigit(serial, modid, 0, (bt == TDC_LEADING), channel, 0, time, 0);   // ignore tdcId in TQDC
                break;
            }
            case TDC_ERROR:
                LOGF(warning, "TDC (serial 0x%08X tdcID %d) error flags: 0x%04X %s", serial,
                     ((word >> 24) & ((1 << 4) - 1)), (word & ((1 << 15) - 1)),
                     ((word & BIT(12)) || (word & BIT(13))) ? "data lost" : "");
                break;
        }
        tdcLine++;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillTQDC_E(UInt_t* d, UInt_t serial, UInt_t& len)
{
    UInt_t index = 0;
    // MStreamHeader* ms = reinterpret_cast<MStreamHeader*> (d);
    index += sizeof(MStreamHeader) / kNBYTESINWORD;
    //    ms->Print();
    MStreamTAI* ms0 = reinterpret_cast<MStreamTAI*>(d + index);
    if (ms0->valid())
        FillWR(serial, fCurEvent.fEventId, ms0->Sec, ms0->NSec);
    index += sizeof(MStreamTAI) / kNBYTESINWORD;
    //    printf("len %u msHeader len %u\n", len, ms.Len / kNBYTESINWORD);
    //    printf("taiFlags %u TAI %s\n",
    //            ms0->Flags, TTimeStamp(time_t(ms0->Sec), ms0->NSec).AsString());
    while (index < len) {
        TqdcDataHeader* th = reinterpret_cast<TqdcDataHeader*>(d + index);
        index += sizeof(TqdcDataHeader) / kNBYTESINWORD;
        //                printf("TQDC DataType %u channel %2u adcBits %u len %4u  %8X\n", th->DataType, th->Chan,
        //                th->AdcBits, th->Len,*th);
        uint16_t blockLen = th->Len / kNBYTESINWORD;
        switch (th->DataType) {
            case kMSTREAM_TYPE_TDC:
                FillBlockTDC(d + index, serial, blockLen, tqdc_tdc);
                break;
            case kMSTREAM_TYPE_ADC:
                FillBlockADC(d + index, serial, th->Chan, blockLen, tqdc_adc);
                break;
            default:
                printf("Wrong TQDC data type %u !\n", th->DataType);
                break;
        }
        index += blockLen;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillTDC72VXS(UInt_t* d, UInt_t serial, UInt_t& len, UInt_t modid = 0)
{
    UInt_t index = 0;
    //    MStreamHeader* ms = reinterpret_cast<MStreamHeader*> (d);
    index += sizeof(MStreamHeader) / kNBYTESINWORD;
    //    ms->Print();
    MStreamTAI* ms0 = reinterpret_cast<MStreamTAI*>(d + index);
    if (ms0->valid())
        FillWR(serial, fCurEvent.fEventId, ms0->Sec, ms0->NSec);
    //        printf("taiFlags %u TAI %s\n",
    //                ms0->Flags, TTimeStamp(time_t(ms0->Sec), ms0->NSec).AsString());
    index += sizeof(MStreamTAI) / kNBYTESINWORD;
    while (index < len) {
        uint8_t dtype = d[index] >> 28;
        bool overflow = d[index] & BIT(16);
        uint16_t blockLen = (d[index++] & (BIT(16) - 1)) / kNBYTESINWORD;
        if (!overflow)
            switch (dtype) {
                case kMSTREAM_TYPE_TDC:
                    //                    printf("TDC at index %4u  len %4u\n", index, blockLen);
                    FillBlockTDC(d + index, serial, blockLen, tdc, modid);
                    break;
                case kMSTREAM_TYPE_STAT:
                    break;
                default:
                    printf("Wrong VXS data type %u !\n", dtype);
                    break;
            }
        index += blockLen;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillTTVXS(UInt_t* d, UInt_t serial, UInt_t& len)
{
    UInt_t index = 0;
    // MStreamHeader* ms = reinterpret_cast<MStreamHeader*> (d);
    index += sizeof(MStreamHeader) / kNBYTESINWORD;
    //    ms->Print();
    MStreamTAI* ms0 = reinterpret_cast<MStreamTAI*>(d + index);
    index += sizeof(MStreamTAI) / kNBYTESINWORD;
    if (ms0->valid())
        FillWR(serial, fCurEvent.fEventId, ms0->Sec, ms0->NSec);
    //    LOGF(debug, "ts :\t %s TTVXS 0x%08X valid %d",
    //            TTimeStamp(time_t(ms0->Sec), ms0->NSec).AsString(), serial, ms0->valid());
    //    LOGF(debug, "ts :\t %10u %9u TTVXS 0x%08X", ms0->Sec, ms0->NSec, serial);
    while (index < len) {
        uint8_t dtype = d[index] >> 28;
        bool overflow = d[index] & BIT(16);
        uint16_t blockLen = (d[index++] & (BIT(16) - 1)) / kNBYTESINWORD;
        //        printf("type 0x%02X  len %4u\n", dtype, blockLen);
        if (!overflow)
            switch (dtype) {
                case kMSTREAM_TYPE_TRIG: {
                    //                    printf("Trig at index %4u  len %4u\n", index, blockLen);
                    //                    uint8_t trigType = (d[index] << 8) & (BIT(8) - 1);
                    //                    uint8_t trigSrc = d[index] & (BIT(8) - 1);
                    //                    printf("Trig type %4u  src %4u\n", trigType, trigSrc);
                } break;
                case kMSTREAM_TYPE_STAT:
                    break;
                default:
                    printf("Wrong TTVXS data type %u !\n", dtype);
                    break;
            }
        index += blockLen;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::ProcessGenericDevice(UInt_t* d, DeviceHeader* dev_hdr)
{
    //    dev_hdr->Print();
    if ((dev_hdr->Serial & 0x00FFFFFF) == kVSP_SERIALS)
        return MapVSP(d, dev_hdr);
    else if (dev_hdr->Serial == kHGND_SERIALS)
        return FillTDC250HGND(d, dev_hdr->Serial, dev_hdr->Len);
    else
        LOGF(error, "BmnRawSource::ProcessGenericDevice: Unknown device serial 0x%08X", dev_hdr->Serial);

    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillTDC250HGND(UInt_t* d, UInt_t serial, UInt_t len)
{
    if (!d || len == 0)
        return kBMNSUCCESS;

    uint16_t* words = reinterpret_cast<uint16_t*>(d);
    size_t total_words = len / sizeof(uint16_t);
    size_t index = 0;

    std::vector<uint16_t> event_words;
    std::unique_ptr<BmnHgndRaw::FIFO_block> event;

    while (index < total_words) {
        uint16_t* word = words + index;
        if (!BmnHgndRaw::Block::is_word_event_header(*word)) {
            LOGF(error, "BmnRawSource::FillTDC250HGND: Wrong header 0x%04X", *word);
            return kBMNERROR;
        }

        auto event_type = BmnHgndRaw::Block::get_event_packet_version(*word);
        uint32_t event_length = BmnHgndRaw::Block::get_block_size(event_type);   // in 16-bit words

        LOGF(debug4, "Event type %d, 16bit words %d (index=%zu, total=%zu)", (uint32_t)event_type, event_length, index,
             total_words);

        if (index + event_length > total_words) {
            LOGF(error, "BmnRawSource::FillTDC250HGND: Event block overruns buffer");
            return kBMNERROR;
        }

        event_words.assign(word, word + event_length);
        if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug4)) {
            for (size_t i = 0; i < event_words.size(); ++i) {
                LOGF(debug4, "0x%04x", event_words[i]);
            }
        }

        if (event_type == BmnHgndRaw::Block::EventPacketVersion::V1) {
            event = std::make_unique<BmnHgndRaw::Event_type_1>(event_words);
        } else if (event_type == BmnHgndRaw::Block::EventPacketVersion::V2) {
            event = std::make_unique<BmnHgndRaw::Event_type_2>(event_words);
            if (!event->validate()) {
                LOGF(error, "BmnRawSource::FillTDC250HGND: Event block validation failed");
                return kBMNERROR;
            }

            auto* ev2 = dynamic_cast<BmnHgndRaw::Event_type_2*>(event.get());
            if (!ev2) {
                LOGF(error, "BmnRawSource::FillTDC250HGND: Event cast to Event_type_2 failed");
                return kBMNERROR;
            }

            uint8_t trise_100ps = ev2->TDC_rise_time;      // raw time rise
            uint8_t tfall_100ps = ev2->TDC_falling_time;   // raw time fall
            int64_t time_sec = ev2->timestamp >> 31;       // timestamp, seconds part
            time_sec -= tai_utc_dif;
            uint64_t time_100ps = ((ev2->timestamp & 0x7ffffff0) >> 4) * 320;   // timestamp, ns part. 32ns step
            time_100ps += (ev2->timestamp & 0xf) * 32;                          // 3.2ns step
            time_100ps += tfall_100ps;                                          // 100ps step
            uint64_t time_nsec = time_100ps / 10;
            uint16_t residual_100ps = static_cast<uint16_t>(time_100ps % 10);   // 100ps step subns part. trise
            uint16_t plen_100ps =
                (ev2->pulse_length + 1) * 32 + tfall_100ps - trise_100ps;   // pulse len x100ps +fall -rise

            LOGF(debug4, "ch_num = 0x%02X", ev2->channel_number);
            LOGF(debug4, "timestamp = 0x%016lX", ev2->timestamp);
            LOGF(debug4, "time_sec = 0x%08X", time_sec);
            LOGF(debug4, "time_nsec = 0x%08X", time_nsec);
            LOGF(debug4, "trise_100ps = 0x%02X", trise_100ps);
            LOGF(debug4, "tfall_100ps = 0x%02X", tfall_100ps);
            LOGF(debug4, "time_100ps = 0x%04X", residual_100ps);
            LOGF(debug4, "plen_100ps = 0x%04X", plen_100ps);
            LOGF(debug4, "global_ch = 0x%04X", ev2->channel_number_global);

            new ((*tdc_hgnd)[tdc_hgnd->GetEntriesFast()]) BmnAbstractTDCDigit(
                serial, ev2->channel_number_global, time_sec, time_nsec, residual_100ps, plen_100ps);
        } else {
            LOGF(error, "BmnRawSource::FillTDC250HGND: Wrong event type");
            return kBMNERROR;
        }

        index += event_length;
    }

    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::MapVSP(UInt_t* d, DeviceHeader* dev_hdr)
{
    LOGF(debug, "MapVSP");
    if (dev_hdr->GetNWords() < 3) {
        LOGF(warning, "Empty VSP block");
        return kBMNERROR;
    }
    uint8_t iComp = (dev_hdr->Serial & 0xFF000000) >> 24;
    time_slice_map[iComp] = d;
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillVSP(TClonesArray* ar)
{
    LOGF(debug, "FillVSP");
    const uint8_t kuMaxComps = 10;                    // maximum AFCKs (comps)
    uint16_t fvuCurrentTsMsbCycle[kuMaxComps] = {};   // FLES Timestamp cycling
    uint64_t fvulCurrentTsMsb[kuMaxComps] = {};       // FLES Ts_Msb buffer values
    unique_ptr<fles::StorableTimeslice> stti(new fles::StorableTimeslice(1));
    uint8_t nComp = time_slice_map.size();
    //    if (nComp != 2)
    //      LOGF(warning, "\t ncomps = %lu", nComp);
    // append components with a single timeslice
    for (uint8_t iw01 = 0; iw01 < nComp; iw01++)
        (*stti).append_component(1);
    // append microslice #0 to each component
    for (auto& el : time_slice_map) {
        uint8_t iw01 = el.first;
        auto* mslice_descr = el.second;
        (*stti).append_microslice(iw01, 0, *((fles::MicrosliceDescriptor*)mslice_descr),
                                  (uint8_t*)mslice_descr + sizeof(fles::MicrosliceDescriptor));
    }
    for (uint8_t uw02 = 0; uw02 < nComp; uw02++) {
        // microslice descriptor #0 (must be only one microslice) of component (FEB) #uw02
        const fles::MicrosliceDescriptor& msd = (*stti).descriptor(uw02, 0);
        // get the current DPB idx
        uint16_t fuCurrentEquipmentId = static_cast<uint16_t>(msd.eq_id);
        uint16_t fusCurrentDpbIdx = static_cast<uint32_t>(fuCurrentEquipmentId & 0xFFFF);

        // get the trigger number from microslice
        uint64_t tgn = msd.idx;
        if (tgn != fCurEvent.fEventId)
            LOGF(warning, "!!! Misplaced VSP event!  tgn = %lu while EventId = %u !!!\n", tgn, fCurEvent.fEventId);
        uint32_t tgNev = static_cast<uint32_t>(tgn);   // Ex24
        //      printf("fles tgN = %lu\t",tgn); printf("fles CurrDpbId = %u\n", fusCurrentDpbIdx);
        //      if ( tgn% kuUpdateTgN==0 ) {
        //	printf("ev. tgN = %u\t",tgNev);
        //	printf("fles tgN = %lu\t",tgn);
        //	printf("fles CurrDpbId = %u\n", fusCurrentDpbIdx);}
        // do what is needed with the microslice

        uint32_t size = 0;          // the buffer size of the Microslice
        uint32_t uNbMessages = 0;   // the number of complete messages
        // double dTriggerTime = 0;      // TrigTime in ns
        uint64_t uiTrgTimeFLES = 0;   // TrigTime in clocks
        uint64_t uiHitTimeFLES = 0;   // Hits Time in clocks

        size = msd.size;
        static const uint32_t kuBytesPerMessage = 4;
        // Compute the number of complete messages in the input microslice buffer
        uNbMessages = (size - (size % kuBytesPerMessage)) / kuBytesPerMessage;
        // Prepare variables for the loop on contents
        // const uint32_t* pInBuff = reinterpret_cast<const uint32_t*>( (*stti).content(0,0) );
        const uint32_t* pInBuff = reinterpret_cast<const uint32_t*>((*stti).content(uw02, 0));

        // uiTrgTimeFLES = 0;
        uint64_t uiTrgTime_TsMsb = 0;
        uint64_t uiHitTime_TsMsb = 0;
        uint64_t uiTrgTime_Epoch = 0;

        uint16_t uiTsMsbMessageNumber = 0;   // to take only the 1st TsMsb message
        uint16_t uiEpochMessageNumber = 0;   // to take only the 1st TsMsb message

        uint16_t usHitMissedEvts = 0;   // local counter of Hits with the Missed Events Flag
        if (uNbMessages == 0)
            continue;
        BmnVspRawDigit* dig = new ((*vsp)[vsp->GetEntriesFast()]) BmnVspRawDigit(tgn, fusCurrentDpbIdx);

        for (uint32_t uIdx = 0; uIdx < uNbMessages; ++uIdx) {
            // Fill message
            uint32_t ulData = static_cast<uint32_t>(pInBuff[uIdx]);

            stsxyter::Message mess(static_cast<uint32_t>(ulData & 0xFFFFFFFF));
            stsxyter::MessType typeMess = mess.GetMessType();
            // if (fbTxtFileOutEna) *foutHitInfoUNI << "typeMess " <<  static_cast< uint16_t >(  typeMess) << std::endl;
            switch (typeMess) {
                case stsxyter::MessType::Epoch: {
                    // The first message in the TS is a special ones: EPOCH
                    uiEpochMessageNumber++;
                    // dTriggerTime = mess.GetEpochVal() * stsxyter::kdClockCycleNs;
                    if (uiEpochMessageNumber == 1)
                        uiTrgTime_Epoch = mess.GetEpochVal();   //  all 29 bits, may24
                    else
                        printf("fles WARNING: more than one EPOCH message in event tgN = %lu, exactly: %u\n", tgn,
                               uiEpochMessageNumber);
                    break;
                }
                case stsxyter::MessType::TsMsb: {
                    uiTsMsbMessageNumber++;
                    uint64_t uVal = mess.GetTsMsbValBinning();
                    if (uVal < fvulCurrentTsMsb[uw02]) {
                        printf("TsMsbCycle: event trigger# %u", tgNev);
                        printf("\tDPB: %u, Old TsMsb= %lu, new TsMsb=%lu, Old MsbCy=%u\n", uw02, fvulCurrentTsMsb[uw02],
                               uVal, fvuCurrentTsMsbCycle[uw02]);

                        if ((fvulCurrentTsMsb[uw02] - uVal) > 10000000)
                            fvuCurrentTsMsbCycle[uw02]++;

                    }   // if( uVal < fvulCurrentTsMsb[uw02] )
                    fvulCurrentTsMsb[uw02] = uVal;
                    uiHitTime_TsMsb = uVal << 10;   // shift to 10 bits;
                    uiHitTime_TsMsb += static_cast<uint64_t>(stsxyter::kulTsCycleNbBinsBinning)
                                       * fvuCurrentTsMsbCycle[uw02];   // TsMsbCycle added
                    if (uiTsMsbMessageNumber == 1) {
                        uiTrgTime_TsMsb =
                            uiHitTime_TsMsb & 0xFFE0000000;   // remove the lower 29 bits, the faster procedure
                    }
                    break;
                }   // case stsxyter::MessType::TsMsb :
                case stsxyter::MessType::Hit: {
                    uint16_t usRawTs = static_cast<uint16_t>(mess.GetHitTimeBinning());
                    uiHitTimeFLES = uiHitTime_TsMsb + static_cast<uint64_t>(usRawTs);
                    uint16_t usElinkIdx = mess.GetLinkIndexHitBinning();
                    uint16_t usChan = mess.GetHitChannel();
                    uint16_t usRawAdc = mess.GetHitAdc();

                    if (mess.IsHitMissedEvts())
                        usHitMissedEvts++;
                    dig->AppendHit(uiHitTimeFLES, usElinkIdx, usChan, usRawAdc);
                    break;
                }
                default:
                    break;
            }   // switch( typeMess )

        }   // for( uint32_t uIdx = 0; uIdx < uNbMessages; ++uIdx )
        dig->SetTHitMissedEvts(usHitMissedEvts);
        uiTrgTimeFLES = uiTrgTime_TsMsb + uiTrgTime_Epoch;
        dig->SetTTrigTime(uiTrgTimeFLES);
    }
    time_slice_map.clear();
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillMSC16VE_E(UInt_t* d, DeviceHeader* dev_hdr)
{
    //    msc->Delete();
    UInt_t index = 0;
    const uint32_t len = dev_hdr->GetNWords();
    MSC16VE_EHeader* ms = reinterpret_cast<MSC16VE_EHeader*>(d);
    index += sizeof(MSC16VE_EHeader) / kNBYTESINWORD;
    //        ms->Hdr.Print();
    if (ms->Hdr.Len > (len - 1)) {
        LOGF(error, "MSC header payload length mismatch!");
        return kBMNERROR;
    }
    //   LOGF(info, "MSC TAI\t%s taiFlags %u ", TTimeStamp(time_t(ms->Tai.Sec), ms->Tai.NSec).AsString(),
    //   ms->Tai.Flags); LOGF(info, "MSC ver %u  CntrBitsNum %u  SliceInt %3u ns  ChanNum %2u  ExtCondNum %u",
    //       ms->GetVersion(), ms->NCntrBits, ms->SliceInt, ms->ChanNumber, ms->ExtCondCnt);
    Bool_t hasValues = kFALSE;
    std::array<uint8_t, MSC_N_COUNTERS> cntrs = {};
    switch (ms->GetVersion()) {
        case 1: {
            const uint8_t NumsInWord = 4;
            while (index < len) {
                uint8_t type = d[index] >> 28;
                //        printf(" index %4u  len %4u   type %u\n", index, len, type);
                if (type == MSC_TIME_SLICE_INFO) {
                    if (hasValues) {
                        UInt_t ext_cond = (d[index] >> 24) & (BIT(4) - 1);
                        UInt_t sliceNum = (d[index]) & (BIT(24) - 1);
                        auto sc_time_shift = ms->SliceInt * sliceNum * 1ns;
                        SysPoint p{(ms->Tai.Sec - tai_utc_dif) * 1s + ms->Tai.NSec * 1ns + sc_time_shift};
                        LOGF(debug, "MSC ext_cond %2u  sliceNum %7u  time %s", ext_cond, sliceNum,
                             BmnFunctionSet::TimePoint2String(p).c_str());
                        new ((*msc)[msc->GetEntriesFast()])
                            BmnMSCDigit<uint8_t>(dev_hdr->Serial, 0, cntrs, fCurEvent.fEventId, p, ext_cond);
                        hasValues = kFALSE;
                        memset(cntrs.data(), 0, sizeof(UChar_t) * MSC_N_COUNTERS);
                    }
                } else {
                    for (uint8_t i = 0; i < NumsInWord; i++) {
                        UInt_t cnt = (d[index] >> (i * ms->NCntrBits)) & (BIT(ms->NCntrBits) - 1);
                        cntrs[type * NumsInWord + i] = cnt;
                        //                LOGF(info,"\tcnt[%2u] = %2u", (type * NumsInWord + i), cnt);
                        hasValues = kTRUE;
                    }
                }
                index++;
            }
        } break;
        case 2: {
            UInt_t missing_hits = d[index++];
            if (missing_hits)
                LOGF(info, "missing_hits %3u !!!", missing_hits);
            LOGF(debug, "missing_hits %u", missing_hits);
            //            uint8_t channel = ms->ChanNumber;
            //            TimeDist& scaler_cnts = metadata->Scalers()[make_pair(serial, channel)];
            //            TimeDist& ext_cnts = metadata->ExtConditions();
            SysPoint point{(ms->Tai.Sec - tai_utc_dif) * 1s + ms->Tai.NSec * 1ns};
            uint32_t n_slices = ms->Hdr.Len - 1 - (sizeof(MSC16VE_EHeader) - sizeof(MStreamHeader)) / kNBYTESINWORD;
            BmnMSCZSDigit<uint8_t>* dig = new ((*msc)[msc->GetEntriesFast()])
                BmnMSCZSDigit<uint8_t>(dev_hdr->Serial, ms->ChanNumber, point, n_slices);
            vector<uint8_t>& vals = dig->GetValues();
            vector<SysPoint>& times = dig->GetTimes();
            vector<uint8_t>& conds = dig->GetExtCond();
            int32_t istart = index;
            while (index < len) {
                uint8_t cnt = d[index] & (BIT(ms->NCntrBits) - 1);
                uint8_t ext_cond = (d[index] >> ms->NCntrBits) & (BIT(ms->ExtCondCnt) - 1);
                UInt_t sliceNum = d[index] >> (ms->NCntrBits + ms->ExtCondCnt);
                auto sc_time_shift = ms->SliceInt * sliceNum * 1ns;
                SysPoint point_slice{point + sc_time_shift};
                int32_t iVal = index - istart;
                //                                LOGF(info, "iVal %4u", iVal);
                vals[iVal] = cnt;
                times[iVal] = point_slice;
                conds[iVal] = ext_cond;
                //                                LOGF(info, "MSC i %2u  len %3u", index, len);
                LOGF(debug, "MSC ext_cond %2u cnt %2u sliceNum %7u  time %s", ext_cond, cnt, sliceNum,
                     BmnFunctionSet::TimePoint2String(point_slice).c_str());

                //                scaler_cnts.insert(make_pair(p, cnt));
                //                ext_cnts.insert(make_pair(p, ext_cond));

                //                cntrs[ms->ChanNumber] = cnt;
                //                new ((*msc)[msc->GetEntriesFast()]) BmnMSCDigit<uint8_t>(serial, 0, cntrs, fEventId,
                //                p, ext_cond); memset(cntrs.data(), 0, sizeof(UChar_t) * MSC_N_COUNTERS);
                index++;
            }
        } break;
        default:
            LOGF(warning, "MSC16 ver $u not implemented", ms->GetVersion());
            return kBMNERROR;
            break;
    }
    //    if ((fRootFileOut) && (msc->GetEntries()))
    //        fRawTreeSpills->Fill();
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillVirtualDevice(UInt_t* d, DeviceHeader* dh)
{
    LOGF(info, "Found virtual device 0x%08X block", dh->Serial);
    LOGF(info, "Ev %u  type %u", fCurEvent.fEventId, (UInt_t)(fCurEvent.fCurEvType));
    if (dh->Serial != SERIAL_TO_CFG) {
        LOGF(info, "Not T0 device");
        return kBMNSUCCESS;
    }
    const Int_t bin_block_len = kT0_BIN_BLOCK_WORDS * kNBYTESINWORD;
    if (fExportExternalSpillStat) {
        std::ofstream outfile(Form("virt_device_run_%u_%08X_%02X_t_%lld_%lld.txt", fCurEvent.fRunId, dh->Serial,
                                   dh->DeviceId,   // fSubName.Data(),
                                   fCurEvent.fTime_s, fCurEvent.fTime_ns),
                              ios::out | ios::binary);
        for (UInt_t i = 0; i < kT0_BIN_BLOCK_WORDS; i++)
            outfile << std::hex << d[i];
        outfile.write(reinterpret_cast<const char*>(d) + bin_block_len, dh->Len - bin_block_len);
    }
    //    t0raw->Delete();
    auto lastT0 = new ((*t0raw)[t0raw->GetEntriesFast()]) BmnT0Raw<kT0_BIN_BLOCK_WORDS>(d);
    const Int_t date_len = 19;   // 23.12.2022 14:17:46
    TString str(reinterpret_cast<const char*>(d + kT0_BIN_BLOCK_WORDS), date_len);
    TPRegexp re_str_cfg_t0("(\\d{2})\\.(\\d{2})\\.(\\d{4})\\s(\\d{2}:\\d{2}:\\d{2})");
    int32_t re_subst_ret = re_str_cfg_t0.Substitute(str, "$3-$2-$1 $4");
    LOGF(info, "ts line return %d", re_subst_ret);
    for (UInt_t i = 0; i < kT0_BIN_BLOCK_WORDS; i++) {
        LOGF(debug, "cntr[%3d]: %8u", i, d[i]);
    }
    uint64_t t_2020_sec(d[kT0_BIN_BLOCK_2020_SEC]);
    uint64_t t_unix_sec(*reinterpret_cast<uint64_t*>(d + kT0_BIN_BLOCK_UNIX_SEC));
    //    LOGF(info, "t_2020_sec %u", t_2020_sec);
    //    LOGF(info, "t_unix_sec %u", t_unix_sec);
    TTimeStamp ts_unix_sec(t_unix_sec, 0);
    TTimeStamp ts_2020_sec(t_2020_sec + TTimeStamp(2020, 1, 1, 0, 0, 0), 0);
    LOGF(info, "unix ts: %s", ts_unix_sec.AsString());
    LOGF(info, "2020 ts: %s", ts_2020_sec.AsString());
    uint32_t trig_bit_conf(d[kT0_BIN_BLOCK_WORDS - 1]);
    lastT0->SetTriggerMask(trig_bit_conf);
    bitset<32> trig_bit_conf_bs(trig_bit_conf);
    LOGF(debug, "trig bitset %u", trig_bit_conf);
    LOGF(debug, "trig bitset %s", trig_bit_conf_bs.to_string());
    uint32_t pos = kT0_BIN_BLOCK_WORDS * kNBYTESINWORD + date_len;
    string tcfg(reinterpret_cast<const char*>(d) + pos, dh->Len - pos);
    BmnConverterTools::ParseRawT0TextConfig(tcfg, lastT0);

    ts_unix_sec =
        TTimeStamp(ts_unix_sec.GetSec() - 3 * 3600,
                   ts_unix_sec.GetNanoSec());   // crutch: shifting time zone from Moscow -> GMT for uniformity
    if (t_unix_sec > 0)
        lastT0->SetTS(ts_unix_sec);
    else {
        if (t_2020_sec > 0) {
            lastT0->SetTS(ts_2020_sec);
        } else if (re_subst_ret > 0) {
            TDatime dt(str);
            TTimeStamp ts_from_string(static_cast<UInt_t>(dt.GetDate()), static_cast<UInt_t>(dt.GetTime()), 0u);
            LOGF(info, "ts line: %s", ts_from_string.AsString());
            ts_from_string = TTimeStamp(
                ts_from_string.GetSec() - 3 * 3600,
                ts_from_string.GetNanoSec());   // crutch: shifting time zone from Moscow -> GMT for uniformity
                                                //    struct tm tm_time;
                                                //    strptime(str.Data(), "%d.%m.%Y %H:%M:%S", &tm_time);
                                                //    std::time_t time_t_time = std::mktime(&tm_time);
                                                //    SysPoint time_point = SysClock::from_time_t(time_t_time);
            //    time_point -= 3h;   // crutch: shifting time zone from Moscow -> GMT for uniformity
            //    LOG(info) << "T0 point: " << BmnFunctionSet::TimePoint2String(time_point);
            //    lastT0->SetTime(time_point);
            lastT0->SetTS(ts_from_string);
        } else
            LOGF(warning, "Unable to find raw T0 block timestamp!");
    }
    auto tsle =
        lastT0->GetTime() - BmnFunctionSet::TimeStamp2TP(TTimeStamp(time_t(fCurEvent.fTime_s), fCurEvent.fTime_ns));
    //    lastT0->SetTimeSinceLastEv(tsle);
    // Get time since last event (surely it will be incorrect for several ns, but at average precise enough to assign it
    // to the spill)
    lastT0->SetTimeSinceLastEvNs(duration_cast<nanoseconds>(tsle).count());
    //    if (fRootFileOut)
    //        fRawTreeSpillsT0->Fill();
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillUT24VE_TRC(UInt_t* d, UInt_t& serial, UInt_t& len)
{
    UInt_t index = 0;
    MStreamHeader* ms = reinterpret_cast<MStreamHeader*>(d);
    index += sizeof(MStreamHeader) / kNBYTESINWORD;
    //    ms->Print();
    // start bit input time in the MStreamTAI, and should be the 4th word timeslice
    MStreamTAI* ms0 = reinterpret_cast<MStreamTAI*>(d + index);
    if (ms0->valid()) {
        BmnSyncDigit* sd = FillWR(serial, fCurEvent.fEventId, ms0->Sec, ms0->NSec);
        fCurEvent.fTime_ns = sd->GetTime_ns();
        fCurEvent.fTime_s = sd->GetTime_sec();
    }
    index += sizeof(MStreamTAI) / kNBYTESINWORD;
    if (ms->Len / kNBYTESINWORD > len)
        LOGF(error, "UT24VE-TRC Error! MSHeader payload length larger than from device header!");
    fCurEvent.fCurEvType = (d[index] & BIT(16)) ? kBMNPEDESTAL : kBMNPAYLOAD;
    //     bool randomTrigger = d[index] & BIT(17);
    //     bool periodicTrigger = d[index] & BIT(18);
    //     bool externalTTL = d[index] & BIT(19);
    eventHeaderDAQ->SetInputSignalsAR(d[index++]);
    eventHeaderDAQ->SetInputSignalsBR(d[index]);
    vector<uint32_t>& vec = eventHeaderDAQ->GetInputSignalsVector();
    while (++index < len) {
        uint32_t sig_states = d[index];
        vec.push_back(sig_states);
        bitset<32> sig_states_bs(sig_states);
        LOGF(debug2, "TRC states d[%3u] %s", index, sig_states_bs.to_string());
    }
    return kBMNSUCCESS;
}

BmnSyncDigit* BmnConverterThread::FillWR(UInt_t serial, ULong64_t iEvent, Long64_t t_sec, Long64_t t_ns)
{
    //    LOGF(debug4, "Fill WR %08X", serial);
    //    LOGF(info, "Fill WR %08X   RS ts %s", serial, TTimeStamp(time_t(t_sec), t_ns).AsString());
    if (tai_utc_dif == 0) {
        tai_utc_dif = GetUTCShift(TTimeStamp(time_t(t_sec), t_ns));
        if (tai_utc_dif == 0)
            LOGF(warn, "Possibly wrong state of %08X", serial);
    }
    return new ((*sync)[sync->GetEntriesFast()]) BmnSyncDigit(serial, iEvent, t_sec - tai_utc_dif, t_ns);
}

BmnStatus BmnConverterThread::FillFVME2TMWR(UInt_t* d, UInt_t serial, UInt_t& idx, UInt_t& len)
{
    while (idx < len) {
        UInt_t word = d[idx];
        UInt_t id = word >> 28;
        switch (id) {
            case TMWR_TAI: {
                UInt_t d0 = d[idx + 0];
                UInt_t d1 = d[idx + 1];
                UInt_t d2 = d[idx + 2];
                UInt_t d3 = d[idx + 3];
                if ((d0 >> 28) != 2 || (d1 >> 28) != 2 || (d2 >> 28) != 2 || (d3 >> 28) != 2)
                    return kBMNERROR;   // check TAI code
                Long64_t ts_t0_s = -1;
                Long64_t ts_t0_ns = -1;
                Long64_t GlobalEvent = -1;
                ts_t0_ns = (d0 & 0x0FFFFFFF) | ((d1 & 0x3) << 28);
                ts_t0_s = ((d1 >> 4) & 0xFFFFFF) | ((d2 & 0xFFFF) << 24);
                GlobalEvent = ((d3 & 0x0FFFFFFF) << 12) | ((d2 >> 16) & 0xFFF);
                BmnSyncDigit* sd = FillWR(serial, GlobalEvent, ts_t0_s, ts_t0_ns);
                if (fPeriodId < 8) {
                    fCurEvent.fTime_ns = sd->GetTime_ns();
                    fCurEvent.fTime_s = sd->GetTime_sec();
                }
                idx += 4;
                LOGF(debug3, "ts :\t %10lli %9lli TMWR  0x%08X", ts_t0_s, ts_t0_ns, serial);
            } break;
            case TMWR_REL_TS: {
                UInt_t d0 = d[idx + 0];
                UInt_t d1 = d[idx + 1];
                if (((d0 >> 28) != 4) || ((d1 >> 28) != 5))
                    return kBMNERROR;
                // Int_t rts(((d0 & (BIT(24) - 1)) << 8) | ((d1 >> 20) & (BIT(8) - 1)));
                // UInt_t ext_word((d1 >> 16) & (BIT(4) - 1));
                // UInt_t trg_word(d1 & (BIT(16) - 1));
                idx += 2;
                //                printf("rts %7d  ext_tr word %u  tr word %u\n" , rts, ext_word, trg_word);
            } break;
            case TMWR_EOS_CNT_MATCHED:
            case TMWR_EOS_CNT: {
                // UInt_t d0 = d[idx + 0];
                // UInt_t logic_cnt(d0 & (BIT(28) - 1));
                //                 printf("logic_cnt %7u %s\n" , logic_cnt, id == TMWR_EOS_CNT_MATCHED ? "matched" :
                //                 "");
                idx++;
            } break;
            case TMWR_PULSE:
            case TMWR_REL_TS_TB:
                idx++;   // just skip
                break;
            case kMODTRAILER:
            case kSTATUS:
            case kPADDING:
                idx--;
                return kBMNSUCCESS;
            default:
                printf("unrecognized TMWR id %u\n", id);
                idx++;
                break;
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnConverterThread::FillMSC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t& idx)
{
    UInt_t type = d[idx] >> 28;
    UInt_t iCnt = 0;
    BmnMSCDigit<UInt_t>* dig =
        new ((*msc)[msc->GetEntriesFast()]) BmnMSCDigit<UInt_t>(serial, slot, fCurEvent.fEventId);
    std::array<UInt_t, MSC_N_COUNTERS>& cntrArrCur = dig->GetValue();
    //    printf("MSC type %u serial %08X last eventID = %6u\n", type, serial, fEventId);
    while (type < 6) {
        if (type < 5) {
            // UInt_t cnt3 = (d[idx] >> 21) & (BIT(8) - 1);
            // UInt_t cnt2 = (d[idx] >> 14) & (BIT(8) - 1);
            // UInt_t cnt1 = (d[idx] >> 7) & (BIT(8) - 1);
            // UInt_t cnt0 = d[idx] & (BIT(8) - 1);
            //             printf("type = %u  %06u  %06u  %06u  %06u  \n", type, cnt3, cnt2, cnt1, cnt0);
        } else if (type == 5) {
            UInt_t cnt = d[idx] & (BIT(28) - 1);
            if (iCnt >= MSC_N_COUNTERS)
                continue;
            cntrArrCur[iCnt++] = cnt;
            //            printf("\ttype = %u  arr[%2u] = %8u\n", type, iCnt - 1, cntrArrCur[iCnt - 1]);
        }
        type = (d[++idx] >> 28) & (BIT(5) - 1);
    }

    return kBMNSUCCESS;
};

BmnStatus BmnConverterThread::InitUTCShift()
{
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1972, 1, 1, 0, 0, 9), 10));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1972, 7, 1, 0, 0, 10), 11));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1973, 1, 1, 0, 0, 11), 12));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1974, 1, 1, 0, 0, 12), 13));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1975, 1, 1, 0, 0, 13), 14));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1976, 1, 1, 0, 0, 14), 15));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1977, 1, 1, 0, 0, 15), 16));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1978, 1, 1, 0, 0, 16), 17));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1979, 1, 1, 0, 0, 17), 18));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1980, 1, 1, 0, 0, 18), 19));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1981, 7, 1, 0, 0, 19), 20));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1982, 7, 1, 0, 0, 20), 21));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1983, 7, 1, 0, 0, 21), 22));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1985, 7, 1, 0, 0, 22), 23));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1988, 1, 1, 0, 0, 23), 24));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1990, 1, 1, 0, 0, 24), 25));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1991, 1, 1, 0, 0, 25), 26));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1992, 7, 1, 0, 0, 26), 27));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1993, 7, 1, 0, 0, 27), 28));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1994, 7, 1, 0, 0, 28), 29));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1996, 1, 1, 0, 0, 29), 30));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1997, 7, 1, 0, 0, 30), 31));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1999, 1, 1, 0, 0, 31), 32));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2006, 1, 1, 0, 0, 32), 33));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2009, 1, 1, 0, 0, 33), 34));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2012, 7, 1, 0, 0, 34), 35));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2015, 7, 1, 0, 0, 35), 36));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2017, 1, 1, 0, 0, 36), 37));
    utc_valid = TTimeStamp(2026, 12, 30, 0, 0, 1);
    return kBMNSUCCESS;
}

Int_t BmnConverterThread::GetUTCShift(TTimeStamp t)
{
    if (t < leaps.begin()->first) {
        LOGF(warning, "Wrong time! %s", t.AsString());
        return 0;
    }
    if (t > utc_valid)
        LOGF(warning, "Warning! Leap seconds table expired!");
    Int_t shift = 0;
    auto it = leaps.lower_bound(t);
    if ((it == leaps.end()))
        it--;
    else if (it->first > t)
        it--;
    shift = it->second;
    return shift;
}

void BmnConverterThread::ClearRawArrays()
{
    sync->Delete();
    tdc->Delete();
    tqdc_adc->Delete();
    tqdc_tdc->Delete();
    tdc_hgnd->Delete();
    hrb->Delete();
    adc32->Delete();
    adc64->Delete();
    adc128->Delete();
    adc->Delete();
    vsp->Delete();
}

void BmnConverterThread::ClearRawSpillArrays()
{
    msc->Delete();
    t0raw->Delete();
}
