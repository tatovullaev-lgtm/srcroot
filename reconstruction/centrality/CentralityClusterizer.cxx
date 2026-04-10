#include "CentralityClusterizer.h"

#include <BmnFHCalEvent.h>
#include <BmnHodoEvent.h>
#include <TBranch.h>
#include <TClass.h>
#include <TKey.h>
#include <TROOT.h>
#include <TVector2.h>

CentralityClusterizer::CentralityClusterizer(const std::string& rootFilePath)
    : fRootFilePath(rootFilePath)
    , fNumClusters(0)
{}

CentralityClusterizer::~CentralityClusterizer()
{
    for (auto& entry : fPDF) {
        delete entry.second;   // Delete the TH1F object
    }
    fPDF.clear();   // Clear the map after deleting all entries
    if (fPdfFile)
        fPdfFile->Close();
    if (fBmnEventCentrality)
        delete fBmnEventCentrality;
}

InitStatus CentralityClusterizer::Init()
{
    fworkTime = 0.;
    fpFairRootMgr = FairRootManager::Instance();
    LoadHistograms();

    for (auto& pdf : fPDF) {
        auto key = pdf.first;
        key = key;

        // TODO
        // It would be much better to do something abstract and take a float like
        // fpFairRootMgr->SetBranchAddress(feature.c_str(), fFeature[feature]);
        // But for now I do not know how to do that

        {
            std::string feature = "FHCalEvent";
            BmnFHCalEvent* branch = (BmnFHCalEvent*)fpFairRootMgr->GetObject(feature.c_str());
            if (!branch) {
                LOG(error) << "CentralityClusterizer::Init(): Failed to initialize Branch " << feature
                           << " ! Task will be deactivated.";
                SetActive(kFALSE);
                return kERROR;
            }
        }
        {
            std::string feature = "HodoEvent";
            BmnHodoEvent* branch = (BmnHodoEvent*)fpFairRootMgr->GetObject(feature.c_str());
            if (!branch) {
                LOG(error) << "CentralityClusterizer::Init(): Failed to initialize Branch " << feature
                           << " ! Task will be deactivated.";
                SetActive(kFALSE);
                return kERROR;
            }
        }
    }

    fBmnEventCentrality = new BmnEventCentrality();
    fpFairRootMgr->RegisterAny("EventCentrality", fBmnEventCentrality, kTRUE);
    LOG(debug) << "CentralityClusterizer Reconstructor ready";
    return kSUCCESS;
}

void CentralityClusterizer::LoadHistograms()
{

    std::string dir = getenv("VMCWORKDIR");
    std::string path = dir + "/parameters/centrality/";
    fNumClusters = 0;
    fPdfFile = new TFile((path + fRootFilePath).c_str(), "READONLY");
    TIter nextkey(fPdfFile->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)nextkey())) {

        TClass* cl = gROOT->GetClass(key->GetClassName());
        if (!cl->InheritsFrom("TH1"))
            continue;

        std::string keyName(key->GetName());
        std::smatch matches;
        if (std::regex_search(keyName, matches, fPattern) && matches.size() == 2) {
            int cluster_id = std::stoi(matches[1]);

            // Retrieve the histogram from the file and add it to the map
            TH2F* hist = (TH2F*)fPdfFile->Get(keyName.c_str());
            if (hist) {
                fPDF[cluster_id] = hist;
                LOG(debug) << Form("CentralityClusterizer::LoadHistograms(): %s cluster %d entries %.0f",
                                   keyName.c_str(), cluster_id, fPDF[cluster_id]->GetEntries());

                // Update the number of clusters if necessary
                if (cluster_id + 1 > fNumClusters)
                    fNumClusters = cluster_id + 1;
            } else {
                LOG(error) << "CentralityClusterizer::LoadHistograms(): Failed to retrieve histogram " << keyName;
            }

        } else {
            // Log an error if the key name does not match the pattern
            LOG(error) << "CentralityClusterizer::LoadHistograms(): " << keyName << " No match found.";
        }
    }
}

void CentralityClusterizer::Exec(Option_t* opt)
{
    if (!IsActive())
        return;
    LOG(debug2) << "Exec of CentralityClusterizer";

    TStopwatch sw;
    sw.Start();
    fBmnEventCentrality->Reset();

    float x = ((BmnFHCalEvent*)fpFairRootMgr->GetObject("FHCalEvent"))->GetTotalEnergy();
    float y = ((BmnHodoEvent*)fpFairRootMgr->GetObject("HodoEvent"))->GetTotalSignal();

    std::vector<float> prob(fNumClusters);
    for (auto& pdf : fPDF) {
        auto cluster_id = pdf.first;
        TH2F* hist = pdf.second;
        if (!hist) {
            LOG(error) << "CentralityClusterizer::Exec(): Failed to get histogram " << cluster_id;
            continue;
        }

        // Handle the case where x or y are outside the histogram's range
        if (x < hist->GetXaxis()->GetXmin() || x > hist->GetXaxis()->GetXmax() || y < hist->GetYaxis()->GetXmin()
            || y > hist->GetYaxis()->GetXmax())
        {
            continue;
        }

        float probability = hist->Interpolate(x, y);
        LOG(debug2) << Form("CentralityClusterizer::Exec(): value fhcal %.3f value_hodo %.3f cluster %d prob %.3f", x,
                            y, cluster_id, probability);
        prob.at(cluster_id) = probability;
    }

    for (int cluster_id = 0; cluster_id < fNumClusters; ++cluster_id) {
        fBmnEventCentrality->SetCentrality(cluster_id, prob.at(cluster_id), BmnCentralityClass::Method::FHCalHodo);
    }
    LOG(debug2) << Form("CentralityClusterizer::Exec(): class %d prob %.3f", fBmnEventCentrality->GetClass(),
                        fBmnEventCentrality->GetProbability());

    // handle case of empty bin. Set class by closest centroid
    if (fBmnEventCentrality->GetProbability() < std::numeric_limits<float>::epsilon()) {
        fBmnEventCentrality->Reset();
        TVector2 this_event(x, y);
        float min_distance = std::numeric_limits<float>::max();
        int candida = -1;
        for (auto& pdf : fPDF) {
            auto cluster_id = pdf.first;
            TH2F* hist = pdf.second;
            if (!hist) {
                LOG(error) << "CentralityClusterizer::Exec(): Failed to get histogram " << cluster_id;
                continue;
            }

            TVector2 centroid(hist->GetMean(1), hist->GetMean(2));
            float distance = (this_event - centroid).Mod();
            if (distance < min_distance) {
                min_distance = distance;
                candida = cluster_id;
            }
        }
        fBmnEventCentrality->SetCentrality(candida, 0.0, BmnCentralityClass::Method::FHCalHodo);
    }

    sw.Stop();
    fworkTime += sw.RealTime();
}

void CentralityClusterizer::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch("EventCentrality", &fBmnEventCentrality);
    resultTree->Fill();
}

void CentralityClusterizer::Finish()
{
    printf("Work time of CentralityClusterizer: %4.2f sec.\n", fworkTime);
}
