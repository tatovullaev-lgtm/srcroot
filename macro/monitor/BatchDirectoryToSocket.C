
// R__LOAD_LIBRARY(libFairMQ)
void BatchDirectoryToSocket(TString dirName = "/bmn/run/current/")
{
    unique_ptr<BmnOnlineDecoder> bm = make_unique<BmnOnlineDecoder>();
    bm->BatchDirectoryToSocket(dirName);
}
