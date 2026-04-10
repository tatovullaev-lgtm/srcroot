/** @file BmnVacWallReconstructor.cxx
 ** @author Nikolay Karpushkin <karpushkin@inr.ru>
 ** @date 03.04.2021
 **
 ** Code for BmnVacWall points info transition on reconstruction stage
 **/

#include "BmnVacWallReconstructor.h"

InitStatus BmnVacWallReconstructor::Init()
{
    fworkTime = 0.;
    fpFairRootMgr = FairRootManager::Instance();
    fArrayOfPoints = (TClonesArray*)fpFairRootMgr->GetObject("VacWallPoint");

    if (!fArrayOfPoints) {
        std::cout << "BmnVacWallReconstructor::Init(): branch with Digits not found! Task will be deactivated"
                  << std::endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fpFairRootMgr->Register("VacWallPoint", "VacWall", fArrayOfPoints, kTRUE);

    LOG(detail) << "VacWall Reconstructor ready";
    return kSUCCESS;
}

void BmnVacWallReconstructor::Exec(Option_t* opt)
{

    if (!IsActive())
        return;

    TStopwatch sw;
    sw.Start();
    sw.Stop();
    fworkTime += sw.RealTime();
}

void BmnVacWallReconstructor::OnlineWrite(const std::unique_ptr<TTree>& resultTree)
{
    if (!IsActive())
        return;

    resultTree->Branch("VacWallPoint", &fArrayOfPoints);
    resultTree->Fill();
}

void BmnVacWallReconstructor::Finish()
{
    printf("Work time of BmnVacWallReconstructor: %4.2f sec.\n", fworkTime);
}
