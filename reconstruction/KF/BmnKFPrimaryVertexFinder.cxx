/** Implementation of the BmnKFPrimaryVertexFinder class
 *
 * @author  A.Zinchenko
 * @version 1.0
 * @since   12.10.2021
 *
 */
#include "BmnKFPrimaryVertexFinder.h"

#include "CbmKF.h"
#include "CbmKFMath.h"
#include "CbmKFTrack.h"
#include "CbmKFVertex.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "FairRootManager.h"

#include <TCanvas.h>
#include <TF1.h>
#include <TFitResult.h>   // AZ-310525
#include <TH2.h>          // AZ-300525
#include <TRandom.h>      // AZ-200226
#include <vector>
using std::vector;

//__________________________________________________________________________

InitStatus BmnKFPrimaryVertexFinder::Init()
{

    fInputTracks = (TClonesArray*)FairRootManager::Instance()->GetObject("StsVector");
    if (fInputTracks == 0x0)
        fInputTracks = (TClonesArray*)FairRootManager::Instance()->GetObject("StsTrack");

    // Create and register CbmVertex object
    fPrimVert = new CbmVertex("PrimaryVertex", "Global");
    FairRootManager::Instance()->Register("MpdVertex.", "Global", fPrimVert, kTRUE);
    fHist[0] = new TH1D("hX", "Vertex X", 100, -5, 5);
    fHist[1] = new TH1D("hY", "Vertex Y", 100, -5, 5);
    fHistw[0] = new TH1D("hXw", "Vertex X", 100, -5, 5);
    fHistw[1] = new TH1D("hYw", "Vertex Y", 100, -5, 5);
    fHistPca = new TH1D("hPca", "PCA", 200, -10, 10);
    fHistErr[0] = new TH1D("hErrX", "X-error", 100, 0, 1.0);
    fHistErr[1] = new TH1D("hErrY", "Y-error", 100, 0, 1.0);
    fHistR = new TH1D("hRad", "Radius", 50, 0, 10);

    // AZ-300525
    Double_t zmin = -141, zmax = 39;
    int nz = 30;
    fHist3 = new TH3D("h3", "XYZ", 50, -10, 10, 50, -10, 10, nz, zmin, zmax);
    fHistw3 = new TH3D("hw3", "XYZ", 50, -10, 10, 50, -10, 10, nz, zmin, zmax);
    fHistZ = new TH1D("hZ", "Z-profile", nz, zmin, zmax);
    fHistZ->SetStats(kFALSE);
    fHZR = new TH2D("hZR", "Z-profile", nz, zmin, zmax, 50, 0.0, 10.0);
    return kSUCCESS;
}

//__________________________________________________________________________

void BmnKFPrimaryVertexFinder::Exec(Option_t* option)
{

    // static int eventCounter = 0;
    // cout << " VertexFinder event " << ++eventCounter << endl;

    Clear();

    if (fInputTracks == nullptr)
        return;

    const Double_t c2cut = 3.5 * 3.5;
    Int_t nTracks = fInputTracks->GetEntriesFast();

    CbmKFTrack* cloneArray = new CbmKFTrack[nTracks];

    // <AZ-270425 - sort tracks according to quality
    std::multimap<float, int> mQuInd;
    for (int i = 0; i < nTracks; ++i) {
        CbmStsTrack* st = (CbmStsTrack*)fInputTracks->UncheckedAt(i);
        // if (st->GetB() < -0.1) continue; // AZ-300525 - TMVA cut
        Int_t nHits = st->GetNStsHits();
        float qual = nHits;
        qual += (10 - TMath::Min(10.0, st->GetChi2() / st->GetNDF())) / 11;
        mQuInd.insert(pair<float, int>(qual, i));
    }
    int iii = 0;
    // >AZ

    // AZ-270425 for (int i = 0; i < nTracks; ++i) {
    for (auto mit = mQuInd.begin(); mit != mQuInd.end(); ++mit) {
        int i = mit->second;
        CbmStsTrack* st = (CbmStsTrack*)fInputTracks->UncheckedAt(i);
        Int_t nHits = st->GetNStsHits();
        // if (nHits < 5) continue; // AZ-010625
        // if (st->GetB() < -0.1) continue; // AZ-220525 - using TMVA for fake rejection
        if (nHits < 4)
            continue;
        if (st->GetFlag())
            continue;
        if (st->GetChi2() < 0. || st->GetChi2() > c2cut * st->GetNDF())
            continue;
        // if (1/TMath::Abs(st->GetParamFirst()->GetQp()) < 0.2) continue; //AZ-151023 - exclude low momenta
        // AZ-270425 CbmKFTrack& T = cloneArray[i];
        CbmKFTrack& T = cloneArray[iii++];   // AZ-270425
        T.SetStsTrack(*st);
        T.Extrapolate(12.0);   // AZ-161023 - go closer to the target
        if (!finite(T.GetTrack()[0]) || !finite(T.GetCovMatrix()[0]))
            continue;
        T.SetTrkID(i);   // AZ-090824
        AddTrack(&T);
    }

    EvalVertex();
    if (GetBreakout())
        return;   // AZ-010625 - for plots
    CbmKFVertex v;
    Fit(v);
    v.GetVertex(*fPrimVert);
    // Chi2Vertex();
    fPrimVert->trkID = fTrkID;   // AZ-090824

    // FillVertex(); // fill vertex info

    // AZ-300825 Flag tracks in vertex
    int inVtx = fTrkID.size();

    for (int j = 0; j < inVtx; ++j) {
        int indx = fTrkID[j];
        CbmStsTrack* st = (CbmStsTrack*)fInputTracks->UncheckedAt(indx);
        st->SetFlag(st->GetFlag() | 2);   // flag track in vertex
    }

    delete[] cloneArray;
}

