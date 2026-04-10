// -------------------------------------------------------------------------
// -----               BmnParticleEqualizer source file                    -----
// -----          Created 01/05/2024 by K. Gertsenberger               -----
// -------------------------------------------------------------------------

#include "BmnParticleEqualizer.h"

#include "BmnGlobalTrack.h"
#include "BmnMCInfoDst.h"
#include "CbmMCTrack.h"
#include "CbmStack.h"
#include "FairLogger.h"
#include "FairMCEventHeader.h"
#include "FairRootManager.h"
#include "FairTrackParam.h"
#include "Math/MinimizerOptions.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include "TFile.h"
#include "TFitResult.h"
#include "THashList.h"
#include "TLegend.h"
#include "TMath.h"
#include "TParticlePDG.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TVirtualMC.h"

// iParticleCut: 0 - all primary particles,
// 1 - exclude tracks without points in Silicon and GEMs,
// 2 - exclude tracks without points in Silicon, GEM and ToF detectors
static const int iParticleCut = 2;

Double_t BmnParticleEqualizer::fPMin = 0.4;   ///< Min value of momentum
Double_t BmnParticleEqualizer::fPMax = 5.4;   ///< Max value of momentum
///< Interval count for momentum to equalize particles at all the intervals
Int_t BmnParticleEqualizer::fIntervalCount = 25;
// particle PDG codes to be equalized:           p,   pi+,  pi-, K+,   K-      ,  e-,  e+
vector<Int_t> BmnParticleEqualizer::fParticles({2212, 211, -211, 321, -321});   //, 11, -11});
// their masses in MeV:                      938.272,139.6,139.6,493.7,493.7

Double_t fPtMin = 0, fPtMax = 3, fYMin = 0, fYMax = 3, fEtaMin = -4, fEtaMax = 8, fThetaMin = 0, fThetaMax = 3.2,
         fPhiMin = -3.2, fPhiMax = 3.2;

// Default 'stParticleInfo' constructor for storing particle distrubutons
BmnParticleEqualizer::stParticleInfo::stParticleInfo(int pdg_key, Double_t min_p, Double_t max_p, Int_t interval_count)
    : fY0(0)
    , fSigma(0.72)
    , fT(0.223)
{
    // construct and initialize vector for particle numbers
    fParticleNumber.resize(interval_count);
    fill(fParticleNumber.begin(), fParticleNumber.end(), 0);

    // construct vector with P histograms divided by momentum intervals
    fPHistVector.resize(interval_count);
    fEtaHistVector.resize(interval_count);
}

// 'BmnParticleEqualizer' constructor with input histograms
BmnParticleEqualizer::BmnParticleEqualizer(TString hist_file_name)
    : FairGenerator()
    , fIntervalStep((fPMax - fPMin) / fIntervalCount)
{
    fMaxParticles.resize(fIntervalCount);
    fill(fMaxParticles.begin(), fMaxParticles.end(), 0);

    // initialize map with particle information
    for (auto pdg_key : fParticles)
        fParticleInfos.emplace(pdg_key, make_unique<stParticleInfo>(pdg_key, fPMin, fPMax, fIntervalCount));

    isActive = ReadSampleHistograms(hist_file_name);
    if (!isActive)
        LOG(error) << "Errors occured while initializing BmnParticleEqualizer generator, it will be skipped";
}

// Constructor with specified particle types
BmnParticleEqualizer::BmnParticleEqualizer(TString hist_file_name,
                                           vector<Int_t> pdg_codes,
                                           Double_t min_p,
                                           Double_t max_p,
                                           Int_t intervals)
    : FairGenerator()
{
    fPMin = min_p;
    fPMax = max_p;
    fIntervalCount = intervals;
    fIntervalStep = (max_p - min_p) / intervals;

    // set fParticles to given pdg_codes
    fParticles.swap(pdg_codes);

    fMaxParticles.resize(fIntervalCount);
    fill(fMaxParticles.begin(), fMaxParticles.end(), 0);

    // initialize map with particle information
    for (auto pdg_key : fParticles)
        fParticleInfos.emplace(pdg_key, make_unique<stParticleInfo>(pdg_key, fPMin, fPMax, fIntervalCount));

    isActive = ReadSampleHistograms(hist_file_name);
    if (!isActive)
        LOG(error) << "Errors occured while initializing BmnParticleEqualizer generator, it will be skipped";
}

// Initialize generator
Bool_t BmnParticleEqualizer::Init()
{
    return kTRUE;
}

// fit P histogram to generate the values according to the analytical functions
int BmnParticleEqualizer::FitPHistogram(unique_ptr<TH1D>& hP, int pdg_key, Double_t p_min, Double_t p_max)
{
    if (hP->GetEntries() == 0) {
        LOG(warning) << TString::Format("P (momentum) histogram is empty for PDG = %d", pdg_key);
        return -1;
    }

    // fit P distribution for the particle type by gaus + landau
    TString dist_func_name = TString::Format("distP_pdg%d", pdg_key);
    TF1* p_distribution = new TF1(dist_func_name, "gaus(0)+gaus(3)+landau(6)", p_min, p_max);
    Double_t p_avg = (p_min + p_max) / 2;
    // Double_t y_max_value = hP->GetBinContent(hP->GetMaximumBin());
    Double_t x_max_value = hP->GetXaxis()->GetBinUpEdge(hP->GetMaximumBin());
    p_distribution->SetParLimits(1, p_min, p_avg);
    p_distribution->SetParameter(1, p_min);
    p_distribution->SetParLimits(2, 0.1, p_avg);
    p_distribution->SetParameter(2, p_avg / 4);
    p_distribution->SetParLimits(4, x_max_value - 0.1, p_max);
    p_distribution->SetParameter(4, x_max_value - 0.1);
    p_distribution->SetParLimits(5, 0.1, p_avg);
    p_distribution->SetParameter(5, p_avg / 2);
    p_distribution->SetParLimits(7, p_min, TMath::Min(x_max_value, p_avg));
    p_distribution->SetParameter(7, p_min);
    p_distribution->SetParLimits(8, 0.1, p_avg);
    p_distribution->SetParameter(8, p_avg / 4);

    TFitResultPtr r = hP->Fit(p_distribution, "SQ0");
    if (!r->IsValid()) {
        LOG(error) << TString::Format("Fit of the P (momentum) histogram failed for PDG = %d", pdg_key);
        // return -2;
    }

    return 0;
}

// fit Pt histogram to generate the values according to the analytical functions
int BmnParticleEqualizer::FitPtHistogram(unique_ptr<TH1D>& hPt,
                                         int pdg_key,
                                         Double_t pdg_mass,
                                         Double_t pt_min,
                                         Double_t pt_max,
                                         Double_t& t_output)
{
    if (hPt->GetEntries() == 0) {
        LOG(warning) << TString::Format("Pt (transverse momentum) histogram is empty for PDG = %d", pdg_key);
        return -1;
    }

    // fit Pt distribution for the particle type by expo
    TString dist_func_name = TString::Format("distPt_pdg%d", pdg_key);
    TF1* pt_distribution =
        new TF1(dist_func_name, TString::Format("[0]*x*exp(-sqrt(x*x+%f)/[1])", pdg_mass * pdg_mass), pt_min, pt_max);
    pt_distribution->SetParameter(1, t_output);

    TFitResultPtr r = hPt->Fit(pt_distribution, "SQ0");
    t_output = r->Parameter(1);

    return 0;
}

