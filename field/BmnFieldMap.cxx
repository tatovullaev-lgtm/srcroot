// ---------------------------------------------------------------------------------
// -----                    BmnFieldMap header file                    -------------
// -----                 Created 03/02/2015  by P. Batyuk              -------------
// ---------------------------------------------------------------------------------

#include "BmnFieldMap.h"

#include "BmnFieldPoint.h"
#include "TFile.h"
#include "TMath.h"
#include "TSystem.h"
#include "TTree.h"

#include <iomanip>
#include <iostream>
using namespace std;

BmnFieldMap::BmnFieldMap()
    : FairField()
    , fScale(1.)
    , fPosX(0.)
    , fPosY(0.)
    , fPosZ(0.)
    , fXmin(0.)
    , fXmax(0.)
    , fXstep(0.)
    , fYmin(0.)
    , fYmax(0.)
    , fYstep(0.)
    , fZmin(0.)
    , fZmax(0.)
    , fZstep(0.)
    , fNx(0)
    , fNy(0)
    , fNz(0)
    , fBx(nullptr)
    , fBy(nullptr)
    , fBz(nullptr)
    , fDebugInfo(kFALSE)
    , fIsOff(kFALSE)
{
    // Initilization of arrays is to my knowledge not
    // possible in member initalization lists
    for (Int_t i = 0; i < 2; i++) {
        fHc[i] = 0;
        for (Int_t j = 0; j < 2; j++) {
            fHb[i][j] = 0;
            for (Int_t k = 0; k < 2; k++) {
                fHa[i][j][k] = 0;
            }
        }
    }

    // fName stores file name or path (using $VMCWORKDIR var) with magnetic field map
    fName = "";
    // fType: 1- nosym magnetic field, 2 - sym2, 3 - sym3
    fType = 1;
}

BmnFieldMap::BmnFieldMap(const char* mapFileName)
    : BmnFieldMap()
{
    fName = mapFileName;
    if (!fName.Contains("/"))
        fName = "$VMCWORKDIR/input/" + fName;

    fType = 1;
}

BmnFieldMap::BmnFieldMap(BmnFieldPar* fieldPar)
    : BmnFieldMap()
{
    if (!fieldPar)
        LOG(error) << "BmnFieldMap::BmnFieldMap: empty parameter container";
    else {
        fPosX = fieldPar->GetPositionX();
        fPosY = fieldPar->GetPositionY();
        fPosZ = fieldPar->GetPositionZ();
        fScale = fieldPar->GetScale();
        fIsOff = fieldPar->IsFieldOff();
        fieldPar->MapName(fName);
        if (!fName.Contains("/"))
            fName = "$VMCWORKDIR/input/" + fName;
        fType = fieldPar->GetType();
    }
}

BmnFieldMap::~BmnFieldMap()
{
    if (fBx)
        delete fBx;
    if (fBy)
        delete fBy;
    if (fBz)
        delete fBz;
}

void BmnFieldMap::Init()
{
    if (fName.Contains("extrap")) {
        LOG(info) << "Extrapolated magnetic field map is used";
        gSystem->ExpandPathName(fName);
        ReadRootFileNewFormatExtrap(fName);
    } else {
        LOG(info) << "Basic magnetic field map is used";
        gSystem->ExpandPathName(fName);
        ReadRootFileNewFormat(fName);
    }
    fPosBx = GetBx(fPosX, fPosY, fPosX);
    fPosBy = GetBy(fPosX, fPosY, fPosY);
    fPosBz = GetBz(fPosX, fPosY, fPosZ);
}

