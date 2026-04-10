
#include <FairLogger.h>

// file: full path to raw data file
// nEvents: if 0 then convert all events

void BmnDataToRaw(TString file = "", TString outfile = "", Long_t nEvents = 0, UInt_t period = 8, Int_t port = 7777)
{
    gSystem->ExpandPathName(file);
    gSystem->ExpandPathName(outfile);

    Int_t iVerbose = 1;   ///<- Verbosity level: 0 - Progress Bar; 1 - short info on passed events
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");

    TStopwatch timer;
    timer.Start();

    BmnRawSource* source = new BmnRawSource(file, outfile, nEvents, period, false);
    source->SetVerbose(iVerbose);
    source->SetDecoderConfigFileName(string(getenv("VMCWORKDIR")) + "/config/bmnconf.json");
    source->ConvertRawToRoot();

    timer.Stop();
    printf("\tMacro finished successfully!\n\n");   // marker of successfully execution for software testing systems
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("Real time %f s, CPU time %f s\n", rtime, ctime);
}
