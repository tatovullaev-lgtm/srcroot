/// \class BmnParticle
///
/// Particle object in BM@N (to work with decays).
///
/// \author Alexander Zinchenko (LHEP, JINR, Dubna)

#include "BmnParticle.h"
// #include "MpdHelix.h"
// #include "MpdKalmanFilter.h"
// #include "MpdKalmanHit.h"
// #include "MpdKalmanTrack.h"
#include "BmnMotherFitterPart.h"
#include "CbmKF.h"
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"

#include <TDatabasePDG.h>
#include <TMatrixDSym.h>

//__________________________________________________________________________
BmnParticle::BmnParticle()
    : TObject()
    , fIndx(-1)
    , fCharge(0)
    , fMass(TDatabasePDG::Instance()->GetParticle("pi+")->Mass())
    , fChi2(0.0)
    , fChi2ver(-1.0)
    , fFlag(0)
    , fPoint00(kTRUE)
    , fZ(0.0)
    , fNDF(2)
{
    /// Default constructor

    // fKF = MpdKalmanFilter::Instance();
    fKF = CbmKF::Instance();
    fMeas.ResizeTo(5, 1);
    fq.ResizeTo(3, 1);
    fx.ResizeTo(3, 1);
    fJ.ResizeTo(3, 3);
    fJinv.ResizeTo(3, 3);
    fD.ResizeTo(3, 3);
    fE.ResizeTo(3, 3);
    // fQ.ResizeTo(3,3);
    fA.ResizeTo(5, 3);
    fB.ResizeTo(5, 3);
    fC.ResizeTo(3, 3);
    fG.ResizeTo(5, 5);
    fW.ResizeTo(3, 3);

    fDaughtersInds.clear();
    /* AZ-020725
    Double_t pos[3] = {0.0}, magf[3] = {0.0};
    fKF->GetField(pos, magf);
    fieldConst = 0.3 * 0.01 * magf[2] / 10;
    */
    fXY0[0] = fXY0[1] = 0.0;
    fG(0, 0) = fG(1, 1) = fG(2, 2) = fG(3, 3) = fG(4, 4) = 0.001;   // AZ-139725
}

//__________________________________________________________________________

BmnParticle::BmnParticle(CbmKFTrack& track, Int_t indx, Double_t mass, Double_t* orig)
    //    : TObject(track), fIndx(indx), fCharge(0),
    : TObject()
    , fIndx(indx)
    , fCharge(0)
    , fMass(mass)
    , fChi2(0.0)
    , fChi2ver(-1.0)
    , fFlag(0)
    , fPoint00(kTRUE)
    , fZ(0.0)
    , fNDF(2)
{
    /// Constructor from Kalman track

    fDaughtersInds.clear();
    AddDaughter(indx);
    Track2Part(track, kTRUE, orig);
}

//__________________________________________________________________________

BmnParticle::BmnParticle(CbmStsTrack& track, Int_t indx, Double_t mass, Double_t* orig)
    : TObject(track)
    , fIndx(indx)
    , fCharge(0)
    , fMass(mass)
    , fChi2(0.0)
    , fChi2ver(-1.0)
    , fFlag(0)
    , fPoint00(kTRUE)
    , fNDF(2)
{
    /// Constructor from STS track

    fDaughtersInds.clear();
    AddDaughter(indx);
    CbmKFTrack kftr(track);
    Track2Part(kftr, kTRUE, orig);
}

//__________________________________________________________________________
BmnParticle::BmnParticle(const BmnParticle& part)
    : TObject(part)
    , fIndx(part.fIndx)
    , fPdg(part.fPdg)
    , fCharge(part.fCharge)
    , fMass(part.fMass)
    , fieldConst(part.fieldConst)
    , fMeas(part.fMeas)
    , fq(part.fq)
    , fx(part.fx)
    , fJ(part.fJ)
    , fJinv(part.fJinv)
    , fD(part.fD)
    , fE(part.fE)
    , fA(part.fA)
    , fB(part.fB)
    , fC(part.fC)
    , fG(part.fG)
    , fW(part.fW)
    , fChi2(part.fChi2)
    , fChi2ver(part.fChi2ver)
    , fFlag(part.fFlag)
    , fPoint00(part.fPoint00)
    , fZ(part.fZ)
    , fNDF(part.fNDF)
{
    /// copy constructor

    fXY0[0] = part.fXY0[0];
    fXY0[1] = part.fXY0[1];
    fDaughtersInds = part.fDaughtersInds;

    fKF = part.fKF;
}

