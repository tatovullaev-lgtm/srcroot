/* 
 * File:   BmnScWallAnalyzer.h
 * Author: Sergey Nepochatykh
 *
 */

#ifndef BMNSCWALLANALYZER_H
#define BMNSCWALLANALYZER_H

#define DO_SORT 1 

#include "FairLogger.h"
#include <TStopwatch.h>

#include <iostream>
#include <fstream>

using namespace std;

#include <FairTask.h>
#include <TClonesArray.h>

#include "BmnScWallReconstructor.h"
#include "BmnScWallDigi.h"
#include "DstEventHeader.h"
#include "BmnDchTrack.h"

#pragma GCC system_header
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

namespace pt = boost::property_tree;

using MAP1 = map <UInt_t, vector <UInt_t>>;

using MAP2 = map <UInt_t, vector <Double_t>>;

class BmnScWallAnalyzer : public FairTask
{
  public:
    BmnScWallAnalyzer(TString TargetType="LH2");
    
    virtual ~BmnScWallAnalyzer();

    virtual InitStatus Init();
    
    virtual InitStatus ReInit();
    
    virtual void Exec(Option_t* opt);
    
    virtual void Finish();
    
  private:
    TClonesArray * fArrayOfScWallDigits; 
  
    TClonesArray * fBmnScWallReconstructor; 
    
    DstEventHeader * fevHeader;
    
    TClonesArray * fDchTrack;
      
    TString fInputDigitsBranchName;
      
    TString fOutputHitsBranchName;
    
    TString fInputTracksBranchName;
    
    TString fmapFile;
    
    TString fTargetType;
    
    MAP2 fScWallMapper;
    
    MAP2 GetMapper(TString);
    
    MAP1 fScWallNeighbour;
    
    MAP1 GetNeighbour();
    
    void Do_Reconstruction();
    
    vector <vector <Double_t>> fRESULT; 
    
    void Fill_Dst_Event_Header(TStopwatch& SW, vector <vector <Double_t>> RESULT={}, TString OPTION="");
    
    vector <UInt_t> Is_UD_and_Match_Status(Double_t&, Double_t&, vector <Double_t>&, vector <Double_t>&, vector <UInt_t>&, MAP1&, MAP2&);
    
    UInt_t Is_Small_Next_Neighbour_of_Small_Cell(Double_t&, Double_t&, UInt_t&, MAP1&, MAP2&);
    
    void Put_Info_From_DCH(vector <Double_t>&, vector <Double_t>&);
    
    void Put_Info_From_ScWall(vector <Double_t>&, vector <Double_t>&, vector <Double_t>&, vector <UInt_t>&);
    
    Bool_t Is_in_Box(Double_t, Double_t, Double_t, Double_t, Double_t, Double_t);
    
    void Do_Calibration(Double_t&); 
    
    void Put_Info_From_VetoBox(vector <UInt_t>&);

    ClassDef(BmnScWallAnalyzer, 1);
};

#endif /* BMNSCWALLANALYZER_H */
