#include "BmnSlewingTOF700.h"

#include <BmnMath.h>
#include <arpa/inet.h> /* For ntohl for Big Endian LAND. */
#include <sys/stat.h>

using namespace std;

BmnSlewingTOF700::BmnSlewingTOF700()
{
    fRunId = 0;
    fPeriodId = 0;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = 0;
    fLengthRawFile = 0;
    fCurentPositionRawFile = 0;
    eventHeaderDAQ = NULL;
    eventHeader = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fT0Time = 0.0;
    fRawTree = NULL;
    fRootFileIn = NULL;
    fRootFileOut = NULL;
    fRawFileIn = NULL;
    sync = NULL;
    tdc = NULL;
    tqdc_tdc = NULL;
    tqdc_adc = NULL;
    hrb = NULL;
    adc32 = NULL;
    adc128 = NULL;
    adc = NULL;
    msc = NULL;
    fRootFileName = "";
    fRawFileName = "";
    fTrigPlaceMapFileName = "";
    fTrigChannelMapFileName = "";
    fTof700MapFileName = "";
    fTof700GeomFileName = "";
    fDat = 0;
    fTrigMapper = NULL;
    fTof700Mapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fBmnSetup = kBMNSETUP;
    fT0Serial = 0;
}

BmnSlewingTOF700::BmnSlewingTOF700(TString file, ULong_t nEvents, ULong_t period)
{

    isRawRootInputFile = kFALSE;

    InitUTCShift();

    eventHeaderDAQ = NULL;
    eventHeader = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fT0Time = 0.0;
    fRawTree = NULL;
    fRootFileIn = NULL;
    fRootFileOut = NULL;
    fRawFileIn = NULL;
    sync = NULL;
    hrb = NULL;
    tdc = NULL;
    tqdc_tdc = NULL;
    tqdc_adc = NULL;
    adc32 = NULL;
    adc128 = NULL;
    adc = NULL;
    msc = NULL;
    fRawFileName = file;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = nEvents;
    fPeriodId = period;

    if (fRawFileName != "") {
        fRunId = GetRunIdFromFile(fRawFileName);
        TString subName = GetSubNameAfterRunId(file, fRunId);
        printf("RunId %d got from the %s file\n", fRunId, isRawRootInputFile ? "RawRoot" : "RawData");
        TString DigiFileName = Form("bmn_run%d_digi%s.root", fRunId, subName.Data());
        if (fRootFileName.Length()) {

        } else {
            Int_t lastSlash = DigiFileName.Last('/');
            TString digiPath = (lastSlash == TString::kNPOS) ? "" : TString(DigiFileName(0, lastSlash + 1));
            fRootFileName = Form("%sbmn_run%d_raw%s.root", digiPath.Data(), fRunId, subName.Data());
        }
    }

    //    fRunId = GetRunIdFromFile(fRawFileName);
    //    fRootFileName = Form("bmn_run%04d_raw.root", fRunId);
    //    fRootFileName = Form("bmn_run%04d_raw.root", (fRunId=6847));
    fTrigPlaceMapFileName = "";
    fTrigChannelMapFileName = "";
    fTof700MapFileName = file;
    fTof700GeomFileName = "";
    fDat = 0;
    fTrigMapper = NULL;
    fTof700Mapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fBmnSetup = kBMNSETUP;
    fT0Serial = 0;
    // InitMaps();
}

BmnSlewingTOF700::~BmnSlewingTOF700() {}

/*
BmnStatus BmnSlewingTOF700::FillTimeShiftsMap() {
    if (fT0Serial == 0) return kBMNERROR;
    Long64_t t0time = 0;
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        if (syncDig->GetSerial() == fT0Serial) {
            t0time = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
            break;
        }
    }
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        Long64_t syncTime = (t0time == 0.0) ? 0 : syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
        fTimeShifts.insert(pair<UInt_t, Long64_t>(syncDig->GetSerial(), syncTime - t0time));
    }

    return kBMNSUCCESS;
}
*/

