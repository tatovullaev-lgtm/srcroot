#include <fstream>
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "base/BmnEnums.h"
#include "macro/run/bmnloadlibs.C"

#define CALRUNNAME 1
#define CALIBFILE "bmn_run4704_digi_calibration.root"
#define CORRECTIONFILE "bmn_argon_protonbased_time_corrections.root"

// Fragments
#define PFRMAX 2.0
#define MFRMIN -0.3
#define TXBEAM 0.002
#define TYBEAM 0.005
#define PTMIN 0.0
#define PTMAX 1.4
#define YLMIN 0.0
#define YLMAX 2.0
#define NPT 7
#define NYL 5
#define MPR 0.938
#define M1 (MPR*MPR)
#define M2 (4.*MPR*MPR)
#define M3 (9.*MPR*MPR)
#define M4 (16.*MPR*MPR)
// Proton region min and max (M1 - one nucleon, Z1 - charge +1, etc)
#define M1Z1MIN 0.5
#define M1Z1MAX 1.4
// D
#define M2Z1MIN 2.4
#define M2Z1MAX 6.0
// T
#define M3Z1MIN 6.0
#define M3Z1MAX 9.5
// He3
#define M3Z2MIN 1.4
#define M3Z2MAX 2.4
// He
#define M2Z1MIN 2.4
#define M2Z1MAX 6.0

//

#define XGEMCOR 1
#define XTOFCOR 1
#define PUREDCH 0
#define USEDCHXY 0
#define USEDCHTXTY 0
#define USEGEMTXTY 0

#define TRACKS_ONLY 0

#define REFIT 1
#define REFIT_DEBUG 0
#define DCHCOVPAR 0

#define MINDCHHITS 7

#if XGEMCOR != 0
Double_t Xgemcorrection(Double_t p, Double_t *txcor, Double_t *xsigma, Double_t *ysigma);
Bool_t GemCorInit = true;
TH1D *hftmp = 0, *hftsp = 0, *hftmn = 0, *hftsn = 0;
TH1D *hfmp = 0, *hfsp = 0, *hfmn = 0, *hfsn = 0;
TH1D *hfmpy = 0, *hfspy = 0, *hfmny = 0, *hfsny = 0;
TF1 *fmp = 0, *fsp = 0, *fmn = 0, *fsn = 0;
TF1 *ftmp = 0, *ftsp = 0, *ftmn = 0, *ftsn = 0;
TF1 *fmpy = 0, *fspy = 0, *fmny = 0, *fsny = 0;
Double_t NDXsigma = 2.5;
#else
Double_t NDXsigma = 0;
#endif
Double_t DXsigma = 0.;
Double_t DYsigma = 0.;
Double_t DTXcor = 0.;

#if XTOFCOR >= 0
Double_t Xgemcorrectiontof700(Double_t p0, Double_t *xsigmatof700, Double_t *ysigmatof700);
Bool_t GemCorInittof700 = true;
TH1D *hfmptof700 = 0, *hfsptof700 = 0, *hfmntof700 = 0, *hfsntof700 = 0;
TH1D *hfmpytof700 = 0, *hfspytof700 = 0, *hfmnytof700 = 0, *hfsnytof700 = 0;
TF1 *fmptof700 = 0, *fsptof700 = 0, *fmntof700 = 0, *fsntof700 = 0;
TF1 *fmpytof700 = 0, *fspytof700 = 0, *fmnytof700 = 0, *fsnytof700 = 0;
#endif
#define MAX_NPAIRS 1000
Double_t NTOFsigma = 2.5;
Double_t SigmaX_TOF700 = 0.;
Double_t SigmaY_TOF700 = 0.;

// Sigma DCH-TOF700
#define SigmaX_DCH_TOF 1.2
#define SigmaY_DCH_TOF 1.6

// Sigma GEM-TOF700
#define SigmaX_GEM_TOF 1.7
#define SigmaY_GEM_TOF 1.4

#define GEMTYPE 2

//Gleb_SIGEM_v1_*.root   - Gem + SILICON треки версия Глеба с damount = dx * dx / (ddxgem + ddxdch) + dy * dy / (ddygem + ddydch); 
#if GEMTYPE == 0
#define DTXDCH +0.0
#define DTYDCH +0.0
#define DXDCH  -9.2454
#define DYDCH  +3.7912
#define DZDCH  -0.1655
#define MINGEMNDF 4
#define MINSIHITS 2
#define TRACKFILE "argon_tracks_gleb_chi2_files.txt"

//Gleb_SIGEM_v1_filtered_*.root - Отфильтрованный Gem + SILICON треки версия Глеба с damount = dx * dx / (ddxgem + ddxdch) + dy * dy / (ddygem + ddydch); 
#elif GEMTYPE == 1
#define DTXDCH +0.0
#define DTYDCH +0.0
#define DXDCH  -10.2734
#define DYDCH  +3.6434
#define DZDCH  -4.26
#define MINGEMNDF 4
#define MINSIHITS 2
#define TRACKFILE "argon_tracks_gleb_chi2_filter_files.txt"

//Gleb_SIGEM_v2_*.root   - Gem + SILICON треки версия Глеба с damount = dx * dx  + dy * dy;
#elif GEMTYPE == 2
/*
#define DTXDCH +0.0
#define DTYDCH +0.0
#define DXDCH  -9.2454
#define DYDCH  +3.7912
#define DZDCH  -0.1655
#define MINGEMNDF 6
#define TRACKFILE "argon_tracks_gleb_files.txt"
*/
#define DTXDCH +0.0
#define DTYDCH +0.0
#define DXDCH  -9.1845
#define DYDCH  +3.143
#define DZDCH  -0.1655
#define MINGEMNDF 4
#define MINSIHITS 2
#define TRACKFILE "argon_tracks_lalyo_final2_without_ecal.txt"
//#define TRACKFILE "argon_tracks_lalyo_final2.txt"
//#define TRACKFILE "argon_tracks_lalyo_final1.txt"
//#define TRACKFILE "argon_tracks_lalyo_final.txt"
//#define TRACKFILE "argon_tracks_lalyo_fragments_recent.txt"
//#define TRACKFILE "argon_tracks_lalyo_fragments_new.txt"
//#define TRACKFILE "argon_tracks_lalyo_fragments.txt"
//#define TRACKFILE "argon_tracks_lalyo_no_correction_with_cov_dch678_fullfull.txt"
//#define TRACKFILE "argon_tracks_lalyo_no_correction_with_cov_dch678_full.txt"
//#define TRACKFILE "argon_tracks_lalyo_files_Xgem_correction_with_cov_dch678_full.txt"
//#define TRACKFILE "argon_tracks_lalyo_files_no_correction_with_cov_dch_full.txt"
//#define TRACKFILE "argon_tracks_lalyo_files_no_correction_with_cov_dch_1.txt"
//#define TRACKFILE "argon_tracks_lalyo_files_Xgem_TXgem_correction_new.txt"
//#define TRACKFILE "argon_tracks_lalyo_files_Xgem_TXgem_correction.txt"
//#define TRACKFILE "argon_tracks_lalyo_files_Xgem_correction.txt"
//#define TRACKFILE "argon_tracks_lalyo_files_full_shiftfield_fixpv.txt"
//#define TRACKFILE "argon_tracks_lalyo_files_full_shiftfield.txt"
//#define TRACKFILE "argon_tracks_lalyo_files_full.txt"

//Merts_SIGEM_v1_*.root - Gem + SILICON треки версия Merts с damount = dx * dx / (ddxgem + ddxdch) + dy * dy / (ddygem + ddydch);
#elif GEMTYPE == 3
#define DTXDCH +0.0065589
#define DTYDCH -0.0004069
#define DXDCH  -8.00097
#define DYDCH  -3.1156
#define DZDCH  -2.32
#define MINGEMNDF 4
#define MINSIHITS 2
#define TRACKFILE "argon_tracks_merts_chi2_files.txt"

//Merts_SIGEM_v2_*.root - Gem + SILICON треки версия Merts с damount = dx * dx  + dy * dy;
#elif GEMTYPE == 4
#define DTXDCH +0.0065589
#define DTYDCH -0.0004069
#define DXDCH  -8.00097
#define DYDCH  -3.1156
#define DZDCH  -2.32
#define MINGEMNDF 4
#define MINSIHITS 2
#define TRACKFILE "argon_tracks_merts_files.txt"

#endif

//Old Gleb
#define DTX0DCH +0.0
#define DTY0DCH +0.0
#define DX0DCH  -10.3038
#define DY0DCH  +2.55
#define DZ0DCH  +1.94

#define MINTIME true

#define GEMDEBUG false

#define XYPANIN 0
#define XYPETNEW 1

//#define DXGEMDCHMAX 5.
//#define DYGEMDCHMAX 3.
#define DXGEMDCHMAX 2.5
#define DYGEMDCHMAX 1.5

#define USE_CLUSTERS 1

#define STRIP_CORRECTIONS 1
#define LIST_STRIP_CORRECTIONS 0

// CALIBRATION - 0 - read corrections from root file *calibration.root, main time peak from TOF700_MC_time_run7.txt
//               1 - no calibration correction on input, calculation and store corrections in *calibration.root file, mean time peak from beam momentum
//               2 - no calibration correction on input, calculation and store corrections in *calibration.root file, mean time peak from TOF700_MC_time_run7.txt
//               3 - no calibration correction on input, calculation and store corrections in *calibration.root file, mean time peak from TOF700_MC_time_run7.txt
#define CALIBRATION 0

// GEMCUTS = 0 - don't use additional cuts on GEM tracks, 1 - use additional cuts
#define GEMCUTS 1

// PRIMARY_VERTEX_CUTS = 0 - don't use additional cuts on primary vertex, 1 - use additional cuts
#define PRIMARY_VERTEX_CUTS 1
//#define PV_HARD 3.0f
#define PV_HARD 0.0f

// CVERS=0 -  use last calibration tables, 1 - use *;1 version
#define CVERS 1

// CFACTOR - beta factor for main time peak particles
#define CFACTOR 0.97f

// FLAG -1 - (GEM+DCH1+DCH2), 0 - all, +1 (GEM+DCH1)
#define FLAG 0

// BETAMAX max velocity for proton selection
#define BETAMAX 1.00f

// PMIN min momentum for track selection
#define PMIN 0.5f

// PMAX max momentum for track selection
#define PMAX 5.0f

// PMAX1 max momentum for proton mass limits for calculate corrections
#define PMAX1 3.0f

// PMAX2 max momentum for proton mass limits for resolution hists
#define PMAX2 2.5f

// PRES min momentum for resolution hists
#define PRES 2.0f

// PRES1 another min momentum for resolution hists
#define PRES1 3.0f

// PMAXPI max momentum for pion track selection for calculate corrections
#define PMAXPI 3.0f

// MPIMAX max limit pion mass
#define MPIMAX 0.2f

// MP proton mass
#define MP 0.938f

// MP2 proton mass sqare
#define MP2 (MP*MP)

// pion mass
#define MPI  0.139

// pion mass square
#define MPI2 (MPI*MPI)

// scale factor for min mass proton region selection
#define DMPSCALE1 1.

// scale factor for max mass proton region selection
#define DMPSCALE2 1.

// scale factor for min mass proton region selection for time hist
#define DMPSCALET1 1.

// scale factor for max mass proton region selection for time hist
#define DMPSCALET2 1.5

// scale factor for pion region selection
#define DMPISCALE 1.

// PSCALE - momentum scale factor
//#define PSCALE 0.96f
#define PSCALE 0.99f
//#define PSCALE 1.00f

#define WRITE_CALIBRATION 0

//#define XFACTOR 5.0f
#define XFACTOR 1.0f
//#define DISTMAX 20.0f
//#define DXMAX 5.0f
//#define DYMAX 3.0f
#define DISTMAX 20.0f
#define DXMAX 5.0f
#define DYMAX 3.0f
#define DXMAXPAN 5.0f
#define DYMAXPAN 5.0f
#define GEM2SCALE 1.5f

#define ACCEPTANCE false
#define DXBORDER 4.
#define DYBORDER 4.

#define TOFMAX 39.0f

#define IN_WIDTH_LIMITS 0
#define WMIN 2900
#define WMAX 9000

#define NUMBER_CHAMBERS 59

Float_t halfwidthx[NUMBER_CHAMBERS] = {8.0};
Float_t halfwidthy[NUMBER_CHAMBERS] = {0.5};

int wmi[NUMBER_CHAMBERS] = {2900};
int wma[NUMBER_CHAMBERS] = {9000};

int nhi = 0;
int nhit[NUMBER_CHAMBERS] = {0};
int nhits[NUMBER_CHAMBERS][32] = {{0}};
float tof[NUMBER_CHAMBERS][32] = {{0.}};
float tofWidths[NUMBER_CHAMBERS][32] = {{0.}};
double xhi[1000], yhi[1000], zhi[1000];
double lrd[1000];
double xhic[1000], yhic[1000], zhic[1000];
using namespace std;
using namespace TMath;

BmnTof2Raw2DigitNew *TOF2 = NULL;

TProfile2D *itcalibr = 0;
TProfile   *itcalibrc = 0;

float idchambers[59] = {27.1,28.1,3.1,1.1,29.1,4.1,33.1,30.1,5.1,19.3,31.1,6.1,2.1,32.1,15.2,16.2,17.2,
18.2,19.2,20.2,7.1,115.2,113.1,117.1,35.1,9.1,37.1,11.1,39.1,13.1,34.1,8.1,36.1,10.1,38.1,12.1,21.2,
23.2,25.2,22.2,24.2,26.2,107.2,108.2,109.2,110.2,111.2,112.2,114.1,116.2,118.1,14.1,40.1,119.2,120.2,
121.2,122.2,123.2,124.2 };

Double_t tofcalmc[NUMBER_CHAMBERS] = {0.f};
Double_t tofcalc[NUMBER_CHAMBERS] = {0.f};
Double_t tofcals[NUMBER_CHAMBERS][32] = {{0.f}};

Int_t Atar[6] = {0,12,27,63,120,208}, Ztar[6] = {0,6,13,29,50,82}, Ntar[10000] = {0}, IdTrig[10000] = {0}, fSp41[10000] = {0};
TString NameTar[6] = {" ","C","Al","Cu","Sn","Pb"};
void selecting_records_target();
void xystrip(TH2D *h, Int_t i, Int_t j);

Int_t readMCTimeFile(const char *MCTimeFile = "TOF700_MC_argon_qgsm_time_run7.txt");

TFile *f = NULL;