//__________________________________________________________________________
BmnParticle& BmnParticle::operator=(const BmnParticle& part)
{
    /// Asignment operator

    // check assignement to self
    if (this == &part)
        return *this;

    // base class assignement
    TObject::operator=(part);

    fIndx = part.fIndx;
    fPdg = part.fPdg;
    fCharge = part.fCharge;
    fMass = part.fMass;
    fieldConst = part.fieldConst;
    fMeas = part.fMeas;
    fq = part.fq;
    fx = part.fx;
    fJ = part.fJ;
    fJinv = part.fJinv;
    fD = part.fD;
    fE = part.fE;
    fA = part.fA;
    fB = part.fB;
    fC = part.fC;
    fG = part.fG;
    fW = part.fW;
    fChi2 = part.fChi2;
    fChi2ver = part.fChi2ver;
    fFlag = part.fFlag;
    fKF = part.fKF;
    fNDF = part.fNDF;
    fZ = part.fZ;

    fXY0[0] = part.fXY0[0];
    fXY0[1] = part.fXY0[1];
    fDaughtersInds = part.fDaughtersInds;

    return *this;
}

//__________________________________________________________________________
BmnParticle::~BmnParticle()
{
    /// Destructor
}

/*
//__________________________________________________________________________
Int_t MpdParticle::Compare(const TObject* part) const
{
/// "Compare" function to sort in descending order in pt

  MpdKalmanTrack *trackKF = (MpdKalmanTrack*) track;
  Double_t pt = 1. / TMath::Abs(trackKF->GetParam(4));
  Double_t ptthis = 1. / TMath::Abs((*fParam)(4,0));
  if (ptthis < pt) return 1;
  else if (ptthis > pt) return -1;
  return 0;
}
*/
//__________________________________________________________________________
void BmnParticle::SetMass(Double_t mass)
{
    /// Set particle mass (if negative, use PDG table value)

    fMass = (mass > -1.0) ? mass : TDatabasePDG::Instance()->GetParticle(fPdg)->Mass();
}

//__________________________________________________________________________
void BmnParticle::Track2Part(CbmKFTrack& track, Bool_t setWeight, Double_t* orig)
{
    /// Fill BmnParticle from KFTrack at Z = 0

    // fKF = MpdKalmanFilter::Instance();
    fKF = CbmKF::Instance();
    fCharge = TMath::Sign(1.5, track.GetTrack()[4]);   // Qp
    fMeas.ResizeTo(5, 1);
    fq.ResizeTo(3, 1);
    fx.ResizeTo(3, 1);
    fJ.ResizeTo(3, 3);
    fJinv.ResizeTo(3, 3);
    fD.ResizeTo(3, 3);
    fE.ResizeTo(3, 3);
    // fQ.ResizeTo(3,3);
    fA.ResizeTo(5, 3);
    fB.ResizeTo(5, 3);
    fC.ResizeTo(3, 3);
    fG.ResizeTo(5, 5);
    fW.ResizeTo(3, 3);

    /* AZ-020725
       Double_t pos[3] = {0.0}, magf[3] = {0.0};
       fKF->GetField(pos, magf);
       fieldConst = 0.3 * 0.01 * magf[2] / 10;
    */
    fXY0[0] = fXY0[1] = 0.0;
    // AZ if (orig) fPoint00 = kFALSE;
    fXY0[0] = track.GetTrack()[5];   // AZ-031225 - Z-ccordinate of the first hit

    Double_t vert[3] = {0};
    if (orig == NULL)
        orig = vert;

    CbmKFTrack kftr(track);
    // FindPca (kftr, orig);
    kftr.Extrapolate(0.0);   // extrapolate to Z=0 - AZ-070725
    // kftr.Propagate(0.0); // extrapolate to Z=0
    FairTrackParam param;
    kftr.GetTrackParam(param);
    fMeas(0, 0) = param.GetX();    // X
    fMeas(1, 0) = param.GetY();    // Y
    fMeas(2, 0) = param.GetTx();   // Tx
    fMeas(3, 0) = param.GetTy();   // Ty
    fMeas(4, 0) = param.GetQp();   // 1/p
    fZ = param.GetZ();             // Z

    if (!setWeight)
        return;   // skip all the rest

    fq(0, 0) = fMeas(2, 0);
    fq(1, 0) = fMeas(3, 0);
    fq(2, 0) = fMeas(4, 0);

    TMatrixDSym covar(5);

    for (int i = 0; i < 5; ++i) {
        covar(i, i) = param.GetCovariance(i, i);

        for (int j = i + 1; j < 5; ++j)
            covar(i, j) = covar(j, i) = param.GetCovariance(i, j);
    }
    covar.Invert();
    TMatrixD weight(5, 5);
    weight.SetMatrixArray(covar.GetMatrixArray());

    SetG(weight);
}

