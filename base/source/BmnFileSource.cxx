// -------------------------------------------------------------------------
// -----                       BmnFileSource source file               -----
// -------------------------------------------------------------------------
/** BmnFileSource.cxx
 *@author Created by Mohammad Al-Turany. Modified by Konstantin Gertsenberger <gertsen@jinr.ru>
 **
 ** Class presents a source inherited from FairFileSourceBase to work with BmnRoot data in ROOT files
 **/

#include "BmnFileSource.h"

#include "BmnEventHeader.h"
#include "DigiRunHeader.h"
#include "DstEventHeader.h"
#include "DstRunHeader.h"
#include "FairEventHeader.h"
#include "FairMCEventHeader.h"
#include "FairRootManager.h"

#include <TChainElement.h>
#include <TCollection.h>   // for TIter
#include <TDirectory.h>    // for TDirectory::TContext
#include <TList.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TRandom.h>   // for TRandom, gRandom
#include <algorithm>   // for find
#include <cmath>       // fmod
#include <fairlogger/Logger.h>
#include <vector>

using namespace std;

BmnFileSource::BmnFileSource(TFile* f, const char* Title, UInt_t)
    : FairFileSourceBase()
    , fInputTitle(Title)
    , fRootFile(f)
    , fFriendFileList()
    , fInputChainList()
    , fFriendTypeList()
    , fInputLevel()
    , fRunIdInfoAll()
    , fInChain(0)
    , fInTree(0)
    , fCbmout(0)
    , fCbmroot(0)
    , fSourceIdentifier(0)
    , fNoOfEntries(-1)
    , IsInitialized(kFALSE)
    , fMCHeader(0)
    , fEvtHeader(0)
    , fFileHeader(0)
    , fEventTimeInMCHeader(kTRUE)
    , fEvtHeaderIsNew(kFALSE)
    , fCurrentEntryNo(0)
    , fTimeforEntryNo(-1)
    , fEventTimeMin(0.)
    , fEventTimeMax(0.)
    , fEventTime(0.)
    , fBeamTime(-1.)
    , fGapTime(-1.)
    , fEventMeanTime(0.)
    , fCheckFileLayout(kTRUE)
{
    if ((!fRootFile) || fRootFile->IsZombie()) {
        LOG(fatal) << "Error opening the Input file";
    }

    ExtractRunNumber();
    LOG(debug) << "BmnFileSource created------------";
}

BmnFileSource::BmnFileSource(const TString* RootFileName, const char* Title, UInt_t)
    : FairFileSourceBase()
    , fInputTitle(Title)
    , fRootFile(0)
    , fFriendFileList()
    , fInputChainList()
    , fFriendTypeList()
    , fInputLevel()
    , fRunIdInfoAll()
    , fInChain(0)
    , fInTree(0)
    , fCbmout(0)
    , fCbmroot(0)
    , fSourceIdentifier(0)
    , fNoOfEntries(-1)
    , IsInitialized(kFALSE)
    , fMCHeader(0)
    , fEvtHeader(0)
    , fFileHeader(0)
    , fEventTimeInMCHeader(kTRUE)
    , fEvtHeaderIsNew(kFALSE)
    , fCurrentEntryNo(0)
    , fTimeforEntryNo(-1)
    , fEventTimeMin(0.)
    , fEventTimeMax(0.)
    , fEventTime(0.)
    , fBeamTime(-1.)
    , fGapTime(-1.)
    , fEventMeanTime(0.)
    , fCheckFileLayout(kTRUE)
{
    fRootFile = TFile::Open(RootFileName->Data());
    if ((!fRootFile) || fRootFile->IsZombie()) {
        LOG(fatal) << "Error opening the Input file";
    }

    ExtractRunNumber();
    LOG(debug) << "BmnFileSource created------------";
}

