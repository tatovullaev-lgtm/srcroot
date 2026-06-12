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


const Double_t toff_fix = -2.448 + 1002.5; //ns; 986.54;
const Double_t toff_fixL = -2.448 + 1002.5 + 10.21; //left arm additional offset//ns; 986.54;
const Double_t beam_toff = 3.452 + 3.0; //ns; 3.5 GeV/c/u from T0 to center target ~100cm;


void Goran_Tracks_Allignment(TString infilename, TString outfilename, TString badfileslist) {

	printf("start read");
  
  double bar_toffR[15] = {
		1224.57 - 1224.57,
		1223.51 - 1224.57,
		1224.1 - 1224.57,
		1225.64 - 1224.57,
		1223.5 - 1224.57,
		1224.51 - 1224.57, 
		1223.76 - 1224.57,
		1226.14 - 1224.57,
		1225.59 - 1224.57,
		1224.82 - 1224.57,
		1224.68 - 1224.57,
		1224.16 - 1224.57,
		1224.97 - 1224.57,
		1223.72 - 1224.57,
		1226.06 - 1224.57
	};

	double bar_toffL[15] = {
		1234.63 - 1234.63,
		1235.94 - 1234.63,
		1235.39 - 1234.63,
		1236.23 - 1234.63,
		1235.47 - 1234.63,
		1235.93 - 1234.63,
		1234.11 - 1234.63,
		1235.65 - 1234.63,
		1235.35 - 1234.63,
		1236.43 - 1234.63,
		1236.13 - 1234.63,
		1235.9 - 1234.63,
		1236.03 - 1234.63,
		1235.63 - 1234.63,
		1235.22 - 1234.63
	};


   double degtorad = 0.01745329;
  double targetZ = -576.2; // in July version no shift + 0.6;   //#Misha used targetZ = -575.6 for checking TOF/m. difference of 6 mm    #targetZ = -576.8

  double globalArmXShift = 1.46548e+00 - 9.15031e-01 - 1.51759e-01;
  double globalArmYShift = -7.77338e-02 + 7.69002e-02 + 1.53860e-01;
  double globalArmZShift = 0; //#-4.14235e-01;

  double gemLeftArmLength = 113.033;
  double gemRightArmLength = 111.832;
  double gemLeftArmAngle = 30.53;
  double gemRightArmAngle = -30.87;

  double cscLeftArmLength = 186.415 + 1.25 + 0.38; // + 1.25 + 0.38cscArmZCorrection
  double cscRightArmLength = 186.636 + 1.25 + 0.38;
  double cscLeftArmAngle = 30.67;
  double cscRightArmAngle = -30.77;



  TVector3 vecGemLeftCenter(0, 0, gemLeftArmLength);
  TVector3 vecGemRightCenter(0, 0, gemRightArmLength);
  TVector3 vecCscLeftCenter(0, 0, cscLeftArmLength);
  TVector3 vecCscRightCenter(0, 0, cscRightArmLength);
  TVector3 vecTargetCenter(0, 0, targetZ);


  bool applyLaserALignment = true;
  double align_left_gem_y0=0, align_right_gem_y0=0, align_left_csc_y0=0, align_right_csc_y0=0;
  double gemLeftXAngle=0, gemLeftYAngle=0, gemLeftZAngle=0, gemRightXAngle=0, gemRightYAngle=0, gemRightZAngle=0; 
  double cscLeftXAngle=0, cscLeftYAngle=0, cscLeftZAngle=0, cscRightXAngle=0, cscRightYAngle=0, cscRightZAngle=0; 
  if(applyLaserALignment) {
    align_left_gem_y0 = -0.056; //from laser measurement
    align_right_gem_y0 = 0.003;
    align_left_csc_y0 = -0.127;
    align_right_csc_y0 = -0.056;
  
    gemLeftXAngle = 0.09; //-0.0435
    gemLeftYAngle = 0.44; //# - changed for vertex Z double peak (cancelled)
    gemLeftZAngle = -0.23;

    gemRightXAngle = 0.19;
    gemRightYAngle = -0.4; //#-0.48
    gemRightZAngle = 0.0;

    cscLeftXAngle = 0.0913;
    cscLeftYAngle = -0.24; //# - changed for vertex Z double peak (cancelled)
    cscLeftZAngle = 0.0;

    cscRightXAngle = 0.89;
    cscRightYAngle = -0.23;
    cscRightZAngle = -0.215;
  }


  double innerAlignLeftGemZX = 4.71899e-01 + 6.65723e-02 + 1.42250e-01;
  double innerAlignLeftGemY = -4.74045e-01 -3.04214e-03 - 5.82438e-02;
  double innerAlignLeftCscZX = -1.98832e-01 + 1.43292e-01 + 2.23026e-01;
  double innerAlignLeftCscY = -6.65933e-01 -1.07423e-01 - 2.91908e-02;
  double innerAlignRightGemZX = -5.10908e-01 -1.87645e-01 - 3.01557e-02;
  double innerAlignRightGemY = -5.90914e-01 + 3.04933e-03 - 1.13033e-01;
  double innerAlignRightCscZX = 4.26027e-02 -1.26240e-01 + 1.52801e-01;
  double innerAlignRightCscY = -1.01546e+00 + 8.99139e-02 - 5.56919e-02;

 
  double RighArmAlignZXAngle = 0;
  double LeftRighAlignZXAngle = 0.2;
  double LeftRighAlignYAngle = 0;

  double sin_lrzx = sin(0.2*degtorad);
  double cos_lrzx = cos(0.2*degtorad);


  // #generalTimeCorrection = 5.0 / 30.0
  // #generalTimeCorrection = -0.05#add 50 ps for trying to fix Px issue in arms
  double generalTimeCorrection = 0.2; //#add substract 100 ps to fix mmiss2 shfift (and TOF mishmatch with simulation)

  //TString gPathFullBC = "/zfs/scratch/lhep-src/bmnroot/macro/run/Reconstruction_12_25_G/BadTriggerList.txt";
  ifstream fin1(badfileslist.Data());
  int runZ=0;
 	string unused;
  vector <double> runisbad;  
  while (getline(fin1, unused)){    
      fin1>>runZ;
      runisbad.push_back(runZ);
          
  }
  fin1.close();


  //filenum=5;

  //TString filename ="/zfs/scratch/lhep-src/reconstruction_11_24/ArmTr_G/dec25_G/Run_" + to_string(filenum) + "_GTrack_Alig.root";
	TFile *treefile = new TFile(outfilename, "recreate");
	//output tree and its branches
	TTree *tree1 = new TTree("tree1","treelibrated tree");
	//tree1->SetDirectory(treefile);
  int event, run, isGoodRun;
  vector <double> *Pq_MDF_Flag=nullptr, *Pq_MDF_UpSt=nullptr, *Pq_MDF_X_UpSt=nullptr, *Pq_MDF_Y_UpSt=nullptr, *Pq_MDF_Z_UpSt=nullptr, *Tx_MDF_UpSt=nullptr, *Tx_AT=nullptr, *Ty_AT=nullptr;
  vector <int> *Fragment=nullptr;
  int Pq_MDF_UpSt_nTracks;
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
  vector <Int_t> *TofCal_Layer=nullptr, *TofCal_Bar=nullptr, *TofCal_Arm=nullptr, *TofCal_GlBar=nullptr; // new
  Double_t BC1_Time0, BC1_Time1, BC2_Time0, BC2_Time1;  // new  

  Short_t rightGemN, rightCscN, leftGemN, leftCscN;
  vector <Float_t> *rightGemX=nullptr, *rightGemY=nullptr, *rightGemZ=nullptr, *leftGemX=nullptr, *leftGemY=nullptr, *leftGemZ=nullptr,
  *rightCscX=nullptr, *rightCscY=nullptr, *rightCscZ=nullptr, *leftCscX=nullptr, *leftCscY=nullptr, *leftCscZ=nullptr, 
  *rightGemXLocal=nullptr, *rightGemYLocal=nullptr, *leftGemXLocal=nullptr, *leftGemYLocal=nullptr, 
  *rightCscXLocal=nullptr, *rightCscYLocal=nullptr, *leftCscXLocal=nullptr, *leftCscYLocal=nullptr;

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
  TBranch *br_Pq_MDF_UpSt_nTracks =tree1->Branch("Pq_MDF_UpSt_nTracks",&Pq_MDF_UpSt_nTracks);
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

  TBranch *br_Is_laser = tree1->Branch("Is_laser", &Is_laser); 
  TBranch *br_BC1_Time0 = tree1->Branch("BC1_Time0", &BC1_Time0); 
  TBranch *br_BC1_Time1 = tree1->Branch("BC1_Time1", &BC1_Time1); 
  TBranch *br_BC2_Time0 = tree1->Branch("BC2_Time0", &BC2_Time0); 
  TBranch *br_BC2_Time1 = tree1->Branch("BC2_Time1", &BC2_Time1); 


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



 


  //filename = "/zfs/scratch/lhep-src/reconstruction_11_24/MDF/dec25_G/Run_"+to_string(filenum)+"_MDF.root"; // input
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

  tree->SetBranchAddress("Pq_MDF_UpSt",&Pq_MDF_UpSt);
  tree->SetBranchAddress("Pq_MDF_X_UpSt",&Pq_MDF_X_UpSt);
  tree->SetBranchAddress("Pq_MDF_Y_UpSt",&Pq_MDF_Y_UpSt);
  tree->SetBranchAddress("Pq_MDF_Z_UpSt",&Pq_MDF_Z_UpSt);
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
  tree->SetBranchAddress("TofCal_Plane", &TofCal_Layer);
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
  tree->SetBranchAddress("MWPC3_X",&MWPC3->X);
  tree->SetBranchAddress("MWPC3_Y",&MWPC3->Y);
  tree->SetBranchAddress("MWPC3_Z",&MWPC3->Z);
  tree->SetBranchAddress("MWPC3_Tx",&MWPC3->Tx);
  tree->SetBranchAddress("MWPC3_Ty",&MWPC3->Ty);
  tree->SetBranchAddress("MWPC3_Chi2",&MWPC3->Chi2X);
  tree->SetBranchAddress("MWPC3_nTracks",&MWPC3->nTracks);

  //Track *MWPC4 = new Track;
  tree->SetBranchAddress("MWPC4_X",&MWPC4->X);
  tree->SetBranchAddress("MWPC4_Y",&MWPC4->Y);
  tree->SetBranchAddress("MWPC4_Z",&MWPC4->Z);
  tree->SetBranchAddress("MWPC4_Tx",&MWPC4->Tx);
  tree->SetBranchAddress("MWPC4_Ty",&MWPC4->Ty);
  tree->SetBranchAddress("MWPC4_Chi2",&MWPC4->Chi2X);
  tree->SetBranchAddress("MWPC4_nTracks",&MWPC4->nTracks);

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




  int NEv = tree->GetEntries();
  int N_non0_DCH_Upst=0;
  int N_non0_Pq=0;
  int N_double_Pq=0;
  int N_double_Pq_ev=0;
  cout<<NEv<<endl; 
  int counter=0;
  for (int ev=0; ev<NEv; ev++) {
    tree->GetEntry(ev);


    Pq_MDF_UpSt_nTracks=Pq_MDF_UpSt->size();


    isGoodRun=1;
    for (int i=0; i<runisbad.size(); i++){
      if (isGoodRun==0) continue;
      if (runisbad.at(i)==run) isGoodRun=0;
    }

    if (run<3500) isGoodRun=0;
    if (run>5000) isGoodRun=0;

    bool leftGemHotZone = false;
    bool leftGemAboveHotZone = false;

    //prepare branches + pre-allignment for TofCal
    for (int i=0; i<Tof400_nHits; i++) {
      if (Tof400_X->at(i)>0) {
        Tof400L_X->push_back(Tof400_X->at(i));
        Tof400L_Y->push_back(Tof400_Y->at(i));
        Tof400L_Z->push_back(Tof400_Z->at(i));
        Tof400L_DetId->push_back(Tof400_DetId->at(i));
        Tof400L_T->push_back(Tof400_TS->at(i));
      }
      if (Tof400_X->at(i)<0) {
        Tof400R_X->push_back(Tof400_X->at(i));
        Tof400R_Y->push_back(Tof400_Y->at(i));
        Tof400R_Z->push_back(Tof400_Z->at(i));
        Tof400R_DetId->push_back(Tof400_DetId->at(i));
        Tof400R_T->push_back(Tof400_TS->at(i));
      }
    }
    Tof400L_nHits=Tof400L_X->size();
    Tof400R_nHits=Tof400R_X->size();

    double TofCalT0=-10000.;
    for (int i=0; i<TofCal_nHits; i++) {
      if (TofCal_GlBar->at(i)!=120) continue;
      TofCalT0=TofCal_Time0->at(i);
    }

    for (int i=0; i<TofCal_nHits; i++) {
      if (TofCal_GlBar->at(i)==120) continue;
      if (TofCal_Arm->at(i)==1) {
        if (TofCal_Layer->at(i)!=0) continue;
        TofCalL_X->push_back(361.414*sin(30.78*0.01745329)-(10.0*(7.0-TofCal_Bar->at(i)))*cos(30.78*0.01745329));
        // TofCalLeftX[nTofCalLeft] = tofcalLeftArmLength * sin(tofcalLeftArmAngle * degtorad) - local_leftTofCalX * cos(tofcalLeftArmAngle * degtorad);
        TofCalL_Y->push_back(-TofCal_Y->at(i));
        TofCalL_Z->push_back(-576.2+(361.414+10.0*TofCal_Layer->at(i))*cos(30.78*0.01745329)+(10.0*(7.0-TofCal_Bar->at(i)))*sin(30.78*0.01745329));
        //TofCalLeftZ[nTofCalLeft] = middleZ + (tofcalLeftArmLength + 10.0 * tofcalPlane) * cos(tofcalLeftArmAngle * degtorad) + local_leftTofCalX * sin(tofcalLeftArmAngle * degtorad);
        
        // toff_fixLR = toff_fixL + bar_toffL[tofCal->GetGlobBar()-60];
				// TofCalTimeLeft[nTofCalLeft] = 0.5 * (TofCalTimeUpLeft[nTofCalLeft] + TofCalTimeDownLeft[nTofCalLeft]) - TofCalT0 /*-T0AfterWalk(4pms) */ + 1258.0 - toff_fixLR - beam_toff;
        double toff_fixLR=toff_fixL+bar_toffL[TofCal_Bar->at(i)];
        TofCalL_T->push_back(0.5*(TofCal_Time0->at(i)+TofCal_Time1->at(i))-TofCalT0+1258.0-toff_fixLR-beam_toff);
        TofCalL_Bar->push_back(TofCal_Bar->at(i));
        TofCalL_Layer->push_back(TofCal_Layer->at(i));
      }
      if (TofCal_Arm->at(i)==0) {
        if (TofCal_Layer->at(i)!=0) continue;
        TofCalR_X->push_back(358.472*sin(-30.83*0.01745329)+(10.0*(7.0-TofCal_Bar->at(i)))*cos(-30.83*0.01745329));
        TofCalR_Y->push_back(-TofCal_Y->at(i));
        TofCalR_Z->push_back(-576.2+(358.472+10.0*TofCal_Layer->at(i))*cos(-30.83*0.01745329)-(10.0*(7.0-TofCal_Bar->at(i)))*sin(-30.83*0.01745329));
        
        // toff_fixLR = toff_fix + bar_toffR[tofCal->GetGlobBar()];
				// TofCalTimeRight[nTofCalRight] = 0.5 * (TofCalTimeUpRight[nTofCalRight] + TofCalTimeDownRight[nTofCalRight]) - TofCalT0 + 1258.0 - toff_fixLR - beam_toff;
        
        double toff_fixLR=toff_fix+bar_toffR[TofCal_Bar->at(i)];
        TofCalR_T->push_back(0.5*(TofCal_Time0->at(i)+TofCal_Time1->at(i))-TofCalT0+1258.0-toff_fixLR-beam_toff);
        TofCalR_Bar->push_back(TofCal_Bar->at(i));
        TofCalR_Layer->push_back(TofCal_Layer->at(i));
      }
    }
    TofCalL_nHits=TofCalL_X->size();
    TofCalR_nHits=TofCalR_X->size();

    //allignment
    GemL_nHits=leftGemN;
    for (int i=0; i<GemL_nHits; i++) {
      TVector3 vecGemL(-leftGemXLocal->at(i), leftGemYLocal->at(i)+align_left_gem_y0, 0);
      vecGemL.RotateX(gemLeftXAngle * degtorad);
      vecGemL.RotateY(gemLeftYAngle * degtorad);
      vecGemL.RotateZ(gemLeftZAngle * degtorad);
      vecGemL += vecGemLeftCenter;
      vecGemL.RotateY(gemLeftArmAngle * degtorad);
      vecGemL += vecTargetCenter;
            
      double innerX = innerAlignLeftGemZX * cos(gemLeftArmAngle * degtorad);
      double innerZ = -innerAlignLeftGemZX * sin(gemLeftArmAngle * degtorad);
     
      if(vecGemL.X() > 40 and vecGemL.X() < 45 and vecGemL.Y() > -30 and vecGemL.Y() < -20){
        leftGemHotZone = true;
      }
      if(vecGemL.X() > 40 and vecGemL.X() < 45 and vecGemL.Y() > -15 and vecGemL.Y() < -5){
        leftGemAboveHotZone = true;
      }

      
      vecGemL.SetX(vecGemL.X() + globalArmXShift + innerX);
      vecGemL.SetY(vecGemL.Y() + globalArmYShift + innerAlignLeftGemY);
      vecGemL.SetZ(vecGemL.Z() + globalArmZShift + innerZ);

      //LR corrections 0.2 deg look wrong the order influence the result ...
      vecGemL.SetZ(vecGemL.Z() - vecTargetCenter.Z());
      vecGemL.SetZ(cos(0.2*degtorad) * vecGemL.Z() + sin(0.2*degtorad) * vecGemL.X());
      vecGemL.SetX(-sin(0.2*degtorad) * vecGemL.Z() + cos(0.2*degtorad) * vecGemL.X());
      vecGemL.SetZ(vecGemL.Z() + vecTargetCenter.Z());
      
     


      GemL_X->push_back(vecGemL.X());
      GemL_Y->push_back(vecGemL.Y());
      GemL_Z->push_back(vecGemL.Z());
      GemL_Tx->push_back((GemL_X->at(GemL_X->size()-1) - vecTargetCenter.X()) / (GemL_Z->at(GemL_Z->size()-1)  - vecTargetCenter.Z()));
      GemL_Ty->push_back((GemL_Y->at(GemL_Y->size()-1) - vecTargetCenter.Y()) / (GemL_Z->at(GemL_Z->size()-1)  - vecTargetCenter.Z()));    
    }

    GemR_nHits=rightGemN;
    for (int i=0; i<GemR_nHits; i++) {
      TVector3 vecGemR(rightGemXLocal->at(i), rightGemYLocal->at(i)+align_right_gem_y0, 0);
      vecGemR.RotateX(gemRightXAngle * degtorad);
      vecGemR.RotateY(gemRightYAngle * degtorad);
      vecGemR.RotateZ(gemRightZAngle * degtorad);
      vecGemR += vecGemRightCenter;
      vecGemR.RotateY(gemRightArmAngle * degtorad);
      vecGemR += vecTargetCenter;
            
      double innerX = innerAlignRightGemZX * cos(gemRightArmAngle * degtorad);
      double innerZ = -innerAlignRightGemZX * sin(gemRightArmAngle * degtorad);
               
      GemR_X->push_back(vecGemR.X() + globalArmXShift + innerX);
      GemR_Y->push_back(vecGemR.Y() + globalArmYShift + innerAlignRightGemY);
      GemR_Z->push_back(vecGemR.Z() + globalArmZShift + innerZ);
      GemR_Tx->push_back((GemR_X->at(GemR_X->size()-1) - vecTargetCenter.X()) / (GemR_Z->at(GemR_Z->size()-1)  - vecTargetCenter.Z()));
      GemR_Ty->push_back((GemR_Y->at(GemR_Y->size()-1) - vecTargetCenter.Y()) / (GemR_Z->at(GemR_Z->size()-1)  - vecTargetCenter.Z()));    
    }

    CscL_nHits=leftCscN;
    for (int i=0; i<CscL_nHits; i++) {
      TVector3 vecCscL(-leftCscXLocal->at(i), leftCscYLocal->at(i)+align_left_csc_y0, 0);
      vecCscL.RotateX(cscLeftXAngle * degtorad);
      vecCscL.RotateY(cscLeftYAngle * degtorad);
      vecCscL.RotateZ(cscLeftZAngle * degtorad);
      vecCscL += vecCscLeftCenter;
      vecCscL.RotateY(cscLeftArmAngle * degtorad);
      vecCscL += vecTargetCenter;
            
      double innerX = innerAlignLeftCscZX * cos(cscLeftArmAngle * degtorad);
      double innerZ = -innerAlignLeftCscZX * sin(cscLeftArmAngle * degtorad);

      vecCscL.SetX(vecCscL.X() + globalArmXShift + innerX);
      vecCscL.SetY(vecCscL.Y() + globalArmYShift + innerAlignLeftCscY);
      vecCscL.SetZ(vecCscL.Z() + globalArmZShift + innerZ);

      //LR corrections 0.2 deg look wrong the order influence the result ...
      vecCscL.SetZ(vecCscL.Z() - vecTargetCenter.Z());
      vecCscL.SetZ(cos(0.2*degtorad) * vecCscL.Z() + sin(0.2*degtorad) * vecCscL.X());
      vecCscL.SetX(-sin(0.2*degtorad) * vecCscL.Z() + cos(0.2*degtorad) * vecCscL.X());
      vecCscL.SetZ(vecCscL.Z() + vecTargetCenter.Z());
      
     


      CscL_X->push_back(vecCscL.X());
      CscL_Y->push_back(vecCscL.Y());
      CscL_Z->push_back(vecCscL.Z());
      CscL_Tx->push_back((CscL_X->at(CscL_X->size()-1) - vecTargetCenter.X()) / (CscL_Z->at(CscL_Z->size()-1)  - vecTargetCenter.Z()));
      CscL_Ty->push_back((CscL_Y->at(CscL_Y->size()-1) - vecTargetCenter.Y()) / (CscL_Z->at(CscL_Z->size()-1)  - vecTargetCenter.Z()));    
    }

    CscR_nHits=rightCscN;
    for (int i=0; i<CscR_nHits; i++) {
      TVector3 vecCscR(rightCscXLocal->at(i), rightCscYLocal->at(i)+align_right_csc_y0, 0);
      vecCscR.RotateX(cscRightXAngle * degtorad);
      vecCscR.RotateY(cscRightYAngle * degtorad);
      vecCscR.RotateZ(cscRightZAngle * degtorad);
      vecCscR += vecCscRightCenter;
      vecCscR.RotateY(cscRightArmAngle * degtorad);
      vecCscR += vecTargetCenter;
            
      double innerX = innerAlignRightCscZX * cos(cscRightArmAngle * degtorad);
      double innerZ = -innerAlignRightCscZX * sin(cscRightArmAngle * degtorad);
               
      CscR_X->push_back(vecCscR.X() + globalArmXShift + innerX);
      CscR_Y->push_back(vecCscR.Y() + globalArmYShift + innerAlignRightCscY);
      CscR_Z->push_back(vecCscR.Z() + globalArmZShift + innerZ);
      CscR_Tx->push_back((CscR_X->at(CscR_X->size()-1) - vecTargetCenter.X()) / (CscR_Z->at(CscR_Z->size()-1)  - vecTargetCenter.Z()));
      CscR_Ty->push_back((CscR_Y->at(CscR_Y->size()-1) - vecTargetCenter.Y()) / (CscR_Z->at(CscR_Z->size()-1)  - vecTargetCenter.Z()));    
    }


    for (int i=0; i<Tof400L_nHits; i++) {
      // Tof400L_X.push_back(Tof400LG_X[i] + globalArmXShift);
      // Tof400L_Y.push_back(Tof400LG_Y[i] + globalArmYShift);     
      // Tof400L_Z.push_back(Tof400LG_Z[i] + globalArmZShift);
      float Tof400L_X0=Tof400L_X->at(i);
      float Tof400L_Y0=Tof400L_Y->at(i);
      float Tof400L_Z0=Tof400L_Z->at(i);

      Tof400L_X->at(i) += globalArmXShift;
      Tof400L_Y->at(i) += globalArmYShift;
      Tof400L_Z->at(i) += globalArmZShift;

      //LR corrections 0.2 deg look wrong the order influence the result ...     
      Tof400L_Z->at(i) -= vecTargetCenter.Z();
      Tof400L_Z->at(i) = cos(0.2*degtorad) * Tof400L_Z->at(i) + sin(0.2*degtorad) * Tof400L_X->at(i);
      Tof400L_X->at(i) = -sin(0.2*degtorad) * Tof400L_Z->at(i) + cos(0.2*degtorad) * Tof400L_X->at(i);
      Tof400L_Z->at(i) += vecTargetCenter.Z();


      
      
      Tof400L_Tx->push_back((Tof400L_X->at(i) - vecTargetCenter.X()) / (Tof400L_Z->at(i)  - vecTargetCenter.Z()));
      Tof400L_Ty->push_back((Tof400L_Y->at(i) - vecTargetCenter.Y()) / (Tof400L_Z->at(i)  - vecTargetCenter.Z()));

      Tof400L_X->at(i)=Tof400L_X0; //some magic happened in Goran's code
      Tof400L_Y->at(i)=Tof400L_Y0;     
      Tof400L_Z->at(i)=Tof400L_Z0;


    }

    for (int i=0; i<Tof400R_nHits; i++) {
      // Tof400R_X.push_back(Tof400RG_X[i] + globalArmXShift);
      // Tof400R_Y.push_back(Tof400RG_Y[i] + globalArmYShift);
      // Tof400R_Z.push_back(Tof400RG_Z[i] + globalArmZShift);

      Tof400R_Tx->push_back((Tof400R_X->at(i) + globalArmXShift - vecTargetCenter.X()) / (Tof400R_Z->at(i)  - vecTargetCenter.Z()));
      Tof400R_Ty->push_back((Tof400R_Y->at(i) + globalArmYShift - vecTargetCenter.Y()) / (Tof400R_Z->at(i)  - vecTargetCenter.Z()));


    }

    for (int i=0; i<TofCalL_nHits; i++) {
      if (TofCalL_Layer->at(i)!=0) continue;         
      TofCalL_Tx->push_back((TofCalL_X->at(i) - vecTargetCenter.X()) / (TofCalL_Z->at(i) - vecTargetCenter.Z()));
      TofCalL_Ty->push_back((TofCalL_Y->at(i) - vecTargetCenter.Y()) / (TofCalL_Z->at(i) - vecTargetCenter.Z()));
      
    }

    for (int i=0; i<TofCalR_nHits; i++) {
      if (TofCalR_Layer->at(i)!=0) continue;
      
      TofCalR_Tx->push_back((TofCalR_X->at(i) - vecTargetCenter.X()) / (TofCalR_Z->at(i) - vecTargetCenter.Z()));
      TofCalR_Ty->push_back((TofCalR_Y->at(i) - vecTargetCenter.Y()) / (TofCalR_Z->at(i) - vecTargetCenter.Z()));

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

    Tof400_X->clear();
    Tof400_Y->clear();
    Tof400_Z->clear();
    Tof400_TS->clear();
    Tof400_DetId->clear();
    Tof400_Module->clear();//
    Tof400_Station->clear();//

    TofCal_Bar->clear(); //new 
    TofCal_Layer->clear();
    TofCal_Arm->clear(); 
    TofCal_GlBar->clear(); 
    TofCal_Time0->clear();
    TofCal_Time1->clear();
    TofCal_Amp0->clear();
    TofCal_Amp1->clear();
    TofCal_X->clear();
    TofCal_Y->clear();

    rightGemX->clear(); 
    rightGemY->clear(); 
    rightGemZ->clear();
    leftGemX->clear(); 
    leftGemY->clear(); 
    leftGemZ->clear();
    rightCscX->clear(); 
    rightCscY->clear(); 
    rightCscZ->clear();
    leftCscX->clear(); 
    leftCscY->clear(); 
    leftCscZ->clear();  
    rightGemXLocal->clear();
    rightGemYLocal->clear();
    leftGemXLocal->clear();
    leftGemYLocal->clear();
    rightCscXLocal->clear();
    rightCscYLocal->clear();
    leftCscXLocal->clear();
    leftCscYLocal->clear(); 

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

   

  } 
  //cout<<NEv<<"  "<<N_non0_DCH_Upst<<" "<<N_non0_Pq<<" "<<N_double_Pq<<" "<<N_double_Pq_ev<<endl;
  treefile->cd();
	//tree1->SetDirectory(treefile);
	tree1->Write();
	treefile->Close();
	
}

