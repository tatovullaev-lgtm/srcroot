

void TestField() {

    TString fileNew = "FieldMap_1900.root";
    TString fileOld = "field_sp41v5_ascii_Extrap.root";
    BmnFieldMap* fieldNew = new BmnNewFieldMap(fileNew.Data());
    BmnFieldMap* fieldOld = new BmnNewFieldMap(fileOld.Data());
    fieldNew->SetFormat(1);
    fieldOld->SetFormat(0);
    fieldNew->SetScale(900.0 / 1600.0);
    fieldOld->SetScale(1.0);
    fieldNew->Init();
    fieldOld->Init();

    const Float_t xMin = -155.0;
    const Float_t xMax = +144.0;
    const Int_t nXbins = 100;
    Float_t xStep = (xMax - xMin) / nXbins;
    const Float_t yMin = -37.0;
    const Float_t yMax = +54.0;
    const Int_t nYbins = 100;
    Float_t yStep = (yMax - yMin) / nYbins;
    const Float_t zMin = -162.0;
    const Float_t zMax = +439.0;
    const Int_t nZbins = 100;
    Float_t zStep = (zMax - zMin) / nZbins;

    TFile* f = new TFile("field_QA.root", "RECREATE");


    TH1F* hBx_Z_center_old = new TH1F("hBx_Z_center_old", "Z profile of Bx in center; Z, cm; By, kGs", nZbins, zMin, zMax);
    TH1F* hBx_Z_center_new = new TH1F("hBx_Z_center_new", "Z profile of Bx in center; Z, cm; By, kGs", nZbins, zMin, zMax);
    TH1F* hBy_Z_center_old = new TH1F("hBy_Z_center_old", "Z profile of By in center; Z, cm; By, kGs", nZbins, zMin, zMax);
    TH1F* hBy_Z_center_new = new TH1F("hBy_Z_center_new", "Z profile of By in center; Z, cm; By, kGs", nZbins, zMin, zMax);
    TH1F* hBz_Z_center_old = new TH1F("hBz_Z_center_old", "Z profile of Bz in center; Z, cm; By, kGs", nZbins, zMin, zMax);
    TH1F* hBz_Z_center_new = new TH1F("hBz_Z_center_new", "Z profile of Bz in center; Z, cm; By, kGs", nZbins, zMin, zMax);

    for (Int_t k = 0; k < nZbins; ++k) {
        Float_t z = zMin + k * zStep;
        hBx_Z_center_new->SetBinContent(k, fieldNew->GetBx(0, 0, z));
        hBx_Z_center_old->SetBinContent(k, fieldOld->GetBx(0, 0, z));
        hBy_Z_center_new->SetBinContent(k, fieldNew->GetBy(0, 0, z));
        hBy_Z_center_old->SetBinContent(k, fieldOld->GetBy(0, 0, z));
        hBz_Z_center_new->SetBinContent(k, fieldNew->GetBz(0, 0, z));
        hBz_Z_center_old->SetBinContent(k, fieldOld->GetBz(0, 0, z));
    }

    hBx_Z_center_new->Write();
    hBx_Z_center_old->Write();
    hBy_Z_center_new->Write();
    hBy_Z_center_old->Write();
    hBz_Z_center_new->Write();
    hBz_Z_center_old->Write();

    TH1F* hBx_Y_center_old = new TH1F("hBx_Y_center_old", "Y profile of Bx in center; Y, cm; Bx, kGs", nYbins, yMin, yMax);
    TH1F* hBx_Y_center_new = new TH1F("hBx_Y_center_new", "Y profile of Bx in center; Y, cm; Bx, kGs", nYbins, yMin, yMax);
    TH1F* hBy_Y_center_old = new TH1F("hBy_Y_center_old", "Y profile of By in center; Y, cm; By, kGs", nYbins, yMin, yMax);
    TH1F* hBy_Y_center_new = new TH1F("hBy_Y_center_new", "Y profile of By in center; Y, cm; By, kGs", nYbins, yMin, yMax);
    TH1F* hBz_Y_center_old = new TH1F("hBz_Y_center_old", "Y profile of Bz in center; Y, cm; Bz, kGs", nYbins, yMin, yMax);
    TH1F* hBz_Y_center_new = new TH1F("hBz_Y_center_new", "Y profile of Bz in center; Y, cm; Bz, kGs", nYbins, yMin, yMax);

    for (Int_t k = 0; k < nYbins; ++k) {
        Float_t y = yMin + k * yStep;
        hBx_Y_center_new->SetBinContent(k, fieldNew->GetBx(0, y, 0));
        hBx_Y_center_old->SetBinContent(k, fieldOld->GetBx(0, y, 0));
        hBy_Y_center_new->SetBinContent(k, fieldNew->GetBy(0, y, 0));
        hBy_Y_center_old->SetBinContent(k, fieldOld->GetBy(0, y, 0));
        hBz_Y_center_new->SetBinContent(k, fieldNew->GetBz(0, y, 0));
        hBz_Y_center_old->SetBinContent(k, fieldOld->GetBz(0, y, 0));
    }

    hBx_Y_center_new->Write();
    hBx_Y_center_old->Write();
    hBy_Y_center_new->Write();
    hBy_Y_center_old->Write();
    hBz_Y_center_new->Write();
    hBz_Y_center_old->Write();

    TH1F* hBx_X_center_old = new TH1F("hBx_X_center_old", "X profile of Bx in center; X, cm; Bx, kGs", nXbins, xMin, xMax);
    TH1F* hBx_X_center_new = new TH1F("hBx_X_center_new", "X profile of Bx in center; X, cm; Bx, kGs", nXbins, xMin, xMax);
    TH1F* hBy_X_center_old = new TH1F("hBy_X_center_old", "X profile of By in center; X, cm; By, kGs", nXbins, xMin, xMax);
    TH1F* hBy_X_center_new = new TH1F("hBy_X_center_new", "X profile of By in center; X, cm; By, kGs", nXbins, xMin, xMax);
    TH1F* hBz_X_center_old = new TH1F("hBz_X_center_old", "X profile of Bz in center; X, cm; Bz, kGs", nXbins, xMin, xMax);
    TH1F* hBz_X_center_new = new TH1F("hBz_X_center_new", "X profile of Bz in center; X, cm; Bz, kGs", nXbins, xMin, xMax);

    for (Int_t k = 0; k < nXbins; ++k) {
        Float_t x = xMin + k * xStep;
        hBx_X_center_new->SetBinContent(k, fieldNew->GetBx(x, 0, 0));
        hBx_X_center_old->SetBinContent(k, fieldOld->GetBx(x, 0, 0));
        hBy_X_center_new->SetBinContent(k, fieldNew->GetBy(x, 0, 0));
        hBy_X_center_old->SetBinContent(k, fieldOld->GetBy(x, 0, 0));
        hBz_X_center_new->SetBinContent(k, fieldNew->GetBz(x, 0, 0));
        hBz_X_center_old->SetBinContent(k, fieldOld->GetBz(x, 0, 0));
    }

    hBx_X_center_new->Write();
    hBx_X_center_old->Write();
    hBy_X_center_new->Write();
    hBy_X_center_old->Write();
    hBz_X_center_new->Write();
    hBz_X_center_old->Write();


    const Int_t nXslices = 10;
    const Int_t nYslices = 10;
    const Int_t nZslices = 10;
    const Int_t nComp = 3;
    TString comp[nComp] = { "x", "y", "z" };
    TH2F* hB_XZ_new[nComp][nYslices];
    TH2F* hB_XZ_old[nComp][nYslices];
    TH2F* hB_XZ_dif[nComp][nYslices];
    TH2F* hB_YZ_new[nComp][nXslices];
    TH2F* hB_YZ_old[nComp][nXslices];
    TH2F* hB_YZ_dif[nComp][nXslices];
    TH2F* hB_XY_new[nComp][nZslices];
    TH2F* hB_XY_old[nComp][nZslices];
    TH2F* hB_XY_dif[nComp][nZslices];

    const Int_t nPlanes = 3;
    TString planes[nPlanes] = { "XZ", "YZ", "XY" };
    TCanvas* cB[nComp][nPlanes];

    for (Int_t iC = 0; iC < nComp; ++iC) {
        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            cB[iC][iPlane] = new TCanvas(Form("cB%s_%s", comp[iC].Data(), planes[iPlane].Data()), Form("cB%s_%s", comp[iC].Data(), planes[iPlane].Data()), 300, 1000);
            cB[iC][iPlane]->Divide(3, 10);
        }
        for (Int_t i = 0; i < nYslices; ++i) {
            hB_XZ_new[iC][i] = new TH2F(Form("hB%s_XZ_new_%d", comp[iC].Data(), i), Form("B%s new, XZ profile, Y slice %d; Z, cm; X, cm", comp[iC].Data(), i), nZbins, zMin, zMax, nXbins, xMin, xMax);
            hB_XZ_old[iC][i] = new TH2F(Form("hB%s_XZ_old_%d", comp[iC].Data(), i), Form("B%s old, XZ profile, Y slice %d; Z, cm; X, cm", comp[iC].Data(), i), nZbins, zMin, zMax, nXbins, xMin, xMax);
            hB_XZ_dif[iC][i] = new TH2F(Form("hB%s_XZ_dif_%d", comp[iC].Data(), i), Form("B%s dif, XZ profile, Y slice %d; Z, cm; X, cm", comp[iC].Data(), i), nZbins, zMin, zMax, nXbins, xMin, xMax);
            Float_t y = yMin + i * (yMax - yMin) / nYslices;
            for (Int_t j = 0; j < nXbins; ++j) {
                Float_t x = xMin + j * xStep;
                for (Int_t k = 0; k < nZbins; ++k) {
                    Float_t z = zMin + k * zStep;
                    Float_t bNew = (iC == 0) ? fieldNew->GetBx(x, y, z) : (iC == 1) ? fieldNew->GetBy(x, y, z) : (iC == 2) ? fieldNew->GetBz(x, y, z) : 0.0;
                    Float_t bOld = (iC == 0) ? fieldOld->GetBx(x, y, z) : (iC == 1) ? fieldOld->GetBy(x, y, z) : (iC == 2) ? fieldOld->GetBz(x, y, z) : 0.0;
                    hB_XZ_new[iC][i]->SetBinContent(k, j, bNew);
                    hB_XZ_old[iC][i]->SetBinContent(k, j, bOld);
                    hB_XZ_dif[iC][i]->SetBinContent(k, j, bNew - bOld);
                }
            }
            hB_XZ_new[iC][i]->Write();
            hB_XZ_old[iC][i]->Write();
            hB_XZ_dif[iC][i]->Write();
            cB[iC][0]->cd(3 * i + 1);
            hB_XZ_new[iC][i]->Draw("colz");
            cB[iC][0]->cd(3 * i + 2);
            hB_XZ_old[iC][i]->Draw("colz");
            cB[iC][0]->cd(3 * i + 3);
            hB_XZ_dif[iC][i]->Draw("colz");
        }
        for (Int_t i = 0; i < nXslices; ++i) {
            hB_YZ_new[iC][i] = new TH2F(Form("hB%s_YZ_new_%d", comp[iC].Data(), i), Form("B%s new, YZ profile, X slice %d; Z, cm; Y, cm", comp[iC].Data(), i), nZbins, zMin, zMax, nYbins, yMin, yMax);
            hB_YZ_old[iC][i] = new TH2F(Form("hB%s_YZ_old_%d", comp[iC].Data(), i), Form("B%s old, YZ profile, X slice %d; Z, cm; Y, cm", comp[iC].Data(), i), nZbins, zMin, zMax, nYbins, yMin, yMax);
            hB_YZ_dif[iC][i] = new TH2F(Form("hB%s_YZ_dif_%d", comp[iC].Data(), i), Form("B%s dif, YZ profile, X slice %d; Z, cm; Y, cm", comp[iC].Data(), i), nZbins, zMin, zMax, nYbins, yMin, yMax);
            Float_t x = xMin + i * (xMax - xMin) / nXslices;
            for (Int_t j = 0; j < nYbins; ++j) {
                Float_t y = yMin + j * yStep;
                for (Int_t k = 0; k < nZbins; ++k) {
                    Float_t z = zMin + k * zStep;
                    Float_t bNew = (iC == 0) ? fieldNew->GetBx(x, y, z) : (iC == 1) ? fieldNew->GetBy(x, y, z) : (iC == 2) ? fieldNew->GetBz(x, y, z) : 0.0;
                    Float_t bOld = (iC == 0) ? fieldOld->GetBx(x, y, z) : (iC == 1) ? fieldOld->GetBy(x, y, z) : (iC == 2) ? fieldOld->GetBz(x, y, z) : 0.0;
                    hB_YZ_new[iC][i]->SetBinContent(k, j, bNew);
                    hB_YZ_old[iC][i]->SetBinContent(k, j, bOld);
                    hB_YZ_dif[iC][i]->SetBinContent(k, j, bNew - bOld);
                }
            }
            hB_YZ_new[iC][i]->Write();
            hB_YZ_old[iC][i]->Write();
            hB_YZ_dif[iC][i]->Write();
            cB[iC][1]->cd(3 * i + 1);
            hB_YZ_new[iC][i]->Draw("colz");
            cB[iC][1]->cd(3 * i + 2);
            hB_YZ_old[iC][i]->Draw("colz");
            cB[iC][1]->cd(3 * i + 3);
            hB_YZ_dif[iC][i]->Draw("colz");
        }
        for (Int_t i = 0; i < nZslices; ++i) {
            hB_XY_new[iC][i] = new TH2F(Form("hB%s_XY_new_%d", comp[iC].Data(), i), Form("B%s new, XY profile, Z slice %d; Y, cm; X, cm", comp[iC].Data(), i), nYbins, yMin, yMax, nXbins, xMin, xMax);
            hB_XY_old[iC][i] = new TH2F(Form("hB%s_XY_old_%d", comp[iC].Data(), i), Form("B%s old, XY profile, Z slice %d; Y, cm; X, cm", comp[iC].Data(), i), nYbins, yMin, yMax, nXbins, xMin, xMax);
            hB_XY_dif[iC][i] = new TH2F(Form("hB%s_XY_dif_%d", comp[iC].Data(), i), Form("B%s dif, XY profile, Z slice %d; Y, cm; X, cm", comp[iC].Data(), i), nYbins, yMin, yMax, nXbins, xMin, xMax);
            Float_t z = zMin + i * (zMax - zMin) / nZslices;
            for (Int_t j = 0; j < nXbins; ++j) {
                Float_t x = xMin + j * xStep;
                for (Int_t k = 0; k < nYbins; ++k) {
                    Float_t y = yMin + k * yStep;
                    Float_t bNew = (iC == 0) ? fieldNew->GetBx(x, y, z) : (iC == 1) ? fieldNew->GetBy(x, y, z) : (iC == 2) ? fieldNew->GetBz(x, y, z) : 0.0;
                    Float_t bOld = (iC == 0) ? fieldOld->GetBx(x, y, z) : (iC == 1) ? fieldOld->GetBy(x, y, z) : (iC == 2) ? fieldOld->GetBz(x, y, z) : 0.0;
                    hB_XY_new[iC][i]->SetBinContent(k, j, bNew);
                    hB_XY_old[iC][i]->SetBinContent(k, j, bOld);
                    hB_XY_dif[iC][i]->SetBinContent(k, j, bNew - bOld);
                }
            }
            hB_XY_new[iC][i]->Write();
            hB_XY_old[iC][i]->Write();
            hB_XY_dif[iC][i]->Write();
            cB[iC][2]->cd(3 * i + 1);
            hB_XY_new[iC][i]->Draw("colz");
            cB[iC][2]->cd(3 * i + 2);
            hB_XY_old[iC][i]->Draw("colz");
            cB[iC][2]->cd(3 * i + 3);
            hB_XY_dif[iC][i]->Draw("colz");
        }
        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            cB[iC][iPlane]->SaveAs(Form("cB%s_%s.pdf", comp[iC].Data(), planes[iPlane].Data()));
        }
}


    f->Close();

}