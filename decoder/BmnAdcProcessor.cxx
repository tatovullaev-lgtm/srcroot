#include "BmnAdcProcessor.h"

#include "P4_F32vec4.h"
#include "smmintrin.h"

#include <BmnMath.h>

BmnAdcProcessor::BmnAdcProcessor(Int_t period, Int_t run, TString det, Int_t nCh, const Int_t nSmpl)
    : fVerbose(0)
    , drawCnt(0)
    , drawCnt2d(0)
    , thrDif(0)
    , FinalThr(0)
    , fNIters(0)
    , cmodcut(0)
    , thrMax(0)
    , fNSerials(0)
    , fNSamples(nSmpl)
    , fNChannels(nCh)
    , fEvForPedestals(N_EV_FOR_PEDESTALS)
    , fPedDat(nullptr)
    , fSetup(kBMNSETUP)
    , fApplyThreshold(true)
    , fSigProf(nullptr)
    , fNoisyChannels(nullptr)
    , fAdc(nullptr)
    , fPedRms(nullptr)
    , fPedVal(nullptr)
    , fPedValTemp(nullptr)
    , fCMode(nullptr)
    , fCMode0(nullptr)
    , fSMode(nullptr)
    , fSMode0(nullptr)
    , fNoisyChipChannels(nullptr)
    , fNvals(nullptr)
    , fNvalsCMod(nullptr)
    , fNvalsADC(nullptr)
    , fPedCMod(nullptr)
    , fPedCMod2(nullptr)
    , fSumRmsV(nullptr)
    , fDetName(det)
    , fPeriod(period)
    , fRun(run)
{
    PrecalcEventModsImp = (GetRun() > (Int_t)GetBoundaryRun(fNSamples) || GetPeriod() >= 7)
                              ?
    // #if CMAKE_BUILD_TYPE == Debug
    //         &BmnAdcProcessor::PrecalcEventMods : &BmnAdcProcessor::PrecalcEventModsOld;
    //         printf("\n\nDebug!!!\n\n");
    // #else
    //         &BmnAdcProcessor::PrecalcEventMods_simd : &BmnAdcProcessor::PrecalcEventModsOld;
    //         printf("\n\nRelease!!!\n\n");
    // #endif

#ifdef BUILD_DEBUG
                              &BmnAdcProcessor::PrecalcEventMods
                              : &BmnAdcProcessor::PrecalcEventModsOld;
    if (fVerbose)
        printf("\n\nDebug!!!\n");
#else
                              &BmnAdcProcessor::PrecalcEventMods_simd
                              : &BmnAdcProcessor::PrecalcEventModsOld;
    if (fVerbose)
        printf("\n\nRelease!!!\n");
#endif
}

void BmnAdcProcessor::GrabNewSerial(UInt_t serial)
{
    auto serIter = fSerMap.find(serial);
    if (serIter == fSerMap.end()) {
        if (fVerbose > 1)
            printf("adding %08X to the map, iSer = %d\n", serial, fNSerials);
        fSerMap.insert(pair<UInt_t, Int_t>(serial, fNSerials++));
        fAdcSerials.push_back(serial);
    }
}

void BmnAdcProcessor::InitArrays()
{
    fPedVal = new Float_t**[fNSerials];
    // fPedVal = new Double_t**[fNSerials];
    fPedValTemp = new Double_t**[fNSerials];
    //  fNvals = new UInt_t*[fNSerials];
    fNvals = new Float_t*[fNSerials];
    fNvalsCMod = new UInt_t**[fNSerials];
    fNvalsADC = new UInt_t**[fNSerials];
    fPedRms = new Double_t**[fNSerials];
    fPedCMod = new Double_t**[fNSerials];
    fPedCMod2 = new Double_t**[fNSerials];
    fSumRmsV = new Double_t*[fNSerials];
    fNoisyChipChannels = new Bool_t**[fNSerials];
    fCMode = new Float_t*[fNSerials];
    fCMode0 = new Float_t*[fNSerials];
    fSMode = new Float_t*[fNSerials];
    fSMode0 = new Float_t*[fNSerials];
    fAdcProfiles = new UInt_t**[fNSerials];
    fAdc = new Float_t**[fNSerials];
    // fAdc = new Double_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        // fPedVal[iCr] = new Double_t*[fNChannels];
        fPedVal[iCr] = new Float_t*[fNChannels];
        fPedValTemp[iCr] = new Double_t*[fNChannels];
        // fNvals[iCr] = new UInt_t[fNChannels];
        fNvals[iCr] = new Float_t[fNChannels];
        memset(fNvals[iCr], 0, sizeof(Float_t) * fNChannels);
        fNvalsCMod[iCr] = new UInt_t*[fNChannels];
        fNvalsADC[iCr] = new UInt_t*[fNChannels];
        fPedRms[iCr] = new Double_t*[fNChannels];
        fPedCMod[iCr] = new Double_t*[fNChannels];
        fPedCMod2[iCr] = new Double_t*[fNChannels];
        fSumRmsV[iCr] = new Double_t[fNChannels];
        memset(fSumRmsV[iCr], 0, sizeof(Double_t) * fNChannels);
        fNoisyChipChannels[iCr] = new Bool_t*[fNChannels];
        fAdcProfiles[iCr] = new UInt_t*[fNChannels];
        // fAdc[iCr] = new Double_t*[fNChannels];
        fAdc[iCr] = new Float_t*[fNChannels];
        fCMode[iCr] = new Float_t[fNChannels];
        fCMode0[iCr] = new Float_t[fNChannels];
        fSMode[iCr] = new Float_t[fNChannels];
        fSMode0[iCr] = new Float_t[fNChannels];
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            // fPedVal[iCr][iCh] = new Double_t[fNSamples];
            fPedVal[iCr][iCh] = new Float_t[fNSamples];
            fPedValTemp[iCr][iCh] = new Double_t[fNSamples];
            fNvalsADC[iCr][iCh] = new UInt_t[fNSamples];
            memset(fNvalsADC[iCr][iCh], 0, sizeof(UInt_t) * fNSamples);
            fNvalsCMod[iCr][iCh] = new UInt_t[fNSamples];
            memset(fNvalsCMod[iCr][iCh], 0, sizeof(UInt_t) * fNSamples);
            fPedRms[iCr][iCh] = new Double_t[fNSamples];
            fPedCMod[iCr][iCh] = new Double_t[fNSamples];
            fPedCMod2[iCr][iCh] = new Double_t[fNSamples];
            fNoisyChipChannels[iCr][iCh] = new Bool_t[fNSamples];
            fAdcProfiles[iCr][iCh] = new UInt_t[fNSamples];
            // fAdc[iCr][iCh] = new Double_t[fNSamples];
            fAdc[iCr][iCh] = new Float_t[fNSamples];
            fCMode[iCr][iCh] = 0.0;
            fCMode0[iCr][iCh] = 0.0;
            fSMode[iCr][iCh] = 0.0;
            fSMode0[iCr][iCh] = 0.0;
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedValTemp[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fPedCMod[iCr][iCh][iSmpl] = 0.0;
                fPedCMod2[iCr][iCh][iSmpl] = 0.0;
                fNoisyChipChannels[iCr][iCh][iSmpl] = kFALSE;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
                fAdc[iCr][iCh][iSmpl] = 0.0;
            }
        }
    }

    fPedDat = new Double_t***[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fPedDat[iCr] = new Double_t**[N_EV_FOR_PEDESTALS];
        for (UInt_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
            fPedDat[iCr][iEv] = new Double_t*[fNChannels];
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                fPedDat[iCr][iEv][iCh] = new Double_t[fNSamples];
                for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl)
                    fPedDat[iCr][iEv][iCh][iSmpl] = 0.0;
            }
        }
    }
    //    Int_t nevents = N_EV_FOR_PEDESTALS - 2;
    //    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
    //        vector<TH1*> hv;
    //        vector<TH1*> hcm;
    //        vector<TH1*> hsm;
    //        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
    //            TString hname = TString(Form("%08X:%02d pedestals SM ev", fAdcSerials[iCr], iCh));
    //            TH1* h = new TH2F(hname, hname,
    //                    N_EV_FOR_PEDESTALS, 0, N_EV_FOR_PEDESTALS,
    //                    fNSamples, 0, fNSamples);
    //            h->GetXaxis()->SetTitle("Event #");
    //            h->GetYaxis()->SetTitle("Sample(channel) #");
    //            hv.push_back(h);
    //
    //            hname = TString(Form("%08X:%02d cmods SM ev", fAdcSerials[iCr], iCh));
    //            TH1* hc = new TH1F(hname, hname, nevents, 0, nevents);
    //            hc->GetXaxis()->SetTitle("Event #");
    //            hcm.push_back(hc);
    //
    //            hname = TString(Form("%08X:%02d smods SM ev", fAdcSerials[iCr], iCh));
    //            TH1* hs = new TH1F(hname, hname, nevents, 0, nevents);
    //            hs->GetXaxis()->SetTitle("Event #");
    //            hsm.push_back(hs);
    //        }
    //        hPedLine.push_back(hv);
    //        hCMode.push_back(hcm);
    //        hSMode.push_back(hsm);
    //    }
}

