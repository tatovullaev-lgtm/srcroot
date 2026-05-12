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

//#include <stdio.h>
//#include <iostream>
//#include <iomanip>
//#include <stdlib.h>
//#include <Math.h>
//
//
//#include "TLorentzVector.h"
//#include "TVector3.h"
//#include "TMath.h"
//#include "Math/Vector3D.h"
//#include "Math/Vector4D.h"
//#include "TVectorD.h"
//
//#include <TROOT.h> 
//#include <vector>
//#include <TFile.h>
//#include "Riostream.h"
//#ifndef CLONESA_EVENT_SECOND_RUN
//#include "Math/Vector4D.h"
//
//#include <TTree.h>


//Z coordinates for all modules all stations
const double ZtargPos=-574.91;
const double Ztarg[4][2]={
  {123.545+ZtargPos, 122.79+ZtargPos}, 
  {127.825+ZtargPos, 128.55+ZtargPos},
  {175.585+ZtargPos, 174.83+ZtargPos},
  {179.82+ZtargPos, 180.555+ZtargPos}
};
const double SiTrackZPos=151.69+ZtargPos;


using namespace ROOT::Math;

struct Inf {  

  vector <Double_t> *X=nullptr; 
  vector <Double_t> *Y=nullptr;
  vector <Double_t> *Z=nullptr;
  vector <Double_t> *Tx=nullptr;
  vector <Double_t> *Ty=nullptr;
  vector <Int_t> *Prim=nullptr;
  vector <Double_t> *Charge=nullptr;
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
  vector <Int_t> *MId=nullptr; 
  vector <Int_t> *Prim=nullptr;
  vector <Double_t> *Charge=nullptr;
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


struct Notes {
  int ev; 
  int st;
  int mod; 
  double X;
  double Y;
  int L0NHits;
  int L1NHits;
  int NHits;
};

void GetFitParam (vector <long double> x, vector <long double> z, vector <long double> sigma, long double &a, long double &b, long double &Chi2);



void Si_MWPC_Tracks_Simulation() {

	printf("start read");


  double CutDX=0.5;
  double CutDY=0.5;
  // double CutDX=50;
  // double CutDY=50;
  double CutVX=5;
  double CutVY=5;
  double CutChi2X=5;
  double CutChi2Y=5;
  double sigma_S1=0.0029;
  double sigma_S2=0.0029;
  double sigma_S3=0.0029;
  double sigma_S4=0.0029;
  double sigma_M3=0.0417;
  double sigma_M4=0.0417;


  TString filename ="/home/srcuser3/bmnroot/macro/run/SiAligMWPCTracks_Sim.root";
  //TString filename ="/eos/nica/src/users/mpatsyuk/data/2022/Timur_Test/Upstream_Tracks_" + to_string(filenum) + ".root";
  //TString filename ="/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiAligMWPCTracks_4780_4799_wo_ampcut_Chi2_10.root";
  //TString filename ="/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiAligMWPCTracks_4780_4799_ampcut_1700.root";

	TFile *treefile = new TFile(filename, "recreate");
	//output tree and its branches
	TTree *tree1 = new TTree("tree1","treelibrated tree");
	//tree1->SetDirectory(treefile);
  int event, UnReactC;
   
  vector <double> dX43; 
  vector <double> dY43; 
  
  Inf *Sil1H = new Inf;
  Inf *Sil2H = new Inf;
  Inf *Sil3H = new Inf;
  Inf *Sil4H = new Inf;
  Inf *Sil1HU = new Inf;
  Inf *Sil2HU = new Inf;
  Inf *Sil3HU = new Inf;
  Inf *Sil4HU = new Inf;
  Inf *Sil1HF = new Inf;
  Inf *Sil2HF = new Inf;
  Inf *Sil3HF = new Inf;
  Inf *Sil4HF = new Inf;
  Inf *Sil1H_MC = new Inf;
  Inf *Sil2H_MC = new Inf;
  Inf *Sil3H_MC = new Inf;
  Inf *Sil4H_MC = new Inf;

  TBranch *br_event =tree1->Branch("event",&event);
  TBranch *br_run =tree1->Branch("UnReactC",&UnReactC);
  

  TBranch *br_dX43 =tree1->Branch("dX43",&dX43);
  TBranch *br_dY43 =tree1->Branch("dY43",&dY43);
  
  
  Track *MC = new Track;
  TBranch *br_MC_X =tree1->Branch("MC_X",&MC->X);
  TBranch *br_MC_Y =tree1->Branch("MC_Y",&MC->Y);
  TBranch *br_MC_Z =tree1->Branch("MC_Z",&MC->Z);
  TBranch *br_MC_Tx =tree1->Branch("MC_Tx",&MC->Tx);
  TBranch *br_MC_Ty =tree1->Branch("MC_Ty",&MC->Ty);
  TBranch *br_MC_MId =tree1->Branch("MC_MId",&MC->MId);
  TBranch *br_MC_Chi2X =tree1->Branch("MC_Chi2",&MC->Chi2X);
  // TBranch *br_MC_Chi2Y =tree1->Branch("MC_Chi2Y",&MC->Chi2Y);
  TBranch *br_MC_nTracks =tree1->Branch("MC_nTracks",&MC->nTracks);

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

  Track *MWPC3F = new Track;
  TBranch *br_MWPC3F_X =tree1->Branch("MWPC3F_X",&MWPC3F->X);
  TBranch *br_MWPC3F_Y =tree1->Branch("MWPC3F_Y",&MWPC3F->Y);
  TBranch *br_MWPC3F_Z =tree1->Branch("MWPC3F_Z",&MWPC3F->Z);
  TBranch *br_MWPC3F_Tx =tree1->Branch("MWPC3F_Tx",&MWPC3F->Tx);
  TBranch *br_MWPC3F_Ty =tree1->Branch("MWPC3F_Ty",&MWPC3F->Ty);
  TBranch *br_MWPC3F_Chi2X =tree1->Branch("MWPC3F_Chi2",&MWPC3F->Chi2X);
  // TBranch *br_MWPC3F_Chi2Y =tree1->Branch("MWPC3F_Chi2Y",&MWPC3F->Chi2Y);
  TBranch *br_MWPC3F_nTracks =tree1->Branch("MWPC3F_nTracks",&MWPC3F->nTracks);

  Track *MWPC4F = new Track;
  TBranch *br_MWPC4F_X =tree1->Branch("MWPC4F_X",&MWPC4F->X);
  TBranch *br_MWPC4F_Y =tree1->Branch("MWPC4F_Y",&MWPC4F->Y);
  TBranch *br_MWPC4F_Z =tree1->Branch("MWPC4F_Z",&MWPC4F->Z);
  TBranch *br_MWPC4F_Tx =tree1->Branch("MWPC4F_Tx",&MWPC4F->Tx);
  TBranch *br_MWPC4F_Ty =tree1->Branch("MWPC4F_Ty",&MWPC4F->Ty);
  TBranch *br_MWPC4F_Chi2X =tree1->Branch("MWPC4F_Chi2",&MWPC4F->Chi2X);
  // TBranch *br_MWPC4F_Chi2Y =tree1->Branch("MWPC4F_Chi2Y",&MWPC4F->Chi2Y);
  TBranch *br_MWPC4F_nTracks =tree1->Branch("MWPC4F_nTracks",&MWPC4F->nTracks);

  Track *MWPC3U = new Track;
  TBranch *br_MWPC3U_X =tree1->Branch("MWPC3U_X",&MWPC3U->X);
  TBranch *br_MWPC3U_Y =tree1->Branch("MWPC3U_Y",&MWPC3U->Y);
  TBranch *br_MWPC3U_Z =tree1->Branch("MWPC3U_Z",&MWPC3U->Z);
  TBranch *br_MWPC3U_Tx =tree1->Branch("MWPC3U_Tx",&MWPC3U->Tx);
  TBranch *br_MWPC3U_Ty =tree1->Branch("MWPC3U_Ty",&MWPC3U->Ty);
  TBranch *br_MWPC3U_Chi2X =tree1->Branch("MWPC3U_Chi2",&MWPC3U->Chi2X);
  // TBranch *br_MWPC3U_Chi2Y =tree1->Branch("MWPC3U_Chi2Y",&MWPC3U->Chi2Y);
  TBranch *br_MWPC3U_nTracks =tree1->Branch("MWPC3U_nTracks",&MWPC3U->nTracks);

  Track *MWPC4U = new Track;
  TBranch *br_MWPC4U_X =tree1->Branch("MWPC4U_X",&MWPC4U->X);
  TBranch *br_MWPC4U_Y =tree1->Branch("MWPC4U_Y",&MWPC4U->Y);
  TBranch *br_MWPC4U_Z =tree1->Branch("MWPC4U_Z",&MWPC4U->Z);
  TBranch *br_MWPC4U_Tx =tree1->Branch("MWPC4U_Tx",&MWPC4U->Tx);
  TBranch *br_MWPC4U_Ty =tree1->Branch("MWPC4U_Ty",&MWPC4U->Ty);
  TBranch *br_MWPC4U_Chi2X =tree1->Branch("MWPC4U_Chi2",&MWPC4U->Chi2X);
  // TBranch *br_MWPC4U_Chi2Y =tree1->Branch("MWPC4U_Chi2Y",&MWPC4U->Chi2Y);
  TBranch *br_MWPC4U_nTracks =tree1->Branch("MWPC4U_nTracks",&MWPC4U->nTracks);

  Track *MWPC1_MC = new Track;
  TBranch *br_MWPC1_X_MC =tree1->Branch("MWPC1_X_MC",&MWPC1_MC->X);
  TBranch *br_MWPC1_Y_MC =tree1->Branch("MWPC1_Y_MC",&MWPC1_MC->Y);
  TBranch *br_MWPC1_Z_MC =tree1->Branch("MWPC1_Z_MC",&MWPC1_MC->Z);
  TBranch *br_MWPC1_Charge_MC =tree1->Branch("MWPC1_Charge_MC",&MWPC1_MC->Charge);
  TBranch *br_MWPC1_Prim_MC =tree1->Branch("MWPC1_Prim_MC",&MWPC1_MC->Prim);
  TBranch *br_MWPC1_Tx_MC =tree1->Branch("MWPC1_Tx_MC",&MWPC1_MC->Tx);
  TBranch *br_MWPC1_Ty_MC =tree1->Branch("MWPC1_Ty_MC",&MWPC1_MC->Ty);
  TBranch *br_MWPC1_Chi2X_MC =tree1->Branch("MWPC1_Chi2_MC",&MWPC1_MC->Chi2X);
  // TBranch *br_MWPC3_Chi2Y_MC =tree1->Branch("MWPC1_Chi2Y_MC",&MWPC1_MC->Chi2Y);
  TBranch *br_MWPC1_nTracks_MC =tree1->Branch("MWPC1_nTracks_MC",&MWPC1_MC->nTracks);

  Track *MWPC2_MC = new Track;
  TBranch *br_MWPC2_X_MC =tree1->Branch("MWPC2_X_MC",&MWPC2_MC->X);
  TBranch *br_MWPC2_Y_MC =tree1->Branch("MWPC2_Y_MC",&MWPC2_MC->Y);
  TBranch *br_MWPC2_Z_MC =tree1->Branch("MWPC2_Z_MC",&MWPC2_MC->Z);
  TBranch *br_MWPC2_Charge_MC =tree1->Branch("MWPC2_Charge_MC",&MWPC2_MC->Charge);
  TBranch *br_MWPC2_Prim_MC =tree1->Branch("MWPC2_Prim_MC",&MWPC2_MC->Prim);
  TBranch *br_MWPC2_Tx_MC =tree1->Branch("MWPC2_Tx_MC",&MWPC2_MC->Tx);
  TBranch *br_MWPC2_Ty_MC =tree1->Branch("MWPC2_Ty_MC",&MWPC2_MC->Ty);
  TBranch *br_MWPC2_Chi2X_MC =tree1->Branch("MWPC2_Chi2_MC",&MWPC2_MC->Chi2X);
  // TBranch *br_MWPC3_Chi2Y_MC =tree1->Branch("MWPC2_Chi2Y_MC",&MWPC2_MC->Chi2Y);
  TBranch *br_MWPC2_nTracks_MC =tree1->Branch("MWPC2_nTracks_MC",&MWPC2_MC->nTracks);

  Track *MWPC3_MC = new Track;
  TBranch *br_MWPC3_X_MC =tree1->Branch("MWPC3_X_MC",&MWPC3_MC->X);
  TBranch *br_MWPC3_Y_MC =tree1->Branch("MWPC3_Y_MC",&MWPC3_MC->Y);
  TBranch *br_MWPC3_Z_MC =tree1->Branch("MWPC3_Z_MC",&MWPC3_MC->Z);
  TBranch *br_MWPC3_Charge_MC =tree1->Branch("MWPC3_Charge_MC",&MWPC3_MC->Charge);
  TBranch *br_MWPC3_Prim_MC =tree1->Branch("MWPC3_Prim_MC",&MWPC3_MC->Prim);
  TBranch *br_MWPC3_Tx_MC =tree1->Branch("MWPC3_Tx_MC",&MWPC3_MC->Tx);
  TBranch *br_MWPC3_Ty_MC =tree1->Branch("MWPC3_Ty_MC",&MWPC3_MC->Ty);
  TBranch *br_MWPC3_Chi2X_MC =tree1->Branch("MWPC3_Chi2_MC",&MWPC3_MC->Chi2X);
  // TBranch *br_MWPC3_Chi2Y_MC =tree1->Branch("MWPC3_Chi2Y_MC",&MWPC3_MC->Chi2Y);
  TBranch *br_MWPC3_nTracks_MC =tree1->Branch("MWPC3_nTracks_MC",&MWPC3_MC->nTracks);
  

  Track *MWPC4_MC = new Track;
  TBranch *br_MWPC4_X_MC =tree1->Branch("MWPC4_X_MC",&MWPC4_MC->X);
  TBranch *br_MWPC4_Y_MC =tree1->Branch("MWPC4_Y_MC",&MWPC4_MC->Y);
  TBranch *br_MWPC4_Z_MC =tree1->Branch("MWPC4_Z_MC",&MWPC4_MC->Z);
  TBranch *br_MWPC4_Charge_MC =tree1->Branch("MWPC4_Charge_MC",&MWPC4_MC->Charge);
  TBranch *br_MWPC4_Prim_MC =tree1->Branch("MWPC4_Prim_MC",&MWPC4_MC->Prim);
  TBranch *br_MWPC4_Tx_MC =tree1->Branch("MWPC4_Tx_MC",&MWPC4_MC->Tx);
  TBranch *br_MWPC4_Ty_MC =tree1->Branch("MWPC4_Ty_MC",&MWPC4_MC->Ty);
  TBranch *br_MWPC4_Chi2X_MC =tree1->Branch("MWPC4_Chi2_MC",&MWPC4_MC->Chi2X);
  // TBranch *br_MWPC4_Chi2Y_MC =tree1->Branch("MWPC4_Chi2Y_MC",&MWPC4_MC->Chi2Y);
  TBranch *br_MWPC4_nTracks_MC =tree1->Branch("MWPC4_nTracks_MC",&MWPC4_MC->nTracks);


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
  TBranch *br_Sil123_DX12 =tree1->Branch("Si1234M34_DX12",&Sil1234->DX12);
  TBranch *br_Sil123_DY12 =tree1->Branch("Si1234M34_DY12",&Sil1234->DY12);
  TBranch *br_Sil123_DX34 =tree1->Branch("Si1234M34_DX34",&Sil1234->DX34);
  TBranch *br_Sil123_DY34 =tree1->Branch("Si1234M34_DY34",&Sil1234->DY34);
  TBranch *br_Sil1234_nTracks =tree1->Branch("Si1234M34_nTracks",&Sil1234->nTracks);
  TBranch *br_Sil1234_nTracksX =tree1->Branch("Si1234M34_nTracksX",&Sil1234->nTracksX);
  TBranch *br_Sil1234_nTracksY =tree1->Branch("Si1234M34_nTracksY",&Sil1234->nTracksY);
  TBranch *br_Sil1234_Flag =tree1->Branch("Si1234M34_Flag",&Sil1234->Flag);

  Track *Sil1234MWPC34 = new Track;
  TBranch *br_Sil1234MWPC34_X =tree1->Branch("Si1234_X",&Sil1234MWPC34->X);
  TBranch *br_Sil1234MWPC34_Y =tree1->Branch("Si1234_Y",&Sil1234MWPC34->Y);
  TBranch *br_Sil1234MWPC34_Z =tree1->Branch("Si1234_Z",&Sil1234MWPC34->Z);
  TBranch *br_Sil1234MWPC34_Tx =tree1->Branch("Si1234_Tx",&Sil1234MWPC34->Tx);
  TBranch *br_Sil1234MWPC34_Ty =tree1->Branch("Si1234_Ty",&Sil1234MWPC34->Ty);
  TBranch *br_Sil1234MWPC34_Chi2X =tree1->Branch("Si1234_Chi2X",&Sil1234MWPC34->Chi2X);
  TBranch *br_Sil1234MWPC34_Chi2Y =tree1->Branch("Si1234_Chi2Y",&Sil1234MWPC34->Chi2Y);
  TBranch *br_Sil1234MWPC34_IDX =tree1->Branch("Si1234_IDX",&Sil1234MWPC34->IDX);
  TBranch *br_Sil1234MWPC34_IDY =tree1->Branch("Si1234_IDY",&Sil1234MWPC34->IDY);
  TBranch *br_Sil1234MWPC34_caseX =tree1->Branch("Si1234_caseX",&Sil1234MWPC34->caseX);
  TBranch *br_Sil1234MWPC34_caseY =tree1->Branch("Si1234_caseY",&Sil1234MWPC34->caseY);
  TBranch *br_Sil1234MWPC34_nTracks =tree1->Branch("Si1234_nTracks",&Sil1234MWPC34->nTracks);
  TBranch *br_Sil1234MWPC34_nTracksX =tree1->Branch("Si1234_nTracksX",&Sil1234MWPC34->nTracksX);
  TBranch *br_Sil1234MWPC34_nTracksY =tree1->Branch("Si1234_nTracksY",&Sil1234MWPC34->nTracksY);
  TBranch *br_Sil1234MWPC34_Flag =tree1->Branch("Si1234_Flag",&Sil1234MWPC34->Flag);


  TBranch *br_Si1H_X=tree1->Branch("Si1H_X",&Sil1H->X);
  TBranch *br_Si1H_Y=tree1->Branch("Si1H_Y",&Sil1H->Y);
  TBranch *br_Si1H_St=tree1->Branch("Si1H_St",&Sil1H->St);
  TBranch *br_Si1H_Mod=tree1->Branch("Si1H_Mod",&Sil1H->Mod);
  TBranch *br_Sil1H_L0H =tree1->Branch("Si1H_L0NHits",&Sil1H->L0H);
  TBranch *br_Sil1H_L1H =tree1->Branch("Si1H_L1NHits",&Sil1H->L1H);
  TBranch *br_Sil1H_AmpDir =tree1->Branch("Si1H_AmpDir",&Sil1H->AmpDir);
  TBranch *br_Sil1H_AmpIncl =tree1->Branch("Si1H_AmpIncl",&Sil1H->AmpIncl);
  TBranch *br_Si1H_nHits=tree1->Branch("Si1H_nHits",&Sil1H->nHits);

  TBranch *br_Si1HF_X=tree1->Branch("Si1HF_X",&Sil1HF->X);
  TBranch *br_Si1HF_Y=tree1->Branch("Si1HF_Y",&Sil1HF->Y);
  TBranch *br_Si1HF_St=tree1->Branch("Si1HF_St",&Sil1HF->St);
  TBranch *br_Si1HF_Mod=tree1->Branch("Si1HF_Mod",&Sil1HF->Mod);
  TBranch *br_Sil1HF_L0H =tree1->Branch("Si1HF_L0NHits",&Sil1HF->L0H);
  TBranch *br_Sil1HF_L1H =tree1->Branch("Si1HF_L1NHits",&Sil1HF->L1H);
  TBranch *br_Sil1HF_AmpDir =tree1->Branch("Si1HF_AmpDir",&Sil1HF->AmpDir);
  TBranch *br_Sil1HF_AmpIncl =tree1->Branch("Si1HF_AmpIncl",&Sil1HF->AmpIncl);
  TBranch *br_Si1HF_nHits=tree1->Branch("Si1HF_nHits",&Sil1HF->nHits);

  TBranch *br_Si1HU_X=tree1->Branch("Si1HU_X",&Sil1HU->X);
  TBranch *br_Si1HU_Y=tree1->Branch("Si1HU_Y",&Sil1HU->Y);
  TBranch *br_Si1HU_St=tree1->Branch("Si1HU_St",&Sil1HU->St);
  TBranch *br_Si1HU_Mod=tree1->Branch("Si1HU_Mod",&Sil1HU->Mod);
  TBranch *br_Sil1HU_L0H =tree1->Branch("Si1HU_L0NHits",&Sil1HU->L0H);
  TBranch *br_Sil1HU_L1H =tree1->Branch("Si1HU_L1NHits",&Sil1HU->L1H);
  TBranch *br_Sil1HU_AmpDir =tree1->Branch("Si1HU_AmpDir",&Sil1HU->AmpDir);
  TBranch *br_Sil1HU_AmpIncl =tree1->Branch("Si1HU_AmpIncl",&Sil1HU->AmpIncl);
  TBranch *br_Si1HU_nHits=tree1->Branch("Si1HU_nHits",&Sil1HU->nHits);

  TBranch *br_Si1H_X_MC=tree1->Branch("Si1H_X_MC",&Sil1H_MC->X);
  TBranch *br_Si1H_Y_MC=tree1->Branch("Si1H_Y_MC",&Sil1H_MC->Y);
  TBranch *br_Si1H_Z_MC=tree1->Branch("Si1H_Z_MC",&Sil1H_MC->Z);
  TBranch *br_Si1H_Tx_MC=tree1->Branch("Si1H_Tx_MC",&Sil1H_MC->Tx);
  TBranch *br_Si1H_Ty_MC=tree1->Branch("Si1H_Ty_MC",&Sil1H_MC->Ty);
  TBranch *br_Si1H_Charge_MC =tree1->Branch("Si1H_Charge_MC",&Sil1H_MC->Charge);
  TBranch *br_Si1H_Prim_MC =tree1->Branch("Si1H_Prim_MC",&Sil1H_MC->Prim);
  TBranch *br_Si1H_St_MC=tree1->Branch("Si1H_St_MC",&Sil1H_MC->St);
  TBranch *br_Si1H_Mod_MC=tree1->Branch("Si1H_Mod_MC",&Sil1H_MC->Mod);
  TBranch *br_Sil1H_L0H_MC =tree1->Branch("Si1H_L0NHits_MC",&Sil1H_MC->L0H);
  TBranch *br_Sil1H_L1H_MC =tree1->Branch("Si1H_L1NHits_MC",&Sil1H_MC->L1H);
  TBranch *br_Sil1H_AmpDir_MC =tree1->Branch("Si1H_AmpDir_MC",&Sil1H_MC->AmpDir);
  TBranch *br_Sil1H_AmpIncl_MC =tree1->Branch("Si1H_AmpIncl_MC",&Sil1H_MC->AmpIncl);
  TBranch *br_Si1H_nHits_MC=tree1->Branch("Si1H_nHits_MC",&Sil1H_MC->nHits);

  TBranch *br_Si2H_X=tree1->Branch("Si2H_X",&Sil2H->X);
  TBranch *br_Si2H_Y=tree1->Branch("Si2H_Y",&Sil2H->Y);
  TBranch *br_Si2H_St=tree1->Branch("Si2H_St",&Sil2H->St);
  TBranch *br_Si2H_Mod=tree1->Branch("Si2H_Mod",&Sil2H->Mod);
  TBranch *br_Sil2H_L0H =tree1->Branch("Si2H_L0NHits",&Sil2H->L0H);
  TBranch *br_Sil2H_L1H =tree1->Branch("Si2H_L1NHits",&Sil2H->L1H);
  TBranch *br_Sil2H_AmpDir =tree1->Branch("Si2H_AmpDir",&Sil2H->AmpDir);
  TBranch *br_Sil2H_AmpIncl =tree1->Branch("Si2H_AmpIncl",&Sil2H->AmpIncl);
  TBranch *br_Si2H_nHits=tree1->Branch("Si2H_nHits",&Sil2H->nHits);

  TBranch *br_Si2HF_X=tree1->Branch("Si2HF_X",&Sil2HF->X);
  TBranch *br_Si2HF_Y=tree1->Branch("Si2HF_Y",&Sil2HF->Y);
  TBranch *br_Si2HF_St=tree1->Branch("Si2HF_St",&Sil2HF->St);
  TBranch *br_Si2HF_Mod=tree1->Branch("Si2HF_Mod",&Sil2HF->Mod);
  TBranch *br_Sil2HF_L0H =tree1->Branch("Si2HF_L0NHits",&Sil2HF->L0H);
  TBranch *br_Sil2HF_L1H =tree1->Branch("Si2HF_L1NHits",&Sil2HF->L1H);
  TBranch *br_Sil2HF_AmpDir =tree1->Branch("Si2HF_AmpDir",&Sil2HF->AmpDir);
  TBranch *br_Sil2HF_AmpIncl =tree1->Branch("Si2HF_AmpIncl",&Sil2HF->AmpIncl);
  TBranch *br_Si2HF_nHits=tree1->Branch("Si2HF_nHits",&Sil2HF->nHits);

  TBranch *br_Si2HU_X=tree1->Branch("Si2HU_X",&Sil2HU->X);
  TBranch *br_Si2HU_Y=tree1->Branch("Si2HU_Y",&Sil2HU->Y);
  TBranch *br_Si2HU_St=tree1->Branch("Si2HU_St",&Sil2HU->St);
  TBranch *br_Si2HU_Mod=tree1->Branch("Si2HU_Mod",&Sil2HU->Mod);
  TBranch *br_Sil2HU_L0H =tree1->Branch("Si2HU_L0NHits",&Sil2HU->L0H);
  TBranch *br_Sil2HU_L1H =tree1->Branch("Si2HU_L1NHits",&Sil2HU->L1H);
  TBranch *br_Sil2HU_AmpDir =tree1->Branch("Si2HU_AmpDir",&Sil2HU->AmpDir);
  TBranch *br_Sil2HU_AmpIncl =tree1->Branch("Si2HU_AmpIncl",&Sil2HU->AmpIncl);
  TBranch *br_Si2HU_nHits=tree1->Branch("Si2HU_nHits",&Sil2HU->nHits);

  TBranch *br_Si2H_X_MC=tree1->Branch("Si2H_X_MC",&Sil2H_MC->X);
  TBranch *br_Si2H_Y_MC=tree1->Branch("Si2H_Y_MC",&Sil2H_MC->Y);
  TBranch *br_Si2H_Z_MC=tree1->Branch("Si2H_Z_MC",&Sil2H_MC->Z);
  TBranch *br_Si2H_Tx_MC=tree1->Branch("Si2H_Tx_MC",&Sil2H_MC->Tx);
  TBranch *br_Si2H_Ty_MC=tree1->Branch("Si2H_Ty_MC",&Sil2H_MC->Ty);
  TBranch *br_Si2H_Charge_MC =tree1->Branch("Si2H_Charge_MC",&Sil2H_MC->Charge);
  TBranch *br_Si2H_Prim_MC =tree1->Branch("Si2H_Prim_MC",&Sil2H_MC->Prim);
  TBranch *br_Si2H_St_MC=tree1->Branch("Si2H_St_MC",&Sil2H_MC->St);
  TBranch *br_Si2H_Mod_MC=tree1->Branch("Si2H_Mod_MC",&Sil2H_MC->Mod);
  TBranch *br_Sil2H_L0H_MC =tree1->Branch("Si2H_L0NHits_MC",&Sil2H_MC->L0H);
  TBranch *br_Sil2H_L1H_MC =tree1->Branch("Si2H_L1NHits_MC",&Sil2H_MC->L1H);
  TBranch *br_Sil2H_AmpDir_MC =tree1->Branch("Si2H_AmpDir_MC",&Sil2H_MC->AmpDir);
  TBranch *br_Sil2H_AmpIncl_MC =tree1->Branch("Si2H_AmpIncl_MC",&Sil2H_MC->AmpIncl);
  TBranch *br_Si2H_nHits_MC=tree1->Branch("Si2H_nHits_MC",&Sil2H_MC->nHits);

  TBranch *br_Si3H_X=tree1->Branch("Si3H_X",&Sil3H->X);
  TBranch *br_Si3H_Y=tree1->Branch("Si3H_Y",&Sil3H->Y);
  TBranch *br_Si3H_St=tree1->Branch("Si3H_St",&Sil3H->St);
  TBranch *br_Si3H_Mod=tree1->Branch("Si3H_Mod",&Sil3H->Mod);
  TBranch *br_Sil3H_L0H =tree1->Branch("Si3H_L0NHits",&Sil3H->L0H);
  TBranch *br_Sil3H_L1H =tree1->Branch("Si3H_L1NHits",&Sil3H->L1H);
  TBranch *br_Sil3H_AmpDir =tree1->Branch("Si3H_AmpDir",&Sil3H->AmpDir);
  TBranch *br_Sil3H_AmpIncl =tree1->Branch("Si3H_AmpIncl",&Sil3H->AmpIncl);
  TBranch *br_Si3H_nHits=tree1->Branch("Si3H_nHits",&Sil3H->nHits);

  TBranch *br_Si3HF_X=tree1->Branch("Si3HF_X",&Sil3HF->X);
  TBranch *br_Si3HF_Y=tree1->Branch("Si3HF_Y",&Sil3HF->Y);
  TBranch *br_Si3HF_St=tree1->Branch("Si3HF_St",&Sil3HF->St);
  TBranch *br_Si3HF_Mod=tree1->Branch("Si3HF_Mod",&Sil3HF->Mod);
  TBranch *br_Sil3HF_L0H =tree1->Branch("Si3HF_L0NHits",&Sil3HF->L0H);
  TBranch *br_Sil3HF_L1H =tree1->Branch("Si3HF_L1NHits",&Sil3HF->L1H);
  TBranch *br_Sil3HF_AmpDir =tree1->Branch("Si3HF_AmpDir",&Sil3HF->AmpDir);
  TBranch *br_Sil3HF_AmpIncl =tree1->Branch("Si3HF_AmpIncl",&Sil3HF->AmpIncl);
  TBranch *br_Si3HF_nHits=tree1->Branch("Si3HF_nHits",&Sil3HF->nHits);

  TBranch *br_Si3HU_X=tree1->Branch("Si3HU_X",&Sil3HU->X);
  TBranch *br_Si3HU_Y=tree1->Branch("Si3HU_Y",&Sil3HU->Y);
  TBranch *br_Si3HU_St=tree1->Branch("Si3HU_St",&Sil3HU->St);
  TBranch *br_Si3HU_Mod=tree1->Branch("Si3HU_Mod",&Sil3HU->Mod);
  TBranch *br_Sil3HU_L0H =tree1->Branch("Si3HU_L0NHits",&Sil3HU->L0H);
  TBranch *br_Sil3HU_L1H =tree1->Branch("Si3HU_L1NHits",&Sil3HU->L1H);
  TBranch *br_Sil3HU_AmpDir =tree1->Branch("Si3HU_AmpDir",&Sil3HU->AmpDir);
  TBranch *br_Sil3HU_AmpIncl =tree1->Branch("Si3HU_AmpIncl",&Sil3HU->AmpIncl);
  TBranch *br_Si3HU_nHits=tree1->Branch("Si3HU_nHits",&Sil3HU->nHits);

  TBranch *br_Si3H_X_MC=tree1->Branch("Si3H_X_MC",&Sil3H_MC->X);
  TBranch *br_Si3H_Y_MC=tree1->Branch("Si3H_Y_MC",&Sil3H_MC->Y);
  TBranch *br_Si3H_Z_MC=tree1->Branch("Si3H_Z_MC",&Sil3H_MC->Z);
  TBranch *br_Si3H_Tx_MC=tree1->Branch("Si1H_Tx_MC",&Sil3H_MC->Tx);
  TBranch *br_Si3H_Ty_MC=tree1->Branch("Si1H_Ty_MC",&Sil3H_MC->Ty);
  TBranch *br_Si3H_Charge_MC =tree1->Branch("Si3H_Charge_MC",&Sil3H_MC->Charge);
  TBranch *br_Si3H_Prim_MC =tree1->Branch("Si3H_Prim_MC",&Sil3H_MC->Prim);
  TBranch *br_Si3H_St_MC=tree1->Branch("Si3H_St_MC",&Sil3H_MC->St);
  TBranch *br_Si3H_Mod_MC=tree1->Branch("Si3H_Mod_MC",&Sil3H_MC->Mod);
  TBranch *br_Sil3H_L0H_MC =tree1->Branch("Si3H_L0NHits_MC",&Sil3H_MC->L0H);
  TBranch *br_Sil3H_L1H_MC =tree1->Branch("Si3H_L1NHits_MC",&Sil3H_MC->L1H);
  TBranch *br_Sil3H_AmpDir_MC =tree1->Branch("Si3H_AmpDir_MC",&Sil3H_MC->AmpDir);
  TBranch *br_Sil3H_AmpIncl_MC =tree1->Branch("Si3H_AmpIncl_MC",&Sil3H_MC->AmpIncl);
  TBranch *br_Si3H_nHits_MC=tree1->Branch("Si3H_nHits_MC",&Sil3H_MC->nHits);

  TBranch *br_Si4H_X=tree1->Branch("Si4H_X",&Sil4H->X);
  TBranch *br_Si4H_Y=tree1->Branch("Si4H_Y",&Sil4H->Y);
  TBranch *br_Si4H_St=tree1->Branch("Si4H_St",&Sil4H->St);
  TBranch *br_Si4H_Mod=tree1->Branch("Si4H_Mod",&Sil4H->Mod);
  TBranch *br_Sil4H_L0H =tree1->Branch("Si4H_L0NHits",&Sil4H->L0H);
  TBranch *br_Sil4H_L1H =tree1->Branch("Si4H_L1NHits",&Sil4H->L1H);
  TBranch *br_Sil4H_AmpDir =tree1->Branch("Si4H_AmpDir",&Sil4H->AmpDir);
  TBranch *br_Sil4H_AmpIncl =tree1->Branch("Si4H_AmpIncl",&Sil4H->AmpIncl);
  TBranch *br_Si4H_nHits=tree1->Branch("Si4H_nHits",&Sil4H->nHits);

  TBranch *br_Si4HF_X=tree1->Branch("Si4HF_X",&Sil4HF->X);
  TBranch *br_Si4HF_Y=tree1->Branch("Si4HF_Y",&Sil4HF->Y);
  TBranch *br_Si4HF_St=tree1->Branch("Si4HF_St",&Sil4HF->St);
  TBranch *br_Si4HF_Mod=tree1->Branch("Si4HF_Mod",&Sil4HF->Mod);
  TBranch *br_Sil4HF_L0H =tree1->Branch("Si4HF_L0NHits",&Sil4HF->L0H);
  TBranch *br_Sil4HF_L1H =tree1->Branch("Si4HF_L1NHits",&Sil4HF->L1H);
  TBranch *br_Sil4HF_AmpDir =tree1->Branch("Si4HF_AmpDir",&Sil4HF->AmpDir);
  TBranch *br_Sil4HF_AmpIncl =tree1->Branch("Si4HF_AmpIncl",&Sil4HF->AmpIncl);
  TBranch *br_Si4HF_nHits=tree1->Branch("Si4HF_nHits",&Sil4HF->nHits);

  TBranch *br_Si4HU_X=tree1->Branch("Si4HU_X",&Sil4HU->X);
  TBranch *br_Si4HU_Y=tree1->Branch("Si4HU_Y",&Sil4HU->Y);
  TBranch *br_Si4HU_St=tree1->Branch("Si4HU_St",&Sil4HU->St);
  TBranch *br_Si4HU_Mod=tree1->Branch("Si4HU_Mod",&Sil4HU->Mod);
  TBranch *br_Sil4HU_L0H =tree1->Branch("Si4HU_L0NHits",&Sil4HU->L0H);
  TBranch *br_Sil4HU_L1H =tree1->Branch("Si4HU_L1NHits",&Sil4HU->L1H);
  TBranch *br_Sil4HU_AmpDir =tree1->Branch("Si4HU_AmpDir",&Sil4HU->AmpDir);
  TBranch *br_Sil4HU_AmpIncl =tree1->Branch("Si4HU_AmpIncl",&Sil4HU->AmpIncl);
  TBranch *br_Si4HU_nHits=tree1->Branch("Si4HU_nHits",&Sil4HU->nHits);

  TBranch *br_Si4H_X_MC=tree1->Branch("Si4H_X_MC",&Sil4H_MC->X);
  TBranch *br_Si4H_Y_MC=tree1->Branch("Si4H_Y_MC",&Sil4H_MC->Y);
  TBranch *br_Si4H_Z_MC=tree1->Branch("Si4H_Z_MC",&Sil4H_MC->Z);
  TBranch *br_Si4H_Tx_MC=tree1->Branch("Si4H_Tx_MC",&Sil4H_MC->Tx);
  TBranch *br_Si4H_Ty_MC=tree1->Branch("Si4H_Ty_MC",&Sil4H_MC->Ty);
  TBranch *br_Si4H_Charge_MC =tree1->Branch("Si4H_Charge_MC",&Sil4H_MC->Charge);
  TBranch *br_Si4H_Prim_MC =tree1->Branch("Si4H_Prim_MC",&Sil4H_MC->Prim);
  TBranch *br_Si4H_St_MC=tree1->Branch("Si4H_St_MC",&Sil4H_MC->St);
  TBranch *br_Si4H_Mod_MC=tree1->Branch("Si4H_Mod_MC",&Sil4H_MC->Mod);
  TBranch *br_Sil4H_L0H_MC =tree1->Branch("Si4H_L0NHits_MC",&Sil4H_MC->L0H);
  TBranch *br_Sil4H_L1H_MC =tree1->Branch("Si4H_L1NHits_MC",&Sil4H_MC->L1H);
  TBranch *br_Sil4H_AmpDir_MC =tree1->Branch("Si4H_AmpDir_MC",&Sil4H_MC->AmpDir);
  TBranch *br_Sil4H_AmpIncl_MC =tree1->Branch("Si4H_AmpIncl_MC",&Sil4H_MC->AmpIncl);
  TBranch *br_Si4H_nHits_MC=tree1->Branch("Si4H_nHits_MC",&Sil4H_MC->nHits);

  

  

  // Track *MWPC1 = new Track;
  // Track *MWPC2 = new Track;
  // Track *MWPC3 = new Track;
  // Track *MWPC4 = new Track;


  filename = "/home/srcuser3/bmnroot/macro/run/Sim_Si.root";
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiMwpcAligDchCoord4780_4799_BMNS_wo_gap_wo_rot_WithDCH_MWPC_Zinv.root";
  //filename = "/eos/nica/src/users/mpatsyuk/data/2022/Timur_Test/Si_MWPC_DCH_combined_data_" + to_string(filenum) + ".root";
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiMwpcAligDchCoord4780_4799_BMNS_wo_gap_wo_rot_WithDCH_MWPC_Zinv_ampcut_1700.root";
  
      
  TFile *hfile = new TFile(filename);
  //		if (!hfile->IsOpen())  continue;
  //		if (!hfile->Get("bmndata")) 		continue;
        
  TTree *tree = (TTree*)hfile->Get("bmndata");

  TClonesArray *mcTracks = nullptr;
  //TClonesArray *silTrack = new TClonesArray("BmnSiliconTrack");
  TClonesArray *SiHits = new TClonesArray("BmnBCPoint");
  // TClonesArray *mwpcsegment = new TClonesArray("BmnMwpcSegment");
  //TClonesArray *silHit1 = new TClonesArray("BmnSiliconHit");
  //TClonesArray *silHit2 = new TClonesArray("BmnSiliconHit");
  FairMCEventHeader *evHeader = nullptr;

  tree->SetBranchAddress("MCEventHeader.", &evHeader);
  tree->SetBranchAddress("BCPoint_2", &SiHits);
  tree->SetBranchAddress("MCTrack", &mcTracks);
  //tree->SetBranchAddress("BmnMwpcSegment", &mwpcsegment);
  //tree->SetBranchAddress("BmnSiliconTrack", &silTrack);
  //tree->SetBranchAddress("BmnSiliconHitsX", &silHit1);
  //tree->SetBranchAddress("BmnSiliconHitsXY", &silHit2);

  

  int NEv = tree->GetEntries();
  int EvWithtrack=0;
  int EvWithtrackX=0;
  int EvWithtrackY=0;
  cout<<NEv<<endl; 
  int counter=0;
  for (int ev=0; ev<NEv; ev++) {
    tree->GetEntry(ev);
    
    cout<<SiHits->GetEntries()<<" "<<mcTracks->GetEntries()<<endl;

      
    double x[4]={0};
    double y[4]={0};
    double z[4]={0};

    vector <double> Si1X;
    vector <double> Si1Y;
    vector <double> Si1Z;

    vector <double> Si2X;
    vector <double> Si2Y;
    vector <double> Si2Z;

    vector <double> Si3X;
    vector <double> Si3Y;
    vector <double> Si3Z;

    vector <double> Si4X;
    vector <double> Si4Y;
    vector <double> Si4Z;

    Si1X.clear();
    Si1Y.clear();
    Si1Z.clear();

    Si2X.clear();
    Si2Y.clear();
    Si2Z.clear();

    Si3X.clear();
    Si3Y.clear();
    Si3Z.clear();

    Si4X.clear();
    Si4Y.clear();
    Si4Z.clear();


    Sil1H_MC->nHits=0;
    Sil2H_MC->nHits=0;
    Sil3H_MC->nHits=0;
    Sil4H_MC->nHits=0;

    Sil1H->nHits=0;
    Sil2H->nHits=0;
    Sil3H->nHits=0;
    Sil4H->nHits=0;

    MWPC1_MC->nTracks=0;
    MWPC2_MC->nTracks=0;
    MWPC3_MC->nTracks=0;
    MWPC4_MC->nTracks=0;

    MWPC1->nTracks=0;
    MWPC2->nTracks=0;
    MWPC3->nTracks=0;
    MWPC4->nTracks=0;

    MC->nTracks=mcTracks->GetEntries();
    for (int i=0; i<mcTracks->GetEntries(); i++) {
      CbmMCTrack* mcTrack = (CbmMCTrack*)mcTracks->At(i);
      MC->X->push_back(mcTrack->GetStartX());
      MC->Y->push_back(mcTrack->GetStartY());
      MC->Z->push_back(mcTrack->GetStartZ());
      MC->Tx->push_back(mcTrack->GetPx()/mcTrack->GetPz());
      MC->Ty->push_back(mcTrack->GetPy()/mcTrack->GetPz());
      MC->MId->push_back(mcTrack->GetMotherId());
      
    }
        

    for (int i=0; i<SiHits->GetEntries(); i++) {
      BmnBCPoint* SiHit = (BmnBCPoint*)SiHits->At(i);
      
      if (SiHit->GetCharge()<2) continue;
      if (SiHit->GetDetectorID()==20) {
        Sil1H_MC->X->push_back(SiHit->GetX()-0.5);
        Sil1H_MC->Y->push_back(SiHit->GetY()+4.6);
        Sil1H_MC->Z->push_back(SiHit->GetZ());
        Sil1H_MC->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        Sil1H_MC->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        Sil1H_MC->Prim->push_back(SiHit->GetIsPrimary());
        Sil1H_MC->Charge->push_back(SiHit->GetCharge());
        Sil1H->X->push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0500)-0.5);
        Sil1H->Y->push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0106)+4.6);
        //Sil1H->Z->push_back(SiHit->GetZ());
        Sil1H->St->push_back(1);
        Sil1H->Mod->push_back(-1);
        Sil1H->L0H->push_back(4);
        Sil1H->L1H->push_back(4);
        Sil1H->AmpDir->push_back(-1);
        Sil1H->AmpIncl->push_back(-1);  
        Si1X.push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0500)-0.5);
        Si1Y.push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0106)+4.6);
        Si1Z.push_back(SiHit->GetZ());
        Sil1H_MC->nHits++;
        Sil1H->nHits++;
      }
      if (SiHit->GetDetectorID()==21) {
        Sil2H_MC->X->push_back(SiHit->GetX()-0.5);
        Sil2H_MC->Y->push_back(SiHit->GetY()+4.6);
        Sil2H_MC->Z->push_back(SiHit->GetZ());
        Sil2H_MC->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        Sil2H_MC->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        Sil2H_MC->Prim->push_back(SiHit->GetIsPrimary());
        Sil2H_MC->Charge->push_back(SiHit->GetCharge());
        Sil2H->X->push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0106)-0.5);
        Sil2H->Y->push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0500)+4.6);
        //Sil2H->Z->push_back(SiHit->GetZ());
        Sil2H->St->push_back(2);
        Sil2H->Mod->push_back(-1);
        Sil2H->L0H->push_back(4);
        Sil2H->L1H->push_back(4);
        Sil2H->AmpDir->push_back(-1);
        Sil2H->AmpIncl->push_back(-1);
        Si2X.push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0106)-0.5);
        Si2Y.push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0500)+4.6);
        Si2Z.push_back(SiHit->GetZ());
        Sil2H_MC->nHits++;
        Sil2H->nHits++;
      }
      if (SiHit->GetDetectorID()==22) {
        Sil3H_MC->X->push_back(SiHit->GetX()-0.5);
        Sil3H_MC->Y->push_back(SiHit->GetY()+4.6);
        Sil3H_MC->Z->push_back(SiHit->GetZ());
        Sil3H_MC->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        Sil3H_MC->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        Sil3H_MC->Prim->push_back(SiHit->GetIsPrimary());
        Sil3H_MC->Charge->push_back(SiHit->GetCharge());
        Sil3H->X->push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0500)-0.5);
        Sil3H->Y->push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0106)+4.6);
        //Sil3H->Z->push_back(SiHit->GetZ());
        Sil3H->St->push_back(3);
        Sil3H->Mod->push_back(-1);
        Sil3H->L0H->push_back(4);
        Sil3H->L1H->push_back(4);
        Sil3H->AmpDir->push_back(-1);
        Sil3H->AmpIncl->push_back(-1);
        Si3X.push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0500)-0.5);
        Si3Y.push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0106)+4.6);
        Si3Z.push_back(SiHit->GetZ());
        Sil3H_MC->nHits++;
        Sil3H->nHits++;
      }
      if (SiHit->GetDetectorID()==23) {
        Sil4H_MC->X->push_back(SiHit->GetX()-0.5);
        Sil4H_MC->Y->push_back(SiHit->GetY()+4.6);
        Sil4H_MC->Z->push_back(SiHit->GetZ());
        Sil4H_MC->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        Sil4H_MC->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        Sil4H_MC->Prim->push_back(SiHit->GetIsPrimary());
        Sil4H_MC->Charge->push_back(SiHit->GetCharge());
        Sil4H->X->push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0106)-0.5);
        Sil4H->Y->push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0500)+4.6);
        //Sil4H->Z->push_back(SiHit->GetZ());
        Sil4H->St->push_back(4);
        Sil4H->Mod->push_back(-1);
        Sil4H->L0H->push_back(4);
        Sil4H->L1H->push_back(4);
        Sil4H->AmpDir->push_back(-1);
        Sil4H->AmpIncl->push_back(-1);
        Si4X.push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0106)-0.5);
        Si4Y.push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0500)+4.6);
        Si4Z.push_back(SiHit->GetZ());
        Sil4H_MC->nHits++;
        Sil4H->nHits++;
      }
      if (SiHit->GetDetectorID()==24) {
        MWPC1_MC->X->push_back(SiHit->GetX()-0.5);
        MWPC1_MC->Y->push_back(SiHit->GetY()+4.6);
        MWPC1_MC->Z->push_back(SiHit->GetZ());
        MWPC1_MC->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        MWPC1_MC->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        MWPC1_MC->Prim->push_back(SiHit->GetIsPrimary());
        MWPC1_MC->Charge->push_back(SiHit->GetCharge());
        MWPC1->X->push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0510)-0.5);
        MWPC1->Y->push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0510)+4.6);
        MWPC1->Z->push_back(SiHit->GetZ());
        MWPC1->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        MWPC1->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        MWPC1->Chi2X->push_back(-1);
        MWPC1_MC->nTracks++;
        MWPC1->nTracks++;
      }
      if (SiHit->GetDetectorID()==25) {
        MWPC2_MC->X->push_back(SiHit->GetX()-0.5);
        MWPC2_MC->Y->push_back(SiHit->GetY()+4.6);
        MWPC2_MC->Z->push_back(SiHit->GetZ());
        MWPC2_MC->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        MWPC2_MC->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        MWPC2_MC->Prim->push_back(SiHit->GetIsPrimary());
        MWPC2_MC->Charge->push_back(SiHit->GetCharge());
        MWPC2->X->push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0510)-0.5);
        MWPC2->Y->push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0510)+4.6);
        MWPC2->Z->push_back(SiHit->GetZ());
        MWPC2->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        MWPC2->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        MWPC2->Chi2X->push_back(-1);
        MWPC2_MC->nTracks++;
        MWPC2->nTracks++;
      }
      if (SiHit->GetDetectorID()==26) {
        MWPC3_MC->X->push_back(SiHit->GetX()-0.5);
        MWPC3_MC->Y->push_back(SiHit->GetY()+4.6);
        MWPC3_MC->Z->push_back(SiHit->GetZ());
        MWPC3_MC->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        MWPC3_MC->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        MWPC3_MC->Prim->push_back(SiHit->GetIsPrimary());
        MWPC3_MC->Charge->push_back(SiHit->GetCharge());
        MWPC3->X->push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0510)-0.5);
        MWPC3->Y->push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0510)+4.6);
        MWPC3->Z->push_back(SiHit->GetZ());
        MWPC3->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        MWPC3->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        MWPC3->Chi2X->push_back(-1);
        MWPC3_MC->nTracks++;
        MWPC3->nTracks++;
      }
      if (SiHit->GetDetectorID()==27) {
        MWPC4_MC->X->push_back(SiHit->GetX()-0.5);
        MWPC4_MC->Y->push_back(SiHit->GetY()+4.6);
        MWPC4_MC->Z->push_back(SiHit->GetZ());
        MWPC4_MC->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        MWPC4_MC->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        MWPC4_MC->Prim->push_back(SiHit->GetIsPrimary());
        MWPC4_MC->Charge->push_back(SiHit->GetCharge());
        MWPC4->X->push_back(SiHit->GetX()+gRandom->Gaus(0, 0.0510)-0.5);
        MWPC4->Y->push_back(SiHit->GetY()+gRandom->Gaus(0, 0.0510)+4.6);
        MWPC4->Z->push_back(SiHit->GetZ());
        MWPC4->Tx->push_back(SiHit->GetPx()/SiHit->GetPz());
        MWPC4->Ty->push_back(SiHit->GetPy()/SiHit->GetPz());
        MWPC4->Chi2X->push_back(-1);
        MWPC4_MC->nTracks++;
        MWPC4->nTracks++;
      }

      
    }

    
       
    Sil1HU->nHits=0;
    Sil2HU->nHits=0;
    Sil3HU->nHits=0;
    Sil4HU->nHits=0;
    MWPC3U->nTracks=0;
    MWPC4U->nTracks=0;

    Sil1234->X->clear();
    Sil1234->Y->clear();
    Sil1234->Z->clear();
    Sil1234->Tx->clear();
    Sil1234->Ty->clear();
    Sil1234->Chi2X->clear();
    Sil1234->Chi2Y->clear();
    Sil1234->IDX->clear();
    Sil1234->IDY->clear();
    Sil1234->DX12->clear();
    Sil1234->DY12->clear();
    Sil1234->DX34->clear();
    Sil1234->DY34->clear();
    // Sil1234->Flag->clear();

    
    Sil1234MWPC34->X->clear();
    Sil1234MWPC34->Y->clear();
    Sil1234MWPC34->Z->clear();
    Sil1234MWPC34->Tx->clear();
    Sil1234MWPC34->Ty->clear();
    Sil1234MWPC34->Chi2X->clear();
    Sil1234MWPC34->Chi2Y->clear();
    Sil1234MWPC34->IDX->clear();
    Sil1234MWPC34->IDY->clear();
    Sil1234MWPC34->caseX->clear();
    Sil1234MWPC34->caseY->clear();

    counter++;


  


    int nH_Si1=Sil1H->X->size();
    int nH_Si2=Sil2H->X->size();
    int nH_Si3=Sil3H->X->size();
    int nH_Si4=Sil4H->X->size();
    int nH_MWPC1=MWPC1->X->size();
    int nH_MWPC2=MWPC2->X->size();
    int nH_MWPC3=MWPC3->X->size();
    int nH_MWPC4=MWPC4->X->size();

    

   

   


    
    
    //cout<<ev<<" 8 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
    int mult[nH_Si2][nH_Si2];
    int isdouble=0;
    for (int i=0; i<nH_Si2; i++) {
      for (int j=0; j<nH_Si2; j++) {
        mult[i][j]=0;
      }
    }

  
  
    MWPC3F->nTracks=MWPC3->nTracks;
    for (int i=0; i<MWPC3->X->size(); i++) {
        MWPC3F->X->push_back(MWPC3->X->at(i));
        MWPC3F->Y->push_back(MWPC3->Y->at(i));
        MWPC3F->Z->push_back(MWPC3->Z->at(i));
        MWPC3F->Tx->push_back(MWPC3->Tx->at(i));
        MWPC3F->Ty->push_back(MWPC3->Ty->at(i));
        MWPC3F->Chi2X->push_back(MWPC3->Chi2X->at(i));
           
    }
    
    MWPC4F->nTracks=MWPC4->nTracks;
    for (int i=0; i<MWPC4->X->size(); i++) {
        MWPC4F->X->push_back(MWPC4->X->at(i));
        MWPC4F->Y->push_back(MWPC4->Y->at(i));
        MWPC4F->Z->push_back(MWPC4->Z->at(i));
        MWPC4F->Tx->push_back(MWPC4->Tx->at(i));
        MWPC4F->Ty->push_back(MWPC4->Ty->at(i));
        MWPC4F->Chi2X->push_back(MWPC4->Chi2X->at(i));
           
    }

    Sil2HF->nHits=Sil2H->nHits;
    for (int i=0; i<Sil2H->X->size(); i++) {
        Sil2HF->X->push_back(Sil2H->X->at(i));
        Sil2HF->Y->push_back(Sil2H->Y->at(i));
        Sil2HF->St->push_back(Sil2H->St->at(i));
        Sil2HF->Mod->push_back(Sil2H->Mod->at(i));
        Sil2HF->L0H->push_back(Sil2H->L0H->at(i));
        Sil2HF->L1H->push_back(Sil2H->L1H->at(i));
        Sil2HF->AmpDir->push_back(Sil2H->AmpDir->at(i));
        Sil2HF->AmpIncl->push_back(Sil2H->AmpIncl->at(i));      
    }


    Sil1HF->nHits=Sil1H->nHits;
    for (int i=0; i<Sil1H->X->size(); i++) {
        Sil1HF->X->push_back(Sil1H->X->at(i));
        Sil1HF->Y->push_back(Sil1H->Y->at(i));
        Sil1HF->St->push_back(Sil1H->St->at(i));
        Sil1HF->Mod->push_back(Sil1H->Mod->at(i));
        Sil1HF->L0H->push_back(Sil1H->L0H->at(i));
        Sil1HF->L1H->push_back(Sil1H->L1H->at(i));
        Sil1HF->AmpDir->push_back(Sil1H->AmpDir->at(i));
        Sil1HF->AmpIncl->push_back(Sil1H->AmpIncl->at(i));      
    }

    Sil2HF->nHits=Sil2H->nHits;
    for (int i=0; i<Sil2H->X->size(); i++) {
        Sil2HF->X->push_back(Sil2H->X->at(i));
        Sil2HF->Y->push_back(Sil2H->Y->at(i));
        Sil2HF->St->push_back(Sil2H->St->at(i));
        Sil2HF->Mod->push_back(Sil2H->Mod->at(i));
        Sil2HF->L0H->push_back(Sil2H->L0H->at(i));
        Sil2HF->L1H->push_back(Sil2H->L1H->at(i));
        Sil2HF->AmpDir->push_back(Sil2H->AmpDir->at(i));
        Sil2HF->AmpIncl->push_back(Sil2H->AmpIncl->at(i));      
    }

    Sil3HF->nHits=Sil3H->nHits;
    for (int i=0; i<Sil3H->X->size(); i++) {
        Sil3HF->X->push_back(Sil3H->X->at(i));
        Sil3HF->Y->push_back(Sil3H->Y->at(i));
        Sil3HF->St->push_back(Sil3H->St->at(i));
        Sil3HF->Mod->push_back(Sil3H->Mod->at(i));
        Sil3HF->L0H->push_back(Sil3H->L0H->at(i));
        Sil3HF->L1H->push_back(Sil3H->L1H->at(i));
        Sil3HF->AmpDir->push_back(Sil3H->AmpDir->at(i));
        Sil3HF->AmpIncl->push_back(Sil3H->AmpIncl->at(i));      
    }

    Sil4HF->nHits=Sil4H->nHits;
    for (int i=0; i<Sil4H->X->size(); i++) {
        Sil4HF->X->push_back(Sil4H->X->at(i));
        Sil4HF->Y->push_back(Sil4H->Y->at(i));
        Sil4HF->St->push_back(Sil4H->St->at(i));
        Sil4HF->Mod->push_back(Sil4H->Mod->at(i));
        Sil4HF->L0H->push_back(Sil4H->L0H->at(i));
        Sil4HF->L1H->push_back(Sil4H->L1H->at(i));
        Sil4HF->AmpDir->push_back(Sil4H->AmpDir->at(i));
        Sil4HF->AmpIncl->push_back(Sil4H->AmpIncl->at(i));      
    }
    

    if(nH_Si1>5) nH_Si1=0;
    if(nH_Si2>5) nH_Si2=0;
    if(nH_Si3>4) nH_Si3=0;
    if(nH_Si4>4) nH_Si4=0;
    if(nH_MWPC1>9) nH_MWPC1=0;
    if(nH_MWPC2>9) nH_MWPC2=0;
    if(nH_MWPC3>9) nH_MWPC3=0;
    if(nH_MWPC4>9) nH_MWPC4=0;

    long double ax, bx, ay, by, chi2x, chi2y;
    Sil1234->nTracks=0;
    Sil1234MWPC34->nTracks=0;
    Sil1234MWPC34->nTracksX=0;
    Sil1234MWPC34->nTracksY=0;
    MWPC_p0->nTracks=0;
    MWPC_p1->nTracks=0;
    
    Sil1234MWPC34->Flag=9000000+100000*nH_Si1+10000*nH_Si2+1000*nH_Si3+100*nH_Si4+10*nH_MWPC3+nH_MWPC4;

    int numX=0;
    int numY=0;
    int numX1=0;
    int numY1=0;
    
    if (nH_MWPC1>0 && nH_MWPC2>0) {
      for (int k=0; k<nH_MWPC1; k++){
        for (int l=0; l<nH_MWPC2; l++){

              if (MWPC1->Tx->at(k)-MWPC2->Tx->at(l)>0.0001) continue;
              if (MWPC1->Ty->at(k)-MWPC2->Ty->at(l)>0.0001) continue;
              vector <long double> x1234M34={MWPC1->X->at(k), MWPC2->X->at(l)};
              vector <long double> y1234M34={MWPC1->Y->at(k), MWPC2->Y->at(l)};
              vector <long double> z1234M34={MWPC1->Z->at(k), MWPC2->Z->at(l)};
              vector <long double> sigma={sigma_M3, sigma_M4};
              //vector <int> clsize={0};
              ax, bx, ay, by, chi2x, chi2y=0;
              GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
              GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
              if (abs(ax*ZtargPos+bx)>CutVX) continue;
              if (abs(ay*ZtargPos+by)>CutVY) continue;
              MWPC_p0->Tx->push_back(ax);
              MWPC_p0->X->push_back(ax*(-782.6)+bx);
              MWPC_p0->Ty->push_back(ay);
              MWPC_p0->Y->push_back(ay*(-782.6)+by);
              MWPC_p0->Chi2X->push_back(chi2x+chi2y);
              MWPC_p0->nTracks++;

                
        }        
      }
    }

    if (nH_MWPC3>0 && nH_MWPC4>0) {
      for (int k=0; k<nH_MWPC3; k++){
        for (int l=0; l<nH_MWPC4; l++){

              if (MWPC3->Tx->at(k)-MWPC4->Tx->at(l)>0.0001) continue;
              if (MWPC3->Ty->at(k)-MWPC4->Ty->at(l)>0.0001) continue;
              vector <long double> x1234M34={MWPC3->X->at(k), MWPC4->X->at(l)};
              vector <long double> y1234M34={MWPC3->Y->at(k), MWPC4->Y->at(l)};
              vector <long double> z1234M34={MWPC3->Z->at(k), MWPC4->Z->at(l)};
              vector <long double> sigma={sigma_M3, sigma_M4};
              //vector <int> clsize={0};
              ax, bx, ay, by, chi2x, chi2y=0;
              GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
              GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
              if (abs(ax*ZtargPos+bx)>CutVX) continue;
              if (abs(ay*ZtargPos+by)>CutVY) continue;
              MWPC_p1->Tx->push_back(ax);
              MWPC_p1->X->push_back(ax*(-296.1)+bx);
              MWPC_p1->Ty->push_back(ay);
              MWPC_p1->Y->push_back(ay*(-296.1)+by);
              MWPC_p1->Chi2X->push_back(chi2x+chi2y);
              MWPC_p1->nTracks++;

                
        }        
      }
    }

 
    for (int maxTracks=0; maxTracks<6; maxTracks++) {

      //cout<<ev<<" 1 "<<maxTracks<<" "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<" "<<nH_MWPC3<<" "<<nH_MWPC4<<endl;
      // int caseX=0;
      // int caseY=0;

      Sil1234MWPC34->X->clear();
      Sil1234MWPC34->Y->clear();
      Sil1234MWPC34->Z->clear();
      Sil1234MWPC34->Tx->clear();
      Sil1234MWPC34->Ty->clear();
      Sil1234MWPC34->Chi2X->clear();
      Sil1234MWPC34->Chi2Y->clear();
      Sil1234MWPC34->IDX->clear();
      Sil1234MWPC34->IDY->clear();
      Sil1234MWPC34->caseX->clear();
      Sil1234MWPC34->caseY->clear();

      if (nH_Si2>0 && nH_Si4>0 && nH_MWPC3>0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si2; i++){
          for (int j=0; j<nH_Si4; j++){
            for (int k=0; k<nH_MWPC3; k++){
              for (int l=0; l<nH_MWPC4; l++){


                vector <long double> x1234M34={Si2X.at(i), Si4X.at(j), MWPC3->X->at(k), MWPC4->X->at(l)};
                vector <long double> z1234M34={Si2Z.at(i), Si4Z.at(j), MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_S2*Sil2H->L0H->at(i), sigma_S4*Sil4H->L0H->at(j), sigma_M3, sigma_M4};
                // vector <int> clsize={Sil2H->L0H->at(i), Sil4H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                //cout<<"Chi2 "<<chi2x<<endl;
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                //cout<<"Chi2 "<<chi2x<<endl;
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*(i+1)+1000*nH_Si3+100*(j+1)+10*(k+1)+(l+1));
                Sil1234MWPC34->caseX->push_back(1);
                

          
              }      
            }        
          }
        }
      }

      if (nH_Si2>0 && nH_Si4>0 && nH_MWPC3>0 && nH_MWPC4==0) {
        for (int i=0; i<nH_Si2; i++){
          for (int j=0; j<nH_Si4; j++){
            for (int k=0; k<nH_MWPC3; k++){
              


                vector <long double> x1234M34={Si2X.at(i), Si4X.at(j), MWPC3->X->at(k)};
                vector <long double> z1234M34={Si2Z.at(i), Si4Z.at(j), MWPC3->Z->at(k)};
                vector <long double> sigma={sigma_S2*Sil2H->L0H->at(i), sigma_S4*Sil4H->L0H->at(j), sigma_M3};
                //vector <int> clsize={Sil2H->L0H->at(i), Sil4H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X*0.6) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*(i+1)+1000*nH_Si3+100*(j+1)+10*(k+1));
                Sil1234MWPC34->caseX->push_back(2);

          
                
            }        
          }
        }
      }

      if (nH_Si2>0 && nH_Si4>0 && nH_MWPC3==0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si2; i++){
          for (int j=0; j<nH_Si4; j++){
            for (int k=0; k<nH_MWPC4; k++){
              


                vector <long double> x1234M34={Si2X.at(i), Si4X.at(j), MWPC4->X->at(k)};
                vector <long double> z1234M34={Si2Z.at(i), Si4Z.at(j), MWPC4->Z->at(k)};
                vector <long double> sigma={sigma_S2*Sil2H->L0H->at(i), sigma_S4*Sil4H->L0H->at(j), sigma_M4};
                //vector <int> clsize={Sil2H->L0H->at(i), Sil4H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X*0.6) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*(i+1)+1000*nH_Si3+100*(j+1)+(k+1));
                Sil1234MWPC34->caseX->push_back(3);

          
                
            }        
          }
        }
      }

      if (nH_Si2>0 && nH_Si4==0 && nH_MWPC3>0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si2; i++){
          for (int k=0; k<nH_MWPC3; k++){
            for (int l=0; l<nH_MWPC4; l++){


                vector <long double> x1234M34={Si2X.at(i), MWPC3->X->at(k), MWPC4->X->at(l)};
                vector <long double> z1234M34={Si2Z.at(i), MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_S2*Sil2H->L0H->at(i), sigma_M3, sigma_M4};
                //vector <int> clsize={Sil2H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X*0.6) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*(i+1)+1000*nH_Si3+100*0+10*(k+1)+(l+1));
                Sil1234MWPC34->caseX->push_back(4);

                //cout<<Sil1234MWPC34->IDX->at(i)<<"  "<<i+1<<"j  "<<nH_Si3<<"  "<<l+1<<"  "<<endl;

          
            }      
          }        
        }
      }

      if (nH_Si2==0 && nH_Si4>0 && nH_MWPC3>0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si4; i++){
          for (int k=0; k<nH_MWPC3; k++){
            for (int l=0; l<nH_MWPC4; l++){


                vector <long double> x1234M34={Si4X.at(i), MWPC3->X->at(k), MWPC4->X->at(l)};
                vector <long double> z1234M34={Si4Z.at(i), MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_S4*Sil4H->L0H->at(i), sigma_M3, sigma_M4};
                //vector <int> clsize={Sil4H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X*0.6) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*0+1000*nH_Si3+100*(i+1)+10*(k+1)+(l+1));
                Sil1234MWPC34->caseX->push_back(5);

          
            }      
          }        
        }
      }

      if (nH_Si2>0 && nH_Si4>0 && nH_MWPC3==0 && nH_MWPC4==0) {
        for (int i=0; i<nH_Si2; i++){
          for (int j=0; j<nH_Si4; j++){
              


                vector <long double> x1234M34={Si2X.at(i), Si4X.at(j)};
                vector <long double> z1234M34={Si2Z.at(i), Si4Z.at(j)};
                vector <long double> sigma={sigma_S2*Sil2H->L0H->at(i), sigma_S4*Sil4H->L0H->at(j)};
                //vector <int> clsize={Sil2H->L0H->at(i), Sil4H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*(i+1)+1000*nH_Si3+100*(j+1));
                Sil1234MWPC34->caseX->push_back(6);

                
          }
        }
      }

      if (nH_Si2>0 && nH_Si4==0 && nH_MWPC3>0 && nH_MWPC4==0) {
        for (int i=0; i<nH_Si2; i++){
          for (int k=0; k<nH_MWPC3; k++){

                vector <long double> x1234M34={Si2X.at(i), MWPC3->X->at(k)};
                vector <long double> z1234M34={Si2Z.at(i), MWPC3->Z->at(k)};
                vector <long double> sigma={sigma_S2*Sil2H->L0H->at(i), sigma_M3};
                //vector <int> clsize={Sil2H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*(i+1)+1000*nH_Si3+100*0+10*(k+1)+0);
                Sil1234MWPC34->caseX->push_back(7);

              
          }        
        }
      }

      if (nH_Si2>0 && nH_Si4==0 && nH_MWPC3==0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si2; i++){
          for (int k=0; k<nH_MWPC4; k++){

                vector <long double> x1234M34={Si2X.at(i), MWPC4->X->at(k)};
                vector <long double> z1234M34={Si2Z.at(i), MWPC4->Z->at(k)};
                vector <long double> sigma={sigma_S2*Sil2H->L0H->at(i), sigma_M4};
                //vector <int> clsize={Sil2H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*(i+1)+1000*nH_Si3+100*0+(k+1));
                Sil1234MWPC34->caseX->push_back(8);

              
          }        
        }
      }

      if (nH_Si2==0 && nH_Si4>0 && nH_MWPC3>0 && nH_MWPC4==0) {
        for (int i=0; i<nH_Si4; i++){
          for (int k=0; k<nH_MWPC3; k++){

                vector <long double> x1234M34={Si4X.at(i), MWPC3->X->at(k)};
                vector <long double> z1234M34={Si4Z.at(i), MWPC3->Z->at(k)};
                vector <long double> sigma={sigma_S4*Sil4H->L0H->at(i), sigma_M3};
                //vector <int> clsize={Sil4H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*0+1000*nH_Si3+100*(i+1)+10*(k+1)+0);
                Sil1234MWPC34->caseX->push_back(9);

              
          }        
        }
      }

      if (nH_Si2==0 && nH_Si4>0 && nH_MWPC3==0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si4; i++){
          for (int k=0; k<nH_MWPC4; k++){

                vector <long double> x1234M34={Si4X.at(i), MWPC4->X->at(k)};
                vector <long double> z1234M34={Si4Z.at(i), MWPC4->Z->at(k)};
                vector <long double> sigma={sigma_S4*Sil4H->L0H->at(i), sigma_M4};
                //vector <int> clsize={Sil4H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*0+1000*nH_Si3+100*(i+1)+(k+1));
                Sil1234MWPC34->caseX->push_back(10);

              
          }        
        }
      }

      if (nH_Si2==0 && nH_Si4==0 && nH_MWPC3>0 && nH_MWPC4>0) {
        for (int k=0; k<nH_MWPC3; k++){
          for (int l=0; l<nH_MWPC4; l++){


                vector <long double> x1234M34={MWPC3->X->at(k), MWPC4->X->at(l)};
                vector <long double> z1234M34={MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_M3, sigma_M4};
                //vector <int> clsize={0};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*0+1000*nH_Si3+100*0+10*(k+1)+(l+1));
                Sil1234MWPC34->caseX->push_back(11);

                
          }        
        }
      }


      
      
      if (nH_Si1>0 && nH_Si3>0 && nH_MWPC3>0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si1; i++){
          for (int j=0; j<nH_Si3; j++){
            for (int k=0; k<nH_MWPC3; k++){
              for (int l=0; l<nH_MWPC4; l++){


                vector <long double> y1234M34={Si1Y.at(i), Si3Y.at(j), MWPC3->Y->at(k), MWPC4->Y->at(l)};
                vector <long double> z1234M34={Si1Z.at(i), Si3Z.at(j), MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_S1*Sil1H->L0H->at(i), sigma_S3*Sil3H->L0H->at(j), sigma_M3, sigma_M4};
                //vector <int> clsize={Sil1H->L0H->at(i), Sil3H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*(i+1)+10000*nH_Si2+1000*(j+1)+100*nH_Si4+10*(k+1)+(l+1));
                Sil1234MWPC34->caseY->push_back(1);

          
              }      
            }        
          }
        }
      }

      if (nH_Si1>0 && nH_Si3>0 && nH_MWPC3>0 && nH_MWPC4==0) {
        for (int i=0; i<nH_Si1; i++){
          for (int j=0; j<nH_Si3; j++){
            for (int k=0; k<nH_MWPC3; k++){
              


                vector <long double> y1234M34={Si1Y.at(i), Si3Y.at(j), MWPC3->Y->at(k)};
                vector <long double> z1234M34={Si1Z.at(i), Si3Z.at(j), MWPC3->Z->at(k)};
                vector <long double> sigma={sigma_S1*Sil1H->L0H->at(i), sigma_S3*Sil3H->L0H->at(j), sigma_M3};
                //vector <int> clsize={Sil1H->L0H->at(i), Sil3H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y*0.6) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*(i+1)+10000*nH_Si2+1000*(j+1)+100*nH_Si4+10*(k+1)+0);
                Sil1234MWPC34->caseY->push_back(2);

          
                
            }        
          }
        }
      }

      if (nH_Si1>0 && nH_Si3>0 && nH_MWPC3==0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si1; i++){
          for (int j=0; j<nH_Si3; j++){
            for (int k=0; k<nH_MWPC4; k++){
              


                vector <long double> y1234M34={Si1Y.at(i), Si3Y.at(j), MWPC4->Y->at(k)};
                vector <long double> z1234M34={Si1Z.at(i), Si3Z.at(j), MWPC4->Z->at(k)};
                vector <long double> sigma={sigma_S1*Sil1H->L0H->at(i), sigma_S3*Sil3H->L0H->at(j), sigma_M4};
                //vector <int> clsize={Sil1H->L0H->at(i), Sil3H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y*0.6) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*(i+1)+10000*nH_Si2+1000*(j+1)+100*nH_Si4+(k+1));
                Sil1234MWPC34->caseY->push_back(3);

          
                
            }        
          }
        }
      }

      if (nH_Si1>0 && nH_Si3==0 && nH_MWPC3>0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si1; i++){
          for (int k=0; k<nH_MWPC3; k++){
            for (int l=0; l<nH_MWPC4; l++){


                vector <long double> y1234M34={Si1Y.at(i), MWPC3->Y->at(k), MWPC4->Y->at(l)};
                vector <long double> z1234M34={Si1Z.at(i), MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_S1*Sil1H->L0H->at(i), sigma_M3, sigma_M4};
                //vector <int> clsize={Sil1H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y*0.6) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*(i+1)+10000*nH_Si2+1000*0+100*nH_Si4+10*(k+1)+(l+1));
                Sil1234MWPC34->caseY->push_back(4);

          
            }      
          }        
        }
      }

      if (nH_Si1==0 && nH_Si3>0 && nH_MWPC3>0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si3; i++){
          for (int k=0; k<nH_MWPC3; k++){
            for (int l=0; l<nH_MWPC4; l++){


                vector <long double> y1234M34={Si3Y.at(i), MWPC3->Y->at(k), MWPC4->Y->at(l)};
                vector <long double> z1234M34={Si3Z.at(i), MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_S3*Sil3H->L0H->at(i), sigma_M3, sigma_M4};
                //vector <int> clsize={Sil3H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y*0.6) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*0+10000*nH_Si2+1000*(i+1)+100*nH_Si4+10*(k+1)+(l+1));
                Sil1234MWPC34->caseY->push_back(5);

          
            }      
          }        
        }
      }

      if (nH_Si1>0 && nH_Si3>0 && nH_MWPC3==0 && nH_MWPC4==0) {
        for (int i=0; i<nH_Si1; i++){
          for (int j=0; j<nH_Si3; j++){
              


                vector <long double> y1234M34={Si1Y.at(i), Si3Y.at(j)};
                vector <long double> z1234M34={Si1Z.at(i), Si3Z.at(j)};
                vector <long double> sigma={sigma_S1*Sil1H->L0H->at(i), sigma_S3*Sil3H->L0H->at(j)};
                //vector <int> clsize={Sil1H->L0H->at(i), Sil3H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*(i+1)+10000*nH_Si2+1000*(j+1)+100*nH_Si4);
                Sil1234MWPC34->caseY->push_back(6);

                
          }
        }
      }

      if (nH_Si1>0 && nH_Si3==0 && nH_MWPC3>0 && nH_MWPC4==0) {
        for (int i=0; i<nH_Si1; i++){
          for (int k=0; k<nH_MWPC3; k++){

                vector <long double> y1234M34={Si1Y.at(i), MWPC3->Y->at(k)};
                vector <long double> z1234M34={Si1Z.at(i), MWPC3->Z->at(k)};
                vector <long double> sigma={sigma_S1*Sil1H->L0H->at(i), sigma_M3};
                //vector <int> clsize={Sil1H->L0H->at(i)};
                
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*(i+1)+10000*nH_Si2+1000*0+100*nH_Si4+10*(k+1)+0);
                Sil1234MWPC34->caseY->push_back(7);

              
          }        
        }
      }

      if (nH_Si1>0 && nH_Si3==0 && nH_MWPC3==0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si1; i++){
          for (int k=0; k<nH_MWPC4; k++){

                vector <long double> y1234M34={Si1Y.at(i), MWPC4->Y->at(k)};
                vector <long double> z1234M34={Si1Z.at(i), MWPC4->Z->at(k)};
                vector <long double> sigma={sigma_S1*Sil1H->L0H->at(i), sigma_M4};
                //vector <int> clsize={Sil1H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*(i+1)+10000*nH_Si2+1000*0+100*nH_Si4+(k+1));
                Sil1234MWPC34->caseY->push_back(8);

              
          }        
        }
      }

      if (nH_Si1==0 && nH_Si3>0 && nH_MWPC3>0 && nH_MWPC4==0) {
        for (int i=0; i<nH_Si3; i++){
          for (int k=0; k<nH_MWPC3; k++){

                vector <long double> y1234M34={Si3Y.at(i), MWPC3->Y->at(k)};
                vector <long double> z1234M34={Si3Z.at(i), MWPC3->Z->at(k)};
                vector <long double> sigma={sigma_S3*Sil3H->L0H->at(i), sigma_M3};
                //vector <int> clsize={Sil3H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*0+10000*nH_Si2+1000*(i+1)+100*nH_Si4+10*(k+1)+0);
                Sil1234MWPC34->caseY->push_back(9);

              
          }        
        }
      }

      if (nH_Si1==0 && nH_Si3>0 && nH_MWPC3==0 && nH_MWPC4>0) {
        for (int i=0; i<nH_Si3; i++){
          for (int k=0; k<nH_MWPC4; k++){

                vector <long double> y1234M34={Si3Y.at(i), MWPC4->Y->at(k)};
                vector <long double> z1234M34={Si3Z.at(i), MWPC4->Z->at(k)};
                vector <long double> sigma={sigma_S3*Sil3H->L0H->at(i), sigma_M4};
                //vector <int> clsize={Sil3H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*0+10000*nH_Si2+1000*(i+1)+100*nH_Si4+10*0+(k+1));
                Sil1234MWPC34->caseY->push_back(10);

              
          }        
        }
      }

      if (nH_Si1==0 && nH_Si3==0 && nH_MWPC3>0 && nH_MWPC4>0) {
        for (int k=0; k<nH_MWPC3; k++){
          for (int l=0; l<nH_MWPC4; l++){


                vector <long double> y1234M34={MWPC3->Y->at(k), MWPC4->Y->at(l)};
                vector <long double> z1234M34={MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_M3, sigma_M4};
                //vector <int> clsize={0};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*0+10000*nH_Si2+1000*0+100*nH_Si4+10*(k+1)+(l+1));
                Sil1234MWPC34->caseY->push_back(11);

                
          }        
        }
      }
      

      if (nH_Si2>0 && nH_Si4>0 && nH_MWPC3>0 && nH_MWPC4>0 && Sil1234MWPC34->nTracksX==0) {
        for (int i=0; i<nH_Si2; i++){
          for (int j=0; j<nH_Si4; j++){
            for (int k=0; k<nH_MWPC3; k++){
              


                vector <long double> x1234M34={Si2X.at(i), Si4X.at(j), MWPC3->X->at(k)};
                vector <long double> z1234M34={Si2Z.at(i), Si4Z.at(j), MWPC3->Z->at(k)};
                vector <long double> sigma={sigma_S2*Sil2H->L0H->at(i), sigma_S4*Sil4H->L0H->at(j), sigma_M3};
                //vector <int> clsize={Sil2H->L0H->at(i), Sil4H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X*0.6) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*(i+1)+1000*nH_Si3+100*(j+1)+10*(k+1));
                Sil1234MWPC34->caseX->push_back(12);

          
                
            }        
          }
        }
        for (int i=0; i<nH_Si2; i++){
          for (int j=0; j<nH_Si4; j++){
            for (int k=0; k<nH_MWPC4; k++){
              


                vector <long double> x1234M34={Si2X.at(i), Si4X.at(j), MWPC4->X->at(k)};
                vector <long double> z1234M34={Si2Z.at(i), Si4Z.at(j), MWPC4->Z->at(k)};
                vector <long double> sigma={sigma_S2*Sil2H->L0H->at(i), sigma_S4*Sil4H->L0H->at(j), sigma_M4};
                //vector <int> clsize={Sil2H->L0H->at(i), Sil4H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X*0.6) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*(i+1)+1000*nH_Si3+100*(j+1)+(k+1));
                Sil1234MWPC34->caseX->push_back(13);

          
                
            }        
          }
        }
        for (int i=0; i<nH_Si2; i++){
          for (int k=0; k<nH_MWPC3; k++){
            for (int l=0; l<nH_MWPC4; l++){


                vector <long double> x1234M34={Si2X.at(i), MWPC3->X->at(k), MWPC4->X->at(l)};
                vector <long double> z1234M34={Si2Z.at(i), MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_S2*Sil2H->L0H->at(i), sigma_M3, sigma_M4};
                //vector <int> clsize={Sil2H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X*0.6) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*(i+1)+1000*nH_Si3+100*0+10*(k+1)+(l+1));
                Sil1234MWPC34->caseX->push_back(14);

                //cout<<Sil1234MWPC34->IDX->at(i)<<"  "<<i+1<<"j  "<<nH_Si3<<"  "<<l+1<<"  "<<endl;

          
            }      
          }        
        }
        for (int i=0; i<nH_Si4; i++){
          for (int k=0; k<nH_MWPC3; k++){
            for (int l=0; l<nH_MWPC4; l++){


                vector <long double> x1234M34={Si4X.at(i), MWPC3->X->at(k), MWPC4->X->at(l)};
                vector <long double> z1234M34={Si4Z.at(i), MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_S4*Sil4H->L0H->at(i), sigma_M3, sigma_M4};
                //vector <int> clsize={Sil4H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
                if (abs(ax*ZtargPos+bx)>CutVX) continue;
                if (chi2x>CutChi2X*0.6) continue;
                Sil1234MWPC34->Tx->push_back(ax);
                Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
                Sil1234MWPC34->Chi2X->push_back(chi2x);
                Sil1234MWPC34->nTracksX++;
                Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*0+1000*nH_Si3+100*(i+1)+10*(k+1)+(l+1));
                Sil1234MWPC34->caseX->push_back(15);

        
            }      
          }        
        }
      }

      if (nH_Si1>0 && nH_Si3>0 && nH_MWPC3>0 && nH_MWPC4>0 && Sil1234MWPC34->nTracksY==0) {

        for (int i=0; i<nH_Si1; i++){
          for (int j=0; j<nH_Si3; j++){
            for (int k=0; k<nH_MWPC3; k++){
              


                vector <long double> y1234M34={Si1Y.at(i), Si3Y.at(j), MWPC3->Y->at(k)};
                vector <long double> z1234M34={Si1Z.at(i), Si3Z.at(j), MWPC3->Z->at(k)};
                vector <long double> sigma={sigma_S1*Sil1H->L0H->at(i), sigma_S3*Sil3H->L0H->at(j), sigma_M3};
                //vector <int> clsize={Sil1H->L0H->at(i), Sil3H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y*0.6) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*(i+1)+10000*nH_Si2+1000*(j+1)+100*nH_Si4+10*(k+1)+0);
                Sil1234MWPC34->caseY->push_back(12);

          
                
            }        
          }
        }
        for (int i=0; i<nH_Si1; i++){
          for (int j=0; j<nH_Si3; j++){
            for (int k=0; k<nH_MWPC4; k++){
              


                vector <long double> y1234M34={Si1Y.at(i), Si3Y.at(j), MWPC4->Y->at(k)};
                vector <long double> z1234M34={Si1Z.at(i), Si3Z.at(j), MWPC4->Z->at(k)};
                vector <long double> sigma={sigma_S1*Sil1H->L0H->at(i), sigma_S3*Sil3H->L0H->at(j), sigma_M4};
                //vector <int> clsize={Sil1H->L0H->at(i), Sil3H->L0H->at(j)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y*0.6) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*(i+1)+10000*nH_Si2+1000*(j+1)+100*nH_Si4+(k+1));
                Sil1234MWPC34->caseY->push_back(13);

          
                
            }        
          }
        }
        for (int i=0; i<nH_Si1; i++){
          for (int k=0; k<nH_MWPC3; k++){
            for (int l=0; l<nH_MWPC4; l++){


                vector <long double> y1234M34={Si1Y.at(i), MWPC3->Y->at(k), MWPC4->Y->at(l)};
                vector <long double> z1234M34={Si1Z.at(i), MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_S1*Sil1H->L0H->at(i), sigma_M3, sigma_M4};
                //vector <int> clsize={Sil1H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y*0.6) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*(i+1)+10000*nH_Si2+1000*0+100*nH_Si4+10*(k+1)+(l+1));
                Sil1234MWPC34->caseY->push_back(14);

          
            }      
          }        
        }
        for (int i=0; i<nH_Si3; i++){
          for (int k=0; k<nH_MWPC3; k++){
            for (int l=0; l<nH_MWPC4; l++){


                vector <long double> y1234M34={Si3Y.at(i), MWPC3->Y->at(k), MWPC4->Y->at(l)};
                vector <long double> z1234M34={Si3Z.at(i), MWPC3->Z->at(k), MWPC4->Z->at(l)};
                vector <long double> sigma={sigma_S3*Sil3H->L0H->at(i), sigma_M3, sigma_M4};
                //vector <int> clsize={Sil3H->L0H->at(i)};
                ax, bx, ay, by, chi2x, chi2y=0;
                GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
                if (abs(ay*ZtargPos+by)>CutVY) continue;
                if (chi2y>CutChi2Y*0.6) continue;
                Sil1234MWPC34->Ty->push_back(ay);
                Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
                Sil1234MWPC34->Chi2Y->push_back(chi2y);
                Sil1234MWPC34->nTracksY++;
                Sil1234MWPC34->IDY->push_back(9000000+100000*0+10000*nH_Si2+1000*(i+1)+100*nH_Si4+10*(k+1)+(l+1));
                Sil1234MWPC34->caseY->push_back(15);

          
            }      
          }        
        }
      
      }

      if (nH_MWPC3>0 && nH_MWPC4>0 && Sil1234MWPC34->nTracksX==0 && Sil1234MWPC34->nTracksY!=0) {
        //cout<<ev<<" case 16 "<<Sil1234MWPC34->nTracksY<<endl;
        double BestChi2=100000;
        int bestX=-1;
        int bestY=-1;
        
        for (int i=0; i<Sil1234MWPC34->nTracksY; i++){ 
          if (Sil1234MWPC34->Chi2Y->at(i)<BestChi2) {
            BestChi2=Sil1234MWPC34->Chi2Y->at(i);
            bestY=i;
          }
        }

        if (Sil1234MWPC34->caseY->at(bestY)==1 || Sil1234MWPC34->caseY->at(bestY)==4 || Sil1234MWPC34->caseY->at(bestY)==5 || Sil1234MWPC34->caseY->at(bestY)==14 || Sil1234MWPC34->caseY->at(bestY)==15 || Sil1234MWPC34->caseY->at(bestY)==11) {      
          for (int k=0; k<nH_MWPC3; k++){
            for (int l=0; l<nH_MWPC4; l++){


              vector <long double> x1234M34={MWPC3->X->at(k), MWPC4->X->at(l)};
              vector <long double> z1234M34={MWPC3->Z->at(k), MWPC4->Z->at(l)};
              vector <long double> sigma={sigma_M3, sigma_M4};
              //vector <int> clsize={0};
              ax, bx, ay, by, chi2x, chi2x=0;
              GetFitParam(x1234M34,z1234M34, sigma, ax, bx, chi2x);
              if (abs(ax*ZtargPos+bx)>CutVX) continue;
              if (chi2x>CutChi2X) continue;
              Sil1234MWPC34->Tx->push_back(ax);
              Sil1234MWPC34->X->push_back(ax*SiTrackZPos+bx);
              Sil1234MWPC34->Chi2X->push_back(chi2x);
              Sil1234MWPC34->nTracksX++;
              Sil1234MWPC34->IDX->push_back(9000000+100000*nH_Si1+10000*0+1000*nH_Si3+100*0+10*(k+1)+(l+1));
              Sil1234MWPC34->caseX->push_back(16);

                    
            }        
          }
        }
      }

      if (nH_MWPC3>0 && nH_MWPC4>0 && Sil1234MWPC34->nTracksY==0 && Sil1234MWPC34->nTracksX!=0) {

        double BestChi2=100000;
        int bestX=-1;
        int bestY=-1;
        
        for (int i=0; i<Sil1234MWPC34->nTracksX; i++){ 
          if (Sil1234MWPC34->Chi2X->at(i)<BestChi2) {
            BestChi2=Sil1234MWPC34->Chi2X->at(i);
            bestX=i;
          }
        }
        
        if (Sil1234MWPC34->caseX->at(bestX)==1 || Sil1234MWPC34->caseX->at(bestX)==4 || Sil1234MWPC34->caseX->at(bestX)==5 || Sil1234MWPC34->caseX->at(bestX)==14 || Sil1234MWPC34->caseX->at(bestX)==15 || Sil1234MWPC34->caseX->at(bestX)==11) {      
          for (int k=0; k<nH_MWPC3; k++){
            for (int l=0; l<nH_MWPC4; l++){


              vector <long double> y1234M34={MWPC3->Y->at(k), MWPC4->Y->at(l)};
              vector <long double> z1234M34={MWPC3->Z->at(k), MWPC4->Z->at(l)};
              vector <long double> sigma={sigma_M3, sigma_M4};
              //vector <int> clsize={0};
              ax, bx, ay, by, chi2x, chi2y=0;
              GetFitParam(y1234M34,z1234M34, sigma, ay, by, chi2y);
              if (abs(ay*ZtargPos+by)>CutVY) continue;
              if (chi2y>CutChi2Y) continue;
              Sil1234MWPC34->Ty->push_back(ay);
              Sil1234MWPC34->Y->push_back(ay*SiTrackZPos+by);
              Sil1234MWPC34->Chi2Y->push_back(chi2y);
              Sil1234MWPC34->nTracksY++;
              Sil1234MWPC34->IDY->push_back(9000000+100000*0+10000*nH_Si2+1000*0+100*nH_Si4+10*(k+1)+(l+1));
              Sil1234MWPC34->caseY->push_back(16);

                    
            }        
          }
        }
      }
      
      
      
      //cout<<ev<<" 2 "<<maxTracks<<" "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<" "<<nH_MWPC3<<" "<<nH_MWPC4<<endl;
      if (Sil1234MWPC34->nTracksX==0) { 
        
        for (int i=0; i<1; i++) {
          if (nH_MWPC3>0 && nH_MWPC4>0 && Sil1234MWPC34->nTracksY>0)
          Sil1234->DY12->push_back(100*maxTracks+Sil1234MWPC34->caseY->at(i));
        }
        maxTracks=6;
        continue;
      }
      if (Sil1234MWPC34->nTracksY==0) { 
        for (int i=0; i<1; i++) {
          if (nH_MWPC3>0 && nH_MWPC4>0 && Sil1234MWPC34->nTracksX>0)
          Sil1234->DX12->push_back(100*maxTracks+Sil1234MWPC34->caseX->at(i));
        }
        maxTracks=6;
        continue;
      }
      //if (Sil1234MWPC34->caseX->at(0)==11 && Sil1234MWPC34->caseY->at(0)==11) { maxTracks=6; continue;}
     // cout<<ev<<" 3 "<<maxTracks<<" "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<" "<<nH_MWPC3<<" "<<nH_MWPC4<<endl;
      // for (int i=0; i<Sil1234MWPC34->nTracksX; i++) {
      //   cout<<ev<<" 0X "<<maxTracks<<" "<<Sil1234MWPC34->caseX->at(i)<<"  "<<Sil1234MWPC34->Chi2X->at(i)<<endl;
      // }
      // for (int i=0; i<Sil1234MWPC34->nTracksY; i++) {
      //   cout<<ev<<" 0Y "<<maxTracks<<" "<<Sil1234MWPC34->caseY->at(i)<<"  "<<Sil1234MWPC34->Chi2Y->at(i)<<endl;
      // }
      double Chi2Sum[Sil1234MWPC34->nTracksX][Sil1234MWPC34->nTracksY];
      double BestChiSum=100000;
      int bestX=-1;
      int bestY=-1;
      int nXmatch=0;
      int nYmatch=0;
      for (int i=0; i<Sil1234MWPC34->nTracksX; i++) {
        for (int j=0; j<Sil1234MWPC34->nTracksY; j++) {

          
          int S1X=int(Sil1234MWPC34->IDX->at(i)-9000000)/100000;
          int S2X=int(Sil1234MWPC34->IDX->at(i)-9000000-S1X*100000)/10000;
          int S3X=int(Sil1234MWPC34->IDX->at(i)-9000000-S1X*100000-S2X*10000)/1000;
          int S4X=int(Sil1234MWPC34->IDX->at(i)-9000000-S1X*100000-S2X*10000-S3X*1000)/100;
          int M3X=int(Sil1234MWPC34->IDX->at(i)-9000000-S1X*100000-S2X*10000-S3X*1000-S4X*100)/10;
          int M4X=int(Sil1234MWPC34->IDX->at(i)-9000000-S1X*100000-S2X*10000-S3X*1000-S4X*100-M3X*10);
          // if (Sil1234MWPC34->caseX->at(i)==11 && Sil1234MWPC34->caseY->at(j)==11)
          //cout<<ev<<" "<<Sil1234MWPC34->caseX->at(i)<<" "<<Sil1234MWPC34->nTracksX<<" "<<Sil1234MWPC34->IDX->at(i)<<"  "<<S1X<<"  "<<S2X<<"  "<<S3X<<"  "<<S4X<<"  "<<M3X<<"  "<<M4X<<endl;
          //cout<<ev<<" 1 "<<Sil1234MWPC34->caseX->at(i)<<" "<<Sil1234MWPC34->caseY->at(j)<<endl;
          int S1Y=int(Sil1234MWPC34->IDY->at(j)-9000000)/100000;
          int S2Y=int(Sil1234MWPC34->IDY->at(j)-9000000-S1Y*100000)/10000;
          int S3Y=int(Sil1234MWPC34->IDY->at(j)-9000000-S1Y*100000-S2Y*10000)/1000;
          int S4Y=int(Sil1234MWPC34->IDY->at(j)-9000000-S1Y*100000-S2Y*10000-S3Y*1000)/100;
          int M3Y=int(Sil1234MWPC34->IDY->at(j)-9000000-S1Y*100000-S2Y*10000-S3Y*1000-S4Y*100)/10;
          int M4Y=int(Sil1234MWPC34->IDY->at(j)-9000000-S1Y*100000-S2Y*10000-S3Y*1000-S4Y*100-M3Y*10);
          // if (Sil1234MWPC34->caseX->at(i)==11 && Sil1234MWPC34->caseY->at(j)==11)
          //cout<<ev<<" "<<Sil1234MWPC34->caseY->at(j)<<" "<<Sil1234MWPC34->nTracksY<<" "<<Sil1234MWPC34->IDY->at(j)<<"  "<<S1Y<<"  "<<S2Y<<"  "<<S3Y<<"  "<<S4Y<<"  "<<M3Y<<"  "<<M4Y<<endl;

          // if (M3X!=M3Y) continue;
          // if (M4X!=M4Y) continue;
          // if (caseX==12 && S2X!=0 && S4X!=0 && M3X!=0 && M4X!=0) caseX=1;
          // if (caseX==1 && !(S2X!=0 && S4X!=0 && M3X!=0 && M4X!=0)) caseX=12;
          
          // if (caseY==12 && S1Y!=0 && S3Y!=0 && M3Y!=0 && M4Y!=0) caseY=1;
          // if (caseY==1 && !(S1Y!=0 && S3Y!=0 && M3Y!=0 && M4Y!=0)) caseY=12;
      
          if (Sil1234MWPC34->caseX->at(i)<12 && Sil1234MWPC34->caseY->at(j)<12) {
            if (M3X!=M3Y) continue;
            if (M4X!=M4Y) continue;
          }
          //if (Sil1234MWPC34->caseX->at(i)==11 && Sil1234MWPC34->caseY->at(j)==11)
          //cout<<ev<<" "<<Sil1234MWPC34->caseX->at(i)<<" "<<Sil1234MWPC34->nTracksX<<" "<<Sil1234MWPC34->IDX->at(i)<<"  "<<S1X<<"  "<<S2X<<"  "<<S3X<<"  "<<S4X<<"  "<<M3X<<"  "<<M4X<<endl;
          //if (Sil1234MWPC34->caseX->at(i)==11 && Sil1234MWPC34->caseY->at(j)==11)
          //cout<<ev<<" "<<Sil1234MWPC34->caseY->at(j)<<" "<<Sil1234MWPC34->nTracksY<<" "<<Sil1234MWPC34->IDY->at(j)<<"  "<<S1Y<<"  "<<S2Y<<"  "<<S3Y<<"  "<<S4Y<<"  "<<M3Y<<"  "<<M4Y<<endl;

          else if (Sil1234MWPC34->caseX->at(i)>=12 && Sil1234MWPC34->caseY->at(j)<12 && M3X==0) {
            //if (M3X!=M3Y) continue;
            if (M4X!=M4Y) continue;
          }

          else if (Sil1234MWPC34->caseX->at(i)>=12 && Sil1234MWPC34->caseY->at(j)<12 && M4X==0) {
            if (M3X!=M3Y) continue;
            //if (M4X!=M4Y) continue;
          }

          else if (Sil1234MWPC34->caseX->at(i)<12 && Sil1234MWPC34->caseY->at(j)>=12 && M3Y==0) {
            //if (M3X!=M3Y) continue;
            if (M4X!=M4Y) continue;
          }

          else if (Sil1234MWPC34->caseX->at(i)<12 && Sil1234MWPC34->caseY->at(j)>=12 && M4Y==0) {
            if (M3X!=M3Y) continue;
            //if (M4X!=M4Y) continue;
          }

          else if (Sil1234MWPC34->caseX->at(i)>=12 && Sil1234MWPC34->caseY->at(j)>=12 && M3X==0 && M3Y==0) {
            //if (M3X!=M3Y) continue;
            if (M4X!=M4Y) continue;
          }

          else if (Sil1234MWPC34->caseX->at(i)>=12 && Sil1234MWPC34->caseY->at(j)>=12 && M4X==0 && M4Y==0) {
            if (M3X!=M3Y) continue;
            //if (M4X!=M4Y) continue;
          }

          else if (Sil1234MWPC34->caseX->at(i)>=12 && Sil1234MWPC34->caseY->at(j)>=12 && M3X==0 && M4Y==0) continue;
          else if (Sil1234MWPC34->caseX->at(i)>=12 && Sil1234MWPC34->caseY->at(j)>=12 && M4X==0 && M3Y==0) continue;

          // if (Sil1234MWPC34->caseX->at(i)==9) 
          // cout<<ev<<" "<<Sil1234MWPC34->caseX->at(i)<<" "<<Sil1234MWPC34->caseY->at(j)<<" "<<Sil1234MWPC34->Chi2X->at(i)<<" "<<Sil1234MWPC34->Chi2Y->at(j)<<endl;
          // cout<<ev<<" "<<maxTracks<<" "<<caseX<<" "<<caseY<<" ij"<<i<<" "<<j<<" "<<Sil1234MWPC34->Chi2X->at(i)<<" "<<Sil1234MWPC34->Chi2Y->at(j)<<endl;
          // if (S1Y!=0 && S2X!=0) {
          //   if (abs(Sil1H->X->at(S1Y-1)-Sil2H->X->at(S2X-1))>CutDX) cout<<ev<<" "<<maxTracks<<" 12X Si1X  "<<Sil1H->X->at(S1Y-1)<<"  Si2X "<<Sil2H->X->at(S2X-1)<<" Chi2  "<<Sil1234MWPC34->Chi2X->at(i)<<" "<<Sil1234MWPC34->Chi2Y->at(j)<<endl;
          //   if (abs(Sil1H->Y->at(S1Y-1)-Sil2H->Y->at(S2X-1))>CutDY) cout<<ev<<" "<<maxTracks<<" 12Y Si1Y  "<<Sil1H->Y->at(S1Y-1)<<"  Si2Y "<<Sil2H->Y->at(S2X-1)<<" Chi2  "<<Sil1234MWPC34->Chi2X->at(i)<<" "<<Sil1234MWPC34->Chi2Y->at(j)<<endl;
          // }
 
          // if (S3Y!=0 && S4X!=0) {
          //   // dX43.push_back(Sil3H->X->at(S3Y-1)-Sil4H->X->at(S4X-1));
          //   // dY43.push_back(Sil3H->Y->at(S3Y-1)-Sil4H->Y->at(S4X-1));
          //   if (abs(Sil3H->X->at(S3Y-1)-Sil4H->X->at(S4X-1))>CutDX) cout<<ev<<" "<<maxTracks<<" 34X Si3X  "<<Sil3H->X->at(S3Y-1)<<"  Si4X "<<Sil4H->X->at(S4X-1)<<" Chi2  "<<Sil1234MWPC34->Chi2X->at(i)<<" "<<Sil1234MWPC34->Chi2Y->at(j)<<endl;
          //   if (abs(Sil3H->Y->at(S3Y-1)-Sil4H->Y->at(S4X-1))>CutDY) cout<<ev<<" "<<maxTracks<<" 34Y Si3Y  "<<Sil3H->Y->at(S3Y-1)<<"  Si4Y "<<Sil4H->Y->at(S4X-1)<<" Chi2  "<<Sil1234MWPC34->Chi2X->at(i)<<" "<<Sil1234MWPC34->Chi2Y->at(j)<<endl;
          // }

          // cout<<ev<<" "<<maxTracks<<" "<<caseX<<" "<<caseY<<" ij"<<i<<" "<<j<<" "<<Sil1234MWPC34->Chi2X->at(i)<<" "<<Sil1234MWPC34->Chi2Y->at(j)<<endl;
          // if (S1Y!=0 && S2X!=0) {
          //   if (abs(Sil1H->X->at(S1Y-1)-Sil2H->X->at(S2X-1))>CutDX) continue;
          //   if (abs(Sil1H->Y->at(S1Y-1)-Sil2H->Y->at(S2X-1))>CutDY) continue;
          // }
 
          // if (S3Y!=0 && S4X!=0) {
          //   dX43.push_back(Sil3H->X->at(S3Y-1)-Sil4H->X->at(S4X-1));
          //   dY43.push_back(Sil3H->Y->at(S3Y-1)-Sil4H->Y->at(S4X-1));
          //   if (abs(Sil3H->X->at(S3Y-1)-Sil4H->X->at(S4X-1))>CutDX) continue;
          //   if (abs(Sil3H->Y->at(S3Y-1)-Sil4H->Y->at(S4X-1))>CutDY) continue;
          // }

        
          

          if (Sil1234MWPC34->Chi2X->at(i)==0 && Sil1234MWPC34->nTracksX>1) nXmatch++;
          if (Sil1234MWPC34->Chi2Y->at(j)==0 && Sil1234MWPC34->nTracksY>1) nYmatch++;
          //if (Sil1234MWPC34->Chi2X->at(i)>0 && Sil1234MWPC34->Chi2Y->at(j)>0){
          
          
          Chi2Sum[i][j]=Sil1234MWPC34->Chi2X->at(i)+Sil1234MWPC34->Chi2Y->at(j);
          if (Chi2Sum[i][j]<BestChiSum) {
            BestChiSum=Chi2Sum[i][j];
            bestX=i;
            bestY=j;
          }
                


          //}
        }
      }
      for (int i=0; i<Sil1234MWPC34->nTracksX; i++) {
        for (int j=0; j<Sil1234MWPC34->nTracksX; j++) {
          if (i>=j) continue;
          if (Sil1234MWPC34->caseX->at(i)==1 && Sil1234MWPC34->caseX->at(j)==13) {
              dX43.push_back(Sil1234MWPC34->Chi2X->at(i)-Sil1234MWPC34->Chi2X->at(j));
          }
        }
      }

      for (int i=0; i<Sil1234MWPC34->nTracksY; i++) {
        for (int j=0; j<Sil1234MWPC34->nTracksY; j++) {
          if (i>=j) continue;
          if (Sil1234MWPC34->caseY->at(i)==1 && Sil1234MWPC34->caseY->at(j)==13) {
              dY43.push_back(Sil1234MWPC34->Chi2Y->at(i)-Sil1234MWPC34->Chi2Y->at(j));
          }
        }
      }

          
      if (bestX<0 || bestY<0) { 

        maxTracks=6;
        continue;

      }
      //if (bestY<0) { maxTracks=6; continue;}
      //if (Sil1234MWPC34->caseX->at(bestX)==11 && Sil1234MWPC34->caseY->at(bestY)==11) 
      //cout<<ev<<" "<<Sil1234MWPC34->caseX->at(bestX)<<" "<<Sil1234MWPC34->caseY->at(bestY)<<" "<<nXmatch<<" "<<nYmatch<<" "<<Sil1234MWPC34->nTracksX<<" "<<Sil1234MWPC34->nTracksY<<endl;
      if (nXmatch>1 || nYmatch>1) { 
        for (int i=0; i<1; i++) {
          if (MWPC_p1->nTracks>0)
          Sil1234->DX34->push_back(100*maxTracks+Sil1234MWPC34->caseX->at(i));
        }
        for (int i=0; i<1; i++) {
          if (MWPC_p1->nTracks>0)
          Sil1234->DY34->push_back(100*maxTracks+Sil1234MWPC34->caseY->at(i));
        }
        maxTracks=6; 
        continue;}
      //if (nYmatch>1 && Sil1234MWPC34->caseY->at(bestY)!=11) { maxTracks=6; continue;}

      //cout<<"best chi2  "<<Sil1234MWPC34->Chi2X->at(bestX)<<" "<<Sil1234MWPC34->Chi2Y->at(bestY)<<endl;

      Sil1234->X->push_back(Sil1234MWPC34->X->at(bestX));
      Sil1234->Y->push_back(Sil1234MWPC34->Y->at(bestY));
      Sil1234->Z->push_back(SiTrackZPos);
      Sil1234->Tx->push_back(Sil1234MWPC34->Tx->at(bestX));
      Sil1234->Ty->push_back(Sil1234MWPC34->Ty->at(bestY));
      Sil1234->Chi2X->push_back(Sil1234MWPC34->Chi2X->at(bestX));
      Sil1234->Chi2Y->push_back(Sil1234MWPC34->Chi2Y->at(bestY));
      Sil1234->nTracks++;
     
      if (Sil1234->nTracks==1 && Sil1234MWPC34->caseX->at(bestX)==1 && Sil1234MWPC34->caseY->at(bestY)==1 && ev<1000)
      cout<<Sil1234MWPC34->Chi2X->at(bestX)<<" "<<Sil1234MWPC34->Chi2Y->at(bestY)<<endl;



      int S1X=int(Sil1234MWPC34->IDX->at(bestX)-9000000)/100000;
      int S2X=int(Sil1234MWPC34->IDX->at(bestX)-9000000-S1X*100000)/10000;
      int S3X=int(Sil1234MWPC34->IDX->at(bestX)-9000000-S1X*100000-S2X*10000)/1000;
      int S4X=int(Sil1234MWPC34->IDX->at(bestX)-9000000-S1X*100000-S2X*10000-S3X*1000)/100;
      int M3X=int(Sil1234MWPC34->IDX->at(bestX)-9000000-S1X*100000-S2X*10000-S3X*1000-S4X*100)/10;
      int M4X=int(Sil1234MWPC34->IDX->at(bestX)-9000000-S1X*100000-S2X*10000-S3X*1000-S4X*100-M3X*10);

      //cout<<ev<<" "<<Sil1234MWPC34->nTracksX<<" "<<Sil1234MWPC34->IDX->at(bestX)<<"  "<<S1X<<"  "<<S2X<<"  "<<S3X<<"  "<<S4X<<"  "<<M3X<<"  "<<M4X<<endl;

      int S1Y=int(Sil1234MWPC34->IDY->at(bestY)-9000000)/100000;
      int S2Y=int(Sil1234MWPC34->IDY->at(bestY)-9000000-S1Y*100000)/10000;
      int S3Y=int(Sil1234MWPC34->IDY->at(bestY)-9000000-S1Y*100000-S2Y*10000)/1000;
      int S4Y=int(Sil1234MWPC34->IDY->at(bestY)-9000000-S1Y*100000-S2Y*10000-S3Y*1000)/100;
      int M3Y=int(Sil1234MWPC34->IDY->at(bestY)-9000000-S1Y*100000-S2Y*10000-S3Y*1000-S4Y*100)/10;
      int M4Y=int(Sil1234MWPC34->IDY->at(bestY)-9000000-S1Y*100000-S2Y*10000-S3Y*1000-S4Y*100-M3Y*10);

      //cout<<ev<<" "<<Sil1234MWPC34->nTracksY<<" "<<Sil1234MWPC34->IDY->at(bestY)<<"  "<<S1Y<<"  "<<S2Y<<"  "<<S3Y<<"  "<<S4Y<<"  "<<M3Y<<"  "<<M4Y<<endl;
      
      // if (caseX==12 && S2X!=0 && S4X!=0 && M3X!=0 && M4X!=0) caseX=1;
      // if (caseX==1 && !(S2X!=0 && S4X!=0 && M3X!=0 && M4X!=0)) caseX=12;
          
      // if (caseY==12 && S1Y!=0 && S3Y!=0 && M3Y!=0 && M4Y!=0) caseY=1;
      // if (caseY==1 && !(S1Y!=0 && S3Y!=0 && M3Y!=0 && M4Y!=0)) caseY=12;

      Sil1234->IDX->push_back(Sil1234->nTracks*100+Sil1234MWPC34->caseX->at(bestX));
      Sil1234->IDY->push_back(Sil1234->nTracks*100+Sil1234MWPC34->caseY->at(bestY));

      // if (S1Y!=0 && S2X!=0) {
      //   Sil1234->DX12->push_back(Sil1H->X->at(S1Y-1)-Sil2H->X->at(S2X-1));
      //   Sil1234->DY12->push_back(Sil1H->Y->at(S1Y-1)-Sil2H->Y->at(S2X-1));
      // }
      // else {
      //   Sil1234->DX12->push_back(0);
      //   Sil1234->DY12->push_back(0);
      // }
      // if (S3Y!=0 && S4X!=0) {
      //   Sil1234->DX34->push_back(Sil3H->X->at(S3Y-1)-Sil4H->X->at(S4X-1));
      //   Sil1234->DY34->push_back(Sil3H->Y->at(S3Y-1)-Sil4H->Y->at(S4X-1));
      // }
      //  else {
      //   Sil1234->DX34->push_back(0);
      //   Sil1234->DY34->push_back(0);
      // }


     
      // if (S3Y!=0 && S4X!=0) {
      //   dX43.push_back(Sil3H->X->at(S3Y-1)-Sil4H->X->at(S4X-1));
      //   dY43.push_back(Sil3H->Y->at(S3Y-1)-Sil4H->Y->at(S4X-1));
      // }

      // if (S3Y!=0 && S4X!=0) {
      //   dX43.push_back(Sil3H->X->at(S3Y-1)-(Sil1234->X->at(Sil1234->X->size()-1)+(Si3Z.at(S3Y-1)-SiTrackZPos)*Sil1234->Tx->at(Sil1234->Tx->size()-1)));
      //   dY43.push_back((Sil1234->Y->at(Sil1234->Y->size()-1)+(Si4Z.at(S4X-1)-SiTrackZPos)*Sil1234->Ty->at(Sil1234->Ty->size()-1))-Sil4H->Y->at(S4X-1));
      // }


      for (int i=0; i<nH_Si1; i++) {
        //cout<<Si1X.at(i)<<" before "<< Si1Y.at(i)<<endl;
      }
      
      if (S1Y!=0) {
        Sil1HU->X->push_back(Sil1H->X->at(S1Y-1));
        Sil1HU->Y->push_back(Sil1H->Y->at(S1Y-1));
        Sil1HU->St->push_back(Sil1H->St->at(S1Y-1));
        Sil1HU->Mod->push_back(Sil1H->Mod->at(S1Y-1));
        Sil1HU->L0H->push_back(Sil1H->L0H->at(S1Y-1));
        Sil1HU->L1H->push_back(Sil1H->L1H->at(S1Y-1));
        Sil1HU->AmpDir->push_back(Sil1H->AmpDir->at(S1Y-1));
        Sil1HU->AmpIncl->push_back(Sil1H->AmpIncl->at(S1Y-1));
        Sil1HU->nHits++;
        Si1X.erase(Si1X.begin()+S1Y-1);
        Si1Y.erase(Si1Y.begin()+S1Y-1);
        Si1Z.erase(Si1Z.begin()+S1Y-1);
        Sil1H->X->erase(Sil1H->X->begin()+S1Y-1);
        Sil1H->Y->erase(Sil1H->Y->begin()+S1Y-1);
        Sil1H->St->erase(Sil1H->St->begin()+S1Y-1);
        Sil1H->Mod->erase(Sil1H->Mod->begin()+S1Y-1);
        Sil1H->L0H->erase(Sil1H->L0H->begin()+S1Y-1);
        Sil1H->L1H->erase(Sil1H->L1H->begin()+S1Y-1);
        Sil1H->AmpDir->erase(Sil1H->AmpDir->begin()+S1Y-1);
        Sil1H->AmpIncl->erase(Sil1H->AmpIncl->begin()+S1Y-1);
        Sil1H->nHits--;
        nH_Si1--;
      }
      else {
        Sil1HU->X->push_back(0);
        Sil1HU->Y->push_back(0);
        Sil1HU->St->push_back(-1);
        Sil1HU->Mod->push_back(-1);
        Sil1HU->L0H->push_back(0);
        Sil1HU->L1H->push_back(0);
        Sil1HU->AmpDir->push_back(0);
        Sil1HU->AmpIncl->push_back(0);
        Sil1HU->nHits++;
      }

      for (int i=0; i<nH_Si1; i++) {
        //cout<<Si1X.at(i)<<" after "<< Si1Y.at(i)<<endl;
      }

      if (S2X!=0) {
        Sil2HU->X->push_back(Sil2H->X->at(S2X-1));
        Sil2HU->Y->push_back(Sil2H->Y->at(S2X-1));
        Sil2HU->St->push_back(Sil2H->St->at(S2X-1));
        Sil2HU->Mod->push_back(Sil2H->Mod->at(S2X-1));
        Sil2HU->L0H->push_back(Sil2H->L0H->at(S2X-1));
        Sil2HU->L1H->push_back(Sil2H->L1H->at(S2X-1));
        Sil2HU->AmpDir->push_back(Sil2H->AmpDir->at(S2X-1));
        Sil2HU->AmpIncl->push_back(Sil2H->AmpIncl->at(S2X-1));
        Sil2HU->nHits++;
        Si2X.erase(Si2X.begin()+S2X-1);
        Si2Y.erase(Si2Y.begin()+S2X-1);
        Si2Z.erase(Si2Z.begin()+S2Y-1);
        Sil2H->X->erase(Sil2H->X->begin()+S2X-1);
        Sil2H->Y->erase(Sil2H->Y->begin()+S2X-1);
        Sil2H->St->erase(Sil2H->St->begin()+S2X-1);
        Sil2H->Mod->erase(Sil2H->Mod->begin()+S2X-1);
        Sil2H->L0H->erase(Sil2H->L0H->begin()+S2X-1);
        Sil2H->L1H->erase(Sil2H->L1H->begin()+S2X-1);
        Sil2H->AmpDir->erase(Sil2H->AmpDir->begin()+S2X-1);
        Sil2H->AmpIncl->erase(Sil2H->AmpIncl->begin()+S2X-1);
        Sil2H->nHits--;
        nH_Si2--;
      }
      else {
        Sil2HU->X->push_back(0);
        Sil2HU->Y->push_back(0);
        Sil2HU->St->push_back(-1);
        Sil2HU->Mod->push_back(-1);
        Sil2HU->L0H->push_back(0);
        Sil2HU->L1H->push_back(0);
        Sil2HU->AmpDir->push_back(0);
        Sil2HU->AmpIncl->push_back(0);
        Sil2HU->nHits++;
      }

      if (S3Y!=0) {
        Sil3HU->X->push_back(Sil3H->X->at(S3Y-1));
        Sil3HU->Y->push_back(Sil3H->Y->at(S3Y-1));
        Sil3HU->St->push_back(Sil3H->St->at(S3Y-1));
        Sil3HU->Mod->push_back(Sil3H->Mod->at(S3Y-1));
        Sil3HU->L0H->push_back(Sil3H->L0H->at(S3Y-1));
        Sil3HU->L1H->push_back(Sil3H->L1H->at(S3Y-1));
        Sil3HU->AmpDir->push_back(Sil3H->AmpDir->at(S3Y-1));
        Sil3HU->AmpIncl->push_back(Sil3H->AmpIncl->at(S3Y-1));
        Sil3HU->nHits++;
        Si3X.erase(Si3X.begin()+S3Y-1);
        Si3Y.erase(Si3Y.begin()+S3Y-1);
        Si3Z.erase(Si3Z.begin()+S3Y-1);
        Sil3H->X->erase(Sil3H->X->begin()+S3Y-1);
        Sil3H->Y->erase(Sil3H->Y->begin()+S3Y-1);
        Sil3H->St->erase(Sil3H->St->begin()+S3Y-1);
        Sil3H->Mod->erase(Sil3H->Mod->begin()+S3Y-1);
        Sil3H->L0H->erase(Sil3H->L0H->begin()+S3Y-1);
        Sil3H->L1H->erase(Sil3H->L1H->begin()+S3Y-1);
        Sil3H->AmpDir->erase(Sil3H->AmpDir->begin()+S3Y-1);
        Sil3H->AmpIncl->erase(Sil3H->AmpIncl->begin()+S3Y-1);
        Sil3H->nHits--;
        nH_Si3--;
      }

      else {
        Sil3HU->X->push_back(0);
        Sil3HU->Y->push_back(0);
        Sil3HU->St->push_back(-1);
        Sil3HU->Mod->push_back(-1);
        Sil3HU->L0H->push_back(0);
        Sil3HU->L1H->push_back(0);
        Sil3HU->AmpDir->push_back(0);
        Sil3HU->AmpIncl->push_back(0);
        Sil3HU->nHits++;
      }

      if (S4X!=0) {
        Sil4HU->X->push_back(Sil4H->X->at(S4X-1));
        Sil4HU->Y->push_back(Sil4H->Y->at(S4X-1));
        Sil4HU->St->push_back(Sil4H->St->at(S4X-1));
        Sil4HU->Mod->push_back(Sil4H->Mod->at(S4X-1));
        Sil4HU->L0H->push_back(Sil4H->L0H->at(S4X-1));
        Sil4HU->L1H->push_back(Sil4H->L1H->at(S4X-1));
        Sil4HU->AmpDir->push_back(Sil4H->AmpDir->at(S4X-1));
        Sil4HU->AmpIncl->push_back(Sil4H->AmpIncl->at(S4X-1));
        Sil4HU->nHits++;
        Si4X.erase(Si4X.begin()+S4X-1);
        Si4Y.erase(Si4Y.begin()+S4X-1);
        Si4Z.erase(Si4Z.begin()+S4Y-1);
        Sil4H->X->erase(Sil4H->X->begin()+S4X-1);
        Sil4H->Y->erase(Sil4H->Y->begin()+S4X-1);
        Sil4H->St->erase(Sil4H->St->begin()+S4X-1);
        Sil4H->Mod->erase(Sil4H->Mod->begin()+S4X-1);
        Sil4H->L0H->erase(Sil4H->L0H->begin()+S4X-1);
        Sil4H->L1H->erase(Sil4H->L1H->begin()+S4X-1);
        Sil4H->AmpDir->erase(Sil4H->AmpDir->begin()+S4X-1);
        Sil4H->AmpIncl->erase(Sil4H->AmpIncl->begin()+S4X-1);
        Sil4H->nHits--;
        nH_Si4--;
      }

      else {
        Sil4HU->X->push_back(0);
        Sil4HU->Y->push_back(0);
        Sil4HU->St->push_back(-1);
        Sil4HU->Mod->push_back(-1);
        Sil4HU->L0H->push_back(0);
        Sil4HU->L1H->push_back(0);
        Sil4HU->AmpDir->push_back(0);
        Sil4HU->AmpIncl->push_back(0);
        Sil4HU->nHits++;
      }

    
      for (int i=0; i<nH_MWPC3; i++) {
        //cout<<MWPC3->X->at(i)<<" M3 "<< MWPC3->Y->at(i)<<endl;
      }
      if (M3X!=0) {
        MWPC3U->X->push_back(MWPC3->X->at(M3X-1));
        MWPC3U->Y->push_back(MWPC3->Y->at(M3X-1));
        MWPC3U->Z->push_back(MWPC3->Z->at(M3X-1));
        MWPC3U->Tx->push_back(MWPC3->Tx->at(M3X-1));
        MWPC3U->Ty->push_back(MWPC3->Ty->at(M3X-1));
        MWPC3U->Chi2X->push_back(MWPC3->Chi2X->at(M3X-1));
        MWPC3U->nTracks++;
        MWPC3->X->erase(MWPC3->X->begin()+M3X-1);
        MWPC3->Y->erase(MWPC3->Y->begin()+M3X-1);
        MWPC3->Z->erase(MWPC3->Z->begin()+M3X-1);
        MWPC3->Tx->erase(MWPC3->Tx->begin()+M3X-1);
        MWPC3->Ty->erase(MWPC3->Ty->begin()+M3X-1);
        MWPC3->Chi2X->erase(MWPC3->Chi2X->begin()+M3X-1);
        MWPC3->nTracks--;
        nH_MWPC3--;
      }
      else {
        MWPC3U->X->push_back(0);
        MWPC3U->Y->push_back(0);
        MWPC3U->Z->push_back(0);
        MWPC3U->Tx->push_back(0);
        MWPC3U->Ty->push_back(0);
        MWPC3U->Chi2X->push_back(0);
        MWPC3U->nTracks++;
      }
       
       
      // for (int i=0; i<nH_MWPC4; i++) {
      //   cout<<MWPC4->X->at(i)<<" M4 "<< MWPC4->Y->at(i)<<"  "<<M4X<<endl;
      // }

      if (M4X!=0) {
        MWPC4U->X->push_back(MWPC4->X->at(M4X-1));
        MWPC4U->Y->push_back(MWPC4->Y->at(M4X-1));
        MWPC4U->Z->push_back(MWPC4->Z->at(M4X-1));
        MWPC4U->Tx->push_back(MWPC4->Tx->at(M4X-1));
        MWPC4U->Ty->push_back(MWPC4->Ty->at(M4X-1));
        MWPC4U->Chi2X->push_back(MWPC4->Chi2X->at(M4X-1));
        MWPC4U->nTracks++;
        MWPC4->X->erase(MWPC4->X->begin()+M4X-1);
        MWPC4->Y->erase(MWPC4->Y->begin()+M4X-1);
        MWPC4->Z->erase(MWPC4->Z->begin()+M4X-1);
        MWPC4->Tx->erase(MWPC4->Tx->begin()+M4X-1);
        MWPC4->Ty->erase(MWPC4->Ty->begin()+M4X-1);
        MWPC4->Chi2X->erase(MWPC4->Chi2X->begin()+M4X-1);
        MWPC4->nTracks--;
        nH_MWPC4--;
      }
      else {
        MWPC4U->X->push_back(0);
        MWPC4U->Y->push_back(0);
        MWPC4U->Z->push_back(0);
        MWPC4U->Tx->push_back(0);
        MWPC4U->Ty->push_back(0);
        MWPC4U->Chi2X->push_back(0);
        MWPC4U->nTracks++;
      }
       
      // cout<<nH_MWPC4<<endl;
      // for (int i=0; i<nH_MWPC4; i++) {
      //   cout<<MWPC4->X->at(i)<<"  M4A "<< MWPC4->Y->at(i)<<endl;
      // }

      //  if (Sil1234->nTracks==0 && MWPC_p1->nTracks>0){

      //   for (int i=0; i<MWPC_p1->nTracks; i++) {
      //     Sil1234->X->push_back(MWPC_p1->X->at(i)+(SiTrackZPos-MWPC_p1->Z->at(i))*MWPC_p1->Tx->at(i));
      //     Sil1234->Y->push_back(MWPC_p1->Y->at(i)+(SiTrackZPos-MWPC_p1->Z->at(i))*MWPC_p1->Ty->at(i));
      //     Sil1234->Z->push_back(SiTrackZPos);
      //     Sil1234->Tx->push_back(MWPC_p1->Tx->at(i));
      //     Sil1234->Ty->push_back(MWPC_p1->Ty->at(i));
      //     Sil1234->Chi2X->push_back(0);
      //     Sil1234->Chi2Y->push_back(0);
      //     Sil1234->DX12->push_back(0);
      //     Sil1234->DY12->push_back(0);
      //     Sil1234->nTracks++;
      //     Sil1234->IDX->push_back(Sil1234->nTracks*100+11);
      //     Sil1234->IDY->push_back(Sil1234->nTracks*100+11);
      //   }
      //   maxTracks=6;




      // }
      
      Sil1234MWPC34->X->clear();
      Sil1234MWPC34->Y->clear();
      Sil1234MWPC34->Z->clear();
      Sil1234MWPC34->Tx->clear();
      Sil1234MWPC34->Ty->clear();
      Sil1234MWPC34->Chi2X->clear();
      Sil1234MWPC34->Chi2Y->clear();
      Sil1234MWPC34->IDX->clear();
      Sil1234MWPC34->IDY->clear();
      Sil1234MWPC34->caseX->clear();
      Sil1234MWPC34->caseY->clear();
      Sil1234MWPC34->nTracks=0;
      Sil1234MWPC34->nTracksX=0;
      Sil1234MWPC34->nTracksY=0;
 
      //cout<<ev<<" "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<" "<<nH_MWPC3<<" "<<nH_MWPC4<<endl;

    }

    //cout<<ev<<" 4 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<" "<<nH_MWPC3<<" "<<nH_MWPC4<<endl;
    tree1->Fill();


    
    Sil1H->X->clear();
    Sil1H->Y->clear();
    Sil1H->St->clear();
    Sil1H->Mod->clear();
    Sil1H->L0H->clear();
    Sil1H->L1H->clear();
    Sil1H->AmpDir->clear();
    Sil1H->AmpIncl->clear();

    Sil1HF->X->clear();
    Sil1HF->Y->clear();
    Sil1HF->St->clear();
    Sil1HF->Mod->clear();
    Sil1HF->L0H->clear();
    Sil1HF->L1H->clear();
    Sil1HF->AmpDir->clear();
    Sil1HF->AmpIncl->clear();

    Sil1HU->X->clear();
    Sil1HU->Y->clear();
    Sil1HU->St->clear();
    Sil1HU->Mod->clear();
    Sil1HU->L0H->clear();
    Sil1HU->L1H->clear();
    Sil1HU->AmpDir->clear();
    Sil1HU->AmpIncl->clear();

    Sil1H_MC->X->clear();
    Sil1H_MC->Y->clear();
    Sil1H_MC->Z->clear();
    Sil1H_MC->Tx->clear();
    Sil1H_MC->Ty->clear();
    Sil1H_MC->Charge->clear();
    Sil1H_MC->Prim->clear();
    Sil1H_MC->St->clear();
    Sil1H_MC->Mod->clear();
    Sil1H_MC->L0H->clear();
    Sil1H_MC->L1H->clear();
    Sil1H_MC->AmpDir->clear();
    Sil1H_MC->AmpIncl->clear();

    Sil2H->X->clear();
    Sil2H->Y->clear();
    Sil2H->St->clear();
    Sil2H->Mod->clear();
    Sil2H->L0H->clear();
    Sil2H->L1H->clear();
    Sil2H->AmpDir->clear();
    Sil2H->AmpIncl->clear();

    Sil2HF->X->clear();
    Sil2HF->Y->clear();
    Sil2HF->St->clear();
    Sil2HF->Mod->clear();
    Sil2HF->L0H->clear();
    Sil2HF->L1H->clear();
    Sil2HF->AmpDir->clear();
    Sil2HF->AmpIncl->clear();

    Sil2HU->X->clear();
    Sil2HU->Y->clear();
    Sil2HU->St->clear();
    Sil2HU->Mod->clear();
    Sil2HU->L0H->clear();
    Sil2HU->L1H->clear();
    Sil2HU->AmpDir->clear();
    Sil2HU->AmpIncl->clear();

    Sil2H_MC->X->clear();
    Sil2H_MC->Y->clear();
    Sil2H_MC->Z->clear();
    Sil2H_MC->Tx->clear();
    Sil2H_MC->Ty->clear();
    Sil2H_MC->Charge->clear();
    Sil2H_MC->Prim->clear();
    Sil2H_MC->St->clear();
    Sil2H_MC->Mod->clear();
    Sil2H_MC->L0H->clear();
    Sil2H_MC->L1H->clear();
    Sil2H_MC->AmpDir->clear();
    Sil2H_MC->AmpIncl->clear();

    Sil3H->X->clear();
    Sil3H->Y->clear();
    Sil3H->St->clear();
    Sil3H->Mod->clear();
    Sil3H->L0H->clear();
    Sil3H->L1H->clear();
    Sil3H->AmpDir->clear();
    Sil3H->AmpIncl->clear();

    Sil3HF->X->clear();
    Sil3HF->Y->clear();
    Sil3HF->St->clear();
    Sil3HF->Mod->clear();
    Sil3HF->L0H->clear();
    Sil3HF->L1H->clear();
    Sil3HF->AmpDir->clear();
    Sil3HF->AmpIncl->clear();

    Sil3HU->X->clear();
    Sil3HU->Y->clear();
    Sil3HU->St->clear();
    Sil3HU->Mod->clear();
    Sil3HU->L0H->clear();
    Sil3HU->L1H->clear();
    Sil3HU->AmpDir->clear();
    Sil3HU->AmpIncl->clear();

    Sil3H_MC->X->clear();
    Sil3H_MC->Y->clear();
    Sil3H_MC->Z->clear();
    Sil3H_MC->Tx->clear();
    Sil3H_MC->Ty->clear();
    Sil3H_MC->Charge->clear();
    Sil3H_MC->Prim->clear();
    Sil3H_MC->St->clear();
    Sil3H_MC->Mod->clear();
    Sil3H_MC->L0H->clear();
    Sil3H_MC->L1H->clear();
    Sil3H_MC->AmpDir->clear();
    Sil3H_MC->AmpIncl->clear();

    Sil4H->X->clear();
    Sil4H->Y->clear();
    Sil4H->St->clear();
    Sil4H->Mod->clear();
    Sil4H->L0H->clear();
    Sil4H->L1H->clear();
    Sil4H->AmpDir->clear();
    Sil4H->AmpIncl->clear();

    Sil4HF->X->clear();
    Sil4HF->Y->clear();
    Sil4HF->St->clear();
    Sil4HF->Mod->clear();
    Sil4HF->L0H->clear();
    Sil4HF->L1H->clear();
    Sil4HF->AmpDir->clear();
    Sil4HF->AmpIncl->clear();

    Sil4HU->X->clear();
    Sil4HU->Y->clear();
    Sil4HU->St->clear();
    Sil4HU->Mod->clear();
    Sil4HU->L0H->clear();
    Sil4HU->L1H->clear();
    Sil4HU->AmpDir->clear();
    Sil4HU->AmpIncl->clear();

    Sil4H_MC->X->clear();
    Sil4H_MC->Y->clear();
    Sil4H_MC->Z->clear();
    Sil4H_MC->Tx->clear();
    Sil4H_MC->Ty->clear();
    Sil4H_MC->Charge->clear();
    Sil4H_MC->Prim->clear();
    Sil4H_MC->St->clear();
    Sil4H_MC->Mod->clear();
    Sil4H_MC->L0H->clear();
    Sil4H_MC->L1H->clear();
    Sil4H_MC->AmpDir->clear();
    Sil4H_MC->AmpIncl->clear();
    

    MWPC1->X->clear();
    MWPC1->Y->clear();
    MWPC1->Z->clear();
    MWPC1->Tx->clear();
    MWPC1->Ty->clear();
    MWPC1->Chi2X->clear();
    //MWPC1->Chi2Y->clear();

    MWPC1_MC->X->clear();
    MWPC1_MC->Y->clear();
    MWPC1_MC->Z->clear();
    MWPC1_MC->Charge->clear();
    MWPC1_MC->Prim->clear();
    MWPC1_MC->Tx->clear();
    MWPC1_MC->Ty->clear();
    MWPC1_MC->Chi2X->clear();
    //MWPC1_MC->Chi2Y->clear();

    MWPC2->X->clear();
    MWPC2->Y->clear();
    MWPC2->Z->clear();
    MWPC2->Tx->clear();
    MWPC2->Ty->clear();
    MWPC2->Chi2X->clear();
    //MWPC2->Chi2Y->clear();

    MWPC2_MC->X->clear();
    MWPC2_MC->Y->clear();
    MWPC2_MC->Z->clear();
    MWPC2_MC->Charge->clear();
    MWPC2_MC->Prim->clear();
    MWPC2_MC->Tx->clear();
    MWPC2_MC->Ty->clear();
    MWPC2_MC->Chi2X->clear();
    //MWPC2_MC->Chi2Y->clear();

    MWPC3->X->clear();
    MWPC3->Y->clear();
    MWPC3->Z->clear();
    MWPC3->Tx->clear();
    MWPC3->Ty->clear();
    MWPC3->Chi2X->clear();
    //MWPC3->Chi2Y->clear();

    MWPC3_MC->X->clear();
    MWPC3_MC->Y->clear();
    MWPC3_MC->Z->clear();
    MWPC3_MC->Charge->clear();
    MWPC3_MC->Prim->clear();
    MWPC3_MC->Tx->clear();
    MWPC3_MC->Ty->clear();
    MWPC3_MC->Chi2X->clear();
    //MWPC3_MC->Chi2Y->clear();

    MWPC4->X->clear();
    MWPC4->Y->clear();
    MWPC4->Z->clear();
    MWPC4->Tx->clear();
    MWPC4->Ty->clear();
    MWPC4->Chi2X->clear();
    //MWPC4->Chi2Y->clear();

    MWPC4_MC->X->clear();
    MWPC4_MC->Y->clear();
    MWPC4_MC->Z->clear();
    MWPC4_MC->Charge->clear();
    MWPC4_MC->Prim->clear();
    MWPC4_MC->Tx->clear();
    MWPC4_MC->Ty->clear();
    MWPC4_MC->Chi2X->clear();
    //MWPC4_MC->Chi2Y->clear();

    MWPC3F->X->clear();
    MWPC3F->Y->clear();
    MWPC3F->Z->clear();
    MWPC3F->Tx->clear();
    MWPC3F->Ty->clear();
    MWPC3F->Chi2X->clear();
    //MWPC3F->Chi2Y->clear();

    MWPC4F->X->clear();
    MWPC4F->Y->clear();
    MWPC4F->Z->clear();
    MWPC4F->Tx->clear();
    MWPC4F->Ty->clear();
    MWPC4F->Chi2X->clear();
    //MWPC4F->Chi2Y->clear();

    MWPC3U->X->clear();
    MWPC3U->Y->clear();
    MWPC3U->Z->clear();
    MWPC3U->Tx->clear();
    MWPC3U->Ty->clear();
    MWPC3U->Chi2X->clear();
    //MWPC3U->Chi2Y->clear();

    MWPC4U->X->clear();
    MWPC4U->Y->clear();
    MWPC4U->Z->clear();
    MWPC4U->Tx->clear();
    MWPC4U->Ty->clear();
    MWPC4U->Chi2X->clear();
    //MWPC4U->Chi2Y->clear();

    MC->X->clear();
    MC->Y->clear();
    MC->Z->clear();
    MC->Tx->clear();
    MC->Ty->clear();
    MC->Chi2X->clear();
    MC->MId->clear();
    //MWPC1_MC->Chi2Y->clear();

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


    
    dX43.clear();
    dY43.clear();
        

   

  } 
  cout<<counter<<"  "<<EvWithtrack<<" "<<(double)EvWithtrack/(double)counter<<"  "<<EvWithtrackX<<" "<<(double)EvWithtrackX/(double)counter<<"  "<<EvWithtrackY<<" "<<(double)EvWithtrackY/(double)counter<<endl; 
  treefile->cd();
	//tree1->SetDirectory(treefile);
	tree1->Write();
	treefile->Close();
	
}

