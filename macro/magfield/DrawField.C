
void DrawPlane() {

    //TString file = "$VMCWORKDIR/input/FieldMap_1900.root";
    TString file = "$VMCWORKDIR/input/FieldMap_1900_extrap.root";
    TFile* myFile = TFile::Open(file);
    TTreeReader myReader("Field_map", myFile);
    TTreeReaderArray<Double_t> x(myReader, "field_points.X");
    TTreeReaderArray<Double_t> y(myReader, "field_points.Y");
    TTreeReaderArray<Double_t> z(myReader, "field_points.Z");
    TTreeReaderArray<Double_t> bx(myReader, "field_points.Bx");
    TTreeReaderArray<Double_t> by(myReader, "field_points.By");
    TTreeReaderArray<Double_t> bz(myReader, "field_points.Bz");

    Float_t fieldScale = -0.929; //minus just for plotting

    TGraph2D* hBy_plane = new TGraph2D();
    TGraph* hBy_proj = new TGraph();

    Int_t pntNumber = 0;
    Int_t cntr1 = 0;

    while (myReader.Next()) {
        if (pntNumber % 10000 == 0) printf("pntNumber = %d\n", pntNumber);
        pntNumber++;
        for (Int_t i = 0; i < x.GetSize(); ++i) {
            if (y[i] < 100 && y[i] > 50) {
                if (x[i] < 1 && x[i] > 0) {
                    //hBy_plane->SetPoint(cntr1++, z[i], x[i], by[i] * fieldScale);
                    hBy_proj->SetPoint(cntr1++, z[i], by[i] * fieldScale);
                }
            }
        }
    }

    gStyle->SetPalette(77);
    gStyle->SetTitleFont(62, "xyz");
    gStyle->SetTitleSize(0.05, "xyz");
    gStyle->SetLabelFont(62, "xyz");
    gStyle->SetLabelSize(0.04, "xyz");
    auto c = new TCanvas("c", "c", 1000, 1000);
    c->Divide(1, 1);
    c->cd(1);
    hBy_proj->Draw("AL");
}