BmnStatus BmnSlewingTOF700::FillTimeShiftsMap()
{
    if (fT0Serial == 0)
        return kBMNERROR;
    Long64_t t0time = 0;
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*)sync->At(i);
        if (syncDig->GetSerial() == fT0Serial) {
            t0time = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
            fTime_s = syncDig->GetTime_sec();
            fTime_ns = syncDig->GetTime_ns();
            // printf("serial %08X sync: %s\n", syncDig->GetSerial(), TTimeStamp(time_t(fTime_s), fTime_ns).AsString());
            if (fRunStartTime == TimeZero) {
                fRunStartTime = TTimeStamp(time_t(fTime_s), fTime_ns);
            }
            //            printf("T0 sync: %s \n", TTimeStamp(time_t(fTime_s), fTime_ns).AsString());
            break;
        }
    }
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*)sync->At(i);
        Long64_t syncTime = (t0time == 0.0) ? 0 : syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
        fTimeShifts.insert(pair<UInt_t, Long64_t>(syncDig->GetSerial(), syncTime - t0time));
    }
    return kBMNSUCCESS;
}

BmnStatus BmnSlewingTOF700::FillTimeShiftsMapNoDB(UInt_t t0serial)
{

    Long64_t t0time = -1;
    //    printf(" sync size %d, t0serial 0x%0x\n", sync->GetEntriesFast(), t0serial);
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*)sync->At(i);
        //	printf(" have 0x%0x requred 0x%0x\n", syncDig->GetSerial(), t0serial);
        if (syncDig->GetSerial() == t0serial) {
            t0time = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
            break;
        }
    }

    if (t0time == -1) {
        //        cerr << "No T0 digit found in tree" << endl;
        return kBMNERROR;
    }

    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*)sync->At(i);
        Long64_t syncTime = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
        fTimeShifts.insert(pair<UInt_t, Long64_t>(syncDig->GetSerial(), syncTime - t0time));
    }

    return kBMNSUCCESS;
}

