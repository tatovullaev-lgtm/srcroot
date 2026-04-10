#include "MpdGetNumEvents.h"

#include "FairRootManager.h"
#include "TChain.h"
#include "TFile.h"
#include "TKey.h"
#include "TString.h"
#include "TSystem.h"

#include <iostream>
using namespace std;

MpdLibZ::MpdLibZ(const char* filename)
{
    fileName = filename;
}

MpdLibZ::~MpdLibZ()
{
    if (file != NULL) {
        close();
        file = NULL;
    }
}

/* Opens a gzip (.gz) file for reading or writing. The mode parameter
   is as in fopen ("rb" or "wb") but can also include a compression level
   ("wb9") or a strategy: 'f' for filtered data as in "wb6f", 'h' for
   Huffman only compression as in "wb1h", or 'R' for run-length encoding
   as in "wb1R". (See the description of deflateInit2 for more information
   about the strategy parameter.)

     gzopen can be used to read a file which is not in gzip format; in this
   case gzread will directly read from the file without decompression.

     gzopen returns NULL if the file could not be opened or if there was
   insufficient memory to allocate the (de)compression state; errno
   can be checked to distinguish the two cases (if errno is zero, the
   zlib error is Z_MEM_ERROR).  */
int MpdLibZ::open(const char* mode)
{
    file = gzopen(fileName, mode);

    if (file == NULL)
        return -1;
    else
        return 0;
}

/* Returns 1 when EOF has previously been detected reading the given
   input stream, otherwise zero. */
int MpdLibZ::eof()
{
    return gzeof(file);
}

/* Flushes all pending output if necessary, closes the compressed file
   and deallocates all the (de)compression state. The return value is the zlib
   error number (see function gzerror below). */
int MpdLibZ::close()
{
    int ret = gzclose(file);
    if (ret == 0)
        file = NULL;

    return ret;
}

/* Writes the given null-terminated string to the compressed file, excluding
   the terminating null character.
   gzputs returns the number of characters written, or -1 in case of error. */
int MpdLibZ::puts(char* s)
{
    return gzputs(file, s);
}

/* Reads bytes from the compressed file until len-1 characters are read, or
   a newline character is read and transferred to buf, or an end-of-file
   condition is encountered.  The string is then terminated with a null
   character.
   gzgets returns buf, or Z_NULL in case of error. */
char* MpdLibZ::gets(char* buf, int len)
{
    return gzgets(file, buf, len);
}

/* Reads the given number of uncompressed bytes from the compressed file.
   If the input file was not in gzip format, gzread copies the given number
   of bytes into the buffer.
   gzread returns the number of uncompressed bytes actually read (0 for
   end of file, -1 for error). */
int MpdLibZ::read(voidp buf, unsigned len)
{
    return gzread(file, buf, len);
}

/* Writes the given number of uncompressed bytes into the compressed file.
   gzwrite returns the number of uncompressed bytes actually written
   (0 in case of error). */
int MpdLibZ::write(voidpc buf, unsigned len)
{
    return gzwrite(file, buf, len);
}

/* Sets the starting position for the next gzread or gzwrite on the given
   compressed file. The offset represents a number of bytes in the
   uncompressed data stream. The whence parameter is defined as in lseek(2);
   the value SEEK_END is not supported.

   If the file is opened for reading, this function is emulated but can be
   extremely slow. If the file is opened for writing, only forward seeks are
   supported; gzseek then compresses a sequence of zeroes up to the new
   starting position.

   gzseek returns the resulting offset location as measured in bytes from
   the beginning of the uncompressed stream, or -1 in case of error, in
   particular if the file is opened for writing and the new starting position
   would be before the current position. */
off_t MpdLibZ::seek(off_t pos, int whence)
{
    switch (whence) {
        case 0:
            return gzseek(file, pos, SEEK_SET);
        case 1:
            return gzseek(file, pos, SEEK_CUR);
        case 2:
            return gzseek(file, pos, SEEK_END);
    }

    return -1;
}

