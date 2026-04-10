using namespace TMath;

void CheckMap() {


  TString name1900 = "FieldMap_1900_extrap.root";
  TFile* file1900 = TFile::Open(name1900);
  TTreeReader reader("Field_map", file1900);
  TTreeReaderArray<Double_t> x(reader, "field_points.X");
  TTreeReaderArray<Double_t> y(reader, "field_points.Y");
  TTreeReaderArray<Double_t> z(reader, "field_points.Z");
  TTreeReaderArray<Double_t> bx(reader, "field_points.Bx");
  TTreeReaderArray<Double_t> by(reader, "field_points.By");
  TTreeReaderArray<Double_t> bz(reader, "field_points.Bz");

  Int_t fNx = 206;
  Int_t fNy = 87;
  Int_t fNz = 341;

  TGraph* gBy_vs_z[fNy][fNx];

  Int_t pntNumber = 0;
  while (reader.Next()) {
    if (pntNumber % 10000 == 0) printf("pntNumber = %d\n", pntNumber);

    Int_t iz = pntNumber % fNz;
    Int_t ix = (pntNumber / fNz) % fNx;
    Int_t iy = ((pntNumber / fNz) / fNx) % fNy;

    if (iz == 0) {
      gBy_vs_z[iy][ix] = new TGraph(fNz);
      gBy_vs_z[iy][ix]->SetTitle(Form("%3.2f_%3.2f", y[0], x[0]));
      gBy_vs_z[iy][ix]->SetName(Form("%3.2f_%3.2f", y[0], x[0]));
    }

    gBy_vs_z[iy][ix]->SetPoint(iz, z[0], by[0]);

    pntNumber++;
  }


  TFile* f = new TFile("MagFieldGraphs.root", "RECREATE");
  for (Int_t iy = 0; iy < fNy; ++iy) {
    for (Int_t ix = 0; ix < fNx; ++ix) {
      Bool_t badGraph = kFALSE;
      for (Int_t iz = 1; iz < fNz; ++iz) {
        if (Abs(gBy_vs_z[iy][ix]->GetPointY(iz) - gBy_vs_z[iy][ix]->GetPointY(iz - 1)) > 1.0) {
          badGraph = kTRUE;
          break;
        }
      }
      if (badGraph) {
        cout << iy << "  " << ix << endl;
        gBy_vs_z[iy][ix]->Write();
      }
    }
  }
  f->Write();

}
