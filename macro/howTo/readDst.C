// @(#)bmnroot/macro/howTo:$Id$
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// readDst.C                                                                  //
//                                                                            //
// An example how to read reconstructed data (DST) from bmndst.root           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <RtypesCore.h>
#include <fairlogger/Logger.h>

enum enumTracking
{
    VectorFinder,
    L1Tracking
};

int readDst(TString dstFilePath = "", enumTracking tracking = VectorFinder)
{
    if (dstFilePath == "") {
        LOG(error) << "DST file not specified!";
        return -1;
    }

    // check DST file exists
    if (!BmnFunctionSet::CheckFileExist(dstFilePath, 1))
        return -2;

    // open DST file with 'bmndata' tree
    TChain* dataChain = new TChain("bmndata");
    dataChain->Add(dstFilePath.Data());
    LOG(info) << "Event Number = " << dataChain->GetEntries();

    // read DST Run Header if present
    DstRunHeader* run_header = (DstRunHeader*)dataChain->GetCurrentFile()->Get("DstRunHeader");
    if (run_header) {
        Int_t run_period = run_header->GetPeriodId();
        Double_t fieldScale = run_header->GetFieldScale();
        cout << "|||||||||||| DST RUN SUMMARY |||||||||||||||" << endl;
        cout << "||\t\t\t\t\t  ||" << endl;
        cout << "||   Period:        " << run_period << "\t\t\t  ||" << endl;
        cout << "||   Number:        " << run_header->GetRunId() << "\t\t\t  ||" << endl;
        cout << "||   Start Time:    " << run_header->GetStartTime().AsString("s") << "\t  ||" << endl;
        cout << "||   End Time:      " << run_header->GetFinishTime().AsString("s") << "\t  ||" << endl;
        cout << "||   Beam:          A = " << run_header->GetBeamA() << ", Z = " << run_header->GetBeamA() << "\t  ||"
             << endl;
        cout << "||   Beam energy:   " << run_header->GetBeamEnergy() << " GeV\t\t  ||" << endl;
        cout << "||   Target:        A = " << run_header->GetTargetA() << ", Z = " << run_header->GetTargetZ()
             << "\t  ||" << endl;
        Double_t map_current = (run_period == 8) ? 112.0 : (run_period == 7) ? 55.87 : 0.0;
        Double_t field_voltage = fieldScale * map_current;
        cout << "||   Field voltage: " << setprecision(4) << field_voltage << " mV\t\t  ||" << endl;
        cout << "||\t\t\t\t\t  ||" << endl;
        cout << "||||||||||||||||||||||||||||||||||||||||||||" << endl << endl;
    }

    /** assign TClonesArrays to DST tree branches **/
    // assign event header class to the DST event header
    DstEventHeader* event_header = nullptr;
    dataChain->SetBranchAddress("DstEventHeader.", &event_header);
    if (event_header == nullptr) {
        LOG(error) << "'DstEventHeader.' branch was not found, please, check the structure of the DST file, exiting...";
        return -3;
    }

    // assign CbmVertex object to the branch with reconstructed primary vertex
    CbmVertex* primaryVertex = nullptr;
    dataChain->SetBranchAddress("PrimaryVertex.", &primaryVertex);
    if (primaryVertex == nullptr) {
        LOG(error) << "'PrimaryVertex.' branch was not found, please, check the structure of the DST file, exiting...";
        return -4;
    }

    // assign TClonesArray to the branch with reconstructed global tracks
    TClonesArray* globalTracks = nullptr;
    dataChain->SetBranchAddress("BmnGlobalTrack", &globalTracks);
    if (globalTracks == nullptr) {
        LOG(error) << "'BmnGlobalTrack' branch was not found, please, check the structure of the DST file, exiting...";
        return -5;
    }

    // assign TClonesArrays to branches with local segments of tracks in the detectors and their hits
    // TClonesArray* gemTracks = nullptr;
    // dataChain->SetBranchAddress("BmnGemTrack", &gemTracks);
    // if (gemTracks == nullptr)
    //{
    //    LOG(error)<<"'BmnGemTrack' branch was not found, please, check the structure of the DST file, exiting...";
    //    return -6;
    //}
    // TClonesArray* gemHits = nullptr;
    // dataChain->SetBranchAddress("BmnGemStripHit", &gemHits);
    // if (gemHits == nullptr)
    //{
    //    LOG(error)<<"'BmnGemStripHit' branch was not found, please, check the structure of the DST file, exiting...";
    //    return -7;
    //}

    TString stsBranchName = "StsVector";
    if (tracking == L1Tracking)
        stsBranchName = "StsTrack";
    TClonesArray* stsTracks = nullptr;
    dataChain->SetBranchAddress(stsBranchName, &stsTracks);
    if (stsTracks == nullptr) {
        LOG(error) << "'StsTrack' branch was not found, please, check the structure of the DST file, exiting...";
        return -8;
    }
    TClonesArray* stsHits = nullptr;
    dataChain->SetBranchAddress("StsHit", &stsHits);
    if (stsHits == nullptr) {
        LOG(error) << "'StsHit' branch was not found, please, check the structure of the DST file, exiting...";
        return -9;
    }

    TClonesArray* tof400Hits = nullptr;
    dataChain->SetBranchAddress("BmnTof400Hit", &tof400Hits);
    if (tof400Hits == nullptr) {
        LOG(error) << "'BmnTof400Hit' branch was not found, please, check the structure of the DST file, exiting...";
        return -10;
    }

    TClonesArray* tof700Hits = nullptr;
    dataChain->SetBranchAddress("BmnTof700Hit", &tof700Hits);
    if (tof700Hits == nullptr) {
        LOG(error) << "'BmnTof700Hit' branch was not found, please, check the structure of the DST file, exiting...";
        return -11;
    }

    TClonesArray* cscHits = nullptr;
    dataChain->SetBranchAddress("BmnCSCHit", &cscHits);
    if (cscHits == nullptr) {
        LOG(error) << "'BmnCSCHit' branch was not found, please, check the structure of the DST file, exiting...";
        return -12;
    }

    TClonesArray* dchTracks = nullptr;
    dataChain->SetBranchAddress("BmnDchTrack", &dchTracks);
    if (dchTracks == nullptr) {
        LOG(warn)
            << "'BmnDchTrack' branch was not found";   //, please, check the structure of the DST file, exiting...";
        // return -13;
    }

    // and other necessary detector track segments and hits in a similar way...

    // event loop
    for (Int_t iEvent = 0; iEvent < dataChain->GetEntries(); iEvent++) {
        LOG(info) << "Event# " << iEvent;

        // Next Event
        dataChain->GetEntry(iEvent);

        // Getting DST event header
        if (event_header) {
            cout << "|||||||||||| DST EVENT SUMMARY |||||||||||||" << endl;
            cout << "||\t\t\t\t\t  ||" << endl;
            cout << "||   Event number:     " << event_header->GetEventId() << "\t\t  ||" << endl;
            cout << "||   Impact parameter: " << event_header->GetB() << " fm\t\t  ||" << endl;
            cout << "||\t\t\t\t\t  ||" << endl;
            cout << "||||||||||||||||||||||||||||||||||||||||||||" << endl;

            // See $VMCWORKDIR/base/header/BmnEventHeader.h to get more ...
        }

        if (primaryVertex) {
            LOG(info) << "VERTEX INFO (X:Y:Z) = "
                      << TString::Format("%.3f : %.3f : %.3f\n", primaryVertex->GetX(), primaryVertex->GetY(),
                                         primaryVertex->GetZ());

            // See $VMCWORKDIR/base/cbm/CbmVertex.h to get more ...
        }

        // Getting vertices in the event: 0 -- primary, 1 -- secondary
        // for (Int_t iVertex = 0; iVertex < vertices->GetEntriesFast(); iVertex++) {
        //    BmnVertex* vertex = (BmnVertex*) vertices->UncheckedAt(iVertex);

        // See $VMCWORKDIR/base/data/BmnVertex.h to get more ...
        //}

        // Getting "local" tracks, i.e. the segments of the global tracks in the detectors
        for (Int_t iTrack = 0; iTrack < stsTracks->GetEntriesFast(); iTrack++) {
            CbmStsTrack* track = (CbmStsTrack*)stsTracks->UncheckedAt(iTrack);

            // See $VMCWORKDIR/base/cbm/CbmStsTrack.h to get more ...
        }

        // Getting global tracks and their corresponding components
        for (Int_t iTrack = 0; iTrack < globalTracks->GetEntriesFast(); iTrack++) {
            BmnGlobalTrack* track = (BmnGlobalTrack*)globalTracks->UncheckedAt(iTrack);

            // Getting track params. predicted by Kalman filter ...
            FairTrackParam* parFirst = track->GetParamFirst();
            FairTrackParam* parLast = track->GetParamLast();

            // See YOUR_FAIRROOT_INSTALATION/include/FairTrackParam.h to get more ...

            // STS tracks ...
            if (track->GetSilTrackIndex() != -1) {
                CbmStsTrack* stsTr = (CbmStsTrack*)stsTracks->UncheckedAt(track->GetSilTrackIndex());

                // Hits on STS track
                for (Int_t iHit = 0; iHit < stsTr->GetNStsHits(); iHit++) {
                    CbmStsHit* hit = (CbmStsHit*)stsHits->UncheckedAt(stsTr->GetStsHitIndex(iHit));

                    // See $VMCWORKDIR/detectors/sts/CbmStsHit.h to get more ...
                }
            }

            // TOF400 matched hit ...
            if (track->GetTof1HitIndex() != -1) {
                BmnTofHit* hit = (BmnTofHit*)tof400Hits->UncheckedAt(track->GetTof1HitIndex());

                // See $VMCWORKDIR/bmnd/BmnTofHit.h to get more ...
            }

            // TOF700 matched hit ...
            if (track->GetTof2HitIndex() != -1) {
                BmnTofHit* hit = (BmnTofHit*)tof700Hits->UncheckedAt(track->GetTof2HitIndex());

                // See $VMCWORKDIR/detectprs/tof/BmnTofHit.h to get more ...
            }

            // CSC matched hits ...
            for (Int_t indHit : track->GetVectorCscHitIndices())
                if (indHit != -1) {
                    BmnCSCHit* hit = (BmnCSCHit*)cscHits->UncheckedAt(indHit);

                    // See $VMCWORKDIR/detectors/csc/BmnCSCHit.h to get more ...
                }

            // DCH matched track
            if (track->GetDchTrackIndex() != -1) {
                BmnDchTrack* dchTrack = (BmnDchTrack*)dchTracks->UncheckedAt(track->GetDchTrackIndex());

                // See $VMCWORKDIR/detectors/dch/BmnDchTrack.h to get more ...

                FairTrackParam* parFirstDch = dchTrack->GetParamFirst();

                // See YOUR_FAIRROOT_INSTALATION/include/FairTrackParam.h to get more ...
            }
        }   // for (Int_t iTrack = 0; iTrack < globalTracks->GetEntriesFast(); iTrack++)
    }   // for (Int_t iEvent = 0; iEvent < out->GetEntries(); iEvent++)

    return 0;
}
