#include "BmnTof1GeoUtils.h"

#include "BmnTOF1Point.h"
#include "BmnTofHit.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TGeoNode.h"

#include <TGeoManager.h>
#include <assert.h>
using namespace std;

BmnTof1GeoUtils::BmnTof1GeoUtils() {}

void BmnTof1GeoUtils::FindNeighborStrips(Int_t verbose)
{
    size_t NR = 0, NL = 0;
    const LStrip1* strip2;
    double distance;
    for (MStripIT it1 = mStrips.begin(), itEnd1 = mStrips.end(); it1 != itEnd1; it1++)   // cycle1 by strips
    {
        LStrip1* strip1 = &(it1->second);

        for (MStripCIT it2 = mStrips.begin(), itEnd2 = mStrips.end(); it2 != itEnd2; it2++)   // cycle2 by strips
        {
            strip2 = &(it2->second);

            // CATION: Ckeck  only left and right sides(one row strips NOW)
            distance = strip1->Distance(LStrip1::kRight, *strip2);
            if (distance < 0.8)   // CAUTION: constant depends on the geometry layout
            {
                strip1->neighboring[LStrip1::kRight] = strip2->volumeUID;
                NR++;
            }

            distance = strip1->Distance(LStrip1::kLeft, *strip2);
            if (distance < 0.8)   // CAUTION: constant depends on the geometry layout
            {
                strip1->neighboring[LStrip1::kLeft] = strip2->volumeUID;
                NL++;
            }

        }   // cycle2 by strips
    }   // cycle1 by strips

    if (verbose)
        cout << " [BmnTof1GeoUtils::FindNeighborStrips] Neighbor strips: left = " << NL << ", right = " << NR << endl;
}

void BmnTof1GeoUtils::FindNeighborStrips701(Int_t verbose)
{
    size_t LO = 0, UP = 0;
    const LStrip1* strip2;
    double distance;
    for (MStripIT it1 = mStrips.begin(), itEnd1 = mStrips.end(); it1 != itEnd1; it1++)   // cycle1 by strips
    {
        LStrip1* strip1 = &(it1->second);

        for (MStripCIT it2 = mStrips.begin(), itEnd2 = mStrips.end(); it2 != itEnd2; it2++)   // cycle2 by strips
        {
            strip2 = &(it2->second);

            // CATION: Ckeck  only left and right sides(one row strips NOW)
            distance = strip1->Distance701(LStrip1::kLower, *strip2);

            if (distance < 0.8 && !strip2->GetIsKilled())   // CAUTION: constant depends on the geometry layout
            {
                strip1->neighboring[LStrip1::kLower] = strip2->volumeUID;
                LO++;
            } else if (distance < 0.8 && strip2->GetIsKilled()) {

                strip1->neighboring[LStrip1::kLower] = LRectangle1::Side_t::kInvalid;
            }
            distance = strip1->Distance701(LStrip1::kUpper, *strip2);

            if (distance < 0.8)   // CAUTION: constant depends on the geometry layout
            {
                strip1->neighboring[LStrip1::kUpper] = strip2->volumeUID;
                UP++;
            }

        }   // cycle2 by strips
    }   // cycle1 by strips
    const LStrip1* pstrip;

    if (verbose)
        cout << " [BmnTof1GeoUtils::FindNeighborStrips701] Neighbor strips: lower = " << LO << ", upper = " << UP
             << endl;
}