/* Returns the starting position for the next gzread or gzwrite on the given
   compressed file.  This position represents a number of bytes in the
   uncompressed data stream, and is zero when starting, even if appending or
   reading a gzip stream from the middle of a file using gzdopen().

   gztell(file) is equivalent to gzseek(file, 0L, SEEK_CUR) */
off_t MpdLibZ::tell()
{
    return gztell(file);
}

// return event count in a given file produced by the PHSD event generator
// filename - file path to the generator file, iVerbose - output level (0 - no output, 1 - only errors, 2 - full output)
Int_t MpdGetNumEvents::GetNumPHSDEvents(const char* filename, int iVerbose)
{
    string file_path = filename;
    TString strFilePath(file_path);
    gSystem->ExpandPathName(strFilePath);
    if (gSystem->AccessPathName(strFilePath.Data()) == true) {
        if (iVerbose > 0)
            cout << "GetNumEvents ERROR: No generator file was found: " << strFilePath << endl;
        return -1;
    }

    MpdLibZ* libz = new MpdLibZ(filename);
    int ret_code = libz->open("rb");
    if (ret_code != 0) {
        if (iVerbose > 0)
            cout << "GetNumPHSDEvents ERROR while opening file: " << filename << endl;
        return ret_code;
    }

    char fbuffer[256];
    Int_t fntr;
    Float_t fb;
    int num = 0;

    while (!libz->eof()) {
        libz->gets(fbuffer, 256);   // 1st line
        if (libz->eof())
            return -1;   // end of file reached

        int res = sscanf(fbuffer, "%d %*d %*d %e", &fntr, &fb);
        if (res != 2) {
            if (iVerbose > 0)
                printf("GetNumPHSDEvents ERROR: selftest error in header, scan %d of 2\n", res);
            return -2;
        }

        libz->gets(fbuffer, 256);   // 2nd line
        if (libz->eof()) {
            if (iVerbose > 0)
                printf("GetNumPHSDEvents ERROR: unexpected end of file\n");
            return -2;
        }

        for (Int_t i = 1; i <= fntr; i++) {
            /* read track */
            libz->gets(fbuffer, 256);
            if (libz->eof()) {
                if (iVerbose > 0)
                    printf("GetNumPHSDEvents ERROR: unexpected end of file\n");
                return -2;
            }   // if
        }   // for

        num++;
    }   // while (!libz->eof())

    libz->close();
    delete libz;

    if (iVerbose > 1)
        cout << "The total number of events in the PHSD file will be processed - " << num << endl;

    return num;
}