// fit Y histogram to generate the values according to the analytical functions
int BmnParticleEqualizer::FitYHistogram(unique_ptr<TH1D>& hY, int pdg_key, Double_t& y0_output, Double_t& sigma_putput)
{
    if (hY->GetEntries() == 0) {
        LOG(warning) << TString::Format("Rapidity histogram is empty for %d PDG code", pdg_key);
        return -1;
    }

    // fit rapidity distribution for the particle type by gauss
    if (pdg_key != 2212) {
        TFitResultPtr r = hY->Fit("gaus", "SQ0");
        y0_output = r->Parameter(1);
        sigma_putput = r->Parameter(2);
        // if proton (PDG == 2212)
    } else {
        Double_t max_bin_value = hY->GetMaximum();
        const string minimizer_type = ROOT::Math::MinimizerOptions::DefaultMinimizerType();
        ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Genetic");

        // FITTING ONLY MEDIUM PEAK with background
        // TF1* fProtonYDistribution = new TF1("distProtonY", "gaus(0)+pol0(3)", 0.3, 2);
        // fProtonYDistribution->SetParLimits(0, 0, max_bin_value);
        // fProtonYDistribution->SetParLimits(1, 0.3, 2);
        // fProtonYDistribution->SetParName(1,"proton_y0");
        // fProtonYDistribution->SetParLimits(2, 0.05, 1);
        // fProtonYDistribution->SetParName(2,"proton_sigma");
        // fProtonYDistribution->SetParLimits(3, 0, max_bin_value);
        // TFitResultPtr r = hY->Fit(fProtonYDistribution,"SQ0","", 0.3, 2);
        // FITTING THREE PEAKS: TARGET FRAGMENTATION + PARTICIPANTS + SPECTATORS
        unique_ptr<TF1> fProtonYDistribution =
            make_unique<TF1>("distProtonY", "gaus(0)+gaus(3)+gaus(6)", 0.2, 5);   // 0, 5);
        fProtonYDistribution->SetParLimits(0, 0, max_bin_value);
        fProtonYDistribution->SetParLimits(1, 0, 0.05);
        fProtonYDistribution->SetParLimits(2, 0.01, 0.4);
        fProtonYDistribution->SetParameter(2, 0.2);
        fProtonYDistribution->SetParLimits(3, 0, max_bin_value);
        fProtonYDistribution->SetParName(4, "proton_y0");
        fProtonYDistribution->SetParLimits(4, 0.3, 2);
        fProtonYDistribution->SetParameter(4, 1);
        fProtonYDistribution->SetParName(5, "proton_sigma");
        fProtonYDistribution->SetParLimits(5, 0.05, 1);
        fProtonYDistribution->SetParameter(5, 0.5);
        fProtonYDistribution->SetParLimits(6, 0, max_bin_value);
        fProtonYDistribution->SetParLimits(7, 1.5, 3);
        fProtonYDistribution->SetParameter(7, 2.25);
        fProtonYDistribution->SetParLimits(8, 0.05, 1);
        fProtonYDistribution->SetParameter(8, 0.5);

        TFitResultPtr r = hY->Fit(fProtonYDistribution.get(), "SQ0", "", 0, 5);
        y0_output = r->Parameter(r->Index("proton_y0"));
        sigma_putput = r->Parameter(r->Index("proton_sigma"));

        ROOT::Math::MinimizerOptions::SetDefaultMinimizer(minimizer_type.c_str());
    }

    return 0;
}

// fit Eta (pseudorapidity) histogram to generate the values according to the analytical functions
int BmnParticleEqualizer::FitEtaHistogram(unique_ptr<TH1D>& hEta, int pdg_key, Double_t eta_min, Double_t eta_max)
{
    if (hEta->GetEntries() == 0) {
        LOG(warning) << TString::Format("Eta (pseudorapidity) histogram is empty for PDG = %d", pdg_key);
        return -1;
    }

    // fit P distribution for the particle type by gaus + landau
    TString dist_func_name = TString::Format("distEta_pdg%d", pdg_key);
    TF1* eta_distribution = new TF1(dist_func_name, "crystalball", eta_min, eta_max);
    eta_distribution->SetParLimits(1, 1, 4);
    eta_distribution->SetParameter(1, 2.5);
    eta_distribution->SetParLimits(2, 0.1, 1.5);
    eta_distribution->SetParameter(2, 0.5);
    eta_distribution->SetParLimits(3, 0.1, 2);
    eta_distribution->SetParameter(3, 1);
    eta_distribution->SetParLimits(4, 1, 10);
    eta_distribution->SetParameter(4, 3);

    TFitResultPtr r = hEta->Fit(eta_distribution, "SQ0");
    if (!r->IsValid()) {
        LOG(error) << TString::Format("Fit of the Eta (pseudorapidity) histogram failed for PDG = %d", pdg_key);
        // return -2;
    }

    return 0;
}

