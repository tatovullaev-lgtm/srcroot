//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <TROOT.h> 
#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include "TStopwatch.h"
//#include <stdbool.h>
#include <string>
#include <cstdlib>
#include "TObject.h"
#include "TObjString.h"
#include "TObjArray.h"
#include <TTimeStamp.h>
#include "TVector3.h"
#include "TClonesArray.h"
//#include "Combine_Digi.h"


using namespace std;

// define paths
TString srcroot_recoDir = "/zfs/scratch/tatov/Reconstruction/srcroot_reco/jun26";
TString SiDigiDir_txt = "/lustre/stor2/src/Reconstruction/SiDigi_old/txt";
TString SiDigiDir_root = "/zfs/scratch/tatov/Reconstruction/SiDigi_old/root/jun26";
TString GEM_CSC_DigiDir_txt = "/lustre/stor2/src/Reconstruction/GEM_CSC_Digi/txt";
TString GEM_CSC_DigiDir_root = "/zfs/scratch/tatov/Reconstruction/GEM_CSC_Digi/root/jun26";
TString D_CSC_DigiDir_txt = "/lustre/stor2/src/Reconstruction/D_CSC_Digi/txt";
TString D_CSC_DigiDir_root = "/zfs/scratch/tatov/Reconstruction/D_CSC_Digi/root/jun26";
TString outDir = "/zfs/scratch/tatov/Reconstruction/Combined_Files/jun26";

const double ShiftX1M1=0;
const double ShiftY2M1=0;
const double ShiftX3M1=0;
const double ShiftY4M1=0;

TStopwatch timer;

int flag_Si=0, flag_downCSC=0, flag_Right_gemcsc=0, flag_Left_gemcsc=0, flag_TOF=0;

void convert_to_root_Si(int filenum) 
{
	cout<<endl; printf("start read Si for run "); cout<<filenum<<endl;

  string name_file = "_SILICONS_COORDINATE_FULL_wo_gap_wo_rot_wo_ampcut.txt";
  TString gPathFull =  SiDigiDir_txt + "/" + to_string(filenum) + name_file;

  cout<<endl<<gPathFull<<endl;

  //ifstream fin(gPathFull.Data(), ios::binary);
  fstream fin;
  fin.open(gPathFull, ios::in);

  if (/*!fin.is_open()*/ fin.fail()) 
  
  {
    cout<<endl<<endl<<filenum<<"  no_si_digi"<<endl<<endl;  flag_Si=1; return;
  } cout<<gPathFull<<endl;
 
  TString OutFileName(SiDigiDir_root + "/" + to_string(filenum) +"_silicon.root"); cout << "Output txt to root file: " << OutFileName << endl; 
  TFile *outfile = new TFile(OutFileName, "RECREATE"); 
  TTree* tsi = new TTree("Silicon", "Silicon");
  
  Int_t Si_index;
  Int_t Event_ID; 
  vector <Double_t> X; 
  vector <Double_t> Y;
  Int_t St;
  Int_t Mod; 
  vector <Int_t> L0NHits; 
  vector <Int_t> L1NHits; 
  vector <Double_t> AmpDirect; 
  vector <Double_t> AmpInclined; 

  tsi->Branch("Event_ID", &Event_ID, "Event_ID/I");
  tsi->Branch("X", &X);
  tsi->Branch("Y", &Y);
  tsi->Branch("St",&St, "St/I");
  tsi->Branch("Mod", &Mod, "Mod/I");
  tsi->Branch("Si_index", &Si_index, "Si_index/I");
  tsi->Branch("L0NHits", &L0NHits);
  tsi->Branch("L1NHits", &L1NHits);
  tsi->Branch("AmpDirect", &AmpDirect);
  tsi->Branch("AmpInclined", &AmpInclined);  

  int ev, station, tmod, tL0NHits, tL1NHits;
  Double_t txtX, txtY, tAmpDir, tAmpIncl;

  int event_contor;
	int k=0;

  
  fin.seekg(0);
  string unused;
  //while ( getline(fin, unused))
  while (fin.peek()!=EOF )  
  {
    X.clear(); Y.clear();  L0NHits.clear(); L1NHits.clear(); AmpDirect.clear(); AmpInclined.clear();
    fin>>ev>>station>>tmod>>txtX>>txtY>>tL0NHits>>tL1NHits>>tAmpDir>>tAmpIncl;

    if (fin.fail()) {
        if (fin.eof()) {
            cout << "Reached end of file" << endl;
        } else {
            cout << "Error reading file at position " << fin.tellg() << endl;
        }
        break;
    }
 
    Event_ID = ev+1;
    Si_index = 900 + station*10 + tmod;
    event_contor = ev;
    int station_contor = station, mod_contor=tmod;

    X.push_back(txtX);
    Y.push_back(txtY);
    St=station;
    Mod=tmod;
    L0NHits.push_back(tL0NHits);
    L1NHits.push_back(tL1NHits);
    AmpDirect.push_back(tAmpDir);
    AmpInclined.push_back(tAmpIncl);

    streampos oldpos;
  

    while (true) {
        // Save current position
        streampos pos = fin.tellg();
        
        // Try to read next line
        fin >> ev >> station >> tmod >> txtX >> txtY >> tL0NHits >> tL1NHits >> tAmpDir >> tAmpIncl;
        
        if (fin.fail()) break;
        
        // Check if it's the same event, station, and module
        if (ev == event_contor && station == station_contor && tmod == mod_contor) {
            
            // Process this hit //cout << "*";
            X.push_back(txtX);
            Y.push_back(txtY);
            L0NHits.push_back(tL0NHits);
            L1NHits.push_back(tL1NHits);
            AmpDirect.push_back(tAmpDir);
            AmpInclined.push_back(tAmpIncl);
        } else {
            // Not the same group, rewind and break
            fin.seekg(pos);
            break;
        }
    }


    tsi -> Fill();
    //cout<<event_contor<<" ";
   
 }
   fin.close();
   tsi->Write();
   outfile -> Write();
   outfile->Close();
   //gApplication->Terminate();//so this can run better with multiple bash lines code
  
 printf("C'est fini Silicon to root \nReal time %f s, CPU time %f s\n", timer.RealTime(), timer.CpuTime());
 cout<<"txt to root conversion for Si: less than "<<(int)timer.RealTime()/60+1<<" minutes, so less than "<< (int)timer.RealTime()/60/60 +1 <<endl<<" hours" <<endl;
 cout<<endl<<endl;
}


bool in_active_area(Double_t x, Double_t y)
{
  return x>-56 && x<60 && y>-50 && y<50 ;
}

Double_t do_alignment_x(Double_t x)
{
  return x + 43.707;
}

Double_t do_alignment_y(Double_t y)
{
  return y + 0.511;
}

void downstreamCSC_txt_to_root(int runId)
{ 
  fstream cscfile;

  int x, y;
  x= runId / 100 * 100;
  y = x + 99;

  if(runId < 4605)
  {
    TString fileName(D_CSC_DigiDir_txt + "/" + std::to_string(runId) + "_C.txt");
    cscfile.open(fileName, ios::in); 
  }
  if (runId >= 4605)
  {
    TString fileName(D_CSC_DigiDir_txt + "/" + std::to_string(runId) + "_L.txt");  
    cscfile.open(fileName, ios::in); 
  } 

  if(cscfile.fail() /*!cscfile.is_open()*/)
  {
    cout<<endl<<runId<<" down CSC not open!"<<endl<<endl; flag_downCSC=1; return;
  }

  ULong64_t hextimestamp; Float_t local_X, local_Y; Short_t nhits; UInt_t check_flag; double dummy;
  TTree* tCsc = new TTree("FragmentCscTree", "");
  Int_t eventId, eventId1, eventId2;
  Int_t sec, nanosec;
  const Short_t maxhits = 40;
  Short_t cscN;
  vector <Float_t> cscX, cscY, cscZ_BMN;
  
  tCsc->Branch("RunId", &runId, "RunId/I");
  tCsc->Branch("EventId", &eventId1, "EventId/I");
  tCsc->Branch("Second", &sec, "Second/I");
  tCsc->Branch("NanoSecond", &nanosec, "NanoSecond/I");
  tCsc->Branch("CscN", &cscN, "CscN/S");
  tCsc->Branch("CscX", &cscX);
  tCsc->Branch("CscY", &cscY);
  tCsc->Branch("CscZ", &cscZ_BMN);

  TFile *outfile; 
  TString OutFileName(D_CSC_DigiDir_root + "/" + "DownstreamCscRun" + std::to_string(runId) + ".root"); 
  cout << "Output  downstream CSC file: " << OutFileName << endl; 
  outfile = new TFile(OutFileName, "RECREATE"); 

  Int_t eventN = 0, nr_outside = 0, nr_hits = 0, ev_outside = 0, nr_files_unsued=0;
  Bool_t display = true;
  int null_multip=0;
  
  eventN = 0;   

  while (cscfile.peek()!=EOF )
  {
    cscfile >> std::hex >> hextimestamp >> std::dec >> eventId1 >> eventId >> nhits;
    if (eventId == 0 && eventId1==0) { cscfile >> dummy >> dummy; continue; } //because of 2 lines with 0 0.00
    eventN++;

    check_flag = hextimestamp & 0xc0000000;
    uint tai_flag = hextimestamp & 3;
    uint tai_ns = (hextimestamp >> 2) & 0x3fffffff;
    uint tai_s = (hextimestamp >> 32) & 0xffffffff; 
    sec = tai_s;
    nanosec = tai_ns;
    
    cscN = 0; int N_active = 0; int aa=0;
    cscX.clear(); cscY.clear(); cscZ_BMN.clear(); 
          
    for(int h = 0; h < nhits; h++)
    {
      cscfile >> local_X >> local_Y;
      nr_hits++;

      // if( !in_active_area(local_X,local_Y))
      // {
      //   nr_outside++;
      //   if (aa==0) { ev_outside++;} aa++;
      //   continue;
      // } 

      local_X = do_alignment_x(local_X);
      local_Y = do_alignment_y(local_Y);
      cscX.push_back(local_X);
      cscY.push_back(local_Y);
      cscZ_BMN.push_back(424.8);            //424.8 related to BMN zero 
      cscN++;
    }
    if (cscN!=0)  {  tCsc->Fill();  }
  }

  cscfile.close();
    
  
  cout << ev_outside << " from " << eventN <<" so " << (double)ev_outside / eventN *100  << " percent events outside active zone "<<endl;
  cout << null_multip << " events with null multiplicity" << endl;
  
  tCsc->Write();
  outfile->Close();

  printf( " Real time %f s, CPU time %f s\n", timer.RealTime(), timer.CpuTime()); timer.Continue();
  cout<<"End of downstream CSC txt conversion to root"<<endl<<endl;
}

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


#define gemLeftArmLength 113.033
#define gemRightArmLength 111.832
#define gemLeftArmAngle 30.53
#define gemRightArmAngle -30.87

#define cscLeftArmLength 186.415
#define cscRightArmLength 186.636
#define cscLeftArmAngle 30.67
#define cscRightArmAngle -30.77

#define degtorad 0.01745329
#define middleZ -576.2
#define middleX  0.0
#define middleY 0.0


TVector3 translated, rotated, changed; 

Float_t BMNcenter_GemRight_x = -57 ,
  BMNcenter_GemRight_y = 0,
  BMNcenter_GemRight_z = 854;  //BMNcenter_GemRight_z = 854;


Float_t BMNcenter_CSCRight_x = -95  ,
  BMNcenter_CSCRight_y = -0.2 ,
  BMNcenter_CSCRight_z = 918; //BMNcenter_CSCRight_z = 918;

Float_t BMNcenter_GemLeft_x = 57.4  ,
  BMNcenter_GemLeft_y = -0.1 ,
  BMNcenter_GemLeft_z =  855.6; //BMNcenter_GemLeft_z =  855.6;

Float_t BMNcenter_CSCLeft_x = 95 ,
  BMNcenter_CSCLeft_y = -0.1 ,
  BMNcenter_CSCLeft_z = 918; //BMNcenter_CSCLeft_z = 918;


Float_t theta_Gem_Right =  -30.87, //-
  theta_CSC_Right =  -30.77  , // -
  theta_Gem_Left = 30.53,
  theta_CSC_Left = 30.67;


float  BMN_gem_left, BMN_csc_left, BMN_gem_right, BMN_csc_right;

void calcualte_Z_arms()
{
  float target_center = -5763.9;
  //from PreciseGometry
  float angle_gem_left = 30.53,
      angle_gem_right = 30.87 ,
      angle_csc_left = 30.67 ,
      angle_csc_right = 30.77 ;

  float  magnet_gem_left = 1130.33 ,
        magnet_gem_right = 1118.32 ,
        magnet_csc_left = 1864.15,
        magnet_csc_right = 1866.36  ;    

 float diff_gem_left = cos(angle_gem_left * degtorad) *  magnet_gem_left,
        diff_csc_left = cos(angle_csc_left * degtorad) *  magnet_csc_left,
        diff_gem_right = cos(angle_gem_right * degtorad) *  magnet_gem_right,
        diff_csc_right = cos(angle_csc_right * degtorad) *  magnet_csc_right;

  BMN_gem_left = (target_center + diff_gem_left)/10;
  BMN_csc_left = (target_center + diff_csc_left)/10;
  BMN_gem_right = (target_center + diff_gem_right)/10; 
  BMN_csc_right = (target_center + diff_csc_right)/10;

   
  cout<<"distance from magnet center to center of GEM left (mm) "<<diff_gem_left<<" and in BMN coordinates (cm) " << BMN_gem_left << endl;
  cout<<"distance from magnet center to center of CSC left (mm) "<<diff_csc_left<<" and in BMN coordinates (cm) " << BMN_csc_left<<endl;
  cout<<"distance from magnet center to center of GEM right (mm) "<<diff_gem_right<<" and in BMN coordinates (cm) " << BMN_gem_right<<endl;
  cout<<"distance from magnet center to center of CSC right (mm) "<<diff_csc_right<<" and in BMN coordinates (cm) " << BMN_csc_right<<endl;

  cout<<endl<<endl;
      
}  