//__________________________________________________________________________

void BmnKFPrimaryVertexFinder::Clear()
{
    fTracks.clear();
    for (int j = 0; j < 2; ++j) {
        fHist[j]->Reset();
        fHistw[j]->Reset();
        fHistErr[j]->Reset();
    }
    fTrkID.clear();   // AZ-090824

    // AZ-300525
    fHist3->Reset();
    fHistw3->Reset();
    fHistZ->Reset();
    fHZR->Reset();
    fHistR->Reset();
}

//__________________________________________________________________________

void BmnKFPrimaryVertexFinder::AddTrack(CbmKFTrackInterface* Track)
{
    fTracks.push_back(Track);
}

//__________________________________________________________________________

void BmnKFPrimaryVertexFinder::SetTracks(vector<CbmKFTrackInterface*>& vTr)
{
    fTracks = vTr;
}

//__________________________________________________________________________

void BmnKFPrimaryVertexFinder::EvalVertex()
{
    // Evaluate X and Y coordinates of vertex at Z = Ztarg

    int nTracks = fTracks.size();
    if (nTracks == 0)
        return;
    Double_t ztarg = 0.0, sigmas[2] = {0};

    if (!CbmKF::Instance()->vTargets.empty()) {
        CbmKFTube& t = CbmKF::Instance()->vTargets[0];
        ztarg = t.z;
    }
    for (int i = 0; i < 2; ++i) {
        fHist[i]->SetAxisRange(10, 0);    // reset axes range
        fHistw[i]->SetAxisRange(10, 0);   // reset axes range
    }

    //---> AZ-300525
    int nz = fHist3->GetNbinsZ();

    for (int itr = 0; itr < nTracks; ++itr) {
        CbmKFTrack tr(*fTracks[itr]);
        CbmStsTrack* ststr = (CbmStsTrack*)fInputTracks->UncheckedAt(fTracks[itr]->GetTrkID());
        Double_t www = ststr->GetNStsHits();

        for (int iz = nz; iz >= 1; --iz) {
            Double_t z = fHist3->GetZaxis()->GetBinCenter(iz);
            Bool_t err = tr.Extrapolate(z);
            if (err)
                continue;
            Double_t xtr = tr.GetTrack()[0];
            Double_t ytr = tr.GetTrack()[1];
            fHist3->Fill(xtr, ytr, z, 1);
            fHistw3->Fill(xtr, ytr, z, www);
            if (TMath::Abs(xtr) < 2.5 && TMath::Abs(ytr) < 2.5)
                fHistZ->Fill(z);
            fHZR->Fill(z, TMath::Sqrt(xtr * xtr + ytr * ytr), 1);
        }
    }
    TH2D* hxy = (TH2D*)fHist3->Project3D("yx");
    TH2D* hzx = (TH2D*)fHist3->Project3D("xz");
    TH2D* hzy = (TH2D*)fHist3->Project3D("yz");
    // Find Z-position
    int ibmax = fHistZ->GetMaximumBin(), nmax = 0;
    Double_t wind = fHistZ->GetBinWidth(1) * 5;
    Double_t zmin = fHistZ->GetBinCenter(ibmax) - wind;
    Double_t zmax = fHistZ->GetBinCenter(ibmax) + wind;
    if (fHistZ->GetMaximum() > 0.5) {
        // auto fitRes = fHistZ->Fit("gaus","sw","",zmin,zmax);
        auto fitRes = fHistZ->Fit("gaus", "sw0q", "", zmin, zmax);
        Double_t peakint = fHistZ->Integral(TMath::Max(1, ibmax - 5), TMath::Min(nz, ibmax + 5));
        Double_t zerr = fitRes->GetParams()[2] / TMath::Sqrt(peakint);
        Double_t zpeak = fitRes->GetParams()[1];
        fMean[2] = 999;
        if (TMath::Abs(zpeak - ztarg) > 3 * zerr) {   // 3*sigma
            // Check peak in ZY-histogram
            hzy->GetXaxis()->SetRange(ibmax - 5, ibmax + 5);
            if (hzy->GetMaximum() > 2) {
                int ix, iy, iz;
                hzy->GetMaximumBin(ix, iy, iz);
                zpeak = fHistZ->GetBinCenter(ix);
                if (TMath::Abs(zpeak - ztarg) > 3 * zerr) {
                    fMean[2] = zpeak;
                    ztarg = fMean[2];
                }
            }
            hzy->GetXaxis()->SetRange(2, 1);   // reset range
        }
        fSigma[2] = zerr;
        // cout << zpeak << " " << zerr << " " << ztarg << endl;
        //  Find maxima
        if (fHistZ->GetMaximum() > 2) {
            ++nmax;
            int ib = 1, ie = ibmax, idi = 1, cmax = -1;
            if (nz - ibmax > ie - 1) {
                ib = nz;
                idi = -1;
            }
            Double_t cmax2 = 0.25 * fHistZ->GetMaximum();   // minimum value of 2'nd maximum
            cmax2 = TMath::Max(cmax2, 2.0);

            for (int j = ib; j != ie; j += idi) {
                int cont = fHistZ->GetBinContent(j);
                if (cont > cmax2 && cont > cmax)
                    cmax = cont;
                else if (cont < cmax * 0.75) {
                    ++nmax;
                    break;
                }
            }
        }
    }   // if (fHistZ->GetMaximum() > 0.5)
    //<--- AZ-300525
    fPrimVert->SetUniqueID(nmax);
    vector<Double_t> vecDist(nTracks, 999);

    for (int itr = 0; itr < nTracks; ++itr) {
        CbmKFTrack tr(*fTracks[itr]);
        Bool_t err = tr.Extrapolate(ztarg);
        if (err)
            continue;
        // AZ-240525 Double_t wx = 1 / TMath::Sqrt(tr.GetCovMatrix()[0]);
        // AZ-240525 Double_t wy = 1 / TMath::Sqrt(tr.GetCovMatrix()[2]);
        // cout << itr << " xxx " << fTracks[itr]->GetTrkID() << endl;
        CbmStsTrack* ststr = (CbmStsTrack*)fInputTracks->UncheckedAt(
            fTracks[itr]->GetTrkID());   // AZ-240525
                                         // AZ-020725 Double_t wx = ststr->GetNStsHits(); // AZ-240525
        // AZ-020725 Double_t wy = wx; // AZ-240525
        Double_t wx = 1 / ststr->GetParamFirst()->GetCovariance(0, 0);   // AZ-020725
        Double_t wy = 1 / ststr->GetParamFirst()->GetCovariance(1, 1);   // AZ-020725
        // cout << wx << " -------- " << wy << endl;
        Double_t xtr = tr.GetTrack()[0];
        Double_t ytr = tr.GetTrack()[1];
        fHist[0]->Fill(xtr);
        fHist[1]->Fill(ytr);
        fHistw[0]->Fill(xtr, wx);
        fHistw[1]->Fill(ytr, wy);
        fHistErr[0]->Fill(TMath::Sqrt(tr.GetCovMatrix()[0]));   // AZ-240524
        fHistErr[1]->Fill(TMath::Sqrt(tr.GetCovMatrix()[2]));
        Double_t dist = TMath::Sqrt(xtr * xtr + ytr * ytr);
        fHistR->Fill(dist);
        vecDist[itr] = dist;
    }
    // AZ-200226 Double_t peakDist = fHistR->GetBinCenter(fHistR->GetMaximumBin());
    //---> AZ-200226 - procedure to avoid "circular" artefacts
    int irmax = fHistR->GetMaximumBin(), nbins = 0;
    int i1 = irmax - 1, i2 = irmax + 1;
    if (i1 < 1) {
        ++i1;
        ++i2;
    }
    if (i2 > fHistR->GetNbinsX()) {
        --i1;
        --i2;
    }

    // Take mean of 3 bins
    Double_t wr = 0.0, wsum = 0.0, peakDist = 0.0;

    for (int iii = i1; iii <= i2; ++iii) {
        Double_t w = fHistR->GetBinContent(iii);
        wsum += w;
        wr += (w * fHistR->GetBinCenter(iii));
        if (w > 0.1)
            ++nbins;
    }
    if (nbins > 1)
        peakDist = wr / wsum;
    else
        peakDist = fHistR->GetBinLowEdge(irmax) + gRandom->Rndm() * fHistR->GetBinWidth(irmax);
    //<--- AZ

    // Reshuffle tracks according to their distance to the mean value
    multimap<Double_t, int> mapDist;

    for (int itr = 0; itr < nTracks; ++itr)
        mapDist.insert(pair<Double_t, int>(TMath::Abs(vecDist[itr] - peakDist), itr));

    vector<CbmKFTrackInterface*> vTracks;

    for (auto mit = mapDist.begin(); mit != mapDist.end(); ++mit) {
        if (mit->first > 99)
            break;
        vTracks.push_back(fTracks[mit->second]);
    }
    // cout << " Tracks: " << vTracks.size() << " " << fTracks.size() << endl;
    fTracks.clear();
    fTracks = vTracks;

    fSigma[0] = fSigma[1] = -1;

    for (int ipass = 0; ipass < 2; ++ipass) {
        /*cout << " EvalVertex: " << fHistw[0]->GetMean() << " " << fHist[0]->GetMaximum() << " "
          << fHistw[1]->GetMean() << " " << fHist[1]->GetMaximum() << " " << fHist[0]->GetEntries() << endl;*/

        for (int i = 0; i < 2; ++i) {
            sigmas[i] = fHistw[i]->GetRMS();
            if (sigmas[i] < 0.001)
                continue;
            // cout << " EvalVertex: " << sigmas[i] << endl;
            int sum = fHist[i]->GetEffectiveEntries();
            // cout << i << " " << sum << " " << fHist[i]->GetMaximum() << endl;
            if (sum > fHist[i]->GetMaximum()) {
                // More than 1 bin
                // AZ-240525 fSigma[i] = sigmas[i];
                fSigma[i] = fHistw[i]->GetMeanError();   // AZ-240525
                // AZ-240525 fMean[i] = fHistw[i]->GetMean();
                fMean[i] = fHistw[i]->GetBinCenter(fHistw[i]->GetMaximumBin());   // AZ-240525
            }
        }

        if (ipass == 0) {
            for (int i = 0; i < 2; ++i) {
                if (sigmas[i] < 0.001)
                    continue;
                Double_t xpeak = fHistw[i]->GetBinCenter(fHistw[i]->GetMaximumBin());
                Double_t xmin = xpeak - sigmas[i];
                Double_t xmax = xpeak + sigmas[i];
                fHistw[i]->SetAxisRange(xmin, xmax);   // change axis range
                fHist[i]->SetAxisRange(xmin, xmax);    // change axis range
            }
        }
    }
    TCanvas* c1 = (TCanvas*)gROOT->FindObject("cv1");
    if (c1) {
        for (int j = 0; j < 6; ++j) {
            if (j == 0) {
                fHistZ->Draw();
            } else if (j == 1)
                hxy->Draw("col");
            else if (j == 2)
                hzx->Draw("col");
            else if (j == 3)
                hzy->Draw("col");
            else if (j == 4)
                fHZR->Draw("col");
            else if (j == 5) {
                TAxis* zax = fHist3->GetZaxis();
                int nbz = zax->GetNbins();
                int iz0 = zax->FindBin(0.0);
                zax->SetRange(iz0, iz0);
                TH2D* hhh = (TH2D*)fHist3->Project3D("yx");
                hhh->Draw("col");
                zax->SetRange(1, nbz);
            }
            c1->Modified();
            c1->Update();
            if (getchar() == 'e') {   // AZ-010625 - for plots
                SetBreakout(1);
                return;
            }
        }
        for (int i = 0; i < 2; ++i) {
            // fHist[i]->Draw();
            fHistw[i]->Draw();
            c1->Modified();
            c1->Update();
            getchar();
        }
    }   // if (c1)
}

