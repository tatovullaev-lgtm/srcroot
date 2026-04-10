/*
 * File:   BmnScWallDigitizer.cxx
 * Author: Sergey Morozov
 *
 * Created on 16.09.2021, 12:00
 */

#include "BmnScWallDigitizer.h"

#include "BmnScWallAddress.h"
#include "BmnScWallPoint.h"
#include "FairRootManager.h"

#include <FairRun.h>
#include <FairRunSim.h>

BmnScWallDigitizer::BmnScWallDigitizer()
{

    fArrayOfScWallDigits = nullptr;

    fGeV2MIP = 0.005;
    fMIP2Pix = 55.;
    fMIPNoise = 0.2;
}

BmnScWallDigitizer::~BmnScWallDigitizer()
{

    if (fArrayOfScWallDigits) {
        fArrayOfScWallDigits->Delete();
        delete fArrayOfScWallDigits;
    }
}

InitStatus BmnScWallDigitizer::Init()
{

    FairRootManager* ioman = FairRootManager::Instance();
    fArrayOfScWallPoints = (TClonesArray*)ioman->GetObject("ScWallPoint");
    if (!fArrayOfScWallPoints) {
        cout << "BmnScWallDigitizer::Init(): branch ScWallPoint not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fArrayOfScWallDigits = new TClonesArray("BmnScWallDigit");
    ioman->Register("ScWallDigit", "ScWall", fArrayOfScWallDigits, kTRUE);

    LOG(detail) << "ScWall digitizer ready";
    return kSUCCESS;
}

void BmnScWallDigitizer::Exec(Option_t* opt)
{

    if (!IsActive())
        return;

    // Initialize
    fArrayOfScWallDigits->Delete();

    float eSumCell[BmnScWallGeoPar::fTotalCells + 1];

    for (Int_t i = 0; i <= BmnScWallGeoPar::fTotalCells; i++)
        eSumCell[i] = 0;

    // Collect points
    Int_t N = fArrayOfScWallPoints->GetEntries();

    for (Int_t i = 0; i < N; i++) {
        BmnScWallPoint* p = (BmnScWallPoint*)fArrayOfScWallPoints->At(i);

        Int_t iCell = p->GetCopyMother();

        if (iCell <= BmnScWallGeoPar::fTotalCells) {
            // collect and sum up energy losses for sections
            eSumCell[iCell] += p->GetEnergyLoss();
        } else {
            LOG(error) << Form("ScWall cell %d ignored", iCell);
        }
    }

    // Digitize SiPM response and store digits

    Double_t eSumScWall = 0.;
    Double_t eSumScWallMC = 0.;

    for (Int_t iCell = 1; iCell <= BmnScWallGeoPar::fTotalCells; iCell++) {

        Double_t eSumGeant = eSumCell[iCell];
        Double_t eSumMIP = eSumGeant / fGeV2MIP;   // convert energy to MIP
        Double_t eSumPix = eSumMIP * fMIP2Pix;     // convert MIP to Npix in SiPM
        Double_t eSumMIPSmeared = gRandom->Gaus(eSumPix, sqrt(eSumPix)) / fMIP2Pix;
        Double_t eMIPNoise = gRandom->Gaus(0, fMIPNoise);   // MIP electronic noise
        eSumMIPSmeared += eMIPNoise;
        Double_t eSumSmeared = eSumMIPSmeared * fGeV2MIP;   // from MIP to energy

        Double_t amp = eSumSmeared * fScale;

        // if (amp == 0.) continue;

        if (iCell <= BmnScWallGeoPar::fSmallCells) {
            if (amp < fSmallModThreshold)
                continue;
        } else {
            if (amp < fLargeModThreshold)
                continue;
        }

        eSumScWallMC += eSumGeant;
        eSumScWall += amp;

        auto address = BmnScWallAddress::GetAddress(iCell);
        /*BmnScWallDigit * p = */
        new ((*fArrayOfScWallDigits)[fArrayOfScWallDigits->GetEntriesFast()]) BmnScWallDigit(address, 0, amp);
    }   // for (Int_t iCell
}
