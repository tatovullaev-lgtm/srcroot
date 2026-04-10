/*
Macro takes files with magnetic field measurements and
changes planes with data from old Hall sensor
to the average value of two neighboring planes.
Author: S. Lebedeva (SЗSU)
*/

R__ADD_INCLUDE_PATH($VMCWORKDIR)

typedef struct {
    Int_t N;
    Float_t min;
    Float_t max;
    Float_t step;
} Coordinate_info_t;

void create_files() {

    const Int_t nZ = 47; 
    vector<TString> files[nZ];

    //==== Z=1 ====//
    files[0].push_back("/home/merz/data/magfield/1900/1_new/2022x05x30_file_3_5250_CW.csv");
    files[0].push_back("/home/merz/data/magfield/1900/1_new/2022x05x30_file_2_2000_CW.csv");
    files[0].push_back("/home/merz/data/magfield/1900/1_new/2022x05x30_file_1_0_CW.csv");

    //==== Z=5 ====//
    files[2].push_back("/home/merz/data/magfield/1900/5_new/2022x05x30_file_3_4500_CW.csv");
    files[2].push_back("/home/merz/data/magfield/1900/5_new/2022x05x30_file_2_3350_CW.csv");
    files[2].push_back("/home/merz/data/magfield/1900/5_new/2022x05x30_file_1_0_CW.csv");
    
    //==== Z=9 ====//
    files[4].push_back("/home/merz/data/magfield/1900/9_new/2022x05x30_file_4_4450_CW.csv");
    files[4].push_back("/home/merz/data/magfield/1900/9_new/2022x05x29_file_3_1775_CW.csv");
    files[4].push_back("/home/merz/data/magfield/1900/9_new/2022x05x29_file_2_0_CW.csv");
    files[4].push_back("/home/merz/data/magfield/1900/9_new/2022x05x29_file_1_0_CW.csv");
    
     //==== Z=13 ====//
    files[6].push_back("/home/merz/data/magfield/1900/13_new/2022x05x29_file_5_5300_CW.csv");
    files[6].push_back("/home/merz/data/magfield/1900/13_new/2022x05x29_file_4_2675_CW.csv");
    files[6].push_back("/home/merz/data/magfield/1900/13_new/2022x05x29_file_3_2675_CW.csv");
    files[6].push_back("/home/merz/data/magfield/1900/13_new/2022x05x29_file_2_25_CW.csv");
    files[6].push_back("/home/merz/data/magfield/1900/13_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=17 ====//
    files[8].push_back("/home/merz/data/magfield/1900/17_new/2022x05x29_file_3_3500_CW.csv");
    files[8].push_back("/home/merz/data/magfield/1900/17_new/2022x05x29_file_2_3325_CW.csv");
    files[8].push_back("/home/merz/data/magfield/1900/17_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=21 ====//
    files[10].push_back("/home/merz/data/magfield/1900/21_new/2022x05x29_file_2_3275_CW.csv");
    files[10].push_back("/home/merz/data/magfield/1900/21_new/2022x05x29_file_1_0_CW.csv");
    
    //==== Z=25 ====//    
    files[12].push_back("/home/merz/data/magfield/1900/25/2022x05x26_file_10_5725_CW.csv");
    files[12].push_back("/home/merz/data/magfield/1900/25/2022x05x26_file_9_3900_CW.csv");
    files[12].push_back("/home/merz/data/magfield/1900/25/2022x05x26_file_8_2100_CW.csv");
    files[12].push_back("/home/merz/data/magfield/1900/25/2022x05x26_file_7_1675_CW.csv");
    files[12].push_back("/home/merz/data/magfield/1900/25/2022x05x26_file_6_1450_CW.csv");
    files[12].push_back("/home/merz/data/magfield/1900/25/2022x05x26_file_5_1225_CW.csv");
    files[12].push_back("/home/merz/data/magfield/1900/25/2022x05x26_file_4_1000_CW.csv");
    files[12].push_back("/home/merz/data/magfield/1900/25/2022x05x26_file_3_550_CW.csv");
    files[12].push_back("/home/merz/data/magfield/1900/25/2022x05x26_file_2_325_CW.csv");
    files[12].push_back("/home/merz/data/magfield/1900/25/2022x05x26_file_1_0_CW.csv");

    //==== Z=29 ====//
    files[14].push_back("/home/merz/data/magfield/1900/29/2022x05x26_file_4_6000_CW.csv");
    files[14].push_back("/home/merz/data/magfield/1900/29/2022x05x26_file_3_5125_CW.csv");
    files[14].push_back("/home/merz/data/magfield/1900/29/2022x05x26_file_2_3500_CW.csv");
    files[14].push_back("/home/merz/data/magfield/1900/29/2022x05x26_file_1_0_CW.csv");

    //==== Z=31 ====//
    files[15].push_back("/home/merz/data/magfield/1900/31_new/2022x05x29_file_3_4725_CW.csv");
    files[15].push_back("/home/merz/data/magfield/1900/31_new/2022x05x29_file_2_1800_CW.csv");
    files[15].push_back("/home/merz/data/magfield/1900/31_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=33 ====//
    files[16].push_back("/home/merz/data/magfield/1900/33/2022x05x26_file_4_4625_CW.csv");
    files[16].push_back("/home/merz/data/magfield/1900/33/2022x05x26_file_3_4275_CW.csv");
    files[16].push_back("/home/merz/data/magfield/1900/33/2022x05x26_file_2_2250_CW.csv");
    files[16].push_back("/home/merz/data/magfield/1900/33/2022x05x26_file_1_0_CW.csv");
    
    //==== Z=37 ====//
    files[18].push_back("/home/merz/data/magfield/1900/37_new/2022x05x29_file_3_5175_CW.csv");
    files[18].push_back("/home/merz/data/magfield/1900/37_new/2022x05x29_file_2_2150_CW.csv");
    files[18].push_back("/home/merz/data/magfield/1900/37_new/2022x05x29_file_1_0_CW.csv");
    
    //==== Z=41 ====//
    files[20].push_back("/home/merz/data/magfield/1900/41_new/2022x05x29_file_3_5650_CW.csv");
    files[20].push_back("/home/merz/data/magfield/1900/41_new/2022x05x29_file_2_2725_CW.csv");
    files[20].push_back("/home/merz/data/magfield/1900/41_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=51 ====//
    files[25].push_back("/home/merz/data/magfield/1900/51_new/2022x05x29_file_2_3475_CW.csv");
    files[25].push_back("/home/merz/data/magfield/1900/51_new/2022x05x28_file_1_0_CW.csv");

    //==== Z=55 ====//
    files[27].push_back("/home/merz/data/magfield/1900/55_new/2022x05x28_file_2_3100_CW.csv");
    files[27].push_back("/home/merz/data/magfield/1900/55_new/2022x05x28_file_1_0_CW.csv");

    //==== Z=73 ====//
    files[36].push_back("/home/merz/data/magfield/1900/73/2022x05x27_file_3_3475_CW.csv");
    files[36].push_back("/home/merz/data/magfield/1900/73/2022x05x27_file_2_500_CW.csv");
    files[36].push_back("/home/merz/data/magfield/1900/73/2022x05x27_file_1_0_CW.csv");
    
    //==== Z=77 ====//
    files[38].push_back("/home/merz/data/magfield/1900/77/2022x05x27_file_2_3175_CW.csv");
    files[38].push_back("/home/merz/data/magfield/1900/77/2022x05x27_file_1_0_CW.csv");

     //==== Z=81 ====//
    files[40].push_back("/home/merz/data/magfield/1900/81/2022x05x28_file_3_5275_CW.csv");
    files[40].push_back("/home/merz/data/magfield/1900/81/2022x05x28_file_2_2225_CW.csv");
    files[40].push_back("/home/merz/data/magfield/1900/81/2022x05x28_file_1_0_CW.csv");

    //==== Z=85 ====//
    files[42].push_back("/home/merz/data/magfield/1900/85_repeat/2022x05x28_file_3_4975_CW.csv");
    files[42].push_back("/home/merz/data/magfield/1900/85_repeat/2022x05x28_file_2_2000_CW.csv");
    files[42].push_back("/home/merz/data/magfield/1900/85_repeat/2022x05x28_file_1_0_CW.csv");
    


    BmnFieldParser* parser = new BmnFieldParser();
    //vector<Int_t> zCoord = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91 ,93 };
    //vector<Int_t> zCoordOfOldSensor = { 3, 7, 11, 15, 23, 27, 35, 39, 53, 75, 79, 83 };
    vector<Int_t> zOfOldSensor = { 1, 3, 5, 7, 11, 13, 17, 19, 26, 37, 39, 41 };

    for ( Int_t k : zOfOldSensor ) {     
        Int_t i = k - 1;
        Int_t j = k + 1;
        
        map<pair<Int_t, Int_t>, BmnFieldPoint> all_field_points_i;
        for (auto file : files[i]) {
            vector<BmnFieldPoint> fieldPoints = parser->ParseData(file);
            for (auto pnt : fieldPoints) {
                all_field_points_i.insert(pair<pair<Int_t, Int_t>, BmnFieldPoint>(pair<Int_t, Int_t>(Int_t(pnt.GetX()), Int_t(pnt.GetY())), pnt));
            }
        }  
        
        map<pair<Int_t, Int_t>, BmnFieldPoint> all_field_points_j;            
        for (auto file : files[j]) {
            vector<BmnFieldPoint> fieldPoints = parser->ParseData(file);
            for (auto pnt : fieldPoints) {
                all_field_points_j.insert(pair<pair<Int_t, Int_t>, BmnFieldPoint>(pair<Int_t, Int_t>(Int_t(pnt.GetX()), Int_t(pnt.GetY())), pnt));
            }
        }  
        
                       
        ofstream outFile;
        outFile.open("/home/merz/data/magfield/1900/"+ to_string(2*k + 1) + "/file_" + to_string(2*k + 1) + "_.csv");
        outFile << "Enc position, mm" << "\t" << "X coord, mm" << "\t" << "Y coord, mm" << "\t" << "Z coord, mm" << "\t" << "Monitor" << "\t" << "Bx, mT" << "\t" << "By, mT" << "\t" << "Bz, mT" << "\n";
        for (Int_t y = 0; y <= 6000; y += 25) {
            for (Int_t x = 0; x <= 3000; x += 24) { 
                pair<int, int> p(x, y);
                BmnFieldPoint pnt0 = all_field_points_i[p];
                BmnFieldPoint pnt2 = all_field_points_j[p];
                
                Float_t bx0 = pnt0.GetBx();
                Float_t bx2 = pnt2.GetBx();
                Float_t bx_mean = ( bx0 + bx2 ) / 2;
                
                Float_t by0 = pnt0.GetBy();
                Float_t by2 = pnt2.GetBy();
                Float_t by_mean = ( by0 + by2 ) / 2;
                
                Float_t bz0 = pnt0.GetBz();
                Float_t bz2 = pnt2.GetBz();
                Float_t bz_mean = ( bz0 + bz2 ) / 2;
                
                Float_t z = 2 * k + 1;
                Float_t m = 0;
                Float_t e = 0;
                
                
                outFile << e << "\t" << x << "\t" << y << "\t" << z << "\t" << m << "\t" << bx_mean << "\t" << by_mean << "\t" << bz_mean << "\n";  
            }
        } 
        outFile.close();
    }    
}        
                









