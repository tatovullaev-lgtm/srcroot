/**
 * \file BmnQaBase.h
 * \brief FairTask for tracking performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru> - disentangle Exp and MC processing
 * \date 2007-2022
 */

#ifndef BMNQABASE_H_
#define BMNQABASE_H_

#include "BmnCSCStationSet.h"
#include "BmnGemStripStationSet.h"
#include "BmnMCTrackCreator.h"
#include "BmnSiliconStationSet.h"
#include "BmnTrackingQaExpReport.h"
#include "BmnVSPStationSet.h"
#include "CbmVertex.h"
#include "FairTask.h"

#include <TSystem.h>
#include <chrono>
#include <time.h>

class BmnHistManager;

using std::string;

class BmnQaBase : public FairTask
{
#define QA_COORD_BINS 250
#define QA_COORD_INDENT_X 1
#define QA_COORD_INDENT_Y 1
  public:
    /**
     * \brief Constructor.
     */
    BmnQaBase() {};

    /**
     * \brief Constructor.
     */
    BmnQaBase(const char* name, Int_t iVerbose);

    /**
     * \brief Destructor.
     */
    virtual ~BmnQaBase();

    /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

    void SetOutputDir(const std::string& dir) { fOutputDir = dir; }

    void SetMonitorMode(const Bool_t mm) { fMonitorMode = mm; }

    Bool_t GetMonitorMode() const { return fMonitorMode; }
    void SetObjServer(THttpServer* s) { fServer = s; }
    THttpServer* GetObjServer() const { return fServer; }

  protected:
    /**
     * \brief Read data branches from input data files.
     */
    virtual void ReadDataBranches() = 0;

    virtual void ReadEventHeader();

    virtual void CreateHistograms() = 0;
    virtual void ProcessGlobal() = 0;

    void CreateH1(const string& name,
                  const string& xTitle,
                  const string& yTitle,
                  Int_t nofBins,
                  Double_t minBin,
                  Double_t maxBin);

    void CreateH2(const string& name,
                  const string& xTitle,
                  const string& yTitle,
                  const string& zTitle,
                  Int_t nofBinsX,
                  Double_t minBinX,
                  Double_t maxBinX,
                  Int_t nofBinsY,
                  Double_t minBinY,
                  Double_t maxBinY);

    template<typename SST>
    void CreateStripDigiHists(SST& ss, DetectorId detId)
    {
        TString detName;
        BmnDetectorList::GetSystemNameCaps(detId, detName);
        UInt_t sumMods(0);
        UInt_t maxLayers(0);
        UInt_t maxMods(0);
        for (Short_t iSt = 0; iSt < ss->GetNStations(); ++iSt) {
            auto* st = ss->GetStation(iSt);
            sumMods += st->GetNModules();
            if (maxMods < st->GetNModules())
                maxMods = st->GetNModules();
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                auto* mod = st->GetModule(iModule);
                if (maxLayers < mod->GetNStripLayers())
                    maxLayers = mod->GetNStripLayers();

                for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                    auto& lay = mod->GetStripLayer(iLayer);
                    CreateH1(BmnSimulationReport::HistNameStripDigit(detName, iSt, iModule, iLayer), "Strip #", "Count",
                             "", lay.GetNStrips(), 0, lay.GetNStrips());
                    //                BmnHist::SetHistStyleTH1(h);
                }
            }
        }
    }

    void FillStripDigiHists(DetectorId detId, TClonesArray* hits);

    template<typename SST>
    void CreateStripHits(SST& ss, DetectorId detId)
    {
        TString detName;
        BmnDetectorList::GetSystemNameCaps(detId, detName);
        for (Short_t iSt = 0; iSt < ss->GetNStations(); ++iSt) {
            auto* st = ss->GetStation(iSt);
            int32_t coord_bins = (ss->GetNStations() == 1) ? 400 : QA_COORD_BINS;
            CreateH2(BmnSimulationReport::HistNameHits(detName, iSt), "x cm", "y cm", "", coord_bins,
                     -st->GetXMinStation() - QA_COORD_INDENT_X, -st->GetXMaxStation() + QA_COORD_INDENT_X, coord_bins,
                     st->GetYMinStation() - QA_COORD_INDENT_Y, st->GetYMaxStation() + QA_COORD_INDENT_Y);
        }
    }

    void FillStripHits(DetectorId detId, TClonesArray* hits);

    THttpServer* fServer;
    TString fOutName;

    BmnHistManager* fHM;   // Histogram manager
    string fOutputDir;     // Output directory for results

    BmnSimulationReport* fReport;

    Bool_t fMonitorMode;

    UInt_t fRunId;
    UInt_t fPrevRunId;
    Int_t fEventNo;   // event counter
    Int_t fEventId;   // event id

    const Int_t fNItersToUpdate = 500;
    const chrono::seconds fTimeToUpdate = chrono::seconds(5);   //! redraw each timeout seconds
    Int_t fNItersSinceUpdate;
    chrono::time_point<chrono::system_clock> fTicksLastUpdate;   //!

    BmnQaBase(const BmnQaBase&) = delete;
    BmnQaBase& operator=(const BmnQaBase&) = delete;

    ClassDef(BmnQaBase, 1);
};

#endif /* BMNQABASE_H_ */
