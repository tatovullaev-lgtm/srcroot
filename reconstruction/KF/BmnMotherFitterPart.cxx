/// \class BmnMotherFitterPart
///
/// Kalman filter mother particle fitter for the BM@N detector (using BmnParticle)
/// \author Alexander Zinchenko (LHEP, JINR, Dubna)
/// 03-07-2025

#include "BmnMotherFitterPart.h"
// #include "MpdKalmanFilter.h"
// #include "MpdKalmanGeoScheme.h"
// #include "MpdKalmanTrack.h"
// #include "MpdKalmanHit.h"
// #include "MpdHelix.h"
#include "CbmKF.h"
#include "CbmKFMath.h"
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
// #include "MpdMCTrack.h"

#include "FairField.h"
#include "FairRunAna.h"
#include "FairTask.h"

#include <TCanvas.h>
#include <TMath.h>
#include <TMatrixD.h>
#include <TVector3.h>
#include <iostream>
using std::cout;
using std::endl;

BmnMotherFitterPart* BmnMotherFitterPart::fgMF = 0x0;

//__________________________________________________________________________
BmnMotherFitterPart::BmnMotherFitterPart()
    : FairTask()
{
    /// Default constructor

    fCovar.ResizeTo(3, 3);
    // fKF = MpdKalmanFilter::Instance();
    fKF = CbmKF::Instance();
}

//__________________________________________________________________________
BmnMotherFitterPart::BmnMotherFitterPart(const char* name, const char* title)
    : FairTask(name)
{
    /// Constructor

    fgMF = this;
    fCovar.ResizeTo(3, 3);

    // fKF = MpdKalmanFilter::Instance();
    fKF = CbmKF::Instance();
}

//__________________________________________________________________________
BmnMotherFitterPart* BmnMotherFitterPart::Instance()
{
    /// Get pointer to the mother fitter singleton object
    if (!fgMF) {
        fgMF = new BmnMotherFitterPart;
        fgMF->Init();
        // automatic destruction is forced
        std::atexit(DestroyInstance);
    }
    return fgMF;
}

//__________________________________________________________________________
BmnMotherFitterPart* BmnMotherFitterPart::Instance(const char* name, const char* title)
{
    /// Get pointer to the mother fitter singleton object
    if (!fgMF) {
        fgMF = new BmnMotherFitterPart(name, title);
        fgMF->Init();
        // automatic destruction is forced
        std::atexit(DestroyInstance);
    }
    return fgMF;
}

//__________________________________________________________________________
BmnMotherFitterPart::~BmnMotherFitterPart()
{
    /// Destructor
    // FairRootManager *manager = FairRootManager::Instance();
    // manager->Write();
}

//__________________________________________________________________________
InitStatus BmnMotherFitterPart::Init()
{

    cout << "InitStatus MpdMotherFitterPart::Init\n\n";
    Double_t bZ = 5.;
    FairField* magField = FairRunAna::Instance()->GetField();
    if (!magField || TMath::Abs(magField->GetBz(0, 0, 0)) < 0.01) {
        cout << " -I- MpdMotherFitterPart::Init - Using the default constant magnetic field Bz = 5 kG " << endl;
    } else
        bZ = TMath::Abs(magField->GetBz(0, 0, 0));
    fieldConst = 0.3 * 0.01 * bZ / 10;

    // AZ-021225 - for testing
    fTestHist = nullptr;
    TCanvas* c1 = (TCanvas*)gROOT->FindObject("cMothFit");
    if (c1)
        fTestHist = new TH1D("hTestHist", " ", 110, -10, 100);
    //
    return kSUCCESS;
}

//__________________________________________________________________________
InitStatus BmnMotherFitterPart::ReInit()
{
    // fMagField = FairRunAna::Instance()->GetField(); // !!! interim solution !!!
    return kSUCCESS;
}

//__________________________________________________________________________
void BmnMotherFitterPart::Reset()
{
    ///
    // cout << " MpdMotherFitterPart::Reset  " << endl;
}

//__________________________________________________________________________
void BmnMotherFitterPart::Register()
{
    ///
    // FairRootManager::Instance()->
    // Register("TpcLheKalmanTrack","TpcLheKalmanTrack", fTracks, kFALSE);
}

//__________________________________________________________________________
void BmnMotherFitterPart::Finish()
{
    ///
}

//__________________________________________________________________________
void BmnMotherFitterPart::Exec(Option_t* option)
{
    ///
}

//__________________________________________________________________________

