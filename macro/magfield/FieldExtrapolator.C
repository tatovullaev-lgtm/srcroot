

using namespace TMath;

void FieldExtrapolator() {


  TString name1900 = "FieldMap_1900.root";
  TFile* file1900 = TFile::Open(name1900);
  TTreeReader reader("Field_map", file1900);
  TTreeReaderArray<Double_t> x(reader, "field_points.X");
  TTreeReaderArray<Double_t> y(reader, "field_points.Y");
  TTreeReaderArray<Double_t> z(reader, "field_points.Z");
  TTreeReaderArray<Double_t> bx(reader, "field_points.Bx");
  TTreeReaderArray<Double_t> by(reader, "field_points.By");
  TTreeReaderArray<Double_t> bz(reader, "field_points.Bz");

  Int_t fNx = 126;
  Double_t fXmin = -155.78;
  Double_t fXmax = 144.82;
  Double_t fXstep = (fXmax - fXmin) / (fNx - 1);

  Int_t fNy = 47;
  Double_t fYmin = -37.67;
  Double_t fYmax = 54.33;
  Double_t fYstep = (fYmax - fYmin) / (fNy - 1);

  Int_t fNz = 241;
  Double_t fZmin = -162.16;
  Double_t fZmax = 439.04;
  Double_t fZstep = (fZmax - fZmin) / (fNz - 1);

  cout << fXstep << " " << fYstep << " " << fZstep << endl;
  //fXstep = 2.4cm
  //fYstep = 2.0cm
  //fZstep = 2.5cm

  Int_t nExtraPointsX = 40; //количество точек, добавляемых справа и слева от графиков после экстраполяции по X, 40 точек соответствует +-96 см
  Int_t nExtraPointsY = 20; //количество точек, добавляемых справа и слева от графиков после экстраполяции по Y, 20 точек соответствует +-40 см
  Int_t nExtraPointsZ = 50; //количество точек, добавляемых справа и слева от графиков после экстраполяции по z, 50 точек соответствует +-125 см

  TGraph* gBx_vs_z[fNy][fNx];
  TGraph* gBy_vs_z[fNy][fNx];
  TGraph* gBz_vs_z[fNy][fNx];

  Int_t pntNumber = 0;
  while (reader.Next()) {
    if (pntNumber % 10000 == 0) printf("pntNumber = %d\n", pntNumber);

    Int_t iz = pntNumber % fNz;
    Int_t ix = (pntNumber / fNz) % fNx;
    Int_t iy = ((pntNumber / fNz) / fNx) % fNy;

    if (iz == 0) {
      gBx_vs_z[iy][ix] = new TGraph(fNz + nExtraPointsZ + nExtraPointsZ);
      gBy_vs_z[iy][ix] = new TGraph(fNz + nExtraPointsZ + nExtraPointsZ);
      gBz_vs_z[iy][ix] = new TGraph(fNz + nExtraPointsZ + nExtraPointsZ);
    }
    Double_t Bx = bx[0];
    Double_t By = by[0];
    Double_t Bz = bz[0];

    gBx_vs_z[iy][ix]->SetPoint(iz + nExtraPointsZ, z[0], Bx);
    gBy_vs_z[iy][ix]->SetPoint(iz + nExtraPointsZ, z[0], Abs(By));
    gBz_vs_z[iy][ix]->SetPoint(iz + nExtraPointsZ, z[0], Bz);

    pntNumber++;
  }

  for (Int_t iy = 0; iy < fNy; ++iy) {
    for (Int_t ix = 0; ix < fNx; ++ix) {

      TF1* ry = new TF1("rightY", "expo");
      TF1* ly = new TF1("leftY", "expo");
      Bool_t badFitL = kFALSE;
      Bool_t badFitR = kFALSE;
      if (gBy_vs_z[iy][ix]->Fit("rightY", "S0Q+", "", 420, fZmax)->IsValid() == kFALSE || ry->GetParameter(1) > 0) {
        badFitR = kTRUE;
      }
      if (gBy_vs_z[iy][ix]->Fit("leftY", "S0Q+", "", fZmin, -140)->IsValid() == kFALSE || ly->GetParameter(1) < 0) {
        badFitL = kTRUE;
      }

      for (Int_t iz = 0; iz < nExtraPointsZ; ++iz) {
        Double_t z = fZmax + iz * fZstep;
        gBx_vs_z[iy][ix]->SetPoint(fNz + nExtraPointsZ + iz, z, 0.0/*rx->Eval(z)*/);
        gBy_vs_z[iy][ix]->SetPoint(fNz + nExtraPointsZ + iz, z, (badFitR) ? 0.0 : ry->Eval(z));
        gBz_vs_z[iy][ix]->SetPoint(fNz + nExtraPointsZ + iz, z, 0.0/*rz->Eval(z)*/);
      }
      for (Int_t iz = 0; iz < nExtraPointsZ; ++iz) {
        Double_t z = fZmin - iz * fZstep;
        gBx_vs_z[iy][ix]->SetPoint(nExtraPointsZ - 1 - iz, z, 0.0/*lx->Eval(z)*/);
        gBy_vs_z[iy][ix]->SetPoint(nExtraPointsZ - 1 - iz, z, (badFitL) ? 0.0 : ly->Eval(z));
        gBz_vs_z[iy][ix]->SetPoint(nExtraPointsZ - 1 - iz, z, 0.0/*lz->Eval(z)*/);
      }
      delete ry;
      delete ly;
    }
  }

  fZmin -= (nExtraPointsZ) * fZstep;
  fNz = gBy_vs_z[0][0]->GetN(); //обновляем количество узлов по Z после успешной (надеюсь :) ) экстраполяции

  TGraph* gBx_vs_x[fNy][fNz];
  TGraph* gBy_vs_x[fNy][fNz];
  TGraph* gBz_vs_x[fNy][fNz];
  for (Int_t iy = 0; iy < fNy; ++iy) {
    for (Int_t iz = 0; iz < fNz; ++iz) {
      gBx_vs_x[iy][iz] = new TGraph(fNx + nExtraPointsX + nExtraPointsX);
      gBy_vs_x[iy][iz] = new TGraph(fNx + nExtraPointsX + nExtraPointsX);
      gBz_vs_x[iy][iz] = new TGraph(fNx + nExtraPointsX + nExtraPointsX);
    }
  }

  for (Int_t iy = 0; iy < fNy; ++iy) {
    for (Int_t ix = 0; ix < fNx; ++ix) {
      Double_t x = fXmin + ix * fXstep;
      for (Int_t iz = 0; iz < fNz; ++iz) {
        Double_t bx = gBx_vs_z[iy][ix]->GetPointY(iz);
        Double_t by = gBy_vs_z[iy][ix]->GetPointY(iz);
        Double_t bz = gBz_vs_z[iy][ix]->GetPointY(iz);
        gBx_vs_x[iy][iz]->SetPoint(ix + nExtraPointsX, x, bx);
        gBy_vs_x[iy][iz]->SetPoint(ix + nExtraPointsX, x, by);
        gBz_vs_x[iy][iz]->SetPoint(ix + nExtraPointsX, x, bz);
      }
    }
  }

  for (Int_t iy = 0; iy < fNy; ++iy) {
    for (Int_t iz = 0; iz < fNz; ++iz) {

      TF1* ry = new TF1("rightY", "expo");
      TF1* ly = new TF1("leftY", "expo");
      Bool_t badFitL = kFALSE;
      Bool_t badFitR = kFALSE;
      if (gBy_vs_x[iy][iz]->Fit("rightY", "S0Q+", "", 120, 145)->IsValid() == kFALSE || ry->GetParameter(1) > 0) {
        badFitR = kTRUE;
      }
      if (gBy_vs_x[iy][iz]->Fit("leftY", "S0Q+", "", -155, -130)->IsValid() == kFALSE || ly->GetParameter(1) < 0) {
        badFitL = kTRUE;
      }
      
      for (Int_t ix = 0; ix < nExtraPointsX; ++ix) {
        Double_t x = fXmax + ix * fXstep;
        gBx_vs_x[iy][iz]->SetPoint(fNx + nExtraPointsX + ix, x, 0.0/*rx->Eval(x)*/);
        gBy_vs_x[iy][iz]->SetPoint(fNx + nExtraPointsX + ix, x, (badFitR) ? 0.0 : ry->Eval(x));
        gBz_vs_x[iy][iz]->SetPoint(fNx + nExtraPointsX + ix, x, 0.0/*rz->Eval(x)*/);
      }
      for (Int_t ix = 0; ix < nExtraPointsX; ++ix) {
        Double_t x = fXmin - ix * fXstep;
        gBx_vs_x[iy][iz]->SetPoint(nExtraPointsX - 1 - ix, x, 0.0/*lx->Eval(x)*/);
        gBy_vs_x[iy][iz]->SetPoint(nExtraPointsX - 1 - ix, x, (badFitL) ? 0.0 : ly->Eval(x));
        gBz_vs_x[iy][iz]->SetPoint(nExtraPointsX - 1 - ix, x, 0.0/*lz->Eval(x)*/);
      }

      delete ry;
      delete ly;
    }
  }

  fXmin -= (nExtraPointsX) * fXstep;
  fNx = gBy_vs_x[0][0]->GetN(); //обновляем количество узлов по X после успешной (надеюсь :) ) экстраполяции

  TGraph* gBx_vs_y[fNx][fNz];
  TGraph* gBy_vs_y[fNx][fNz];
  TGraph* gBz_vs_y[fNx][fNz];
  for (Int_t ix = 0; ix < fNx; ++ix) {
    for (Int_t iz = 0; iz < fNz; ++iz) {
      gBx_vs_y[ix][iz] = new TGraph(fNy + nExtraPointsY + nExtraPointsY);
      gBy_vs_y[ix][iz] = new TGraph(fNy + nExtraPointsY + nExtraPointsY);
      gBz_vs_y[ix][iz] = new TGraph(fNy + nExtraPointsY + nExtraPointsY);
    }
  }

  for (Int_t iy = 0; iy < fNy; ++iy) {
    Double_t y = fYmin + iy * fYstep;
    for (Int_t ix = 0; ix < fNx; ++ix) {
      for (Int_t iz = 0; iz < fNz; ++iz) {
        Double_t bx = gBx_vs_x[iy][iz]->GetPointY(ix);
        Double_t by = gBy_vs_x[iy][iz]->GetPointY(ix);
        Double_t bz = gBz_vs_x[iy][iz]->GetPointY(ix);
        gBx_vs_y[ix][iz]->SetPoint(iy + nExtraPointsY, y, bx);
        gBy_vs_y[ix][iz]->SetPoint(iy + nExtraPointsY, y, by);
        gBz_vs_y[ix][iz]->SetPoint(iy + nExtraPointsY, y, bz);
      }
    }
  }

  for (Int_t ix = 0; ix < fNx; ++ix) {
    for (Int_t iz = 0; iz < fNz; ++iz) {

      TF1* ry = new TF1("rightY", "expo");
      TF1* ly = new TF1("leftY", "expo");
      Bool_t badFitL = kFALSE;
      Bool_t badFitR = kFALSE;
      if (gBy_vs_y[ix][iz]->Fit("rightY", "S0Q+", "", 40, 55)->IsValid() == kFALSE || ry->GetParameter(1) > 0) {
        badFitR = kTRUE;
      }
      if (gBy_vs_y[ix][iz]->Fit("leftY", "S0Q+", "", -40, -20)->IsValid() == kFALSE || ly->GetParameter(1) < 0) {
        badFitL = kTRUE;
      }

      for (Int_t iy = 0; iy < nExtraPointsY; ++iy) {
        Double_t y = fYmax + iy * fYstep;
        gBx_vs_y[ix][iz]->SetPoint(fNy + nExtraPointsY + iy, y, 0.0/*rx->Eval(y)*/);
        gBy_vs_y[ix][iz]->SetPoint(fNy + nExtraPointsY + iy, y, (badFitR) ? 0.0 : ry->Eval(y));
        gBz_vs_y[ix][iz]->SetPoint(fNy + nExtraPointsY + iy, y, 0.0/*rz->Eval(y)*/);
      }
      for (Int_t iy = 0; iy < nExtraPointsY; ++iy) {
        Double_t y = fYmin - iy * fYstep;
        gBx_vs_y[ix][iz]->SetPoint(nExtraPointsY - 1 - iy, y, 0.0/*lx->Eval(y)*/);
        gBy_vs_y[ix][iz]->SetPoint(nExtraPointsY - 1 - iy, y, (badFitL) ? 0.0 : ly->Eval(y));
        gBz_vs_y[ix][iz]->SetPoint(nExtraPointsY - 1 - iy, y, 0.0/*lz->Eval(y)*/);
      }

      delete ry;
      delete ly;
    }
  }

  fYmin -= (nExtraPointsY) * fYstep;
  fNy = gBy_vs_y[0][0]->GetN(); //обновляем количество узлов по Y после успешной (надеюсь :) ) экстраполяции


  TString out = "FieldMap_1900_extrap.root";
  TFile* f = new TFile(out.Data(), "RECREATE");
  TTree* t = new TTree("Field_map", "data from ASCII");

  BmnFieldPoint pointToWrite;
  t->Branch("field_points", &pointToWrite, "X/D:Y/D:Z/D:Bx/D:By/D:Bz/D");

  cout << fXmin << " " << fXmin + (fNx - 1) * fXstep << endl;
  cout << fYmin << " " << fYmin + (fNy - 1) * fYstep << endl;
  cout << fZmin << " " << fZmin + (fNz - 1) * fZstep << endl;

  for (Int_t iy = 0; iy < fNy; ++iy) {
    for (Int_t ix = 0; ix < fNx; ++ix) {
      for (Int_t iz = 0; iz < fNz; ++iz) {
        Double_t xOut = fXmin + ix * fXstep;
        Double_t yOut = fYmin + iy * fYstep;
        Double_t zOut = fZmin + iz * fZstep;

        Double_t BxOut = gBx_vs_y[ix][iz]->GetPointY(iy);
        Double_t ByOut = -gBy_vs_y[ix][iz]->GetPointY(iy); //минуc так как брал модуль для нормального фитирования.
        Double_t BzOut = gBz_vs_y[ix][iz]->GetPointY(iy);
        BmnFieldPoint tmpPnt(xOut, yOut, zOut, BxOut, ByOut, BzOut);
        pointToWrite = tmpPnt;
        t->Fill();
      }
    }
  }

  f->Write();

}