void GetFitParam (vector <long double> x, vector <long double> z, vector <long double> sigma, long double &a, long double &b, long double &Chi2) {

  Chi2=0;
  int nPoints=x.size();
  if (nPoints!=z.size())
  cout<<"Warning check X and Z vectors sizes"<<endl;
  
  vector  <long double> y_pr, x1, x2;

 

  long double sum_x1_2, sum_x2_2, sum_x1_x2, sum_x1_y, sum_x2_y=0;
  long double alpha, d1, d2;
  long double k_pr, b_pr;
  long double sign_sum_x1_2, sign_sum_x2_2, sign_sum_x1_x2, sign_sum_x1_y, sign_sum_x2_y=1;

   for (int i=0; i<nPoints; i++) {
    //z.at(i)=z.at(i)+400;
    y_pr.push_back(x.at(i)/sigma.at(i));
    x1.push_back(1./sigma.at(i));


    x2.push_back(z.at(i)/sigma.at(i));
    sum_x1_2+=x1.at(i)*x1.at(i);
    sum_x2_2+=x2.at(i)*x2.at(i);
    sum_x1_x2+=x1.at(i)*x2.at(i);
    sum_x1_y+=x1.at(i)*y_pr.at(i);
    sum_x2_y+=x2.at(i)*y_pr.at(i);
    //cout<<setprecision(15)<<i<<"  "<<x.at(i)<<"  "<<sigma.at(i)<<"  "<<z.at(i)<<endl;
    //cout<<setprecision(15)<<i<<"  "<<sum_x1_2<<"  "<<sum_x2_2<<"  "<<sum_x1_x2<<"  "<<sum_x1_y<<"  "<<sum_x2_y<<endl;
  }

  // if (sum_x1_2<0) {
  //   sign_sum_x1_2=-1;
  //   sum_x1_2=-sum_x1_2;
  // }

  // if (sum_x2_2<0) {
  //   sign_sum_x2_2=-1;
  //   sum_x2_2=-sum_x2_2;
  // }

  // if (sum_x1_x2<0) {
  //   sign_sum_x1_x2=-1;
  //   sum_x1_x2=-sum_x1_x2;
  // }

  // if (sum_x1_y<0) {
  //   sign_sum_x1_y=-1;
  //   sum_x1_y=-sum_x1_y;
  // }

  // if (sum_x2_y<0) {
  //   sign_sum_x2_y=-1;
  //   sum_x2_y=-sum_x2_y;
  // }

  //cout<<setprecision(15)<<nPoints<<"  "<<sum_x1_2<<"  "<<sum_x2_2<<"  "<<sum_x1_x2<<"  "<<sum_x1_y<<"  "<<sum_x2_y<<endl;
  //cout<<nPoints<<"  "<<sum_x1_2<<"  "<<sum_x2_2<<"  "<<sum_x1_x2<<endl;
  // alpha=sum_x1_2*sum_x2_2-sum_x1_x2*sum_x1_x2;
  // d1=sum_x2_2*sum_x1_y-sum_x1_x2*sum_x2_y;
  // d2=sum_x1_2*sum_x2_y-sum_x1_x2*sum_x1_y;

  // k_pr=d2/alpha;
  // b_pr=d1/alpha;

  // k_pr=(sum_x1_2*sum_x2_y-sum_x1_x2*sum_x1_y)/(sum_x1_2*sum_x2_2-sum_x1_x2*sum_x1_x2);
  // b_pr=(sum_x2_2*sum_x1_y-sum_x1_x2*sum_x2_y)/(sum_x1_2*sum_x2_2-sum_x1_x2*sum_x1_x2);

  //cout<<setprecision(15)<<nPoints<<"  "<<k_pr<<"  "<<b_pr<<endl;

  // a=1/k_pr;
  // b=-b_pr/k_pr;

  // a=(sum_x1_2*sum_x2_2-sum_x1_x2*sum_x1_x2)/(sum_x1_2*sum_x2_y-sum_x1_x2*sum_x1_y);
  // b=-(sum_x2_2*sum_x1_y-sum_x1_x2*sum_x2_y)/(sum_x1_2*sum_x2_y-sum_x1_x2*sum_x1_y);

  // a=(sum_x1_2*sum_x2_y-sum_x1_x2*sum_x1_y)/(sum_x1_2*sum_x2_2-sum_x1_x2*sum_x1_x2);
  // b=(sum_x2_2*sum_x1_y-sum_x1_x2*sum_x2_y)/(sum_x1_2*sum_x2_2-sum_x1_x2*sum_x1_x2);

  a=(long double)1./(sum_x2_2/sum_x2_y-sum_x1_x2/sum_x1_2*sum_x1_x2/sum_x2_y)-(long double)1./(sum_x1_2/sum_x1_x2*sum_x2_2/sum_x1_y-sum_x1_x2/sum_x1_y);
  b=(long double)1./(sum_x1_2/sum_x1_y-sum_x1_x2/sum_x2_2*sum_x1_x2/sum_x1_y)-(long double)1./(sum_x1_2/sum_x1_x2*sum_x2_2/sum_x2_y-sum_x1_x2/sum_x2_y);


  // a=1./(sign_sum_x2_y*sign_sum_x2_2*exp(log(sum_x2_y)-log(sum_x2_2))-sign_sum_x1_x2*sign_sum_x1_y*sign_sum_x1_2*sign_sum_x2_2*exp(log(sum_x1_x2)+log(sum_x1_y)-log(sum_x1_2)-log(sum_x2_2)))
  //  -1./(sign_sum_x1_2*sign_sum_x2_y*sign_sum_x1_x2*sign_sum_x1_x2*exp(log(sum_x1_2)+log(sum_x2_y)-log(sum_x1_x2)-log(sum_x1_x2))-sign_sum_x1_y*sign_sum_x1_x2*exp(log(sum_x1_y)-log(sum_x1_x2)));
  // b=-1./(sign_sum_x1_2*sign_sum_x2_y*sign_sum_x2_2*sign_sum_x1_y*exp(log(sum_x1_2)+log(sum_x2_y)-log(sum_x2_2)-log(sum_x1_y))-sign_sum_x1_x2*sign_sum_x2_2*exp(log(sum_x1_x2)-log(sum_x2_2)))
  //  +1./(sign_sum_x1_2*sign_sum_x1_x2*exp(log(sum_x1_2)-log(sum_x1_x2))-sign_sum_x1_y*sign_sum_x2_y*exp(log(sum_x1_y)-log(sum_x2_y)));

  // a=1./(sum_x2_y/sum_x2_2-sum_x1_x2/sum_x1_2*sum_x1_y/sum_x2_2)-1./(sum_x1_2/sum_x1_x2*sum_x2_y/sum_x1_x2-sum_x1_y/sum_x1_x2);
  // b=-1./(sum_x1_2/sum_x2_2*sum_x2_y/sum_x1_y-sum_x1_x2/sum_x2_2)+1./(sum_x1_2/sum_x1_x2-sum_x1_y/sum_x2_y);
  

  //cout<<setprecision(15)<<nPoints<<"  "<<a<<"  "<<b<<endl;

  for (int i=0; i<nPoints; i++) {
    if (nPoints<3) continue;
    Chi2+=(long double)1./((long double)(nPoints)-(long double)2.)*(x.at(i)-(a*z.at(i)+b))*(x.at(i)-(a*z.at(i)+b))/sigma.at(i)/sigma.at(i);
    //cout<<nPoints<<"  "<<x.at(i)<<"  "<<z.at(i)<<"  "<<sigma.at(i)<<"  "<<a<<" "<<b<<"  "<<Chi2<<"  "<<a<<" "<<b<<endl;
  }

  //cout<<setprecision(15)<<nPoints<<"  "<<Chi2<<endl;
  y_pr.clear();
  x1.clear();
  x2.clear();

    
   
}

