#ifndef BMNCSCCONFIGURATION_H
#define BMNCSCCONFIGURATION_H

namespace BmnCSCConfiguration
{

enum CSC_CONFIG
{
    None = 0,           // empty config
    RunSpring2018,      // 1 station - (geometry file: CSC_RunSpring2018.root) (BM@N RUN-7)
    RunSRCSpring2018,   // 1 station - (geometry file: CSC_RunSRCSpring2018.root) (BM@N SRC RUN-7)
    Run8,               // 4 stations  - (geometry file: CSC_Run8.root)
    RunSRC2021,         // 2 stations  - (geometry file: CSC_RunSRC2021.root)
    LargeCSC_Run8,      // 1 station - (geometry file: LargeCSC_Run8.root)
    FullCSC_Run8,       // 5 stations - (geometry file: FullCSC_Run8_detailed.root) (4 small CSC + 1 large CSC)
    Run9                // 6 stations - (geometry file: CSC_Run9_detailed.root) (4 small CSC + 2 large CSC)
};
}   // namespace BmnCSCConfiguration

#endif