void BmnEfficiencyTOF700(const char *TracksGemDch = TRACKFILE, const char *DigitsTOF700 = "argon_tof700_digits_files_pro_full_new.txt") {

    /* Load basic libraries */
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs(); // load BmnRoot libraries

    int RunPeriod = 7;
    char mapping[256];
    if(RunPeriod >= 1 && RunPeriod <= 7)
    {
	sprintf(mapping, "TOF700_map_period_%d.txt", RunPeriod);
    }
    else
    {
	printf("Non-existing run period number %d!", RunPeriod);
	return;
    }

    BmnEventHeader *eventHeadertof700 = NULL;
    TClonesArray *tof700Digits = NULL, *eventHeader = NULL;
    TClonesArray *tracks = NULL, *mtracks = NULL;
    CbmVertex* primaryVertex = NULL;
    TString filename;

    TChain *bmnTree = new TChain("bmndata");
    if (strstr(TracksGemDch,".root"))
    {
	bmnTree->Add(TracksGemDch);
    }
    else
    {
	ifstream fin(TracksGemDch);
	if (fin.is_open())
	{
	    for(int idx=0; idx<1000; idx++) {
		if (fin.eof()) break;
		fin >> filename;
		if (strlen(filename.Data())==0) break;
		if ((filename.Data())[0]=='#') continue;
		cout << " GEM+DCH tracks file #" << idx+1 << " " << filename.Data() << endl;
		bmnTree->Add(filename);
	    }
	    fin.close();
	}
	else
	{
	    printf("Non-existing run list file: %s!", TracksGemDch);
	    return;
	}
    }
    bmnTree->SetBranchAddress("EventHeader", &eventHeader);
    bmnTree->SetBranchAddress("GemDchTracks", &mtracks);
    bmnTree->SetBranchAddress("PrimaryVertex", &primaryVertex);

    char fnametof700[256];
    TChain *bmnTreetof700 = new TChain("bmndata");
    if (strstr(DigitsTOF700,".root"))
    {
	bmnTreetof700->Add(DigitsTOF700); //
	strcpy(fnametof700, DigitsTOF700);
    }
    else
    {
	ifstream fin(DigitsTOF700);
	if (fin.is_open())
	{
	    for(int idx=0; idx<1000; idx++) {
		if (fin.eof()) break;
		fin >> filename;
		if (strlen(filename.Data())==0) break;
		if ((filename.Data())[0]=='#') continue;
		cout << " TOF digits file #" << idx+1 << " " << filename.Data() << endl;
		bmnTreetof700->Add(filename);
		strcpy(fnametof700, filename.Data());
	    }
	    fin.close();
	}
	else
	{
	    printf("Non-existing run list file: %s!", DigitsTOF700);
	    return;
	}
    }
    bmnTreetof700->SetBranchAddress("BmnEventHeader.", &eventHeadertof700);
    bmnTreetof700->SetBranchAddress("TOF700", &tof700Digits);

    char fnametof700raw[256];
    strcpy(fnametof700raw,fnametof700);
    strcpy(strstr(fnametof700raw,"digi.root"),"raw.root");
    if (XYPANIN)
	TOF2 = new BmnTof2Raw2DigitNew(mapping, fnametof700raw, 0, 0, "TOF700_geometry_run7_panin.txt");
    else
	TOF2 = new BmnTof2Raw2DigitNew(mapping, fnametof700raw, 0, 0, "TOF700_geometry_run7.txt");

    Double_t xmintof700 = +1000., xmaxtof700 = -1000., ymintof700 = +1000., ymaxtof700 = -1000.;
    Float_t xs = 0., ys = 0., zs = 0.;
    Double_t xsmin = 0., ysmin = 0., xsmax = 0., ysmax = 0.;
    Double_t z_tof700_average = 0.;
    for (int c=0; c<NUMBER_CHAMBERS; c++)
    {
	halfwidthy[c]=0.5f;
	if (idchambers[c] < 100.f)
	{
	    halfwidthx[c]=8.0f;
	}
	else
	{
	    halfwidthx[c]=28.0f;
	}
	wmi[c] = WMIN;
	wma[c] = WMAX;
	int nstr = 32;
	if (idchambers[c] > 100.f) nstr = 16;
	TOF2->get_strip_xyz(c,0, &xs,&ys,&zs);
	if (idchambers[c] != 19.3f) z_tof700_average += zs;
	xsmin = xs - halfwidthx[c];
	xsmax = xs + halfwidthx[c];
	ysmin = ys - halfwidthy[c];
	ysmax = ys + halfwidthy[c];
	if (xsmin < xmintof700) xmintof700 = xsmin;
	if (xsmax > xmaxtof700) xmaxtof700 = xsmax;
	if (ysmin < ymintof700) ymintof700 = ysmin;
	if (ysmax > ymaxtof700) ymaxtof700 = ysmax;
	TOF2->get_strip_xyz(c,nstr-1, &xs,&ys,&zs);
	xsmin = xs - halfwidthx[c];
	xsmax = xs + halfwidthx[c];
	ysmin = ys - halfwidthy[c];
	ysmax = ys + halfwidthy[c];
	if (xsmin < xmintof700) xmintof700 = xsmin;
	if (xsmax > xmaxtof700) xmaxtof700 = xsmax;
	if (ysmin < ymintof700) ymintof700 = ysmin;
	if (ysmax > ymaxtof700) ymaxtof700 = ysmax;
    }
    xmintof700 -= DXBORDER;
    xmaxtof700 += DXBORDER;
    ymintof700 -= DYBORDER;
    ymaxtof700 += DYBORDER;

    z_tof700_average /= (NUMBER_CHAMBERS-1);

    char name[128], title[128];

    gStyle->SetOptFit(111);
    gStyle->SetOptStat(111111);

    readMCTimeFile();

    if (CALIBRATION == 0)
    {
	char fname_calib_root[128];
#if CALRUNNAME == 0
	TString dir = getenv("VMCWORKDIR");
#ifdef CORRECTIONFILE
	TString path = dir + "/input/" + CORRECTIONFILE;
#else
	TString path = dir + "/input/" + CALIBFILE;
#endif
	strcpy(fname_calib_root, path.Data());
#else
	char *delim = 0;
	strcpy(fname_calib_root, gSystem->BaseName(fnametof700));
	if ((delim = strrchr(fname_calib_root, (int)'.'))) *delim = '\0';
	strcat(fname_calib_root, "_calibration.root");
#endif
	TFile *fc = new TFile(fname_calib_root,"READ", "Proton mass based calibration of BmnTOF700");
	if (CVERS == 0)
	{
	    itcalibr = (TProfile2D *)fc->Get("tcalibr");
	    itcalibrc = (TProfile *)fc->Get("tcalibrc");
	}
	else
	{
	    itcalibr = (TProfile2D *)fc->Get("tcalibr;1");
	    itcalibrc = (TProfile *)fc->Get("tcalibrc;1");
	}
	printf("\n ******************* Time offsets for whole chamber **********************\n");
	for (int c=0; c<NUMBER_CHAMBERS; c++) { tofcalc[c] = itcalibrc->GetBinContent(c+1); printf("%d %f\n",c,tofcalc[c]); }
	if (STRIP_CORRECTIONS)
	{
	    if (LIST_STRIP_CORRECTIONS) printf("\n ******************* Time offsets for one strip **************************\n");
	    for (int c=0; c<NUMBER_CHAMBERS; c++)
	    {
		if (LIST_STRIP_CORRECTIONS) printf("\n Chamber  %d %.1f\n",c,idchambers[c]);
		int smax = 32;
		if (idchambers[c] >= 100.f) smax = 16;
		for (int s=0; s<smax; s++)
		{
		    tofcals[c][s] = itcalibr->GetBinContent(c+1,s+1);
		    if (LIST_STRIP_CORRECTIONS) printf("   strip %d %f\n",s,tofcals[c][s]);
		}
		if (LIST_STRIP_CORRECTIONS) printf("\n *************************************************************************\n");
	    }
	}
    }

    if (strlen(fnametof700))
    {
	char fname_root[128], *delim = 0;
	strcpy(fname_root, gSystem->BaseName(fnametof700));
	if ((delim = strrchr(fname_root, (int)'.'))) *delim = '\0';
	if (TRACKS_ONLY) strcat(fname_root, "_efficiency_ntr_results.root");
	else             strcat(fname_root, "_efficiency_results.root");
	f = new TFile(fname_root,"RECREATE","Results of BmnTOF700");
    }

    selecting_records_target();

#if XGEMCOR != 0
			if (Xgemcorrection(1., &DTXcor, &DXsigma, &DYsigma) == -777.) return;
			DXsigma = 0.;
			DYsigma = 0.;
			DTXcor = 0.;
#endif

    TH1D *htarget = new TH1D("htarget","Statistic vs target", 10,0,10);
    TH1D *hntr = new TH1D("hntr","Number of all tracks", 200,0,200);
    TH1D *hntr0 = new TH1D("hntr0","Number of selected tracks", 200,0,200);
    TH1D *hntr1 = new TH1D("hntr1","Number of tracks with FLAG conditions", 200,0,200);

    TH1D *hntrgem0 = new TH1D("hntrgem0","Number of GEM tracks", 200,0,200);
    TH1D *hntrgemcut  = new TH1D("hntrgemcut","Number of GEM tracks after cuts", 200,0,200);
    TH1D *hntrgem  = new TH1D("hntrgem","Number of good GEM tracks", 200,0,200);
    TH1D *hntrdch0 = new TH1D("hntrdch0","Number of DCH tracks", 200,0,200);
    TH1D *hntrdch = new TH1D("hntrdch","Number of good DCH tracks", 200,0,200);
    TH2D *hntrgemdch = new TH2D("hntrgemdch","Number of good DCH vs GEM tracks", 50,0,50,50,0,50);

    TH2D *hxydch = new TH2D("hxydch","XY DCH refitted tracks", 70,-140,140,60,-90,90);
    hxydch->SetOption("COLZ");
    hxydch->Sumw2();
    TH2D *hxydch1 = new TH2D("hxydch1","XY DCH tracks", 70,-140,140,60,-90,90);
    hxydch1->SetOption("COLZ");
    hxydch1->Sumw2();
    TH2D *hxygem = new TH2D("hxygem","XY GEM tracks", 70,-140,140,60,-90,90);
    hxygem->SetOption("COLZ");
    hxygem->Sumw2();
    TProfile2D *pxygem_p  = new TProfile2D("pxygem_p","Momentum variation vs XY GEM positive tracks", 70,-140,140,60,-90,90,0,5,"s");
    pxygem_p->Sumw2();
    TProfile2D *pxygem_pn = new TProfile2D("pxygem_pn","Momentum variation vs XY GEM negative tracks", 70,-140,140,60,-90,90,0,5,"s");
    pxygem_pn->Sumw2();
    TProfile2D *pxygem_tx = new TProfile2D("pxygem_tx","TX variation vs XY GEM positive tracks", 70,-140,140,60,-90,90,-0.5,+0.5,"s");
    pxygem_tx->Sumw2();
    TProfile2D *pxygem_txn = new TProfile2D("pxygem_txn","TX variation vs XY GEM negative tracks", 70,-140,140,60,-90,90,-0.5,+0.5,"s");
    pxygem_txn->Sumw2();
    TProfile2D *pxygem_ty = new TProfile2D("pxygem_ty","TY variation vs XY GEM positive tracks", 70,-140,140,60,-90,90,-0.5,+0.5,"s");
    pxygem_ty->Sumw2();
    TProfile2D *pxygem_tyn = new TProfile2D("pxygem_tyn","TY variation vs XY GEM negative tracks", 70,-140,140,60,-90,90,-0.5,+0.5,"s");
    pxygem_tyn->Sumw2();

    TH2D *hxygem0 = new TH2D("hxygem0","XY GEM tracks (1st plane)", 20,-10,10,20,-10,10);
    hxygem0->SetOption("COLZ");
    hxygem0->Sumw2();
    TH2D *hxygem0p = new TH2D("hxygem0p","XY GEM positive tracks (1st plane)", 20,-10,10,20,-10,10);
    hxygem0p->SetOption("COLZ");
    hxygem0p->Sumw2();
    TH2D *hxygem0n = new TH2D("hxygem0n","XY GEM negative tracks (1st plane)", 20,-10,10,20,-10,10);
    hxygem0n->SetOption("COLZ");
    hxygem0n->Sumw2();
    TProfile2D *pxygem0_p  = new TProfile2D("pxygem0_p","Momentum variation vs XY GEM positive tracks (1st plane)", 20,-10,10,20,-10,10,0,5,"s");
    pxygem0_p->Sumw2();
    TProfile2D *pxygem0_pn = new TProfile2D("pxygem0_pn","Momentum variation vs XY GEM negative tracks (1st plane)", 20,-10,10,20,-10,10,0,5,"s");
    pxygem0_pn->Sumw2();
    TProfile2D *pxygem0_tx = new TProfile2D("pxygem0_tx","TX variation vs XY GEM positive tracks (1st plane)", 20,-10,10,20,-10,10,-0.5,+0.5,"s");
    pxygem0_tx->Sumw2();
    TProfile2D *pxygem0_txn = new TProfile2D("pxygem0_txn","TX variation vs XY GEM negative tracks (1st plane)", 20,-10,10,20,-10,10,-0.5,+0.5,"s");
    pxygem0_txn->Sumw2();
    TProfile2D *pxygem0_ty = new TProfile2D("pxygem0_ty","TY variation vs XY GEM positive tracks (1st plane)", 20,-10,10,20,-10,10,-0.5,+0.5,"s");
    pxygem0_ty->Sumw2();
    TProfile2D *pxygem0_tyn = new TProfile2D("pxygem0_tyn","TY variation vs XY GEM negative tracks (1st plane)", 20,-10,10,20,-10,10,-0.5,+0.5,"s");
    pxygem0_tyn->Sumw2();
    TH2D *hxygoodtrgem0 = new TH2D("hxygoodtrgem0","XY (gem 1st plane) GEM+DCH+TOF700 good tracks", 20,-10,10,20,-10,10);
    hxygoodtrgem0->SetOption("COLZ");
    hxygoodtrgem0->Sumw2();
    TH2D *hxyefftrgem0 = new TH2D("hxyefftrgem0","XY (gem 1st plane) GEM+DCH+TOF700 good tracks", 20,-10,10,20,-10,10);
    hxyefftrgem0->SetOption("COLZ");
    hxyefftrgem0->Sumw2();
    TH2D *hxygoodtrgem0p = new TH2D("hxygoodtrgem0p","XY (gem 1st plane) GEM+DCH+TOF700 good positive tracks", 20,-10,10,20,-10,10);
    hxygoodtrgem0p->SetOption("COLZ");
    hxygoodtrgem0p->Sumw2();
    TH2D *hxyefftrgem0p = new TH2D("hxyefftrgem0p","XY (gem 1st plane) GEM+DCH+TOF700 efficiency positive tracks", 20,-10,10,20,-10,10);
    hxyefftrgem0p->SetOption("COLZ");
    hxyefftrgem0p->Sumw2();
    TH2D *hxygoodtrgem0n = new TH2D("hxygoodtrgem0n","XY (gem 1st plane) GEM+DCH+TOF700 good negative tracks", 20,-10,10,20,-10,10);
    hxygoodtrgem0n->SetOption("COLZ");
    hxygoodtrgem0n->Sumw2();
    TH2D *hxyefftrgem0n = new TH2D("hxyefftrgem0n","XY (gem 1st plane) GEM+DCH+TOF700 efficiency negative tracks", 20,-10,10,20,-10,10);
    hxyefftrgem0n->SetOption("COLZ");
    hxyefftrgem0n->Sumw2();

    TH1D *hptr = new TH1D("hptr","Momentum of GEM tracks", 1000,-10,10);
    TH1D *hpgemdch = new TH1D("hpgemdch","Momentum of GEM+DCH tracks", 1000,-10,10);
    hpgemdch->Sumw2();
    TH1D *hpgemtof = new TH1D("hpgemtof","Momentum of GEMTOF tracks", 1000,-10,10);
    hpgemtof->Sumw2();
    TH1D *hpgemdch2 = new TH1D("hpgemdch2","Momentum of GEMTOF+DCH tracks", 1000,-10,10);
    hpgemdch2->Sumw2();
    TH1D *hpgoodhit = new TH1D("hpgoodhit","Momentum of GEM+DCH+TOF700 tracks", 1000,-10,10);
    hpgoodhit->Sumw2();
    TH1D *hpgoodhit2 = new TH1D("hpgoodhit2","Momentum of GEMTOF+DCH+TOF700 tracks", 1000,-10,10);
    hpgoodhit2->Sumw2();
    TH1D *hpbadhit = new TH1D("hpbadhit","Momentum of GEM+DCH+NoTOF700 tracks", 1000,-10,10);
    TH1D *hp0  = new TH1D("hp0","Momentum of tracks (all)", 1000,-10,10);
    TH1D *hltr = new TH1D("hltr","Lenght of GEM tracks", 100,500,700);
    TH1D *hltra = new TH1D("hltra","Lenght of GEM+DCH tracks", 100,500,700);
    TH2D *hxytrtof = new TH2D("hxytrtof","XY tracks with TOF700 hits", 1500,-150,150,1000,-100,100);
    hxytrtof->SetOption("COLZ");

// TOF700 hits
    TH2D *hxyhit = new TH2D("hxyhit","XY track hit distance from TOF700 hit", 200,-50,50,40,-10,10);
    hxyhit->SetOption("COLZ");
    TH2D *hxyghit = new TH2D("hxyghit","XY GEM track hit distance from DCH1 hit", 200,-50,50,40,-10,10);
    hxyghit->SetOption("COLZ");
    TH2D *hxyhit1 = new TH2D("hxyhit1","XY track hit distance from TOF700 hit (DCH track)", 200,-50,50,40,-10,10);
    hxyhit1->SetOption("COLZ");
    TH2D *hxyhit2 = new TH2D("hxyhit2","XY track hit distance from TOF700 hit (GEMTOFDCH track)", 200,-50,50,40,-10,10);
    hxyhit2->SetOption("COLZ");
    TH2D *hxyhit2g = new TH2D("hxyhit2g","XY track hit distance from TOF700 hit (GEMTOF track)", 200,-50,50,40,-10,10);
    hxyhit2g->SetOption("COLZ");
    // GEM+DCH track
    TH2D *hhitdxvsx = new TH2D("hhitdxvsx","X track distance from TOF700 hit vs X", 50,-50,150,50,-2.5,2.5);
    hhitdxvsx->SetOption("COLZ");
    //TH2D *hhitdxvsy = new TH2D("hhitdxvsy","X track hit distance from TOF700 hit vs Y", 25,-10,90,50,-2.5,2.5);
    //TH2D *hhitdyvsx = new TH2D("hhitdyvsx","Y track hit distance from TOF700 hit vs X", 50,-50,150,50,-2.5,2.5);
    //TH2D *hhitdyvsy = new TH2D("hhitdyvsy","Y track hit distance from TOF700 hit vs Y", 25,-10,90,50,-2.5,2.5);
    TProfile *phitdxvsx = new TProfile("phitdxvsx","Average X track hit distance from TOF700 hit vs X", 50,-50,150,-10,10,"s");
    TProfile *phitdxvsy = new TProfile("phitdxvsy","Average X track hit distance from TOF700 hit vs Y", 25,-10,90,-10,10,"s");
    TProfile *phitdyvsx = new TProfile("phitdyvsx","Average Y track hit distance from TOF700 hit vs X", 50,-50,150,-10,10,"s");
    TProfile *phitdyvsy = new TProfile("phitdyvsy","Average Y track hit distance from TOF700 hit vs Y", 25,-10,90,-10,10,"s");
    TH2D *hhitdxvsp = new TH2D("hhitdxvsp","X track distance from TOF700 hit vs momentum", 50,0,5,100,-5,5);
    hhitdxvsp->SetOption("COLZ");
    TH2D *hhitdxvspn = new TH2D("hhitdxvspn","X track distance from TOF700 hit vs momentum (negative)", 50,0,5,100,-5,5);
    hhitdxvspn->SetOption("COLZ");
    TH2D *hhitdxvsp1 = new TH2D("hhitdxvsp1","X track distance from TOF700 hit vs momentum (X<0)", 50,0,5,100,-5,5);
    TH2D *hhitdxvsp2 = new TH2D("hhitdxvsp2","X track distance from TOF700 hit vs momentum (0<X<40)", 50,0,5,100,-5,5);
    TH2D *hhitdxvsp3 = new TH2D("hhitdxvsp3","X track distance from TOF700 hit vs momentum (40<X<80)", 50,0,5,100,-5,5);
    TH2D *hhitdxvsp4 = new TH2D("hhitdxvsp4","X track distance from TOF700 hit vs momentum (80<X<120)", 50,0,5,100,-5,5);
    TH2D *hhitdxvsp5 = new TH2D("hhitdxvsp5","X track distance from TOF700 hit vs momentum (X>120)", 50,0,5,100,-5,5);
    TH2D *hhitdyvsp = new TH2D("hhitdyvsp","Y track distance from TOF700 hit vs momentum", 50,0,5,100,-5,5);
    hhitdyvsp->SetOption("COLZ");
    TH2D *hhitdyvspn = new TH2D("hhitdyvspn","Y track distance from TOF700 hit vs momentum (negative)", 50,0,5,100,-5,5);
    hhitdyvspn->SetOption("COLZ");

    // GEM track
    TH2D *hhitdxvsx2 = new TH2D("hhitdxvsx2","X track distance from TOF700 hit vs X (GEMTOFDCH track)", 50,-50,150,50,-2.5,2.5);
    hhitdxvsx2->SetOption("COLZ");
    TProfile *phitdxvsx2 = new TProfile("phitdxvsx2","Average X track hit distance from TOF700 hit vs X (GEMTOFDCH track)", 50,-50,150,-10,10,"s");
    TProfile *phitdxvsy2 = new TProfile("phitdxvsy2","Average X track hit distance from TOF700 hit vs Y (GEMTOFDCH track)", 25,-10,90,-10,10,"s");
    TProfile *phitdyvsx2 = new TProfile("phitdyvsx2","Average Y track hit distance from TOF700 hit vs X (GEMTOFDCH track)", 50,-50,150,-10,10,"s");
    TProfile *phitdyvsy2 = new TProfile("phitdyvsy2","Average Y track hit distance from TOF700 hit vs Y (GEMTOFDCH track)", 25,-10,90,-10,10,"s");
    TH2D *hhitdxvsp22 = new TH2D("hhitdxvsp22","X GEMTOFDCH track distance from TOF700 hit vs momentum", 50,0,5,100,-5,5);
    hhitdxvsp22->SetOption("COLZ");
    TH2D *hhitdxvspn2 = new TH2D("hhitdxvspn2","X GEMTOFDCH track distance from TOF700 hit vs momentum (negative)", 50,0,5,100,-5,5);
    hhitdxvspn2->SetOption("COLZ");
    TH2D *hhitdyvsp2 = new TH2D("hhitdyvsp2","Y GEMTOFDCH track distance from TOF700 hit vs momentum", 50,0,5,100,-5,5);
    hhitdyvsp2->SetOption("COLZ");
    TH2D *hhitdyvspn2 = new TH2D("hhitdyvspn2","Y GEMTOFDCH track distance from TOF700 hit vs momentum (negative)", 50,0,5,100,-5,5);
    hhitdyvspn2->SetOption("COLZ");

    TH2D *hhitdxvsx2g = new TH2D("hhitdxvsx2g","X track distance from TOF700 hit vs X (GEMTOF track)", 50,-50,150,50,-2.5,2.5);
    hhitdxvsx2g->SetOption("COLZ");
    TH2D *hhitdxvsp2g = new TH2D("hhitdxvsp2g","X GEMTOF track distance from TOF700 hit vs momentum", 50,0,5,100,-5,5);
    hhitdxvsp2g->SetOption("COLZ");
    TH2D *hhitdxvspn2g = new TH2D("hhitdxvspn2g","X GEMTOF track distance from TOF700 hit vs momentum (negative)", 50,0,5,100,-5,5);
    hhitdxvspn2g->SetOption("COLZ");
    TH2D *hhitdyvsp2g = new TH2D("hhitdyvsp2g","Y GEMTOF track distance from TOF700 hit vs momentum", 50,0,5,100,-5,5);
    hhitdyvsp2g->SetOption("COLZ");
    TH2D *hhitdyvspn2g = new TH2D("hhitdyvspn2g","Y GEMTOFDCH track distance from TOF700 hit vs momentum (negative)", 50,0,5,100,-5,5);
    hhitdyvspn2g->SetOption("COLZ");

    // DCH track
    TH2D *hhitdxvsx1 = new TH2D("hhitdxvsx1","X track distance from TOF700 hit vs X (DCH track)", 50,-50,150,50,-2.5,2.5);
    hhitdxvsx1->SetOption("COLZ");
    TProfile *phitdxvsx1 = new TProfile("phitdxvsx1","Average X track hit distance from TOF700 hit vs X (DCH track)", 50,-50,150,-10,10,"s");
    TProfile *phitdxvsy1 = new TProfile("phitdxvsy1","Average X track hit distance from TOF700 hit vs Y (DCH track)", 25,-10,90,-10,10,"s");
    TProfile *phitdyvsx1 = new TProfile("phitdyvsx1","Average Y track hit distance from TOF700 hit vs X (DCH track)", 50,-50,150,-10,10,"s");
    TProfile *phitdyvsy1 = new TProfile("phitdyvsy1","Average Y track hit distance from TOF700 hit vs Y (DCH track)", 25,-10,90,-10,10,"s");
    TH2D *hhitdxvsp11 = new TH2D("hhitdxvsp11","X DCH track distance from TOF700 hit vs momentum", 50,0,5,100,-5,5);
    hhitdxvsp11->SetOption("COLZ");
    TH2D *hhitdxvspn1 = new TH2D("hhitdxvspn1","X DCH track distance from TOF700 hit vs momentum (negative)", 50,0,5,100,-5,5);
    hhitdxvspn1->SetOption("COLZ");
    TH2D *hhitdyvsp1 = new TH2D("hhitdyvsp1","Y DCH track distance from TOF700 hit vs momentum", 50,0,5,100,-5,5);
    hhitdyvsp1->SetOption("COLZ");
    TH2D *hhitdyvspn1 = new TH2D("hhitdyvspn1","Y DCH track distance from TOF700 hit vs momentum (negative)", 50,0,5,100,-5,5);
    hhitdyvspn1->SetOption("COLZ");
#if REFIT != 0
    TH1D *hdxrefit1 = new TH1D("hdxrefit1","TX(refit)-TX(dch)", 200,-5,+5);
    TH1D *hdyrefit1 = new TH1D("hdyrefit1","TX(refit)-TX(dch)", 200,-5,+5);
    TH1D *hdtxrefit1 = new TH1D("hdtxrefit1","TX(refit)-TX(dch)", 500,-0.5,+0.5);
    TH1D *hdtyrefit1 = new TH1D("hdtyrefit1","TY(refit)-TY(dch)", 500,-0.5,+0.5);
    TH1D *hdxrefit = new TH1D("hdxrefit","TX(refit)-TX(gem)", 200,-5,+5);
    TH1D *hdyrefit = new TH1D("hdyrefit","TX(refit)-TX(gem)", 200,-5,+5);
    TH1D *hdtxrefit = new TH1D("hdtxrefit","TX(refit)-TX(gem)", 500,-0.5,+0.5);
    TH1D *hdtyrefit = new TH1D("hdtyrefit","TY(refit)-TY(gem)", 500,-0.5,+0.5);
    TH1D *hdprefit = new TH1D("hdprefit","P(refit)-P(gem)", 200,-0.5,+0.5);
#endif

// DCH1 hits
    TH2D *hghitdxvsx = new TH2D("hghitdxvsx","X track distance from DCH1 hit vs X", 50,-50,150,50,-2.5,2.5);
    hghitdxvsx->SetOption("COLZ");
    TH2D *hghitdxvsp = new TH2D("hghitdxvsp","X GEM track distance from DCH1 hit vs momentum", 50,0,5,100,-5,5);
    hghitdxvsp->SetOption("COLZ");
    TH2D *hghitdxvspn = new TH2D("hghitdxvspn","X GEM track distance from DCH1 hit vs momentum (negative)", 50,0,5,100,-5,5);
    hghitdxvspn->SetOption("COLZ");
    TH2D *hghitdxvspa = new TH2D("hghitdxvspa","X GEM track distance from DCH1 hit vs momentum (all combinations)", 50,0,5,200,-10,10);
    hghitdxvspa->SetOption("COLZ");
    TH2D *hghitdxvspw1 = new TH2D("hghitdxvspw1","X GEM track distance from DCH1 hit vs momentum (all combinations, default window)", 50,0,5,200,-10,10);
    hghitdxvspw1->SetOption("COLZ");
    TH2D *hghitdxvspw2 = new TH2D("hghitdxvspw2","X GEM track distance from DCH1 hit vs momentum (all combinations, double window)", 50,0,5,200,-10,10);
    hghitdxvspw2->SetOption("COLZ");
    TH2D *hghitdxvspw3 = new TH2D("hghitdxvspw3","X GEM track distance from DCH1 hit vs momentum (all combinations, 2*sigma window)", 50,0,5,200,-10,10);
    hghitdxvspw3->SetOption("COLZ");

    TH2D *hghitdxvspan = new TH2D("hghitdxvspan","X GEM track distance from DCH1 hit vs momentum (negative, all combinations)", 50,0,5,200,-10,10);
    hghitdxvspan->SetOption("COLZ");
    TH2D *hghitdxvspw1n = new TH2D("hghitdxvspw1n","X GEM track distance from DCH1 hit vs momentum (negative, all combinations, default window)", 50,0,5,200,-10,10);
    hghitdxvspw1n->SetOption("COLZ");
    TH2D *hghitdxvspw2n = new TH2D("hghitdxvspw2n","X GEM track distance from DCH1 hit vs momentum (negative, all combinations, double window)", 50,0,5,200,-10,10);
    hghitdxvspw2n->SetOption("COLZ");
    TH2D *hghitdxvspw3n = new TH2D("hghitdxvspw3n","X GEM track distance from DCH1 hit vs momentum (negative, all combinations, 2*sigma window)", 50,0,5,200,-10,10);
    hghitdxvspw3n->SetOption("COLZ");

    TH2D *hghitdxvspar = new TH2D("hghitdxvspar","X GEM track distance from DCH1 hit vs momentum (non-shifted, all combinations)", 50,0,5,200,-10,10);
    hghitdxvspar->SetOption("COLZ");
    TH2D *hghitdxvspw1r = new TH2D("hghitdxvspw1r","X GEM track distance from DCH1 hit vs momentum (non-shifted, all combinations, default window)", 50,0,5,200,-10,10);
    hghitdxvspw1r->SetOption("COLZ");
    TH2D *hghitdxvspw2r = new TH2D("hghitdxvspw2r","X GEM track distance from DCH1 hit vs momentum (non-shifted, all combinations, double window)", 50,0,5,200,-10,10);
    hghitdxvspw2r->SetOption("COLZ");
    TH2D *hghitdxvspw3r = new TH2D("hghitdxvspw3r","X GEM track distance from DCH1 hit vs momentum (non-shifted, all combinations, 2*sigma window)", 50,0,5,200,-10,10);
    hghitdxvspw3r->SetOption("COLZ");

    TH2D *hghitdxvsparn = new TH2D("hghitdxvsparn","X GEM track distance from DCH1 hit vs momentum (negative, non-shifted, all combinations)", 50,0,5,200,-10,10);
    hghitdxvsparn->SetOption("COLZ");
    TH2D *hghitdxvspw1rn = new TH2D("hghitdxvspw1rn","X GEM track distance from DCH1 hit vs momentum (negative, non-shifted, all combinations, default window)", 50,0,5,200,-10,10);
    hghitdxvspw1rn->SetOption("COLZ");
    TH2D *hghitdxvspw2rn = new TH2D("hghitdxvspw2rn","X GEM track distance from DCH1 hit vs momentum (negative, non-shifted, all combinations, double window)", 50,0,5,200,-10,10);
    hghitdxvspw2rn->SetOption("COLZ");
    TH2D *hghitdxvspw3rn = new TH2D("hghitdxvspw3rn","X GEM track distance from DCH1 hit vs momentum (negative, non-shifted, all combinations, 2*sigma window)", 50,0,5,200,-10,10);
    hghitdxvspw3rn->SetOption("COLZ");


    TH2D *hghitdxvsp1 = new TH2D("hghitdxvsp1","X GEM track distance from DCH1 hit vs momentum (X<0)", 50,0,5,100,-5,5);
    TH2D *hghitdxvsp2 = new TH2D("hghitdxvsp2","X GEM track distance from DCH1 hit vs momentum (0<X<40)", 50,0,5,100,-5,5);
    TH2D *hghitdxvsp3 = new TH2D("hghitdxvsp3","X GEM track distance from DCH1 hit vs momentum (40<X<80)", 50,0,5,100,-5,5);
    TH2D *hghitdxvsp4 = new TH2D("hghitdxvsp4","X GEM track distance from DCH1 hit vs momentum (80<X<120)", 50,0,5,100,-5,5);
    TH2D *hghitdxvsp5 = new TH2D("hghitdxvsp5","X GEM track distance from DCh1 hit vs momentum (X>120)", 50,0,5,100,-5,5);

    TH2D *hghitdyvsp = new TH2D("hghitdyvsp","Y GEM track distance from DCH1 hit vs momentum", 50,0,5,100,-5,5);
    hghitdyvsp->SetOption("COLZ");
    TH2D *hghitdyvspn = new TH2D("hghitdyvspn","Y GEM track distance from DCH1 hit vs momentum (negative)", 50,0,5,100,-5,5);
    hghitdyvspn->SetOption("COLZ");
    TH2D *hghitdyvspa = new TH2D("hghitdyvspa","Y GEM track distance from DCH1 hit vs momentum (all combinations)", 50,0,5,200,-10,10);
    hghitdyvspa->SetOption("COLZ");
    TH2D *hghitdyvspw1 = new TH2D("hghitdyvspw1","Y GEM track distance from DCH1 hit vs momentum (all combinations, default window)", 50,0,5,200,-10,10);
    hghitdyvspw1->SetOption("COLZ");
    TH2D *hghitdyvspw2 = new TH2D("hghitdyvspw2","Y GEM track distance from DCH1 hit vs momentum (all combinations, double window)", 50,0,5,200,-10,10);
    hghitdyvspw2->SetOption("COLZ");
    TH2D *hghitdyvspw3 = new TH2D("hghitdyvspw3","Y GEM track distance from DCH1 hit vs momentum (all combinations, 2*sigma window)", 50,0,5,200,-10,10);
    hghitdyvspw3->SetOption("COLZ");
//
    TH1D *hnwin1 = new TH1D("hnwin1","Number of pairs in default DXDY window", 10,0,10);
    TH1D *hnwin2 = new TH1D("hnwin2","Number of pairs in twice wider  DXDY window", 10,0,10);
    TH1D *hnwins = new TH1D("hnwins","Number of pairs in default DXDY N-sigma window", 10,0,10);

    TH1D *hchambigx = new TH1D("hchambigx","Chambers for X > 110", 60,0,60);
    TH1D *hchambigy = new TH1D("hchambigy","Chambers for Y > 75", 60,0,60);
    TH2D *hxvsxbigx = new TH2D("hxvsxbigx","Xtr vs Xhit for big X", 25,100,150,25,100,150);
    TH2D *hyvsybigy = new TH2D("hyvsybigy","Ytr vs Yhit for big Y", 40,70,90,40,70,90);
    TH1D *hpbigx = new TH1D("hpbigx","Momentum for X > 110", 500,-10,10);
    TH1D *hpbigy = new TH1D("hpbigy","Momentum for Y > 75", 500,-10,10);
    TH1D *hybigx = new TH1D("hybigx","Y for X > 110", 25,-10,90);
    TH1D *hxbigy = new TH1D("hxbigy","X for Y > 75", 50,-50,150);
    TH1D *hlrbigx = new TH1D("hlrbigx","LR difference for TOF700 hits with track (big X)", 100,-5,+5);
    TH2D *hxxhitbigx = new TH2D("hxxhitbigx","X-X0 track hit vs TOF700 hit (big X)", 200,-100,100,200,-100,100);
    TH2D *hxyhitlr = new TH2D("hxyhitlr","XY track hit distance from TOF700 hit (lr cut)", 200,-50,50,40,-10,10);
    TH1D *hxdiffs = new TH1D("hxdiffs","X-X0 difference for TOF700 hits with track (small chambers)", 100,-50,+50);
    TH1D *hxdiffb = new TH1D("hxdiffb","X-X0 difference for TOF700 hits with track (big chambers)", 100,-50,+50);
    TH2D *hxxhits = new TH2D("hxxhits","X-X0 track hit vs TOF700 hit (small chambers)", 200,-100,100,200,-100,100);
    TH2D *hxxhitb = new TH2D("hxxhitb","X-X0 track hit vs TOF700 hit (big chambers)", 200,-100,100,200,-100,100);
    TH1D *hlrs = new TH1D("hlrs","LR difference for TOF700 hits with track (small chambers)", 100,-5,+5);
    TH1D *hlrb = new TH1D("hlrb","LR difference for TOF700 hits with track (big chambers)", 100,-5,+5);

    TH1D *htoftr = new TH1D("htoftr","TOF of tracks with TOF700 hits", 600,-20,40);
    TH1D *htof = new TH1D("htof","TOF for hits", 600,-20,40);
    TH1D *hntof = new TH1D("hntof","Number of hits", 100,0,100);
    TH1D *hncl = new TH1D("hncl","Number of all clusters", 100,0,100);
    TH1D *hncln = new TH1D("hncln","Number of all clusters without 19.2 chamber", 100,0,100);
    TH1D *hncls = new TH1D("hncls","Number of clusters (small chambers)", 100,0,100);
    TH1D *hnclsn = new TH1D("hnclsn","Number of clusters (small chambers without 19.2)", 100,0,100);
    TH1D *hnclb = new TH1D("hnclb","Number of clusters (big chambers)", 100,0,100);
    TH2D *hnclvschambs = new TH2D("hnclvschambs","Number of clusters vs chamber vs ncl (small)", 50,0,50,10,0,10);
    TH2D *hnclvschambb = new TH2D("hnclvschambb","Number of clusters vs chamber vs ncl (big)", 50,0,50,10,0,10);
    TH1D *hnclvschambs1 = new TH1D("hnclvschambs1","Number of clusters vs chamber(small)", 50,0,50);
    TH1D *hnclvschambb1 = new TH1D("hnclvschambb1","Number of clusters vs chamber (big)", 50,0,50);
    TH1D *hncltr = new TH1D("hncltr","Number of clusters with a track", 100,0,100);
    TH2D *hncltrvschambs = new TH2D("hncltrvschambs","Number of clusters with a track vs chamber vs ncl (small)", 50,0,50,10,0,10);
    TH2D *hncltrvschambb = new TH2D("hncltrvschambb","Number of clusters with a track vs chamber vs ncl (big)", 50,0,50,10,0,10);
    TH1D *hncltrvschambs1 = new TH1D("hncltrvschambs1","Number of clusters with a track vs chamber (small)", 50,0,50);
    TH1D *hncltrvschambb1 = new TH1D("hncltrvschambb1","Number of clusters with a track vs chamber (big)", 50,0,50);
//    TH2D *hnclchambeffs = new TH2D("hnclchambeffs","Efficiency of clusters with a track vs chamber vs ncl (small)", 50,0,50,10,0,10);
//    TH2D *hnclchambeffb = new TH2D("hnclchambeffb","Efficiency of clusters with a track vs chamber vs ncl (big)", 50,0,50,10,0,10);
    TH1D *hchambeffs = new TH1D("hchambeffs","Efficiency of clusters with a track vs chamber vs ncl (small)", 50,0,50);
    TH1D *hchambeffb = new TH1D("hchambeffb","Efficiency of clusters with a track vs chamber vs ncl (big)", 50,0,50);
    TH1D *hclnums = new TH1D("hclnums","Number of strips in cluster (small chambers)", 10,0,10);
    TH1D *hclnumb = new TH1D("hclnumb","Number of strips in cluster (big chambers)", 10,0,10);
    TH2D *hclwids = new TH2D("hclwids","Cluser amp vs chamber ID (small chambers)", 50,0,50,250,0,25000);
    TH2D *hclwidb = new TH2D("hclwidb","Cluser amp vs chamber ID (big chambers)", 50,0,50,250,0,25000);
    TH2D *hclamps = new TH2D("hclamps","Strip amp vs chamber ID (small chambers)", 50,0,50,250,0,25000);
    TH2D *hclampb = new TH2D("hclampb","Strip amp vs chamber ID (big chambers)", 50,0,50,250,0,25000);
    TH2D *hxycluster = new TH2D("hxycluster","XY TOF700 cluster", 70,-140,140,60,-90,90);
    hxycluster->SetOption("COLZ");
    hxycluster->Sumw2();
    TH2D *hxyhits = new TH2D("hxyhits","XY TOF700 hits", 20, 0, 20, 128,0,128);
    hxyhits->SetOption("COLZ");
    hxyhits->Sumw2();
    TH2D *hxystrips = new TH2D("hxystrips","XY TOF700 strips", 20, 0, 20, 128,0,128);
    hxystrips->SetOption("COLZ");
    hxystrips->Sumw2();
    TH2D *hxygoodcluster = new TH2D("hxygoodcluster","XY TOF700 cluster with GEM+DCH tracks", 70,-140,140,60,-90,90);
    hxygoodcluster->SetOption("COLZ");
    hxygoodcluster->Sumw2();
    TH2D *hxygoodstrips = new TH2D("hxygoodstrips","XY TOF700 strips with matched GEM+DCH tracks", 20, 0, 20, 128,0,128);
    hxygoodstrips->SetOption("COLZ");
    hxygoodstrips->Sumw2();
    TH2D *hxygood1strips = new TH2D("hxygood1strips","XY TOF700 strips with matched(1) GEM+DCH tracks", 20, 0, 20, 128,0,128);
    hxygood1strips->SetOption("COLZ");
    hxygood1strips->Sumw2();
    TH2D *hxytrackstrips = new TH2D("hxytrackstrips","XY TOF700 strips for GEM+DCH tracks", 20, 0, 20, 128,0,128);
    hxytrackstrips->SetOption("COLZ");
    hxytrackstrips->Sumw2();
    TH2D *hxygoodclustergem = new TH2D("hxygoodclustergem","XY TOF700 cluster with GEM tracks", 70,-140,140,60,-90,90);
    hxygoodclustergem->SetOption("COLZ");
    hxygoodclustergem->Sumw2();
    TH2D *hxygoodclusterdch = new TH2D("hxygoodclusterdch","XY TOF700 cluster with DCH tracks", 70,-140,140,60,-90,90);
    hxygoodclusterdch->SetOption("COLZ");
    hxygoodclusterdch->Sumw2();
    TH2D *hxyclustereff = new TH2D("hxyclustereff","Efficiency GEM+DCH vs XY TOF700 cluster", 70,-140,140,60,-90,90);
    hxyclustereff->SetOption("COLZ");
    hxyclustereff->Sumw2();
    TH2D *hxystripseff = new TH2D("hxystripseff","XY TOF700 strips efficiency (over hits)", 20, 0, 20, 128,0,128);
    hxystripseff->SetOption("COLZ");
    hxystripseff->Sumw2();
    TH2D *hxytrackstripseff = new TH2D("hxytrackstripseff","XY TOF700 strips efficiency (over tracks)", 20, 0, 20, 128,0,128);
    hxytrackstripseff->SetOption("COLZ");
    hxytrackstripseff->Sumw2();
    TH2D *hxytrackstripseff1 = new TH2D("hxytrackstripseff1","XY TOF700 strips efficiency (1) (over tracks)", 20, 0, 20, 128,0,128);
    hxytrackstripseff1->SetOption("COLZ");
    hxytrackstripseff1->Sumw2();
    TH2D *hxyclustereffgem = new TH2D("hxyclustereffgem","Efficiency GEM vs XY TOF700 cluster", 70,-140,140,60,-90,90);
    hxyclustereffgem->SetOption("COLZ");
    hxyclustereffgem->Sumw2();
    TH2D *hxyclustereffdch = new TH2D("hxyclustereffdch","Efficiency DCH vs XY TOF700 cluster", 70,-140,140,60,-90,90);
    hxyclustereffdch->SetOption("COLZ");
    hxyclustereffdch->Sumw2();

    TH1D *hnhit = new TH1D("hnhit","Number of hits in width limits", 100,0,100);
    TH2D *hxytof = new TH2D("hxytof","XY TOF700 hits", 1500,-150,150,1000,-100,100);
    hxytof->SetOption("COLZ");
    hxytof->Sumw2();
    TH2D *hpbeta = new TH2D("hpbeta","Beta vs momentum", 200,0,10,220,0,1.1f);
    hpbeta->SetOption("COLZ");
    TH2D *hpbetac = new TH2D("hpbetac","Beta vs momentum, chamber corrections", 200,0,10,220,0,1.1f);
    hpbetac->SetOption("COLZ");
    TH2D *hpbetas = new TH2D("hpbetas","Beta vs momentum, strip corrections", 200,0,10,220,0,1.1f);
    hpbetas->SetOption("COLZ");
    TH2D *hpbetacm = new TH2D("hpbetacm","Beta vs momentum, Z<0, chamber corrections", 200,0,10,220,0,1.1f);
    hpbetacm->SetOption("COLZ");
    TH2D *hpbetasm = new TH2D("hpbetasm","Beta vs momentum, Z<0, strip corrections", 200,0,10,220,0,1.1f);
    hpbetasm->SetOption("COLZ");
    TH1D *hmass = new TH1D("hmass", "Mass, p<2", 400,-0.5,9.5);
    TH1D *hmassc = new TH1D("hmassc", "Mass, chamber corrections", 400,-0.5,9.5);
    TH1D *hmassc15 = new TH1D("hmassc15", "Mass, chamber corrections, p<1.5", 400,-0.5,9.5);
    TH1D *hmassc20 = new TH1D("hmassc20", "Mass, chamber corrections, p<2.0", 400,-0.5,9.5);
    TH1D *hmasss = new TH1D("hmasss", "Mass, strip corrections", 400,-0.5,9.5);
    TH1D *hmasss15 = new TH1D("hmasss15", "Mass, strip corrections, p<1.5", 400,-0.5,9.5);
    TH1D *hmasss20 = new TH1D("hmasss20", "Mass, strip corrections, p<2.0", 400,-0.5,9.5);
    TH3D *hmasss15tt = new TH3D("hmasss15tt", "Mass vs trigger vs target, strip corrections, p<1.5", 400,-0.5,9.5,5,1,6,6,0,6);
    TH3D *hmasss20tt = new TH3D("hmasss20tt", "Mass vs trigger vs target, strip corrections, p<2.0", 400,-0.5,9.5,5,1,6,6,0,6);
    TH1D *hmasscm = new TH1D("hmasscm", "Mass, Z<0, chamber corrections", 200,-1,4);
    TH1D *hmasssm = new TH1D("hmasssm", "Mass, Z<0, strip corrections", 200,-1,4);

    TH1D *hdtps = new TH1D("hdtps", "Time resolution, protons, strip corrections", 200,-1.,+1.);
    TH2D *hdtpsvsp = new TH2D("hdtpsvsp", "Time resolution vs momentum, protons, strip corrections", 240,0.,6.,200,-1.,+1.);
    hdtpsvsp->SetOption("COLZ");
    TH3D *hdtpsvscvss = new TH3D("hdtpsvscvss", "Time resolution vs chamber vs strip, strip corrections", 59,0,59, 32,0.,32.,200,-1.,+1.);
    TH3D *hdtpcvscvsp = new TH3D("hdtpcvscvsp", "Time resolution vs chamber vs momentum, chamber corrections", 59,0,59, 60,0.,6.,200,-1.,+1.);

    TH1D *hdtpc = new TH1D("hdtpc", "Time resolution, protons, chamber corrections", 200,-1.,+1.);
    TH2D *hdtpcvsp = new TH2D("hdtpcvsp", "Time resolution vs momentum, protons, chamber corrections", 240,0.,6.,200,-1.,+1.);
    hdtpcvsp->SetOption("COLZ");
    TH2D *hdtpcvspwlim = new TH2D("hdtpcvspwlim", "Time resolution vs momentum, protons, chamber corrections, w>3000", 240,0.,6.,200,-1.,+1.);
    hdtpcvspwlim->SetOption("COLZ");
    TH1D *hdtpcplim = new TH1D("hdtpcplim", "Time resolution, protons, p>3", 200,-1.,+1.);
    TH1D *hdtpcp1lim = new TH1D("hdtpcp1lim", "Time resolution, protons, p>4", 200,-1.,+1.);
    TH1D *hdtpcp2lim = new TH1D("hdtpcp2lim", "Time resolution, protons, p<2", 200,-1.,+1.);
    TH1D *hdtpcp3lim = new TH1D("hdtpcp3lim", "Time resolution, protons, 2<p<5", 200,-1.,+1.);
    TH1D *hdtpcwlim = new TH1D("hdtpcwlim", "Time resolution, protons, w>3000", 200,-1.,+1.);
    TH1D *hdtpcwplim = new TH1D("hdtpcwplim", "Time resolution, protons, p>3, w>3000", 200,-1.,+1.);
    TH1D *hdtpcwp1lim = new TH1D("hdtpcwp1lim", "Time resolution, protons, p>4, w>3000", 200,-1.,+1.);

    TH2D *hmassvsp = new TH2D("hmassvsp", "Mass vs momentum", 240,0.,6.,200,-0.2,4.8);
    hmassvsp->SetOption("COLZ");
    TH2D *hmasssvsp = new TH2D("hmasssvsp", "Mass vs momentum, strip correction", 240,0.,6.,200,-0.2,4.8);
    hmasssvsp->SetOption("COLZ");
    TH2D *hmasscvsp = new TH2D("hmasscvsp", "Mass vs momentum, chamber corrections", 240,0.,6.,200,-0.2,4.8);
    hmasscvsp->SetOption("COLZ");
    TH2D *hmasssmvsp = new TH2D("hmasssmvsp", "Mass vs momentum, Z<0, strip correction", 240,0.,6.,200,-0.2,4.8);
    hmasssmvsp->SetOption("COLZ");
    TH2D *hmasscmvsp = new TH2D("hmasscmvsp", "Mass vs momentum, Z<0, chamber corrections", 240,0.,6.,200,-0.2,4.8);
    hmasscmvsp->SetOption("COLZ");

    TH1D *htofp= new TH1D("htofp","TOF for proton hits, p<2", 600,-20,40);

    TH1D *hstr = new TH1D("hstr","Strips", 32,0,32);
    TH1D *hcha = new TH1D("hcha","Chambers", 60,0,60);
    TH1D *hstrtr = new TH1D("hstrtr","Strips with tracks", 32,0,32);
    TH1D *hchatr = new TH1D("hchatr","Chambers with tracks", 60,0,60);

    TH2D *hdxy0 = new TH2D("hdxy0","DxDy all",40,-5,+5,40,-5,+5);
    hdxy0->SetOption("COLZ");
    TH2D *hgdtxvsp = new TH2D("hgdtxvsp","DTX vs P GEM-DCH (positive)",50,0,5,250,-0.5,+0.5);
    hgdtxvsp->SetOption("COLZ");
    TH2D *hgdtxvspn = new TH2D("hgdtxvspn","DTX vs P GEM-DCH (negative)",50,0,5,250,-0.5,+0.5);
    hgdtxvspn->SetOption("COLZ");
    TH2D *hgdtxrvsp = new TH2D("hgdtxrvsp","DTX vs P GEMDCH-DCH (positive)",50,0,5,250,-0.5,+0.5);
    hgdtxrvsp->SetOption("COLZ");
    TH2D *hgdtxrvspn = new TH2D("hgdtxrvspn","DTX vs P GEMDCH-DCH (negative)",50,0,5,250,-0.5,+0.5);
    hgdtxrvspn->SetOption("COLZ");
    TH2D *hgdtxvspall = new TH2D("hgdtxvspall","DTX vs P GEM-DCH (all pairs, positive)",50,0,5,250,-0.5,+0.5);
    hgdtxvspall->SetOption("COLZ");
    TH2D *hgdtxvspnall = new TH2D("hgdtxvspnall","DTX vs P GEM-DCH (all pairs, negative)",50,0,5,250,-0.5,+0.5);
    hgdtxvspnall->SetOption("COLZ");
    TH2D *hgdtxvsdxall = new TH2D("hgdtxvsdxall","DTX vs DX GEM-DCH (all pairs, positive)",100,-5,5,250,-0.5,+0.5);
    hgdtxvsdxall->SetOption("COLZ");
    TH2D *hgdtx1vsp = new TH2D("hgdtx1vsp","DX/LENG vs P GEM-DCH (positive)",50,0,5,250,-0.1,+0.1);
    hgdtx1vsp->SetOption("COLZ");
    TH2D *hgdtx1vspn = new TH2D("hgdtx1vspn","DX/LENG vs P GEM-DCH (negative)",50,0,5,250,-0.1,+0.1);
    hgdtx1vspn->SetOption("COLZ");
    TH2D *hgdtx1vspall = new TH2D("hgdtx1vspall","DX/LENG vs P GEM-DCH (all pairs, positive)",50,0,5,250,-0.5,+0.5);
    hgdtx1vspall->SetOption("COLZ");
    TH2D *hgdtx1vspnall = new TH2D("hgdtx1vspnall","DX/LENG vs P GEM-DCH (all pairs, negative)",50,0,5,250,-0.5,+0.5);
    hgdtx1vspnall->SetOption("COLZ");
    TH2D *hgdtxty = new TH2D("hgdtxty","DTY vs DTX GEM-DCH (positive)",250,-0.5,+0.5,250,-0.5,+0.5);
    hgdtxty->SetOption("COLZ");
    TH2D *hgdtxtyn = new TH2D("hgdtxtyn","DTY vs DTX GEM-DCH (negative)",250,-0.5,+0.5,250,-0.5,+0.5);
    hgdtxtyn->SetOption("COLZ");
    TH2D *hdxyreal = new TH2D("hdxyreal","DxDy all real",40,-5,+5,40,-5,+5);
    hdxyreal->SetOption("COLZ");
    TH2D *hdtxtyreal = new TH2D("hdtxtyreal","DtxDty all real",250,-0.5,+0.5,250,-0.5,+0.5);
    hdtxtyreal->SetOption("COLZ");
    TH2D *hdxyrealall = new TH2D("hdxyrealall","DxDy all combinations real",40,-5,+5,40,-5,+5);
    hdxyrealall->SetOption("COLZ");
    TH2D *hdtxtyrealall = new TH2D("hdtxtyrealall","DtxDty all combinations real",250,-0.5,+0.5,250,-0.5,+0.5);
    hdtxtyrealall->SetOption("COLZ");
    TH2D *hdtxtyrealallw = new TH2D("hdtxtyrealallw","DtxDty all combinations real in window",250,-0.5,+0.5,250,-0.5,+0.5);
    hdtxtyrealallw->SetOption("COLZ");
    TH1D *hdx = new TH1D("hdx","dx all",40,-5,+5);
    TH1D *hdy = new TH1D("hdy","dy all",40,-5,+5);
    TH2D *hdxy = new TH2D("hdxy","DxDy selected",40,-5,+5,40,-5,+5);
    hdxy->SetOption("COLZ");
    hdxy->Sumw2();
    TH2D *hdxy2 = new TH2D("hdxy2","DxDy selected (GEMTOF tracks)",40,-5,+5,40,-5,+5);
    hdxy2->SetOption("COLZ");
    hdxy2->Sumw2();
    TH2D *hdxytof = new TH2D("hdxytof","DxDy with TOF700 hits",40,-5,+5,40,-5,+5);
    hdxytof->SetOption("COLZ");
    hdxytof->Sumw2();
    TH2D *hdxytof2 = new TH2D("hdxytof2","DxDy with TOF700 hits (GEMTOF tracks)",40,-5,+5,40,-5,+5);
    hdxytof2->SetOption("COLZ");
    hdxytof2->Sumw2();
    TH2D *hdxyeff = new TH2D("hdxyeff","Efficiency vs DxDy",40,-5,+5,40,-5,+5);
    hdxyeff->SetOption("COLZ");
    hdxyeff->Sumw2();
    TH2D *hdxyeff2 = new TH2D("hdxyeff2","Efficiency vs DxDy (GEMTOF tracks)",40,-5,+5,40,-5,+5);
    hdxyeff2->SetOption("COLZ");
    hdxyeff2->Sumw2();
    TH1D *hdxeff = new TH1D("hdxeff","Efficiency vs Dx",40,-5,+5);
    TH1D *hdyeff = new TH1D("hdyeff","Efficiency vs Dy",40,-5,+5);
    TH2D *hxygoodtr = new TH2D("hxygoodtr","XY(dch) GEM+DCH good tracks", 70,-140,140,60,-90,90);
    hxygoodtr->SetOption("COLZ");
    hxygoodtr->Sumw2();
    TH2D *hxygoodtrp = new TH2D("hxygoodtrp","XY(dch) GEM+DCH good positive tracks", 70,-140,140,60,-90,90);
    hxygoodtrp->SetOption("COLZ");
    hxygoodtrp->Sumw2();
    TH2D *hxygoodtrn = new TH2D("hxygoodtrn","XY(dch) GEM+DCH good negative tracks", 70,-140,140,60,-90,90);
    hxygoodtrn->SetOption("COLZ");
    hxygoodtrn->Sumw2();
    TH2D *hxygoodtrgem = new TH2D("hxygoodtrgem","XY(gem) GEM+DCH good tracks", 70,-140,140,60,-90,90);
    hxygoodtrgem->SetOption("COLZ");
    hxygoodtrgem->Sumw2();
    TH2D *hxygoodtr2 = new TH2D("hxygoodtr2","XY(dch) GEMTOF+DCH good tracks", 70,-140,140,60,-90,90);
    hxygoodtr2->SetOption("COLZ");
    hxygoodtr2->Sumw2();
    TH2D *hxygoodtrgem2 = new TH2D("hxygoodtrgem2","XY(gem) GEMTOF+DCH good tracks", 70,-140,140,60,-90,90);
    hxygoodtrgem2->SetOption("COLZ");
    hxygoodtrgem2->Sumw2();
    TH2D *hxygoodhit = new TH2D("hxygoodhit","XY good tracks (GEM+DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhit->SetOption("COLZ");
    hxygoodhit->Sumw2();
    TH2D *hxygoodhitp = new TH2D("hxygoodhitp","XY good positive tracks (GEM+DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhitp->SetOption("COLZ");
    hxygoodhitp->Sumw2();
    TH2D *hxygoodhitn = new TH2D("hxygoodhitn","XY good negative tracks (GEM+DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhitn->SetOption("COLZ");
    hxygoodhitn->Sumw2();
    TH2D *hxygoodhit2 = new TH2D("hxygoodhit2","XY good tracks (GEMTOF+DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhit2->SetOption("COLZ");
    hxygoodhit2->Sumw2();
    TH2D *hxygoodhitgem2 = new TH2D("hxygoodhitgem2","XY (gem) good tracks (GEMTOF+DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhitgem2->SetOption("COLZ");
    hxygoodhitgem2->Sumw2();
    TH2D *hxygoodhita = new TH2D("hxygoodhita","XY good tracks (GEM+DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhita->SetOption("COLZ");
    hxygoodhita->Sumw2();
    TH2D *hxygoodhita2 = new TH2D("hxygoodhita2","XY good tracks (GEMTOF+DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhita2->SetOption("COLZ");
    hxygoodhita2->Sumw2();
    TH2D *hxygoodhit1 = new TH2D("hxygoodhit1","XY good tracks (DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhit1->SetOption("COLZ");
    hxygoodhit1->Sumw2();
    TH2D *hxybadhit = new TH2D("hxybadhit","XY good tracks (GEM+DCH) with no hits", 70,-140,140,60,-90,90);
    hxybadhit->SetOption("COLZ");
    TH2D *hxybadhita = new TH2D("hxybadhita","XY good tracks (GEM+DCH) with no hits", 70,-140,140,60,-90,90);
    hxybadhita->SetOption("COLZ");

    TH2D *hxygemtofdchtof = new TH2D("hxygemtofdchtof","XY(gem) GEMTOF and DCHTOF good tracks", 70,-140,140,60,-90,90);
    hxygemtofdchtof->SetOption("COLZ");
    hxygemtofdchtof->Sumw2();
    TH2D *hxygemtofdchtofd = new TH2D("hxygemtofdchtofd","XY(dch) GEMTOF and DCHTOF good tracks", 70,-140,140,60,-90,90);
    hxygemtofdchtofd->SetOption("COLZ");
    hxygemtofdchtofd->Sumw2();
    TH2D *hxygemtofdchtof1 = new TH2D("hxygemtofdchtof1","XY(gem) GEMTOF+DCHTOF good tracks", 70,-140,140,60,-90,90);
    hxygemtofdchtof1->SetOption("COLZ");
    hxygemtofdchtof1->Sumw2();
    TH2D *hxygemtofdchtofd1 = new TH2D("hxygemtofdchtofd1","XY(dch) GEMTOF+DCHTOF good tracks", 70,-140,140,60,-90,90);
    hxygemtofdchtofd1->SetOption("COLZ");
    hxygemtofdchtofd1->Sumw2();
    TH2D *hxygemtofdchtof2 = new TH2D("hxygemtofdchtof2","XY(gem) GEMTOF+DCHTOF good tracks (same TOF hit)", 70,-140,140,60,-90,90);
    hxygemtofdchtof2->SetOption("COLZ");
    hxygemtofdchtof2->Sumw2();
    TH2D *hxygemtofdchtofd2 = new TH2D("hxygemtofdchtofd2","XY(dch) GEMTOF+DCHTOF good tracks (same TOF hit)", 70,-140,140,60,-90,90);
    hxygemtofdchtofd2->SetOption("COLZ");
    hxygemtofdchtofd2->Sumw2();

    TH1D *hpgemtofdchtof = new TH1D("hpgemtofdchtof","P GEMTOF and DCHTOF good tracks", 1000, -10, 10);
    hpgemtofdchtof->SetOption("COLZ");
    hpgemtofdchtof->Sumw2();
    TH1D *hpgemtofdchtof1 = new TH1D("hpgemtofdchtof1","P GEMTOF+DCHTOF good tracks", 1000, -10, 10);
    hpgemtofdchtof1->SetOption("COLZ");
    hpgemtofdchtof1->Sumw2();
    TH1D *hpgemtofdchtof2 = new TH1D("hpgemtofdchtof2","P GEMTOF+DCHTOF good tracks (same TOF hit)", 1000, -10, 10);
    hpgemtofdchtof2->SetOption("COLZ");
    hpgemtofdchtof2->Sumw2();


// Efficiencies
    TH2D *hxyeff = new TH2D("hxyeff","Efficiency GEM+DCH+TOF700 vs XY good GEM+DCH tracks", 70,-140,140,60,-90,90);
    hxyeff->SetOption("COLZ");
    hxyeff->Sumw2();
    TH2D *hxyeffp = new TH2D("hxyeffp","Efficiency GEM+DCH+TOF700 vs XY good GEM+DCH positive tracks", 70,-140,140,60,-90,90);
    hxyeffp->SetOption("COLZ");
    hxyeffp->Sumw2();
    TH2D *hxyeffn = new TH2D("hxyeffn","Efficiency GEM+DCH+TOF700 vs XY good GEM+DCH negative tracks", 70,-140,140,60,-90,90);
    hxyeffn->SetOption("COLZ");
    hxyeffn->Sumw2();
    TH2D *hxyeff2 = new TH2D("hxyeff2","Efficiency GEMTOF+DCH+TOF700 vs XY good GEMTOF+DCH tracks", 70,-140,140,60,-90,90);
    hxyeff2->SetOption("COLZ");
    hxyeff2->Sumw2();
    TH1D *hxeff = new TH1D("hxeff","Efficiency GEM+DCH+TOF700 vs X good GEM+DCH tracks", 70,-140,140);
    TH1D *hyeff = new TH1D("hyeff","Efficiency GEM+DCH+TOF700 vs Y good GEM+DCH tracks", 60,-90,90);
    TH2D *hxyeffmg = new TH2D("hxyeffmg","Efficiency GEM+DCH vs XY good GEM tracks", 70,-140,140,60,-90,90);
    hxyeffmg->SetOption("COLZ");
    hxyeffmg->Sumw2();
    TH2D *hxyeffmg2 = new TH2D("hxyeffmg2","Efficiency GEMTOF+DCH vs XY good GEMTOF tracks", 70,-140,140,60,-90,90);
    hxyeffmg2->SetOption("COLZ");
    hxyeffmg2->Sumw2();
    TH2D *hxyeffmg2f = new TH2D("hxyeffmg2f","Efficiency GEMTOF+DCH vs XY good GEM tracks (full)", 70,-140,140,60,-90,90);
    hxyeffmg2f->SetOption("COLZ");
    hxyeffmg2f->Sumw2();
    TH2D *hxyeffmd = new TH2D("hxyeffmd","Efficiency GEM+DCH vs XY good DCH tracks", 70,-140,140,60,-90,90);
    hxyeffmd->SetOption("COLZ");
    hxyeffmd->Sumw2();
    TH2D *hxyeffm1d = new TH2D("hxyeffm1d","Efficiency DCH+TOF700 vs XY good DCH tracks", 70,-140,140,60,-90,90);
    hxyeffm1d->SetOption("COLZ");
    hxyeffm1d->Sumw2();
    TH2D *hxyeffdg = new TH2D("hxyeffdg","Efficiency DCH vs XY good GEM tracks", 70,-140,140,60,-90,90);
    hxyeffdg->SetOption("COLZ");
    hxyeffdg->Sumw2();
    TH2D *hxyeffgd = new TH2D("hxyeffgd","Efficiency GEM vs XY good DCH tracks", 70,-140,140,60,-90,90);
    hxyeffgd->SetOption("COLZ");
    hxyeffgd->Sumw2();
    TH2D *hxyeffgemtof = new TH2D("hxyeffgemtof","Efficiency GEMTOF vs XY good GEM tracks", 70,-140,140,60,-90,90);
    hxyeffgemtof->SetOption("COLZ");
    hxyeffgemtof->Sumw2();
    TH2D *hxyeffgemtofdchtof1 = new TH2D("hxyeffgemtofdchtof1","Efficiency vs XY(gem) GEMTOF+DCHTOF good tracks", 70,-140,140,60,-90,90);
    hxyeffgemtofdchtof1->SetOption("COLZ");
    hxyeffgemtofdchtof1->Sumw2();
    TH2D *hxyeffgemtofdchtofd1 = new TH2D("hxyeffgemtofdchtofd1","Efficiency vs XY(dch) GEMTOF+DCHTOF good tracks", 70,-140,140,60,-90,90);
    hxyeffgemtofdchtofd1->SetOption("COLZ");
    hxyeffgemtofdchtofd1->Sumw2();
    TH2D *hxyeffgemtofdchtof2 = new TH2D("hxyeffgemtofdchtof2","Efficiency vs XY(gem) GEMTOF+DCHTOF good tracks (same TOF hit)", 70,-140,140,60,-90,90);
    hxyeffgemtofdchtof2->SetOption("COLZ");
    hxyeffgemtofdchtof2->Sumw2();
    TH2D *hxyeffgemtofdchtofd2 = new TH2D("hxyeffgemtofdchtofd2","Efficiency vs XY(dch) GEMTOF+DCHTOF good tracks (same TOF hit)", 70,-140,140,60,-90,90);
    hxyeffgemtofdchtofd2->SetOption("COLZ");
    hxyeffgemtofdchtofd2->Sumw2();


// Efficiencies vs P
    TH1D *hpeff = new TH1D("hpeff","Efficiency GEM+DCH+TOF700 vs P good GEM+DCH tracks", 1000,-10,10);
    hpeff->Sumw2();
    TH1D *hpeff2 = new TH1D("hpeff2","Efficiency GEMTOF+DCH+TOF700 vs P good GEMTOF+DCH tracks", 1000,-10,10);
    hpeff2->Sumw2();
    TH1D *hpeff2gt = new TH1D("hpeff2gt","Efficiency GEMTOF+DCH+TOF700 vs P good GEMTOF tracks", 1000,-10,10);
    hpeff2gt->Sumw2();
    TH1D *hpeffmg = new TH1D("hpeffmg","Efficiency GEM+DCH vs P good GEM tracks", 1000,-10,10);
    hpeffmg->Sumw2();
    TH1D *hpeffmg2 = new TH1D("hpeffmg2","Efficiency GEMTOF+DCH vs P good GEMTOF tracks", 1000,-10,10);
    hpeffmg2->Sumw2();
    TH1D *hpeffmg2f = new TH1D("hpeffmg2f","Efficiency GEMTOF+DCH vs P good GEM tracks", 1000,-10,10);
    hpeffmg2f->Sumw2();
    TH1D *hpeffgemtof = new TH1D("hpeffgemtof","Efficiency GEMTOFvs P good GEM tracks", 1000,-10,10);
    hpeffgemtof->Sumw2();

    TH1D *hpeffgemtofdchtof = new TH1D("hpeffgemtofdchtof","Efficiency vs P GEMTOF and DCHTOF good tracks", 1000, -10, 10);
    hpeffgemtofdchtof->SetOption("COLZ");
    hpeffgemtofdchtof->Sumw2();
    TH1D *hpeffgemtofdchtof1 = new TH1D("hpeffgemtofdchtof1","Efficiency vs P P GEMTOF+DCHTOF good tracks", 1000, -10, 10);
    hpeffgemtofdchtof1->SetOption("COLZ");
    hpeffgemtofdchtof1->Sumw2();
    TH1D *hpeffgemtofdchtof2 = new TH1D("hpeffgemtofdchtof2","Efficiency vs P P GEMTOF+DCHTOF good tracks (same TOF hit)", 1000, -10, 10);
    hpeffgemtofdchtof2->SetOption("COLZ");
    hpeffgemtofdchtof2->Sumw2();

// Fragments

    TH2D *hyp0 = new TH2D("hyp0","PT vs YLAB all particles", NYL*3,YLMIN,6,NPT*2,PTMIN,PTMAX);
    hyp0->SetOption("COLZ");
    hyp0->Sumw2();

    TH1D *hmfr0= new TH1D("hmfr0","M2 all particles", 400,-0.5,9.5);
    TH1D *hmfrall= new TH1D("hmfrall","M2 all fragments", 400,-0.5,9.5);
    TH1D *hmfr= new TH1D("hmfr","M2 selected fragments", 400,-0.5,9.5);

    TH3D *hmfr0tt= new TH3D("hmfr0tt","M2 vs trigger vs target all particles", 400,-0.5,9.5,5,1,6,6,0,6);
    TH3D *hmfralltt= new TH3D("hmfralltt","M2 vs trigger vs target all fragments", 400,-0.5,9.5,5,1,6,6,0,6);
    TH3D *hmfrtt= new TH3D("hmfrtt","M2 vs trigger vs target selected fragments", 400,-0.5,9.5,5,1,6,6,0,6);

    TH2D *hypall = new TH2D("hypall","PT vs YLAB all fragments", NYL,YLMIN,YLMAX,NPT,PTMIN,PTMAX);
    hypall->SetOption("COLZ");
    hypall->Sumw2();

    TH2D *hypall1 = new TH2D("hypall1","PT vs YLAB all fragments", 20,YLMIN,YLMAX,28,PTMIN,PTMAX);
    hypall1->SetOption("COLZ");
    hypall1->Sumw2();

    TH2D *hyp_p = new TH2D("hyp_p","PT vs YLAB protons", NYL,YLMIN,YLMAX,NPT,PTMIN,PTMAX);
    hyp_p->SetOption("COLZ");
    hyp_p->Sumw2();

    TH2D *hyp_p1 = new TH2D("hyp_p1","PT vs YLAB protons", 20,YLMIN,YLMAX,28,PTMIN,PTMAX);
    hyp_p1->SetOption("COLZ");
    hyp_p1->Sumw2();

    TH2D *hyp_d = new TH2D("hyp_d","PT vs YLAB deuteron+he4", NYL,YLMIN,YLMAX,NPT,PTMIN,PTMAX);
    hyp_d->SetOption("COLZ");
    hyp_d->Sumw2();

    TH2D *hyp_d1 = new TH2D("hyp_d1","PT vs YLAB deuteron+he4", 20,YLMIN,YLMAX,28,PTMIN,PTMAX);
    hyp_d1->SetOption("COLZ");
    hyp_d1->Sumw2();

    TH2D *hyp_he3 = new TH2D("hyp_he3","PT vs YLAB He3", NYL,YLMIN,YLMAX,NPT,PTMIN,PTMAX);
    hyp_he3->SetOption("COLZ");
    hyp_he3->Sumw2();

    TH2D *hyp_t = new TH2D("hyp_t","PT vs YLAB triton", NYL,YLMIN,YLMAX,NPT,PTMIN,PTMAX);
    hyp_t->SetOption("COLZ");
    hyp_t->Sumw2();

    TH2D *hyp_he31 = new TH2D("hyp_he31","PT vs YLAB He3", 20,YLMIN,YLMAX,28,PTMIN,PTMAX);
    hyp_he31->SetOption("COLZ");
    hyp_he31->Sumw2();

    TH2D *hyp_t1 = new TH2D("hyp_t1","PT vs YLAB triton", 20,YLMIN,YLMAX,28,PTMIN,PTMAX);
    hyp_t1->SetOption("COLZ");
    hyp_t1->Sumw2();

    TH2D *hghitdxvsp_pi = new TH2D("hghitdxvsp_pi","X GEM track distance from DCH1 hit vs momentum (pions)", 50,0,5,100,-5,5);
    hghitdxvsp_pi->SetOption("COLZ");

    TH2D *hghitdxvsp_p = new TH2D("hghitdxvsp_p","X GEM track distance from DCH1 hit vs momentum (protons)", 50,0,5,100,-5,5);
    hghitdxvsp_p->SetOption("COLZ");

    TH2D *hghitdxvsp_d = new TH2D("hghitdxvsp_d","X GEM track distance from DCH1 hit vs momentum (deuterons)", 50,0,5,100,-5,5);
    hghitdxvsp_d->SetOption("COLZ");

    TH2D *hhitdxvsp_pi = new TH2D("hhitdxvsp_pi","X track distance from TOF700 hit vs momentum (pions)", 50,0,5,100,-5,5);
    hhitdxvsp_pi->SetOption("COLZ");

    TH2D *hhitdxvsp_p = new TH2D("hhitdxvsp_p","X track distance from TOF700 hit vs momentum (protons)", 50,0,5,100,-5,5);
    hhitdxvsp_p->SetOption("COLZ");

    TH2D *hhitdxvsp_d = new TH2D("hhitdxvsp_d","X track distance from TOF700 hit vs momentum (deuterons)", 50,0,5,100,-5,5);
    hhitdxvsp_d->SetOption("COLZ");

    TH1F *hplaz =  new TH1F("hplaz","Z hits coordinates TOF700 planes",100,600,650);
    TH1F *hpla =  new TH1F("hpla","X hits on all TOF700 planes",300,-150,150);
    TH1F *hpla1a =  new TH1F("hpla1a","X hits on TOF700 plane 1A",300,-150,150);
    TH1F *hpla1b =  new TH1F("hpla1b","X hits on TOF700 plane 1B",300,-150,150);
    TH1F *hpla2ac =  new TH1F("hpla2ac","X hits on TOF700 plane 2A, central row",300,-150,150);
    TH1F *hpla2au =  new TH1F("hpla2au","X hits on TOF700 plane 2A, upper row",300,-150,150);
    TH1F *hpla2bc =  new TH1F("hpla2bc","X hits on TOF700 plane 2B, central row",300,-150,150);
    TH1F *hpla2bu =  new TH1F("hpla2bu","X hits on TOF700 plane 2B, upper row",300,-150,150);
    TH1F *hpla2abc =  new TH1F("hpla2abc","X hits on TOF700 plane 2A & 2B, central row",300,-150,150);

    TH2F *h2pla1a =  new TH2F("h2pla1a","XY hits on TOF700 plane 1A",300,-150,150,170,-85,+85);
    TH2F *h2pla1b =  new TH2F("h2pla1b","XY hits on TOF700 plane 1B",300,-150,150,170,-85,+85);
    TH2F *h2pla2ac =  new TH2F("h2pla2ac","XY hits on TOF700 plane 2A, central row",300,-150,150,170,-85,+85);
    TH2F *h2pla2au =  new TH2F("h2pla2au","XY hits on TOF700 plane 2A, upper row",300,-150,150,170,-85,+85);
    TH2F *h2pla2bc =  new TH2F("h2pla2bc","XY hits on TOF700 plane 2B, central row",300,-150,150,170,-85,+85);
    TH2F *h2pla2bu =  new TH2F("h2pla2bu","XY hits on TOF700 plane 2B, upper row",300,-150,150,170,-85,+85);
    TH2F *h2pla2abc =  new TH2F("h2pla2abc","XY hits on TOF700 plane 2A & 2B, central row",300,-150,150,170,-85,+85);

    TH1F *hplatrz =  new TH1F("hplatrz","Z matched hits coordinates TOF700 planes",100,600,650);
    TH1F *hplatr =  new TH1F("hplatr","X matched hits on all TOF700 planes",300,-150,150);
    TH1F *hplatr1a =  new TH1F("hplatr1a","X matched hits on TOF700 plane 1A",300,-150,150);
    TH1F *hplatr1b =  new TH1F("hplatr1b","X matched hits on TOF700 plane 1B",300,-150,150);
    TH1F *hplatr2ac =  new TH1F("hplatr2ac","X matched hits on TOF700 plane 2A, central row",300,-150,150);
    TH1F *hplatr2au =  new TH1F("hplatr2au","X matched hits on TOF700 plane 2A, upper row",300,-150,150);
    TH1F *hplatr2bc =  new TH1F("hplatr2bc","X matched hits on TOF700 plane 2B, central row",300,-150,150);
    TH1F *hplatr2bu =  new TH1F("hplatr2bu","X matched hits on TOF700 plane 2B, upper row",300,-150,150);
    TH1F *hplatr2abc =  new TH1F("hplatr2abc","X matched hits on TOF700 plane 2A & 2B, central row",300,-150,150);

    TH1F *hplaeffz =  new TH1F("hplaeffz","Z efficiency (hits) TOF700 planes",100,600,650);
    TH1F *hplaeff =  new TH1F("hplaeff","X efficiency (hits) on all TOF700 planes",300,-150,150);
    TH1F *hplaeff1a =  new TH1F("hplaeff1a","X efficiency (hits) on TOF700 plane 1A",300,-150,150);
    TH1F *hplaeff1b =  new TH1F("hplaeff1b","X efficiency (hits) on TOF700 plane 1B",300,-150,150);
    TH1F *hplaeff2ac =  new TH1F("hplaeff2ac","X efficiency (hits) on TOF700 plane 2A, central row",300,-150,150);
    TH1F *hplaeff2au =  new TH1F("hplaeff2au","X efficiency (hits) on TOF700 plane 2A, upper row",300,-150,150);
    TH1F *hplaeff2bc =  new TH1F("hplaeff2bc","X efficiency (hits) on TOF700 plane 2B, central row",300,-150,150);
    TH1F *hplaeff2bu =  new TH1F("hplaeff2bu","X efficiency (hits) on TOF700 plane 2B, upper row",300,-150,150);
    TH1F *hplaeff2abc =  new TH1F("hplaeff2abc","X efficiency (hits) on TOF700 plane 2A & 2B, central row",300,-150,150);

    TH1F *hplatraz =  new TH1F("hplatraz","Z tracks coordinates TOF700 planes",100,600,650);
    TH1F *hplatra =  new TH1F("hplatra","X tracks on all TOF700 planes",300,-150,150);
    TH1F *hplatra1a =  new TH1F("hplatra1a","X tracks on TOF700 plane 1A",300,-150,150);
    TH1F *hplatra1b =  new TH1F("hplatra1b","X tracks on TOF700 plane 1B",300,-150,150);
    TH1F *hplatra2ac =  new TH1F("hplatra2ac","X tracks on TOF700 plane 2A, central row",300,-150,150);
    TH1F *hplatra2au =  new TH1F("hplatra2au","X tracks on TOF700 plane 2A, upper row",300,-150,150);
    TH1F *hplatra2bc =  new TH1F("hplatra2bc","X tracks on TOF700 plane 2B, central row",300,-150,150);
    TH1F *hplatra2bu =  new TH1F("hplatra2bu","X tracks on TOF700 plane 2B, upper row",300,-150,150);
    TH1F *hplatra2abc =  new TH1F("hplatra2abc","X tracks on TOF700 plane 2A & 2B, central row",300,-150,150);

    TH1F *hplatramz =  new TH1F("hplatramz","Z matched tracks coordinates TOF700 planes",100,600,650);
    TH1F *hplatram =  new TH1F("hplatram","X matched tracks on all TOF700 planes",300,-150,150);
    TH1F *hplatram1a =  new TH1F("hplatram1a","X matched tracks on TOF700 plane 1A",300,-150,150);
    TH1F *hplatram1b =  new TH1F("hplatram1b","X matched tracks on TOF700 plane 1B",300,-150,150);
    TH1F *hplatram2ac =  new TH1F("hplatram2ac","X matched tracks on TOF700 plane 2A, central row",300,-150,150);
    TH1F *hplatram2au =  new TH1F("hplatram2au","X matched tracks on TOF700 plane 2A, upper row",300,-150,150);
    TH1F *hplatram2bc =  new TH1F("hplatram2bc","X matched tracks on TOF700 plane 2B, central row",300,-150,150);
    TH1F *hplatram2bu =  new TH1F("hplatram2bu","X matched tracks on TOF700 plane 2B, upper row",300,-150,150);
    TH1F *hplatram2abc =  new TH1F("hplatram2abc","X matched tracks on TOF700 plane 2A & 2B, central row",300,-150,150);

    TH1F *hplaeffaz =  new TH1F("hplaeffaz","Z efficiency (tracks) TOF700 planes",100,600,650);
    TH1F *hplaeffa =  new TH1F("hplaeffa","X efficiency (tracks) on all TOF700 planes",300,-150,150);
    TH1F *hplaeffa1a =  new TH1F("hplaeffa1a","X efficiency (tracks) on TOF700 plane 1A",300,-150,150);
    TH1F *hplaeffa1b =  new TH1F("hplaeffa1b","X efficiency (tracks) on TOF700 plane 1B",300,-150,150);
    TH1F *hplaeffa2ac =  new TH1F("hplaeffa2ac","X efficiency (tracks) on TOF700 plane 2A, central row",300,-150,150);
    TH1F *hplaeffa2au =  new TH1F("hplaeffa2au","X efficiency (tracks) on TOF700 plane 2A, upper row",300,-150,150);
    TH1F *hplaeffa2bc =  new TH1F("hplaeffa2bc","X efficiency (tracks) on TOF700 plane 2B, central row",300,-150,150);
    TH1F *hplaeffa2bu =  new TH1F("hplaeffa2bu","X efficiency (tracks) on TOF700 plane 2B, upper row",300,-150,150);
    TH1F *hplaeffa2abc =  new TH1F("hplaeffa2abc","X efficiency (tracks) on TOF700 plane 2A & 2B, central row",300,-150,150);

// Fragments tree

    TTree *tfr = new TTree("tfr","Tree with fragments");
    Int_t nurun = 0, nuev = 0, tri = 0, tar = 0;
    Double_t pfr = 0., yfr = 0., ptfr = 0., efr = 0., pxfr = 0., pyfr = 0., pzfr = 0., bfr = 0., m2fr = 0.;
    Double_t x700fr = 0., y700fr = 0.;
    tfr->Branch("x700fr",&x700fr,"x700fr/D");
    tfr->Branch("y700fr",&y700fr,"y700fr/D");
    tfr->Branch("m2fr",&m2fr,"m2fr/D");
    tfr->Branch("pxfr",&pxfr,"px/D");
    tfr->Branch("pyfr",&pyfr,"py/D");
    tfr->Branch("pzfr",&pzfr,"pz/D");
    tfr->Branch("pfr",&pfr,"pfr/D");
    tfr->Branch("yfr",&yfr,"yfr/D");
    tfr->Branch("ptfr",&ptfr,"ptfr/D");
    tfr->Branch("bfr",&bfr,"bfr/D");
    tfr->Branch("efr",&efr,"efr/D");
    tfr->Branch("tri",&tri,"tri/i");
    tfr->Branch("tar",&tar,"tar/i");
    tfr->Branch("nurun",&nurun,"nurun/i");
    tfr->Branch("nuev",&nuev,"nuev/i");
//
    TH1D *hdtx = new TH1D("hdtx","dtx",100,-0.25,+0.25);
    TH1D *hdty = new TH1D("hdty","dty",100,-0.25,+0.25);
    TH1D *hDdx = new TH1D("hDdx","Err dx",100,0,10);
    TH1D *hDdy = new TH1D("hDdy","Err dy",100,0,10);
    TH1D *hDdtx = new TH1D("hDdtx","Err dtx",100,0,0.05);
    TH1D *hDdty = new TH1D("hDdty","Err dty",100,0,0.05);
    TH1D *hpdx = new TH1D("hpdx","Pull dx",500,-2.5,+2.5);
    TH1D *hpdy = new TH1D("hpdy","Pull dy",500,-2.5,+2.5);
    TH1D *hpdtx = new TH1D("hpdtx","Pull dtx",500,-25.,+25.);
    TH1D *hpdty = new TH1D("hpdty","Pull dty",500,-25.,+25.);
    TH1D *hchi2m = new TH1D("hChi2m","Chi2 GemDch track",1000,0,100);
    TH1D *hchi2 = new TH1D("hchi2","Chi2 tracks matching",1000,0,100);

    TProfile2D *tcalibrs = new TProfile2D("tcalibr","Time calibration based on proton mass, strips",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,32,0,32,-1.,+1.,"e");
    TProfile *tcalibrc = new TProfile("tcalibrc","Time calibration based on proton mass, chambers",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,-1.,+1.,"e");

    TProfile *tpeak = new TProfile("tpeak","Peak time vs chamber",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,0.,+30.,"e");
    TProfile *bpeak = new TProfile("bpeak","Peak velosity vs chamber",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,0.,+1.5,"e");

    char nam[64],tit[64];

    float cvel = 29.97925f;
    float ekin = 3.2f, ebeam = ekin + MP, pbeam = TMath::Sqrt(ebeam*ebeam+MP*MP);
    float cbeam = pbeam/ebeam;
    float cpeak = cbeam*CFACTOR*cvel;
    float ztarg = 0.f; //-2.2f;
    float zca[NUMBER_CHAMBERS] = {0.f};
    for (int i = 0; i<NUMBER_CHAMBERS; i++)
    {
	TOF2->get_chamber_z(i,&zca[i]);
    }

    Int_t startEvent = 0;
    Int_t nEvents = bmnTree->GetEntries();
    Int_t nEventstof700 = bmnTreetof700->GetEntries();
    printf("\nNumber of events in tracks chain is %d (tof700 chain %d)\n", nEvents, nEventstof700);

    int Ntr = 0, Ntr1 = 0, Ntr1g = 0, Ntr2g = 0, Ntr0 = 0, Ntr02 = 0;
    Int_t iEv = 0, iEvtof700 = 0, fCurrent = -1, nevs = 0;
    UInt_t trackEvId = 0, tof700EvId = 0;
    UInt_t trackRunId = 0, tof700RunId = 0;
    TH1D *projtmp = 0, *projtmp1 = 0;
    for (Int_t iEv0 = startEvent; iEv0 < startEvent + nEvents; iEv0++)
    {
        if (iEv % 10000 == 0)
	{
	    cout << "Event: " << iEv << "/" << startEvent + nEvents << " ( Run " << trackRunId << " EvId " << trackEvId << " )" << endl;
	}
    	BmnGemDchTrack *gemdchtrack0 = NULL;
	do
	{
	    if (iEv >= nEvents) break;
	    bmnTree->GetEntry(iEv);
	    if (mtracks == NULL) { iEv++; continue; };
    	    gemdchtrack0 = (BmnGemDchTrack*)(mtracks->UncheckedAt(0));
//    	    gemdchtrack0 = (BmnGemDchTrack*)(mtracks->At(0));
//	    printf("First attempt, iEv = %d, iEv0 = %d\n",iEv,iEv0);
	    iEv++;
	}
	while (gemdchtrack0 == NULL);
//	printf("Point 0, iEv = %d\n",iEv);
	if (iEv >= nEvents) break;
//	trackEvId = gemdchtrack0->GetEventId();
	trackEvId = ((BmnEventHeader*)eventHeader)->GetEventId();
//	printf("Point 1, iEv = %d, trackEvId = %d\n",iEv,trackEvId);
	if (eventHeader == NULL) continue;
//	trackRunId = ((BmnEventHeader*)eventHeader->UncheckedAt(0))->GetRunId();
	trackRunId = ((BmnEventHeader*)eventHeader)->GetRunId();
//	printf("Point 2, iEv = %d, trackRunId = %d\n",iEv,trackRunId);
	if (tof700RunId == trackRunId && tof700EvId > trackEvId) continue;
	if (tof700RunId > trackRunId) continue;
//	printf("Point 3, iEv = %d\n",iEv);
	if (!(tof700EvId==trackEvId && tof700RunId==trackRunId))
	{
          do
	  {
	    bmnTreetof700->GetEntry(iEvtof700);
	    if (eventHeadertof700 == NULL) break;
	    tof700EvId = eventHeadertof700->GetEventId();
//	    tof700RunId = ((BmnEventHeader*) eventHeadertof700->UncheckedAt(0))->GetRunId();
	    tof700RunId = eventHeadertof700->GetRunId();
//	    tof700EvId = ((BmnEventHeader*) eventHeadertof700->UncheckedAt(0))->GetEventId();
//	    tof700RunId = ((BmnEventHeader*) eventHeadertof700->UncheckedAt(0))->GetRunId();
//	    cout << "Tracks Event:   " << iEv << ", TOF700 Event: " << iEvtof700 << endl;
//	    cout << "Tracks EventId: " << trackEvId << ", TOF700 EventId: " << tof700EvId << endl;
//	    cout << "Tracks RunId: " << trackRunId << ", TOF700 RunId: " << tof700RunId << endl;
	    iEvtof700++;
	    if (iEvtof700 >= nEventstof700) break;
	    if (tof700RunId > trackRunId) break;
	    if (tof700RunId == trackRunId && tof700EvId > trackEvId) break;
	  }
	  while (!(tof700EvId==trackEvId && tof700RunId==trackRunId));
	}
	if (eventHeadertof700 == NULL) continue;
        if (!(tof700EvId==trackEvId && tof700RunId==trackRunId))
	{
	    cout << "Not found EventId " << trackEvId << " in tof700 digits file" << endl;
	    cout << "Tracks Event:   " << iEv << ", TOF700 Event: " << iEvtof700 << endl;
	    cout << "Tracks RunId:   " << trackRunId << ", TOF700 RunId:   " << tof700RunId << endl;
	    cout << "Tracks EventId: " << trackEvId <<  ", TOF700 EventId: " << tof700EvId << endl;
	    if (iEvtof700 >= nEventstof700) break;
	    if (tof700RunId > trackRunId) continue;
	    if (tof700RunId == trackRunId && tof700EvId > trackEvId) continue;
	    break;
//	    continue;
	}

	// Skip zero field data
	if (fSp41[trackRunId] <= 0) continue;

	tar = Ntar[trackRunId];
	htarget->Fill(tar);
//	if (true) continue;

	float wmax[NUMBER_CHAMBERS] = {0.};
	float tmax[NUMBER_CHAMBERS] = {-10000000.};
	int smax[NUMBER_CHAMBERS] = {-1};
	int smax1[NUMBER_CHAMBERS] = {-1};
	int istr[NUMBER_CHAMBERS] = {-1};

	for (int i = 0; i<NUMBER_CHAMBERS; i++)
	{
	    wmax[i] = 0.;
	    tmax[i] = -1000000.;
	    smax[i] = -1;
	    smax1[i] = -1;
	    nhit[i] = 0;
	    for (int j=0; j<32; j++) nhits[i][j] = 0;
	    for (int j=0; j<32; j++) tofWidths[i][j] = 0.;
	    for (int j=0; j<32; j++) tof[i][j] = 0.;
	}
	nhi = 0;

	int ntof = tof700Digits->GetEntriesFast();
	hntof->Fill(ntof);
	float x = 0., y = 0., z = 0.;
	int hchamb[1000] = {0}, hstrip[1000] = {0};
	int clchamb[1000] = {0}, clstrip[1000] = {0};
	Float_t clwid[1000] = {0};
	Bool_t usedstrip[1000] = {false};
	Bool_t usedstrip1[1000] = {false};
	Bool_t usedstrip2[1000] = {false};
	Bool_t usedtrack[1000] = {false};
	Bool_t usedtrack1[1000] = {false};
	Bool_t usedtrack2[1000] = {false};
	Int_t numhit[NUMBER_CHAMBERS][32] = {{0}};
	for (Int_t iDig = 0; iDig < ntof; ++iDig) {
    	    BmnTof2Digit *digit = (BmnTof2Digit*) tof700Digits->At(iDig);
    	    if (digit == NULL) continue;
    	    Short_t plane = digit->GetPlane();
    	    Short_t strip = digit->GetStrip();
    	    Float_t time = digit->GetTime();
//	    if (plane == 8) time -= 40.69f; // Chamber 5.1
    	    Float_t width = digit->GetAmplitude();
	    Float_t lrdiff = digit->GetDiff();
//    	    if (iEv % 10 == 0) cout << "plane = " << plane << " strip = " << strip << " time = " << time << " width = " << width << endl;
//    	    if (iEv % 10 == 0) cout << "wmi = " << wmi[plane] << " wma = " << wma[plane] << endl;
	    if (plane >= NUMBER_CHAMBERS) continue;
	    if (XYPANIN && (TOF2->get_hit_xp(plane, strip, lrdiff) == 0.)) continue;
//	    if (lrdiff < -0.5 || lrdiff > 0.5) continue;
	    if (IN_WIDTH_LIMITS) if (width < wmi[plane] || width > wma[plane]) continue;

	    if (XYPANIN)
		TOF2->get_hit_xyzp(plane, strip, lrdiff, &x, &y, &z);
	    else if (XYPETNEW)
	    {
		TOF2->get_hit_xyzng(plane, strip, lrdiff, &x, &y, &z);
		if (z == 0.) TOF2->get_hit_xyz(plane, strip, lrdiff, &x, &y, &z);
		if (z == 0.) continue;
	    }
	    else
		TOF2->get_hit_xyz(plane, strip, lrdiff, &x, &y, &z);

	    htof->Fill(time);
	    hchamb[nhi] = plane;
	    hstrip[nhi] = strip;
	    smax1[plane] = strip;
	    if (width > wmax[plane])
	    {
		wmax[plane] = width;
		tmax[plane] = time;
		smax[plane] = strip;
	    }
	    nhit[plane]++;
	    nhits[plane][strip]++;
	    tof[plane][strip] = time;
            tofWidths[plane][strip] = width;
	    istr[plane] = strip;

	    xhi[nhi] = x;
	    yhi[nhi] = y;
	    zhi[nhi] = z;
	    lrd[nhi] = lrdiff;
	    hxytof->Fill(x,y);
/*	    
	    hplaz->Fill(z);
	    hpla->Fill(x);
	    if (z < 600) hpla1a->Fill(x);
	    if (z > 605 && z < 610) hpla1b->Fill(x);
	    if (z > 625 && z < 630 && y < 20) hpla2ac->Fill(x);
	    if (z > 625 && z < 630 && y > 20) hpla2au->Fill(x);
	    if (z > 635 && z < 660 && y < 20) hpla2bc->Fill(x);
	    if (z > 635 && z < 660 && y > 20) hpla2bu->Fill(x);
	    if (z > 625 && z < 660 && y < 20) hpla2abc->Fill(x);
*/	    
	    TOF2->get_strip_xyz(plane, strip, &x, &y, &z);
	    xhic[nhi] = x;
	    yhic[nhi] = y;
	    zhic[nhi] = z;
	    hstr->Fill(strip);
	    hcha->Fill(plane);
	    numhit[plane][strip] = nhi;
	    nhi++;
	} // tof700Digits loop
	hnhit->Fill(nhi);
// Clusterization
	int ncl0 = 0, ncl[NUMBER_CHAMBERS] = {0}, ncls = 0, nclb = 0;
	int ncln = 0, nclsn = 0;
	Float_t tcl[NUMBER_CHAMBERS][32] = {{0}};
	Int_t acl[NUMBER_CHAMBERS][32] = {{0}};
	Int_t wcl[NUMBER_CHAMBERS][32] = {{0}};
	if (USE_CLUSTERS)
	{
		    for (int i = 0; i<NUMBER_CHAMBERS ; i++)
		    {
			    ncl[i] = 0;
//			    if (nhit[i] == 0) continue;
			    int clstart = -1, clwidth = -1, cstr = -1;
			    float samps = 0., timemin = 1000000000.;
			    float widthmax = 0.;
			    int nstr = 32;
			    if (idchambers[i] > 100.f) nstr = 16;
			    for (int j = 0; j<nstr; j++)
			    {
//				if (nhits[i][j] == 0) continue;
				if (tofWidths[i][j] > 0.)
				{
//				    if (idchambers[i] != 19.3) xystrip(hxyhits, i, j);
				    if (clstart < 0)
				    {
					clstart = j;
					cstr = j;
					timemin = tof[i][j];
					widthmax = tofWidths[i][j];
				    }
				    else
				    {
					if (MINTIME)
					{
					    if (tof[i][j] < timemin) {timemin = tof[i][j]; cstr = j; }
					}
					else
					{
					    if (tofWidths[i][j] > widthmax) {widthmax = tofWidths[i][j]; cstr = j; }
					}
				    }
				    samps += tofWidths[i][j];
				    if (idchambers[i] < 100.f) hclamps->Fill(idchambers[i]!=19.3?(Int_t)idchambers[i]:49, tofWidths[i][j]);
				    else                       hclampb->Fill((Int_t)idchambers[i]-100, tofWidths[i][j]);
				    if (j == (nstr-1))
				    {
					clchamb[ncl0] = i;
					clstrip[ncl0] = cstr;
					clwid[ncl0] = samps;
					clwidth = (j+1-clstart);
					tcl[i][ncl[i]] = timemin;
					acl[i][ncl[i]] = samps;
					wcl[i][ncl[i]] = clwidth;
					ncl0++;
					ncl[i]++;
					samps = 0.;
					timemin =  1000000000.;
					widthmax = 0.;
					int ih = numhit[i][cstr];
					if (idchambers[i] < 100.f) hclnums->Fill(clwidth);
					else                       hclnumb->Fill(clwidth);
					if (idchambers[i] < 100.f) hclwids->Fill(idchambers[i]!=19.3?(Int_t)idchambers[i]:49, samps);
					else                       hclwidb->Fill((Int_t)idchambers[i]-100, samps);
					hxycluster->Fill(xhi[ih],yhi[ih]);
					if (idchambers[i] == 19.3) continue;
					xystrip(hxystrips, i, cstr);
					hplaz->Fill(zhi[numhit[i][cstr]]);
					hpla->Fill(xhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] < 600) hpla1a->Fill(xhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 605 && zhi[numhit[i][cstr]] < 610) hpla1b->Fill(xhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 630 && yhi[numhit[i][cstr]] < 20) hpla2ac->Fill(xhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 630 && yhi[numhit[i][cstr]] > 20) hpla2au->Fill(xhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 635 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] < 20) hpla2bc->Fill(xhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 635 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] > 20) hpla2bu->Fill(xhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] < 20) hpla2abc->Fill(xhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] < 600) h2pla1a->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 605 && zhi[numhit[i][cstr]] < 610) h2pla1b->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 630 && yhi[numhit[i][cstr]] < 20) h2pla2ac->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 630 && yhi[numhit[i][cstr]] > 20) h2pla2au->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 635 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] < 20) h2pla2bc->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 635 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] > 20) h2pla2bu->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
					if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] < 20) h2pla2abc->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
				    }
				}
				else if (clstart >= 0)
				{
				    clchamb[ncl0] = i;
				    clstrip[ncl0] = cstr;
				    clwid[ncl0] = samps;
				    clwidth = (j-clstart);
				    tcl[i][ncl[i]] = timemin;
				    acl[i][ncl[i]] = samps;
				    wcl[i][ncl[i]] = clwidth;
				    ncl0++;
				    ncl[i]++;
				    samps = 0.;
				    timemin =  1000000000.;
				    widthmax = 0.;
				    clstart = -1;
				    int ih = numhit[i][cstr];
				    if (idchambers[i] < 100.f) hclnums->Fill(clwidth);
				    else                       hclnumb->Fill(clwidth);
				    if (idchambers[i] < 100.f) hclwids->Fill(idchambers[i]!=19.3?(Int_t)idchambers[i]:49, samps);
				    else                       hclwidb->Fill((Int_t)idchambers[i]-100, samps);
				    hxycluster->Fill(xhi[ih],yhi[ih]);
				    if (idchambers[i] == 19.3) continue;
				    xystrip(hxystrips, i, cstr);
				    hplaz->Fill(zhi[numhit[i][cstr]]);
				    hpla->Fill(xhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] < 600) hpla1a->Fill(xhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 605 && zhi[numhit[i][cstr]] < 610) hpla1b->Fill(xhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 630 && yhi[numhit[i][cstr]] < 20) hpla2ac->Fill(xhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 630 && yhi[numhit[i][cstr]] > 20) hpla2au->Fill(xhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 635 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] < 20) hpla2bc->Fill(xhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 635 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] > 20) hpla2bu->Fill(xhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] < 20) hpla2abc->Fill(xhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] < 600) h2pla1a->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 605 && zhi[numhit[i][cstr]] < 610) h2pla1b->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 630 && yhi[numhit[i][cstr]] < 20) h2pla2ac->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 630 && yhi[numhit[i][cstr]] > 20) h2pla2au->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 635 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] < 20) h2pla2bc->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 635 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] > 20) h2pla2bu->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
				    if (zhi[numhit[i][cstr]] > 625 && zhi[numhit[i][cstr]] < 660 && yhi[numhit[i][cstr]] < 20) h2pla2abc->Fill(xhi[numhit[i][cstr]],yhi[numhit[i][cstr]]);
				}
			    }
			    if (idchambers[i] < 100.f) hnclvschambs->Fill(idchambers[i]!=19.3?(Int_t)idchambers[i]:49, ncl[i]);
			    else                       hnclvschambb->Fill((Int_t)idchambers[i]-100, ncl[i]);
			    if (idchambers[i] < 100.f) hnclvschambs1->Fill(idchambers[i]!=19.3?(Int_t)idchambers[i]:49, ncl[i]);
			    else                       hnclvschambb1->Fill((Int_t)idchambers[i]-100, ncl[i]);

			    if (idchambers[i] < 100.f) ncls+=ncl[i];
			    else                       nclb+=ncl[i];
			    if ((int)idchambers[i] != 19) ncln+=ncl[i];
			    if (idchambers[i] < 100.f && (int)idchambers[i] != 19) nclsn+=ncl[i];

		    }
		    hncl->Fill(ncl0);
		    hncls->Fill(ncls);
		    hnclb->Fill(nclb);
		    hncln->Fill(ncln);
		    hnclsn->Fill(nclsn);
	}