BmnFileSource::BmnFileSource(const TString RootFileName, const char* Title, UInt_t)
    : FairFileSourceBase()
    , fInputTitle(Title)
    , fRootFile(0)
    , fFriendFileList()
    , fInputChainList()
    , fFriendTypeList()
    , fInputLevel()
    , fRunIdInfoAll()
    , fInChain(0)
    , fInTree(0)
    , fCbmout(0)
    , fCbmroot(0)
    , fSourceIdentifier(0)
    , fNoOfEntries(-1)
    , IsInitialized(kFALSE)
    , fMCHeader(0)
    , fEvtHeader(0)
    , fFileHeader(0)
    , fEventTimeInMCHeader(kTRUE)
    , fEvtHeaderIsNew(kFALSE)
    , fCurrentEntryNo(0)
    , fTimeforEntryNo(-1)
    , fEventTimeMin(0.)
    , fEventTimeMax(0.)
    , fEventTime(0.)
    , fBeamTime(-1.)
    , fGapTime(-1.)
    , fEventMeanTime(0.)
    , fCheckFileLayout(kTRUE)
{
    fRootFile = TFile::Open(RootFileName.Data());
    if ((!fRootFile) || fRootFile->IsZombie()) {
        LOG(fatal) << "Error opening the Input file";
    }

    ExtractRunNumber();
    LOG(debug) << "BmnFileSource created------------";
}

//_____________________________________________________________________________
BmnFileSource::BmnFileSource(const TString RootFileName, int& period_number, int& run_number)
    : FairFileSourceBase()
    , fInputTitle("InputRootFile")
    , fRootFile(0)
    , fFriendFileList()
    , fInputChainList()
    , fFriendTypeList()
    , fInputLevel()
    , fRunIdInfoAll()
    , fInChain(0)
    , fInTree(0)
    , fCbmout(0)
    , fCbmroot(0)
    , fSourceIdentifier(0)
    , fNoOfEntries(-1)
    , IsInitialized(kFALSE)
    , fMCHeader(0)
    , fEvtHeader(0)
    , fFileHeader(0)
    , fEventTimeInMCHeader(kTRUE)
    , fEvtHeaderIsNew(kFALSE)
    , fCurrentEntryNo(0)
    , fTimeforEntryNo(-1)
    , fEventTimeMin(0.)
    , fEventTimeMax(0.)
    , fEventTime(0.)
    , fBeamTime(-1.)
    , fGapTime(-1.)
    , fEventMeanTime(0.)
    , fCheckFileLayout(kTRUE)
{
    fRootFile = TFile::Open(RootFileName.Data());
    if ((!fRootFile) || fRootFile->IsZombie()) {
        LOG(fatal) << "Error opening the Input file";
        return;
    }

    ExtractRunNumber();
    period_number = fPeriodNumber;
    run_number = fRunNumber;
    LOG(debug) << "BmnFileSource created------------";
}

int BmnFileSource::ExtractRunNumber()
{
    auto list = fRootFile->Get<TList>("BranchList");
    int isSimulationFile = 0;
    if (list)
        isSimulationFile = 1;

    // get period and run number
    DigiRunHeader* digi_run_header = (DigiRunHeader*)fRootFile->Get("DigiRunHeader");   // read DigiRunHeader if present
    if (digi_run_header) {
        fPeriodNumber = digi_run_header->GetPeriodId();
        fRunNumber = digi_run_header->GetRunId();
        fFileType = 0 + 10 * isSimulationFile;
        delete digi_run_header;
    } else {
        DstRunHeader* dst_run_header = (DstRunHeader*)fRootFile->Get("DstRunHeader");   // read DstRunHeader if present
        if (dst_run_header) {
            fPeriodNumber = dst_run_header->GetPeriodId();
            fRunNumber = dst_run_header->GetRunId();
            fFileType = 1 + 10 * isSimulationFile;
            delete dst_run_header;
        } else   // temporary solution supporting run 4 - run 7
        {
            TTree* bmn_tree = (TTree*)fRootFile->Get("bmndata");
            if (!bmn_tree) {
                LOG(fatal) << "ERROR: no 'bmndata' tree in file: " << fRootFile->GetName();
                return -1;
            }

            BmnEventHeader* fEventHeader = nullptr;
            bmn_tree->SetBranchAddress("BmnEventHeader.", &fEventHeader);
            if (bmn_tree->GetEntries() < 1) {
                LOG(fatal) << "ERROR: no entries in 'bmndata' tree (file: " << fRootFile->GetName() << ")";
                return -2;
            }

            bmn_tree->GetEntry(0);
            if (!fEventHeader) {
                LOG(debug) << "ERROR: headers not found in 'bmndata' tree (file: " << fRootFile->GetName()
                           << ") to extract the period and run numbers";
                return 0;
            }
            fRunNumber = fEventHeader->GetRunId();
            if (fRunNumber > 2000)
                fPeriodNumber = 7;
            else {
                if (fRunNumber > 1100)
                    fPeriodNumber = 6;
                else {
                    if (fRunNumber > 400)
                        fPeriodNumber = 5;
                    else
                        fPeriodNumber = 4;
                }
            }
        }
    }

    return 0;
}