TVector3 change_coordinates_Gem_Right(Float_t x_local, Float_t y_local)
{
  //the rotation around Y
  TVector3 initial(x_local, y_local, 0);
  TVector3 origin_local(0,0,0);

  float theta = theta_Gem_Right * degtorad; 
  
  initial.RotateY(theta);

  //the translation from 0,0,0 to the center in BMN coordinates
  //TVector3 origin_BMN(-BMNcenter_GemRight_x, -BMNcenter_GemRight_y, -BMNcenter_GemRight_z);
  TVector3 origin_BMN(-BMNcenter_GemRight_x, -BMNcenter_GemRight_y, -BMN_gem_right);

  changed = initial - origin_BMN;

  return changed;
}


TVector3 change_coordinates_CSC_Right(Float_t x_local, Float_t y_local)
{
  TVector3 initial(x_local, y_local, 0);
  float theta = theta_CSC_Right * degtorad;
  initial.RotateY(theta);

  //TVector3 origin_BMN(-BMNcenter_CSCRight_x, -BMNcenter_CSCRight_y, -BMNcenter_CSCRight_z);
  TVector3 origin_BMN(-BMNcenter_CSCRight_x, -BMNcenter_CSCRight_y, -BMN_csc_right);
  changed = initial - origin_BMN;

  return  changed;
}


TVector3 change_coordinates_Gem_Left(Float_t x_local, Float_t y_local)
{
  TVector3 initial(x_local, y_local, 0);
  float theta = theta_Gem_Left * degtorad; 
  initial.RotateY(theta);
  initial.RotateY(180 * degtorad);


  //TVector3 origin_BMN(-BMNcenter_GemLeft_x, -BMNcenter_GemLeft_y, -BMNcenter_GemLeft_z);
  TVector3 origin_BMN(-BMNcenter_GemLeft_x, -BMNcenter_GemLeft_y, -BMN_gem_left);
  changed = initial - origin_BMN;

  return changed;
}


TVector3 change_coordinates_CSC_Left(Float_t x_local, Float_t y_local)
{
  TVector3 initial(x_local, y_local, 0);
  float theta = theta_CSC_Left * degtorad; 
  initial.RotateY(theta);
  initial.RotateY(180 * degtorad);

  //TVector3 origin_BMN(-BMNcenter_CSCLeft_x, -BMNcenter_CSCLeft_y, -BMNcenter_CSCLeft_z);
  TVector3 origin_BMN(-BMNcenter_CSCLeft_x, -BMNcenter_CSCLeft_y, -BMN_csc_left);
  
  changed = initial - origin_BMN;

  return changed;
}


void GemCscTextToRoot_right(Int_t filenum)
{
  int x, y;
  x= filenum / 100 * 100;
  y = x + 99;
  char path[100];

  string rigth = "_R.txt";
  //string folder = "Runs";
  TString fileNameRight; //(path_gemcsc + std::to_string(filenum) + "_R.txt");
  //fileNameRight = path_gemcsc + folder + std::to_string(x) + "-" + std::to_string(y) + "/" + std::to_string(filenum) + rigth;//("%d_R.txt", filenum);
  fileNameRight = GEM_CSC_DigiDir_txt + "/" + to_string(filenum) + rigth;//("%d_R.txt", filenum);
  
  fstream gemcscfileRight;
  gemcscfileRight.open(fileNameRight, ios::in);
   
  if(gemcscfileRight.fail() /*!gemcscfileRight.is_open()*/)
  {
    cout <<endl<<filenum<< " Error opening file gemCSC" <<fileNameRight<< endl<<endl;
    flag_Right_gemcsc=1;
    return;
  }
  cout<<"Opening file GemCSC right "<<fileNameRight<<endl;

  TTree* tGemCsc = new TTree("GemCscTree", "Right Gem Csc tree");
  Int_t runId = filenum;
  Int_t eventId, eventId1, eventIdLeft, eventId1Left, /*eventId_tmp,*/ eventId1LR, eventIdLR;
  ULong64_t hex_right, hex_left;
  Int_t right_sec, right_nanosec, left_sec, left_nanosec, sec, nanosec, nanosecDiv10K;
  Short_t rightGemN, rightCscN;
  Short_t rightGemN_header, rightCscN_header;
  Short_t leftGemN, leftCscN;
  Short_t leftGemN_header, leftCscN_header;
  vector <Float_t> rightGemX, rightGemY, rightGemZ, rightGemXLocal, rightGemYLocal; 
  vector <Float_t> rightCscX, rightCscY, rightCscZ, rightCscXLocal, rightCscYLocal;
  Float_t temp;

  tGemCsc->Branch("RunId", &runId, "RunId/I");
  tGemCsc->Branch("EventId", &eventId);
  tGemCsc->Branch("EventId1", &eventId1);
  tGemCsc->Branch("Second", &sec);
  tGemCsc->Branch("NanoSecond", &nanosec);
  tGemCsc->Branch("RightGemN", &rightGemN );
  tGemCsc->Branch("RightGemX", &rightGemX );
  tGemCsc->Branch("RightGemY", &rightGemY );
  tGemCsc->Branch("RightGemZ", &rightGemZ );
  tGemCsc->Branch("RightGemXLocal", &rightGemXLocal );
  tGemCsc->Branch("RightGemYLocal", &rightGemYLocal );
  tGemCsc->Branch("RightCscN", &rightCscN );
  tGemCsc->Branch("RightCscX", &rightCscX );
  tGemCsc->Branch("RightCscY", &rightCscY );
  tGemCsc->Branch("RightCscZ", &rightCscZ );
  tGemCsc->Branch("RightCscXLocal", &rightCscXLocal );
  tGemCsc->Branch("RightCscYLocal", &rightCscYLocal );
          
  TFile *outfile;
  //TString OutFileName(""); OutFileName.Form(path_out_root + "GemCscRuns_Right_%d.root", filenum); 
  TString OutFileName(GEM_CSC_DigiDir_root + "/GemCscRuns_Right_" + to_string(filenum) + ".root");
  cout << "Output file right: " << OutFileName << endl;
  outfile = new TFile(OutFileName, "RECREATE");
    
  Int_t totalEvents = 0;

  

  
  
   
  Float_t  local_rightCscX, local_rightCscY;
  Float_t  local_rightGemX, local_rightGemY;
  UInt_t check_flag;
  double dummy;
  float valueX, valueY, valueZ;

  while(gemcscfileRight.peek()!=EOF)
  {
    rightCscXLocal.clear(); rightCscYLocal.clear(); 
    rightCscX.clear(); rightCscY.clear(); rightCscZ.clear();
    rightGemXLocal.clear(); rightGemYLocal.clear(); 
    rightGemX.clear(); rightGemY.clear(); rightGemZ.clear();
  
    gemcscfileRight >> std::hex >> hex_right >> std::dec >> eventId1 >> eventId >> rightCscN_header >> rightGemN_header;
    if (eventId == 0 && eventId1==0) { gemcscfileRight >> dummy >> dummy >> dummy >> dummy; continue; }
    totalEvents++;

    right_nanosec = hex_right & 0x0fffffff;
    right_sec = (hex_right >> 32) & 0xffffffff;
    check_flag = hex_right & 0xc0000000;

    uint tai_flag = hex_right & 3;
    uint tai_ns = (hex_right >> 2) & 0x3fffffff;
    uint tai_s = (hex_right >> 32) & 0xffffffff;
    
    right_sec = tai_s;
    right_nanosec = tai_ns;   
    
    if(rightCscN_header == 0) gemcscfileRight >> dummy >> dummy;
    else
    for(Int_t c = 0; c < rightCscN_header; c++)
    {
      gemcscfileRight >> local_rightCscX >> local_rightCscY;

      rightCscXLocal.push_back(local_rightCscX);
      rightCscYLocal.push_back(local_rightCscY);

      change_coordinates_CSC_Right(local_rightCscX, local_rightCscY);
      rightCscX.push_back(changed.X());
      rightCscY.push_back(changed.Y());
      rightCscZ.push_back(changed.Z()); 
    }   

    rightCscN = rightCscN_header;
        
    
    if(rightGemN_header==0)  gemcscfileRight >> dummy >> dummy;  
    else
    for(Int_t c = 0; c < rightGemN_header; c++)
    {   
      gemcscfileRight >> local_rightGemX >> local_rightGemY;

      rightGemXLocal.push_back(local_rightGemX);
      rightGemYLocal.push_back(local_rightGemY);

      change_coordinates_Gem_Right(local_rightGemX, local_rightGemY);
      rightGemX.push_back(changed.X());
      rightGemY.push_back(changed.Y());
      rightGemZ.push_back(changed.Z()); 
    } 

    rightGemN =  rightGemN_header; 

    tGemCsc->Fill();         
  }
 
  gemcscfileRight.close();

 
  cout << totalEvents << " total events processed in gemcsc Right, run " << filenum  << endl;
  gemcscfileRight.close();
  
  tGemCsc->Write();
  //outfile->Write();
  outfile->Close();
  
  printf( " Real time %f s, CPU time %f s\n", timer.RealTime(), timer.CpuTime()); timer.Continue();
  cout<<endl<<endl;
}


void GemCscTextToRoot_left(Int_t filenum)
{
  int x, y;
  x= filenum / 100 * 100;
  y = x + 99;
  char path[100];

  string left = "_L.txt";
  //string folder = "Runs";
  TString fileNameLeft; //(path_gemcsc + std::to_string(filenum) + "_R.txt");
  //fileNameLeft = path_gemcsc + std::to_string(filenum) + left;//("%d_R.txt", filenum);
  //fileNameLeft = path_gemcsc + folder + std::to_string(x) + "-" + std::to_string(y) + "/" + std::to_string(filenum) + left;//("%d_R.txt", filenum);
  fileNameLeft = GEM_CSC_DigiDir_txt + "/" + to_string(filenum) + left;//("%d_R.txt", filenum);

   
  fstream gemcscfileLeft;
  gemcscfileLeft.open(fileNameLeft, ios::in);
  //
  if(gemcscfileLeft.fail() /*!gemcscfileLeft.is_open()*/)
  {
    cout << endl<<filenum<< " Error opening file gemCSC"<< fileNameLeft<< endl;
    flag_Left_gemcsc=1;
    return;
  }
  cout << "Opening file gemCSC left" << fileNameLeft << endl;

  TTree* tGemCsc = new TTree("GemCscTree", "Left Gem Csc tree");
  Int_t runId = filenum;
  Int_t eventId, eventId1, eventIdLeft, eventId1Left, /*eventId_tmp,*/ eventId1LR, eventIdLR;
  ULong64_t hex_Left, hex_left;
  Int_t Left_sec, Left_nanosec, left_sec, left_nanosec, sec, nanosec, nanosecDiv10K;
  Short_t LeftGemN, LeftCscN;
  Short_t LeftGemN_header, LeftCscN_header;
  Short_t leftGemN, leftCscN;
  Short_t leftGemN_header, leftCscN_header;
  vector <Float_t> LeftGemX, LeftGemY, LeftGemZ, LeftGemXLocal, LeftGemYLocal; 
  vector <Float_t> LeftCscX, LeftCscY, LeftCscZ, LeftCscXLocal, LeftCscYLocal;
  Float_t temp;

  tGemCsc->Branch("RunId", &runId, "RunId/I");
  tGemCsc->Branch("EventId", &eventId);
  tGemCsc->Branch("EventId1", &eventId1);
  tGemCsc->Branch("Second", &sec);
  tGemCsc->Branch("NanoSecond", &nanosec);
  tGemCsc->Branch("LeftGemN", &LeftGemN );
  tGemCsc->Branch("LeftGemX", &LeftGemX );
  tGemCsc->Branch("LeftGemY", &LeftGemY );
  tGemCsc->Branch("LeftGemZ", &LeftGemZ );
  tGemCsc->Branch("LeftGemXLocal", &LeftGemXLocal );
  tGemCsc->Branch("LeftGemYLocal", &LeftGemYLocal );
  tGemCsc->Branch("LeftCscN", &LeftCscN );
  tGemCsc->Branch("LeftCscX", &LeftCscX );
  tGemCsc->Branch("LeftCscY", &LeftCscY );
  tGemCsc->Branch("LeftCscZ", &LeftCscZ );
  tGemCsc->Branch("LeftCscXLocal", &LeftCscXLocal );
  tGemCsc->Branch("LeftCscYLocal", &LeftCscYLocal );
          
  TFile *outfile;
  // TString OutFileName("");OutFileName.Form(path_out_root +"GemCscRuns_Left_%d.root", filenum); 
  TString OutFileName(GEM_CSC_DigiDir_root + "/GemCscRuns_Left_" + to_string(filenum) + ".root");
  cout << "Output file Left: " << OutFileName << endl;
  outfile = new TFile(OutFileName, "RECREATE");
    
  Int_t totalEvents = 0;

  
   
  Float_t  local_LeftCscX, local_LeftCscY;
  Float_t  local_LeftGemX, local_LeftGemY;
  UInt_t check_flag;
  double dummy;
  float valueX, valueY, valueZ;

  while(gemcscfileLeft.peek()!=EOF)
  {
    LeftCscXLocal.clear(); LeftCscYLocal.clear(); 
    LeftCscX.clear(); LeftCscY.clear(); LeftCscZ.clear();
    LeftGemXLocal.clear(); LeftGemYLocal.clear(); 
    LeftGemX.clear(); LeftGemY.clear(); LeftGemZ.clear();
  
    gemcscfileLeft >> std::hex >> hex_Left >> std::dec >> eventId1 >> eventId >> LeftCscN_header >> LeftGemN_header;
    if (eventId == 0 && eventId1==0) { gemcscfileLeft >> dummy >> dummy >> dummy >> dummy; continue; }
    totalEvents++;

    Left_nanosec = hex_Left & 0x0fffffff;
    Left_sec = (hex_Left >> 32) & 0xffffffff;
    check_flag = hex_Left & 0xc0000000;

    uint tai_flag = hex_Left & 3;
    uint tai_ns = (hex_Left >> 2) & 0x3fffffff;
    uint tai_s = (hex_Left >> 32) & 0xffffffff;
    
    Left_sec = tai_s;
    Left_nanosec = tai_ns;   
    
    if(LeftCscN_header == 0) gemcscfileLeft >> dummy >> dummy;
    else
    for(Int_t c = 0; c < LeftCscN_header; c++)
    {
      gemcscfileLeft >> local_LeftCscX >> local_LeftCscY;

      LeftCscXLocal.push_back(local_LeftCscX);
      LeftCscYLocal.push_back(local_LeftCscY);

      change_coordinates_CSC_Left(local_LeftCscX, local_LeftCscY);
      LeftCscX.push_back(changed.X());
      LeftCscY.push_back(changed.Y());
      LeftCscZ.push_back(changed.Z());
    }   

    LeftCscN = LeftCscN_header;
        
    
    if(LeftGemN_header==0)  gemcscfileLeft >> dummy >> dummy;  
    else
    for(Int_t c = 0; c < LeftGemN_header; c++)
    {   
      gemcscfileLeft >> local_LeftGemX >> local_LeftGemY;

      LeftGemXLocal.push_back(local_LeftGemX);
      LeftGemYLocal.push_back(local_LeftGemY);

      change_coordinates_Gem_Left(local_LeftGemX, local_LeftGemY);
      LeftGemX.push_back(changed.X());
      LeftGemY.push_back(changed.Y());
      LeftGemZ.push_back(changed.Z());  
    } 

    LeftGemN =  LeftGemN_header; 

    tGemCsc->Fill();         
  }
 
  gemcscfileLeft.close();

 
  cout << totalEvents << " total events processed in gemcsc Left, run " << filenum  << endl;
  gemcscfileLeft.close();
  
  tGemCsc->Write();
  //outfile->Write();
  outfile->Close();
  
  printf( " Real time %f s, CPU time %f s\n", timer.RealTime(), timer.CpuTime()); timer.Continue();
  cout<<endl<<endl;
  //gApplication->Terminate();


}