void BmnFieldMap::WriteAsciiFile(const char* asciiFileName)
{
    TString strFileName(asciiFileName);
    gSystem->ExpandPathName(strFileName);
    // Open file
    LOG(info) << "BmnFieldMap: Writing field map to ASCII file " << strFileName.Data() << endl;
    ofstream mapFile(strFileName.Data());
    if (!mapFile.is_open()) {
        LOG(error) << "BmnFieldMap:ReadAsciiFile: Could not open file" << endl;
        return;
    }

    // Write field map grid parameters
    mapFile.precision(6);
    mapFile << showpoint;
    if (fType == 1)
        mapFile << "nosym" << endl;
    if (fType == 2)
        mapFile << "sym2" << endl;
    if (fType == 3)
        mapFile << "sym3" << endl;
    mapFile << fXmin << " " << fXmax << " " << fNx << endl;
    mapFile << fYmin << " " << fYmax << " " << fNy << endl;
    mapFile << fZmin << " " << fZmax << " " << fNz << endl;

    // Write field values
    Double_t factor = 10. * fScale;   // Takes out scaling and converts kG->T
    cout << right;
    Int_t nTot = fNx * fNy * fNz;
    LOG(info) << "BmnFieldMap: " << fNx * fNy * fNz << " entries to write... " << setw(3) << 0 << " % ";
    Int_t index = 0;
    div_t modul;
    Int_t iDiv = TMath::Nint(nTot / 100.);
    for (Int_t ix = 0; ix < fNx; ix++) {
        for (Int_t iy = 0; iy < fNy; iy++) {
            for (Int_t iz = 0; iz < fNz; iz++) {
                index = ix * fNy * fNz + iy * fNz + iz;
                modul = div(index, iDiv);
                if (modul.rem == 0) {
                    Double_t perc = TMath::Nint(100. * index / nTot);
                    cout << "\b\b\b\b\b\b" << setw(3) << perc << " % " << flush;
                }
                mapFile << fBx->At(index) / factor << " " << fBy->At(index) / factor << " " << fBz->At(index) / factor
                        << endl;
            }   // z-Loop
        }   // y-Loop
    }   // x-Loop
    cout << "   " << index + 1 << " written" << endl;
    mapFile.close();
}

void BmnFieldMap::WriteRootFile(const char* rootFileName, const char* mapFileName)
{
    BmnFieldMapData* data = new BmnFieldMapData(mapFileName, *this);
    TFile* oldFile = gFile;
    TString strFileName(rootFileName);
    gSystem->ExpandPathName(strFileName);
    TFile* file = TFile::Open(strFileName, "RECREATE");
    data->Write();
    delete file;
    if (oldFile)
        oldFile->cd();
}

void BmnFieldMap::SetPosition(Double_t x, Double_t y, Double_t z)
{
    fPosX = x;
    fPosY = y;
    fPosZ = z;
    fPosBx = GetBx(fPosX, fPosY, fPosX);
    fPosBy = GetBy(fPosX, fPosY, fPosY);
    fPosBz = GetBz(fPosX, fPosY, fPosZ);
}

void BmnFieldMap::Print(Option_t*) const
{
    TString type = "Map";
    if (fType == 2)
        type = "Map sym2";
    if (fType == 3)
        type = "Map sym3";
    cout << "======================================================" << endl;
    cout.precision(4);
    cout << showpoint;
    cout << "----  " << fTitle << " : " << fName << endl;
    cout << "----" << endl;
    cout << "----  Field type     : " << type << endl;
    cout << "----" << endl;
    cout << "----  Field map grid : " << endl;
    cout << "----  x = " << setw(4) << fXmin << " to " << setw(4) << fXmax << " cm, " << fNx
         << " grid points, dx = " << fXstep << " cm" << endl;
    cout << "----  y = " << setw(4) << fYmin << " to " << setw(4) << fYmax << " cm, " << fNy
         << " grid points, dy = " << fYstep << " cm" << endl;
    cout << "----  z = " << setw(4) << fZmin << " to " << setw(4) << fZmax << " cm, " << fNz
         << " grid points, dz = " << fZstep << " cm" << endl;
    cout << endl;
    cout << "----  Target position: ( " << setw(6) << fPosX << ", " << setw(6) << fPosY << ", " << setw(6) << fPosZ
         << ") cm" << endl;
    cout << "----  Field scaling factor: " << fScale << endl;
    if (fIsOff)
        cout << "----  Field is off  ----" << endl;
    cout << "----" << endl;
    cout << "----  Field at Target Position ( " << setw(6) << fPosBx << ", " << setw(6) << fPosBy << ", " << setw(6)
         << fPosBz << ") kG" << endl;
    cout << "======================================================" << endl;
}

