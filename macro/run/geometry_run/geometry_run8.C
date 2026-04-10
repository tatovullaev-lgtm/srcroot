// construct detector geometry
void geometry(FairRunSim* fRun)
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
    // silicon->SetGeometryFileName("Silicon_Run8.root");
    silicon->SetGeometryFileName("Silicon_Run8_detailed.root");
    fRun->AddModule(silicon);

    FairDetector* gems = new CbmSts("GEM", kTRUE);
    gems->SetGeometryFileName("GEMS_Run8_detailed.root");
    // gems->SetGeometryFileName("GEMS_Run8_detailed_aligned.root");
    fRun->AddModule(gems);

    FairDetector* csc = new BmnCSC("CSC", kTRUE);
    csc->SetGeometryFileName("FullCSC_Run8_detailed.root");
    fRun->AddModule(csc);

    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    tof1->SetGeometryFileName("TOF400_RUN8_v3.root");
    fRun->AddModule(tof1);

    FairDetector* dch = new BmnDch("DCH", kTRUE);
    dch->SetGeometryFileName("DCH_Run8.root");
    fRun->AddModule(dch);

    FairDetector* tof2 = new BmnTOF701("TOF", kTRUE);
    tof2->SetGeometryFileName("tof701_run8_v2.root");
    fRun->AddModule(tof2);

    // FairDetector* ecal = new BmnEcal("ECAL", kTRUE);
    // ecal->SetGeometryFileName("ECAL_v3_run8_pos5.root");
    // fRun->AddModule(ecal);

    FairDetector* scwall = new BmnScWall("SCWALL", kTRUE);
    scwall->SetGeometryFileName("ScWall_for_run8_with_hole_with_hole_in_box_cells_numbering_for_run8_Zpos_741.5cm_"
                                "Xshift_68.70cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(scwall);

    FairDetector* hodo = new BmnHodo("HODO", kTRUE);
    hodo->SetGeometryFileName("Hodo_run8_v2.root");
    fRun->AddModule(hodo);

    FairDetector* vacwall = new BmnVacWall("VacWall", kTRUE);
    vacwall->SetGeometryFileName("vacBox_ONEDET_for_nDet_PROTOTYPE_12x12cm_15slices_Zdist_595.517cm_Ysh_0.518cm_rotY_-"
                                 "27.30deg_rotX_0.0deg.root");
    // vacwall->SetGeometryFileName("vacBox_ONEDET_for_nDet_PROTOTYPE_12x12cm_15slices_Zdist_838.4cm_Ysh_0.52cm_rotY_"
    //                            "0.70deg_rotX_0.0deg.root");

    fRun->AddModule(vacwall);

    FairDetector* ndet = new BmnNdet("NDET", kTRUE);
    // nDet at -27.3 deg.
    ndet->SetGeometryFileName(
        "nDet_PROTOTYPE_ONEDET_Planes_ZdistDET_1_595.617cm_Ysh_0.518cm_rotY_-27.30deg_rotX_0.0deg.root");

    // nDet at 0.7 deg.
    // ndet->SetGeometryFileName(
    //"nDet_PROTOTYPE_ONEDET_Planes_ZdistDET_1_838.50cm_Ysh_0.52cm_rotY_0.70deg_rotX_0.0deg.root");

    fRun->AddModule(ndet);

    FairDetector* fhcal = new BmnFHCal("FHCAL", kTRUE);
    // zdc->SetBirk();
    fhcal->SetGeometryFileName("FHCal_for_run8_CBM_20mods_NICA_34mods_54mods_hole_Zpos_977.8cm_Xshift_65.30cm_Yshift-0."
                               "8cm_rotationY_1.6deg_v1.root");
    fRun->AddModule(fhcal);
}
