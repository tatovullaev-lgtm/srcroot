#include "BmnTrackingQaExpReport.h"

#include "BmnReportElement.h"
#include "TStyle.h"

BmnTrackingQaExpReport::BmnTrackingQaExpReport()
    : BmnSimulationReport()
{
    SetReportName("tracking_qa");
}

BmnTrackingQaExpReport::BmnTrackingQaExpReport(TString name)
    : BmnSimulationReport()
{
    SetReportName(name);
}

BmnTrackingQaExpReport::~BmnTrackingQaExpReport() {}
void BmnTrackingQaExpReport::Draw()
{

    //    gSystem->Sleep(5000);

    gStyle->SetPalette(77);

    DrawThreeH2("Vertex_2D", "VertXY", "VertZX", "VertZY");
    DrawThreeH2("Vertex_vs_number_of_tracks", "VertX_vs_Ntracks", "VertY_vs_Ntracks", "VertZ_vs_Ntracks");
    DrawThreeH1("Vertex_1D", "VertX", "VertY", "VertZ");

    DrawOneH1("Rigidity", "Rigidity", "");
    DrawTwoH1("Multiplicity", "NInnTr", "NGlobTr", "", kFALSE);
    DrawTwoH1("Hits_Multiplicity_in_InnerTracker", "NSilHits", "NGemHits", "", kFALSE);

    // TOF
    DrawTwoH2("Banana_plots", "banana_tof400", "banana_tof700", 2 * baseW, baseH);
    DrawTwoH1("Hits_Multiplicity_in_ToF", "NTof400Hits", "NTof700Hits", "", kFALSE);
    DrawTwoH2("ToF Hits", "TOF400_X_Y", "TOF700_X_Y", 2 * baseW, baseH);

    // CSC
    DrawTwoH1("Hits_Multiplicity_in_CSC_1_2", "NsCSC1Hits", "NsCSC2Hits", "", kFALSE);
    DrawTwoH1("Hits_Multiplicity_in_CSC_3_4", "NsCSC3Hits", "NsCSC4Hits", "", kFALSE);
    DrawTwoH1("Hits_Multiplicity_in_large_CSCs", "NlCSC5Hits", "NlCSC6Hits", "", kFALSE);

    // DCH1

    // DCH2

    // SiBT
    DrawOneH1("Hits Multiplicity in SiBT", "NSiBTHits", "");
    DrawThreeH2("BmnSiBT_Hits", "x_vs_y_SiBT_st0", "x_vs_y_SiBT_st1", "x_vs_y_SiBT_st2");
    DrawThreeH2("BmnSiBT_meanHits", "x_vs_y_SiBT_st0_meanInEvent", "x_vs_y_SiBT_st1_meanInEvent",
                "x_vs_y_SiBT_st2_meanInEvent");
    // DrawFourH2("BmnSiBTTracksParam", "VertexX_vs_SiBTTracksX", "SiBTTracks_FirstXY", "VertexX_vs_SiBTTracksY",
    // "SiBTTracksFirstTx_Ty");
    DrawTwoH2("Vertex_SiBT_correlation", "VertexX_vs_SiBTTracksX", "VertexY_vs_SiBTTracksY", baseW, baseH);
    DrawThreeH1("BmnSiBTNHits", "NumberOfHitsInEvent_st0", "NumberOfHitsInEvent_st1", "NumberOfHitsInEvent_st2");

    DrawHits2D();
    if (!fInitCanvasesDone)
        fInitCanvasesDone = kTRUE;
}