BmnAdcProcessor::~BmnAdcProcessor()
{
    for (Int_t iCr = 0; iCr < fNSerials; iCr++) {
        for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; iEv++) {
            for (Int_t iCh = 0; iCh < fNChannels; iCh++)
                delete[] fPedDat[iCr][iEv][iCh];
            delete[] fPedDat[iCr][iEv];
        }
        delete[] fPedDat[iCr];
    }
    delete[] fPedDat;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            delete[] fPedVal[iCr][iCh];
            delete[] fPedValTemp[iCr][iCh];
            delete[] fNvalsCMod[iCr][iCh];
            delete[] fNvalsADC[iCr][iCh];
            delete[] fPedRms[iCr][iCh];
            delete[] fPedCMod[iCr][iCh];
            delete[] fPedCMod2[iCr][iCh];
            delete[] fAdcProfiles[iCr][iCh];
            delete[] fAdc[iCr][iCh];
            delete[] fNoisyChipChannels[iCr][iCh];
        }
        delete[] fPedVal[iCr];
        delete[] fPedValTemp[iCr];
        delete[] fNvalsADC[iCr];
        delete[] fNvals[iCr];
        delete[] fNvalsCMod[iCr];
        delete[] fPedRms[iCr];
        delete[] fPedCMod[iCr];
        delete[] fPedCMod2[iCr];
        delete[] fSumRmsV[iCr];
        delete[] fAdcProfiles[iCr];
        delete[] fAdc[iCr];
        delete[] fCMode[iCr];
        delete[] fCMode0[iCr];
        delete[] fSMode[iCr];
        delete[] fSMode0[iCr];
        delete[] fNoisyChipChannels[iCr];
    }
    delete[] fPedVal;
    delete[] fPedValTemp;
    delete[] fNvalsADC;
    delete[] fNvals;
    delete[] fNvalsCMod;
    delete[] fPedRms;
    delete[] fPedCMod;
    delete[] fPedCMod2;
    delete[] fSumRmsV;
    delete[] fAdcProfiles;
    delete[] fAdc;
    delete[] fCMode;
    delete[] fCMode0;
    delete[] fSMode;
    delete[] fSMode0;
    delete[] fNoisyChipChannels;
    /*canStrip->cd(1);
    h->Draw("colz");
    canStrip->cd(2);
    hp->Draw("colz");
    canStrip->cd(3);
    hcms->Draw("colz");
    canStrip->cd(4);
    hscms_adc->Draw("colz");
    canStrip->cd(5);
    hcms1p->Draw("colz");
    canStrip->cd(6);
    hscms1p_adc->Draw("colz");
    canStrip->cd(7);
    hcms1->Draw("colz");
    canStrip->cd(8);
    hscms1_adc->Draw("colz");
    canStrip->SaveAs("can-cms.png");*/
    //    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
    //        delete hSModeSi[iCr];
    //        delete hCModeSi[iCr];
    //        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
    //            delete hPedLineSi[iCr][iCh];
    //        }
    //    }
    LOGF(info, "%10s: Real time %4.3f s, CPU time %4.3f s ADC processor. Precalc", fDetName.Data(), workTime_real_p,
         workTime_cpu_p);
    LOGF(info, "%10s: Real time %4.3f s, CPU time %4.3f s ADC processor", fDetName.Data(), workTime_real, workTime_cpu);
}

void BmnAdcProcessor::CopyData2PedMap(TClonesArray* adc, UInt_t ev)
{
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*)adc->At(iAdc);
        //            printf("GEM ser 0x%08X, ev %d\n", adcDig->GetSerial(), ev);
        auto serIter = fSerMap.find(adcDig->GetSerial());
        if (serIter == fSerMap.end()) {
            //            printf("Serial %08X not found in the map\n", ser);
            continue;
        }
        Int_t iSer = serIter->second;
        //                printf("GEM ser = 0x%08x, iSer = %02d, ev= %05d, ch = %d\n", adcDig->GetSerial(), iSer, ev,
        //                adcDig->GetChannel());
        for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
            if ((fRun > (Int_t)GetBoundaryRun(ADC32_N_SAMPLES)) || (fPeriod >= 7)) {
                fPedDat[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t)(adcDig->GetShortValue())[iSmpl] / 16.0;
            } else
                fPedDat[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t)(adcDig->GetUShortValue())[iSmpl] / 16.0;
            //                printf("adc = %i pedData = %f\n", (adcDig->GetShortValue())[iSmpl],
            //                pedData[iSer][ev][adcDig->GetChannel()][iSmpl]);
        }
    }
}

