// -------------------------------------------------------------------------
// -----                 BmnStsVectorFinder header file                -----
// -----  Created 3/12/21  by A.Zinchenko, D.Zinchenko, R.Zinchenko    -----
// -------------------------------------------------------------------------

/** BmnStsVectorFinder.h
 *@author A.Zinchenko <Alexander.Zinchenko@jinr.ru>
 **
 ** Vector Finder in BM@N tracker
 **/

#ifndef BMNSTSVECTORFINDER_H
#define BMNSTSVECTORFINDER_H 1

#include "BmnStsKFTrackFitter.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"

#include "FairTask.h"

#include <TClonesArray.h>
#include <TMVA/Reader.h> //AZ-300922
#include <TProfile2D.h>
#include <TVector3.h>

#include <set>
#include <string>
#include <unordered_map>

//typedef std::pair<Double_t,std::map<Int_t,Int_t> > candvec;

class BmnStsVectorFinder : public FairTask
{

 public:

  struct hitinfo {
    hitinfo() {}
    hitinfo (TVector3 v3, Double_t phxi, Double_t txi, Double_t tyi) : xyz(v3), phx(phxi), tx(txi), ty(tyi), used(0) {;}
    TVector3 xyz;
    Double_t phx;
    Double_t tx;
    Double_t ty;
    int used;
  };

  struct candvec {
    Int_t nskips;
    Int_t idmaxP;
    Double_t momxz;
    Double_t ty;
    Double_t lengxz;
    //Double_t varx;
    Double_t tx;
    std::map<Int_t,Int_t> stahit;
    std::string code;
    FairTrackParam param;
    Double_t chi2;
    candvec() {}
    candvec (Double_t momi, std::map<Int_t,Int_t> stahi) : nskips(0), momxz(momi), stahit(stahi) {;}
  };
  
  /** Default constructor **/  
  BmnStsVectorFinder();

  /** Destructor **/
  ~BmnStsVectorFinder();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  /** Virtual method FinishEvent **/
  virtual void FinishEvent();
  
  /** Virtual method Finish **/
  virtual void Finish();

  void SetMatBudgetFileName (TString s) { fMatBudgetFileName = s; }
  void SetNofSiStations(int nsi) { fNSi = nsi; } //AZ-051022

 private:

  //void ReadMatBudget(); 
  Int_t ExcludeHits(Int_t minHits, TClonesArray *trArray);
  std::set<Int_t> GetHitId(CbmStsHit *hit, Int_t &idmaxP);
  std::set<Int_t> GetHitIdBmn(CbmStsHit *hit, Int_t &idmaxP);
  std::set<Int_t> GetHitId(Int_t hitInd, Int_t &idmaxP) {
    return GetHitId((CbmStsHit*)fHitArray->UncheckedAt(hitInd), idmaxP); }
  void BuildTrackCand();
  void BuildDoublets();
  Bool_t CheckVarx(Double_t dx, Double_t dz, Double_t tx, Double_t distxz, Double_t& varx); //AZ-310722
  void BuildTriplets();
  void BuildTracks();
  void ExtendTrack(candvec cand);
  Double_t FitTrack(candvec &cand);
  Double_t FilterHit(candvec &cand, CbmStsTrack &track);
  void MakeStsTrack(candvec &cand, std::string &hitcode, CbmStsTrack &track);
  void ExtendTracks(Int_t ista);
  TVector3 Parabola(TVector3& pos0, TVector3& pos1, TVector3& pos2);
  void FitTracks();
  void RemoveDoubles();
  Bool_t AreTracksDoubles(CbmStsTrack &tr1, CbmStsTrack &tr2);
  void RemoveFakes();
  void ExcludeFakes();
  Double_t DxVsMom(Int_t ista, candvec &aaa);
  Double_t Proxim(Double_t ang1, Double_t ang2);
  //AZ-280722 Double_t LinearFit(CbmStsTrack *tr, Int_t newtr, Double_t &ty);
  Double_t LinearFit(candvec &cand, candvec &cand2, CbmStsTrack *tr, Int_t newtr, Double_t &ty); //AZ-280722
  Double_t Curv3(candvec &cand1, candvec &cand2, candvec &cand3, int newtr3);
  std::set<int> KalmanWindow(candvec &cand, int hitIndx);
  std::string MakeCode(candvec &cand);
  void PrintHits(candvec &cand);
  void InitTMVA3(); //AZ-300922
  Float_t TMVAOutput3(candvec &aaa1, candvec &aaa2); //AZ-300922
  void InitTMVA2(); //AZ-221022
  Float_t TMVAOutput2(candvec &aaa1); //AZ-241022
   
