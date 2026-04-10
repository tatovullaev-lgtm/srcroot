
#ifndef BMNHIST_H
#define BMNHIST_H 1

#include "BmnEventHeader.h"
#include "DigiArrays.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TFolder.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THStack.h"
#include "THttpServer.h"
#include "TString.h"

#include <BmnPadBranch.h>
#include <PadInfo.h>
#include <TNamed.h>
#include <stdlib.h>
#include <vector>

#define PAD_WIDTH 512
#define PAD_HEIGHT 384

using std::unique_ptr;
using std::vector;

/**
 * Base class for histogram sets
 */
class BmnHist : public TNamed
{
  public:
    BmnHist(Int_t periodID = 7, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHist();
    virtual void Reset() = 0;
    virtual void Register(THttpServer* serv) = 0;
    virtual void SetDir(TFile* outFile = NULL, TTree* recoTree = NULL) = 0;
    virtual void DrawBoth() = 0;
    virtual void FillFromDigi(DigiArrays* fDigiArrays) = 0;
    virtual void ClearRefRun() = 0;
    //    virtual BmnStatus LoadRefRun(TString FileName) = 0;
    //    template <class HH>
    static void DrawRef(unique_ptr<TCanvas>& canGemStrip, vector<PadInfo*>* canGemStripPads);
    static void DrawRef(TCanvas* canGemStrip, vector<PadInfo*>* canGemStripPads);
    static void DrawPad(TVirtualPad* pad, PadInfo* info);
    static void FillPad(PadInfo* info, TTree* tree);
    static BmnStatus LoadRefRun(Int_t refID,
                                TString FullName,
                                TString fTitle,
                                vector<PadInfo*> canPads,
                                vector<TString> Names);
    static BmnStatus LoadRefRun(Int_t refID, TString FullName, TString fTitle, vector<PadInfo*> canPads);
    static BmnStatus DrawPadFromTree(BmnPadBranch* br);
    static void ResetPadTree(BmnPadBranch* br);
    static BmnStatus FillPadFromTree(BmnPadBranch* br, TTree* tree);
    static void SetHistStyleTH1(TH1* h);

    void SetRefRunName(TString v) { this->refRunName = v; }

    TString GetRefRunName() const { return refRunName; }

    void SetRefID(Int_t v) { this->refID = v; }

    Int_t GetRefID() const { return refID; }

    void SetRefPath(TString v) { this->refPath = v; }

    TString GetRefPath() const { return refPath; }

    void SetPeriodID(Int_t v) { this->fPeriodID = v; }

    Int_t GetPeriodID() const { return fPeriodID; }

    void SetBmnSetup(BmnSetup v) { this->fSetup = v; }

    BmnSetup GetBmnSetup() const { return fSetup; }

    void SetShow(Bool_t v) { this->isShown = v; }

    Bool_t GetShow() const { return isShown; }

  protected:
    Bool_t isShown;
    THttpServer* fServer;
    TTree* frecoTree;
    TDirectory* fDir;
    TString refPath;
    TString refRunName;
    Int_t refID;
    TFile* refFile;
    Int_t fPeriodID;
    BmnSetup fSetup;

    ClassDef(BmnHist, 1)
};

#endif /* BMNHIST_H */