void DrawField() {

    TString file = "$VMCWORKDIR/input/FieldMap_1900_extrap.root";
    TFile* myFile = TFile::Open(file);
    TTreeReader myReader("Field_map", myFile);
    TTreeReaderArray<Double_t> x(myReader, "field_points.X");
    TTreeReaderArray<Double_t> y(myReader, "field_points.Y");
    TTreeReaderArray<Double_t> z(myReader, "field_points.Z");
    TTreeReaderArray<Double_t> bx(myReader, "field_points.Bx");
    TTreeReaderArray<Double_t> by(myReader, "field_points.By");
    TTreeReaderArray<Double_t> bz(myReader, "field_points.Bz");

    Float_t fieldScale = -0.929; //minus just for plotting

    TGraph* hBy_vs_z_minY_minX = new TGraph();
    TGraph* hBy_vs_z_midY_minX = new TGraph();
    TGraph* hBy_vs_z_maxY_minX = new TGraph();
    TGraph* hBy_vs_z_minY_midX = new TGraph();
    TGraph* hBy_vs_z_midY_midX = new TGraph();
    TGraph* hBy_vs_z_maxY_midX = new TGraph();
    TGraph* hBy_vs_z_minY_maxX = new TGraph();
    TGraph* hBy_vs_z_midY_maxX = new TGraph();
    TGraph* hBy_vs_z_maxY_maxX = new TGraph();

    TGraph2D* hBx_min = new TGraph2D();
    TGraph2D* hBx_mid = new TGraph2D();
    TGraph2D* hBx_max = new TGraph2D();
    TGraph2D* hBy_min = new TGraph2D();
    TGraph2D* hBy_mid = new TGraph2D();
    TGraph2D* hBy_max = new TGraph2D();
    TGraph2D* hBz_min = new TGraph2D();
    TGraph2D* hBz_mid = new TGraph2D();
    TGraph2D* hBz_max = new TGraph2D();
    hBx_min->SetTitle("-B_{x} component at bottom plane; Z, cm; X, cm; -B_{x}, kGs");
    hBx_mid->SetTitle("-B_{x} component at middle plane; Z, cm; X, cm; -B_{x}, kGs");
    hBx_max->SetTitle("-B_{x} component at upper plane; Z, cm; X, cm; -B_{x}, kGs");
    hBy_min->SetTitle("-B_{y} component at bottom plane; Z, cm; X, cm; -B_{y}, kGs");
    hBy_mid->SetTitle("-B_{y} component at middle plane; Z, cm; X, cm; -B_{y}, kGs");
    hBy_max->SetTitle("-B_{y} component at upper plane; Z, cm; X, cm; -B_{y}, kGs");
    hBz_min->SetTitle("-B_{z} component at bottom plane; Z, cm; X, cm; -B_{z}, kGs");
    hBz_mid->SetTitle("-B_{z} component at middle plane; Z, cm; X, cm; -B_{z}, kGs");
    hBz_max->SetTitle("-B_{z} component at upper plane; Z, cm; X, cm; -B_{z}, kGs");
    hBy_vs_z_minY_minX->SetTitle("-B_{y} component vs Z for X = min && Y = min; Z, cm; -B_{y}, kGs");
    hBy_vs_z_midY_minX->SetTitle("-B_{y} component vs Z for X = min && Y = mid; Z, cm; -B_{y}, kGs");
    hBy_vs_z_maxY_minX->SetTitle("-B_{y} component vs Z for X = min && Y = max; Z, cm; -B_{y}, kGs");
    hBy_vs_z_minY_midX->SetTitle("-B_{y} component vs Z for X = mid && Y = min; Z, cm; -B_{y}, kGs");
    hBy_vs_z_midY_midX->SetTitle("-B_{y} component vs Z for X = mid && Y = mid; Z, cm; -B_{y}, kGs");
    hBy_vs_z_maxY_midX->SetTitle("-B_{y} component vs Z for X = mid && Y = max; Z, cm; -B_{y}, kGs");
    hBy_vs_z_minY_maxX->SetTitle("-B_{y} component vs Z for X = max && Y = min; Z, cm; -B_{y}, kGs");
    hBy_vs_z_midY_maxX->SetTitle("-B_{y} component vs Z for X = max && Y = mid; Z, cm; -B_{y}, kGs");
    hBy_vs_z_maxY_maxX->SetTitle("-B_{y} component vs Z for X = max && Y = max; Z, cm; -B_{y}, kGs");



    Int_t pntNumber = 0;
    Int_t iP_min = 0;
    Int_t iP_mid = 0;
    Int_t iP_max = 0;
    Int_t cntr1 = 0;
    Int_t cntr2 = 0;
    Int_t cntr3 = 0;
    Int_t cntr4 = 0;
    Int_t cntr5 = 0;
    Int_t cntr6 = 0;
    Int_t cntr7 = 0;
    Int_t cntr8 = 0;
    Int_t cntr9 = 0;
    while (myReader.Next()) {
        if (pntNumber % 10000 == 0) printf("pntNumber = %d\n", pntNumber);
        pntNumber++;
        for (Int_t i = 0; i < x.GetSize(); ++i) {
            if (y[i] < 9 && y[i] > 8) {
                hBx_mid->SetPoint(iP_mid, z[i], x[i], bx[i] * fieldScale);
                hBy_mid->SetPoint(iP_mid, z[i], x[i], by[i] * fieldScale);
                hBz_mid->SetPoint(iP_mid, z[i], x[i], bz[i] * fieldScale);
                iP_mid++;
            } else if (y[i] > 54) {
                hBx_max->SetPoint(iP_max, z[i], x[i], bx[i] * fieldScale);
                hBy_max->SetPoint(iP_max, z[i], x[i], by[i] * fieldScale);
                hBz_max->SetPoint(iP_max, z[i], x[i], bz[i] * fieldScale);
                iP_max++;
            } else if (y[i] < -37) {
                hBx_min->SetPoint(iP_min, z[i], x[i], bx[i] * fieldScale);
                hBy_min->SetPoint(iP_min, z[i], x[i], by[i] * fieldScale);
                hBz_min->SetPoint(iP_min, z[i], x[i], bz[i] * fieldScale);
                iP_min++;
            }
            if (y[i] < 9 && y[i] > 8) {
                if (x[i] < -155)
                    hBy_vs_z_midY_minX->SetPoint(cntr1++, z[i], by[i] * fieldScale);
                if (x[i] > 144)
                    hBy_vs_z_midY_maxX->SetPoint(cntr2++, z[i], by[i] * fieldScale);
                if (x[i] > -1 && x[i] < 1)
                    hBy_vs_z_midY_midX->SetPoint(cntr3++, z[i], by[i] * fieldScale);
            }
            if (y[i] > 54) {
                if (x[i] < -155)
                    hBy_vs_z_maxY_minX->SetPoint(cntr4++, z[i], by[i] * fieldScale);
                if (x[i] > 144)
                    hBy_vs_z_maxY_maxX->SetPoint(cntr5++, z[i], by[i] * fieldScale);
                if (x[i] > -1 && x[i] < 1)
                    hBy_vs_z_maxY_midX->SetPoint(cntr6++, z[i], by[i] * fieldScale);
            }
            if (y[i] < -37) {
                if (x[i] < -155)
                    hBy_vs_z_minY_minX->SetPoint(cntr7++, z[i], by[i] * fieldScale);
                if (x[i] > 144)
                    hBy_vs_z_minY_maxX->SetPoint(cntr8++, z[i], by[i] * fieldScale);
                if (x[i] > -1 && x[i] < 1)
                    hBy_vs_z_minY_midX->SetPoint(cntr9++, z[i], by[i] * fieldScale);
            }
        }
    }
    // hBx_min->SetPoint(iP_min++, -300, -150, 0);
    // hBx_min->SetPoint(iP_min++, 500, +150, 0);`
    // hBx_mid->SetPoint(iP_mid++, -300, -150, 0);
    // hBx_mid->SetPoint(iP_mid++, 500, +150, 0);
    // hBx_max->SetPoint(iP_max++, -300, -150, 0);
    // hBx_max->SetPoint(iP_max++, 500, +150, 0);
    // hBy_min->SetPoint(iP_min++, -300, -150, 0);
    // hBy_min->SetPoint(iP_min++, 500, +150, 0);
    // hBy_mid->SetPoint(iP_mid++, -300, -150, 0);
    // hBy_mid->SetPoint(iP_mid++, 500, +150, 0);
    // hBy_max->SetPoint(iP_max++, -300, -150, 0);
    // hBy_max->SetPoint(iP_max++, 500, +150, 0);
    // hBz_min->SetPoint(iP_min++, -300, -150, 0);
    // hBz_min->SetPoint(iP_min++, 500, +150, 0);
    // hBz_mid->SetPoint(iP_mid++, -300, -150, 0);
    // hBz_mid->SetPoint(iP_mid++, 500, +150, 0);
    // hBz_max->SetPoint(iP_max++, -300, -150, 0);
    // hBz_max->SetPoint(iP_max++, 500, +150, 0);

    gStyle->SetPalette(77);
    gStyle->SetTitleFont(62, "xyz");
    gStyle->SetTitleSize(0.05, "xyz");
    gStyle->SetLabelFont(62, "xyz");
    gStyle->SetLabelSize(0.04, "xyz");
    auto c = new TCanvas("c", "c", 1000, 1000);
    c->Divide(3, 3);
    c->cd(1);
    hBx_min->Draw("surf1");
    c->cd(2);
    hBx_mid->Draw("surf1");
    c->cd(3);
    hBx_max->Draw("surf1");
    c->cd(4);
    hBy_min->Draw("surf1");
    c->cd(5);
    hBy_mid->Draw("surf1");
    c->cd(6);
    hBy_max->Draw("surf1");
    c->cd(7);
    hBz_min->Draw("surf1");
    c->cd(8);
    hBz_mid->Draw("surf1");
    c->cd(9);
    hBz_max->Draw("surf1");


    auto c1 = new TCanvas("c1", "c1", 1000, 1000);
    c1->Divide(3, 3);
    c1->cd(1);
    hBy_vs_z_maxY_maxX->Draw("AL");
    c1->cd(2);
    hBy_vs_z_maxY_midX->Draw("AL");
    c1->cd(3);
    hBy_vs_z_maxY_minX->Draw("AL");
    c1->cd(4);
    hBy_vs_z_midY_maxX->Draw("AL");
    c1->cd(5);
    hBy_vs_z_midY_midX->Draw("AL");
    c1->cd(6);
    hBy_vs_z_midY_minX->Draw("AL");
    c1->cd(7);
    hBy_vs_z_minY_maxX->Draw("AL");
    c1->cd(8);
    hBy_vs_z_minY_midX->Draw("AL");
    c1->cd(9);
    hBy_vs_z_minY_minX->Draw("AL");

}

