// file: full path to raw file
// nEvents: if 0 then decode all events
// doConvert: convert RAW --> ROOT before decoding or use file converted before
#define SORTED 1

void BmnDataToRootParallel(vector<TString> files,
                           UInt_t threadCount = 1,
                           UInt_t period = 8,
                           TString outDir = "",
                           Bool_t tempFileOnDisk = false)
{
    Int_t iVerbose = 0;   ///<- Verbosity level: 0 - summary info on progress; 1 - event level, 2 - event objects level,
    ///< 3 - full debug output
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("info");   // for info write "INFO"

    TStopwatch timer;
    timer.Start();

    BmnConverter* conv = new BmnConverter(threadCount, period, outDir, tempFileOnDisk);
    conv->SetPeriod(8);
    for (TString& name : files)
        conv->FeedFile(name);
    if (conv->GetTempFileOnDisk()) {
        conv->SeparateEventsBySpills();
        conv->OutputSpills();
        //        conv->SeparateEventsBySpillsRDR();
    } else
        conv->SeparateEventsBySpillsTM();
    timer.Stop();
    //    if (decoStatus == kBMNSUCCESS) {
    //        if (!doHoldRawRoot) gSystem->Exec(TString::Format("rm -f %s", decoder->GetRootFileName().Data()));
    printf("\tMacro finished successfully!\n\n");   // marker of successfully execution for software testing systems
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("Real time %f s, CPU time %f s\n", rtime, ctime);
    //    }

    delete conv;
}
