/*
 * Macro:
 *
 *
 * Author: D. Baranov
 * Created: 18.07.2024
 */

#include "Rtypes.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)

//headers used within the code
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2D.h"


using namespace std;

//declarations
void ShowSignalInLayer(BmnVSPModule *module, BmnVSPLayer &layer, Int_t ilayer, Int_t imodule, Int_t istation);
void ShowStripNumbersOfClustersInModule(BmnVSPModule *module, Int_t layer_type, Int_t module_num, Int_t station_num);
void ShowStripNumbersOfClustersInModule_FromBranches(Int_t event_num, Int_t module_num, Int_t station_num);
void ShowReadoutPlane(BmnVSPModule *module, Int_t imodule, Int_t istation);
void PaintBin(TCanvas *c, TH1F *h, Int_t bin, Double_t bin_height, Int_t fcolor, Int_t bcolor);
void DrawMCMatches(TCanvas *c, TH1F *h, Int_t strip_num, BmnVSPLayer &layer);
void DrawStripHits(TCanvas *c, TH1F *h, BmnVSPLayer &layer);
void DrawMCStripLines(TCanvas *c, TH1F *h, BmnVSPModule *module, Int_t layer_num);
void LoadDataFromMCPoints(BmnVSPStationSet *station_set, BmnVSPTransform *transf_set,  Int_t event_num);
void LoadDataFromDigits(BmnVSPStationSet *station_set, BmnVSPTransform *transf_set,  Int_t event_num);

void VSPClusterAnalyser() {

    //Using OpenGL that is needed for transparency (the common strip of two clusters)
    gStyle->SetCanvasPreferGL(1);

    //--------------------------------------------------------------------------

    //Loading the configuration
    TString gPathDetectorConfig = gSystem->Getenv("VMCWORKDIR");
    gPathDetectorConfig += "/parameters/vsp/XMLConfigs/";
    gPathDetectorConfig += "VSP_Run9.xml";

    Int_t EventNum = 0;
    Int_t ViewStationNum = 0;
    Int_t ViewModuleNum = 3;

    BmnVSPStationSet *StationSet = new BmnVSPStationSet(gPathDetectorConfig);

    BmnVSPTransform * TransfSet = new BmnVSPTransform();
    TransfSet->LoadFromXMLFile(gPathDetectorConfig);


    //Load data from MC-points and add them to the detector
    LoadDataFromMCPoints(StationSet, TransfSet, EventNum);

    //Load data from digits
    //LoadDataFromDigits(StationSet, TransfSet, EventNum);

    //Data reconstruction
    StationSet->ProcessPointsInDetector();

    BmnVSPStation *Station = StationSet->GetStation(ViewStationNum);
    BmnVSPModule *Module = Station->GetModule(ViewModuleNum);
    vector<BmnVSPLayer> Layers = Module->GetStripLayers();

    for(Int_t ilayer = 0; ilayer < Layers.size(); ++ilayer) {
        BmnVSPLayer layer = Layers[ilayer];
        ShowSignalInLayer(Module, layer, ilayer, ViewModuleNum, ViewStationNum);
    }

    ShowReadoutPlane(Module, ViewModuleNum, ViewStationNum);

    ShowStripNumbersOfClustersInModule(Module, 0/*lower || layer*/, ViewModuleNum, ViewStationNum);
    ShowStripNumbersOfClustersInModule(Module, 1/*upper // layer*/, ViewModuleNum, ViewStationNum);

    ShowStripNumbersOfClustersInModule_FromBranches(EventNum, ViewModuleNum, ViewStationNum);


    cout << "It's the end" << "\n";
    //gApplication->Terminate();
}
//------------------------------------------------------------------------------

