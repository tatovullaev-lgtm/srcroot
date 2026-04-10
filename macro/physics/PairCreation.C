// #include <Rtypes.h>
#include <TString.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)

void PairCreation(
                  TString inputFileName = "/scratch1/ramin/bmnroot_final/bmnroot/macro/run/bmndst_mc_latest_0000.root",
                  TString outputFileName = "/scratch1/ramin/bmnroot_final/bmnroot/macro/run/treepair_lambda_mc_latest_1000k_0000.root",
                  UInt_t nEvents = 0, Int_t runId = 8008, Int_t pdg = 3122) // Lambda0 3122, K_S0 310
{

    //Int_t run_period = 8;
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // Create Run Manager
    FairRunAna *fRunAna = new FairRunAna();

    // Input file association
    //FairSource *fFileSource = new BmnFileSource(inReco, run_period, runId);
    //fRunAna->SetSource(fFileSource);

    // Output file association
    //fRunAna->SetSink(new FairRootFileSink(output));

    // Magnetic field
    //UniRun *pCurrentRun = UniRun::GetRun(run_period, runId);
    //if (pCurrentRun == 0)
    //    return;
    //Double_t *field_voltage = pCurrentRun->GetFieldVoltage();
    //Double_t map_voltage = 112.0;
    //Double_t fieldScale = (*field_voltage) / map_voltage;
    //BmnFieldMap *magField = new BmnNewFieldMap("FieldMap_1900_extrap_noPed.root");
    //magField->SetScale(fieldScale);
    //magField->Init();
    //fRunAna->SetField(magField);

    Int_t run_period = 8;
    Int_t fieldType = -1;
    Double_t fieldScale = 0.;
    FairField* magField = BmnFunctionAna::GetDSTField(inputFileName, fieldScale, fieldType);
    if (magField != nullptr) fRunAna->SetField(magField);

    // Download geometry file
    gRandom->SetSeed(0);
    TString geoFileName = Form("current_geo_file_%d.root", UInt_t(gRandom->Integer(UINT32_MAX)));
    Int_t res_code = UniRun::ReadGeometryFile(run_period, runId, (char *)geoFileName.Data());
    TGeoManager::Import(geoFileName);

    //New way
    // Input file association
    FairSource* fFileSource = new BmnFileSource(inputFileName);
    fRunAna->SetSource(fFileSource);

    //fRunAna->SetSource(new FairFileSource(inputFileName));
    // Output file association
    if (BmnFunctionSet::CreateDirectoryTree(outputFileName, 1) < 0) exit(-2);
    fRunAna->SetSink(new FairRootFileSink(outputFileName));
    fRunAna->SetGenerateRunInfo(false);

    //TFile* inFile = TFile::Open(inputFileName.Data());
    //DstRunHeader* runHeader = (DstRunHeader*)inFile->Get("DstRunHeader");
    //Double_t fieldScale = runHeader->GetFieldScale();
    //BmnFieldMap* magField = new BmnNewFieldMap(runHeader->GetFieldName());
    //magField->SetScale(fieldScale);
    //magField->Init();
    //fRunAna->SetField(magField);

    TList* parFileNameList = new TList();


    BmnPairFinder *lambda = new BmnPairFinder(TDatabasePDG::Instance()->GetParticle(pdg));
    // lambda->SetGeometryFile("");

    fRunAna->AddTask(lambda);
    

//    FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
//    FairParRootFileIo* parIo1 = new FairParRootFileIo();
//    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
//    parIo1->open(inputFileName.Data());
//    parIo2->open(parFileNameList, "in");
//    rtdb->setFirstInput(parIo1);
//    rtdb->setSecondInput(parIo2);
//    rtdb->setOutput(parIo1);
//    rtdb->saveOutput();


    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*)inputFileName.Data());
    fRunAna->Init();
    fRunAna->Run(0, nEvents);
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl
         << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    gSystem->Exec(TString::Format("rm -f %s", geoFileName.Data()));

    //delete lambda;
}