void BmnAdcProcessor::DrawDebugHists()
{
    TString docName = TString(Form("%s-mods-ev-%d.pdf", fDetName.Data(), drawCnt));
    const UInt_t PAD_WIDTH_SIL = 1920;   // 8192;
    const UInt_t PAD_HEIGHT_SIL = 1080;
    //    TString docName = "sil-ped-cms.pdf";
    TCanvas* c = new TCanvas("can cms", "can", PAD_WIDTH_SIL, 2 * PAD_HEIGHT_SIL);
    c->Divide(1, 2);
    c->Print(docName + "[");
    for (int iCr = 0; iCr < fNSerials; iCr++)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            // auto *p = c->cd(iCr * fNChannels + iCh + 1);
            c->Clear("D");
            //            hPedLine[iCr][iCh]->Draw("colz");
            c->cd(1);
            hSMode[iCr][iCh]->Draw("colz");
            c->cd(2);
            hCMode[iCr][iCh]->Draw("colz");
            c->Print(docName);
            //            c->SaveAs(Form("%s.png", hPedLine[iCr][iCh]->GetName()));
        }
    c->Print(docName + "]");
    //        c->SaveAs();
    drawCnt++;
}

void BmnAdcProcessor::DrawDebugHists2D()
{
    TString docName = TString(Form("%s-run-%d-mods-%d.pdf", fDetName.Data(), fRun, drawCnt2d));
    const UInt_t PAD_WIDTH_SIL = 1920;   // 8192;
    const UInt_t PAD_HEIGHT_SIL = 1080;
    gStyle->SetOptStat(0);
    //    TColor::InvertPalette();
    gStyle->SetPalette(kDeepSea);
    TCanvas* c = new TCanvas("can cms", "can", PAD_WIDTH_SIL, 2 * PAD_HEIGHT_SIL);
    c->Divide(1, 3);
    c->Print(docName + "[");
    for (int iCr = 0; iCr < fNSerials; iCr++) {
        c->Clear("D");
        c->cd(1);
        hPedSi[iCr]->Draw("box");
        c->cd(2);
        hCModeSi[iCr]->Draw("colz");
        c->cd(3);
        hSModeSi[iCr]->Draw("colz");
        c->Print(docName);
    }
    //        c->Print(docName + "]");
    delete c;
    c = new TCanvas("can pedestals", "can", PAD_WIDTH_SIL, PAD_HEIGHT_SIL);
    //    c->Print(docName + "[");
    c->Clear("D");
    //    c->Divide(1, 2);
    for (int iCr = 0; iCr < fNSerials; iCr++)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            c->Clear("D");
            c->cd(1);
            hPedLineSi[iCr][iCh]->Draw("colz");
            c->Print(docName);
            //            c->SaveAs(Form("%s.png", hPedLine[iCr][iCh]->GetName()));
        }
    c->Print(docName + "]");
    delete c;
    drawCnt2d++;
}

void BmnAdcProcessor::ClearDebugHists()
{
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        hPedSi[iCr]->Reset();
        hSModeSi[iCr]->Reset();
        hCModeSi[iCr]->Reset();
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            hPedLine[iCr][iCh]->Reset();
            hSMode[iCr][iCh]->Reset();
            hCMode[iCr][iCh]->Reset();
        }
    }
}

BmnStatus BmnAdcProcessor::RecalculatePedestals()
{
    const UShort_t nSmpl = fNSamples;

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
            }
        }
    // cout << fDetName << " pedestals calculation..." << endl;
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                Double_t signals[nSmpl];
                for (Int_t i = 0; i < nSmpl; ++i)
                    signals[i] = 0.0;
                Int_t nOk = 0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0)
                        continue;
                    signals[iSmpl] = fPedDat[iCr][iEv][iCh][iSmpl];
                    nOk++;
                }
                Double_t CMS = CalcCMS(signals, nOk);
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    fPedVal[iCr][iCh][iSmpl] += ((signals[iSmpl] - CMS) / N_EV_FOR_PEDESTALS);
                }
            }
    }

    // cout << fDetName << " RMS calculation..." << endl;
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                Double_t signals[nSmpl];
                for (Int_t i = 0; i < nSmpl; ++i)
                    signals[i] = 0.0;
                Int_t nOk = 0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0)
                        continue;
                    signals[iSmpl] = fPedDat[iCr][iEv][iCh][iSmpl];
                    nOk++;
                }
                Double_t CMS = CalcCMS(signals, nOk);
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    Float_t ped = fPedVal[iCr][iCh][iSmpl];
                    fPedRms[iCr][iCh][iSmpl] += ((signals[iSmpl] - CMS - ped) * (signals[iSmpl] - CMS - ped));
                }
            }
    }

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                fPedRms[iCr][iCh][iSmpl] = Sqrt(fPedRms[iCr][iCh][iSmpl] / N_EV_FOR_PEDESTALS);

    ofstream pedFile(Form("%s/input/%s_pedestals_%d.txt", getenv("VMCWORKDIR"), fDetName.Data(), fRun));
    pedFile << "Serial\tCh_id\tPed\tRMS" << endl;
    pedFile << "============================================" << endl;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                pedFile << hex << fAdcSerials[iCr] << dec << "\t" << iCh * nSmpl + iSmpl << "\t"
                        << fPedVal[iCr][iCh][iSmpl] << "\t" << fPedRms[iCr][iCh][iSmpl] << endl;
    pedFile.close();

    return kBMNSUCCESS;
}