//
	float xyz[3] = {0.}, cxyz[3] = {0.}, xrpc = 0., yrpc = 0., dz = 100., tx = 0., ty = 0., zcha = 0., tx0 = 0.;
	float xrpcm = 0., yrpcm = 0., zrpcm = 0.;
	float xrpcm1 = 0., yrpcm1 = 0., zrpcm1 = 0.;
	Int_t crpc1  = -1, crpc2 = -1;
	float txdch = 0., tydch = 0., xyzdch[3] = {0.}, cxyzdch[3] = {0.};
	float beta = 1.0f, mass = 1.0f, mass2 = 1.0f, tcor = 0., tcorc = 0., tcors = 0., tcorspi = 0., tcorcpi = 0.;
	int nh = 0;
	int ntr = mtracks->GetEntriesFast();
	hntr->Fill(ntr);
	Ntr+=ntr;
	int ntr0 = 0, ntr02 = 0;
	int ntr1 = 0, ntr1g = 0, ntr2g = 0;
	int iflag = 0, igem = 0, idch = 0;
	float chi2 = 0.f;
    	BmnGemDchTrack *gemdchtrack = NULL;
    	BmnTrack *track = NULL;
#if GEMTYPE > 2
    	BmnTrack *gtrack = NULL; // Merts GEM tracks
