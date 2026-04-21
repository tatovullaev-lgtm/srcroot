/* 
 * File:   BmnScWallAnalyzer.cxx
 * Author: Sergey Nepochatykh
 *
 */

#include "BmnScWallAnalyzer.h"

static Float_t workTime = 0.0;

static const Double_t X_Shift_Align = 1.19846e+02, Y_Shift_Align = -1.50211e+00;

BmnScWallAnalyzer::BmnScWallAnalyzer(TString TargetType) :
  
  fTargetType(TargetType),
  
  FairTask("BmnScWallAnalyzer"),

  fArrayOfScWallDigits(nullptr),
  
  fBmnScWallReconstructor(nullptr),
  
  fDchTrack(nullptr),
  
  fRESULT(4)

{
  
  fInputDigitsBranchName = "ScWallDigi";
  
  fOutputHitsBranchName = "BmnScWallReconstructor";
  
  fInputTracksBranchName = "BmnDchTrack";
  
  fmapFile = TString(getenv("VMCWORKDIR")) + "/input/SCWALL_map_dry_run_2022.txt";
  
  fScWallMapper = GetMapper(fmapFile);
  
  fevHeader = new DstEventHeader();
  
  fScWallNeighbour = GetNeighbour();
 
}


BmnScWallAnalyzer::~BmnScWallAnalyzer() {
 
 fRESULT.clear();
 
 fScWallMapper.clear();
 
 fScWallNeighbour.clear();
 
 delete fArrayOfScWallDigits, fBmnScWallReconstructor, fevHeader, fDchTrack; 
 
}


InitStatus BmnScWallAnalyzer::Init()
{
    
  FairRootManager* ioman = FairRootManager::Instance();
    
  fArrayOfScWallDigits = (TClonesArray*) ioman -> GetObject(fInputDigitsBranchName);
  
  fevHeader = (DstEventHeader*) ioman -> GetObject("DstEventHeader.");
  
  fDchTrack = (TClonesArray*) ioman -> GetObject(fInputTracksBranchName);
    
  if (fArrayOfScWallDigits == nullptr)
  {
     
   LOG(error) << "BmnScWallAnalyzer::Init() branch " + fInputDigitsBranchName + " not found! Task will be deactivated";
     
   return kERROR;

  }
  
  fBmnScWallReconstructor = new TClonesArray(fOutputHitsBranchName, 100);
  
  ioman -> Register(fOutputHitsBranchName, "ScWall", fBmnScWallReconstructor, kTRUE);
    
  return kSUCCESS;
    
}


InitStatus BmnScWallAnalyzer::ReInit()
{
    
  return kSUCCESS;
    
}


