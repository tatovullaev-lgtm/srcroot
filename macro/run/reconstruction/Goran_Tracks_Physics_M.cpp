#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>
//#include <math.h>
//#include <TMath.h>
#include <vector>

#include <TROOT.h> 

#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TMinuit.h>
#include <TLatex.h>
#include <TRandom3.h> 
//#include <thread>
#include <algorithm>
#include <complex>
//#include "AtlasStyle.C"
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "TVectorD.h"


using namespace std;
using namespace TMath;

int sign(double val) {
  if (val>0) return 1;
  if (val==0) return 0;
  if (val<0) return -1;
}

using namespace ROOT::Math;

struct Inf {  

  vector <Double_t> *X=nullptr; 
  vector <Double_t> *Y=nullptr;
  vector <Int_t> *St=nullptr;
  vector <Int_t> *Mod=nullptr; 
  vector <Int_t> *L0H=nullptr; 
  vector <Int_t> *L1H=nullptr; 
  vector <Double_t> *AmpDir=nullptr; 
  vector <Double_t> *AmpIncl=nullptr; 
  int nHits;
  
};

struct Track {  

  vector <Double_t> *X=nullptr; 
  vector <Double_t> *Y=nullptr;
  vector <Double_t> *Z=nullptr;
  vector <Double_t> *Tx=nullptr;
  vector <Double_t> *Chi2X=nullptr; 
  vector <Double_t> *Ty=nullptr;
  vector <Double_t> *Chi2Y=nullptr;
  vector <Int_t> *IDX=nullptr; 
  vector <Int_t> *IDY=nullptr;
  vector <Int_t> *caseX=nullptr; 
  vector <Int_t> *caseY=nullptr;  
  vector <Double_t> *DX12=nullptr; 
  vector <Double_t> *DY12=nullptr; 
  vector <Double_t> *DX34=nullptr; 
  vector <Double_t> *DY34=nullptr; 
  int nTracks;
  int nTracksX;
  int nTracksY;
  int Flag;
  
  
};



