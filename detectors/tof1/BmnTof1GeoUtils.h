#ifndef __BMN_TOF1_HIT_GEOUTILS_H
#define __BMN_TOF1_HIT_GEOUTILS_H 1

#include <TClonesArray.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TList.h>
#include <TVector3.h>
#include <iostream>
#include <map>

class LRectangle1 : public TObject   // convex quadrangle
{
  private:
    bool IsInvalid;

  public:
    enum Side_t
    {
        kRight = 0,
        kLeft = 1,
        kInvalid = -1,
        kUpper = 0,
        kLower = 1
    };
    // enum Side701_t
    // {
    //     kUpper = 0,
    //     kLower = 1
    // };

    Int_t volumeUID;
    Double_t length, width;

    TVector3 A, B, C, D, center, perp;   // [cm]

    LRectangle1()
        : IsInvalid(true)
        , volumeUID(kInvalid)
    {}
    LRectangle1(Int_t uid,
                const TVector3& a,
                const TVector3& b,
                const TVector3& c,
                const TVector3& d,
                bool check = false);

    Double_t GetLength() const { return length; }
    Double_t GetWidth() const { return width; }
    void GetInteriorAngle(int vertexIndex) const {}   // FIXME
    TVector3 GetCenter() const { return (A + B + C + D) * 0.25; }
    bool isInvalid() const { return IsInvalid; }

    Double_t DistanceFromPointToLineSegment(const TVector3* pos, const TVector3& P1, const TVector3& P2) const;
    Double_t DistanceFromPointToLine(const TVector3* pos, const TVector3& P1, const TVector3& P2) const;
    Double_t MinDistanceToEdge(const TVector3* pos, Side_t& side) const;
    Double_t MinDistanceToEdge701(const TVector3* pos, Side_t& side) const;

    void Print(std::ostream& out, const TVector3& point, const char* comment = nullptr) const;
    void Dump(const char* comment, std::ostream& out = std::cout) const;

    void Shift(const TVector3& shift)
    {
        A += shift;
        B += shift;
        C += shift;
        D += shift;
    };

    inline void InitCenterPerp()
    {
        center = (A + B + C + D) * 0.25;
        perp = (B - A).Cross(D - A).Unit();
    }

    inline void CalculateLengthnWidth()
    {
        length = TMath::Abs(B.X() - A.X());
        if (length != 16 && length != 56) {
            if (length == 15)
                length = 16;
            else if (length == 55)
                length = 56;
        }
        width = TMath::Abs(C.Y() - B.Y());
    }

    inline void CheckInValid()
    {
        IsInvalid = false;

        // Convex Polygon Definition: A polygon that has all interior angles less than 180°
        ;
        // Sum of Interior Angles, sum = 180*(n-2) degree, where n is the number of sides
        // A square has 4 sides, 	so interior angles sum = 360°
        ;
        // Rectangle check - all angles == 90 degree
        TVector3 ab = A - B, bc = B - C, cd = C - D, da = D - A;
        if (ab.Dot(bc) != 0. || bc.Dot(cd) != 0. || cd.Dot(da) != 0. || da.Dot(ab) != 0.) {
            std::cerr << "\n ---> ERROR: invalid Rectangle.";
            Dump("", std::cerr);
            IsInvalid = true;
        }
    }
    //  ClassDef(LRectangle1, 1);
};

class LStrip1 : public LRectangle1
{
  public:
    Bool_t isKilled = false;
    Int_t sectorID, boxID, detectorID, stripID;
    Int_t neighboring[2];   // dim same as  Side_t enum

    LStrip1();
    LStrip1(Int_t uid, Int_t sector, Int_t box, Int_t detector, Int_t strip);

    void SetIDs(Int_t uid, Int_t sector, Int_t box, Int_t detector, Int_t strip)
    {
        volumeUID = uid;
        sectorID = sector;
        boxID = box;
        detectorID = detector;
        stripID = strip;
    }

    inline void SetIsKilled(bool status) { isKilled = status; }
    inline bool GetIsKilled() const { return isKilled; }
    inline bool IsSameDetector(const LStrip1& strip) const
    {
        return (sectorID == strip.sectorID && boxID == strip.boxID && detectorID == strip.detectorID);
    }
    inline bool operator==(const LStrip1& rhs)
    {
        return (sectorID == rhs.sectorID && boxID == rhs.boxID && detectorID == rhs.detectorID
                && stripID == rhs.stripID);
    }
    inline bool operator!=(const LStrip1& rhs) { return !((*this) == rhs); }

    void Dump(const char* comment = nullptr, std::ostream& out = std::cout) const;
    Double_t Distance(Side_t side, const LStrip1& strip);
    Double_t Distance701(Side_t side, const LStrip1& strip);

    //  ClassDef(LStrip1, 1);
};

class BmnTof1GeoUtils : public TObject
{
    typedef std::map<Int_t, LStrip1> MStripType;   // pair<detectorUID, Strip parameters>
    typedef MStripType::const_iterator MStripCIT;
    typedef MStripType::iterator MStripIT;

    MStripType mStrips;   //!  indexing strips by detectorUID

  public:
    BmnTof1GeoUtils();

    void FindNeighborStrips(Int_t verbose = 0);
    void FindNeighborStrips701(Int_t verbose = 0);
    Int_t ParseTGeoManager(bool useMCinput, bool forced = false, Int_t verbose = 0);
    Int_t ParseTGeoManager701(bool useMCinput, bool forced = false, Int_t verbose = 0);
    const LStrip1* FindStrip(Int_t UID);
    const LStrip1* FindStrip701(Int_t UID);

    // ClassDef(BmnTof1GeoUtils, 1);
};

#endif