int BmnParticleEqualizer::ProduceSampleHistograms(TString input_list_file, TString output_histo_file)
{
    gSystem->ExpandPathName(input_list_file);

    // get file list from the input text file
    list<string> file_list;
    ifstream list_file(input_list_file.Data());
    string s;
    while (getline(list_file, s))
        file_list.push_back(s);
    if (file_list.empty()) {
        cout << "WARNING: There are no files in the list of the input text file: " << input_list_file << endl;
        return 0;
    }

    // form ROOT chain for the file list
    TChain dataChain("bmndata");
    for (auto const& cur_file_path : file_list) {
        TString strFilePath(cur_file_path);
        gSystem->ExpandPathName(strFilePath);

        dataChain.AddFile(strFilePath);
    }

    if (dataChain.GetEntries() < 1) {
        cout << "WARNING: There are no correct input files in the text list stored at: \"" << input_list_file << "\""
             << endl;
        return 0;
    }

    // flag whether the input files have SIM or DST format
    bool isDST = false;
    TClonesArray* mcTracks = nullptr;
    BmnMCInfoDst* mcInfo = nullptr;
    if (dataChain.GetBranch("MCTrack") == nullptr) {
        LOG(info) << "'MCTrack' branch was not found, searching for 'BmnMCInfo->MCTrack'...";
        if (dataChain.GetBranch("BmnMCInfo.") == nullptr) {
            LOG(error) << "'BmnMCInfo.' branch was not found too, please, check the structure of the files, exiting...";
            return -1;
        }
        dataChain.SetBranchAddress("BmnMCInfo.", &mcInfo);
        if (mcInfo == nullptr) {
            LOG(error)
                << "'BmnMCInfo.' branch was not read correctly, please, check the structure of the files, exiting...";
            return -2;
        }
        isDST = true;
        LOG(info) << "'BmnMCInfo->MCTrack' has been found";
        dataChain.SetBranchStatus("*", 0);
        dataChain.SetBranchStatus("BmnMCInfo*", 1);
    } else {
        dataChain.SetBranchAddress("MCTrack", &mcTracks);
        if (mcTracks == nullptr) {
            LOG(error)
                << "MCTrack' branch was not read correctly, please, check the structure of the files, exiting...";
            return -3;
        }
        dataChain.SetBranchStatus("*", 0);
        dataChain.SetBranchStatus("MCTrack*", 1);
    }

    int iEventNumber = dataChain.GetEntries();
    if (iEventNumber < 1) {
        cout << "WARNING: There are no correct entries in the file list stored at: \"" << input_list_file << "\""
             << endl;
        return 0;
    }

    TFile* hist_file = TFile::Open(output_histo_file, "RECREATE");

    // create map with particle infos
    map<Int_t, unique_ptr<stParticleInfo>> particle_infos;
    for (auto pdg_key : fParticles)
        particle_infos.emplace(pdg_key, make_unique<stParticleInfo>(pdg_key, fPMin, fPMax, fIntervalCount));

    Double_t interval_width = (fPMax - fPMin) / fIntervalCount;
    // create all the histogramавторефератеs using make_unique
    for (auto pdg_key : fParticles) {
        TString histogram_name = TString::Format("hP_pdg%d", pdg_key);
        particle_infos[pdg_key]->fPHist = make_unique<TH1D>(histogram_name, histogram_name, 80, fPMin, fPMax);
        histogram_name = TString::Format("hPt_pdg%d", pdg_key);
        particle_infos[pdg_key]->fPtHist = make_unique<TH1D>(histogram_name, histogram_name, 60, fPtMin, fPtMax);
        histogram_name = TString::Format("hY_pdg%d", pdg_key);
        particle_infos[pdg_key]->fYHist = make_unique<TH1D>(histogram_name, histogram_name, 60, fYMin, fYMax);
        histogram_name = TString::Format("hEta_pdg%d", pdg_key);
        particle_infos[pdg_key]->fEtaHist = make_unique<TH1D>(histogram_name, histogram_name, 60, fEtaMin, fEtaMax);
        histogram_name = TString::Format("hTheta_pdg%d", pdg_key);
        particle_infos[pdg_key]->fThetaHist =
            make_unique<TH1D>(histogram_name, histogram_name, 61, fThetaMin, fThetaMax);
        histogram_name = TString::Format("hPhi_pdg%d", pdg_key);
        particle_infos[pdg_key]->fPhiHist = make_unique<TH1D>(histogram_name, histogram_name, 64, fPhiMin, fPhiMax);
        int hist_counter = 0;
        for (auto& p_hist : particle_infos[pdg_key]->fPHistVector) {
            histogram_name = TString::Format("hP%d_pdg%d", ++hist_counter, pdg_key);
            p_hist = make_unique<TH1D>(histogram_name, histogram_name, 40, fPMin + (hist_counter - 1) * interval_width,
                                       fPMin + hist_counter * interval_width);
        }
        hist_counter = 0;
        for (auto& eta_hist : particle_infos[pdg_key]->fEtaHistVector) {
            histogram_name = TString::Format("hEta%d_pdg%d", ++hist_counter, pdg_key);
            eta_hist = make_unique<TH1D>(histogram_name, histogram_name, 60, -4, 8);
        }
    }

    // fill all the histograms for specified particles by the PDG code
    int iCurrentEvent = 0;
    LOG(info) << "Processing events...";
    for (Int_t iEvent = 0; iEvent < iEventNumber; iEvent++) {
        // Next Event
        iCurrentEvent++;
        if (iCurrentEvent % 1000 == 0)
            LOG(info) << "Processing event #" << iCurrentEvent << " of " << iEventNumber;
        dataChain.GetEntry(iEvent);

        TClonesArray* mc_tracks = nullptr;
        if (isDST)
            mc_tracks = mcInfo->GetMCTracks();
        else
            mc_tracks = mcTracks;

        for (int i = 0; i < mc_tracks->GetEntries(); i++) {
            CbmMCTrack* pTrack = (CbmMCTrack*)mc_tracks->At(i);
            Int_t part_code = pTrack->GetPdgCode();

            // exclude secondary particles
            if (pTrack->GetMotherId() >= 0)
                continue;
            // filter particles
            if (iParticleCut > 0) {
                // exclude tracks without points in Silicon and GEMs
                if ((pTrack->GetNPoints(kGEM) < 1) || (pTrack->GetNPoints(kSILICON) < 1))
                    continue;
                if (iParticleCut > 1) {
                    // exclude tracks without points in ToF detectors
                    if ((pTrack->GetNPoints(kTOF) < 1) && (pTrack->GetNPoints(kTOF1) < 1)
                        && (pTrack->GetNPoints(kTOF701) < 1))
                        continue;
                }
            }

            if (auto part_iter = particle_infos.find(part_code); part_iter != particle_infos.end()) {
                int interval_number = (pTrack->GetP() - fPMin) / interval_width;
                part_iter->second->fPHist->Fill(pTrack->GetP());
                part_iter->second->fPtHist->Fill(pTrack->GetPt());
                part_iter->second->fYHist->Fill(pTrack->GetRapidity());
                part_iter->second->fEtaHist->Fill(
                    0.5 * TMath::Log((pTrack->GetP() + pTrack->GetPz()) / (pTrack->GetP() - pTrack->GetPz())));
                part_iter->second->fThetaHist->Fill(
                    TMath::ACos(pTrack->GetPz() / pTrack->GetP()));   // *TMath::RadToDeg()
                part_iter->second->fPhiHist->Fill(
                    TMath::ATan2(pTrack->GetPy(), pTrack->GetPx()));   // *TMath::RadToDeg()
                // compare doubles: abs(min(a,b))*numeric_limits<double>::epsilon()*error_factor
                if ((pTrack->GetP() >= fPMin) && (isless(pTrack->GetP(), fPMax))) {
                    part_iter->second->fPHistVector[interval_number]->Fill(pTrack->GetP());
                    part_iter->second->fEtaHistVector[interval_number]->Fill(
                        0.5 * TMath::Log((pTrack->GetP() + pTrack->GetPz()) / (pTrack->GetP() - pTrack->GetPz())));
                }
            }
        }   // for (int i = 0; i < mc_tracks->GetEntries(); i++) {
    }   // while (sim_reader.Next())

    // check correctness of all the histograms
    for (auto pdg_key : fParticles) {
        if (particle_infos[pdg_key]->fPHist->GetEntries() == 0) {
            LOG(error) << "Momentum histogram is empty for pdg=" << pdg_key;
            return -4;
        }
        if (particle_infos[pdg_key]->fPtHist->GetEntries() == 0) {
            LOG(error) << "Transverse momentum histogram is empty for pdg=" << pdg_key;
            return -5;
        }
        if (particle_infos[pdg_key]->fYHist->GetEntries() == 0) {
            LOG(error) << "Rapidity histogram is empty for pdg=" << pdg_key;
            return -6;
        }
        if (particle_infos[pdg_key]->fEtaHist->GetEntries() == 0) {
            LOG(error) << "Pseudorapidity histogram is empty for pdg=" << pdg_key;
            return -7;
        }
        if (particle_infos[pdg_key]->fThetaHist->GetEntries() == 0) {
            LOG(error) << "Theta angle histogram is empty for pdg=" << pdg_key;
            return -8;
        }
        if (particle_infos[pdg_key]->fPhiHist->GetEntries() == 0) {
            LOG(error) << "Phi angle histogram is empty for pdg=" << pdg_key;
            return -9;
        }
    }

    // fit histograms and calculate fY0, fSigma, fT for all the specified particle types
    Double_t fY0 = 0, fSigma = 0.72, fT = 0.223;
    for (auto pdg_key : fParticles) {
        TDatabasePDG* pdgBase = TDatabasePDG::Instance();
        TParticlePDG* particle = pdgBase->GetParticle(pdg_key);
        if (!particle) {
            LOG(fatal) << "PDG code " << pdg_key << "is not defined. exiting...";
            return -10;
        }
        Double_t fPDGMass = particle->Mass();

        // fit full particle momentum histograms
        if (FitPHistogram(particle_infos[pdg_key]->fPHist, pdg_key, fPMin, fPMax) != 0)
            return -11;

        // fit Pt momentum histograms
        if (FitPtHistogram(particle_infos[pdg_key]->fPtHist, pdg_key, fPDGMass, fPtMin, fPtMax, fT) != 0)
            return -12;

        // fit Y (rapidity) histograms
        if (FitYHistogram(particle_infos[pdg_key]->fYHist, pdg_key, fY0, fSigma) != 0)
            return -13;
        TVector3 Y0SigmaT(fY0, fSigma, fT);
        TString vector_name = TString::Format("Y0SigmaT_pdg%d", pdg_key);
        hist_file->WriteObject(&Y0SigmaT, vector_name);

        // fit Eta (pseudorapidity) histograms
        // if (FitEtaHistogram(particle_infos[pdg_key]->fEtaHist, pdg_key, eta_min, eta_max) != 0)
        //    return -5;

        LOG(info) << TString::Format("pdg=%i y0=%4.2f sigma_y=%4.2f T_pt=%6.4f", pdg_key, fY0, fSigma, fT);
    }

    // write the histograms to the output ROOT file
    for (auto pdg_key : fParticles) {
        particle_infos[pdg_key]->fPHist->Write();
        particle_infos[pdg_key]->fPtHist->Write();
        particle_infos[pdg_key]->fYHist->Write();
        particle_infos[pdg_key]->fEtaHist->Write();
        particle_infos[pdg_key]->fThetaHist->Write();
        particle_infos[pdg_key]->fPhiHist->Write();
        for (auto& p_hist : particle_infos[pdg_key]->fPHistVector)
            p_hist->Write();
        for (auto& eta_hist : particle_infos[pdg_key]->fEtaHistVector)
            eta_hist->Write();
        // hist_file->WriteObject(&particle_infos[pdg_key]->fPHistVector, "PHistVector");
        // hist_file->WriteObject(&particle_infos[pdg_key]->fEtaHistVector, "EtaHistVector");
    }

    hist_file->Close();
    return 0;
}