void Goran_Tracks_Physics_M(TString infilename, TString outfilename) {

	printf("start read");
  
  
  //filenum=5;

  //TString filename ="/zfs/scratch/lhep-src/reconstruction_11_24/ArmTr_M/jan26/Run_" + to_string(filenum) + "_GTrack_Phys.root";
	TFile *treefile = new TFile(outfilename, "recreate");
	//output tree and its branches
	TTree *tree1 = new TTree("tree1","treelibrated tree");
	//tree1->SetDirectory(treefile);
  int event, run, isGoodRun;
  vector <double> *Pq_MDF_Flag=nullptr, *Pq_MDF_UpSt=nullptr, *Pq_MDF_X_UpSt=nullptr, *Pq_MDF_Y_UpSt=nullptr, *Pq_MDF_Z_UpSt=nullptr, *Tx_MDF_UpSt=nullptr, *Tx_AT=nullptr, *Ty_AT=nullptr;
  vector <int> *Fragment=nullptr;
  int Pq_MDF_UpSt_nTracks;
  double BC1, BC2, BC3, BC4, BC5;
  double BC1_Time0, BC1_Time1, BC2_Time0, BC2_Time1; 
  time_t TS_sec;
  long TS_nsec;
  int chargein, charge34, charge35, charge45, trigger, veto, SW_Mult, SW_NMatchTr, VB_Mult, Tof400_nHits;

  vector <Double_t> *SW_ELoss=nullptr;
  vector <Double_t> *SW_CellId=nullptr;
  vector <Double_t> *SW_Flag=nullptr;
  vector <Double_t> *SW_TrId=nullptr;
  vector <Double_t> *SW_OrderId=nullptr;

  vector <Int_t> *VB_CellId=nullptr;

  
  Int_t Is_laser; // new
  
  vector <double> *TrackL_X=nullptr, *TrackL_Y=nullptr, *TrackL_Z=nullptr, *TrackL_Tx=nullptr, *TrackL_Ty=nullptr, *TrackL_Flag=nullptr, *TrackR_X=nullptr, *TrackR_Y=nullptr, *TrackR_Z=nullptr, *TrackR_Tx=nullptr, *TrackR_Ty=nullptr, *TrackR_Flag=nullptr;
  vector <double> *TrackCandL_X1=nullptr, *TrackCandL_Y1=nullptr, *TrackCandL_Z1=nullptr, *TrackCandL_X2=nullptr, *TrackCandL_Y2=nullptr, *TrackCandL_Z2=nullptr, *TrackCandL_Flag=nullptr, *TrackCandR_X1=nullptr, *TrackCandR_Y1=nullptr, *TrackCandR_Z1=nullptr, *TrackCandR_X2=nullptr, *TrackCandR_Y2=nullptr, *TrackCandR_Z2=nullptr, *TrackCandR_Flag=nullptr;
  int nTracksL, nTracksR, nTrackCandL, nTrackCandR;


  int VertexXY_Mult;
  vector <double> *VertexZ=nullptr, *VertexX=nullptr, *VertexY=nullptr, *VertexX_match=nullptr, *VertexY_match=nullptr, *VertexXY_nTrMatched=nullptr, *VertexXY_Flag=nullptr; 
  vector <double> *beta_L=nullptr, *beta_R=nullptr; 
  vector <int> *is_good_beta_L=nullptr, *is_good_beta_R=nullptr;
  double C_betaX, C_betaY, C_betaZ;
  double OpAn_Th, OpAn_Phi, Mmiss2, Mmiss2_ex, s, t, u, min_tu, mp, mFr, m12C, m11B, m10B, m10Be;

  bool QE_cut, b_cut, vert_cut, OpAn_cut, Pmiss_cut, Emiss_cut, t400_cut;

  double PL_P, PL_Px, PL_Py, PL_Pz, PL_E;
  double PR_P, PR_Px, PR_Py, PR_Pz, PR_E;
  double PTarg_P, PTarg_Px, PTarg_Py, PTarg_Pz, PTarg_E;
  double PFr_P, PFr_Px, PFr_Py, PFr_Pz, PFr_E;
  double PC_P, PC_Px, PC_Py, PC_Pz, PC_E;

  double PL_lab_P, PL_lab_Px, PL_lab_Py, PL_lab_Pz, PL_lab_E;
  double PR_lab_P, PR_lab_Px, PR_lab_Py, PR_lab_Pz, PR_lab_E;
  double PTarg_lab_P, PTarg_lab_Px, PTarg_lab_Py, PTarg_lab_Pz, PTarg_lab_E;
  double PFr_lab_P, PFr_lab_Px, PFr_lab_Py, PFr_lab_Pz, PFr_lab_E;
  double PC_lab_P, PC_lab_Px, PC_lab_Py, PC_lab_Pz, PC_lab_E;

  double Pmiss_P, Pmiss_Px, Pmiss_Py, Pmiss_Pz, Pmiss_E;
  double Pmiss_ex_P, Pmiss_ex_Px, Pmiss_ex_Py, Pmiss_ex_Pz, Pmiss_ex_E;

  mp = 0.938272;
  m11B = 11.009305 * 0.931494;
  m10B = 10.012937 * 0.931494;
  m10Be = 10.013535 * 0.931494;
  m12C = 12 * 0.931494;

  Int_t dCSCH_sec, dCSCH_nanosec;
  Short_t dCSCH_cscN;
  vector <Float_t> *dCSCH_cscX=nullptr, *dCSCH_cscY=nullptr, *dCSCH_cscZ=nullptr;

  vector <double> *GemL_X=nullptr, *GemL_Y=nullptr, *GemL_Z=nullptr, *GemL_Tx=nullptr, *GemL_Ty=nullptr, *GemR_X=nullptr, *GemR_Y=nullptr, *GemR_Z=nullptr, *GemR_Tx=nullptr, *GemR_Ty=nullptr;
  vector <double> *CscL_X=nullptr, *CscL_Y=nullptr, *CscL_Z=nullptr, *CscL_Tx=nullptr, *CscL_Ty=nullptr, *CscR_X=nullptr, *CscR_Y=nullptr, *CscR_Z=nullptr, *CscR_Tx=nullptr, *CscR_Ty=nullptr;
  vector <double> *Tof400L_X=nullptr, *Tof400L_Y=nullptr, *Tof400L_Z=nullptr, *Tof400L_Tx=nullptr, *Tof400L_Ty=nullptr, *Tof400L_T=nullptr, *Tof400L_DetId=nullptr;
  vector <double> *Tof400R_X=nullptr, *Tof400R_Y=nullptr, *Tof400R_Z=nullptr, *Tof400R_Tx=nullptr, *Tof400R_Ty=nullptr, *Tof400R_T=nullptr, *Tof400R_DetId=nullptr;
  vector <double> *TofCalL_X=nullptr, *TofCalL_Y=nullptr, *TofCalL_Z=nullptr, *TofCalL_Tx=nullptr, *TofCalL_Ty=nullptr, *TofCalL_T=nullptr, *TofCalL_Bar=nullptr, *TofCalL_Layer=nullptr, *TofCalL_EU=nullptr, *TofCalL_ED=nullptr, *TofCalL_E=nullptr;
  vector <double> *TofCalR_X=nullptr, *TofCalR_Y=nullptr, *TofCalR_Z=nullptr, *TofCalR_Tx=nullptr, *TofCalR_Ty=nullptr, *TofCalR_T=nullptr, *TofCalR_Bar=nullptr, *TofCalR_Layer=nullptr, *TofCalR_EU=nullptr, *TofCalR_ED=nullptr, *TofCalR_E=nullptr;
  int GemL_nHits, GemR_nHits, CscL_nHits, CscR_nHits, Tof400L_nHits, Tof400R_nHits,TofCalL_nHits, TofCalR_nHits;


  TBranch *br_event =tree1->Branch("event",&event);
  TBranch *br_trigger =tree1->Branch("trigger",&trigger);
  TBranch *br_veto =tree1->Branch("VC",&veto);
  TBranch *br_run =tree1->Branch("run",&run);
  TBranch *br_isGoodRun =tree1->Branch("isGoodRun",&isGoodRun);
  TBranch *br_Is_laser = tree1->Branch("Is_laser", &Is_laser);
  TBranch *br_TS_sec =tree1->Branch("TS_sec",&TS_sec);
  TBranch *br_TS_nsec =tree1->Branch("TS_nsec",&TS_nsec);

  TBranch *br_BC1 =tree1->Branch("BC1",&BC1);
  TBranch *br_BC2 =tree1->Branch("BC2",&BC2);
  TBranch *br_BC3 =tree1->Branch("BC3",&BC3);
  TBranch *br_BC4 =tree1->Branch("BC4",&BC4);
  TBranch *br_BC5 =tree1->Branch("BC5",&BC5);
  TBranch *br_chargein =tree1->Branch("chargein",&chargein);
  TBranch *br_charge34 =tree1->Branch("charge34",&charge34);
  TBranch *br_charge35 =tree1->Branch("charge35",&charge35);
  TBranch *br_charge45 =tree1->Branch("charge45",&charge45); 
  TBranch *br_BC1_Time0 = tree1->Branch("BC1_Time0", &BC1_Time0); 
  TBranch *br_BC1_Time1 = tree1->Branch("BC1_Time1", &BC1_Time1); 
  TBranch *br_BC2_Time0 = tree1->Branch("BC2_Time0", &BC2_Time0); 
  TBranch *br_BC2_Time1 = tree1->Branch("BC2_Time1", &BC2_Time1); 
  TBranch *br_Pq_MDF_UpSt =tree1->Branch("Pq_MDF_UpSt",&Pq_MDF_UpSt);
  TBranch *br_Pq_MDF_X_UpSt =tree1->Branch("Pq_MDF_X_UpSt",&Pq_MDF_X_UpSt);
  TBranch *br_Pq_MDF_Y_UpSt =tree1->Branch("Pq_MDF_Y_UpSt",&Pq_MDF_Y_UpSt);
  TBranch *br_Pq_MDF_Z_UpSt =tree1->Branch("Pq_MDF_Z_UpSt",&Pq_MDF_Z_UpSt);
  TBranch *br_Pq_MDF_UpSt_nTracks =tree1->Branch("Pq_MDF_UpSt_nTracks",&Pq_MDF_UpSt_nTracks);
  TBranch *br_Tx_AT =tree1->Branch("Tx_AT",&Tx_AT);
  TBranch *br_Ty_AT =tree1->Branch("Ty_AT",&Ty_AT);
  TBranch *br_Pq_MDF_Flag =tree1->Branch("Pq_MDF_Flag",&Pq_MDF_Flag);
  TBranch *br_Tx_MDF_UpSt =tree1->Branch("Tx_MDF_UpSt",&Tx_MDF_UpSt);
  TBranch *br_Fragment =tree1->Branch("Fragment",&Fragment);

  TBranch *br_VertexX_match =tree1->Branch("VertexX_match",&VertexX_match); //dx && dy <0.5cm(~3sigma) in target center
  TBranch *br_VertexY_match =tree1->Branch("VertexY_match",&VertexY_match); //dx && dy <0.5cm(~3sigma) in target center
  TBranch *br_VertexXY_nTrMatched =tree1->Branch("VertexXY_nTrMatched",&VertexXY_nTrMatched); //number of matched tracks
  TBranch *br_VertexXY_Flag =tree1->Branch("VertexXY_Flag",&VertexXY_Flag);// 13 - 1st and 3rd tracks matched
  TBranch *br_VertexXY_Mult =tree1->Branch("VertexXY_Mult",&VertexXY_Mult); //to check how much multiple matched vertexes
  TBranch *br_VertexX =tree1->Branch("VertexX",&VertexX);
  TBranch *br_VertexY =tree1->Branch("VertexY",&VertexY);
  TBranch *br_VertexZ =tree1->Branch("VertexZ",&VertexZ);
  TBranch *br_beta_L =tree1->Branch("beta_L",&beta_L);
  TBranch *br_beta_R =tree1->Branch("beta_R",&beta_R);
  TBranch *br_is_good_beta_L =tree1->Branch("is_good_beta_L",&is_good_beta_L);
  TBranch *br_is_good_beta_R =tree1->Branch("is_good_beta_R",&is_good_beta_R);
  TBranch *br_C_betaX =tree1->Branch("C_betaX",&C_betaX);
  TBranch *br_C_betaY =tree1->Branch("C_betaY",&C_betaY);
  TBranch *br_C_betaZ =tree1->Branch("C_betaZ",&C_betaZ);

  TBranch *br_PL_P =tree1->Branch("PL_P",&PL_P);
  TBranch *br_PL_Px =tree1->Branch("PL_Px",&PL_Px);
  TBranch *br_PL_Py =tree1->Branch("PL_Py",&PL_Py);
  TBranch *br_PL_Pz =tree1->Branch("PL_Pz",&PL_Pz);
  TBranch *br_PL_E =tree1->Branch("PL_E",&PL_E);

  TBranch *br_PR_P =tree1->Branch("PR_P",&PR_P);
  TBranch *br_PR_Px =tree1->Branch("PR_Px",&PR_Px);
  TBranch *br_PR_Py =tree1->Branch("PR_Py",&PR_Py);
  TBranch *br_PR_Pz =tree1->Branch("PR_Pz",&PR_Pz);
  TBranch *br_PR_E =tree1->Branch("PR_E",&PR_E);

  TBranch *br_PTarg_P =tree1->Branch("PTarg_P",&PTarg_P);
  TBranch *br_PTarg_Px =tree1->Branch("PTarg_Px",&PTarg_Px);
  TBranch *br_PTarg_Py =tree1->Branch("PTarg_Py",&PTarg_Py);
  TBranch *br_PTarg_Pz =tree1->Branch("PTarg_Pz",&PTarg_Pz);
  TBranch *br_PTarg_E =tree1->Branch("PTarg_E",&PTarg_E);

  TBranch *br_PFr_P =tree1->Branch("PFr_P",&PFr_P);
  TBranch *br_PFr_Px =tree1->Branch("PFr_Px",&PFr_Px);
  TBranch *br_PFr_Py =tree1->Branch("PFr_Py",&PFr_Py);
  TBranch *br_PFr_Pz =tree1->Branch("PFr_Pz",&PFr_Pz);
  TBranch *br_PFr_E =tree1->Branch("PFr_E",&PFr_E);

  TBranch *br_PC_P =tree1->Branch("PC_P",&PC_P);
  TBranch *br_PC_Px =tree1->Branch("PC_Px",&PC_Px);
  TBranch *br_PC_Py =tree1->Branch("PC_Py",&PC_Py);
  TBranch *br_PC_Pz =tree1->Branch("PC_Pz",&PC_Pz);
  TBranch *br_PC_E =tree1->Branch("PC_E",&PC_E);

  TBranch *br_PL_lab_P =tree1->Branch("PL_lab_P",&PL_lab_P);
  TBranch *br_PL_lab_Px =tree1->Branch("PL_lab_Px",&PL_lab_Px);
  TBranch *br_PL_lab_Py =tree1->Branch("PL_lab_Py",&PL_lab_Py);
  TBranch *br_PL_lab_Pz =tree1->Branch("PL_lab_Pz",&PL_lab_Pz);
  TBranch *br_PL_lab_E =tree1->Branch("PL_lab_E",&PL_lab_E);

  TBranch *br_PR_lab_P =tree1->Branch("PR_lab_P",&PR_lab_P);
  TBranch *br_PR_lab_Px =tree1->Branch("PR_lab_Px",&PR_lab_Px);
  TBranch *br_PR_lab_Py =tree1->Branch("PR_lab_Py",&PR_lab_Py);
  TBranch *br_PR_lab_Pz =tree1->Branch("PR_lab_Pz",&PR_lab_Pz);
  TBranch *br_PR_lab_E =tree1->Branch("PR_lab_E",&PR_lab_E);

  TBranch *br_PTarg_lab_P =tree1->Branch("PTarg_lab_P",&PTarg_lab_P);
  TBranch *br_PTarg_lab_Px =tree1->Branch("PTarg_lab_Px",&PTarg_lab_Px);
  TBranch *br_PTarg_lab_Py =tree1->Branch("PTarg_lab_Py",&PTarg_lab_Py);
  TBranch *br_PTarg_lab_Pz =tree1->Branch("PTarg_lab_Pz",&PTarg_lab_Pz);
  TBranch *br_PTarg_lab_E =tree1->Branch("PTarg_lab_E",&PTarg_lab_E);

  TBranch *br_PFr_lab_P =tree1->Branch("PFr_lab_P",&PFr_lab_P);
  TBranch *br_PFr_lab_Px =tree1->Branch("PFr_lab_Px",&PFr_lab_Px);
  TBranch *br_PFr_lab_Py =tree1->Branch("PFr_lab_Py",&PFr_lab_Py);
  TBranch *br_PFr_lab_Pz =tree1->Branch("PFr_lab_Pz",&PFr_lab_Pz);
  TBranch *br_PFr_lab_E =tree1->Branch("PFr_lab_E",&PFr_lab_E);

  TBranch *br_PC_lab_P =tree1->Branch("PC_lab_P",&PC_lab_P);
  TBranch *br_PC_lab_Px =tree1->Branch("PC_lab_Px",&PC_lab_Px);
  TBranch *br_PC_lab_Py =tree1->Branch("PC_lab_Py",&PC_lab_Py);
  TBranch *br_PC_lab_Pz =tree1->Branch("PC_lab_Pz",&PC_lab_Pz);
  TBranch *br_PC_lab_E =tree1->Branch("PC_lab_E",&PC_lab_E);

   TBranch *br_Pmiss_P =tree1->Branch("Pmiss",&Pmiss_P);
  TBranch *br_Pmiss_Px =tree1->Branch("Pmiss_X",&Pmiss_Px);
  TBranch *br_Pmiss_Py =tree1->Branch("Pmiss_Y",&Pmiss_Py);
  TBranch *br_Pmiss_Pz =tree1->Branch("Pmiss_Z",&Pmiss_Pz);
  TBranch *br_Pmiss_E =tree1->Branch("Emiss",&Pmiss_E);
  
  TBranch *br_Pmiss_ex_P =tree1->Branch("Pmiss_ex",&Pmiss_ex_P);
  TBranch *br_Pmiss_ex_Px =tree1->Branch("Pmiss_ex_X",&Pmiss_ex_Px);
  TBranch *br_Pmiss_ex_Py =tree1->Branch("Pmiss_ex_Y",&Pmiss_ex_Py);
  TBranch *br_Pmiss_ex_Pz =tree1->Branch("Pmiss_ex_Z",&Pmiss_ex_Pz);
  TBranch *br_Pmiss_ex_E =tree1->Branch("Emiss_ex",&Pmiss_ex_E);

  TBranch *br_Mmiss2 =tree1->Branch("Mmiss2",&Mmiss2);
  TBranch *br_Mmiss2_ex =tree1->Branch("Mmiss2_ex",&Mmiss2_ex);
  TBranch *br_S =tree1->Branch("S",&s);
  TBranch *br_T =tree1->Branch("T",&t);
  TBranch *br_U =tree1->Branch("U",&u);
  TBranch *br_min_tu =tree1->Branch("min_TU",&min_tu);
  TBranch *br_OpAn_Th =tree1->Branch("OpAn_Th",&OpAn_Th);
  TBranch *br_OpAn_Phi =tree1->Branch("OpAn_Phi",&OpAn_Phi);
  TBranch *br_mp =tree1->Branch("mp",&mp);
  TBranch *br_mFr =tree1->Branch("mFr",&mFr);
  TBranch *br_m12C =tree1->Branch("m12C",&m12C);

  TBranch *br_QE_cut =tree1->Branch("QE_cut",&QE_cut);
  TBranch *br_b_cut =tree1->Branch("b_cut",&b_cut);
  TBranch *br_vert_cut =tree1->Branch("vert_cut",&vert_cut);
  TBranch *br_OpAn_cut =tree1->Branch("OpAn_cut",&OpAn_cut);
  TBranch *br_Pmiss_cut =tree1->Branch("Pmiss_cut",&Pmiss_cut);
  TBranch *br_Emiss_cut =tree1->Branch("Emiss_cut",&Emiss_cut);
  TBranch *br_t400_cut =tree1->Branch("t400_cut",&t400_cut);

  TBranch *br_TrackCandL_X1 =tree1->Branch("TrackCandL_X1",&TrackCandL_X1);
  TBranch *br_TrackCandL_Y1 =tree1->Branch("TrackCandL_Y1",&TrackCandL_Y1);
  TBranch *br_TrackCandL_Z1 =tree1->Branch("TrackCandL_Z1",&TrackCandL_Z1);
  TBranch *br_TrackCandL_X2 =tree1->Branch("TrackCandL_X2",&TrackCandL_X2);
  TBranch *br_TrackCandL_Y2 =tree1->Branch("TrackCandL_Y2",&TrackCandL_Y2);
  TBranch *br_TrackCandL_Z2 =tree1->Branch("TrackCandL_Z2",&TrackCandL_Z2);
  TBranch *br_TrackCandL_Flag =tree1->Branch("TrackCandL_Flag",&TrackCandL_Flag);
  TBranch *br_nTrackCandL =tree1->Branch("nTrackCandL",&nTrackCandL);

  TBranch *br_TrackCandR_X1 =tree1->Branch("TrackCandR_X1",&TrackCandR_X1);
  TBranch *br_TrackCandR_Y1 =tree1->Branch("TrackCandR_Y1",&TrackCandR_Y1);
  TBranch *br_TrackCandR_Z1 =tree1->Branch("TrackCandR_Z1",&TrackCandR_Z1);
  TBranch *br_TrackCandR_X2 =tree1->Branch("TrackCandR_X2",&TrackCandR_X2);
  TBranch *br_TrackCandR_Y2 =tree1->Branch("TrackCandR_Y2",&TrackCandR_Y2);
  TBranch *br_TrackCandR_Z2 =tree1->Branch("TrackCandR_Z2",&TrackCandR_Z2);
  TBranch *br_TrackCandR_Flag =tree1->Branch("TrackCandR_Flag",&TrackCandR_Flag);
  TBranch *br_nTrackCandR =tree1->Branch("nTrackCandR",&nTrackCandR);

  TBranch *br_TrackL_X =tree1->Branch("TrackL_X",&TrackL_X);
  TBranch *br_TrackL_Y =tree1->Branch("TrackL_Y",&TrackL_Y);
  TBranch *br_TrackL_Z =tree1->Branch("TrackL_Z",&TrackL_Z);
  TBranch *br_TrackL_Tx =tree1->Branch("TrackL_Tx",&TrackL_Tx);
  TBranch *br_TrackL_Ty =tree1->Branch("TrackL_Ty",&TrackL_Ty);
  TBranch *br_TrackL_Flag =tree1->Branch("TrackL_Flag",&TrackL_Flag);
  TBranch *br_nTracksL =tree1->Branch("nTracksL",&nTracksL);

  TBranch *br_TrackR_X =tree1->Branch("TrackR_X",&TrackR_X);
  TBranch *br_TrackR_Y =tree1->Branch("TrackR_Y",&TrackR_Y);
  TBranch *br_TrackR_Z =tree1->Branch("TrackR_Z",&TrackR_Z);
  TBranch *br_TrackR_Tx =tree1->Branch("TrackR_Tx",&TrackR_Tx);
  TBranch *br_TrackR_Ty =tree1->Branch("TrackR_Ty",&TrackR_Ty);
  TBranch *br_TrackR_Flag =tree1->Branch("TrackR_Flag",&TrackR_Flag);
  TBranch *br_nTracksR =tree1->Branch("nTracksR",&nTracksR);

  TBranch *br_SW_ELoss =tree1->Branch("SW_ELoss",&SW_ELoss);
  TBranch *br_SW_CellId =tree1->Branch("SW_CellId",&SW_CellId);
  TBranch *br_SW_Flag =tree1->Branch("SW_Flag",&SW_Flag);
  TBranch *br_SW_TrId =tree1->Branch("SW_TrId",&SW_TrId);
  TBranch *br_SW_OrderId =tree1->Branch("SW_OrderId",&SW_OrderId);
  TBranch *br_SW_Mult =tree1->Branch("SW_Mult",&SW_Mult);
  TBranch *br_SW_NMatchTr =tree1->Branch("SW_NMatchTr",&SW_NMatchTr);

  TBranch *br_VB_Mult =tree1->Branch("VB_Mult",&VB_Mult);
  TBranch *br_VB_CellId =tree1->Branch("VB_CellId",&VB_CellId);

  TBranch *br_GemL_X =tree1->Branch("GemL_X",&GemL_X);
  TBranch *br_GemL_Y =tree1->Branch("GemL_Y",&GemL_Y);
  TBranch *br_GemL_Z =tree1->Branch("GemL_Z",&GemL_Z);
  TBranch *br_GemL_Tx =tree1->Branch("GemL_Tx",&GemL_Tx);
  TBranch *br_GemL_Ty =tree1->Branch("GemL_Ty",&GemL_Ty);
  TBranch *br_GemL_nHits =tree1->Branch("GemL_nHits",&GemL_nHits);

  TBranch *br_GemR_X =tree1->Branch("GemR_X",&GemR_X);
  TBranch *br_GemR_Y =tree1->Branch("GemR_Y",&GemR_Y);
  TBranch *br_GemR_Z =tree1->Branch("GemR_Z",&GemR_Z);
  TBranch *br_GemR_Tx =tree1->Branch("GemR_Tx",&GemR_Tx);
  TBranch *br_GemR_Ty =tree1->Branch("GemR_Ty",&GemR_Ty);
  TBranch *br_GemR_nHits =tree1->Branch("GemR_nHits",&GemR_nHits);

  TBranch *br_CscL_X =tree1->Branch("CscL_X",&CscL_X);
  TBranch *br_CscL_Y =tree1->Branch("CscL_Y",&CscL_Y);
  TBranch *br_CscL_Z =tree1->Branch("CscL_Z",&CscL_Z);
  TBranch *br_CscL_Tx =tree1->Branch("CscL_Tx",&CscL_Tx);
  TBranch *br_CscL_Ty =tree1->Branch("CscL_Ty",&CscL_Ty);
  TBranch *br_CscL_nHits =tree1->Branch("CscL_nHits",&CscL_nHits);

  TBranch *br_CscR_X =tree1->Branch("CscR_X",&CscR_X);
  TBranch *br_CscR_Y =tree1->Branch("CscR_Y",&CscR_Y);
  TBranch *br_CscR_Z =tree1->Branch("CscR_Z",&CscR_Z);
  TBranch *br_CscR_Tx =tree1->Branch("CscR_Tx",&CscR_Tx);
  TBranch *br_CscR_Ty =tree1->Branch("CscR_Ty",&CscR_Ty);
  TBranch *br_CscR_nHits =tree1->Branch("CscR_nHits",&CscR_nHits);

  TBranch *br_Tof400L_X =tree1->Branch("Tof400L_X",&Tof400L_X);
  TBranch *br_Tof400L_Y =tree1->Branch("Tof400L_Y",&Tof400L_Y);
  TBranch *br_Tof400L_Z =tree1->Branch("Tof400L_Z",&Tof400L_Z);
  TBranch *br_Tof400L_Tx =tree1->Branch("Tof400L_Tx",&Tof400L_Tx);
  TBranch *br_Tof400L_Ty =tree1->Branch("Tof400L_Ty",&Tof400L_Ty);
  TBranch *br_Tof400L_T =tree1->Branch("Tof400L_T",&Tof400L_T);
  TBranch *br_Tof400L_DetId =tree1->Branch("Tof400L_DetId",&Tof400L_DetId);
  TBranch *br_Tof400L_nHits =tree1->Branch("Tof400L_nHits",&Tof400L_nHits);

  TBranch *br_Tof400R_X =tree1->Branch("Tof400R_X",&Tof400R_X);
  TBranch *br_Tof400R_Y =tree1->Branch("Tof400R_Y",&Tof400R_Y);
  TBranch *br_Tof400R_Z =tree1->Branch("Tof400R_Z",&Tof400R_Z);
  TBranch *br_Tof400R_Tx =tree1->Branch("Tof400R_Tx",&Tof400R_Tx);
  TBranch *br_Tof400R_Ty =tree1->Branch("Tof400R_Ty",&Tof400R_Ty);
  TBranch *br_Tof400R_T =tree1->Branch("Tof400R_T",&Tof400R_T);
  TBranch *br_Tof400R_DetId =tree1->Branch("Tof400R_DetId",&Tof400R_DetId);
  TBranch *br_Tof400R_nHits =tree1->Branch("Tof400R_nHits",&Tof400R_nHits);

  TBranch *br_TofCalL_X =tree1->Branch("TofCalL_X",&TofCalL_X);
  TBranch *br_TofCalL_Y =tree1->Branch("TofCalL_Y",&TofCalL_Y);
  TBranch *br_TofCalL_Z =tree1->Branch("TofCalL_Z",&TofCalL_Z);
  TBranch *br_TofCalL_Tx =tree1->Branch("TofCalL_Tx",&TofCalL_Tx);
  TBranch *br_TofCalL_Ty =tree1->Branch("TofCalL_Ty",&TofCalL_Ty);
  TBranch *br_TofCalL_T =tree1->Branch("TofCalL_T",&TofCalL_T);
  TBranch *br_TofCalL_Bar =tree1->Branch("TofCalL_Bar",&TofCalL_Bar);
  TBranch *br_TofCalL_Layer =tree1->Branch("TofCalL_Layer",&TofCalL_Layer);
  TBranch *br_TofCalL_nHits =tree1->Branch("TofCalL_nHits",&TofCalL_nHits);

  TBranch *br_TofCalR_X =tree1->Branch("TofCalR_X",&TofCalR_X);
  TBranch *br_TofCalR_Y =tree1->Branch("TofCalR_Y",&TofCalR_Y);
  TBranch *br_TofCalR_Z =tree1->Branch("TofCalR_Z",&TofCalR_Z);
  TBranch *br_TofCalR_Tx =tree1->Branch("TofCalR_Tx",&TofCalR_Tx);
  TBranch *br_TofCalR_Ty =tree1->Branch("TofCalR_Ty",&TofCalR_Ty);
  TBranch *br_TofCalR_T =tree1->Branch("TofCalR_T",&TofCalR_T);
  TBranch *br_TofCalR_Bar =tree1->Branch("TofCalR_Bar",&TofCalR_Bar);
  TBranch *br_TofCalR_Layer =tree1->Branch("TofCalR_Layer",&TofCalR_Layer);
  TBranch *br_TofCalR_nHits =tree1->Branch("TofCalR_nHits",&TofCalR_nHits);




  TBranch *br_dCSCH_sec =tree1->Branch("dCSCH_sec",&dCSCH_sec);
  TBranch *br_dCSCH_nanosec =tree1->Branch("dCSCH_nanosec",&dCSCH_nanosec);
  TBranch *br_dCSCH_cscN =tree1->Branch("dCSCH_N",&dCSCH_cscN);
  TBranch *br_dCSCH_cscX =tree1->Branch("dCSCH_X",&dCSCH_cscX);
  TBranch *br_dCSCH_cscY =tree1->Branch("dCSCH_Y",&dCSCH_cscY);
  TBranch *br_dCSCH_cscZ =tree1->Branch("dCSCH_Z",&dCSCH_cscZ);


  Track *MWPC_p1 = new Track;
  TBranch *br_MWPC_p1_X =tree1->Branch("MWPC_p1_X",&MWPC_p1->X);
  TBranch *br_MWPC_p1_Y =tree1->Branch("MWPC_p1_Y",&MWPC_p1->Y);
  TBranch *br_MWPC_p1_Z =tree1->Branch("MWPC_p1_Z",&MWPC_p1->Z);
  TBranch *br_MWPC_p1_Tx =tree1->Branch("MWPC_p1_Tx",&MWPC_p1->Tx);
  TBranch *br_MWPC_p1_Ty =tree1->Branch("MWPC_p1_Ty",&MWPC_p1->Ty);
  TBranch *br_MWPC_p1_Chi2X =tree1->Branch("MWPC_p1_Chi2",&MWPC_p1->Chi2X);
  // TBranch *br_MWPC3_Chi2Y =tree1->Branch("MWPC_p1_Chi2Y",&MWPC_p1->Chi2Y);
  TBranch *br_MWPC_p1_nTracks =tree1->Branch("MWPC_p1_nTracks",&MWPC_p1->nTracks);

  Track *MWPC_p0 = new Track;
  TBranch *br_MWPC_p0_X =tree1->Branch("MWPC_p0_X",&MWPC_p0->X);
  TBranch *br_MWPC_p0_Y =tree1->Branch("MWPC_p0_Y",&MWPC_p0->Y);
  TBranch *br_MWPC_p0_Z =tree1->Branch("MWPC_p0_Z",&MWPC_p0->Z);
  TBranch *br_MWPC_p0_Tx =tree1->Branch("MWPC_p0_Tx",&MWPC_p0->Tx);
  TBranch *br_MWPC_p0_Ty =tree1->Branch("MWPC_p0_Ty",&MWPC_p0->Ty);
  TBranch *br_MWPC_p0_Chi2X =tree1->Branch("MWPC_p0_Chi2",&MWPC_p0->Chi2X);
  // TBranch *br_MWPC3_Chi2Y =tree1->Branch("MWPC_p0_Chi2Y",&MWPC_p0->Chi2Y);
  TBranch *br_MWPC_p0_nTracks =tree1->Branch("MWPC_p0_nTracks",&MWPC_p0->nTracks);

  Track *MWPC1 = new Track;
  TBranch *br_MWPC1_X =tree1->Branch("MWPC1_X",&MWPC1->X);
  TBranch *br_MWPC1_Y =tree1->Branch("MWPC1_Y",&MWPC1->Y);
  TBranch *br_MWPC1_Z =tree1->Branch("MWPC1_Z",&MWPC1->Z);
  TBranch *br_MWPC1_Tx =tree1->Branch("MWPC1_Tx",&MWPC1->Tx);
  TBranch *br_MWPC1_Ty =tree1->Branch("MWPC1_Ty",&MWPC1->Ty);
  TBranch *br_MWPC1_Chi2X =tree1->Branch("MWPC1_Chi2",&MWPC1->Chi2X);
  // TBranch *br_MWPC3_Chi2Y =tree1->Branch("MWPC1_Chi2Y",&MWPC1->Chi2Y);
  TBranch *br_MWPC1_nTracks =tree1->Branch("MWPC1_nTracks",&MWPC1->nTracks);

  Track *MWPC2 = new Track;
  TBranch *br_MWPC2_X =tree1->Branch("MWPC2_X",&MWPC2->X);
  TBranch *br_MWPC2_Y =tree1->Branch("MWPC2_Y",&MWPC2->Y);
  TBranch *br_MWPC2_Z =tree1->Branch("MWPC2_Z",&MWPC2->Z);
  TBranch *br_MWPC2_Tx =tree1->Branch("MWPC2_Tx",&MWPC2->Tx);
  TBranch *br_MWPC2_Ty =tree1->Branch("MWPC2_Ty",&MWPC2->Ty);
  TBranch *br_MWPC2_Chi2X =tree1->Branch("MWPC2_Chi2",&MWPC2->Chi2X);
  // TBranch *br_MWPC3_Chi2Y =tree1->Branch("MWPC2_Chi2Y",&MWPC2->Chi2Y);
  TBranch *br_MWPC2_nTracks =tree1->Branch("MWPC2_nTracks",&MWPC2->nTracks);

  Track *MWPC3 = new Track;
  TBranch *br_MWPC3_X =tree1->Branch("MWPC3_X",&MWPC3->X);
  TBranch *br_MWPC3_Y =tree1->Branch("MWPC3_Y",&MWPC3->Y);
  TBranch *br_MWPC3_Z =tree1->Branch("MWPC3_Z",&MWPC3->Z);
  TBranch *br_MWPC3_Tx =tree1->Branch("MWPC3_Tx",&MWPC3->Tx);
  TBranch *br_MWPC3_Ty =tree1->Branch("MWPC3_Ty",&MWPC3->Ty);
  TBranch *br_MWPC3_Chi2X =tree1->Branch("MWPC3_Chi2",&MWPC3->Chi2X);
  // TBranch *br_MWPC3_Chi2Y =tree1->Branch("MWPC3_Chi2Y",&MWPC3->Chi2Y);
  TBranch *br_MWPC3_nTracks =tree1->Branch("MWPC3_nTracks",&MWPC3->nTracks);

  Track *MWPC4 = new Track;
  TBranch *br_MWPC4_X =tree1->Branch("MWPC4_X",&MWPC4->X);
  TBranch *br_MWPC4_Y =tree1->Branch("MWPC4_Y",&MWPC4->Y);
  TBranch *br_MWPC4_Z =tree1->Branch("MWPC4_Z",&MWPC4->Z);
  TBranch *br_MWPC4_Tx =tree1->Branch("MWPC4_Tx",&MWPC4->Tx);
  TBranch *br_MWPC4_Ty =tree1->Branch("MWPC4_Ty",&MWPC4->Ty);
  TBranch *br_MWPC4_Chi2X =tree1->Branch("MWPC4_Chi2",&MWPC4->Chi2X);
  // TBranch *br_MWPC4_Chi2Y =tree1->Branch("MWPC4_Chi2Y",&MWPC4->Chi2Y);
  TBranch *br_MWPC4_nTracks =tree1->Branch("MWPC4_nTracks",&MWPC4->nTracks);

  Track *Sil1234 = new Track;
  TBranch *br_Sil1234_X =tree1->Branch("Si1234M34_X",&Sil1234->X);
  TBranch *br_Sil1234_Y =tree1->Branch("Si1234M34_Y",&Sil1234->Y);
  TBranch *br_Sil1234_Z =tree1->Branch("Si1234M34_Z",&Sil1234->Z);
  TBranch *br_Sil1234_Tx =tree1->Branch("Si1234M34_Tx",&Sil1234->Tx);
  TBranch *br_Sil1234_Ty =tree1->Branch("Si1234M34_Ty",&Sil1234->Ty);
  TBranch *br_Sil1234_Chi2X =tree1->Branch("Si1234M34_Chi2X",&Sil1234->Chi2X);
  TBranch *br_Sil1234_Chi2Y =tree1->Branch("Si1234M34_Chi2Y",&Sil1234->Chi2Y);
  TBranch *br_Sil123_IDX =tree1->Branch("Si1234M34_IDX",&Sil1234->IDX);
  TBranch *br_Sil123_IDY =tree1->Branch("Si1234M34_IDY",&Sil1234->IDY);
  TBranch *br_Sil1234_nTracks =tree1->Branch("Si1234M34_nTracks",&Sil1234->nTracks);
  TBranch *br_Sil1234_Flag =tree1->Branch("Si1234M34_Flag",&Sil1234->Flag);

  
  Track *DCH = new Track;
 	TBranch *br_DCH_Ntr =tree1->Branch("DCH_Ntr",&DCH->nTracks);
  TBranch *br_DCH_X =tree1->Branch("DCH_X",&DCH->X);
  TBranch *br_DCH_Y =tree1->Branch("DCH_Y",&DCH->Y);
  TBranch *br_DCH_Z =tree1->Branch("DCH_Z",&DCH->Z);
  TBranch *br_DCH_Tx =tree1->Branch("DCH_Tx",&DCH->Tx);
  TBranch *br_DCH_Ty =tree1->Branch("DCH_Ty",&DCH->Ty);
  // TBranch *br_DCH_Chi2 =tree1->Branch("DCH_Chi2",&DCH->Chi2);
  // TBranch *br_DCH_nHits =tree1->Branch("DCH_nHits",&DCH->nHits);
  // TBranch *br_DCH_IsGlob =tree1->Branch("DCH_IsGlobalTrack",&DCH->IsGlDCH);
  // TBranch *br_DCH_NGlTracks =tree1->Branch("DCH_NGlobalTr",&DCH->nGlobDCHTracks);
  
  Track *DCH1 = new Track;
 	TBranch *br_DCH1_Ntr =tree1->Branch("DCH1_Ntr",&DCH1->nTracks);
  TBranch *br_DCH1_X =tree1->Branch("DCH1_X",&DCH1->X);
  TBranch *br_DCH1_Y =tree1->Branch("DCH1_Y",&DCH1->Y);
  TBranch *br_DCH1_Z =tree1->Branch("DCH1_Z",&DCH1->Z);
  TBranch *br_DCH1_Tx =tree1->Branch("DCH1_Tx",&DCH1->Tx);
  TBranch *br_DCH1_Ty =tree1->Branch("DCH1_Ty",&DCH1->Ty);
  // TBranch *br_DCH1_Chi2 =tree1->Branch("DCH1_Chi2",&DCH1->Chi2);
  // TBranch *br_DCH1_nHits =tree1->Branch("DCH1_nHits",&DCH1->nHits);
  // TBranch *br_DCH1_IsGlob =tree1->Branch("DCH1_IsGlobalTrack",&DCH1->IsGlDCH);
  // TBranch *br_DCH1_NGlTracks =tree1->Branch("DCH1_NGlobalTr",&DCH1->nGlobDCHTracks);
  
  Track *DCH2 = new Track;
 	TBranch *br_DCH2_Ntr =tree1->Branch("DCH2_Ntr",&DCH2->nTracks);
  TBranch *br_DCH2_X =tree1->Branch("DCH2_X",&DCH2->X);
  TBranch *br_DCH2_Y =tree1->Branch("DCH2_Y",&DCH2->Y);
  TBranch *br_DCH2_Z =tree1->Branch("DCH2_Z",&DCH2->Z);
  TBranch *br_DCH2_Tx =tree1->Branch("DCH2_Tx",&DCH2->Tx);
  TBranch *br_DCH2_Ty =tree1->Branch("DCH2_Ty",&DCH2->Ty);
  // TBranch *br_DCH2_Chi2 =tree1->Branch("DCH2_Chi2",&DCH2->Chi2);
  // TBranch *br_DCH2_nHits =tree1->Branch("DCH2_nHits",&DCH2->nHits);
  //TBranch *br_DCH2_IsGlob =tree1->Branch("DCH2_IsGlobalTrack",&DCH2->IsGlDCH);
  //TBranch *br_DCH2_NGlTracks =tree1->Branch("DCH2_NGlobalTr",&DCH2->nGlobDCHTracks);

  Inf *Sil1H = new Inf;
  TBranch *br_Si1H_X=tree1->Branch("Si1H_X",&Sil1H->X);
  TBranch *br_Si1H_Y=tree1->Branch("Si1H_Y",&Sil1H->Y);
  TBranch *br_Si1H_St=tree1->Branch("Si1H_St",&Sil1H->St);
  TBranch *br_Si1H_Mod=tree1->Branch("Si1H_Mod",&Sil1H->Mod);
  TBranch *br_Sil1H_L0H =tree1->Branch("Si1H_L0NHits",&Sil1H->L0H);
  TBranch *br_Sil1H_L1H =tree1->Branch("Si1H_L1NHits",&Sil1H->L1H);
  TBranch *br_Sil1H_AmpDir =tree1->Branch("Si1H_AmpDir",&Sil1H->AmpDir);
  TBranch *br_Sil1H_AmpIncl =tree1->Branch("Si1H_AmpIncl",&Sil1H->AmpIncl);
  TBranch *br_Si1H_nHits=tree1->Branch("Si1H_nHits",&Sil1H->nHits);

  Inf *Sil2H = new Inf;
  TBranch *br_Si2H_X=tree1->Branch("Si2H_X",&Sil2H->X);
  TBranch *br_Si2H_Y=tree1->Branch("Si2H_Y",&Sil2H->Y);
  TBranch *br_Si2H_St=tree1->Branch("Si2H_St",&Sil2H->St);
  TBranch *br_Si2H_Mod=tree1->Branch("Si2H_Mod",&Sil2H->Mod);
  TBranch *br_Sil2H_L0H =tree1->Branch("Si2H_L0NHits",&Sil2H->L0H);
  TBranch *br_Sil2H_L1H =tree1->Branch("Si2H_L1NHits",&Sil2H->L1H);
  TBranch *br_Sil2H_AmpDir =tree1->Branch("Si2H_AmpDir",&Sil2H->AmpDir);
  TBranch *br_Sil2H_AmpIncl =tree1->Branch("Si2H_AmpIncl",&Sil2H->AmpIncl);
  TBranch *br_Si2H_nHits=tree1->Branch("Si2H_nHits",&Sil2H->nHits);

  Inf *Sil3H = new Inf;
  TBranch *br_Si3H_X=tree1->Branch("Si3H_X",&Sil3H->X);
  TBranch *br_Si3H_Y=tree1->Branch("Si3H_Y",&Sil3H->Y);
  TBranch *br_Si3H_St=tree1->Branch("Si3H_St",&Sil3H->St);
  TBranch *br_Si3H_Mod=tree1->Branch("Si3H_Mod",&Sil3H->Mod);
  TBranch *br_Sil3H_L0H =tree1->Branch("Si3H_L0NHits",&Sil3H->L0H);
  TBranch *br_Sil3H_L1H =tree1->Branch("Si3H_L1NHits",&Sil3H->L1H);
  TBranch *br_Sil3H_AmpDir =tree1->Branch("Si3H_AmpDir",&Sil3H->AmpDir);
  TBranch *br_Sil3H_AmpIncl =tree1->Branch("Si3H_AmpIncl",&Sil3H->AmpIncl);
  TBranch *br_Si3H_nHits=tree1->Branch("Si3H_nHits",&Sil3H->nHits);

  Inf *Sil4H = new Inf;
  TBranch *br_Si4H_X=tree1->Branch("Si4H_X",&Sil4H->X);
  TBranch *br_Si4H_Y=tree1->Branch("Si4H_Y",&Sil4H->Y);
  TBranch *br_Si4H_St=tree1->Branch("Si4H_St",&Sil4H->St);
  TBranch *br_Si4H_Mod=tree1->Branch("Si4H_Mod",&Sil4H->Mod);
  TBranch *br_Sil4H_L0H =tree1->Branch("Si4H_L0NHits",&Sil4H->L0H);
  TBranch *br_Sil4H_L1H =tree1->Branch("Si4H_L1NHits",&Sil4H->L1H);
  TBranch *br_Sil4H_AmpDir =tree1->Branch("Si4H_AmpDir",&Sil4H->AmpDir);
  TBranch *br_Sil4H_AmpIncl =tree1->Branch("Si4H_AmpIncl",&Sil4H->AmpIncl);
  TBranch *br_Si4H_nHits=tree1->Branch("Si4H_nHits",&Sil4H->nHits);



 


  //filename = "/zfs/scratch/lhep-src/reconstruction_11_24/ArmTr_G/dec25_G/Run_"+to_string(filenum)+"_GTrack_Alig.root"; // input
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiAligMWPCTracks_4780_4799_Chi2_5_Weight_Fit_0_tracks.root";
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiMwpcAligDchCoord4780_4799_BMNS_wo_gap_wo_rot_WithDCH_MWPC_Zinv.root";
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiMwpcAligDchCoord4780_4799_BMNS_wo_gap_wo_rot_WithDCH_MWPC_Zinv_wo_ampcut.root";
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiMwpcAligDchCoord4780_4799_BMNS_wo_gap_wo_rot_WithDCH_MWPC_Zinv_ampcut_1700.root";
  
  
  TFile *hfile = new TFile(infilename);
  //		if (!hfile->IsOpen())  continue;
  //		if (!hfile->Get("bmndata")) 		continue;
        
  TTree *tree = (TTree*)hfile->Get("tree1");

  tree->SetBranchAddress("event",&event);
  tree->SetBranchAddress("trigger",&trigger);
  tree->SetBranchAddress("VC",&veto);
  tree->SetBranchAddress("run",&run);
  tree->SetBranchAddress("isGoodRun",&isGoodRun);
  tree->SetBranchAddress("TS_sec",&TS_sec);
  tree->SetBranchAddress("TS_nsec",&TS_nsec);

  tree->SetBranchAddress("BC1",&BC1);
  tree->SetBranchAddress("BC2",&BC2);
  tree->SetBranchAddress("BC3",&BC3);
  tree->SetBranchAddress("BC4",&BC4);
  tree->SetBranchAddress("BC5",&BC5);
  tree->SetBranchAddress("chargein",&chargein);
  tree->SetBranchAddress("charge34",&charge34);
  tree->SetBranchAddress("charge35",&charge35);
  tree->SetBranchAddress("charge45",&charge45);
  tree->SetBranchAddress("Pq_MDF_UpSt",&Pq_MDF_UpSt);
  tree->SetBranchAddress("Pq_MDF_X_UpSt",&Pq_MDF_X_UpSt);
  tree->SetBranchAddress("Pq_MDF_Y_UpSt",&Pq_MDF_Y_UpSt);
  tree->SetBranchAddress("Pq_MDF_Z_UpSt",&Pq_MDF_Z_UpSt);
  tree->SetBranchAddress("Pq_MDF_UpSt_nTracks",&Pq_MDF_UpSt_nTracks);
  tree->SetBranchAddress("Tx_AT",&Tx_AT);
  tree->SetBranchAddress("Ty_AT",&Ty_AT);
  tree->SetBranchAddress("Pq_MDF_Flag",&Pq_MDF_Flag);
  tree->SetBranchAddress("Tx_MDF_UpSt",&Tx_MDF_UpSt);
  tree->SetBranchAddress("Fragment",&Fragment);

  tree->SetBranchAddress("SW_ELoss",&SW_ELoss);
  tree->SetBranchAddress("SW_CellId",&SW_CellId);
  tree->SetBranchAddress("SW_Flag",&SW_Flag);
  tree->SetBranchAddress("SW_TrId",&SW_TrId);
  tree->SetBranchAddress("SW_OrderId",&SW_OrderId);
  tree->SetBranchAddress("SW_Mult",&SW_Mult);
  tree->SetBranchAddress("SW_NMatchTr",&SW_NMatchTr);

  tree->SetBranchAddress("VB_Mult",&VB_Mult);
  tree->SetBranchAddress("VB_CellId",&VB_CellId);

  tree->SetBranchAddress("GemL_X",&GemL_X);
  tree->SetBranchAddress("GemL_Y",&GemL_Y);
  tree->SetBranchAddress("GemL_Z",&GemL_Z);
  tree->SetBranchAddress("GemL_Tx",&GemL_Tx);
  tree->SetBranchAddress("GemL_Ty",&GemL_Ty);
  tree->SetBranchAddress("GemL_nHits",&GemL_nHits);

  tree->SetBranchAddress("GemR_X",&GemR_X);
  tree->SetBranchAddress("GemR_Y",&GemR_Y);
  tree->SetBranchAddress("GemR_Z",&GemR_Z);
  tree->SetBranchAddress("GemR_Tx",&GemR_Tx);
  tree->SetBranchAddress("GemR_Ty",&GemR_Ty);
  tree->SetBranchAddress("GemR_nHits",&GemR_nHits);

  tree->SetBranchAddress("CscL_X",&CscL_X);
  tree->SetBranchAddress("CscL_Y",&CscL_Y);
  tree->SetBranchAddress("CscL_Z",&CscL_Z);
  tree->SetBranchAddress("CscL_Tx",&CscL_Tx);
  tree->SetBranchAddress("CscL_Ty",&CscL_Ty);
  tree->SetBranchAddress("CscL_nHits",&CscL_nHits);

  tree->SetBranchAddress("CscR_X",&CscR_X);
  tree->SetBranchAddress("CscR_Y",&CscR_Y);
  tree->SetBranchAddress("CscR_Z",&CscR_Z);
  tree->SetBranchAddress("CscR_Tx",&CscR_Tx);
  tree->SetBranchAddress("CscR_Ty",&CscR_Ty);
  tree->SetBranchAddress("CscR_nHits",&CscR_nHits);

  tree->SetBranchAddress("Tof400L_X",&Tof400L_X);
  tree->SetBranchAddress("Tof400L_Y",&Tof400L_Y);
  tree->SetBranchAddress("Tof400L_Z",&Tof400L_Z);
  tree->SetBranchAddress("Tof400L_Tx",&Tof400L_Tx);
  tree->SetBranchAddress("Tof400L_Ty",&Tof400L_Ty);
  tree->SetBranchAddress("Tof400L_T",&Tof400L_T);
  tree->SetBranchAddress("Tof400L_DetId",&Tof400L_DetId);
  tree->SetBranchAddress("Tof400L_nHits",&Tof400L_nHits);

  tree->SetBranchAddress("Tof400R_X",&Tof400R_X);
  tree->SetBranchAddress("Tof400R_Y",&Tof400R_Y);
  tree->SetBranchAddress("Tof400R_Z",&Tof400R_Z);
  tree->SetBranchAddress("Tof400R_Tx",&Tof400R_Tx);
  tree->SetBranchAddress("Tof400R_Ty",&Tof400R_Ty);
  tree->SetBranchAddress("Tof400R_T",&Tof400R_T);
  tree->SetBranchAddress("Tof400R_DetId",&Tof400R_DetId);
  tree->SetBranchAddress("Tof400R_nHits",&Tof400R_nHits);

  tree->SetBranchAddress("TofCalL_X",&TofCalL_X);
  tree->SetBranchAddress("TofCalL_Y",&TofCalL_Y);
  tree->SetBranchAddress("TofCalL_Z",&TofCalL_Z);
  tree->SetBranchAddress("TofCalL_Tx",&TofCalL_Tx);
  tree->SetBranchAddress("TofCalL_Ty",&TofCalL_Ty);
  tree->SetBranchAddress("TofCalL_T",&TofCalL_T);
  tree->SetBranchAddress("TofCalL_Bar",&TofCalL_Bar);
  tree->SetBranchAddress("TofCalL_Layer",&TofCalL_Layer);
  tree->SetBranchAddress("TofCalL_nHits",&TofCalL_nHits);

  tree->SetBranchAddress("TofCalR_X",&TofCalR_X);
  tree->SetBranchAddress("TofCalR_Y",&TofCalR_Y);
  tree->SetBranchAddress("TofCalR_Z",&TofCalR_Z);
  tree->SetBranchAddress("TofCalR_Tx",&TofCalR_Tx);
  tree->SetBranchAddress("TofCalR_Ty",&TofCalR_Ty);
  tree->SetBranchAddress("TofCalR_T",&TofCalR_T);
  tree->SetBranchAddress("TofCalR_Bar",&TofCalR_Bar);
  tree->SetBranchAddress("TofCalR_Layer",&TofCalR_Layer);
  tree->SetBranchAddress("TofCalR_nHits",&TofCalR_nHits);

  tree->SetBranchAddress("Is_laser", &Is_laser); 
  tree->SetBranchAddress("BC1_Time0", &BC1_Time0); 
  tree->SetBranchAddress("BC1_Time1", &BC1_Time1); 
  tree->SetBranchAddress("BC2_Time0", &BC2_Time0); 
  tree->SetBranchAddress("BC2_Time1", &BC2_Time1); 


  tree->SetBranchAddress("dCSCH_sec",&dCSCH_sec);
  tree->SetBranchAddress("dCSCH_nanosec",&dCSCH_nanosec);
  tree->SetBranchAddress("dCSCH_N",&dCSCH_cscN);
  tree->SetBranchAddress("dCSCH_X",&dCSCH_cscX);
  tree->SetBranchAddress("dCSCH_Y",&dCSCH_cscY);
  tree->SetBranchAddress("dCSCH_Z",&dCSCH_cscZ);


  tree->SetBranchAddress("MWPC_p1_X",&MWPC_p1->X);
  tree->SetBranchAddress("MWPC_p1_Y",&MWPC_p1->Y);
  tree->SetBranchAddress("MWPC_p1_Z",&MWPC_p1->Z);
  tree->SetBranchAddress("MWPC_p1_Tx",&MWPC_p1->Tx);
  tree->SetBranchAddress("MWPC_p1_Ty",&MWPC_p1->Ty);
  tree->SetBranchAddress("MWPC_p1_Chi2",&MWPC_p1->Chi2X);
  tree->SetBranchAddress("MWPC_p1_nTracks",&MWPC_p1->nTracks);


  tree->SetBranchAddress("MWPC_p0_X",&MWPC_p0->X);
  tree->SetBranchAddress("MWPC_p0_Y",&MWPC_p0->Y);
  tree->SetBranchAddress("MWPC_p0_Z",&MWPC_p0->Z);
  tree->SetBranchAddress("MWPC_p0_Tx",&MWPC_p0->Tx);
  tree->SetBranchAddress("MWPC_p0_Ty",&MWPC_p0->Ty);
  tree->SetBranchAddress("MWPC_p0_Chi2",&MWPC_p0->Chi2X);
  tree->SetBranchAddress("MWPC_p0_nTracks",&MWPC_p0->nTracks);


  tree->SetBranchAddress("MWPC1_X",&MWPC1->X);
  tree->SetBranchAddress("MWPC1_Y",&MWPC1->Y);
  tree->SetBranchAddress("MWPC1_Z",&MWPC1->Z);
  tree->SetBranchAddress("MWPC1_Tx",&MWPC1->Tx);
  tree->SetBranchAddress("MWPC1_Ty",&MWPC1->Ty);
  tree->SetBranchAddress("MWPC1_Chi2",&MWPC1->Chi2X);
  tree->SetBranchAddress("MWPC1_nTracks",&MWPC1->nTracks);

  tree->SetBranchAddress("MWPC2_X",&MWPC2->X);
  tree->SetBranchAddress("MWPC2_Y",&MWPC2->Y);
  tree->SetBranchAddress("MWPC2_Z",&MWPC2->Z);
  tree->SetBranchAddress("MWPC2_Tx",&MWPC2->Tx);
  tree->SetBranchAddress("MWPC2_Ty",&MWPC2->Ty);
  tree->SetBranchAddress("MWPC2_Chi2",&MWPC2->Chi2X);
  tree->SetBranchAddress("MWPC2_nTracks",&MWPC2->nTracks);

  tree->SetBranchAddress("MWPC3_X",&MWPC3->X);
  tree->SetBranchAddress("MWPC3_Y",&MWPC3->Y);
  tree->SetBranchAddress("MWPC3_Z",&MWPC3->Z);
  tree->SetBranchAddress("MWPC3_Tx",&MWPC3->Tx);
  tree->SetBranchAddress("MWPC3_Ty",&MWPC3->Ty);
  tree->SetBranchAddress("MWPC3_Chi2",&MWPC3->Chi2X);
  tree->SetBranchAddress("MWPC3_nTracks",&MWPC3->nTracks);

  tree->SetBranchAddress("MWPC4_X",&MWPC4->X);
  tree->SetBranchAddress("MWPC4_Y",&MWPC4->Y);
  tree->SetBranchAddress("MWPC4_Z",&MWPC4->Z);
  tree->SetBranchAddress("MWPC4_Tx",&MWPC4->Tx);
  tree->SetBranchAddress("MWPC4_Ty",&MWPC4->Ty);
  tree->SetBranchAddress("MWPC4_Chi2",&MWPC4->Chi2X);
  tree->SetBranchAddress("MWPC4_nTracks",&MWPC4->nTracks);

  tree->SetBranchAddress("Si1234M34_X",&Sil1234->X);
  tree->SetBranchAddress("Si1234M34_Y",&Sil1234->Y);
  tree->SetBranchAddress("Si1234M34_Z",&Sil1234->Z);
  tree->SetBranchAddress("Si1234M34_Tx",&Sil1234->Tx);
  tree->SetBranchAddress("Si1234M34_Ty",&Sil1234->Ty);
  tree->SetBranchAddress("Si1234M34_Chi2X",&Sil1234->Chi2X);
  tree->SetBranchAddress("Si1234M34_Chi2Y",&Sil1234->Chi2Y);
  tree->SetBranchAddress("Si1234M34_IDX",&Sil1234->IDX);
  tree->SetBranchAddress("Si1234M34_IDY",&Sil1234->IDY);
  tree->SetBranchAddress("Si1234M34_nTracks",&Sil1234->nTracks);
  tree->SetBranchAddress("Si1234M34_Flag",&Sil1234->Flag);

 	tree->SetBranchAddress("DCH_Ntr",&DCH->nTracks);
  tree->SetBranchAddress("DCH_X",&DCH->X);
  tree->SetBranchAddress("DCH_Y",&DCH->Y);
  tree->SetBranchAddress("DCH_Z",&DCH->Z);
  tree->SetBranchAddress("DCH_Tx",&DCH->Tx);
  tree->SetBranchAddress("DCH_Ty",&DCH->Ty);
  // TBranch *br_DCH_Chi2 =tree1->Branch("DCH_Chi2",&DCH->Chi2);
  // TBranch *br_DCH_nHits =tree1->Branch("DCH_nHits",&DCH->nHits);
  // TBranch *br_DCH_IsGlob =tree1->Branch("DCH_IsGlobalTrack",&DCH->IsGlDCH);
  // TBranch *br_DCH_NGlTracks =tree1->Branch("DCH_NGlobalTr",&DCH->nGlobDCHTracks);
  
 	tree->SetBranchAddress("DCH1_Ntr",&DCH1->nTracks);
  tree->SetBranchAddress("DCH1_X",&DCH1->X);
  tree->SetBranchAddress("DCH1_Y",&DCH1->Y);
  tree->SetBranchAddress("DCH1_Z",&DCH1->Z);
  tree->SetBranchAddress("DCH1_Tx",&DCH1->Tx);
  tree->SetBranchAddress("DCH1_Ty",&DCH1->Ty);
  // TBranch *br_DCH1_Chi2 =tree1->Branch("DCH1_Chi2",&DCH1->Chi2);
  // TBranch *br_DCH1_nHits =tree1->Branch("DCH1_nHits",&DCH1->nHits);
  // TBranch *br_DCH1_IsGlob =tree1->Branch("DCH1_IsGlobalTrack",&DCH1->IsGlDCH);
  // TBranch *br_DCH1_NGlTracks =tree1->Branch("DCH1_NGlobalTr",&DCH1->nGlobDCHTracks);
  
 	tree->SetBranchAddress("DCH2_Ntr",&DCH2->nTracks);
  tree->SetBranchAddress("DCH2_X",&DCH2->X);
  tree->SetBranchAddress("DCH2_Y",&DCH2->Y);
  tree->SetBranchAddress("DCH2_Z",&DCH2->Z);
  tree->SetBranchAddress("DCH2_Tx",&DCH2->Tx);
  tree->SetBranchAddress("DCH2_Ty",&DCH2->Ty);
  // TBranch *br_DCH2_Chi2 =tree1->Branch("DCH2_Chi2",&DCH2->Chi2);
  // TBranch *br_DCH2_nHits =tree1->Branch("DCH2_nHits",&DCH2->nHits);
  //TBranch *br_DCH2_IsGlob =tree1->Branch("DCH2_IsGlobalTrack",&DCH2->IsGlDCH);
  //TBranch *br_DCH2_NGlTracks =tree1->Branch("DCH2_NGlobalTr",&DCH2->nGlobDCHTracks);


  tree->SetBranchAddress("Si1H_X",&Sil1H->X);
  tree->SetBranchAddress("Si1H_Y",&Sil1H->Y);
  tree->SetBranchAddress("Si1H_St",&Sil1H->St);
  tree->SetBranchAddress("Si1H_Mod",&Sil1H->Mod);
  tree->SetBranchAddress("Si1H_L0NHits",&Sil1H->L0H);
  tree->SetBranchAddress("Si1H_L1NHits",&Sil1H->L1H);
  tree->SetBranchAddress("Si1H_AmpDir",&Sil1H->AmpDir);
  tree->SetBranchAddress("Si1H_AmpIncl",&Sil1H->AmpIncl);
  tree->SetBranchAddress("Si1H_nHits",&Sil1H->nHits);

  tree->SetBranchAddress("Si2H_X",&Sil2H->X);
  tree->SetBranchAddress("Si2H_Y",&Sil2H->Y);
  tree->SetBranchAddress("Si2H_St",&Sil2H->St);
  tree->SetBranchAddress("Si2H_Mod",&Sil2H->Mod);
  tree->SetBranchAddress("Si2H_L0NHits",&Sil2H->L0H);
  tree->SetBranchAddress("Si2H_L1NHits",&Sil2H->L1H);
  tree->SetBranchAddress("Si2H_AmpDir",&Sil2H->AmpDir);
  tree->SetBranchAddress("Si2H_AmpIncl",&Sil2H->AmpIncl);
  tree->SetBranchAddress("Si2H_nHits",&Sil2H->nHits);

  tree->SetBranchAddress("Si3H_X",&Sil3H->X);
  tree->SetBranchAddress("Si3H_Y",&Sil3H->Y);
  tree->SetBranchAddress("Si3H_St",&Sil3H->St);
  tree->SetBranchAddress("Si3H_Mod",&Sil3H->Mod);
  tree->SetBranchAddress("Si3H_L0NHits",&Sil3H->L0H);
  tree->SetBranchAddress("Si3H_L1NHits",&Sil3H->L1H);
  tree->SetBranchAddress("Si3H_AmpDir",&Sil3H->AmpDir);
  tree->SetBranchAddress("Si3H_AmpIncl",&Sil3H->AmpIncl);
  tree->SetBranchAddress("Si3H_nHits",&Sil3H->nHits);


  tree->SetBranchAddress("Si4H_X",&Sil4H->X);
  tree->SetBranchAddress("Si4H_Y",&Sil4H->Y);
  tree->SetBranchAddress("Si4H_St",&Sil4H->St);
  tree->SetBranchAddress("Si4H_Mod",&Sil4H->Mod);
  tree->SetBranchAddress("Si4H_L0NHits",&Sil4H->L0H);
  tree->SetBranchAddress("Si4H_L1NHits",&Sil4H->L1H);
  tree->SetBranchAddress("Si4H_AmpDir",&Sil4H->AmpDir);
  tree->SetBranchAddress("Si4H_AmpIncl",&Sil4H->AmpIncl);
  tree->SetBranchAddress("Si4H_nHits",&Sil4H->nHits);




  int NEv = tree->GetEntries();
  int N_non0_DCH_Upst=0;
  int N_non0_Pq=0;
  int N_double_Pq=0;
  int N_double_Pq_ev=0;
  cout<<NEv<<endl; 


  int counter=0;
  for (int ev=0; ev<NEv; ev++) {
    tree->GetEntry(ev);

    OpAn_Th=-100;
    OpAn_Phi=-100;


    vector <int> CscLHitUsed;
    vector <int> Tof400LHitUsed;
    vector <int> TofCalLHitUsed;
    for (int i=0; i<CscL_nHits; i++) {CscLHitUsed.push_back(-1);}
    for (int i=0; i<Tof400L_nHits; i++) {Tof400LHitUsed.push_back(-1);}
    for (int i=0; i<TofCalL_nHits; i++) {TofCalLHitUsed.push_back(-1);}
    nTrackCandL=0;
    nTracksL=0;

   

    // for (int i=0; i<GemL_nHits; i++) {
    //   int was_gem=0;
    //   double trackcand[1000][10]={0};
    //   int ntracks=0;
    //   for (int j=0; j<CscL_nHits; j++) {
    //     if (CscLHitUsed.at(j)==0) continue;
    //     double d = sqrt((GemL_Tx->at(i)-CscL_Tx->at(j))*(GemL_Tx->at(i)-CscL_Tx->at(j)) + (GemL_Ty->at(i)-CscL_Ty->at(j))*(GemL_Ty->at(i)-CscL_Ty->at(j)));
    //     if (isnan(d)) continue;
    //     if (d>0.065) continue;
    //     trackcand[ntracks][0]=GemL_X->at(i);
    //     trackcand[ntracks][1]=GemL_Y->at(i);
    //     trackcand[ntracks][2]=GemL_Z->at(i);
    //     trackcand[ntracks][3]=CscL_X->at(j);
    //     trackcand[ntracks][4]=CscL_Y->at(j);
    //     trackcand[ntracks][5]=CscL_Z->at(j);
    //     trackcand[ntracks][6]=i+1;
    //     trackcand[ntracks][7]=j+1;
    //     trackcand[ntracks][8]=0;
    //     trackcand[ntracks][9]=0;
    //     ntracks++;
    //     if (was_gem==0){
    //       nTrackCandL++;
    //       was_gem++;
    //     }
    //     CscLHitUsed.at(j)=0;
    //   }
    //   for (int j=0; j<Tof400L_nHits; j++) {
    //     if (Tof400LHitUsed.at(j)==0) continue;
    //     double d = sqrt((GemL_Tx->at(i)-Tof400L_Tx->at(j))*(GemL_Tx->at(i)-Tof400L_Tx->at(j)) + (GemL_Ty->at(i)-Tof400L_Ty->at(j))*(GemL_Ty->at(i)-Tof400L_Ty->at(j)));
    //     if (isnan(d)) continue;
    //     if (d>0.1) continue;
    //     trackcand[ntracks][0]=GemL_X->at(i);
    //     trackcand[ntracks][1]=GemL_Y->at(i);
    //     trackcand[ntracks][2]=GemL_Z->at(i);
    //     trackcand[ntracks][3]=Tof400L_X->at(j);
    //     trackcand[ntracks][4]=Tof400L_Y->at(j);
    //     trackcand[ntracks][5]=Tof400L_Z->at(j);
    //     trackcand[ntracks][6]=i+1;
    //     trackcand[ntracks][7]=0;
    //     trackcand[ntracks][8]=j+1;
    //     trackcand[ntracks][9]=0;
    //     ntracks++;
    //     if (was_gem==0){
    //       nTrackCandL++;
    //       was_gem++;
    //     }
    //     Tof400LHitUsed.at(j)=0;
    //   }
    //   for (int j=0; j<TofCalL_nHits; j++) {
    //     if (TofCalLHitUsed.at(j)==0) continue;
    //     double dx = abs((GemL_Tx->at(i)-TofCalL_Tx->at(j)));
    //     double dy = abs((GemL_Ty->at(i)-TofCalL_Ty->at(j)));
    //     if (isnan(dx) || isnan(dy)) continue;
    //     if (dx>0.2) continue;
    //     if (dy>0.2) continue;
    //     trackcand[ntracks][0]=GemL_X->at(i);
    //     trackcand[ntracks][1]=GemL_Y->at(i);
    //     trackcand[ntracks][2]=GemL_Z->at(i);
    //     trackcand[ntracks][3]=TofCalL_X->at(j);
    //     // if (ntracks>0)  {trackcand[ntracks][4]=trackcand[0][4]; trackcand[ntracks][5]=trackcand[0][5];}
    //     // else {trackcand[ntracks][4]=-10000.; trackcand[ntracks][5]=-10000.;}
    //     trackcand[ntracks][4]=TofCalL_Y->at(j);
    //     trackcand[ntracks][5]=TofCalL_Z->at(j);
    //     trackcand[ntracks][6]=i+1;
    //     trackcand[ntracks][7]=0;
    //     trackcand[ntracks][8]=0;
    //     trackcand[ntracks][9]=j+1;
    //     ntracks++;
    //     if (was_gem==0){
    //       nTrackCandL++;
    //       was_gem++;
    //     }
    //     TofCalLHitUsed.at(j)=0;
    //   }

    //   if (nTrackCandL<=5 && ntracks>0) {
    //     TrackCandL_X1->push_back(trackcand[ntracks-1][0]);
    //     TrackCandL_Y1->push_back(trackcand[ntracks-1][1]);
    //     TrackCandL_Z1->push_back(trackcand[ntracks-1][2]);
    //     TrackCandL_X2->push_back(trackcand[ntracks-1][3]);
    //     TrackCandL_Y2->push_back(trackcand[ntracks-1][4]);
    //     TrackCandL_Z2->push_back(trackcand[ntracks-1][5]);
    //     TrackCandL_Flag->push_back(trackcand[ntracks-1][6]*1000000 + trackcand[ntracks-1][7]*10000 + trackcand[ntracks-1][8]*100 + trackcand[ntracks-1][9]);
    //   }
    //   if (nTrackCandL==1) {
    //     int csc_index=-1;
    //     int tof400_index=-1;
    //     for (int k=ntracks-1; k>=0; k--) {
    //     //for (int k=0; k<ntracks; k++) {
    //       // if (trackcand[k][7]>0) csc_index=trackcand[k][7]-1;
    //       // if (trackcand[k][8]>0) tof400_index=trackcand[k][8]-1;
    //       if (trackcand[k][7]>0) csc_index=k;
    //       if (trackcand[k][8]>0) tof400_index=k;
    //     }
    //     if (tof400_index<0) continue;
    //     if (csc_index>=0) {
    //       TrackL_X->push_back(trackcand[csc_index][0]);
    //       TrackL_Y->push_back(trackcand[csc_index][1]);
    //       TrackL_Z->push_back(trackcand[csc_index][2]);
    //       TrackL_Tx->push_back((trackcand[csc_index][3]-trackcand[csc_index][0])/(trackcand[csc_index][5]-trackcand[csc_index][2]));
    //       TrackL_Ty->push_back((trackcand[csc_index][4]-trackcand[csc_index][1])/(trackcand[csc_index][5]-trackcand[csc_index][2]));
    //       TrackL_Flag->push_back(trackcand[csc_index][6]*1000000 + trackcand[csc_index][7]*10000 + trackcand[csc_index][8]*100 + trackcand[csc_index][9]);
    //       nTracksL++;
    //     } else {
    //       TrackL_X->push_back(trackcand[tof400_index][0]);
    //       TrackL_Y->push_back(trackcand[tof400_index][1]);
    //       TrackL_Z->push_back(trackcand[tof400_index][2]);
    //       TrackL_Tx->push_back((trackcand[tof400_index][3]-trackcand[tof400_index][0])/(trackcand[tof400_index][5]-trackcand[tof400_index][2]));
    //       TrackL_Ty->push_back((trackcand[tof400_index][4]-trackcand[tof400_index][1])/(trackcand[tof400_index][5]-trackcand[tof400_index][2]));
    //       TrackL_Flag->push_back(trackcand[tof400_index][6]*1000000 + trackcand[tof400_index][7]*10000 + trackcand[tof400_index][8]*100 + trackcand[tof400_index][9]);
    //       nTracksL++;
    //     }
        
    //   }
    //}

    for (int i=0; i<CscL_nHits; i++) {
      if (CscLHitUsed.at(i)==0) continue;
      int was_csc=0;
      double trackcand[1000][10]={0};
      int ntracks=0;
      for (int j=0; j<Tof400L_nHits; j++) {
        if (Tof400LHitUsed.at(j)==0) continue;
        double d = sqrt((CscL_Tx->at(i)-Tof400L_Tx->at(j))*(CscL_Tx->at(i)-Tof400L_Tx->at(j)) + (CscL_Ty->at(i)-Tof400L_Ty->at(j))*(CscL_Ty->at(i)-Tof400L_Ty->at(j)));
        if (isnan(d)) continue;
        if (d>0.1) continue;
        trackcand[ntracks][0]=CscL_X->at(i);
        trackcand[ntracks][1]=CscL_Y->at(i);
        trackcand[ntracks][2]=CscL_Z->at(i);
        trackcand[ntracks][3]=Tof400L_X->at(j);
        trackcand[ntracks][4]=Tof400L_Y->at(j);
        trackcand[ntracks][5]=Tof400L_Z->at(j);
        trackcand[ntracks][6]=0;
        trackcand[ntracks][7]=i+1;
        trackcand[ntracks][8]=j+1;
        trackcand[ntracks][9]=0;
        ntracks++;
        if (was_csc==0){
          nTrackCandL++;
          was_csc++;
        }
        // if (run==3538 && event==1669) {
        //   cout<<" CscL_Hit "<<i<<" Tof400L_Hit "<<j<<" was_csc "<<was_csc<<"  nTrackCandL "<<nTrackCandL<<endl;
        // }
        Tof400LHitUsed.at(j)=0;
      }
      for (int j=0; j<TofCalL_nHits; j++) {
        if (TofCalLHitUsed.at(j)==0) continue;
        double dx = abs((CscL_Tx->at(i)-TofCalL_Tx->at(j)));
        double dy = abs((CscL_Ty->at(i)-TofCalL_Ty->at(j)));
        if (isnan(dx) || isnan(dy)) continue;
        if (dx>0.2) continue;
        if (dy>0.2) continue;
        trackcand[ntracks][0]=CscL_X->at(i);
        trackcand[ntracks][1]=CscL_Y->at(i);
        trackcand[ntracks][2]=CscL_Z->at(i);
        trackcand[ntracks][3]=TofCalL_X->at(j);
        // if (ntracks>0)  {trackcand[ntracks][4]=trackcand[0][4]; trackcand[ntracks][5]=trackcand[0][5];}
        // else {trackcand[ntracks][4]=-10000.; trackcand[ntracks][5]=-10000.;}
        trackcand[ntracks][4]=TofCalL_Y->at(j);
        trackcand[ntracks][5]=TofCalL_Z->at(j);
        trackcand[ntracks][6]=0;
        trackcand[ntracks][7]=i+1;
        trackcand[ntracks][8]=0;
        trackcand[ntracks][9]=j+1;
        ntracks++;
        if (was_csc==0){
          nTrackCandL++;
          was_csc++;
        }
        TofCalLHitUsed.at(j)=0;
      }

      if (nTrackCandL<=5 && ntracks>0) {
        TrackCandL_X1->push_back(trackcand[ntracks-1][0]);
        TrackCandL_Y1->push_back(trackcand[ntracks-1][1]);
        TrackCandL_Z1->push_back(trackcand[ntracks-1][2]);
        TrackCandL_X2->push_back(trackcand[ntracks-1][3]);
        TrackCandL_Y2->push_back(trackcand[ntracks-1][4]);
        TrackCandL_Z2->push_back(trackcand[ntracks-1][5]);
        TrackCandL_Flag->push_back(trackcand[ntracks-1][6]*1000000 + trackcand[ntracks-1][7]*10000 + trackcand[ntracks-1][8]*100 + trackcand[ntracks-1][9]);
      }

      if (nTrackCandL==1) {
        int tof400_index=-1;
        for (int k=ntracks-1; k>=0; k--) {
        //for (int k=0; k<ntracks; k++) {
          //if (trackcand[k][8]>0) tof400_index=trackcand[k][8]-1;
          if (trackcand[k][8]>0) tof400_index=k;
        }
        if (tof400_index<0) continue;
        TrackL_X->push_back(trackcand[tof400_index][0]);
        TrackL_Y->push_back(trackcand[tof400_index][1]);
        TrackL_Z->push_back(trackcand[tof400_index][2]);
        TrackL_Tx->push_back((trackcand[tof400_index][3]-trackcand[tof400_index][0])/(trackcand[tof400_index][5]-trackcand[tof400_index][2]));
        TrackL_Ty->push_back((trackcand[tof400_index][4]-trackcand[tof400_index][1])/(trackcand[tof400_index][5]-trackcand[tof400_index][2]));
        TrackL_Flag->push_back(trackcand[tof400_index][6]*1000000 + trackcand[tof400_index][7]*10000 + trackcand[tof400_index][8]*100 + trackcand[tof400_index][9]);
        nTracksL++;    
      }

      // if (run==3538 && event==1669) {
      //   for (int k=0; k<ntracks; k++) {
      //     cout<<" TrackCandR_X1 "<<trackcand[k][0]<<" trackcand[k][1] "<<trackcand[k][1]<<" trackcand[k][2] "<<trackcand[k][2]<<" trackcand[k][3] "<<trackcand[k][3]<<" trackcand[k][4] "<<trackcand[k][4]<<" trackcand[k][5] "<<trackcand[k][5]<<endl;
      //   }
      // }

    }

    for (int i=0; i<Tof400L_nHits; i++) {
      if (Tof400LHitUsed.at(i)==0) continue;
      int was_tof400=0;
      double trackcand[1000][10]={0};
      int ntracks=0;
      for (int j=0; j<TofCalL_nHits; j++) {
        if (TofCalLHitUsed.at(j)==0) continue;
        double dx = abs((Tof400L_Tx->at(i)-TofCalL_Tx->at(j)));
        double dy = abs((Tof400L_Ty->at(i)-TofCalL_Ty->at(j)));
        if (isnan(dx) || isnan(dy)) continue;
        if (dx>0.2) continue;
        if (dy>0.2) continue;
        trackcand[ntracks][0]=Tof400L_X->at(i);
        trackcand[ntracks][1]=Tof400L_Y->at(i);
        trackcand[ntracks][2]=Tof400L_Z->at(i);
        trackcand[ntracks][3]=TofCalL_X->at(j);
        // if (ntracks>0)  {trackcand[ntracks][4]=trackcand[0][4]; trackcand[ntracks][5]=trackcand[0][5];}
        // else {trackcand[ntracks][4]=-10000.; trackcand[ntracks][5]=-10000.;}
        trackcand[ntracks][4]=TofCalL_Y->at(j);
        trackcand[ntracks][5]=TofCalL_Z->at(j);
        trackcand[ntracks][6]=0;
        trackcand[ntracks][7]=0;
        trackcand[ntracks][8]=i+1;
        trackcand[ntracks][9]=j+1;
        ntracks++;
        if (was_tof400==0){
          nTrackCandL++;
          was_tof400++;
        }
        TofCalLHitUsed.at(j)=0;
        Tof400LHitUsed.at(i)=0;
      }
      
      if (nTrackCandL<=5 && ntracks>0) {
        TrackCandL_X1->push_back(trackcand[ntracks-1][0]);
        TrackCandL_Y1->push_back(trackcand[ntracks-1][1]);
        TrackCandL_Z1->push_back(trackcand[ntracks-1][2]);
        TrackCandL_X2->push_back(trackcand[ntracks-1][3]);
        TrackCandL_Y2->push_back(trackcand[ntracks-1][4]);
        TrackCandL_Z2->push_back(trackcand[ntracks-1][5]);
        TrackCandL_Flag->push_back(trackcand[ntracks-1][6]*1000000 + trackcand[ntracks-1][7]*10000 + trackcand[ntracks-1][8]*100 + trackcand[ntracks-1][9]);
      }

      if (nTrackCandL==1) {
        int tofcal_index=-1;
        for (int k=ntracks-1; k>=0; k--) {
        //for (int k=0; k<ntracks; k++) {
          // if (trackcand[k][9]>0) tofcal_index=trackcand[k][9]-1;
          if (trackcand[k][9]>0) tofcal_index=k;
        }
        if (tofcal_index<0) continue;
        TrackL_X->push_back(trackcand[tofcal_index][0]);
        TrackL_Y->push_back(trackcand[tofcal_index][1]);
        TrackL_Z->push_back(trackcand[tofcal_index][2]);
        TrackL_Tx->push_back((trackcand[tofcal_index][3]-trackcand[tofcal_index][0])/(trackcand[tofcal_index][5]-trackcand[tofcal_index][2]));
        TrackL_Ty->push_back((trackcand[tofcal_index][4]-trackcand[tofcal_index][1])/(trackcand[tofcal_index][5]-trackcand[tofcal_index][2]));
        TrackL_Flag->push_back(trackcand[tofcal_index][6]*1000000 + trackcand[tofcal_index][7]*10000 + trackcand[tofcal_index][8]*100 + trackcand[tofcal_index][9]);
        nTracksL++;
      }

      // if (run==3538 && event==1669) {
      //   for (int k=0; k<ntracks; k++) {
      //     cout<<" TrackCandR_X1 "<<trackcand[k][0]<<" trackcand[k][1] "<<trackcand[k][1]<<" trackcand[k][2] "<<trackcand[k][2]<<" trackcand[k][3] "<<trackcand[k][3]<<" trackcand[k][4] "<<trackcand[k][4]<<" trackcand[k][5] "<<trackcand[k][5]<<endl;
      //   }
      // }

    } 






    vector <int> CscRHitUsed;
    vector <int> Tof400RHitUsed;
    vector <int> TofCalRHitUsed;
    for (int i=0; i<CscR_nHits; i++) {CscRHitUsed.push_back(-1);}
    for (int i=0; i<Tof400R_nHits; i++) {Tof400RHitUsed.push_back(-1);}
    for (int i=0; i<TofCalR_nHits; i++) {TofCalRHitUsed.push_back(-1);}
    nTrackCandR=0;
    nTracksR=0;
    
    // for (int i=0; i<GemR_nHits; i++) {
    //   int was_gem=0;
    //   double trackcand[1000][10]={0};
    //   int ntracks=0;
    //   for (int j=0; j<CscR_nHits; j++) {
    //     if (CscRHitUsed.at(j)==0) continue;
    //     double d = sqrt((GemR_Tx->at(i)-CscR_Tx->at(j))*(GemR_Tx->at(i)-CscR_Tx->at(j)) + (GemR_Ty->at(i)-CscR_Ty->at(j))*(GemR_Ty->at(i)-CscR_Ty->at(j)));
    //     if (isnan(d)) continue;
    //     if (d>0.065) continue;
    //     trackcand[ntracks][0]=GemR_X->at(i);
    //     trackcand[ntracks][1]=GemR_Y->at(i);
    //     trackcand[ntracks][2]=GemR_Z->at(i);
    //     trackcand[ntracks][3]=CscR_X->at(j);
    //     trackcand[ntracks][4]=CscR_Y->at(j);
    //     trackcand[ntracks][5]=CscR_Z->at(j);
    //     trackcand[ntracks][6]=i+1;
    //     trackcand[ntracks][7]=j+1;
    //     trackcand[ntracks][8]=0;
    //     trackcand[ntracks][9]=0;
    //     ntracks++;
    //     if (was_gem==0){
    //       nTrackCandR++;
    //       was_gem++;
    //     }
    //     CscRHitUsed.at(j)=0;
    //   }
    //   for (int j=0; j<Tof400R_nHits; j++) {
    //     if (Tof400RHitUsed.at(j)==0) continue;
    //     double d = sqrt((GemR_Tx->at(i)-Tof400R_Tx->at(j))*(GemR_Tx->at(i)-Tof400R_Tx->at(j)) + (GemR_Ty->at(i)-Tof400R_Ty->at(j))*(GemR_Ty->at(i)-Tof400R_Ty->at(j)));
    //     if (isnan(d)) continue;
    //     if (d>0.1) continue;
    //     trackcand[ntracks][0]=GemR_X->at(i);
    //     trackcand[ntracks][1]=GemR_Y->at(i);
    //     trackcand[ntracks][2]=GemR_Z->at(i);
    //     trackcand[ntracks][3]=Tof400R_X->at(j);
    //     trackcand[ntracks][4]=Tof400R_Y->at(j);
    //     trackcand[ntracks][5]=Tof400R_Z->at(j);
    //     trackcand[ntracks][6]=i+1;
    //     trackcand[ntracks][7]=0;
    //     trackcand[ntracks][8]=j+1;
    //     trackcand[ntracks][9]=0;
    //     ntracks++;
    //     if (was_gem==0){
    //       nTrackCandR++;
    //       was_gem++;
    //     }
    //     Tof400RHitUsed.at(j)=0;
    //   }
    //   for (int j=0; j<TofCalR_nHits; j++) {
    //     if (TofCalRHitUsed.at(j)==0) continue;
    //     double dx = abs((GemR_Tx->at(i)-TofCalR_Tx->at(j)));
    //     double dy = abs((GemR_Ty->at(i)-TofCalR_Ty->at(j)));
    //     if (isnan(dx) || isnan(dy)) continue;
    //     if (dx>0.2) continue;
    //     if (dy>0.2) continue;
    //     trackcand[ntracks][0]=GemR_X->at(i);
    //     trackcand[ntracks][1]=GemR_Y->at(i);
    //     trackcand[ntracks][2]=GemR_Z->at(i);
    //     trackcand[ntracks][3]=TofCalR_X->at(j);
    //     trackcand[ntracks][4]=TofCalR_Y->at(j);
    //     trackcand[ntracks][5]=TofCalR_Z->at(j);
    //     trackcand[ntracks][6]=i+1;
    //     trackcand[ntracks][7]=0;
    //     trackcand[ntracks][8]=0;
    //     trackcand[ntracks][9]=j+1;
    //     ntracks++;
    //     if (was_gem==0){
    //       nTrackCandR++;
    //       was_gem++;
    //     }
    //     TofCalRHitUsed.at(j)=0;
    //   }

    //   if (nTrackCandR<=5 && ntracks>0) {
    //     TrackCandR_X1->push_back(trackcand[ntracks-1][0]);
    //     TrackCandR_Y1->push_back(trackcand[ntracks-1][1]);
    //     TrackCandR_Z1->push_back(trackcand[ntracks-1][2]);
    //     TrackCandR_X2->push_back(trackcand[ntracks-1][3]);
    //     TrackCandR_Y2->push_back(trackcand[ntracks-1][4]);
    //     TrackCandR_Z2->push_back(trackcand[ntracks-1][5]);
    //     TrackCandR_Flag->push_back(trackcand[ntracks-1][6]*1000000 + trackcand[ntracks-1][7]*10000 + trackcand[ntracks-1][8]*100 + trackcand[ntracks-1][9]);
    //   }

    //     if (nTrackCandR==1) {
    //     int csc_index=-1;
    //     int tof400_index=-1;
    //     for (int k=ntracks-1; k>=0; k--) {
    //     //for (int k=0; k<ntracks; k++) {
    //       // if (trackcand[k][7]>0) csc_index=trackcand[k][7]-1;
    //       // if (trackcand[k][8]>0) tof400_index=trackcand[k][8]-1;
    //       if (trackcand[k][7]>0) csc_index=k;
    //       if (trackcand[k][8]>0) tof400_index=k;
    //     }
    //     if (tof400_index<0) continue;
    //     if (csc_index>=0) {
    //       TrackR_X->push_back(trackcand[csc_index][0]);
    //       TrackR_Y->push_back(trackcand[csc_index][1]);
    //       TrackR_Z->push_back(trackcand[csc_index][2]);
    //       TrackR_Tx->push_back((trackcand[csc_index][3]-trackcand[csc_index][0])/(trackcand[csc_index][5]-trackcand[csc_index][2]));
    //       TrackR_Ty->push_back((trackcand[csc_index][4]-trackcand[csc_index][1])/(trackcand[csc_index][5]-trackcand[csc_index][2]));
    //       TrackR_Flag->push_back(trackcand[csc_index][6]*1000000 + trackcand[csc_index][7]*10000 + trackcand[csc_index][8]*100 + trackcand[csc_index][9]);
    //       nTracksR++;
    //     } else {
    //       TrackR_X->push_back(trackcand[tof400_index][0]);
    //       TrackR_Y->push_back(trackcand[tof400_index][1]);
    //       TrackR_Z->push_back(trackcand[tof400_index][2]);
    //       TrackR_Tx->push_back((trackcand[tof400_index][3]-trackcand[tof400_index][0])/(trackcand[tof400_index][5]-trackcand[tof400_index][2]));
    //       TrackR_Ty->push_back((trackcand[tof400_index][4]-trackcand[tof400_index][1])/(trackcand[tof400_index][5]-trackcand[tof400_index][2]));
    //       TrackR_Flag->push_back(trackcand[tof400_index][6]*1000000 + trackcand[tof400_index][7]*10000 + trackcand[tof400_index][8]*100 + trackcand[tof400_index][9]);
    //       nTracksR++;
    //     }
        
    //   }

    //   if (run==3538 && event==1669) {
    //     for (int k=0; k<ntracks; k++) {
    //       cout<<" TrackCandR_X1 "<<trackcand[k][0]<<" trackcand[k][1] "<<trackcand[k][1]<<" trackcand[k][2] "<<trackcand[k][2]<<" trackcand[k][3] "<<trackcand[k][3]<<" trackcand[k][4] "<<trackcand[k][4]<<" trackcand[k][5] "<<trackcand[k][5]<<endl;
    //     }
    //   }

    // }


    for (int i=0; i<CscR_nHits; i++) {
      if (CscRHitUsed.at(i)==0) continue;
      int was_csc=0;
      double trackcand[1000][10]={0};
      int ntracks=0;
      for (int j=0; j<Tof400R_nHits; j++) {
        if (Tof400RHitUsed.at(j)==0) continue;
        double d = sqrt((CscR_Tx->at(i)-Tof400R_Tx->at(j))*(CscR_Tx->at(i)-Tof400R_Tx->at(j)) + (CscR_Ty->at(i)-Tof400R_Ty->at(j))*(CscR_Ty->at(i)-Tof400R_Ty->at(j)));
        if (isnan(d)) continue;
        if (d>0.1) continue;
        trackcand[ntracks][0]=CscR_X->at(i);
        trackcand[ntracks][1]=CscR_Y->at(i);
        trackcand[ntracks][2]=CscR_Z->at(i);
        trackcand[ntracks][3]=Tof400R_X->at(j);
        trackcand[ntracks][4]=Tof400R_Y->at(j);
        trackcand[ntracks][5]=Tof400R_Z->at(j);
        trackcand[ntracks][6]=0;
        trackcand[ntracks][7]=i+1;
        trackcand[ntracks][8]=j+1;
        trackcand[ntracks][9]=0;
        ntracks++;
        if (was_csc==0){
          nTrackCandR++;
          was_csc++;
        }
        Tof400RHitUsed.at(j)=0;
      }
      for (int j=0; j<TofCalR_nHits; j++) {
        if (TofCalRHitUsed.at(j)==0) continue;
        double dx = abs((CscR_Tx->at(i)-TofCalR_Tx->at(j)));
        double dy = abs((CscR_Ty->at(i)-TofCalR_Ty->at(j)));
        if (isnan(dx) || isnan(dy)) continue;
        if (dx>0.2) continue;
        if (dy>0.2) continue;
        trackcand[ntracks][0]=CscR_X->at(i);
        trackcand[ntracks][1]=CscR_Y->at(i);
        trackcand[ntracks][2]=CscR_Z->at(i);
        trackcand[ntracks][3]=TofCalR_X->at(j);
        trackcand[ntracks][4]=TofCalR_Y->at(j);
        trackcand[ntracks][5]=TofCalR_Z->at(j);
        trackcand[ntracks][6]=0;
        trackcand[ntracks][7]=i+1;
        trackcand[ntracks][8]=0;
        trackcand[ntracks][9]=j+1;
        ntracks++;
        TofCalRHitUsed.at(j)=0;
        if (was_csc==0){
          nTrackCandR++;
          was_csc++;
        }
      }

      if (nTracksR<=5 && ntracks>0) {
        TrackCandR_X1->push_back(trackcand[ntracks-1][0]);
        TrackCandR_Y1->push_back(trackcand[ntracks-1][1]);
        TrackCandR_Z1->push_back(trackcand[ntracks-1][2]);
        TrackCandR_X2->push_back(trackcand[ntracks-1][3]);
        TrackCandR_Y2->push_back(trackcand[ntracks-1][4]);
        TrackCandR_Z2->push_back(trackcand[ntracks-1][5]);
        TrackCandR_Flag->push_back(trackcand[ntracks-1][6]*1000000 + trackcand[ntracks-1][7]*10000 + trackcand[ntracks-1][8]*100 + trackcand[ntracks-1][9]);
      }

      if (nTrackCandR==1) {
        int tof400_index=-1;
        for (int k=ntracks-1; k>=0; k--) {
        //for (int k=0; k<ntracks; k++) {
          // if (trackcand[k][8]>0) tof400_index=trackcand[k][8]-1;
          if (trackcand[k][8]>0) tof400_index=k;
        }
        if (tof400_index<0) continue;
        TrackR_X->push_back(trackcand[tof400_index][0]);
        TrackR_Y->push_back(trackcand[tof400_index][1]);
        TrackR_Z->push_back(trackcand[tof400_index][2]);
        TrackR_Tx->push_back((trackcand[tof400_index][3]-trackcand[tof400_index][0])/(trackcand[tof400_index][5]-trackcand[tof400_index][2]));
        TrackR_Ty->push_back((trackcand[tof400_index][4]-trackcand[tof400_index][1])/(trackcand[tof400_index][5]-trackcand[tof400_index][2]));
        TrackR_Flag->push_back(trackcand[tof400_index][6]*1000000 + trackcand[tof400_index][7]*10000 + trackcand[tof400_index][8]*100 + trackcand[tof400_index][9]);
        nTracksR++;    
      }

      // if (run==3538 && event==1669) {
      //   for (int k=0; k<ntracks; k++) {
      //     cout<<" TrackCandR_X1 "<<trackcand[k][0]<<" trackcand[k][1] "<<trackcand[k][1]<<" trackcand[k][2] "<<trackcand[k][2]<<" trackcand[k][3] "<<trackcand[k][3]<<" trackcand[k][4] "<<trackcand[k][4]<<" trackcand[k][5] "<<trackcand[k][5]<<endl;
      //   }
      // }

    }

    for (int i=0; i<Tof400R_nHits; i++) {
      if (Tof400RHitUsed.at(i)==0) continue;
      int was_tof400=0;
      double trackcand[1000][10]={0};
      int ntracks=0;
      for (int j=0; j<TofCalR_nHits; j++) {
        if (TofCalRHitUsed.at(j)==0) continue;
        double dx = abs((Tof400R_Tx->at(i)-TofCalR_Tx->at(j)));
        double dy = abs((Tof400R_Ty->at(i)-TofCalR_Ty->at(j)));
        if (isnan(dx) || isnan(dy)) continue;
        if (dx>0.2) continue;
        if (dy>0.2) continue;
        trackcand[ntracks][0]=Tof400R_X->at(i);
        trackcand[ntracks][1]=Tof400R_Y->at(i);
        trackcand[ntracks][2]=Tof400R_Z->at(i);
        trackcand[ntracks][3]=TofCalR_X->at(j);
        trackcand[ntracks][4]=TofCalR_Y->at(j);
        trackcand[ntracks][5]=TofCalR_Z->at(j);
        trackcand[ntracks][6]=0;
        trackcand[ntracks][7]=0;
        trackcand[ntracks][8]=i+1;
        trackcand[ntracks][9]=j+1;
        ntracks++;
        if (was_tof400==0){
          nTrackCandR++;
          was_tof400++;
        }
        TofCalRHitUsed.at(j)=0;
        Tof400RHitUsed.at(i)=0;
      }

      if (nTracksR<=5 && ntracks>0) {
        TrackCandR_X1->push_back(trackcand[ntracks-1][0]);
        TrackCandR_Y1->push_back(trackcand[ntracks-1][1]);
        TrackCandR_Z1->push_back(trackcand[ntracks-1][2]);
        TrackCandR_X2->push_back(trackcand[ntracks-1][3]);
        TrackCandR_Y2->push_back(trackcand[ntracks-1][4]);
        TrackCandR_Z2->push_back(trackcand[ntracks-1][5]);
        TrackCandR_Flag->push_back(trackcand[ntracks-1][6]*1000000 + trackcand[ntracks-1][7]*10000 + trackcand[ntracks-1][8]*100 + trackcand[ntracks-1][9]);
      }

      if (nTrackCandR==1) {
        int tofcal_index=-1;
        for (int k=ntracks-1; k>=0; k--) {
        //for (int k=0; k<ntracks; k++) {
          // if (trackcand[k][9]>0) tofcal_index=trackcand[k][9]-1;
          if (trackcand[k][9]>0) tofcal_index=k;
        }
        if (tofcal_index<0) continue;
        TrackR_X->push_back(trackcand[tofcal_index][0]);
        TrackR_Y->push_back(trackcand[tofcal_index][1]);
        TrackR_Z->push_back(trackcand[tofcal_index][2]);
        TrackR_Tx->push_back((trackcand[tofcal_index][3]-trackcand[tofcal_index][0])/(trackcand[tofcal_index][5]-trackcand[tofcal_index][2]));
        TrackR_Ty->push_back((trackcand[tofcal_index][4]-trackcand[tofcal_index][1])/(trackcand[tofcal_index][5]-trackcand[tofcal_index][2]));
        TrackR_Flag->push_back(trackcand[tofcal_index][6]*1000000 + trackcand[tofcal_index][7]*10000 + trackcand[tofcal_index][8]*100 + trackcand[tofcal_index][9]);
        nTracksR++;
      }

      // if (run==3538 && event==1669) {
      //   for (int k=0; k<ntracks; k++) {
      //     cout<<" TrackCandR_X1 "<<trackcand[k][0]<<" trackcand[k][1] "<<trackcand[k][1]<<" trackcand[k][2] "<<trackcand[k][2]<<" trackcand[k][3] "<<trackcand[k][3]<<" trackcand[k][4] "<<trackcand[k][4]<<" trackcand[k][5] "<<trackcand[k][5]<<endl;
      //   }
      // }

    }

    if (nTrackCandR!=1 || nTrackCandL!=1) {
      TrackR_X->clear();
      TrackR_Y->clear();
      TrackR_Z->clear();
      TrackR_Tx->clear();
      TrackR_Ty->clear();
      TrackR_Flag->clear();     
      nTracksR=0;
      TrackL_X->clear();
      TrackL_Y->clear();
      TrackL_Z->clear();
      TrackL_Tx->clear();
      TrackL_Ty->clear();
      TrackL_Flag->clear();    
      nTracksL=0; 
    }

    
    int vert_mdf_index=-1;
    int vertex_cand[5]={0};
    int vertex_cand_used[5]={0};
    int vertex_cand_index[100]={0};
    int ncand=0;
    VertexXY_Mult=0;
    if (nTracksR==1 && nTracksL==1) {
      VertexZ->push_back((TrackR_X->at(0)-TrackR_Tx->at(0)*TrackR_Z->at(0)-TrackL_X->at(0)+TrackL_Tx->at(0)*TrackL_Z->at(0))/(TrackL_Tx->at(0)-TrackR_Tx->at(0)));
      
      for (int i=0; i<Sil1234->nTracks; i++) {
        VertexX->push_back(Sil1234->X->at(i)-Sil1234->Tx->at(i)*(Sil1234->Z->at(i)-VertexZ->at(0)));
        VertexY->push_back(Sil1234->Y->at(i)-Sil1234->Ty->at(i)*(Sil1234->Z->at(i)-VertexZ->at(0)));
        if (Sil1234->nTracks==1){
          VertexX_match->push_back(VertexX->at(i));
          VertexY_match->push_back(VertexY->at(i));
          VertexXY_nTrMatched->push_back(1);
          VertexXY_Flag->push_back(1*pow(10, i));
          VertexXY_Mult=VertexX_match->size();
        }
        // for (int j=0; j<Pq_MDF_UpSt->size(); j++){
        //   vert_mdf_index=int(Pq_MDF_Flag->at(j)/100)-10*int(Pq_MDF_Flag->at(j)/1000);
        //   cout<<Pq_MDF_Flag->at(j)<<" "<<vert_mdf_index<<endl;
        // }
      }

      
      int counter=0;
      for (int i=0; i<Sil1234->nTracks; i++) {
        for (int j=0; j<Sil1234->nTracks; j++) {
          if (i<=j) continue;
          if (abs(VertexX->at(i)-VertexX->at(j))>0.5) continue;
          if (abs(VertexY->at(i)-VertexY->at(j))>0.5) continue;
          
          //cout<<i<<" test1  "<<j<<endl;
          int was_i=vertex_cand[i];
          int was_j=vertex_cand[j];

          if (was_i==0 && was_j==0){
            counter++;
            vertex_cand[i]=counter;
            vertex_cand[j]=counter;
            
          }
          if (was_i!=0 && was_j==0){
            vertex_cand[j]=was_i;
          }
          if (was_i==0 && was_j!=0){
            vertex_cand[i]=was_j;
          }
          if (was_i!=0 && was_j!=0){
            for (int k=0; k<5; k++){
              if (vertex_cand[k]==counter){
                vertex_cand[k]=min(was_i, was_j);
              }
            }
          }
          

        } 
      }
      int was0=0;
      int was0_n=0;

      for (int i=0; i<5; i++){
        if (vertex_cand[i]==1) {
          if (VertexX_match->size()==0){
            VertexX_match->push_back(VertexX->at(i));
            VertexY_match->push_back(VertexY->at(i));
            VertexXY_nTrMatched->push_back(1);
            VertexXY_Flag->push_back(1*pow(10, i));
            VertexXY_Mult=VertexX_match->size();
          }
          
          else {
            VertexX_match->at(0)+=VertexX->at(i);
            VertexY_match->at(0)+=VertexY->at(i);
            VertexXY_nTrMatched->at(0)++;
            VertexXY_Flag->at(0)+=1*pow(10, i);
          }
          //cout<<i<<" test2  "<<i<<endl;
        }


        if (vertex_cand[i]==2) {  
          if (VertexX_match->size()<2){
            VertexX_match->push_back(VertexX->at(i));
            VertexY_match->push_back(VertexY->at(i));
            VertexXY_nTrMatched->push_back(1);
            VertexXY_Flag->push_back(2*pow(10, i));
            VertexXY_Mult=VertexX_match->size();
          }
          
          else {
            VertexX_match->at(1)+=VertexX->at(i);
            VertexY_match->at(1)+=VertexY->at(i);
            VertexXY_nTrMatched->at(1)++;
            VertexXY_Flag->at(1)+=2*pow(10, i);
          }
          //cout<<i<<" test3  "<<i<<endl;
        }
        if (vertex_cand[i]==0) {was0=was0+pow(10, i); was0_n++;}
      }
      for (int i=0; i<VertexX_match->size(); i++){
        VertexX_match->at(i)=VertexX_match->at(i)/VertexXY_nTrMatched->at(i);
        VertexY_match->at(i)=VertexY_match->at(i)/VertexXY_nTrMatched->at(i);
      }

      //cout<<VertexXY_Mult<<" test4  "<<Sil1234->nTracks<<endl;
      // if (run==3524 && event==116379) {
      // cout<<event<<" TrackR_X->at(0) "<<TrackR_X->at(0)<<" TrackR_Y->at(0) "<<TrackR_Y->at(0)<<" TrackR_Z->at(0) "<<TrackR_Z->at(0)<<" TrackR_Tx->at(0) "<<TrackR_Tx->at(0)<<" TrackR_Ty->at(0) "<<TrackR_Ty->at(0)<<endl;
      // cout<<event<<" TrackL_X->at(0) "<<TrackL_X->at(0)<<" TrackL_Y->at(0) "<<TrackL_Y->at(0)<<" TrackL_Z->at(0) "<<TrackL_Z->at(0)<<" TrackL_Tx->at(0) "<<TrackL_Tx->at(0)<<" TrackL_Ty->at(0) "<<TrackL_Ty->at(0)<<endl;
      // cout<<event<<fixed<<" TrackR_Flag->at(0) "<<TrackR_Flag->at(0)<<" TrackL_Flag->at(0) "<<TrackL_Flag->at(0)<<endl;
      // cout<<event<<" VertexZ->at(0) "<<VertexZ->at(0)<<endl;
      
      // }
    }
    

    if (VertexZ->size()>0 && VertexXY_Mult==1) {
      

      double tof400L_X, tof400L_Y, tof400L_Z, tof400L_T, tof400R_X, tof400R_Y, tof400R_Z, tof400R_T;
      for (int i=Tof400L_nHits-1; i>-1; i--) {
        if (Tof400LHitUsed.at(i)!=0) continue;
        tof400L_X=Tof400L_X->at(i);
        tof400L_Y=Tof400L_Y->at(i);
        tof400L_Z=Tof400L_Z->at(i);
        tof400L_T=Tof400L_T->at(i);
      }
      for (int i=Tof400R_nHits-1; i>-1; i--) {
        if (Tof400RHitUsed.at(i)!=0) continue;
        tof400R_X=Tof400R_X->at(i);
        tof400R_Y=Tof400R_Y->at(i);
        tof400R_Z=Tof400R_Z->at(i);
        tof400R_T=Tof400R_T->at(i);
      }

      double c_cmns=29.9792458;
      
      double dL=sqrt(pow(tof400L_X - VertexX_match->at(0), 2) + pow(tof400L_Y - VertexY_match->at(0), 2) + pow(tof400L_Z - VertexZ->at(0), 2));
      double d0L = sqrt(pow(tof400L_X - 0, 2) + pow(tof400L_Y - 0, 2) + pow(tof400L_Z + 576.2, 2));
      double dD0L = dL - d0L;
      tof400L_T = tof400L_T + dD0L / c_cmns;
      beta_L->push_back(dL / ((tof400L_T) * c_cmns));

      double dR=sqrt(pow(tof400R_X - VertexX_match->at(0), 2) + pow(tof400R_Y - VertexY_match->at(0), 2) + pow(tof400R_Z - VertexZ->at(0), 2));
      double d0R = sqrt(pow(tof400R_X - 0, 2) + pow(tof400R_Y - 0, 2) + pow(tof400R_Z + 576.2, 2));
      double dD0R = dR - d0R;
      tof400R_T = tof400R_T + dD0R / c_cmns;
      beta_R->push_back(dR / ((tof400R_T) * c_cmns));

      double P12CperNucleon = 3.721; //Gev/c/u beam momentum

      double mp = 0.938272;
      double m11B = 11.009305 * 0.931494;
      double m10B = 10.012937 * 0.931494;
      double m10Be = 10.013535 * 0.931494;
      double m12C = 12 * 0.931494;
      double mp_bound = m12C / 12.;


      double LH2dE30cm = 0.31629 / 12.;
      double MylarWindowdE = 0.048916 / 12.;

      double FractionOfTarget = (VertexZ->at(0) + 575.6 + 15.0) / 30.0; //between 0 and 1
      if (FractionOfTarget<0) FractionOfTarget=0;
      if (FractionOfTarget>1) FractionOfTarget=1;
      
      double p = P12CperNucleon - MylarWindowdE - LH2dE30cm * FractionOfTarget;
      double C_beta = p / sqrt(p * p + mp_bound * mp_bound);

      C_betaX = 0.0;
      C_betaY = 0.0;
      C_betaZ = C_beta;
        
      if(MWPC_p0->nTracks == 1){
        C_betaZ = C_beta / sqrt(MWPC_p0->Tx->at(0) * MWPC_p0->Tx->at(0) + MWPC_p0->Ty->at(0) * MWPC_p0->Ty->at(0) + 1);
        C_betaX = MWPC_p0->Tx->at(0) * C_betaZ;
        C_betaY = MWPC_p0->Ty->at(0) * C_betaZ;
        
      }

      double gamma_L, gamma_R;

      gamma_L = 1.0 / sqrt(1.0 - beta_L->at(0) * beta_L->at(0));
      gamma_R = 1.0 / sqrt(1.0 - beta_R->at(0) * beta_R->at(0));

      PL_lab_P=gamma_L*mp*beta_L->at(0);
      PL_lab_Pz=PL_lab_P/sqrt(1+TrackL_Tx->at(0)*TrackL_Tx->at(0)+TrackL_Ty->at(0)*TrackL_Ty->at(0));
      PL_lab_Px=PL_lab_Pz*TrackL_Tx->at(0);
      PL_lab_Py=PL_lab_Pz*TrackL_Ty->at(0);
      PL_lab_E=sqrt(mp*mp+PL_lab_P*PL_lab_P);

      PR_lab_P=gamma_R*mp*beta_R->at(0);
      PR_lab_Pz=PR_lab_P/sqrt(1+TrackR_Tx->at(0)*TrackR_Tx->at(0)+TrackR_Ty->at(0)*TrackR_Ty->at(0));
      PR_lab_Px=PR_lab_Pz*TrackR_Tx->at(0);
      PR_lab_Py=PR_lab_Pz*TrackR_Ty->at(0);
      PR_lab_E=sqrt(mp*mp+PR_lab_P*PR_lab_P);

      TLorentzVector PL, PR, PTarg, PFr, PC, PL_lab, PR_lab, PTarg_lab, PFr_lab, PC_lab, Pmiss, Pmiss_ex;

      PL_lab.SetPxPyPzE(PL_lab_Px, PL_lab_Py, PL_lab_Pz, PL_lab_E);
      PR_lab.SetPxPyPzE(PR_lab_Px, PR_lab_Py, PR_lab_Pz, PR_lab_E);

      PL.SetPxPyPzE(PL_lab_Px, PL_lab_Py, PL_lab_Pz, PL_lab_E);
      PR.SetPxPyPzE(PR_lab_Px, PR_lab_Py, PR_lab_Pz, PR_lab_E);
      PL.Boost(-C_betaX, -C_betaY, -C_betaZ);
      PR.Boost(-C_betaX, -C_betaY, -C_betaZ);

      PTarg_lab.SetPxPyPzE(0, 0, 0, mp);
      PC_lab.SetPxPyPzE(0, 0, 0, m12C);
      PC_lab.Boost(C_betaX, C_betaY, C_betaZ);

      PTarg.SetPxPyPzE(0, 0, 0, mp);
      PTarg.Boost(-C_betaX, -C_betaY, -C_betaZ);
      PC.SetPxPyPzE(0, 0, 0, m12C);

      int f=-1;
      for (int i=0; i<Fragment->size(); i++){
        if (Fragment->at(i)==511 || Fragment->at(i)==510 || Fragment->at(i)==410){
          f=i;
        }
      }
      if (f>0) {
        if (Fragment->at(f)==511){
          mFr=m11B;
          PFr_lab.SetPxPyPzE(Pq_MDF_X_UpSt->at(f)*5, Pq_MDF_Y_UpSt->at(f)*5, Pq_MDF_Z_UpSt->at(f)*5, sqrt(Pq_MDF_UpSt->at(f)*Pq_MDF_UpSt->at(f)*25 + mFr*mFr)); 
          PFr.SetPxPyPzE(Pq_MDF_X_UpSt->at(f)*5, Pq_MDF_Y_UpSt->at(f)*5, Pq_MDF_Z_UpSt->at(f)*5, sqrt(Pq_MDF_UpSt->at(f)*Pq_MDF_UpSt->at(f)*25 + mFr*mFr)); 
          PFr.Boost(-C_betaX, -C_betaY, -C_betaZ);
        }
        
        if (Fragment->at(f)==510){
          mFr=m10B;
          PFr_lab.SetPxPyPzE(Pq_MDF_X_UpSt->at(f)*5, Pq_MDF_Y_UpSt->at(f)*5, Pq_MDF_Z_UpSt->at(f)*5, sqrt(Pq_MDF_UpSt->at(f)*Pq_MDF_UpSt->at(f)*25 + mFr*mFr)); 
          PFr.SetPxPyPzE(Pq_MDF_X_UpSt->at(f)*5, Pq_MDF_Y_UpSt->at(f)*5, Pq_MDF_Z_UpSt->at(f)*5, sqrt(Pq_MDF_UpSt->at(f)*Pq_MDF_UpSt->at(f)*25 + mFr*mFr)); 
          PFr.Boost(-C_betaX, -C_betaY, -C_betaZ);

        }

        if (Fragment->at(f)==410){
          mFr=m10Be;
          PFr_lab.SetPxPyPzE(Pq_MDF_X_UpSt->at(f)*4, Pq_MDF_Y_UpSt->at(f)*4, Pq_MDF_Z_UpSt->at(f)*4, sqrt(Pq_MDF_UpSt->at(f)*Pq_MDF_UpSt->at(f)*16 + mFr*mFr)); 
          PFr.SetPxPyPzE(Pq_MDF_X_UpSt->at(f)*4, Pq_MDF_Y_UpSt->at(f)*4, Pq_MDF_Z_UpSt->at(f)*4, sqrt(Pq_MDF_UpSt->at(f)*Pq_MDF_UpSt->at(f)*16 + mFr*mFr)); 
          PFr.Boost(-C_betaX, -C_betaY, -C_betaZ);
        }



        Pmiss=PL+PR-PTarg;
        if(MWPC_p0->nTracks == 1){
          Pmiss.RotateY(atan(MWPC_p0->Tx->at(0)));
          Pmiss.RotateX(atan(MWPC_p0->Ty->at(0)));
        }
        Pmiss_ex=PC+PTarg-PL-PR-PFr;
        if(MWPC_p0->nTracks == 1){
          Pmiss_ex.RotateY(atan(MWPC_p0->Tx->at(0)));
          Pmiss_ex.RotateX(atan(MWPC_p0->Ty->at(0)));
        }
        s=(PL+PR).Mag2();
        t=(PTarg-PL).Mag2();
        u=(PTarg-PR).Mag2();


        // double TrackR_Theta=atan(sqrt(tan(TrackR_Tx->at(0))*tan(TrackR_Tx->at(0))+tan(TrackR_Ty->at(0))*tan(TrackR_Ty->at(0))));
        // double TrackL_Theta=atan(sqrt(tan(TrackL_Tx->at(0))*tan(TrackL_Tx->at(0))+tan(TrackL_Ty->at(0))*tan(TrackL_Ty->at(0))));
        // double TrackR_Phi=atan(tan(TrackR_Ty->at(0))/tan(TrackR_Tx->at(0)));
        // double TrackL_Phi=atan(tan(TrackL_Ty->at(0))/tan(TrackL_Tx->at(0)));

        double TrackR_Theta=atan(sqrt(TrackR_Tx->at(0)*TrackR_Tx->at(0)+TrackR_Ty->at(0)*TrackR_Ty->at(0)));
        double TrackL_Theta=atan(sqrt(TrackL_Tx->at(0)*TrackL_Tx->at(0)+TrackL_Ty->at(0)*TrackL_Ty->at(0)));
        double TrackR_Phi=sign(TrackR_Ty->at(0))*acos(-1.)*abs((sign(TrackR_Tx->at(0))-1))/2+atan(TrackR_Ty->at(0)/TrackR_Tx->at(0));
        double TrackL_Phi=sign(TrackL_Ty->at(0))*acos(-1.)*abs((sign(TrackL_Tx->at(0))-1))/2+atan(TrackL_Ty->at(0)/TrackL_Tx->at(0));

        OpAn_Th=(TrackR_Theta+TrackL_Theta)/acos(-1.)*180;  //(PL_lab.Theta()+PR_lab.Theta())/acos(-1.)*180;
        OpAn_Phi=(TrackL_Phi-TrackR_Phi)/acos(-1.)*180; //(PL_lab.Phi()+PR_lab.Phi())/acos(-1.)*180;
            
        PL_P=PL.P();
        PL_Pz=PL.Px();
        PL_Px=PL.Py();
        PL_Py=PL.Pz();
        PL_E=PL.E();

        PR_P=PR.P();
        PR_Pz=PR.Px();
        PR_Px=PR.Py();
        PR_Py=PR.Pz();
        PR_E=PR.E();

        PC_P=PC.P();
        PC_Pz=PC.Px();
        PC_Px=PC.Py();
        PC_Py=PC.Pz();
        PC_E=PC.E();

        PC_lab_P=PC_lab.P();
        PC_lab_Pz=PC_lab.Px();
        PC_lab_Px=PC_lab.Py();
        PC_lab_Py=PC_lab.Pz();
        PC_lab_E=PC_lab.E();

        PTarg_P=PTarg.P();
        PTarg_Pz=PTarg.Px();
        PTarg_Px=PTarg.Py();
        PTarg_Py=PTarg.Pz();
        PTarg_E=PTarg.E();

        PTarg_lab_P=PTarg_lab.P();
        PTarg_lab_Pz=PTarg_lab.Px();
        PTarg_lab_Px=PTarg_lab.Py();
        PTarg_lab_Py=PTarg_lab.Pz();
        PTarg_lab_E=PTarg_lab.E();

        PFr_P=PFr.P();
        PFr_Pz=PFr.Px();
        PFr_Px=PFr.Py();
        PFr_Py=PFr.Pz();
        PFr_E=PFr.E();

        PFr_lab_P=PFr_lab.P();
        PFr_lab_Pz=PFr_lab.Px();
        PFr_lab_Px=PFr_lab.Py();
        PFr_lab_Py=PFr_lab.Pz();
        PFr_lab_E=PFr_lab.E();

        Pmiss_P=Pmiss.P();
        Pmiss_Pz=Pmiss.Px();
        Pmiss_Px=Pmiss.Py();
        Pmiss_Py=Pmiss.Pz();
        Pmiss_E=mp-Pmiss.E(); //the same as Goran did
        Mmiss2=Pmiss.Mag2();

        Pmiss_ex_P=Pmiss_ex.P();
        Pmiss_ex_Pz=Pmiss_ex.Px();
        Pmiss_ex_Px=Pmiss_ex.Py();
        Pmiss_ex_Py=Pmiss_ex.Pz();
        Pmiss_ex_E=Pmiss_ex.E();
        Mmiss2_ex=Pmiss_ex.Mag2();

        if ((OpAn_Th - 65.6)*(OpAn_Th - 65.6)/(2 * 2.22 * 2 * 2.22) +  (Mmiss2 - 0.896)*(Mmiss2 - 0.896)/(1.1* 0.232 * 1.1* 0.232)  < 1) QE_cut=1; 
        else QE_cut=0; 
        if (beta_L->at(0)<0.98 && beta_R->at(0)<0.98 && beta_L->at(0)>0.8 && beta_R->at(0)>0.8 ) b_cut=1;
        else b_cut=0; 
        if (VertexZ->at(0) > -589.2 && VertexZ->at(0) < -559.2 && (VertexX_match->at(0) - 0.5)*(VertexX_match->at(0) - 0.5) + VertexY_match->at(0)*VertexY_match->at(0) < 9) vert_cut=1; //??
        else vert_cut=0;
        if (!(tof400L_X>85 && tof400L_X<152 && tof400L_Y>-22 && tof400L_Y<5) || !(tof400R_X<-131 && tof400R_Y>-14.5 && tof400R_Y<37)) t400_cut=1;
        else t400_cut=0;
      }

    }

    for (int i=0; i<beta_L->size(); i++) {
      if (beta_L->at(i)>0 && beta_L->at(i)<1.5) is_good_beta_L->push_back(1);
      else is_good_beta_L->push_back(0);
    }
    for (int i=0; i<beta_R->size(); i++) {
      if (beta_R->at(i)>0 && beta_R->at(i)<1.5) is_good_beta_R->push_back(1);
      else is_good_beta_R->push_back(0);
    }







    

    tree1->Fill();

    Pq_MDF_UpSt->clear();
    Pq_MDF_X_UpSt->clear();
    Pq_MDF_Y_UpSt->clear();
    Pq_MDF_Z_UpSt->clear();
    Tx_AT->clear();
    Ty_AT->clear();
    Pq_MDF_Flag->clear();
    Tx_MDF_UpSt->clear();
    Fragment->clear();

    SW_ELoss->clear();
    SW_CellId->clear();
    SW_Flag->clear();
    SW_TrId->clear();
    SW_OrderId->clear();

    VB_CellId->clear();

    dCSCH_cscX->clear(); 
    dCSCH_cscY->clear(); 
    dCSCH_cscZ->clear();

    Sil1234->X->clear();
    Sil1234->Y->clear();
    Sil1234->Z->clear();
    Sil1234->Tx->clear();
    Sil1234->Ty->clear();
    Sil1234->Chi2X->clear();
    Sil1234->Chi2Y->clear();
    Sil1234->IDX->clear();
    Sil1234->IDY->clear();
    //Sil1234->Flag->clear();


    MWPC1->X->clear();
    MWPC1->Y->clear();
    MWPC1->Z->clear();
    MWPC1->Tx->clear();
    MWPC1->Ty->clear();
    MWPC1->Chi2X->clear();
    //MWPC1->Chi2Y->clear();

    MWPC2->X->clear();
    MWPC2->Y->clear();
    MWPC2->Z->clear();
    MWPC2->Tx->clear();
    MWPC2->Ty->clear();
    MWPC2->Chi2X->clear();
    //MWPC2->Chi2Y->clear();

    MWPC3->X->clear();
    MWPC3->Y->clear();
    MWPC3->Z->clear();
    MWPC3->Tx->clear();
    MWPC3->Ty->clear();
    MWPC3->Chi2X->clear();
    //MWPC3->Chi2Y->clear();

    MWPC4->X->clear();
    MWPC4->Y->clear();
    MWPC4->Z->clear();
    MWPC4->Tx->clear();
    MWPC4->Ty->clear();
    MWPC4->Chi2X->clear();
    //MWPC4->Chi2Y->clear();

    MWPC_p0->X->clear();
    MWPC_p0->Y->clear();
    MWPC_p0->Z->clear();
    MWPC_p0->Tx->clear();
    MWPC_p0->Ty->clear();
    MWPC_p0->Chi2X->clear();
    //MWPC_p0->Chi2Y->clear();

    MWPC_p1->X->clear();
    MWPC_p1->Y->clear();
    MWPC_p1->Z->clear();
    MWPC_p1->Tx->clear();
    MWPC_p1->Ty->clear();
    MWPC_p1->Chi2X->clear();
    //MWPC_p1->Chi2Y->clear();


    DCH->X->clear();
		DCH->Y->clear();
		DCH->Z->clear();
		DCH->Tx->clear();
		DCH->Ty->clear(); 
    
    
    DCH1->X->clear();
		DCH1->Y->clear();
		DCH1->Z->clear();
		DCH1->Tx->clear();
		DCH1->Ty->clear(); 

    DCH2->X->clear();
		DCH2->Y->clear();
		DCH2->Z->clear();
		DCH2->Tx->clear();
		DCH2->Ty->clear(); 

    
    GemL_X->clear();
    GemL_Y->clear();
    GemL_Z->clear();
    GemL_Tx->clear();
    GemL_Ty->clear();

    GemR_X->clear();
    GemR_Y->clear();
    GemR_Z->clear();
    GemR_Tx->clear();
    GemR_Ty->clear();

    CscL_X->clear();
    CscL_Y->clear();
    CscL_Z->clear();
    CscL_Tx->clear();
    CscL_Ty->clear();

    CscR_X->clear();
    CscR_Y->clear();
    CscR_Z->clear();
    CscR_Tx->clear();
    CscR_Ty->clear();

    Tof400L_X->clear();
    Tof400L_Y->clear();
    Tof400L_Z->clear();
    Tof400L_Tx->clear();
    Tof400L_Ty->clear();
    Tof400L_T->clear();
    Tof400L_DetId->clear();

    Tof400R_X->clear();
    Tof400R_Y->clear();
    Tof400R_Z->clear();
    Tof400R_Tx->clear();
    Tof400R_Ty->clear();
    Tof400R_T->clear();
    Tof400R_DetId->clear();

    TofCalL_X->clear();
    TofCalL_Y->clear();
    TofCalL_Z->clear();
    TofCalL_Tx->clear();
    TofCalL_Ty->clear();
    TofCalL_T->clear();
    TofCalL_Bar->clear();
    TofCalL_Layer->clear();

    TofCalR_X->clear();
    TofCalR_Y->clear();
    TofCalR_Z->clear();
    TofCalR_Tx->clear();
    TofCalR_Ty->clear();
    TofCalR_T->clear();
    TofCalR_Bar->clear();
    TofCalR_Layer->clear();

    TrackL_X->clear();
    TrackL_Y->clear();
    TrackL_Z->clear();
    TrackL_Tx->clear();
    TrackL_Ty->clear();
    TrackL_Flag->clear();

    TrackR_X->clear();
    TrackR_Y->clear();
    TrackR_Z->clear();
    TrackR_Tx->clear();
    TrackR_Ty->clear();
    TrackR_Flag->clear();

    TrackCandL_X1->clear();
    TrackCandL_Y1->clear();
    TrackCandL_Z1->clear();
    TrackCandL_X2->clear();
    TrackCandL_Y2->clear();
    TrackCandL_Z2->clear();
    TrackCandL_Flag->clear();

    TrackCandR_X1->clear();
    TrackCandR_Y1->clear();
    TrackCandR_Z1->clear();
    TrackCandR_X2->clear();
    TrackCandR_Y2->clear();
    TrackCandR_Z2->clear();
    TrackCandR_Flag->clear();

    VertexX_match->clear();
    VertexY_match->clear();
    VertexXY_nTrMatched->clear();
    VertexXY_Flag->clear();
    VertexX->clear();
    VertexY->clear();
    VertexZ->clear();
    beta_L->clear();
    beta_R->clear();
    is_good_beta_L->clear();
    is_good_beta_R->clear();

   

  } 
  //cout<<NEv<<"  "<<N_non0_DCH_Upst<<" "<<N_non0_Pq<<" "<<N_double_Pq<<" "<<N_double_Pq_ev<<endl;
  treefile->cd();
	//tree1->SetDirectory(treefile);
	tree1->Write();
	treefile->Close();
	
}