bool MpdGetNumEvents::GetQGSMEventHeader(char* ss, MpdLibZ* libz, Int_t& fQGSM_format_ID, int iVerbose)
{
    libz->gets(ss, 250);

    // If end of input file is reached : close it and abort run
    if (libz->eof()) {
        libz->close();
        return false;
    }
    bool wrong_file = false;

    TString tss(ss);
    if (tss.Contains("QGSM"))   // 0
    {
        libz->gets(ss, 250);
        tss = ss;
        Int_t lines = 0;
        while (!(tss.Contains("particles, b,bx,by"))) {
            if (tss.Contains("QGSM format ID")) {
                sscanf(ss, "QGSM format ID=%d", &fQGSM_format_ID);

                // correction of incorrect format_ID in some data files
                if (fQGSM_format_ID == 2) {
                    off_t file_pos = libz->tell();
                    for (int k = 0; k < 5; k++)
                        libz->gets(ss, 250);

                    if (strlen(ss) > 90)
                        fQGSM_format_ID = 3;

                    libz->seek(file_pos, SEEK_SET);
                }
            }

            libz->gets(ss, 250);
            tss = ss;
            lines++;

            if ((fQGSM_format_ID >= 2) && (lines >= 4))
                return true;

            if (lines > 25) {
                wrong_file = true;
                break;
            }
        }   // while (!(tss.Contains("particles, b,bx,by")))
    } else {
        if (fQGSM_format_ID >= 2)
            return true;

        if (!tss.Contains("particles, b,bx,by")) {
            libz->gets(ss, 250);
            tss = ss;
        }
    }   // else

    if (wrong_file) {
        if (iVerbose > 0)
            cout << "GetQGSMEventHeader ERROR: Wrong file header! 1 " << endl;
        libz->close();
        return false;
    }

    // 3
    if (!tss.Contains("event")) {
        if (fQGSM_format_ID >= 2)
            return true;
        else {
            if (iVerbose > 0)
                cout << "GetQGSMEventHeader ERROR: Wrong event header! 2 " << endl;
            if (iVerbose > 1)
                printf("Format id:%d \n %s \n", fQGSM_format_ID, ss);
            libz->close();
            return false;
        }   // else
    }

    char tmp[250];

    switch (fQGSM_format_ID) {
        case 0:
        case 1:
            libz->gets(tmp, 250);
            break;
        case 2:
        case 3:
        case 4: {
            libz->gets(ss, 250);
            libz->gets(ss, 250);
            break;
        }
        default:
            break;
    }   // switch

    return true;
}

// return event count in a given file produced by the QGSM/DCM-QGSM event generator
// filename - file path to the generator file, iVerbose - output level (0 - no output, 1 - only errors, 2 - full output)
Int_t MpdGetNumEvents::GetNumQGSMEvents(const char* filename, int iVerbose)
{
    string file_path = filename;
    TString strFilePath(file_path);
    gSystem->ExpandPathName(strFilePath);
    if (gSystem->AccessPathName(strFilePath.Data()) == true) {
        if (iVerbose > 0)
            cout << "GetNumEvents ERROR: No generator file was found: " << strFilePath << endl;
        return -1;
    }

    Int_t fQGSM_format_ID = 0;
    MpdLibZ* libz = new MpdLibZ(filename);
    int ret_code = libz->open("rb");
    if (ret_code != 0) {
        if (iVerbose > 0)
            cout << "GetNumQGSMEvents ERROR while opening file: " << filename << endl;
        return ret_code;
    }

    char ss[252];
    TString sss = ss;
    Int_t eventId = 0, nTracks = 0, num = 0;

    while (GetQGSMEventHeader(ss, libz, fQGSM_format_ID, iVerbose)) {
        num++;
        sss = ss;

        // read event
        sscanf(ss, " %d %d", &eventId, &nTracks);
        // Loop over tracks in the current event
        for (Int_t itrack = 0; itrack < nTracks; itrack++)
            libz->gets(ss, 250);
    }

    libz->close();
    delete libz;

    if (iVerbose > 1)
        cout << "The total number of events in the QGSM file will be processed - " << num << endl;
    return num;
}

