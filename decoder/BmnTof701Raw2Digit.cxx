#include "BmnTof701Raw2Digit.h"

//	Written by JINR summer programme 2016 student Kurganov Alexander

//	Supervisors:
//	Vadim Babkin
//	Mikhail Rumyantsev
//	Vyatcheslav Golovatjuk

typedef std::map<std::pair<UInt_t, UChar_t>, UInt_t>::iterator Tof701PlMapIter;
typedef std::map<UInt_t, BmnTof701TDCParameters>::iterator Tof701TDCMapIter;
typedef std::pair<UInt_t, UChar_t> Tof701PlMapKey;
typedef std::pair<UInt_t, BmnTof701TDCParameters> Tof701TDCMapElem;

using namespace std;

int Tof701GlobalNameCounter = 0;

// Simple constructors for the BmnTof701Map2 element class

BmnTof701Map2::BmnTof701Map2(Short_t p, Short_t s, Bool_t sd)
{
    plane = p;
    strip = s;
    side = sd;
}

BmnTof701Map2::BmnTof701Map2()
{
    plane = 0;
    strip = 0;
    side = false;
}

// Main constructor for the BmnTof701TDCParameters()

BmnTof701TDCParameters::BmnTof701TDCParameters()
{
    for (int i = 0; i < TOF701_CHANNEL_NUMBER; i++) {
        for (int j = 0; j < TOF701_BIN_NUMBER; j++) {
            INL[i][j] = 0;
        }
        ChannelMap[i] = BmnTof701Map2(0, 0, false);
        t[i] = -1;
    }
}

// A small functional class, which is used to compare two BmnTDCDigits
// It is used to sort by time the TDCDigits in the BmnTof701Raw2Digit::FillEvent()

bool _Tof701TDCCompare::operator()(const BmnTDCDigit& a, const BmnTDCDigit& b) const
{
    return a.GetValue() < b.GetValue();
}

// A small function, which inserts in the placement map a key-value pair
// Just to make everything more clear

void BmnTof701Raw2Digit::plmap_insert(UInt_t Serial, UChar_t Slot, UInt_t TDCSerial)
{
    PlacementMap.insert(std::pair<Tof701PlMapKey, UInt_t>(Tof701PlMapKey(Serial, Slot), TDCSerial));
}

// BmnTof701Raw2Digit main constructor

BmnTof701Raw2Digit::BmnTof701Raw2Digit()
{
    init();
}

// BmnTof701Raw2Digit constructor, which also loads mapping from the DB

BmnTof701Raw2Digit::BmnTof701Raw2Digit(Int_t nPeriod, Int_t nRun, Int_t verbose = 0)
{
    fVerbose = verbose;
    init();
    setRun(nPeriod, nRun);
}

// BmnTof701Raw2Digit destructor

BmnTof701Raw2Digit::~BmnTof701Raw2Digit() {}

// BmnTof701Raw2Digit init function (called in BmnTof701Raw2Digit constructors)

void BmnTof701Raw2Digit::init()
{
    // Nothing here
}

// A simple function to convert Hptdcid (0 -- 9) and Hptdc's channel (0 -- 8) to "global" tdc channel (0 -- 72)

UShort_t BmnTof701Raw2Digit::ToGlobalChannel(UChar_t HptdcId, UChar_t channel)
{
    return HptdcId * 8 + channel;
}

// Loads mapping from the DB

