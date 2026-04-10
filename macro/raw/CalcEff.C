
void CalcEff(uint32_t runId, TString digiPath = ".")
{
    TString file_names(Form("%s/bmn_run%u_ev*_p*_digi.root", digiPath.Data(), runId));
    ROOT::RDataFrame df{"bmndata", file_names};

    auto df_num = df.Filter(
        [&](BmnEventHeader& eh) { return ((eh.GetInputSignalsAR() & BIT(11)) && (eh.GetInputSignalsAR() & BIT(5))); },
        {"BmnEventHeader."});
    uint64_t num = *(df_num.Count());
    auto df_den =
        df.Filter([&](BmnEventHeader& eh) { return ((eh.GetInputSignalsAR() & BIT(5))); }, {"BmnEventHeader."});
    uint64_t den = *(df_den.Count());
    printf("Run %u \n\tnFD&CCT1 %7lu\n\t    CCT1 %7lu\n", runId, num, den);
    if (den > 0) {
        double eff = num / (double)den;
        printf("\t\tEff %2.2f\n", eff);
    }
}