void BmnScWallAnalyzer::Exec(Option_t* opt) {  

  TStopwatch sw;
    
  sw.Start();
    
  fBmnScWallReconstructor -> Delete();
    
  Do_Reconstruction();
  
  if (!fBmnScWallReconstructor -> GetEntries()) { Fill_Dst_Event_Header(sw); return; }
  
  vector <Double_t> X_Middle_Cell, Y_Middle_Cell, ELoss;
  
  vector <UInt_t> CELL_ID;
  
  Put_Info_From_ScWall(X_Middle_Cell, Y_Middle_Cell, ELoss, CELL_ID); 
  
  for (auto& el: ELoss) { Do_Calibration(el); }
  
  fRESULT[0] = ELoss;
  
  ELoss.clear();
  
  for (auto el: CELL_ID) { fRESULT[1].push_back((Double_t)el); fRESULT[2].push_back(0); fRESULT[3].push_back(-1); }
  
  if (!fevHeader -> GetDCH_Mult() || fevHeader -> GetDCH_Mult() > 2)
  {
    
   Fill_Dst_Event_Header(sw, fRESULT, "UNMATCHED");
   
   for (auto& el: fRESULT) { el.clear(); }
   
   return;
   
  }
  
  vector <Double_t> DCH_Track_X, DCH_Track_Y; 
  
  Put_Info_From_DCH(DCH_Track_X, DCH_Track_Y);
  
  if (DCH_Track_X.size() == 1)
  {
   
   vector <UInt_t> Get_UD_and_Status = Is_UD_and_Match_Status(DCH_Track_X[0], DCH_Track_Y[0], X_Middle_Cell, Y_Middle_Cell, CELL_ID, fScWallNeighbour, fScWallMapper);
   
   if (!Get_UD_and_Status[1]) { Fill_Dst_Event_Header(sw, fRESULT, "UNMATCHED"); }
   
   else { fRESULT[2][Get_UD_and_Status[0]] = 1; fRESULT[3][Get_UD_and_Status[0]] = 0; Fill_Dst_Event_Header(sw, fRESULT, "MATCHED"); }
   
   for (auto& el: fRESULT) { el.clear(); }
   
   return;
   
  }
  
  vector <UInt_t> Get_UD_and_Status0 = Is_UD_and_Match_Status(DCH_Track_X[0], DCH_Track_Y[0], X_Middle_Cell, Y_Middle_Cell, CELL_ID, fScWallNeighbour, fScWallMapper);
  
  vector <UInt_t> Get_UD_and_Status1 = Is_UD_and_Match_Status(DCH_Track_X[1], DCH_Track_Y[1], X_Middle_Cell, Y_Middle_Cell, CELL_ID, fScWallNeighbour, fScWallMapper);
  
  X_Middle_Cell.clear(); Y_Middle_Cell.clear(); CELL_ID.clear();
  
  if (Get_UD_and_Status0[1] && Get_UD_and_Status1[1] && Get_UD_and_Status0[0] != Get_UD_and_Status1[0])
  {
   
   fRESULT[2][Get_UD_and_Status0[0]] = 1; fRESULT[2][Get_UD_and_Status1[0]] = 1;
   
   fRESULT[3][Get_UD_and_Status0[0]] = 0; fRESULT[3][Get_UD_and_Status1[0]] = 1;
   
   Fill_Dst_Event_Header(sw, fRESULT, "MATCHED");
   
   for (auto& el: fRESULT) { el.clear(); }
   
   return;
   
  }
  
  if ((Get_UD_and_Status0[1] && !Get_UD_and_Status1[1]) || (!Get_UD_and_Status0[1] && Get_UD_and_Status1[1]))
  {
   
   if (Get_UD_and_Status0[1]) { fRESULT[2][Get_UD_and_Status0[0]] = 1; fRESULT[3][Get_UD_and_Status0[0]] = 0; }
   
   else { fRESULT[2][Get_UD_and_Status1[0]] = 1; fRESULT[3][Get_UD_and_Status1[0]] = 1; }
   
   Fill_Dst_Event_Header(sw, fRESULT, "MATCHED");
   
   for (auto& el: fRESULT) { el.clear(); }
   
   return;
   
  }
  
  Fill_Dst_Event_Header(sw, fRESULT, "UNMATCHED");
  
  for (auto& el: fRESULT) { el.clear(); }
  
}


void BmnScWallAnalyzer::Finish() {
    
  printf("Work time of BmnScWallAnalyzer: %4.2f sec.\n", workTime);
    
}


MAP2 BmnScWallAnalyzer::GetMapper(TString mapFile) {
  
  MAP2 Res;
  
  ifstream in(mapFile);
 
  for (int i = 0; i < 20; i ++) 
  { 
   
   string var; 
   
   getline(in, var); 
   
  } 
 
  for (int i = 0; i < 192; i ++) 
  {  
  
   string config, ADC_serial, sign, Zone; 
   
   Double_t ADC_ch, x, y, size, X1, X2, Y1, Y2; 
   
   UInt_t CellId;
   
   in >> config >> sign >> ADC_serial >> ADC_ch >> CellId >> Zone >> x >> y >> size;
   
   if (Zone == "Z") { continue; } 
  
   X1 = x * 0.1 - 0.5 * size * 0.1; 
   
   X2 = x * 0.1 + 0.5 * size * 0.1;
  
   Y1 = y * 0.1 - 0.5 * size * 0.1; 
   
   Y2 = y * 0.1 + 0.5 * size * 0.1; 
  
   if (CellId <= 40)
   {
     
    if (CellId <= 20) 
    { 
     
     Y1 += 0.05; 
     
     Y2 += 0.05; 
     
    } 
    
    else { Y1 -= 0.05; Y2 -= 0.05; } 
    
    Bool_t SHIFT = kFALSE;
     
    for (int k = 0; k < 4; k ++) 
    { 
     
     if (CellId >= 1 + k * 10 && CellId <= 4 + k * 10) 
     { 
      
      X1 -= 0.05; 
      
      X2 -= 0.05; 
      
      SHIFT = kTRUE; 
      
     } 
     
    } 
   
    if (!SHIFT) 
    { 
     
     X1 += 0.05; X2 += 0.05; 
     
    }
      
   }
   
   Res.insert({CellId, { X1, X2, Y1, Y2 }});
 
  }
 
 in.close();
 
 return Res;
  
}