void ShowSignalInLayer(BmnVSPModule *module, BmnVSPLayer &layer, Int_t ilayer, Int_t imodule, Int_t istation) {

    //layer name
    TString layer_textId = TString("layer[") + TString::Itoa(ilayer, 10) + TString("], ");
    layer_textId += TString("module[") + TString::Itoa(imodule, 10) + TString("], ");
    layer_textId += TString("station[") + TString::Itoa(istation, 10) + TString("]");

    Int_t nStrips = layer.GetNStrips();
    Int_t firstStripNumber = layer.GetFirstStripNumber();
    Int_t lastStripNumber = layer.GetLastStripNumber();

    cout << "Station[" << istation << "], Module[" << imodule << "], Layer[" << ilayer << "]:\n";
    cout << "  nStrips = " << nStrips << "\n";
    cout << "  firstStripNumber = " << firstStripNumber << "\n";
    cout << "  lastStripNumber = " << lastStripNumber << "\n";

    TCanvas *layer_canv = new TCanvas(layer_textId + TString("_canv"), layer_textId, 0, 0, 1800, 1200);
    layer_canv->SetGrid();
    TH1F *layer_hist1f = new TH1F(layer_textId + TString("_hist1f"), layer_textId, nStrips, firstStripNumber, lastStripNumber+1);

    layer_hist1f->Reset(); //clear previous data

    //fill the histogram with layer data
    for(Int_t istrip = firstStripNumber; istrip <= lastStripNumber; ++istrip) {
        Double_t signal = layer.GetStripSignal(istrip);
        layer_hist1f->Fill(istrip, signal);
    }

    layer_canv->cd();
    layer_hist1f->SetFillColor(TColor::GetColor("#cccccc"));
    layer_hist1f->SetLineColor(TColor::GetColor("#333333"));
    layer_hist1f->GetXaxis()->SetLabelOffset(0.015);
    layer_hist1f->GetXaxis()->SetLabelSize(0.02); //0.035
    layer_hist1f->GetYaxis()->SetLabelSize(0.02); //0.035
    layer_hist1f->Draw("HIST");
    layer_canv->Update();

    //statistics area
    TPaveStats *stats_p = (TPaveStats*) layer_hist1f->FindObject("stats");
    stats_p->SetX1NDC(0.85); stats_p->SetX2NDC(0.995);
    stats_p->SetY1NDC(0.9); stats_p->SetY2NDC(0.95);
    stats_p->SetOptStat(1100);

    //legend
    TLegend *legend = new TLegend(0.85,0.77,0.995,0.9);
    legend->Draw();

    TLine *line_type1 = new TLine(0.86,0.87,0.88,0.87);
    line_type1->SetNDC(true);
    line_type1->SetLineColor(TColor::GetColor("#990000"));
    line_type1->SetLineWidth(3.0);
    line_type1->SetLineStyle(2);
    line_type1->Draw();

    TText *text_type1 = new TText(0.89,0.87,"strip hit (weighted cluster)");
    text_type1->SetNDC(true);
    text_type1->SetTextAlign(2);
    text_type1->SetTextSize(0.015);
    text_type1->SetTextFont(42);
    text_type1->Draw();

    TLine *line_type2 = new TLine(0.86,0.85,0.88,0.85);
    line_type2->SetNDC(true);
    line_type2->SetLineColor(TColor::GetColor("#000099"));
    line_type2->SetLineWidth(3.0);
    line_type2->SetLineStyle(2);
    line_type2->Draw();

    TText *text_type2 = new TText(0.89,0.85,"MC point position");
    text_type2->SetNDC(true);
    text_type2->SetTextAlign(2);
    text_type2->SetTextSize(0.015);
    text_type2->SetTextFont(42);
    text_type2->Draw();

    TText *text_type3a = new TText(0.86,0.83,"v(v)");
    text_type3a->SetNDC(true);
    text_type3a->SetTextAlign(2);
    text_type3a->SetTextSize(0.015);
    text_type3a->SetTextFont(82);
    text_type3a->Draw();

    TText *text_type3 = new TText(0.89,0.83,"MC number + strip signal");
    text_type3->SetNDC(true);
    text_type3->SetTextAlign(2);
    text_type3->SetTextSize(0.015);
    text_type3->SetTextFont(42);
    text_type3->Draw();

    TText *text_type4a = new TText(0.86,0.81,"v(v)");
    text_type4a->SetNDC(true);
    text_type4a->SetTextAlign(2);
    text_type4a->SetTextSize(0.015);
    text_type4a->SetTextFont(82);
    text_type4a->SetTextColor(TColor::GetColor("#990000"));
    text_type4a->Draw();

    TText *text_type4 = new TText(0.89,0.81,"pos. + cluster signal");
    text_type4->SetNDC(true);
    text_type4->SetTextAlign(2);
    text_type4->SetTextSize(0.015);
    text_type4->SetTextFont(42);
    text_type4->Draw();

    TText *text_type5a = new TText(0.86,0.79,"mc:v");
    text_type5a->SetNDC(true);
    text_type5a->SetTextAlign(2);
    text_type5a->SetTextSize(0.015);
    text_type5a->SetTextFont(82);
    text_type5a->SetTextColor(TColor::GetColor("#000099"));
    text_type5a->Draw();

    TText *text_type5 = new TText(0.89,0.79,"MC number");
    text_type5->SetNDC(true);
    text_type5->SetTextAlign(2);
    text_type5->SetTextSize(0.015);
    text_type5->SetTextFont(42);
    text_type5->Draw();
    //--------------------------------------------------------------------------

    vector<StripCluster> StripClusters = layer.GetStripClusters();
    Int_t nClusters = StripClusters.size();
    cout << "  nClusters = " << nClusters << "\n";

    //define a color collection for clusters
    vector<Int_t> colors = {3, 2, 5, 6, 7, 8, 29, 38, 41, 30, 43, 42, 45, 46,  0/*curr_pos*/}; //color numbers: TColor

    //run iteratively through clusters
    for(Int_t icluster = 0; icluster < nClusters; ++icluster) {
        cout << "   cluster[" << icluster << "]:\n";
        StripCluster cluster = StripClusters[icluster];

        Int_t nStrips = cluster.GetClusterSize();
        cout << "    nStrips = " << nStrips << "\n";

        for(Int_t istripInCluster = 0; istripInCluster < nStrips; ++istripInCluster) {
            Int_t strip_num = cluster.Strips[istripInCluster];
            Double_t signal = cluster.Signals[istripInCluster];

            cout << "    stripInCluster[" << istripInCluster << "]:\n";
            cout << "      strip_num = " << strip_num << "\n";
            cout << "      signal = " << signal << "\n";

            Int_t bin = layer_hist1f->FindBin(strip_num);

            PaintBin(layer_canv, layer_hist1f, bin, signal, colors[colors[colors.size()-1]], TColor::GetColor("#333333"));
            DrawMCMatches(layer_canv, layer_hist1f, strip_num, layer);

        }
        if ( colors[colors.size()-1] == (colors.size()-1) ) colors[colors.size()-1] = 0;
        else colors[colors.size()-1]++;
    }
    //--------------------------------------------------------------------------

    //Show strip hit positions on the current layer as vertical lines
    DrawStripHits(layer_canv, layer_hist1f, layer);

    //Show MC point positions on the current layer as vertical lines
    DrawMCStripLines(layer_canv, layer_hist1f, module, ilayer);

    cout << "\n";
}
//------------------------------------------------------------------------------