BmnStatus BmnAdcProcessor::RecalculatePedestalsAugmented()
{
    //    if (hSModeSi.size() == 0) {
    //        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
    //            vector<TH1*> hv;
    //            vector<TH1*> hcm;
    //            vector<TH1*> hsm;
    //            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
    //                //                        printf("Creating  icr %2d ich %2d %s\n", iCr, iCh, Form("%08X:%02d
    //                pedestal line MK", fSerials[iCr], iCh)); TString hname = TString(Form("%08X:%02d pedestals SM",
    //                fAdcSerials[iCr], iCh)); TH1* h = new TH2F(hname, hname,
    //                        500, 0, 500,
    //                        fNSamples, 0, fNSamples);
    //                h->GetXaxis()->SetTitle("Event #");
    //                h->GetYaxis()->SetTitle("Sample(channel) #");
    //                h->SetDirectory(0);
    //                hv.push_back(h);
    //            }
    //            hPedLineSi.push_back(hv);
    //        }
    //        const Int_t maxAdc = 8192;
    //        const Int_t MaxSig = 2300;
    //        const Int_t RngSig = 400;
    //        const Int_t StripSi = fNChannels * fNSamples;
    //        const Int_t StripSiLo = 9 * fNSamples;
    //        const Int_t StripSiHi = 14 * fNSamples;
    //        const Int_t StripSiBins = (StripSiHi - StripSiLo);
    //        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
    //            TString hname = TString(Form("%08X pedestals SM", fAdcSerials[iCr]));
    //            //        TH1* h = new TH2F(hname, hname, maxAdc, 0, maxAdc, MaxSig, 0, MaxSig);
    //            TH1* h = new TH2F(hname, hname, StripSiBins, StripSiLo, StripSiHi, 2 * RngSig, -RngSig, RngSig);
    //            //    printf("maxAdc %04d max %04f peds\n", maxAdc, h->GetXaxis()->GetXmax());
    //            h->GetXaxis()->SetTitle("Channel #");
    //            h->GetYaxis()->SetTitle("Signal");
    //            h->SetDirectory(0);
    //            hPedSi.push_back(h);
    //            hname = TString(Form("%08X cmods SM", fAdcSerials[iCr]));
    //            TH1* hc = new TH2F(hname, hname, StripSiBins, StripSiLo, StripSiHi, 2 * RngSig, -RngSig, RngSig);
    //            //    printf("maxAdc %04d max %04f cmode\n", maxAdc, hc->GetXaxis()->GetXmax());
    //            hc->GetXaxis()->SetTitle("Channel #");
    //            hc->GetYaxis()->SetTitle("Signal");
    //            hc->SetDirectory(0);
    //            hCModeSi.push_back(hc);
    //            hname = TString(Form("%08X smods SM", fAdcSerials[iCr]));
    //            TH1* hs = new TH2F(hname, hname, StripSiBins, StripSiLo, StripSiHi, 2 * RngSig, -RngSig, RngSig);
    //            hs->GetXaxis()->SetTitle("Channel #");
    //            hs->GetYaxis()->SetTitle("Signal");
    //            hs->SetDirectory(0);
    //            hSModeSi.push_back(hs);
    //        }
    //    }
    if (fVerbose)
        printf("%s %s started   niter %d  thrMax  %4.2f\n", fDetName.Data(), __func__, fNIters, thrMax);
    const UShort_t nSmpl = fNSamples;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            //            memset(fNvalsADC[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedValTemp[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
                fNvalsADC[iCr][iCh][iSmpl] = 0;
                //                fNoisyChipChannels[iCr][iCh][iSmpl] = kFALSE;
            }
        }
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                Int_t nOk = 0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0.0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)
                        continue;
                    fPedVal[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl];   // / N_EV_FOR_PEDESTALS);
                    fNvalsADC[iCr][iCh][iSmpl]++;
                    nOk++;
                    // static_cast<TH2*> (hPedLineSi[iCr][iCh])->Fill(iEv, iSmpl, fPedDat[iCr][iEv][iCh][iSmpl]);
                }
            }
    }
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                if (fNvalsADC[iCr][iCh][iSmpl])
                    fPedVal[iCr][iCh][iSmpl] /= fNvalsADC[iCr][iCh][iSmpl];
    // iteratively calculate pedestals and CMSs
    //    Double_t rmsthr = 200.0;
    //    Double_t rmsthrf = 200.0;
    Double_t sumRms = thrMax / 3.5;
    for (Int_t iter = 0; iter < fNIters; iter++) {
        Double_t thr = thrMax - thrDif * iter;
        if (thr < 0)
            thr = 0;
        if (fVerbose)
            printf("iter %d thr %4.2f\n", iter, thr);
        UInt_t nFiltered = 0;
        // clear
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
            memset(fSumRmsV[iCr], 0, sizeof(Double_t) * fNChannels);
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                memset(fNvalsCMod[iCr][iCh], 0, sizeof(UInt_t) * fNSamples);
                memset(fNvalsADC[iCr][iCh], 0, sizeof(UInt_t) * fNSamples);
                memset(fPedValTemp[iCr][iCh], 0, sizeof(Double_t) * fNSamples);
                memset(fPedCMod[iCr][iCh], 0, sizeof(Double_t) * fNSamples);
                memset(fPedCMod2[iCr][iCh], 0, sizeof(Double_t) * fNSamples);
            }
        }
        for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS - 1; ++iEv) {
            // clear
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
                // memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
                memset(fNvals[iCr], 0, sizeof(Float_t) * fNChannels);
                memset(fCMode[iCr], 0, sizeof(Float_t) * fNChannels);
                memset(fSMode[iCr], 0, sizeof(Float_t) * fNChannels);
            }
            // Pedestals pre filtering
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                        //                        if (iCr == 3 && iCh == 8) {
                        //                            printf("iter %i iEv %i fpedDat %f noise %i\n", iter, iEv,
                        //                            fPedDat[iCr][iEv][iCh][iSmpl],
                        //                            fNoisyChipChannels[iCr][iCh][iSmpl]);
                        //                        }
                        if (fPedDat[iCr][iEv][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)
                            continue;
                        Double_t Asig = TMath::Abs(fPedDat[iCr][iEv][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl]);
                        if (Asig < thr) {
                            fSMode[iCr][iCh] += fPedDat[iCr][iEv][iCh][iSmpl];   // CMS from current event
                            fCMode[iCr][iCh] += fPedVal[iCr][iCh][iSmpl];        // CMS over all pedestals
                            //                        fPedValTemp[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl]; //
                            //                        CMS from current event fNvalsADC[iCr][iCh][iSmpl]++;
                            fNvals[iCr][iCh]++;
                        }
                    }
                }
            // normalize
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    if (fNvals[iCr][iCh]) {
                        fSMode[iCr][iCh] /= fNvals[iCr][iCh];
                        fCMode[iCr][iCh] /= fNvals[iCr][iCh];
                        //                        hSMode[iCr][iCh]->SetBinContent(iEv, fSMode[iCr][iCh]);
                        //                        hCMode[iCr][iCh]->SetBinContent(iEv, fCMode[iCr][iCh]);
                    } else {
                        fSMode[iCr][iCh] = 0;
                        fCMode[iCr][iCh] = 0;
                    }
                }
            // Pedestals filtering
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                        Double_t Adc = fPedDat[iCr][iEv][iCh][iSmpl];
                        if ((Adc == 0) || (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE))
                            continue;
                        Double_t sig = fPedDat[iCr][iEv][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + fCMode[iCr][iCh]
                                       - fSMode[iCr][iCh];
                        Double_t Asig = TMath::Abs(sig);
                        if (Asig < thr) {
                            fPedValTemp[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl];
                            fNvalsADC[iCr][iCh][iSmpl]++;

                            Adc = fPedDat[iCr][iEv][iCh][iSmpl] - fSMode[iCr][iCh];
                            fPedCMod[iCr][iCh][iSmpl] += Adc;
                            fPedCMod2[iCr][iCh][iSmpl] += Adc * Adc;
                            fNvalsCMod[iCr][iCh][iSmpl]++;
                            nFiltered++;
                        }
                        // if (iter == fEndIter - 1) {
                        // Int_t ic = iCh * nSmpl + iSmpl;
                        //                             printf("iCh %4d iSmpl %4d  ic %4d  cmod %5f smod %5f\n", iCh,
                        //                             iSmpl, ic, fCMode[iCr][iCh], fSMode[iCr][iCh]);
                        //                             hCModeSi[iCr]->Fill(ic, fCMode[iCr][iCh]);
                        //                             hSModeSi[iCr]->Fill(ic, fSMode[iCr][iCh]);
                        //                             hPedSi[iCr]->Fill(ic, fPedVal[iCr][iCh][iSmpl]);
                        //}
                    }
                }
        }   // event loop

        sumRms = 0.0;
        Int_t nrms = 0;

        // hists fill
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                Int_t nvrms = 0;
                fSumRmsV[iCr][iCh] = 0.0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)
                        continue;
                    if (fNvalsCMod[iCr][iCh][iSmpl]) {
                        fPedCMod[iCr][iCh][iSmpl] /= fNvalsCMod[iCr][iCh][iSmpl];
                        fPedCMod2[iCr][iCh][iSmpl] = Sqrt(Abs(fPedCMod2[iCr][iCh][iSmpl] / fNvalsCMod[iCr][iCh][iSmpl]
                                                              - Sq(fPedCMod[iCr][iCh][iSmpl])));
                        sumRms += fPedCMod2[iCr][iCh][iSmpl];
                        fSumRmsV[iCr][iCh] += fPedCMod2[iCr][iCh][iSmpl];
                        nrms++;
                        nvrms++;
                    }
                    if (fNvalsADC[iCr][iCh][iSmpl])
                        fPedVal[iCr][iCh][iSmpl] = fPedValTemp[iCr][iCh][iSmpl] / fNvalsADC[iCr][iCh][iSmpl];
                    else
                        fPedVal[iCr][iCh][iSmpl] = 0.0;
                    //                            printf("fPedVal[%3d][%3d][%3d] = %4.2f\n", iCr, iCh,
                    //                            iSmpl,fPedVal[iCr][iCh][iSmpl]);
                    fNvalsADC[iCr][iCh][iSmpl] = 0;
                    //                    fCMode[iCr][iCh] += fPedVal[iCr][iCh][iSmpl]; // CMS over all pedestals
                }
                if (nvrms)
                    fSumRmsV[iCr][iCh] /= nvrms;
            }
        if (nrms > 0)
            sumRms /= nrms;

        // noise ch detection
        if (fDetName.Contains("SiBT") || fDetName.Contains("SiProf"))
            continue;
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                if ((iter == fNIters - 1) && (fVerbose > 1))
                    printf("cr %8X ich %2d  fSumRmsV %4f sumRms %4f\n", fAdcSerials[iCr], iCh, fSumRmsV[iCr][iCh],
                           sumRms);
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)
                        continue;
                    if ((iter == fNIters - 1) && (fVerbose > 1))
                        printf("cr %8X ich %2d iSmpl %2d  fPedCMod2 %4f\n", fAdcSerials[iCr], iCh, iSmpl,
                               fPedCMod2[iCr][iCh][iSmpl]);
                    if ((fPedCMod2[iCr][iCh][iSmpl]
                         > 4 * sumRms) /* || (fPedCMod2[iCr][iCh][iSmpl] > 5 * fSumRmsV[iCr][iCh])*/)
                    {
                        fNoisyChipChannels[iCr][iCh][iSmpl] = kTRUE;
                        if (fVerbose)
                            printf("new noisy ch on  cr %i %08X ch %i smpl %i  by signal %4.2f\n", iCr,
                                   fAdcSerials[iCr], iCh, iSmpl, fPedCMod2[iCr][iCh][iSmpl]);
                    }
                }
            }
    }   // iter loop
    return kBMNSUCCESS;
}