void BmnFieldMap::Reset()
{
    fPosX = fPosY = fPosZ = 0.;
    fPosBx = fPosBy = fPosBz = 0.;
    fXmin = fYmin = fZmin = 0.;
    fXmax = fYmax = fZmax = 0.;
    fXstep = fYstep = fZstep = 0.;
    fNx = fNy = fNz = 0;
    fScale = 1.;
    fIsOff = kFALSE;
    if (fBx) {
        delete fBx;
        fBx = NULL;
    }
    if (fBy) {
        delete fBy;
        fBy = NULL;
    }
    if (fBz) {
        delete fBz;
        fBz = NULL;
    }
}

void BmnFieldMap::ReadAsciiFile(const char* asciiFileName)
{
    Double_t bx = 0., by = 0., bz = 0.;

    TString strFileName(asciiFileName);
    gSystem->ExpandPathName(strFileName);
    // Open file
    LOG(info) << "BmnFieldMap: Reading field map from ASCII file " << strFileName.Data();
    ifstream mapFile(strFileName.Data());
    if (!mapFile.is_open()) {
        LOG(error) << "BmnFieldMap:ReadAsciiFile: Could not open file!";
        LOG(fatal) << "BmnFieldMap:ReadAsciiFile: Could not open file!";
    }

    // Read map type
    TString type;
    mapFile >> type;
    Int_t iType = 0;
    if (type == "nosym")
        iType = 1;
    if (type == "sym2")
        iType = 2;
    if (type == "sym3")
        iType = 3;
    if (fType != iType) {
        cout << "-E- BmnFieldMap::ReadAsciiFile: Incompatible map types!" << endl;
        cout << "    Field map is of type " << fType << " but map on file is of type " << iType << endl;
        Fatal("ReadAsciiFile", "Incompatible map types");
    }

    // Read grid parameters
    mapFile >> fXmin >> fXmax >> fNx;
    mapFile >> fYmin >> fYmax >> fNy;
    mapFile >> fZmin >> fZmax >> fNz;
    fXstep = (fXmax - fXmin) / Double_t(fNx - 1);
    fYstep = (fYmax - fYmin) / Double_t(fNy - 1);
    fZstep = (fZmax - fZmin) / Double_t(fNz - 1);

    // Create field arrays
    fBx = new TArrayF(fNx * fNy * fNz);
    fBy = new TArrayF(fNx * fNy * fNz);
    fBz = new TArrayF(fNx * fNy * fNz);

    // Read the field values
    Double_t factor = fScale * 10.;   // Factor 10 for T -> kG
    cout << right;
    Int_t nTot = fNx * fNy * fNz;
    // cout << "-I- BmnFieldMap: " << nTot << " entries to read... " << setw(3) << 0 << " % ";
    Int_t index = 0;
    div_t modul;
    Int_t iDiv = TMath::Nint(nTot / 100.);
    for (Int_t ix = 0; ix < fNx; ix++) {
        for (Int_t iy = 0; iy < fNy; iy++) {
            for (Int_t iz = 0; iz < fNz; iz++) {
                if (!mapFile.good())
                    cerr << "-E- BmnFieldMap::ReadAsciiFile: "
                         << "I/O Error at " << ix << " " << iy << " " << iz << endl;
                index = ix * fNy * fNz + iy * fNz + iz;
                modul = div(index, iDiv);
                // if (modul.rem == 0) {
                //     Double_t perc = TMath::Nint(100. * index / nTot);
                //  cout << "\b\b\b\b\b\b" << setw(3) << perc << " % " << flush;
                //}
                mapFile >> bx >> by >> bz;
                fBx->AddAt(factor * bx, index);
                fBy->AddAt(factor * by, index);
                fBz->AddAt(factor * bz, index);
                if (mapFile.eof()) {
                    cerr << endl
                         << "-E- BmnFieldMap::ReadAsciiFile: EOF"
                         << " reached at " << ix << " " << iy << " " << iz << endl;
                    mapFile.close();
                    break;
                }
            }   // z-Loop
        }   // y-Loop0)
    }   // x-Loop

    cout << "   " << index + 1 << " read" << endl;

    fXmin += fPosX;
    fXmax += fPosX;

    fYmin += fPosY;
    fYmax += fPosY;

    fZmin += fPosZ;
    fZmax += fPosZ;

    mapFile.close();
}