  /** Input / output array of CbmStsHits **/
  TClonesArray *fClusArray[4], *fHitArray, *fTrackArray, *fDigiMatches, *fStsPoints, *fSilPoints;
  TClonesArray *fVectorArray; // output tracks
  Int_t fPass; // pass number
  Int_t fNsta; // number of stations
  Int_t fNbranches; // max number of branches in a triplet from one doublet
  Int_t fExact; // exact match - for debug
  Int_t fExactSel; // exact match for selected ID - for debug
  Int_t discarded; // number of discarded 3-hit candidates with bad chi2, debug
  int fOut; //AZ-210922 - flag for writing out
  TVector3 fXyzv; // mean vertex position
  Double_t fBy; // max. field in Tesla
  std::multimap<Double_t,Int_t> fmapPhx[19], fmapTy[19], fmapX[19], fmapY[19];
  std::vector<candvec> fSeedVec[19]; // 
  std::vector<candvec> fCandVec[19]; // 
  std::set<string> fCandCodes[19];
  //std::multimap<Int_t,candvec> fCandMap2[19]; // 
  //std::multimap<Int_t,candvec> fCandMap3[19]; // 

  std::vector<candvec> fCandVec2[19]; // doublets
  std::map<std::string,int> fMap2Out; //AZ-210922 - doublets for writing out
  std::map<std::string,int> *fMap2OutPtr; //AZ-210922 - doublets for writing out
  std::map<std::string,int> fMap3Out; //AZ-260922 - triplets for writing out
  std::map<std::string,int> *fMap3OutPtr; //AZ-260922 - triplets for writing out
  //AZ-130722 std::multimap<int,int> fMap2[19]; // hit index -> index in fCandVec2
  std::unordered_multimap<int,int> fMap2[19]; //AZ-130722 hit index -> index in fCandVec2

  std::vector<candvec> fCandVec3[19]; // triplets
  //AZ-130722 std::multimap<int,int> fMap3[19]; // hit index -> index in fCandVec3
  std::unordered_multimap<int,int> fMap3[19]; //AZ-130722 hit index -> index in fCandVec3
  //AZ-130722 std::map<std::string,int> fMapCode3[19]; // triplet code -> index in fCandVec3
  std::unordered_map<std::string,int> fMapCode3[19]; //AZ-130722 triplet code -> index in fCandVec3

  std::multimap<Double_t,CbmStsTrack> fTracks; // 
  //AZ-130722 std::map<Int_t,hitinfo> fmapHits;
  std::unordered_map<Int_t,hitinfo> fmapHits; //AZ-130722
  //AZ-130722 std::multimap<Int_t,Int_t> fHit2id, fClusMaps[2];
  std::unordered_multimap<Int_t,Int_t> fHit2id, fClusMaps[2]; //AZ-130722
  Int_t *fNhitsMin, fNskips[20];
   Double_t *fdTanX, *fdTanY, *fdTanY3, *fdTanXB0, *fPhiXZ, *fTanXmax, *fZmean, *fPTcut, *fCurvSta;
  //CbmStsKFTrackFitter fitter;
  BmnStsKFTrackFitter fitter;
  std::set<string> fCandSet[19];

  TString fMatBudgetFileName;
  //std::map<Double_t,TProfile2D*> fMatHistos;
  int fUseTMVA; //AZ-300922
  int fNSi; //AZ-051022
  Float_t fVarTMVA[20]; //AZ-300922
  TMVA::Reader *fReaderTMVA3; //AZ-300922
  TMVA::Reader *fReaderTMVA2; //AZ-221022
  
  BmnStsVectorFinder(const BmnStsVectorFinder&);
  BmnStsVectorFinder& operator=(const BmnStsVectorFinder&);

  ClassDef(BmnStsVectorFinder,0);

};

#endif