void ShowStripNumbersOfClustersInModule(BmnVSPModule *module, Int_t layer_type, Int_t module_num, Int_t station_num) {

    //module name
    TString module_textId = TString("Strip numbers of all clusters in the module: ") + TString("layer_type[") + TString::Format("%i", layer_type) + TString("], module[") + TString::Format("%i", module_num) + TString("], ");
    module_textId += TString("station[") + TString::Format("%i", station_num) + TString("]");

    Int_t nStrips = 0;
    Int_t firstStripNumber = 0;
    Int_t lastStripNumber = 0;

    vector<BmnVSPLayer> Layers = module->GetStripLayers();
    for(Int_t ilayer = 0; ilayer < Layers.size(); ++ilayer) {
        BmnVSPLayer layer = Layers[ilayer];
        if(layer.GetType() == layer_type) {
            if(layer.GetFirstStripNumber() < firstStripNumber) firstStripNumber = layer.GetFirstStripNumber();
            if(layer.GetLastStripNumber() > lastStripNumber) lastStripNumber = layer.GetLastStripNumber();
        }
    }
    nStrips = lastStripNumber - firstStripNumber + 1;

    TCanvas *module_canv = new TCanvas(module_textId + TString("_canv"), module_textId, 0, 0, 1800, 1200);
    module_canv->SetGrid();
    TH1F *module_hist1f = new TH1F(module_textId + TString("_hist1f"), module_textId, nStrips, firstStripNumber, lastStripNumber+1);

    for(Int_t ilayer = 0; ilayer < Layers.size(); ++ilayer) {
        BmnVSPLayer layer = Layers[ilayer];
        Int_t type = layer.GetType();

        if(layer.GetType() == layer_type) {
            vector<StripCluster> StripClusters = layer.GetStripClusters();
            Int_t nClusters = StripClusters.size();

            for(Int_t icluster = 0; icluster < nClusters; ++icluster) {
                StripCluster cluster = StripClusters[icluster];
                Int_t nStrips = cluster.GetClusterSize();

                for(Int_t istripInCluster = 0; istripInCluster < nStrips; ++istripInCluster) {
                    Int_t strip_num = cluster.Strips[istripInCluster];
                    Double_t signal = cluster.Signals[istripInCluster];

                    module_hist1f->Fill(strip_num);
                }
            }
        }
    }
    module_canv->cd();
    module_hist1f->SetFillColor(TColor::GetColor("#cccccc"));
    module_hist1f->SetLineColor(TColor::GetColor("#333333"));
    module_hist1f->GetXaxis()->SetLabelOffset(0.015);
    module_hist1f->GetXaxis()->SetLabelSize(0.02); //0.035
    module_hist1f->GetYaxis()->SetLabelSize(0.02); //0.035
    module_hist1f->Draw("hist");
}
//------------------------------------------------------------------------------