// return event count in a given file produced by the UrQMD event generator
// filename - file path to the generator file, iVerbose - output level (0 - no output, 1 - onstringly errors, 2 - full
// output)
Int_t MpdGetNumEvents::GetNumURQMDEvents(const char* filename, int iVerbose)
{
    string file_path = filename;
    TString strFilePath(file_path);
    gSystem->ExpandPathName(strFilePath);
    if (gSystem->AccessPathName(strFilePath.Data()) == true) {
        if (iVerbose > 0)
            cout << "GetNumEvents ERROR: No generator file was found: " << strFilePath << endl;
        return -1;
    }

    MpdLibZ* libz = new MpdLibZ(filename);
    int ret_code = libz->open("rb");
    if (ret_code != 0) {
        if (iVerbose > 0)
            cout << "GetNumURQMDEvents ERROR: Failure of opening file: " << filename << endl;
        return ret_code;
    }

    char read[200];
    int ntracks, num = 0;
    while (!libz->eof()) {
        // ---> Read and check first event header line from input file
        libz->gets(read, 200);
        Int_t urqmdVersion = 0;
        sscanf(read, "UQMD   version:       %d   1000  %d  output_file  14", &urqmdVersion, &urqmdVersion);

        if (libz->eof()) {
            // cout<<"GetNumURQMDEvents ERROR: Incorrect end of input file reached!"<<endl;
            break;
        }

        if (read[0] != 'U') {
            if (iVerbose > 0)
                cout << "GetNumURQMDEvents ERROR: Wrong event header!" << endl;
            return -2;
        }

        // ---> Read rest of event header
        for (int iline = 0; iline < ((urqmdVersion == 30400) ? 16 : 13); iline++)
            libz->gets(read, 200);

        libz->gets(read, 200);
        sscanf(read, "%d", &ntracks);
        libz->gets(read, 200);

        for (int itrack = 0; itrack < ntracks; itrack++)
            libz->gets(read, 200);

        num++;
    }

    libz->close();
    delete libz;

    if (iVerbose > 1)
        cout << "The total number of events in the UrQMD file will be processed - " << num << endl;
    return num;
}

// return event count in a given file produced by the DCM-SMM event generator
// filename - file path to the generator file, iVerbose - output level (0 - no output, 1 - only errors, 2 - full output)
Int_t MpdGetNumEvents::GetNumDCMSMMEvents(const char* filename, int iVerbose)
{
    string file_path = filename;
    TString strFilePath(file_path);
    gSystem->ExpandPathName(strFilePath);
    if (gSystem->AccessPathName(strFilePath.Data()) == true) {
        if (iVerbose > 0)
            cout << "GetNumEvents ERROR: No generator file was found: " << strFilePath << endl;
        return -1;
    }

    MpdLibZ* libz = new MpdLibZ(filename);
    libz->open("rb");

    char r200[200];
    Int_t header_size = 1;
    for (Int_t i = 0; i < header_size; i++) {
        libz->gets(r200, 200);
        if (i == 0) {
            string str0(r200);
            if (str0.find("Results of DCM-SMM") != string::npos)   // new version
                header_size = 20;
            else if (str0.find("Results of QGSM") != string::npos)   // old version
                header_size = 3;
            else {
                if (iVerbose > 0)
                    cout << "GetNumDCMSMMEvents ERROR: Wrong input file format" << endl;
                return -2;
            }
        }
    }

    char read[128];
    int ntracks, num = 0;
    Int_t evnr = 0, last_evnr = 0;
    while (!libz->eof()) {
        char* gets_res = libz->gets(read, 128);
        if (gets_res == Z_NULL)
            break;   // EOF reached
        sscanf(read, "%d", &evnr);
        // if an event is skipped, print it
        if (evnr - last_evnr != 1)
            cout << "ERROR: the sequence of events was interrupted " << last_evnr << "-" << evnr << endl;
        // if (evnr-num != 1) break; // otherwise it gives 1 more event after eof
        last_evnr = evnr;
        for (Int_t ibeam = 0; ibeam < 3; ibeam++) {
            libz->gets(read, 128);
            sscanf(read, "%d", &ntracks);
            for (Int_t i = 0; i < ntracks; i++)
                libz->gets(read, 128);
        }
        num++;
    }

    libz->close();
    delete libz;

    if (iVerbose > 1)
        cout << "The total number of events in the DCMSMM file will be processed - " << num << endl;
    return num;
}