void BmnAdcProcessor::PrecalcEventModsOld(TClonesArray* adc)
{
    TStopwatch timer;
    // Double_t rtime;
    // Double_t ctime;
    timer.Start();
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        memset(fNvals[iCr], 0, sizeof(Float_t) * fNChannels);
        memset(fCMode[iCr], 0, sizeof(Float_t) * fNChannels);
        memset(fSMode[iCr], 0, sizeof(Float_t) * fNChannels);
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            memset(fAdc[iCr][iCh], 0, sizeof(Float_t) * fNSamples);
        }
    }
    timer.Stop();
    // rtime = timer.RealTime();
    // ctime = timer.CpuTime();
    //     printf("\nReal time %f s, CPU time %f s  clear\n", rtime, ctime);
    timer.Start();
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*)adc->At(iAdc);
        UInt_t iCh = adcDig->GetChannel();
        UInt_t ser = adcDig->GetSerial();
        //        printf("Serial %08X \n", ser);
        auto serIter = fSerMap.find(ser);
        //        printf("iter %08X end %08X\n", serIter, fSerMap.end());
        if (serIter == fSerMap.end()) {
            //            printf("Serial %08X not found in the map\n", ser);
            continue;
        }
        Int_t iCr = serIter->second;
        for (Int_t iSmpl = 0; iSmpl < fNSamples; iSmpl++) {
            if ((fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE))
                continue;
            Double_t val = static_cast<Double_t>(adcDig->GetUShortValue()[iSmpl] / 16);
            fAdc[iCr][iCh][iSmpl] = val;

            Double_t Sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl];
            Double_t Asig = TMath::Abs(Sig);
            //                        printf("adc %6f ped %6f\n", fAdc[iCr][iCh][iSmpl], fPedVal[iCr][iCh][iSmpl]);

            if ((Asig < thrMax)) {
                //                        printf("adc %6f < thrMax %6f\n", fAdc[iCr][iCh][iSmpl], thrMax);
                fSMode[iCr][iCh] += fAdc[iCr][iCh][iSmpl];
                fCMode[iCr][iCh] += fPedVal[iCr][iCh][iSmpl];
                fNvals[iCr][iCh]++;
            }
        }
    }
    timer.Stop();
    // rtime = timer.RealTime();
    // ctime = timer.CpuTime();
    //     printf("\nReal time %f s, CPU time %f s  fill array\n", rtime, ctime);
}