Double_t BmnMotherFitterPart::BuildMother(BmnParticle* mother, vector<BmnParticle*>& vDaught)
{
    /// Build mother particle from daughters which were smoothed
    /// according to the decay vertex constraint (after FindVertex)

    TVector3 vtx;
    Double_t chi2 = BmnMotherFitterPart::Instance()->FindVertex(vDaught, vtx);
    // cout << " FindVertex: " << fVtx[0] << " " << fVtx[1] << " " << fVtx[2] << " " << chi2 << endl;
    if (chi2 < -1.0)
        return chi2;   // failed to find decay vertex (too high chi2)
    Int_t nDaught = vDaught.size();

    // Check for sanity
    /*
    for (Int_t id = 0; id < nDaught; ++id) {
       Double_t theta = vDaught[id]->Theta();
       if (theta < 0 || theta > TMath::Pi()) return -chi2; // weird track
    }
    */

    mother->SetChi2(chi2);

    TVector3 mom3;
    Int_t charge = 0, ndf = 0;
    Double_t energy = 0.0;

    for (Int_t i = 0; i < nDaught; ++i) {
        BmnParticle* part = vDaught[i];
        charge += part->GetCharge();
        mom3 += part->Momentum3();
        part->FillJ();
        Double_t ptot = part->Momentum3().Mag();
        energy += TMath::Sqrt(part->GetMass() * part->GetMass() + ptot * ptot);
        mother->AddDaughter(part->GetIndx());
        // cout << " ---> " << i << " " << part->GetCharge() << " " << part->GetMass() << " " << ptot << " "
        //    << 1/part->GetMeas(4) << endl;
        ndf += part->GetNDF();
    }
    mother->SetNDF(ndf);
    mother->SetCharge(charge);
    mother->SetMass(TMath::Sqrt(energy * energy - mom3.X() * mom3.X() - mom3.Y() * mom3.Y() - mom3.Z() * mom3.Z()));
    mother->Setx(vDaught[0]->Getx());
    mother->SetZ(mother->Getx()(2, 0));
    TMatrixD qm(3, 1);
    /* AZ-050725
    qm(0, 0) = mom3.Phi();
    qm(1, 0) = mom3.Theta();
    if (charge == 0)
       qm(2, 0) = mom3.Mag();
    else
       qm(2, 0) = -fieldConst / mom3.Pt() * TMath::Abs(charge);
    */
    qm(0, 0) = mom3.X() / mom3.Z();
    qm(1, 0) = mom3.Y() / mom3.Z();
    if (charge == 0)
        qm(2, 0) = mom3.Mag();
    else
        qm(2, 0) = charge / mom3.Mag();

    mother->Setq(qm);

    // AZ-120725 ParamsAtDca(mother); // compute params at DCA
    // AZ-080825 mother->ParamsAtDca();   // compute params at DCA
    // return chi2; //

    mother->FillJinv(mom3);

    // Compute covariance matrix
    TMatrixD en(3, 3);
    TMatrixD ec(3, 3);
    for (Int_t i = 0; i < nDaught; ++i) {
        BmnParticle* part = vDaught[i];
        // E += E*Jt
        TMatrixD jt = TMatrixD(TMatrixD::kTransposed, part->GetJ());
        TMatrixD tmp = TMatrixD(part->GetE(), TMatrixD::kMult, jt);
        if (part->GetCharge()) {
            // Charged track
            ec += tmp;
        } else {
            // Neutral
            en += tmp;
        }
    }

    TMatrixD etot = ec;
    etot += en;
    TMatrixD c = GetCovariance();
    TMatrixD qtot = ComputeQmatr(vDaught);

    TMatrixD ck0(5, 1);
    // ComputeAandB(mother->Getx(), *mother, fA, fB, ck0);
    Bool_t err = ComputeAandB(mother->Getx(), *mother, mother->GetA(), mother->GetB(), ck0);
    if (err)
        return -9.0;

    // Covar. matrix
    TMatrixD at(TMatrixD::kTransposed, mother->GetA());
    TMatrixD tmp11(c, TMatrixD::kMult, at);
    TMatrixD tmp12(mother->GetA(), TMatrixD::kMult, tmp11);

    TMatrixD bt(TMatrixD::kTransposed, mother->GetB());
    TMatrixD tmp21(mother->GetJinv(), TMatrixD::kTransposeMult, bt);
    TMatrixD tmp22(etot, TMatrixD::kMult, tmp21);
    TMatrixD tmp23(mother->GetA(), TMatrixD::kMult, tmp22);

    TMatrixD tmp31(etot, TMatrixD::kTransposeMult, at);
    TMatrixD tmp32(mother->GetJinv(), TMatrixD::kMult, tmp31);
    TMatrixD tmp33(mother->GetB(), TMatrixD::kMult, tmp32);

    TMatrixD tmp41(mother->GetJinv(), TMatrixD::kTransposeMult, bt);
    TMatrixD tmp42(qtot, TMatrixD::kMult, tmp41);
    TMatrixD tmp43(mother->GetJinv(), TMatrixD::kMult, tmp42);
    TMatrixD tmp44(mother->GetB(), TMatrixD::kMult, tmp43);

    TMatrixD gm(5, 5);
    gm = tmp12;
    gm += tmp23;
    gm += tmp33;
    gm += tmp44;
    // cout << " Mother covariance " << endl;
    // fG.Print();
    gm.Invert();   // mother weight
    mother->SetG(gm);

    mother->ParamsAtDca();   // AZ-080825 compute params at DCA

    return chi2;
}