void ShowStripNumbersOfClustersInModule_FromBranches(Int_t event_num, Int_t module_num, Int_t station_num) {

    //Loading data from ROOT files
    TString file_prefix = "";
    //file_prefix = "_CC_4gev_b0_16fm";

    //Input Lower clusters (||) for VSP
    TClonesArray* fVSPLowerClusters_Array = new TClonesArray("");
    TString fVSPLowerClusters_FileName = "$VMCWORKDIR/macro/run/bmndst";
        fVSPLowerClusters_FileName += file_prefix;
        fVSPLowerClusters_FileName += ".root";
    TFile *fVSPLowerClusters_File = new TFile(fVSPLowerClusters_FileName, "READ");
    TTree *fVSPLowerClusters_Tree = (TTree*) fVSPLowerClusters_File->Get("bmndata");
    fVSPLowerClusters_Tree->GetBranch("BmnVSPLowerCluster")->SetAutoDelete(kTRUE);
    fVSPLowerClusters_Tree->SetBranchAddress("BmnVSPLowerCluster", &fVSPLowerClusters_Array);

    //Input Upper clusters (//) for VSP
    TClonesArray* fVSPUpperClusters_Array = new TClonesArray("");
    TString fVSPUpperClusters_FileName = "$VMCWORKDIR/macro/run/bmndst";
        fVSPUpperClusters_FileName += file_prefix;
        fVSPUpperClusters_FileName += ".root";
    TFile *fVSPUpperClusters_File = new TFile(fVSPUpperClusters_FileName, "READ");
    TTree *fVSPUpperClusters_Tree = (TTree*) fVSPUpperClusters_File->Get("bmndata");
    fVSPUpperClusters_Tree->GetBranch("BmnVSPUpperCluster")->SetAutoDelete(kTRUE);
    fVSPUpperClusters_Tree->SetBranchAddress("BmnVSPUpperCluster", &fVSPUpperClusters_Array);

    //Set the current event
    fVSPLowerClusters_Tree->GetEntry(event_num);
    fVSPUpperClusters_Tree->GetEntry(event_num);

    //module name
    TString module_lowerStrips_textId = TString("Branch: Strip numbers of all clusters in the module: ") + TString("Lower strips, module[") + TString::Format("%i", module_num) + TString("], ");
    module_lowerStrips_textId += TString("station[") + TString::Format("%i", station_num) + TString("]");

    TString module_upperStrips_textId = TString("Branch: Strip numbers of all clusters in the module: ") + TString("Upper strips, module[") + TString::Format("%i", module_num) + TString("], ");
    module_upperStrips_textId += TString("station[") + TString::Format("%i", station_num) + TString("]");

    Int_t nStrips = 640;
    Int_t firstStripNumber = 0;
    Int_t lastStripNumber = 639;

    TCanvas *module_lowerStrips_canv = new TCanvas(module_lowerStrips_textId + TString("_canv"), module_lowerStrips_textId, 0, 0, 1800, 1200);
    module_lowerStrips_canv->SetGrid();
    TH1F *module_lowerStrips_hist1f = new TH1F(module_lowerStrips_textId + TString("_hist1f"), module_lowerStrips_textId, nStrips, firstStripNumber, lastStripNumber+1);

    TCanvas *module_upperStrips_canv = new TCanvas(module_upperStrips_textId + TString("_canv"), module_upperStrips_textId, 0, 0, 1800, 1200);
    module_upperStrips_canv->SetGrid();
    TH1F *module_upperStrips_hist1f = new TH1F(module_upperStrips_textId + TString("_hist1f"), module_upperStrips_textId, nStrips, firstStripNumber, lastStripNumber+1);

    for(Int_t icluster = 0; icluster < fVSPLowerClusters_Array->GetEntries(); ++icluster) {
        StripCluster *cluster = (StripCluster*) fVSPLowerClusters_Array->At(icluster);

        if( (cluster->GetType() == 0) && (cluster->GetStation() == station_num) && (cluster->GetModule() == module_num)) {
            Int_t nStrips = cluster->GetClusterSize();
            for(Int_t istripInCluster = 0; istripInCluster < nStrips; ++istripInCluster) {
                Int_t strip_num = cluster->Strips[istripInCluster];
                Double_t signal = cluster->Signals[istripInCluster];

                module_lowerStrips_hist1f->Fill(strip_num);
            }
        }
    }

    for(Int_t icluster = 0; icluster < fVSPUpperClusters_Array->GetEntries(); ++icluster) {
        StripCluster *cluster = (StripCluster*) fVSPUpperClusters_Array->At(icluster);

        if( (cluster->GetType() == 1) && (cluster->GetStation() == station_num) && (cluster->GetModule() == module_num) ) {
            Int_t nStrips = cluster->GetClusterSize();
            for(Int_t istripInCluster = 0; istripInCluster < nStrips; ++istripInCluster) {
                Int_t strip_num = cluster->Strips[istripInCluster];
                Double_t signal = cluster->Signals[istripInCluster];

                module_upperStrips_hist1f->Fill(strip_num);
            }
        }
    }

    module_lowerStrips_canv->cd();
    module_lowerStrips_hist1f->SetFillColor(TColor::GetColor("#dddddd"));
    module_lowerStrips_hist1f->SetLineColor(TColor::GetColor("#333333"));
    module_lowerStrips_hist1f->GetXaxis()->SetLabelOffset(0.015);
    module_lowerStrips_hist1f->GetXaxis()->SetLabelSize(0.02); //0.035
    module_lowerStrips_hist1f->GetYaxis()->SetLabelSize(0.02); //0.035
    module_lowerStrips_hist1f->Draw("hist");

    module_upperStrips_canv->cd();
    module_upperStrips_hist1f->SetFillColor(TColor::GetColor("#dddddd"));
    module_upperStrips_hist1f->SetLineColor(TColor::GetColor("#333333"));
    module_upperStrips_hist1f->GetXaxis()->SetLabelOffset(0.015);
    module_upperStrips_hist1f->GetXaxis()->SetLabelSize(0.02); //0.035
    module_upperStrips_hist1f->GetYaxis()->SetLabelSize(0.02); //0.035
    module_upperStrips_hist1f->Draw("hist");
}
//------------------------------------------------------------------------------


