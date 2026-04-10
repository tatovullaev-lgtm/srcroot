// ceph/bmn/ssd/tmp/afi_test_20250521
// c5n12: Top-0 #21323; Top-1 #21325
// c5n21: Top-2 #21326; Top-3 #21327; Top-4 #21328

void monitor(Int_t periodID = 8,
             TString dirName = ".",
             TString refDir = ".",
             TString decoAddr = "localhost",
             Int_t webPort = 9000)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5, 99, 99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    //    bmnloadlibs();
    //    gSystem->Load("libBmnMonitor");
    BmnMonitor* bm = new BmnMonitor();
    bm->SetPeriodID(periodID);
    bm->MonitorStreamZ(dirName, refDir, decoAddr, webPort);
    delete bm;
}