BmnFileSource::~BmnFileSource() = default;

Bool_t BmnFileSource::Init()
{
    if (IsInitialized) {
        LOG(info) << "BmnFileSource already initialized";
        return kTRUE;
    }
    if (!fInChain) {
        fInChain = new TChain(FairRootManager::GetTreeName(), Form("/%s", FairRootManager::GetFolderName()));
        LOG(debug) << "BmnFileSource::Init() chain created";
        FairRootManager::Instance()->SetInChain(fInChain);
    }
    fInChain->Add(fRootFile->GetName());

    // Get the folder structure from file which describes the input tree.
    // There are two different names possible, so check both.
    fCbmroot = fRootFile->Get<TFolder>(FairRootManager::GetFolderName());
    if (!fCbmroot) {
        fCbmroot = fRootFile->Get<TFolder>("cbmroot");
        if (!fCbmroot) {
            fCbmroot = fRootFile->Get<TFolder>("cbmout");
            if (!fCbmroot) {
                fCbmroot = gROOT->GetRootFolder()->AddFolder(FairRootManager::GetFolderName(), "Main Folder");
            } else {
                fCbmroot->SetName(FairRootManager::GetFolderName());
            }
        }
    }

    // Get The list of branches from the input file and add it to the
    // actual list of existing branches.
    // Add this list of branches also to the map of input trees, which
    // stores the information which branches belong to which input tree.
    // There is at least one primary input tree, but there can be many
    // additional friend trees.
    // This information is needed to add new files to the correct friend
    // tree. With this information it is also possible to check if the
    // input files which are added to the input chain all have the same
    // branch structure. Without this check it is possible to add trees
    // with a different branch structure but the same tree name. ROOT
    // probably only checks if the name of the tree is the same.
    auto list = fRootFile->Get<TList>("BranchList");
    if (list == 0) {
        LOG(debug) << "No Branch list in the input file";
    }
    TString chainName = fInputTitle;
    fInputLevel.push_back(chainName);
    if (list) {
        TString ObjName;
        TObjString* Obj = 0;
        LOG(debug) << "Entries in the list " << list->GetEntries();
        for (Int_t i = 0; i < list->GetEntries(); i++) {
            Obj = dynamic_cast<TObjString*>(list->At(i));
            if (Obj != 0) {
                ObjName = Obj->GetString();
                LOG(debug) << "Branch name " << ObjName.Data();
                fCheckInputBranches[chainName].push_back(ObjName);
                FairRootManager::Instance()->AddBranchToList(ObjName.Data());
            }
        }
    }
    // if no special object with branches - 'BranchList'
    else
    {
        TObjArray* fBranchList = fInChain->GetListOfBranches();
        // if no any branches then exit (some errors occured)
        if (fBranchList == NULL)
            return kFALSE;

        LOG(debug) << "Entries in the chain " << fBranchList->GetEntries();
        TObject** ppObj = new TObject*[fBranchList->GetEntries()];
        for (int i = 0; i < fBranchList->GetEntries(); i++) {
            TBranch* pBranch = (TBranch*)fBranchList->At(i);
            TString ObjName = pBranch->GetName();
            LOG(debug) << "Branch name " << ObjName.Data();

            fCheckInputBranches[chainName].push_back(ObjName);
            FairRootManager::Instance()->AddBranchToList(ObjName.Data());

            ppObj[i] = NULL;
            // ActivateObject(&(ppObj[i]), ObjName);
            fInChain->SetBranchAddress(ObjName, &ppObj[i]);
            FairRootManager::Instance()->RegisterInputObject(ObjName, ppObj[i]);
        }
    }

    gROOT->GetListOfBrowsables()->Add(fCbmroot);
    fListFolderBmn.Add(fCbmroot);

    // Store the information about the unique runids in the input file
    // together with the filename and the number of events for each runid
    // this information is needed later to check if inconsitencies exist
    // between the main input chain and any of the friend chains.

    //  GetRunIdInfo(fInFile->GetName(), chainName);

    // Add all additional input files to the input chain and do a
    // consitency check
    {
        TDirectory::TContext restorecwd{};

        for (auto fileName : fInputChainList) {
            // Temporarily open the input file to extract information which
            // is needed to bring the friend trees in the correct order
            TFile* inputFile = TFile::Open(fileName);
            if (inputFile->IsZombie()) {
                LOG(fatal) << "Error opening the file " << fileName.Data()
                           << " which should be added to the input chain or as friend chain";
            }

            if (fCheckFileLayout) {
                // Check if the branchlist is the same as for the first input file.
                Bool_t isOk = CompareBranchList(inputFile, chainName);
                if (!isOk) {
                    LOG(fatal) << "Branch structure of the input file " << fRootFile->GetName()
                               << " and the file to be added " << fileName.Data() << " are different.";
                    return kFALSE;
                }
            }

            // Add the runid information for all files in the chain.
            // GetRunIdInfo(inputFile->GetName(), chainName);
            // Add the file to the input chain
            fInChain->Add(fileName);

            // Close the temporarly file
            inputFile->Close();
        }
    }
    fNoOfEntries = fInChain->GetEntries();

    LOG(debug) << "Entries in this Source " << fNoOfEntries;

    for (Int_t i = 0; i < fListFolderBmn.GetEntriesFast(); i++) {
        TFolder* fold = static_cast<TFolder*>(fListFolderBmn.At(i));

        if ((fFileType == 1) || (fFileType == 11)) {
            fEvtHeader = (DstEventHeader*)fold->FindObjectAny("DstEventHeader.");
            if (fEvtHeader)
                ActivateObject(reinterpret_cast<TObject**>(&fEvtHeader), "DstEventHeader.");
        } else {
            fEvtHeader = static_cast<FairEventHeader*>(fold->FindObjectAny("EventHeader."));
            if (fEvtHeader)
                ActivateObject(reinterpret_cast<TObject**>(&fEvtHeader), "EventHeader.");
        }

        fMCHeader = static_cast<FairMCEventHeader*>(fold->FindObjectAny("MCEventHeader."));
        if (fMCHeader) {
            ActivateObject(reinterpret_cast<TObject**>(&fMCHeader), "MCEventHeader.");
        }
    }

    FairRootManager::Instance()->SetListOfFolders(&fListFolderBmn);

    AddFriendsToChain();

    auto timebasedlist = fRootFile->Get<TList>("TimeBasedBranchList");
    if (timebasedlist == 0) {
        LOG(debug) << "No time based branch list in input file";
    } else {
        FairRootManager::Instance()->SetTimeBasedBranchNameList(timebasedlist);
    }

    return kTRUE;
}