//__________________________________________________________________________

// void BmnParticle::FindPca(CbmKFTrackInterface& tr, Double_t* vert)
void BmnParticle::FindPca(CbmKFTrack& tr, Double_t* vert)
{
    // Find track 2-D PCA w.r.t. vert using parabolic approximation

    // Make 2 iterations
    Double_t zpca = vert[2], dz = 5.0;

    for (int iter = 0; iter < 2; ++iter) {
        TVector3 pos[3];
        if (iter > 0)
            dz = 2.0;

        for (int i = -1; i < 2; ++i) {
            Double_t z = zpca + dz * i;
            tr.Extrapolate(z);
            Double_t* pars = tr.GetTrack();
            Double_t dx = pars[0] - vert[0], dy = pars[1] - vert[1];   //, dz = pars[5] - vert[2];
            // Double_t dist = TMath::Sqrt(dx * dx + dy * dy + dz * dz);
            Double_t dist = TMath::Sqrt(dx * dx + dy * dy);
            pos[i + 1].SetX(dist);
            pos[i + 1].SetZ(z);
        }
        TVector3 cba = Parabola(pos[0], pos[1], pos[2]);
        zpca = -cba[1] / cba[2] / 2;
    }
    tr.Extrapolate(zpca);
}

//__________________________________________________________________________

TVector3 BmnParticle::Parabola(TVector3& pos0, TVector3& pos1, TVector3& pos2)
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

//__________________________________________________________________________

// void BmnParticle::WeightAtDca(CbmStsTrack &tr, Double_t *vert)
void BmnParticle::WeightAtDca(CbmKFTrack& tr, Double_t* vert)
{
    // Obtain BmnParticle weight at DCA from CbmStsTrack weight

    /*
    FairTrackParam par0;
    tr.GetTrackParam (par0);
    Double_t pars[5] = { par0.GetX(), par0.GetY(), par0.GetTx(), par0.GetTy(), par0.GetQp() };

    TMatrixDSym covar;
    //covar.SetMatrixArray (par0.GetCovMatrix());

    //TMatrixDSym *covar  = tr.Weight2Cov(); // get covariance matrix
    TMatrixD     g      = *tr.GetWeight(); // track weight matrix
    TMatrixD     meas0  = GetMeas();
    //TMatrixD     param0 = *tr.GetParamNew();
    TMatrixD     param0 (5, 1, pars);
    TMatrixD jacob (5, 5);
    // Double_t vert[3] = {0.0};

    Double_t dPar;
    // Loop over parameters to find change of the propagated vs initial ones
    Bool_t ok = kTRUE;

   for (Int_t i = 0; i < 5; ++i) {
      dPar = TMath::Sqrt (covar(i, i));
      if (i < 4)
         dPar = TMath::Min(dPar, 0.1);
      else
         dPar = TMath::Min(dPar, 0.1 * TMath::Abs(param0(4, 0)));

      tr.SetParam(param0);
      if (i == 4) dPar *= TMath::Sign(1., -param0(4, 0)); // 1/p
      // else if (i == 2) dPar *= sign;
      else if (i == 3)
         dPar *= TMath::Sign(1., -param0(3, 0)); // dip-angle
      tr.SetParam(i, param0(i, 0) + dPar);
      ok = fKF->FindPca(&tr, vert);
      tr.SetParam(*tr.GetParamNew());
      BmnParticle tmpPart;
      tmpPart.Track2Part(tr, kFALSE, vert);

      Double_t meas = 0.0;
      for (Int_t j = 0; j < 5; ++j) {
         if (j == 2)
            meas = fKF->Proxim(meas0(j, 0), tmpPart.GetMeas(j));
         else
            meas = tmpPart.GetMeas(j);
         jacob(j, i) = (meas - meas0(j, 0)) / dPar;
         // cout << i << " " << j << " " << dPar << " " << track->GetParamNew(j) << " " << paramNew0(j,0)
         //    << " " << track->GetPos() << " " << track->GetPosNew() << " " << jacob(j,i) << endl;
      }
   }

   // jacob.Print();
   jacob.Invert();

   TMatrixD tmp (g, TMatrixD::kMult, jacob);                // WD
   TMatrixD weight1 (jacob, TMatrixD::kTransposeMult, tmp); // DtWD

   SetG (weight1);
   */
}

