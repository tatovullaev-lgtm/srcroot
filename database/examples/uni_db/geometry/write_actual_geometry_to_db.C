R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "database/uni_db/examples/geometry/write_geometry_to_db.C"
#include "macro/run/run_sim_bmn.C"

/**
 *  Macro for writing the currently used full setup geometry to the UniConDa Database for the given runs
 *  Technically it runs run_sim_bmn.C and writes obtained "full_geometry.root" to the database
 *  Example: root 'write_actual_geometry_to_db.C(8, 6310, 8427)'
 **/
void write_actual_geometry_to_db(int period = 9, int start_run = 1, int end_run = 100000)
{
    gRandom->SetSeed(0);
    TString simFileName = Form("$VMCWORKDIR/macro/run/bmnsim_%d.root", UInt_t(gRandom->Integer(UINT32_MAX)));

    run_sim_bmn("", simFileName, 0, 1, BOX, kTRUE, kTRUE);
    gSystem->Unlink(simFileName);

    write_geometry_to_db("full_geometry.root", period, start_run, period, end_run);
    // gSystem->Unlink("full_geometry.root");
}