//__________________________________________________________________________

void BmnKFPrimaryVertexFinder::Fit(CbmKFVertexInterface& vtx)
{
    //* Constants

    // const Double_t CutChi2  = 3.5*3.5;
    const Int_t MaxIter = 3;   // 3;
    // const Double_t chi2acc[3] = {10., 15., 20.};
    const Double_t chi2acc[3] = {10., 12.25, 12.25};

    //* Vertex state vector and the covariance matrix

    Double_t r[3], *C = vtx.GetCovMatrix();

    //* Initialize the vertex

    for (Int_t i = 0; i < 6; i++)
        C[i] = 0;
    if (CbmKF::Instance()->vTargets.empty()) {
        r[0] = r[1] = r[2] = 0.;
        C[0] = C[2] = 5.;
        C[5] = 0.25;
    } else {
        CbmKFTube& t = CbmKF::Instance()->vTargets[0];
        r[0] = r[1] = 0.;
        r[2] = t.z;
        // AZ-151023 C[0] = C[2] = t.RR/3.5/3.5;
        C[0] = C[2] = 2 * t.RR / 3.5 / 3.5;   // AZ-151023
        // AZ-181023 C[5] = (t.dz/2/3.5)*(t.dz/2/3.5);
        C[5] = (t.dz / 3.5) * (t.dz / 3.5);   // AZ-181023
        r[0] = 0.4;                           // AZ-210923 - run 8
        // cout << t.x << " " << t.y << " " << t.z << endl; //AZ-180923
        // cout << t.RR << " " << t.dz << endl; //AZ-180923
        // exit(0); //AZ-180923
    }
    // Take evaluated parameters
    if (fSigma[0] > 0) {
        r[0] = fMean[0];
        C[0] = fSigma[0] * fSigma[0];
    }
    if (fSigma[1] > 0) {
        r[1] = fMean[1];
        C[2] = fSigma[1] * fSigma[1];
    }
    // AZ-310525
    if (fSigma[2] > 0) {
        if (fMean[2] < 900) {
            r[2] = fMean[2];
            C[5] = TMath::Max(fSigma[2] * fSigma[2], C[5]);
        }
    }

    //* Iteratively fit vertex - number of iterations fixed at MaxIter
    TMatrixD c(3, 3), cov(3, 3), xk0(3, 1), xk(3, 1), ck0(5, 1), a(5, 3), b(5, 3);
    TMatrixD g(5, 5);
    int ok = 1, nOK = 0, nTracks = fTracks.size();
    Double_t chi2 = 0.;

    xk.SetMatrixArray(r);
    // cout << " Tracks: " << nTracks << " " << xk(0, 0) << " " << xk(1, 0) << " " << xk(2, 0) << endl;

    for (Int_t iteration = 0; iteration < MaxIter; ++iteration) {

        //* Store vertex from previous iteration

        Double_t r0[3], C0[6];

        for (Int_t i = 0; i < 3; i++)
            r0[i] = r[i];
        for (Int_t i = 0; i < 6; i++)
            C0[i] = C[i];

        //* Initialize the vertex covariance, Chi^2 & NDF

        /*
        C[0] = 100.;
        C[1] =   0.; C[2] = 100.;
        C[3] =   0.; C[4] =   0.; C[5] = 100.;
        */

        // AZ-210923 vtx.GetRefNDF()  = -3;
        vtx.GetRefNDF() = 0;   // AZ-210923
        vtx.GetRefChi2() = 0.;
        vtx.GetRefNTracks() = 0;

        Double_t cutChi2 = chi2acc[TMath::Min(iteration, 2)];
        chi2 = 0.0;

        if (iteration == 0) {
            for (int i = 0; i < 3; ++i) {
                for (int j = i; j < 3; ++j) {
                    int indx = CbmKFMath::indexS(i, j);
                    c(i, j) = c(j, i) = C0[indx];
                }
            }
            cov = TMatrixD(TMatrixD::kInverted, c);
        }

        // AZ-010625 Int_t ibeg = nTracks - 1, iend = -1, istep = -1;
        Int_t ibeg = 0, iend = nTracks, istep = 1;   // AZ-010625
                                                     /* AZ-270425
                                                         if (iteration % 2 > 0) {
                                                             ibeg = 0;
                                                             iend = nTracks;
                                                             istep = 1;
                                                         }
                                                     */
        nOK = 0;

        // for (vector<CbmKFTrackInterface*>::iterator itr = fTracks.begin(); itr != fTracks.end() ; ++itr) {
        for (int itr = ibeg; itr != iend; itr += istep) {

            if (ok) {
                xk0 = xk;   // xk0 stands for x(k-1)
                cov = TMatrixD(TMatrixD::kInverted, c);
            }

            // CbmKFTrack T(**itr);
            CbmKFTrack T(*fTracks[itr]);
            // AZ-131023 Bool_t err = T.Extrapolate( r0[2] );
            // AZ-131023 if( err ) continue;

            // Double_t *m = T.GetTrack();
            Double_t* V = T.GetCovMatrix();
            std::vector<Double_t> covMatr(V, V + 15);   // AZ-190923
            if (iteration == 0) {
                for (unsigned int j = 0; j < covMatr.size(); ++j)
                    covMatr[j] *= 10;
            }   // AZ-190923
            else
            {
                // AZ-240525 - restrict X-ccordinate accuracy to 300 um
                Double_t errx = TMath::Sqrt(covMatr[0]);
                if (errx < -0.01) {
                    Double_t scale = 0.01 / errx;
                    scale *= scale;
                    for (unsigned int j = 0; j < covMatr.size(); ++j)
                        covMatr[j] *= scale;
                }
            }
            V = covMatr.data();   // AZ-190923

            // Set g-matrix
            for (int i = 0; i < 5; ++i) {
                // for (int j = 0; j < 5; ++j) {
                for (int j = i; j < 5; ++j) {
                    int indx = CbmKFMath::indexS(i, j);
                    g(i, j) = g(j, i) = V[indx];
                }
            }
            // g.Print();
            g.Invert();
            // g.Print();
            Bool_t err = T.Extrapolate(r0[2]);
            if (err)
                continue;
            // cout << " --- xxx --- " << itr << " " << (T.GetRefNDF()+5)/2 << endl;

            // ComputeAandB(xk, track, track1, a, b, ck0); // compute matrices of derivatives
            // ComputeAandB (xk, T, (*itr)->GetTrack()[5], a, b, ck0); // compute matrices of derivatives
            err = ComputeAandB(xk, T, fTracks[itr]->GetTrack()[5], a, b, ck0);   // compute matrices of derivatives
            if (err)
                continue;

            // W = (Bt*G*B)'
            TMatrixD tmp(g, TMatrixD::kMult, b);
            TMatrixD w(b, TMatrixD::kTransposeMult, tmp);
            w.Invert();
            // w.Print();

            // Gb = G - G*B*W*Bt*G
            TMatrixD tmp1(b, TMatrixD::kTransposeMult, g);
            TMatrixD tmp2(w, TMatrixD::kMult, tmp1);
            TMatrixD tmp3(b, TMatrixD::kMult, tmp2);
            TMatrixD tmp4(g, TMatrixD::kMult, tmp3);
            TMatrixD gb = g;
            gb -= tmp4;

            // Ck = ((Ck-1)' + At*Gb*A)'
            TMatrixD tmp5(gb, TMatrixD::kMult, a);
            c = TMatrixD(a, TMatrixD::kTransposeMult, tmp5);
            // cout << "gb, cov, c" << endl;
            // gb.Print();
            // cov.Print();
            // c.Print();
            c += cov;
            c.Invert();

            // xk = Ck*((Ck-1)'x(k-1)+At*Gb*(m-ck0))
            // TMatrixD m = *track1.GetParamNew();
            TMatrixD m(5, 1);
            // m.SetMatrixArray ((*itr)->GetTrack());
            m.SetMatrixArray(fTracks[itr]->GetTrack());
            m -= ck0;   // m-ck0
            // cout << " m-ck0: " << endl;
            // ck0.Print();
            // m.Print();
            TMatrixD tmp11(gb, TMatrixD::kMult, m);
            TMatrixD tmp12(a, TMatrixD::kTransposeMult, tmp11);
            TMatrixD tmp13(cov, TMatrixD::kMult, xk0);
            tmp13 += tmp12;
            xk = TMatrixD(c, TMatrixD::kMult, tmp13);
            // cout << " Vertex: " << itr << " " << track->GetTrackID() << " " << xk(0,0) << " " << xk(1,0) << " " <<
            // xk(2,0) << endl;

            // qk = W*Bt*G*(m-ck0-A*xk)
            TMatrixD tmp21(a, TMatrixD::kMult, xk);
            tmp21 *= -1;
            tmp21 += m;   // m-ck0-A*xk
            TMatrixD tmp22(g, TMatrixD::kMult, tmp21);
            TMatrixD tmp23(b, TMatrixD::kTransposeMult, tmp22);
            TMatrixD qk(w, TMatrixD::kMult, tmp23);

            // Residual m-ck0-A*xk-B*qk
            TMatrixD res = tmp21;
            TMatrixD tmp31(b, TMatrixD::kMult, qk);
            res -= tmp31;
            // cout << " Residual matrix: " << endl;
            // res.Print();
            // qk.Print();

            // Chi2 = rt*G*res + (xk-x(k-1))t*(Ck-1)'*(xk-x(k-1))
            TMatrixD tmp41(g, TMatrixD::kMult, res);
            TMatrixD chi21(res, TMatrixD::kTransposeMult, tmp41);
            // chi21.Print();
            TMatrixD dx = xk;
            dx -= xk0;
            // dx.Print();
            TMatrixD tmp42(cov, TMatrixD::kMult, dx);
            TMatrixD chi22(dx, TMatrixD::kTransposeMult, tmp42);
            // chi22.Print();
            if (chi21(0, 0) < 0 || chi22(0, 0) < 0) {
                cout << " FindVertex: chi2 < 0 " << chi21(0, 0) << " " << chi22(0, 0) << " " << iteration << " "
                     << endl;
                // cout << track->GetNode() << " " << track->GetNodeNew() << " " << track->GetUniqueID() << " "
                //    << track->Momentum() << " " << track->GetDirection() << endl;
                //  exit(0);
                chi21(0, 0) = TMath::Max(chi21(0, 0), 0.);
                chi22(0, 0) = TMath::Max(chi22(0, 0), 0.);
            }
            chi21 += chi22;
            if (chi21(0, 0) > cutChi2) {
                ok = 0;
                continue;
            }   // skip track
            chi2 += chi21(0, 0);
            ++nOK;
            ok = 1;
            if (iteration == MaxIter - 1)
                fTrkID.push_back(fTracks[itr]->GetTrkID());   // AZ-090824
        }   // for (int itr = ibeg; itr != iend;
        /*
        if (ok)
           cout << " Vertex position: " << iteration << " " << xk(0, 0) << " " << xk(1, 0) << " " << xk(2, 0) << " " <<
        chi2
                << " " << nOK << " " << nPrim << endl;
        else
           cout << " Vertex position: " << iteration << " " << xk0(0, 0) << " " << xk0(1, 0) << " " << xk0(2, 0) << " "
                << chi2 << " " << nOK << " " << nPrim << endl;
        */

        if (iteration < MaxIter - 1) {   // AZ-181023 - reduce target-Z constraint
            if (ok)
                c *= 4;
            else
                cov *= 0.25;
        }

    }   // for (Int_t iteration = 0; iteration < MaxIter;

    if (ok == 0) {
        // Take vertex after last accepted track
        c = TMatrixD(TMatrixD::kInverted, cov);
        xk = xk0;
    }
    /*
        Double_t a = 0, b = 0;
        {
          Double_t zeta[2] = { r0[0]-m[0],  r0[1]-m[1] };

          // Check track Chi^2 deviation from the r0 vertex estimate

          //AZ-190923 Double_t S[3] = { (C0[2]+V[2]), -(C0[1]+V[1]), (C0[0]+V[0]) };
          Double_t S[3] = { (C0[0]+V[0]), -(C0[1]+V[1]), (C0[2]+V[2]) }; //AZ-190923
          Double_t    s =  S[2]*S[0] - S[1]*S[1];
          Double_t chi2 =  zeta[0]*zeta[0]*S[0] + 2*zeta[0]*zeta[1]*S[1]
                                                +   zeta[1]*zeta[1]*S[2];
          if( chi2 > s*CutChi2 ) continue;

          // Fit of vertex track slopes (a,b) to r0 vertex

          s = V[0]*V[2] - V[1]*V[1];
          if ( s < 1.E-20 ) continue;
          s = 1./s;
          a = m[2] + s*(   (  V[3]*V[2] - V[4]*V[1] )*zeta[0]
                         + (- V[3]*V[1] + V[4]*V[0] )*zeta[1] );
          b = m[3] + s*(   (  V[6]*V[2] - V[7]*V[1] )*zeta[0]
                         + (- V[6]*V[1] + V[7]*V[0] )*zeta[1] );
        }

        // Update the vertex (r,C) with the track estimate (m,V) :

        // Linearized measurement matrix H = { { 1, 0, -a}, { 0, 1, -b} };

        // Residual (measured - estimated)

        Double_t zeta[2] = { m[0] - ( r[0] - a*(r[2]-r0[2]) ),
                             m[1] - ( r[1] - b*(r[2]-r0[2]) ) };

        // CHt = CH'

        Double_t CHt[3][2] = { { C[0] - a*C[3], C[1] - b*C[3]},
                               { C[1] - a*C[4], C[2] - b*C[4]},
                               { C[3] - a*C[5], C[4] - b*C[5]} };

        // S = (H*C*H' + V )^{-1}

        Double_t S[3] = { V[0] + CHt[0][0] - a*CHt[2][0],
                          V[1] + CHt[1][0] - b*CHt[2][0],
                          V[2] + CHt[1][1] - b*CHt[2][1] };

        // Invert S
        {
          Double_t w = S[0]*S[2] - S[1]*S[1];
          if ( w < 1.E-20 ) continue;
          w = 1./w;
          Double_t S0 = S[0];
          S[0] =  w*S[2];
          S[1] = -w*S[1];
          S[2] =  w*S0;
        }

        // Calculate Chi^2

        vtx.GetRefChi2()+=  zeta[0]*zeta[0]*S[0] + 2*zeta[0]*zeta[1]*S[1]
           //AZ-190923 + zeta[1]*zeta[1]*S[2] + T.GetRefChi2();
           + zeta[1]*zeta[1]*S[2]; //AZ-190923
        //AZ-190923 vtx.GetRefNDF() += 2 + T.GetRefNDF();
        vtx.GetRefNDF() += 2; //AZ-190923
        vtx.GetRefNTracks()++;

       // Kalman gain K = CH'*S

        Double_t K[3][2];

        for( Int_t i=0; i<3; ++i ){  K[i][0] = CHt[i][0]*S[0] + CHt[i][1]*S[1] ;
                                     K[i][1] = CHt[i][0]*S[1] + CHt[i][1]*S[2] ; }

        // New estimation of the vertex position r += K*zeta

        for( Int_t i=0; i<3; ++i ) r[i]+= K[i][0]*zeta[0] + K[i][1]*zeta[1];

        // New covariance matrix C -= K*(CH')'

        C[0] -= K[0][0]*CHt[0][0] + K[0][1]*CHt[0][1];
        C[1] -= K[1][0]*CHt[0][0] + K[1][1]*CHt[0][1];
        C[2] -= K[1][0]*CHt[1][0] + K[1][1]*CHt[1][1];
        C[3] -= K[2][0]*CHt[0][0] + K[2][1]*CHt[0][1];
        C[4] -= K[2][0]*CHt[1][0] + K[2][1]*CHt[1][1];
        C[5] -= K[2][0]*CHt[2][0] + K[2][1]*CHt[2][1];

      }// itr

    }// finished iterations
    */

    // Copy state vector to output (covariance matrix was used directly )

    // vtx.GetRefX() = r[0];
    // vtx.GetRefY() = r[1];
    // vtx.GetRefZ() = r[2];
    vtx.GetRefX() = xk(0, 0);
    vtx.GetRefY() = xk(1, 0);
    vtx.GetRefZ() = xk(2, 0);
    vtx.GetRefNTracks() = nOK;
    vtx.GetRefChi2() = chi2;
    vtx.GetRefNDF() = nOK * 2;

    for (int i = 0; i < 3; ++i) {
        for (int j = i; j < 3; ++j) {
            int indx = CbmKFMath::indexS(i, j);
            C[indx] = c(i, j);
        }
    }
}