void BmnFileSource::SetInTree(TTree* tempTree)
{
    fInTree = nullptr;
    fInTree = tempTree;
    fRootFile = tempTree->GetCurrentFile();
    fInChain->Reset();
    IsInitialized = kFALSE;
    Init();
}

Int_t BmnFileSource::ReadEvent(UInt_t i)
{
    fCurrentEntryNo = i;
    fEventTime = GetEventTime();
    if (fInChain->GetEntry(i))
        return 0;

    return 1;
}

Bool_t BmnFileSource::SpecifyRunId()
{
    /**Read the first entry in Chain to fill the event header and get the RunID*/
    return fInChain->GetEntry(0) != 0;
}

void BmnFileSource::Close()
{
    CloseInFile();
}

void BmnFileSource::AddFriend(TString fName)
{
    fFriendFileList.push_back(fName);
}

void BmnFileSource::AddFile(TString FileName)
{
    fInputChainList.push_back(FileName);
}

void BmnFileSource::AddFriendsToChain()
{
    // Loop over all Friend files and extract the type. The type is defined by
    // the tree which is stored in the file. If there is already a chain of with
    // this type of tree then the file will be added to this chain.
    // If there is no such chain it will be created.
    //
    // Check if the order of runids and the event numbers per runid for all
    // friend chains is the same as the one defined by the input chain.
    // TODO: Should the order be corrected or should the execution be stopped.
    // The order in the input tree defined by the order in which the files have
    // been added. A file is defined by the runid.

    // In the old way it was needed sometimes to add a freind file more
    // than once. This is not needed any longer, so we remove deuplicates
    // from the list and display a warning.
    list<TString> friendList;
    for (auto fileName : fFriendFileList) {
        if (find(friendList.begin(), friendList.end(), fileName) == friendList.end()) {
            friendList.push_back(fileName);
        }
    }
    // TODO: print a warning if it was neccessary to remove a filname from the
    // list. This can be chacked by comparing the size of both list

    TDirectory::TContext restorecwd{};

    Int_t friendType = 1;
    // Loop over all files which have been added as friends
    for (auto fileName : friendList) {
        TString inputLevel;
        // Loop over all already defined input levels to check if this type
        // of friend tree is already added.
        // If this type of friend tree already exist add the file to the
        // then already existing friend chain. If this type of friend tree
        // does not exist already create a new friend chain and add the file.
        Bool_t inputLevelFound = kFALSE;
        TFile* inputFile;
        for (auto level : fInputLevel) {
            inputLevel = level;

            inputFile = TFile::Open(fileName);
            if (inputFile->IsZombie()) {
                LOG(fatal) << "Error opening the file " << level.Data()
                           << " which should be added to the input chain or as friend chain";
            }

            // Check if the branchlist is already stored in the map. If it is
            // already stored add the file to the chain.
            Bool_t isOk = CompareBranchList(inputFile, inputLevel);
            if (isOk) {
                inputLevelFound = kTRUE;
                inputFile->Close();
                continue;
            }
            inputFile->Close();
        }
        if (!inputLevelFound) {
            inputLevel = Form("FriendTree_%i", friendType);
            CreateNewFriendChain(fileName, inputLevel);
            friendType++;
        }

        TChain* chain = fFriendTypeList[inputLevel];
        chain->AddFile(fileName, TTree::kMaxEntries, FairRootManager::GetTreeName());
    }

    // Check if all friend chains have the same runids and the same
    // number of event numbers as the corresponding input chain
    // CheckFriendChains();

    // Add all the friend chains which have been created to the main input chain.
    for (const auto& mi : fFriendTypeList) {
        TChain* chain = static_cast<TChain*>(mi.second);
        fInChain->AddFriend(chain);
    }

    // Print some output about the input structure
    PrintFriendList();
}