// void GetFitParam (vector <double> x, vector <double> z, vector <int> clsize, double &a, double &b, double &Chi2, int caseN) {

//   double xsum=0;
//   double zsum=0;
//   double z2sum=0;
//   double xzsum=0;
//   Chi2=0;
//   int nPoints=x.size();
//   if (nPoints!=z.size())
//   cout<<"Warning check X and Z vectors sizes"<<endl;
  
//   for (int i=0; i<nPoints; i++) {
//     xsum+=x.at(i);
//     zsum+=z.at(i);
//     z2sum+=z.at(i)*z.at(i);
//     xzsum+=x.at(i)*z.at(i);
//   }
  
  
//   double delta=z2sum*nPoints-zsum*zsum;
//   double deltaa=xzsum*nPoints-zsum*xsum;
//   double deltab=z2sum*xsum-xzsum*zsum;
//   a=deltaa/delta;
//   b=deltab/delta;
  
//   //if (delta==0) cout<<zsum<<endl;
//   if (caseN==101) {
//     Chi2=0.5*((x.at(0)-(a*z.at(0)+b))*(x.at(0)-(a*z.at(0)+b))/0.0029/0.0029/clsize.at(0)/clsize.at(0)+(x.at(1)-(a*z.at(1)+b))*(x.at(1)-(a*z.at(1)+b))/0.0029/0.0029/clsize.at(1)/clsize.at(1)+(x.at(2)-(a*z.at(2)+b))*(x.at(2)-(a*z.at(2)+b))/0.0417/0.0417+(x.at(3)-(a*z.at(3)+b))*(x.at(3)-(a*z.at(3)+b))/0.0417/0.0417);
//   //cout<<x.at(0)<<"  "<<z.at(0)<<"  "<<x.at(1)<<"  "<<z.at(1)<<"  "<<x.at(2)<<"  "<<z.at(2)<<"  "<<x.at(3)<<"  "<<z.at(3)<<"  "<<a<<" "<<b<<" "<<"  "<<clsize.at(0)<<" "<<clsize.at(1)<<caseN<<" "<<Chi2<<endl;
//   }
//   if (caseN==102) {
//     Chi2=((x.at(0)-(a*z.at(0)+b))*(x.at(0)-(a*z.at(0)+b))/0.0029/0.0029/clsize.at(0)/clsize.at(0)+(x.at(1)-(a*z.at(1)+b))*(x.at(1)-(a*z.at(1)+b))/0.0029/0.0029/clsize.at(1)/clsize.at(1)+(x.at(2)-(a*z.at(2)+b))*(x.at(2)-(a*z.at(2)+b))/0.0417/0.0417);
//     //cout<<Chi2<<" case  "<<caseN<<endl;
//      //cout<<x.at(0)<<"  "<<z.at(0)<<"  "<<x.at(1)<<"  "<<z.at(1)<<"  "<<x.at(2)<<"  "<<z.at(2)<<"  "<<a<<" "<<b<<" "<<caseN<<"  "<<Chi2<<endl;
//   }
//   if (caseN==103) {
//     Chi2=((x.at(0)-(a*z.at(0)+b))*(x.at(0)-(a*z.at(0)+b))/0.0029/0.0029/clsize.at(0)/clsize.at(0)+(x.at(1)-(a*z.at(1)+b))*(x.at(1)-(a*z.at(1)+b))/0.0029/0.0029/clsize.at(1)/clsize.at(1)+(x.at(2)-(a*z.at(2)+b))*(x.at(2)-(a*z.at(2)+b))/0.0417/0.0417);
//     //cout<<Chi2<<" case  "<<caseN<<endl;
//      //cout<<x.at(0)<<"  "<<z.at(0)<<"  "<<x.at(1)<<"  "<<z.at(1)<<"  "<<x.at(2)<<"  "<<z.at(2)<<"  "<<a<<" "<<b<<" "<<caseN<<"  "<<Chi2<<endl;
//   }
//   if (caseN==104 || caseN==105) {
//     Chi2=((x.at(0)-(a*z.at(0)+b))*(x.at(0)-(a*z.at(0)+b))/0.0029/0.0029/clsize.at(0)/clsize.at(0)+(x.at(1)-(a*z.at(1)+b))*(x.at(1)-(a*z.at(1)+b))/0.0417/0.0417+(x.at(2)-(a*z.at(2)+b))*(x.at(2)-(a*z.at(2)+b))/0.0417/0.0417);
//     //cout<<x.at(0)<<"  "<<z.at(0)<<"  "<<x.at(1)<<"  "<<z.at(1)<<"  "<<x.at(2)<<"  "<<z.at(2)<<"  "<<a<<" "<<b<<" "<<caseN<<"  "<<Chi2<<endl;
//   }

