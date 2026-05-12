// construct detector geometry
void geometry(FairRunSim *fRun)
{
    // Set Material file Name
    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo");
    fRun->AddModule(cave);

    //SP-41 Magnet
    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName("magnet_modified.root");
    fRun->AddModule(magnet);

    // SP-57 Magnet
    FairModule* magnetSP57 = new FairMagnet("MAGNET_SP57");
    magnetSP57->SetGeometryFileName("magnetSP57_1.root");
    fRun->AddModule(magnetSP57);

    // -----   Create detectors        -------------------------

    // FairDetector* mwpc = new BmnMwpc("MWPC", kTRUE);
    // mwpc->SetGeometryFileName("MWPC_RunSRCSpring2018.root");
    // fRun->AddModule(mwpc);

    // FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    // silicon->SetGeometryFileName("Silicon_RunSRCSpring2018.root");
    // fRun->AddModule(silicon);





    FairDetector* sts = new CbmSts("STS", kTRUE);
    // sts->SetGeometryFileName("GEMS_RunSRCSpring2018.root");
    // sts->SetGeometryFileName("GEMS_RunSRC2021_detailed.root");
    sts->SetGeometryFileName("GEMS_Yaopeng.root");
    fRun->AddModule(sts);

    FairDetector* csc = new BmnCSC("CSC", kTRUE);
    // csc->SetGeometryFileName("CSC_RunSRCSpring2018.root");
    // csc->SetGeometryFileName("CSC_RunSRC2021.root");
    csc->SetGeometryFileName("CSC_Yaopeng.root");
    fRun->AddModule(csc);
    
    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    // tof1->SetGeometryFileName("TOF400_RUN7_SRC_AllignmentZY_v3.root");
    tof1->SetGeometryFileName("TOF400_Yaopeng.root");
    fRun->AddModule(tof1);

    // FairDetector* tof = new BmnTOF("TOF", kTRUE);
    // tof->SetGeometryFileName("tof700_run8_with_support.root");
    // fRun->AddModule(tof);

    //CSC after SP-41 Magnet
    // FairDetector* csc = new BmnCSC("CSC", kTRUE);
    // // csc->SetGeometryFileName("CSC_After_Magnet_RunSRCSpring2022.root");
    // csc->SetGeometryFileName("CSC_After_Magnet_RunSRCSpring2022_For_Yaopeng.root");
    // fRun->AddModule(csc);


    // FairDetector* tof = new BmnTOF("TOF", kTRUE);
    // tof->SetGeometryFileName("tof700_run7_with_support.root");
    // fRun->AddModule(tof);

    // FairDetector* dch = new BmnDch("DCH", kTRUE);
    // dch->SetGeometryFileName("DCH_RunSpring2018.root");
    // fRun->AddModule(dch);

    // BmnZdc* zdc = new BmnZdc("ZDC", kTRUE);
    // zdc->SetGeometryFileName("ZDC_RunSpring2018.root");
    // fRun->AddModule(zdc);
    
    // FairDetector* arm = new BmnArmTrig("SRCArmTriggers", kTRUE);
    // arm->SetGeometryFileName("SRCArmTriggers_Spring2018.root");
    // fRun->AddModule(arm);

    // FairDetector* bc = new BmnBC("BC", kTRUE);
    // bc->SetGeometryFileName("BC_Yaopeng.root");
    // fRun->AddModule(bc);

}
