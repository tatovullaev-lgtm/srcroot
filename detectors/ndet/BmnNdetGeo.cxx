#include "BmnNdetGeo.h"

#include <FairLogger.h>
#include <TGeoBBox.h>
#include <TGeoManager.h>

std::pair<BmnNdetGeo::BoxFace, BmnNdetGeo::Direction> BmnNdetGeo::GetEnteredFace(const TVector3& globalPos,
                                                                                 const TVector3& globalMom)
{
    if (!gGeoManager)
        return {BoxFace::Unknown, Direction::Unknown};

    TGeoNode* envelopeNode = gGeoManager->GetCurrentNode();
    if (!envelopeNode)
        return {BoxFace::Unknown, Direction::Unknown};

    auto* box = dynamic_cast<TGeoBBox*>(envelopeNode->GetVolume()->GetShape());
    if (!box)
        return {BoxFace::Unknown, Direction::Unknown};

    double gpos[3] = {globalPos.X(), globalPos.Y(), globalPos.Z()};
    double gdir[3] = {globalMom.X(), globalMom.Y(), globalMom.Z()};
    double lpos[3], ldir[3];

    gGeoManager->MasterToLocal(gpos, lpos);
    gGeoManager->MasterToLocalVect(gdir, ldir);

    TVector3 local(lpos[0], lpos[1], lpos[2]);
    TVector3 direc(ldir[0], ldir[1], ldir[2]);

    struct
    {
        BoxFace face;
        double coord;
        TVector3 normal;
    } checks[] = {{BoxFace::Xplus, local.X() - box->GetDX(), TVector3(1, 0, 0)},
                  {BoxFace::Xminus, local.X() + box->GetDX(), TVector3(-1, 0, 0)},
                  {BoxFace::Yplus, local.Y() - box->GetDY(), TVector3(0, 1, 0)},
                  {BoxFace::Yminus, local.Y() + box->GetDY(), TVector3(0, -1, 0)},
                  {BoxFace::Zplus, local.Z() - box->GetDZ(), TVector3(0, 0, 1)},
                  {BoxFace::Zminus, local.Z() + box->GetDZ(), TVector3(0, 0, -1)}};

    const double eps = 1e-3;
    for (const auto& check : checks) {
        if (std::abs(check.coord) < eps) {
            double dot = direc.Dot(check.normal);
            Direction d = (dot > 0) ? Direction::FromInside : (dot < 0) ? Direction::FromOutside : Direction::Unknown;
            return {check.face, d};
        }
    }

    return {BoxFace::Unknown, Direction::Unknown};
}
