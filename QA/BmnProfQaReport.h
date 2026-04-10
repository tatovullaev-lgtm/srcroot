#ifndef BMNPROFQAREPORT_H_
#define BMNPROFQAREPORT_H_

#include "BmnSimulationReport.h"
class TH1;

/**
 * \class BmnProfQaReport
 * \brief Profilometer Report QA.
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru>
 * \date 2023
 */
class BmnProfQaReport : public BmnSimulationReport {
public:

    /**
     * \brief Constructor.
     */
    BmnProfQaReport();

    /**
     * \brief Constructor with parameters.
     */
    BmnProfQaReport(TString name);

    /**
     * \brief Destructor.
     */
    virtual ~BmnProfQaReport();
    
    /**
     * \brief Inherited from BmnSimulationReport.
     */
    virtual void Draw();
    
    static char* HistNameProfDigit(TString &detName, Short_t iSt, Short_t iMod, Short_t iLayer){
        return Form("%s_board%d_%s_side", detName.Data(), (iSt + 1), (iLayer ? "N" : "P"));
    }
    static char* HistNameProfHits(TString &detName, Short_t iSt){
        return Form("%s_board%d_x_vs_y", detName.Data(), (iSt + 1));
    }

protected:
    /**
     * \brief Inherited from BmnSimulationReport.
     */
    virtual void Create();
private:
    
    ClassDef(BmnProfQaReport, 1)
};

#endif /* BMNPROFQAREPORT_H_ */