// Calculate parameters for all the particle types
// according to the given histogram stored in the 'input_histo_file' file
Bool_t BmnParticleEqualizer::ReadSampleHistograms(TString input_histo_file)
{
    // open file with histograms
    TFile* hist_file = TFile::Open(input_histo_file);
    // get histograms from the file and estimate parameters for embedding new particles of this type
    for (auto pdg_key : fParticles) {
        TString histogram_name = TString::Format("hP_pdg%d", pdg_key);
        fParticleInfos[pdg_key]->fPHist = unique_ptr<TH1D>((TH1D*)hist_file->Get(histogram_name));
        if (fParticleInfos[pdg_key]->fPHist.get() == nullptr) {
            LOG(error) << "Momentum histogram does not exist for pdg=" << pdg_key;
            return kFALSE;
        }
        if (fParticleInfos[pdg_key]->fPHist->GetEntries() == 0) {
            LOG(error) << "Momentum histogram is empty for pdg=" << pdg_key;
            return kFALSE;
        }
        histogram_name = TString::Format("hPt_pdg%d", pdg_key);
        fParticleInfos[pdg_key]->fPtHist = unique_ptr<TH1D>((TH1D*)hist_file->Get(histogram_name));
        if (fParticleInfos[pdg_key]->fPtHist.get() == nullptr) {
            LOG(error) << "Transverse momentum histogram does not exist for pdg=" << pdg_key;
            return kFALSE;
        }
        if (fParticleInfos[pdg_key]->fPtHist->GetEntries() == 0) {
            LOG(error) << "Transverse momentum histogram is empty for pdg=" << pdg_key;
            return kFALSE;
        }
        histogram_name = TString::Format("hY_pdg%d", pdg_key);
        fParticleInfos[pdg_key]->fYHist = unique_ptr<TH1D>((TH1D*)hist_file->Get(histogram_name));
        if (fParticleInfos[pdg_key]->fYHist.get() == nullptr) {
            LOG(error) << "Rapidity histogram does not exist for pdg=" << pdg_key;
            return kFALSE;
        }
        if (fParticleInfos[pdg_key]->fYHist->GetEntries() == 0) {
            LOG(error) << "Rapidity histogram is empty for pdg=" << pdg_key;
            return kFALSE;
        }
        histogram_name = TString::Format("hEta_pdg%d", pdg_key);
        fParticleInfos[pdg_key]->fEtaHist = unique_ptr<TH1D>((TH1D*)hist_file->Get(histogram_name));
        if (fParticleInfos[pdg_key]->fEtaHist.get() == nullptr) {
            LOG(error) << "Pseudorapidity histogram does not exist for pdg=" << pdg_key;
            return kFALSE;
        }
        if (fParticleInfos[pdg_key]->fEtaHist->GetEntries() == 0) {
            LOG(error) << "Pseudorapidity histogram is empty for pdg=" << pdg_key;
            return kFALSE;
        }
        histogram_name = TString::Format("hTheta_pdg%d", pdg_key);
        fParticleInfos[pdg_key]->fThetaHist = unique_ptr<TH1D>((TH1D*)hist_file->Get(histogram_name));
        if (fParticleInfos[pdg_key]->fThetaHist.get() == nullptr) {
            LOG(error) << "Theta angle histogram does not exist for pdg=" << pdg_key;
            return kFALSE;
        }
        if (fParticleInfos[pdg_key]->fThetaHist->GetEntries() == 0) {
            LOG(error) << "Theta angle histogram is empty for pdg=" << pdg_key;
            return kFALSE;
        }
        histogram_name = TString::Format("hPhi_pdg%d", pdg_key);
        fParticleInfos[pdg_key]->fPhiHist = unique_ptr<TH1D>((TH1D*)hist_file->Get(histogram_name));
        if (fParticleInfos[pdg_key]->fPhiHist.get() == nullptr) {
            LOG(error) << "Phi angle histogram does not exist for pdg=" << pdg_key;
            return kFALSE;
        }
        if (fParticleInfos[pdg_key]->fPhiHist->GetEntries() == 0) {
            LOG(error) << "Phi angle histogram is empty for pdg=" << pdg_key;
            return kFALSE;
        }
        int hist_counter = 0;
        for (auto& p_hist : fParticleInfos[pdg_key]->fPHistVector) {
            histogram_name = TString::Format("hP%d_pdg%d", ++hist_counter, pdg_key);
            p_hist = unique_ptr<TH1D>((TH1D*)hist_file->Get(histogram_name));
            if (p_hist.get() == nullptr) {
                LOG(error) << "Momentum histogram does not exist for pdg=" << pdg_key
                           << " in interval=" << hist_counter;
                return kFALSE;
            }
        }
        hist_counter = 0;
        for (auto& eta_hist : fParticleInfos[pdg_key]->fEtaHistVector) {
            histogram_name = TString::Format("hEta%d_pdg%d", ++hist_counter, pdg_key);
            eta_hist = unique_ptr<TH1D>((TH1D*)hist_file->Get(histogram_name));
            if (eta_hist.get() == nullptr) {
                LOG(error) << "Pseudorapidity histogram does not exist for pdg=" << pdg_key
                           << " in interval=" << hist_counter;
                return kFALSE;
            }
        }

        TDatabasePDG* pdgBase = TDatabasePDG::Instance();
        TParticlePDG* particle = pdgBase->GetParticle(pdg_key);
        if (!particle) {
            Fatal("PrepareDistribution", "PDG code %d is not defined. exiting...", pdg_key);
            return kFALSE;
        }
        Double_t fPDGMass = particle->Mass();

        // check that there is Pt histogram for the particle type in the file
        if ((!fParticleInfos[pdg_key]->fPtHist) || (fParticleInfos[pdg_key]->fPtHist->GetEntries() == 0)) {
            LOG(warning) << "BmnParticleEqualizer: Pt histogram is absent or empty for PDG = " << pdg_key;
            continue;
        }
        // fit Pt distribution for the particle type by expo
        FitPtHistogram(fParticleInfos[pdg_key]->fPtHist, pdg_key, fPDGMass, fPMin, fPMax, fParticleInfos[pdg_key]->fT);

        Info("PrepareDistribution", "pdg=%i y0=%4.2f sigma_y=%4.2f T_pt=%6.4f", pdg_key, fParticleInfos[pdg_key]->fY0,
             fParticleInfos[pdg_key]->fSigma, fParticleInfos[pdg_key]->fT);

        // check that there is Y histogram for the particle type in the file
        if ((!fParticleInfos[pdg_key]->fYHist) || (fParticleInfos[pdg_key]->fYHist->GetEntries() == 0)) {
            Warning("PrepareDistribution", "Y histogram is absent or empty for %d PDG code", pdg_key);
            continue;
        }
        FitYHistogram(fParticleInfos[pdg_key]->fYHist, pdg_key, fParticleInfos[pdg_key]->fY0,
                      fParticleInfos[pdg_key]->fSigma);
    }

    return kTRUE;
}

