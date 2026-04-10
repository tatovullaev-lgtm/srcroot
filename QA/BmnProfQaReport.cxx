#include "BmnProfQaReport.h"
#include "TStyle.h"
#include "BmnReportElement.h"

BmnProfQaReport::BmnProfQaReport() : BmnSimulationReport() {
    SetReportName("prof_qa");
}

BmnProfQaReport::BmnProfQaReport(TString name) : BmnSimulationReport() {
    SetReportName(name);
}

BmnProfQaReport::~BmnProfQaReport() {
}

void BmnProfQaReport::Create() {
    Out().precision(3);
    Out() << R()->DocumentBegin();
    Out() << R()->Title(0, GetTitle());
    PrintCanvases();
    Out() << R()->DocumentEnd();
}

void BmnProfQaReport::Draw() {
    gStyle->SetPalette(77);
    TString detName;
    BmnDetectorList::GetSystemNameCaps(kSiProf, detName);
    const Int_t can_w = 500;
    const Int_t can_h = 500;
    DrawTwoH1("Strips_board1",
            HistNameProfDigit(detName, 0, 0, 0),
            HistNameProfDigit(detName, 0, 0, 1),
            "");
    DrawTwoH1("Strips_board2",
            HistNameProfDigit(detName, 1, 0, 0),
            HistNameProfDigit(detName, 1, 0, 1),
            "");
    DrawTwoH1("Signals_board1",
            TString(HistNameProfDigit(detName, 0, 0, 0)) + "_sig",
            TString(HistNameProfDigit(detName, 0, 0, 1)) + "_sig",
            "HIST");
    DrawTwoH1("Signals_board2",
            TString(HistNameProfDigit(detName, 1, 0, 0)) + "_sig",
            TString(HistNameProfDigit(detName, 1, 0, 1)) + "_sig",
            "HIST");
    DrawTwoH2("Pedestals_board1",
            TString(HistNameProfDigit(detName, 0, 0, 0)) + "_pedestals",
            TString(HistNameProfDigit(detName, 0, 0, 1)) + "_pedestals",
            can_w, can_h);
    DrawTwoH2("Pedestals_board2",
            TString(HistNameProfDigit(detName, 1, 0, 0)) + "_pedestals",
            TString(HistNameProfDigit(detName, 1, 0, 1)) + "_pedestals",
            can_w, can_h);
    DrawOneH2("Hits_board1", HistNameProfHits(detName, 0));
    DrawOneH2("Hits_board2", HistNameProfHits(detName, 1));
    //    DrawHits2D();
    if (!fInitCanvasesDone)
        fInitCanvasesDone = kTRUE;
}