
#define PAD_WIDTH 720
#define PAD_HEIGHT 560
/**
 * readScalers
 * Example of reading scaler values
 * @param fileName
 * @param examples number of events to show
 */
void readScalers(TString fileName = "", Long64_t NExamples = 0)
{
    if (fileName == "") {
        cout << "File not specified!" << endl;
        return;
    }

    TFile* file = new TFile(fileName, "READ");
    if (file->IsOpen() == false) {
        printf("\nCannot open file %s !\n", fileName.Data());
        return;
    }
    /**
     * Reading Run Header for the whole run statistics:
     *  beam counters, run start/end time
     */
    printf("Run Header content:\n");
    DigiRunHeader* rh = (DigiRunHeader*)file->Get("DigiRunHeader");

    UInt_t PeriodId = rh->GetPeriodId();
    UInt_t RunId = rh->GetRunId();
    printf("\tPeriod: %u  Run: %u\n", PeriodId, RunId);

    //    Double_t BT = rh->GetBT(); // Beam Trigger
    //    Double_t BTnBusy = rh->GetBTnBusy(); // BT & not Busy
    //        // (BT & not Busy) normalized by Before&After protection ratio
    //    Double_t TotalFlux = rh->GetBTAccepted();
    //    printf("\tBeam Trigger                       : %f\n", BT);
    //    printf("\tBeam Trigger and Not Busy          : %f\n", BTnBusy);
    //    printf("\tBeam Trigger Accepted Events (Flux): %f\n", TotalFlux);
    TTimeStamp RunStartTime = rh->GetRunStartTime();
    TTimeStamp RunEndTime = rh->GetRunEndTime();
    printf("\tSTART  : %s\n", RunStartTime.AsString());
    printf("\tFINISH : %s\n", RunEndTime.AsString());
    /**
     * Reading Event Header for each event in the range:
     *  current spill statistics,
     */
    const vector<TString> trc_map{"BC1L", "pBT", "BT", "NiT", "CCT1", "MBT", "BT&nBUSY", "CCT2"};
    vector<uint64_t> trc_counts_BR;
    vector<uint64_t> trc_counts_AR;
    size_t trigCount = trc_map.size();
    uint64_t payloadEvCnt(0);
    trc_counts_BR.resize(trigCount, 0);
    trc_counts_AR.resize(trigCount, 0);
    TString name = "Scalers";
    TCanvas* can = new TCanvas(name, name, PAD_WIDTH * 1, PAD_HEIGHT * 2);
    can->Divide(1, 2);
    name = "Before_reduction";
    TH1I* hBR = new TH1I(name, name, trigCount, 0, (Double_t)trigCount);
    name = "After__reduction";
    TH1I* hAR = new TH1I(name, name, trigCount, 0, (Double_t)trigCount);
    TAxis* xaBR = hBR->GetXaxis();
    TAxis* xaAR = hAR->GetXaxis();
    for (Int_t i = 0; i < trigCount; ++i) {
        xaBR->SetBinLabel(i + 1, trc_map[i]);
        xaAR->SetBinLabel(i + 1, trc_map[i]);
    }

    TTree* tree = (TTree*)file->Get("bmndata");
    BmnEventHeader* eh = nullptr;
    tree->SetBranchAddress("BmnEventHeader.", &eh);
    Long64_t NEvents = tree->GetEntries();
    NExamples = (NExamples == 0) ? NEvents : min(NEvents, NExamples);
    for (Int_t i = 0; i < NExamples; i++) {
        tree->GetEntry(i);
        if (eh->GetEventType() != kBMNPAYLOAD)
            continue;
        ++payloadEvCnt;
        //        printf("ev %7d  type %d \n\t BR %3u \n\t AR %3u\n",
        //                eh->GetEventId(), eh->GetEventType(),
        //                eh->GetInputSignalsBR(),
        //                eh->GetInputSignalsAR());
        for (Int_t iBit = 0; iBit < trigCount; ++iBit) {
            bool isSetBR = eh->GetInputSignalsBR() & BIT(iBit);
            bool isSetAR = eh->GetInputSignalsAR() & BIT(iBit);
            trc_counts_BR[iBit] += isSetBR;
            trc_counts_AR[iBit] += isSetAR;
            if (isSetBR) {
                hBR->Fill(trc_map[iBit], 1);
            }
            if (isSetAR) {
                hAR->Fill(trc_map[iBit], 1);
            }
        }
    }
    can->cd(1);
    hBR->Draw();
    can->cd(2);
    hAR->Draw();
    can->SaveAs("scalers.png");
    printf("Before reduction\n");
    for (Int_t iBit = 0; iBit < trigCount; ++iBit) {
        printf("%15s %lu\n", trc_map[iBit].Data(), trc_counts_BR[iBit]);
    }
    printf("After  reduction\n");
    for (Int_t iBit = 0; iBit < trigCount; ++iBit) {
        printf("%15s %lu\n", trc_map[iBit].Data(), trc_counts_AR[iBit]);
    }
    printf(" %lld events shown.\n", NExamples);
    printf(" %lu payload events.\n", payloadEvCnt);
}
