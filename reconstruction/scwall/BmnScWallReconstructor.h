/* 
 * File:   BmnScWallReconstructor.h
 * Author: Sergey Nepochatykh
 *
 */

#ifndef BMNSCWALLRECONSTRUCTOR_H
#define BMNSCWALLRECONSTRUCTOR_H

#include <TNamed.h>

class BmnScWallReconstructor : public TNamed {
  public:
    BmnScWallReconstructor();
    
    virtual ~BmnScWallReconstructor();
    
    void SetELosses(Double_t ELosses);

    Double_t GetELosses();
    
    void SetCellId(UInt_t CellId);
    
    UInt_t GetCellId();
    
    void SetX1(Double_t X1);
    
    Double_t GetX1();
    
    void SetX2(Double_t X2);
    
    Double_t GetX2();
    
    void SetY1(Double_t Y1);
    
    Double_t GetY1();
    
    void SetY2(Double_t Y2);
    
    Double_t GetY2();

  private:
    Double_t fELosses;
    
    UInt_t fCellId;
    
    Double_t fX1, fX2, fY1, fY2;
    
    ClassDef(BmnScWallReconstructor, 2);
};

#endif /* BMNSCWALLRECONSTRUCTOR_H */

