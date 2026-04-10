/********************************************************************************
 *    BmnTrigInfoDst.cxx                                                        *
 *******************************************************************************/
#include "BmnTrigInfoDst.h"

#include "BmnFDPoint.h"
#include "BmnTrigDigit.h"
#include "BmnTrigWaveDigit.h"
#include "FairLogger.h"

void BmnTrigInfoDst::CreateBranches(Bool_t isExp)
{
    if (fBC1Digits || fBC2Digits || fFDDigits || fBDDigits) {
        LOG(error) << "BmnTrigInfoDst: branches have been already created. It is a programming error";
        return;
    }

    isExpData = isExp;
    if (isExpData) {
        fBC0Digits = new TClonesArray("BmnTrigWaveDigit");
        fBC1Digits = new TClonesArray("BmnTrigWaveDigit");
        fBC2Digits = new TClonesArray("BmnTrigWaveDigit");
        fFDDigits = new TClonesArray("BmnTrigWaveDigit");
    } else
        fFDDigits = new TClonesArray("BmnFDPoint");
    fBDDigits = new TClonesArray("BmnTrigDigit");
}

// Default constructor
BmnTrigInfoDst::BmnTrigInfoDst()
    : TNamed("BmnTrigInfoDst", "DST")
    , isExpData(true)
    , fVCAmp(0.)
    , fBC0Amp(0.)
    , fBC1Amp(0.)
    , fBC1Integral(0.)
    , fBC2Amp(0.)
    , fBDMult(0)
    , fSiMDMult(0)
    , fFDAmp(0.)
    , fInputsAR(0)
    , fInputsBR(0)
    , fBC0Digits(nullptr)
    , fBC1Digits(nullptr)
    , fBC2Digits(nullptr)
    , fBDDigits(nullptr)
    , fFDDigits(nullptr)
{}

// Constructor with flag whether experimental data are used
BmnTrigInfoDst::BmnTrigInfoDst(Bool_t isExp)
    : TNamed("BmnTrigInfoDst", "DST")
    , fVCAmp(0.)
    , fBC0Amp(0.)
    , fBC1Amp(0.)
    , fBC1Integral(0.)
    , fBC2Amp(0.)
    , fBDMult(0)
    , fSiMDMult(0)
    , fFDAmp(0.)
    , fInputsAR(0)
    , fInputsBR(0)
    , fBC0Digits(nullptr)
    , fBC1Digits(nullptr)
    , fBC2Digits(nullptr)
    , fBDDigits(nullptr)
    , fFDDigits(nullptr)
{
    isExpData = isExp;
    CreateBranches(isExpData);
}

// Copy constructor
BmnTrigInfoDst::BmnTrigInfoDst(const BmnTrigInfoDst& other)
    : TNamed("BmnTrigInfoDst", "DST")
    , isExpData(other.isExpData)
    , fVCAmp(other.fVCAmp)
    , fBC0Amp(other.fBC0Amp)
    , fBC1Amp(other.fBC1Amp)
    , fBC1Integral(other.fBC1Integral)
    , fBC2Amp(other.fBC2Amp)
    , fBDMult(other.fBDMult)
    , fSiMDMult(other.fSiMDMult)
    , fFDAmp(other.fFDAmp)
    , fInputsAR(other.fInputsAR)
    , fInputsBR(other.fInputsBR)
{
    if (other.fBC0Digits)
        fBC0Digits = new TClonesArray(*other.fBC0Digits);
    else
        fBC0Digits = nullptr;
    if (other.fBC1Digits)
        fBC1Digits = new TClonesArray(*other.fBC1Digits);
    else
        fBC1Digits = nullptr;
    if (other.fBC2Digits)
        fBC2Digits = new TClonesArray(*other.fBC2Digits);
    else
        fBC2Digits = nullptr;
    if (other.fBDDigits)
        fBDDigits = new TClonesArray(*other.fBDDigits);
    else
        fBDDigits = nullptr;
    if (other.fFDDigits)
        fFDDigits = new TClonesArray(*other.fFDDigits);
    else
        fFDDigits = nullptr;
}