#else
    	CbmStsTrack *gtrack = NULL; // Gleb GEM tracks
#endif
	Bool_t gooddch = false, goodgem = false, goodgemdch = false, gooddchtof = false, goodgemdchtof = false, goodgemtof = false;
	Bool_t goodgemdch2 = false, goodgemdchtof2 = false;
        Double_t xGem = 0.;
        Double_t xGem0 = 0.;
        Double_t yGem = 0.;
        Double_t zGem = 0.;
        Double_t txGem = 0.;
        Double_t tyGem = 0.;
        Double_t xGem1st = 0.;
        Double_t yGem1st = 0.;
        Double_t zGem1st = 0.;
        Double_t txGem1st = 0.;
        Double_t tyGem1st = 0.;
        Double_t QpGem = 0.;
        Double_t QpDch = 0.;
        Int_t nGemHits = 0;
        Int_t nSiHits = 0;
	Double_t Xpv = 0.;
	Double_t Ypv = 0.;
	Double_t dxm = 0.;
	Double_t dym = 0.;
	Double_t dtx = 0.;
	Double_t dty = 0.;
	Double_t Ddx = 0.;
	Double_t Ddy = 0.;
	Double_t Ddtx = 0.;
	Double_t Ddty = 0.;
	Double_t Chi2m = 0.;
        Double_t xgoodtr = 0.;
        Double_t ygoodtr = 0.;
        Double_t xgoodtr1 = 0.;
        Double_t ygoodtr1 = 0.;
	Double_t le = 0., p = 0., ps = 0., pmod = 0., lenr = 0.;
	int himin = -1, himin1 = -1, himin2 = -1;