//__________________________________________________________________________

// void BmnKFPrimaryVertexFinder::ComputeAandB(TMatrixD &xk0, const MpdKalmanTrack *track, const MpdKalmanTrack &trackM,
//                                           TMatrixD &a, TMatrixD &b, TMatrixD &ck0)
// void BmnKFPrimaryVertexFinder::ComputeAandB(TMatrixD &xk0, const CbmKFTrack &track, Double_t zhit, TMatrixD &a,
// TMatrixD &b, TMatrixD &ck0)
Bool_t BmnKFPrimaryVertexFinder::ComputeAandB(TMatrixD& xk0,
                                              CbmKFTrackInterface& track,
                                              Double_t zhit,
                                              TMatrixD& a,
                                              TMatrixD& b,
                                              TMatrixD& ck0)
{
    /// Compute matrices of derivatives w.r.t. vertex coordinates and track momentum
    // (track - at dca to (0,0), trackM - at TPC inner shell - for Proxim)

    Double_t vert0[3], *vert = xk0.GetMatrixArray();

    for (Int_t i = 0; i < 3; ++i)
        vert0[i] = vert[i];

    /*AZ-121023 - doesn't seem to be necessary
    MpdKalmanTrack trackk = *track;
    trackk.SetPos(trackk.GetPosNew());
    // trackk.GetParam()->Print();
    //  Propagate track to PCA w.r.t. point xk0
    MpdKalmanFilter::Instance()->FindPca(&trackk, vert0);
    // MpdKalmanFilter::Instance()->FindPca(&trackk,zero); // just for test
    // cout << trackk.GetPosNew() << endl;
    trackk.SetParam(*trackk.GetParamNew());
    // trackk.GetParam()->Print();
    */
    // Find PCA to vert0
    // CbmKFTrack trackk(track);
    // FindPca (trackk, vert0);

    // Put track at xk0
    CbmKFTrack tr(track);
    // CbmKFTrack tr(trackk);
    tr.GetTrack()[0] = vert0[0];
    tr.GetTrack()[1] = vert0[1];
    tr.GetTrack()[5] = vert0[2];
    CbmKFTrack tr0(tr);

    // Propagate track to zhit
    Bool_t err = tr.Extrapolate(zhit);
    // if (err) continue;
    if (err)
        return err;

    // Double_t shift = 0.01; // 100 um coordinate shift
    Double_t shift = 0.1;   // 1 mm coordinate shift

    for (Int_t i = 0; i < 3; ++i) {
        // MpdKalmanTrack track1 = track0;
        CbmKFTrack tr1(tr0);
        vert0[i] += shift;
        if (i > 0)
            vert0[i - 1] -= shift;
        tr1.GetTrack()[0] = vert0[0];
        tr1.GetTrack()[1] = vert0[1];
        tr1.GetTrack()[5] = vert0[2];

        // Propagate track to zhit
        err = tr1.Extrapolate(zhit);
        // if (err) continue;
        if (err)
            return err;

        // Derivatives
        for (Int_t j = 0; j < 5; ++j) {
            // a(j, i) = (track1.GetParamNew(j) - trackk.GetParamNew(j)) / shift;
            a(j, i) = (tr1.GetTrack()[j] - tr.GetTrack()[j]) / shift;
        }
    }

    for (Int_t i = 0; i < 3; ++i) {
        // MpdKalmanTrack track1 = track0;
        CbmKFTrack tr1(tr0);
        Int_t j = i + 2;
        // shift                 = (*track->GetCovariance())(j, j);
        int indx = CbmKFMath::indexS(j, j);
        shift = track.GetCovMatrix()[indx];
        shift = TMath::Sqrt(shift);
        if (j == 4)
            shift *= TMath::Sign(1., -tr0.GetTrack()[j]);   // 1/p
        // track1.SetParam(j, track0.GetParamNew(j) + shift);
        tr1.GetTrack()[j] += shift;

        // Propagate track to zhit
        err = tr1.Extrapolate(zhit);
        // if (err) continue;
        if (err)
            return err;

        // Derivatives
        for (Int_t k = 0; k < 5; ++k) {
            // b(k, i) = (track1.GetParamNew(k) - trackk.GetParamNew(k)) / shift;
            b(k, i) = (tr1.GetTrack()[k] - tr.GetTrack()[k]) / shift;
        }
    }

    TMatrixD qk0(3, 1);
    // for (Int_t i = 0; i < 3; ++i) qk0(i, 0) = track0.GetParamNew(i + 2);
    for (Int_t i = 0; i < 3; ++i)
        qk0(i, 0) = tr0.GetTrack()[i + 2];
    // qk0.Print();
    // ck0 = *trackk.GetParamNew();
    for (int i = 0; i < 5; ++i)
        ck0(i, 0) = tr.GetTrack()[i];
    ck0 -= TMatrixD(a, TMatrixD::kMult, xk0);
    ck0 -= TMatrixD(b, TMatrixD::kMult, qk0);
    // cout << " Derivatives: " << endl;
    // a.Print();
    // b.Print();
    // ck0.Print();
    // TMatrixD(b,TMatrixD::kMult,qk0).Print();
    // TMatrixD(a,TMatrixD::kMult,xk0).Print();
    return kFALSE;
}

//__________________________________________________________________________

void BmnKFPrimaryVertexFinder::FindPca(CbmKFTrackInterface& tr, Double_t* vert)
{
    // Find track PCA w.r.t. vert using parabolic approximation

    // CbmKFTrack tr(track);
    fHistPca->Reset();

    for (int i = -1; i < 2; ++i) {
        tr.Extrapolate(vert[2] + 1.0 * i);
        Double_t* pars = tr.GetTrack();
        Double_t dx = pars[0] - vert[0], dy = pars[1] - vert[1], dz = pars[5] - vert[2];
        Double_t dist = TMath::Sqrt(dx * dx + dy * dy + dz * dz);
        fHistPca->Fill(dz, dist);
    }
    fHistPca->Fit("pol2", "Q0");
    TF1* func = fHistPca->GetFunction("pol2");
    Double_t dzPca = func->GetMinimumX();
    // cout << dzPca << endl;
    tr.Extrapolate(vert[2] + dzPca);

    // TCanvas *c1 = (TCanvas*) gROOT->FindObject("c1");
    // if (c1) { fHistPca->Draw(); c1->Modified(); c1->Update(); getchar(); }
}