// Copy assignment operator
BmnTrigInfoDst& BmnTrigInfoDst::operator=(const BmnTrigInfoDst& other)
{
    if (&other != this) {
        isExpData = other.isExpData;
        fBC0Amp = other.fBC0Amp;
        fVCAmp = other.fVCAmp;
        fBC1Amp = other.fBC1Amp;
        fBC1Integral = other.fBC1Integral;
        fBC2Amp = other.fBC2Amp;
        fBDMult = other.fBDMult;
        fSiMDMult = other.fSiMDMult;
        fFDAmp = other.fFDAmp;
        fInputsAR = other.fInputsAR;
        fInputsBR = other.fInputsBR;

        TCAFree();
        if (other.fBC0Digits)
            fBC0Digits = new TClonesArray(*other.fBC0Digits);
        else
            fBC0Digits = nullptr;
        if (other.fBC1Digits)
            fBC1Digits = new TClonesArray(*other.fBC1Digits);
        else
            fBC1Digits = nullptr;
        if (other.fBC2Digits)
            fBC2Digits = new TClonesArray(*other.fBC2Digits);
        else
            fBC2Digits = nullptr;
        if (other.fBDDigits)
            fBDDigits = new TClonesArray(*other.fBDDigits);
        else
            fBDDigits = nullptr;
        if (other.fFDDigits)
            fFDDigits = new TClonesArray(*other.fFDDigits);
        else
            fFDDigits = nullptr;
    }
    return *this;
}

// Move constructor
BmnTrigInfoDst::BmnTrigInfoDst(BmnTrigInfoDst&& other)
    : TNamed("BmnTrigInfoDst", "DST")
    , isExpData(other.isExpData)
    , fVCAmp(other.fVCAmp)
    , fBC0Amp(other.fBC0Amp)
    , fBC1Amp(other.fBC1Amp)
    , fBC1Integral(other.fBC1Integral)
    , fBC2Amp(other.fBC2Amp)
    , fBDMult(other.fBDMult)
    , fSiMDMult(other.fSiMDMult)
    , fFDAmp(other.fFDAmp)
    , fInputsAR(other.fInputsAR)
    , fInputsBR(other.fInputsBR)
{
    fBC0Digits = std::exchange(other.fBC0Digits, nullptr);
    fBC1Digits = std::exchange(other.fBC1Digits, nullptr);
    fBC2Digits = std::exchange(other.fBC2Digits, nullptr);
    fBDDigits = std::exchange(other.fBDDigits, nullptr);
    fFDDigits = std::exchange(other.fFDDigits, nullptr);
}

// Move assignment operator
BmnTrigInfoDst& BmnTrigInfoDst::operator=(BmnTrigInfoDst&& other)
{
    isExpData = other.isExpData;
    fBC0Amp = other.fBC0Amp;
    fVCAmp = other.fVCAmp;
    fBC1Amp = other.fBC1Amp;
    fBC1Integral = other.fBC1Integral;
    fBC2Amp = other.fBC2Amp;
    fBDMult = other.fBDMult;
    fSiMDMult = other.fSiMDMult;
    fFDAmp = other.fFDAmp;
    fInputsAR = other.fInputsAR;
    fInputsBR = other.fInputsBR;

    TCAFree();
    fBC0Digits = std::exchange(other.fBC0Digits, nullptr);
    fBC1Digits = std::exchange(other.fBC1Digits, nullptr);
    fBC2Digits = std::exchange(other.fBC2Digits, nullptr);
    fBDDigits = std::exchange(other.fBDDigits, nullptr);
    fFDDigits = std::exchange(other.fFDDigits, nullptr);

    return *this;
}

void BmnTrigInfoDst::Reset()
{
    fVCAmp = fBC1Amp = fBC1Integral = fBC2Amp = fFDAmp = 0.;
    fBDMult = fSiMDMult = 0;
    if (fBC0Digits)
        fBC0Digits->Delete();
    if (fBC1Digits)
        fBC1Digits->Delete();
    if (fBC2Digits)
        fBC2Digits->Delete();
    if (fFDDigits)
        fFDDigits->Delete();
    if (fBDDigits)
        fBDDigits->Delete();
}

void BmnTrigInfoDst::TCAFree()
{
    if (fBC0Digits) {
        fBC0Digits->Delete();
        delete fBC0Digits;
    }
    if (fBC1Digits) {
        fBC1Digits->Delete();
        delete fBC1Digits;
    }
    if (fBC2Digits) {
        fBC2Digits->Delete();
        delete fBC2Digits;
    }
    if (fFDDigits) {
        fFDDigits->Delete();
        delete fFDDigits;
    }
    if (fBDDigits) {
        fBDDigits->Delete();
        delete fBDDigits;
    }
}

BmnTrigInfoDst::~BmnTrigInfoDst()
{
    TCAFree();
}