Int_t BmnTof1GeoUtils::ParseTGeoManager(bool useMCinput, bool forced, Int_t verbose)
{
    assert(gGeoManager);
    if (!forced && !mStrips.empty())
        return -1;   // already parsed and filled
    mStrips.clear();
    //	TString stripName, pathTOF = "/cave_1/TOFB1_0";
    TString stripName, pathTOF = "/cave_1/TOF400_0";
    gGeoManager->cd(pathTOF);
    Double_t* X0Y0Z0 = new Double_t[3];
    X0Y0Z0[0] = X0Y0Z0[1] = X0Y0Z0[2] = 0.;   // center of sensetive detector
    Double_t *local = new Double_t[3], master[3], dX, dY, dZ;

    Int_t volumeUID, detectorID, stripID;
    Int_t nDetectors = 0, nStrips = 0;

    TObjArray* array = gGeoManager->GetCurrentVolume()->GetNodes();
    TIterator* it1 = array->MakeIterator();

    TGeoNode *detectorNode, *stripNode;
    while ((detectorNode = (TGeoNode*)it1->Next()))   // detectors
    {
        TString PATH1 = pathTOF + "/" + detectorNode->GetName();
        detectorID = detectorNode->GetNumber();
        TString detectorNodeName = (TString)detectorNode->GetName();
        if (!detectorNodeName.Contains("Detector"))
            continue;
        nDetectors++;
        //    		cout<<"\n DETECTOR: "<<detectorNode->GetName()<<", copy# "<<detectorID<<" path= "<<PATH1.Data();

        TIterator* it2 = detectorNode->GetNodes()->MakeIterator();
        while ((stripNode = (TGeoNode*)it2->Next()))   // strips
        {
            stripName = stripNode->GetName();
            if (!stripName.Contains("StripActiveGas"))
                continue;

            TString PATH2 = PATH1 + "/" + stripName;
            stripID = stripNode->GetNumber();
            nStrips++;
            //    			cout<<"\n \tSTRIP: "<<stripNode->GetName()<<", copy# "<<stripID<<" path=
            //    "<<PATH2.Data();

            gGeoManager->cd(PATH2);

            TGeoMatrix* matrix = gGeoManager->GetCurrentMatrix();   // calculate global TGeoHMatrix for current node
            matrix->LocalToMaster(X0Y0Z0, master);                  // 0.0.0. --> MRS

            TGeoBBox* box = (TGeoBBox*)gGeoManager->GetCurrentNode()->GetVolume()->GetShape();
            dX = box->GetDX();
            dY = box->GetDY();
            dZ = box->GetDZ();
            //      		cout<<"\n center: ("<<master[0]<<", "<<master[1]<<", "<<master[2]<<") d("<<dX<<",
            //      "<<dY<<", "<<dZ<<")";

            volumeUID = BmnTOF1Point::GetVolumeUID(0, detectorID, stripID);   // regionID == 0 now

            LStrip1 stripData(volumeUID, 0, 0, detectorID, stripID);
            stripData.center.SetXYZ(master[0], master[1], master[2]);

            // edges on the front plate of the strips. perp along Z.
            local[0] = -dX;
            local[1] = -dY;
            local[2] = -dZ;
            matrix->LocalToMaster(local, master);
            stripData.A.SetXYZ(master[0], master[1], master[2]);

            local[0] = +dX;
            local[1] = -dY;
            local[2] = -dZ;
            matrix->LocalToMaster(local, master);
            stripData.B.SetXYZ(master[0], master[1], master[2]);

            local[0] = +dX;
            local[1] = +dY;
            local[2] = -dZ;
            matrix->LocalToMaster(local, master);
            stripData.C.SetXYZ(master[0], master[1], master[2]);

            local[0] = -dX;
            local[1] = +dY;
            local[2] = -dZ;
            matrix->LocalToMaster(local, master);
            stripData.D.SetXYZ(master[0], master[1], master[2]);

            stripData.InitCenterPerp();

            // stripData.Dump("\n strip:");
            bool IsUniqueUID = mStrips.insert(make_pair(volumeUID, stripData)).second;
            assert(IsUniqueUID);

        }   // strips
        delete it2;
    }   // detectors

    if (verbose)
        LOG(info) << "[BmnTof400HitProducer::ParseTGeoManager] detectors = " << nDetectors << ", strips= " << nStrips
                  << ". ";
    delete it1;
    delete[] X0Y0Z0;
    delete[] local;
    return nDetectors;
}