// Creates an event with equal number of particles at all momentum intervals
Bool_t BmnParticleEqualizer::ReadEvent(FairPrimaryGenerator* primGen)
{
    if (!isActive)
        return kFALSE;

    /** CLEARING **/
    // reset particle count for all the particle types
    for (auto part_iter = fParticleInfos.begin(); part_iter != fParticleInfos.end(); part_iter++)
        fill(part_iter->second->fParticleNumber.begin(), part_iter->second->fParticleNumber.end(), 0);
    // clear maximum number of a particle type among all particles (divided by momentum intervals)
    fill(fMaxParticles.begin(), fMaxParticles.end(), 0);

    /** FORM EVENT DISTRIBUTION FOR GENERATION PARTICLES TO EQUALIZE THEIR NUMBERS **/
    // form particle distributions for the current event (obtained by the previous generators)
    TClonesArray* arrParticles = ((CbmStack*)gMC->GetStack())->GetListOfParticles();
    for (int i = 0; i < arrParticles->GetEntries(); i++) {
        // get particle information
        TParticle* part = (TParticle*)arrParticles->At(i);
        Int_t part_code = part->GetPdgCode();
        Double_t part_p = part->P();
        Double_t part_eta = part->Eta();

        // exclude secondary particles
        if (!part->IsPrimary())
            continue;

        // check particle momentum is inside the momentum range
        int interval_number = (part_p - fPMin) / fIntervalStep;
        if ((interval_number < 0) || (interval_number >= fIntervalCount))
            continue;

        // add found particle according to the momentum to the "PDG -> Particle Count" map (increase by 1)
        LOG(debug) << TString::Format("Found particle: PDG=%i P=%4.2f Eta=%4.2f", part_code, part_p, part_eta);
        if (auto part_iter = fParticleInfos.find(part_code); part_iter != fParticleInfos.end()) {
            // filter particles according to the momentum and pseudorapidity in case of empty statistics for the values
            if (iParticleCut > 0) {
                if (fParticleInfos[part_code]->fPHistVector[interval_number]->GetBinContent(
                        fParticleInfos[part_code]->fPHistVector[interval_number]->FindBin(part_p))
                    < 1)
                    continue;
                if (fParticleInfos[part_code]->fEtaHistVector[interval_number]->GetBinContent(
                        fParticleInfos[part_code]->fEtaHistVector[interval_number]->FindBin(part_eta))
                    < 1)
                    continue;
                // if (fParticleInfos[part_code]->fPtHist->GetBinContent(
                //     fParticleInfos[part_code]->fPtHist->FindBin(part->Pt())) < 1)
                //     continue;
                // if (fParticleInfos[part_code]->fYHist->GetBinContent(
                //     fParticleInfos[part_code]->fYHist->FindBin(part->Y())) < 1)
                //     continue;
                // if (fParticleInfos[part_code]->fThetaHist->GetBinContent(
                //     fParticleInfos[part_code]->fThetaHist->FindBin(part->Theta())) < 1)
                //     continue;
            }

            part_iter->second->fParticleNumber[interval_number] += 1;
            // update maximum number among all the particle if greater (for the current momentum interval)
            if (part_iter->second->fParticleNumber[interval_number] > fMaxParticles[interval_number])
                fMaxParticles[interval_number] = part_iter->second->fParticleNumber[interval_number];
        }
    }   // for (int i = 0; i < arrParticles->GetEntries(); i++)

    // generate new particles to equalize their numbers for different types
    for (auto pdg_key : fParticles) {
        LOG(debug) << TString::Format("Current particle type to be equalized: pdg=%i y0=%4.2f sigma_y=%4.2f T_pt=%6.4f",
                                      pdg_key, fParticleInfos[pdg_key]->fY0, fParticleInfos[pdg_key]->fSigma,
                                      fParticleInfos[pdg_key]->fT);
        Double_t cur_p = fPMin;
        // add a number of particles for all momentum intervals to equalize their count
        LOG(debug) << "Adding paticle to equalize the numbers...";
        for (int cur_interval = 0; cur_interval < fIntervalCount; cur_interval++, cur_p += fIntervalStep) {
            int add_particle = fMaxParticles[cur_interval] - fParticleInfos[pdg_key]->fParticleNumber[cur_interval];
            LOG(debug) << endl
                       << "   " << add_particle << " particle with PDG = " << pdg_key << " (" << cur_p << " GeV)";
            // cout << "Adding " << add_particle << " particles with PDG = " << pdg_key
            //      << " (" << cur_p << " GeV)" << endl;
            for (Int_t k = 0; k < add_particle; k++) {
                Double_t phi = gRandom->Uniform(0, TMath::TwoPi());
                Double_t p = 0;
                if (fParticleInfos[pdg_key]->fPHistVector[cur_interval]->GetEntries() > 2)
                    p = fParticleInfos[pdg_key]->fPHistVector[cur_interval]->GetRandom();
                else {
                    TString dist_func_name = TString::Format("distP_pdg%d", pdg_key);
                    p = fParticleInfos[pdg_key]
                            ->fPHist->GetFunction(dist_func_name)
                            ->GetRandom(cur_p, cur_p + fIntervalStep);
                }
                // Double_t pt = fParticleInfos[pdg_key]->fPtDistribution->GetRandom(cur_pt, cur_pt + fIntervalStep);
                // Double_t y = gRandom->Gaus(particle_info->fY0, particle_info->fSigma);
                // Double_t mt = TMath::Sqrt(fPDGMass * fPDGMass + pt * pt);
                // Double_t pz = mt * TMath::SinH(y);
                Double_t eta = 0;
                if (fParticleInfos[pdg_key]->fEtaHistVector[cur_interval]->GetEntries() > 2)
                    eta = fParticleInfos[pdg_key]->fEtaHistVector[cur_interval]->GetRandom();
                else {
                    // TString dist_func_name = TString::Format("distEta_pdg%d", pdg_key);
                    eta = fParticleInfos[pdg_key]->fEtaHist->GetRandom();
                }
                Double_t pt = p / TMath::CosH(eta);
                Double_t px = pt * TMath::Cos(phi);
                Double_t py = pt * TMath::Sin(phi);
                Double_t pz = pt * TMath::SinH(eta);

                LOG(debug) << "Particle generated: pdg=" << pdg_key << " p=" << p << " eta=" << eta;
                FairMCEventHeader* pCurrent = primGen->GetEvent();
                // AddTrack parameters: pdgid, px, py, pz, vx, vy, vz
                primGen->AddTrack(pdg_key, px, py, pz, pCurrent->GetX(), pCurrent->GetY(), pCurrent->GetZ());
            }
        }
    }

    return kTRUE;
}

