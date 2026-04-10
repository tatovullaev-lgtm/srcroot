/**
 * \file BmnSimulationReport.h
 * \brief Base class for simulation reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef BMNSIMULATIONREPORT_H_
#define BMNSIMULATIONREPORT_H_

#include "BmnDetectorList.h"
#include "BmnDrawHist.h"
#include "BmnReport.h"
#include "TF1.h"
#include "TPaveStats.h"

#include <string>

using std::ostream;
using std::string;

class BmnHistManager;

// basic height and wigth of pad
const Int_t baseW = 700;
const Int_t baseH = 500;

/**
 * \class BmnSimulationReport
 * \brief Base class for simulation reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class BmnSimulationReport : public BmnReport
{
  public:
    /**
     * \brief Constructor.
     */
    BmnSimulationReport();

    /**
     * \brief Destructor.
     */
    virtual ~BmnSimulationReport();

    /**
     * \brief Main function which creates report data.
     *
     * Non virtual interface pattern is used here.
     * User always creates simulation report using this public non virtual method.
     * In order to configure concrete report generation class
     * user has to implement protected Create() method.
     *
     * \param[in] histManager Pointer to histogram manager for which report is created.
     * \param[in] outputDir Path to directory for output results.
     */
    void Create(BmnHistManager* histManager, const string& outputDir);
    void Create(const string& outputDir);
    void Create();

    /**
     * \brief Main function which creates report data.
     *
     * Same pattern is used here.
     *
     * \param[in] fileName Name of the file with histograms.
     * \param[in] outputDir Path to directory for output results.
     */
    void Create(const string& fileName, const string& outputDir);

    void SetHM(BmnHistManager* hm) { fHM = hm; }

    /**
     * \brief Return pointer to Histogram manager.
     */
    BmnHistManager* HM() const { return fHM; }

    /**
     * \brief Select by pattern TH1 histograms and draw each histogram on separate canvas.
     * \param[in] histNamePattern Name pattern for histogram.
     */
    void DrawH1ByPattern(const string& histNamePattern);

    /**
     * \brief Select by pattern TH1 histograms and draw all histograms on the same canvas.
     * \param[in] histNamePattern Name pattern for histogram.
     */
    void DrawH1ByPattern(const string& histNamePattern, string (*labelFormatter)(const string&, const BmnHistManager*));

    /**
     * \brief Select by pattern TH2 histograms and draw each histogram on separate canvas.
     * \param[in] histNamePattern Name pattern for histogram.
     * \param[in] logx Specify logarithmic or linear scale for X axis.
     * \param[in] logy Specify logarithmic or linear scale for Y axis.
     * \param[in] logz Specify logarithmic or linear scale for Z axis.
     * \param[in] drawOpt Other drawing options (see ROOT documentation for details).
     */
    void DrawH2ByPattern(const string& histNamePattern,
                         HistScale logx = kLinear,
                         HistScale logy = kLinear,
                         HistScale logz = kLinear,
                         const string& drawOpt = "");

    void SetOnlyPrimes(const Bool_t prime) { fPrimes = prime; }

    Bool_t GetOnlyPrimes() const { return fPrimes; }

    void AddStationSet(DetectorId id, void* ss) { fDetStationSets.insert(std::make_pair(id, ss)); }

    void DrawHits2D();

    static TClass* GetStationSetClass(DetectorId det);
    static char* HistNameHits(string detName, Short_t iSt) { return Form("%s_st%d_x_vs_y", detName.data(), iSt); }
    static char* HistNameHits(TString& detName, Short_t iSt) { return Form("%s_st%d_x_vs_y", detName.Data(), iSt); }
    static char* HistNameStripDigit(TString& detName, Short_t iSt, Short_t iMod, Short_t iLayer)
    {
        return Form("%s_Station_%d_module_%d_layer_%d", detName.Data(), iSt, iMod, iLayer);
    }

  protected:
    void DrawMuSigma(TVirtualPad* pad, TH1* h);
    void DrawOneH1(const TString canvasName,
                   const TString name1,
                   const TString drawOpt,
                   Int_t histW = baseW,
                   Int_t histH = baseH);
    void DrawOneH1Log(const TString canvasName,
                      const TString name1,
                      const TString drawOpt,
                      Int_t histW = baseW,
                      Int_t histH = baseH);
    void DrawTwoH1(const TString canvasName,
                   const TString name1,
                   const TString name2,
                   const TString drawOpt,
                   Bool_t doFit = kFALSE,
                   Int_t histW = baseW,
                   Int_t histH = baseH);
    void DrawOneH2(const TString canvasName, const TString name1, Int_t histW = baseW, Int_t histH = baseH);
    void DrawTwoH2(const TString canvasName,
                   const TString name1,
                   const TString name2,
                   Int_t histW = baseW,
                   Int_t histH = baseH);
    void DrawThreeH1(const TString canvasName,
                     const TString name1,
                     const TString name2,
                     const TString name3,
                     Int_t histW = baseW,
                     Int_t histH = baseH);
    void DrawThreeH2(const TString canvasName,
                     const TString name1,
                     const TString name2,
                     const TString name3,
                     Int_t histW = baseW,
                     Int_t histH = baseH);
    void DrawFourH2(const TString canvasName,
                    const TString name1,
                    const TString name2,
                    const TString name3,
                    const TString name4,
                    Int_t histW = baseW,
                    Int_t histH = baseH);
    void DrawNH2(const string canvasName,
                 const vector<string>& names,
                 Int_t histW = baseW,
                 Int_t histH = baseH,
                 Int_t ncols = 2);
    const Int_t MinNEntries4Fit = 10;

  private:
    BmnHistManager* fHM;   // Histogram manager
    Bool_t fPrimes;

    map<DetectorId, void*> fDetStationSets;

    BmnSimulationReport(const BmnSimulationReport&) = delete;
    BmnSimulationReport& operator=(const BmnSimulationReport&) = delete;

    ClassDef(BmnSimulationReport, 1)
};

#endif /* BmnSIMULATIONREPORT_H_ */
