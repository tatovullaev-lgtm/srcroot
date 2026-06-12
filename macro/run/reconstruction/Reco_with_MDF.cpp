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
#include "MDF_S7.C"
#include "MDF_Tx_S6.C"

static Float_t workTime = 0.0;
const double Bdl=2.63;
using namespace std;
using namespace TMath;
const Double_t X0_offset  = 0; //1.21651;
const Double_t TX0_offset = 0; //-3.32e-03;
const Double_t TX1_offset = 0;

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





void Reco_with_MDF(TString infilename, TString outfilename) {

	printf("start read");
  double C12_Range[2]={7.400-1.8*0.1156, 7.400+3*0.1156};
  double C11_Range[2]={6.772-2.5*0.1241, 6.772+2.0*0.1241};
  double C10_Range[2]={6.159-1.5*0.09865, 6.159+1.5*0.09865};

  double B11_Range[2]={8.14-1.8*0.1507, 8.14+3*0.1507};
  double B10_Range[2]={7.388-3*0.1563, 7.388+1.5*0.1563};

  double Be10_Range[2]={9.245-1.5*0.1815, 9.245+2.5*0.1815};
  double Be9_Range[2]={8.329-2*0.1920, 8.329+1.5*0.1920};
  double Be7_Range[2]={6.481-2.5*0.1778, 6.481+2.5*0.1778};

  double Li9_Range[2]={11.1-1.5*0.2866, 11.11+1.5*0.2866};
  double Li8_Range[2]={9.859-1.5*0.2003, 9.859+2.0*0.2003};
  double Li7_Range[2]={8.623-2.0*0.2532, 8.623+2.5*0.2532};
  double Li6_Range[2]={7.411-2.5*0.2279, 7.411+2.0*0.2279};





  //filenum=5;

  //TString filename ="/zfs/scratch/lhep-src/reconstruction_11_24/MDF/dec25_G/Run_" + to_string(filenum) + "_MDF.root";
	TFile *treefile = new TFile(outfilename, "recreate");
	//output tree and its branches
	TTree *tree1 = new TTree("tree1","treelibrated tree");
	//tree1->SetDirectory(treefile);
  int event, run;
  vector <double> Pq_MDF_Flag, Pq_MDF_UpSt, Pq_MDF_X_UpSt, Pq_MDF_Y_UpSt, Pq_MDF_Z_UpSt, Tx_MDF_UpSt, Tx_AT, Ty_AT;
  vector <int> Fragment;
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


  TBranch *br_event =tree1->Branch("event",&event);
  TBranch *br_trigger =tree1->Branch("trigger",&trigger);
  TBranch *br_veto =tree1->Branch("VC",&veto);
  TBranch *br_run =tree1->Branch("run",&run);
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
  TBranch *br_Pq_MDF_UpSt =tree1->Branch("Pq_MDF_UpSt",&Pq_MDF_UpSt);
  TBranch *br_Pq_MDF_X_UpSt =tree1->Branch("Pq_MDF_X_UpSt",&Pq_MDF_X_UpSt);
  TBranch *br_Pq_MDF_Y_UpSt =tree1->Branch("Pq_MDF_Y_UpSt",&Pq_MDF_Y_UpSt);
  TBranch *br_Pq_MDF_Z_UpSt =tree1->Branch("Pq_MDF_Z_UpSt",&Pq_MDF_Z_UpSt);
  TBranch *br_Tx_AT =tree1->Branch("Tx_AT",&Tx_AT);
  TBranch *br_Ty_AT =tree1->Branch("Ty_AT",&Ty_AT);
  TBranch *br_Pq_MDF_Flag =tree1->Branch("Pq_MDF_Flag",&Pq_MDF_Flag);
  TBranch *br_Tx_MDF_UpSt =tree1->Branch("Tx_MDF_UpSt",&Tx_MDF_UpSt);
  TBranch *br_Fragment =tree1->Branch("Fragment",&Fragment);

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
  TBranch *br_Is_laser = tree1->Branch("Is_laser", &Is_laser); 
  TBranch *br_BC1_Time0 = tree1->Branch("BC1_Time0", &BC1_Time0); 
  TBranch *br_BC1_Time1 = tree1->Branch("BC1_Time1", &BC1_Time1); 
  TBranch *br_BC2_Time0 = tree1->Branch("BC2_Time0", &BC2_Time0); 
  TBranch *br_BC2_Time1 = tree1->Branch("BC2_Time1", &BC2_Time1); 

 	TBranch *br_TofCal_Bar = tree1->Branch("TofCal_Bar", &TofCal_Bar); 
 	TBranch *br_TofCal_Plane = tree1->Branch("TofCal_Plane", &TofCal_Plane); 
  TBranch *br_TofCal_Arm = tree1->Branch("TofCal_Arm", &TofCal_Arm); 
  TBranch *br_TofCal_GlBar = tree1->Branch("TofCal_GlBar", &TofCal_GlBar); 
  TBranch *br_TofCal_Time0 = tree1->Branch("TofCal_Time0", &TofCal_Time0); 
 	TBranch *br_TofCal_Time1 = tree1->Branch("TofCal_Time1", &TofCal_Time1); 
  TBranch *br_TofCal_Amp0 = tree1->Branch("TofCal_Amp0", &TofCal_Amp0); 
 	TBranch *br_TofCal_Amp1 = tree1->Branch("TofCal_Amp1", &TofCal_Amp1); 
  TBranch *br_TofCal_X = tree1->Branch("TofCal_X", &TofCal_X); 
 	TBranch *br_TofCal_Y = tree1->Branch("TofCal_Y", &TofCal_Y); // 

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



 


  //filename = "/zfs/scratch/lhep-src/reconstruction_11_24/UpSt/dec25_G/Run_"+to_string(filenum)+"_Upstr.root"; // input
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiAligMWPCTracks_4780_4799_Chi2_5_Weight_Fit_0_tracks.root";
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiMwpcAligDchCoord4780_4799_BMNS_wo_gap_wo_rot_WithDCH_MWPC_Zinv.root";
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiMwpcAligDchCoord4780_4799_BMNS_wo_gap_wo_rot_WithDCH_MWPC_Zinv_wo_ampcut.root";
  //filename = "/home/srcuser3/bmnroot_upstream_track/bmnroot/macro/run/run8/preco/UpstreamTracks/SiMwpcAligDchCoord4780_4799_BMNS_wo_gap_wo_rot_WithDCH_MWPC_Zinv_ampcut_1700.root";
  
  
  TFile *hfile = new TFile(infilename);
  //		if (!hfile->IsOpen())  continue;
  //		if (!hfile->Get("bmndata")) 		continue;
        
  TTree *tree = (TTree*)hfile->Get("tree1");

  tree->SetBranchAddress("event",&event);
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
  tree->SetBranchAddress("TofCal_Arm", &TofCal_Arm);
  tree->SetBranchAddress("TofCal_GlBar", &TofCal_GlBar);
  tree->SetBranchAddress("TofCal_Plane", &TofCal_Plane);
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


  //Track *MWPC1 = new Track;
  tree->SetBranchAddress("MWPC1_X",&MWPC1->X);
  tree->SetBranchAddress("MWPC1_Y",&MWPC1->Y);
  tree->SetBranchAddress("MWPC1_Z",&MWPC1->Z);
  tree->SetBranchAddress("MWPC1_Tx",&MWPC1->Tx);
  tree->SetBranchAddress("MWPC1_Ty",&MWPC1->Ty);
  tree->SetBranchAddress("MWPC1_Chi2",&MWPC1->Chi2X);
  tree->SetBranchAddress("MWPC1_nTracks",&MWPC1->nTracks);

  //Track *MWPC2 = new Track;
  tree->SetBranchAddress("MWPC2_X",&MWPC2->X);
  tree->SetBranchAddress("MWPC2_Y",&MWPC2->Y);
  tree->SetBranchAddress("MWPC2_Z",&MWPC2->Z);
  tree->SetBranchAddress("MWPC2_Tx",&MWPC2->Tx);
  tree->SetBranchAddress("MWPC2_Ty",&MWPC2->Ty);
  tree->SetBranchAddress("MWPC2_Chi2",&MWPC2->Chi2X);
  tree->SetBranchAddress("MWPC2_nTracks",&MWPC2->nTracks);

  //Track *MWPC3 = new Track;
  tree->SetBranchAddress("MWPC3F_X",&MWPC3->X);
  tree->SetBranchAddress("MWPC3F_Y",&MWPC3->Y);
  tree->SetBranchAddress("MWPC3F_Z",&MWPC3->Z);
  tree->SetBranchAddress("MWPC3F_Tx",&MWPC3->Tx);
  tree->SetBranchAddress("MWPC3F_Ty",&MWPC3->Ty);
  tree->SetBranchAddress("MWPC3F_Chi2",&MWPC3->Chi2X);
  tree->SetBranchAddress("MWPC3F_nTracks",&MWPC3->nTracks);

  //Track *MWPC4 = new Track;
  tree->SetBranchAddress("MWPC4F_X",&MWPC4->X);
  tree->SetBranchAddress("MWPC4F_Y",&MWPC4->Y);
  tree->SetBranchAddress("MWPC4F_Z",&MWPC4->Z);
  tree->SetBranchAddress("MWPC4F_Tx",&MWPC4->Tx);
  tree->SetBranchAddress("MWPC4F_Ty",&MWPC4->Ty);
  tree->SetBranchAddress("MWPC4F_Chi2",&MWPC4->Chi2X);
  tree->SetBranchAddress("MWPC4F_nTracks",&MWPC4->nTracks);

  //Track *MWPC_p1 = new Track;
  tree->SetBranchAddress("MWPC_p1_X",&MWPC_p1->X);
  tree->SetBranchAddress("MWPC_p1_Y",&MWPC_p1->Y);
  tree->SetBranchAddress("MWPC_p1_Z",&MWPC_p1->Z);
  tree->SetBranchAddress("MWPC_p1_Tx",&MWPC_p1->Tx);
  tree->SetBranchAddress("MWPC_p1_Ty",&MWPC_p1->Ty);
  tree->SetBranchAddress("MWPC_p1_Chi2",&MWPC_p1->Chi2X);
  tree->SetBranchAddress("MWPC_p1_nTracks",&MWPC_p1->nTracks);

  //Track *MWPC_p0 = new Track;
  tree->SetBranchAddress("MWPC_p0_X",&MWPC_p0->X);
  tree->SetBranchAddress("MWPC_p0_Y",&MWPC_p0->Y);
  tree->SetBranchAddress("MWPC_p0_Z",&MWPC_p0->Z);
  tree->SetBranchAddress("MWPC_p0_Tx",&MWPC_p0->Tx);
  tree->SetBranchAddress("MWPC_p0_Ty",&MWPC_p0->Ty);
  tree->SetBranchAddress("MWPC_p0_Chi2",&MWPC_p0->Chi2X);
  tree->SetBranchAddress("MWPC_p0_nTracks",&MWPC_p0->nTracks);

  tree->SetBranchAddress("DCH_X",&DCH->X);
  tree->SetBranchAddress("DCH_Y",&DCH->Y);
  tree->SetBranchAddress("DCH_Z",&DCH->Z);
  tree->SetBranchAddress("DCH_Tx",&DCH->Tx);
  tree->SetBranchAddress("DCH_Ty",&DCH->Ty);
 // tree->SetBranchAddress("DCH_Chi2",&DCH->Chi2X);
  tree->SetBranchAddress("DCH_Ntr",&DCH->nTracks);

  tree->SetBranchAddress("DCH1_X",&DCH1->X);
  tree->SetBranchAddress("DCH1_Y",&DCH1->Y);
  tree->SetBranchAddress("DCH1_Z",&DCH1->Z);
  tree->SetBranchAddress("DCH1_Tx",&DCH1->Tx);
  tree->SetBranchAddress("DCH1_Ty",&DCH1->Ty);
  //tree->SetBranchAddress("DCH1_Chi2",&DCH1->Chi2X);
  tree->SetBranchAddress("DCH1_Ntr",&DCH1->nTracks);

  tree->SetBranchAddress("DCH2_X",&DCH2->X);
  tree->SetBranchAddress("DCH2_Y",&DCH2->Y);
  tree->SetBranchAddress("DCH2_Z",&DCH2->Z);
  tree->SetBranchAddress("DCH2_Tx",&DCH2->Tx);
  tree->SetBranchAddress("DCH2_Ty",&DCH2->Ty);
  //tree->SetBranchAddress("DCH2_Chi2",&DCH2->Chi2X);
  tree->SetBranchAddress("DCH2_Ntr",&DCH2->nTracks);

    //Inf *Sil1H = new Inf;
 	tree->SetBranchAddress("Si1HF_X",&Sil1H->X);
  tree->SetBranchAddress("Si1HF_Y",&Sil1H->Y);
  tree->SetBranchAddress("Si1HF_St",&Sil1H->St);
  tree->SetBranchAddress("Si1HF_Mod",&Sil1H->Mod);
  tree->SetBranchAddress("Si1HF_L0NHits",&Sil1H->L0H);
  tree->SetBranchAddress("Si1HF_L1NHits",&Sil1H->L1H);
  tree->SetBranchAddress("Si1HF_AmpDir",&Sil1H->AmpDir);
  tree->SetBranchAddress("Si1HF_AmpIncl",&Sil1H->AmpIncl);
  tree->SetBranchAddress("Si1HF_nHits",&Sil1H->nHits);

  //Inf *Sil2H = new Inf;
 	tree->SetBranchAddress("Si2HF_X",&Sil2H->X);
  tree->SetBranchAddress("Si2HF_Y",&Sil2H->Y);
  tree->SetBranchAddress("Si2HF_St",&Sil2H->St);
  tree->SetBranchAddress("Si2HF_Mod",&Sil2H->Mod);
  tree->SetBranchAddress("Si2HF_L0NHits",&Sil2H->L0H);
  tree->SetBranchAddress("Si2HF_L1NHits",&Sil2H->L1H);
  tree->SetBranchAddress("Si2HF_AmpDir",&Sil2H->AmpDir);
  tree->SetBranchAddress("Si2HF_AmpIncl",&Sil2H->AmpIncl);
  tree->SetBranchAddress("Si2HF_nHits",&Sil2H->nHits);

  //Inf *Sil3H = new Inf;
 	tree->SetBranchAddress("Si3HF_X",&Sil3H->X);
  tree->SetBranchAddress("Si3HF_Y",&Sil3H->Y);
  tree->SetBranchAddress("Si3HF_St",&Sil3H->St);
  tree->SetBranchAddress("Si3HF_Mod",&Sil3H->Mod);
  tree->SetBranchAddress("Si3HF_L0NHits",&Sil3H->L0H);
  tree->SetBranchAddress("Si3HF_L1NHits",&Sil3H->L1H);
  tree->SetBranchAddress("Si3HF_AmpDir",&Sil3H->AmpDir);
  tree->SetBranchAddress("Si3HF_AmpIncl",&Sil3H->AmpIncl);
  tree->SetBranchAddress("Si3HF_nHits",&Sil3H->nHits);

  //Inf *Sil4H = new Inf;
 	tree->SetBranchAddress("Si4HF_X",&Sil4H->X);
  tree->SetBranchAddress("Si4HF_Y",&Sil4H->Y);
  tree->SetBranchAddress("Si4HF_St",&Sil4H->St);
  tree->SetBranchAddress("Si4HF_Mod",&Sil4H->Mod);
  tree->SetBranchAddress("Si4HF_L0NHits",&Sil4H->L0H);
  tree->SetBranchAddress("Si4HF_L1NHits",&Sil4H->L1H);
  tree->SetBranchAddress("Si4HF_AmpDir",&Sil4H->AmpDir);
  tree->SetBranchAddress("Si4HF_AmpIncl",&Sil4H->AmpIncl);
  tree->SetBranchAddress("Si4HF_nHits",&Sil4H->nHits);




  int NEv = tree->GetEntries();
  int N_non0_DCH_Upst=0;
  int N_non0_Pq=0;
  int N_double_Pq=0;
  int N_double_Pq_ev=0;
  cout<<NEv<<endl; 
  int counter=0;
  for (int ev=0; ev<NEv; ev++) {
    tree->GetEntry(ev);
    
    // if (chargein!=6) continue;
    // if (charge35!=6) continue;
    counter++;
    if (DCH->nTracks!=0 && Sil1234->nTracks!=0) N_non0_DCH_Upst++;
    double edata[10], txdata[9];
   
    int multi_use[100][100]={0};
    for (int i=0; i<DCH->nTracks; i++){
            for (int j=0; j<Sil1234->nTracks; j++){
               //if (i>j) continue;
               
	            

               
               

               UInt_t k=0;	
               txdata[k]	= Sil1234->X->at(j)+X0_offset;
            	 txdata[++k]	= Sil1234->Y->at(j);
            	 txdata[++k]	= Sil1234->Z->at(j);
            	 txdata[++k]	= Sil1234->Ty->at(j);
            
            	 txdata[++k]	= DCH->X->at(i);
            	 txdata[++k]	= DCH->Y->at(i);
            	 txdata[++k]	= DCH->Z->at(i);
            	 txdata[++k]	= DCH->Tx->at(i);
            	 txdata[++k]	= DCH->Ty->at(i);
               
               if (abs(MDF_TX(txdata)-Sil1234->Tx->at(j))>0.004) continue;
               if (abs(DCH->Ty->at(i)-Sil1234->Ty->at(j))>0.0044) continue;

              Tx_MDF_UpSt.push_back(MDF_TX(txdata));

              
              k=0;	
               
               
               
//               double Ztarg=-647.5;
//               double Xtarg=MWPC_X->at(j)+MWPC_Tx->at(j)*(Ztarg-MWPC_Z->at(j));
//               double Ytarg=MWPC_Y->at(j)+MWPC_Ty->at(j)*(Ztarg-MWPC_Z->at(j));
               //pos_vec.SetXYZ(Xtarg, Ytarg, Ztarg);
               
             	 edata[k]	= Sil1234->X->at(j)+X0_offset;
            	 edata[++k]	= Sil1234->Y->at(j);
            	 edata[++k]	= Sil1234->Z->at(j);
//               edata[k]	= Xtarg;
//           	 edata[++k]	= Ytarg;
//            	 edata[++k]	= Ztarg;
            	 edata[++k]	= Sil1234->Tx->at(j)+TX0_offset;
            	 edata[++k]	= Sil1234->Ty->at(j);
            
            	 edata[++k]	= DCH->X->at(i);
            	 edata[++k]	= DCH->Y->at(i);
            	 edata[++k]	= DCH->Z->at(i);
            	 edata[++k]	= DCH->Tx->at(i);
            	 edata[++k]	= DCH->Ty->at(i);  

               Pq_MDF_UpSt.push_back(MDF(edata));
               Pq_MDF_X_UpSt.push_back(Sil1234->Tx->at(j)*MDF(edata)/sqrt(Sil1234->Tx->at(j)*Sil1234->Tx->at(j)+Sil1234->Ty->at(j)*Sil1234->Ty->at(j)+1));
               Pq_MDF_Y_UpSt.push_back(Sil1234->Ty->at(j)*MDF(edata)/sqrt(Sil1234->Tx->at(j)*Sil1234->Tx->at(j)+Sil1234->Ty->at(j)*Sil1234->Ty->at(j)+1));
               Pq_MDF_Z_UpSt.push_back(MDF(edata)/sqrt(Sil1234->Tx->at(j)*Sil1234->Tx->at(j)+Sil1234->Ty->at(j)*Sil1234->Ty->at(j)+1));
               Tx_AT.push_back(Sil1234->Tx->at(j));
               Ty_AT.push_back(Sil1234->Ty->at(j));
               Pq_MDF_Flag.push_back(i*1000000+Sil1234->IDX->at(j)*1000+Sil1234->IDY->at(j));
               Fragment.push_back(100);
               if (chargein==6 && charge35==6) {
                if (MDF(edata)>C12_Range[0] && MDF(edata)<C12_Range[1]) Fragment.at(Fragment.size()-1)=612;
                if (MDF(edata)>C11_Range[0] && MDF(edata)<C11_Range[1]) Fragment.at(Fragment.size()-1)=611;
                if (MDF(edata)>C10_Range[0] && MDF(edata)<C10_Range[1]) Fragment.at(Fragment.size()-1)=610;
               }

               if (chargein==6 && charge35==5) {
                if (MDF(edata)>B11_Range[0] && MDF(edata)<B11_Range[1]) Fragment.at(Fragment.size()-1)=511;
                if (MDF(edata)>B10_Range[0] && MDF(edata)<B10_Range[1]) Fragment.at(Fragment.size()-1)=510;
               }

               if (chargein==6 && charge35==4) {
                if (MDF(edata)>Be10_Range[0] && MDF(edata)<Be10_Range[1]) Fragment.at(Fragment.size()-1)=410;
                if (MDF(edata)>Be9_Range[0] && MDF(edata)<Be9_Range[1]) Fragment.at(Fragment.size()-1)=409;
                if (MDF(edata)>Be7_Range[0] && MDF(edata)<Be7_Range[1]) Fragment.at(Fragment.size()-1)=407;
               }

               if (chargein==6 && charge35==3) {
                if (MDF(edata)>Li9_Range[0] && MDF(edata)<Li9_Range[1]) Fragment.at(Fragment.size()-1)=309;
                if (MDF(edata)>Li8_Range[0] && MDF(edata)<Li8_Range[1]) Fragment.at(Fragment.size()-1)=308;
                if (MDF(edata)>Li7_Range[0] && MDF(edata)<Li7_Range[1]) Fragment.at(Fragment.size()-1)=307;
              
                if (MDF(edata)>Li6_Range[0] && MDF(edata)<Li6_Range[1]) Fragment.at(Fragment.size()-1)=306;
               }

               multi_use[i][j]++;
            }
    }

    if (Pq_MDF_UpSt.size()!=0) N_non0_Pq++; 

    int was_double=0;
    for (int i=0; i<DCH->nTracks; i++){
      int UpStSum=0;
      for (int j=0; j<Sil1234->nTracks; j++){
        UpStSum+=multi_use[i][j];
        if(UpStSum>1) N_double_Pq++;
        if(UpStSum>1 && was_double==0) {
          N_double_Pq_ev++;
          was_double=1;
        }
      }
    }

    for (int i=0; i<Sil1234->nTracks; i++){
      int DCHSum=0;
      for (int j=0; j<DCH->nTracks; j++){
        DCHSum+=multi_use[j][i];
        if(DCHSum>1) N_double_Pq++;
        if(DCHSum>1 && was_double==0) {
          N_double_Pq_ev++;
          was_double=1;
        }
      }
    }

    if (was_double==1) {
      Tx_AT.clear();
      Ty_AT.clear();
      Pq_MDF_UpSt.clear();
      Pq_MDF_X_UpSt.clear();
      Pq_MDF_Y_UpSt.clear();
      Pq_MDF_Z_UpSt.clear();
      Pq_MDF_Flag.clear();
      Tx_MDF_UpSt.clear();
      Fragment.clear();
    }

    tree1->Fill();

    Pq_MDF_UpSt.clear();
    Pq_MDF_X_UpSt.clear();
    Pq_MDF_Y_UpSt.clear();
    Pq_MDF_Z_UpSt.clear();
    Tx_AT.clear();
    Ty_AT.clear();
    Pq_MDF_Flag.clear();
    Tx_MDF_UpSt.clear();
    Fragment.clear();

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

      TofCal_Bar->clear(); //new 
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

    Sil1234->X->clear();
    Sil1234->Y->clear();
    Sil1234->Z->clear();
    Sil1234->Tx->clear();
    Sil1234->Ty->clear();
    Sil1234->Chi2X->clear();
    Sil1234->Chi2Y->clear();
    Sil1234->IDX->clear();
    Sil1234->IDY->clear();
    // Sil1234->DX12->clear();
    // Sil1234->DY12->clear();
    // Sil1234->DX34->clear();
    // Sil1234->DY34->clear();
    // Sil1234->Flag->clear();


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
    
   
        

   

  } 
  //cout<<NEv<<"  "<<N_non0_DCH_Upst<<" "<<N_non0_Pq<<" "<<N_double_Pq<<" "<<N_double_Pq_ev<<endl;
  treefile->cd();
	//tree1->SetDirectory(treefile);
	tree1->Write();
	treefile->Close();
	
}