Bool_t BmnTof701Raw2Digit::setRun(Int_t nPeriod, Int_t nRun)
{

    Bool_t flag_temp = kTRUE;
    if (fVerbose >= 1)
        printf(ANSI_COLOR_BLUE "Loading the TOF701 Data from DB: Period %d, Run %d  ...\n" ANSI_COLOR_RESET, nPeriod,
               nRun);
    PeriodIndex = nPeriod;
    RunIndex = nRun;
    // return kFALSE;
    //--------------------------------------------------------------------------
    // Load the TDC's map (placement map) from the DB
    if (fVerbose >= 1)
        cout << "\tLoading the placement map..." << endl;
    UniDetectorParameter* pLoadedPlacement =
        UniDetectorParameter::GetDetectorParameter("TOF2", "TOF2_TdcMap", nPeriod, nRun);
    if (pLoadedPlacement == 0) {
        printf(ANSI_COLOR_BLUE "\t\tTOF701 Crucial error: failed to load the placement map from DB\n" ANSI_COLOR_RESET);
        // printf(ANSI_COLOR_RED "\tTDCMap are not loaded\n\tThere will be no TOF701 digits in the output file\n"
        // ANSI_COLOR_RESET);
        printf(ANSI_COLOR_BLUE "\t\tWill load from FILE\n" ANSI_COLOR_RESET);
        flag_temp = kFALSE;
    } else {
        // Get the placement map
        vector<UniValue*> tdcmap_array;
        pLoadedPlacement->GetValue(tdcmap_array);
        MapIntValue* temptdcMap = nullptr;
        // loop over TDCs stored in the DB
        for (size_t i = 0; i < tdcmap_array.size(); i++) {
            temptdcMap = (MapIntValue*)tdcmap_array.at(i);
            plmap_insert(temptdcMap->serial, temptdcMap->channel, temptdcMap->value);
            // create TDC in the map
            TDCMap.insert(Tof701TDCMapElem(temptdcMap->value, BmnTof701TDCParameters()));
        }

        // Print the result
        if (fVerbose >= 1)
            printf("\t\tLoaded %d TDCs\n", (int)tdcmap_array.size());
        // Free the memory
        if (!tdcmap_array.empty())
            for (size_t i = 0; i < tdcmap_array.size(); i++)
                delete tdcmap_array[i];
        delete pLoadedPlacement;
        //      delete temptdcMap; //???
    }

    //    Tof701PlMapIter parPair = PlacementMap.begin();
    //    while (parPair != PlacementMap.end()){
    //        printf ("\t%08X \t %08X \t %08X \n", parPair->first.first, parPair->first.second, parPair->second);
    //        parPair++;
    //    }
    //    getchar();

    //--------------------------------------------------------------------------
    // Load the strip mapping from the DB (TDCs channel -> plane, strip, side).
    if (fVerbose >= 1)
        cout << "\tLoading strip map..." << endl;
    UniDetectorParameter* pLoadedStr =
        UniDetectorParameter::GetDetectorParameter("TOF2", "TOF2_StripMap", nPeriod, nRun);
    if (pLoadedStr == 0) {
        printf(ANSI_COLOR_RED "\t\tTOF701 Crucial error: failed to load the strip map from DB\n" ANSI_COLOR_RESET);
        // printf(ANSI_COLOR_RED "\tStripMap are not loaded\n\tThere will be no TOF701 digits in the output file\n"
        // ANSI_COLOR_RESET);
        printf(ANSI_COLOR_BLUE "\t\tWill load from FILE\n" ANSI_COLOR_RESET);
        flag_temp = kFALSE;
    } else {
        // Get the str map
        vector<UniValue*> stripmap_array;
        pLoadedStr->GetValue(stripmap_array);
        MapDVectorValue* tempstrMap = nullptr;
        for (size_t i = 0; i < stripmap_array.size(); i++) {
            tempstrMap = (MapDVectorValue*)stripmap_array.at(i);
            // Looking for TDC in the map
            Tof701TDCMapIter TDCPair = TDCMap.find(tempstrMap->serial);
            if (TDCPair == TDCMap.end()) {
                printf(ANSI_COLOR_RED "\t\tError: TDC # 0x%08x are not found in the strip map\n" ANSI_COLOR_RESET,
                       tempstrMap->serial);
                // fill map for this TDC by under range value
                BmnTof701Map2* elem = &((TDCPair->second).ChannelMap[tempstrMap->channel]);
                elem->plane = -1;
                elem->strip = -1;
                elem->side = TOF701_LEFT;
            }
            // get the channel map from
            BmnTof701Map2* elem = &((TDCPair->second).ChannelMap[tempstrMap->channel]);
            elem->plane = tempstrMap->value.at(0);
            elem->strip = tempstrMap->value.at(1);
            if (tempstrMap->value.at(2) == 0)
                elem->side = TOF701_LEFT;
            else if (tempstrMap->value.at(2) == 1)
                elem->side = TOF701_RIGHT;
            //        cout << std::hex << tempstrMap->serial << std::dec << " ch " << tempstrMap->channel << " -> " <<
            //        tempstrMap->value.at(0) << "  " << tempstrMap->value.at(1) << "  " << tempstrMap->value.at(2) <<
            //        endl;
        }

        // Print the result
        if (fVerbose >= 1)
            printf("\t\tLoaded %d strips\n", (int)stripmap_array.size());
        // Free the memory
        if (!stripmap_array.empty())
            for (size_t i = 0; i < stripmap_array.size(); i++)
                delete stripmap_array[i];
        delete pLoadedStr;
        //        delete tempstrMap; // ???
    }

    // printf("Check map of TDCs\n");
    // Tof701TDCMapIter TDCPair = TDCMap.begin();
    // while (TDCPair != TDCMap.end()) {
    //     printf("TDC %08X;   Det = %d;\n", TDCPair->first, TDCPair->second.ChannelMap[0].plane);
    //     ++TDCPair;
    // }

    //--------------------------------------------------------------------------
    // Load Inl from DB
    if (fVerbose >= 1)
        cout << "\tLoading the INL correction from DB... " << endl;
    UniDetectorParameter* pLoadedInl = UniDetectorParameter::GetDetectorParameter(
        "TOF2", "TOF2_inl", nPeriod, nRun);   //(detector_name, parameter_name, period_number, run_number)
    if (pLoadedInl == NULL) {
        printf(ANSI_COLOR_RED "\tINL are not exist in DB\n\t!! Time information will be wrong !!\n" ANSI_COLOR_RESET);
        flag_temp = kFALSE;   //
    } else {
        // get INL from DB to vector
        vector<UniValue*> inl_array;
        pLoadedInl->GetValue(inl_array);
        TdcInlValue* tempInl = nullptr;
        UInt_t serialtemp;
        Bool_t flagtemp = false;
        Tof701TDCMapIter tdcs = TDCMap.begin();
        // loop over TDCs in the map
        while (tdcs != TDCMap.end()) {
            serialtemp = tdcs->first;
            flagtemp = false;

            // looking for INL for current TDC in the INL_vector from DB
            for (size_t iv = 0; iv < inl_array.size(); iv++) {
                tempInl = (TdcInlValue*)inl_array.at(iv);
                if (tempInl->serial == serialtemp) {
                    if (fVerbose >= 2)
                        cout << "\t\tINL for " << std::hex << serialtemp << std::dec << " are found" << endl;
                    flagtemp = true;
                    for (Int_t ich = 0; ich < TOF701_CHANNEL_NUMBER; ich++)
                        for (Int_t ibin = 0; ibin < TOF701_BIN_NUMBER; ibin++) {
                            (tdcs->second).INL[ich][ibin] = tempInl->inl[ich][ibin];
                            //                            (tdcs->second).INL[ich][ibin] = 0;
                        }
                }
            }
            if (flagtemp == false) {
                printf(ANSI_COLOR_RED "\t\tError: INL for TDC # 0x%08x are not stored in DB\n" ANSI_COLOR_RESET,
                       serialtemp);
                // fill INL for this TDC by zero
                for (Int_t ich = 0; ich < TOF701_CHANNEL_NUMBER; ich++)
                    for (Int_t ibin = 0; ibin < TOF701_BIN_NUMBER; ibin++)
                        (tdcs->second).INL[ich][ibin] = 0.;
            }
            // increase iterator
            tdcs++;
        }
        // Free the memory
        if (!inl_array.empty())
            for (size_t i = 0; i < inl_array.size(); i++)
                delete inl_array[i];
        delete pLoadedInl;
        if (fVerbose >= 1)
            cout << "\t\tLoading INL for TOF701 complete." << endl;
    }

    //--------------------------------------------------------------------------
    return flag_temp;
}

