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



using namespace ROOT::Math;

struct Inf {  

  vector <Double_t> X; 
  vector <Double_t> Y;
  vector <Int_t> St;
  vector <Int_t> Mod; 
  vector <Int_t> L0H; 
  vector <Int_t> L1H; 
  vector <Double_t> AmpDir; 
  vector <Double_t> AmpIncl; 
  int nHits;
  
};

struct Track {  

  vector <Double_t> X; 
  vector <Double_t> Y;
  vector <Double_t> Z;
  vector <Double_t> Tx; 
  vector <Double_t> Ty;
  //vector <bool> IsGlDCH;
  int nTracks;
  //int nGlobDCHTracks;
  vector <int> nHits;
  vector <double> Chi2;
};



struct Notes {
  int ev; 
  int st;
  int mod; 
  double X;
  double Y;
  int L0NHits;
  int L1NHits;
  double AmpDir;
  double AmpIncl;
  int NHits;
};


void Read_DCH_Sim_ELoss() {

	printf("start read");

   
	int numLines = 0;
 
	int k=0;
	string unused;
	
  TString filename ="DCH_Sim_Real_Beam_wo_Tof700.root";
	TFile *treefile = new TFile(filename, "recreate");
	//output tree and its branches
	TTree *tree1 = new TTree("tree1","treelibrated tree");
	//tree1->SetDirectory(treefile);
  int EventID;
  
  Track *DCH = new Track;
 	TBranch *br_DCH_Ntr =tree1->Branch("DCH_Ntr",&DCH->nTracks);
  TBranch *br_DCH_X =tree1->Branch("DCH_X",&DCH->X);
  TBranch *br_DCH_Y =tree1->Branch("DCH_Y",&DCH->Y);
  TBranch *br_DCH_Z =tree1->Branch("DCH_Z",&DCH->Z);
  TBranch *br_DCH_Tx =tree1->Branch("DCH_Tx",&DCH->Tx);
  TBranch *br_DCH_Ty =tree1->Branch("DCH_Ty",&DCH->Ty);
  TBranch *br_DCH_Chi2 =tree1->Branch("DCH_Chi2",&DCH->Chi2);
  TBranch *br_DCH_nHits =tree1->Branch("DCH_nHits",&DCH->nHits);
  // TBranch *br_DCH_IsGlob =tree1->Branch("DCH_IsGlobalTrack",&DCH->IsGlDCH);
  // TBranch *br_DCH_NGlTracks =tree1->Branch("DCH_NGlobalTr",&DCH->nGlobDCHTracks);
  
  Track *DCH1 = new Track;
 	TBranch *br_DCH1_Ntr =tree1->Branch("DCH1_Ntr",&DCH1->nTracks);
  TBranch *br_DCH1_X =tree1->Branch("DCH1_X",&DCH1->X);
  TBranch *br_DCH1_Y =tree1->Branch("DCH1_Y",&DCH1->Y);
  TBranch *br_DCH1_Z =tree1->Branch("DCH1_Z",&DCH1->Z);
  TBranch *br_DCH1_Tx =tree1->Branch("DCH1_Tx",&DCH1->Tx);
  TBranch *br_DCH1_Ty =tree1->Branch("DCH1_Ty",&DCH1->Ty);
  TBranch *br_DCH1_Chi2 =tree1->Branch("DCH1_Chi2",&DCH1->Chi2);
  TBranch *br_DCH1_nHits =tree1->Branch("DCH1_nHits",&DCH1->nHits);
  // TBranch *br_DCH1_IsGlob =tree1->Branch("DCH1_IsGlobalTrack",&DCH1->IsGlDCH);
  // TBranch *br_DCH1_NGlTracks =tree1->Branch("DCH1_NGlobalTr",&DCH1->nGlobDCHTracks);
  
  Track *DCH2 = new Track;
 	TBranch *br_DCH2_Ntr =tree1->Branch("DCH2_Ntr",&DCH2->nTracks);
  TBranch *br_DCH2_X =tree1->Branch("DCH2_X",&DCH2->X);
  TBranch *br_DCH2_Y =tree1->Branch("DCH2_Y",&DCH2->Y);
  TBranch *br_DCH2_Z =tree1->Branch("DCH2_Z",&DCH2->Z);
  TBranch *br_DCH2_Tx =tree1->Branch("DCH2_Tx",&DCH2->Tx);
  TBranch *br_DCH2_Ty =tree1->Branch("DCH2_Ty",&DCH2->Ty);
  TBranch *br_DCH2_Chi2 =tree1->Branch("DCH2_Chi2",&DCH2->Chi2);
  TBranch *br_DCH2_nHits =tree1->Branch("DCH2_nHits",&DCH2->nHits);
  //TBranch *br_DCH2_IsGlob =tree1->Branch("DCH2_IsGlobalTrack",&DCH2->IsGlDCH);
  //TBranch *br_DCH2_NGlTracks =tree1->Branch("DCH2_NGlobalTr",&DCH2->nGlobDCHTracks);
  
  filename = "Reco_DCH_Sim_For_Decay_FullGeom_WO_Target_Field_Scaled_Real_Beam_wo_Tof700.root";
  //Reco_DCH_Sim_For_Decay_FullGeom_WO_Target_Field_Scaled_Real_Beam_wo_Tof700
      
  TFile *hfile = new TFile(filename);
  // if (!hfile->IsOpen())  continue;
  // if (!hfile->Get("bmndata")) 		continue;
        
  TTree *tree = (TTree*)hfile->Get("bmndata");
    
  int t = tree->GetEntries();
  cout<<t<<endl;
    
  TClonesArray *dchTrack = new TClonesArray("BmnDchTrack");
  DstEventHeader *evHeader = nullptr;

  tree->SetBranchAddress("DstEventHeader.", &evHeader);
  tree->SetBranchAddress("BmnDchTrack", &dchTrack);
  
  int NEv=tree->GetEntries();

      
  cout<<NEv<<endl;
  
  
  for (int i=0; i<NEv; i++)  //cycle by events
    {
      
      tree->GetEntry(i);
      EventID=i;
      
      DCH->nTracks=0;
      DCH1->nTracks=0;
      DCH2->nTracks=0;
        
			//DCH->nGlobDCHTracks=0;
			for (int k=0; k<dchTrack->GetEntries(); k++){
              
              
				BmnDchTrack *dch = (BmnDchTrack *)dchTrack->At(k);
        if (dch->GetParamFirst()->GetZ()>550 && dch->GetParamFirst()->GetZ()<650) {
          DCH->X.push_back(dch->GetParamFirst()->GetX());
  				DCH->Y.push_back(dch->GetParamFirst()->GetY());
  				DCH->Z.push_back(dch->GetParamFirst()->GetZ());
  				DCH->Tx.push_back(dch->GetParamFirst()->GetTx());
  				DCH->Ty.push_back(dch->GetParamFirst()->GetTy());
          DCH->Chi2.push_back(dch->GetChi2());
          DCH->nHits.push_back(dch->GetNHits());                                                  
  				DCH->nTracks=DCH->nTracks+1;      
        }
            
        if (dch->GetParamFirst()->GetZ()<550) {
          DCH1->X.push_back(dch->GetParamFirst()->GetX());
  				DCH1->Y.push_back(dch->GetParamFirst()->GetY());
					DCH1->Z.push_back(dch->GetParamFirst()->GetZ());
					DCH1->Tx.push_back(dch->GetParamFirst()->GetTx());
  				DCH1->Ty.push_back(dch->GetParamFirst()->GetTy());
          DCH1->Chi2.push_back(dch->GetChi2());
          DCH1->nHits.push_back(dch->GetNHits());                                                  
  				DCH1->nTracks=DCH1->nTracks+1;       
        }
            
        if (dch->GetParamFirst()->GetZ()>650) {
          DCH2->X.push_back(dch->GetParamFirst()->GetX());
  				DCH2->Y.push_back(dch->GetParamFirst()->GetY());
  				DCH2->Z.push_back(dch->GetParamFirst()->GetZ());
					DCH2->Tx.push_back(dch->GetParamFirst()->GetTx());
  				DCH2->Ty.push_back(dch->GetParamFirst()->GetTy());
          DCH2->Chi2.push_back(dch->GetChi2());
          DCH2->nHits.push_back(dch->GetNHits());                                                  
  				DCH2->nTracks=DCH2->nTracks+1;     
        }
      }  


      tree1->Fill();

      DCH->X.clear();
			DCH->Y.clear();
			DCH->Z.clear();
			DCH->Tx.clear();
			DCH->Ty.clear(); 
      DCH->Chi2.clear();
      DCH->nHits.clear();
         
      DCH1->X.clear();
			DCH1->Y.clear();
			DCH1->Z.clear();
			DCH1->Tx.clear();
			DCH1->Ty.clear(); 
      DCH1->Chi2.clear();
      DCH1->nHits.clear();
         
      DCH2->X.clear();
			DCH2->Y.clear();
			DCH2->Z.clear();
			DCH2->Tx.clear();
			DCH2->Ty.clear(); 
      DCH2->Chi2.clear();
      DCH2->nHits.clear();
		  
         
  }        


	cout<<"Finish!	";
 
 
	treefile->cd();
	//tree1->SetDirectory(treefile);
	tree1->Write();
	treefile->Close();
}

