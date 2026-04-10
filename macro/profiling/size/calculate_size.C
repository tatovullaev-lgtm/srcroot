R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "services/function_set.h"

Long64_t GetTotalSize(TBranch* b, bool ondisk, bool inclusive);
Long64_t GetBasketSize(TBranch* b, bool ondisk, bool inclusive);

Long64_t GetBasketSize(TObjArray* branches, bool ondisk, bool inclusive)
{
    Long64_t result = 0;
    size_t n = branches->GetEntries();
    for (size_t i = 0; i < n; i++)
        result += GetBasketSize(dynamic_cast<TBranch*>( branches->At( i ) ), ondisk, inclusive);

    return result;
}

Long64_t GetBasketSize(TBranch* b, bool ondisk, bool inclusive)
{
    Long64_t result = 0;
    if (b)
    {
        if (ondisk && b->GetZipBytes() > 0)
            result = b->GetZipBytes();
        else
            result = b->GetTotBytes();

        if (inclusive)
        {
            result += GetBasketSize(b->GetListOfBranches(), ondisk, true);
        }
        return result;
    }
    return result;
}

Long64_t GetTotalSize(TBranch* br, bool ondisk, bool inclusive)
{
    TMemFile f("buffer","CREATE");
    if (br->GetTree()->GetCurrentFile())
        f.SetCompressionSettings(br->GetTree()->GetCurrentFile()->GetCompressionSettings());

    f.WriteObject(br, "thisbranch");
    TKey* key = f.GetKey("thisbranch");
    Long64_t size;
    if (ondisk)
        size = key->GetNbytes();
    else
        size = key->GetObjlen();
    return GetBasketSize(br, ondisk, inclusive) + size;
}

Long64_t GetTotalSize(TObjArray* branches, bool ondisk)
{
    Long64_t result = 0;
    size_t n = branches->GetEntries();
    for (size_t i = 0; i < n; i++)
    {
        result += GetTotalSize(dynamic_cast<TBranch*>(branches->At(i)), ondisk, true);
        cerr << "After " << branches->At(i)->GetName() << " " << result << endl;
    }
    return result;
}

Long64_t GetTotalSize(TTree* t, bool ondisk)
{
    TKey* key = 0;
    if (t->GetDirectory())
        key = t->GetDirectory()->GetKey(t->GetName());

    Long64_t ondiskSize = 0;
    Long64_t totalSize = 0;
    if (key)
    {
        ondiskSize = key->GetNbytes();
        totalSize = key->GetObjlen();
    }
    else
    {
        TMemFile f("buffer","CREATE");
        if (t->GetCurrentFile())
            f.SetCompressionSettings(t->GetCurrentFile()->GetCompressionSettings());

        f.WriteTObject(t);
        key = f.GetKey(t->GetName());
        ondiskSize = key->GetNbytes();
        totalSize = key->GetObjlen();
    }
    if (t->GetBranchRef())
    {
        if (ondisk)
            ondiskSize += GetBasketSize(t->GetBranchRef(), true, true);
        else
            totalSize += GetBasketSize(t->GetBranchRef(), false, true);
    }
    if (ondisk)
        return ondiskSize + GetBasketSize(t->GetListOfBranches(), /* ondisk */ true, /* inclusive */ true);
    else
        return totalSize + GetBasketSize(t->GetListOfBranches(), /* ondisk */ false, /* inclusive */ true);
}

Long64_t sizeOnDisk(TTree* t)
{
    // Return the size on disk on this TTree
    return GetTotalSize(t, true);
}

Long64_t sizeOnDisk(TBranch* branch, bool inclusive)
{
    // Return the size on disk on this branch.
    // If 'inclusive' is true, include also the size of all its sub-branches
    return GetTotalSize(branch, true, inclusive);
}

void printBranchSummary(TBranch* br)
{
    cout << "The branch \"" << br->GetName() << "\" takes " << byte_size_to_human(sizeOnDisk(br,true)) << " bytes on disk" <<endl;
    size_t n = br->GetListOfBranches()->GetEntries();
    for (size_t i = 0; i < n; i++)
    {
        TBranch* subbr = dynamic_cast<TBranch*>(br->GetListOfBranches()->At(i));
        cout << "  Its sub-branch \"" << subbr->GetName() << "\" takes " << byte_size_to_human(sizeOnDisk(subbr,true)) << " bytes on disk" <<endl;
    }
}

void printTreeSummary(TTree* t)
{
    cout << "The TTree \"" << t->GetName() << "\" takes " << byte_size_to_human(sizeOnDisk(t)) << " on disk" <<endl;
    size_t n = t->GetListOfBranches()->GetEntries();
    for (size_t i = 0; i < n; i++)
    {
        TBranch* br =dynamic_cast<TBranch*>(t->GetListOfBranches()->At(i));
        cout << "  Its branch \"" << br->GetName() << "\" takes " << byte_size_to_human(sizeOnDisk(br, true)) << " on disk" <<endl;
    }
}

// reserved approach
void cloned_branch_sizez(TTree* t)
{
    TStopwatch timer;
    timer.Start();

    size_t n = t->GetListOfBranches()->GetEntries();
    for (size_t i = 0; i < n; i++)
    {
        TBranch* br =dynamic_cast<TBranch*>(t->GetListOfBranches()->At(i));
        t->SetBranchStatus("*", 0);
        t->SetBranchStatus(br->GetName(), 1);

        TFile* branch_file = new TFile("cloned_branch_size.root", "recreate");
        TTree* branch_tree = t->CloneTree();
        branch_tree->Write();
        branch_file->Close();
        delete branch_file;

        // print file size and remove it
        // ... //
    }

	timer.Stop();
   	Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
   	printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}

void calculate_size(TString root_file_path)
{
    gSystem->ExpandPathName(root_file_path);

    TChain* bmnChain = new TChain("bmndata");
    bmnChain->Add(root_file_path);

    printTreeSummary(bmnChain);
}
