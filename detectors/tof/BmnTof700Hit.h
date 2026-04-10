/* 
 * File:   BmnTof700Hit.h
 * Author: merz
 *
 * Created on October 24, 2014, 10:58 AM
 */

#ifndef BMNTOF700HIT_H
#define BMNTOF700HIT_H

#include <math.h>
#include <iostream>
#include "BmnHit.h"
#include <TObject.h>

using namespace std;

// class TClonesArray;

class BmnTof700Hit : public BmnHit {
public:

    /** Default constructor **/
    BmnTof700Hit();

    /** Constructor to use **/
    BmnTof700Hit(Int_t detUID, TVector3 posHit, TVector3 posHitErr, Int_t pointIndex);

    /** Destructor **/
    virtual ~BmnTof700Hit();

private:

    ClassDef(BmnTof700Hit, 1);

};

#endif /* BMNTOF700HIT_H */