// Load mapping from the file

Bool_t BmnTof701Raw2Digit::setMapFromFile(TString placementMapFile, TString mapFile)
{
    // printf("\tLoad TOF701 mapping from files... \n");
    UInt_t fcrate, fserial, fslot, fchan, fplane, fstrip;
    char side_c;
    Bool_t flag_temp = kTRUE;

    // 1. Placement map
    fstream ff;
    ff.open(placementMapFile.Data(), std::fstream::in);
    if (ff.fail()) {
        printf(ANSI_COLOR_RED "\t\tError: Cannot open the file %s \n" ANSI_COLOR_RESET, placementMapFile.Data());
        printf(
            ANSI_COLOR_RED
            "\tMap for TOF701 is not loaded\n\tThere will be no TOF701 digits in the output file\n" ANSI_COLOR_RESET);
        flag_temp = kFALSE;
    } else {
        if (fVerbose >= 1)
            printf("\tReading TOF701 PlacementMap file %s\n", placementMapFile.Data());
        while (!ff.eof()) {
            ff >> std::hex >> fcrate >> std::dec >> fslot >> std::hex >> fserial >> std::dec;
            if (ff.eof()) {
                break;
            }
            plmap_insert(fcrate, fslot, fserial);
            TDCMap.insert(Tof701TDCMapElem(fserial, BmnTof701TDCParameters()));
            // printf("Crate %08x; Slot %d; SerialTDC %08x", fcrate, fslot, fserial);
        }
        if (fVerbose >= 1)
            printf("\t\tLoaded %d TDCs\n", (int)TDCMap.size());
    }
    ff.close();
    // TDCMap.erase(0x0AA1186F);
    //  printf("DEBUG: Check map of TDCs\n");
    //  Tof701PlMapIter plit = PlacementMap.begin();
    //  while (plit != PlacementMap.end()) {
    //      printf("TTVXS %08X;  Slot %d;  TDC %08X\n", plit->first.first, plit->first.second, plit->second);

    //     ++plit;
    // }

    // 2. Main map
    Int_t nLines = 0;
    ff.open(mapFile.Data(), std::fstream::in);
    if (ff.fail()) {
        printf(ANSI_COLOR_RED "\t\tError: Cannot open the file %s \n" ANSI_COLOR_RESET, mapFile.Data());
        printf(
            ANSI_COLOR_RED
            "\tMap for TOF701 is not loaded\n\tThere will be no TOF701 digits in the output file\n" ANSI_COLOR_RESET);
        flag_temp = kFALSE;
    } else {
        if (fVerbose >= 1)
            printf("\tReading TOF701 StripMap file %s\n", mapFile.Data());
        while (!ff.eof()) {
            ff >> std::hex >> fserial >> std::dec >> fchan >> fplane >> fstrip >> side_c;
            nLines++;
            if (ff.eof()) {
                break;
            }
            Tof701TDCMapIter TDCPair = TDCMap.find(fserial);
            if (TDCPair == TDCMap.end()) {
                printf(ANSI_COLOR_RED "\t\tError:  TDC # 0x%08x  not found\n" ANSI_COLOR_RESET, fserial);
            }
            BmnTof701Map2* elem = &((TDCPair->second).ChannelMap[fchan]);
            elem->plane = fplane;
            elem->strip = fstrip;
            if (side_c == 'L') {
                elem->side = TOF701_LEFT;
            } else if (side_c == 'R') {
                elem->side = TOF701_RIGHT;
            } else {
                cerr << "ToF701 Error: side is not L or R" << endl;
                continue;
            }
            // printf("TDC %08x; Channel %d; Plane %d; Strip %d; Side %d", fserial, fchan, fplane, fstrip, side_c);
        }
    }
    ff.close();

    // printf("DEBUG: Check map of STRIPs\n");
    // Tof701TDCMapIter TDCPair = TDCMap.begin();
    // while (TDCPair != TDCMap.end()) {
    //     for (UInt_t i = 0; i < TOF701_CHANNEL_NUMBER; i++) {
    //         printf("TDC %08X;  Channel %d; Plane %d; Strip %d; Side %d\n", TDCPair->first, i,
    //         TDCPair->second.ChannelMap[i].plane, TDCPair->second.ChannelMap[i].strip,
    //         TDCPair->second.ChannelMap[i].side);
    //     }
    //     ++TDCPair;
    // }
    if (fVerbose >= 1)
        printf("\t\tLoaded %d strips\n", nLines);
    return flag_temp;
}

