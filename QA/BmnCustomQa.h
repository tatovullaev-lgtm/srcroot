#ifndef BMNCUSTOMQA_H
#define BMNCUSTOMQA_H

#include "BmnHist.h"
#include "BmnPadBranch.h"
#include "BmnPadGenerator.h"
#include "BmnQaBase.h"
#include "FairTask.h"

#include <TBranch.h>
#include <TClass.h>
#include <TObjString.h>
#include <TTree.h>
#include <chrono>
#include <time.h>
// #include "BmnCustomQaReport.h"

using std::string;
using std::vector;

class BmnCustomQa : public BmnQaBase
{
  public:
    /**
     * \brief Constructor.
     */
    BmnCustomQa() {};

    /**
     * \brief Constructor.
     */
    BmnCustomQa(string name, string padConfFile);

    /**
     * \brief Destructor.
     */
    virtual ~BmnCustomQa();

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

    /*
     * Reset all histograms in thepad tree
     */
    void ResetPadTree();

    /*
     * Save pad config from page to file on disk
     */
    void SaveConfig(string str);

    /*
     * Load default config from file on disk
     */
    TObjString* LoadDefaultConfig();

    /*
     * Get current config
     */
    TObjString* FetchConfig();

  private:
    void Redraw();

    /**
     * \brief Read data branches from input data files.
     */
    void ReadDataBranches();

    void CreateHistograms();

    void RecreateHistTree();

    void ProcessGlobal();

    void ReloadConf();

    string fPadConfDirWeb;
    string fPadConfDir;
    string fPadConfFileNameDefault;
    string fPadConfFileName;
    string fPadConfText;
    BmnPadGenerator* fPadGenerator;   //!
    BmnPadBranch* fPadTree;           //!

    TTree* fTreeTemp;   //!
                        //    vector<TObject*> fNamVec;
                        //    vector<TClonesArray*> fArrVec;

    TCanvas* can;         //!
    std::mutex mtx_can;   //!

    TObjString fCurConfig;   //!

    BmnCustomQa(const BmnCustomQa&) = delete;
    BmnCustomQa& operator=(const BmnCustomQa&) = delete;

    ClassDef(BmnCustomQa, 1);
};

#endif /* BMNCUSTOMQA_H */