void ShowReadoutPlane(BmnVSPModule *module, Int_t imodule, Int_t istation) {

    //module borders
    Double_t x_min_module = module->GetXMinModule();
    Double_t x_max_module = module->GetXMaxModule();
    Double_t y_min_module = module->GetYMinModule();
    Double_t y_max_module = module->GetYMaxModule();
    Double_t x_size_module = module->GetXSize();
    Double_t y_size_module = module->GetYSize();

    //area borders for drawing
    Double_t hborder_margin = 0;
    Double_t hborder_left, hborder_right, hborder_bottom, hborder_top;
    if(x_size_module >= y_size_module) {
        hborder_margin = x_size_module * 0.05;
        hborder_left = x_min_module  - hborder_margin;
        hborder_right = x_max_module + hborder_margin;
        hborder_bottom = y_min_module - fabs(x_size_module - y_size_module)*0.5 - hborder_margin;
        hborder_top = y_max_module + fabs(x_size_module - y_size_module)*0.5 + hborder_margin;
    }
    else {
        hborder_margin = y_size_module * 0.05;
        hborder_left = x_min_module - fabs(x_size_module - y_size_module)*0.5 - hborder_margin;
        hborder_right = x_max_module + fabs(x_size_module - y_size_module)*0.5 + hborder_margin;
        hborder_bottom = y_min_module - hborder_margin;
        hborder_top = y_max_module + hborder_margin;
    }

    //module name
    TString module_textId = TString("module[") + TString::Format("%i", imodule) + TString("], ");
            module_textId += TString("station[") + TString::Format("%i", istation) + TString("]");

    TCanvas *module_canv = new TCanvas(module_textId + TString("_canv"), module_textId, 0, 0, 1600, 1600);
    module_canv->SetGrid();
    TH2D *module_hist2d = new TH2D(module_textId + TString("_hist1f"), module_textId, 100, hborder_left, hborder_right,  100, hborder_bottom, hborder_top);
    module_hist2d->GetXaxis()->SetTitle("X(local, inverted)");
    module_hist2d->GetYaxis()->SetTitle("Y");

    module_canv->cd();
    module_hist2d->SetFillColor(TColor::GetColor("#cccccc"));
    module_hist2d->SetLineColor(TColor::GetColor("#333333"));
    module_hist2d->GetXaxis()->SetLabelOffset(0.015);
    module_hist2d->GetXaxis()->SetLabelSize(0.02); //0.035
    module_hist2d->GetYaxis()->SetLabelSize(0.02); //0.035
    module_hist2d->SetStats(0);
    module_hist2d->Draw("");
    module_canv->Update();

    //drawing module borders on the area
    Double_t x_borders_module[4] = {x_min_module, x_min_module, x_max_module, x_max_module};
    Double_t y_borders_module[4] = {y_min_module, y_max_module, y_max_module, y_min_module};
    for(Int_t ibpoint = 0; ibpoint < 4; ++ibpoint) {
        //Lines between points on the XY plane
        Double_t x1, y1, x2, y2;
        if(ibpoint != 3) {
            x1 = x_borders_module[ibpoint];
            y1 = y_borders_module[ibpoint];
            x2 = x_borders_module[ibpoint+1];
            y2 = y_borders_module[ibpoint+1];
        }
        else {
            x1 = x_borders_module[ibpoint];
            y1 = y_borders_module[ibpoint];
            x2 = x_borders_module[0];
            y2 = y_borders_module[0];
        }
        TLine *border_line = new TLine(x1, y1, x2, y2);
        border_line->SetLineColor(TColor::GetColor("#333333"));
        border_line->SetLineWidth(2.0);
        module_canv->cd();
        border_line->Draw();
    }

    //drawing mc points (real points in the current module)
    Int_t nRealPointsInModule = module->GetNRealPoints();
    for(Int_t irpoint = 0; irpoint < nRealPointsInModule; ++irpoint) {
    //for(Int_t irpoint = 0; irpoint < 1; ++irpoint) {
        Double_t x_rpoint = module->GetRealPointX(irpoint);
        Double_t y_rpoint = module->GetRealPointY(irpoint);
        Int_t mc_index = module->GetRealPointMC(irpoint);

        TMarker *rpoint_marker = new TMarker(x_rpoint, y_rpoint, 2);
        rpoint_marker->SetMarkerSize(1.5);
        rpoint_marker->SetMarkerColor(TColor::GetColor("#0000cc"));
        module_canv->cd();
        rpoint_marker->Draw();

        TString content = TString::Format("%i", mc_index);
        TLatex *btext = new TLatex();
        btext->SetText(x_rpoint+0.01, y_rpoint+0.01, content);
        btext->SetTextAlign(11);
        btext->SetTextFont(82);
        btext->SetTextSize(0.015);
        btext->SetTextColorAlpha(TColor::GetColor("#0000cc"), 1.0);
        module_canv->cd();
        btext->Draw();
    }

    //drawing hits (intersections in the current module)
    Int_t nIntersectionPoints = module->GetNIntersectionPoints();
    for(Int_t ihit = 0; ihit < nIntersectionPoints; ++ihit) {
        Double_t x_hit = module->GetIntersectionPointX(ihit);
        Double_t y_hit = module->GetIntersectionPointY(ihit);

        TMarker *hit_marker = new TMarker(x_hit, y_hit, 5);
        hit_marker->SetMarkerSize(1.5);
        hit_marker->SetMarkerColor(TColor::GetColor("#aa0000"));
        module_canv->cd();
        hit_marker->Draw();
    }


    //layers processing --------------------------------------------------------
    vector<BmnVSPLayer> Layers = module->GetStripLayers();
    for(Int_t ilayer = 0; ilayer < Layers.size(); ++ilayer) {
        BmnVSPLayer layer = Layers[ilayer];

        //drawing borders for each layer in the module
        Double_t x_min_layer = layer.GetXMinLayer();
        Double_t x_max_layer = layer.GetXMaxLayer();
        Double_t y_min_layer = layer.GetYMinLayer();
        Double_t y_max_layer = layer.GetYMaxLayer();
        Double_t x_borders_layer[4] = {x_min_layer, x_min_layer, x_max_layer, x_max_layer};
        Double_t y_borders_layer[4] = {y_min_layer, y_max_layer, y_max_layer, y_min_layer};
        for(Int_t ibpoint = 0; ibpoint < 4; ++ibpoint) {
            //Lines between points on the XY plane
            Double_t x1, y1, x2, y2;
            if(ibpoint != 3) {
                x1 = x_borders_layer[ibpoint];
                y1 = y_borders_layer[ibpoint];
                x2 = x_borders_layer[ibpoint+1];
                y2 = y_borders_layer[ibpoint+1];
            }
            else {
                x1 = x_borders_layer[ibpoint];
                y1 = y_borders_layer[ibpoint];
                x2 = x_borders_layer[0];
                y2 = y_borders_layer[0];
            }
            TLine *border_line = new TLine(x1, y1, x2, y2);
            border_line->SetLineColor(TColor::GetColor("#333333"));
            border_line->SetLineWidth(1.0);
            module_canv->cd();
            border_line->Draw();
        }

        //----------------------------------------------------------------------

        for(Int_t istripHit = 0; istripHit < layer.GetNStripHits(); ++ istripHit) {
            Double_t strip_pos = layer.GetStripHitPos(istripHit);
            Double_t B_val = layer.CalculateStripEquationB(strip_pos);
            Double_t angle_rad = layer.GetAngleRad();
            Double_t PiOver2MinusAngleRad = TMath::PiOver2() - angle_rad;

            Double_t x1, y1, x2, y2;

            if(layer.GetType() == 0 &&  ilayer == 0) {
                cout << "strip_pos = " << strip_pos << "\n";
                cout << "angle_rad = " << angle_rad << "\n";
                cout << "B_val = " << B_val << "\n";
                cout << "\n";

            }

            if(fabs(B_val) < 1e+6) {
                x1 = (y_min_layer - B_val) / TMath::Tan(PiOver2MinusAngleRad);
                if(x1 < x_min_layer) x1 = x_min_layer;
                if(x1 > x_max_layer) x1 = x_max_layer;
                y1 = TMath::Tan(PiOver2MinusAngleRad)*x1 + B_val;
                if(y1 < y_min_layer) y1 = y_min_layer;
                if(y1 > y_max_layer) y1 = y_max_layer;

                x2 = (y_max_layer - B_val) / TMath::Tan(PiOver2MinusAngleRad);
                if(x2 < x_min_layer) x2 = x_min_layer;
                if(x2 > x_max_layer) x2 = x_max_layer;
                y2 = TMath::Tan(PiOver2MinusAngleRad)*x2 + B_val;
                if(y2 < y_min_layer) y2 = y_min_layer;
                if(y2 > y_max_layer) y2 = y_max_layer;
            }
            else {

                if(layer.GetStripNumberingOrder() == LeftToRight) {
                    x1 = layer.GetXLeftStripBorderPoint() + (strip_pos - layer.GetFirstStripNumber())*layer.GetPitch();
                }
                else {
                    x1 = layer.GetXRightStripBorderPoint() - (strip_pos - layer.GetFirstStripNumber())*layer.GetPitch();
                }
                y1 = y_min_layer;

                x2 = x1;
                y2 = y_max_layer;

            }

            TLine *strip_line = new TLine(x1, y1, x2, y2);
            strip_line->SetLineColorAlpha(TColor::GetColor("#cc0000"), 0.3);
            strip_line->SetLineWidth(1.0);
            module_canv->cd();
            strip_line->Draw();

            TString content = TString::Format("%.1f", strip_pos);
            TLatex *btext = new TLatex();
            btext->SetText(x2, y2+0.01, content);
            btext->SetTextAlign(21);
            btext->SetTextFont(82);
            btext->SetTextSize(0.015);
            btext->SetTextColorAlpha(TColor::GetColor("#cc0000"), 1.0);
            module_canv->cd();
            btext->Draw();
        }
    }
    //--------------------------------------------------------------------------
}