BmnStatus BmnSlewingTOF700::SlewingTOF700Init()
{

    fDigiTree = new TTree("bmndata", "bmn_digit");
    eventHeader = new TClonesArray("BmnEventHeader");
    fDigiTree->Branch("BmnEventHeader", &eventHeader);

    fRootFileIn = new TFile(fRootFileName, "READ");
    if (fRootFileIn->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s \nSlewingTOF700 are stopped\n!!!!\n", fRootFileName.Data());
        return kBMNERROR;
    } else {
        printf("\nInput root file: %s;\n", fRootFileName.Data());
    }
    fRawTree = (TTree*)fRootFileIn->Get("BMN_RAW");
    tdc = new TClonesArray("BmnTDCDigit");
    tqdc_adc = new TClonesArray("BmnTQDCADCDigit");
    tqdc_tdc = new TClonesArray("BmnTDCDigit");
    sync = new TClonesArray("BmnSyncDigit");
    eventHeaderDAQ = new TClonesArray("BmnEventHeader");
    hrb = new TClonesArray("BmnHRBDigit");
    adc32 = new TClonesArray("BmnADCDigit");
    adc128 = new TClonesArray("BmnADCDigit");
    adc = new TClonesArray("BmnADCDigit");
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("TQDC_ADC", &tqdc_adc);
    fRawTree->SetBranchAddress("TQDC_TDC", &tqdc_tdc);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("EventHeader", &eventHeaderDAQ);
    fRawTree->SetBranchAddress("HRB", &hrb);
    fRawTree->SetBranchAddress("ADC32", &adc32);
    fRawTree->SetBranchAddress("ADC128", &adc128);
    fRawTree->SetBranchAddress("ADC", &adc);

    fNevents = (fMaxEvent > (ULong_t)fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

    fTrigMapper = new BmnTrigRaw2Digit(fTrigPlaceMapFileName, fTrigChannelMapFileName, fDigiTree);
    if (fT0Serial == 0) {
        fT0Serial = fTrigMapper->GetT0Serial();
        printf("T0 serial 0x%X got from trig mapping\n", fT0Serial);
    }

    fTrigMapper->SetSetup(fBmnSetup);

    TString PeriodSetupExt = Form("%d%s.txt", fPeriodId, ((fBmnSetup == kBMNSETUP) ? "" : "_SRC"));

    fTof700Mapper = new BmnTof2Raw2DigitNew(
        fTof700MapFileName, fRootFileName, 0, 0, TString("TOF700_geometry_run") + PeriodSetupExt);
    // fTof700Mapper->print();

    return kBMNSUCCESS;
}

BmnStatus BmnSlewingTOF700::SlewingTOF700()
{

    fTof700Mapper->readSlewingLimits();

    fTof700Mapper->BookSlewing();

    for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 5000 == 0)
            cout << "Slewing T0 event #" << iEv << endl;

        fTrigMapper->ClearArrays();

        fTimeShifts.clear();

        fRawTree->GetEntry(iEv);

        FillTimeShiftsMap();

        // if (FillTimeShiftsMapNoDB(0x6EA9711) == kBMNERROR) {
        //                 cout << "No TimeShiftMap created" << endl;
        // continue;

        //}

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tdc, fTimeShifts);
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc, fTimeShifts);
        }
        fT0Time = 0.;
        GetT0Info(fT0Time, fT0Width);
        if (fT0Time == 0.)
            continue;

        fTof700Mapper->fillSlewingT0(tdc, &fTimeShifts, fT0Time, fT0Width);
    }
    cout << "Slewing T0 event #" << fNevents << endl;

    for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 5000 == 0)
            cout << "Slewing RPC event #" << iEv << endl;

        fTrigMapper->ClearArrays();

        fTimeShifts.clear();

        fRawTree->GetEntry(iEv);

        FillTimeShiftsMap();

        // if (FillTimeShiftsMapNoDB(0x6EA9711) == kBMNERROR) {
        //                 cout << "No TimeShiftMap created" << endl;
        // continue;
        // }

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tdc, fTimeShifts);
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc, fTimeShifts);
        }
        fT0Time = 0.;
        GetT0Info(fT0Time, fT0Width);
        if (fT0Time == 0.)
            continue;

        fTof700Mapper->fillSlewing(tdc, &fTimeShifts, fT0Time, fT0Width);
    }
    cout << "Slewing RPC event #" << fNevents << endl;

    fTof700Mapper->FitSlewing();

    for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 5000 == 0)
            cout << "Equalization RPC strips event #" << iEv << endl;

        fTrigMapper->ClearArrays();

        fTimeShifts.clear();

        fRawTree->GetEntry(iEv);

        FillTimeShiftsMap();

        // if (FillTimeShiftsMapNoDB(0x6EA9711) == kBMNERROR) {
        //                 cout << "No TimeShiftMap created" << endl;
        // continue;
        // }

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tdc, fTimeShifts);
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc, fTimeShifts);
        }
        fT0Time = 0.;
        GetT0Info(fT0Time, fT0Width);
        if (fT0Time == 0.)
            continue;

        fTof700Mapper->fillEqualization(tdc, &fTimeShifts, fT0Time, fT0Width);
    }
    cout << "Equalization RPC strips event #" << fNevents << endl;

    fTof700Mapper->Equalization();

    //    fRootFileIn->Close();

    //    delete trigMapper;
    //    delete tof700Mapper;

    return kBMNSUCCESS;
}

