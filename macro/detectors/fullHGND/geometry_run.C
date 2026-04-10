// construct detector geometry
void geometry(FairRunSim *fRun)
{
    // Set Material file Name
    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo");
    fRun->AddModule(cave);
    
    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName("magnet_modified.root");
    fRun->AddModule(magnet);

    FairModule* target = new FairTarget("Target");
    target->SetGeometryFileName("target_CsI.geo");
    fRun->AddModule(target);

    FairModule* pipe1 = new FairPipe("Pipe_sec1");
    pipe1->SetGeometryFileName("VacuumPipe_section1_Run8.root");
    fRun->AddModule(pipe1);

    FairModule* pipe2 = new FairPipe("Pipe_sec2");
    pipe2->SetGeometryFileName("VacuumPipe_section2_Run8.root");
    fRun->AddModule(pipe2);

    FairModule* pipe3 = new FairPipe("Pipe_sec3");
    pipe3->SetGeometryFileName("VacuumPipe_section3_Run8.root");
    fRun->AddModule(pipe3);

    // -----   Create detectors        -------------------------

    FairDetector* sibt = new BmnSiBT("SiBT", kTRUE);
    sibt->SetGeometryFileName("SiBT_Run8.root");
    fRun->AddModule(sibt);

    // FairDetector* siprof = new BmnSiProf("SiProf", kTRUE);
    // siprof->SetGeometryFileName("SiProf_Run8.root");
    // fRun->AddModule(siprof);

    // FairDetector* simd = new BmnSiMD("SiMD", kTRUE);
    // simd->SetGeometryFileName("SiMD_run8_v1.root");
    // fRun->AddModule(simd);

    FairDetector* bd = new BmnBd("BD", kTRUE);
    bd->SetGeometryFileName("BD_run8_v1.root");
    fRun->AddModule(bd);

    FairDetector* fd = new BmnFD("FD", kTRUE);
    fd->SetGeometryFileName("FD_run8.root");
    fRun->AddModule(fd);

    FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    //silicon->SetGeometryFileName("Silicon_Run8_mods_6_10_14_18_detailed.root");
    silicon->SetGeometryFileName("Silicon_Run8.root");
    //silicon->SetGeometryFileName("Silicon_Run8_detailed_aligned.root");
    fRun->AddModule(silicon);

    FairDetector* gems = new CbmSts("GEM", kTRUE);
    gems->SetGeometryFileName("GEMS_Run8_detailed.root");
    //gems->SetGeometryFileName("GEMS_Run8_detailed_aligned.root");
    fRun->AddModule(gems);

    FairDetector* csc = new BmnCSC("CSC", kTRUE);
    csc->SetGeometryFileName("FullCSC_Run8_detailed.root");
    fRun->AddModule(csc);

    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    tof1->SetGeometryFileName("TOF400_RUN8_v2.root");
    fRun->AddModule(tof1);
    /*
    FairDetector* dch = new BmnDch("DCH", kTRUE);
    dch->SetGeometryFileName("DCH_Run8.root");
    fRun->AddModule(dch);
    */
    FairDetector* tof2 = new BmnTOF("TOF", kTRUE);
    tof2->SetGeometryFileName("tof700_run8_with_support.root");
    fRun->AddModule(tof2);

    // FairDetector* ecal = new BmnEcal("ECAL", kTRUE);
    // ecal->SetGeometryFileName("ECAL_v3_run8_pos5.root");
    // fRun->AddModule(ecal);
    
    FairDetector* scwall = new BmnScWall("SCWALL", kTRUE);
    //scwall->SetGeometryFileName("ScWall_for_run8_with_hole_with_hole_in_box_cells_numbering_for_run8_Zpos_741.5cm_Xshift_68.70cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");

    scwall->SetGeometryFileName("ScWall_with_hole_with_hole_in_box_cells_numbering_for_run8_Zpos_697.4cm_Xshift_65.65cm_Yshift-0.43cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(scwall);

    FairDetector* hodo = new BmnHodo("HODO", kTRUE);
    //hodo->SetGeometryFileName("Hodo_for_run8_with_box_Zpos_970.2cm_Xshift_64.90cm_Yshift_-1.0cm_rotationY_1.6deg_v1.root");

    hodo->SetGeometryFileName("Hodo_with_box_Zpos_970.2cm_Xshift_64.90cm_Yshift_-1.0cm_rotationY_4.2deg_v1.root");

    fRun->AddModule(hodo);
    
    BmnZdc* vacwall = new BmnZdc("ZDC", kTRUE);
    //vacwall->SetBirk();
    //vacwall->SetGeometryFileName("wall_vacuum_zdc_12x12cm_Zpos_529.286cm_Xshift_-267.034cm_Yshift_0.518cm_rotationY_-27.3deg_v1.root");

    //vacwall->SetGeometryFileName("wall_vacuum_zdc_270x120cm_Zpos_741.39cm_Xshift_68.70cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");

//PROTOTYPE
    //vacwall->SetGeometryFileName("vacBox_ONEDET_for_nDet_PROTOTYPE_12x12cm_15slices_Zdist_595.517cm_Ysh_0.518cm_rotY_-27.30deg_rotX_0.0deg.root");
                               
//RNF
    vacwall->SetGeometryFileName("vacBox_TWODET_for_nDet_RNF_44x44cm_8slices_ZdistDET_1_699.90cm_rotY_-10.0deg_rotX_-4.50deg_ZdistDET_2_699.90cm_rotY_-10.0deg_rotX_4.50deg.root");
                               
    fRun->AddModule(vacwall);
    
    FairDetector* ndet = new BmnNdet("NDET", kTRUE);
    //ndet->SetGeometryFileName("nDet_PROTOTYPE_12x12cm_9mods_4x4cm_VETO_25mm_5sl_PLA_2mm_Pb_8mm_9sl_Cu_30mm_Sc_25mm_G10_2mm_Air_no_hole_Zpos_529.375cm_Xshift_-267.080cm_Yshift_0.518cm_rotationY_-27.3deg_v1.root");//nDet at 27.3 deg.

    //ndet->SetGeometryFileName("nDet_PROTOTYPE_ONEDET_12x12cm_9mods_4x4cm_with_rows_columns_VETO_25mm_5slices_PLA_2mm_Pb_8mm_9slices_Cu_30mm_Sc_25mm_G10_2mm_Air_no_hole_ZdistDET_1_595.617cm_rotY_-27.30deg_rotX_0.0deg.root");//nDet at 27.3 deg.

//PROTOTYPE
    //ndet->SetGeometryFileName("nDet_PROTOTYPE_ONEDET_12x12cm_9mods_4x4cm_with_rows_columns_NEW_NUMBERING_VETO_25mm_5slices_PLA_2mm_Pb_8mm_9slices_Cu_30mm_Sc_25mm_G10_2mm_Air_no_hole_ZdistDET_1_595.617cm_Ysh_0.518cm_rotY_-27.30deg_rotX_0.0deg.root");//nDet at 27.3 deg.
                              
//RNF
    ndet->SetGeometryFileName("nDet_RNF_TWODET_44x44cm_121mods_4x4cm_with_rows_columns_NEW_NUMBERING_VETO_25mm_7slices_Cu_30mm_Sc_25mm_PCB_2mm_PLA_2mm_no_hole_ZdistDET_1_700cm_rotY_-10.0deg_rotX_-4.50deg_ZdistDET_2_700cm_rotY_-10.0deg_rotX_4.50deg.root");

    fRun->AddModule(ndet);

    
    FairDetector* fhcal = new BmnFHCal("FHCAL", kTRUE);
    //zdc->SetBirk();
    //fhcal->SetGeometryFileName("FHCal_for_run8_CBM_20mods_NICA_34mods_54mods_hole_Zpos_977.8cm_Xshift_65.30cm_Yshift-0.8cm_rotationY_1.6deg_v1.root");

    fhcal->SetGeometryFileName("FHCal_CBM_20mods_NICA_34mods_54mods_hole_Zpos_977.8cm_Xshift_65.30cm_Yshift-0.8cm_rotationY_4.2deg_v1.root");

    fRun->AddModule(fhcal);
    
}