// TOF700 matching variables

	Int_t Npairs = 0, Npairs1 = 0, Npairs2 = 0;
	Int_t Ihit[MAX_NPAIRS] = {-1}, Ihit1[MAX_NPAIRS] = {-1}, Ihit2[MAX_NPAIRS] = {-1};
//	Int_t Igem[MAX_NPAIRS] = {-1}, Igem1[MAX_NPAIRS] = {-1}, Igem2[MAX_NPAIRS] = {-1};
//	Int_t Idch[MAX_NPAIRS] = {-1}, Idch1[MAX_NPAIRS] = {-1}, Idch2[MAX_NPAIRS] = {-1};
	Int_t Isort[MAX_NPAIRS] = {0}, Isort1[MAX_NPAIRS] = {0}, Isort2[MAX_NPAIRS] = {0};
	Int_t Itrack[MAX_NPAIRS] = {0}, Itrack1[MAX_NPAIRS] = {0}, Itrack2[MAX_NPAIRS] = {0};
	Double_t Dpairs[MAX_NPAIRS] = {0}, Dpairs1[MAX_NPAIRS] = {0}, Dpairs2[MAX_NPAIRS] = {0};
	Double_t Cpairs[MAX_NPAIRS] = {0}, Cpairs1[MAX_NPAIRS] = {0}, Cpairs2[MAX_NPAIRS] = {0};
	Double_t Vpairs[MAX_NPAIRS][25] = {{0}}, Vpairs1[MAX_NPAIRS][25] = {{0}}, Vpairs2[MAX_NPAIRS][25] = {{0}};

//

	double dx, dy, dl;
	double dist = 0., distmin = 1000000000.0f, distmin1 = 1000000000.0f, distmin2 = 1000000000.0f;
	Float_t xc,yc,zc;
	int ic;
	int is;
	double cosa = 0.;
	float hidist = 0.;
	Double_t tofr0 = 0.f;
	Double_t tofr = 0.f;
	Double_t tofp = 0.f;
	Double_t tofpi = 0.f;
        float massmin1 = 0.;
        float massmax1 = 0.;
        float massmin2 = 0.;
        float massmax2 = 0.;

        float massmaxpi = 0.;
        float massminpi = 0.;
	float tcorpi = 0.;
        float tofrc = 0.;
        float tofrs = 0.;
        float betac = 0.;
        float betas = 0.;
	float masss2 = 0.;
	float masss  = 0.;
	float massc2 = 0.;
	float massc  = 0.;
	double timepeak = 0., betapeak = 0.;
	Bool_t first_track = true;
	Int_t ncltr[NUMBER_CHAMBERS] = {0};
	Int_t ntrgem = 0, ntrdch0 = 0, ntrdch = 0, ntrgem0 = 0, ntrgemcut = 0;
	Double_t xtrgem[100],xtrgem0[100],ytrgem[100],xtrdch[100],ytrdch[100],dxtrgem[100],dytrgem[100],ztrgem[100],ztrgem1st[100];
	Double_t txtrgem[100],tytrgem[100],txtrdch[100],tytrdch[100];
	Double_t pgem[100];
	for (Int_t iTr = 0; iTr < ntr; ++iTr) {
    	    gemdchtrack = (BmnGemDchTrack*) mtracks->At(iTr);
    	    if (gemdchtrack == NULL) continue;
    	    track = (BmnTrack*) gemdchtrack->GetDchTrack();
	    gooddch = false;
    	    if (track != NULL && (track->GetUniqueID() == 100 || track->GetUniqueID() == 200 || track->GetUniqueID() == 400))
	    {
    		  if ((FLAG < 0 && track->GetFlag() <  0) || (FLAG > 0 && track->GetFlag() >= 0) || (FLAG == 0))
    		  {
        	    QpDch = track->GetParamLast()->GetQp();
		    if (!TMath::IsNaN(QpDch) && (track->GetParamLast()->GetZ() > 500.f) && (track->GetNHits() >= MINDCHHITS))
		    {
			// get xyz and cxyz here
    			xyz[0] = track->GetParamLast()->GetX() + (DX0DCH) - (DXDCH);
    			xyz[1] = track->GetParamLast()->GetY() + (DY0DCH) - (DYDCH);
    			xyz[2] = track->GetParamLast()->GetZ() + (DZ0DCH) - (DZDCH);
			tx = track->GetParamLast()->GetTx() + (DTX0DCH) - (DTXDCH);
			tx0 = tx;
			ty = track->GetParamLast()->GetTy() + (DTY0DCH) - (DTYDCH);
			cxyz[0] = tx/TMath::Sqrt(1.+tx*tx+ty*ty);
			cxyz[1] = ty/TMath::Sqrt(1.+tx*tx+ty*ty);
			cxyz[2] = 1./TMath::Sqrt(1.+tx*tx+ty*ty);

    			xyzdch[0] = track->GetParamFirst()->GetX() + (DX0DCH) - (DXDCH);
    			xyzdch[1] = track->GetParamFirst()->GetY() + (DY0DCH) - (DYDCH);
    			xyzdch[2] = track->GetParamFirst()->GetZ() + (DZ0DCH) - (DZDCH);
			txdch = track->GetParamFirst()->GetTx() + (DTX0DCH) - (DTXDCH);
			tydch = track->GetParamFirst()->GetTy() + (DTY0DCH) - (DTYDCH);
			cxyzdch[0] = txdch/TMath::Sqrt(1.+txdch*txdch+tydch*tydch);
			cxyzdch[1] = tydch/TMath::Sqrt(1.+txdch*txdch+tydch*tydch);
			cxyzdch[2] = 1./TMath::Sqrt(1.+txdch*txdch+tydch*tydch);

			ntrdch0++;
#if PUREDCH != 0
			xyz[0] = xyzdch[0];
			xyz[1] = xyzdch[1];
			xyz[2] = xyzdch[2];
			tx = txdch;
			ty = tydch;
			cxyz[0] = cxyzdch[0];
			cxyz[1] = cxyzdch[1];
			cxyz[2] = cxyzdch[2];
#endif
			xgoodtr = xyz[0];
			ygoodtr = xyz[1];
			xgoodtr1 = xyzdch[0];
			ygoodtr1 = xyzdch[1];
			if (ACCEPTANCE)
			{
			    Double_t xdch = xgoodtr1+txdch*(z_tof700_average-xyzdch[2]);
			    Double_t ydch = ygoodtr1+tydch*(z_tof700_average-xyzdch[2]);
			    if (xdch >= xmintof700 && xdch <= xmaxtof700 && ydch >= ymintof700 && ydch <= ymaxtof700)
			    {
				hxydch->Fill(xgoodtr,ygoodtr);
				hxydch1->Fill(xgoodtr1,ygoodtr1);
				gooddch = true;
			    }
			}
			else
			{
			    hxydch->Fill(xgoodtr,ygoodtr);
			    hxydch1->Fill(xgoodtr1,ygoodtr1);
			    gooddch = true;
			}
		    }
		  }
	    }
	    if (gooddch)
	    {
		xtrdch[ntrdch] = xyzdch[0];
		ytrdch[ntrdch] = xyzdch[1];
		txtrdch[ntrdch] = txdch;
		tytrdch[ntrdch] = tydch;
		ntrdch++;
	    }
#if GEMTYPE > 2
    	    gtrack = (BmnTrack *) gemdchtrack->GetGemTrack();  // Merts GEM tracks
#else
    	    gtrack = (CbmStsTrack *) gemdchtrack->GetGemTrack(); // Gleb GEM tracks
#endif
	    goodgem = false;
    	    if (gtrack != NULL && (gtrack->GetUniqueID() == 100 || gtrack->GetUniqueID() == 300 || gtrack->GetUniqueID() == 500))
    	    {
// Selection criteria
        	QpGem = gtrack->GetParamLast()->GetQp();
		if (!TMath::IsNaN(QpGem))
		{
		    if (QpGem != 0. && (gtrack->GetParamLast()->GetZ() > 500.f))
		    {
			hp0->Fill(1./QpGem);
		    }
		    else
		    {
//			hp0->Fill(0.);
		    }
//		    ntr1++;
		    ntrgem0++;
        	    xGem = gtrack->GetParamLast()->GetX() + (DX0DCH) - (DXDCH);
        	    yGem = gtrack->GetParamLast()->GetY() + (DY0DCH) - (DYDCH);
        	    zGem = gtrack->GetParamLast()->GetZ() + (DZ0DCH) - (DZDCH);
        	    txGem = gtrack->GetParamLast()->GetTx() + (DTX0DCH) - (DTXDCH);
        	    tyGem = gtrack->GetParamLast()->GetTy() + (DTY0DCH) - (DTYDCH);
		    xGem0 = xGem;
        	    xGem1st = gtrack->GetParamFirst()->GetX();
        	    yGem1st = gtrack->GetParamFirst()->GetY();
        	    zGem1st = gtrack->GetParamFirst()->GetZ();
        	    txGem1st = gtrack->GetParamFirst()->GetTx();
        	    tyGem1st = gtrack->GetParamFirst()->GetTy();
#if GEMTYPE > 2
        	    nGemHits = gtrack->GetNHits(); // Merts GEM tracks
#else
//        	    nGemHits = gtrack->GetNDF(); // Gleb GEM tracks
		    nGemHits = gemdchtrack->GetGemHits(); // Gleb GEM tracks
            	    nSiHits = gemdchtrack->GetSiHits(); // Gleb GEM tracks
#endif
		    Xpv = gemdchtrack->GetXpv();
		    Ypv = gemdchtrack->GetYpv();

        	    if (GEMCUTS)
        	    {
		     Bool_t goodpv = true;
		     if (PRIMARY_VERTEX_CUTS)
		     {
//		        if(ntr < 2) goodpv = false;
		        if(primaryVertex->GetNTracks() < 2) goodpv = false;
    		        if(primaryVertex->GetX() < -6.5+PV_HARD || primaryVertex->GetX() > 7.5-PV_HARD || \
    		           primaryVertex->GetY() < -4.5+PV_HARD || primaryVertex->GetY() > 9.5-PV_HARD   || \
    		           primaryVertex->GetZ() < -6.5+PV_HARD || primaryVertex->GetZ() > 7.5-PV_HARD) goodpv = false;
		     }
                     if ((!goodpv)  || nGemHits < MINGEMNDF || nSiHits < MINSIHITS || \
	    	     xGem  < -140.f || xGem > 240.f || yGem < -40.f   || yGem > 200.f || \
		     txGem < -0.5f  || txGem > 1.0f || tyGem < -0.1f || tyGem > 0.5f || \
		     Xpv < -6.5f+PV_HARD || Xpv > 7.5f-PV_HARD || Ypv < -4.5f+PV_HARD || Ypv > 9.5f-PV_HARD || \
		     Xpv == 0. || Ypv == 0. || \
		     QpGem == 0.0f  || \
            	     1.0f/TMath::Abs(QpGem) > 15.0f || 1.0f/TMath::Abs(QpGem) < 0.15f || zGem < 500.f)
        	     {
        		if (GEMDEBUG) printf("xGem %f yGem %f txGem %f tyGem %f Xpv %f Ypv %f QpGem %f\n", xGem, yGem, txGem, tyGem, Xpv, Ypv, QpGem);
    		     }
		     else
		     {
#if XGEMCOR != 0
			if (XGEMCOR == 1) { xGem -= Xgemcorrection(1./QpGem*PSCALE, &DTXcor, &DXsigma, &DYsigma); /* txGem -= DTXcor; */ }
			else if (XGEMCOR == 2) { Double_t dummy = Xgemcorrection(1./QpGem*PSCALE, &DTXcor, &DXsigma, &DYsigma); DTXcor = 0.; }
#endif
			ntrgemcut++;
			if (ACCEPTANCE)
			{
			    Double_t xgema = xGem+txGem*(z_tof700_average-zGem);
			    Double_t ygema = yGem+tyGem*(z_tof700_average-zGem);
			    if (xgema >= xmintof700 && xgema <= xmaxtof700 && ygema >= ymintof700 && ygema <= ymaxtof700)
			    {
				goodgem = true;
    				le = gemdchtrack->GetLength() - ztarg;
				p = QpGem;
    				ps = p;
				if (p != 0.) p = 1./p;
				p *= PSCALE;
				hptr->Fill(p); // GeV/c
				hltr->Fill(le); // cm
    				pmod = TMath::Abs(p);
				hxygem->Fill(xGem,yGem);
				if (p > 0.) pxygem_p->Fill(xGem,yGem,p);
				else if (p < 0.) pxygem_pn->Fill(xGem,yGem,-p);
				if (p > 0.) pxygem_tx->Fill(xGem,yGem,txGem);
				else if (p < 0.) pxygem_txn->Fill(xGem,yGem,txGem);
				if (p > 0.) pxygem_ty->Fill(xGem,yGem,tyGem);
				else if (p < 0.) pxygem_tyn->Fill(xGem,yGem,tyGem);

				hxygem0->Fill(xGem1st,yGem1st);
				if (p > 0.) pxygem0_p->Fill(xGem1st,yGem1st,p);
				else if (p < 0.) pxygem0_pn->Fill(xGem1st,yGem1st,-p);
				if (p > 0.) pxygem0_tx->Fill(xGem1st,yGem1st,txGem1st);
				else if (p < 0.) pxygem0_txn->Fill(xGem1st,yGem1st,txGem1st);
				if (p > 0.) pxygem0_ty->Fill(xGem1st,yGem1st,tyGem1st);
				else if (p < 0.) pxygem0_tyn->Fill(xGem1st,yGem1st,tyGem1st);
				if (p > 0.) hxygem0p->Fill(xGem1st,yGem1st);
				else if (p < 0.) hxygem0n->Fill(xGem1st,yGem1st);
			    }
			}
			else
			{
			    goodgem = true;
    			    le = gemdchtrack->GetLength() - ztarg;
			    p = QpGem;
    			    ps = p;
			    if (p != 0.) p = 1./p;
			    p *= PSCALE;
			    hptr->Fill(p); // GeV/c
			    hltr->Fill(le); // cm
    			    pmod = TMath::Abs(p);
			    hxygem->Fill(xGem,yGem);
			    if (p > 0.) pxygem_p->Fill(xGem,yGem,p);
			    else if (p < 0.) pxygem_pn->Fill(xGem,yGem,-p);
			    if (p > 0.) pxygem_tx->Fill(xGem,yGem,txGem);
			    else if (p < 0.) pxygem_txn->Fill(xGem,yGem,txGem);
			    if (p > 0.) pxygem_ty->Fill(xGem,yGem,tyGem);
			    else if (p < 0.) pxygem_tyn->Fill(xGem,yGem,tyGem);

			    hxygem0->Fill(xGem1st,yGem1st);
			    if (p > 0.) pxygem0_p->Fill(xGem1st,yGem1st,p);
			    else if (p < 0.) pxygem0_pn->Fill(xGem1st,yGem1st,-p);
			    if (p > 0.) pxygem0_tx->Fill(xGem1st,yGem1st,txGem1st);
			    else if (p < 0.) pxygem0_txn->Fill(xGem1st,yGem1st,txGem1st);
			    if (p > 0.) pxygem0_ty->Fill(xGem1st,yGem1st,tyGem1st);
			    else if (p < 0.) pxygem0_tyn->Fill(xGem1st,yGem1st,tyGem1st);
			    if (p > 0.) hxygem0p->Fill(xGem1st,yGem1st);
			    else if (p < 0.) hxygem0n->Fill(xGem1st,yGem1st);
			}
		     }
		    }
		    else
		    {
			if (ACCEPTANCE)
			{
			    Double_t xgema = xGem+txGem*(z_tof700_average-zGem);
			    Double_t ygema = yGem+tyGem*(z_tof700_average-zGem);
			    if (xgema >= xmintof700 && xgema <= xmaxtof700 && ygema >= ymintof700 && ygema <= ymaxtof700)
			    {
				goodgem = true;
    				le = gemdchtrack->GetLength() - ztarg;
				p = QpGem;
    				ps = p;
				if (p != 0.) p = 1./p;
				p *= PSCALE;
				hptr->Fill(p); // GeV/c
				hltr->Fill(le); // cm
    				pmod = TMath::Abs(p);
				hxygem->Fill(xGem,yGem);
				if (p > 0.) pxygem_p->Fill(xGem,yGem,p);
				else if (p < 0.) pxygem_pn->Fill(xGem,yGem,-p);
				if (p > 0.) pxygem_tx->Fill(xGem,yGem,txGem);
				else if (p < 0.) pxygem_txn->Fill(xGem,yGem,txGem);
				if (p > 0.) pxygem_ty->Fill(xGem,yGem,tyGem);
				else if (p < 0.) pxygem_tyn->Fill(xGem,yGem,tyGem);

				hxygem0->Fill(xGem1st,yGem1st);
				if (p > 0.) pxygem0_p->Fill(xGem1st,yGem1st,p);
				else if (p < 0.) pxygem0_pn->Fill(xGem1st,yGem1st,-p);
				if (p > 0.) pxygem0_tx->Fill(xGem1st,yGem1st,txGem1st);
				else if (p < 0.) pxygem0_txn->Fill(xGem1st,yGem1st,txGem1st);
				if (p > 0.) pxygem0_ty->Fill(xGem1st,yGem1st,tyGem1st);
				else if (p < 0.) pxygem0_tyn->Fill(xGem1st,yGem1st,tyGem1st);
				if (p > 0.) hxygem0p->Fill(xGem1st,yGem1st);
				else if (p < 0.) hxygem0n->Fill(xGem1st,yGem1st);
			    }
			}
			else
			{
			    goodgem = true;
    			    le = gemdchtrack->GetLength() - ztarg;
			    p = QpGem;
    			    ps = p;
			    if (p != 0.) p = 1./p;
			    p *= PSCALE;
			    hptr->Fill(p); // GeV/c
			    hltr->Fill(le); // cm
    			    pmod = TMath::Abs(p);
			    hxygem->Fill(xGem,yGem);
			    if (p > 0.) pxygem_p->Fill(xGem,yGem,p);
			    else if (p < 0.) pxygem_pn->Fill(xGem,yGem,-p);
			    if (p > 0.) pxygem_tx->Fill(xGem,yGem,txGem);
			    else if (p < 0.) pxygem_txn->Fill(xGem,yGem,txGem);
			    if (p > 0.) pxygem_ty->Fill(xGem,yGem,tyGem);
			    else if (p < 0.) pxygem_tyn->Fill(xGem,yGem,tyGem);

			    hxygem0->Fill(xGem1st,yGem1st);
			    if (p > 0.) pxygem0_p->Fill(xGem1st,yGem1st,p);
			    else if (p < 0.) pxygem0_pn->Fill(xGem1st,yGem1st,-p);
			    if (p > 0.) pxygem0_tx->Fill(xGem1st,yGem1st,txGem1st);
			    else if (p < 0.) pxygem0_txn->Fill(xGem1st,yGem1st,txGem1st);
			    if (p > 0.) pxygem0_ty->Fill(xGem1st,yGem1st,tyGem1st);
			    else if (p < 0.) pxygem0_tyn->Fill(xGem1st,yGem1st,tyGem1st);
			    if (p > 0.) hxygem0p->Fill(xGem1st,yGem1st);
			    else if (p < 0.) hxygem0n->Fill(xGem1st,yGem1st);
			}
		    }
		}
	    }
#if USEGEMTXTY != 0
	    tx = txGem;
	    ty = tyGem;
#else
#if USEDCHTXTY != 0
	    tx = txdch;
	    ty = tydch;
#endif
#endif
	    if (goodgem)
	    {
// Fragments
			pfr = fabs(p);
			pzfr = pfr/TMath::Sqrt(1.+txGem1st*txGem1st+tyGem1st*tyGem1st);
			pxfr = pzfr*txGem1st;
			pyfr = pzfr*tyGem1st;
			ptfr = TMath::Sqrt(pxfr*pxfr+pyfr*pyfr);
//
			pgem[ntrgem] = p;
			xtrgem0[ntrgem] = xGem0;
			xtrgem[ntrgem] = xGem;
			ytrgem[ntrgem] = yGem;
			txtrgem[ntrgem] = txGem;
			tytrgem[ntrgem] = tyGem;
			dxtrgem[ntrgem] = DXsigma;
			dytrgem[ntrgem] = DYsigma;
			ztrgem[ntrgem] = zGem;
			ztrgem1st[ntrgem] = zGem1st;
			ntrgem++;
	    }
	    if (TRACKS_ONLY) continue;
	    goodgemdch = false;
	    if (goodgem && gooddch)
	    {


		hdxyreal->Fill(xGem-xyzdch[0], yGem-xyzdch[1]);
		hdtxtyreal->Fill(txGem-txdch, tyGem-tydch);
		ntr1++;
// fill matching parameters
	        dxm = gemdchtrack->GetDx();
	        dym = gemdchtrack->GetDy();
	        dtx = gemdchtrack->GetDtx();
	        dty = gemdchtrack->GetDty();
	        Ddx = gemdchtrack->GetDdx();
	        Ddy = gemdchtrack->GetDdy();
	        Ddtx = gemdchtrack->GetDdtx();
	        Ddty = gemdchtrack->GetDdty();
	        Chi2m = gemdchtrack->GetChi2();
	        hdx->Fill(dxm);
	        hdy->Fill(dym);
	        hdxy0->Fill(dxm,dym);
	        hdtx->Fill(dtx);
	        hdty->Fill(dty);
	        hDdx->Fill(sqrt(Ddx));
	        hDdy->Fill(sqrt(Ddy));
	        hDdtx->Fill(sqrt(Ddtx));
	        hDdty->Fill(sqrt(Ddty));
	        hpdx->Fill(dxm/sqrt(Ddx));
	        hpdy->Fill(dym/sqrt(Ddy));
	        hpdtx->Fill(dtx/sqrt(Ddtx));
	        hpdty->Fill(dty/sqrt(Ddty));
	        hchi2m->Fill(Chi2m);
	        chi2 = dxm*dxm/Ddx + dym*dym/Ddy;
	        hchi2->Fill(chi2);
// Selection criteria - x and y tracks matching
		if (DXsigma > 0. && DYsigma > 0. && NDXsigma > 0)
		{
	          if (fabs(xGem-xyzdch[0]) <= NDXsigma*DXsigma && fabs(yGem-xyzdch[1]) <= NDXsigma*DYsigma)
	          {
		    ntr1g++;
	    	    hdxy->Fill(xGem-xyzdch[0],yGem-xyzdch[1]);
		    hxygoodtr->Fill(xgoodtr,ygoodtr);
		    if (p>0.) hxygoodtrp->Fill(xgoodtr,ygoodtr);
		    else if (p<0.) hxygoodtrn->Fill(xgoodtr,ygoodtr);
		    hxygoodtrgem->Fill(xGem,yGem);
		    hpgemdch->Fill(p); // GeV/c
		    goodgemdch = true;
		  }
		}
		else
		{
	          if (dxm >= -DXGEMDCHMAX && dxm <= +DXGEMDCHMAX && dym >= -DYGEMDCHMAX && dym <= +DYGEMDCHMAX/* && Chi2m <= 30.f && chi2 > 30.f*/)
	          {
		    ntr1g++;
	    	    hdxy->Fill(dxm,dym);
		    hxygoodtr->Fill(xgoodtr,ygoodtr);
		    if (p>0.) hxygoodtrp->Fill(xgoodtr,ygoodtr);
		    else if (p<0.) hxygoodtrn->Fill(xgoodtr,ygoodtr);
		    hxygoodtrgem->Fill(xGem,yGem);
		    hpgemdch->Fill(p); // GeV/c
		    goodgemdch = true;
		  }
		}
	    }
#if REFIT != 0
	    if (goodgemdch)
	    {
		if (REFIT_DEBUG) printf(" ********** Event %d Track %d ******************\n",iEv,iTr);
        	TMatrixDSym covdch0(5), covdch(5), icovdch(5);
		covdch *= 0.;
		covdch0 *= 0.;
//        	track->GetParamFirst()->CovMatrix(covdch0);
		for (int ir = 0; ir<5; ir++) covdch0(ir,ir) = track->GetParamFirst()->GetCovariance(ir,ir);
#if DCHCOVPAR != 0
		covdch(0,0) = covdch0(0,0);
		covdch(1,1) = covdch0(2,2);
		covdch(2,2) = covdch0(1,1);
		covdch(3,3) = covdch0(3,3);
//		covdch(4,4) = covdch0(4,4);
#else
		covdch = covdch0;
#endif
		covdch(4,4) = 10000.;
		if (REFIT_DEBUG) printf(" Err DCH x  = %10.5f\n", TMath::Sqrt(covdch(0,0)));
		if (REFIT_DEBUG) printf(" Err DCH y  = %10.5f\n", TMath::Sqrt(covdch(1,1)));
		if (REFIT_DEBUG) printf(" Err DCH tx = %10.5f\n", TMath::Sqrt(covdch(2,2)));
		if (REFIT_DEBUG) printf(" Err DCH ty = %10.5f\n", TMath::Sqrt(covdch(3,3)));
		icovdch = covdch;
		icovdch.Invert();
        	TMatrixDSym covgem(5), icovgem(5);
		covgem *= 0.;
//        	gtrack->GetParamLast()->CovMatrix(covgem);
		for (int ir = 0; ir<5; ir++) for (int ic = 0; ic<5; ic++) covgem(ir,ic) = gtrack->GetParamLast()->GetCovariance(ir,ic);
		if (REFIT_DEBUG) printf(" Err GEM x  = %10.5f\n", TMath::Sqrt(covgem(0,0)));
		if (REFIT_DEBUG) printf(" Err GEM y  = %10.5f\n", TMath::Sqrt(covgem(1,1)));
		if (REFIT_DEBUG) printf(" Err GEM tx = %10.5f\n", TMath::Sqrt(covgem(2,2)));
		if (REFIT_DEBUG) printf(" Err GEM ty = %10.5f\n", TMath::Sqrt(covgem(3,3)));
		if (REFIT_DEBUG) printf(" Err GEM ip = %10.5f\n", TMath::Sqrt(covgem(4,4))/QpGem);
		icovgem = covgem;
		icovgem.Invert();
        	TMatrixDSym covgemdch(5), icovgemdch(5);
		icovgemdch *= 0.;
		icovgemdch += icovgem;
		icovgemdch += icovdch;
		covgemdch = icovgemdch;
		covgemdch.Invert();
        	TVectorD sumgemdch(5), pargemdch(5);
		TVectorD pargem(5), pardch(5), pagemdch(5);
		pardch(0) = xyzdch[0];
		pardch(1) = xyzdch[1];
		pardch(2) = txdch;
		pardch(3) = tydch;
		pardch(4) = 1.;
		pargem(0) = xGem;
		pargem(1) = yGem;
		pargem(2) = txGem;
		pargem(3) = tyGem;
		pargem(4) = QpGem;
		sumgemdch *= 0.;
		sumgemdch += icovgem*pargem;
		sumgemdch += icovdch*pardch;
		pargemdch = covgemdch*sumgemdch;
		xyz[0] = pargemdch(0);
		xyz[1] = pargemdch(1);
		tx = pargemdch(2);
		ty = pargemdch(3);
		hdxrefit1->Fill(pargemdch(0)-pardch(0));
		hdyrefit1->Fill(pargemdch(1)-pardch(1));
		hdtxrefit1->Fill(pargemdch(2)-pardch(2));
		hdtyrefit1->Fill(pargemdch(3)-pardch(3));
		hdxrefit->Fill(pargemdch(0)-pargem(0));
		hdyrefit->Fill(pargemdch(1)-pargem(1));
		hdtxrefit->Fill(pargemdch(2)-pargem(2));
		hdtyrefit->Fill(pargemdch(3)-pargem(3));
		if (pargem(4)!=0.) hdprefit->Fill((pargemdch(4)-pargem(4))/pargem(4));
//		tx0 = tx;
		cxyz[0] = tx/TMath::Sqrt(1.+tx*tx+ty*ty);
		cxyz[1] = ty/TMath::Sqrt(1.+tx*tx+ty*ty);
		cxyz[2] = 1./TMath::Sqrt(1.+tx*tx+ty*ty);
        	track->GetParamLast()->SetCovMatrix(covgemdch);
		if (REFIT_DEBUG) printf("    x  %10.5f %10.5f %10.5f\n",pargem(0),pardch(0),pargemdch(0));
		if (REFIT_DEBUG) printf("    y  %10.5f %10.5f %10.5f\n",pargem(1),pardch(1),pargemdch(1));
		if (REFIT_DEBUG) printf("    tx %10.5f %10.5f %10.5f\n",pargem(2),pardch(2),pargemdch(2));
		if (REFIT_DEBUG) printf("    ty %10.5f %10.5f %10.5f\n",pargem(3),pardch(3),pargemdch(3));
		if (REFIT_DEBUG) printf("    p  %10.5f %10.5f %10.5f\n",1./pargem(4),1./pardch(4),1./pargemdch(4));
	    }
#endif
	    if (goodgemdch)
	    {
		Int_t nth = 0, itc[100] = {0}, its[100] = {0};
		TOF2->get_track_hits(xyz, cxyz, &nth, itc, its);
		if (nth>0 && idchambers[itc[0]] != 19.3f)
		{
		    crpc1 = itc[0];
		    xystrip(hxytrackstrips, itc[0], its[0]);
		    zrpcm = zca[itc[0]];
		    dz = zrpcm - xyz[2];
		    xrpcm = xyz[0] + dz*tx;
		    yrpcm = xyz[1] + dz*ty;
		    hplatraz->Fill(zrpcm);
		    hplatra->Fill(xrpcm);
		    if (zrpcm < 600) hplatra1a->Fill(xrpcm);
		    if (zrpcm > 605 && zrpcm < 610) hplatra1b->Fill(xrpcm);
		    if (zrpcm > 625 && zrpcm < 630 && yrpcm < 20) hplatra2ac->Fill(xrpcm);
		    if (zrpcm > 625 && zrpcm < 630 && yrpcm > 20) hplatra2au->Fill(xrpcm);
		    if (zrpcm > 635 && zrpcm < 660 && yrpcm < 20) hplatra2bc->Fill(xrpcm);
		    if (zrpcm > 635 && zrpcm < 660 && yrpcm > 20) hplatra2bu->Fill(xrpcm);
		    if (zrpcm > 625 && zrpcm < 660 && yrpcm < 20) hplatra2abc->Fill(xrpcm);
		}
		if (nth>1 && idchambers[itc[1]] != 19.3f)
		{
		    crpc2 = itc[1];
		    xystrip(hxytrackstrips, itc[1], its[1]);
		    zrpcm1 = zca[itc[1]];
		    dz = zrpcm1 - xyz[2];
		    xrpcm1 = xyz[0] + dz*tx;
		    yrpcm1 = xyz[1] + dz*ty;
		    hplatraz->Fill(zrpcm1);
		    hplatra->Fill(xrpcm1);
		    if (zrpcm1 < 600) hplatra1a->Fill(xrpcm1);
		    if (zrpcm1 > 605 && zrpcm1 < 610) hplatra1b->Fill(xrpcm1);
		    if (zrpcm1 > 625 && zrpcm1 < 630 && yrpcm1 < 20) hplatra2ac->Fill(xrpcm1);
		    if (zrpcm1 > 625 && zrpcm1 < 630 && yrpcm1 > 20) hplatra2au->Fill(xrpcm1);
		    if (zrpcm1 > 635 && zrpcm1 < 660 && yrpcm1 < 20) hplatra2bc->Fill(xrpcm1);
		    if (zrpcm1 > 635 && zrpcm1 < 660 && yrpcm1 > 20) hplatra2bu->Fill(xrpcm1);
		    if (zrpcm1 > 625 && zrpcm1 < 660 && yrpcm1 < 20) hplatra2abc->Fill(xrpcm1);
		}
	    }

	    Double_t corxrpc = 0.;
#if XTOFCOR == 1
	    corxrpc = Xgemcorrectiontof700(1./QpGem*PSCALE, &SigmaX_TOF700, &SigmaY_TOF700);
#else
	    Xgemcorrectiontof700(1./QpGem*PSCALE, &SigmaX_TOF700, &SigmaY_TOF700);
#endif
	    if (USE_CLUSTERS)
	    {
		    himin = -1;
		    dist = 0.;
		    distmin = 1000000000.0f;
		    for (int icl = 0; icl<ncl0; icl++)
		    {
			if (!goodgemdch) continue;
			ic = clchamb[icl];
			is = clstrip[icl];
			int ih = numhit[ic][is];
//			if (usedstrip[ih]) continue;
			dz = zhi[ih] - xyz[2];
#if USEDCHXY == 0
			xrpc = xyz[0] + dz*tx;
			yrpc = xyz[1] + dz*ty;
			xrpc -= corxrpc;

#else
			xrpc = xyzdch[0] + dz*tx;
			yrpc = xyzdch[1] + dz*ty;
#endif

			dx = xrpc - xhi[ih];
			dy = yrpc - yhi[ih];
			if (TMath::Abs(dx) > NTOFsigma*SigmaX_TOF700) continue;
			if (TMath::Abs(dy) > NTOFsigma*SigmaY_TOF700) continue;
			dist = TMath::Sqrt(dx*dx + dy*dy);
			himin = numhit[ic][is];
			Ihit[Npairs] = himin;
			Dpairs[Npairs] = dist;
			Cpairs[Npairs] = dist/TMath::Sqrt(SigmaX_TOF700*SigmaX_TOF700+SigmaY_TOF700*SigmaY_TOF700);
			Itrack[Npairs] = iTr;

			Vpairs[Npairs][0] = xgoodtr;
			Vpairs[Npairs][1] = ygoodtr;
			Vpairs[Npairs][2] = xGem;
			Vpairs[Npairs][3] = yGem;
			Vpairs[Npairs][4] = zGem;
			Vpairs[Npairs][5] = txGem;
			Vpairs[Npairs][6] = tyGem;
			Vpairs[Npairs][7] = p;
			Vpairs[Npairs][8] = dxm;
			Vpairs[Npairs][9] = dym;
			Vpairs[Npairs][10] = xGem1st;
			Vpairs[Npairs][11] = yGem1st;
			Vpairs[Npairs][12] = xyz[0];;
			Vpairs[Npairs][13] = xyz[1];
			Vpairs[Npairs][14] = xyz[2];
			Vpairs[Npairs][15] = tx;
			Vpairs[Npairs][16] = ty;
			Vpairs[Npairs][17] = le;
			Vpairs[Npairs][18] = pfr;
			Vpairs[Npairs][19] = pxfr;
			Vpairs[Npairs][20] = pyfr;
			Vpairs[Npairs][21] = pzfr;
			Vpairs[Npairs][22] = ptfr;
			Vpairs[Npairs][23] = corxrpc;

			Npairs++;
		    }
		    himin1 = -1;
		    dist = 0.;
		    distmin1 = 1000000000.0f;
		    for (int icl = 0; icl<ncl0; icl++)
		    {
			if (!gooddch) continue;
			ic = clchamb[icl];
			is = clstrip[icl];
			int ih = numhit[ic][is];
//			if (usedstrip1[ih]) continue;
			dz = zhi[ih] - xyzdch[2];
			xrpc = xyzdch[0] + dz*txdch;
			yrpc = xyzdch[1] + dz*tydch;
			dx = xrpc - xhi[ih];
			dy = yrpc - yhi[ih];
			if (TMath::Abs(dx) > NTOFsigma*SigmaX_DCH_TOF) continue;
			if (TMath::Abs(dy) > NTOFsigma*SigmaY_DCH_TOF) continue;
			dist = TMath::Sqrt(dx*dx + dy*dy);
			himin1 = numhit[ic][is];
			Ihit1[Npairs1] = himin1;
			Dpairs1[Npairs1] = dist;
			Cpairs1[Npairs1] = dist/TMath::Sqrt(SigmaX_DCH_TOF*SigmaX_DCH_TOF+SigmaY_DCH_TOF*SigmaY_DCH_TOF);
			Itrack1[Npairs1] = iTr;

			Vpairs1[Npairs1][0] = xgoodtr1;
			Vpairs1[Npairs1][1] = ygoodtr1;
			Vpairs1[Npairs1][2] = xyzdch[0];
			Vpairs1[Npairs1][3] = xyzdch[1];
			Vpairs1[Npairs1][4] = xyzdch[2];
			Vpairs1[Npairs1][5] = txdch;
			Vpairs1[Npairs1][6] = tydch;
			Vpairs1[Npairs1][7] = p;
			Vpairs1[Npairs1][8] = 0.;
			if (goodgemdch) Vpairs1[Npairs1][8] = 1.;

			Npairs1++;
		    }
		    himin2 = -1;
		    dist = 0.;
		    distmin2 = 1000000000.0f;
		    for (int icl = 0; icl<ncl0; icl++)
		    {
			if (!goodgem) continue;
			ic = clchamb[icl];
			is = clstrip[icl];
			int ih = numhit[ic][is];
//			if (usedstrip2[ih]) continue;
			dz = zhi[ih] - zGem;
			xrpc = xGem + dz*txGem;
			yrpc = yGem + dz*tyGem;
			dx = xrpc - xhi[ih];
			dy = yrpc - yhi[ih];
			if (TMath::Abs(dx) > NTOFsigma*SigmaX_GEM_TOF) continue;
			if (TMath::Abs(dy) > NTOFsigma*SigmaY_GEM_TOF) continue;
			dist = TMath::Sqrt(dx*dx + dy*dy);
			himin2 = numhit[ic][is];
			Ihit2[Npairs2] = himin2;
			Dpairs2[Npairs2] = dist;
			Cpairs2[Npairs2] = dist/TMath::Sqrt(SigmaX_GEM_TOF*SigmaX_GEM_TOF+SigmaY_GEM_TOF*SigmaY_GEM_TOF);
			Itrack2[Npairs2] = iTr;

			Vpairs2[Npairs2][0] = xgoodtr;
			Vpairs2[Npairs2][1] = xgoodtr;
			Vpairs2[Npairs2][2] = xGem;
			Vpairs2[Npairs2][3] = yGem;
			Vpairs2[Npairs2][4] = zGem;
			Vpairs2[Npairs2][5] = txGem;
			Vpairs2[Npairs2][6] = tyGem;
			Vpairs2[Npairs2][7] = p;
			Vpairs2[Npairs2][8] = dxm;
			Vpairs2[Npairs2][9] = dym;
			Vpairs2[Npairs2][10] = 0.;
			if (goodgemdch) Vpairs2[Npairs2][10] = 1.;

			Npairs2++;
		    }
	    }
	    else
	    {
		    nh = nhi;
		    himin = -1;
		    dist = 0.;
		    distmin = 1000000000.0f;
		    for (int ih = 0; ih<nh; ih++)
		    {
			if (!goodgemdch) continue;
//			if (usedstrip[ih]) continue;
			ic = hchamb[ih];
			is = hstrip[ih];
			dz = zhi[ih] - xyz[2];
#if USEDCHXY == 0
			xrpc = xyz[0] + dz*tx;
			yrpc = xyz[1] + dz*ty;
			xrpc -= corxrpc;
#else
			xrpc = xyzdch[0] + dz*tx;
			yrpc = xyzdch[1] + dz*ty;
#endif
			dx = xrpc - xhi[ih];
			dy = yrpc - yhi[ih];
			if (TMath::Abs(dx) > NTOFsigma*SigmaX_TOF700) continue;
			if (TMath::Abs(dy) > NTOFsigma*SigmaY_TOF700) continue;
			dist = TMath::Sqrt(dx*dx + dy*dy);
			himin = numhit[ic][is];
			Ihit[Npairs] = himin;
			Dpairs[Npairs] = dist;
			Cpairs[Npairs] = dist/TMath::Sqrt(SigmaX_TOF700*SigmaX_TOF700+SigmaY_TOF700*SigmaY_TOF700);
			Itrack[Npairs] = iTr;

			Vpairs[Npairs][0] = xgoodtr;
			Vpairs[Npairs][1] = ygoodtr;
			Vpairs[Npairs][2] = xGem;
			Vpairs[Npairs][3] = yGem;
			Vpairs[Npairs][4] = zGem;
			Vpairs[Npairs][5] = txGem;
			Vpairs[Npairs][6] = tyGem;
			Vpairs[Npairs][7] = p;
			Vpairs[Npairs][8] = dxm;
			Vpairs[Npairs][9] = dym;
			Vpairs[Npairs][10] = xGem1st;
			Vpairs[Npairs][11] = yGem1st;
			Vpairs[Npairs][12] = xyz[0];;
			Vpairs[Npairs][13] = xyz[1];
			Vpairs[Npairs][14] = xyz[2];
			Vpairs[Npairs][15] = tx;
			Vpairs[Npairs][16] = ty;
			Vpairs[Npairs][17] = le;
			Vpairs[Npairs][18] = pfr;
			Vpairs[Npairs][19] = pxfr;
			Vpairs[Npairs][20] = pyfr;
			Vpairs[Npairs][21] = pzfr;
			Vpairs[Npairs][22] = ptfr;
			Vpairs[Npairs][23] = corxrpc;

			Npairs++;
		    }

		    nh = nhi;
		    himin1 = -1;
		    dist = 0.;
		    distmin1 = 1000000000.0f;
		    for (int ih = 0; ih<nh; ih++)
		    {
			if (!gooddch) continue;
//			if (usedstrip1[ih]) continue;
			ic = hchamb[ih];
			is = hstrip[ih];
			dz = zhi[ih] - xyz[2];
			xrpc = xyzdch[0] + dz*txdch;
			yrpc = xyzdch[1] + dz*tydch;
			dx = xrpc - xhi[ih];
			dy = yrpc - yhi[ih];
			if (TMath::Abs(dx) > NTOFsigma*SigmaX_DCH_TOF) continue;
			if (TMath::Abs(dy) > NTOFsigma*SigmaY_DCH_TOF) continue;
			dist = TMath::Sqrt(dx*dx + dy*dy);
			himin1 = numhit[ic][is];
			Ihit1[Npairs1] = himin1;
			Dpairs1[Npairs1] = dist;
			Cpairs1[Npairs1] = dist/TMath::Sqrt(SigmaX_DCH_TOF*SigmaX_DCH_TOF+SigmaY_DCH_TOF*SigmaY_DCH_TOF);
			Itrack1[Npairs1] = iTr;

			Vpairs1[Npairs1][0] = xgoodtr1;
			Vpairs1[Npairs1][1] = ygoodtr1;
			Vpairs1[Npairs1][2] = xyzdch[0];
			Vpairs1[Npairs1][3] = xyzdch[1];
			Vpairs1[Npairs1][4] = xyzdch[2];
			Vpairs1[Npairs1][5] = txdch;
			Vpairs1[Npairs1][6] = tydch;
			Vpairs1[Npairs1][7] = p;
			Vpairs1[Npairs1][8] = 0.;
			if (goodgemdch) Vpairs1[Npairs1][8] = 1.;

			Npairs1++;
		    }

		    nh = nhi;
		    himin2 = -1;
		    dist = 0.;
		    distmin2 = 1000000000.0f;
		    for (int ih = 0; ih<nh; ih++)
		    {
			if (!goodgem) continue;
//			if (usedstrip2[ih]) continue;
			ic = hchamb[ih];
			is = hstrip[ih];
			dz = zhi[ih] - zGem;
			xrpc = xGem + dz*txGem;
			yrpc = yGem + dz*tyGem;
			dx = xrpc - xhi[ih];
			dy = yrpc - yhi[ih];
			if (TMath::Abs(dx) > NTOFsigma*SigmaX_GEM_TOF) continue;
			if (TMath::Abs(dy) > NTOFsigma*SigmaY_GEM_TOF) continue;
			dist = TMath::Sqrt(dx*dx + dy*dy);
			himin2 = numhit[ic][is];
			Ihit2[Npairs2] = himin2;
			Dpairs2[Npairs2] = dist;
			Cpairs2[Npairs2] = dist/TMath::Sqrt(SigmaX_GEM_TOF*SigmaX_GEM_TOF+SigmaY_GEM_TOF*SigmaY_GEM_TOF);
			Itrack2[Npairs2] = iTr;

			Vpairs2[Npairs2][0] = xgoodtr;
			Vpairs2[Npairs2][1] = xgoodtr;
			Vpairs2[Npairs2][2] = xGem;
			Vpairs2[Npairs2][3] = yGem;
			Vpairs2[Npairs2][4] = zGem;
			Vpairs2[Npairs2][5] = txGem;
			Vpairs2[Npairs2][6] = tyGem;
			Vpairs2[Npairs2][7] = p;
			Vpairs2[Npairs2][8] = dxm;
			Vpairs2[Npairs2][9] = dym;
			Vpairs2[Npairs2][10] = 0.;
			if (goodgemdch) Vpairs2[Npairs2][10] = 1.;

			Npairs2++;
		    }
	    }
	} // tracks loop