//__________________________________________________________________________
/*

Double_t BmnMotherFitterPart::BuildMother(BmnParticle *mother, vector<CbmStsTrack *> &vTracks,
                                          vector<BmnParticle *> &vDaught)
{
   /// Build mother particle from daughters which were smoothed
   /// according to the decay vertex constraint (after FindVertex).
   /// Daughters are built from tracks and parametrized at their
   /// intersection point.

   vDaught.clear();
   vector<MpdHelix> vhel;

   // Create 2 helices and cross them
   for (Int_t itr = 0; itr < 2; ++itr) {
      Double_t rad = vTracks[itr]->GetPosNew();
      Double_t phi = 0.0;
      if (rad > 1.e-6) phi = (*vTracks[itr]->GetParam())(0, 0) / rad;
      vhel.push_back(MpdHelix(vTracks[itr]->Momentum3(),
                              TVector3(rad * TMath::Cos(phi), rad * TMath::Sin(phi), (*vTracks[itr]->GetParam())(1, 0)),
                              vTracks[itr]->Charge()));
   }
   pair<Double_t, Double_t> paths = vhel[0].pathLengths(vhel[1]);
   TVector3                 cross;
   Double_t                 xyz[3] = {0};
   Int_t                    ntr    = vTracks.size();

   if (paths.first < 100.0 || paths.second < 100.0) {
      // MpdKalmanHit hit;
      // hit.SetType(MpdKalmanHit::kFixedR);
      cross = vhel[0].at(paths.first);
      cross += vhel[1].at(paths.second);
      cross *= 0.5;
      cross.GetXYZ(xyz);

      for (Int_t itr = 0; itr < ntr; ++itr) {
        //Double_t s = vhel[itr].pathLength(cross);
        //TVector3 pca = vhel[itr].at(s);
        //hit.SetPos(pca.Pt());
        //fKF->PropagateToHit(vTacks[itr],&hit,kFALSE);
      }

   }

   // xyz[0] = xyz[1] = xyz[2] = 0;
   for (Int_t itr = 0; itr < ntr; ++itr) {
      fKF->FindPca(vTracks[itr], xyz);
      vTracks[itr]->SetParam(*vTracks[itr]->GetParamNew());
      vTracks[itr]->Weight2Cov(); // AZ
      vDaught.push_back(new MpdParticle(*vTracks[itr], -1, 0.1396, xyz));
   }
   Double_t chi2 = BuildMother(mother, vDaught);
   // Bring back to master frame in transverse plane
   for (Int_t j = 0; j < 2; ++j) mother->Getx()(j, 0) += xyz[j];
    return 0; //chi2;
}
*/

//__________________________________________________________________________

Bool_t BmnMotherFitterPart::EvalVertex(vector<BmnParticle*> vDaught)
{
    /// Evaluate decay vertex as average value of PCAs of particle pair combinations.
    /// Distances between particles vs Z are approximated by parabolas.

    int nDaught = vDaught.size();
    // Double_t dz = 10.0;
    TMatrixFSym covMatr(5);
    int niter = 2;
    fVtx[2] = 999.0;

    for (int iter = 0; iter < niter; ++iter) {
        // Make iterations
        vector<FairTrackParam> vParams;
        if (iter == 0) {
            // Propagate all tracks to Zmin

            for (int ip = 0; ip < nDaught; ++ip) {
                BmnParticle* part = vDaught[ip];
                fVtx[2] = TMath::Min(fVtx[2], part->GetXY(0));
            }
        }

        for (int ip = 0; ip < nDaught; ++ip) {
            BmnParticle* part = vDaught[ip];
            FairTrackParam par;
            CbmKFTrack tr = part->GetKFTrack();
            int ierr = tr.Propagate(fVtx[2]);
            if (ierr != 0)
                return kFALSE;
            tr.GetTrackParam(par);
            vParams.push_back(par);
        }

        // Pair combinations
        Double_t zzz = 0, dxmax = 0;
        int npairs = 0;

        for (int ip = 0; ip < nDaught; ++ip) {
            Double_t ty0 = vParams[ip].GetTy();

            for (int ip1 = ip + 1; ip1 < nDaught; ++ip1) {
                Double_t ty1 = vParams[ip1].GetTy();
                if (TMath::Abs(ty1 - ty0) > 1.e-6) {
                    Double_t dz = vParams[ip1].GetY() - vParams[ip].GetY();
                    dz /= (vParams[ip].GetTy() - vParams[ip1].GetTy());
                    Double_t z = fVtx[2] + dz;
                    zzz += z;
                    ++npairs;
                    // Find dx
                    CbmKFTrack tr(vParams[ip]);
                    CbmKFTrack tr1(vParams[ip1]);
                    int ierr = tr.Propagate(z);
                    int ierr1 = tr1.Propagate(z);
                    if (ierr != 0 || ierr1 != 0)
                        return kFALSE;
                    dxmax = TMath::Max(dxmax, TMath::Abs(tr.GetTrack()[0] - tr1.GetTrack()[0]));
                }
            }
        }
        fVtx[2] = (npairs) ? zzz / npairs : 0.0;
        // cout << " xxx " << iter << " " << fVtx[2] << " " << dxmax << endl;
        if (dxmax > 50)
            return kFALSE;   // too large dX

        // Find distances between 2 parabolas
        if (fTestHist)
            fTestHist->Reset();

        if (fTestHist && iter == 0) {
            // AZ-021225 - for test
            TCanvas* c1 = (TCanvas*)gROOT->FindObject("cMothFit");
            Double_t cmax = fTestHist->GetBinContent(fTestHist->GetMaximumBin());
            fTestHist->SetMaximum(cmax * 1.1);
            // fTestHist->Draw("hist");
            fTestHist->Fit("pol2", "w");
            c1->Modified();
            c1->Update();
            getchar();
        }

        /*
        for (int iz = 0; iz < 3; ++iz) {
        cout << " Dists: " << dists[iz][0].X();
        }
        cout << " " << fVtx[2] << endl;
        */
        if (TMath::Abs(fVtx[2]) > 170)
            return kFALSE;   // failed vertex
        fVtx[0] = fVtx[1] = 0.0;

        for (int ip = 0; ip < nDaught; ++ip) {
            BmnParticle* part = vDaught[ip];
            // FairTrackParam par (part->GetMeas(0), part->GetMeas(1), 0.0, part->GetMeas(2), part->GetMeas(3),
            //		      part->GetMeas(4), covMatr);
            FairTrackParam par;
            CbmKFTrack tr = part->GetKFTrack();
            // tr.Extrapolate (fVtx[2]);
            tr.Propagate(fVtx[2]);
            tr.GetTrackParam(par);
            fVtx[0] += par.GetX();
            fVtx[1] += par.GetY();
        }
        fVtx[0] /= nDaught;
        fVtx[1] /= nDaught;
        // cout << " Vertex eval: " << fVtx[0] << " " << fVtx[1] << " " << fVtx[2] << endl;
        if (TMath::Abs(fVtx[0]) > 50.0 || TMath::Abs(fVtx[1]) > 40.0)
            return kFALSE;
    }   // for (int iter = 0;
    return kTRUE;
}