//------------------------------------------------------------------------------

void PaintBin(TCanvas *c, TH1F *h, Int_t bin, Double_t bin_height, Int_t fcolor, Int_t bcolor) {

    Double_t x1 = h->GetBinLowEdge(bin);
    Double_t y1 = h->GetMinimum();
    Double_t x2 = h->GetBinWidth(bin) + h->GetBinLowEdge(bin);
    //Double_t y2 = h->GetBinContent(bin); //whole bin height: sum of signal
    Double_t y2 = bin_height; //only signal of strip

    Double_t alpha_density = 1.0;
    if(fabs(bin_height / h->GetBinContent(bin)) >= 0.01) alpha_density = 0.7;

    TBox *b_filled = new TBox(x1, y1, x2, y2);
    b_filled->SetFillColorAlpha(fcolor, alpha_density);

    TBox *b_border = new TBox(x1, y1, x2, y2);
    b_border->SetFillStyle(1);
    b_border->SetLineColor(bcolor);
    b_border->SetLineWidth(1);

    c->cd();
    b_filled->Draw("");
    b_border->Draw("");
}
//------------------------------------------------------------------------------

void DrawMCMatches(TCanvas *c, TH1F *h, Int_t strip_num, BmnVSPLayer &layer) {

    BmnMatch match = layer.GetStripMatch(strip_num);

    Int_t n_links = match.GetNofLinks();

    Int_t most_probably_index = -1;
    Double_t max_weight = 0;

    Double_t height_shift = 0.0;
    for (Int_t ilink = 0; ilink < n_links; ilink++) {
        Int_t index = match.GetLink(ilink).GetIndex();
        Double_t weight = match.GetLink(ilink).GetWeight();
        if (weight > max_weight) {
            max_weight = weight;
            most_probably_index = index;
        }
        Int_t bin = h->FindBin(strip_num);
        //TString content = TString::Format("%i", most_probably_index) + TString("(") + TString::Format("%.1f", max_weight) + TString(")");
        TString content = TString::Format("%i", index) + TString("(") + TString::Format("%.1f", weight) + TString(")");
        TLatex *btext = new TLatex();
        btext->SetText(h->GetBinCenter(bin), h->GetBinContent(bin) + 0.5 + height_shift, content);
        btext->SetTextAlign(21);
        btext->SetTextFont(82);
        btext->SetTextSize(0.015);
        height_shift += 2.5;
        c->cd();
        btext->Draw();
    }
}
//------------------------------------------------------------------------------