// DCH+TOF700 matching
// Selection criteria on minimal distance track-hit
// Selection criteria time-of-flight
	Int_t iptra = 0, ip = 0;
	TMath::Sort(Npairs1, Dpairs1, Isort1, false);
	for (int ip0 = 0; ip0 < Npairs1; ip0++)
	{
	    gooddchtof = false;
	    ip = Isort1[ip0];
	    himin1 = Ihit1[ip];
	    iptra = Itrack1[ip];
	    if (himin1 >= 0 && !usedstrip1[himin1] && !usedtrack1[iptra] )
	    {
		ic = hchamb[himin1];
		is = hstrip[himin1];
		if (tof[ic][is] <= TOFMAX)
		{
		    gooddchtof = true;
		    usedstrip1[himin1] = true;
		    usedtrack1[iptra] = true;
		    xgoodtr1 = Vpairs1[ip][0];
		    ygoodtr1 = Vpairs1[ip][1];
		    xyzdch[0] = Vpairs1[ip][2];
		    xyzdch[1] = Vpairs1[ip][3];
		    xyzdch[2] = Vpairs1[ip][4];
		    txdch = Vpairs1[ip][5];
		    tydch = Vpairs1[ip][6];
		    p = Vpairs1[ip][7];
		    goodgemdch = false;
		    if (Vpairs1[ip][8] != 0.) goodgemdch = true;
		    hxygoodhit1->Fill(xgoodtr1,ygoodtr1);
		    hxygoodclusterdch->Fill(xhi[himin1],yhi[himin1]);
		    if (goodgemdch)
		    {
			dz = zhi[himin] - xyzdch[2];
			xrpc = xyzdch[0] + dz*txdch;
			yrpc = xyzdch[1] + dz*tydch;
			hxyhit1->Fill(xrpc-xhi[himin1],yrpc-yhi[himin1]);
			phitdxvsx1->Fill(xrpc, xrpc-xhi[himin1]);
			phitdyvsx1->Fill(xrpc, yrpc-yhi[himin1]);
			phitdxvsy1->Fill(yrpc, xrpc-xhi[himin1]);
			phitdyvsy1->Fill(yrpc, yrpc-yhi[himin1]);
			hhitdxvsx1->Fill(xrpc, xrpc-xhi[himin1]);
	        	if (p>0.f)
	        	{
			    hhitdxvsp11->Fill(p, xrpc-xhi[himin1]);
			    hhitdyvsp1->Fill(p, yrpc-yhi[himin1]);
	        	}
			else if (p<0.f)
			{
		    	    hhitdxvspn1->Fill(fabs(p), xrpc-xhi[himin1]);

		    	    hhitdyvspn1->Fill(fabs(p), yrpc-yhi[himin1]);
			}
		    }
		}
	    }
	} // loop on pairs type 1 (dch_track+tof700_hit)
// GEM+TOF700 matching
// Selection criteria on minimal distance track-hit
// Selection criteria time-of-flight
	TMath::Sort(Npairs2, Dpairs2, Isort2, false);
	for (int ip0 = 0; ip0 < Npairs2; ip0++)
	{
	    goodgemtof = false;
	    goodgemdch2 = false;
	    ip = Isort2[ip0];
	    himin2 = Ihit2[ip];
	    iptra = Itrack2[ip];
	    if (himin2 >= 0 && !usedstrip2[himin2] && !usedtrack2[iptra])
	    {
		ic = hchamb[himin2];
		is = hstrip[himin2];
		if (tof[ic][is] <= TOFMAX)
		{
		    goodgemtof = true;
		    usedstrip2[himin2] = true;
		    usedtrack2[iptra] = true;
		    xgoodtr = Vpairs2[ip][0];
		    ygoodtr = Vpairs2[ip][1];
		    xGem = Vpairs2[ip][2];
		    yGem = Vpairs2[ip][3];
		    zGem = Vpairs2[ip][4];
		    txGem = Vpairs2[ip][5];
		    tyGem = Vpairs2[ip][6];
		    p = Vpairs2[ip][7];
		    dxm = Vpairs2[ip][8];
		    dym = Vpairs2[ip][9];
		    goodgemdch = false;
		    if (Vpairs2[ip][10] != 0.) goodgemdch = true;
		    hxygoodhit2->Fill(xGem,yGem);
		    hpgemtof->Fill(p);
		    dz = zhi[himin2] - zGem;
		    xrpc = xGem + dz*txGem;
		    yrpc = yGem + dz*tyGem;
		    hxygoodclustergem->Fill(xhi[himin2],yhi[himin2]);

		    hxyhit2g->Fill(xrpc-xhi[himin2],yrpc-yhi[himin2]);
		    hhitdxvsx2g->Fill(xrpc, xrpc-xhi[himin2]);
	    	    if (p>0.f)
	    	    {
			hhitdxvsp2g->Fill(p, xrpc-xhi[himin2]);
			hhitdyvsp2g->Fill(p, yrpc-yhi[himin2]);
	    	    }
		    else if (p<0.f)
		    {
			hhitdxvspn2g->Fill(fabs(p), xrpc-xhi[himin2]);
			hhitdyvspn2g->Fill(fabs(p), yrpc-yhi[himin2]);
		    }

		    if (goodgemdch)
		    {
			goodgemdch2 = true;
			ntr2g++;
	    		hdxy2->Fill(dxm,dym);
			hxygoodtrgem2->Fill(xGem,yGem);
			hxygoodtr2->Fill(xgoodtr,ygoodtr);
			hpgemdch2->Fill(p); // GeV/c
			hxyhit2->Fill(xrpc-xhi[himin2],yrpc-yhi[himin2]);
			phitdxvsx2->Fill(xrpc, xrpc-xhi[himin2]);
			phitdyvsx2->Fill(xrpc, yrpc-yhi[himin2]);
			phitdxvsy2->Fill(yrpc, xrpc-xhi[himin2]);
			phitdyvsy2->Fill(yrpc, yrpc-yhi[himin2]);
			hhitdxvsx2->Fill(xrpc, xrpc-xhi[himin2]);
	        	if (p>0.f)
	        	{
			    hhitdxvsp22->Fill(p, xrpc-xhi[himin2]);
			    hhitdyvsp2->Fill(p, yrpc-yhi[himin2]);
	        	}
			else if (p<0.f)
			{
		    	    hhitdxvspn2->Fill(fabs(p), xrpc-xhi[himin2]);
		    	    hhitdyvspn2->Fill(fabs(p), yrpc-yhi[himin2]);
			}
		    }
		}
	    }
	} // loop on pairs type 2 (gem_track+tof700_hit)
// GEMTOF+DCHTOF matching
// Selection criteria on minimal distance track-hit
// Selection criteria time-of-flight

	Int_t ipa = 0, iptraa = 0;
	for (int ip0 = 0; ip0 < Npairs2; ip0++)
	{
	    ip = Isort2[ip0];
	    himin2 = Ihit2[ip];
	    iptra = Itrack2[ip];
	    for (int ip0a = 0; ip0a < Npairs1; ip0a++)
	    {
		ipa = Isort1[ip0];
		himin1 = Ihit1[ipa];
		iptraa = Itrack1[ipa];
		if (usedtrack2[iptra] && usedtrack1[iptraa] && iptra == iptraa)
		{
			xgoodtr = Vpairs2[ip][0];
			ygoodtr = Vpairs2[ip][1];
			xGem = Vpairs2[ip][2];
			yGem = Vpairs2[ip][3];
			p = Vpairs2[ip][7];
			hpgemtofdchtof->Fill(p);
			hxygemtofdchtof->Fill(xGem,yGem);
			hxygemtofdchtofd->Fill(xgoodtr,ygoodtr);
			goodgemdch = false;
			if (Vpairs2[ip][10] != 0.) goodgemdch = true;
			if (goodgemdch)
			{
			    hpgemtofdchtof1->Fill(p);
			    hxygemtofdchtof1->Fill(xGem,yGem);
			    hxygemtofdchtofd1->Fill(xgoodtr,ygoodtr);
			    if (himin1 == himin2)
			    {
				hpgemtofdchtof2->Fill(p);
				hxygemtofdchtof2->Fill(xGem,yGem);
				hxygemtofdchtofd2->Fill(xgoodtr,ygoodtr);
			    }
			}
			break;
		}
	    }
	}

