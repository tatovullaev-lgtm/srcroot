#include "BmnProfAsic2Raw.h"

#include "FairRootManager.h"
#include "TSystem.h"

#include <TChain.h>
#include <TRandom.h>
#include <TStopwatch.h>
#include <zlib.h>

static Double_t workTime = 0.0;

BmnProfAsic2Raw::BmnProfAsic2Raw(Int_t run_period, Int_t run_number, Int_t verbosity)
    : FairTask("BmnProfAsic2Raw", verbosity)
    , fPeriodId(run_period)
    , fRunId(run_number)
    , fPedEvCntr(0)
    , fNoiseEvCntr(0)
    , fEvForPedestals(100)
    , fEv(0)
    , fPedEnough(kFALSE)
    , fInputEventHeaderName("BmnEventHeader.")
    , fInputAdcBranchName("ADC192ASIC")
    , fWorkAdcBranchName("ADC32PROF")
    , fOutputProfBranchName("BmnProfDigit")
{}

BmnProfAsic2Raw::~BmnProfAsic2Raw() {}

InitStatus BmnProfAsic2Raw::Init()
{

    if (fVerbose > 1)
        cout << "=================== BmnProfAsic2Raw::Init() started ====================" << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fEventHead = static_cast<BmnEventHeader*>(ioman->GetObject(fInputEventHeaderName));
    fInAsicArray = (TClonesArray*)ioman->GetObject(fInputAdcBranchName);
    if (!fInAsicArray) {
        cout << "BmnProfAsic2Raw::Init(): branch " << fInputAdcBranchName << " not found! Task will be deactivated"
             << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fWorkAdcArray = new TClonesArray(BmnADCDigit::Class());
    ioman->Register(fWorkAdcBranchName, "ADC32Prof", fWorkAdcArray, kTRUE);

    fBmnProfDigitArray = new TClonesArray(BmnSiProfDigit::Class());
    ioman->Register(fOutputProfBranchName, "Prof", fBmnProfDigitArray, kTRUE);

    TString gPathCSCConfig = gSystem->Getenv("VMCWORKDIR");
    gPathCSCConfig += "/parameters/csc/XMLConfigs/";

    //--------------------------------------------------------------------------
    fMapper = new BmnProfRaw2Digit(8, 0);
    fMapper->SetVerbose(1);
    if (fVerbose > 1)
        cout << "=================== BmnProfAsic2Raw::Init() finished ===================" << endl;

    return kSUCCESS;
}

void BmnProfAsic2Raw::Exec(Option_t* opt)
{
    fEv++;
    TStopwatch sw;
    sw.Start();
    if (!IsActive())
        return;
    fBmnProfDigitArray->Delete();
    fWorkAdcArray->Delete();

    if (fVerbose > 1)
        cout << "=================== BmnProfAsic2Raw::Exec() started ====================" << endl;
    if (fVerbose > 1)
        cout << " BmnProfAsic2Raw::Exec(), Number of BmnAdcDigits = " << fInAsicArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1)
        cout << "=================== BmnProfAsic2Raw::Exec() finished ===================" << endl;

    sw.Stop();
    workTime += sw.RealTime();
    return;
}

void BmnProfAsic2Raw::ProcessDigits()
{

    fMapper->FillExtractedBits(fInAsicArray, fWorkAdcArray);
    fPedEnough = kTRUE;
    if (!fPedEnough) {
        if ((Int_t)fPedEvCntr != fEvForPedestals) {
            fMapper->CopyData2PedMap(fWorkAdcArray, fPedEvCntr);
            fPedEvCntr++;
            if (fVerbose == 1) {
                if (fPedEvCntr % 100 == 0 && fPedEvCntr > 0)
                    cout << "Pedestal event #" << fPedEvCntr << "/" << fEvForPedestals << ";" << endl;
            } else if (fVerbose == 0)
                DrawBar(fPedEvCntr, fEvForPedestals);
        } else {
            fPedEnough = kTRUE;
            fMapper->RecalculatePedestalsAugmented();
        }
    } else
        fMapper->FillEvent(fWorkAdcArray, fBmnProfDigitArray);
    return;
}

void BmnProfAsic2Raw::Finish()
{
    delete fMapper;
    if (fVerbose > 0) {
        printf("Work time of BmnProfAsic2Raw: %4.2f sec.\n", workTime);
        printf("Processed events: %d\n", fEv);
    }
    return;
}