void BmnAdcProcessor::PrecalcEventMods(TClonesArray* adc)
{
    TStopwatch timer;
    // Double_t rtime;
    // Double_t ctime;
    timer.Start();
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        // memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
        memset(fNvals[iCr], 0, sizeof(Float_t) * fNChannels);
        memset(fCMode[iCr], 0, sizeof(Float_t) * fNChannels);
        memset(fSMode[iCr], 0, sizeof(Float_t) * fNChannels);
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            memset(fAdc[iCr][iCh], 0, sizeof(Float_t) * fNSamples);
        }
    }
    timer.Stop();
    // rtime = timer.RealTime();
    // ctime = timer.CpuTime();
    //     printf("\nReal time %f s, CPU time %f s  clear\n", rtime, ctime);
    timer.Start();
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*)adc->At(iAdc);
        UInt_t iCh = adcDig->GetChannel();
        UInt_t ser = adcDig->GetSerial();
        //                printf("Serial %08X  channel %d\n", ser, iCh);
        auto serIter = fSerMap.find(ser);
        if (serIter == fSerMap.end()) {
            //            printf("Serial %08X not found in the map\n", ser);
            continue;
        }
        Int_t iCr = serIter->second;
        for (Int_t iSmpl = 0; iSmpl < fNSamples; iSmpl++) {
            if ((fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE))
                continue;
            Double_t val = static_cast<Double_t>(adcDig->GetShortValue()[iSmpl] / 16);
            fAdc[iCr][iCh][iSmpl] = val;

            Double_t Sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl];
            Double_t Asig = TMath::Abs(Sig);
            //                        printf("adc %6f ped %6f\n", fAdc[iCr][iCh][iSmpl], fPedVal[iCr][iCh][iSmpl]);

            if ((Asig < thrMax)) {
                //                        printf("adc %6f < thrMax %6f\n", fAdc[iCr][iCh][iSmpl], thrMax);
                fSMode[iCr][iCh] += fAdc[iCr][iCh][iSmpl];
                fCMode[iCr][iCh] += fPedVal[iCr][iCh][iSmpl];
                fNvals[iCr][iCh]++;
            }
        }
    }
    timer.Stop();
    workTime_cpu_p += timer.RealTime();
    workTime_real_p += timer.CpuTime();
}

// simd function

void BmnAdcProcessor::PrecalcEventMods_simd(TClonesArray* adc)
{
    const Int_t VecLenSIMD = 4;
    TStopwatch timer;
    timer.Start();
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        // memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
        memset(fNvals[iCr], 0, sizeof(Float_t) * fNChannels);
        memset(fCMode[iCr], 0, sizeof(Float_t) * fNChannels);
        memset(fSMode[iCr], 0, sizeof(Float_t) * fNChannels);
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            memset(fAdc[iCr][iCh], 0, sizeof(Float_t) * fNSamples);
        }
    }
    //    timer.Stop();
    //    rtime = timer.RealTime();
    //    ctime = timer.CpuTime();
    //    //    printf("\nReal time %f s, CPU time %f s  clear\n", rtime, ctime);
    //    timer.Start();

    // fvec * fNvals_vec, * fSMode_vec, * fCMode_vec;
    fvec *fPedVal_vec, *fAdc_vec, *fNoisyChipChannels_vec;
    Float_t*** fNoisy_float;

    fNoisy_float = new Float_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fNoisy_float[iCr] = new Float_t*[fNChannels];
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            fNoisy_float[iCr][iCh] = new Float_t[fNSamples];
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                // fNoisy_float[iCr][iCh][iSmpl] = (Float_t)fNoisyChipChannels[iCr][iCh][iSmpl];
                if (fNoisyChipChannels[iCr][iCh][iSmpl])
                    fNoisy_float[iCr][iCh][iSmpl] = 1.0;
                else
                    fNoisy_float[iCr][iCh][iSmpl] = 0.0;
            }
        }
    }
    __m128 outThresh = _mm_set1_ps(10000);   // some number definitely higher than threshold
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*)adc->At(iAdc);
        UInt_t iCh = adcDig->GetChannel();
        UInt_t ser = adcDig->GetSerial();
        //        printf("Serial %08X \n", ser);
        auto serIter = fSerMap.find(ser);
        if (serIter == fSerMap.end()) {
            //            printf("Serial %08X not found in the map\n", ser);
            continue;
        }

        Int_t iCr = serIter->second;
        //                printf("ser %08X iCr %d\n", ser, iCr);
        for (Int_t iSmpl = 0; iSmpl < fNSamples; iSmpl++) {
            if ((fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE))
                continue;
            Float_t val = static_cast<Float_t>(adcDig->GetShortValue()[iSmpl] / 16);
            fAdc[iCr][iCh][iSmpl] = val;
        }

        fPedVal_vec = (fvec*)fPedVal[iCr][iCh];
        fAdc_vec = (fvec*)fAdc[iCr][iCh];
        fNoisyChipChannels_vec = (fvec*)fNoisy_float[iCr][iCh];   //!!!!!!!
        fvec fSMode_sum = 0.0;
        fvec fCMode_sum = 0.0;
        fvec Nval = 0.0;
        Float_t sum1;
        Float_t sum2;
        Float_t sum3;

        for (Int_t iSmpl = 0; iSmpl < fNSamples / VecLenSIMD; iSmpl++) {
            // if ((fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)) continue;
            //                         fvec sig = if3(fvec(fNoisyChipChannels_vec[iSmpl] == 1.0), 10000, fAdc_vec[iSmpl]
            //                         - fPedVal_vec[iSmpl]);
            fvec sig = _mm_blendv_ps(fAdc_vec[iSmpl] - fPedVal_vec[iSmpl], outThresh,
                                     fvec(fNoisyChipChannels_vec[iSmpl] == 1.0));
            // cout << "result of the comparison: " << sig << endl;
            fvec Asig = fabs(sig);
            // cout << "absolute value: " << Asig << endl;
            fvec thrMax_vec = thrMax;
            //                        Nval = if3(fvec(Asig < thrMax_vec), Nval + 1.0, Nval + 0.0);
            Nval = _mm_blendv_ps(Nval, Nval + 1.0, fvec(Asig < thrMax_vec));
            fSMode_sum = _mm_blendv_ps(fSMode_sum, fSMode_sum + fAdc_vec[iSmpl], fvec(Asig < thrMax_vec));
            fCMode_sum = _mm_blendv_ps(fCMode_sum, fCMode_sum + fPedVal_vec[iSmpl], fvec(Asig < thrMax_vec));
        }
        fSMode_sum = _mm_dp_ps(fSMode_sum, _f32vec4_one, 0xFF);
        _mm_store_ss(&sum1, fSMode_sum);
        fSMode[iCr][iCh] += sum1;
        fCMode_sum = _mm_dp_ps(fCMode_sum, _f32vec4_one, 0xFF);
        _mm_store_ss(&sum2, fCMode_sum);
        fCMode[iCr][iCh] += sum2;
        Nval = _mm_dp_ps(Nval, _f32vec4_one, 0xFF);
        _mm_store_ss(&sum3, Nval);
        fNvals[iCr][iCh] += sum3;
    }
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            delete[] fNoisy_float[iCr][iCh];
        }
        delete[] fNoisy_float[iCr];
    }
    delete[] fNoisy_float;

    timer.Stop();
    workTime_cpu_p += timer.RealTime();
    workTime_real_p += timer.CpuTime();
}