// GEM+DCH+TOF700 matching
// Selection criteria on minimal distance track-hit
// Selection criteria time-of-flight
	Double_t corxrpc = 0.;
	TMath::Sort(Npairs, Dpairs, Isort, false);
	for (int ip0 = 0; ip0 < Npairs; ip0++)
	{
	    goodgemdchtof = false;
	    goodgemdchtof2 = false;
	    ip = Isort[ip0];
	    himin = Ihit[ip];
	    iptra = Itrack[ip];
	    if (himin >= 0)
	    {
		ic = hchamb[himin];
		is = hstrip[himin];
		if (tof[ic][is] <= TOFMAX && !usedstrip[himin] && !usedtrack[iptra])
		{
		    usedstrip[himin] = true;
		    usedtrack[iptra] = true;
		    xgoodtr = Vpairs[ip][0];
		    ygoodtr = Vpairs[ip][1];
		    xGem = Vpairs[ip][2];
		    yGem = Vpairs[ip][3];
		    zGem = Vpairs[ip][4];
		    txGem = Vpairs[ip][5];
		    tyGem = Vpairs[ip][6];
		    p = Vpairs[ip][7];
		    dxm = Vpairs[ip][8];
		    dym = Vpairs[ip][9];
		    xGem1st = Vpairs[ip][10];
		    yGem1st = Vpairs[ip][11];
		    xyz[0] = Vpairs[ip][12];
		    xyz[1] = Vpairs[ip][13];
		    xyz[2] = Vpairs[ip][14];
		    tx = Vpairs[ip][15];
		    ty = Vpairs[ip][16];
		    le = Vpairs[ip][17];
		    pfr = Vpairs[ip][18];
		    pxfr = Vpairs[ip][19];
		    pyfr = Vpairs[ip][20];
		    pzfr = Vpairs[ip][21];
		    ptfr = Vpairs[ip][22];
		    corxrpc = Vpairs[ip][23];
//		    if (goodgemdch2)
		    if (usedtrack2[iptra])
		    {
			ntr02++;
		        goodgemdchtof2 = true;
		        hdxytof2->Fill(dxm,dym);
		        hxygoodhitgem2->Fill(xGem,yGem);
		        hxygoodhita2->Fill(xhi[himin],yhi[himin]);
		        hpgoodhit2->Fill(p); // GeV/c
		    }
		    ntr0++;
		    ncltr[ic]++;
		    goodgemdchtof = true;
		    usedstrip[himin] = true;
		    hdxytof->Fill(dxm,dym);
		    x700fr = xgoodtr;
		    y700fr = ygoodtr;
		    hxygoodhit->Fill(xgoodtr,ygoodtr);
		    if (p>0.) hxygoodhitp->Fill(xgoodtr,ygoodtr);
		    else if (p<0.) hxygoodhitn->Fill(xgoodtr,ygoodtr);
		    hxygoodhita->Fill(xhi[himin],yhi[himin]);
		    hxygoodcluster->Fill(xhi[himin],yhi[himin]);
		    if (idchambers[ic] != 19.3) xystrip(hxygoodstrips, ic, is);
		    if (idchambers[ic] != 19.3 && (ic == crpc1 || ic == crpc2)) xystrip(hxygood1strips, ic, is);
		    hxygoodtrgem0->Fill(xGem1st,yGem1st);
		    if (p>0.) hxygoodtrgem0p->Fill(xGem1st,yGem1st);
		    else if (p<0.) hxygoodtrgem0n->Fill(xGem1st,yGem1st);
		    hpgoodhit->Fill(p); // GeV/c
		    xc = xhic[himin];
		    yc = yhic[himin];
		    zc = zhic[himin];
		    dz = zhi[himin] - xyz[2];
#if USEDCHXY == 0
		    xrpc = xyz[0] + dz*tx;
		    yrpc = xyz[1] + dz*ty;
		    xrpc -= corxrpc;
#else
		    xrpc = xyzdch[0] + dz*txdch;
		    yrpc = xyzdch[1] + dz*tydch;
#endif
		    hxyhit->Fill(xrpc-xhi[himin],yrpc-yhi[himin]);
		    hxyghit->Fill(xGem-xyzdch[0],yGem-xyzdch[1]);
	    	    if (idchambers[ic] < 100.f && fabs(lrd[himin]) < 1.) hxyhitlr->Fill(xrpc-xhi[himin],yrpc-yhi[himin]);
	    	    if (idchambers[ic] > 100.f && fabs(lrd[himin]) < 3.) hxyhitlr->Fill(xrpc-xhi[himin],yrpc-yhi[himin]);
		    if (idchambers[ic] < 100.f) hxdiffs->Fill(xhi[himin]-xc);
		    if (idchambers[ic] > 100.f) hxdiffb->Fill(xhi[himin]-xc);
		    if (idchambers[ic] < 100.f) hxxhits->Fill(xhi[himin]-xc,xrpc-xc);
		    if (idchambers[ic] > 100.f) hxxhitb->Fill(xhi[himin]-xc,xrpc-xc);
		    if (idchambers[ic] < 100.f) hlrs->Fill(lrd[himin]);
		    if (idchambers[ic] > 100.f) hlrb->Fill(lrd[himin]);
		    phitdxvsx->Fill(xrpc, xrpc-xhi[himin]);
		    phitdyvsx->Fill(xrpc, yrpc-yhi[himin]);
		    phitdxvsy->Fill(yrpc, xrpc-xhi[himin]);
		    phitdyvsy->Fill(yrpc, yrpc-yhi[himin]);
		    hhitdxvsx->Fill(xrpc, xrpc-xhi[himin]);
		    if (p>0.f)
		    {
			hhitdxvsp->Fill(p, xrpc-xhi[himin]);
			hhitdyvsp->Fill(p, yrpc-yhi[himin]);
			if (xrpc<0.)        hhitdxvsp1->Fill(p, xrpc-xhi[himin]);
			else if (xrpc<40.)  hhitdxvsp2->Fill(p, xrpc-xhi[himin]);
			else if (xrpc<80.)  hhitdxvsp3->Fill(p, xrpc-xhi[himin]);
			else if (xrpc<120.) hhitdxvsp4->Fill(p, xrpc-xhi[himin]);
			else if (xrpc<160.) hhitdxvsp5->Fill(p, xrpc-xhi[himin]);
		    }
		    else if (p<0.f)
		    {
			hhitdxvspn->Fill(fabs(p), xrpc-xhi[himin]);
			hhitdyvspn->Fill(fabs(p), yrpc-yhi[himin]);
		    }
// DCH1 hits
		    hghitdxvsx->Fill(xGem, xGem-xyzdch[0]);
		    if (p>0.f)
		    {
			hghitdxvsp->Fill(p, xGem-xyzdch[0]);
			hghitdyvsp->Fill(p, yGem-xyzdch[1]);
			if (xGem<0.)        hghitdxvsp1->Fill(p, xGem-xyzdch[0]);
			else if (xGem<40.)  hghitdxvsp2->Fill(p, xGem-xyzdch[0]);
			else if (xGem<80.)  hghitdxvsp3->Fill(p, xGem-xyzdch[0]);
			else if (xGem<120.) hghitdxvsp4->Fill(p, xGem-xyzdch[0]);
			else if (xGem<160.) hghitdxvsp5->Fill(p, xGem-xyzdch[0]);
			hgdtxvsp->Fill(p, txGem-txdch);
			hgdtxrvsp->Fill(p, tx0-txdch);
			hgdtx1vsp->Fill(p, (xGem-xyzdch[0])/(zGem-zGem1st));
			hgdtxty->Fill(txGem-txdch, tyGem-tydch);
		    }
		    else if (p<0.f)
		    {
			hghitdxvspn->Fill(fabs(p), xGem-xyzdch[0]);
			hghitdyvspn->Fill(fabs(p), yGem-xyzdch[1]);
			hgdtxvspn->Fill(fabs(p), txGem-txdch);
			hgdtxrvspn->Fill(fabs(p), tx0-txdch);
			hgdtx1vspn->Fill(fabs(p), (xGem-xyzdch[0])/(zGem-zGem1st));
			hgdtxtyn->Fill(txGem-txdch, tyGem-tydch);
		    }
// Big systematic at edges
		    if (xrpc > 110.) hchambigx->Fill(ic);
		    if (yrpc > 75.) hchambigy->Fill(ic);
		    hxvsxbigx->Fill(xhi[himin],xrpc);
		    hyvsybigy->Fill(yhi[himin],yrpc);
		    if (xrpc > 110.) hpbigx->Fill(p);
		    if (yrpc > 75.) hpbigy->Fill(p);
		    if (xrpc > 110.) hybigx->Fill(yrpc);
		    if (yrpc > 75.) hxbigy->Fill(xrpc);
		    if (xrpc > 110. && (ic == 21 || ic == 22)) hlrbigx->Fill(lrd[himin]);
		    if (xrpc > 110. && (ic == 21 || ic == 22)) hxxhitbigx->Fill(xhi[himin]-xc,xrpc-xc);
//
		    cosa = sqrt(1.+tx*tx+ty*ty);
		    dl = dz*cosa;
		    lenr = le+dl;
		    hltra->Fill(lenr);
		    hidist = TMath::Sqrt(xc*xc+yc*yc+(zc-ztarg)*(zc-ztarg));
		    hstrtr->Fill(is);
		    hchatr->Fill(ic);
		    hxytrtof->Fill(xrpc,yrpc);

		    hplatrz->Fill(zhi[himin]);
		    hplatr->Fill(xhi[himin]);
		    if (zhi[himin] < 600) hplatr1a->Fill(xhi[himin]);
		    if (zhi[himin] > 605 && zhi[himin] < 610) hplatr1b->Fill(xhi[himin]);
		    if (zhi[himin] > 625 && zhi[himin] < 630 && yhi[himin] < 20) hplatr2ac->Fill(xhi[himin]);
		    if (zhi[himin] > 625 && zhi[himin] < 630 && yhi[himin] > 20) hplatr2au->Fill(xhi[himin]);
		    if (zhi[himin] > 635 && zhi[himin] < 660 && yhi[himin] < 20) hplatr2bc->Fill(xhi[himin]);
		    if (zhi[himin] > 635 && zhi[himin] < 660 && yhi[himin] > 20) hplatr2bu->Fill(xhi[himin]);
		    if (zhi[himin] > 625 && zhi[himin] < 660 && yhi[himin] < 20) hplatr2abc->Fill(xhi[himin]);

		    if (ic == crpc1)
		    {
			hplatramz->Fill(zrpcm);
			hplatram->Fill(xrpcm);
			if (zrpcm < 600) hplatram1a->Fill(xrpcm);
			if (zrpcm > 605 && zrpcm < 610) hplatram1b->Fill(xrpcm);
			if (zrpcm > 625 && zrpcm < 630 && yrpcm < 20) hplatram2ac->Fill(xrpcm);
			if (zrpcm > 625 && zrpcm < 630 && yrpcm > 20) hplatram2au->Fill(xrpcm);
			if (zrpcm > 635 && zrpcm < 660 && yrpcm < 20) hplatram2bc->Fill(xrpcm);
			if (zrpcm > 635 && zrpcm < 660 && yrpcm > 20) hplatram2bu->Fill(xrpcm);
			if (zrpcm > 625 && zrpcm < 660 && yrpcm < 20) hplatram2abc->Fill(xrpcm);
		    }
		    else if (ic == crpc2)
		    {
			hplatramz->Fill(zrpcm1);
			hplatram->Fill(xrpcm1);
			if (zrpcm1 < 600) hplatram1a->Fill(xrpcm1);
			if (zrpcm1 > 605 && zrpcm1 < 610) hplatram1b->Fill(xrpcm);
			if (zrpcm1 > 625 && zrpcm1 < 630 && yrpcm1 < 20) hplatram2ac->Fill(xrpcm);
			if (zrpcm1 > 625 && zrpcm1 < 630 && yrpcm1 > 20) hplatram2au->Fill(xrpcm);
			if (zrpcm1 > 635 && zrpcm1 < 660 && yrpcm1 < 20) hplatram2bc->Fill(xrpcm);
			if (zrpcm1 > 635 && zrpcm1 < 660 && yrpcm1 > 20) hplatram2bu->Fill(xrpcm);
			if (zrpcm1 > 625 && zrpcm1 < 660 && yrpcm1 < 20) hplatram2abc->Fill(xrpcm);
		    }
		    else
		    {
			hplatramz->Fill(zrpcm);
			hplatram->Fill(xrpcm);
			if (zrpcm < 600) hplatram1a->Fill(xrpcm);
			if (zrpcm > 605 && zrpcm < 610) hplatram1b->Fill(xrpcm);
			if (zrpcm > 625 && zrpcm < 630 && yrpcm < 20) hplatram2ac->Fill(xrpcm);
			if (zrpcm > 625 && zrpcm < 630 && yrpcm > 20) hplatram2au->Fill(xrpcm);
			if (zrpcm > 635 && zrpcm < 660 && yrpcm < 20) hplatram2bc->Fill(xrpcm);
			if (zrpcm > 635 && zrpcm < 660 && yrpcm > 20) hplatram2bu->Fill(xrpcm);
			if (zrpcm > 625 && zrpcm < 660 && yrpcm < 20) hplatram2abc->Fill(xrpcm);
		    }

		    htoftr->Fill(tof[ic][is]);
//
		    tofr = 0.f;
		    if (CALIBRATION == 0)
		    {
			tofr = tof[ic][is]+tofcalmc[ic];
		    }
		    else if (CALIBRATION == 1)
		    {
			tofr = tof[ic][is]+lenr/cpeak;
		    }
		    else if (CALIBRATION == 2)
		    {
			tofr = tof[ic][is]+tofcalmc[ic];
		    }
		    else if (CALIBRATION == 3)
		    {
			tofr = tof[ic][is]+tofcalmc[ic];
		    }
		    else
		    {
			continue;
		    }
		    tofr0 = tof[ic][is];
		    tofp = 0.f;
		    tofpi = 0.f;
		    beta = lenr/tofr/cvel;
// Selection criteria
		    if (beta <= 0.) continue;
		    mass2 = p*p*(1.f/beta/beta-1.f);
		    mass  = TMath::Sqrt(mass2);
		    hpbeta->Fill(fabs(p),beta);
		    hmass->Fill(mass2);
		    hmassvsp->Fill(fabs(p), mass2);

                    massmin1 = MP2 - MP2/(DMPSCALE1*PMAX1)*(p-0.5);     
                    massmax1 = MP2 + MP2/(DMPSCALE2*PMAX1)*(p-0.5);     
                    if (p > 0.f && mass2 > massmin1 && mass2 < massmax1 && pmod > PMIN && pmod < PMAX)
		    {
			tofp = (lenr/29.97925f)*TMath::Sqrt(MP2+p*p)/p;
			tcor = tofp - tofr;
			tcorc = tcor - tofcalc[ic];
			tcors = tcor - tofcals[ic][is];
			tcalibrc->Fill(ic,tcorc);
			tcalibrs->Fill(ic,is,tcors);
			htofp->Fill(tofr0);
                	massmin2 = MP2 - MP2/(DMPSCALET1*PMAX2)*(p-0.5);     
                	massmax2 = MP2 + MP2/(DMPSCALET2*PMAX2)*(p-0.5);     
                        if (mass2 > massmin2 && mass2 < massmax2)
			{
			    hdtpc->Fill(tcorc);
			    hdtpcvsp->Fill(p,tcorc);
			    hdtps->Fill(tcors);
			    hdtpsvsp->Fill(p,tcors);
			    hdtpsvscvss->Fill(ic,is,tcors);
			    hdtpcvscvsp->Fill(ic,p,tcors);
			    if (p > PRES) hdtpcplim->Fill(tcorc);
			    if (p > PRES1) hdtpcp1lim->Fill(tcorc);
			    if (p < 2.0f) hdtpcp2lim->Fill(tcorc);
			    if (p >= 2.0f && p < 5.0f) hdtpcp3lim->Fill(tcorc);
			    if (tofWidths[ic][is] > WMIN && tofWidths[ic][is] < WMAX)
			    {
				hdtpcwlim->Fill(tcorc);
				hdtpcvspwlim->Fill(p, tcorc);
				if (p > PRES) hdtpcwplim->Fill(tcorc);
				if (p > PRES1) hdtpcwp1lim->Fill(tcorc);
			    }
			}
		    }
//                    massmaxpi = 0.1 + MPIMAX*TMath::Abs(p-0.5)/PMAXPI/DMPISCALE; // Kapishin
                    massmaxpi = 0.1 + MPIMAX*TMath::Abs(pmod-0.5)/PMAXPI;
                    if ( p > 0 && massmaxpi > massmin1) massmaxpi = massmin1;
//                    massminpi =-0.1 - MPIMAX*TMath::Abs(p-0.5)/PMAXPI/DMPISCALE; // Kapishin
                    massminpi =-0.1 - MPIMAX*TMath::Abs(pmod-0.5)/PMAXPI;
                    if (mass2 < massmaxpi && mass2 > massminpi && pmod > PMIN && pmod < PMAXPI)
		    {
			tofpi = (lenr/29.97925f)*TMath::Sqrt(MPI2+p*p)/pmod;
			tcorpi = tofpi - tofr;
			tcorcpi = tcorpi - tofcalc[ic];
			tcorspi = tcorpi - tofcals[ic][is];
			tcalibrc->Fill(ic,tcorcpi);
			tcalibrs->Fill(ic,is,tcorspi);
		    }
		    timepeak = tofcalmc[ic] + tofcalc[ic];
		    betapeak = lenr/timepeak/cvel;
		    tpeak->Fill(ic, timepeak);
		    bpeak->Fill(ic, betapeak);
                    tofrc = tofr + tofcalc[ic];
                    tofrs = tofr + tofcals[ic][is];
                    betac = lenr/tofrc/cvel;
                    betas = lenr/tofrs/cvel;
		    masss2 = p*p*(1.f/betas/betas-1.f);
		    masss  = TMath::Sqrt(masss2);
		    massc2 = p*p*(1.f/betac/betac-1.f);
		    massc  = TMath::Sqrt(massc2);
		    tri = IdTrig[trackRunId];
		    if (tri == 20) tri = 40;
		    else if (tri == 41) tri = 41;
		    else if (tri == 42) tri = 42;
		    else if (tri == 46) tri = 43;
		    else if (tri == 44) tri = 0;
		    else if (tri == 45) tri = 0;
		    else if (tri == 46) tri = 0;
		    else if (tri == 47) tri = 44;
		    else if (tri == 48) tri = 0;
		    else if (tri == 49) tri = 45;
		    else tri = 0;
		    tar = Ntar[trackRunId];
		    if (p>0)
		    {
			hpbetac->Fill(p,betac);
			hmassc->Fill(massc2);
			hmasscvsp->Fill(p, massc2);
			hpbetas->Fill(p,betas);
			hmasss->Fill(masss2);
			hmasssvsp->Fill(p, masss2);
                        if (p<1.5) {
                    	    hmasss15->Fill(masss2);
                    	    if (Ntar[trackRunId] > 0 && tri >= 40) hmasss15tt->Fill(masss2,tar,tri-40);
                    	    hmassc15->Fill(massc2);
                        }
                        if (p<2.0) {
                    	    hmasss20->Fill(masss2);
                    	    if (Ntar[trackRunId] > 0 && tri >= 40) hmasss20tt->Fill(masss2,tar,tri-40);
                    	    hmassc20->Fill(massc2);
                        }
// Fragments
			nurun = trackRunId;
			nuev = trackEvId;
			m2fr = masss2;
			bfr = betas;
			efr = TMath::Sqrt(pfr*pfr+masss2);
			yfr = 0.5*TMath::Log((efr+pzfr)/(efr-pzfr));
			if (masss2 >= MFRMIN) tfr->Fill();
			Double_t pfrmax = PFRMAX;
			Int_t ftype = -1;
			if      (masss2<M1Z1MIN)                    { ftype = 0; pfrmax = PFRMAX; }
			else if (masss2>=M1Z1MIN && masss2<M1Z1MAX) { ftype = 1; pfrmax = PFRMAX; }
			else if (masss2>=M3Z2MIN && masss2<M3Z2MAX) { ftype = 2; pfrmax = PFRMAX+1.; }
			else if (masss2>=M2Z1MIN && masss2<M2Z1MAX) { ftype = 3; pfrmax = PFRMAX+2.; }
			else if (masss2>=M3Z1MIN && masss2<M3Z1MAX) { ftype = 4; pfrmax = PFRMAX+3.; }
			if (p<pfrmax) {
				if      (ftype == 0) { hhitdxvsp_pi->Fill(p, xrpc-xhi[himin]); hghitdxvsp_pi->Fill(p, xGem-xyzdch[0]);}
				else if (ftype == 1) { hhitdxvsp_p->Fill(p, xrpc-xhi[himin]); hghitdxvsp_p->Fill(p, xGem-xyzdch[0]); }
				else if (ftype == 3) { hhitdxvsp_d->Fill(p, xrpc-xhi[himin]); hghitdxvsp_d->Fill(p, xGem-xyzdch[0]); }
				hyp0->Fill(yfr,ptfr);
				hmfr0->Fill(masss2);
                    		if (Ntar[trackRunId] > 0 && tri >= 40) hmfr0tt->Fill(m2fr,tar,tri-40);
				if (ftype <= 0) continue;
				hmfrall->Fill(masss2);
                    		if (Ntar[trackRunId] > 0 && tri >= 40) hmfralltt->Fill(m2fr,tar,tri-40);
				hypall->Fill(yfr, ptfr);
				hypall1->Fill(yfr, ptfr);
				if      (ftype == 1) { hyp_p->Fill(yfr, ptfr);   hyp_p1->Fill(yfr, ptfr);   hmfr->Fill(masss2); }
				else if (ftype == 2) { hyp_he3->Fill(yfr, ptfr); hyp_he31->Fill(yfr, ptfr); hmfr->Fill(masss2); }
				else if (ftype == 3) { hyp_d->Fill(yfr, ptfr);   hyp_d1->Fill(yfr, ptfr);   hmfr->Fill(masss2); }
				else if (ftype == 4) { hyp_t->Fill(yfr, ptfr);   hyp_t1->Fill(yfr, ptfr);   hmfr->Fill(masss2); };
                    		if (ftype <= 4 && Ntar[trackRunId] > 0 && tri >= 40) hmfrtt->Fill(m2fr,tar,tri-40);
			}
//
		    }
		    else
		    {
			hpbetacm->Fill(fabs(p),betac);
			hmasscm->Fill(massc2);
			hmasscmvsp->Fill(fabs(p), massc2);
			hpbetasm->Fill(fabs(p),betas);
			hmasssm->Fill(masss2);
			hmasssmvsp->Fill(fabs(p),masss2);
		    }
		} // hit is good!
	    } // hit exist!
	} // loop on pairs main type (gem+dch_track+tof700_hit)

	int nwin1 = 0, nwin2 = 0, nwins = 0;
	Double_t dxr = 0., dyr = 0.;
	Double_t dxr0 = 0., dyr0 = 0.;
	Double_t dtxr = 0., dtyr = 0.;
	for (int itg = 0; itg<ntrgem; itg++)
	{
	    nwin1 = 0;
	    nwin2 = 0;
	    nwins = 0;
	    for (int itd = 0; itd<ntrdch; itd++)
	    {
		dxr = xtrgem[itg]-xtrdch[itd];
		dyr = ytrgem[itg]-ytrdch[itd];
		dxr0 = xtrgem0[itg]-xtrdch[itd];
//		dyr0 = ytrgem0[itg]-ytrdch[itd];
		dtxr = txtrgem[itg]-txtrdch[itd];
		dtyr = tytrgem[itg]-tytrdch[itd];
		if (pgem[itg]>0.)
		{
		    hghitdxvspa->Fill(pgem[itg], dxr);
		    hghitdxvspar->Fill(pgem[itg], dxr0);
		    hghitdyvspa->Fill(pgem[itg], dyr);
		    hgdtx1vspall->Fill(pgem[itg], (xtrgem[itg]-xtrdch[itd])/(ztrgem[itg]-ztrgem1st[itg]));
		}
		else if (pgem[itg]<0.)
		{
		    hghitdxvspan->Fill(-pgem[itg], dxr);
		    hghitdxvsparn->Fill(-pgem[itg], dxr0);
		    hgdtx1vspnall->Fill(-pgem[itg], (xtrgem[itg]-xtrdch[itd])/(ztrgem[itg]-ztrgem1st[itg]));
		}
		hdxyrealall->Fill(dxr, dyr);
		hdtxtyrealall->Fill(dtxr, dtyr);
	        if (dxr >= -DXGEMDCHMAX && dxr <= +DXGEMDCHMAX && dyr >= -DYGEMDCHMAX && dyr <= +DYGEMDCHMAX/* && Chi2m <= 30.f && chi2 > 30.f*/)
	        {
		    hdtxtyrealallw->Fill(dtxr, dtyr);
		    if (pgem[itg]>0.)
		    {
			hghitdxvspw1->Fill(pgem[itg], dxr);
			hghitdyvspw1->Fill(pgem[itg], dyr);
		    }
		    else if (pgem[itg]<0.)
		    {
			hghitdxvspw1n->Fill(-pgem[itg], dxr);
		    }
		    nwin1++;
		}
	        if (dxr0 >= -DXGEMDCHMAX && dxr0 <= +DXGEMDCHMAX && dyr >= -DYGEMDCHMAX && dyr <= +DYGEMDCHMAX/* && Chi2m <= 30.f && chi2 > 30.f*/)
	        {
		    if (pgem[itg]>0.)
		    {
			hghitdxvspw1r->Fill(pgem[itg], dxr0);
		    }
		    else if (pgem[itg]<0.)
		    {
			hghitdxvspw1rn->Fill(-pgem[itg], dxr);
		    }
		}
	        if (dxr >= -2.*DXGEMDCHMAX && dxr <= +2.*DXGEMDCHMAX && dyr >= -2.*DYGEMDCHMAX && dyr <= +2.*DYGEMDCHMAX/* && Chi2m <= 30.f && chi2 > 30.f*/)
	        {
		    if (pgem[itg]>0.)
		    {
			hghitdxvspw2->Fill(pgem[itg], dxr);
			hghitdyvspw2->Fill(pgem[itg], dyr);
		    }
		    else if (pgem[itg]<0.)
		    {
			hghitdxvspw2n->Fill(-pgem[itg], dxr);
		    }
		    nwin2++;
		}
	        if (dxr0 >= -2.*DXGEMDCHMAX && dxr0 <= +2.*DXGEMDCHMAX && dyr >= -2.*DYGEMDCHMAX && dyr <= +2.*DYGEMDCHMAX/* && Chi2m <= 30.f && chi2 > 30.f*/)
	        {
		    if (pgem[itg]>0.)
		    {
			hghitdxvspw2r->Fill(pgem[itg], dxr0);
			hgdtxvspall->Fill(pgem[itg], dtxr);
			if (pgem[itg]<1.5) hgdtxvsdxall->Fill(dxr0,dtxr);
		    }
		    else if (pgem[itg]<0.)
		    {
			hghitdxvspw2rn->Fill(-pgem[itg], dxr0);
			hgdtxvspnall->Fill(-pgem[itg], dtxr);
		    }
		}
		if (NDXsigma > 0 && dxtrgem[itg] > 0. && dytrgem[itg] > 0.)
		{
	    	    if (fabs(dxr) <= NDXsigma*dxtrgem[itg] && fabs(dyr) <= NDXsigma*dytrgem[itg])
	    	    {
			if (pgem[itg]>0.)
			{
			    hghitdxvspw3->Fill(pgem[itg], dxr);
			    hghitdyvspw3->Fill(pgem[itg], dyr);
			}
			else if (pgem[itg]<0.)
			{
			    hghitdxvspw3n->Fill(-pgem[itg], dxr);
			}
			nwins++;
		    }
		}
		if (NDXsigma > 0 && dxtrgem[itg] > 0. && dytrgem[itg] > 0.)
		{
	    	    if (fabs(dxr0) <= NDXsigma*dxtrgem[itg] && fabs(dyr) <= NDXsigma*dytrgem[itg])
	    	    {
			if (pgem[itg]>0.)
			{
			    hghitdxvspw3r->Fill(pgem[itg], dxr0);
			}
			else if (pgem[itg]<0.)
			{
			    hghitdxvspw3rn->Fill(-pgem[itg], dxr);
			}
		    }
		}
	    }
	    hnwin1->Fill(nwin1);
	    hnwin2->Fill(nwin2);
	    hnwins->Fill(nwins);
	}

	for (int ict = 0; ict<NUMBER_CHAMBERS; ict++)
	{
	    if (ncl[ict] <= 0) continue;
	    if (idchambers[ict] < 100.) hncltrvschambs->Fill(idchambers[ict]!=19.3?(Int_t)idchambers[ict]:49, ncltr[ict]);
	    else                        hncltrvschambb->Fill((Int_t)idchambers[ict]-100, ncltr[ict]);
	    if (idchambers[ict] < 100.) hncltrvschambs1->Fill(idchambers[ict]!=19.3?(Int_t)idchambers[ict]:49, ncltr[ict]);
	    else                        hncltrvschambb1->Fill((Int_t)idchambers[ict]-100, ncltr[ict]);
	}
	hntr1->Fill(ntr1);
	hncltr->Fill(ntr0);
	Ntr1+=ntr1;
	Ntr1g+=ntr1g;
	Ntr2g+=ntr2g;
	hntr0->Fill(ntr0);
	Ntr0+=ntr0;
	Ntr02+=ntr02;
        //t_out->Fill();
	hntrdch0->Fill(ntrdch0);
	hntrdch->Fill(ntrdch);
	hntrgem0->Fill(ntrgem0);
	hntrgemcut->Fill(ntrgemcut);
	hntrgem->Fill(ntrgem);
	hntrgemdch->Fill(ntrgem, ntrdch);

        if (iEv%50000 == 0)
        {
	    hdxyeff->Divide(hdxytof,hdxy);
	    hdxeff->Divide((projtmp = hdxytof->ProjectionX()),(projtmp1 = hdxy->ProjectionX()));
	    if (projtmp) delete projtmp;
	    if (projtmp1) delete projtmp1;
	    hdyeff->Divide((projtmp = hdxytof->ProjectionY()),(projtmp1 = hdxy->ProjectionY()));
	    if (projtmp) delete projtmp;
	    if (projtmp1) delete projtmp1;
	    hdxyeff2->Divide(hdxytof2,hdxy2);
	    hxyeff2->Divide(hxygoodhitgem2,hxygoodtrgem2);
	    hxyeff->Divide(hxygoodhit,hxygoodtr);
	    hxyeffp->Divide(hxygoodhitp,hxygoodtrp);
	    hxyeffn->Divide(hxygoodhitn,hxygoodtrn);
	    hxeff->Divide((projtmp = hxygoodhit->ProjectionX()),(projtmp1 = hxygoodtr->ProjectionX()));
	    if (projtmp) delete projtmp;
	    if (projtmp1) delete projtmp1;
	    hyeff->Divide((projtmp = hxygoodhit->ProjectionY()),(projtmp1 = hxygoodtr->ProjectionY()));
	    if (projtmp) delete projtmp;
	    if (projtmp1) delete projtmp1;
	    hpeff->Divide(hpgoodhit,hpgemdch);
	    hpeffmg->Divide(hpgemdch,hptr);
	    hpeff2->Divide(hpgoodhit2,hpgemdch2);
	    hpeff2gt->Divide(hpgoodhit2,hpgemtof);
	    hpeffmg2->Divide(hpgemdch2,hpgemtof);
	    hpeffmg2f->Divide(hpgemdch2,hptr);
	    hpeffgemtof->Divide(hpgemtof,hptr);
	    hxyeffmg->Divide(hxygoodtrgem,hxygem);
	    hxyeffmg2->Divide(hxygoodtrgem2,hxygoodhit2);
	    hxyeffmg2f->Divide(hxygoodtrgem2,hxygem);
	    hxyefftrgem0->Divide(hxygoodtrgem0,hxygem0);
	    hxyefftrgem0p->Divide(hxygoodtrgem0p,hxygem0p);
	    hxyefftrgem0n->Divide(hxygoodtrgem0n,hxygem0n);
	    hxyeffmd->Divide(hxygoodtr,hxydch);
	    hxyeffdg->Divide(hxydch,hxygem);
	    hxyeffgd->Divide(hxygem,hxydch);
	    hxyeffm1d->Divide(hxygoodhit1,hxydch1);
	    hxyeffgemtof->Divide(hxygoodhit2,hxygem);
	    hxyeffgemtofdchtof1->Divide(hxygemtofdchtof1,hxygemtofdchtof);
	    hxyeffgemtofdchtof2->Divide(hxygemtofdchtof2,hxygemtofdchtof);
	    hxyeffgemtofdchtofd1->Divide(hxygemtofdchtofd1,hxygemtofdchtofd);
	    hxyeffgemtofdchtofd2->Divide(hxygemtofdchtofd2,hxygemtofdchtofd);
	    hpeffgemtofdchtof1->Divide(hpgemtofdchtof1,hpgemtofdchtof);
	    hpeffgemtofdchtof2->Divide(hpgemtofdchtof2,hpgemtofdchtof);
	    hxybadhit->Add(hxygoodtr,hxygoodhit,1.,-1.);
	    hxybadhita->Add(hxygoodtr,hxygoodhita,1.,-1.);
	    hpbadhit->Add(hpgemdch,hpgoodhit,1.,-1.);
//	    hnclchambeffs->Divide(hncltrvschambs,hnclvschambs);
	    hchambeffs->Divide(hncltrvschambs1,hnclvschambs1);
//	    hnclchambeffb->Divide(hncltrvschambb,hnclvschambb);
	    hchambeffb->Divide(hncltrvschambb1,hnclvschambb1);
	    hxyclustereff->Divide(hxygoodcluster,hxycluster);
	    hxyclustereffgem->Divide(hxygoodclustergem,hxycluster);
	    hxyclustereffdch->Divide(hxygoodclusterdch,hxycluster);
	    hxystripseff->Divide(hxygoodstrips,hxystrips);
	    hxytrackstripseff->Divide(hxygoodstrips,hxytrackstrips);
	    hxytrackstripseff1->Divide(hxygood1strips,hxytrackstrips);

	    hplaeffz->Divide(hplatrz, hplaz); 
            hplaeff->Divide(hplatr, hpla);
            hplaeff1a->Divide(hplatr1a, hpla1a);
            hplaeff1b->Divide(hplatr1b, hpla1b);
            hplaeff2ac->Divide(hplatr2ac, hpla2ac);
            hplaeff2au->Divide(hplatr2au, hpla2au);
            hplaeff2bc->Divide(hplatr2bc, hpla2bc);
            hplaeff2bu->Divide(hplatr2bu, hpla2bu);
            hplaeff2abc->Divide(hplatr2abc, hpla2abc);

	    hplaeffaz->Divide(hplatramz, hplatraz); 
            hplaeffa->Divide(hplatram, hplatra);
            hplaeffa1a->Divide(hplatram1a, hplatra1a);
            hplaeffa1b->Divide(hplatram1b, hplatra1b);
            hplaeffa2ac->Divide(hplatram2ac, hplatra2ac);
            hplaeffa2au->Divide(hplatram2au, hplatra2au);
            hplaeffa2bc->Divide(hplatram2bc, hplatra2bc);
            hplaeffa2bu->Divide(hplatram2bu, hplatra2bu);
            hplaeffa2abc->Divide(hplatram2abc, hplatra2abc);

	    if (FLAG < 0)
	    {
		printf("Number of tracks %d, per event %f\n", Ntr, (float)Ntr/nEvents);
		printf("GEM+DCH1+DCH2 tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, (float)Ntr1/Ntr*100.f);
		printf("Good GEM+DCH1+DCH2 tracks %d, per event %f (%f%% per track)\n", Ntr0, (float)Ntr0/nEvents, (float)Ntr0/Ntr*100.f);
	    }
	    else if (FLAG > 0)
	    {
		printf("Number of tracks %d,  per event %f\n", Ntr, (float)Ntr/nEvents);
		printf("GEM+DCH1 tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, (float)Ntr1/Ntr*100.f);
		printf("Good GEM+DCH1 tracks %d,  per event %f (%f%% per track)\n", Ntr0, (float)Ntr0/nEvents, (float)Ntr0/Ntr*100.f);
	    }
	    else
	    {
		printf("Number of tracks %d, per event %f\n", Ntr, (float)Ntr/nEvents);
		printf("GEM+DCH tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, Ntr>0?(float)Ntr1/Ntr*100.f:0.f);
		printf("Good GEM+DCH tracks %d, per event %f (%f%% per valid track)\n", Ntr1g, (float)Ntr1g/nEvents, Ntr1>0?(float)Ntr1g/Ntr1*100.f:0.f);
		printf("Matched GEM+DCH tracks %d,  per event %f (%f%% per good track)\n", Ntr0, (float)Ntr0/nEvents, Ntr1g>0?(float)Ntr0/Ntr1g*100.f:0.f);
		printf("Good GEMTOF+DCH tracks %d, per event %f (%f%% per valid track)\n", Ntr2g, (float)Ntr2g/nEvents, Ntr1>0?(float)Ntr2g/Ntr1*100.f:0.f);
		printf("Matched GEMTOF+DCH tracks %d,  per event %f (%f%% per good track)\n", Ntr02, (float)Ntr02/nEvents, Ntr2g>0?(float)Ntr02/Ntr2g*100.f:0.f);
	    }

	    if (f) {tfr->AutoSave("FlushBaskets"); f->Write(NULL,TObject::kOverwrite);}

	    if (WRITE_CALIBRATION)
	    {
		char fname_calibr_root[128], *delim = 0;
		strcpy(fname_calibr_root, gSystem->BaseName(fnametof700));
		if ((delim = strrchr(fname_calibr_root, (int)'.'))) *delim = '\0';
		strcat(fname_calibr_root, "_calibration.root");
		TFile *fc = new TFile(fname_calibr_root,"RECREATE","Proton mass based calibration of BmnTOF700");
		tcalibrs->Write();
		tcalibrc->Write();
		fc->Close();
	    }
	}
    } // event loop

    hdxyeff->Divide(hdxytof,hdxy);
    hdxeff->Divide((projtmp = hdxytof->ProjectionX()),(projtmp1 = hdxy->ProjectionX()));
    if (projtmp) delete projtmp;
    if (projtmp1) delete projtmp1;
    hdyeff->Divide((projtmp = hdxytof->ProjectionY()),(projtmp1 = hdxy->ProjectionY()));
    if (projtmp) delete projtmp;
    if (projtmp1) delete projtmp1;
    hdxyeff2->Divide(hdxytof2,hdxy2);
    hxyeff2->Divide(hxygoodhitgem2,hxygoodtrgem2);
    hxyeff->Divide(hxygoodhit,hxygoodtr);
    hxyeffp->Divide(hxygoodhitp,hxygoodtrp);
    hxyeffn->Divide(hxygoodhitn,hxygoodtrn);
    hxeff->Divide((projtmp = hxygoodhit->ProjectionX()),(projtmp1 = hxygoodtr->ProjectionX()));
    if (projtmp) delete projtmp;
    if (projtmp1) delete projtmp1;
    hyeff->Divide((projtmp = hxygoodhit->ProjectionY()),(projtmp1 = hxygoodtr->ProjectionY()));
    if (projtmp) delete projtmp;
    if (projtmp1) delete projtmp1;
    hpeff->Divide(hpgoodhit,hpgemdch);
    hpeffmg->Divide(hpgemdch,hptr);
    hpeff2->Divide(hpgoodhit2,hpgemdch2);
    hpeff2gt->Divide(hpgoodhit2,hpgemtof);
    hpeffmg2->Divide(hpgemdch2,hpgemtof);
    hpeffmg2f->Divide(hpgemdch2,hptr);
    hpeffgemtof->Divide(hpgemtof,hptr);
    hxyeffmg->Divide(hxygoodtrgem,hxygem);
    hxyeffmg2->Divide(hxygoodtrgem2,hxygoodhit2);
    hxyeffmg2f->Divide(hxygoodtrgem2,hxygem);
    hxyefftrgem0->Divide(hxygoodtrgem0,hxygem0);
    hxyefftrgem0p->Divide(hxygoodtrgem0p,hxygem0p);
    hxyefftrgem0n->Divide(hxygoodtrgem0n,hxygem0n);
    hxyeffmd->Divide(hxygoodtr,hxydch);
    hxyeffdg->Divide(hxydch,hxygem);
    hxyeffgd->Divide(hxygem,hxydch);
    hxyeffm1d->Divide(hxygoodhit1,hxydch1);
    hxyeffgemtof->Divide(hxygoodhit2,hxygem);
    hxyeffgemtofdchtof1->Divide(hxygemtofdchtof1,hxygemtofdchtof);
    hxyeffgemtofdchtof2->Divide(hxygemtofdchtof2,hxygemtofdchtof);
    hxyeffgemtofdchtofd1->Divide(hxygemtofdchtofd1,hxygemtofdchtofd);
    hxyeffgemtofdchtofd2->Divide(hxygemtofdchtofd2,hxygemtofdchtofd);
    hpeffgemtofdchtof1->Divide(hpgemtofdchtof1,hpgemtofdchtof);
    hpeffgemtofdchtof2->Divide(hpgemtofdchtof2,hpgemtofdchtof);
    hxybadhit->Add(hxygoodtr,hxygoodhit,1.,-1.);
    hxybadhita->Add(hxygoodtr,hxygoodhita,1.,-1.);
    hpbadhit->Add(hpgemdch,hpgoodhit,1.,-1.);
//    hnclchambeffs->Divide(hncltrvschambs,hnclvschambs);
    hchambeffs->Divide(hncltrvschambs1,hnclvschambs1);
//    hnclchambeffb->Divide(hncltrvschambb,hnclvschambb);
    hchambeffb->Divide(hncltrvschambb1,hnclvschambb1);
    hxyclustereff->Divide(hxygoodcluster,hxycluster);
    hxyclustereffgem->Divide(hxygoodclustergem,hxycluster);
    hxyclustereffdch->Divide(hxygoodclusterdch,hxycluster);
    hxystripseff->Divide(hxygoodstrips,hxystrips);
    hxytrackstripseff->Divide(hxygoodstrips,hxytrackstrips);
    hxytrackstripseff1->Divide(hxygood1strips,hxytrackstrips);

    hplaeffz->Divide(hplatrz, hplaz); 
    hplaeff->Divide(hplatr, hpla);
    hplaeff1a->Divide(hplatr1a, hpla1a);
    hplaeff1b->Divide(hplatr1b, hpla1b);
    hplaeff2ac->Divide(hplatr2ac, hpla2ac);
    hplaeff2au->Divide(hplatr2au, hpla2au);
    hplaeff2bc->Divide(hplatr2bc, hpla2bc);
    hplaeff2bu->Divide(hplatr2bu, hpla2bu);
    hplaeff2abc->Divide(hplatr2abc, hpla2abc);

    hplaeffaz->Divide(hplatramz, hplatraz); 
    hplaeffa->Divide(hplatram, hplatra);
    hplaeffa1a->Divide(hplatram1a, hplatra1a);
    hplaeffa1b->Divide(hplatram1b, hplatra1b);
    hplaeffa2ac->Divide(hplatram2ac, hplatra2ac);
    hplaeffa2au->Divide(hplatram2au, hplatra2au);
    hplaeffa2bc->Divide(hplatram2bc, hplatra2bc);
    hplaeffa2bu->Divide(hplatram2bu, hplatra2bu);
    hplaeffa2abc->Divide(hplatram2abc, hplatra2abc);

    if (FLAG < 0)
    {
	printf("Number of tracks %d, per event %f\n", Ntr, (float)Ntr/nEvents);
	printf("GEM+DCH1+DCH2 tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, (float)Ntr1/Ntr*100.f);
	printf("Good GEM+DCH1+DCH2 tracks %d, per event %f (%f%% per track)\n", Ntr0, (float)Ntr0/nEvents, (float)Ntr0/Ntr*100.f);
    }
    else if (FLAG > 0)
    {
	printf("Number of tracks %d,  per event %f\n", Ntr, (float)Ntr/nEvents);
	printf("GEM+DCH1 tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, (float)Ntr1/Ntr*100.f);
	printf("Good GEM+DCH1 tracks %d,  per event %f (%f%% per track)\n", Ntr0, (float)Ntr0/nEvents, (float)Ntr0/Ntr*100.f);
    }
    else
    {
	printf("Number of tracks %d, per event %f\n", Ntr, (float)Ntr/nEvents);
	printf("GEM+DCH tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, Ntr>0?(float)Ntr1/Ntr*100.f:0.f);
	printf("Good GEM+DCH tracks %d, per event %f (%f%% per valid track)\n", Ntr1g, (float)Ntr1g/nEvents, Ntr1>0?(float)Ntr1g/Ntr1*100.f:0.f);
	printf("Matched GEM+DCH tracks %d,  per event %f (%f%% per good track)\n", Ntr0, (float)Ntr0/nEvents, Ntr1g>0?(float)Ntr0/Ntr1g*100.f:0.f);
	printf("Good GEMTOF+DCH tracks %d, per event %f (%f%% per valid track)\n", Ntr2g, (float)Ntr2g/nEvents, Ntr1>0?(float)Ntr2g/Ntr1*100.f:0.f);
	printf("Matched GEMTOF+DCH tracks %d,  per event %f (%f%% per good track)\n", Ntr02, (float)Ntr02/nEvents, Ntr2g>0?(float)Ntr02/Ntr2g*100.f:0.f);
    }

    if (f) {tfr->AutoSave("FlushBaskets"); f->Write(NULL, TObject::kOverwrite);}

    if (WRITE_CALIBRATION)
    {
	char fname_calibr_root[128], *delim = 0;
	strcpy(fname_calibr_root, gSystem->BaseName(fnametof700));
	if ((delim = strrchr(fname_calibr_root, (int)'.'))) *delim = '\0';
	strcat(fname_calibr_root, "_calibration.root");
	TFile *fc = new TFile(fname_calibr_root,"RECREATE","Proton mass based calibration of BmnTOF700");
	tcalibrs->Write();
	tcalibrc->Write();
	fc->Close();
    }

    return;
}

void select_hist()
{
  //example of macro called when a mouse event occurs in a pad.
  // Example:
  // Root > TFile f("hsimple.root");
  // Root > hpxpy.Draw();
  // Root > c1.AddExec("exsel","select_hist()");
  // When moving the mouse in the canvas, a second canvas shows the
  // projection along X of the bin corresponding to the Y position
  // of the mouse. The resulting histogram is fitted with a gaussian.
  // A "dynamic" line shows the current bin position in Y.
  // This more elaborated example can be used as a starting point
  // to develop more powerful interactive applications exploiting CINT
  // as a development engine.
  //Author: Rene Brun

  int event = gPad->GetEvent();
  if (event != 11) return;

  TObject *select = gPad->GetSelected();
  if(!select) return;
  if (!(select->InheritsFrom("TH1"))) return;

  //create or set the new canvas c2
  TCanvas *csingle = 0;
  if ((csingle = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("csingle")));
  else    csingle = new TCanvas("csingle");
  csingle->cd();

  //draw slice corresponding to mouse position
  if (!(select->InheritsFrom("TH1F")))
    {
      TH1F *h = (TH1F *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TH2F")))
    {
      TH2F *h = (TH2F *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TProfile")))
    {
      TProfile *h = (TProfile *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TH1D")))
    {
      TH1D *h = (TH1D *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TH2D")))
    {
      TH2D *h = (TH2D *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TProfile2D")))
    {
      TProfile2D *h = (TProfile2D *)select;
      h->Draw();
    }
  else return;
  csingle->Update();
}

int readMCTimeFile(const char *MCTimeFile)
{
//	cout << "readMCTimeFile: " << MCTimeFile << endl;
	float idchambers[59] = { 27.1,28.1,3.1,1.1,29.1,4.1,33.1,30.1,5.1,19.3,31.1,6.1,2.1,32.1,15.2,16.2,17.2,
	18.2,19.2,20.2,7.1,115.2,113.1,117.1,35.1,9.1,37.1,11.1,39.1,13.1,34.1,8.1,36.1,10.1,38.1,12.1,21.2,
	23.2,25.2,22.2,24.2,26.2,107.2,108.2,109.2,110.2,111.2,112.2,114.1,116.2,118.1,14.1,40.1,119.2,120.2,
	121.2,122.2,123.2,124.2 };
	bool notused[59] = {true};
	int order[59] = {-1};
	int c = 0, cmin = -1, c0 = 0;
	float idmin = 200.f;
	for (c0 = 0; c0 < 59; c0++) { notused[c0] = true; order[c0] = -1; }
	for (c0 = 0; c0 < 59; c0++)
	{
	    cmin = -1;
	    idmin = 200.f;
	    for (c=0; c<59; c++)
	    {
		if (notused[c] && (idchambers[c]<idmin))
		{
		    idmin = idchambers[c];
		    cmin = c;
		}
	    }
//	    printf("c0 %d cmin %d notused %d\n",c0,cmin,(int)notused[cmin]);
	    if (cmin >= 0)
	    {
		notused[cmin] = false;
		order[c0] = cmin;
	    }
	}
	char fname[128];
	FILE *ft = 0;
	float time = 0.f, timesigma = 0.f;
	if (MCTimeFile == NULL)
	{
	    printf("TOF700 MC time-of-flight file name not defined!\n");
	    return 0;
	}
	if (strlen(MCTimeFile) == 0)
	{
	    printf("TOF700 MC time-of-flight file name not defined!\n");
	    return 0;
	}
	TString dir = getenv("VMCWORKDIR");
	sprintf(fname,"%s/geometry/%s",dir.Data(),MCTimeFile);
	ft = fopen(fname,"r");
	if (ft == NULL)
	{
	    printf("TOF700 MC time-of-flight file %s open error!\n", fname);
	    return 0;
	}
	int cexp = 0, ic = -1;
	while(fscanf(ft,"%d %f %f\n", &ic, &time, &timesigma) == 3)
	{
		if (ic > 0 && ic <= 59)
		{
		    cexp = order[ic-1];
		    tofcalmc[cexp] = time;
		    if (1) printf("Chamber %.1f (MC %d, EXP %d) average time-of-flight %f with sigma %f\n",idchambers[cexp], ic, cexp, time, timesigma);
		}
	}
	fclose(ft);
	return 1;
}

void selecting_records_target()
{
    Int_t Nruns[6] = {0};
    TObjArray arrayConditions;
    UniSearchCondition* searchCondition = new UniSearchCondition(columnPeriodNumber, conditionEqual, 7);
    arrayConditions.Add((TObject*)searchCondition);
//    searchCondition = new UniSearchCondition(columnTargetParticle, conditionEqual, TString("Sn"));
//    arrayConditions.Add((TObject*)searchCondition);
    searchCondition = new UniSearchCondition(columnBeamParticle, conditionEqual, TString("Ar"));
    arrayConditions.Add((TObject*)searchCondition);
//    searchCondition = new UniSearchCondition(columnSp41, conditionGreaterOrEqual, 1200);
//    arrayConditions.Add((TObject*)searchCondition);
//    searchCondition = new UniSearchCondition(columnSp41, conditionLessOrEqual, 1300);
//    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* pRecordArray = ElogDbRecord::Search(arrayConditions);

    // clean memory for conditions after search
    arrayConditions.SetOwner(kTRUE);
    arrayConditions.Delete();

    // print run numbers and file path with raw data
    for (int i = 0; i < pRecordArray->GetEntriesFast(); i++)
    {
        ElogDbRecord* pRecord = (ElogDbRecord*) pRecordArray->At(i);
//        cout<<i<<"  Record: time - "<<pRecord->GetRecordDate().AsSQLString()<<", period - "<<(pRecord->GetPeriodNumber() == NULL ? 0 : *pRecord->GetPeriodNumber())
//            <<", run - "<<(pRecord->GetRunNumber() == NULL ? 0 : *pRecord->GetRunNumber())<<", SP-41 field - "<<(pRecord->GetSp41() == NULL ? 0 : *pRecord->GetSp41())<<"A"
//            << ", Target - " << (pRecord->GetTarget() == NULL ? "Beam" : *pRecord->GetTarget()) << ", Energy - "<< *pRecord->GetEnergy() << endl;
	if (pRecord->GetTarget() == NULL) continue;
	if ((pRecord->GetTarget() == NULL ? "Beam" : *pRecord->GetTarget()) == "C")  {/*printf(" Carbon\n");*/ Ntar[(pRecord->GetRunNumber() == NULL ? 0 : *pRecord->GetRunNumber())] = 1; Nruns[1]++;}
	if ((pRecord->GetTarget() == NULL ? "Beam" : *pRecord->GetTarget()) == "Al") {/*printf(" Carbon\n");*/ Ntar[(pRecord->GetRunNumber() == NULL ? 0 : *pRecord->GetRunNumber())] = 2; Nruns[2]++;}
	if ((pRecord->GetTarget() == NULL ? "Beam" : *pRecord->GetTarget()) == "Cu") {/*printf(" Carbon\n");*/ Ntar[(pRecord->GetRunNumber() == NULL ? 0 : *pRecord->GetRunNumber())] = 3; Nruns[3]++;}
	if ((pRecord->GetTarget() == NULL ? "Beam" : *pRecord->GetTarget()) == "Sn") {/*printf(" Carbon\n");*/ Ntar[(pRecord->GetRunNumber() == NULL ? 0 : *pRecord->GetRunNumber())] = 4; Nruns[4]++;}
	if ((pRecord->GetTarget() == NULL ? "Beam" : *pRecord->GetTarget()) == "Pb") {/*printf(" Carbon\n");*/ Ntar[(pRecord->GetRunNumber() == NULL ? 0 : *pRecord->GetRunNumber())] = 5; Nruns[5]++;}
	ElogDbTrigger *t = ElogDbTrigger::GetTrigger((pRecord->GetTriggerId() == NULL ? 0 : *pRecord->GetTriggerId()));
	TString ti = (t == NULL ? "NoInfo" : t->GetTriggerInfo());
	Int_t tid = (t == NULL ? 0 : t->GetTriggerId());
	IdTrig[(pRecord->GetRunNumber() == NULL ? 0 : *pRecord->GetRunNumber())] = tid;
	fSp41[(pRecord->GetRunNumber() == NULL ? 0 : *pRecord->GetRunNumber())] = ((pRecord->GetSp41() == NULL ? 0 : *pRecord->GetSp41()));
//	printf("Pb trigger %s\n",ti.Data());
//	printf("Run %d Target %s Trigger id %d info %s\n", (pRecord->GetRunNumber() == NULL ? 0 : *pRecord->GetRunNumber()), NameTar[Ntar[(pRecord->GetRunNumber() == NULL ? 0 : *pRecord->GetRunNumber())]].Data(), tid, ti.Data());
    }
    printf("\nNruns ");
    for (int it = 1; it<6; it++) printf(" %s %i", NameTar[it].Data(), Nruns[it]);
    printf("\n\n");

    // clean memory after work - deleting the array of runs
    delete pRecordArray;

//    cout << "\nMacro finished successfully" << endl;
}
#if XGEMCOR != 0
Double_t Xgemcorrection(Double_t p0, Double_t *txcor, Double_t *xsigma, Double_t *ysigma)
{
    Double_t p = p0;
    if (GemCorInit)
    {
	char fname_dt_root[128];
	strcpy(fname_dt_root, "dxgar_vs_p.root");
	TFile *fc = new TFile(fname_dt_root,"READ","DCH1 X residuals vs P");
	hfmp = (TH1D *)fc->Get("hghitdxvspw2r_1");
	hfsp = (TH1D *)fc->Get("hghitdxvspw2r_2");
	fmp = (TF1 *)fc->Get("fmp");
	fsp = (TF1 *)fc->Get("fsp");
	if (fmp == 0 || fsp == 0) { printf("\nError - no fmp and fsp functions!\n"); return -777.; }
	fc->Close();
	f->cd();

	strcpy(fname_dt_root, "dxgar_vs_pn.root");
	fc = new TFile(fname_dt_root,"READ","DCH1 X residuals vs P");
	hfmn = (TH1D *)fc->Get("hghitdxvspnw2rn_1");
	hfsn = (TH1D *)fc->Get("hghitdxvspnw2rn_2");
	fmn = (TF1 *)fc->Get("fmn");
	fsn = (TF1 *)fc->Get("fsn");
	if (fmn == 0 || fsn == 0) { printf("\nError - no fmn and fsn functions!\n"); return -777.; }
	fc->Close();
	f->cd();

	strcpy(fname_dt_root, "dyg_vs_p.root");
	fc = new TFile(fname_dt_root,"READ","DCH1 Y residuals vs P");
	hfmpy = (TH1D *)fc->Get("hghitdyvsp_1");
	hfspy = (TH1D *)fc->Get("hghitdyvsp_2");
	fmpy = (TF1 *)fc->Get("fmpy");
	fspy = (TF1 *)fc->Get("fspy");
	if (fmpy == 0 || fspy == 0) { printf("\nError - no fmpy and fspy functions!\n"); return -777.; }
	fc->Close();
	f->cd();

	strcpy(fname_dt_root, "dyg_vs_pn.root");
	fc = new TFile(fname_dt_root,"READ","DCH1 Y residuals vs P");
	hfmny = (TH1D *)fc->Get("hghitdyvspn_1");
	hfsny = (TH1D *)fc->Get("hghitdyvspn_2");
	fmny = (TF1 *)fc->Get("fmny");
	fsny = (TF1 *)fc->Get("fsny");
	if (fmny == 0 || fsny == 0) { printf("\nError - no fmny and fsny functions!\n"); return -777.; }
	fc->Close();
	f->cd();


	strcpy(fname_dt_root, "dtxg_vs_p.root");
	fc = new TFile(fname_dt_root,"READ","DCH1 TX residuals vs P");
	hftmp = (TH1D *)fc->Get("hgdtxvsp_1");
	hftsp = (TH1D *)fc->Get("hgdtxvsp_2");
	ftmp = (TF1 *)fc->Get("ftmp");
	ftsp = (TF1 *)fc->Get("ftsp");
	if (ftmp == 0 || ftsp == 0) { printf("\nError - no ftmp and ftsp functions!\n"); return -777.; }
	fc->Close();
	f->cd();

	strcpy(fname_dt_root, "dtxg_vs_pn.root");
	fc = new TFile(fname_dt_root,"READ","DCH1 Y residuals vs P");
	hftmn = (TH1D *)fc->Get("hgdtxvspn_1");
	hftsn = (TH1D *)fc->Get("hgdtxvspn_2");
	ftmn = (TF1 *)fc->Get("ftmn");
	ftsn = (TF1 *)fc->Get("ftsn");
	if (ftmn == 0 || ftsn == 0) { printf("\nError - no ftmn and ftsn functions!\n"); return -777.; }
	fc->Close();
	f->cd();

	GemCorInit = false;
    }
    if (p > 0.)
    {
	if (p < 0.7) p = 0.7;
	if (p > 5.0) p = 5.0;
	*txcor = 1.00*ftmp->Eval(p);
	p = p0;
	if (p < 0.7) p = 0.7;
	if (p > 5.0) p = 5.0;
	*ysigma = fspy->Eval(p);
	p = p0;
	if (p < 0.6) p = 0.6;
	if (p > 5.0) p = 5.0;
	*xsigma = fsp->Eval(p);
	p = p0;
	if (p < 0.6) p = 0.6;
	if (p > 5.0) p = 5.0;
	return 1.00*(fmp->Eval(p));
    }
    else if (p < 0.)
    {
	p = -p;
	if (p < 0.6) p = 0.6;
	if (p > 4.0) p = 4.0;
	*txcor = 1.00*ftmn->Eval(p);
	p = -p0;
	if (p < 0.8) p = 0.8;
	if (p > 4.0) p = 4.0;
	*ysigma = fsny->Eval(p);
	p = -p0;
	if (p < 0.8) p = 0.8;
	if (p > 4.7) p = 4.7;
	*xsigma = fsn->Eval(p);
	p = -p0;
	if (p < 0.8) p = 0.8;
	if (p > 3.5) p = 3.5;
	return 1.00*(fmn->Eval(p));
    }
    return 0.;
}
#endif

#if XTOFCOR != 0
Double_t Xgemcorrectiontof700(Double_t p0, Double_t *xsigmatof700, Double_t *ysigmatof700)
{
    Double_t p = p0;
    if (GemCorInittof700)
    {
        char fnametof700_dt_root[128];
        strcpy(fnametof700_dt_root, "Dx_vs_p_TOF700_data.root");
        TFile *fctof700 = new TFile(fnametof700_dt_root, "READ", "DCH1 X residuals vs P");
        hfmptof700 = (TH1D *)fctof700->Get("FitsliceMean");
        hfsptof700 = (TH1D *)fctof700->Get("FitsliceSigma");
        fmptof700 = (TF1 *)fctof700->Get("f3");
        fsptof700 = (TF1 *)fctof700->Get("f4");
        if (fmptof700 == 0 || fsptof700 == 0)
        {
            printf("\nError - no f3 and f4 functions!\n");
            return -777.;
        }
        fctof700->Close();

        strcpy(fnametof700_dt_root, "Dx_vs_p_TOF700_data_minus.root");
        fctof700 = new TFile(fnametof700_dt_root, "READ", "DCH1 X residuals vs P");
        hfmntof700 = (TH1D *)fctof700->Get("FitsliceMean");
        hfsntof700 = (TH1D *)fctof700->Get("FitsliceSigma");
        fmntof700 = (TF1 *)fctof700->Get("f3n");
        fsntof700 = (TF1 *)fctof700->Get("f4n");
        if (fmntof700 == 0 || fsntof700 == 0)
        {
            printf("\nError - no f3n and f4n functions!\n");
            return -777.;
        }
        fctof700->Close();

        strcpy(fnametof700_dt_root, "Dy_vs_p_TOF700_data.root");
        fctof700 = new TFile(fnametof700_dt_root, "READ", "DCH1 Y residuals vs P");
        hfmpytof700 = (TH1D *)fctof700->Get("FitsliceMean");
        hfspytof700 = (TH1D *)fctof700->Get("FitsliceSigma");
        fmpytof700 = (TF1 *)fctof700->Get("f3y");
        fspytof700 = (TF1 *)fctof700->Get("f4y");
        if (fmpytof700 == 0 || fspytof700 == 0)
        {
            printf("\nError - no f3y and f4y functions!\n");
            return -777.;
        }
        fctof700->Close();

        strcpy(fnametof700_dt_root, "Dy_vs_p_TOF700_data_minus.root");
        fctof700 = new TFile(fnametof700_dt_root, "READ", "DCH1 Y residuals vs P");
        hfmnytof700 = (TH1D *)fctof700->Get("FitsliceMean");
        hfsnytof700 = (TH1D *)fctof700->Get("FitsliceSigma");
        fmnytof700 = (TF1 *)fctof700->Get("f3yn");
        fsnytof700 = (TF1 *)fctof700->Get("f4yn");
        if (fmnytof700 == 0 || fsnytof700 == 0)
        {
            printf("\nError - no fmny and fsny functions!\n");
            return -777.;
        }
        fctof700->Close();

        GemCorInittof700 = false;
    }
    if (p > 0.)
    {
           if (p < 0.65) p = 0.65;
           if (p > 5.0) p = 5.0;
        *ysigmatof700 = fspytof700->Eval(p);
        p = p0;
           if (p < 0.65) p = 0.65;
           if (p > 5.0) p = 5.0;
        *xsigmatof700 = fsptof700->Eval(p);
        p = p0;
             if (p < 0.65) p = 0.65;
             if (p > 5.0) p = 5.0;
        return 1.00 * (fmptof700->Eval(p));
    }
   else if (p < 0.)
    {
      //  p = -p;
        if (p > -0.65)
            p = -0.65;
        if (p < -2.5)
            p = -2.5;
        *ysigmatof700 = fsnytof700->Eval(p);
    //    p = -p0;
        if (p > -0.65)
            p = -0.65;
        if (p < -2.5)
            p = -2.5;
        *xsigmatof700 = fsntof700->Eval(p);
     //   p = -p0;
        if (p > -0.65)
            p = -0.65;
        if (p < -2.5)
            p = -2.5;
        return 1.00 * (fmntof700->Eval(p));
    }
    return 0.;
}
#endif

void xystrip(TH2D *h, Int_t i, Int_t j)
{
					Int_t ich, ichx, ibx, iby;
					if ((ich = (int)idchambers[i])<=40)
					{
					    ichx = ich;
					    if (ichx>20) ichx += 2;
					    ibx = (ichx-1)%14 + 3;
					    iby = 2 - (ichx-1)/14;
					    iby = 16 + iby*32 + j;
					    h->Fill(ibx,iby);
					}
					else
					{
					    if (ich <= 112)
					    {
						iby = 112 + j;
						ibx = (ich - 107)*3 + 1;
						h->Fill(ibx,iby);
						h->Fill(ibx+1,iby);
						h->Fill(ibx+2,iby);
					    }
					    else if (ich >= 119)
					    {
						iby = j;
						ibx = (ich - 119)*3 + 1;
						h->Fill(ibx,iby);
						h->Fill(ibx+1,iby);
						h->Fill(ibx+2,iby);
					    }
					    else if (ich == 113 || ich == 115 || ich == 117)
					    {
						iby = 2 - (ich - 113)/2;
						iby = 24 + iby*32 + j;
						ibx = 0;
						h->Fill(ibx,iby);
						h->Fill(ibx+1,iby);
						h->Fill(ibx+2,iby);
					    }
					    else if (ich == 114 || ich == 116 || ich == 118)
					    {
						iby = 2 - (ich - 114)/2;
						iby = 24 + iby*32 + j;
						ibx = 17;
						h->Fill(ibx,iby);
						h->Fill(ibx+1,iby);
						h->Fill(ibx+2,iby);
					    }
					}
}