Int_t BmnTof1GeoUtils::ParseTGeoManager701(bool useMCinput, bool forced, Int_t verbose)
{
    assert(gGeoManager);
    if (!forced && !mStrips.empty())
        return -1;   // already parsed and filled
    mStrips.clear();
    //	TString stripName, pathTOF = "/cave_1/TOFB1_0";
    TString stripName, gasName, pathTOF = "/cave_1/tof700_0";
    gGeoManager->cd(pathTOF);
    Double_t* X0Y0Z0 = new Double_t[3];
    X0Y0Z0[0] = X0Y0Z0[1] = X0Y0Z0[2] = 0.;   // center of sensetive detector
    Double_t *local = new Double_t[3], master[3], dX, dY, dZ;

    Int_t volumeUID, detectorID, stripID;
    Int_t nDetectors = 0, nStrips = 0;

    TObjArray* array = gGeoManager->GetCurrentVolume()->GetNodes();
    TIterator* it1 = array->MakeIterator();

    TGeoNode *detectorNode, *detectorNodeGas, *stripNode;
    while ((detectorNode = (TGeoNode*)it1->Next()))   // detectors
    {
        TString PATH1 = pathTOF + "/" + detectorNode->GetName();
        detectorID = detectorNode->GetNumber();
        TString detectorNodeName = detectorNode->GetName();   // aka chamName
        if (!detectorNodeName.Contains("Chamber"))
            continue;
        nDetectors++;
        //    		cout<<"\n DETECTOR: "<<detectorNode->GetName()<<", copy# "<<detectorID<<" path= "<<PATH1.Data();

        TIterator* it2 = detectorNode->GetNodes()->MakeIterator();
        while ((detectorNodeGas = (TGeoNode*)it2->Next()))   // gas
        {
            gasName = detectorNodeGas->GetName();
            if (!gasName.Contains("Gas"))
                continue;
            TString PATHG = PATH1 + "/" + gasName;
            TObjArray* Nodes = detectorNodeGas->GetNodes();
            TIterator* it3 = Nodes->MakeIterator();

            while ((stripNode = (TGeoNode*)it3->Next())) {

                stripName = stripNode->GetName();
                if (!stripName.Contains("ActiveGasStrip"))
                    continue;

                TString PATH2 = PATHG + "/" + stripName;
                stripID = stripNode->GetNumber();
                nStrips++;
                //    			cout<<"\n \tSTRIP: "<<stripNode->GetName()<<", copy# "<<stripID<<" path=
                //    "<<PATH2.Data();

                gGeoManager->cd(PATH2);

                TGeoMatrix* matrix = gGeoManager->GetCurrentMatrix();   // calculate global TGeoHMatrix for current node
                matrix->LocalToMaster(X0Y0Z0, master);                  // 0.0.0. --> MRS

                TGeoBBox* box = (TGeoBBox*)gGeoManager->GetCurrentNode()->GetVolume()->GetShape();
                dX = box->GetDX();
                dY = box->GetDY();
                dZ = box->GetDZ();
                // cout<<"\n center: ("<<master[0]<<", "<<master[1]<<", "<<master[2]<<") d("<<dX<<", "<<dY<<",
                // "<<dZ<<")";

                volumeUID = BmnTOF1Point::GetVolumeUID(0, detectorID, stripID);   // regionID == 0 now
                LStrip1 stripData(volumeUID, 0, 0, detectorID, stripID);
                if (stripID == 1)
                    stripData.SetIsKilled(true);
                stripData.center.SetXYZ(master[0], master[1], master[2]);

                // edges on the front plate of the strips. perp along Z.
                local[0] = -dX;
                local[1] = -dY;
                local[2] = -dZ;
                matrix->LocalToMaster(local, master);
                stripData.A.SetXYZ(master[0], master[1], master[2]);

                local[0] = +dX;
                local[1] = -dY;
                local[2] = -dZ;
                matrix->LocalToMaster(local, master);
                stripData.B.SetXYZ(master[0], master[1], master[2]);

                local[0] = +dX;
                local[1] = +dY;
                local[2] = -dZ;
                matrix->LocalToMaster(local, master);
                stripData.C.SetXYZ(master[0], master[1], master[2]);

                local[0] = -dX;
                local[1] = +dY;
                local[2] = -dZ;
                matrix->LocalToMaster(local, master);
                stripData.D.SetXYZ(master[0], master[1], master[2]);

                stripData.InitCenterPerp();
                stripData.CalculateLengthnWidth();

                // stripData.Dump("\n strip:");
                // cout << stripData.GetLength() << endl << endl;
                bool IsUniqueUID = mStrips.insert(make_pair(volumeUID, stripData)).second;
                assert(IsUniqueUID);
            }   // strips
            delete it3;
        }   // gas
        delete it2;
    }   // detectors

    if (verbose)
        LOG(info) << "[BmnTof700HitProducer::ParseTGeoManager] detectors = " << nDetectors << ", strips= " << nStrips
                  << ". ";
    delete it1;
    delete[] X0Y0Z0;
    delete[] local;
    return nDetectors;
}