void BmnScWallAnalyzer::Do_Reconstruction() {
 
 for (int iHit = 0; iHit < fArrayOfScWallDigits -> GetEntries(); iHit ++)
 {
     
  BmnScWallDigi *det = (BmnScWallDigi *)fArrayOfScWallDigits -> At(iHit);
     
  if (!(det -> GetCellId() < 200 && det -> GetSignal() > 0. && det -> fFitR2 < 0.5 && det -> fTimeMax > 12 && det -> fTimeMax < 20)) { continue; }
     
  BmnScWallReconstructor* signal = new ((*fBmnScWallReconstructor)[fBmnScWallReconstructor -> GetEntriesFast()])BmnScWallReconstructor();
     
  signal -> SetELosses((Double_t)det -> GetSignal());
   
  signal -> SetCellId(det -> GetCellId());
   
  UInt_t CellId = signal -> GetCellId();
   
  signal -> SetX1(fScWallMapper[CellId][0]);
   
  signal -> SetX2(fScWallMapper[CellId][1]);
   
  signal -> SetY1(fScWallMapper[CellId][2]);
   
  signal -> SetY2(fScWallMapper[CellId][3]);
     
 }
 
}


void BmnScWallAnalyzer::Fill_Dst_Event_Header(TStopwatch& SW, vector <vector <Double_t>> RESULT, TString OPTION) { 

 vector <UInt_t> VBCellId {};
 
 Put_Info_From_VetoBox(VBCellId);
 
 fevHeader -> SetVBCellId(VBCellId);
 
 fevHeader -> SetVBMult((UInt_t)VBCellId.size());
 
 if (!OPTION.Length()) 
 {
  
  fevHeader -> SetScWallELoss({});
   
  fevHeader -> SetScWallCellId({});
   
  fevHeader -> SetScWallFlag({});
  
  fevHeader -> SetScWallTrackId({});
  
  fevHeader -> SetScWallMult(0);
  
  fevHeader -> SetScWallNMatchTracks(0);
  
  fevHeader -> SetScWallOrderId({});
  
  SW.Stop(); 
   
  workTime += SW.RealTime();
  
  return;
  
 }
 
 fevHeader -> SetScWallELoss(RESULT[0]);
 
 vector <UInt_t> tmp;
 
 for (auto el: RESULT[1]) { tmp.push_back((UInt_t)el); }
 
 fevHeader -> SetScWallCellId(tmp);
 
 tmp.clear();
 
 for (auto el: RESULT[2]) { tmp.push_back((UInt_t)el); }
 
 fevHeader -> SetScWallFlag(tmp);
 
 UInt_t NMatchTracks = (UInt_t)accumulate(tmp.begin(), tmp.end(), 0);
 
 vector <Int_t> tmp1;
 
 for (auto el: RESULT[3]) { tmp1.push_back((Int_t)el); }
 
 fevHeader -> SetScWallTrackId(tmp1);
 
 vector <UInt_t> tmp2;
 
 auto cur = 0;
 
 for (auto i = 0; i < NMatchTracks; i ++) { auto res = find(tmp.begin() + cur, tmp.end(), 1); cur = distance(tmp.begin(), res); tmp2.push_back(cur ++); }
 
 tmp.clear();
 
 if (NMatchTracks > 1 && DO_SORT) 
 { 
  
  map <UInt_t, UInt_t> res; 
  
  cur = 0;
  
  for (auto i = 0; i < tmp1.size(); i ++) { if (tmp1.at(i) < 0) { continue; } res.insert({tmp1.at(i), tmp2.at(cur ++)}); }
  
  tmp2.clear();
  
  for (const auto& el: res) { tmp2.push_back(el.second); } 
  
  res.clear();
  
 }
 
 tmp1.clear();
 
 fevHeader -> SetScWallOrderId(tmp2);
 
 tmp2.clear();
 
 fevHeader -> SetScWallMult(RESULT[0].size());
 
 fevHeader -> SetScWallNMatchTracks(NMatchTracks); 
   
 SW.Stop(); 
   
 workTime += SW.RealTime();
 
}