void BmnFieldMap::ReadRootFileNewFormat(const char* rootFileName)
{
    TString strFileName(rootFileName);
    gSystem->ExpandPathName(strFileName);
    // Open root file
    LOG(info) << "BmnFieldMap: Reading field map from ROOT file " << strFileName.Data();
    TFile* file = TFile::Open(strFileName, "READ");
    if ((file == nullptr) || (!file->IsOpen()))
        LOG(fatal) << "BmnFieldMap:ReadRootFileNewFormat: Could not open the file with a field map";

    BmnFieldPoint point;
    TTree* tree = (TTree*)file->Get("Field_map");
    tree->SetBranchAddress("field_points", &point);

    fNx = 126;
    fXmin = -155.78;
    fXmax = 144.82;
    fXstep = (fXmax - fXmin) / (fNx - 1);

    fNy = 47;
    fYmin = -37.67;
    fYmax = 54.33;
    fYstep = (fYmax - fYmin) / (fNy - 1);

    fNz = 241;
    fZmin = -162.16;
    fZmax = 439.04;
    fZstep = (fZmax - fZmin) / (fNz - 1);

    Int_t nTot = fNx * fNy * fNz;
    fBx = new TArrayF(nTot);
    fBy = new TArrayF(nTot);
    fBz = new TArrayF(nTot);

    Int_t index = 0;

    for (Int_t i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        // В дереве, сформированном из CSV файлов, данные записаны в одном порядке, а BmnFieldMap они хранятся в другом.
        // Поэтому сначала из общего сквозного индекса получаем отдельные индексы по каждому измерению.
        // При этом индекс по x инвертируем, так как направление оси в Bmn и в рамке с замерами поля противоположные.
        Int_t iz = i % fNz;
        Int_t ix = fNx - ((i / fNz) % fNx) - 1;
        Int_t iy = ((i / fNz) / fNx) % fNy;
        // После этого формируем итоговый сквозной индекс для карты поля в формате BmnFieldMap
        index = ix * fNy * fNz + iy * fNz + iz;
        // И пишем по этому индексу компоненты, предварительно скалируя их.
        fBx->AddAt(fScale * point.GetBx(), index);
        fBy->AddAt(fScale * point.GetBy(), index);
        fBz->AddAt(fScale * point.GetBz(), index);
    }

    delete file;
}

