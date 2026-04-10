// @(#)bmnroot/macro/howTo:$Id$
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// readSim.C                                                                  //
//                                                                            //
// An example how to read simulated data (MC) from bmnsim.root                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <fairlogger/Logger.h>

int readSim(TString simFilePath = "")
{
    if (simFilePath == "") {
        LOG(error)<<"Simulation file not specified!";
        return -1;
    }

    // check SIM file exists
    if (!BmnFunctionSet::CheckFileExist(simFilePath, 1))
        return -2;

    // open SIM file with 'bmndata' tree
    TChain* dataChain = new TChain("bmndata");
    dataChain->Add(simFilePath.Data());
    LOG(info)<<"Event Number = "<<dataChain->GetEntries();

    /** assign TClonesArrays to SIM tree branches **/
    // assign event header class to the MC event header
    FairMCEventHeader* event_header = nullptr;
    dataChain->SetBranchAddress("MCEventHeader.", &event_header);
    if (event_header == nullptr)
    {
        LOG(error)<<"'MCEventHeader.' branch was not found, please, check the structure of the file, exiting...";
        return -3;
    }

    // assign TClonesArray to the branch with MC tracks
    TClonesArray* mcTracks = nullptr;
    dataChain->SetBranchAddress("MCTrack", &mcTracks);
    if (mcTracks == nullptr)
    {
        LOG(error)<<"'MCTrack' branch was not found, please, check the structure of the file, exiting...";
        return -4;
    }

    // assign TClonesArrays to branches with track points in the detectors
    TClonesArray* stsPoints = nullptr;
    dataChain->SetBranchAddress("StsPoint", &stsPoints);
    if (stsPoints == nullptr)
    {
        LOG(error)<<"'StsPoint' branch was not found, please, check the structure of the file, exiting...";
        return -5;
    }
    // and other necessary track points in a similar way...

    // assign TClonesArrays to branches with detector digits
    TClonesArray* siliconDigits = nullptr;
    dataChain->SetBranchAddress("BmnSiliconDigit", &siliconDigits);
    if (siliconDigits == nullptr)
    {
        LOG(error)<<"'BmnSiliconDigit' branch was not found, please, check the structure of the file, exiting...";
        return -6;
    }
    // and other necessary detector digits in a similar way...

    // event loop
    for (Int_t iEvent = 0; iEvent < dataChain->GetEntries(); iEvent++)
    {
        LOG(info)<<"Event# "<<iEvent;

        // next event
        dataChain->GetEntry(iEvent);

        // read MC Event Header
        if (event_header)
        {
            cout << "|||||||||||| MC EVENT SUMMARY ||||||||||||||||||||||" << endl;
            cout << "||\t\t\t\t\t\t  ||" << setprecision(3) << endl;
            cout << "||   Event number:     " << event_header->GetEventID() << "\t\t\t  ||" << endl;
            cout << "||   Vertex (x:y:z):   " << event_header->GetX()<<":"<<event_header->GetY()<<":"<<event_header->GetZ() << " cm\t  ||" << endl;
            cout << "||   Impact parameter: " << event_header->GetB()       << " fm\t\t\t  ||" << endl;
            cout << "||   Beam tilt (x:y):  " << event_header->GetRotX()<<""<<event_header->GetRotY() << " rad\t\t  ||" << endl;
            cout << "||   Event plane:      " << event_header->GetRotZ() << " rad\t\t\t  ||" << endl;
            cout << "||   input tracks:     " << event_header->GetNPrim() << "\t\t\t  ||" << endl;
            cout << "||\t\t\t\t\t\t  ||" << endl;
            cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
        }

        // read MC Tracks
        for (Int_t iTrack = 0; iTrack < mcTracks->GetEntriesFast(); iTrack++)
        {
            CbmMCTrack* mcTrack = (CbmMCTrack*) mcTracks->UncheckedAt(iTrack);

            // Get MC track data
            cout<<endl;
            Int_t MotherId = mcTrack->GetMotherId();
            LOG(info)<<"MC Track "<<iTrack<<": mother track ID = "<<MotherId;
            TVector3 Vertex;
            mcTrack->GetStartVertex(Vertex);
            LOG(info)<<"MC Track "<<iTrack<<": vertex = "<<Vertex.X()<<":"<<Vertex.Y()<<":"<<Vertex.Z();
            TVector3 Ptot;
            mcTrack->GetMomentum(Ptot);
            LOG(info)<<"MC Track "<<iTrack<<": momentum = "<<Ptot.X()<<":"<<Ptot.Y()<<":"<<Ptot.Z();
            //TParticle* P = (TParticle*) tr->GetParticle();
            LOG(info)<<"MC Track "<<iTrack<<": particle PDG code = "<<mcTrack->GetPdgCode();
            TParticlePDG* fParticlePDG = TDatabasePDG::Instance()->GetParticle(mcTrack->GetPdgCode());
            Float_t charge = (fParticlePDG != nullptr) ? fParticlePDG->Charge() : 0., mass = -1, ene = -1;
            if (fParticlePDG)
                mass = fParticlePDG->Mass();
            if (mass >= 0)
                ene = TMath::Sqrt(mass*mass + Ptot.Mag2());

            // Put here your code ...
            // See $VMCWORKDIR/base/data/CbmMCTrack.h to get more ...

            // Get STS points and select ones belonging the current track
            for (Int_t iPoint = 0; iPoint < stsPoints->GetEntriesFast(); iPoint++) {
                CbmStsPoint* stsPoint = (CbmStsPoint*) stsPoints->UncheckedAt(iPoint);

                Int_t TrackID = stsPoint->GetTrackID();
                if (TrackID != iTrack)
                    continue;

                // Put here your code ...
                // See $VMCWORKDIR/base/cbm/CbmStsPoint.h to get more ...
            }

            // Get silicon digits and select ones belonging the current track
            for (Int_t iDigit = 0; iDigit < siliconDigits->GetEntriesFast(); iDigit++)
            {
                BmnStripDigit* siliconDigit = (BmnStripDigit*) siliconDigits->UncheckedAt(iDigit); // BmnSiliconDigit
                if (siliconDigit->IsGoodDigit())
                {
                    Int_t stationNumber = siliconDigit->GetStation();
                    Int_t moduleNumber = siliconDigit->GetModule();
                    Int_t layerNumber = siliconDigit->GetStripLayer();
                    Int_t stripNumber = siliconDigit->GetStripNumber();
                    Double_t signalNumber = siliconDigit->GetStripNumber();
                }

                // Put here your code ...
                // See $VMCWORKDIR/detectos/silicon/BmnSiliconDigit.h and $VMCWORKDIR/base/data/BmnStripDigit.h to get more ...
            }
        }
    }

    return true;
}