string MpdGetNumEvents::FindFirstTree(const char* filename, int iVerbose)
{
    // open file to search for the first tree
    TFile* rootFile = new TFile(filename, "READ");
    if (!rootFile->IsOpen()) {
        if (iVerbose > 0)
            cout << "GetNumROOTEvents ERROR: could not open ROOT file: " << filename << endl;
        return "";
    }

    TIter next(rootFile->GetListOfKeys());
    TKey* key;
    while ((key = static_cast<TKey*>(next())) != nullptr) {
        if (strcmp(key->GetClassName(), "TTree") != 0)
            continue;
        string tree_name = (static_cast<TTree*>(key->ReadObj()))->GetName();
        delete rootFile;
        return tree_name;
    }

    delete rootFile;
    if (iVerbose > 0)
        cout << "GetNumROOTEvents ERROR: could not find a tree in the ROOT file: " << filename << endl;
    return "";
}

// return event count in a given ROOT file as a number of records in a specfied tree
// filename - file path to the ROOT file
// treename - name of the tree in the ROOT file; "" - get FairRootManager default tree name, nullptr - get first tree
// from the file iVerbose - output level (0 - no output, 1 - only errors, 2 - full output)
Int_t MpdGetNumEvents::GetNumROOTEvents(const char* filename, const char* treename, int iVerbose)
{
    string file_path = filename;
    TString strFilePath(file_path);
    gSystem->ExpandPathName(strFilePath);
    if (gSystem->AccessPathName(strFilePath.Data()) == true) {
        if (iVerbose > 0)
            cout << "GetNumEvents ERROR: No generator file was found: " << strFilePath << endl;
        return -1;
    }

    TChain* fileTree;
    if (treename == nullptr) {
        string tree_name = FindFirstTree(filename, iVerbose);
        if (tree_name == "")
            return -2;
        fileTree = new TChain(tree_name.c_str());
    } else if (treename[0] == '\0')
        fileTree = new TChain(FairRootManager::GetTreeName());
    else
        fileTree = new TChain(treename);
    fileTree->Add(filename);

    Int_t num = fileTree->GetEntries();

    delete fileTree;

    if (iVerbose > 1)
        cout << "The total number of events in the ROOT file will be processed - " << num << endl;
    return num;
}

// Return event count in a given file for an event generator defined automatically
// filename - file path to the generator file, iVerbose - output level (0 - no output, 1 - only errors, 2 - full output)
Int_t MpdGetNumEvents::GetNumEvents(const char* filename, int iVerbose)
{
    string file_path = filename;
    TString strFilePath(file_path);
    gSystem->ExpandPathName(strFilePath);
    if (gSystem->AccessPathName(strFilePath.Data()) == true) {
        if (iVerbose > 0)
            cout << "GetNumEvents ERROR: No generator file was found: " << strFilePath << endl;
        return -1;
    }

    if (strFilePath.Contains(".root", TString::kIgnoreCase))
        return MpdGetNumEvents::GetNumROOTEvents(strFilePath.Data(), nullptr, iVerbose);

    if (strFilePath.Contains(".f14", TString::kIgnoreCase))
        return MpdGetNumEvents::GetNumURQMDEvents(strFilePath.Data(), iVerbose);

    if (strFilePath.Contains(".r12", TString::kIgnoreCase)) {
        // find generator name in a header of the ASCII file
        MpdLibZ* libz = new MpdLibZ(filename);
        libz->open("rb");

        // iGenerator: 0 - not defined, 1 - QGSM, 2 - SMM
        int iGenerator = 0;
        char r200[200];
        libz->gets(r200, 200);
        string str0(r200);
        if (str0.find("Results of QGSM") != string::npos)
            iGenerator = 1;
        else if (str0.find("Results of DCM-SMM") != string::npos)
            iGenerator = 2;
        else {
            if (iVerbose > 0)
                cout << "GetNumEvents ERROR: Wrong input file format, generator type was not defined" << endl;
            return -2;
        }

        libz->close();
        delete libz;

        switch (iGenerator) {
            case 1:
                return MpdGetNumEvents::GetNumQGSMEvents(strFilePath.Data(), iVerbose);
            case 2:
                return MpdGetNumEvents::GetNumDCMSMMEvents(strFilePath.Data(), iVerbose);
            default:
                return -2;
        }
    }

    return -3;
}