void BmnFieldMap::ReadRootFileNewFormatExtrap(const char* rootFileName)
{
    TString strFileName(rootFileName);
    gSystem->ExpandPathName(strFileName);
    // Open root file
    LOG(info) << "BmnFieldMap: Reading field map from ROOT file " << strFileName.Data();
    TFile* file = TFile::Open(strFileName, "READ");
    if ((file == nullptr) || (!file->IsOpen()))
        LOG(fatal) << "BmnFieldMap:ReadRootFileNewFormatExtrap: Could not open the file with a field map";

    BmnFieldPoint point;
    TTree* tree = (TTree*)file->Get("Field_map");
    tree->SetBranchAddress("field_points", &point);

    fNx = 206;
    fXmin = -251.972;
    fXmax = 241.012;
    fXstep = (fXmax - fXmin) / (fNx - 1);

    fNy = 87;
    fYmin = -77.67;
    fYmax = 94.33;
    fYstep = (fYmax - fYmin) / (fNy - 1);

    fNz = 341;
    fZmin = -287.41;
    fZmax = 564.29;
    fZstep = (fZmax - fZmin) / (fNz - 1);

    Int_t nTot = fNx * fNy * fNz;
    fBx = new TArrayF(nTot);
    fBy = new TArrayF(nTot);
    fBz = new TArrayF(nTot);

    Int_t index = 0;

    for (Int_t i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        // В дереве, сформированном из CSV файлов, данные записаны в одном порядке, а BmnFieldMap они хранятся в другом.
        // Поэтому сначала из общего сквозного индекса получаем отдельные индексы по каждому измерению.
        // При этом индекс по x инвертируем, так как направление оси в Bmn и в рамке с замерами поля противоположные.
        Int_t iz = i % fNz;
        Int_t ix = fNx - ((i / fNz) % fNx) - 1;
        Int_t iy = ((i / fNz) / fNx) % fNy;
        // После этого формируем итоговый сквозной индекс для карты поля в формате BmnFieldMap
        index = ix * fNy * fNz + iy * fNz + iz;
        // И пишем по этому индексу компоненты, предварительно скалируя их.
        fBx->AddAt(fScale * point.GetBx(), index);
        fBy->AddAt(fScale * point.GetBy(), index);
        fBz->AddAt(fScale * point.GetBz(), index);
    }

    delete file;
}

void BmnFieldMap::ReadRootFile(const char* rootFileName)
{
    TString* type = 0;
    coordinate_info_t X, Y, Z;
    vector<Double_t>* read_Field = NULL;

    TString strFileName(rootFileName);
    gSystem->ExpandPathName(strFileName);
    // Open root file
    LOG(info) << "BmnFieldMap: Reading field map from ROOT file " << strFileName.Data();
    TFile* file = TFile::Open(strFileName, "READ");
    if ((file == nullptr) || (!file->IsOpen()))
        LOG(fatal) << "BmnFieldMap:ReadRootFile: Could not open the file with a field map";

    TTree* tree = (TTree*)file->Get("Main_info");
    TTree* t = (TTree*)file->Get("Field_map");
    tree->SetBranchAddress("Field_type", &type);
    tree->SetBranchAddress("Main_info_X", &X);
    tree->SetBranchAddress("Main_info_Y", &Y);
    tree->SetBranchAddress("Main_info_Z", &Z);
    t->SetBranchAddress("field_map", &read_Field);

    tree->GetEntry(0);
    Int_t iType = 0;

    if (*type == "nosym")
        iType = 1;
    if (*type == "sym2")
        iType = 2;
    if (*type == "sym3")
        iType = 3;
    if (fType != iType) {
        LOG(error) << "BmnFieldMap::ReadRootFile: Incompatible map types!";
        LOG(info) << "    Field map is of type " << fType << " but map on file is of type " << iType;
        LOG(fatal) << "ReadRootFile: Incompatible map types";
    }

    fNx = X.N;
    fXmin = X.min + fPosX;
    fXmax = X.max + fPosX;
    fXstep = X.step;

    fNy = Y.N;
    fYmin = Y.min + fPosY;
    fYmax = Y.max + fPosY;
    fYstep = Y.step;

    fNz = Z.N;
    fZmin = Z.min + fPosZ;
    fZmax = Z.max + fPosZ;
    fZstep = Z.step;

    fBx = new TArrayF(fNx * fNy * fNz);
    fBy = new TArrayF(fNx * fNy * fNz);
    fBz = new TArrayF(fNx * fNy * fNz);

    Double_t factor = fScale;
    Int_t nTot = fNx * fNy * fNz;
    Int_t index = 0;
    div_t modul;
    Int_t iDiv = TMath::Nint(nTot / 100.);

    for (Int_t ix = 0; ix < fNx; ix++) {
        for (Int_t iy = 0; iy < fNy; iy++) {
            for (Int_t iz = 0; iz < fNz; iz++) {
                modul = div(index, iDiv);
                // if (modul.rem == 0) {
                //     Double_t perc = TMath::Nint(100. * index / nTot);
                // }
                index = ix * fNy * fNz + iy * fNz + iz;
                t->GetEntry(index);
                vector<Double_t> v = (*read_Field);
                fBx->AddAt(factor * v[0], index);
                fBy->AddAt(factor * v[1], index);
                fBz->AddAt(factor * v[2], index);
            }   // z-Loop
        }   // y-Loop
    }   // x-Loop

    t->Delete();
    tree->Delete();
    delete type;
    delete read_Field;
    delete file;
}

