// Legacy
// Keep this class for backward compatibility.

#ifndef BMNTOF2DIGIT_H
#define BMNTOF2DIGIT_H 2

#include "BmnTof1Digit.h"

class BmnTof2Digit : public BmnTof1Digit
{
  public:
    /** Default constructor **/
    BmnTof2Digit();

    /** Destructor **/
    virtual ~BmnTof2Digit();

  protected:
    ClassDef(BmnTof2Digit, 3);
};

#endif /* BMNTOF2DIGIT_H */
