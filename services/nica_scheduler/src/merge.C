#if !defined(__CLING__) || defined(__MAKECLING__)

// ROOT includes
#include "TSystem.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TChain.h"
#include "TChainElement.h"

#include <iostream>
using namespace std;
#endif

// pcFileList - file list with space delimiter
// mode: 0 - save only resulting TChain to the first file name with the data trees pointing to partial files
//       1 - merge to the first file name and delete partial files
//       2 - merge to the first file name and not delete partial files
void merge(const char* pcFileList, int mode = 1)
{
    string strFileList(pcFileList);
    if ((mode < 0) || (mode > 2))
    {
        cout<<"Second parameter ('mode') can be 0-2 only. The mode is assigned to a default value (1)."<<endl;
        mode = 1;
    }

    Bool_t res;
    TString sResultFile = "", sFirstFile = "", sCurrentFile = "";
    int i = 0, beg = 0, countFiles = 0;

    // MERGE FILES
    if (mode > 0)
    {
        TFileMerger mergerFull,         // mergerFull for full merging with the first partial file
                    mergerPart;         // mergerPart for merging only a data tree containing in result files

        // cycle through the input string with a file list
        while (strFileList[i] != '\0')
        {
            // if space then add the current file to the merge result
            if ((strFileList[i] == ' ') && (beg != i))
            {
                strFileList[i] = '\0';

                // define the number of the current files
                switch (countFiles)
                {
                    // if first file name then it will be the result file (write to sResultFile string)
                    case 0:
                    {
                        sResultFile = strFileList.substr(beg);
                        gSystem->ExpandPathName(sResultFile);

                        break;
                    }
                    // if second file name then it will be the first file to full merge with a data tree and other objects (e.g. FairBaseParSet)
                    case 1:
                    {
                        sFirstFile = strFileList.substr(beg);
                        gSystem->ExpandPathName(sFirstFile);
                        mergerFull.AddFile(sFirstFile.Data());

                        break;
                    }
                    // if other then it will be a file to merge only a data tree without additional metadata structures
                    default:
                    {
                        sCurrentFile = strFileList.substr(beg);
                        gSystem->ExpandPathName(sCurrentFile);
                        mergerPart.AddFile(sCurrentFile.Data());
                    }
                }

                strFileList.replace(i, 1, " ");
                beg = i+1;
                countFiles++;
            }

            i++;
        }

        // after string end has been found, add the last file to the output file list
        if (beg != i)
        {
            switch (countFiles)
            {
                // if first file name then it will be the result file (write to sResultFile string)
                case 0:
                {
                    sResultFile = strFileList.substr(beg);
                    gSystem->ExpandPathName(sResultFile);

                    break;
                }
                // if second file name then it will be the first file to full merge with a data tree and other objects (e.g. FairBaseParSet)
                case 1:
                {
                    sFirstFile = strFileList.substr(beg);
                    gSystem->ExpandPathName(sFirstFile);
                    mergerFull.AddFile(sFirstFile.Data());

                    break;
                }
                // if other then it will be a file to merge only a data tree
                default:
                {
                    sCurrentFile = strFileList.substr(beg);
                    gSystem->ExpandPathName(sCurrentFile);
                    mergerPart.AddFile(sCurrentFile.Data());
                }
            }

            countFiles++;
        }

        // merge the files to the result file
        switch (countFiles)
        {
            case 0:
            {
                cout<<"Only one file name in the input string, nothing has been merged"<<endl;
                return;
            }
            case 1:
            {
                res = TFile::Cp(sFirstFile, sResultFile);
                if (res == kFALSE)
                    cout<<"ERROR: the single file was not copied to the result file"<<endl;
                else
                {
                    // delete temporary file if 'mode' parameter is equal 1
                    if (mode == 1)
                    {
                        if (remove(sFirstFile.Data()) != 0)
                            cout<<"ERROR: deleting one temporary file "<<sFirstFile.Data()<<endl;
                        else
                            cout<<endl<<"Temporary file were successfully removed"<<endl;
                    }
                }

                break;
            }
            default:
            {
                bool isError = false;
                //mergerPart.AddObjectNames("???");
                mergerPart.OutputFile(sResultFile);
                //cout<<endl<<"Result File: "<<sResultFile<<endl<<endl;

                res = mergerPart.PartialMerge(TFileMerger::kAll|TFileMerger::kIncremental|TFileMerger::kOnlyListed);

                if (res == kFALSE)
                    cout<<"ERROR: partial merging to the result file failed"<<endl;
                else
                {
                    // delete temporary files if 'mode' parameter is equal 1
                    if (mode == 1)
                    {
                        TIter next(mergerPart.GetMergeList());
                        TObjString* url = 0;
                        while ((url = (TObjString*)next()) != NULL)
                        {
                            sCurrentFile = url->GetString().Data();
                            //cout<<endl<<"sCurrentFile: "<<sCurrentFile<<endl<<endl;
                            if (remove(sCurrentFile.Data()) != 0)
                            {
                                isError = true;
                                cout<<"ERROR: deleting temporary file "<<sCurrentFile.Data()<<endl;
                            }
                        }
                    }
                }
                mergerPart.Reset();

                mergerFull.AddFile(sResultFile);
                mergerFull.OutputFile(sResultFile);
                //mergerFull.PrintFiles("");
                res = mergerFull.Merge();

                if (res == kFALSE)
                    cout<<"ERROR: full merging to the result file failed"<<endl;
                else
                {
                    // delete first temporary file if 'mode' parameter is equal 1
                    if (mode == 1)
                    {
                        //cout<<endl<<"sFirstFile: "<<sFirstFile<<endl<<endl;
                        if (remove(sFirstFile.Data()) != 0)
                        {
                            isError = true;
                            cout<<"ERROR: deleting first temporary file "<<sFirstFile.Data()<<"failed"<<endl;
                        }
                    }
                }
                mergerFull.Reset();

                if (mode == 1)
                {
                    if (isError)
                        cout<<"There were errors while temporary files removing"<<endl;
                    else
                        cout<<endl<<"Temporary files were successfully removed"<<endl;
                }
            }// default: count of partitial files > 1
        }// switch: merge the files to the result file
    }// if (mode > 0) : merge partitial files

    // SAVE ONLY TCHAIN if mode is equal 0
    else
    {
        TChain chainUnion("bmndata");

        // go to the end of the string with a file list
        while (strFileList[i] != '\0')
        {
            // if space then add next file to the result chain
            if ((strFileList[i] == ' ') && (beg != i))
            {
                strFileList[i] = '\0';

                // if first file name is defined then it is the result file (write to sResultFile string)
                if (countFiles == 0)
                    sResultFile = strFileList.substr(beg);
                else
                    chainUnion.Add(strFileList.substr(beg).c_str());

                strFileList.replace(i, 1, " ");
                beg = i+1;
                countFiles++;
            }

            i++;
        }

        // after string end has been found, add last file to chain
        if (beg != i)
        {
            if (countFiles == 0)
                sResultFile = strFileList.substr(beg);
            else
                chainUnion.Add(strFileList.substr(beg).c_str());

            countFiles++;
        }

        // write result file with TChain
        if (countFiles > 1)
        {
            TFile fChain(sResultFile, "RECREATE");
            chainUnion.Write();
            fChain.Close();

            Int_t events = chainUnion.GetEntries();
            cout<<"The Chain witn "<<events<<" event(s) was written to file \""<<sResultFile<<"\" to point following files:"<<endl;

            TObjArray *fileElements = chainUnion.GetListOfFiles();
            TIter next(fileElements);
            TChainElement* chEl = 0;
            while ((chEl = (TChainElement*)next()) != NULL)
            {
                const char* pc = chEl->GetTitle();
                cout<<pc<<endl;
            }
        }
    }// else : save only TChain

    // test reading result file
/*  TChain chainRead("bmndata");
    chainRead.Add(sResultFile);

    Int_t events = chainRead.GetEntries();
    cout<<"The count of events in test reading is equal "<<events<<endl;*/
}