//__________________________________________________________________________

Double_t BmnMotherFitterPart::FindVertex(vector<BmnParticle*> vDaught, TVector3& vtx)
{
    /// Kalman filter based secondary vertex fitter

    const Int_t nPass = 3;            // number of iterations
    const Double_t cutChi2 = 1000.;   // chi2-cut

    if (!EvalVertex(vDaught))
        return -cutChi2;

    Int_t nDaught = vDaught.size();
    TMatrixD c(3, 3), cov(3, 3), xk0(3, 1), xk(3, 1), ck0(5, 1);
    TMatrixD a(5, 3), b(5, 3);
    // MpdKalmanHit hit;
    Double_t chi2 = 0;   //, chi2o = 0;

    xk.SetMatrixArray(fVtx);
    // xk += 1.0; // test
    c(0, 0) = c(1, 1) = c(2, 2) = 1;

    for (Int_t ipass = 0; ipass < nPass; ++ipass) {

        // chi2o = chi2;
        chi2 = 0.;
        // c.Zero(); // new 05.06.17
        c(0, 0) = c(1, 1) = c(2, 2) = 100;   // new 05.06.17
        if (ipass == 0)
            cov = TMatrixD(TMatrixD::kInverted, c);

        Int_t ibeg = 0, iend = nDaught, istep = 1;
        if (ipass % 2 > 0) {
            ibeg = nDaught - 1;
            iend = -1;
            istep = -1;
        }

        for (Int_t itr = ibeg; itr != iend; itr += istep) {
            xk0 = xk;   // xk0 stands for x(k-1)
            cov = TMatrixD(TMatrixD::kInverted, c);

            BmnParticle* part = vDaught[itr];

            Bool_t err = ComputeAandB(xk0, *part, a, b, ck0);   // compute matrices of derivatives
            if (err)
                return -9.0;

            TMatrixD g = part->GetG();

            // W = (Bt*G*B)'
            TMatrixD tmp(g, TMatrixD::kMult, b);
            TMatrixD w(b, TMatrixD::kTransposeMult, tmp);
            w.Invert();

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
            c += cov;
            c.Invert();

            // xk = Ck*((Ck-1)'x(k-1)+At*Gb*(m-ck0))
            TMatrixD m = part->GetMeas();
            // m.Print();
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
            TMatrixD r = tmp21;
            TMatrixD tmp31(b, TMatrixD::kMult, qk);
            r -= tmp31;
            // cout << " Residual matrix: " << endl;
            // r.Print();
            // qk.Print();

            // Chi2 = rt*G*r + (xk-x(k-1))t*(Ck-1)'*(xk-x(k-1))
            TMatrixD tmp41(g, TMatrixD::kMult, r);
            TMatrixD chi21(r, TMatrixD::kTransposeMult, tmp41);
            // chi21.Print();
            TMatrixD dx = xk;
            dx -= xk0;
            // Double_t dxMax = dx.Max();
            // if (dxMax > 7*TMath::Sqrt(cov.Max())) return -cutChi2; //AZ-050121
            TMatrixD tmp42(cov, TMatrixD::kMult, dx);
            TMatrixD chi22(dx, TMatrixD::kTransposeMult, tmp42);
            // chi22.Print();
            if (chi21(0, 0) < 0 || chi22(0, 0) < 0) {
                cout << " !!! Chi2 < 0 " << chi21(0, 0) << " " << chi22(0, 0) << " " << ipass << " " << itr << " "
                     << part->GetMeas(4) << endl;
                // exit(0);
            }
            chi21 += chi22;
            chi2 += chi21(0, 0);
            // if (chi2 > cutChi2) {
            if (chi2 > cutChi2 || chi21(0, 0) < 0 || chi22(0, 0) < 0) {
                for (Int_t i = 0; i < 3; ++i)
                    vtx[i] = fVtx[i];
                /*
                for (Int_t i = 0; i < nDaught; ++i) {
                  MpdKalmanTrack trTmp = *vDaught[i];
                  trTmp.SetPos(trTmp.GetPosNew());
                  trTmp.SetParamNew(*trTmp.GetParam());
                  if (trTmp.GetNode() != "") trTmp.SetNode(""); // 25-jul-2012
                  fKF->FindPca(&trTmp,fVtx);
                  vDaught[i]->SetParam(*trTmp.GetParamNew());
                  vDaught[i]->SetPosNew(trTmp.GetPosNew());
                }
                */
                // AZ-Debug
                // cout << " !!! Too high chi2: " << ipass << " " << itr << " " << chi2 << " " << chi22(0,0) << " " <<
                // chi21(0,0) << " " << vtx[0] << " " << vtx[1] << " " << vtx[2] << " " << fVtx[0] << " " << fVtx[1] <<
                // " "
                // << fVtx[2] << " " << tracks[0]->GetNode() << " " << tracks[1]->GetNode() << " ids: " <<
                // tracks[0]->GetTrackID() << " " << tracks[1]->GetTrackID() << " " << xk0(0,0) << " " << xk0(1,0) << "
                // " << xk0(2,0) << " " << xk(0,0) << " " << xk(1,0) << " " << xk(2,0) << endl;
                return -cutChi2;
            }
            // cout << ipass << " " << itr << " " << chi2 << endl;
        }   // for (Int_t itr = ibeg; itr != iend;
    }   // for (Int_t ipass = 0; ipass < nPass;

    for (Int_t i = 0; i < 3; ++i)
        vtx[i] = fVtx[i] = xk(i, 0);
    fCovar = c;

    int err = Smooth(vDaught);
    if (err)
        chi2 = -9.0;
    return chi2;
}

