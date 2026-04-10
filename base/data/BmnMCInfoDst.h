/********************************************************************************
 *    BmnMCInfoDst.h                                                            *
 *    Fill MC information from Simulation chain into DST chain                  *
 *******************************************************************************/
#ifndef BMNMCINFODST_H
#define BMNMCINFODST_H

#include "TClonesArray.h"
#include "TNamed.h"

class BmnMCInfoDst : public TNamed
{
  public:
    /** Default constructor **/
    BmnMCInfoDst();
    /** Destructor **/
    ~BmnMCInfoDst();
    /** Copy constructor **/
    BmnMCInfoDst(const BmnMCInfoDst& other);
    /** Copy assignment operator **/
    BmnMCInfoDst& operator=(const BmnMCInfoDst& other);
    /** Move constructor **/
    BmnMCInfoDst(BmnMCInfoDst&& other);
    /** Move assignment operator **/
    BmnMCInfoDst& operator=(BmnMCInfoDst&& other);

    /** Accessors **/
    Double_t GetB() const { return fB; }         /// impact parameter [fm]
    Double_t GetX() const { return fX; }         /// vertex x [cm]
    Double_t GetY() const { return fY; }         /// vertex y [cm]
    Double_t GetZ() const { return fZ; }         /// vertex z [cm]
    Double_t GetRotX() const { return fRotX; }   /// rot. around x-axis [rad]
    Double_t GetRotY() const { return fRotY; }   /// rot. around y-axis [rad]
    Double_t GetRotZ() const { return fRotZ; }   /// rot. around z-axis [rad]
    TClonesArray* GetMCTracks() const { return fMCTracks; }

    /** Modifiers **/
    void SetB(Double_t b) { fB = b; }
    void SetVertex(Double_t x, Double_t y, Double_t z);
    void SetRotX(Double_t rotx) { fRotX = rotx; }
    void SetRotY(Double_t roty) { fRotY = roty; }
    void SetRotZ(Double_t rotz) { fRotZ = rotz; }

  private:
    Double32_t fB;      ///  Impact parameter [fm] (if relevant)
    Double32_t fX;      ///  Primary vertex x [cm]
    Double32_t fY;      ///  Primary vertex y [cm]
    Double32_t fZ;      ///  Primary vertex z [cm]
    Double32_t fRotX;   ///  Rotation around x-axis (beam tilt) [rad]
    Double32_t fRotY;   ///  Rotation around y-axis (beam tilt) [rad]
    Double32_t fRotZ;   ///  Rotation around z-axis (event plane) [rad]

    TClonesArray* fMCTracks;

    void Reset();

    ClassDef(BmnMCInfoDst, 1);
};

#endif
