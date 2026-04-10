/********************************************************************************
 *    BmnFillTrigInfoDst.h                                                      *
 *    Fill Trigger information from DIGI chain into DST chain                   *
 *    Sergei Merts                                                              *
 *    Created: May 26 2023                                                      *
 *******************************************************************************/
#ifndef BMNFILLTRIGINFODST_H
#define BMNFILLTRIGINFODST_H

#include "BmnEventHeader.h"
#include "BmnTask.h"
#include "BmnTrigInfoDst.h"
#include "TClonesArray.h"

class BmnFillTrigInfoDst : public BmnTask
{
  public:
    /** Default constructor **/
    BmnFillTrigInfoDst();
    /** Constructor with flag whether experimental data are used **/
    BmnFillTrigInfoDst(Bool_t isExp);

    /** Destructor **/
    ~BmnFillTrigInfoDst();

    /** Initialization of task at the beginning **/
    virtual InitStatus Init();

    /** Executed for each event **/
    virtual void Exec(Option_t* opt);

    /** Finish task called at the end **/
    virtual void Finish();

  private:
    // flag whether experimental data are used in the reconstruction
    Bool_t isExpData;

    // input data
    BmnEventHeader* fDigiEventHeader;
    TClonesArray* fVCDigits;
    TClonesArray* fBC0Digits;
    TClonesArray* fBC1Digits;
    TClonesArray* fBC2Digits;
    TClonesArray* fBDDigits;
    TClonesArray* fSiMDDigits;
    TClonesArray* fFDDigits;

    // output data
    BmnTrigInfoDst* fTrigInfo;

    ClassDef(BmnFillTrigInfoDst, 3);
};

#endif