const LStrip1* BmnTof1GeoUtils::FindStrip(Int_t UID)
{

    MStripCIT cit = mStrips.find(UID);
    // assert(cit != mStrips.end());
    if (cit == mStrips.end())
        return NULL;
    return &(cit->second);

    // MStripCIT cit = mStrips.find(UID);
    // assert(cit != mStrips.end());
    // return &(cit->second);
}

const LStrip1* BmnTof1GeoUtils::FindStrip701(Int_t UID)
{
    MStripCIT cit = mStrips.find(UID);
    // assert(cit != mStrips.end());
    if (cit == mStrips.end())
        return NULL;
    return &(cit->second);
}

LRectangle1::LRectangle1(Int_t uid,
                         const TVector3& a,
                         const TVector3& b,
                         const TVector3& c,
                         const TVector3& d,
                         bool check)
    : IsInvalid(false)
    , volumeUID(uid)
    , A(a)
    , B(b)
    , C(c)
    , D(d)
{
    if (check)
        CheckInValid();
}

Double_t LRectangle1::DistanceFromPointToLine(const TVector3* pos, const TVector3& P1, const TVector3& P2) const
{
    assert(P1 != P2);

    return ((*pos - P1).Cross(*pos - P2)).Mag() / (P2 - P1).Mag();
}

Double_t LRectangle1::DistanceFromPointToLineSegment(const TVector3* pos, const TVector3& P1, const TVector3& P2) const
{
    assert(P1 != P2);

    TVector3 v = P2 - P1;
    TVector3 w = (*pos) - P1;

    double c1 = w.Dot(v);
    if (c1 <= 0)
        return w.Mag();

    double c2 = v.Dot(v);
    if (c2 <= c1)
        return ((*pos) - P2).Mag();

    TVector3 Pb = P1 + (c1 / c2) * v;
    return ((*pos) - Pb).Mag();
}

Double_t LRectangle1::MinDistanceToEdge(const TVector3* pos, Side_t& side) const
{
    double right = DistanceFromPointToLineSegment(pos, A, D);
    double left = DistanceFromPointToLineSegment(pos, B, C);

    // sorting & return minimal value
    if (right <= left) {
        side = LStrip1::kRight;
        return right;
    }

    side = LStrip1::kLeft;
    return left;
}

Double_t LRectangle1::MinDistanceToEdge701(const TVector3* pos, Side_t& side) const
{
    double down = DistanceFromPointToLineSegment(pos, A, B);
    double up = DistanceFromPointToLineSegment(pos, C, D);

    // sorting & return minimal value
    if (down <= up) {
        side = LStrip1::kLower;
        return down;
    }

    side = LStrip1::kUpper;
    return up;
}

void LRectangle1::Print(ostream& out, const TVector3& point, const char* comment) const
{
    if (comment)
        out << comment;
    out << " (" << point.X() << ",\t" << point.Y() << ",\t" << point.Z() << ") ";
}

void LRectangle1::Dump(const char* comment, ostream& out) const
{
    if (comment) {
        out << comment;
        out << " uid=" << volumeUID << " IsInvalid=" << IsInvalid;
    }
    Print(out, A, "\tA:");
    Print(out, B, "\tB:");
    Print(out, C, "\tC:");
    Print(out, D, "\tD:");
}