vector <UInt_t> BmnScWallAnalyzer::Is_UD_and_Match_Status(Double_t& TrackX, Double_t& TrackY, vector <Double_t>& MidX, vector <Double_t>& MidY, vector <UInt_t>& CELLID, MAP1& neigh, MAP2& mapp) {
 
 vector <Double_t> dst; 
 
 for (UInt_t iCell = 0; iCell < CELLID.size(); iCell ++) { dst.push_back(sqrt(pow(TrackX - MidX[iCell], 2) + pow(TrackY - MidY[iCell], 2))); }
 
 vector <Double_t>::iterator min_dst = min_element(dst.begin(), dst.end()); 
 
 UInt_t UD = UInt_t(distance(dst.begin(), min_dst));
 
 auto dz = (CELLID[UD] <= 40) ? 7.5 : 15.0;
 
 if (Is_in_Box(TrackX, TrackY, MidX[UD] - 0.5 * dz, MidY[UD] - 0.5 * dz, MidX[UD] + 0.5 * dz, MidY[UD] + 0.5 * dz)) { return { UD, 1 }; }
 
 auto result = find_if(neigh[CELLID[UD]].begin(), neigh[CELLID[UD]].end(), [=](UInt_t Near_Cell)
 {
  
  Double_t X1 = mapp.at(Near_Cell)[0], X2 = mapp.at(Near_Cell)[1], Y1 = mapp.at(Near_Cell)[2], Y2 = mapp.at(Near_Cell)[3];
   
  Double_t tmp = -X1; X1 = -X2; X2 = tmp; 
  
  X1 += X_Shift_Align; X2 += X_Shift_Align; 
  
  Y1 += Y_Shift_Align; Y2 += Y_Shift_Align;
   
  return Is_in_Box(TrackX, TrackY, X1, Y1, X2, Y2);
  
 });
 
 if (result != neigh[CELLID[UD]].end()) { return { UD, 1 }; }
 
 if (CELLID[UD] <= 40 && Is_Small_Next_Neighbour_of_Small_Cell(TrackX, TrackY, CELLID[UD], neigh, mapp)) { return { UD, 1 }; }
 
 return { UD, 0 };
 
}


UInt_t BmnScWallAnalyzer::Is_Small_Next_Neighbour_of_Small_Cell(Double_t& TrackX, Double_t& TrackY, UInt_t& CELLID, MAP1& neigh, MAP2& mapp)
{ 
 
 UInt_t isNextNeighbour = 0; 
 
 vector <UInt_t> tmp1;
      
 copy_if(neigh[CELLID].begin(), neigh[CELLID].end(), back_inserter(tmp1), [](UInt_t Near_Cell) { return Near_Cell <= 40; });
      
 for(auto Near_Cell: tmp1) 
 {
       
  vector <UInt_t> tmp2; 
       
  copy_if(neigh[Near_Cell].begin(), neigh[Near_Cell].end(), back_inserter(tmp2), [](UInt_t Next_Near_Cell) { return Next_Near_Cell <= 40; }); 
       
  auto result = find_if(tmp2.begin(), tmp2.end(), [=](UInt_t Next_Near_Cell)
  {
        
   Double_t X1 = mapp.at(Next_Near_Cell)[0], X2 = mapp.at(Next_Near_Cell)[1], Y1 = mapp.at(Next_Near_Cell)[2], Y2 = mapp.at(Next_Near_Cell)[3];
        
   Double_t tmp = -X1; X1 = -X2; X2 = tmp;
   
   X1 += X_Shift_Align; X2 += X_Shift_Align; 
  
   Y1 += Y_Shift_Align; Y2 += Y_Shift_Align;
        
   return Is_in_Box(TrackX, TrackY, X1, Y1, X2, Y2);
        
  });
       
  isNextNeighbour = (result != tmp2.end()) ? 1 : 0;
       
  if (isNextNeighbour) { break; }
       
 } 
 
 return isNextNeighbour;
 
} 


