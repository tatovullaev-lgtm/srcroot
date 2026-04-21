#ifndef BMNPIDSRC_H
#define BMNPIDSRC_H

#include "BmnDchTrack.h"
#include "DstEventHeader.h"
#include "../../detectors/mwpc/BmnMwpcTrack.h"
#include "FairTask.h"
#include "TClonesArray.h"
#include "TString.h"
#include "TMath.h"
#include "BmnEnums.h"
#include "FairRunAna.h"
#include "FairField.h"

#include "TFile.h"

using namespace std;

class BmnPidSRC : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnPidSRC();
    //BmnPidSRC(Int_t Z);
    virtual ~BmnPidSRC() {};

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void PReco();


private:

    // Private Data Members ------------
    TString fDstEventHeaderBranchName;
    TString fDchTrackBranchName;
    TString fMwpcTrackBranchName;


 
    TClonesArray* fBmnDchTrack;
    TClonesArray* fBmnMwpcTrack;
    DstEventHeader* fDstEventHeader;

    vector <double> fPq, fPq_MDF, fPq_MDF1, fPq_MDF2, fPq_MDF3, fTx_MDF;
    vector <bool> fB10, fB11;
    int fDCH_Mult, fMWPC_Mult;
    ClassDef(BmnPidSRC, 1);
};

#endif /* BMNPIDSRC_H */