void BmnFieldMap::SetField(const BmnFieldMapData* data)
{
    // Check compatibility
    if (data->GetType() != fType) {
        if (!((data->GetType() == 3) && (fType == 5)))   // E.Litvinenko
        {
            LOG(error) << "BmnFieldMap::SetField: Incompatible map types!";
            LOG(info) << "    Field map is of type " << fType << " but map on file is of type " << data->GetType();
            LOG(fatal) << "SetField: Incompatible map types";
        } else
            LOG(info) << "   BmnFieldMap::SetField: Warning: You are using PosDepScaled map (original map type = 3)";
    }

    fXmin = data->GetXmin();
    fYmin = data->GetYmin();
    fZmin = data->GetZmin();
    fXmax = data->GetXmax();
    fYmax = data->GetYmax();
    fZmax = data->GetZmax();
    fNx = data->GetNx();
    fNy = data->GetNy();
    fNz = data->GetNz();
    fXstep = (fXmax - fXmin) / Double_t(fNx - 1);
    fYstep = (fYmax - fYmin) / Double_t(fNy - 1);
    fZstep = (fZmax - fZmin) / Double_t(fNz - 1);
    if (fBx)
        delete fBx;
    if (fBy)
        delete fBy;
    if (fBz)
        delete fBz;
    fBx = new TArrayF(*(data->GetBx()));
    fBy = new TArrayF(*(data->GetBy()));
    fBz = new TArrayF(*(data->GetBz()));

    // Scale and convert from T to kG
    Double_t factor = fScale * 10.;
    Int_t index = 0;
    for (Int_t ix = 0; ix < fNx; ix++) {
        for (Int_t iy = 0; iy < fNy; iy++) {
            for (Int_t iz = 0; iz < fNz; iz++) {
                index = ix * fNy * fNz + iy * fNz + iz;
                if (fBx)
                    (*fBx)[index] = (*fBx)[index] * factor;
                if (fBy)
                    (*fBy)[index] = (*fBy)[index] * factor;
                if (fBz)
                    (*fBz)[index] = (*fBz)[index] * factor;
            }
        }
    }
}

Double_t BmnFieldMap::Interpolate(Double_t dx, Double_t dy, Double_t dz)
{
    // Interpolate in x coordinate
    fHb[0][0] = fHa[0][0][0] + (fHa[1][0][0] - fHa[0][0][0]) * dx;
    fHb[1][0] = fHa[0][1][0] + (fHa[1][1][0] - fHa[0][1][0]) * dx;
    fHb[0][1] = fHa[0][0][1] + (fHa[1][0][1] - fHa[0][0][1]) * dx;
    fHb[1][1] = fHa[0][1][1] + (fHa[1][1][1] - fHa[0][1][1]) * dx;

    // Interpolate in y coordinate
    fHc[0] = fHb[0][0] + (fHb[1][0] - fHb[0][0]) * dy;
    fHc[1] = fHb[0][1] + (fHb[1][1] - fHb[0][1]) * dy;

    // Interpolate in z coordinate
    return fHc[0] + (fHc[1] - fHc[0]) * dz;
}