void BmnAdcProcessor::CalcEventMods()
{
    TStopwatch timer;
    timer.Start();
    // normalize
    Int_t fNvalsMin = 0;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            if (fNvals[iCr][iCh] > fNvalsMin) {
                fSMode[iCr][iCh] /= fNvals[iCr][iCh];
                fCMode[iCr][iCh] /= fNvals[iCr][iCh];
            } else {
                fSMode[iCr][iCh] = 0.0;
                fCMode[iCr][iCh] = 0.0;
            }
        }
    // filter out sigs to get mods
    for (Int_t iter = 0; iter < fNIters; ++iter) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                fSMode0[iCr][iCh] = 0.0;
                fCMode0[iCr][iCh] = 0.0;
                fNvals[iCr][iCh] = 0;
            }
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                Double_t cs = fCMode[iCr][iCh] - fSMode[iCr][iCh];
                for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                    if (fPedVal[iCr][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)
                        continue;
                    Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + cs;
                    Double_t Asig = TMath::Abs(sig);
                    Double_t thr = thrMax - iter * thrDif;
                    if (Asig < thr) {
                        fSMode0[iCr][iCh] += fAdc[iCr][iCh][iSmpl];
                        fCMode0[iCr][iCh] += fPedVal[iCr][iCh][iSmpl];
                        fNvals[iCr][iCh]++;
                        //                    rmsthrf += Sq(fPedDat[iCr][iEv][iCh][iSmpl] - fSigCMS[iCr][iCh]);
                    }
                }
            }

        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                if (fNvals[iCr][iCh] > fNvalsMin) {
                    fSMode[iCr][iCh] = fSMode0[iCr][iCh] / fNvals[iCr][iCh];
                    fCMode[iCr][iCh] = fCMode0[iCr][iCh] / fNvals[iCr][iCh];
                } else {
                    fSMode[iCr][iCh] = 0.0;
                    fCMode[iCr][iCh] = 0.0;
                }
            }
    }
    timer.Stop();
    workTime_cpu += (Double_t)timer.CpuTime();
    workTime_real += (Double_t)timer.RealTime();
}

// simd function

void BmnAdcProcessor::CalcEventMods_simd()
{
    const Int_t VecLenSIMD = 4;
    TStopwatch timer;
    timer.Start();

    // normalize
    fvec fNvalsMin_vec = 0;
    __m128 outThresh = _mm_set1_ps(10000);
    fvec *fNvals_vec, *fSMode_vec, *fCMode_vec;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fNvals_vec = (fvec*)fNvals[iCr];
        fSMode_vec = (fvec*)fSMode[iCr];
        fCMode_vec = (fvec*)fCMode[iCr];
        for (Int_t iCh = 0; iCh < fNChannels / VecLenSIMD; iCh++) {
            fSMode_vec[iCh] =
                _mm_blendv_ps(_f32vec4_zero, fSMode_vec[iCh] / fNvals_vec[iCh], fvec(fNvals_vec[iCh] > fNvalsMin_vec));
            fCMode_vec[iCh] =
                _mm_blendv_ps(_f32vec4_zero, fCMode_vec[iCh] / fNvals_vec[iCh], fvec(fNvals_vec[iCh] > fNvalsMin_vec));
        }
        fNvals[iCr] = (Float_t*)fNvals_vec;
        fSMode[iCr] = (Float_t*)fSMode_vec;
        fCMode[iCr] = (Float_t*)fCMode_vec;
    }
    // filter out sigs to get mods

    Float_t*** fNoisy_float;
    fNoisy_float = new Float_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fNoisy_float[iCr] = new Float_t*[fNChannels];
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            fNoisy_float[iCr][iCh] = new Float_t[fNSamples];
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                // fNoisy_float[iCr][iCh][iSmpl] = (Float_t)fNoisyChipChannels[iCr][iCh][iSmpl];
                if (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)
                    fNoisy_float[iCr][iCh][iSmpl] = 1.0;
                else
                    fNoisy_float[iCr][iCh][iSmpl] = 0.0;
            }
        }
    }

    fvec *fSMode0_vec, *fCMode0_vec;
    fvec *fPedVal_vec, *fAdc_vec, *fNoisyChipChannels_vec;
    for (Int_t iter = 0; iter < fNIters; ++iter) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
            fSMode0_vec = (fvec*)fSMode0[iCr];
            fCMode0_vec = (fvec*)fCMode0[iCr];
            fNvals_vec = (fvec*)fNvals[iCr];
            for (Int_t iCh = 0; iCh < fNChannels / VecLenSIMD; iCh++) {
                fSMode0_vec[iCh] = 0.0;
                fCMode0_vec[iCh] = 0.0;
                fNvals_vec[iCh] = 0;
            }
            fSMode0[iCr] = (Float_t*)fSMode0_vec;
            fCMode0[iCr] = (Float_t*)fCMode0_vec;
            fNvals[iCr] = (Float_t*)fNvals_vec;
        }
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                fPedVal_vec = (fvec*)fPedVal[iCr][iCh];
                fAdc_vec = (fvec*)fAdc[iCr][iCh];
                fNoisyChipChannels_vec = (fvec*)fNoisy_float[iCr][iCh];   //!!!!!!!
                Float_t cs = fCMode[iCr][iCh] - fSMode[iCr][iCh];
                fvec cs_vec = cs;
                fvec fSMode0_sum = 0.0;
                fvec fCMode0_sum = 0.0;
                fvec Nval = 0.0;
                Float_t sum1;
                Float_t sum2;
                Float_t sum3;

                for (Int_t iSmpl = 0; iSmpl < fNSamples / VecLenSIMD; iSmpl++) {
                    // if (fPedVal[iCr][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                    fvec sig =
                        _mm_blendv_ps(fAdc_vec[iSmpl] - fPedVal_vec[iSmpl] + cs_vec, outThresh,
                                      fvec(fPedVal_vec[iSmpl] == 0.0) | fvec(fNoisyChipChannels_vec[iSmpl] == 1.0));
                    fvec Asig = fabs(sig);
                    Float_t thr = thrMax - iter * thrDif;
                    fvec thr_vec = thr;
                    Nval = _mm_blendv_ps(Nval, Nval + 1.0, fvec(Asig < thr_vec));
                    fSMode0_sum = _mm_blendv_ps(fSMode0_sum, fSMode0_sum + fAdc_vec[iSmpl], fvec(Asig < thr_vec));
                    fCMode0_sum = _mm_blendv_ps(fCMode0_sum, fCMode0_sum + fPedVal_vec[iSmpl], fvec(Asig < thr_vec));
                }
                fSMode0_sum = _mm_dp_ps(fSMode0_sum, _f32vec4_one, 0xFF);
                _mm_store_ss(&sum1, fSMode0_sum);
                fSMode0[iCr][iCh] += sum1;
                fCMode0_sum = _mm_dp_ps(fCMode0_sum, _f32vec4_one, 0xFF);
                _mm_store_ss(&sum2, fCMode0_sum);
                fCMode0[iCr][iCh] += sum2;
                Nval = _mm_dp_ps(Nval, _f32vec4_one, 0xFF);
                _mm_store_ss(&sum3, Nval);
                fNvals[iCr][iCh] += sum3;
            }

        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
            fNvals_vec = (fvec*)fNvals[iCr];
            fSMode_vec = (fvec*)fSMode[iCr];
            fCMode_vec = (fvec*)fCMode[iCr];
            fSMode0_vec = (fvec*)fSMode0[iCr];
            fCMode0_vec = (fvec*)fCMode0[iCr];
            for (Int_t iCh = 0; iCh < fNChannels / VecLenSIMD; iCh++) {
                fSMode_vec[iCh] = _mm_blendv_ps(_f32vec4_zero, fSMode0_vec[iCh] / fNvals_vec[iCh],
                                                fvec(fNvals_vec[iCh] > fNvalsMin_vec));
                fCMode_vec[iCh] = _mm_blendv_ps(_f32vec4_zero, fCMode0_vec[iCh] / fNvals_vec[iCh],
                                                fvec(fNvals_vec[iCh] > fNvalsMin_vec));
            }
            fNvals[iCr] = (Float_t*)fNvals_vec;
            fSMode[iCr] = (Float_t*)fSMode_vec;
            fCMode[iCr] = (Float_t*)fCMode_vec;
            //            if (iter == fEndIter - 1) {
            //                for (Int_t iCh = 0; iCh < fNChannels; iCh++) {
            //                    Int_t ic = iCh * fNSamples + fNSamples / 2;
            //                    //                            printf("iCh %4d iSmpl %4d  ic %4d  cmod %5f smod %5f\n",
            //                    iCh, iSmpl, ic, fCMode[iCr][iCh], fSMode[iCr][iCh]); hCModeSi[iCr]->Fill(ic,
            //                    fCMode[iCr][iCh]); hSModeSi[iCr]->Fill(ic, fSMode[iCr][iCh]);
            //                }
            //            }
        }
    }

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            delete[] fNoisy_float[iCr][iCh];
        }
        delete[] fNoisy_float[iCr];
    }
    delete[] fNoisy_float;

    timer.Stop();
    workTime_cpu += timer.CpuTime();
    workTime_real += timer.RealTime();
}