void BmnFileSource::PrintFriendList()
{
    // Print information about the input structure
    // List files from the input chain together with all files of
    // all friend chains

    LOG(info) << "The input consists out of the following trees and files: ";
    LOG(info) << " - " << fInChain->GetName();
    TObjArray* fileElements = fInChain->GetListOfFiles();
    TIter next(fileElements);
    TChainElement* chEl = 0;
    while ((chEl = static_cast<TChainElement*>(next()))) {
        LOG(info) << "    - " << chEl->GetTitle();
    }

    for (const auto& mi : fFriendTypeList) {
        TChain* chain = static_cast<TChain*>(mi.second);
        LOG(info) << " - " << chain->GetName();
        fileElements = chain->GetListOfFiles();
        TIter next1(fileElements);
        chEl = 0;
        while ((chEl = static_cast<TChainElement*>(next1()))) {
            LOG(info) << "    - " << chEl->GetTitle();
        }
    }
}

void BmnFileSource::CheckFriendChains()
{
    multimap<TString, multimap<TString, TArrayI>>::iterator it1;
    multimap<TString, TArrayI> map1;

    // Get the structure from the input chain
    it1 = fRunIdInfoAll.find("InputChain");
    map1 = it1->second;
    vector<Int_t> runid;
    vector<Int_t> events;
    for (auto& mmi : map1) {
        TArrayI bla = mmi.second;
        runid.push_back(bla[0]);
        events.push_back(bla[1]);
    }

    // Now loop over all chains except the input chain and comapare the
    // runids and event numbers.
    // If there is a mismatch stop the execution.
    Int_t errorFlag = 0;
    TString inputLevel;
    for (auto level : fInputLevel) {
        inputLevel = level;
        if (!inputLevel.Contains("InputChain")) {
            it1 = fRunIdInfoAll.find(inputLevel);
            map1 = it1->second;
            if (runid.size() != map1.size()) {
                errorFlag = 1;
                // goto error_label;
                break;
            }
            Int_t counter = 0;
            for (auto& mmi : map1) {
                TArrayI bla = mmi.second;
                if ((bla[0] != runid[counter]) || (bla[1] != events[counter])) {
                    errorFlag = 2;
                    // goto error_label;
                    break;
                }
                counter++;
            }
            if (errorFlag > 0) {
                break;
            }
        }
    }

    // Use goto to leave double loop at once in case of error
    // error_label:
    if (errorFlag > 0) {
        LOG(error) << "The input chain and the friend chain " << inputLevel.Data() << " have a different structure:";
        if (errorFlag == 1) {
            LOG(error) << "The input chain has the following runids and event numbers:";
            for (UInt_t i = 0; i < runid.size(); i++) {
                LOG(error) << " - Runid " << runid[i] << " with " << events[i] << " events";
            }
            LOG(error) << "The " << inputLevel.Data() << " chain has the following runids and event numbers:";
            for (auto& mmi : map1) {
                TArrayI bla = mmi.second;
                LOG(error) << " - Runid " << bla[0] << " with " << bla[1] << " events";
            }
        }
        if (errorFlag == 2) {
            Int_t counter = 0;
            for (auto& mmi : map1) {
                TArrayI bla = mmi.second;
                LOG(error) << "Runid Input Chain, " << inputLevel.Data() << " chain: " << bla[0] << ", "
                           << runid[counter];
                LOG(error) << "Event number Input Chain, " << inputLevel.Data() << " chain: " << bla[1] << ", "
                           << events[counter];
                counter++;
            }
        }
        LOG(fatal) << "Event structure mismatch";
    }
}