LStrip1::LStrip1()
    : sectorID(kInvalid)
    , boxID(kInvalid)
    , detectorID(kInvalid)
    , stripID(kInvalid)
{
    neighboring[kRight] = kInvalid;
    neighboring[kLeft] = kInvalid;
}

LStrip1::LStrip1(Int_t uid, Int_t sector, Int_t box, Int_t detector, Int_t strip)
    : sectorID(sector)
    , boxID(box)
    , detectorID(detector)
    , stripID(strip)
{
    volumeUID = uid;
    neighboring[kRight] = kInvalid;
    neighboring[kLeft] = kInvalid;
}

void LStrip1::Dump(const char* comment, ostream& out) const
{
    if (comment)
        out << comment;
    out << "  ids: " << sectorID << ", " << boxID << ", " << detectorID << ", " << stripID << ", " << GetIsKilled();

    LRectangle1::Dump(nullptr, out);
}

Double_t LStrip1::Distance(Side_t side, const LStrip1& strip)
{
    Double_t value, min1 = 1.e+10, min2 = 1.e+10;   // big value

    if ((*this) == strip)
        return min1 + min2;   // same strip
    if (!IsSameDetector(strip))
        return min1 + min2;   // different modules

    TVector3 *p1, *p2;
    switch (side) {
        case kRight:
            p1 = &A;
            p2 = &D;
            break;
        case kLeft:
            p1 = &B;
            p2 = &C;
            break;
        case kInvalid:
            break;
    }

    value = fabs((*p1 - strip.A).Mag());
    min1 = (value < min1) ? value : min1;
    value = fabs((*p1 - strip.B).Mag());
    min1 = (value < min1) ? value : min1;
    value = fabs((*p1 - strip.C).Mag());
    min1 = (value < min1) ? value : min1;
    value = fabs((*p1 - strip.D).Mag());
    min1 = (value < min1) ? value : min1;

    value = fabs((*p2 - strip.A).Mag());
    min2 = (value < min2) ? value : min2;
    value = fabs((*p2 - strip.B).Mag());
    min2 = (value < min2) ? value : min2;
    value = fabs((*p2 - strip.C).Mag());
    min2 = (value < min2) ? value : min2;
    value = fabs((*p2 - strip.D).Mag());
    min2 = (value < min2) ? value : min2;

    return min1 + min2;
}

Double_t LStrip1::Distance701(Side_t side, const LStrip1& strip)
{
    Double_t value, min1 = 1.e+10, min2 = 1.e+10;   // big value

    if ((*this) == strip)
        return min1 + min2;   // same strip
    if (!IsSameDetector(strip))
        return min1 + min2;   // different modules

    TVector3 *p1, *p2;
    switch (side) {
        case kUpper:
            p1 = &C;
            p2 = &D;
            break;   // ????
        case kLower:
            p1 = &B;
            p2 = &A;
            break;
        case kInvalid:
            break;
    };

    value = fabs((*p1 - strip.A).Mag());
    min1 = (value < min1) ? value : min1;
    value = fabs((*p1 - strip.B).Mag());
    min1 = (value < min1) ? value : min1;
    value = fabs((*p1 - strip.C).Mag());
    min1 = (value < min1) ? value : min1;
    value = fabs((*p1 - strip.D).Mag());
    min1 = (value < min1) ? value : min1;

    value = fabs((*p2 - strip.A).Mag());
    min2 = (value < min2) ? value : min2;
    value = fabs((*p2 - strip.B).Mag());
    min2 = (value < min2) ? value : min2;
    value = fabs((*p2 - strip.C).Mag());
    min2 = (value < min2) ? value : min2;
    value = fabs((*p2 - strip.D).Mag());
    min2 = (value < min2) ? value : min2;
    // cout << "min1 = " << min1 << "; min2 = " << min2 << endl;
    return min1 + min2;
}