// Save mapping to file

void BmnTof701Raw2Digit::saveMapToFile(string placementMapFile, string mapFile)
{
    fstream ff;

    // 1.Placement map
    ff.open(placementMapFile.c_str(), std::fstream::out);
    if (ff.fail()) {
        cerr << "Cannot open the file " << placementMapFile << endl;
        return;
    }

    Tof701PlMapIter plit = PlacementMap.begin();
    while (plit != PlacementMap.end()) {
        ff << std::hex << std::setfill('0') << std::setw(8) << plit->first.first << '\t';
        ff << std::dec << int(plit->first.second) << '\t';
        ff << std::hex << std::setfill('0') << std::setw(8) << plit->second << endl;
        ++plit;
    }
    ff.close();

    // 2. Main map
    ff.open(mapFile.c_str(), std::fstream::out);
    if (ff.fail()) {
        cerr << "Cannot open the file " << mapFile << endl;
        return;
    }

    Tof701TDCMapIter mapit = TDCMap.begin();
    while (mapit != TDCMap.end()) {
        for (int i = 0; i < TOF701_CHANNEL_NUMBER; i++) {
            ff << std::hex << std::setfill('0') << std::setw(8) << mapit->first << std::setfill(' ') << '\t';
            ff << std::dec << i << '\t';
            ff << mapit->second.ChannelMap[i].plane << '\t';
            ff << mapit->second.ChannelMap[i].strip << '\t';
            bool side = mapit->second.ChannelMap[i].side;
            if (side == TOF701_LEFT) {
                ff << 'L';
            } else if (side == TOF701_RIGHT) {
                ff << 'R';
            } else {
                cerr << "Side is not bool." << endl;
            }   // I don't think this can even occur
            ff << endl;
        }
        ++mapit;
    }
}

