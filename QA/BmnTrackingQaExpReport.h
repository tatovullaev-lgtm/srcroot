#ifndef BMNTRACKINGQAEXPREPORT_H_
#define BMNTRACKINGQAEXPREPORT_H_

#include "BmnSimulationReport.h"
// #include "BmnDrawOnline.h"
class TH1;

/**
 * \class BmnTrackingQaExpReport
 * \brief Create report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru> - disentangle Exp and MC processing
 * \date 2011-2022
 */
class BmnTrackingQaExpReport : public BmnSimulationReport
{
  public:
    /**
     * \brief Constructor.
     */
    BmnTrackingQaExpReport();

    /**
     * \brief Constructor with parameters.
     */
    BmnTrackingQaExpReport(TString name);

    /**
     * \brief Destructor.
     */
    virtual ~BmnTrackingQaExpReport();

    /**
     * \brief Inherited from BmnSimulationReport.
     */
    virtual void Draw();

  protected:
  private:
    ClassDef(BmnTrackingQaExpReport, 1)
};

#endif /* BMNTRACKINGQAEXPREPORT_H_ */
