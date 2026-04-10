// #include <Rtypes.h>
#include <TString.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)

void run_qa_exp(TString inFileName = "", TString outFileName = "", Int_t nStartEvent = 0, Int_t nEvents = 0) {

    TStopwatch timer;
    timer.Start();

    // check input file exists
    if (!BmnFunctionSet::CheckFileExist(inFileName, 1)) exit(-1);

    FairRunAna* fRunAna = new FairRunAna();

    Int_t fieldType = -1;
    Double_t fieldScale = 0.;
    FairField* magField = BmnFunctionAna::GetDSTField(inFileName, fieldScale, fieldType);
    if (magField != nullptr) fRunAna->SetField(magField);

    // flag for FairRunAna tasks whether simulation or experimental data are used
    Bool_t isExp = (BmnFunctionSet::isSimulationFile(inFileName) == 0);

    // Download geometry file
    // gRandom->SetSeed(0);
    // TString geoFileName = Form("current_geo_file_%d.root", UInt_t(gRandom->Integer(UINT32_MAX)));
    // Int_t res_code = UniRun::ReadGeometryFile(8, 8000, (char*)geoFileName.Data());
    TGeoManager::Import("current_geo_file.root");

    // Declare input source as simulation file or experimental data
    FairFileSource* fFileSource = new FairFileSource(inFileName);
    fRunAna->SetSource(fFileSource);

    // if directory for the output file does not exist, then create
    if (BmnFunctionSet::CreateDirectoryTree(outFileName, 1) < 0) exit(-2);
    fRunAna->SetSink(new FairRootFileSink(outFileName));
    fRunAna->SetGenerateRunInfo(false);

    FairTask* pKF = new CbmKF();
    fRunAna->AddTask(pKF);
    
    // BmnTof400Qa* tof400QaTask = new BmnTof400Qa();
    // fRunAna->AddTask(tof400QaTask);

    BmnTof700Qa* tof700QaTask = new BmnTof700Qa();
    fRunAna->AddTask(tof700QaTask);

    fRunAna->Init();
    fRunAna->Run(0, nEvents);

    //remove(geoFileName.Data());

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;

}