void DrawStripHits(TCanvas *c, TH1F *h, BmnVSPLayer &layer) {

    Int_t nStripHits = layer.GetNStripHits();
    cout << "  nStripHits = " << nStripHits << "\n";

    //run iteratively through strip hits
    for(Int_t iStripHit = 0; iStripHit < nStripHits; ++iStripHit) {
        cout << "   stripHit[" << iStripHit << "]:\n";
        Double_t strip_hit_pos = layer.GetStripHitPos(iStripHit);
        Double_t strip_hit_signal = layer.GetStripHitTotalSignal(iStripHit);

        cout << "    strip_hit_pos = " << strip_hit_pos << "\n";
        cout << "    strip_hit_signal = " << strip_hit_signal << "\n";

        Int_t bin = h->FindBin(strip_hit_pos);
        Double_t x1_line = strip_hit_pos;
        Double_t x2_line = strip_hit_pos;
        Double_t y1_line = h->GetMinimum() - 1.0;
        Double_t y2_line = h->GetBinContent(bin);

        TLine *strip_hit_line = new TLine(x1_line, y1_line, x2_line, y2_line);
        strip_hit_line->SetLineStyle(2);
        strip_hit_line->SetLineWidth(2.0);
        strip_hit_line->SetLineColor(TColor::GetColor("#990000"));

        c->cd();
        strip_hit_line->Draw();

        TString content = TString::Format("%.1f", strip_hit_pos) + TString("(") + TString::Format("%.1f", strip_hit_signal) + TString(")");
        TLatex *btext = new TLatex();
        btext->SetText(x1_line, y1_line, content);
        btext->SetTextAlign(23);
        btext->SetTextFont(82);
        btext->SetTextColor(TColor::GetColor("#990000"));
        btext->SetTextSize(0.015);
        c->cd();
        btext->Draw();
    }
}
//------------------------------------------------------------------------------

void DrawMCStripLines(TCanvas *c, TH1F *h, BmnVSPModule *module, Int_t layer_num) {

    Int_t nRealPointsInModule = module->GetNRealPoints();

    //run iteratively through real points
    for(Int_t irpoint = 0; irpoint < nRealPointsInModule; ++irpoint) {
        Double_t x = module->GetRealPointX(irpoint);
        Double_t y = module->GetRealPointY(irpoint);
        Int_t mc_index = module->GetRealPointMC(irpoint);

        BmnVSPLayer &layer = module->GetStripLayer(layer_num);
        Int_t layer_zoneID = layer.GetZoneID();

        if(layer.IsPointInsideStripLayer(x, y)) {
            Double_t strip_pos = layer.ConvertPointToStripPosition(x, y);

            Int_t bin = h->FindBin(strip_pos);
            Double_t x1_line = strip_pos;
            Double_t x2_line = strip_pos;
            Double_t y1_line = h->GetMinimum() - 5.0;
            Double_t y2_line = h->GetBinContent(bin);

            TLine *strip_mc_line = new TLine(x1_line, y1_line, x2_line, y2_line);
            strip_mc_line->SetLineStyle(2);
            strip_mc_line->SetLineWidth(2.0);
            strip_mc_line->SetLineColor(TColor::GetColor("#000099"));
            c->cd();
            strip_mc_line->Draw();

            TString content = TString("mc:") + TString::Format("%i", mc_index);
            TLatex *btext = new TLatex();
            btext->SetText(x1_line, y1_line, content);
            btext->SetTextAlign(23);
            btext->SetTextFont(82);
            btext->SetTextColor(TColor::GetColor("#0000cc"));
            btext->SetTextSize(0.015);
            c->cd();
            btext->Draw();
        }
        else {
            for(Int_t ilayer = 0; ilayer < module->GetNStripLayers(); ++ilayer) {
                if (ilayer == layer_num) continue;
                BmnVSPLayer &curr_layer = module->GetStripLayer(ilayer);

                if(layer_zoneID == curr_layer.GetZoneID()) {

                    Double_t strip_pos = curr_layer.ConvertPointToStripPosition(x, y);

                    Int_t bin = h->FindBin(strip_pos);
                    Double_t x1_line = strip_pos;
                    Double_t x2_line = strip_pos;
                    Double_t y1_line = h->GetMinimum() - 5.0;
                    Double_t y2_line = h->GetBinContent(bin);

                    TLine *strip_mc_line = new TLine(x1_line, y1_line, x2_line, y2_line);
                    strip_mc_line->SetLineStyle(2);
                    strip_mc_line->SetLineWidth(2.0);
                    strip_mc_line->SetLineColor(TColor::GetColor("#000099"));
                    c->cd();
                    strip_mc_line->Draw();

                    TString content = TString("mc:") + TString::Format("%i", mc_index);
                    TLatex *btext = new TLatex();
                    btext->SetText(x1_line, y1_line, content);
                    btext->SetTextAlign(23);
                    btext->SetTextFont(82);
                    btext->SetTextColor(TColor::GetColor("#0000cc"));
                    btext->SetTextSize(0.015);
                    c->cd();
                    btext->Draw();
                }
            }
        }
    }
}
//------------------------------------------------------------------------------