// static function to fit the obtained sample histograms (to show whether the results are appropriate)
void BmnParticleEqualizer::FitSampleHistograms(TString input_histo_file)
{
    TFile* f = TFile::Open(input_histo_file);
    for (int particle_number : fParticles) {
        /*TCanvas* c = */ new TCanvas(TString::Format("c_%d", particle_number),
                                      TString::Format("Distributions for particle PDG = %d", particle_number), 1000,
                                      500);
        TH1D* h1 = (TH1D*)f->Get(TString::Format("hP_pdg%d", particle_number));

        Double_t x_max_value = h1->GetXaxis()->GetBinCenter(h1->GetMaximumBin());
        Double_t y_max_value = h1->GetBinContent(h1->GetMaximumBin());
        Double_t p_avg = (fPMin + fPMax) / 2;
        cout << "avg_value = " << p_avg << endl;
        cout << "x_max_value = " << x_max_value << endl;
        cout << "y_max_value = " << y_max_value << endl;
        TF1* p_distribution = new TF1("distFit", "gaus(0)+gaus(3)+landau(6)", fPMin, fPMax);
        p_distribution->SetParLimits(1, fPMin, p_avg);
        p_distribution->SetParameter(1, fPMin);
        p_distribution->SetParLimits(2, 0.1, p_avg);
        p_distribution->SetParameter(2, p_avg / 4);
        p_distribution->SetParLimits(4, x_max_value - 0.1, fPMax);
        p_distribution->SetParameter(4, x_max_value - 0.1);
        p_distribution->SetParLimits(5, 0.1, p_avg);
        p_distribution->SetParameter(5, p_avg / 2);
        p_distribution->SetParLimits(7, fPMin, TMath::Min(x_max_value, p_avg));
        p_distribution->SetParameter(7, fPMin);
        p_distribution->SetParLimits(8, 0.1, p_avg);
        p_distribution->SetParameter(8, p_avg / 4);
        h1->Fit(p_distribution);
        h1->Draw();
    }
}

// static function to show the obtained sample histograms with obtained fit (to check whether the results are
// appropriate)
int BmnParticleEqualizer::ShowSampleHistograms(TString input_histo_file)
{
    TFile* f = TFile::Open(input_histo_file);
    if ((!f) || (!f->IsOpen())) {
        cout << "ERROR: unable to open ROOT file: " << input_histo_file << endl;
        return -1;
    }

    for (int particle_number : fParticles) {
        /*TCanvas* c = */ new TCanvas(TString::Format("c_%d", particle_number),
                                      TString::Format("Distributions for particle PDG = %d", particle_number), 1000,
                                      500);
        TH1D* h1 = (TH1D*)f->Get(TString::Format("hP_pdg%d", particle_number));
        TF1* f1 = h1->GetFunction(TString::Format("distP_pdg%d", particle_number));
        f1->Draw();
        h1->Draw("SAME");
    }

    return 0;
}

// define PDG code of a particle by the given mass
// among particle types set by 'vecParticlePDG' vector or in the TDatabasePDG if the vector is empty
// tolerance: relative if tolerance > 0, absolute (in GeV) in case of tolerance <= 0
// return PDG code or 0 if no corresponding particle was found
Int_t BmnParticleEqualizer::GetPDGCodeByMass(Double_t mass, Double_t tolerance, const vector<int>& vecParticlePDG)
{
    TDatabasePDG* pdg_base = TDatabasePDG::Instance();
    if (!pdg_base) {
        cout << "ERROR: Could not get TDatabasePDG instance" << endl;
        return 0;
    }

    if (mass < 0)
        mass *= -1;

    if (vecParticlePDG.empty()) {
        TIter next(pdg_base->ParticleList());
        while (TParticlePDG* particle = (TParticlePDG*)next()) {
            if (TMath::Abs(particle->Mass() - mass) < (tolerance > 0 ? tolerance * mass : -1 * tolerance))
                return particle->PdgCode();
        }
    } else {
        for (int particle_number : vecParticlePDG) {
            TParticlePDG* particle = pdg_base->GetParticle(particle_number);
            if (TMath::Abs(particle->Mass() - mass) < (tolerance > 0 ? tolerance * mass : -1 * tolerance))
                return particle->PdgCode();
        }
    }

    return 0;
}