//__________________________________________________________________________
Double_t BmnParticle::BuildMother(vector<BmnParticle*>& vDaught)
{
    /// Build mother particle from daughters which were smoothed
    /// according to the decay vertex constraint (after FindVertex)

    return BmnMotherFitterPart::Instance()->BuildMother(this, vDaught);
}

//__________________________________________________________________________

Double_t BmnParticle::BuildMother(vector<CbmStsTrack*>& vTracks, vector<BmnParticle*>& vDaught)
{
    /// Build mother particle from daughters which were smoothed
    /// according to the decay vertex constraint (after FindVertex).
    /// Daughters are built from tracks and parametrized at their
    /// intersection point.

    // AZ-130725 return BmnMotherFitterPart::Instance()->BuildMother(this, vTracks, vDaught);
    return kTRUE;
}

//__________________________________________________________________________

void BmnParticle::FillJ()
{
    /// Fill Jacobian matrix fJ

    Double_t p0 = Momentum();
    Double_t tx = fq(0, 0);
    Double_t ty = fq(1, 0);
    Double_t denom = TMath::Sqrt(tx * tx + ty * ty + 1);
    Double_t denom2 = denom * denom;
    Double_t pz = p0 / denom;
    fJ = 0.0;

    if (fCharge) {
        // Charged particle
        // d/d(tx)
        fJ(0, 0) = pz - pz * tx * tx / denom2;
        fJ(1, 0) = -pz * tx * ty / denom2;
        fJ(2, 0) = -pz * tx / denom2;
        // d/d(ty)
        fJ(0, 1) = -pz * tx * ty / denom2;
        fJ(1, 1) = pz - pz * ty * ty / denom2;
        fJ(2, 1) = -pz * ty / denom2;
        // d/d(q/p) - CHECK ME !!!
        // fJ(0, 2) = -tx * pz * p0;
        /* AZ-050825 fJ(0, 2) = -tx * pz * fq(2, 0);
       fJ(1, 2) = -ty * pz * fq(2, 0);
       fJ(2, 2) = -pz * fq(2, 0); */
        fJ(0, 2) = -tx * pz / fq(2, 0);
        fJ(1, 2) = -ty * pz / fq(2, 0);
        fJ(2, 2) = -pz / fq(2, 0);
    } else {
        // neutral
        // d/d(tx)
        fJ(0, 0) = pz - pz * tx * tx / denom2;
        fJ(1, 0) = -pz * tx * ty / denom2;
        fJ(2, 0) = -pz * tx / denom2;
        // d/d(ty)
        fJ(0, 1) = -pz * tx * ty / denom2;
        fJ(1, 1) = pz - pz * ty * ty / denom2;
        fJ(2, 1) = -pz * ty / denom2;
        // d/dp
        fJ(0, 2) = tx / denom;
        fJ(1, 2) = ty / denom;
        fJ(2, 2) = 1 / denom;
    }

    // AZ-120725 TMatrixD tmp(TMatrixD::kTransposed, fJ);
    // fJ = tmp;
}

