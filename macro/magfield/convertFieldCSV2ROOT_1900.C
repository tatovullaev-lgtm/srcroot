/*
Macro takes files with magnetic field measurements (in V) and
creates magnetic field map in root-format (in kGs)
Author: S. Merts (JINR), S. Lebedeva (SPSU)
*/

R__ADD_INCLUDE_PATH($VMCWORKDIR)

typedef struct {
    Int_t N;
    Float_t min;
    Float_t max;
    Float_t step;
} Coordinate_info_t;

//convertion from voltage to kGauss for NEW and OLD hall sensors
Double_t ConvertVoltageToFeild(Double_t U);
Double_t ConvertVoltageToFeild_X(Double_t U);
Double_t ConvertVoltageToFeild_Y(Double_t U);
Double_t ConvertVoltageToFeild_Z(Double_t U);

void convertFieldCSV2ROOT_1900() {


    const Int_t nZ = 47; // vertical planes
    vector<Int_t> zCoord = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39,
                                41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77,
                                79, 81, 83, 85, 87, 89, 91 ,93 };

    vector<TString> files[nZ];

    //==== Z=1 ====//
    files[0].push_back("/home/merz/data/magfield/1900/1_new/2022x05x30_file_3_5250_CW.csv");
    files[0].push_back("/home/merz/data/magfield/1900/1_new/2022x05x30_file_2_2000_CW.csv");
    files[0].push_back("/home/merz/data/magfield/1900/1_new/2022x05x30_file_1_0_CW.csv");

    //==== Z=3 ====//
    //files[1].push_back("/home/merz/data/magfield/1900/3/2022x05x25_file_3_5775_CW.csv");
    //files[1].push_back("/home/merz/data/magfield/1900/3/2022x05x25_file_2_2450_CW.csv");
    //files[1].push_back("/home/merz/data/magfield/1900/3/2022x05x25_file_1_0_CW.csv");
    files[1].push_back("/home/merz/data/magfield/1900/3/file_3_.csv");

    //==== Z=5 ====//
    files[2].push_back("/home/merz/data/magfield/1900/5_new/2022x05x30_file_3_4500_CW.csv");
    files[2].push_back("/home/merz/data/magfield/1900/5_new/2022x05x30_file_2_3350_CW.csv");
    files[2].push_back("/home/merz/data/magfield/1900/5_new/2022x05x30_file_1_0_CW.csv");

    //==== Z=7 ====//
    //files[3].push_back("/home/merz/data/magfield/1900/7/2022x05x25_file_2_3150_CW.csv");
    //files[3].push_back("/home/merz/data/magfield/1900/7/2022x05x25_file_1_0_CW.csv");
    files[3].push_back("/home/merz/data/magfield/1900/7/file_7_.csv");

    //==== Z=9 ====//
    files[4].push_back("/home/merz/data/magfield/1900/9_new/2022x05x30_file_4_4450_CW.csv");
    files[4].push_back("/home/merz/data/magfield/1900/9_new/2022x05x29_file_3_1775_CW.csv");
    files[4].push_back("/home/merz/data/magfield/1900/9_new/2022x05x29_file_2_0_CW.csv");
    files[4].push_back("/home/merz/data/magfield/1900/9_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=11 ====//
    //files[5].push_back("/home/merz/data/magfield/1900/11/2022x05x25_file_3_4150_CW.csv");
    //files[5].push_back("/home/merz/data/magfield/1900/11/2022x05x25_file_2_3375_CW.csv");
    //files[5].push_back("/home/merz/data/magfield/1900/11/2022x05x25_file_1_0_CW.csv");
    files[5].push_back("/home/merz/data/magfield/1900/11/file_11_.csv");

    //==== Z=13 ====//
    files[6].push_back("/home/merz/data/magfield/1900/13_new/2022x05x29_file_5_5300_CW.csv");
    files[6].push_back("/home/merz/data/magfield/1900/13_new/2022x05x29_file_4_2675_CW.csv");
    files[6].push_back("/home/merz/data/magfield/1900/13_new/2022x05x29_file_3_2675_CW.csv");
    files[6].push_back("/home/merz/data/magfield/1900/13_new/2022x05x29_file_2_25_CW.csv");
    files[6].push_back("/home/merz/data/magfield/1900/13_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=15 ====//
    //files[7].push_back("/home/merz/data/magfield/1900/15/2022x05x25_file_3_4500_CW.csv");
    //files[7].push_back("/home/merz/data/magfield/1900/15/2022x05x25_file_2_3200_CW.csv");
    //files[7].push_back("/home/merz/data/magfield/1900/15/2022x05x25_file_1_0_CW.csv");
    files[7].push_back("/home/merz/data/magfield/1900/15/file_15_.csv");

    //==== Z=17 ====//
    files[8].push_back("/home/merz/data/magfield/1900/17_new/2022x05x29_file_3_3500_CW.csv");
    files[8].push_back("/home/merz/data/magfield/1900/17_new/2022x05x29_file_2_3325_CW.csv");
    files[8].push_back("/home/merz/data/magfield/1900/17_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=19 ====//
    files[9].push_back("/home/merz/data/magfield/1900/19_new/2022x05x30_file_3_3275_CW.csv");
    files[9].push_back("/home/merz/data/magfield/1900/19_new/2022x05x30_file_2_2000_CW.csv");
    files[9].push_back("/home/merz/data/magfield/1900/19_new/2022x05x30_file_1_0_CW.csv");

    //==== Z=21 ====//
    files[10].push_back("/home/merz/data/magfield/1900/21_new/2022x05x29_file_2_3275_CW.csv");
    files[10].push_back("/home/merz/data/magfield/1900/21_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=23 ====//
    //files[11].push_back("/home/merz/data/magfield/1900/23/2022x05x25_file_2_3475_CW.csv");
    //files[11].push_back("/home/merz/data/magfield/1900/23/2022x05x25_file_1_0_CW.csv");
    files[11].push_back("/home/merz/data/magfield/1900/23/file_23_.csv");

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

    //==== Z=27 ====//
    //files[13].push_back("/home/merz/data/magfield/1900/27/2022x05x25_file_2_3825_CW.csv");
    //files[13].push_back("/home/merz/data/magfield/1900/27/2022x05x25_file_1_0_CW.csv");
    files[13].push_back("/home/merz/data/magfield/1900/27/file_27_.csv");

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

    //==== Z=35 ====//
    //files[17].push_back("/home/merz/data/magfield/1900/35/2022x05x24_file_3_4950_CW.csv");
    //files[17].push_back("/home/merz/data/magfield/1900/35/2022x05x24_file_2_975_CW.csv");
    //files[17].push_back("/home/merz/data/magfield/1900/35/2022x05x24_file_1_0_CW.csv");
    files[17].push_back("/home/merz/data/magfield/1900/35/file_35_.csv");

    //==== Z=37 ====//
    files[18].push_back("/home/merz/data/magfield/1900/37_new/2022x05x29_file_3_5175_CW.csv");
    files[18].push_back("/home/merz/data/magfield/1900/37_new/2022x05x29_file_2_2150_CW.csv");
    files[18].push_back("/home/merz/data/magfield/1900/37_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=39 ====//
    //files[19].push_back("/home/merz/data/magfield/1900/39_best/2022x05x24_file_3_3350_CW.csv");
    //files[19].push_back("/home/merz/data/magfield/1900/39_best/2022x05x24_file_1_875_CW.csv");
    //files[19].push_back("/home/merz/data/magfield/1900/39_best/2022x05x24_file_1_0_CW.csv");
    files[19].push_back("/home/merz/data/magfield/1900/39/file_39_.csv");

    //==== Z=41 ====//
    files[20].push_back("/home/merz/data/magfield/1900/41_new/2022x05x29_file_3_5650_CW.csv");
    files[20].push_back("/home/merz/data/magfield/1900/41_new/2022x05x29_file_2_2725_CW.csv");
    files[20].push_back("/home/merz/data/magfield/1900/41_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=43 ====//
    files[21].push_back("/home/merz/data/magfield/1900/43_new_updown/2022x05x29_file_5_5975_CW.csv");
    files[21].push_back("/home/merz/data/magfield/1900/43_new_updown/2022x05x29_file_4_4350_CW.csv");
    files[21].push_back("/home/merz/data/magfield/1900/43_new_updown/2022x05x29_file_3_3000_CW.csv");
    files[21].push_back("/home/merz/data/magfield/1900/43_new_updown/2022x05x29_file_2_1450_CW.csv");
    files[21].push_back("/home/merz/data/magfield/1900/43_new_updown/2022x05x29_file_1_0_CW.csv");

    //==== Z=45 ====//
    files[22].push_back("/home/merz/data/magfield/1900/45/2022x05x27_file_2_3825_CW.csv");
    files[22].push_back("/home/merz/data/magfield/1900/45/2022x05x27_file_1_0_CW.csv");

    //==== Z=47 ====//
    files[23].push_back("/home/merz/data/magfield/1900/47_new/2022x05x29_file_2_3075_CW.csv");
    files[23].push_back("/home/merz/data/magfield/1900/47_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=49 ====//
    files[24].push_back("/home/merz/data/magfield/1900/49/2022x05x27_file_2_4200_CW.csv");
    files[24].push_back("/home/merz/data/magfield/1900/49/2022x05x27_file_1_0_CW.csv");

    //==== Z=51 ====//
    files[25].push_back("/home/merz/data/magfield/1900/51_new/2022x05x29_file_2_3475_CW.csv");
    files[25].push_back("/home/merz/data/magfield/1900/51_new/2022x05x28_file_1_0_CW.csv");

    //==== Z=53 ====//
    //files[26].push_back("/home/merz/data/magfield/1900/53/2022x05x27_file_3_5200_CW.csv");
    //files[26].push_back("/home/merz/data/magfield/1900/53/2022x05x27_file_2_2600_CW.csv");
    //files[26].push_back("/home/merz/data/magfield/1900/53/2022x05x27_file_1_0_CW.csv");
    //files[26].push_back("/home/merz/data/magfield/1900/53/2022x05x27_file_1_1_CW.csv");
    //files[26].push_back("/home/merz/data/magfield/1900/53/2022x05x27_file_1_2_CW.csv");
    files[26].push_back("/home/merz/data/magfield/1900/53/file_53_.csv");
    
    //==== Z=55 ====//
    files[27].push_back("/home/merz/data/magfield/1900/55_new/2022x05x28_file_2_3100_CW.csv");
    files[27].push_back("/home/merz/data/magfield/1900/55_new/2022x05x28_file_1_0_CW.csv");

    //==== Z=57 ====//
    files[28].push_back("/home/merz/data/magfield/1900/57/2022x05x27_file_3_4475_CW.csv");
    files[28].push_back("/home/merz/data/magfield/1900/57/2022x05x27_file_2_3200_CW.csv");
    files[28].push_back("/home/merz/data/magfield/1900/57/2022x05x27_file_1_0_CW.csv");

    //==== Z=59 ====//
    files[29].push_back("/home/merz/data/magfield/1900/59_new/2022x05x28_file_2_3275_CW.csv");
    files[29].push_back("/home/merz/data/magfield/1900/59_new/2022x05x28_file_1_0_CW.csv");

    //==== Z=61 ====//
    files[30].push_back("/home/merz/data/magfield/1900/61/2022x05x27_file_2_4225_CW.csv");
    files[30].push_back("/home/merz/data/magfield/1900/61/2022x05x27_file_1_0_CW.csv");

    //==== Z=63 ====//
    files[31].push_back("/home/merz/data/magfield/1900/63_new/2022x05x28_file_2_3325_CW.csv");
    files[31].push_back("/home/merz/data/magfield/1900/63_new/2022x05x28_file_1_0_CW.csv");

    //==== Z=65 ====//
    files[32].push_back("/home/merz/data/magfield/1900/65/2022x05x27_file_2_3950_CW.csv");
    files[32].push_back("/home/merz/data/magfield/1900/65/2022x05x27_file_1_0_CW.csv");

    //==== Z=67 ====//
    files[33].push_back("/home/merz/data/magfield/1900/67_new/2022x05x28_file_3_4825_CW.csv");
    files[33].push_back("/home/merz/data/magfield/1900/67_new/2022x05x28_file_2_1825_CW.csv");
    files[33].push_back("/home/merz/data/magfield/1900/67_new/2022x05x28_file_1_0_CW.csv");

    //==== Z=69 ====//
    files[34].push_back("/home/merz/data/magfield/1900/69/2022x05x27_file_2_3725_CW.csv");
    files[34].push_back("/home/merz/data/magfield/1900/69/2022x05x27_file_1_0_CW.csv");

    //==== Z=71 ====//
    files[35].push_back("/home/merz/data/magfield/1900/71_new/2022x05x28_file_3_5150_CW.csv");
    files[35].push_back("/home/merz/data/magfield/1900/71_new/2022x05x28_file_2_2850_CW.csv");
    files[35].push_back("/home/merz/data/magfield/1900/71_new/2022x05x28_file_1_0_CW.csv");

    //==== Z=73 ====//
    files[36].push_back("/home/merz/data/magfield/1900/73/2022x05x27_file_3_3475_CW.csv");
    files[36].push_back("/home/merz/data/magfield/1900/73/2022x05x27_file_2_500_CW.csv");
    files[36].push_back("/home/merz/data/magfield/1900/73/2022x05x27_file_1_0_CW.csv");

    //==== Z=75 ====//
    //files[37].push_back("/home/merz/data/magfield/1900/75/2022x05x22_file_2_5075_CW.csv");
    //files[37].push_back("/home/merz/data/magfield/1900/75/2022x05x22_file_1_0_CW.csv");
    //files[37].push_back("/home/merz/data/magfield/1900/75/2022x05x22_1900A_hall_75_CW.csv");
    //files[37].push_back("/home/merz/data/magfield/1900/75/2022x05x22_1900A_hall_75_part2_CW.csv");
    files[37].push_back("/home/merz/data/magfield/1900/75/file_75_.csv");


    //==== Z=77 ====//
    files[38].push_back("/home/merz/data/magfield/1900/77/2022x05x27_file_2_3175_CW.csv");
    files[38].push_back("/home/merz/data/magfield/1900/77/2022x05x27_file_1_0_CW.csv");

    //==== Z=79 ====//
    //files[39].push_back("/home/merz/data/magfield/1900/79/2022x05x22_file_2_4625_CW.csv");
    //files[39].push_back("/home/merz/data/magfield/1900/79/2022x05x22_file_1_0_CW.csv");
    //files[39].push_back("/home/merz/data/magfield/1900/79/2022x05x22_1900A_hall_79_CW.csv");
    //files[39].push_back("/home/merz/data/magfield/1900/79/2022x05x22_1900A_hall_79_part2_CW.csv");
    files[39].push_back("/home/merz/data/magfield/1900/79/file_79_.csv");


    //==== Z=81 ====//
    files[40].push_back("/home/merz/data/magfield/1900/81/2022x05x28_file_3_5275_CW.csv");
    files[40].push_back("/home/merz/data/magfield/1900/81/2022x05x28_file_2_2225_CW.csv");
    files[40].push_back("/home/merz/data/magfield/1900/81/2022x05x28_file_1_0_CW.csv");

    //==== Z=83 ====//
    //files[41].push_back("/home/merz/data/magfield/1900/83/2022x05x22_file_4_6200_CW.csv");
    //files[41].push_back("/home/merz/data/magfield/1900/83/2022x05x22_file_3_2650_CW.csv");
    //files[41].push_back("/home/merz/data/magfield/1900/83/2022x05x22_file_2_175_CW.csv");
    //files[41].push_back("/home/merz/data/magfield/1900/83/2022x05x22_file_1_0_CW.csv");
    //files[41].push_back("/home/merz/data/magfield/1900/83/2022x05x22_1900A_hall_83_CW.csv");
    //files[41].push_back("/home/merz/data/magfield/1900/83/2022x05x22_1900A_hall_83_1_CW.csv");
    //files[41].push_back("/home/merz/data/magfield/1900/83/2022x05x22_1900A_hall_83_2_CW.csv");
    //files[41].push_back("/home/merz/data/magfield/1900/83/2022x05x22_1900A_hall_83_3_CW.csv");
    files[41].push_back("/home/merz/data/magfield/1900/83/file_83_.csv");

    //==== Z=85 ====//
    files[42].push_back("/home/merz/data/magfield/1900/85_repeat/2022x05x28_file_3_4975_CW.csv");
    files[42].push_back("/home/merz/data/magfield/1900/85_repeat/2022x05x28_file_2_2000_CW.csv");
    files[42].push_back("/home/merz/data/magfield/1900/85_repeat/2022x05x28_file_1_0_CW.csv");

    //==== Z=87 ====//
    files[43].push_back("/home/merz/data/magfield/1900/87_new/2022x05x29_file_2_3325_CW.csv");
    files[43].push_back("/home/merz/data/magfield/1900/87_new/2022x05x29_file_1_0_CW.csv");

    //==== Z=89 ====//
    files[44].push_back("/home/merz/data/magfield/1900/89_new/2022x05x28_file_6_5175_CW.csv");
    files[44].push_back("/home/merz/data/magfield/1900/89_new/2022x05x28_file_5_4975_CW.csv");
    files[44].push_back("/home/merz/data/magfield/1900/89_new/2022x05x28_file_4_4775_CW.csv");
    files[44].push_back("/home/merz/data/magfield/1900/89_new/2022x05x28_file_3_4575_CW.csv");
    files[44].push_back("/home/merz/data/magfield/1900/89_new/2022x05x28_file_2_3175_CW.csv");
    files[44].push_back("/home/merz/data/magfield/1900/89_new/2022x05x28_file_1_0_CW.csv");

    //==== Z=91 ====//
    files[45].push_back("/home/merz/data/magfield/1900/91_new/2022x05x28_file_3_4400_CW.csv");
    files[45].push_back("/home/merz/data/magfield/1900/91_new/2022x05x28_file_2_3525_CW.csv");
    files[45].push_back("/home/merz/data/magfield/1900/91_new/2022x05x28_file_1_0_CW.csv");

    //==== Z=93 ====//
    files[46].push_back("/home/merz/data/magfield/1900/93_new/2022x05x28_file_3_5525_CW.csv");
    files[46].push_back("/home/merz/data/magfield/1900/93_new/2022x05x28_file_2_2700_CW.csv");
    files[46].push_back("/home/merz/data/magfield/1900/93_new/2022x05x28_file_1_0_CW.csv");

    BmnFieldParser* parser = new BmnFieldParser();

    vector<Int_t> zOfOldSensor = { 3, 7, 11, 15, 23, 27, 35, 39, 53, 75, 77, 79, 83 };

    TString out = "FieldMap_1900.root";
    TFile* f = new TFile(out.Data(), "RECREATE");
    TTree* t = new TTree("Field_map", "data from ASCII");

    BmnFieldPoint pointToWrite;
    t->Branch("field_points", &pointToWrite, "X/D:Y/D:Z/D:Bx/D:By/D:Bz/D");

    for (Int_t i = 0; i < nZ; ++i) {
        // TString out = Form("field_1900_%d.root", zCoord[i]);
        // TFile* f = new TFile(out.Data(), "RECREATE");
        // TTree* t = new TTree("Field_map", "data from ASCII");

        // BmnFieldPoint pointToWrite;
        // t->Branch("field_points", &pointToWrite, "X/D:Y/D:Z/D:Bx/D:By/D:Bz/D");
        map<pair<Int_t, Int_t>, BmnFieldPoint> all_field_points;
        //Bool_t isNewSensor = kFALSE;
        for (auto file : files[i]) {
            //if (find(zOfOldSensor.begin(), zOfOldSensor.end(), zCoord[i]) == zOfOldSensor.end()) isNewSensor = kTRUE;
            vector<BmnFieldPoint> fieldPoints = parser->ParseData(file);
            for (auto pnt : fieldPoints) {
                all_field_points.insert(pair<pair<Int_t, Int_t>, BmnFieldPoint>(pair<Int_t, Int_t>(Int_t(pnt.GetX()), Int_t(pnt.GetY())), pnt));
            }
        }

        const Double_t By_ped = -0.158; // pedestal for By

        Int_t nPoints = 0;
        for (auto pnt : all_field_points) {

            Double_t xFrame = pnt.second.GetX();
            Double_t yFrame = pnt.second.GetY();
            Double_t zFrame = pnt.second.GetZ();

            if (xFrame < 0 || xFrame > 3000 || yFrame < 0 || yFrame > 6000) {
                continue;
            }

            Double_t BxFrame = pnt.second.GetBx();
            Double_t ByFrame = pnt.second.GetBy();
            Double_t BzFrame = pnt.second.GetBz();

            Double_t xBmn = (1.002 * (-xFrame) + 1448.218) * 0.1;
            Double_t yBmn = (zFrame * 10 - 386.73) * 0.1;
            Double_t zBmn = (1.002 * yFrame - 1621.615) * 0.1;

            //Convert voltage from channels of measurement frame coordinates to magnetic field components of hall sensor coordinates
            Double_t ByHall = ConvertVoltageToFeild_X(+BxFrame);
            Double_t BxHall = ConvertVoltageToFeild_Y(+ByFrame);
            Double_t BzHall = ConvertVoltageToFeild_Z(+BzFrame);

            //Convert field components of hall sensor coordinates to field components of BM@N coordinates
            Double_t BxBmn = -BxHall;
            Double_t ByBmn = -BzHall;
            Double_t BzBmn = -ByHall;

            BmnFieldPoint tmpPnt(xBmn, yBmn, zBmn, BxBmn, ByBmn - By_ped, BzBmn);
            pointToWrite = tmpPnt;
            nPoints++;
            t->Fill();
        }

        // f->Write();
    }
    f->Write();

}

Double_t ConvertVoltageToFeild(Double_t U) {
    //U - voltage in V
    //return field in kGs
    Double_t k = 136.5;
    Double_t b = 23.16;
    Double_t scale = 1000;
    
    return (k * U * scale + b) / 1000;
}

Double_t ConvertVoltageToFeild_X(Double_t U) {
    //U - voltage in V
    //return field in kGs
    Double_t k = 136.652;
    Double_t b = -11.782;
    Double_t scale = 1000;
    return (k * U * scale + b) / 1000;
}

Double_t ConvertVoltageToFeild_Y(Double_t U) {
    //U - voltage in V
    //return field in kGs
    Double_t k = 139.666;
    Double_t b = -2.07;
    Double_t scale = 1000;
    return (k * U * scale + b) / 1000;
}

Double_t ConvertVoltageToFeild_Z(Double_t U) {
    //U - voltage in V
    //return field in kGs
    Double_t k = 144.824;
    Double_t b = 152.728;//50.0; //изначальное значеие из калибровок 152.728, но я изменил его для лучшей совместимости со старой картой
    Double_t scale = 1000;
    return (k * U * scale + b) / 1000;
}