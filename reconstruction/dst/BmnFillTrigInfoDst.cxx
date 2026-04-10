/********************************************************************************
 *    BmnFillTrigInfoDst.cxx                                                    *
 *******************************************************************************/

#include "BmnFillTrigInfoDst.h"

#include "BmnFDPoint.h"
#include "BmnTrigDigit.h"
#include "BmnTrigInfoDst.h"
#include "BmnTrigWaveDigit.h"
#include "FairRootManager.h"

#include <TStopwatch.h>
#include <iostream>
using namespace std;

static Double_t workTime = 0.0;

// ---- Default constructor -------------------------------------------
BmnFillTrigInfoDst::BmnFillTrigInfoDst()
    : BmnFillTrigInfoDst(false)
{
    LOG(debug) << "Defaul Constructor of BmnFillTrigInfoDst";
}

// ---- Constructor with flag whether experimental data are used --------
BmnFillTrigInfoDst::BmnFillTrigInfoDst(Bool_t isExp)
    : BmnTask("BmnFillTrigInfoDst")
    , isExpData(isExp)
    , fDigiEventHeader(nullptr)
    , fVCDigits(nullptr)
    , fBC0Digits(nullptr)
    , fBC1Digits(nullptr)
    , fBC2Digits(nullptr)
    , fBDDigits(nullptr)
    , fSiMDDigits(nullptr)
    , fFDDigits(nullptr)
    , fTrigInfo(nullptr)
{
    LOG(debug) << "Constructor with experimental data flag of BmnFillTrigInfoDst";
}

// ---- Destructor ----------------------------------------------------
BmnFillTrigInfoDst::~BmnFillTrigInfoDst()
{
    LOG(debug) << "Destructor of BmnFillTrigInfoDst";
    if (fTrigInfo)
        delete fTrigInfo;
}

// ---- Init ----------------------------------------------------------
InitStatus BmnFillTrigInfoDst::Init()
{
    LOG(debug) << "Initialization of BmnFillTrigInfoDst";

    // Get a handle from the IO manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        LOG(error) << "Init: FairRootManager is not instantiated! BmnFillTrigInfoDst task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    // Get input branches
    if (isExpData) {
        fDigiEventHeader = (BmnEventHeader*)ioman->GetObject("BmnEventHeader.");
        fVCDigits = (TClonesArray*)ioman->GetObject("TQDC_VCS");
        fBC0Digits = (TClonesArray*)ioman->GetObject("TQDC_BC0");
        fBC1Digits = (TClonesArray*)ioman->GetObject("TQDC_BC1S");
        if (!fBC1Digits)
            fBC1Digits = (TClonesArray*)ioman->GetObject("TQDC_BC1");
        fBC2Digits = (TClonesArray*)ioman->GetObject("TQDC_BC2AS");
        if (!fBC2Digits)
            fBC2Digits = (TClonesArray*)ioman->GetObject("TQDC_BC2");
        fBDDigits = (TClonesArray*)ioman->GetObject("BD");
        fSiMDDigits = (TClonesArray*)ioman->GetObject("SiMD");
        fFDDigits = (TClonesArray*)ioman->GetObject("TQDC_FD");
    } else {
        fBDDigits = (TClonesArray*)ioman->GetObject("BdDigit");
        fFDDigits = (TClonesArray*)ioman->GetObject("FDPoint");
    }

    if (isExpData && !fVCDigits && !fBC0Digits && !fBC1Digits && !fBC2Digits && !fBDDigits && !fSiMDDigits
        && !fFDDigits)
    {
        LOG(error) << "Init: all trigger branches not found! BmnFillTrigInfoDst task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    // Create and register TrigInfo object for trigger output data
    fTrigInfo = new BmnTrigInfoDst(isExpData);
    ioman->Register("BmnTrigInfo.", "TRIGGER", fTrigInfo, kTRUE);
    if (!fTrigInfo) {
        LOG(error) << "Init: BmnTrigInfo not created! BmnFillTrigInfoDst task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------
void BmnFillTrigInfoDst::Exec(Option_t* /*option*/)
{
    if (!IsActive())
        return;

    TStopwatch sw;
    sw.Start();

    if (fVerbose > 1)
        cout << "\n======================== BmnFillTrigInfoDst exec started =====================\n" << endl;

    fTrigInfo->Reset();

    if (fDigiEventHeader) {
        fTrigInfo->SetInputSignalsAR(fDigiEventHeader->GetInputSignalsAR());
        fTrigInfo->SetInputSignalsBR(fDigiEventHeader->GetInputSignalsBR());
    }

    if (fVCDigits && fVCDigits->GetEntriesFast() > 0) {
        BmnTrigWaveDigit* digiVC = (BmnTrigWaveDigit*)fVCDigits->At(0);
        fTrigInfo->SetVCAmp(digiVC->GetPeak());
    }
    if (fBC0Digits && fBC0Digits->GetEntriesFast() > 0) {
        BmnTrigWaveDigit* digiBC0 = (BmnTrigWaveDigit*)fBC0Digits->At(0);
        fTrigInfo->SetBC0Amp(digiBC0->GetPeak());
        fTrigInfo->GetBC0Digits()->AbsorbObjects(fBC0Digits);
    }
    if (fBC1Digits && fBC1Digits->GetEntriesFast() > 0) {
        BmnTrigWaveDigit* digiBC1 = (BmnTrigWaveDigit*)fBC1Digits->At(0);
        fTrigInfo->SetBC1Integral(digiBC1->GetIntegral());
        fTrigInfo->SetBC1Amp(digiBC1->GetPeak());
        fTrigInfo->GetBC1Digits()->AbsorbObjects(fBC1Digits);
    }
    if (fBC2Digits && fBC2Digits->GetEntriesFast() > 0) {
        BmnTrigWaveDigit* digiBC2 = (BmnTrigWaveDigit*)fBC2Digits->At(0);
        fTrigInfo->SetBC2Amp(digiBC2->GetPeak());
        fTrigInfo->GetBC2Digits()->AbsorbObjects(fBC2Digits);
    }
    if (fBDDigits) {
        fTrigInfo->SetBDMult(fBDDigits->GetEntriesFast());
        fTrigInfo->GetBDDigits()->AbsorbObjects(fBDDigits);
    }
    if (fSiMDDigits)
        fTrigInfo->SetSiMDMult(fSiMDDigits->GetEntriesFast());
    if (fFDDigits && fFDDigits->GetEntriesFast() > 0) {
        if (isExpData) {
            BmnTrigWaveDigit* digiFD = (BmnTrigWaveDigit*)fFDDigits->At(0);
            fTrigInfo->SetFDAmp(digiFD->GetPeak());
        }
        fTrigInfo->GetFDDigits()->AbsorbObjects(fFDDigits);
    }

    sw.Stop();
    workTime += sw.RealTime();
}

// ---- Finish --------------------------------------------------------
void BmnFillTrigInfoDst::Finish()
{
    LOG(debug) << "Finish of BmnFillTrigInfoDst";
    printf("Work time of BmnFillTrigInfoDst: %4.2f sec.\n", workTime);
}