BmnStatus BmnSlewingTOF700::PreparationTOF700()
{

    for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 5000 == 0)
            cout << "Preparation stage event #" << iEv << endl;

        fTrigMapper->ClearArrays();

        fTimeShifts.clear();

        fRawTree->GetEntry(iEv);

        FillTimeShiftsMap();

        // if (FillTimeShiftsMapNoDB(0x6EA9711) == kBMNERROR) {
        //                 cout << "No TimeShiftMap created" << endl;
        // continue;
        // }

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tdc, fTimeShifts);
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc, fTimeShifts);
        }
        fT0Time = 0.;
        GetT0Info(fT0Time, fT0Width);
        //        printf(" T0 = %f TW = %f\n", fT0Time, fT0Width);
        if (fT0Time == 0.)
            continue;
        // printf(" T0 = %f TW = %f\n", fT0Time, fT0Width);

        fTof700Mapper->fillPreparation(tdc, &fTimeShifts, fT0Time, fT0Width);
    }

    fTof700Mapper->Equalization0();
    fTof700Mapper->writeSlewingLimits();

    //    fRootFileIn->Close();

    //    delete trigMapper;
    //    delete tof700Mapper;

    return kBMNSUCCESS;
}

/*
Int_t BmnSlewingTOF700::GetRunIdFromFile(TString name) {
    Int_t runId = -1;
    FILE * file = fopen(name.Data(), "rb");
    if (file == NULL) {
        printf("File %s is not open!!!\n", name.Data());
        return -1;
    }
    UInt_t word;
    while (fread(&word, kWORDSIZE1, 1, file)) {
        if (word == kRUNNUMBERSYNC1) {
            fread(&word, kWORDSIZE1, 1, file); //skip word
            fread(&runId, kWORDSIZE1, 1, file);
            return runId;
        }
    }
    fclose(file);
    if (runId <= 0) {
        Int_t run = 0;
        //sscanf(&(((char *)name.Data())[strlen(name.Data())-9]), "%d", &run);
        run = ((TString) name(name.Length() - 9, name.Length() - 5)).Atoi();
        return run;
    } else return runId;
}

BmnStatus BmnSlewingTOF700::GetT0Info(Double_t& t0time, Double_t &t0width) {
    vector<TClonesArray*>* trigArr = fTrigMapper->GetTrigArrays();
    BmnTrigDigit* dig = 0;
    for (auto ar : *trigArr) {
        if (fPeriodId > 6) {
            if (strcmp(ar->GetName(), "BC2")) continue;
        } else {
            if (strcmp(ar->GetName(), "T0")) continue;
        }
        for (int i = 0; i < ar->GetEntriesFast(); i++) {
            dig = (BmnTrigDigit*) ar->At(i);
            if (fPeriodId > 6) {
                if (dig->GetMod() == 0) {
                    t0time = dig->GetTime();
                    t0width = dig->GetAmp();
                    //		printf(" t0 %f t0w %f n %d\n", t0time, t0width, ar->GetEntriesFast());
                    return kBMNSUCCESS;
                }
            } else {
                t0time = dig->GetTime();
                t0width = dig->GetAmp();
                return kBMNSUCCESS;
            }
        }
    }
    return kBMNERROR;
}
*/