void BmnFileSource::CreateNewFriendChain(TString inputFile, TString inputLevel)
{
    TDirectory::TContext restorecwd{};
    TFile* f = TFile::Open(inputFile);

    // CHECK THAT IT WORKS (OR NOT) WITH EXP DIGI!!!
    TString folderName1 = FairRootManager::GetFolderName();
    TString folderName = Form("/%s", folderName1.Data());
    auto added = f->Get<TFolder>(folderName1);
    if (!added) {
        folderName = "/cbmout";
        folderName1 = "cbmout";
        added = f->Get<TFolder>("cbmout");
        if (!added) {
            folderName = "/cbmroot";
            folderName1 = "cbmroot";
            added = f->Get<TFolder>("cbmroot");
            if (!added) {
                LOG(fatal) << "Could not find folder cbmout nor cbmroot.";
                exit(-1);
            }
        }
    }
    folderName1 = folderName1 + "_" + inputLevel;
    added->SetName(folderName1);
    fListFolderBmn.Add(added);

    /**Get The list of branches from the friend file and add it to the actual list*/
    auto list = f->Get<TList>("BranchList");
    TString chainName = inputLevel;
    fInputLevel.push_back(chainName);
    if (list) {
        TObjString* Obj = 0;
        for (Int_t i = 0; i < list->GetEntries(); i++) {
            Obj = dynamic_cast<TObjString*>(list->At(i));
            fCheckInputBranches[chainName].push_back(Obj->GetString());
            FairRootManager::Instance()->AddBranchToList(Obj->GetString().Data());
        }
    }

    TChain* chain = new TChain(inputLevel, folderName);
    fFriendTypeList[inputLevel] = chain;

    f->Close();
}

Bool_t BmnFileSource::ActivateObject(TObject** obj, const char* BrName)
{
    if (fInTree) {
        fInTree->SetBranchStatus(BrName, 1);
        fInTree->SetBranchAddress(BrName, obj);
    }
    if (fInChain) {
        fInChain->SetBranchStatus(BrName, 1);
        fInChain->SetBranchAddress(BrName, obj);
    }

    return kTRUE;
}

Bool_t BmnFileSource::ActivateObjectAny(void** obj, const type_info& info, const char* BrName)
{
    if (fInTree) {
        return ActivateObjectAnyImpl(fInTree, obj, info, BrName);
    }
    if (fInChain) {
        return ActivateObjectAnyImpl(fInChain, obj, info, BrName);
    }
    return kFALSE;
}

void BmnFileSource::SetInputFile(TString name)
{
    fRootFile = TFile::Open(name.Data());
    if (!fRootFile || fRootFile->IsZombie()) {
        LOG(fatal) << "Error opening the input file";
    }
    LOG(info) << "BmnFileSource set------------";
}

Int_t BmnFileSource::CheckMaxEventNo(Int_t EvtEnd)
{
    Int_t MaxEventNo = 0;
    if (EvtEnd != 0) {
        MaxEventNo = EvtEnd;
    } else {
        MaxEventNo = fInChain->GetEntries();
    }

    return MaxEventNo;
}

void BmnFileSource::SetEventMeanTime(Double_t mean)
{
    fEventMeanTime = mean;
    fTimeProb = make_unique<TF1>("TimeProb", "(1/[0])*exp(-x/[0])", 0., mean * 10);
    fTimeProb->SetParameter(0, mean);
    fTimeProb->GetRandom();
    fEventTimeInMCHeader = kFALSE;
}

