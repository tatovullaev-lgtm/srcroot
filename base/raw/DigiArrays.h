#ifndef DIGIARRAYS_H
#define DIGIARRAYS_H 1

#include "BmnEventHeader.h"
#include "TClonesArray.h"

#include <vector>

class DigiArrays : public TObject
{
  public:
    DigiArrays()
        : silicon(nullptr)
        , gem(nullptr)
        , csc(nullptr)
        , vsp(nullptr)
        , tof400(nullptr)
        , tof700(nullptr)
        , tof701(nullptr)
        , zdc(nullptr)
        , scwall(nullptr)
        , fhcal(nullptr)
        , hodo(nullptr)
        , ndet(nullptr)
        , hgnd(nullptr)
        , ecal(nullptr)
        , dch(nullptr)
        , mwpc(nullptr)
        , sibt(nullptr)
        , siprof(nullptr)
        , msc(nullptr)
        , trigAr(nullptr)
        , header(nullptr)
    {}

    ~DigiArrays() {}

    void Clear()
    {
        if (dch) {
            dch->Delete();
            delete dch;
        }
        if (gem) {
            gem->Delete();
            delete gem;
        }
        if (csc) {
            csc->Delete();
            delete csc;
        }
        if (vsp) {
            vsp->Delete();
            delete vsp;
        }
        if (header) {
            delete header;
        }
        if (mwpc) {
            mwpc->Delete();
            delete mwpc;
        }
        if (silicon) {
            silicon->Delete();
            delete silicon;
        }
        if (tof400) {
            tof400->Delete();
            delete tof400;
        }
        if (tof700) {
            tof700->Delete();
            delete tof700;
        }
        if (tof701) {
            tof701->Delete();
            delete tof701;
        }
        if (zdc) {
            zdc->Delete();
            delete zdc;
        }
        if (scwall) {
            scwall->Delete();
            delete scwall;
        }
        if (fhcal) {
            fhcal->Delete();
            delete fhcal;
        }
        if (hodo) {
            hodo->Delete();
            delete hodo;
        }
        if (ndet) {
            ndet->Delete();
            delete ndet;
        }
        if (hgnd) {
            hgnd->Delete();
            delete hgnd;
        }
        if (ecal) {
            ecal->Delete();
            delete ecal;
        }
        if (sibt) {
            sibt->Delete();
            delete sibt;
        }
        if (msc) {
            msc->Delete();
            delete msc;
        }
        if (trigAr) {
            for (TClonesArray* ar : (*trigAr))
                if (ar) {
                    ar->Delete();
                    delete ar;
                }
            delete trigAr;
        }
    }

    TClonesArray* silicon;
    TClonesArray* gem;
    TClonesArray* csc;
    TClonesArray* vsp;
    TClonesArray* tof400;
    TClonesArray* tof700;
    TClonesArray* tof701;
    TClonesArray* zdc;
    TClonesArray* scwall;
    TClonesArray* fhcal;
    TClonesArray* hodo;
    TClonesArray* ndet;
    TClonesArray* hgnd;
    TClonesArray* ecal;
    TClonesArray* dch;
    TClonesArray* mwpc;
    TClonesArray* sibt;
    TClonesArray* siprof;
    TClonesArray* msc;
    std::vector<TClonesArray*>* trigAr;

    BmnEventHeader* header;   //->

    ClassDef(DigiArrays, 5)
};

#endif /* DIGIARRAYS_H */