Int_t BmnSlewingTOF700::GetRunIdFromFile(TString name)
{
    Int_t runId = -1;
    TPRegexp re(".+\\.root");
    if (re.MatchB(name)) {
        printf("Name is %s\n", name.Data());
        fRootFileIn = new TFile(name, "READ");
        if (fRootFileIn->IsOpen()) {
            BmnEventHeader* tempHdr = nullptr;
            TTree* RawTree = (TTree*)fRootFileIn->Get("BMN_RAW");
            if (RawTree) {
                RawTree->SetBranchAddress("BmnEventHeader.", &tempHdr);
                if (RawTree->GetEntriesFast()) {
                    RawTree->GetEntry(0);
                    runId = tempHdr->GetRunId();
                    fRootFileName = name;
                }
                delete RawTree;
            }
            fRootFileIn->Close();
        }
        delete fRootFileIn;
    }
    if (runId > 0) {
        isRawRootInputFile = true;
        return runId;
    }

    FILE* file = fopen(name.Data(), "rb");
    if (file == nullptr) {
        printf("File %s is not open!!!\n", name.Data());
        return -1;
    }
    UInt_t word;
    Int_t cntr = 0;
    while (fread(&word, kWORDSIZE, 1, file)) {
        // printf("%X\n", word);
        if (cntr == 20) {
            printf("After checking %d words in input file run number not found!\n", cntr);
            printf("Let's get run number from file name\n");
            break;
        }
        if (word == RECORD_RUN_NUMBER) {
            if (fread(&word, kWORDSIZE, 1, file) != 1)
                break;   // skip word
            if (fread(&runId, kWORDSIZE, 1, file) != 1)
                break;
            return runId;
        }
        cntr++;
    }
    fclose(file);
    if (runId <= 0) {
        // Int_t run = 0;
        // sscanf(&(((char *)name.Data())[strlen(name.Data())-9]), "%d", &run);
        TPRegexp(".*\\w+_\\w+_\\w+_(\\d+)[^/]*\\.data").Substitute(name, "$1");

        // cout << name << endl;

        return name.Atoi();

    } else
        return runId;
}
BmnStatus BmnSlewingTOF700::GetT0Info(Double_t& t0time, Double_t& t0width)
{
    vector<TClonesArray*>* trigArr = fTrigMapper->GetTrigArrays();
    BmnTrigDigit* dig = 0;
    for (auto ar : *trigArr) {
        if ((fPeriodId == 8) && (fBmnSetup == kSRCSETUP)) {   // temporary crutches
            if ((fRunId >= 986) && (fRunId <= 1086)) {
                if ((strcmp(ar->GetName(), "BC2_1_A")))
                    continue;
            } else {
                if ((strcmp(ar->GetName(), "T0_1_A")))
                    continue;
            }
        } else {
            if (fPeriodId > 6) {
                if (fPeriodId == 8) {
                    if (strcmp(ar->GetName(), "BC2AT"))
                        continue;
                } else {
                    if (strcmp(ar->GetName(), "BC2"))
                        continue;
                }
            } else {
                if (strcmp(ar->GetName(), "T0"))
                    continue;
            }
        }
        for (int i = 0; i < ar->GetEntriesFast(); i++) {
            dig = (BmnTrigDigit*)ar->At(i);
            if (dig->GetMod() == 0) {
                t0time = dig->GetTime();
                t0width = dig->GetAmp();
                //		printf(" t0 %f t0w %f n %d\n", t0time, t0width, ar->GetEntriesFast());
                return kBMNSUCCESS;
            }
        }
    }
    return kBMNERROR;
}

BmnStatus BmnSlewingTOF700::InitUTCShift()
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
    utc_valid = TTimeStamp(2023, 6, 1, 0, 0, 1);
    return kBMNSUCCESS;
}

Int_t BmnSlewingTOF700::GetUTCShift(TTimeStamp t)
{
    if (t < leaps.begin()->first) {
        //        if (fVerbose == 0) // verbosity as defined in BmnDataToRoot
        printf("Wrong time!\n");
        return 0;
    }
    if (t > utc_valid)
        printf(ANSI_COLOR_RED "Warning! Leap seconds table expired!\n" ANSI_COLOR_RESET);
    Int_t shift = 0;
    auto it = leaps.lower_bound(t);
    if ((it == leaps.end()))
        it--;
    else if (it->first > t)
        it--;
    shift = it->second;
    return shift;
}

TString BmnSlewingTOF700::GetSubNameAfterRunId(TString name, Int_t RunID)
{
    TString ID = TString::Itoa(RunID, 10);
    TRegexp reg1 = ID;
    TRegexp reg2 = (".data");
    Int_t index1 = name.Index(reg1) + ID.Length();
    Int_t index2 = name.Index(reg2);
    TSubString str = name(index1, index2 - index1);

    return str;
}
