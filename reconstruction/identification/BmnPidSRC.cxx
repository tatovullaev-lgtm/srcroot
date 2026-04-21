#include "BmnPidSRC.h"
#include "MDF.C"
#include "MDF_Tx.C"

static Float_t workTime = 0.0;
const double Bdl=2.63;
using namespace std;
using namespace TMath;
const Double_t X0_offset  = 1.21651;
const Double_t TX0_offset = -3.32e-03;
const Double_t TX1_offset = 0;



BmnPidSRC::BmnPidSRC() {

    fDstEventHeader = nullptr;
    fBmnDchTrack=nullptr;
    fBmnMwpcTrack=nullptr;
    fDCH_Mult=0;
    fMWPC_Mult=0;
   
    fMwpcTrackBranchName = "BmnMwpcTrack";
    fDstEventHeaderBranchName = "DstEventHeader.";
    fDchTrackBranchName = "BmnDchTrack";
    
}

InitStatus BmnPidSRC::Init() {

    if (fVerbose > 1) cout << "=========================== Pid init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");

    fBmnMwpcTrack = (TClonesArray*) ioman->GetObject(fMwpcTrackBranchName); //in
    if (!fBmnMwpcTrack) {
        cout << "BmnPidSRC::Init(): branch " << fMwpcTrackBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnDchTrack = (TClonesArray*) ioman->GetObject(fDchTrackBranchName); //in
    if (!fBmnDchTrack) {
        cout << "BmnPidSRC::Init(): branch " << fDchTrackBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

   

    fDstEventHeader=(DstEventHeader*) ioman->GetObject(fDstEventHeaderBranchName);
    if (!fDstEventHeader) {
        cout << "BmnPidSRC::Init(): branch " << fDstEventHeaderBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    if (fVerbose > 1) cout << "=========================== Pid init finished ===================" << endl<<endl<<endl<<endl<<endl;

    return kSUCCESS;
}

void BmnPidSRC::Exec(Option_t* opt) {
    if (!IsActive())
        return;
    clock_t tStart = clock();
    


    PReco();

    if (fVerbose > 1) cout << "\n======================== Pid exec finished ======================" << endl;

    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnPidSRC::PReco(){


         fDCH_Mult=0;
         fMWPC_Mult=0;
         double edata[10], txdata[9];
         vector <double> DCH_X, DCH_Y, DCH_Z, DCH_Ty, DCH_Tx, MWPC_X, MWPC_Y, MWPC_Z, MWPC_Ty, MWPC_Tx;
         for (int i=0; i<fBmnDchTrack->GetEntries(); i++){
           BmnDchTrack *dch = (BmnDchTrack *)fBmnDchTrack->At(i);
           if (dch->GetParamFirst()->GetZ()>550 && dch->GetParamFirst()->GetZ()<650) {
              fDCH_Mult=fDCH_Mult+1;
              DCH_X.push_back(dch->GetParamFirst()->GetX());
              DCH_Y.push_back(dch->GetParamFirst()->GetY());
              DCH_Z.push_back(dch->GetParamFirst()->GetZ());
              DCH_Ty.push_back(dch->GetParamFirst()->GetTy());
              DCH_Tx.push_back(dch->GetParamFirst()->GetTx());
           }
         }  
         
          for (int j=0; j<fBmnMwpcTrack->GetEntries(); j++){
            BmnMwpcTrack *mwpc = (BmnMwpcTrack *)fBmnMwpcTrack->At(j);
            if(mwpc->GetParamFirst()->GetZ()>-400){
              fMWPC_Mult=fMWPC_Mult+1;
              MWPC_X.push_back(-mwpc->GetParamFirst()->GetX());
              MWPC_Y.push_back(mwpc->GetParamFirst()->GetY());
              MWPC_Z.push_back(mwpc->GetParamFirst()->GetZ());
              MWPC_Ty.push_back(mwpc->GetParamFirst()->GetTy());
              MWPC_Tx.push_back(-mwpc->GetParamFirst()->GetTx());
            }
          }
               
          for (int i=0; i<fDCH_Mult; i++){
            for (int j=0; j<fMWPC_Mult; j++){
               if (i!=j) continue;   
	             fPq.push_back(0.3*Bdl/(atan(DCH_Tx[i])-atan(MWPC_Tx[j])));
               UInt_t k=0;	
               
               
               
//               double Ztarg=-647.5;
//               double Xtarg=MWPC_X[j]+MWPC_Tx[j]*(Ztarg-MWPC_Z[j]);
//               double Ytarg=MWPC_Y[j]+MWPC_Ty[j]*(Ztarg-MWPC_Z[j]);
               //pos_vec.SetXYZ(Xtarg, Ytarg, Ztarg);
               
             	 edata[k]	= MWPC_X[j]+X0_offset;
            	 edata[++k]	= MWPC_Y[j];
            	 edata[++k]	= MWPC_Z[j];
//               edata[k]	= Xtarg;
//           	 edata[++k]	= Ytarg;
//            	 edata[++k]	= Ztarg;
            	 edata[++k]	= MWPC_Tx[j]+TX0_offset;
            	 edata[++k]	= MWPC_Ty[j];
            
            	 edata[++k]	= DCH_X[i];
            	 edata[++k]	= DCH_Y[i];
            	 edata[++k]	= DCH_Z[i];
            	 edata[++k]	= DCH_Tx[i];
            	 edata[++k]	= DCH_Ty[i];
               fPq_MDF.push_back(MDF(edata));
               
               
               k=0;
               edata[k]	= -MWPC_X[j]+X0_offset;
            	 edata[++k]	= MWPC_Y[j];
            	 edata[++k]	= MWPC_Z[j];
//               edata[k]	= Xtarg;
//           	 edata[++k]	= Ytarg;
//            	 edata[++k]	= Ztarg;
            	 edata[++k]	= MWPC_Tx[j]+TX0_offset;
            	 edata[++k]	= MWPC_Ty[j];
            
            	 edata[++k]	= DCH_X[i];
            	 edata[++k]	= DCH_Y[i];
            	 edata[++k]	= DCH_Z[i];
            	 edata[++k]	= DCH_Tx[i];
             	 edata[++k]	= DCH_Ty[i];
               fPq_MDF1.push_back(MDF(edata));
               
               
               k=0;
               edata[k]	= MWPC_X[j]+X0_offset;
            	 edata[++k]	= MWPC_Y[j];
            	 edata[++k]	= MWPC_Z[j];
//               edata[k]	= Xtarg;
//           	 edata[++k]	= Ytarg;
//            	 edata[++k]	= Ztarg;
            	 edata[++k]	= -MWPC_Tx[j]+TX0_offset;
            	 edata[++k]	= MWPC_Ty[j];
            
            	 edata[++k]	= DCH_X[i];
            	 edata[++k]	= DCH_Y[i];
            	 edata[++k]	= DCH_Z[i];
            	 edata[++k]	= DCH_Tx[i];
            	 edata[++k]	= DCH_Ty[i];
               fPq_MDF2.push_back(MDF(edata));
               
               
               k=0;
               edata[k]	= -MWPC_X[j]+X0_offset;
            	 edata[++k]	= MWPC_Y[j];
            	 edata[++k]	= MWPC_Z[j];
//               edata[k]	= Xtarg;
//           	 edata[++k]	= Ytarg;
//            	 edata[++k]	= Ztarg;
            	 edata[++k]	= -MWPC_Tx[j]+TX0_offset;
            	 edata[++k]	= MWPC_Ty[j];
            
            	 edata[++k]	= DCH_X[i];
            	 edata[++k]	= DCH_Y[i];
            	 edata[++k]	= DCH_Z[i];
            	 edata[++k]	= DCH_Tx[i];
            	 edata[++k]	= DCH_Ty[i];
               fPq_MDF3.push_back(MDF(edata));
               
               k=0;
               txdata[k]	= MWPC_X[j]+X0_offset;
            	 txdata[++k]	= MWPC_Y[j];
            	 txdata[++k]	= MWPC_Z[j];
            	 txdata[++k]	= MWPC_Ty[j];
            
            	 txdata[++k]	= DCH_X[i];
            	 txdata[++k]	= DCH_Y[i];
            	 txdata[++k]	= DCH_Z[i];
            	 txdata[++k]	= DCH_Tx[i];
            	 txdata[++k]	= DCH_Ty[i];
               fTx_MDF.push_back(MDF_TX(txdata));
               //std:
            }  
         }
         
         double charge34=fDstEventHeader->GetZoutBC34_12();
         double charge35=fDstEventHeader->GetZoutBC35_12();
         double charge45=fDstEventHeader->GetZoutBC45_12();
         for ( int i=0; i<fPq.size(); i++) {
          if (fPq[i]>6.9 && fPq[i]<7.7 && charge34==5 && charge45==5 && charge35==5) fB10.push_back(1);
          else fB10.push_back(0);
          if (fPq[i]>7.9 && fPq[i]<8.6 && charge34==5 && charge45==5 && charge35==5) fB11.push_back(1);
          else fB11.push_back(0);
        }
        fDstEventHeader->SetPq(fPq);
        fDstEventHeader->SetPq_MDF(fPq_MDF);
        fDstEventHeader->SetPq_MDF1(fPq_MDF1);
        fDstEventHeader->SetPq_MDF2(fPq_MDF2);
        fDstEventHeader->SetPq_MDF3(fPq_MDF3);
        fDstEventHeader->SetTx_MDF(fTx_MDF);
        fDstEventHeader->SetB11(fB11);
        fDstEventHeader->SetB10(fB10); 
        fDstEventHeader->SetDCH_Mult(fDCH_Mult); 
        fDstEventHeader->SetMWPC_Mult(fMWPC_Mult);  
        fPq.clear();
        fPq_MDF.clear();
        fPq_MDF1.clear();
        fPq_MDF2.clear();
        fPq_MDF3.clear();    
        fTx_MDF.clear();    
        fB10.clear();
        fB11.clear();
        DCH_Tx.clear();
        DCH_Y.clear();
        DCH_Z.clear();
        DCH_Ty.clear();
        DCH_Tx.clear();
        MWPC_X.clear();
        MWPC_Y.clear();
        MWPC_Z.clear();
        MWPC_Ty.clear();
        MWPC_Tx.clear();
                 
}                    
               


void BmnPidSRC::Finish() {
    
    cout << "Work time of PID: " << workTime << endl;

}

 