//   if (caseN==201) {
//     Chi2=0.5*((x.at(0)-(a*z.at(0)+b))*(x.at(0)-(a*z.at(0)+b))/0.0029/0.0029/clsize.at(0)/clsize.at(0)+(x.at(1)-(a*z.at(1)+b))*(x.at(1)-(a*z.at(1)+b))/0.0029/0.0029/clsize.at(1)/clsize.at(1)+(x.at(2)-(a*z.at(2)+b))*(x.at(2)-(a*z.at(2)+b))/0.0417/0.0417+(x.at(3)-(a*z.at(3)+b))*(x.at(3)-(a*z.at(3)+b))/0.0417/0.0417);
//   }
//   if (caseN==202) {
//     Chi2=((x.at(0)-(a*z.at(0)+b))*(x.at(0)-(a*z.at(0)+b))/0.0029/0.0029/clsize.at(0)/clsize.at(0)+(x.at(1)-(a*z.at(1)+b))*(x.at(1)-(a*z.at(1)+b))/0.0029/0.0029/clsize.at(1)/clsize.at(1)+(x.at(2)-(a*z.at(2)+b))*(x.at(2)-(a*z.at(2)+b))/0.0417/0.0417);
//   }
//   if (caseN==203) {
//     Chi2=((x.at(0)-(a*z.at(0)+b))*(x.at(0)-(a*z.at(0)+b))/0.0029/0.0029/clsize.at(0)/clsize.at(0)+(x.at(1)-(a*z.at(1)+b))*(x.at(1)-(a*z.at(1)+b))/0.0029/0.0029/clsize.at(1)/clsize.at(1)+(x.at(2)-(a*z.at(2)+b))*(x.at(2)-(a*z.at(2)+b))/0.0417/0.0417);
//   }
//   if (caseN==204 || caseN==205) {
//     Chi2=((x.at(0)-(a*z.at(0)+b))*(x.at(0)-(a*z.at(0)+b))/0.0029/0.0029/clsize.at(0)/clsize.at(0)+(x.at(1)-(a*z.at(1)+b))*(x.at(1)-(a*z.at(1)+b))/0.0417/0.0417+(x.at(2)-(a*z.at(2)+b))*(x.at(2)-(a*z.at(2)+b))/0.0417/0.0417);
//   }
  
//   // for (int i=0; i<nPoints; i++) {
//   //   Chi2+=(x.at(i)-(a*z.at(i)+b))*(x.at(i)-(a*z.at(i)+b))/0.073;
//   // }


  
// }