//__________________________________________________________________________
Bool_t BmnMotherFitterPart::Smooth(vector<BmnParticle*> vDaught)
{
    /// Smooth particle momentum and corresponding covariance matrices

    TMatrixD c(3, 3), xk(3, 1), ck0(5, 1);
    TMatrixD a(5, 3), b(5, 3);

    xk.SetMatrixArray(fVtx);
    Int_t nDaught = vDaught.size();
    Bool_t err = kFALSE;

    for (Int_t itr = 0; itr < nDaught; ++itr) {

        BmnParticle* part = vDaught[itr];

        TMatrixD g = part->GetG();                  // track weight matrix
        err = ComputeAandB(xk, *part, a, b, ck0);   // compute matrices of derivatives
        if (err)
            return err;

        // W = (Bt*G*B)'
        TMatrixD tmp(g, TMatrixD::kMult, b);
        TMatrixD w(b, TMatrixD::kTransposeMult, tmp);
        w.Invert();

        TMatrixD m = part->GetMeas();
        m -= ck0;   // m-ck0

        // qk = W*Bt*G*(m-ck0-A*xk)
        TMatrixD tmp21(a, TMatrixD::kMult, xk);
        tmp21 *= -1;
        tmp21 += m;   // m-ck0-A*xk
        TMatrixD tmp22(g, TMatrixD::kMult, tmp21);
        TMatrixD tmp23(b, TMatrixD::kTransposeMult, tmp22);
        TMatrixD qk(w, TMatrixD::kMult, tmp23);

        // Update momentum and last coordinates
        /*
        TMatrixD par = *track->GetParam();
        for (Int_t i = 0; i < 3; ++i) par(i+2,0) = qk(i,0);
        par(0,0) = rad * phi;
        par(1,0) = fVtx[2];
        track->SetParam(par);
        track->SetPosNew(rad);

        // Update track length
        Double_t dLeng = track1.GetLength(); // track length from DCA to last saved position
        track1.SetParam(par);
        track1.SetPos(rad);
        track1.SetLength(0.);
        if (track->GetNode() == "") fKF->PropagateParamR(&track1,&hit,kTRUE);
        else fKF->PropagateParamP(&track1,&hit,kTRUE,kTRUE);

        track->SetLength (track->GetLength() - dLeng + track1.GetLength());
        */

        // Save all matrices for further usage
        part->Setq(qk);
        part->Setx(xk);
        part->SetA(a);
        part->SetB(b);
        part->SetC(fCovar);
        part->SetW(w);

        // E = -C*At*G*B*W
        TMatrixD tmp31(b, TMatrixD::kMult, w);
        TMatrixD tmp32(g, TMatrixD::kMult, tmp31);
        TMatrixD tmp33(a, TMatrixD::kTransposeMult, tmp32);
        TMatrixD e(fCovar, TMatrixD::kMult, tmp33);

        // D = W + W*Bt*G*A*C*At*G*B*W = W + W*Bt*G*A*(-E)
        TMatrixD tmp41(a, TMatrixD::kMult, e);
        e *= -1.0;
        part->SetCovE(e);
        TMatrixD tmp42(g, TMatrixD::kMult, tmp41);
        TMatrixD tmp43(b, TMatrixD::kTransposeMult, tmp42);
        TMatrixD d(w, TMatrixD::kMult, tmp43);
        d += w;
        part->SetCovD(d);
    }   // for (Int_t itr = 0; itr < nDaught;

    return err;
}

//__________________________________________________________________________