// Save INL to file

void BmnTof701Raw2Digit::setINLFromFile(string INLFile)
{
    fstream fList(INLFile.c_str(), std::fstream::in);
    if (fList.fail()) {
        cerr << "Failed to open " << INLFile << endl;
        return;
    }
    TString FileINL[100];
    Int_t TDCSerial;
    Int_t counter = 0;
    TString dir = Form("%s%s", getenv("VMCWORKDIR"), "/input/");
    while (!fList.eof()) {
        if (counter == 51)
            break;

        fList >> FileINL[counter];
        fstream ff(dir + FileINL[counter], std::fstream::in);

        // The format of the header seems to be [TDC-THESERIAL-inl_corr]
        ff.ignore(10, '-');
        ff >> std::hex >> TDCSerial >> std::dec;
        ff.ignore(100, '\n');

        // Find the TDC
        Tof701TDCMapIter TDCPair = TDCMap.find(TDCSerial);
        if (TDCPair == TDCMap.end()) {
            cerr << "Tof701: TDC " << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::setfill(' ')
                 << std::dec;
            cerr << " isn't in the placement map." << endl;
            ff.close();
            return;
        }

        unsigned int chan_id = 0;
        unsigned int lines_num = 0;

        while (!ff.eof()) {
            string line;
            char dummy;

            std::getline(ff, line, '\n');
            if (ff.eof()) {
                break;
            }
            if (line == "") {
                continue;
            }
            istringstream ss(line);

            ss >> chan_id >> dummy;
            if (dummy != '=') {
                cerr << "Tof701: Wrong INL file format." << endl;
                ff.close();
                return;
            }
            if (chan_id > TOF701_CHANNEL_NUMBER) {
                cerr << "Tof701: Wrong channel in in the INL file" << endl;
                ff.close();
                return;
            }

            double* INLelem = (TDCPair->second).INL[chan_id];
            unsigned int i_bin = 0;
            while (ss.tellg() != -1) {
                if (i_bin > 1023) {
                    break;
                }

                if (i_bin >= TOF701_BIN_NUMBER) {
                    cout << "Tof701: INL File contains too many bins in channel."
                         << "\tchan_id = " << chan_id << "\t TDC = " << std::hex << TDCSerial << endl;
                    ff.close();
                    return;
                }
                if (ss.peek() == ',') {
                    ss.ignore();
                }
                ss >> INLelem[i_bin];
                if (i_bin == 0 || i_bin == 1023)
                    if (i_bin >= TOF701_BIN_NUMBER)
                        cout << "WARNING: i_bin = " << i_bin << "; INElem = " << INLelem[i_bin] << endl;
                i_bin++;
            }
            lines_num++;
        }
        ff.close();
        counter++;
    }
    fList.close();
}