void LoadDataFromMCPoints(BmnVSPStationSet *station_set, BmnVSPTransform *transf_set,  Int_t event_num) {

    //Reset the previous data before loading
    station_set->Reset();

    //Loading data from ROOT files
    TString file_prefix = "";
    //file_prefix = "_CC_4gev_b0_16fm";

    // Input MC-points for VSP
    TClonesArray* fVSPMCPoints_Array = new TClonesArray("");
    TString fVSPMCPoints_FileName = "$VMCWORKDIR/macro/run/bmnsim";
        fVSPMCPoints_FileName += file_prefix;
        fVSPMCPoints_FileName += ".root";
    TFile *fVSPMCPoints_File = new TFile(fVSPMCPoints_FileName, "READ");
    TTree *fVSPMCPoints_Tree = (TTree*) fVSPMCPoints_File->Get("bmndata");
    fVSPMCPoints_Tree->GetBranch("VSPPoint")->SetAutoDelete(kTRUE);
    fVSPMCPoints_Tree->SetBranchAddress("VSPPoint", &fVSPMCPoints_Array);

    //Set the current event
    fVSPMCPoints_Tree->GetEntry(event_num);

    Int_t n_points = fVSPMCPoints_Array->GetEntries();
    Int_t added_points_cnt = 0;

    for(Int_t ipoint = 0; ipoint < n_points; ++ipoint) {
            BmnVSPPoint *mc_point = (BmnVSPPoint*) fVSPMCPoints_Array->At(ipoint);

            Double_t x = - ((FairMCPoint*) mc_point)->GetX(); // invert because in current geometry +x is left, -x is right
            Double_t y = ((FairMCPoint*) mc_point)->GetY();
            Double_t z = ((FairMCPoint*) mc_point)->GetZ();

            Double_t px = - ((FairMCPoint*) mc_point)->GetPx(); // invert because in current geometry +x is left, -x is right
            Double_t py = ((FairMCPoint*) mc_point)->GetPy();
            Double_t pz = ((FairMCPoint*) mc_point)->GetPz();

            Double_t d_eloss = mc_point->GetEnergyLoss()*1e6; // in keV
            Int_t ref_id = ipoint;

            //Information from MC-points
            Int_t mc_station_num = ((BmnVSPPoint*) mc_point)->GetStation();
            Int_t mc_module_num = ((BmnVSPPoint*) mc_point)->GetModule();

            //Transform MC-point coordinates to local coordinate system
            if(transf_set && mc_station_num < station_set->GetNStations()) {
                if(mc_module_num < station_set->GetStation(mc_station_num)->GetNModules()) {
                    Plane3D::Point loc_point = transf_set->ApplyInverseTransforms(Plane3D::Point(-x, y, z), mc_station_num, mc_module_num);
                    Plane3D::Point loc_direct = transf_set->ApplyInverseTransforms(Plane3D::Point(-(px+x), (py+y), (pz+z)), mc_station_num, mc_module_num);
                    x = -loc_point.X();
                    y = loc_point.Y();
                    z = loc_point.Z();

                    px = -(loc_direct.X() - loc_point.X());
                    py = loc_direct.Y() - loc_point.Y();
                    pz = loc_direct.Z() - loc_point.Z();
                }
            }

            //adding the current point to the detector
            if (station_set->AddPointToDetector(x, y, z, px, py, pz, d_eloss, ref_id)) added_points_cnt++;
    }

    cout << "------------------------------------\n";
    cout << "LoadDataFromMCPoints:\n  " << added_points_cnt << " of " << n_points << " Monte-Carlo points have been added to the detector\n";
    cout << "------------------------------------\n";
}
//------------------------------------------------------------------------------

void LoadDataFromDigits(BmnVSPStationSet *station_set, BmnVSPTransform *transf_set,  Int_t event_num) {

    //Reset the previous data before loading
    station_set->Reset();

    //Loading data from ROOT files
    TString file_prefix = "";
    //file_prefix = "_CC_4gev_b0_16fm";

    // Input Digits for VSP
    TClonesArray* fVSPDigits_Array = new TClonesArray("");
    TString fVSPDigits_FileName = "$VMCWORKDIR/macro/run/bmnsim";
        fVSPDigits_FileName += file_prefix;
        fVSPDigits_FileName += ".root";
    TFile *fVSPDigits_File = new TFile(fVSPDigits_FileName, "READ");
    TTree *fVSPDigits_Tree = (TTree*) fVSPDigits_File->Get("bmndata");
    fVSPDigits_Tree->GetBranch("BmnVSPDigit")->SetAutoDelete(kTRUE);
    fVSPDigits_Tree->SetBranchAddress("BmnVSPDigit", &fVSPDigits_Array);

    //Set the current event
    fVSPDigits_Tree->GetEntry(event_num);

    Int_t n_digits = fVSPDigits_Array->GetEntries();
    Int_t added_digits_cnt = 0;

    for (Int_t idigit = 0; idigit < n_digits; idigit++) {
        BmnVSPDigit *digit = (BmnVSPDigit*) fVSPDigits_Array->At(idigit);

        BmnVSPStation *station = station_set->GetStation(digit->GetStation());
        BmnVSPModule *module = station->GetModule(digit->GetModule());

        if (module->SetStripSignalInLayerByZoneId(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal())) added_digits_cnt++;

    }

    cout << "------------------------------------\n";
    cout << "LoadDataFromDigits:\n  " << added_digits_cnt << " of " << n_digits << " digits have been added to the detector\n";
    cout << "------------------------------------\n";
}