Bool_t BmnMotherFitterPart::ComputeAandB(const TMatrixD& xk0,
                                         const BmnParticle& part,
                                         TMatrixD& a,
                                         TMatrixD& b,
                                         TMatrixD& ck0,
                                         Bool_t flag)
{
    /// Compute matrices of derivatives w.r.t. vertex coordinates and particle momentum

    if (part.GetCharge() == 0) {
        // Neutral particle
        return ComputeAB0(xk0, part, a, b, ck0, flag);
    }

    Double_t vert0[3];
    const Double_t* vert = xk0.GetMatrixArray();

    for (Int_t i = 0; i < 3; ++i)
        vert0[i] = vert[i];

    // Put track at xk0
    CbmKFTrack tr = part.GetKFTrack();
    tr.Extrapolate(vert0[2]);
    tr.GetTrack()[0] = vert0[0];
    tr.GetTrack()[1] = vert0[1];
    tr.GetTrack()[5] = vert0[2];
    CbmKFTrack tr0(tr);

    // Propagate track to zhit
    // AZ-060725 Bool_t err = tr.Extrapolate(zhit);
    // Bool_t err = tr.Extrapolate (part.GetZ());
    Bool_t err = tr.Propagate(part.GetZ0());
    // if (err) continue;
    if (err)
        return err;

    // Double_t shift = 0.01; // 100 um coordinate shift
    Double_t shift = 0.1;   // 1 mm coordinate shift

    for (Int_t i = 0; i < 3; ++i) {
        CbmKFTrack tr1(tr0);
        vert0[i] += shift;
        if (i > 0)
            vert0[i - 1] -= shift;
        tr1.GetTrack()[0] = vert0[0];
        tr1.GetTrack()[1] = vert0[1];
        tr1.GetTrack()[5] = vert0[2];

        // Propagate track to zhit
        // AZ-060725 err = tr1.Extrapolate(zhit);
        // err = tr1.Extrapolate (part.GetZ());
        err = tr1.Propagate(part.GetZ0());
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
        CbmKFTrack tr1(tr0);
        Int_t j = i + 2;
        // shift                 = (*track->GetCovariance())(j, j);
        int indx = CbmKFMath::indexS(j, j);
        // AZ-060725 shift = track.GetCovMatrix()[indx];
        shift = tr0.GetCovMatrix()[indx];
        // AZ-010126 shift = TMath::Sqrt(shift);
        shift = TMath::Sqrt(TMath::Abs(shift));   // AZ-010126
        if (j == 4)
            // AZ-070725 shift *= TMath::Sign(1., -tr0.GetTrack()[j]);   // 1/p
            shift *= TMath::Sign(1., tr0.GetTrack()[j]);   // 1/p
        // track1.SetParam(j, track0.GetParamNew(j) + shift);
        tr1.GetTrack()[j] += shift;

        // Propagate track to zhit
        // AZ-060725 err = tr1.Extrapolate(zhit);
        // err = tr1.Extrapolate(part.GetZ());
        err = tr1.Propagate(part.GetZ0());
        // if (err) continue;
        if (err)
            return err;

        // Derivatives
        for (Int_t k = 0; k < 5; ++k) {
            // b(k, i) = (track1.GetParamNew(k) - trackk.GetParamNew(k)) / shift;
            b(k, i) = (tr1.GetTrack()[k] - tr.GetTrack()[k]) / shift;
        }
    }

    if (!flag)
        return kFALSE;

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

Bool_t BmnMotherFitterPart::ComputeAB0(const TMatrixD& xk0,
                                       const BmnParticle& part,
                                       TMatrixD& a,
                                       TMatrixD& b,
                                       TMatrixD& ck0,
                                       Bool_t flag)
{
    /// Compute matrices of derivatives w.r.t. vertex coordinates and particle momentum
    /// (for neutrals)

    // AZ-130725 Made as for the charged particles

    Double_t vert0[3];
    const Double_t* vert = xk0.GetMatrixArray();

    for (Int_t i = 0; i < 3; ++i)
        vert0[i] = vert[i];

    // Put track at xk0
    CbmKFTrack tr = part.GetKFTrack();
    tr.Extrapolate(vert0[2]);
    tr.GetTrack()[0] = vert0[0];
    tr.GetTrack()[1] = vert0[1];
    tr.GetTrack()[5] = vert0[2];
    CbmKFTrack tr0(tr);

    // Propagate track to zhit
    // AZ-060725 Bool_t err = tr.Extrapolate(zhit);
    // Bool_t err = tr.Extrapolate (part.GetZ());
    Bool_t err = tr.Propagate(part.GetZ0());
    // if (err) continue;
    if (err)
        return err;

    // Double_t shift = 0.01; // 100 um coordinate shift
    Double_t shift = 0.1;   // 1 mm coordinate shift

    for (Int_t i = 0; i < 3; ++i) {
        CbmKFTrack tr1(tr0);
        vert0[i] += shift;
        if (i > 0)
            vert0[i - 1] -= shift;
        tr1.GetTrack()[0] = vert0[0];
        tr1.GetTrack()[1] = vert0[1];
        tr1.GetTrack()[5] = vert0[2];

        // Propagate track to zhit
        // AZ-060725 err = tr1.Extrapolate(zhit);
        // err = tr1.Extrapolate (part.GetZ());
        err = tr1.Propagate(part.GetZ0());
        // if (err) continue;
        if (err)
            return err;

        // Derivatives
        for (Int_t j = 0; j < 5; ++j) {
            // a(j, i) = (track1.GetParamNew(j) - trackk.GetParamNew(j)) / shift;
            a(j, i) = (tr1.GetTrack()[j] - tr.GetTrack()[j]) / shift;
        }
    }

    b = 0.0;   // AZ-150725

    // AZ-150725 for (Int_t i = 0; i < 3; ++i) {
    for (Int_t i = 0; i < 2; ++i) {   // !!! AZ-150725 - d/dp can't be computed via this scheme
        CbmKFTrack tr1(tr0);
        Int_t j = i + 2;
        /*
            // shift                 = (*track->GetCovariance())(j, j);
            int indx = CbmKFMath::indexS(j, j);
            // AZ-060725 shift = track.GetCovMatrix()[indx];
            shift = tr0.GetCovMatrix()[indx];
            shift = TMath::Sqrt(shift);
        */
        // shift = TMath::Max (0.01, tr1.GetTrack()[j] * 0.05); // AZ-130725
        // AZ-080825 shift = TMath::Max(0.05, tr1.GetTrack()[j] * 0.10);   // AZ-130725
        shift = TMath::Max(0.05, TMath::Abs(tr1.GetTrack()[j]) * 0.10);   // AZ-080825
        shift *= TMath::Sign(1.0, -tr1.GetTrack()[j]);                    // AZ-080825
        tr1.GetTrack()[j] += shift;

        // Propagate track to zhit
        // AZ-060725 err = tr1.Extrapolate(zhit);
        // err = tr1.Extrapolate(part.GetZ());
        err = tr1.Propagate(part.GetZ0());
        // if (err) continue;
        if (err)
            return err;

        // Derivatives
        for (Int_t k = 0; k < 5; ++k) {
            // b(k, i) = (track1.GetParamNew(k) - trackk.GetParamNew(k)) / shift;
            b(k, i) = (tr1.GetTrack()[k] - tr.GetTrack()[k]) / shift;
        }
    }
    b(4, 2) = 1.0;   // dp / dp

    if (!flag)
        return kFALSE;

    // ck0
    /* AZ-130725
    ck0(0, 0) = r * sinksi;
    // ck0(1,0) = xk0(2,0) - r * cosksi / tanth;
    ck0(1, 0) = xk0(2, 0) - r * cosksi * coTan;
    ck0(2, 0) = part.GetMeas(2);
    ck0(3, 0) = part.GetMeas(3);
    ck0(4, 0) = part.GetMeas(4);

    TMatrixD qk0(3, 1);
    qk0(0, 0) = part.GetMeas(2);
    qk0(1, 0) = part.GetMeas(3);
    qk0(2, 0) = part.GetMeas(4);
    ck0 -= TMatrixD(a, TMatrixD::kMult, xk0);
    ck0 -= TMatrixD(b, TMatrixD::kMult, qk0);
    */
    TMatrixD qk0(3, 1);
    // for (Int_t i = 0; i < 3; ++i) qk0(i, 0) = track0.GetParamNew(i + 2);
    for (Int_t i = 0; i < 3; ++i)
        // qk0(i, 0) = tr0.GetTrack()[i + 2];
        qk0(i, 0) = part.GetMeas(i + 2);
    // qk0.Print();
    // ck0 = *trackk.GetParamNew();
    // for (int i = 0; i < 5; ++i)
    for (int i = 2; i < 5; ++i)
        ck0(i, 0) = part.GetMeas(i);
    ck0(0, 0) = tr.GetTrack()[0];   // AZ-150725
    ck0(1, 0) = tr.GetTrack()[1];   // AZ-150725
    ck0 -= TMatrixD(a, TMatrixD::kMult, xk0);
    ck0 -= TMatrixD(b, TMatrixD::kMult, qk0);
    return kFALSE;
}

//__________________________________________________________________________

TMatrixD BmnMotherFitterPart::ComputeQmatr(vector<BmnParticle*> vDaught)
{
    /// Compute matrix Q = covariance cov(qk,qj)
    /// Qkj = Wk*Bkt*Gk*Ak*C*Ajt*Gj*Bj*Wj = Wk*Bkt*Gk*Ak*(-E), k!=j
    /// Qii = D

    TMatrixD qc(3, 3), qn(3, 3), qnc(3, 3);
    Int_t nDaught = vDaught.size();

    // Correlation between neutral-neutral or charged-charged

    for (Int_t i = 0; i < nDaught; ++i) {
        BmnParticle* part = vDaught[i];

        for (Int_t j = 0; j < nDaught; ++j) {
            BmnParticle* part1 = vDaught[j];
            if (part1->GetCharge() * part->GetCharge() == 0 && (part1->GetCharge() != 0 || part->GetCharge() != 0))
                continue;

            TMatrixD q(3, 3);
            if (j == i) {
                q = part->GetD();
            } else {
                TMatrixD tmp = part1->GetE();
                tmp *= -1.0;
                TMatrixD tmp1 = TMatrixD(part->GetA(), TMatrixD::kMult, tmp);
                TMatrixD tmp2 = TMatrixD(part->GetG(), TMatrixD::kMult, tmp1);
                TMatrixD tmp3 = TMatrixD(part->GetB(), TMatrixD::kTransposeMult, tmp2);
                q = TMatrixD(part->GetW(), TMatrixD::kMult, tmp3);
            }

            TMatrixD jt = TMatrixD(TMatrixD::kTransposed, part1->GetJ());
            TMatrixD tmp11 = TMatrixD(q, TMatrixD::kMult, jt);
            TMatrixD tmp12 = TMatrixD(part->GetJ(), TMatrixD::kMult, tmp11);
            if (part->GetCharge() && part1->GetCharge())
                qc += tmp12;
            else if (part->GetCharge() == 0 && part1->GetCharge() == 0)
                qn += tmp12;
        }
    }

    // Correlation between neutral and charged particles

    for (Int_t i = 0; i < nDaught; ++i) {
        BmnParticle* part = vDaught[i];
        if (part->GetCharge() != 0)
            continue;

        for (Int_t j = 0; j < nDaught; ++j) {
            BmnParticle* part1 = vDaught[j];
            if (part1->GetCharge() == 0)
                continue;

            TMatrixD q(3, 3);
            TMatrixD tmp = part1->GetE();
            tmp *= -1.0;
            TMatrixD tmp1 = TMatrixD(part->GetA(), TMatrixD::kMult, tmp);
            TMatrixD tmp2 = TMatrixD(part->GetG(), TMatrixD::kMult, tmp1);
            TMatrixD tmp3 = TMatrixD(part->GetB(), TMatrixD::kTransposeMult, tmp2);
            q = TMatrixD(part->GetW(), TMatrixD::kMult, tmp3);

            TMatrixD jt = TMatrixD(TMatrixD::kTransposed, part1->GetJ());
            TMatrixD tmp11 = TMatrixD(q, TMatrixD::kMult, jt);
            TMatrixD tmp12 = TMatrixD(part->GetJ(), TMatrixD::kMult, tmp11);
            qnc += tmp12;

            TMatrixD jjt = TMatrixD(TMatrixD::kTransposed, part->GetJ());
            TMatrixD tmp21 = TMatrixD(q, TMatrixD::kTransposeMult, jjt);
            TMatrixD tmp22 = TMatrixD(part1->GetJ(), TMatrixD::kMult, tmp21);
            qnc += tmp22;
        }
    }

    qc += qn;
    qc += qnc;
    return qc;
}

//__________________________________________________________________________
Double_t BmnMotherFitterPart::Chi2Vertex(BmnParticle* part, const CbmVertex* vtx)
{
    /// Compute Chi2 w.r.t. vertex

    Double_t vpos[3] = {vtx->GetX(), vtx->GetY(), vtx->GetZ()};
    TMatrixD xk(3, 1), xk0(3, 1), ck0(5, 1), a(5, 3), b(5, 3), c(3, 3);
    TMatrixFSym cov(3);
    cov.SetTol(1.e-10);   // AZ-311220

    xk0.SetMatrixArray(vpos);
    vtx->CovMatrix(cov);
    cov.Invert();

    int err = ComputeAandB(xk0, *part, a, b, ck0, kTRUE);   // compute matrices of derivatives
    if (err)
        return -9.0;
    TMatrixD g = part->GetG();

    // W = (Bt*G*B)'
    TMatrixD tmp(g, TMatrixD::kMult, b);
    TMatrixD w(b, TMatrixD::kTransposeMult, tmp);
    w.Invert();

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
    c += cov;
    c.Invert();

    // xk = Ck*((Ck-1)'x(k-1)+At*Gb*(m-ck0))
    TMatrixD m = part->GetMeas();
    m -= ck0;   // m-ck0

    TMatrixD tmp11(gb, TMatrixD::kMult, m);
    TMatrixD tmp12(a, TMatrixD::kTransposeMult, tmp11);
    TMatrixD tmp13(cov, TMatrixD::kMult, xk0);
    tmp13 += tmp12;
    xk = TMatrixD(c, TMatrixD::kMult, tmp13);

    // qk = W*Bt*G*(m-ck0-A*xk)
    TMatrixD tmp21(a, TMatrixD::kMult, xk);
    tmp21 *= -1;
    tmp21 += m;   // m-ck0-A*xk
    TMatrixD tmp22(g, TMatrixD::kMult, tmp21);
    TMatrixD tmp23(b, TMatrixD::kTransposeMult, tmp22);
    TMatrixD qk(w, TMatrixD::kMult, tmp23);

    // Residual m-ck0-A*xk-B*qk
    TMatrixD r = tmp21;
    TMatrixD tmp31(b, TMatrixD::kMult, qk);
    r -= tmp31;

    // Chi2 = rt*G*r + (xk-x(k-1))t*(Ck-1)'*(xk-x(k-1))
    TMatrixD tmp41(g, TMatrixD::kMult, r);
    TMatrixD chi21(r, TMatrixD::kTransposeMult, tmp41);
    TMatrixD dx = xk;
    dx -= xk0;
    TMatrixD tmp42(cov, TMatrixD::kMult, dx);
    TMatrixD chi22(dx, TMatrixD::kTransposeMult, tmp42);
    chi21 += chi22;

    return chi21(0, 0);
}

//__________________________________________________________________________

TVector3 BmnMotherFitterPart::Parabola(TVector3& pos0, TVector3& pos1, TVector3& pos2)
{
    // Get parabolic track approximation from 3 points (X vs Z)
    // y = a*x^2 + bx + c

    Double_t x[3] = {pos0[2], pos1[2], pos2[2]};
    Double_t y[3] = {pos0[0], pos1[0], pos2[0]};

    Double_t denom = (x[0] - x[1]) * (x[0] - x[2]) * (x[1] - x[2]);
    Double_t dy10 = y[1] - y[0];
    Double_t dy02 = y[0] - y[2];
    Double_t dy21 = y[2] - y[1];
    Double_t a = x[2] * dy10 + x[1] * dy02 + x[0] * dy21;
    a /= denom;
    Double_t b = -x[0] * x[0] * dy21 - x[2] * x[2] * dy10 - x[1] * x[1] * dy02;
    b /= denom;
    Double_t c = x[1] * x[1] * (x[2] * y[0] - x[0] * y[2]) + x[1] * (x[0] * x[0] * y[2] - x[2] * x[2] * y[0])
                 + x[0] * x[2] * (x[2] - x[0]) * y[1];
    c /= denom;

    return TVector3(c, b, a);
    // 𝐴=𝑥3(𝑦2−𝑦1)+𝑥2(𝑦1−𝑦3)+𝑥1(𝑦3−𝑦2) / (𝑥1−𝑥2)(𝑥1−𝑥3)(𝑥2−𝑥3)
    // 𝐵=𝑥21(𝑦2−𝑦3)+𝑥23(𝑦1−𝑦2)+𝑥22(𝑦3−𝑦1) / (𝑥1−𝑥2)(𝑥1−𝑥3)(𝑥2−𝑥3)
    // 𝐶=𝑥22(𝑥3𝑦1−𝑥1𝑦3)+𝑥2(𝑥21𝑦3−𝑥23𝑦1)+𝑥1𝑥3(𝑥3−𝑥1)𝑦2 / (𝑥1−𝑥2)(𝑥1−𝑥3)(𝑥2−𝑥3)
}

ClassImp(BmnMotherFitterPart);
