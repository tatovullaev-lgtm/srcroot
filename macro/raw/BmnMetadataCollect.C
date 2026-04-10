

void BmnMetadataCollect(vector<string> files = {},
                        string input_digi_dir = "",
                        string outfile = "bmn_run9092_meta.root",
                        UInt_t run_id = 9092,
                        UInt_t period = 9)
{
    gSystem->ExpandPathName(outfile.data());
    //        ROOT::EnableImplicitMT();
    FairLogger::GetLogger()->SetLogToScreen(true);
    FairLogger::GetLogger()->SetLogScreenLevel("info");   // for info write "INFO"

    TStopwatch timer;
    TStopwatch timer_conv;
    timer.Start();
    // Vasiliy: для рана 7830 N_BT х (BTprescale+1) = 25869*(1999 + 1) = 51738000
    string tar_dir = "/ncx/eos/nica/bmn/exp/digi/run8/24.04.0/0_technichal_info/";
    //     string dig_dir = ".";
    //     string dig_dir = "/mnt/ddc/eos/lpp/nica/bmn/exp/digi/run8/25.09.0/";
    //  string dig_dir = "/home/ilnur/filesbmn/";
    //      string dig_dir = "/ncx/eos/nica/bmn/users/ilnur/spill_test/";
    //        string dig_dir = "/eos/lpp/nica/bmn/exp/digi/run8/25.04.0/;
    string dig_dir = "/home/ilnur/filesbmn/spill_test/";
    //        string dig_dir = "/data/spill_test/";
    //  string dig_dir = "/mnt/ddc/ceph/bmn/work/ilnur/spill_test9/";
    //     string dig_dir = "/mnt/ddc/eos/lpp/nica/bmn/users/ilnur/spill_test/";

    BmnMetadataDecoder* mdd = new BmnMetadataDecoder(files, outfile, period);
    BmnSetup stp = kBMNSETUP;
    mdd->SetRunId(run_id);   // 8008
    TString PeriodSetupExt = Form("%d%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"));
    mdd->SetMSCMapping(TString("MSC_map_Run") + PeriodSetupExt);
    if (files.size() == 0)
        mdd->SetDigiDirectory((input_digi_dir.length() > 0) ? input_digi_dir : dig_dir);
    //    mdd->SetRawRootDirPath("/dev/shm/temp_raw/");
    mdd->SetBmnSetup(stp);

    // Set converter/decoder general parameters
    mdd->SetDecoderConfigFileName(string(getenv("VMCWORKDIR")) + "/config/bmnconf.json");

    BmnStatus JStatusStatus = mdd->ProcessStatusBlocks();
    BmnStatus SpillTreeStatus = mdd->ProcessSpillTree();
    mdd->SaveData();
    timer.Stop();
    if ((JStatusStatus == kBMNSUCCESS) && (SpillTreeStatus == kBMNSUCCESS)) {
        printf("\tMacro finished successfully!\n\n");
        Double_t rtime = timer.RealTime();
        Double_t ctime = timer.CpuTime();
        printf("All steps: Real time %f s, CPU time %f s\n", rtime, ctime);
    }
    delete mdd;
}