void GemCscTextToRoot(Int_t filenum)
{
  calcualte_Z_arms(); 
  GemCscTextToRoot_right(filenum);
  GemCscTextToRoot_left(filenum);
}

void sync_bmn_Si_downstreamCSC_gemCSC(int filenum)
{
  cout<<endl<<endl; printf("start syncronization"); cout<<endl; //TStopwatch timer; timer.Start();
   
  int numLines = 0;

   string runn = "Run_";
   string reco = "_Reco.root";  //"_Reco_TOF400.root";
   TString filenamet400 = srcroot_recoDir + "/Run_" + to_string(filenum) + reco;

  TFile *hfile = new TFile(filenamet400);  cout<<endl;
  if (!hfile->IsOpen()) {cout<<filenamet400<<" not open!!!"<<endl; flag_TOF=1; return;}
  if (!hfile->Get("bmndata"))    {cout<<filenamet400<<" problem bmndata tree!!!"<<endl; flag_TOF=1; return;}
    
  TTree *tree = (TTree*)hfile->Get("bmndata");
  int NEv=tree->GetEntries();  cout<<endl<<"run "<<filenum<<" with "<<NEv<<" events in bmn file "<<endl;
  if (NEv==0) cout<<filenum<<" zero events"<<endl;
  cout<<" BMN tree file "<<filenamet400<<endl;
  if(NEv<1) {cout<<"empty TOF400 BMN data "<<endl; return;}
 
  int k=0;
  string unused;
  TString filename = outDir + "/All_Digi_" + to_string(filenum) + ".root";
  
  cout<<endl<<"Output file: "<<filename<<endl<<endl;

  TFile *treefile = new TFile(filename, "recreate");
  //output tree and its branches
  TTree *tree1 = new TTree("tree1","treelibrated tree");

  int EventID, EventID_Bmn, EventIDN, run;
  double BC1, BC2, BC3, BC4, BC5, BC1_cor, BC2_cor, BC3_cor, BC4_cor, BC5_cor;
  time_t TS_sec;
  long TS_nsec;
  int nrun, chargein, charge34, charge35, charge45, chargein_cor, charge34_cor, charge35_cor, charge45_cor, trigger, veto, SW_Mult, SW_NMatchTr, VB_Mult;
  int LeftTof400_nHits, RightTof400_nHits;
  vector <Double_t> LeftTof400_X,LeftTof400_Y,LeftTof400_Z, LeftTof400_TS;
  vector <Int_t> LeftTof400_DetId, LeftTof400_Module, LeftTof400_Station;
  vector <Double_t> RightTof400_X,RightTof400_Y,RightTof400_Z, RightTof400_TS;
  vector <Int_t> RightTof400_DetId, RightTof400_Module, RightTof400_Station;
  vector <Double_t> Tof400_X,Tof400_Y,Tof400_Z, Tof400_TS;
  vector <Int_t> Tof400_DetId, Tof400_Module, Tof400_Station;

  vector <Double_t> SW_ELoss, SW_CellId, SW_Flag, SW_TrId, SW_OrderId;
  vector <Int_t> VB_CellId;

  Int_t Is_laser, TofCal_nHits; //
  vector <Double_t> TofCal_Time0, TofCal_Time1, TofCal_Amp0, TofCal_Amp1, TofCal_X, TofCal_Y; 
  vector <Int_t> TofCal_Plane, TofCal_Bar, TofCal_Arm, TofCal_GlBar; 
  Double_t BC1_Time0, BC1_Time1, BC2_Time0, BC2_Time1;  //  

  TBranch *br_EventID_Bmn =tree1->Branch("eventBmn",&EventID_Bmn);
  //TBranch *br_EventID =tree1->Branch("event",&EventID);
  TBranch *br_trigger =tree1->Branch("trigger",&trigger);
  TBranch *br_veto =tree1->Branch("VC",&veto);
  TBranch *br_TS_sec =tree1->Branch("TS_sec",&TS_sec);
  TBranch *br_TS_nsec =tree1->Branch("TS_nsec",&TS_nsec);
  TBranch *br_run =tree1->Branch("run",&run);
  TBranch *br_BC1 =tree1->Branch("BC1",&BC1);
  TBranch *br_BC2 =tree1->Branch("BC2",&BC2);
  TBranch *br_BC3 =tree1->Branch("BC3",&BC3);
  TBranch *br_BC4 =tree1->Branch("BC4",&BC4);
  TBranch *br_BC5 =tree1->Branch("BC5",&BC5);
  TBranch *br_chargein =tree1->Branch("chargein",&chargein);
  TBranch *br_charge34 =tree1->Branch("charge34",&charge34);
  TBranch *br_charge35 =tree1->Branch("charge35",&charge35);
  TBranch *br_charge45 =tree1->Branch("charge45",&charge45);

  TBranch *br_BC1_cor =tree1->Branch("BC1_cor",&BC1_cor);
  TBranch *br_BC2_cor =tree1->Branch("BC2_cor",&BC2_cor);
  TBranch *br_BC3_cor =tree1->Branch("BC3_cor",&BC3_cor);
  TBranch *br_BC4_cor =tree1->Branch("BC4_cor",&BC4_cor);
  TBranch *br_BC5_cor =tree1->Branch("BC5_cor",&BC5_cor);
  TBranch *br_chargein_cor =tree1->Branch("chargein_cor",&chargein_cor);
  TBranch *br_charge34_cor =tree1->Branch("charge34_cor",&charge34_cor);
  TBranch *br_charge35_cor =tree1->Branch("charge35_cor",&charge35_cor);
  TBranch *br_charge45_cor =tree1->Branch("charge45_cor",&charge45_cor);

  TBranch *br_SW_ELoss =tree1->Branch("SW_ELoss",&SW_ELoss);
  TBranch *br_SW_CellId =tree1->Branch("SW_CellId",&SW_CellId);
  TBranch *br_SW_Flag =tree1->Branch("SW_Flag",&SW_Flag);
  TBranch *br_SW_TrId =tree1->Branch("SW_TrId",&SW_TrId);
  TBranch *br_SW_OrderId =tree1->Branch("SW_OrderId",&SW_OrderId);
  TBranch *br_SW_Mult =tree1->Branch("SW_Mult",&SW_Mult);
  TBranch *br_SW_NMatchTr =tree1->Branch("SW_NMatchTr",&SW_NMatchTr);

  TBranch *br_VB_Mult =tree1->Branch("VB_Mult",&VB_Mult);
  TBranch *br_VB_CellId =tree1->Branch("VB_CellId",&VB_CellId);

  TBranch *br_LeftTof400_X =tree1->Branch("LeftTof400_X",&LeftTof400_X);
  TBranch *br_LeftTof400_Y =tree1->Branch("LeftTof400_Y",&LeftTof400_Y);
  TBranch *br_LeftTof400_Z =tree1->Branch("LeftTof400_Z",&LeftTof400_Z);
  TBranch *br_LeftTof400_TS =tree1->Branch("LeftTof400_TS",&LeftTof400_TS);
  TBranch *br_LeftTof400_DetId =tree1->Branch("LeftTof400_DetId",&LeftTof400_DetId);
  TBranch *br_LeftTof400_Module =tree1->Branch("LeftTof400_Module",&LeftTof400_Module);
  TBranch *br_LeftTof400_Station =tree1->Branch("LeftTof400_Station",&LeftTof400_Station);
  TBranch *br_LeftTof400_nHits =tree1->Branch("LeftTof400_nHits",&LeftTof400_nHits);

  TBranch *br_RightTof400_X =tree1->Branch("RightTof400_X",&RightTof400_X);
  TBranch *br_RightTof400_Y =tree1->Branch("RightTof400_Y",&RightTof400_Y);
  TBranch *br_RightTof400_Z =tree1->Branch("RightTof400_Z",&RightTof400_Z);
  TBranch *br_RightTof400_TS =tree1->Branch("RightTof400_TS",&RightTof400_TS);
  TBranch *br_RightTof400_DetId =tree1->Branch("RightTof400_DetId",&RightTof400_DetId);
  TBranch *br_RightTof400_Module =tree1->Branch("RightTof400_Module",&RightTof400_Module);
  TBranch *br_RightTof400_Station =tree1->Branch("RightTof400_Station",&RightTof400_Station);
  TBranch *br_RightTof400_nHits =tree1->Branch("RightTof400_nHits",&RightTof400_nHits);

  TBranch *br_TofCal_nHits = tree1->Branch("TofCal_nHits", &TofCal_nHits); 
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
  TBranch *br_TofCal_Y = tree1->Branch("TofCal_Y", &TofCal_Y); 
  
  
     Inf *Sil1H = new Inf;
  TBranch *br_Sil1H_X =tree1->Branch("Si1H_X",&Sil1H->X);
  TBranch *br_Sil1H_Y =tree1->Branch("Si1H_Y",&Sil1H->Y);
  TBranch *br_Sil1H_St =tree1->Branch("Si1H_St",&Sil1H->St);
  TBranch *br_Sil1H_Mod =tree1->Branch("Si1H_Mod",&Sil1H->Mod);
  TBranch *br_Sil1H_L0H =tree1->Branch("Si1H_L0NHits",&Sil1H->L0H);
  TBranch *br_Sil1H_L1H =tree1->Branch("Si1H_L1NHits",&Sil1H->L1H);
  TBranch *br_Sil1H_AmpDir =tree1->Branch("Si1H_AmpDir",&Sil1H->AmpDir);
  TBranch *br_Sil1H_AmpIncl =tree1->Branch("Si1H_AmpIncl",&Sil1H->AmpIncl);
  TBranch *br_Sil1H_nHits =tree1->Branch("Si1H_nHits",&Sil1H->nHits);
  
  Inf *Sil2H = new Inf;
  TBranch *br_Sil2H_X =tree1->Branch("Si2H_X",&Sil2H->X);
  TBranch *br_Sil2H_Y =tree1->Branch("Si2H_Y",&Sil2H->Y);
  TBranch *br_Sil2H_St =tree1->Branch("Si2H_St",&Sil2H->St);
  TBranch *br_Sil2H_Mod =tree1->Branch("Si2H_Mod",&Sil2H->Mod);
  TBranch *br_Sil2H_L0H =tree1->Branch("Si2H_L0NHits",&Sil2H->L0H);
  TBranch *br_Sil2H_L1H =tree1->Branch("Si2H_L1NHits",&Sil2H->L1H);
  TBranch *br_Sil2H_AmpDir =tree1->Branch("Si2H_AmpDir",&Sil2H->AmpDir);
  TBranch *br_Sil2H_AmpIncl =tree1->Branch("Si2H_AmpIncl",&Sil2H->AmpIncl);
  TBranch *br_Sil2H_nHits =tree1->Branch("Si2H_nHits",&Sil2H->nHits);

  Inf *Sil3H = new Inf;
  TBranch *br_Sil3H_X =tree1->Branch("Si3H_X",&Sil3H->X);
  TBranch *br_Sil3H_Y =tree1->Branch("Si3H_Y",&Sil3H->Y);
  TBranch *br_Sil3H_St =tree1->Branch("Si3H_St",&Sil3H->St);
  TBranch *br_Sil3H_Mod =tree1->Branch("Si3H_Mod",&Sil3H->Mod);
  TBranch *br_Sil3H_L0H =tree1->Branch("Si3H_L0NHits",&Sil3H->L0H);
  TBranch *br_Sil3H_L1H =tree1->Branch("Si3H_L1NHits",&Sil3H->L1H);
  TBranch *br_Sil3H_AmpDir =tree1->Branch("Si3H_AmpDir",&Sil3H->AmpDir);
  TBranch *br_Sil3H_AmpIncl =tree1->Branch("Si3H_AmpIncl",&Sil3H->AmpIncl);
  TBranch *br_Sil3H_nHits =tree1->Branch("Si3H_nHits",&Sil3H->nHits);
  
  Inf *Sil4H = new Inf;
  TBranch *br_Sil4H_X =tree1->Branch("Si4H_X",&Sil4H->X);
  TBranch *br_Sil4H_Y =tree1->Branch("Si4H_Y",&Sil4H->Y);
  TBranch *br_Sil4H_St =tree1->Branch("Si4H_St",&Sil4H->St);
  TBranch *br_Sil4H_Mod =tree1->Branch("Si4H_Mod",&Sil4H->Mod);
  TBranch *br_Sil4H_L0H =tree1->Branch("Si4H_L0NHits",&Sil4H->L0H);
  TBranch *br_Sil4H_L1H =tree1->Branch("Si4H_L1NHits",&Sil4H->L1H);
  TBranch *br_Sil4H_AmpDir =tree1->Branch("Si4H_AmpDir",&Sil4H->AmpDir);
  TBranch *br_Sil4H_AmpIncl =tree1->Branch("Si4H_AmpIncl",&Sil4H->AmpIncl);
  TBranch *br_Sil4H_nHits =tree1->Branch("Si4H_nHits",&Sil4H->nHits);
  
  Inf *Sil11H = new Inf;
  TBranch *br_Sil11H_X =tree1->Branch("Si11H_X",&Sil11H->X);
  TBranch *br_Sil11H_Y =tree1->Branch("Si11H_Y",&Sil11H->Y);
  TBranch *br_Sil11H_St =tree1->Branch("Si11H_St",&Sil11H->St);
  TBranch *br_Sil11H_Mod =tree1->Branch("Si11H_Mod",&Sil11H->Mod);
  TBranch *br_Sil11H_L0H =tree1->Branch("Si11H_L0NHits",&Sil11H->L0H);
  TBranch *br_Sil11H_L1H =tree1->Branch("Si11H_L1NHits",&Sil11H->L1H);
  TBranch *br_Sil11H_AmpDir =tree1->Branch("Si11H_AmpDir",&Sil11H->AmpDir);
  TBranch *br_Sil11H_AmpIncl =tree1->Branch("Si11H_AmpIncl",&Sil11H->AmpIncl);
  TBranch *br_Sil11H_nHits =tree1->Branch("Si11H_nHits",&Sil11H->nHits);
  
  Inf *Sil12H = new Inf;
  TBranch *br_Sil12H_X =tree1->Branch("Si12H_X",&Sil12H->X);
  TBranch *br_Sil12H_Y =tree1->Branch("Si12H_Y",&Sil12H->Y);
  TBranch *br_Sil12H_St =tree1->Branch("Si12H_St",&Sil12H->St);
  TBranch *br_Sil12H_Mod =tree1->Branch("Si12H_Mod",&Sil12H->Mod);
  TBranch *br_Sil12H_L0H =tree1->Branch("Si12H_L0NHits",&Sil12H->L0H);
  TBranch *br_Sil12H_L1H =tree1->Branch("Si12H_L1NHits",&Sil12H->L1H);
  TBranch *br_Sil12H_AmpDir =tree1->Branch("Si12H_AmpDir",&Sil12H->AmpDir);
  TBranch *br_Sil12H_AmpIncl =tree1->Branch("Si12H_AmpIncl",&Sil12H->AmpIncl);
  TBranch *br_Sil12H_nHits =tree1->Branch("Si12H_nHits",&Sil12H->nHits);
 
  Inf *Sil21H = new Inf;
  TBranch *br_Sil21H_X =tree1->Branch("Si21H_X",&Sil21H->X);
  TBranch *br_Sil21H_Y =tree1->Branch("Si21H_Y",&Sil21H->Y);
  TBranch *br_Sil21H_St =tree1->Branch("Si21H_St",&Sil21H->St);
  TBranch *br_Sil21H_Mod =tree1->Branch("Si21H_Mod",&Sil21H->Mod);
  TBranch *br_Sil21H_L0H =tree1->Branch("Si21H_L0NHits",&Sil21H->L0H);
  TBranch *br_Sil21H_L1H =tree1->Branch("Si21H_L1NHits",&Sil21H->L1H);
  TBranch *br_Sil21H_AmpDir =tree1->Branch("Si21H_AmpDir",&Sil21H->AmpDir);
  TBranch *br_Sil21H_AmpIncl =tree1->Branch("Si21H_AmpIncl",&Sil21H->AmpIncl);
  TBranch *br_Sil21H_nHits =tree1->Branch("Si21H_nHits",&Sil21H->nHits);

  Inf *Sil22H = new Inf;
  TBranch *br_Sil22H_X =tree1->Branch("Si22H_X",&Sil22H->X);
  TBranch *br_Sil22H_Y =tree1->Branch("Si22H_Y",&Sil22H->Y);
  TBranch *br_Sil22H_St =tree1->Branch("Si22H_St",&Sil22H->St);
  TBranch *br_Sil22H_Mod =tree1->Branch("Si22H_Mod",&Sil22H->Mod);
  TBranch *br_Sil22H_L0H =tree1->Branch("Si22H_L0NHits",&Sil22H->L0H);
  TBranch *br_Sil22H_L1H =tree1->Branch("Si22H_L1NHits",&Sil22H->L1H);
  TBranch *br_Sil22H_AmpDir =tree1->Branch("Si22H_AmpDir",&Sil22H->AmpDir);
  TBranch *br_Sil22H_AmpIncl =tree1->Branch("Si22H_AmpIncl",&Sil22H->AmpIncl);
  TBranch *br_Sil22H_nHits =tree1->Branch("Si22H_nHits",&Sil22H->nHits);

  Inf *Sil31H = new Inf;
  TBranch *br_Sil31H_X =tree1->Branch("Si31H_X",&Sil31H->X);
  TBranch *br_Sil31H_Y =tree1->Branch("Si31H_Y",&Sil31H->Y);
  TBranch *br_Sil31H_St =tree1->Branch("Si31H_St",&Sil31H->St);
  TBranch *br_Sil31H_Mod =tree1->Branch("Si31H_Mod",&Sil31H->Mod);
  TBranch *br_Sil31H_L0H =tree1->Branch("Si31H_L0NHits",&Sil31H->L0H);
  TBranch *br_Sil31H_L1H =tree1->Branch("Si31H_L1NHits",&Sil31H->L1H);
  TBranch *br_Sil31H_AmpDir =tree1->Branch("Si31H_AmpDir",&Sil31H->AmpDir);
  TBranch *br_Sil31H_AmpIncl =tree1->Branch("Si31H_AmpIncl",&Sil31H->AmpIncl);
  TBranch *br_Sil31H_nHits =tree1->Branch("Si31H_nHits",&Sil31H->nHits);

  Inf *Sil32H = new Inf;
  TBranch *br_Sil32H_X =tree1->Branch("Si32H_X",&Sil32H->X);
  TBranch *br_Sil32H_Y =tree1->Branch("Si32H_Y",&Sil32H->Y);
  TBranch *br_Sil32H_St =tree1->Branch("Si32H_St",&Sil32H->St);
  TBranch *br_Sil32H_Mod =tree1->Branch("Si32H_Mod",&Sil32H->Mod);
  TBranch *br_Sil32H_L0H =tree1->Branch("Si32H_L0NHits",&Sil32H->L0H);
  TBranch *br_Sil32H_L1H =tree1->Branch("Si32H_L1NHits",&Sil32H->L1H);
  TBranch *br_Sil32H_AmpDir =tree1->Branch("Si32H_AmpDir",&Sil32H->AmpDir);
  TBranch *br_Sil32H_AmpIncl =tree1->Branch("Si32H_AmpIncl",&Sil32H->AmpIncl);
  TBranch *br_Sil32H_nHits =tree1->Branch("Si32H_nHits",&Sil32H->nHits);

  Inf *Sil41H = new Inf;
  TBranch *br_Sil41H_X =tree1->Branch("Si41H_X",&Sil41H->X);
  TBranch *br_Sil41H_Y =tree1->Branch("Si41H_Y",&Sil41H->Y);
  TBranch *br_Sil41H_St =tree1->Branch("Si41H_St",&Sil41H->St);
  TBranch *br_Sil41H_Mod =tree1->Branch("Si41H_Mod",&Sil41H->Mod);
  TBranch *br_Sil41H_L0H =tree1->Branch("Si41H_L0NHits",&Sil41H->L0H);
  TBranch *br_Sil41H_L1H =tree1->Branch("Si41H_L1NHits",&Sil41H->L1H);
  TBranch *br_Sil41H_AmpDir =tree1->Branch("Si41H_AmpDir",&Sil41H->AmpDir);
  TBranch *br_Sil41H_AmpIncl =tree1->Branch("Si41H_AmpIncl",&Sil41H->AmpIncl);
  TBranch *br_Sil41H_nHits =tree1->Branch("Si41H_nHits",&Sil41H->nHits);

  Inf *Sil42H = new Inf;
  TBranch *br_Sil42H_X =tree1->Branch("Si42H_X",&Sil42H->X);
  TBranch *br_Sil42H_Y =tree1->Branch("Si42H_Y",&Sil42H->Y);
  TBranch *br_Sil42H_St =tree1->Branch("Si42H_St",&Sil42H->St);
  TBranch *br_Sil42H_Mod =tree1->Branch("Si42H_Mod",&Sil42H->Mod);
  TBranch *br_Sil42H_L0H =tree1->Branch("Si42H_L0NHits",&Sil42H->L0H);
  TBranch *br_Sil42H_L1H =tree1->Branch("Si42H_L1NHits",&Sil42H->L1H);
  TBranch *br_Sil42H_AmpDir =tree1->Branch("Si42H_AmpDir",&Sil42H->AmpDir);
  TBranch *br_Sil42H_AmpIncl =tree1->Branch("Si42H_AmpIncl",&Sil42H->AmpIncl);
  TBranch *br_Sil42H_nHits =tree1->Branch("Si42H_nHits",&Sil42H->nHits);

  Track *MWPC_1 = new Track;
  TBranch *br_MWPC_1_Ntr =tree1->Branch("MWPC_1_Ntr",&MWPC_1->nTracks);
  TBranch *br_MWPC_1_X =tree1->Branch("MWPC_1_X",&MWPC_1->X);
  TBranch *br_MWPC_1_Y =tree1->Branch("MWPC_1_Y",&MWPC_1->Y);
  TBranch *br_MWPC_1_Z =tree1->Branch("MWPC_1_Z",&MWPC_1->Z);
  TBranch *br_MWPC_1_Tx =tree1->Branch("MWPC_1_Tx",&MWPC_1->Tx);
  TBranch *br_MWPC_1_Ty =tree1->Branch("MWPC_1_Ty",&MWPC_1->Ty);
  TBranch *br_MWPC_1_Chi2 =tree1->Branch("MWPC_1_Chi2",&MWPC_1->Chi2);
  TBranch *br_MWPC_1_NHits =tree1->Branch("MWPC_1_NHits",&MWPC_1->nHits);
  // TBranch *br_MWPC_1_IsGlob =tree1->Branch("MWPC_1_IsGlobalTrack",&MWPC_1->IsGlDCH);
  // TBranch *br_MWPC_1_NGlTracks =tree1->Branch("MWPC_1_NGlobalTr",&MWPC_1->nGlobDCHTracks);
  
  
  Track *MWPC_2 = new Track;
  TBranch *br_MWPC_2_Ntr =tree1->Branch("MWPC_2_Ntr",&MWPC_2->nTracks);
  TBranch *br_MWPC_2_X =tree1->Branch("MWPC_2_X",&MWPC_2->X);
  TBranch *br_MWPC_2_Y =tree1->Branch("MWPC_2_Y",&MWPC_2->Y);
  TBranch *br_MWPC_2_Z =tree1->Branch("MWPC_2_Z",&MWPC_2->Z);
  TBranch *br_MWPC_2_Tx =tree1->Branch("MWPC_2_Tx",&MWPC_2->Tx);
  TBranch *br_MWPC_2_Ty =tree1->Branch("MWPC_2_Ty",&MWPC_2->Ty);
  TBranch *br_MWPC_2_Chi2 =tree1->Branch("MWPC_2_Chi2",&MWPC_2->Chi2);
  TBranch *br_MWPC_2_nHits =tree1->Branch("MWPC_2_nHits",&MWPC_2->nHits);
  // TBranch *br_MWPC_2_IsGlob =tree1->Branch("MWPC_2_IsGlobalTrack",&MWPC_4->IsGlDCH);
  //TBranch *br_MWPC_2_NGlTracks =tree1->Branch("MWPC_2_NGlobalTr",&MWPC_4->nGlobDCHTracks);

  Track *MWPC_3 = new Track;
  TBranch *br_MWPC_3_Ntr =tree1->Branch("MWPC_3_Ntr",&MWPC_3->nTracks);
  TBranch *br_MWPC_3_X =tree1->Branch("MWPC_3_X",&MWPC_3->X);
  TBranch *br_MWPC_3_Y =tree1->Branch("MWPC_3_Y",&MWPC_3->Y);
  TBranch *br_MWPC_3_Z =tree1->Branch("MWPC_3_Z",&MWPC_3->Z);
  TBranch *br_MWPC_3_Tx =tree1->Branch("MWPC_3_Tx",&MWPC_3->Tx);
  TBranch *br_MWPC_3_Ty =tree1->Branch("MWPC_3_Ty",&MWPC_3->Ty);
  TBranch *br_MWPC_3_Chi2 =tree1->Branch("MWPC_3_Chi2",&MWPC_3->Chi2);
  TBranch *br_MWPC_3_NHits =tree1->Branch("MWPC_3_NHits",&MWPC_3->nHits);
  // TBranch *br_MWPC_3_IsGlob =tree1->Branch("MWPC_3_IsGlobalTrack",&MWPC_3->IsGlDCH);
  // TBranch *br_MWPC_3_NGlTracks =tree1->Branch("MWPC_3_NGlobalTr",&MWPC_3->nGlobDCHTracks);
  
  
  Track *MWPC_4 = new Track;
  TBranch *br_MWPC_4_Ntr =tree1->Branch("MWPC_4_Ntr",&MWPC_4->nTracks);
  TBranch *br_MWPC_4_X =tree1->Branch("MWPC_4_X",&MWPC_4->X);
  TBranch *br_MWPC_4_Y =tree1->Branch("MWPC_4_Y",&MWPC_4->Y);
  TBranch *br_MWPC_4_Z =tree1->Branch("MWPC_4_Z",&MWPC_4->Z);
  TBranch *br_MWPC_4_Tx =tree1->Branch("MWPC_4_Tx",&MWPC_4->Tx);
  TBranch *br_MWPC_4_Ty =tree1->Branch("MWPC_4_Ty",&MWPC_4->Ty);
  TBranch *br_MWPC_4_Chi2 =tree1->Branch("MWPC_4_Chi2",&MWPC_4->Chi2);
  TBranch *br_MWPC_4_nHits =tree1->Branch("MWPC_4_nHits",&MWPC_4->nHits);
  // TBranch *br_MWPC_4_IsGlob =tree1->Branch("MWPC_4_IsGlobalTrack",&MWPC_4->IsGlDCH);
  //TBranch *br_MWPC_4_NGlTracks =tree1->Branch("MWPC_4_NGlobalTr",&MWPC_4->nGlobDCHTracks);

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
  

  
  Track *MWPC_p1 = new Track;
  TBranch *br_MWPC_p1_Ntr =tree1->Branch("MWPC_p1_Ntr",&MWPC_p1->nTracks);
  TBranch *br_MWPC_p1_X =tree1->Branch("MWPC_p1_X",&MWPC_p1->X);
  TBranch *br_MWPC_p1_Y =tree1->Branch("MWPC_p1_Y",&MWPC_p1->Y);
  TBranch *br_MWPC_p1_Z =tree1->Branch("MWPC_p1_Z",&MWPC_p1->Z);
  TBranch *br_MWPC_p1_Tx =tree1->Branch("MWPC_p1_Tx",&MWPC_p1->Tx);
  TBranch *br_MWPC_p1_Ty =tree1->Branch("MWPC_p1_Ty",&MWPC_p1->Ty);
  TBranch *br_MWPC_p1_Chi2 =tree1->Branch("MWPC_p1_Chi2",&MWPC_p1->Chi2);
  TBranch *br_MWPC_p1_nHits =tree1->Branch("MWPC_p1_nHits",&MWPC_p1->nHits);
  // TBranch *br_MWPC_p1_IsGlob =tree1->Branch("MWPC_p1_IsGlobalTrack",&MWPC_p1->IsGlDCH);
  // TBranch *br_MWPC_p1_NGlTracks =tree1->Branch("MWPC_p1_NGlobalTr",&MWPC_p1->nGlobDCHTracks);

  Track *MWPC_p0 = new Track;
  TBranch *br_MWPC_p0_Ntr =tree1->Branch("MWPC_p0_Ntr",&MWPC_p0->nTracks);
  TBranch *br_MWPC_p0_X =tree1->Branch("MWPC_p0_X",&MWPC_p0->X);
  TBranch *br_MWPC_p0_Y =tree1->Branch("MWPC_p0_Y",&MWPC_p0->Y);
  TBranch *br_MWPC_p0_Z =tree1->Branch("MWPC_p0_Z",&MWPC_p0->Z);
  TBranch *br_MWPC_p0_Tx =tree1->Branch("MWPC_p0_Tx",&MWPC_p0->Tx);
  TBranch *br_MWPC_p0_Ty =tree1->Branch("MWPC_p0_Ty",&MWPC_p0->Ty);
  TBranch *br_MWPC_p0_Chi2 =tree1->Branch("MWPC_p0_Chi2",&MWPC_p0->Chi2);
  TBranch *br_MWPC_p0_nHits =tree1->Branch("MWPC_p0_nHits",&MWPC_p0->nHits);
  // TBranch *br_MWPC_p0_IsGlob =tree1->Branch("MWPC_p0_IsGlobalTrack",&MWPC_p0->IsGlDCH);
  // TBranch *br_MWPC_p0_NGlTracks =tree1->Branch("MWPC_p0_NGlobalTr",&MWPC_p0->nGlobDCHTracks);
  

  //new  downstream CSC data
  Int_t dCSCH_sec, dCSCH_nanosec;
  Short_t dCSCH_cscN;
  vector <Float_t> dCSCH_cscX, dCSCH_cscY, dCSCH_cscZ;
  TBranch *br_dCSCH_sec =tree1->Branch("dCSCH_sec",&dCSCH_sec);
  TBranch *br_dCSCH_nanosec =tree1->Branch("dCSCH_nanosec",&dCSCH_nanosec);
  TBranch *br_dCSCH_cscN =tree1->Branch("dCSCH_N",&dCSCH_cscN);
  TBranch *br_dCSCH_cscX =tree1->Branch("dCSCH_X",&dCSCH_cscX);
  TBranch *br_dCSCH_cscY =tree1->Branch("dCSCH_Y",&dCSCH_cscY);
  TBranch *br_dCSCH_cscZ =tree1->Branch("dCSCH_Z",&dCSCH_cscZ);

  //new GemCSC
  Int_t  eventId1_gemcsc, eventIdLeft, eventId1Left, /*eventId_tmp,*/ eventId1LR, eventIdLR;
  Int_t   sec_gemcsc, nanosec_gemcsc;
  Short_t rightGemN, rightCscN;
  Short_t leftGemN, leftCscN;
  Short_t leftGemN_header, leftCscN_header;
  vector <Float_t> rightGemX, rightGemY, rightGemZ, 
  leftGemX, leftGemY, leftGemZ,
  rightCscX, rightCscY, rightCscZ, leftCscX, leftCscY, leftCscZ;

  TBranch *br_Second =tree1->Branch("Second_gemCSC", &sec_gemcsc);
  TBranch *br_NanoSecond =tree1->Branch("NanoSecond_gemCSC", &nanosec_gemcsc);
  TBranch *br_RightGemN =tree1->Branch("RightGemN", &rightGemN);
  TBranch *br_RightGemX =tree1->Branch("RightGemX", &rightGemX);
  TBranch *br_RightGemY =tree1->Branch("RightGemY", &rightGemY);
  TBranch *br_RightGemZ =tree1->Branch("RightGemZ", &rightGemZ);
  TBranch *br_RightCscN =tree1->Branch("RightCscN", &rightCscN);
  TBranch *br_RightCscX =tree1->Branch("RightCscX", &rightCscX);
  TBranch *br_RightCscY =tree1->Branch("RightCscY", &rightCscY);
  TBranch *br_RightCscZ =tree1->Branch("RightCscZ", &rightCscZ);
   
  TBranch *br_LeftGemN =tree1->Branch("LeftGemN", &leftGemN);
  TBranch *br_LeftGemX =tree1->Branch("LeftGemX", &leftGemX);
  TBranch *br_LeftGemY =tree1->Branch("LeftGemY", &leftGemY);
  TBranch *br_LeftGemZ =tree1->Branch("LeftGemZ", &leftGemZ);
  TBranch *br_LeftCscN =tree1->Branch("LeftCscN", &leftCscN);
  TBranch *br_LeftCscX =tree1->Branch("LeftCscX", &leftCscX);
  TBranch *br_LeftCscY =tree1->Branch("LeftCscY", &leftCscY);
  TBranch *br_LeftCscZ =tree1->Branch("LeftCscZ", &leftCscZ);
  
  EventID=0;
 
  TClonesArray *dchTrack = new TClonesArray("BmnDchTrack");
  TClonesArray *mwpctrack = new TClonesArray("BmnMwpcTrack");
  TClonesArray *mwpcsegment = new TClonesArray("BmnMwpcSegment");
  TClonesArray *tof400 = new TClonesArray("BmnTofHit");
  DstEventHeader *evHeader = nullptr;

  tree->SetBranchAddress("DstEventHeader.", &evHeader);
  tree->SetBranchAddress("BmnDchTrack", &dchTrack);
  tree->SetBranchAddress("BmnMwpcTrack", &mwpctrack);
  tree->SetBranchAddress("BmnMwpcSegment", &mwpcsegment);
  tree->SetBranchAddress("BmnTof400Hit", &tof400);

  TBranch *br_Tof400_X =tree->Branch("Tof400_X",&Tof400_X);
  TBranch *br_Tof400_Y =tree->Branch("Tof400_Y",&Tof400_Y);
  TBranch *br_Tof400_Z =tree->Branch("Tof400_Z",&Tof400_Z);
  TBranch *br_Tof400_TS =tree->Branch("Tof400_TS",&Tof400_TS);
  TBranch *br_Tof400_DetId =tree->Branch("Tof400_DetId",&Tof400_DetId);
  TBranch *br_Tof400_Module =tree->Branch("Tof400_Module",&Tof400_Module);
  TBranch *br_Tof400_Station =tree->Branch("Tof400_Station",&Tof400_Station);
 

  Int_t Si_index;
  Int_t Event_ID; 
  vector <Double_t> *X=0; 
  vector <Double_t> *Y=0;
  Int_t St;
  Int_t Mod; 
  vector <Int_t> *L0NHits=0; 
  vector <Int_t> *L1NHits=0; 
  vector <Double_t> *AmpDirect=0; 
  vector <Double_t> *AmpInclined=0; 

  TString Si_FileName(SiDigiDir_root + "/" + to_string(filenum) +"_silicon.root"); //cout << "was opened: " << Si_FileName << endl; 
  TFile *Si_root = new TFile (Si_FileName); TTree *tsi;

  if (!Si_root->IsOpen())
  {
    cout<<Si_FileName<<" not open!!!"<<endl;  flag_Si=1; 
  } 
  if (!Si_root->Get("Silicon")) 
  {
    cout<<Si_FileName<<" problem bmndata tree!!!"<<endl; flag_Si=1;
  }
   if(flag_Si!=1)
    {
      tsi = (TTree*)Si_root ->Get("Silicon");  cout<<"entries in Si file "<<tsi->GetEntries()<<endl;
      
      tsi->SetBranchAddress("Event_ID", &Event_ID);
      tsi->SetBranchAddress("X", &X);
      tsi->SetBranchAddress("Y", &Y);
      tsi->SetBranchAddress("St",&St);
      tsi->SetBranchAddress("Mod", &Mod);
      tsi->SetBranchAddress("Si_index", &Si_index);
      tsi->SetBranchAddress("L0NHits", &L0NHits);
      tsi->SetBranchAddress("L1NHits", &L1NHits);
      tsi->SetBranchAddress("AmpDirect", &AmpDirect);
      tsi->SetBranchAddress("AmpInclined", &AmpInclined);  

    }  

  if(flag_Si!=1 && tsi->GetEntries()<1) 
  {
    cout<<Si_FileName<<" zero events in tree "<<endl; flag_Si=1;
  }
  

  Int_t eventId, runId;
  Int_t sec, nanosec;
  Short_t cscN;
  vector <Float_t> *cscX=0, *cscY=0, *cscZ_BMN=0;
  
  TString DownCSCFileName(D_CSC_DigiDir_root + "/DownstreamCscRun" + std::to_string(filenum) + ".root"); 
  TFile *DownCSCfile = new TFile (DownCSCFileName); TTree *tDownCSC;
  
  if (!DownCSCfile->IsOpen())
  {
    cout<<DownCSCFileName<<" not open!!!"<<endl;  flag_downCSC=1;
  } 
  if (!DownCSCfile->Get("FragmentCscTree"))
  {
    cout<<DownCSCFileName<<" problem FragmentCscTree!!!"<<endl; flag_downCSC=1;
  }    
  else
    {
      tDownCSC = (TTree*)DownCSCfile ->Get("FragmentCscTree"); cout<<"entries in downstream CSC file "<<tDownCSC->GetEntries()<<endl;
      
      tDownCSC->SetBranchAddress("RunId", &runId);
      tDownCSC->SetBranchAddress("EventId", &eventId);
      tDownCSC->SetBranchAddress("Second", &sec);
      tDownCSC->SetBranchAddress("NanoSecond", &nanosec);
      tDownCSC->SetBranchAddress("CscN", &cscN);
      tDownCSC->SetBranchAddress("CscX", &cscX);
      tDownCSC->SetBranchAddress("CscY", &cscY);
      tDownCSC->SetBranchAddress("CscZ", &cscZ_BMN);
    }

  if(flag_downCSC!=1 &&  tDownCSC->GetEntries()<1)
  {
    cout<<DownCSCFileName<<" zero events in tree "<<endl; flag_downCSC=1;
  }

  TString gemCSCFileName_Right(GEM_CSC_DigiDir_root + "/GemCscRuns_Right_" + std::to_string(filenum) + ".root"); 
  //TFile *gemCSCfile_Right = new TFile (gemCSCFileName_Right); 
  TFile *gemCSCfile_Right = new TFile (gemCSCFileName_Right); TTree *tgemCSC_Right;

  Int_t  eventId1_txtroot_Right, eventId_txtroot_Right;
  Int_t   sec_gemcsc_txtroot_Right, nanosec_gemcsc_txtroot_Right;
  Short_t rightGemN_txtroot, rightCscN_txtroot;
  vector <Float_t> *rightGemX_txtroot=0, *rightGemY_txtroot=0, *rightGemZ_txtroot=0; 
  vector <Float_t> *rightCscX_txtroot=0, *rightCscY_txtroot=0, *rightCscZ_txtroot=0;


  if (!gemCSCfile_Right || gemCSCfile_Right->IsZombie()) 
  {
     cout<<gemCSCFileName_Right<<" not open!!!"<<endl;    flag_Right_gemcsc=1; 
  }

  if (!gemCSCfile_Right->Get("GemCscTree")) 
  {
    cout<<gemCSCFileName_Right<<" problem bmndata tree!!!"<<endl; flag_Right_gemcsc=1;
  }
  else{
    tgemCSC_Right = (TTree*)gemCSCfile_Right->Get("GemCscTree");  cout<<"entries in gemCSC right "<<tgemCSC_Right->GetEntries()<<endl;


    tgemCSC_Right->SetBranchAddress("EventId1", &eventId1_txtroot_Right);
    tgemCSC_Right->SetBranchAddress("Second", &sec_gemcsc_txtroot_Right);
    tgemCSC_Right->SetBranchAddress("NanoSecond", &nanosec_gemcsc_txtroot_Right);
    tgemCSC_Right->SetBranchAddress("RightGemN", &rightGemN_txtroot);
    tgemCSC_Right->SetBranchAddress("RightGemX", &rightGemX_txtroot);
    tgemCSC_Right->SetBranchAddress("RightGemY", &rightGemY_txtroot);
    tgemCSC_Right->SetBranchAddress("RightGemZ", &rightGemZ_txtroot);
    tgemCSC_Right->SetBranchAddress("RightCscN", &rightCscN_txtroot);
    tgemCSC_Right->SetBranchAddress("RightCscX", &rightCscX_txtroot);
    tgemCSC_Right->SetBranchAddress("RightCscY", &rightCscY_txtroot);
    tgemCSC_Right->SetBranchAddress("RightCscZ", &rightCscZ_txtroot);

  }

  if(flag_Right_gemcsc!=1 && tgemCSC_Right->GetEntries()<1) 
  {
    cout<<gemCSCFileName_Right<<" zero events in tree "<<endl; flag_Right_gemcsc=1;
  }

   

  TString gemCSCFileName_Left(GEM_CSC_DigiDir_root + "/GemCscRuns_Left_" + std::to_string(filenum) + ".root"); 
  TFile *gemCSCfile_Left = new TFile (gemCSCFileName_Left); TTree *tgemCSC_Left;

  Int_t  eventId1_txtroot_Left, eventId_txtroot_Left;
  Int_t   sec_gemcsc_txtroot_Left, nanosec_gemcsc_txtroot_Left;
  Short_t leftGemN_txtroot, leftCscN_txtroot;
  vector <Float_t> *leftGemX_txtroot=0, *leftGemY_txtroot=0, *leftGemZ_txtroot=0; 
  vector <Float_t> *leftCscX_txtroot=0, *leftCscY_txtroot=0, *leftCscZ_txtroot=0;


  if (!gemCSCfile_Left || gemCSCfile_Left->IsZombie()) 
  {
     cout<<gemCSCFileName_Left<<" not open!!!"<<endl;    flag_Left_gemcsc=1; 
  }

  if (!gemCSCfile_Left->Get("GemCscTree")) 
  {
    cout<<gemCSCFileName_Left<<" problem bmndata tree!!!"<<endl; flag_Left_gemcsc=1;
  }
  else{
    tgemCSC_Left = (TTree*)gemCSCfile_Left->Get("GemCscTree"); cout<<"entries in gemCSC left "<<tgemCSC_Left->GetEntries()<<endl;

    tgemCSC_Left->SetBranchAddress("EventId1", &eventId1_txtroot_Left);
    tgemCSC_Left->SetBranchAddress("Second", &sec_gemcsc_txtroot_Left);
    tgemCSC_Left->SetBranchAddress("NanoSecond", &nanosec_gemcsc_txtroot_Left);
    tgemCSC_Left->SetBranchAddress("LeftGemN", &leftGemN_txtroot);
    tgemCSC_Left->SetBranchAddress("LeftGemX", &leftGemX_txtroot);
    tgemCSC_Left->SetBranchAddress("LeftGemY", &leftGemY_txtroot);
    tgemCSC_Left->SetBranchAddress("LeftGemZ", &leftGemZ_txtroot);
    tgemCSC_Left->SetBranchAddress("LeftCscN", &leftCscN_txtroot);
    tgemCSC_Left->SetBranchAddress("LeftCscX", &leftCscX_txtroot);
    tgemCSC_Left->SetBranchAddress("LeftCscY", &leftCscY_txtroot);
    tgemCSC_Left->SetBranchAddress("LeftCscZ", &leftCscZ_txtroot);

  }

  if(flag_Left_gemcsc!=1 && tgemCSC_Left->GetEntries()<1) 
  {
    cout<<gemCSCFileName_Left<<" zero events in tree "<<endl; flag_Left_gemcsc=1;
  }


  
    //sorting events one after another

  if(flag_Si!=1) tsi->BuildIndex("Event_ID","Si_index");
  if(flag_downCSC!=1) tDownCSC->BuildIndex("EventId"); 
  if(flag_Left_gemcsc!=1) tgemCSC_Left->BuildIndex("EventId1");///sau care event ID?  
  if(flag_Right_gemcsc!=1) tgemCSC_Right->BuildIndex("EventId1");

  for (int i=0; i<NEv; i++)  //cycle by events
  {
    if (i%10000==0) {cout<<i<<" "; printf( " Real time %f s, CPU time %f s\n", timer.RealTime(), timer.CpuTime()); timer.Continue();}

    Sil11H->X.clear();
    Sil11H->Y.clear();
    Sil11H->St.clear();
    Sil11H->Mod.clear();
    Sil11H->L0H.clear();
    Sil11H->L1H.clear();
    Sil11H->AmpDir.clear();
    Sil11H->AmpIncl.clear();
    Sil11H->nHits=0;

    Sil12H->X.clear();
    Sil12H->Y.clear();
    Sil12H->St.clear();
    Sil12H->Mod.clear();
    Sil12H->L0H.clear();
    Sil12H->L1H.clear();
    Sil12H->AmpDir.clear();
    Sil12H->AmpIncl.clear();
    Sil12H->nHits=0;

    Sil21H->X.clear();
    Sil21H->Y.clear();
    Sil21H->St.clear();
    Sil21H->Mod.clear();
    Sil21H->L0H.clear();
    Sil21H->L1H.clear();
    Sil21H->AmpDir.clear();
    Sil21H->AmpIncl.clear();
    Sil21H->nHits=0;

    Sil22H->X.clear();
    Sil22H->Y.clear();
    Sil22H->St.clear();
    Sil22H->Mod.clear();
    Sil22H->L0H.clear();
    Sil22H->L1H.clear();
    Sil22H->AmpDir.clear();
    Sil22H->AmpIncl.clear();
    Sil22H->nHits=0;

    Sil31H->X.clear();
    Sil31H->Y.clear();
    Sil31H->St.clear();
    Sil31H->Mod.clear();
    Sil31H->L0H.clear();
    Sil31H->L1H.clear();
    Sil31H->AmpDir.clear();
    Sil31H->AmpIncl.clear();
    Sil31H->nHits=0;

    Sil32H->X.clear();
    Sil32H->Y.clear();
    Sil32H->St.clear();
    Sil32H->Mod.clear();
    Sil32H->L0H.clear();
    Sil32H->L1H.clear();
    Sil32H->AmpDir.clear();
    Sil32H->AmpIncl.clear();
    Sil32H->nHits=0;

    Sil41H->X.clear();
    Sil41H->Y.clear();
    Sil41H->St.clear();
    Sil41H->Mod.clear();
    Sil41H->L0H.clear();
    Sil41H->L1H.clear();
    Sil41H->AmpDir.clear();
    Sil41H->AmpIncl.clear();
    Sil41H->nHits=0;

    Sil42H->X.clear();
    Sil42H->Y.clear();
    Sil42H->St.clear();
    Sil42H->Mod.clear();
    Sil42H->L0H.clear();
    Sil42H->L1H.clear();
    Sil42H->AmpDir.clear();
    Sil42H->AmpIncl.clear();
    Sil42H->nHits=0;

    Sil1H->X.clear();
    Sil1H->Y.clear();
    Sil1H->St.clear();
    Sil1H->Mod.clear();
    Sil1H->L0H.clear();
    Sil1H->L1H.clear();
    Sil1H->AmpDir.clear();
    Sil1H->AmpIncl.clear();
    Sil1H->nHits=0;

    Sil2H->X.clear();
    Sil2H->Y.clear();
    Sil2H->St.clear();
    Sil2H->Mod.clear();
    Sil2H->L0H.clear();
    Sil2H->L1H.clear();
    Sil2H->AmpDir.clear();
    Sil2H->AmpIncl.clear();
    Sil2H->nHits=0;


    Sil3H->X.clear();
    Sil3H->Y.clear();
    Sil3H->St.clear();
    Sil3H->Mod.clear();
    Sil3H->L0H.clear();
    Sil3H->L1H.clear();
    Sil3H->AmpDir.clear();
    Sil3H->AmpIncl.clear();
    Sil3H->nHits=0;

    Sil4H->X.clear();
    Sil4H->Y.clear();
    Sil4H->St.clear();
    Sil4H->Mod.clear();
    Sil4H->L0H.clear();
    Sil4H->L1H.clear();
    Sil4H->AmpDir.clear();
    Sil4H->AmpIncl.clear();
    Sil4H->nHits=0;

    tree->GetEntry(i-1);
    EventID_Bmn=evHeader->GetEventId();
    //EventID=EventID+1;
    TS_sec=evHeader->GetEventTimeTS().GetSec();
    TS_nsec=evHeader->GetEventTimeTS().GetNanoSec();

    run=filenum;
     
    BC1=evHeader->GetBC1_12();
    BC2=evHeader->GetBC2_12();
    BC3=evHeader->GetBC3_12();
    BC4=evHeader->GetBC4_12();
    BC5=evHeader->GetBC5_12();
    chargein=evHeader->GetZinBC12();
    charge34=evHeader->GetZoutBC34_12();
    charge35=evHeader->GetZoutBC35_12();
    charge45=evHeader->GetZoutBC45_12();
    BC1_cor=evHeader->GetBC1_cor();
    BC2_cor=evHeader->GetBC2_cor();
    BC3_cor=evHeader->GetBC3_cor();
    BC4_cor=evHeader->GetBC4_cor();
    BC5_cor=evHeader->GetBC5_cor();
    chargein_cor=evHeader->GetZinBC12_cor();
    charge34_cor=evHeader->GetZoutBC34_12_cor();
    charge35_cor=evHeader->GetZoutBC35_12_cor();
    charge45_cor=evHeader->GetZoutBC45_12_cor();
    trigger=evHeader->GetTriger();
    veto=evHeader->GetVeto();

    for (int i=0; i<evHeader->GetScWallELoss().size(); i++) {
      SW_ELoss.push_back(evHeader->GetScWallELoss().at(i));
    }
    for (int i=0; i<evHeader->GetScWallCellId().size(); i++) {
      SW_CellId.push_back(evHeader->GetScWallCellId().at(i));
    }
    for (int i=0; i<evHeader->GetScWallFlag().size(); i++) {
      SW_Flag.push_back(evHeader->GetScWallFlag().at(i));
    }
    for (int i=0; i<evHeader->GetScWallTrackId().size(); i++) {
      SW_TrId.push_back(evHeader->GetScWallTrackId().at(i));
    }
    for (int i=0; i<evHeader->GetScWallOrderId().size(); i++) {
      SW_OrderId.push_back(evHeader->GetScWallOrderId().at(i));
    }
      
    SW_Mult=evHeader->GetScWallMult();
    SW_NMatchTr=evHeader->GetScWallNMatchTracks();

    for (int i=0; i<evHeader->GetVBCellId().size(); i++) {
      VB_CellId.push_back(evHeader->GetVBCellId().at(i));
    }
    VB_Mult=evHeader->GetVBMult();

    MWPC_1->nTracks=0;
    MWPC_2->nTracks=0;
    MWPC_3->nTracks=0;
    MWPC_4->nTracks=0;

    for (int k=0; k<mwpcsegment->GetEntries(); k++){
      BmnMwpcSegment *mwpcseg = (BmnMwpcSegment *)mwpcsegment->At(k);
           
        if(mwpcseg->GetParamFirst()->GetZ()<-800){
          MWPC_1->X.push_back(-mwpcseg->GetParamFirst()->GetX());
          MWPC_1->Y.push_back(mwpcseg->GetParamFirst()->GetY());
          MWPC_1->Z.push_back(mwpcseg->GetParamFirst()->GetZ());
          MWPC_1->Tx.push_back(-mwpcseg->GetParamFirst()->GetTx());
          MWPC_1->Ty.push_back(mwpcseg->GetParamFirst()->GetTy()); 
          MWPC_1->Chi2.push_back(mwpcseg->GetChi2());
          MWPC_1->nHits.push_back(mwpcseg->GetNHits());  
          MWPC_1->nTracks=MWPC_1->nTracks+1;
        } 

        if(mwpcseg->GetParamFirst()->GetZ()>-800 && mwpcseg->GetParamFirst()->GetZ()<-700){
          MWPC_2->X.push_back(-mwpcseg->GetParamFirst()->GetX());
          MWPC_2->Y.push_back(mwpcseg->GetParamFirst()->GetY());
          MWPC_2->Z.push_back(mwpcseg->GetParamFirst()->GetZ());
          MWPC_2->Tx.push_back(-mwpcseg->GetParamFirst()->GetTx());
          MWPC_2->Ty.push_back(mwpcseg->GetParamFirst()->GetTy()); 
          MWPC_2->Chi2.push_back(mwpcseg->GetChi2());
          MWPC_2->nHits.push_back(mwpcseg->GetNHits());  
          MWPC_2->nTracks=MWPC_2->nTracks+1;
        } 
    }
   
    for (int k=0; k<mwpcsegment->GetEntries(); k++){
      BmnMwpcSegment *mwpcseg = (BmnMwpcSegment *)mwpcsegment->At(k);
           
        if(mwpcseg->GetParamFirst()->GetZ()>-400 && mwpcseg->GetParamFirst()->GetZ()<-300){
          MWPC_3->X.push_back(-mwpcseg->GetParamFirst()->GetX());
          MWPC_3->Y.push_back(mwpcseg->GetParamFirst()->GetY());
          MWPC_3->Z.push_back(mwpcseg->GetParamFirst()->GetZ());
          MWPC_3->Tx.push_back(-mwpcseg->GetParamFirst()->GetTx());
          MWPC_3->Ty.push_back(mwpcseg->GetParamFirst()->GetTy()); 
          MWPC_3->Chi2.push_back(mwpcseg->GetChi2());
          MWPC_3->nHits.push_back(mwpcseg->GetNHits());  
          MWPC_3->nTracks=MWPC_3->nTracks+1;
        } 

        if(mwpcseg->GetParamFirst()->GetZ()>-300){
          MWPC_4->X.push_back(-mwpcseg->GetParamFirst()->GetX());
          MWPC_4->Y.push_back(mwpcseg->GetParamFirst()->GetY());
          MWPC_4->Z.push_back(mwpcseg->GetParamFirst()->GetZ());
          MWPC_4->Tx.push_back(-mwpcseg->GetParamFirst()->GetTx());
          MWPC_4->Ty.push_back(mwpcseg->GetParamFirst()->GetTy()); 
          MWPC_4->Chi2.push_back(mwpcseg->GetChi2());
          MWPC_4->nHits.push_back(mwpcseg->GetNHits());  
          MWPC_4->nTracks=MWPC_4->nTracks+1;
        } 
    }

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

    MWPC_p1->nTracks=0;
    MWPC_p0->nTracks=0;
       
    for (int k=0; k<mwpctrack->GetEntries(); k++){
          BmnMwpcTrack *mwpc = (BmnMwpcTrack *)mwpctrack->At(k);
           
          if(mwpc->GetParamFirst()->GetZ()>-400){
            MWPC_p1->X.push_back(-mwpc->GetParamFirst()->GetX());
            MWPC_p1->Y.push_back(mwpc->GetParamFirst()->GetY());
            MWPC_p1->Z.push_back(mwpc->GetParamFirst()->GetZ());
            MWPC_p1->Tx.push_back(-mwpc->GetParamFirst()->GetTx());
            MWPC_p1->Ty.push_back(mwpc->GetParamFirst()->GetTy()); 
            MWPC_p1->Chi2.push_back(mwpc->GetChi2());
            MWPC_p1->nHits.push_back(mwpc->GetNHits());  
            MWPC_p1->nTracks=MWPC_p1->nTracks+1;
          } 
          if(mwpc->GetParamFirst()->GetZ()<-400){
            MWPC_p0->X.push_back(-mwpc->GetParamFirst()->GetX());
            MWPC_p0->Y.push_back(mwpc->GetParamFirst()->GetY());
            MWPC_p0->Z.push_back(mwpc->GetParamFirst()->GetZ());
            MWPC_p0->Tx.push_back(-mwpc->GetParamFirst()->GetTx());
            MWPC_p0->Ty.push_back(mwpc->GetParamFirst()->GetTy()); 
            MWPC_p0->Chi2.push_back(mwpc->GetChi2());
            MWPC_p0->nHits.push_back(mwpc->GetNHits());  
            MWPC_p0->nTracks=MWPC_p0->nTracks+1;
          } 
    }

    LeftTof400_nHits=0; RightTof400_nHits=0;
    for (int k=0; k<tof400->GetEntries(); k++){
          BmnTofHit *tof400hit = (BmnTofHit *)tof400->At(k);

          if (tof400hit->GetX() > 0 )
          {
            LeftTof400_X.push_back(tof400hit->GetX());
            LeftTof400_Y.push_back(tof400hit->GetY());
            LeftTof400_Z.push_back(tof400hit->GetZ());
            LeftTof400_TS.push_back(tof400hit->GetTimeStamp());
            LeftTof400_DetId.push_back(tof400hit->GetDetectorID()); 
            LeftTof400_Module.push_back(tof400hit->GetModule());
            LeftTof400_Station.push_back(tof400hit->GetStation());
            LeftTof400_nHits++;
          }

          if (tof400hit->GetX() < 0 )
          {
            RightTof400_X.push_back(tof400hit->GetX());
            RightTof400_Y.push_back(tof400hit->GetY());
            RightTof400_Z.push_back(tof400hit->GetZ());
            RightTof400_TS.push_back(tof400hit->GetTimeStamp());
            RightTof400_DetId.push_back(tof400hit->GetDetectorID()); 
            RightTof400_Module.push_back(tof400hit->GetModule());
            RightTof400_Station.push_back(tof400hit->GetStation());
            RightTof400_nHits++;
          }
          
    }

    BC1_Time0 = evHeader->GetBC1_Time0(); //
    BC1_Time1 = evHeader->GetBC1_Time1();
    BC2_Time0 = evHeader->GetBC2_Time0();
    BC2_Time1 = evHeader->GetBC2_Time1();
    TofCal_nHits = evHeader->GetTofCal_nHits();
    Is_laser = evHeader->GetIs_laser();

    TofCal_Bar = evHeader->GetTofCal_Bar();
    TofCal_Arm = evHeader->GetTofCal_Arm();
    TofCal_GlBar = evHeader->GetTofCal_GlobalBar();
    TofCal_Plane = evHeader->GetTofCal_Plane();
    TofCal_Time0 = evHeader->GetTofCal_Time0();
    TofCal_Time1 = evHeader->GetTofCal_Time1();
    TofCal_Amp0 = evHeader->GetTofCal_Amp0();
    TofCal_Amp1 = evHeader->GetTofCal_Amp1();
    TofCal_X = evHeader->GetTofCal_X();
    TofCal_Y = evHeader->GetTofCal_Y();  // cout<<TofCal_Y<<" ";

 
    

    // for(int j=0;j<TofCal_nHits;j++)
    // {

    //   TofCal_Bar.push_back(evHeader->GetTofCal_Bar().at(j));
    //   TofCal_Arm.push_back(evHeader->GetTofCal_Arm().at(j));
    //   TofCal_GlBar.push_back(evHeader->GetTofCal_GlobalBar().at(j));
    //   TofCal_Plane.push_back(evHeader->GetTofCal_Plane().at(j));
    //   TofCal_Time0.push_back(evHeader->GetTofCal_Time0().at(j));
    //   TofCal_Time1.push_back(evHeader->GetTofCal_Time1().at(j));
    //   TofCal_Amp0.push_back(evHeader->GetTofCal_Amp0().at(j));
    //   TofCal_Amp1.push_back(evHeader->GetTofCal_Amp1().at(j));
    //   TofCal_X.push_back(evHeader->GetTofCal_X().at(j));
    //   TofCal_Y.push_back(evHeader->GetTofCal_Y().at(j)); 
    // }


    


    if( flag_Si != 1)
    {
      X->clear(); Y->clear(); L0NHits->clear(); L1NHits->clear(); AmpDirect->clear(); AmpInclined->clear();
    }
    
    auto SiTreeIndex_900 = -10;
    if(flag_Si != 1) SiTreeIndex_900 = tsi->GetEntryNumberWithIndex (EventID_Bmn, 900); 
    if(flag_Si != 1 && SiTreeIndex_900 > -1)
    {
      tsi->GetEntry(SiTreeIndex_900);
      for(int j=0;j<X->size();j++)
      {
        //cout<<X->at(j)<<" ";
        Sil11H->Y.push_back(X->at(j)-6.3);//-0.228
        Sil11H->X.push_back(-(Y->at(j)-6.3));
        Sil11H->St.push_back(St+1);
        Sil11H->Mod.push_back(Mod);
        Sil11H->L0H.push_back(L0NHits->at(j));
        Sil11H->L1H.push_back(L1NHits->at(j));
        Sil11H->AmpDir.push_back(AmpDirect->at(j));
        Sil11H->AmpIncl.push_back(AmpInclined->at(j));
        Sil11H->nHits=Sil11H->nHits+1;
        
        Sil1H->Y.push_back(X->at(j)-6.3);//-0.228
        Sil1H->X.push_back(-(Y->at(j)-6.3));
        Sil1H->St.push_back(St+1);
        Sil1H->Mod.push_back(Mod);
        Sil1H->L0H.push_back(L0NHits->at(j));
        Sil1H->L1H.push_back(L1NHits->at(j));
        Sil1H->AmpDir.push_back(AmpDirect->at(j));
        Sil1H->AmpIncl.push_back(AmpInclined->at(j));
        Sil1H->nHits=Sil1H->nHits+1;
      }
    }

    auto SiTreeIndex_901 = -10;
    if(flag_Si != 1) SiTreeIndex_901 = tsi->GetEntryNumberWithIndex (EventID_Bmn, 901); 
    if(flag_Si != 1 && SiTreeIndex_901 > -1)
    {
      tsi->GetEntry(SiTreeIndex_901);
      for(int j=0;j<X->size();j++)
      {
        Sil12H->Y.push_back(X->at(j)-6.3);//-0.145
        Sil12H->X.push_back(-(Y->at(j)-6.3));
        Sil12H->St.push_back(St+1);
        Sil12H->Mod.push_back(Mod);
        Sil12H->L0H.push_back(L0NHits->at(j));
        Sil12H->L1H.push_back(L1NHits->at(j));
        Sil12H->AmpDir.push_back(AmpDirect->at(j));
        Sil12H->AmpIncl.push_back(AmpInclined->at(j));
        Sil12H->nHits=Sil12H->nHits+1;

        Sil1H->Y.push_back(X->at(j)-6.3);//-0.145
        Sil1H->X.push_back(-(Y->at(j)-6.3));
        Sil1H->St.push_back(St+1);
        Sil1H->Mod.push_back(Mod);
        Sil1H->L0H.push_back(L0NHits->at(j));
        Sil1H->L1H.push_back(L1NHits->at(j));
        Sil1H->AmpDir.push_back(AmpDirect->at(j));
        Sil1H->AmpIncl.push_back(AmpInclined->at(j));
        Sil1H->nHits=Sil1H->nHits+1;
      }
    }

    auto SiTreeIndex_910 = -10;
    if(flag_Si != 1) SiTreeIndex_910 = tsi->GetEntryNumberWithIndex (EventID_Bmn, 910); 
    if(flag_Si != 1 && SiTreeIndex_910 > -1)
    {
      tsi->GetEntry(SiTreeIndex_910);
      for(int j=0;j<X->size();j++)
      {
        Sil21H->Y.push_back(Y->at(j)-6.3);
        Sil21H->X.push_back(-(X->at(j)-6.3));//-0.208
        Sil21H->St.push_back(St+1);
        Sil21H->Mod.push_back(Mod);
        Sil21H->L0H.push_back(L0NHits->at(j));
        Sil21H->L1H.push_back(L1NHits->at(j));
        Sil21H->AmpDir.push_back(AmpDirect->at(j));
        Sil21H->AmpIncl.push_back(AmpInclined->at(j));
        Sil21H->nHits=Sil21H->nHits+1;

        Sil2H->Y.push_back(Y->at(j)-6.3);
        Sil2H->X.push_back(-(X->at(j)-6.3));//-0.208
        Sil2H->St.push_back(St+1);
        Sil2H->Mod.push_back(Mod);
        Sil2H->L0H.push_back(L0NHits->at(j));
        Sil2H->L1H.push_back(L1NHits->at(j));
        Sil2H->AmpDir.push_back(AmpDirect->at(j));
        Sil2H->AmpIncl.push_back(AmpInclined->at(j));
        Sil2H->nHits=Sil2H->nHits+1;
      }
    }

    auto SiTreeIndex_911 = -10;
    if(flag_Si != 1) SiTreeIndex_911 = tsi->GetEntryNumberWithIndex (EventID_Bmn, 911); 
    if(flag_Si != 1 && SiTreeIndex_911 > -1)
    {
      tsi->GetEntry(SiTreeIndex_911);
      for(int j=0;j<X->size();j++)
      {
        Sil22H->Y.push_back(Y->at(j)-6.3);
        Sil22H->X.push_back(-(X->at(j)-6.3));//-0.145
        Sil22H->St.push_back(St+1);
        Sil22H->Mod.push_back(Mod);
        Sil22H->L0H.push_back(L0NHits->at(j));
        Sil22H->L1H.push_back(L1NHits->at(j));
        Sil22H->AmpDir.push_back(AmpDirect->at(j));
        Sil22H->AmpIncl.push_back(AmpInclined->at(j));
        Sil22H->nHits=Sil22H->nHits+1;

        Sil2H->Y.push_back(Y->at(j)-6.3);
        Sil2H->X.push_back(-(X->at(j)-6.3));//-0.145
        Sil2H->St.push_back(St+1);
        Sil2H->Mod.push_back(Mod);
        Sil2H->L0H.push_back(L0NHits->at(j));
        Sil2H->L1H.push_back(L1NHits->at(j));
        Sil2H->AmpDir.push_back(AmpDirect->at(j));
        Sil2H->AmpIncl.push_back(AmpInclined->at(j));
        Sil2H->nHits=Sil2H->nHits+1;
      }
    }

    auto SiTreeIndex_920 = -10;
    if(flag_Si != 1) SiTreeIndex_920 = tsi->GetEntryNumberWithIndex (EventID_Bmn, 920); 
    if(flag_Si != 1 && SiTreeIndex_920 > -1)
    {
      tsi->GetEntry(SiTreeIndex_920);
      for(int j=0;j<X->size();j++)
      {
        Sil31H->Y.push_back(X->at(j)-6.3);//-0.192
        Sil31H->X.push_back(-(Y->at(j)-6.3));
        Sil31H->St.push_back(St+1);
        Sil31H->Mod.push_back(Mod);
        Sil31H->L0H.push_back(L0NHits->at(j));
        Sil31H->L1H.push_back(L1NHits->at(j));
        Sil31H->AmpDir.push_back(AmpDirect->at(j));
        Sil31H->AmpIncl.push_back(AmpInclined->at(j));
        Sil31H->nHits=Sil31H->nHits+1;

        Sil3H->Y.push_back(X->at(j)-6.3);//-0.192
        Sil3H->X.push_back(-(Y->at(j)-6.3));
        Sil3H->St.push_back(St+1);
        Sil3H->Mod.push_back(Mod);
        Sil3H->L0H.push_back(L0NHits->at(j));
        Sil3H->L1H.push_back(L1NHits->at(j));
        Sil3H->AmpDir.push_back(AmpDirect->at(j));
        Sil3H->AmpIncl.push_back(AmpInclined->at(j));
        Sil3H->nHits=Sil3H->nHits+1;
      }
    }

    auto SiTreeIndex_921 = -10;
    if(flag_Si != 1) SiTreeIndex_921 = tsi->GetEntryNumberWithIndex (EventID_Bmn, 921); 
    if(flag_Si != 1 && SiTreeIndex_921 > -1)
    {
      tsi->GetEntry(SiTreeIndex_921);
      for(int j=0;j<X->size();j++)
      {
        Sil32H->Y.push_back(X->at(j)-6.3);//-0.115
        Sil32H->X.push_back(-(Y->at(j)-6.3));
        Sil32H->St.push_back(St+1);
        Sil32H->Mod.push_back(Mod);
        Sil32H->L0H.push_back(L0NHits->at(j));
        Sil32H->L1H.push_back(L1NHits->at(j));
        Sil32H->AmpDir.push_back(AmpDirect->at(j));
        Sil32H->AmpIncl.push_back(AmpInclined->at(j));
        Sil32H->nHits=Sil32H->nHits+1;

        Sil3H->Y.push_back(X->at(j)-6.3);//-0.115
        Sil3H->X.push_back(-(Y->at(j)-6.3));
        Sil3H->St.push_back(St+1);
        Sil3H->Mod.push_back(Mod);
        Sil3H->L0H.push_back(L0NHits->at(j));
        Sil3H->L1H.push_back(L1NHits->at(j));
        Sil3H->AmpDir.push_back(AmpDirect->at(j));
        Sil3H->AmpIncl.push_back(AmpInclined->at(j));
        Sil3H->nHits=Sil3H->nHits+1;
      }
    }

    auto SiTreeIndex_930 = -10;
    if(flag_Si != 1) SiTreeIndex_930 = tsi->GetEntryNumberWithIndex (EventID_Bmn, 930); 
    if(flag_Si != 1 && SiTreeIndex_930 > -1)
    {
      tsi->GetEntry(SiTreeIndex_930);
      for(int j=0;j<X->size();j++)
      {
        Sil41H->Y.push_back(Y->at(j)-6.3);
        Sil41H->X.push_back(-(X->at(j)-6.3));//-0.215
        Sil41H->St.push_back(St+1);
        Sil41H->Mod.push_back(Mod);
        Sil41H->L0H.push_back(L0NHits->at(j));
        Sil41H->L1H.push_back(L1NHits->at(j));
        Sil41H->AmpDir.push_back(AmpDirect->at(j));
        Sil41H->AmpIncl.push_back(AmpInclined->at(j));
        Sil41H->nHits=Sil41H->nHits+1;

        Sil4H->Y.push_back(Y->at(j)-6.3);
        Sil4H->X.push_back(-(X->at(j)-6.3));//-0.215
        Sil4H->St.push_back(St+1);
        Sil4H->Mod.push_back(Mod);
        Sil4H->L0H.push_back(L0NHits->at(j));
        Sil4H->L1H.push_back(L1NHits->at(j));
        Sil4H->AmpDir.push_back(AmpDirect->at(j));
        Sil4H->AmpIncl.push_back(AmpInclined->at(j));
        Sil4H->nHits=Sil4H->nHits+1;
      }
    }

    auto SiTreeIndex_931 = -10;
    if(flag_Si != 1) SiTreeIndex_931 = tsi->GetEntryNumberWithIndex (EventID_Bmn, 931); 
    if(flag_Si != 1 && SiTreeIndex_931 > -1)
    {
      tsi->GetEntry(SiTreeIndex_931);
      for(int j=0;j<X->size();j++)
      {
        Sil42H->Y.push_back(Y->at(j)-6.3);
        Sil42H->X.push_back(-(X->at(j)-6.3));//-0.167
        Sil42H->St.push_back(St+1);
        Sil42H->Mod.push_back(Mod);
        Sil42H->L0H.push_back(L0NHits->at(j));
        Sil42H->L1H.push_back(L1NHits->at(j));
        Sil42H->AmpDir.push_back(AmpDirect->at(j));
        Sil42H->AmpIncl.push_back(AmpInclined->at(j));
        Sil42H->nHits=Sil42H->nHits+1;

        Sil4H->Y.push_back(Y->at(j)-6.3);
        Sil4H->X.push_back(-(X->at(j)-6.3));//-0.167
        Sil4H->St.push_back(St+1);
        Sil4H->Mod.push_back(Mod);
        Sil4H->L0H.push_back(L0NHits->at(j));
        Sil4H->L1H.push_back(L1NHits->at(j));
        Sil4H->AmpDir.push_back(AmpDirect->at(j));
        Sil4H->AmpIncl.push_back(AmpInclined->at(j));
        Sil4H->nHits=Sil4H->nHits+1;
      }
    }
     

    //downstream CSC 
    dCSCH_cscX.clear(); dCSCH_cscY.clear(); dCSCH_cscZ.clear(); 
    dCSCH_cscN=0; dCSCH_sec=0; dCSCH_nanosec=0;
    
    //auto downcscTreeIndex = tDownCSC->GetEntryNumberWithIndex (runId, EventID_Bmn); 
    auto downcscTreeIndex = -10;
    if(flag_downCSC != 1) downcscTreeIndex = tDownCSC->GetEntryNumberWithIndex (EventID_Bmn);
    if(downcscTreeIndex > -1)
    {
      tDownCSC->GetEntry(downcscTreeIndex);
      dCSCH_sec = sec;
      dCSCH_nanosec = nanosec;
      dCSCH_cscN = cscN;

      for(int i=0;i<cscN;i++)
      { 
          dCSCH_cscX.push_back(cscX->at(i));  
          dCSCH_cscY.push_back(cscY->at(i));
          dCSCH_cscZ.push_back(cscZ_BMN->at(i));  //cout<<cscX_link.at(i)<<" ";
      }
    }


    //gemCSC
    rightGemX.clear(); rightGemY.clear(); rightGemZ.clear();
    leftGemX.clear(); leftGemY.clear(); leftGemZ.clear();
    rightCscX.clear(); rightCscY.clear(); rightCscZ.clear();
    leftCscX.clear(); leftCscY.clear(); leftCscZ.clear();  
    rightGemN=0; leftGemN=0; rightCscN=0; leftCscN=0; sec_gemcsc=0; nanosec_gemcsc=0;
    
    //auto gemcscIndex_Right = tgemCSC_Right->GetEntryNumberWithIndex (runId, EventID_Bmn); 
    auto gemcscIndex_Right = -10;
    if(flag_Right_gemcsc != 1) gemcscIndex_Right = tgemCSC_Right->GetEntryNumberWithIndex (EventID_Bmn); 
    if (gemcscIndex_Right > -1)
    {
      tgemCSC_Right->GetEntry(gemcscIndex_Right);

      sec_gemcsc = sec_gemcsc_txtroot_Right;
      nanosec_gemcsc = nanosec_gemcsc_txtroot_Right;
      rightGemN = rightGemN_txtroot;
      rightCscN = rightCscN_txtroot;

      for(int i = 0; i<rightGemN; i++)
      {
        //cout<<rightGemX_txtroot[i]<<" ";
        rightGemX.push_back(rightGemX_txtroot->at(i));
        rightGemY.push_back(rightGemY_txtroot->at(i));
        rightGemZ.push_back(rightGemZ_txtroot->at(i));
      }

      for(int i = 0; i<rightCscN; i++)
      {
        rightCscX.push_back(rightCscX_txtroot->at(i));
        rightCscY.push_back(rightCscY_txtroot->at(i));
        rightCscZ.push_back(rightCscZ_txtroot->at(i));
      }
    }

    //auto gemcscIndex_Left = tgemCSC_Left->GetEntryNumberWithIndex (runId, EventID_Bmn); 
    auto gemcscIndex_Left = -10;
    if(flag_Left_gemcsc != 1 ) gemcscIndex_Left = tgemCSC_Left->GetEntryNumberWithIndex (EventID_Bmn); 
    if (gemcscIndex_Left > -1)
    {
      tgemCSC_Left->GetEntry(gemcscIndex_Left);

      sec_gemcsc = sec_gemcsc_txtroot_Left;
      nanosec_gemcsc = nanosec_gemcsc_txtroot_Left;
      leftGemN = leftGemN_txtroot;
      leftCscN = leftCscN_txtroot;

      for(int i = 0; i<leftGemN; i++)
      {
        leftGemX.push_back(leftGemX_txtroot->at(i));
        leftGemY.push_back(leftGemY_txtroot->at(i));
        leftGemZ.push_back(leftGemZ_txtroot->at(i));
      }

      for(int i = 0; i<leftCscN; i++)
      {
        leftCscX.push_back(leftCscX_txtroot->at(i));
        leftCscY.push_back(leftCscY_txtroot->at(i));
        leftCscZ.push_back(leftCscZ_txtroot->at(i));
      }
    }

    tree1->Fill();

           
    SW_ELoss.clear();
    SW_CellId.clear();
    SW_Flag.clear();
    SW_TrId.clear();
    SW_OrderId.clear();
    SW_Mult=0;

    VB_CellId.clear();

    LeftTof400_X.clear();
    LeftTof400_Y.clear();
    LeftTof400_Z.clear();
    LeftTof400_TS.clear();
    LeftTof400_DetId.clear();
    LeftTof400_Module.clear();
    LeftTof400_Station.clear();
    LeftTof400_nHits=0;

    RightTof400_X.clear();
    RightTof400_Y.clear();
    RightTof400_Z.clear();
    RightTof400_TS.clear();
    RightTof400_DetId.clear();
    RightTof400_Module.clear();
    RightTof400_Station.clear();
    RightTof400_nHits=0;

    TofCal_Bar.clear(); //
    TofCal_Plane.clear();
    TofCal_Arm.clear();
    TofCal_GlBar.clear();
    TofCal_Time0.clear();
    TofCal_Time1.clear();
    TofCal_Amp0.clear();
    TofCal_Amp1.clear();
    TofCal_X.clear();
    TofCal_Y.clear(); //
    TofCal_nHits = 0;
 

    // Sil1H_Bmn->X.clear();
    // Sil1H_Bmn->Y.clear();
    // Sil1H_Bmn->St.clear();
    // Sil1H_Bmn->Mod.clear();
    // Sil1H_Bmn->L0H.clear();
    // Sil1H_Bmn->L1H.clear();
    // Sil1H_Bmn->AmpDir.clear();
    // Sil1H_Bmn->AmpIncl.clear();


    MWPC_1->X.clear();
    MWPC_1->Y.clear();
    MWPC_1->Z.clear();
    MWPC_1->Tx.clear();
    MWPC_1->Ty.clear();
    MWPC_1->Chi2.clear();
    MWPC_1->nHits.clear(); 
   
    MWPC_2->X.clear();
    MWPC_2->Y.clear();
    MWPC_2->Z.clear();
    MWPC_2->Tx.clear();
    MWPC_2->Ty.clear();
    MWPC_2->Chi2.clear();
    MWPC_2->nHits.clear(); 
   

    MWPC_3->X.clear();
    MWPC_3->Y.clear();
    MWPC_3->Z.clear();
    MWPC_3->Tx.clear();
    MWPC_3->Ty.clear();
    MWPC_3->Chi2.clear();
    MWPC_3->nHits.clear(); 
 
       
    MWPC_4->X.clear();
    MWPC_4->Y.clear();
    MWPC_4->Z.clear();
    MWPC_4->Tx.clear();
    MWPC_4->Ty.clear();
    MWPC_4->Chi2.clear();
    MWPC_4->nHits.clear(); 
    

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
      
    MWPC_p0->X.clear();
    MWPC_p0->Y.clear();
    MWPC_p0->Z.clear();
    MWPC_p0->Tx.clear();
    MWPC_p0->Ty.clear();
    MWPC_p0->Chi2.clear();
    MWPC_p0->nHits.clear(); 

    MWPC_p1->X.clear();
    MWPC_p1->Y.clear();
    MWPC_p1->Z.clear();
    MWPC_p1->Tx.clear();
    MWPC_p1->Ty.clear();
    MWPC_p1->Chi2.clear();
    MWPC_p1->nHits.clear(); 


  }     
          

  //out.close();
 
  treefile->cd();
  tree1->Write();
  treefile->Close();

  Si_root->Close();
  DownCSCfile->Close();
  gemCSCfile_Right->Close();
  gemCSCfile_Left->Close();

  
 //  timer.Stop(); printf("\n \n C'est fini \nReal time %f s, CPU time %f s\n", timer.RealTime(), timer.CpuTime());
 // double min = (int)timer.RealTime()/60; 
 // //cout<<"Synchrozization: less than "<<(int)timer.RealTime()/60/60 + 1 <<" hours"<<" namely less than "<<(int)timer.RealTime()/60+1<<" minutes for run "<<filenum<<endl<<endl;
 // cout<<"Synchrozization: "<<(int)timer.RealTime()/360 <<" hours"<<" and  "<<((int)timer.RealTime()%360)/60 <<" minutes for run "<<filenum<<endl<<endl;

}