// Load INL from file

void BmnTof701Raw2Digit::saveINLToFile(string INLFile, unsigned int TDCSerial)
{
    // Find the TDC
    Tof701TDCMapIter TDCPair = TDCMap.find(TDCSerial);
    if (TDCPair == TDCMap.end()) {
        cerr << "Tof701: TDC " << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::setfill(' ')
             << std::dec;
        cerr << " isn't in the placement map." << endl;
        return;
    }

    fstream ff(INLFile.c_str(), std::fstream::out);
    ff << "[TDC-" << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::dec << std::setfill(' ')
       << "-inl_corr]" << endl;

    for (int chan = 0; chan < TOF701_CHANNEL_NUMBER; chan++) {
        ff << chan << "=";
        double* INLelem = (TDCPair->second).INL[chan];
        for (int bin = 0; bin < TOF701_BIN_NUMBER; bin++) {
            ff << INLelem[bin];
            if (bin != TOF701_BIN_NUMBER - 1) {
                ff << ", ";
            }
        }
        if (chan != TOF701_CHANNEL_NUMBER - 1) {
            ff << endl;
        }
    }

    ff.close();
}

// Main function. "Converts" the TObjArray *data of BmnTDCDigit to the TObjArray *output of BmnTof701Digit

void BmnTof701Raw2Digit::FillEvent(TClonesArray* data, unordered_map<UInt_t, Long64_t>* mapTS, TClonesArray* output)
{
    // 0. Initialize: clear all the tempory times in the BmnTof701TDCParameters
    Tof701TDCMapIter tdci = TDCMap.begin();
    while (tdci != TDCMap.end()) {
        for (int i = 0; i < TOF701_CHANNEL_NUMBER; i++) {
            (tdci->second).t[i] = -1;
        }
        tdci++;
    }

    // 1. Filter and sort the TDC Data
    // TODO: Well.. maybe it's better to do it NOT here?
    // Maybe both of these should be done in the BmnRawDataDecoder?
    int nOut = output->GetEntries();
    // nOut variable is needed. GetEntries is quite a slow function,
    // so storing the number of entries before filling the *output TObjArray works much faster

    multiset<BmnTDCDigit, _Tof701TDCCompare> temp;   // Create a multiset
    // Multiset automaticly sorts the elements in it.
    //_Tof701TDCCompare functional class is used to compare two BmnTDCDigit (see above)

    // Fill the multiset from the input array (it sorts the input array)
    TIter it = data->begin();
    while (it != data->end()) {
        BmnTDCDigit* obj = static_cast<BmnTDCDigit*>(*it);
        if (obj->GetType() != TOF701_TDC_TYPE && obj->GetType() != TOF701_TDCVXS_TYPE) {
            ++it;
            continue;
        }   // Just skip it in case if TDC is not TDC64VHL (see TOF701_TDC_TYPE define in the .h file)
        temp.insert(*obj);
        // printf("DEBUG: temp size %ld\n", temp.size());
        // cout << std::hex << obj->GetSerial() << " " << obj->GetType() << " "  << std::dec << int(obj->GetSlot()) << "
        // " << int(obj->GetHptdcId()) << " " << int(obj->GetChannel()) << " " << (obj->GetLeading()?"L":"T") <<
        // obj->GetValue() << endl;
        ++it;
    }

    // Now a loop over the temp multiset using iterator si
    multiset<BmnTDCDigit, _Tof701TDCCompare>::iterator si = temp.begin();

    while (si != temp.end()) {
        // Look for the crate serial and slot in the placement map
        Tof701PlMapIter parPair = PlacementMap.begin();

        // befor RUN8 BMN TOF400 are used FVME, so in the date we see serial of the trigger modules only and slot number
        // of the TDC So pair of FVME+slot is key to get serial of TDC (for INL correction and mapping)
        if (PeriodIndex <= 8 && RunIndex <= 5300) {
            parPair = PlacementMap.find(std::make_pair<UInt_t, UChar_t>(si->GetSerial(), si->GetSlot()));
            // cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Run " << PeriodIndex << endl;
        }

        // Since RUN8 BMN TOF400 use TDC72VXS, so in the date we see serial of the TDC (second value of the map)
        // But for WR correction we should to know Serial of TTVXS module in the current crate (First.first value of the
        // map)
        if (PeriodIndex >= 8 && RunIndex > 5300) {
            // cout << "===================================" << endl;
            // printf("Looking for TDC %08X in placement map\n", si->GetSerial());
            while (parPair != PlacementMap.end()) {
                // printf("Serial of TDC = %08X, Serial of TTVXS = %08X \n", parPair->second, parPair->first.first);
                if (parPair->second == si->GetSerial()) {
                    // printf("DEBUG: TDC from data = %08X, TDC from PlaceMap = %08X, TTVXS = %08X\n", si->GetSerial(),
                    // parPair->second, parPair->first.first); cout << "===================================" << endl;
                    break;
                }
                parPair++;
            }
            //            getchar();
        }
        // In case if nothing was found
        if (parPair == PlacementMap.end()) {
            // Just skip it
            si++;
            continue;
            printf(" TDC are not in DB\n");
        }

        if (PeriodIndex <= 8 && RunIndex <= 5300)
            fSerialForTS = si->GetSerial();
        if (PeriodIndex >= 8 && RunIndex > 5300)
            fSerialForTS = parPair->first.first;
        // printf("DEBUG: Crate Serial for TS %08X\n", fSerialForTS);
        auto itTS = mapTS->find(fSerialForTS);
        // maybe comment out this and try TimeShift = 0 in line 554
        if (itTS == mapTS->end()) {
            si++;
            continue;
        }
        Long64_t TimeShift = itTS->second;
        // printf(" Time Shift = %lld\n", TimeShift);

        // Get the TDC Serial for this crate serial and slot
        UInt_t TDC_Serial = parPair->second;
        // And look for this TDC Serial in the TDCMap to get the BmnTof701TDCParameters for this TDC
        Tof701TDCMapIter TDCPair = TDCMap.find(TDC_Serial);
        // printf("DEBUG: TDC_Serial: %08X\n", TDC_Serial);
        // In case if TDC Serial isn't in the TDCMap
        if (TDCPair == TDCMap.end()) {
            // Display a warning and skip. This is quite a serious error, actually
            if (fVerbose >= 2)
                cerr << "Warning: TDC with ID " << std::hex << TDC_Serial << std::dec << ": no data" << endl;
            si++;
            continue;
        }

        // Finally, get the BmnTof701TDCParameters instance for this TDC Serial
        BmnTof701TDCParameters* par = &(TDCPair->second);

        // And get it's channel using BmnTof701Raw2Digit::ToGlobalChannel (see above)
        UShort_t rchan = ToGlobalChannel(si->GetHptdcId(), si->GetChannel());

        // printf("DEBUG: GlobChan %d, plane from par %d; strip %d\n", rchan, par->ChannelMap[rchan].plane,
        // par->ChannelMap[rchan].strip);

        //        Int_t pl = par->ChannelMap[rchan].plane;
        //        if (si->GetSerial() == 0x07a9b1d6) {
        //            printf("Plane = %d\n", pl);
        //            getchar();
        //        }

        // Get the time from the TDC value
        // t = (val + INL[channel][val % 1024]) * (24ns / 1024)
        // See TOF701_BIN_NUMBER and TOF701_MAX_TIME defines in the .h file
        /*
        if(par->INL[rchan][(si->GetValue()) % TOF701_BIN_NUMBER] == 0) {
                cout << std::hex << TDC_Serial << std::dec << rchan << ":" << ((si->GetValue()) % TOF701_BIN_NUMBER) <<
        " - " << par->INL[rchan][(si->GetValue()) % TOF701_BIN_NUMBER] << endl;
        }
         */
        double timeFromDigit = (si->GetValue() + par->INL[rchan][(si->GetValue()) % TOF701_BIN_NUMBER])
                               * TOF701_MAX_TIME / double(TOF701_BIN_NUMBER);
        // printf("DEBUG: Ch(si) %d\t Ch(rchan) %d\t INLbin(si) %f\t DigitTime %f\n", si->GetChannel(), rchan,
        // par->INL[rchan][(si->GetValue()) % TOF701_BIN_NUMBER], timeFromDigit);

        if (si->GetLeading()) {
            // If this is a leading TDC digit, just fill the temporary time in the BmnTof701TDCParameters.
            par->t[rchan] = timeFromDigit;
        } else {
            // If this is NOT a leading TDC Digit...
            if (par->t[rchan] != -1) {
                // BmbTof701TDCParameters' temporary time (par->t[rchan]) has been already set
                // This temporary time should store the leading time
                if (timeFromDigit < (par->t[rchan]) && fVerbose >= 2) {
                    // This is impossible:
                    // Leading value is larger than the trailing time
                    // The BmnTDCDigit array has already been sorted, so this shouldn't happen
                    // But let's just make sure, this could be a serious error
                    cout << "Error: negative dt" << endl;

                    // Print all the TDC Digits we have loaded in the sorted order
                    multiset<BmnTDCDigit, _Tof701TDCCompare>::iterator si2 = temp.begin();
                    while (si2 != temp.end()) {
                        cout << std::hex << si2->GetSerial() << " " << int(si2->GetSlot()) << " " << std::dec
                             << int(si2->GetHptdcId()) << " " << int(si2->GetChannel()) << " "
                             << (si2->GetLeading() ? "L" : "T") << si2->GetValue() << endl;
                        si2++;
                    }
                    // Print where exactly the error has occured
                    cout << "Error at: " << endl;
                    cout << std::hex << si->GetSerial() << " " << int(si->GetSlot()) << " " << std::dec
                         << int(si->GetHptdcId()) << " " << int(si->GetChannel()) << " "
                         << (si->GetLeading() ? "L" : "T") << si->GetValue() << endl;
                    cout << std::hex << TDC_Serial << std::dec << " " << rchan << " " << (par->t[rchan]) << "--"
                         << timeFromDigit << endl;
                } else {
                    // So we've got the Leading and Trail times and everything seems to be okay
                    // Find the BmnTof701Map2 mapping element, which stores the plane, strip and side.
                    BmnTof701Map2* elem = &(par->ChannelMap[rchan]);

                    // Create a new BmnTof701Digit:
                    // Plane, strip, and side are loaded from the found BmnTof701Map2
                    // Leading time is loaded from the temporary storage in the BmnTof701TDCParameters class
                    // Trailing time is just the time in current TDC Digit
                    //(See above: this piece of code is executed only if the current BmnTDCDigit is NOT leading)

                    new ((*output)[nOut]) BmnTof701Digit(elem->plane, elem->strip, elem->side,
                                                         par->t[rchan] + TimeShift, timeFromDigit - (par->t[rchan]));
                    nOut++;
                }
            }
            // We don't need to do anything in case if no leading time has occured yet (if par->t[rchan] == -1)

            // Finally, reset the temporary storage to prevent errors
            par->t[rchan] = -1;
        }
        si++;
    }

    // if (fVerbose >= 2) printf("TOF701 :  %d digits\n", nOut);
    // cout << "================================" << endl;
}
