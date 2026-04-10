#include "BmnRawDataDecoder.h"

#include <iostream>
// ROOT
#include "TStopwatch.h"
// FairRoot
#include "FairLogger.h"
// BmnRoot
#include "BmnConverterTools.h"
#include "UniDetectorParameter.h"
#include "UniRawFile.h"
#include "UniRun.h"

// Double_t realtime = 0.0;
// Double_t converterTime = 0.0;

#include <arpa/inet.h> /* For ntohl for Big Endian LAND. */

// how protruded the noisy channel should be
double BmnRawDataDecoder::threshGem = 3.0;
double BmnRawDataDecoder::threshCsc = 3.0;
double BmnRawDataDecoder::threshSil = 3.0;

// if not enough statistic is in histograms
double BmnRawDataDecoder::correctionGem = 0;
double BmnRawDataDecoder::correctionCsc = 0;
double BmnRawDataDecoder::correctionSil = 0;

// window size
int BmnRawDataDecoder::samplesGem = 32;
int BmnRawDataDecoder::samplesCsc = 32;
int BmnRawDataDecoder::samplesSil = 32;

BmnRawDataDecoder::BmnRawDataDecoder(TString file, TString outfile, ULong_t nEvents, ULong_t period)
    : fRawSource(nullptr)
    , fDecoder(nullptr)
    , isTaskMode(false)
    , isForwardMode(false)
    , isRawRootInputFile(false)
    , fApplyThreshold(kTRUE)
    , fPeriodId(period)
    //    , fEventId(0)
    , fNTotalEvents(0)
    , fNSignalEvents(0)
    , fStartEventId(0)
    , fRawFileName(file)
    , fMaxEvent(nEvents)
    , fBmnSetup(kBMNSETUP)
    , fUseCalibFile(false)
    , fT0Serial(0)
    , fVerbose(0)
{
    isRawRootInputFile = IsRootFile(fRawFileName);
    fRunId = GetRunIdFromFile(fRawFileName);
    fSubName = BmnConverterTools::GetSubNameAfterRunId(fRawFileName);
    LOG(debug) << "subname " << fSubName;
    LOG(info) << "RunId " << fRunId << " got from " << (isRawRootInputFile ? "RawRoot" : "RawData") << " file";
    fDigiFileName = (outfile == "") ? Form("bmn_run%d%s_digi.root", fRunId, fSubName.Data()) : outfile;
    if (isRawRootInputFile)
        fRootFileName = fRawFileName;
}

BmnRawDataDecoder::~BmnRawDataDecoder()
{
    LOG(debug) << "~BmnRawDataDecoder()";
    if (fRawSource)
        delete fRawSource;
    if (fDecoder)
        delete fDecoder;
}

void BmnRawDataDecoder::CreateConverter()
{
    if (fRootFileName.Length() == 0) {
        Int_t lastSlash = fDigiFileName.Last('/');
        fDigiDirPath = (lastSlash == TString::kNPOS) ? "" : TString(fDigiFileName(0, lastSlash + 1));
        if (fRootDirPath.Length() == 0)
            fRootDirPath = fDigiDirPath;
        fRootFileName = Form("%sbmn_run%d%s_raw.root", fRootDirPath.Data(), fRunId, fSubName.Data());
    }
    if (fRootDirPath.Length()) {
        bool dc = std::filesystem::create_directories(fRootDirPath.Data());
        LOGF(info, "Raw directory: %s was %s", fRootDirPath.Data(), dc ? "created" : "already present");
    }
    fRawSource = new BmnRawSource(fRawFileName, fRootFileName, fMaxEvent, fPeriodId);
    fRawSource->SetDecoderConfigFileName(fDecoderConfigFileName);
    fRawSource->SetRunId(fRunId);
    fRawSource->SetVerbose(fVerbose);
}

void BmnRawDataDecoder::CreateDecoder()
{
    if (fRootFileName.Length() == 0) {
        Int_t lastSlash = fDigiFileName.Last('/');
        fDigiDirPath = (lastSlash == TString::kNPOS) ? "" : TString(fDigiFileName(0, lastSlash + 1));
        if (fRootDirPath.Length() == 0)
            fRootDirPath = fDigiDirPath;
        fRootFileName = Form("%sbmn_run%d%s_raw.root", fRootDirPath.Data(), fRunId, fSubName.Data());
    }
    fDecoder = new BmnDecoder(fRootFileName, fDigiFileName, fMaxEvent, fPeriodId);
    fDecoder->SetForwardMode(isForwardMode);
    fDecoder->SetDecoderConfigFileName(fDecoderConfigFileName);
    fDecoder->SetDetectorSetup(fDetectorSetup);
    fDecoder->SetBmnSetup(fBmnSetup);
    fDecoder->SetRunId(fRunId);
    fDecoder->SetVerbose(fVerbose);
}

BmnStatus BmnRawDataDecoder::InitConverter(TString FileName)
{
    return fRawSource->InitConverter(FileName);
}

BmnStatus BmnRawDataDecoder::InitConverter()
{
    return fRawSource->InitConverter();
}

BmnStatus BmnRawDataDecoder::InitDecoder()
{
    return fDecoder->InitDecoder();
}

void BmnRawDataDecoder::ConnectForward()
{
    TTree* raw_tree = fRawSource->GetRawTree();
    fDecoder->SetRawTree(raw_tree);
    fDecoder->AssignInputs();
}

BmnStatus BmnRawDataDecoder::ConvertRawToRootIterate(UInt_t* buf, UInt_t len)
{
    return fRawSource->ConvertRawToRootIterate(buf, len);
}

BmnStatus BmnRawDataDecoder::DecodeDataToDigiIterate()
{
    return fDecoder->DecodeDataToDigiIterate();
}

BmnStatus BmnRawDataDecoder::ConvertRawToRoot()
{
    if (isRawRootInputFile) {
        LOGF(info, "Input is raw root file. Nothing to do!");
        return kBMNSUCCESS;
    } else {
        if (!fRawSource)
            CreateConverter();
        return fRawSource->ConvertRawToRoot();
    }
}

BmnStatus BmnRawDataDecoder::DecodeDataToDigi()
{
    return fDecoder->DecodeDataToDigi();
}

Int_t BmnRawDataDecoder::GetRunIdFromFile(TString name)
{
    Int_t lastSlash = name.Last('/');
    TString str(name(lastSlash + 1, name.Length() - lastSlash));
    TPRegexp re_universal("[^0123456789]*(\\d+)(.*)\\.(\\w+)");
    re_universal.Substitute(str, "$1");
    return str.Atoi();
}

Bool_t BmnRawDataDecoder::IsRootFile(TString name)
{
    TPRegexp re(".+\\.root");
    return re.MatchB(name);
}

Bool_t BmnRawDataDecoder::IsRawFile(TString name)
{
    TPRegexp re(".+\\.data");
    return re.MatchB(name);
}
