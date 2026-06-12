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


//void Si_MWPC_Tracks_Final_Mass_Production(TString infilename = "/zfs/scratch/lhep-src/Reconstruction/Combined_Files/may26/Si_MWPC_DCH_dCSC_gemCSC_combined_data_5100.root", TString outfilename = "/zfs/scratch/lhep-src/Reconstruction/UpStream_Tracks/may26/Run_5100_Upstr.root") {
void Si_MWPC_Tracks_Final_Mass_Production(TString infilename, TString outfilename) {

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


  //TString filename ="/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiAligMWPCTracks_5132_5162_Chi2_10_Weight_Fit_0_tracks.root";
  //TString filename =outfiledir + "/Run_" + to_string(filenum) + "_Upstr.root"; // output
  //TString filename ="/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiAligMWPCTracks_4780_4799_wo_ampcut_Chi2_10.root";
  

	TFile *treefile = new TFile(outfilename, "recreate");
	//output tree and its branches
	TTree *tree1 = new TTree("tree1","treelibrated tree");
	//tree1->SetDirectory(treefile);
  int event, run;
  //double BC1, BC2, BC3, BC4, BC5;
  double BC1, BC2, BC3, BC4, BC5;
  time_t TS_sec;
  long TS_nsec;
  int chargein, charge34, charge35, charge45, trigger, veto, SW_Mult, SW_NMatchTr, VB_Mult, Tof400_nHits;
  vector <Double_t> *SW_ELoss=nullptr;
  vector <Double_t> *SW_CellId=nullptr;
  vector <Double_t> *SW_Flag=nullptr;
  vector <Double_t> *SW_TrId=nullptr;
  vector <Double_t> *SW_OrderId=nullptr;
  vector <Int_t> *VB_CellId=nullptr;

  vector <Double_t> *Tof400_X=nullptr;
  vector <Double_t> *Tof400_Y=nullptr;
  vector <Double_t> *Tof400_Z=nullptr;
  vector <Double_t> *Tof400_TS=nullptr;
  vector <Int_t> *Tof400_DetId=nullptr;
  vector <Int_t> *Tof400_Module=nullptr;
  vector <Int_t> *Tof400_Station=nullptr;

  Int_t Is_laser, TofCal_nHits; // new
  vector <Double_t> *TofCal_Time0=nullptr, *TofCal_Time1=nullptr, *TofCal_Amp0=nullptr, *TofCal_Amp1=nullptr, *TofCal_X=nullptr, *TofCal_Y=nullptr; // new
  vector <Int_t> *TofCal_Plane=nullptr, *TofCal_Bar=nullptr, *TofCal_Arm=nullptr, *TofCal_GlBar=nullptr; // new
  Double_t BC1_Time0, BC1_Time1, BC2_Time0, BC2_Time1;  // new  

  Short_t rightGemN, rightCscN, leftGemN, leftCscN;
  vector <Float_t> *rightGemX=nullptr, *rightGemY=nullptr, *rightGemZ=nullptr, *leftGemX=nullptr, *leftGemY=nullptr, *leftGemZ=nullptr,
  *rightCscX=nullptr, *rightCscY=nullptr, *rightCscZ=nullptr, *leftCscX=nullptr, *leftCscY=nullptr, *leftCscZ=nullptr, 
  *rightGemXLocal=nullptr, *rightGemYLocal=nullptr, *leftGemXLocal=nullptr, *leftGemYLocal=nullptr, 
  *rightCscXLocal=nullptr, *rightCscYLocal=nullptr, *leftCscXLocal=nullptr, *leftCscYLocal=nullptr;

  Int_t dCSCH_sec, dCSCH_nanosec;
  Short_t dCSCH_cscN;
  vector <Float_t> *dCSCH_cscX=nullptr, *dCSCH_cscY=nullptr, *dCSCH_cscZ=nullptr;

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

  TBranch *br_event =tree1->Branch("event",&event);
  TBranch *br_trigger =tree1->Branch("trigger",&trigger);
  TBranch *br_veto =tree1->Branch("VC",&veto);
  TBranch *br_run =tree1->Branch("run",&run);
  TBranch *br_TS_sec =tree1->Branch("TS_sec",&TS_sec);
  TBranch *br_TS_nsec =tree1->Branch("TS_nsec",&TS_nsec);

  TBranch *br_dX43 =tree1->Branch("dX43",&dX43);
  TBranch *br_dY43 =tree1->Branch("dY43",&dY43);
  
  TBranch *br_BC1 =tree1->Branch("BC1",&BC1);
  TBranch *br_BC2 =tree1->Branch("BC2",&BC2);
  TBranch *br_BC3 =tree1->Branch("BC3",&BC3);
  TBranch *br_BC4 =tree1->Branch("BC4",&BC4);
  TBranch *br_BC5 =tree1->Branch("BC5",&BC5);
  TBranch *br_chargein =tree1->Branch("chargein",&chargein);
  TBranch *br_charge34 =tree1->Branch("charge34",&charge34);
  TBranch *br_charge35 =tree1->Branch("charge35",&charge35);
  TBranch *br_charge45 =tree1->Branch("charge45",&charge45);

  TBranch *br_SW_ELoss =tree1->Branch("SW_ELoss",&SW_ELoss);
  TBranch *br_SW_CellId =tree1->Branch("SW_CellId",&SW_CellId);
  TBranch *br_SW_Flag =tree1->Branch("SW_Flag",&SW_Flag);
  TBranch *br_SW_TrId =tree1->Branch("SW_TrId",&SW_TrId);
  TBranch *br_SW_OrderId =tree1->Branch("SW_OrderId",&SW_OrderId);
  TBranch *br_SW_Mult =tree1->Branch("SW_Mult",&SW_Mult);
  TBranch *br_SW_NMatchTr =tree1->Branch("SW_NMatchTr",&SW_NMatchTr);

  TBranch *br_VB_Mult =tree1->Branch("VB_Mult",&VB_Mult);
  TBranch *br_VB_CellId =tree1->Branch("VB_CellId",&VB_CellId);

  TBranch *br_Tof400_X =tree1->Branch("Tof400_X",&Tof400_X);
  TBranch *br_Tof400_Y =tree1->Branch("Tof400_Y",&Tof400_Y);
  TBranch *br_Tof400_Z =tree1->Branch("Tof400_Z",&Tof400_Z);
  TBranch *br_Tof400_TS =tree1->Branch("Tof400_TS",&Tof400_TS);
  TBranch *br_Tof400_DetId =tree1->Branch("Tof400_DetId",&Tof400_DetId);
  TBranch *br_Tof400_Module =tree1->Branch("Tof400_Module",&Tof400_Module);
  TBranch *br_Tof400_Station =tree1->Branch("Tof400_Station",&Tof400_Station);
  TBranch *br_Tof400_nHits =tree1->Branch("Tof400_nHits",&Tof400_nHits);


  TBranch *br_TofCal_nHits = tree1->Branch("TofCal_nHits", &TofCal_nHits); // new
  TBranch *br_Is_laser = tree1->Branch("Is_laser", &Is_laser); // new
  TBranch *br_BC1_Time0 = tree1->Branch("BC1_Time0", &BC1_Time0); // new
  TBranch *br_BC1_Time1 = tree1->Branch("BC1_Time1", &BC1_Time1); // new
  TBranch *br_BC2_Time0 = tree1->Branch("BC2_Time0", &BC2_Time0); // new
  TBranch *br_BC2_Time1 = tree1->Branch("BC2_Time1", &BC2_Time1); // new

 	TBranch *br_TofCal_Bar = tree1->Branch("TofCal_Bar", &TofCal_Bar); // new
 	TBranch *br_TofCal_Plane = tree1->Branch("TofCal_Plane", &TofCal_Plane); // new
  TBranch *br_TofCal_Arm = tree1->Branch("TofCal_Arm", &TofCal_Arm); // new
  TBranch *br_TofCal_GlBar = tree1->Branch("TofCal_GlBar", &TofCal_GlBar); // new
  TBranch *br_TofCal_Time0 = tree1->Branch("TofCal_Time0", &TofCal_Time0); // new
 	TBranch *br_TofCal_Time1 = tree1->Branch("TofCal_Time1", &TofCal_Time1); // new
  TBranch *br_TofCal_Amp0 = tree1->Branch("TofCal_Amp0", &TofCal_Amp0); // new
 	TBranch *br_TofCal_Amp1 = tree1->Branch("TofCal_Amp1", &TofCal_Amp1); // new
  TBranch *br_TofCal_X = tree1->Branch("TofCal_X", &TofCal_X); // new
 	TBranch *br_TofCal_Y = tree1->Branch("TofCal_Y", &TofCal_Y); // new

  TBranch *br_RightGemN =tree1->Branch("R_GemN", &rightGemN); //
  TBranch *br_RightGemX =tree1->Branch("R_GemX", &rightGemX);
  TBranch *br_RightGemY =tree1->Branch("R_GemY", &rightGemY);
  TBranch *br_RightGemZ =tree1->Branch("R_GemZ", &rightGemZ);
  TBranch *br_RightCscN =tree1->Branch("R_CscN", &rightCscN);
  TBranch *br_RightCscX =tree1->Branch("R_CscX", &rightCscX);
  TBranch *br_RightCscY =tree1->Branch("R_CscY", &rightCscY);
  TBranch *br_RightCscZ =tree1->Branch("R_CscZ", &rightCscZ);
  TBranch *br_LeftGemN =tree1->Branch("L_GemN", &leftGemN);
  TBranch *br_LeftGemX =tree1->Branch("L_GemX", &leftGemX);
  TBranch *br_LeftGemY =tree1->Branch("L_GemY", &leftGemY);
  TBranch *br_LeftGemZ =tree1->Branch("L_GemZ", &leftGemZ);
  TBranch *br_LeftCscN =tree1->Branch("L_CscN", &leftCscN);
  TBranch *br_LeftCscX =tree1->Branch("L_CscX", &leftCscX);
  TBranch *br_LeftCscY =tree1->Branch("L_CscY", &leftCscY);
  TBranch *br_LeftCscZ =tree1->Branch("L_CscZ", &leftCscZ);

  TBranch *br_rightGemXLocal =tree1->Branch("R_GemXLocal", &rightGemXLocal); //
  TBranch *br_rightGemYLocal =tree1->Branch("R_GemYLocal", &rightGemYLocal);
  TBranch *br_leftGemXLocal =tree1->Branch("L_GemXLocal", &leftGemXLocal);
  TBranch *br_leftGemYLocal =tree1->Branch("L_GemYLocal", &leftGemYLocal);
  TBranch *br_rightCscXLocal =tree1->Branch("R_CscXLocal", &rightCscXLocal);
  TBranch *br_rightCscYLocal =tree1->Branch("R_CscYLocal", &rightCscYLocal);
  TBranch *br_leftCscXLocal =tree1->Branch("L_CscXLocal", &leftCscXLocal);
  TBranch *br_leftCscYLocal =tree1->Branch("L_CscYLocal", &leftCscYLocal); 

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

  

  

  // Track *MWPC1 = new Track;
  // Track *MWPC2 = new Track;
  // Track *MWPC3 = new Track;
  // Track *MWPC4 = new Track;



  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiMwpcAligDchCoord5132_5162_BMNS_wo_gap_wo_rot_WithDCH_MWPC_Zinv_wo_ampcut.root";
  //filename = infiledir + "/Si_MWPC_DCH_dCSC_gemCSC_combined_data_" + to_string(filenum) + ".root"; // input
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiMwpcAligDchCoord4780_4799_BMNS_wo_gap_wo_rot_WithDCH_MWPC_Zinv_ampcut_1700.root";
  
      
  TFile *hfile = new TFile(infilename);
  //		if (!hfile->IsOpen())  continue;
  //		if (!hfile->Get("bmndata")) 		continue;
        
  TTree *tree = (TTree*)hfile->Get("tree1");

  tree->SetBranchAddress("eventBmn",&event);
  tree->SetBranchAddress("run",&run);
  tree->SetBranchAddress("trigger",&trigger);
  tree->SetBranchAddress("VC",&veto);
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

  tree->SetBranchAddress("SW_ELoss",&SW_ELoss);
  tree->SetBranchAddress("SW_CellId",&SW_CellId);
  tree->SetBranchAddress("SW_Flag",&SW_Flag);
  tree->SetBranchAddress("SW_TrId",&SW_TrId);
  tree->SetBranchAddress("SW_OrderId",&SW_OrderId);
  tree->SetBranchAddress("SW_Mult",&SW_Mult);
  tree->SetBranchAddress("SW_NMatchTr",&SW_NMatchTr);

  tree->SetBranchAddress("VB_CellId",&VB_CellId);
  tree->SetBranchAddress("VB_Mult",&VB_Mult);

  tree->SetBranchAddress("Tof400_X",&Tof400_X);
  tree->SetBranchAddress("Tof400_Y",&Tof400_Y);
  tree->SetBranchAddress("Tof400_Z",&Tof400_Z);
  tree->SetBranchAddress("Tof400_TS",&Tof400_TS);
  tree->SetBranchAddress("Tof400_DetId",&Tof400_DetId);
  tree->SetBranchAddress("Tof400_Module",&Tof400_Module);
  tree->SetBranchAddress("Tof400_Station",&Tof400_Station);
  tree->SetBranchAddress("Tof400_nHits",&Tof400_nHits);


  tree->SetBranchAddress("BC1_Time0", &BC1_Time0);//new
  tree->SetBranchAddress("BC1_Time1", &BC1_Time1);
  tree->SetBranchAddress("BC2_Time0", &BC2_Time0);
  tree->SetBranchAddress("BC2_Time1", &BC2_Time1);
  tree->SetBranchAddress("TofCal_nHits", &TofCal_nHits);
  tree->SetBranchAddress("Is_laser", &Is_laser);
  tree->SetBranchAddress("TofCal_Bar", &TofCal_Bar);
  tree->SetBranchAddress("TofCal_Plane", &TofCal_Plane);
  tree->SetBranchAddress("TofCal_Arm", &TofCal_Arm);
  tree->SetBranchAddress("TofCal_GlBar", &TofCal_GlBar);
  tree->SetBranchAddress("TofCal_Time0", &TofCal_Time0);
  tree->SetBranchAddress("TofCal_Time1", &TofCal_Time1);
  tree->SetBranchAddress("TofCal_Amp0", &TofCal_Amp0);
  tree->SetBranchAddress("TofCal_Amp1", &TofCal_Amp1);
  tree->SetBranchAddress("TofCal_X", &TofCal_X);
  tree->SetBranchAddress("TofCal_Y", &TofCal_Y); // new

  tree->SetBranchAddress("R_GemN", &rightGemN);
  tree->SetBranchAddress("R_GemX", &rightGemX);
  tree->SetBranchAddress("R_GemY", &rightGemY);
  tree->SetBranchAddress("R_GemZ", &rightGemZ);
  tree->SetBranchAddress("R_CscN", &rightCscN);
  tree->SetBranchAddress("R_CscX", &rightCscX);
  tree->SetBranchAddress("R_CscY", &rightCscY);
  tree->SetBranchAddress("R_CscZ", &rightCscZ);
   
  tree->SetBranchAddress("L_GemN", &leftGemN);
  tree->SetBranchAddress("L_GemX", &leftGemX);
  tree->SetBranchAddress("L_GemY", &leftGemY);
  tree->SetBranchAddress("L_GemZ", &leftGemZ);
  tree->SetBranchAddress("L_CscN", &leftCscN);
  tree->SetBranchAddress("L_CscX", &leftCscX);
  tree->SetBranchAddress("L_CscY", &leftCscY);
  tree->SetBranchAddress("L_CscZ", &leftCscZ);
  
  tree->SetBranchAddress("R_GemXLocal", &rightGemXLocal); //
  tree->SetBranchAddress("R_GemYLocal", &rightGemYLocal);
  tree->SetBranchAddress("L_GemXLocal", &leftGemXLocal);
  tree->SetBranchAddress("L_GemYLocal", &leftGemYLocal);

  tree->SetBranchAddress("R_CscXLocal", &rightCscXLocal);
  tree->SetBranchAddress("R_CscYLocal", &rightCscYLocal);
  tree->SetBranchAddress("L_CscXLocal", &leftCscXLocal);
  tree->SetBranchAddress("L_CscYLocal", &leftCscYLocal);


  tree->SetBranchAddress("dCSCH_sec",&dCSCH_sec);
  tree->SetBranchAddress("dCSCH_nanosec",&dCSCH_nanosec);
  tree->SetBranchAddress("dCSCH_N",&dCSCH_cscN);
  tree->SetBranchAddress("dCSCH_X",&dCSCH_cscX);
  tree->SetBranchAddress("dCSCH_Y",&dCSCH_cscY);
  tree->SetBranchAddress("dCSCH_Z",&dCSCH_cscZ);
  
    
  // tree->SetBranchAddress("Si1H_X",&Sil1H->X);
  // tree->SetBranchAddress("Si1H_Y",&Sil1H->Y);
  // tree->SetBranchAddress("Si1H_St",&Sil1H->St);
  // tree->SetBranchAddress("Si1H_Mod",&Sil1H->Mod);
  
  // tree->SetBranchAddress("Si1H_nHits",&Sil1H->nHits);


  //Inf *Sil1H = new Inf;
 	tree->SetBranchAddress("Si1H_X",&Sil1H->X);
  tree->SetBranchAddress("Si1H_Y",&Sil1H->Y);
  tree->SetBranchAddress("Si1H_St",&Sil1H->St);
  tree->SetBranchAddress("Si1H_Mod",&Sil1H->Mod);
  tree->SetBranchAddress("Si1H_L0NHits",&Sil1H->L0H);
  tree->SetBranchAddress("Si1H_L1NHits",&Sil1H->L1H);
  tree->SetBranchAddress("Si1H_AmpDir",&Sil1H->AmpDir);
  tree->SetBranchAddress("Si1H_AmpIncl",&Sil1H->AmpIncl);
  tree->SetBranchAddress("Si1H_nHits",&Sil1H->nHits);

  //Inf *Sil2H = new Inf;
 	tree->SetBranchAddress("Si2H_X",&Sil2H->X);
  tree->SetBranchAddress("Si2H_Y",&Sil2H->Y);
  tree->SetBranchAddress("Si2H_St",&Sil2H->St);
  tree->SetBranchAddress("Si2H_Mod",&Sil2H->Mod);
  tree->SetBranchAddress("Si2H_L0NHits",&Sil2H->L0H);
  tree->SetBranchAddress("Si2H_L1NHits",&Sil2H->L1H);
  tree->SetBranchAddress("Si2H_AmpDir",&Sil2H->AmpDir);
  tree->SetBranchAddress("Si2H_AmpIncl",&Sil2H->AmpIncl);
  tree->SetBranchAddress("Si2H_nHits",&Sil2H->nHits);

  //Inf *Sil3H = new Inf;
 	tree->SetBranchAddress("Si3H_X",&Sil3H->X);
  tree->SetBranchAddress("Si3H_Y",&Sil3H->Y);
  tree->SetBranchAddress("Si3H_St",&Sil3H->St);
  tree->SetBranchAddress("Si3H_Mod",&Sil3H->Mod);
  tree->SetBranchAddress("Si3H_L0NHits",&Sil3H->L0H);
  tree->SetBranchAddress("Si3H_L1NHits",&Sil3H->L1H);
  tree->SetBranchAddress("Si3H_AmpDir",&Sil3H->AmpDir);
  tree->SetBranchAddress("Si3H_AmpIncl",&Sil3H->AmpIncl);
  tree->SetBranchAddress("Si3H_nHits",&Sil3H->nHits);

  //Inf *Sil4H = new Inf;
 	tree->SetBranchAddress("Si4H_X",&Sil4H->X);
  tree->SetBranchAddress("Si4H_Y",&Sil4H->Y);
  tree->SetBranchAddress("Si4H_St",&Sil4H->St);
  tree->SetBranchAddress("Si4H_Mod",&Sil4H->Mod);
  tree->SetBranchAddress("Si4H_L0NHits",&Sil4H->L0H);
  tree->SetBranchAddress("Si4H_L1NHits",&Sil4H->L1H);
  tree->SetBranchAddress("Si4H_AmpDir",&Sil4H->AmpDir);
  tree->SetBranchAddress("Si4H_AmpIncl",&Sil4H->AmpIncl);
  tree->SetBranchAddress("Si4H_nHits",&Sil4H->nHits);

  //Track *MWPC1 = new Track;
  tree->SetBranchAddress("MWPC_1_X",&MWPC1->X);
  tree->SetBranchAddress("MWPC_1_Y",&MWPC1->Y);
  tree->SetBranchAddress("MWPC_1_Z",&MWPC1->Z);
  tree->SetBranchAddress("MWPC_1_Tx",&MWPC1->Tx);
  tree->SetBranchAddress("MWPC_1_Ty",&MWPC1->Ty);
  tree->SetBranchAddress("MWPC_1_Chi2",&MWPC1->Chi2X);
  tree->SetBranchAddress("MWPC_1_Ntr",&MWPC1->nTracks);

  //Track *MWPC2 = new Track;
  tree->SetBranchAddress("MWPC_2_X",&MWPC2->X);
  tree->SetBranchAddress("MWPC_2_Y",&MWPC2->Y);
  tree->SetBranchAddress("MWPC_2_Z",&MWPC2->Z);
  tree->SetBranchAddress("MWPC_2_Tx",&MWPC2->Tx);
  tree->SetBranchAddress("MWPC_2_Ty",&MWPC2->Ty);
  tree->SetBranchAddress("MWPC_2_Chi2",&MWPC2->Chi2X);
  tree->SetBranchAddress("MWPC_2_Ntr",&MWPC2->nTracks);

  //Track *MWPC3 = new Track;
  tree->SetBranchAddress("MWPC_3_X",&MWPC3->X);
  tree->SetBranchAddress("MWPC_3_Y",&MWPC3->Y);
  tree->SetBranchAddress("MWPC_3_Z",&MWPC3->Z);
  tree->SetBranchAddress("MWPC_3_Tx",&MWPC3->Tx);
  tree->SetBranchAddress("MWPC_3_Ty",&MWPC3->Ty);
  tree->SetBranchAddress("MWPC_3_Chi2",&MWPC3->Chi2X);
  tree->SetBranchAddress("MWPC_3_Ntr",&MWPC3->nTracks);

  //Track *MWPC4 = new Track;
  tree->SetBranchAddress("MWPC_4_X",&MWPC4->X);
  tree->SetBranchAddress("MWPC_4_Y",&MWPC4->Y);
  tree->SetBranchAddress("MWPC_4_Z",&MWPC4->Z);
  tree->SetBranchAddress("MWPC_4_Tx",&MWPC4->Tx);
  tree->SetBranchAddress("MWPC_4_Ty",&MWPC4->Ty);
  tree->SetBranchAddress("MWPC_4_Chi2",&MWPC4->Chi2X);
  tree->SetBranchAddress("MWPC_4_Ntr",&MWPC4->nTracks);

  //Track *MWPC_p1 = new Track;
  tree->SetBranchAddress("MWPC_p1_X",&MWPC_p1->X);
  tree->SetBranchAddress("MWPC_p1_Y",&MWPC_p1->Y);
  tree->SetBranchAddress("MWPC_p1_Z",&MWPC_p1->Z);
  tree->SetBranchAddress("MWPC_p1_Tx",&MWPC_p1->Tx);
  tree->SetBranchAddress("MWPC_p1_Ty",&MWPC_p1->Ty);
  tree->SetBranchAddress("MWPC_p1_Chi2",&MWPC_p1->Chi2X);
  tree->SetBranchAddress("MWPC_p1_Ntr",&MWPC_p1->nTracks);

  //Track *MWPC_p0 = new Track;
  tree->SetBranchAddress("MWPC_p0_X",&MWPC_p0->X);
  tree->SetBranchAddress("MWPC_p0_Y",&MWPC_p0->Y);
  tree->SetBranchAddress("MWPC_p0_Z",&MWPC_p0->Z);
  tree->SetBranchAddress("MWPC_p0_Tx",&MWPC_p0->Tx);
  tree->SetBranchAddress("MWPC_p0_Ty",&MWPC_p0->Ty);
  tree->SetBranchAddress("MWPC_p0_Chi2",&MWPC_p0->Chi2X);
  tree->SetBranchAddress("MWPC_p0_Ntr",&MWPC_p0->nTracks);

  tree->SetBranchAddress("DCH_X",&DCH->X);
  tree->SetBranchAddress("DCH_Y",&DCH->Y);
  tree->SetBranchAddress("DCH_Z",&DCH->Z);
  tree->SetBranchAddress("DCH_Tx",&DCH->Tx);
  tree->SetBranchAddress("DCH_Ty",&DCH->Ty);
  tree->SetBranchAddress("DCH_Chi2",&DCH->Chi2X);
  tree->SetBranchAddress("DCH_Ntr",&DCH->nTracks);

  tree->SetBranchAddress("DCH1_X",&DCH1->X);
  tree->SetBranchAddress("DCH1_Y",&DCH1->Y);
  tree->SetBranchAddress("DCH1_Z",&DCH1->Z);
  tree->SetBranchAddress("DCH1_Tx",&DCH1->Tx);
  tree->SetBranchAddress("DCH1_Ty",&DCH1->Ty);
  tree->SetBranchAddress("DCH1_Chi2",&DCH1->Chi2X);
  tree->SetBranchAddress("DCH1_Ntr",&DCH1->nTracks);

  tree->SetBranchAddress("DCH2_X",&DCH2->X);
  tree->SetBranchAddress("DCH2_Y",&DCH2->Y);
  tree->SetBranchAddress("DCH2_Z",&DCH2->Z);
  tree->SetBranchAddress("DCH2_Tx",&DCH2->Tx);
  tree->SetBranchAddress("DCH2_Ty",&DCH2->Ty);
  tree->SetBranchAddress("DCH2_Chi2",&DCH2->Chi2X);
  tree->SetBranchAddress("DCH2_Ntr",&DCH2->nTracks);



  int NEv = tree->GetEntries();
  int EvWithtrack=0;
  int EvWithtrackX=0;
  int EvWithtrackY=0;
  cout<<NEv<<endl; 
  int counter=0;
  for (int ev=0; ev<NEv; ev++) {
    tree->GetEntry(ev);
    
    

   
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


    
   
    // if (Sil1H->X->size()!=1) continue;
    // if (Sil2H->X->size()!=1) continue;
    // if (Sil3H->X->size()!=1) continue;
    // if (Sil4H->X->size()!=1) continue;
    // if (MWPC1->X->size()!=1) continue;
    // if (MWPC2->X->size()!=1) continue;
    // if (MWPC3->X->size()!=1) continue;
    // if (MWPC4->X->size()!=1) continue;
    // if (MWPC_p1->X->size()!=1) continue;
    
    counter++;


    for (int j=0; j<MWPC_p0->Tx->size(); j++)
    {
      MWPC_p0->Tx->at(j)=-MWPC_p0->Tx->at(j);
    }

    for (int j=0; j<MWPC_p0->X->size(); j++)
    {
      MWPC_p0->X->at(j)=-MWPC_p0->X->at(j);
    }


    for (int j=0; j<MWPC1->X->size(); j++)
    {
      MWPC1->X->at(j)=-MWPC1->X->at(j);
      MWPC1->Tx->at(j)=-MWPC1->Tx->at(j);
    }

    for (int j=0; j<MWPC2->X->size(); j++)
    {
      MWPC2->X->at(j)=-MWPC2->X->at(j);
      MWPC2->Tx->at(j)=-MWPC2->Tx->at(j);
    }

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


    int nH_Si1=Sil1H->X->size();
    int nH_Si2=Sil2H->X->size();
    int nH_Si3=Sil3H->X->size();
    int nH_Si4=Sil4H->X->size();
    int nH_MWPC3=MWPC3->X->size();
    int nH_MWPC4=MWPC4->X->size();

    

   // cout<<ev<<" 5 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;

    // if (nH_Si1>10) cout<<nH_Si1<<"  "<<nH_Si2<<"  "<<nH_Si3<<"  "<<nH_Si4<<"  "<<nH_MWPC3<<"  "<<nH_MWPC4<<endl;

    // if (nH_Si1==nH_Si2==nH_Si3==nH_Si4==nH_MWPC3==nH_MWPC4 && nH_Si1>1) EvWithtrack++;
    // else if (nH_Si1==nH_Si2==nH_Si3==nH_Si4==nH_MWPC3 && nH_Si1>1) EvWithtrack++;
    // else if (nH_Si1==nH_Si2==nH_Si3==nH_Si4==nH_MWPC4 && nH_Si1>1) EvWithtrack++;
    // else if (nH_Si1==nH_Si2==nH_Si3==nH_MWPC3==nH_MWPC4 && nH_Si1>1) EvWithtrack++;
    // else if (nH_Si1==nH_Si2==nH_Si4==nH_MWPC3==nH_MWPC4 && nH_Si1>1) EvWithtrack++;
    // else if (nH_Si1==nH_Si3==nH_Si4==nH_MWPC3==nH_MWPC4 && nH_Si1>1) EvWithtrack++;
    // else if (nH_Si2==nH_Si3==nH_Si4==nH_MWPC3==nH_MWPC4 && nH_Si2>1) EvWithtrack++;

    // if (nH_Si1==nH_Si2==nH_Si3==nH_Si4 && nH_Si1>0) EvWithtrack++;
    // else if (nH_Si1==nH_Si2==nH_Si3 && nH_Si1>0) EvWithtrack++;
    // else if (nH_Si1==nH_Si2==nH_Si4 && nH_Si1>0) EvWithtrack++;
    // else if (nH_Si1==nH_Si3==nH_Si4 && nH_Si1>0) EvWithtrack++;
    // else if (nH_Si2==nH_Si3==nH_Si4 && nH_Si2>0) EvWithtrack++;

    //if (nH_MWPC4>0 && nH_MWPC3>0) EvWithtrack++;
   

    // if (nH_Si1>0 && nH_Si2>0 && nH_Si3>0 && nH_Si4>0 && nH_MWPC3>0 && nH_MWPC4>0) EvWithtrack++;
    // else if (nH_Si1>0 && nH_Si2>0 && nH_Si3>0 && nH_Si4>0 && nH_MWPC3>0) EvWithtrack++;
    // else if (nH_Si1>0 && nH_Si2>0 && nH_Si3>0 && nH_Si4>0 && nH_MWPC4>0) EvWithtrack++;
    // else if (nH_Si1>0 && nH_Si2>0 && nH_Si3>0 && nH_MWPC3>0 && nH_MWPC4>0) EvWithtrack++;
    // else if (nH_Si1>0 && nH_Si2>0 && nH_Si4>0 && nH_MWPC3>0 && nH_MWPC4>0) EvWithtrack++;
    // else if (nH_Si1>0 && nH_Si3>0 && nH_Si4>0 && nH_MWPC3>0 && nH_MWPC4>0) EvWithtrack++;
    // else if (nH_Si2>0 && nH_Si3>0 && nH_Si4>0 && nH_MWPC3>0 && nH_MWPC4>0) EvWithtrack++;

    //if (nH_Si1>0 && nH_Si2>0 && nH_Si3>0 && nH_Si4>0) EvWithtrack++;
    // else if (nH_Si1>0 && nH_Si2>0 && nH_Si3>0) EvWithtrack++;
    // else if (nH_Si1>0 && nH_Si2>0 && nH_Si4>0) EvWithtrack++;
    // else if (nH_Si1>0 && nH_Si3>0 && nH_Si4>0) EvWithtrack++;
    // else if (nH_Si2>0 && nH_Si3>0 && nH_Si4>0) EvWithtrack++;

    //if (nH_Si1>10 || nH_Si2>10 || nH_Si3>10 || nH_Si4>10 || nH_MWPC3>10 || nH_MWPC4>10) EvWithtrack++;

    // int isX=0;
    // int isY=0;
    // if (nH_Si1>0 && nH_Si3>0) {EvWithtrackX++; isX=1;}
    // else if (nH_Si1>0 && nH_MWPC3>0) {EvWithtrackX++; isX=1;}
    // else if (nH_Si1>0 && nH_MWPC4>0) {EvWithtrackX++; isX=1;}
    // else if (nH_Si3>0 && nH_MWPC3>0) {EvWithtrackX++; isX=1;}
    // else if (nH_Si3>0 && nH_MWPC4>0) {EvWithtrackX++; isX=1;}
    // else if (nH_MWPC4>0 && nH_MWPC3>0) {EvWithtrackX++; isX=1;}

    // if (nH_Si2>0 && nH_Si4>0) {EvWithtrackY++; isY=1;}
    // else if (nH_Si2>0 && nH_MWPC3>0) {EvWithtrackY++; isY=1;}
    // else if (nH_Si2>0 && nH_MWPC4>0) {EvWithtrackY++; isY=1;}
    // else if (nH_Si4>0 && nH_MWPC3>0) {EvWithtrackY++; isY=1;}
    // else if (nH_Si4>0 && nH_MWPC4>0) {EvWithtrackY++; isY=1;}
    // else if (nH_MWPC4>0 && nH_MWPC3>0) {EvWithtrackY++; isY=1;}


    // if (isX!=isY) EvWithtrack++;

    


    double YglobalShift=0.9611+0.1975+0.0311-0.0014;//1.22;
    double XglobalShift=0.019;//0.07;//0.03;

    //cout<<ev<<" "<<Sil1H->nHits<<" "<<nH_Si1<<" "<<Sil1H->X->size()<<" "<<Sil1H->Y->size()<<"  "<<Si1X.size()<<endl;
    for (int i=0; i<Sil1H->X->size(); i++) {
      if (Sil1H->X->at(i)>6.5 || Sil1H->X->at(i)<-6 || Sil1H->L0H->at(i)>10 || Sil1H->AmpIncl->at(i)==0 || Sil1H->AmpDir->at(i)==0){
        Sil1H->X->erase(Sil1H->X->begin()+i);
        Sil1H->Y->erase(Sil1H->Y->begin()+i);
        Sil1H->St->erase(Sil1H->St->begin()+i);
        Sil1H->Mod->erase(Sil1H->Mod->begin()+i);
        Sil1H->L0H->erase(Sil1H->L0H->begin()+i);
        Sil1H->L1H->erase(Sil1H->L1H->begin()+i);
        Sil1H->AmpDir->erase(Sil1H->AmpDir->begin()+i);
        Sil1H->AmpIncl->erase(Sil1H->AmpIncl->begin()+i);
        Sil1H->nHits--;
        nH_Si1--;
        i=i-1;
      }
      
    }

    for (int i=0; i<Sil2H->Y->size(); i++) {
      if (Sil2H->Y->at(i)>6 || Sil2H->Y->at(i)<-6.5 || Sil2H->L0H->at(i)>10 || Sil2H->AmpIncl->at(i)==0 || Sil2H->AmpDir->at(i)==0){
        Sil2H->X->erase(Sil2H->X->begin()+i);
        Sil2H->Y->erase(Sil2H->Y->begin()+i);
        Sil2H->St->erase(Sil2H->St->begin()+i);
        Sil2H->Mod->erase(Sil2H->Mod->begin()+i);
        Sil2H->L0H->erase(Sil2H->L0H->begin()+i);
        Sil2H->L1H->erase(Sil2H->L1H->begin()+i);
        Sil2H->AmpDir->erase(Sil2H->AmpDir->begin()+i);
        Sil2H->AmpIncl->erase(Sil2H->AmpIncl->begin()+i);
        Sil2H->nHits--;
        nH_Si2--;
        i=i-1;
      }
      
    }

    for (int i=0; i<Sil3H->X->size(); i++) {
      if (Sil3H->X->at(i)>6.5 || Sil3H->X->at(i)<-6 || Sil3H->L0H->at(i)>10 || Sil3H->AmpIncl->at(i)==0 || Sil3H->AmpDir->at(i)==0){
        Sil3H->X->erase(Sil3H->X->begin()+i);
        Sil3H->Y->erase(Sil3H->Y->begin()+i);
        Sil3H->St->erase(Sil3H->St->begin()+i);
        Sil3H->Mod->erase(Sil3H->Mod->begin()+i);
        Sil3H->L0H->erase(Sil3H->L0H->begin()+i);
        Sil3H->L1H->erase(Sil3H->L1H->begin()+i);
        Sil3H->AmpDir->erase(Sil3H->AmpDir->begin()+i);
        Sil3H->AmpIncl->erase(Sil3H->AmpIncl->begin()+i);
        Sil3H->nHits--;
        nH_Si3--;
        i=i-1;
      }
      
    }

    for (int i=0; i<Sil4H->Y->size(); i++) {
      if (Sil4H->Y->at(i)>6 || Sil4H->Y->at(i)<-6.5 || Sil4H->L0H->at(i)>10 || Sil4H->AmpIncl->at(i)==0 || Sil4H->AmpDir->at(i)==0){
        Sil4H->X->erase(Sil4H->X->begin()+i);
        Sil4H->Y->erase(Sil4H->Y->begin()+i);
        Sil4H->St->erase(Sil4H->St->begin()+i);
        Sil4H->Mod->erase(Sil4H->Mod->begin()+i);
        Sil4H->L0H->erase(Sil4H->L0H->begin()+i);
        Sil4H->L1H->erase(Sil4H->L1H->begin()+i);
        Sil4H->AmpDir->erase(Sil4H->AmpDir->begin()+i);
        Sil4H->AmpIncl->erase(Sil4H->AmpIncl->begin()+i);
        Sil4H->nHits--;
        nH_Si4--;
        i=i-1;
      }
      
    }

    
    // cout<<ev<<" 6 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
    // for (int i=0; i<Sil1H->X->size(); i++) {
    //       //cout<<Si1X.at(i)<<" "<<Si1Y.at(i)<<" "<<endl;
    //       cout<<Sil1H->X->at(i)<<" 6  "<<Sil1H->Y->at(i)<<" "<<Sil1H->AmpIncl->at(i)<<" "<<Sil1H->AmpIncl->at(i)<<" "<<endl;
    //   }
    // for (int i=0; i<Sil2H->X->size(); i++) {
    //     //cout<<Si1X.at(i)<<" "<<Si1Y.at(i)<<" "<<endl;
    //     cout<<Sil2H->X->at(i)<<" "<<Sil2H->Y->at(i)<<" "<<endl;
    // }
    for (int i=0; i<Sil1H->X->size(); i++) {
      for (int j=0; j<Sil1H->X->size(); j++) {
        if (i<=j) continue;
        if (Sil1H->Y->at(i)==Sil1H->Y->at(j)){
          Sil1H->X->at(j)=(Sil1H->X->at(i)*Sil1H->AmpIncl->at(i)+Sil1H->X->at(j)*Sil1H->AmpIncl->at(j))/(Sil1H->AmpIncl->at(i)+Sil1H->AmpIncl->at(j));
          Sil1H->L1H->at(j)=(Sil1H->L1H->at(i)+Sil1H->L1H->at(j));
          Sil1H->AmpIncl->at(j)=(Sil1H->AmpIncl->at(i)+Sil1H->AmpIncl->at(j));
          Sil1H->X->erase(Sil1H->X->begin()+i);
          Sil1H->Y->erase(Sil1H->Y->begin()+i);
          Sil1H->St->erase(Sil1H->St->begin()+i);
          Sil1H->Mod->erase(Sil1H->Mod->begin()+i);
          Sil1H->L0H->erase(Sil1H->L0H->begin()+i);
          Sil1H->L1H->erase(Sil1H->L1H->begin()+i);
          Sil1H->AmpDir->erase(Sil1H->AmpDir->begin()+i);
          Sil1H->AmpIncl->erase(Sil1H->AmpIncl->begin()+i);
          Sil1H->nHits--;
          nH_Si1--;
          i=i-1;

        }
      }
    }
    for (int i=0; i<Sil2H->X->size(); i++) {
      for (int j=0; j<Sil2H->X->size(); j++) {
        if (i<=j) continue;
        if (Sil2H->X->at(i)==Sil2H->X->at(j)){
          Sil2H->Y->at(j)=(Sil2H->Y->at(i)*Sil2H->AmpIncl->at(i)+Sil2H->Y->at(j)*Sil2H->AmpIncl->at(j))/(Sil2H->AmpIncl->at(i)+Sil2H->AmpIncl->at(j));
          Sil2H->L1H->at(j)=(Sil2H->L1H->at(i)+Sil2H->L1H->at(j));
          Sil2H->AmpIncl->at(j)=(Sil2H->AmpIncl->at(i)+Sil2H->AmpIncl->at(j));
          Sil2H->X->erase(Sil2H->X->begin()+i);
          Sil2H->Y->erase(Sil2H->Y->begin()+i);
          Sil2H->St->erase(Sil2H->St->begin()+i);
          Sil2H->Mod->erase(Sil2H->Mod->begin()+i);
          Sil2H->L0H->erase(Sil2H->L0H->begin()+i);
          Sil2H->L1H->erase(Sil2H->L1H->begin()+i);
          Sil2H->AmpDir->erase(Sil2H->AmpDir->begin()+i);
          Sil2H->AmpIncl->erase(Sil2H->AmpIncl->begin()+i);
          Sil2H->nHits--;
          nH_Si2--;
          i=i-1;

        }
      }
    }
    for (int i=0; i<Sil3H->X->size(); i++) {
      for (int j=0; j<Sil3H->X->size(); j++) {
        if (i<=j) continue;
        if (Sil3H->Y->at(i)==Sil3H->Y->at(j)){
          Sil3H->X->at(j)=(Sil3H->X->at(i)*Sil3H->AmpIncl->at(i)+Sil3H->X->at(j)*Sil3H->AmpIncl->at(j))/(Sil3H->AmpIncl->at(i)+Sil3H->AmpIncl->at(j));
          Sil3H->L1H->at(j)=(Sil3H->L1H->at(i)+Sil3H->L1H->at(j));
          Sil3H->AmpIncl->at(j)=(Sil3H->AmpIncl->at(i)+Sil3H->AmpIncl->at(j));
          Sil3H->X->erase(Sil3H->X->begin()+i);
          Sil3H->Y->erase(Sil3H->Y->begin()+i);
          Sil3H->St->erase(Sil3H->St->begin()+i);
          Sil3H->Mod->erase(Sil3H->Mod->begin()+i);
          Sil3H->L0H->erase(Sil3H->L0H->begin()+i);
          Sil3H->L1H->erase(Sil3H->L1H->begin()+i);
          Sil3H->AmpDir->erase(Sil3H->AmpDir->begin()+i);
          Sil3H->AmpIncl->erase(Sil3H->AmpIncl->begin()+i);
          Sil3H->nHits--;
          nH_Si3--;
          i=i-1;

        }
      }
    }
    for (int i=0; i<Sil4H->X->size(); i++) {
      for (int j=0; j<Sil4H->X->size(); j++) {
        if (i<=j) continue;
        if (Sil4H->X->at(i)==Sil4H->X->at(j)){
          Sil4H->Y->at(j)=(Sil4H->Y->at(i)*Sil4H->AmpIncl->at(i)+Sil4H->Y->at(j)*Sil4H->AmpIncl->at(j))/(Sil4H->AmpIncl->at(i)+Sil4H->AmpIncl->at(j));
          Sil4H->L1H->at(j)=(Sil4H->L1H->at(i)+Sil4H->L1H->at(j));
          Sil4H->AmpIncl->at(j)=(Sil4H->AmpIncl->at(i)+Sil4H->AmpIncl->at(j));
          Sil4H->X->erase(Sil4H->X->begin()+i);
          Sil4H->Y->erase(Sil4H->Y->begin()+i);
          Sil4H->St->erase(Sil4H->St->begin()+i);
          Sil4H->Mod->erase(Sil4H->Mod->begin()+i);
          Sil4H->L0H->erase(Sil4H->L0H->begin()+i);
          Sil4H->L1H->erase(Sil4H->L1H->begin()+i);
          Sil4H->AmpDir->erase(Sil4H->AmpDir->begin()+i);
          Sil4H->AmpIncl->erase(Sil4H->AmpIncl->begin()+i);
          Sil4H->nHits--;
          nH_Si4--;
          i=i-1;

        }
      }
    }
    // if (wasev!=0) {
    //   cout<<ev<<" "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
      // for (int i=0; i<Sil1H->X->size(); i++) {
      //     //cout<<Si1X.at(i)<<" "<<Si1Y.at(i)<<" "<<endl;
      //     cout<<Sil1H->X->at(i)<<" "<<Sil1H->Y->at(i)<<" "<<endl;
      // }
    // }

    // cout<<ev<<" 7 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
    // cout<<ev<<" 7 "<<Sil1H->X->size()<<" "<<Sil2H->X->size()<<" "<<Sil3H->X->size()<<" "<<Sil4H->X->size()<<"  "<<MWPC_p1->nTracks<<endl;
    // cout<<ev<<" 7 "<<Sil1H->Y->size()<<" "<<Sil2H->Y->size()<<" "<<Sil3H->Y->size()<<" "<<Sil4H->Y->size()<<"  "<<MWPC_p1->nTracks<<endl;
    
    for (int i=0; i<nH_Si1; i++) {

      //cout<<ev<<" "<<i<<" 71 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;

      if (Sil1H->X->size()>0) {
        if (Sil1H->X->at(i)<0.337){
          Si1X.push_back(Sil1H->X->at(i)+XglobalShift-0.0384);
        }
        if (Sil1H->X->at(i)>0.337){
          Si1X.push_back(Sil1H->X->at(i)+XglobalShift+0.1286);
        }
        //cout<<ev<<" "<<i<<" 72 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
        if (Sil1H->Mod->at(i)==0){
          Si1Y.push_back(Sil1H->Y->at(i)+YglobalShift-0.1975-0.0311+0.0014+0.0192);
        }
        if (Sil1H->Mod->at(i)==1){
          Si1Y.push_back(Sil1H->Y->at(i)+YglobalShift-0.3476-0.0241+0.0023+0.1696-0.0099);
        }
        //cout<<ev<<" "<<i<<" 73 "<<Sil1H->X->size()<<" "<<Si1X.size()<<" "<<Sil1H->X->at(i)<<" "<<Si1Y.size()<<"  "<<MWPC_p1->nTracks<<endl;
        Sil1H->X->at(i)=Si1X.at(i);
        Sil1H->Y->at(i)=Si1Y.at(i);
        
        if (Sil1H->Mod->at(i)==0)
        Si1Z.push_back(Ztarg[0][0]);
        if (Sil1H->Mod->at(i)==1)
        Si1Z.push_back(Ztarg[0][1]);
      }
      
    }
    //cout<<ev<<" 7 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
    for (int i=0; i<nH_Si2; i++) {

      if (Sil2H->X->size()>0) {
        if (Sil2H->Mod->at(i)==0){
          Si2X.push_back(Sil2H->X->at(i)+XglobalShift+0.1881-0.0010-0.0009+0.1286-0.0220);
        }
        if (Sil2H->Mod->at(i)==1){
          Si2X.push_back(Sil2H->X->at(i)+XglobalShift+0.3636-0.0318+0.0011+0.0186-0.0384);
        }

        if (Sil2H->Y->at(i)<-0.481){
          Si2Y.push_back(Sil2H->Y->at(i)+YglobalShift+0.0192);
        }
        if (Sil2H->Y->at(i)>-0.481){
          Si2Y.push_back(Sil2H->Y->at(i)+YglobalShift+0.1696);    
        }

        Sil2H->X->at(i)=Si2X.at(i);
        Sil2H->Y->at(i)=Si2Y.at(i);

        if (Sil2H->Mod->at(i)==0)
        Si2Z.push_back(Ztarg[1][0]);
        if (Sil2H->Mod->at(i)==1)
        Si2Z.push_back(Ztarg[1][1]);
      }
    }
    //cout<<ev<<" "<<nH_Si2<<"SiH "<<Si2X.size()<<" "<<Sil2H->X->size()<<endl;

    for (int i=0; i<nH_Si3; i++) {

      if (Sil3H->X->size()>0) {
        if (Sil3H->X->at(i)<0.628){
          Si3X.push_back(Sil3H->X->at(i)+XglobalShift-0.2281-0.0292-0.0088+0.0067-0.0384);
        }
        if (Sil3H->X->at(i)>0.628){
          Si3X.push_back(Sil3H->X->at(i)+XglobalShift-0.2281-0.0292-0.0088+0.0067+0.1286);
        }
        if (Sil3H->Mod->at(i)==0){
          Si3Y.push_back(Sil3H->Y->at(i)+YglobalShift+0.05408-0.1975-0.0189+0.0033-0.0179-0.0014+0.0192);
        }
        if (Sil3H->Mod->at(i)==1){
          Si3Y.push_back(Sil3H->Y->at(i)+YglobalShift+0.05408-0.3476-0.0189-0.0024-0.0179+0.0013+0.1696);
        }

        Sil3H->X->at(i)=Si3X.at(i);
        Sil3H->Y->at(i)=Si3Y.at(i);

        if (Sil3H->Mod->at(i)==0)
        Si3Z.push_back(Ztarg[2][0]);
        if (Sil3H->Mod->at(i)==1)
        Si3Z.push_back(Ztarg[2][1]);
      }
    }

    for (int i=0; i<nH_Si4; i++) {

      if (Sil4H->X->size()>0) {
        if (Sil4H->Mod->at(i)==0){
          Si4X.push_back(Sil4H->X->at(i)+XglobalShift-0.2281+0.1881-0.0292-0.0006-0.0088-0.0009+0.0067+0.1286); 
        }
        if (Sil4H->Mod->at(i)==1){
          Si4X.push_back(Sil4H->X->at(i)+XglobalShift-0.2281+0.3636-0.0292+0.0006-0.0088-0.0017+0.0067-0.0384+0.0042);      
        }

        if (Sil4H->Y->at(i)<-0.45){
          Si4Y.push_back(Sil4H->Y->at(i)+YglobalShift+0.05408-0.0189-0.0179+0.0192);
        }
        if (Sil4H->Y->at(i)>-0.45){
          Si4Y.push_back(Sil4H->Y->at(i)+YglobalShift+0.05408-0.0189-0.0179+0.1696);
        }

        Sil4H->X->at(i)=Si4X.at(i);
        Sil4H->Y->at(i)=Si4Y.at(i);

        if (Sil4H->Mod->at(i)==0)
        Si4Z.push_back(Ztarg[3][0]);
        if (Sil4H->Mod->at(i)==1)
        Si4Z.push_back(Ztarg[3][1]);
      }
    }


    for (int i=0; i<nH_MWPC3; i++) {
      if (MWPC3->X->size()>0) { 
        MWPC3->X->at(i)=MWPC3->X->at(i)-0.07315-0.0198;
        MWPC3->Y->at(i)=MWPC3->Y->at(i)+0.04894+0.01155;
      }
    }

    for (int i=0; i<MWPC_p1->nTracks; i++) {
        MWPC_p1->Tx->at(i)=MWPC_p1->Tx->at(i)+0.000101;
        MWPC_p1->Ty->at(i)=MWPC_p1->Ty->at(i)+0.000086;
        MWPC_p1->X->at(i)=MWPC_p1->X->at(i)+0.02247;
        MWPC_p1->Y->at(i)=MWPC_p1->Y->at(i)-0.02317;
    }

    // if ((MWPC_p1->nTracks)==2) 
    //  if ((DCH->nTracks)==2)   
    // cout<<ev<<" "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
    // for (int i=0; i<Si1X.size(); i++) {
    //     cout<<Si1X.at(i)<<" "<<Si1Y.at(i)<<" "<<endl;
    //     //cout<<Sil1H->X->at(i)<<" "<<Sil1H->Y->at(i)<<" "<<endl;
    // }


    
    
    //cout<<ev<<" 8 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
    int mult[nH_Si2][nH_Si2];
    int isdouble=0;
    for (int i=0; i<nH_Si2; i++) {
      for (int j=0; j<nH_Si2; j++) {
        mult[i][j]=0;
      }
    }

    //cout<<ev<<" "<<Sil1H->nHits<<" "<<nH_Si1<<" "<<Sil1H->X->size()<<" "<<Sil1H->Y->size()<<"  "<<Si1X.size()<<endl;
    for (int i=0; i<Sil1H->X->size(); i++) {
        //cout<<Si1X.at(i)<<" "<<Si1Y.at(i)<<" "<<endl;
        //cout<<Sil1H->X->at(i)<<" "<<Sil1H->Y->at(i)<<" "<<endl;
    }

    for (int i=0; i<Sil1H->nHits; i++) {
      for (int j=0; j<Sil1H->nHits; j++) {
        if (i>=j) continue;
        // dX43.push_back(Si1X.at(j)-Si1X.at(i));
        // dY43.push_back(Si1Y.at(j)-Si1Y.at(i));
        //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
        if (Sil1H->Mod->at(i)==Sil1H->Mod->at(j) && abs(Si1Y.at(j)-Si1Y.at(i))>0.015 && abs(Si1Y.at(j)-Si1Y.at(i))<0.04 && Si1X.at(j)-Si1X.at(i)>0.4 && Si1X.at(j)-Si1X.at(i)<1) {
          //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
          Si1X.at(j)=(Si1X.at(j)*Sil1H->AmpIncl->at(j)+Si1X.at(i)*Sil1H->AmpIncl->at(i))/(Sil1H->AmpIncl->at(i)+Sil1H->AmpIncl->at(j));
          Si1Y.at(j)=(Si1Y.at(j)*Sil1H->AmpDir->at(j)+Si1Y.at(i)*Sil1H->AmpDir->at(i))/(Sil1H->AmpDir->at(i)+Sil1H->AmpDir->at(j));
          Si1X.erase(Si1X.begin()+i);
          Si1Y.erase(Si1Y.begin()+i);
          Si1Z.erase(Si1Z.begin()+i);
          
          Sil1H->X->at(j)=(Sil1H->X->at(i)*Sil1H->AmpIncl->at(i)+Sil1H->X->at(j)*Sil1H->AmpIncl->at(j))/(Sil1H->AmpIncl->at(i)+Sil1H->AmpIncl->at(j));
          Sil1H->Y->at(j)=(Sil1H->Y->at(i)*Sil1H->AmpDir->at(i)+Sil1H->Y->at(j)*Sil1H->AmpDir->at(j))/(Sil1H->AmpDir->at(i)+Sil1H->AmpDir->at(j));
          Sil1H->L0H->at(j)=(Sil1H->L0H->at(i)+Sil1H->L0H->at(j));
          Sil1H->L1H->at(j)=(Sil1H->L1H->at(i)+Sil1H->L1H->at(j));
          Sil1H->AmpDir->at(j)=(Sil1H->AmpDir->at(i)+Sil1H->AmpDir->at(j));
          Sil1H->AmpIncl->at(j)=(Sil1H->AmpIncl->at(i)+Sil1H->AmpIncl->at(j));
          Sil1H->X->erase(Sil1H->X->begin()+i);
          Sil1H->Y->erase(Sil1H->Y->begin()+i);
          Sil1H->St->erase(Sil1H->St->begin()+i);
          Sil1H->Mod->erase(Sil1H->Mod->begin()+i);
          Sil1H->L0H->erase(Sil1H->L0H->begin()+i);
          Sil1H->L1H->erase(Sil1H->L1H->begin()+i);
          Sil1H->AmpDir->erase(Sil1H->AmpDir->begin()+i);
          Sil1H->AmpIncl->erase(Sil1H->AmpIncl->begin()+i);
          Sil1H->nHits--;
          nH_Si1--;
          
          isdouble++;
          
          j--;
          
          //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
        }

        else if (abs(Si1Y.at(j)-Si1Y.at(i))<0.04 && (Sil1H->Mod->at(i)!=Sil1H->Mod->at(j))) {
          //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
          Si1X.at(j)=(Si1X.at(j)*Sil1H->AmpIncl->at(j)+Si1X.at(i)*Sil1H->AmpIncl->at(i))/(Sil1H->AmpIncl->at(i)+Sil1H->AmpIncl->at(j));
          Si1Y.at(j)=(Si1Y.at(j)*Sil1H->AmpDir->at(j)+Si1Y.at(i)*Sil1H->AmpDir->at(i))/(Sil1H->AmpDir->at(i)+Sil1H->AmpDir->at(j));
          Si1X.erase(Si1X.begin()+i);
          Si1Y.erase(Si1Y.begin()+i);
          Si1Z.erase(Si1Z.begin()+i);
          
          Sil1H->X->at(j)=(Sil1H->X->at(i)*Sil1H->AmpIncl->at(i)+Sil1H->X->at(j)*Sil1H->AmpIncl->at(j))/(Sil1H->AmpIncl->at(i)+Sil1H->AmpIncl->at(j));
          Sil1H->Y->at(j)=(Sil1H->Y->at(i)*Sil1H->AmpDir->at(i)+Sil1H->Y->at(j)*Sil1H->AmpDir->at(j))/(Sil1H->AmpDir->at(i)+Sil1H->AmpDir->at(j));
          Sil1H->L0H->at(j)=(Sil1H->L0H->at(i)+Sil1H->L0H->at(j));
          Sil1H->L1H->at(j)=(Sil1H->L1H->at(i)+Sil1H->L1H->at(j));
          Sil1H->AmpDir->at(j)=(Sil1H->AmpDir->at(i)+Sil1H->AmpDir->at(j));
          Sil1H->AmpIncl->at(j)=(Sil1H->AmpIncl->at(i)+Sil1H->AmpIncl->at(j));
          Sil1H->X->erase(Sil1H->X->begin()+i);
          Sil1H->Y->erase(Sil1H->Y->begin()+i);
          Sil1H->St->erase(Sil1H->St->begin()+i);
          Sil1H->Mod->erase(Sil1H->Mod->begin()+i);
          Sil1H->L0H->erase(Sil1H->L0H->begin()+i);
          Sil1H->L1H->erase(Sil1H->L1H->begin()+i);
          Sil1H->AmpDir->erase(Sil1H->AmpDir->begin()+i);
          Sil1H->AmpIncl->erase(Sil1H->AmpIncl->begin()+i);
          Sil1H->nHits--;
          nH_Si1--;
          
          isdouble++;
          
          j--;
          
          //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
        }
      }
    }
    // for (int i=0; i<nH_Si1; i++) {
    //   for (int j=0; j<nH_Si1; j++) {
    //     if (i>=j) continue;
    //     dX43.push_back(Si1X.at(j)-Si1X.at(i));
    //     dY43.push_back(Si1Y.at(j)-Si1Y.at(i));
    //   }
    // }

    for (int i=0; i<Sil2H->nHits; i++) {
      for (int j=0; j<Sil2H->nHits; j++) {
        if (i>=j) continue;
        // dX43.push_back(Si2X.at(j)-Si2X.at(i));
        // dY43.push_back(Si2Y.at(j)-Si2Y.at(i));
        //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
        if (Sil2H->Mod->at(i)==Sil2H->Mod->at(j) && abs(Si2X.at(j)-Si2X.at(i))>0.015 && abs(Si2X.at(j)-Si2X.at(i))<0.04 && Si2Y.at(j)-Si2Y.at(i)>-1 && Si2Y.at(j)-Si2Y.at(i)<-0.4) {
          //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
          Si2X.at(j)=(Si2X.at(j)*Sil2H->AmpDir->at(j)+Si2X.at(i)*Sil2H->AmpDir->at(i))/(Sil2H->AmpDir->at(i)+Sil2H->AmpDir->at(j));
          Si2Y.at(j)=(Si2Y.at(j)*Sil2H->AmpIncl->at(j)+Si2Y.at(i)*Sil2H->AmpIncl->at(i))/(Sil2H->AmpIncl->at(i)+Sil2H->AmpIncl->at(j));
          Si2X.erase(Si2X.begin()+i);
          Si2Y.erase(Si2Y.begin()+i);
          Si2Z.erase(Si2Z.begin()+i);

          Sil2H->X->at(j)=(Sil2H->X->at(i)*Sil2H->AmpDir->at(i)+Sil2H->X->at(j)*Sil2H->AmpDir->at(j))/(Sil2H->AmpDir->at(i)+Sil2H->AmpDir->at(j));
          Sil2H->Y->at(j)=(Sil2H->Y->at(i)*Sil2H->AmpIncl->at(i)+Sil2H->Y->at(j)*Sil2H->AmpIncl->at(j))/(Sil2H->AmpIncl->at(i)+Sil2H->AmpIncl->at(j));
          Sil2H->L0H->at(j)=(Sil2H->L0H->at(i)+Sil2H->L0H->at(j));
          Sil2H->L1H->at(j)=(Sil2H->L1H->at(i)+Sil2H->L1H->at(j));
          Sil2H->AmpDir->at(j)=(Sil2H->AmpDir->at(i)+Sil2H->AmpDir->at(j));
          Sil2H->AmpIncl->at(j)=(Sil2H->AmpIncl->at(i)+Sil2H->AmpIncl->at(j));
          Sil2H->X->erase(Sil2H->X->begin()+i);
          Sil2H->Y->erase(Sil2H->Y->begin()+i);
          Sil2H->St->erase(Sil2H->St->begin()+i);
          Sil2H->Mod->erase(Sil2H->Mod->begin()+i);
          Sil2H->L0H->erase(Sil2H->L0H->begin()+i);
          Sil2H->L1H->erase(Sil2H->L1H->begin()+i);
          Sil2H->AmpDir->erase(Sil2H->AmpDir->begin()+i);
          Sil2H->AmpIncl->erase(Sil2H->AmpIncl->begin()+i);
          Sil2H->nHits--;
          nH_Si2--;
          
          mult[i][j]++;
          isdouble++;
          j--;
          //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
        }

        else if (abs(Si2X.at(j)-Si2X.at(i))<0.04 && (Sil2H->Mod->at(i)!=Sil2H->Mod->at(j))) {
          //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
          Si2X.at(j)=(Si2X.at(j)*Sil2H->AmpDir->at(j)+Si2X.at(i)*Sil2H->AmpDir->at(i))/(Sil2H->AmpDir->at(i)+Sil2H->AmpDir->at(j));
          Si2Y.at(j)=(Si2Y.at(j)*Sil2H->AmpIncl->at(j)+Si2Y.at(i)*Sil2H->AmpIncl->at(i))/(Sil2H->AmpIncl->at(i)+Sil2H->AmpIncl->at(j));
          Si2X.erase(Si2X.begin()+i);
          Si2Y.erase(Si2Y.begin()+i);
          Si2Z.erase(Si2Z.begin()+i);

          Sil2H->X->at(j)=(Sil2H->X->at(i)*Sil2H->AmpDir->at(i)+Sil2H->X->at(j)*Sil2H->AmpDir->at(j))/(Sil2H->AmpDir->at(i)+Sil2H->AmpDir->at(j));
          Sil2H->Y->at(j)=(Sil2H->Y->at(i)*Sil2H->AmpIncl->at(i)+Sil2H->Y->at(j)*Sil2H->AmpIncl->at(j))/(Sil2H->AmpIncl->at(i)+Sil2H->AmpIncl->at(j));
          Sil2H->L0H->at(j)=(Sil2H->L0H->at(i)+Sil2H->L0H->at(j));
          Sil2H->L1H->at(j)=(Sil2H->L1H->at(i)+Sil2H->L1H->at(j));
          Sil2H->AmpDir->at(j)=(Sil2H->AmpDir->at(i)+Sil2H->AmpDir->at(j));
          Sil2H->AmpIncl->at(j)=(Sil2H->AmpIncl->at(i)+Sil2H->AmpIncl->at(j));
          Sil2H->X->erase(Sil2H->X->begin()+i);
          Sil2H->Y->erase(Sil2H->Y->begin()+i);
          Sil2H->St->erase(Sil2H->St->begin()+i);
          Sil2H->Mod->erase(Sil2H->Mod->begin()+i);
          Sil2H->L0H->erase(Sil2H->L0H->begin()+i);
          Sil2H->L1H->erase(Sil2H->L1H->begin()+i);
          Sil2H->AmpDir->erase(Sil2H->AmpDir->begin()+i);
          Sil2H->AmpIncl->erase(Sil2H->AmpIncl->begin()+i);
          Sil2H->nHits--;
          nH_Si2--;
          
          mult[i][j]++;
          isdouble++;
          j--;
          //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
        }
      }
    }
    // for (int i=0; i<nH_Si2; i++) {
    //   for (int j=0; j<nH_Si2; j++) {
    //     if (i>=j) continue;
    //     dX43.push_back(Si2X.at(j)-Si2X.at(i));
    //     dY43.push_back(Si2Y.at(j)-Si2Y.at(i));
    //   }
    // }

    for (int i=0; i<Sil3H->nHits; i++) {
      for (int j=0; j<Sil3H->nHits; j++) {
        if (i>=j) continue;
        // dX43.push_back(Si3X.at(j)-Si3X.at(i));
        // dY43.push_back(Si3Y.at(j)-Si3Y.at(i));
        //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
        if (Sil3H->Mod->at(i)==Sil3H->Mod->at(j) && abs(Si3Y.at(j)-Si3Y.at(i))>0.015 && abs(Si3Y.at(j)-Si3Y.at(i))<0.04 && Si3X.at(j)-Si3X.at(i)>0.4 && Si3X.at(j)-Si3X.at(i)<1) {
          //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
          Si3X.at(j)=(Si3X.at(j)*Sil3H->AmpIncl->at(j)+Si3X.at(i)*Sil3H->AmpIncl->at(i))/(Sil3H->AmpIncl->at(i)+Sil3H->AmpIncl->at(j));
          Si3Y.at(j)=(Si3Y.at(j)*Sil3H->AmpDir->at(j)+Si3Y.at(i)*Sil3H->AmpDir->at(i))/(Sil3H->AmpDir->at(i)+Sil3H->AmpDir->at(j));
          Si3X.erase(Si3X.begin()+i);
          Si3Y.erase(Si3Y.begin()+i);
          Si3Z.erase(Si3Z.begin()+i);
          
          Sil3H->X->at(j)=(Sil3H->X->at(i)*Sil3H->AmpIncl->at(i)+Sil3H->X->at(j)*Sil3H->AmpIncl->at(j))/(Sil3H->AmpIncl->at(i)+Sil3H->AmpIncl->at(j));
          Sil3H->Y->at(j)=(Sil3H->Y->at(i)*Sil3H->AmpDir->at(i)+Sil3H->Y->at(j)*Sil3H->AmpDir->at(j))/(Sil3H->AmpDir->at(i)+Sil3H->AmpDir->at(j));
          Sil3H->L0H->at(j)=(Sil3H->L0H->at(i)+Sil3H->L0H->at(j));
          Sil3H->L1H->at(j)=(Sil3H->L1H->at(i)+Sil3H->L1H->at(j));
          Sil3H->AmpDir->at(j)=(Sil3H->AmpDir->at(i)+Sil3H->AmpDir->at(j));
          Sil3H->AmpIncl->at(j)=(Sil3H->AmpIncl->at(i)+Sil3H->AmpIncl->at(j));
          Sil3H->X->erase(Sil3H->X->begin()+i);
          Sil3H->Y->erase(Sil3H->Y->begin()+i);
          Sil3H->St->erase(Sil3H->St->begin()+i);
          Sil3H->Mod->erase(Sil3H->Mod->begin()+i);
          Sil3H->L0H->erase(Sil3H->L0H->begin()+i);
          Sil3H->L1H->erase(Sil3H->L1H->begin()+i);
          Sil3H->AmpDir->erase(Sil3H->AmpDir->begin()+i);
          Sil3H->AmpIncl->erase(Sil3H->AmpIncl->begin()+i);
          Sil3H->nHits--;
          nH_Si3--;
          
          isdouble++;
          
          j--;
          
          //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
        }

        else if (abs(Si3Y.at(j)-Si3Y.at(i))<0.04 && (Sil3H->Mod->at(i)!=Sil3H->Mod->at(j))) {
          //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
          Si3X.at(j)=(Si3X.at(j)*Sil3H->AmpIncl->at(j)+Si3X.at(i)*Sil3H->AmpIncl->at(i))/(Sil3H->AmpIncl->at(i)+Sil3H->AmpIncl->at(j));
          Si3Y.at(j)=(Si3Y.at(j)*Sil3H->AmpDir->at(j)+Si3Y.at(i)*Sil3H->AmpDir->at(i))/(Sil3H->AmpDir->at(i)+Sil3H->AmpDir->at(j));
          Si3X.erase(Si3X.begin()+i);
          Si3Y.erase(Si3Y.begin()+i);
          Si3Z.erase(Si3Z.begin()+i);
          
          Sil3H->X->at(j)=(Sil3H->X->at(i)*Sil3H->AmpIncl->at(i)+Sil3H->X->at(j)*Sil3H->AmpIncl->at(j))/(Sil3H->AmpIncl->at(i)+Sil3H->AmpIncl->at(j));
          Sil3H->Y->at(j)=(Sil3H->Y->at(i)*Sil3H->AmpDir->at(i)+Sil3H->Y->at(j)*Sil3H->AmpDir->at(j))/(Sil3H->AmpDir->at(i)+Sil3H->AmpDir->at(j));
          Sil3H->L0H->at(j)=(Sil3H->L0H->at(i)+Sil3H->L0H->at(j));
          Sil3H->L1H->at(j)=(Sil3H->L1H->at(i)+Sil3H->L1H->at(j));
          Sil3H->AmpDir->at(j)=(Sil3H->AmpDir->at(i)+Sil3H->AmpDir->at(j));
          Sil3H->AmpIncl->at(j)=(Sil3H->AmpIncl->at(i)+Sil3H->AmpIncl->at(j));
          Sil3H->X->erase(Sil3H->X->begin()+i);
          Sil3H->Y->erase(Sil3H->Y->begin()+i);
          Sil3H->St->erase(Sil3H->St->begin()+i);
          Sil3H->Mod->erase(Sil3H->Mod->begin()+i);
          Sil3H->L0H->erase(Sil3H->L0H->begin()+i);
          Sil3H->L1H->erase(Sil3H->L1H->begin()+i);
          Sil3H->AmpDir->erase(Sil3H->AmpDir->begin()+i);
          Sil3H->AmpIncl->erase(Sil3H->AmpIncl->begin()+i);
          Sil3H->nHits--;
          nH_Si3--;
          
          isdouble++;
          
          j--;
          
          //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
        }
      }
    }
    // for (int i=0; i<nH_Si3; i++) {
    //   for (int j=0; j<nH_Si3; j++) {
    //     if (i>=j) continue;
    //     dX43.push_back(Si3X.at(j)-Si3X.at(i));
    //     dY43.push_back(Si3Y.at(j)-Si3Y.at(i));
    //   }
    // }

    for (int i=0; i<Sil4H->nHits; i++) {
      for (int j=0; j<Sil4H->nHits; j++) {
        if (i>=j) continue;
        // dX43.push_back(Si4X.at(j)-Si4X.at(i));
        // dY43.push_back(Si4Y.at(j)-Si4Y.at(i));
        //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
        if (Sil4H->Mod->at(i)==Sil4H->Mod->at(j) && abs(Si4X.at(j)-Si4X.at(i))>0.015 && abs(Si4X.at(j)-Si4X.at(i))<0.04 && Si4Y.at(j)-Si4Y.at(i)>-1 && Si4Y.at(j)-Si4Y.at(i)<-0.4) {
          //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
          Si4X.at(j)=(Si4X.at(j)*Sil4H->AmpDir->at(j)+Si4X.at(i)*Sil4H->AmpDir->at(i))/(Sil4H->AmpDir->at(i)+Sil4H->AmpDir->at(j));
          Si4Y.at(j)=(Si4Y.at(j)*Sil4H->AmpIncl->at(j)+Si4Y.at(i)*Sil4H->AmpIncl->at(i))/(Sil4H->AmpIncl->at(i)+Sil4H->AmpIncl->at(j));
          Si4X.erase(Si4X.begin()+i);
          Si4Y.erase(Si4Y.begin()+i);
          Si4Z.erase(Si4Z.begin()+i);

          Sil4H->X->at(j)=(Sil4H->X->at(i)*Sil4H->AmpDir->at(i)+Sil4H->X->at(j)*Sil4H->AmpDir->at(j))/(Sil4H->AmpDir->at(i)+Sil4H->AmpDir->at(j));
          Sil4H->Y->at(j)=(Sil4H->Y->at(i)*Sil4H->AmpIncl->at(i)+Sil4H->Y->at(j)*Sil4H->AmpIncl->at(j))/(Sil4H->AmpIncl->at(i)+Sil4H->AmpIncl->at(j));
          Sil4H->L0H->at(j)=(Sil4H->L0H->at(i)+Sil4H->L0H->at(j));
          Sil4H->L1H->at(j)=(Sil4H->L1H->at(i)+Sil4H->L1H->at(j));
          Sil4H->AmpDir->at(j)=(Sil4H->AmpDir->at(i)+Sil4H->AmpDir->at(j));
          Sil4H->AmpIncl->at(j)=(Sil4H->AmpIncl->at(i)+Sil4H->AmpIncl->at(j));
          Sil4H->X->erase(Sil4H->X->begin()+i);
          Sil4H->Y->erase(Sil4H->Y->begin()+i);
          Sil4H->St->erase(Sil4H->St->begin()+i);
          Sil4H->Mod->erase(Sil4H->Mod->begin()+i);
          Sil4H->L0H->erase(Sil4H->L0H->begin()+i);
          Sil4H->L1H->erase(Sil4H->L1H->begin()+i);
          Sil4H->AmpDir->erase(Sil4H->AmpDir->begin()+i);
          Sil4H->AmpIncl->erase(Sil4H->AmpIncl->begin()+i);
          Sil4H->nHits--;
          nH_Si4--;
          
          //mult[i][j]++;
          isdouble++;
          j--;
          //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
        }

        else if (abs(Si4X.at(j)-Si4X.at(i))<0.04 && (Sil4H->Mod->at(i)!=Sil4H->Mod->at(j))) {
          //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
          Si4X.at(j)=(Si4X.at(j)*Sil4H->AmpDir->at(j)+Si4X.at(i)*Sil4H->AmpDir->at(i))/(Sil4H->AmpDir->at(i)+Sil4H->AmpDir->at(j));
          Si4Y.at(j)=(Si4Y.at(j)*Sil4H->AmpIncl->at(j)+Si4Y.at(i)*Sil4H->AmpIncl->at(i))/(Sil4H->AmpIncl->at(i)+Sil4H->AmpIncl->at(j));
          Si4X.erase(Si4X.begin()+i);
          Si4Y.erase(Si4Y.begin()+i);
          Si4Z.erase(Si4Z.begin()+i);

          Sil4H->X->at(j)=(Sil4H->X->at(i)*Sil4H->AmpDir->at(i)+Sil4H->X->at(j)*Sil4H->AmpDir->at(j))/(Sil4H->AmpDir->at(i)+Sil4H->AmpDir->at(j));
          Sil4H->Y->at(j)=(Sil4H->Y->at(i)*Sil4H->AmpIncl->at(i)+Sil4H->Y->at(j)*Sil4H->AmpIncl->at(j))/(Sil4H->AmpIncl->at(i)+Sil4H->AmpIncl->at(j));
          Sil4H->L0H->at(j)=(Sil4H->L0H->at(i)+Sil4H->L0H->at(j));
          Sil4H->L1H->at(j)=(Sil4H->L1H->at(i)+Sil4H->L1H->at(j));
          Sil4H->AmpDir->at(j)=(Sil4H->AmpDir->at(i)+Sil4H->AmpDir->at(j));
          Sil4H->AmpIncl->at(j)=(Sil4H->AmpIncl->at(i)+Sil4H->AmpIncl->at(j));
          Sil4H->X->erase(Sil4H->X->begin()+i);
          Sil4H->Y->erase(Sil4H->Y->begin()+i);
          Sil4H->St->erase(Sil4H->St->begin()+i);
          Sil4H->Mod->erase(Sil4H->Mod->begin()+i);
          Sil4H->L0H->erase(Sil4H->L0H->begin()+i);
          Sil4H->L1H->erase(Sil4H->L1H->begin()+i);
          Sil4H->AmpDir->erase(Sil4H->AmpDir->begin()+i);
          Sil4H->AmpIncl->erase(Sil4H->AmpIncl->begin()+i);
          Sil4H->nHits--;
          nH_Si4--;
          
          //mult[i][j]++;
          isdouble++;
          j--;
          //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
        }

        // if (Si4X.at(i)<1.2 && Si4X.at(i)>0.8) {
        //   //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
          
        //   Si4X.erase(Si4X.begin()+i);
        //   Si4Y.erase(Si4Y.begin()+i);
        //   Si4Z.erase(Si4Z.begin()+i);

        //   Sil4H->X->erase(Sil4H->X->begin()+i);
        //   Sil4H->Y->erase(Sil4H->Y->begin()+i);
        //   Sil4H->St->erase(Sil4H->St->begin()+i);
        //   Sil4H->Mod->erase(Sil4H->Mod->begin()+i);
        //   Sil4H->L0H->erase(Sil4H->L0H->begin()+i);
        //   Sil4H->L1H->erase(Sil4H->L1H->begin()+i);
        //   Sil4H->AmpDir->erase(Sil4H->AmpDir->begin()+i);
        //   Sil4H->AmpIncl->erase(Sil4H->AmpIncl->begin()+i);
        //   Sil4H->nHits--;
        //   nH_Si4--;
          
        //   //mult[i][j]++;
        //   isdouble++;
        //   j--;
        //   //cout<<i<<" "<<j<<" "<<Si2X.size()<<" "<<Si2Y.size()<<" "<<endl;
        // }
      }
    }
    //cout<<ev<<" 9 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
    // for (int i=0; i<nH_Si4; i++) {
    //   for (int j=0; j<nH_Si4; j++) {
    //     if (i>=j) continue;
    //     dX43.push_back(Si4X.at(j)-Si4X.at(i));
    //     dY43.push_back(Si4Y.at(j)-Si4Y.at(i));
    //   }
    // }
  

    // for (int i=0; i<nH_MWPC3; i++) {
    //   for (int j=0; j<nH_MWPC3; j++) {
    //     //if (i>=j) continue;
    //     dX43.push_back(MWPC3->X->at(j)-MWPC3->X->at(i));
    //     dY43.push_back(MWPC3->Y->at(j)-MWPC3->Y->at(i));
    //   }
    // }
    // for (int i=0; i<Sil1H->nHits; i++) {
      
    //   if (Sil1H->AmpDir->at(i)<1000 && Sil1H->AmpIncl->at(i)<1000) {
    //     //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
    //     Si1X.erase(Si1X.begin()+i);
    //     Si1Y.erase(Si1Y.begin()+i);
      
    //     Sil1H->X->erase(Sil1H->X->begin()+i);
    //     Sil1H->Y->erase(Sil1H->Y->begin()+i);
    //     Sil1H->St->erase(Sil1H->St->begin()+i);
    //     Sil1H->Mod->erase(Sil1H->Mod->begin()+i);
    //     Sil1H->L0H->erase(Sil1H->L0H->begin()+i);
    //     Sil1H->L1H->erase(Sil1H->L1H->begin()+i);
    //     Sil1H->AmpDir->erase(Sil1H->AmpDir->begin()+i);
    //     Sil1H->AmpIncl->erase(Sil1H->AmpIncl->begin()+i);
    //     Sil1H->nHits--;
    //     nH_Si1--;
          
    //     i--;
          
    //       //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
    //   }
    // }

    // for (int i=0; i<Sil2H->nHits; i++) {
      
    //   if (Sil2H->AmpDir->at(i)<1000 && Sil2H->AmpIncl->at(i)<1000) {
    //     //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
    //     Si2X.erase(Si2X.begin()+i);
    //     Si2Y.erase(Si2Y.begin()+i);
      
    //     Sil2H->X->erase(Sil2H->X->begin()+i);
    //     Sil2H->Y->erase(Sil2H->Y->begin()+i);
    //     Sil2H->St->erase(Sil2H->St->begin()+i);
    //     Sil2H->Mod->erase(Sil2H->Mod->begin()+i);
    //     Sil2H->L0H->erase(Sil2H->L0H->begin()+i);
    //     Sil2H->L1H->erase(Sil2H->L1H->begin()+i);
    //     Sil2H->AmpDir->erase(Sil2H->AmpDir->begin()+i);
    //     Sil2H->AmpIncl->erase(Sil2H->AmpIncl->begin()+i);
    //     Sil2H->nHits--;
    //     nH_Si2--;
          
    //     i--;
          
    //       //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
    //   }
    // }

    // for (int i=0; i<Sil3H->nHits; i++) {
      
    //   if (Sil3H->AmpDir->at(i)<1000 && Sil3H->AmpIncl->at(i)<1000) {
    //     //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
    //     Si3X.erase(Si3X.begin()+i);
    //     Si3Y.erase(Si3Y.begin()+i);
      
    //     Sil3H->X->erase(Sil3H->X->begin()+i);
    //     Sil3H->Y->erase(Sil3H->Y->begin()+i);
    //     Sil3H->St->erase(Sil3H->St->begin()+i);
    //     Sil3H->Mod->erase(Sil3H->Mod->begin()+i);
    //     Sil3H->L0H->erase(Sil3H->L0H->begin()+i);
    //     Sil3H->L1H->erase(Sil3H->L1H->begin()+i);
    //     Sil3H->AmpDir->erase(Sil3H->AmpDir->begin()+i);
    //     Sil3H->AmpIncl->erase(Sil3H->AmpIncl->begin()+i);
    //     Sil3H->nHits--;
    //     nH_Si3--;
          
    //     i--;
          
    //       //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
    //   }
    // }

    // for (int i=0; i<Sil4H->nHits; i++) {
      
    //   if (Sil4H->AmpDir->at(i)<1000 && Sil4H->AmpIncl->at(i)<1000) {
    //     //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
    //     Si4X.erase(Si4X.begin()+i);
    //     Si4Y.erase(Si4Y.begin()+i);
      
    //     Sil4H->X->erase(Sil4H->X->begin()+i);
    //     Sil4H->Y->erase(Sil4H->Y->begin()+i);
    //     Sil4H->St->erase(Sil4H->St->begin()+i);
    //     Sil4H->Mod->erase(Sil4H->Mod->begin()+i);
    //     Sil4H->L0H->erase(Sil4H->L0H->begin()+i);
    //     Sil4H->L1H->erase(Sil4H->L1H->begin()+i);
    //     Sil4H->AmpDir->erase(Sil4H->AmpDir->begin()+i);
    //     Sil4H->AmpIncl->erase(Sil4H->AmpIncl->begin()+i);
    //     Sil4H->nHits--;
    //     nH_Si4--;
          
    //     i--;
          
    //       //cout<<i<<" "<<j<<" "<<Si1X.size()<<" "<<Si1Y.size()<<" "<<endl;
    //   }
    // }

    
    // for (int i=0; i<nH_Si4; i++) {
    //   for (int j=0; j<nH_Si4; j++) {
    //     if (i>=j) continue;
    //     dX43.push_back(Si4X.at(j)-Si4X.at(i));
    //     dY43.push_back(Si4Y.at(j)-Si4Y.at(i));
    //   }
    // }
    //cout<<ev<<" "<<nH_Si2<<"  "<<Si2X.size()<<" "<<Si2Y.size()<<endl;
    // for (int i=0; i<nH_Si2; i++) {
    //   for (int j=0; j<nH_Si2; j++) {
    //     if (i>=j) continue;
    //     dX43.push_back(Si2X.at(j)-Si2X.at(i));
    //     dY43.push_back(Si2Y.at(j)-Si2Y.at(i));
    //   }
    // }
    // if (isdouble>0){
    //   cout<<ev<<" "<<nH_Si2<<"  "<<isdouble<<endl;
    //   for (int i=0; i<nH_Si2; i++) {
    //     cout<<Si2X.at(i)<<" "<<Si2Y.at(i)<<" "<<endl;
    //   }
    //   // for (int i=0; i<nH_Si2; i++) {
    //   //   for (int j=0; j<nH_Si2; j++) {
    //   //     //if (i>=j) continue;
    //   //     //cout<<mult[i][j]<<" ";
    //   //   }
    //   //   cout<<endl;
    //   // }
    // }
    //if (mult>4) EvWithtrack++;
    // double dX[nH_Si1][nH_Si2];
    // double dY[nH_Si1][nH_Si2];
    //if (nH_Si2==2) {
      // for (int i=0; i<nH_Si4; i++) {
      //   for (int j=0; j<nH_Si4; j++) {
      //     if (i==j) continue;
      //     //dX[i][j]=abs (Si1X.at(i)-Si1X.at(j));
      //     dX43.push_back(Si4X.at(j)-Si4X.at(i));
      //     dY43.push_back(Si4Y.at(j)-Si4Y.at(i));
      //     //dY[i][j]=abs (Si1Y.at(i)-Si1Y.at(j));
      //     //if (dX>0.5 || dY>0.5) continue;
      //     //cout<<dX[i][j]<<" "<<dY<<"  "<<nH_Si1<<"  "<<nH_Si2<<"  "<<ev<<endl;
      //   }
      // }
    //}

  
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
    //cout<<ev<<" 10 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
    if(nH_Si1>5) nH_Si1=0;
    if(nH_Si2>5) nH_Si2=0;
    if(nH_Si3>4) nH_Si3=0;
    if(nH_Si4>4) nH_Si4=0;
    if(nH_MWPC3>9) nH_MWPC3=0;
    if(nH_MWPC4>9) nH_MWPC4=0;

    long double ax, bx, ay, by, chi2x, chi2y;
    Sil1234->nTracks=0;
    Sil1234MWPC34->nTracks=0;
    Sil1234MWPC34->nTracksX=0;
    Sil1234MWPC34->nTracksY=0;
    
    Sil1234MWPC34->Flag=9000000+100000*nH_Si1+10000*nH_Si2+1000*nH_Si3+100*nH_Si4+10*nH_MWPC3+nH_MWPC4;

    int numX=0;
    int numY=0;
    int numX1=0;
    int numY1=0;
    // if (nH_Si4>0 && nH_Si3>0) {

    //   for (int i=0; i<nH_Si3; i++){
    //     for (int j=0; j<1; j++){
    //         dY43.push_back(Si4Y.at(j)-Si3Y.at(i));
    //         if (BC3<600) continue;
    //         if (BC5<600) continue;
    //         if (abs(Si4X.at(j)-Si3X.at(i))<0.005) numX++;
    //         if (abs(Si4Y.at(j)-Si3Y.at(i))<0.005) numY++;
    //         if (numX>1 && numX1==0) {EvWithtrackX++; numX1=1;}
    //         if (numY>1 && numY1==0) {EvWithtrackY++; numY1=1;}
    //     }
    //   }
    // }


   
    //cout<<ev<<" 11 "<<nH_Si1<<" "<<nH_Si2<<" "<<nH_Si3<<" "<<nH_Si4<<"  "<<MWPC_p1->nTracks<<endl;
 
    for (int maxTracks=0; maxTracks<6; maxTracks++) {
      // if (BC3<600) continue; // new change
      // if (BC5<600) continue; // new change
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
                ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
              ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
              ax=0, bx=0, ay=0, by=0, chi2x=0, chi2y=0;
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
     
      if (Sil1234->nTracks==1 && Sil1234MWPC34->caseX->at(bestX)==1 && Sil1234MWPC34->caseY->at(bestY)==1 && ev<1000) {
      //cout<<Sil1234MWPC34->Chi2X->at(bestX)<<" "<<Sil1234MWPC34->Chi2Y->at(bestY)<<endl;
      }



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


    SW_ELoss->clear();
    SW_CellId->clear();
    SW_Flag->clear();
    SW_TrId->clear();
    SW_OrderId->clear();

    VB_CellId->clear();

    Tof400_X->clear();
    Tof400_Y->clear();
    Tof400_Z->clear();
    Tof400_TS->clear();
    Tof400_DetId->clear();
    Tof400_Module->clear();//
    Tof400_Station->clear();//

      TofCal_Bar->clear(); // new 
      TofCal_Plane->clear();
      TofCal_Arm->clear(); 
      TofCal_GlBar->clear(); 
      TofCal_Time0->clear();
      TofCal_Time1->clear();
      TofCal_Amp0->clear();
      TofCal_Amp1->clear();
      TofCal_X->clear();
      TofCal_Y->clear();

    rightGemX->clear(); rightGemY->clear(); rightGemZ->clear();
    leftGemX->clear(); leftGemY->clear(); leftGemZ->clear();
    rightCscX->clear(); rightCscY->clear(); rightCscZ->clear();
    leftCscX->clear(); leftCscY->clear(); leftCscZ->clear();  
    rightGemXLocal->clear(), rightGemYLocal->clear(), leftGemXLocal->clear(), leftGemYLocal->clear(), 
    rightCscXLocal->clear(), rightCscYLocal->clear(), leftCscXLocal->clear(), leftCscYLocal->clear(); 

    dCSCH_cscX->clear(); dCSCH_cscY->clear(); dCSCH_cscZ->clear();


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