void BmnScWallAnalyzer::Put_Info_From_DCH(vector <Double_t>& DCH_Track_X, vector <Double_t>& DCH_Track_Y) {
 
 for (int i = 0; i < fDchTrack -> GetEntries(); i ++)
 {
  
  BmnDchTrack *dch = (BmnDchTrack *)fDchTrack -> At(i);
  
  if (dch -> GetParamFirst() -> GetZ() > 550 && dch -> GetParamFirst() -> GetZ() < 650)
  {
   
   Double_t Extrapolated_X = dch -> GetParamFirst() -> GetX() + dch -> GetParamFirst() -> GetTx() * (948.946 - dch -> GetParamFirst() -> GetZ());
   
   Double_t Extrapolated_Y = dch -> GetParamFirst() -> GetY() + dch -> GetParamFirst() -> GetTy() * (948.946 - dch -> GetParamFirst() -> GetZ());
   
   DCH_Track_X.push_back(Extrapolated_X); DCH_Track_Y.push_back(Extrapolated_Y);
   
  }
  
 }
 
}


void BmnScWallAnalyzer::Put_Info_From_ScWall(vector <Double_t>& X_Middle_Cell, vector <Double_t>& Y_Middle_Cell, vector <Double_t>& ELoss, vector <UInt_t>& CELL_ID) {
 
 for (int i = 0; i < fBmnScWallReconstructor -> GetEntries(); i ++)
 {
  
  BmnScWallReconstructor *det = (BmnScWallReconstructor *)fBmnScWallReconstructor -> At(i);
  
  Double_t X1 = det -> GetX1(), X2 = det -> GetX2(), Y1 = det -> GetY1(), Y2 = det -> GetY2(); 
  
  Double_t tmp = -X1; X1 = -X2; X2 = tmp;
  
  X1 += X_Shift_Align; X2 += X_Shift_Align; 
  
  Y1 += Y_Shift_Align; Y2 += Y_Shift_Align;
  
  X_Middle_Cell.push_back((X1 + X2) * 0.5);
  
  Y_Middle_Cell.push_back((Y1 + Y2) * 0.5);
  
  CELL_ID.push_back(det -> GetCellId());
  
  ELoss.push_back(det -> GetELosses());
  
 }
 
}


Bool_t BmnScWallAnalyzer::Is_in_Box(Double_t X, Double_t Y, Double_t X0, Double_t Y0, Double_t X1, Double_t Y1) { 
 
 return X >= X0 && X <= X1 && Y >= Y0 && Y <= Y1; 
 
}


MAP1 BmnScWallAnalyzer::GetNeighbour()
{ 
 
 pt::ptree conf;
 
 pt::read_json(string(getenv("VMCWORKDIR")) + "/reconstruction/scwall/neighbours.json", conf);

 MAP1 Res;

 BOOST_FOREACH(pt::ptree::value_type &v, conf) 
 { 
  
  string tmp = conf.get<string>(v.first); 
  
  tmp.push_back(' ');  
  
  auto cur = 0; 
  
  vector <UInt_t> RES; 
  
  for (auto i = 0; i < count(tmp.begin(), tmp.end(), ' '); i ++) 
  { 
   
   string tmp1 = ""; 
   
   for (auto j = cur; j < tmp.size(); j ++) 
   { 
    
    if (tmp[j] == ' ') 
    { 
     
     RES.push_back((UInt_t)atoi(tmp1.c_str())); 
     
     cur = j + 1; 
     
     break; 
    
    } 
    
    tmp1 += tmp[j]; 
    
   } 
  
  }
  
  Res.insert({(UInt_t)atoi(v.first.c_str()), RES}); 
 
 } 
 
 return Res;
 
}   


void BmnScWallAnalyzer::Do_Calibration(Double_t& X) { 
 
 if (fTargetType != "LH2") return;
 
 Double_t A = 1.02419823684244, B = 1.04951685950668, C = -0.295265809164639; 
 
 X = A * TMath::Power(X, B) + C; 
 
}


void BmnScWallAnalyzer::Put_Info_From_VetoBox(vector <UInt_t>& VBCellId) {
 
 for (int iHit = 0; iHit < fArrayOfScWallDigits -> GetEntries(); iHit ++)
 {
     
  BmnScWallDigi *det = (BmnScWallDigi *)fArrayOfScWallDigits -> At(iHit);
     
  if (det -> GetCellId() < 200) { continue; }
     
  VBCellId.push_back(det -> GetCellId());
     
 }
 
}


ClassImp(BmnScWallAnalyzer)
