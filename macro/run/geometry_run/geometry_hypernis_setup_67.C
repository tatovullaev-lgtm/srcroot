// construct detector geometry
void geometry(FairRunSim *fRun)
{
    // Set Material file Name
    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo"); //air
    //cave->SetGeometryFileName("cave_JK.geo"); //JK, vacuum
    fRun->AddModule(cave);

    FairModule* magnet = new FairMagnet("MAGNET");
    //magnet->SetGeometryFileName("magnetSP40_geometry_qi.root");
    magnet->SetGeometryFileName("magnet_SP40_1_DK_67cm.root");
    fRun->AddModule(magnet);

/*
    FairModule* magnet = new FairMagnet("magnetSP57");
//    FairModule* target = new FairTarget("Target");
//    target->SetGeometryFileName("target_CsI.geo");
    magnet->SetGeometryFileName("magnetSP57_1.root");
    fRun->AddModule(magnet);
*/

    FairModule* target = new FairMagnet("magnetSP57");
//    FairModule* target = new FairTarget("Target");
//    target->SetGeometryFileName("target_CsI.geo");
    target->SetGeometryFileName("magnetSP57_1.root");
    fRun->AddModule(target);



//    FairModule* magnet2 = new FairMagnet("MAGNET2");
//    magnet2->SetGeometryFileName("magnet_SP40_2_DK_50cm.root");
//    fRun->AddModule(magnet2);
    
    // -----   Create detectors        -------------------------

//    FairDetector* M12 = new BmnBC("M12", kTRUE);
//    M12->SetGeometryFileName("M12_hypernis.root");
//    fRun->AddModule(M12);

//YN
//    FairDetector* PNPICh = new BmnBC("PNPICh", kTRUE);
//    PNPICh->SetGeometryFileName("PNPICh_hypernis.root");
//    fRun->AddModule(PNPICh);

//    FairDetector* GroupA = new BmnBC("GroupA", kTRUE);
//    GroupA->SetGeometryFileName("GroupA_hypernis.root");
//    fRun->AddModule(GroupA);

//    FairDetector* GroupB = new BmnBC("GroupB", kTRUE);
//    GroupB->SetGeometryFileName("GroupB_hypernis.root");
//    fRun->AddModule(GroupB);

//    FairDetector* GroupC = new BmnBC("GroupC", kTRUE);
//    GroupC->SetGeometryFileName("GroupC_hypernis.root");
//    fRun->AddModule(GroupC);
    
//    FairDetector* GroupC56 = new BmnBC("GroupC56", kTRUE);
//    GroupC56->SetGeometryFileName("GroupC56_hypernis.root");
//    fRun->AddModule(GroupC56);

//YN
    FairDetector* csc = new BmnCSC("CSC", kTRUE);
//    csc->SetGeometryFileName("FullCSC_Run8_detailed.root");
//YN 27022026    csc->SetGeometryFileName("CSC_RunSRCSpring2018.root"); // OK with CSC points
//wrk report    csc->SetGeometryFileName("CSC_for_SRC_PNPI.root"); // OK with CSC points
    csc->SetGeometryFileName("CSC_for_SRC_PNPI_2arms.root"); // OK with CSC points
//wrc    csc->SetGeometryFileName("CSC_Yaopeng.root"); // no CSC points
//    csc->SetGeometryFileName("create_rootgeom_CSC_RunSRCSpring2018.root");
//    csc->SetGeometryFileName("create_rootgeom_FullCSC_Run8_detailed.root");
    fRun->AddModule(csc);

//    FairDetector* mwpc = new BmnMwpc("MWPC", kTRUE);
//    mwpc->SetGeometryFileName("MWPC_RunSpring2018.root");
//    mwpc->SetGeometryFileName("MWPC_RunSRCSpring2018.root");
//    mwpc->SetGeometryFileName("MWPC_RunSRCSpring2018.root");
//    fRun->AddModule(mwpc);

//    FairDetector* PnpiCh = new BmnPnpiCh("PnpiCh", kTRUE);
//    mwpc->SetGeometryFileName("MWPC_RunSpring2018.root");
//    fRun->AddModule(PnpiCh);

//     FairDetector* tofwall = new BmnTOFWall("SRCTOFWall", kTRUE);
//     tofwall->SetGeometryFileName("TOFWALL_hypernis.root");
//     tofwall->SetGeometryFileName("TOF400_RUN7_SRC_AllignmentZY_v3.root");
//     fRun->AddModule(tofwall);
  
//YN    FairDetector* activeplanes = new BmnActivePlane("SRCActivePlanes", kTRUE);
//YN    activeplanes->SetGeometryFileName("ACTIVEPLANES_hypernis_APs.root");
//YN    fRun->AddModule(activeplanes);

    
    
    // FairDetector* mwpc = new BmnMwpc("MWPC", kTRUE);
    // mwpc->SetGeometryFileName("MWPC_RunSRCSpring2018.root");
    //fRun->AddModule(mwpc);
    
    // FairDetector* M1 = new BmnBC("BC1", kTRUE);
    // M1->SetGeometryFileName("M1_hypernis.root");
    // fRun->AddModule(M1);

    // FairDetector* M2 = new BmnBC("BC2", kTRUE);
    // M2->SetGeometryFileName("M2_hypernis.root");
    // fRun->AddModule(M2);

    // FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    // //silicon->SetGeometryFileName("Silicon_RunSRCSpring2018.root");
    // silicon->SetGeometryFileName("SILICON_hypernis.root");
    // // fRun->AddModule(silicon);

    // FairDetector* sts = new CbmSts("STS", kTRUE);
    // //orig    sts->SetGeometryFileName("GEMS_RunSRCSpring2018.root");
    // sts->SetGeometryFileName("GEMS_FutureConfig2020.root");
    // //fRun->AddModule(sts);

    // FairDetector* csc = new BmnCSC("CSC", kTRUE);
    // csc->SetGeometryFileName("CSC_RunSRCSpring2018.root");
    //fRun->AddModule(csc);

    // FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    // tof1->SetGeometryFileName("TOF400_RUN7_SRC_AllignmentZY_v3.root");
    //fRun->AddModule(tof1);

    // FairDetector* tof = new BmnTOF("TOF", kTRUE);
    // tof->SetGeometryFileName("tof700_run7_with_support.root");
    //fRun->AddModule(tof);

    // FairDetector* dch = new BmnDch("DCH", kTRUE);
    // //dch->SetGeometryFileName("DCH_RunSpring2018.root");
    // dch->SetGeometryFileName("DCH_MoveSP40.root");
    //fRun->AddModule(dch);

    // BmnZdc* zdc = new BmnZdc("ZDC", kTRUE);
    // zdc->SetGeometryFileName("ZDC_RunSpring2018.root");
    //fRun->AddModule(zdc);
    
    //FairDetector* arm = new BmnArmTrig("SRCArmTriggers", kTRUE);
    //arm->SetGeometryFileName("SRCArmTriggers_Spring2018.root");
    //fRun->AddModule(arm);
    
    //FairDetector* calo = new BmnCalorimeter("SRCCalorimeter", kTRUE);
    //calo->SetGeometryFileName("CALORIMETER_hypernis.root");
    //calo->SetGeometryFileName("TOF700_RUN8.root");
    //fRun->AddModule(calo);
    
    //FairDetector* activeplane = new BmnActivePlane("SRCActivePlane", kTRUE);
    //activeplane->SetGeometryFileName("ACTIVEPLANE_Frag1_hypernis.root");
    //fRun->AddModule(activeplane);
    
    //FairDetector* activeplane2 = new BmnActivePlane("SRCActivePlane2", kTRUE);
    //activeplane2->SetGeometryFileName("ACTIVEPLANE_Recoil_hypernis.root");
    //fRun->AddModule(activeplane2);

}