//__________________________________________________________________________
void BmnParticle::FillJinv(TVector3& mom3)
{
    /// Fill Jacobian matrix fJinv

    Double_t px0 = mom3.X();
    Double_t py0 = mom3.Y();
    Double_t pz0 = mom3.Z();
    Double_t p0 = mom3.Mag();
    Double_t p03 = p0 * p0 * p0;
    Double_t pz02 = pz0 * pz0;
    fJinv = 0.0;

    if (fCharge) {
        // Charged mother - CHECK ME !!!
        // d/d(px)
        fJinv(0, 0) = 1 / pz0;
        fJinv(1, 0) = 0;
        fJinv(2, 0) = -fCharge * px0 / p03;
        // d/d(py)
        fJinv(0, 1) = 0;
        fJinv(1, 1) = 1 / pz0;
        fJinv(2, 1) = -fCharge * py0 / p03;
        // d/d(pz)
        fJinv(0, 2) = -px0 / pz02;
        fJinv(1, 2) = -py0 / pz02;
        fJinv(2, 2) = -fCharge * pz0 / p03;
    } else {
        // neutral mother
        // d/d(px)
        fJinv(0, 0) = 1 / pz0;
        fJinv(1, 0) = 0;
        fJinv(2, 0) = px0 / p0;
        // d/d(py)
        fJinv(0, 1) = 0;
        fJinv(1, 1) = 1 / pz0;
        fJinv(2, 1) = py0 / p0;
        // d/d(pz)
        fJinv(0, 2) = -px0 / pz02;
        fJinv(1, 2) = -py0 / pz02;
        fJinv(2, 2) = pz0 / p0;
    }

    // AZ-120725 TMatrixD tmp(TMatrixD::kTransposed, fJinv);
    // fJinv = tmp;
}

//__________________________________________________________________________
Double_t BmnParticle::Chi2Vertex(CbmVertex* vtx)
{
    /// Compute Chi2 w.r.t. vertex

    fChi2ver = BmnMotherFitterPart::Instance()->Chi2Vertex(this, vtx);
    return fChi2ver;
}

//__________________________________________________________________________
Double_t BmnParticle::Energy() const
{
    /// Return particle energy

    Double_t mom = Momentum();
    return TMath::Sqrt(mom * mom + fMass * fMass);
}

//__________________________________________________________________________
Double_t BmnParticle::Rapidity() const
{
    /// Return particle rapidity

    TVector3 mom3 = Momentum3();
    Double_t mom = Momentum();
    Double_t e = TMath::Sqrt(mom * mom + fMass * fMass);
    Double_t pz = mom3.Z();
    Double_t y = 0.5 * TMath::Log((e + pz) / (e - pz));
    return y;
}

//__________________________________________________________________________
void BmnParticle::Print()
{
    /// Print particle info
}

//__________________________________________________________________________

void BmnParticle::ParamsAtDca()
{
    /// Compute particle parameters at DCA (at Z == 0)

    TMatrixD meas(5, 1);

    meas(0, 0) = Getx()(0, 0);
    meas(1, 0) = Getx()(1, 0);
    meas(2, 0) = Getq()(0, 0);
    meas(3, 0) = Getq()(1, 0);
    meas(4, 0) = Getq()(2, 0);
    SetMeas(meas);
    fZ = Getx()(2, 0);
    //* AZ-130725
    CbmKFTrack tr = GetKFTrack();
    tr.Propagate(0.0);

    FairTrackParam par;
    tr.GetTrackParam(par);

    meas(0, 0) = par.GetX();
    meas(1, 0) = par.GetY();
    meas(2, 0) = par.GetTx();
    meas(3, 0) = par.GetTy();
    Double_t qp = (GetCharge()) ? par.GetQp() : Getq()(2, 0);
    meas(4, 0) = qp;
    SetMeas(meas);
    fZ = par.GetZ();
    //*/

    // AZ-090825 - set cov. matrix
    TMatrixDSym covar(5);

    for (int i = 0; i < 5; ++i) {
        covar(i, i) = par.GetCovariance(i, i);

        for (int j = i + 1; j < 5; ++j)
            covar(i, j) = covar(j, i) = par.GetCovariance(i, j);
    }
    covar.Invert();
    TMatrixD weight(5, 5);
    weight.SetMatrixArray(covar.GetMatrixArray());

    SetG(weight);
}

//__________________________________________________________________________

CbmKFTrack BmnParticle::GetKFTrack() const
{
    /// Create CbmKFTrack

    TMatrixFSym covMatr(5);

    for (int i = 0; i < 5; ++i) {
        covMatr(i, i) = fG(i, i);

        for (int j = i + 1; j < 5; ++j) {
            covMatr(i, j) = covMatr(j, i) = fG(i, j);
        }
    }
    covMatr.Invert();
    Double_t qp = GetMeas(4);
    if (GetCharge() == 0)
        qp = 0.0;   // !!! AZ-090725 - this is for CbmKFTrack !!!
    FairTrackParam par(GetMeas(0), GetMeas(1), fZ, GetMeas(2), GetMeas(3), qp, covMatr);
    CbmKFTrack tr(par);
    return tr;
}
ClassImp(BmnParticle);