Double_t BmnAdcProcessor::CalcCMS(Double_t* samples, Int_t size)
{

    const UShort_t kNITER = 10;
    Double_t CMS = 0.0;
    UInt_t nStr = size;
    Double_t upd[size];
    for (Int_t iSmpl = 0; iSmpl < size; ++iSmpl)
        upd[iSmpl] = samples[iSmpl];

    for (Int_t itr = 0; itr < kNITER; ++itr) {
        if (nStr == 0)
            break;
        Double_t cms = 0.0;   // common mode shift
        for (UInt_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            cms += upd[iSmpl];
        cms /= nStr;
        Double_t rms = 0.0;   // chip noise
        for (UInt_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            rms += (upd[iSmpl] - cms) * (upd[iSmpl] - cms);
        rms = Sqrt(rms / nStr);

        UInt_t nOk = 0;
        for (UInt_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            if (Abs(upd[iSmpl] - cms) < 3 * rms)
                upd[nOk++] = upd[iSmpl];
        nStr = nOk;
        CMS = cms;
    }
    return CMS;
}

BmnStatus BmnAdcProcessor::SaveFilterInfo(TFile* calibFile)
{
    BmnCalibData data;
    vector<vector<vector<bool>>>& noise_obj(data.GetNoise());
    vector<vector<vector<Float_t>>>& calib_obj(data.GetCalibration());
    noise_obj.resize(fNSerials, vector<vector<bool>>(fNChannels, vector<bool>(fNSamples, false)));
    calib_obj.resize(fNSerials, vector<vector<Float_t>>(fNChannels, vector<Float_t>(fNSamples, 0.0)));
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                noise_obj[iCr][iCh][iSmpl] = fNoisyChipChannels[iCr][iCh][iSmpl];
                calib_obj[iCr][iCh][iSmpl] = fPedVal[iCr][iCh][iSmpl];
            }
    calibFile->cd();   // ??? global variables
    calibFile->WriteObject(&data, (fDetName + ".Calibration").Data());
    return kBMNSUCCESS;
}

BmnStatus BmnAdcProcessor::LoadFilterInfo(TFile* calibFile)
{
    BmnCalibData* data(nullptr);
    calibFile->GetObject((fDetName + ".Calibration").Data(), data);
    if (data) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
                for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                    //    printf("iCr %3d iCh %3d iSmpl %4d\n", iCr, iCh,iSmpl);
                    fNoisyChipChannels[iCr][iCh][iSmpl] = data->GetNoise()[iCr][iCh][iSmpl];
                    fPedVal[iCr][iCh][iSmpl] = data->GetCalibration()[iCr][iCh][iSmpl];
                }
        delete data;
    } else
        return kBMNERROR;
    //    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
    //        printf("\n");
    //        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
    //            printf("\n\t");
    //            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
    //                printf(" %3.2f ", fPedVal[iCr][iCh][iSmpl]);
    //            }
    //        }
    //    }
    return kBMNSUCCESS;
}

BmnStatus BmnAdcProcessor::DrawFilterInfo(BmnCalibData* data)
{
    //    if (!data)
    //        return kBMNERROR;
    //    for (auto )
    //    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
    //        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
    //            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
    //                //    printf("iCr %3d iCh %3d iSmpl %4d\n", iCr, iCh,iSmpl);
    //                fNoisyChipChannels[iCr][iCh][iSmpl] = data->GetNoise()[iCr][iCh][iSmpl];
    //                fPedVal[iCr][iCh][iSmpl] = data->GetCalibration()[iCr][iCh][iSmpl];
    //            }
    return kBMNSUCCESS;
}

void BmnAdcProcessor::SetThreshold(Double_t final_thr, Double_t thr_dif, Double_t n_iters, Double_t cmod_cut)
{
    FinalThr = final_thr;
    if (thr_dif >= 0)
        thrDif = thr_dif;
    if (n_iters >= 0)
        fNIters = n_iters;
    if (cmod_cut >= 0)
        cmodcut = cmod_cut;
    thrMax = FinalThr + (fNIters - 1) * thrDif;
    return;
}