void BmnFileSource::SetEventTimeInterval(Double_t min, Double_t max)
{
    // disable fTimeProb for the uniform distribution
    fTimeProb.reset();
    fEventTimeMin = min;
    fEventTimeMax = max;
    fEventMeanTime = (fEventTimeMin + fEventTimeMax) / 2;
    fEventTimeInMCHeader = kFALSE;
}

void BmnFileSource::SetBeamTime(Double_t beamTime, Double_t gapTime)
{
    fBeamTime = beamTime;
    fGapTime = gapTime;
}

void BmnFileSource::SetEventTime()
{
    // Check if the time for the current entry is already set
    if (fTimeforEntryNo == fCurrentEntryNo)
        return;
    LOG(debug) << "Set event time for Entry = " << fTimeforEntryNo << " , where the current entry is "
               << fCurrentEntryNo << " and eventTime is " << fEventTime;
    if (fBeamTime < 0) {
        fEventTime += GetDeltaEventTime();
    } else {
        do {
            fEventTime += GetDeltaEventTime();
        } while (fmod(fEventTime, fBeamTime + fGapTime) > fBeamTime);
    }
    LOG(debug) << "New time = " << fEventTime;
    fTimeforEntryNo = fCurrentEntryNo;
}

Double_t BmnFileSource::GetDeltaEventTime()
{
    Double_t deltaTime = 0;
    if (fTimeProb) {
        deltaTime = fTimeProb->GetRandom();
        LOG(debug) << "Time set via sampling method : " << deltaTime;
    } else {
        deltaTime = gRandom->Uniform(fEventTimeMin, fEventTimeMax);
        LOG(debug) << "Time set via Uniform Random : " << deltaTime;
    }
    return deltaTime;
}

Double_t BmnFileSource::GetEventTime()
{
    LOG(debug) << "-- Get Event Time --";
    if (!fEvtHeaderIsNew && fEvtHeader != 0) {
        Double_t EvtTime = fEvtHeader->GetEventTime();
        if (!(EvtTime < 0)) {
            return EvtTime;
        }
    }

    if (fEventTimeInMCHeader && !fMCHeader) {
        LOG(debug) << "No MCEventHeader, time is set to 0";
        return 0;
    } else if (fEventTimeInMCHeader && fMCHeader) {
        fEventTime = fMCHeader->GetT();
        LOG(debug) << "Get event time from MCEventHeader : " << fEventTime << " ns";
        return fEventTime;
    } else {

        if (fTimeforEntryNo != fCurrentEntryNo) {
            SetEventTime();
        }
        LOG(debug) << "Calculate event time from user input : " << fEventTime << " ns";
        return fEventTime;
    }
}

void BmnFileSource::ReadBranchEvent(const char* BrName)
{
    /**fill the object with content if the other branches in this tree entry were already read**/
    if (fEvtHeader == 0) {
        return;
    }   // No event header, Reading will start later
    if (fInTree) {
        fInTree->FindBranch(BrName)->GetEntry(fEvtHeader->GetMCEntryNumber());
        fEventTime = GetEventTime();
        return;
    }
    if (fInChain) {
        fInChain->FindBranch(BrName)->GetEntry(fEvtHeader->GetMCEntryNumber());
        fEventTime = GetEventTime();
        return;
    }
}

void BmnFileSource::ReadBranchEvent(const char* BrName, Int_t Entry)
{
    fCurrentEntryNo = Entry;
    if (fInTree) {
        fInTree->FindBranch(BrName)->GetEntry(Entry);
        fEventTime = GetEventTime();
        return;
    }
    if (fInChain) {
        fInChain->FindBranch(BrName)->GetEntry(Entry);
        fEventTime = GetEventTime();
        return;
    }
}

void BmnFileSource::FillEventHeader(FairEventHeader* feh)
{
    feh->SetEventTime(GetEventTime());
    if (fEvtHeader) {
        feh->SetRunId(fEvtHeader->GetRunId());
        feh->SetMCEntryNumber(fEvtHeader->GetMCEntryNumber());
    } else {
        if (fFileType == 0)
            feh->SetRunId(fRunNumber);
    }
    if (fMCHeader) {
        feh->SetRunId(fMCHeader->GetRunID());
        feh->SetMCEntryNumber(fMCHeader->GetEventID());
    }
    feh->SetInputFileId(0);
}