int Combine_Digi(int filenum = 4707) 
{
  timer.Start();
  int run1, run2;
  // cout<<"which start run: "; cin>>run1; //cout<<" let's process for run "<<run1<<endl;
  // cout<<"which end run: "; cin>>run2; cout<<" let's process for runs between  "<<run1<<" and "<<run2<<endl<<endl<<endl;

  ofstream file_problem;
  TString fileNameprob = "problems_runs.txt";
  file_problem.open(fileNameprob);

  //string path_Si = "/home/srcuser2/Miloi/digi-reco/";
  // cout<<argv[0]<<" "<<argv[1]<<endl;
  // string path_Si = argv[1];
  // string path_downCSC = argv[2];
  // string path_gemcsc = argv[3];
  // string path_tof400 = argv[4];

  // string path_out_root = argv[5];
  // string path_out_combined = argv[6];

  // for(int run=run1; run<=run2; run++)
  // {
    int run = filenum;
    convert_to_root_Si(run);
    downstreamCSC_txt_to_root(run);
    GemCscTextToRoot(run);

    sync_bmn_Si_downstreamCSC_gemCSC(run);
  
    if(flag_Si==1) file_problem<<run<<" Silicon_txt"<<endl;
    if(flag_downCSC==1) file_problem<<run<<" Down_CSC_txt"<<endl;
    if(flag_Left_gemcsc==1) file_problem<<run<<" left_gemcsc_txt"<<endl;
    if(flag_Right_gemcsc==1) file_problem<<run<<" right_gemcsc_txt"<<endl;
    //if(flag_TOF==1) file_problem<<run<<" tof400"<<endl; //cannot work, the code stops before
    
    flag_Si=0, flag_downCSC=0, flag_Right_gemcsc=0, flag_Left_gemcsc=0, flag_TOF=0;

    printf("\n \n C'est fini \nReal time %f s, CPU time %f s\n", timer.RealTime(), timer.CpuTime());
    //double min = (int)timer.RealTime()/60; 
    cout<<"Synchrozization (total time) after run  : "<<run<<" "<<(int)timer.RealTime()/3600 <<" hours"<<" and  "<<((int)timer.RealTime()%3600)/60 <<" minutes for run "<<run<<endl<<endl;

  // }

  timer.Stop(); printf("\n \n END!!!! \nReal time %f s, CPU time %f s\n", timer.RealTime(), timer.CpuTime());
  double min = (int)timer.RealTime()/60; 
  cout<<"Synchrozization: "<<(int)timer.RealTime()/3600 <<" hours"<<" and  "<<((int)timer.RealTime()%3600)/60 <<" minutes "<<endl<<endl;


  file_problem.close();

  gApplication->Terminate();
  return 0; 
}

 