void CompareFieldMaps() {


    const Int_t nCoord = 3;

    TString coordName[nCoord] = { "x", "y", "z" };

    TGraph* hBOld_vs_X[nCoord];
    TGraph* hBNew_vs_X[nCoord];
    TGraph* hBOld_vs_Y[nCoord];
    TGraph* hBNew_vs_Y[nCoord];
    TGraph* hBOld_vs_Z[nCoord];
    TGraph* hBNew_vs_Z[nCoord];
    TGraph* hB_vs_X_Diff[nCoord];
    TGraph* hB_vs_Y_Diff[nCoord];
    TGraph* hB_vs_Z_Diff[nCoord];

    for (Int_t iCoord = 0; iCoord < nCoord; ++iCoord) {
        hBOld_vs_X[iCoord] = new TGraph();
        hBNew_vs_X[iCoord] = new TGraph();
        hBOld_vs_Y[iCoord] = new TGraph();
        hBNew_vs_Y[iCoord] = new TGraph();
        hBOld_vs_Z[iCoord] = new TGraph();
        hBNew_vs_Z[iCoord] = new TGraph();
        hBOld_vs_X[iCoord]->SetTitle(Form("B_{%s}^{old} component vs X for Z = 130 && Y = 0; X, cm; B_{y}^{old}, kGs", coordName[iCoord].Data()));
        hBNew_vs_X[iCoord]->SetTitle(Form("B_{%s}^{new} component vs X for Z = 130 && Y = 0; X, cm; B_{y}^{new}, kGs", coordName[iCoord].Data()));
        hBOld_vs_X[iCoord]->SetLineColor(kBlue);
        hBNew_vs_X[iCoord]->SetLineColor(kRed);
        hBNew_vs_X[iCoord]->SetLineWidth(2);
        hBOld_vs_X[iCoord]->SetLineWidth(2);
        hBOld_vs_Y[iCoord]->SetTitle(Form("B_{%s}^{old} component vs Y for Z = 130 && X = 0; Y, cm; B_{y}^{old}, kGs", coordName[iCoord].Data()));
        hBNew_vs_Y[iCoord]->SetTitle(Form("B_{%s}^{new} component vs Y for Z = 130 && X = 0; Y, cm; B_{y}^{new}, kGs", coordName[iCoord].Data()));
        hBOld_vs_Y[iCoord]->SetLineColor(kBlue);
        hBNew_vs_Y[iCoord]->SetLineColor(kRed);
        hBNew_vs_Y[iCoord]->SetLineWidth(2);
        hBOld_vs_Y[iCoord]->SetLineWidth(2);
        hBOld_vs_Z[iCoord]->SetTitle(Form("B_{%s}^{old} component vs Z for X = 0 && Y = 0; Z, cm; B_{y}^{old}, kGs", coordName[iCoord].Data()));
        hBNew_vs_Z[iCoord]->SetTitle(Form("B_{%s}^{new} component vs Z for X = 0 && Y = 0; Z, cm; B_{y}^{new}, kGs", coordName[iCoord].Data()));
        hBOld_vs_Z[iCoord]->SetLineColor(kBlue);
        hBNew_vs_Z[iCoord]->SetLineColor(kRed);
        hBNew_vs_Z[iCoord]->SetLineWidth(2);
        hBOld_vs_Z[iCoord]->SetLineWidth(2);

        hB_vs_X_Diff[iCoord] = new TGraph();
        hB_vs_Y_Diff[iCoord] = new TGraph();
        hB_vs_Z_Diff[iCoord] = new TGraph();
        hB_vs_X_Diff[iCoord]->SetTitle(Form("B_{%s} component difference vs X for Z = 130 && Y = 0; X, cm; B_{y}^{new} - B_{y}^{old}, kGs", coordName[iCoord].Data()));
        hB_vs_X_Diff[iCoord]->SetLineColor(kBlack);
        hB_vs_X_Diff[iCoord]->SetLineWidth(2);
        hB_vs_Y_Diff[iCoord]->SetTitle(Form("B_{%s} component difference vs Y for Z = 130 && X = 0; Y, cm; B_{y}^{new} - B_{y}^{old}, kGs", coordName[iCoord].Data()));
        hB_vs_Y_Diff[iCoord]->SetLineColor(kBlack);
        hB_vs_Y_Diff[iCoord]->SetLineWidth(2);
        hB_vs_Z_Diff[iCoord]->SetTitle(Form("B_{%s} component difference vs Z for X = 0 && Y = 0; Z, cm; B_{y}^{new} - B_{y}^{old}, kGs", coordName[iCoord].Data()));
        hB_vs_Z_Diff[iCoord]->SetLineColor(kBlack);
        hB_vs_Z_Diff[iCoord]->SetLineWidth(2);
    }


    BmnFieldMap* magFieldNew = new BmnNewFieldMap("FieldMap_1900_extrap.root");
    //BmnFieldMap* magFieldNew = new BmnNewFieldMap("FieldMap_1900_new_shifted.root");
    Double_t fieldScaleNew = 0.929;
    magFieldNew->SetScale(fieldScaleNew);
    magFieldNew->Init();

    BmnFieldMap* magFieldOld = new BmnNewFieldMap("field_sp41v5_ascii_Extrap.root");
    Double_t fieldScaleOld = 1.861;
    magFieldOld->SetScale(fieldScaleOld);
    magFieldOld->Init();

    const Float_t xMin = -100.0;
    const Float_t xMax = 100.0;
    const Float_t dx = 5.0;

    const Float_t yMin = -37.0;
    const Float_t yMax = 40.0;
    const Float_t dy = 1.0;

    const Float_t dz = 10.0;
    const Float_t zMax = 500.0;
    const Float_t zMin = -200.0;

    Float_t x, y, z;

    x = 0;
    y = 0;
    z = zMin;
    while (z < zMax) {

        Float_t bxOld = magFieldOld->GetBx(x, y, z);
        Float_t bxNew = magFieldNew->GetBx(x, y, z);
        Float_t byOld = magFieldOld->GetBy(x, y, z);
        Float_t byNew = magFieldNew->GetBy(x, y, z);
        Float_t bzOld = magFieldOld->GetBz(x, y, z);
        Float_t bzNew = magFieldNew->GetBz(x, y, z);
        // printf("(%4.2f, %4.2f, %4.2f):\t%4.2f |\t%4.2f |\t%4.2f \n", x, y, z, byNew, byOld, byNew - byOld);

        hBOld_vs_Z[0]->SetPoint(hBOld_vs_Z[0]->GetN(), z, bxOld);
        hBNew_vs_Z[0]->SetPoint(hBNew_vs_Z[0]->GetN(), z, bxNew);
        hB_vs_Z_Diff[0]->SetPoint(hB_vs_Z_Diff[0]->GetN(), z, bxNew - bxOld);

        hBOld_vs_Z[1]->SetPoint(hBOld_vs_Z[1]->GetN(), z, byOld);
        hBNew_vs_Z[1]->SetPoint(hBNew_vs_Z[1]->GetN(), z, byNew);
        hB_vs_Z_Diff[1]->SetPoint(hB_vs_Z_Diff[1]->GetN(), z, byNew - byOld);

        hBOld_vs_Z[2]->SetPoint(hBOld_vs_Z[2]->GetN(), z, bzOld);
        hBNew_vs_Z[2]->SetPoint(hBNew_vs_Z[2]->GetN(), z, bzNew);
        hB_vs_Z_Diff[2]->SetPoint(hB_vs_Z_Diff[2]->GetN(), z, bzNew - bzOld);

        z += dz;
    }

    x = xMin;
    y = 0;
    z = 130;
    while (x < xMax) {

        Float_t bxOld = magFieldOld->GetBx(x, y, z);
        Float_t bxNew = magFieldNew->GetBx(x, y, z);
        Float_t byOld = magFieldOld->GetBy(x, y, z);
        Float_t byNew = magFieldNew->GetBy(x, y, z);
        Float_t bzOld = magFieldOld->GetBz(x, y, z);
        Float_t bzNew = magFieldNew->GetBz(x, y, z);
        // printf("(%4.2f, %4.2f, %4.2f):\t%4.2f |\t%4.2f |\t%4.2f \n", x, y, z, byNew, byOld, byNew - byOld);

        hBOld_vs_X[0]->SetPoint(hBOld_vs_X[0]->GetN(), x, bxOld);
        hBNew_vs_X[0]->SetPoint(hBNew_vs_X[0]->GetN(), x, bxNew);
        hB_vs_X_Diff[0]->SetPoint(hB_vs_X_Diff[0]->GetN(), x, bxNew - bxOld);

        hBOld_vs_X[1]->SetPoint(hBOld_vs_X[1]->GetN(), x, byOld);
        hBNew_vs_X[1]->SetPoint(hBNew_vs_X[1]->GetN(), x, byNew);
        hB_vs_X_Diff[1]->SetPoint(hB_vs_X_Diff[1]->GetN(), x, byNew - byOld);

        hBOld_vs_X[2]->SetPoint(hBOld_vs_X[2]->GetN(), x, bzOld);
        hBNew_vs_X[2]->SetPoint(hBNew_vs_X[2]->GetN(), x, bzNew);
        hB_vs_X_Diff[2]->SetPoint(hB_vs_X_Diff[2]->GetN(), x, bzNew - bzOld);

        x += dx;
    }

    x = 0;
    y = yMin;
    z = 130;
    while (y < yMax) {

        Float_t bxOld = magFieldOld->GetBx(x, y, z);
        Float_t bxNew = magFieldNew->GetBx(x, y, z);
        Float_t byOld = magFieldOld->GetBy(x, y, z);
        Float_t byNew = magFieldNew->GetBy(x, y, z);
        Float_t bzOld = magFieldOld->GetBz(x, y, z);
        Float_t bzNew = magFieldNew->GetBz(x, y, z);
        // printf("(%4.2f, %4.2f, %4.2f):\t%4.2f |\t%4.2f |\t%4.2f \n", x, y, z, byNew, byOld, byNew - byOld);

        hBOld_vs_Y[0]->SetPoint(hBOld_vs_Y[0]->GetN(), y, bxOld);
        hBNew_vs_Y[0]->SetPoint(hBNew_vs_Y[0]->GetN(), y, bxNew);
        hB_vs_Y_Diff[0]->SetPoint(hB_vs_Y_Diff[0]->GetN(), y, bxNew - bxOld);

        hBOld_vs_Y[1]->SetPoint(hBOld_vs_Y[1]->GetN(), y, byOld);
        hBNew_vs_Y[1]->SetPoint(hBNew_vs_Y[1]->GetN(), y, byNew);
        hB_vs_Y_Diff[1]->SetPoint(hB_vs_Y_Diff[1]->GetN(), y, byNew - byOld);

        hBOld_vs_Y[2]->SetPoint(hBOld_vs_Y[2]->GetN(), y, bzOld);
        hBNew_vs_Y[2]->SetPoint(hBNew_vs_Y[2]->GetN(), y, bzNew);
        hB_vs_Y_Diff[2]->SetPoint(hB_vs_Y_Diff[2]->GetN(), y, bzNew - bzOld);

        y += dy;
    }

    TCanvas* c[nCoord];
    TMultiGraph* mgX[nCoord];
    TMultiGraph* mgY[nCoord];
    TMultiGraph* mgZ[nCoord];
    for (Int_t iCoord = 0; iCoord < nCoord; ++iCoord) {
        c[iCoord] = new TCanvas(Form("c_%s", coordName[iCoord].Data()), Form("c_%s", coordName[iCoord].Data()), 1000, 500);
        c[iCoord]->Divide(3, 2);
        mgX[iCoord] = new TMultiGraph();
        mgX[iCoord]->Add(hBOld_vs_X[iCoord]);
        mgX[iCoord]->Add(hBNew_vs_X[iCoord]);
        c[iCoord]->cd(1);
        mgX[iCoord]->Draw("AL");
        mgY[iCoord] = new TMultiGraph();
        mgY[iCoord]->Add(hBOld_vs_Y[iCoord]);
        mgY[iCoord]->Add(hBNew_vs_Y[iCoord]);
        c[iCoord]->cd(2);
        mgY[iCoord]->Draw("AL");
        mgZ[iCoord] = new TMultiGraph();
        mgZ[iCoord]->Add(hBOld_vs_Z[iCoord]);
        mgZ[iCoord]->Add(hBNew_vs_Z[iCoord]);
        c[iCoord]->cd(3);
        mgZ[iCoord]->Draw("AL");
        c[iCoord]->cd(4);
        hB_vs_X_Diff[iCoord]->Draw("AL");
        c[iCoord]->cd(5);
        hB_vs_Y_Diff[iCoord]->Draw("AL");
        c[iCoord]->cd(6);
        hB_vs_Z_Diff[iCoord]->Draw("AL");
    }


}