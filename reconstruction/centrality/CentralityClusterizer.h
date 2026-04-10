/** \file CentralityClusterizer.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 05.03.2024
 **/

/** \class CentralityClusterizer
 ** \brief Class for event centrality determination through clusterization
 ** \version 1.0
 **/

#ifndef CENTRALITY_CLUSTERIZER_H
#define CENTRALITY_CLUSTERIZER_H

#include "FairLogger.h"
#include "FairRootManager.h"

#include "BmnTask.h"
#include "BmnEventCentrality.h"

#include <string>
#include <vector>
#include <utility>
#include <regex>

#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TStopwatch.h>
#include <TClonesArray.h>

class CentralityClusterizer : public BmnTask {
public:
    CentralityClusterizer(const std::string& rootFilePath);
    virtual ~CentralityClusterizer();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    virtual void OnlineWrite(const std::unique_ptr<TTree>& resultTree);

private:
    FairRootManager* fpFairRootMgr = nullptr;
    TFile* fPdfFile = nullptr; // file with histograms of PDFs 
    std::string fRootFilePath; // file with histograms of PDFs 

    int fNumClusters;
    std::map<int, TH2F*> fPDF; // map <cluster> to histogram holding probability density function
    static inline const std::regex fPattern{ R"(cluster_(\d+))" }; // cluster_%d
    BmnEventCentrality* fBmnEventCentrality = nullptr; // output

    void LoadHistograms();
    float fworkTime;

  ClassDef(CentralityClusterizer, 1);
};

#endif // CENTRALITY_CLUSTERIZER_H