// draw histogram with multiplicity of different particle types for all the momentum intervals
// as well as P, Pt, Y, Eta, Theta, Phi distributions after the equalizing
// it work both for SIM and DST files listed in the given text file ('input_list_file' var)
int BmnParticleEqualizer::ShowResultDistributions(TString input_list_file)
{
    gSystem->ExpandPathName(input_list_file);

    // get file list from the input text file
    list<string> file_list;
    ifstream list_file(input_list_file.Data());
    string s;
    while (getline(list_file, s))
        file_list.push_back(s);
    if (file_list.empty()) {
        cout << "WARNING: There are no files in the list of the input text file: " << input_list_file << endl;
        return 0;
    }

    // form ROOT chain for the file list
    TChain dataChain("bmndata");
    for (auto const& cur_file_path : file_list) {
        TString strFilePath(cur_file_path);
        gSystem->ExpandPathName(strFilePath);

        dataChain.Add(strFilePath);
    }

    // flag whether the input files have SIM or DST format
    bool isDST = false;
    TClonesArray *mcTracks = nullptr, *globalTracks = nullptr;
    BmnMCInfoDst* mcInfo = nullptr;
    if (dataChain.GetBranch("MCTrack") == nullptr) {
        if (dataChain.GetBranch("BmnMCInfo.") == nullptr) {
            LOG(error) << "'MCTrack' and 'BmnMCInfo.' branches were not found, please, check the structure of the "
                          "files, exiting...";
            return -1;
        }
        dataChain.SetBranchAddress("BmnMCInfo.", &mcInfo);
        if (mcInfo == nullptr) {
            LOG(error)
                << "'BmnMCInfo.' branch was not read correctly, please, check the structure of the files, exiting...";
            return -2;
        }
        dataChain.SetBranchAddress("BmnGlobalTrack", &globalTracks);
        if (globalTracks == nullptr) {
            LOG(error) << "'BmnGlobalTrack' branch was not read correctly, please, check the structure of the files, "
                          "exiting...";
            return -4;
        }
        isDST = true;
        // dataChain.SetBranchStatus("*", false);
        // dataChain.SetBranchStatus("BmnMCInfo*", true);
        // dataChain.SetBranchStatus("BmnGlobalTrack", true);
    } else {
        dataChain.SetBranchAddress("MCTrack", &mcTracks);
        if (mcTracks == nullptr) {
            LOG(error)
                << "MCTrack' branch was not read correctly, please, check the structure of the files, exiting...";
            return -3;
        }
        // dataChain.SetBranchStatus("*", false);
        // dataChain.SetBranchStatus("MCTrack*", true);
    }

    // create histogram with bars
    TCanvas* c_number =
        new TCanvas("c_number", "Multiplicity of different particle types for all the momentum intervals", 1000, 500);
    gPad->SetGrid();

    int bin_count = fIntervalCount, cur_hist = 0;
    // create histograms for particle multiplicities
    map<int, TH1I*> mapParticleNumberHists;
    for (int particle_number : fParticles) {
        TH1I* hParticleNumber =
            new TH1I(TString::Format("hParticleNumber_%d", particle_number),
                     TString::Format("hParticleNumber_%d", particle_number), bin_count, fPMin, fPMax);
        // gStyle->SetTitleFontSize(0.03);    // only one approach to change histogram title size
        hParticleNumber->SetStats(kFALSE);
        hParticleNumber->LabelsDeflate();
        hParticleNumber->SetLabelFont(62);
        hParticleNumber->SetMarkerSize(0.6);
        // hParticleNumber->LabelsOption("v");
        hParticleNumber->SetTitle("");
        hParticleNumber->GetXaxis()->SetTitle("momentum interval");
        hParticleNumber->GetXaxis()->SetTitleSize(0.03);
        hParticleNumber->GetXaxis()->CenterTitle();
        hParticleNumber->GetXaxis()->SetLabelSize(0.02 - 0.00017 * bin_count);
        // hParticleNumber->GetXaxis()->CenterLabels();
        hParticleNumber->GetXaxis()->SetNdivisions(bin_count);
        hParticleNumber->GetYaxis()->SetTitle("particle multiplicity");
        // hParticleNumber->GetYaxis()->SetTitleSize(0.03);
        // hParticleNumber->GetYaxis()->SetNdivisions(1007);
        hParticleNumber->GetYaxis()->SetLabelSize(0.02);
        hParticleNumber->SetBarWidth(0.18);
        hParticleNumber->SetBarOffset(0.05 + cur_hist * 0.18);
        switch (cur_hist) {
            case 0:
                hParticleNumber->SetFillColor(49);
                break;
            case 1:
                hParticleNumber->SetFillColor(50);
                break;
            case 2:
                hParticleNumber->SetFillColor(42);
                break;
            case 3:
                hParticleNumber->SetFillColor(29);
                break;
            case 4:
                hParticleNumber->SetFillColor(36);
                break;
            default:
                break;
        }

        mapParticleNumberHists[particle_number] = hParticleNumber;
        cur_hist++;
    }

    // create histograms for P, Pt, Y, Eta, Theta, Phi
    map<int, TH1D*> mapPHists, mapPtHists, mapYHists, mapEtaHists, mapThetaHists, mapPhiHists;
    for (int particle_number : fParticles) {
        TH1D* hP = new TH1D(TString::Format("hP_%d", particle_number), TString::Format("hP_%d", particle_number), 80,
                            fPMin, fPMax);
        mapPHists[particle_number] = hP;
        TH1D* hPt = new TH1D(TString::Format("hPt_%d", particle_number), TString::Format("hPt_%d", particle_number), 60,
                             fPtMin, fPtMax);
        mapPtHists[particle_number] = hPt;
        TH1D* hY = new TH1D(TString::Format("hY_%d", particle_number), TString::Format("hY_%d", particle_number), 60,
                            fYMin, fYMax);
        mapYHists[particle_number] = hY;
        TH1D* hEta = new TH1D(TString::Format("hEta_%d", particle_number), TString::Format("hEta_%d", particle_number),
                              60, fEtaMin, fEtaMax);
        mapEtaHists[particle_number] = hEta;
        TH1D* hTheta = new TH1D(TString::Format("hTheta_%d", particle_number),
                                TString::Format("hTheta_%d", particle_number), 61, fThetaMin, fThetaMax);
        mapThetaHists[particle_number] = hTheta;
        TH1D* hPhi = new TH1D(TString::Format("hPhi_%d", particle_number), TString::Format("hPhi_%d", particle_number),
                              64, fPhiMin, fPhiMax);
        mapPhiHists[particle_number] = hPhi;
    }

    int iEventNumber = dataChain.GetEntries(), iCurrentEvent = 0;
    if (iEventNumber < 1) {
        cout << "WARNING: There are no correct entries in the file list stored at: \"" << input_list_file << "\""
             << endl;
        return 0;
    }

    // fill all the histograms
    for (iCurrentEvent = 0; iCurrentEvent < iEventNumber; iCurrentEvent++) {
        // Next Event
        if ((iCurrentEvent + 1) % 1000 == 0)
            LOG(info) << "Processing event #" << iCurrentEvent + 1 << " of " << iEventNumber;
        dataChain.GetEntry(iCurrentEvent);

        Int_t track_count = 0;
        TClonesArray* mc_tracks = nullptr;
        if (isDST) {
            mc_tracks = mcInfo->GetMCTracks();
            track_count = globalTracks->GetEntries();
        } else {
            mc_tracks = mcTracks;
            track_count = mc_tracks->GetEntries();
        }

        // Getting tracks of the event and their corresponding components
        for (Int_t iTrack = 0; iTrack < track_count; iTrack++) {
            Int_t track_pdg = -1, track_mass = -1;
            Double_t track_p = 0, track_pt = 0, track_y = 0, track_eta = 0, track_theta = 0, track_phi = 0;
            CbmMCTrack* mc_track;
            BmnGlobalTrack* global_track;

            if (isDST) {
                // cout<<"iTrack = "<<iTrack<<", track_count = "<<track_count<<endl;
                global_track = (BmnGlobalTrack*)globalTracks->UncheckedAt(iTrack);
                if (!global_track->IsPrimary())
                    continue;

                // get corresponding MC track to define PDG
                Int_t mc_index = global_track->GetRefIndex();
                if (mc_index > -1) {
                    mc_track = (CbmMCTrack*)mc_tracks->At(mc_index);
                    track_pdg = mc_track->GetPdgCode();
                    track_mass = mc_track->GetMass();
                } else {
                    track_pdg = global_track->GetPDG();
                    if (track_pdg != 0) {
                        TDatabasePDG* pdg_db = TDatabasePDG::Instance();
                        TParticlePDG* part_pdg = pdg_db->GetParticle(track_pdg);
                        if (part_pdg)
                            track_mass = part_pdg->Mass();
                        else {
                            cout << "WARNING: PDG code was not found in the PDG database: " << track_pdg << endl;
                            continue;
                        }
                    } else {
                        Double_t mass_tof = global_track->GetMass2(1);
                        if (mass_tof == -1000.0) {
                            mass_tof = global_track->GetMass2(2);
                            if (mass_tof == -1000.0) {
                                // cout << "WARNING: the particle was not identified by the TOF detectors " << endl;
                                continue;
                            }
                        }
                        // cout<<"Get PDG for mass: "<<mass_tof<<" with tolerance 50 MeV"<<endl;
                        track_pdg = GetPDGCodeByMass(mass_tof, -0.03, fParticles);
                        if (track_pdg == 0) {
                            // cout << "WARNING: the particle PDG code was not defined for its mass = " <<mass_tof<<
                            // endl;
                            continue;
                        }
                    }   // if (track_pdg == 0)
                }   // if (mc_index == -1)

                // Getting track params predicted by Kalman filter ...
                FairTrackParam* parFirst = global_track->GetParamFirst();
                // FairTrackParam* parLast = track->GetParamLast();
                Double_t track_qp = parFirst->GetQp();
                track_p = (TMath::Abs(track_qp) > 1.e-4) ? 1. / TMath::Abs(track_qp) : 1.e4;
                TVector3 vecMom;
                parFirst->Momentum(vecMom);
                track_pt = TMath::Sqrt(vecMom.x() * vecMom.x() + vecMom.y() * vecMom.y());
                Double_t e = TMath::Sqrt(track_p * track_p + track_mass * track_mass);
                track_y = 0.5 * TMath::Log((e + vecMom.z()) / (e - vecMom.z()));
                track_eta = 0.5 * TMath::Log((track_p + vecMom.z()) / (track_p - vecMom.z()));
                track_phi = TMath::ATan2(parFirst->GetTy(), parFirst->GetTx());         // phi = arctan(tgy/tgx)
                track_theta = TMath::ATan2(parFirst->GetTx(), TMath::Cos(track_phi));   // theta = arctan(tgx/cos(phi))
            }   // if (isDST)
            else
            {
                mc_track = (CbmMCTrack*)mc_tracks->At(iTrack);
                if (mc_track->GetMotherId() >= 0)
                    continue;

                // filter particles
                if (iParticleCut > 0) {
                    // exclude tracks without points in Silicon and GEMs
                    if ((mc_track->GetNPoints(kGEM) < 1) || (mc_track->GetNPoints(kSILICON) < 1))
                        continue;
                    if (iParticleCut > 1) {
                        // exclude tracks without points in ToF detectors
                        if ((mc_track->GetNPoints(kTOF) < 1) && (mc_track->GetNPoints(kTOF1) < 1)
                            && (mc_track->GetNPoints(kTOF701) < 1))
                            continue;
                    }
                }

                track_pdg = mc_track->GetPdgCode();
                track_p = mc_track->GetP();
                track_pt = mc_track->GetPt();
                track_y = mc_track->GetRapidity();
                track_eta = 0.5 * TMath::Log((track_p + mc_track->GetPz()) / (track_p - mc_track->GetPz()));
                track_theta = TMath::ATan2(track_pt, mc_track->GetPz());
                track_phi = TMath::ATan2(mc_track->GetPy(), mc_track->GetPx());
            }   // if not (isDST)

            auto iter_pdg = find(fParticles.begin(), fParticles.end(), track_pdg);
            if (iter_pdg == fParticles.end())
                continue;

            if ((track_p >= fPMin) && (track_p < fPMax)) {
                mapParticleNumberHists[*iter_pdg]->Fill(track_p);
                mapPHists[*iter_pdg]->Fill(track_p);
                mapPtHists[*iter_pdg]->Fill(track_pt);
                mapYHists[*iter_pdg]->Fill(track_y);
                mapEtaHists[*iter_pdg]->Fill(track_eta);
                mapThetaHists[*iter_pdg]->Fill(track_theta);
                mapPhiHists[*iter_pdg]->Fill(track_phi);
            }
        }   // for (Int_t iTrack = 0; iTrack < track_count; iTrack++)
    }   // for (Int_t iEvent = 0; iEvent < iEventNumber; iEvent++) {

    // if no required branches in the tree or events then clear memory and exit
    if (iCurrentEvent == 0) {
        cout << "ERROR: There are no necessary tree branches or DST events in the files from the list: "
             << input_list_file << endl;
        for (const auto& pair : mapPHists)
            delete pair.second;
        for (const auto& pair : mapPtHists)
            delete pair.second;
        for (const auto& pair : mapYHists)
            delete pair.second;
        for (const auto& pair : mapEtaHists)
            delete pair.second;
        for (const auto& pair : mapThetaHists)
            delete pair.second;
        for (const auto& pair : mapPhiHists)
            delete pair.second;
        delete c_number;
        return 0;
    }

    // find max Y-axis value
    Double_t max_y_value = 0;
    for (const auto& pair_hist : mapParticleNumberHists) {
        if (max_y_value < pair_hist.second->GetMaximum())
            max_y_value = pair_hist.second->GetMaximum();
    }

    // draw bar histogram with particle multiplicity for all the particle types
    TLegend* legend = new TLegend(0.7, 0.74, 0.85, 0.88);
    TString hist_options = "bar2,hist,text";   // 'text' option for vertical captions with event count
    cur_hist = 0;
    for (const auto& pair_hist : mapParticleNumberHists) {
        if (cur_hist == 0) {
            if (pair_hist.second->GetMaximum() < max_y_value)
                pair_hist.second->SetMaximum(max_y_value + 10);
            pair_hist.second->DrawCopy(hist_options);
        } else {
            pair_hist.second->DrawCopy(hist_options + ",same");
        }

        legend->AddEntry(pair_hist.second, TString::Format("Multiplitcity for %d", pair_hist.first), "f");
        legend->Draw();
        cur_hist++;
    }

    // draw histograms (P, Pt, Y, Eta, Theta, Phi) for different particle types
    TCanvas* c_p = new TCanvas("c_p", "Particle Momentum", 1000, 500);
    c_p->Divide((fParticles.size() / 3) + 1, 3);
    cur_hist = 1;
    for (int particle_number : fParticles) {
        c_p->cd(cur_hist);
        mapPHists[particle_number]->Draw();
        cur_hist++;
    }
    TCanvas* c_pt = new TCanvas("c_pt", "Transverse Momentum", 1000, 500);
    c_pt->Divide((fParticles.size() / 3) + 1, 3);
    cur_hist = 1;
    for (int particle_number : fParticles) {
        c_pt->cd(cur_hist);
        mapPtHists[particle_number]->Draw();
        cur_hist++;
    }
    TCanvas* c_y = new TCanvas("c_y", "Rapidity", 1000, 500);
    c_y->Divide((fParticles.size() / 3) + 1, 3);
    cur_hist = 1;
    for (int particle_number : fParticles) {
        c_y->cd(cur_hist);
        mapYHists[particle_number]->Draw();
        cur_hist++;
    }
    TCanvas* c_eta = new TCanvas("c_eta", "Pseudorapidity", 1000, 500);
    c_eta->Divide((fParticles.size() / 3) + 1, 3);
    cur_hist = 1;
    for (int particle_number : fParticles) {
        c_eta->cd(cur_hist);
        mapEtaHists[particle_number]->Draw();
        cur_hist++;
    }
    TCanvas* c_theta = new TCanvas("c_theta", "Theta angle", 1000, 500);
    c_theta->Divide((fParticles.size() / 3) + 1, 3);
    cur_hist = 1;
    for (int particle_number : fParticles) {
        c_theta->cd(cur_hist);
        mapThetaHists[particle_number]->Draw();
        cur_hist++;
    }
    TCanvas* c_phi = new TCanvas("c_phi", "Phi Angle", 1000, 500);
    c_phi->Divide((fParticles.size() / 3) + 1, 3);
    cur_hist = 1;
    for (int particle_number : fParticles) {
        c_phi->cd(cur_hist);
        mapPhiHists[particle_number]->Draw();
        cur_hist++;
    }

    return 0;
}
