/********************************************************************************
 *    BmnTrigInfoDst.h                                                      *
 *    Fill Trigger information from DIGI chain into DST chain                   *
 *    Sergei Merts                                                              *
 *    Created: May 26 2023                                                      *
 *******************************************************************************/
#ifndef BMNTRIGINFODST_H
#define BMNTRIGINFODST_H

#include "TClonesArray.h"
#include "TNamed.h"

class BmnTrigInfoDst : public TNamed
{
  public:
    /** Default constructor **/
    BmnTrigInfoDst();
    /** Constructor with flag whether experimental data are used **/
    BmnTrigInfoDst(Bool_t isExp);
    /** Destructor **/
    ~BmnTrigInfoDst();
    /** Copy constructor **/
    BmnTrigInfoDst(const BmnTrigInfoDst& other);
    /** Copy assignment operator **/
    BmnTrigInfoDst& operator=(const BmnTrigInfoDst& other);
    /** Move constructor **/
    BmnTrigInfoDst(BmnTrigInfoDst&& other);
    /** Move assignment operator **/
    BmnTrigInfoDst& operator=(BmnTrigInfoDst&& other);

    void CreateBranches(Bool_t isExp);
    void Reset();

    Float_t GetVCAmp() { return fVCAmp; }
    Float_t GetBC0Amp() { return fBC0Amp; }
    Float_t GetBC1Amp() { return fBC1Amp; }
    Float_t GetBC1Integral() { return fBC1Integral; }
    Float_t GetBC2Amp() { return fBC2Amp; }
    Float_t GetFDAmp() { return fFDAmp; }
    UInt_t GetBDMult() { return fBDMult; }
    UInt_t GetSiMDMult() { return fSiMDMult; }
    TClonesArray* GetBC0Digits() { return fBC0Digits; }
    TClonesArray* GetBC1Digits() { return fBC1Digits; }
    TClonesArray* GetBC2Digits() { return fBC2Digits; }
    TClonesArray* GetBDDigits() { return fBDDigits; }
    TClonesArray* GetFDDigits() { return fFDDigits; }

    void SetVCAmp(Float_t a) { fVCAmp = a; }
    void SetBC0Amp(Float_t a) { fBC0Amp = a; }
    void SetBC1Amp(Float_t a) { fBC1Amp = a; }
    void SetBC2Amp(Float_t a) { fBC2Amp = a; }
    void SetFDAmp(Float_t a) { fFDAmp = a; }
    void SetBC1Integral(Float_t a) { fBC1Integral = a; }
    void SetBDMult(UInt_t a) { fBDMult = a; }
    void SetSiMDMult(UInt_t a) { fSiMDMult = a; }

    /** Set the Input signal states, after reduction */
    void SetInputSignalsAR(UInt_t v) { fInputsAR = v; }
    /** Set the Input signal states, before reduction */
    void SetInputSignalsBR(UInt_t v) { fInputsBR = v; }

    Bool_t IsTriggerBitTrueAR(Short_t bit) { return Bool_t((fInputsAR >> bit) & 1); }
    Bool_t IsTriggerBitTrueBR(Short_t bit) { return Bool_t((fInputsBR >> bit) & 1); }

    /** Get Input signal states, after reduction */
    UInt_t GetInputSignalsAR() { return fInputsAR; }
    /** Get Input signal states, before reduction */
    UInt_t GetInputSignalsBR() { return fInputsBR; }

  private:
    Bool_t isExpData;   //!

    Float_t fVCAmp;
    Float_t fBC0Amp;
    Float_t fBC1Amp;
    Float_t fBC1Integral;
    Float_t fBC2Amp;
    UInt_t fBDMult;
    UInt_t fSiMDMult;
    Float_t fFDAmp;
    UInt_t fInputsAR;
    UInt_t fInputsBR;

    TClonesArray* fBC0Digits;
    TClonesArray* fBC1Digits;
    TClonesArray* fBC2Digits;
    TClonesArray* fBDDigits;
    TClonesArray* fFDDigits;

    void TCAFree();

    ClassDef(BmnTrigInfoDst, 3);
};

#endif
