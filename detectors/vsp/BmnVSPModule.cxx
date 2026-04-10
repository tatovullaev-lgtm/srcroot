#include "BmnVSPModule.h"

#include "TMath.h"
#include "TRandom.h"
#include "TF1.h"
#include "TH1F.h"

#include <iostream>

using namespace TMath;
using namespace std;

BmnVSPModule::BmnVSPModule() {

    Verbosity = true;

    ZStartModulePosition = 0.0;
    ModuleThickness = 0.03; //cm

    ModuleRotationAlgleDeg = 0.0;
    ModuleRotationAlgleRad = ModuleRotationAlgleDeg*TMath::DegToRad();
    ModuleRotationCenterX = 0.0;
    ModuleRotationCenterY = 0.0;

    XMinModule = 0.0;
    XMaxModule = 0.0;
    YMinModule = 0.0;
    YMaxModule = 0.0;
}

BmnVSPModule::BmnVSPModule(Double_t z_start_pos) {

    Verbosity = true;

    ZStartModulePosition = z_start_pos;
    ModuleThickness = 0.03; //cm

    ModuleRotationAlgleDeg = 0.0;
    ModuleRotationAlgleRad = ModuleRotationAlgleDeg*TMath::DegToRad();
    ModuleRotationCenterX = 0.0;
    ModuleRotationCenterY = 0.0;

    XMinModule = 0.0;
    XMaxModule = 0.0;
    YMinModule = 0.0;
    YMaxModule = 0.0;
}

BmnVSPModule::~BmnVSPModule() {

}

Double_t BmnVSPModule::GetZPositionRegistered() {
    return ZStartModulePosition + ModuleThickness*0.5;
}

Bool_t BmnVSPModule::SetModuleRotation(Double_t angleDeg, Double_t rot_center_x, Double_t rot_center_y) {
    ModuleRotationAlgleDeg = angleDeg;
    ModuleRotationAlgleRad = ModuleRotationAlgleDeg*TMath::DegToRad();
    ModuleRotationCenterX = rot_center_x;
    ModuleRotationCenterY = rot_center_y;
    return true;
}

void BmnVSPModule::AddStripLayer(BmnVSPLayer strip_layer) {
    StripLayers.push_back(strip_layer);

    DefineModuleBorders();
}

Bool_t BmnVSPModule::SetStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal) {
    if (layer_num >= 0 && layer_num < (Int_t)StripLayers.size())
    {
        return StripLayers[layer_num].SetStripSignal(strip_num, signal);
    }
    return false;
}

Bool_t BmnVSPModule::AddStripSignalInLayer(Int_t layer_num, Int_t strip_num, Double_t signal) {
    if( layer_num >= 0 && layer_num < (Int_t)StripLayers.size() ) {
        return StripLayers[layer_num].AddStripSignal(strip_num, signal);
    }
    return false;
}

Bool_t BmnVSPModule::SetStripMatchInLayer(Int_t layer_num, Int_t strip_num, BmnMatch mc_match) {
    if( layer_num >= 0 && layer_num < (Int_t)StripLayers.size() ) {
        return StripLayers[layer_num].SetStripMatch(strip_num, mc_match);
    }
    return false;
}

Bool_t BmnVSPModule::SetStripDigitNumberMatchInLayer(Int_t layer_num, Int_t strip_num, BmnMatch digit_num_match) {
    if( layer_num >= 0 && layer_num < (Int_t)StripLayers.size() ) {
        return StripLayers[layer_num].SetStripDigitNumberMatch(strip_num, digit_num_match);
    }
    return false;
}

Bool_t BmnVSPModule::SetStripSignalInLayerByZoneId(Int_t zone_id, Int_t strip_num, Double_t signal) {
    Int_t n_layers_the_same_id = 0;
    for(size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].SetStripSignal(strip_num, signal)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnVSPModule::AddStripSignalInLayerByZoneId(Int_t zone_id, Int_t strip_num, Double_t signal) {
    Int_t n_layers_the_same_id = 0;
    for(size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].AddStripSignal(strip_num, signal)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnVSPModule::SetStripMatchInLayerByZoneId(Int_t zone_id, Int_t strip_num, BmnMatch mc_match) {
    Int_t n_layers_the_same_id = 0;
    for(size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].SetStripMatch(strip_num, mc_match)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnVSPModule::AddStripMatchInLayerByZoneId(Int_t zone_id, Int_t strip_num, Double_t weight, Int_t mc_num) {
    Int_t n_layers_the_same_id = 0;
    for(size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].AddLinkToStripMatch(strip_num, weight, mc_num)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnVSPModule::SetStripDigitNumberMatchInLayerByZoneId(Int_t zone_id, Int_t strip_num, BmnMatch digit_num_match) {
    Int_t n_layers_the_same_id = 0;
    for(size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].SetStripDigitNumberMatch(strip_num, digit_num_match)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Bool_t BmnVSPModule::AddStripDigitNumberMatchInLayerByZoneId(Int_t zone_id, Int_t strip_num, Double_t weight, Int_t digit_num) {
    Int_t n_layers_the_same_id = 0;
    for(size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if(StripLayers[ilayer].AddLinkToStripDigitNumberMatch(strip_num, weight, digit_num)) n_layers_the_same_id++;
        }
    }
    return (Bool_t)n_layers_the_same_id;
}

Double_t BmnVSPModule::GetStripSignalInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < (Int_t)StripLayers.size() ) {
        return StripLayers[layer_num].GetStripSignal(strip_num);
    }
    return -1;
}

BmnMatch BmnVSPModule::GetStripMatchInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < (Int_t)StripLayers.size() ) {
        return StripLayers[layer_num].GetStripMatch(strip_num);
    }
    return BmnMatch();
}

BmnMatch BmnVSPModule::GetStripDigitNumberMatchInLayer(Int_t layer_num, Int_t strip_num) {
    if( layer_num >= 0 && layer_num < (Int_t)StripLayers.size() ) {
        return StripLayers[layer_num].GetStripDigitNumberMatch(strip_num);
    }
    return BmnMatch();
}

Int_t BmnVSPModule::GetFirstStripInZone(Int_t zone_id) {
    Int_t first_strip = 1E6;

    for(size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            Int_t first_strip_curr_layer = StripLayers[ilayer].GetFirstStripNumber();
            if(first_strip > first_strip_curr_layer) first_strip = first_strip_curr_layer;
        }
    }
    return first_strip;
}

Int_t BmnVSPModule::GetLastStripInZone(Int_t zone_id) {
    Int_t last_strip = 0;

    for(size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            Int_t last_strip_curr_layer = StripLayers[ilayer].GetLastStripNumber();
            if(last_strip < last_strip_curr_layer) last_strip = last_strip_curr_layer;
        }
    }
    return last_strip;
}

Double_t BmnVSPModule::GetStripSignalInZone(Int_t zone_id, Int_t strip_num) {
    for (size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer)
    {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if( strip_num >= StripLayers[ilayer].GetFirstStripNumber() && strip_num <= StripLayers[ilayer].GetLastStripNumber() ) {
                return StripLayers[ilayer].GetStripSignal(strip_num);
            }
        }
    }
    return -1;
}

BmnMatch BmnVSPModule::GetStripMatchInZone(Int_t zone_id, Int_t strip_num) {
    for (size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer)
    {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if( strip_num >= StripLayers[ilayer].GetFirstStripNumber() && strip_num <= StripLayers[ilayer].GetLastStripNumber() ) {
                return StripLayers[ilayer].GetStripMatch(strip_num);
            }
        }
    }
    return BmnMatch();
}

BmnMatch BmnVSPModule::GetStripDigitNumberMatchInZone(Int_t zone_id, Int_t strip_num) {
    for (size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer)
    {
        if( (StripLayers[ilayer].GetZoneID() == zone_id) ) {
            if( strip_num >= StripLayers[ilayer].GetFirstStripNumber() && strip_num <= StripLayers[ilayer].GetLastStripNumber() ) {
                return StripLayers[ilayer].GetStripDigitNumberMatch(strip_num);
            }
        }
    }
    return BmnMatch();
}

void BmnVSPModule::ResetModuleData() {
    for (size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer)
        StripLayers[ilayer].ResetLayer();

    DefineModuleBorders();

    ResetRealPoints();
    ResetIntersectionPoints();
    ResetPseudoIntersections();
}

Bool_t BmnVSPModule::IsPointInsideModule(Double_t x, Double_t y, Bool_t isLocal = false) {

    Double_t xloc = x;
    Double_t yloc = y;

    if(!isLocal) {
        xloc = ConvertRotatedToLocalX(x, y);
        yloc = ConvertRotatedToLocalY(x, y);
    }

    for(size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer) {
        if( StripLayers[ilayer].IsPointInsideStripLayer(xloc, yloc) ) return true;
    }
    return false;
}

Bool_t BmnVSPModule::IsPointInsideModule(Double_t x, Double_t y, Double_t z, Bool_t isLocal = false) {
    Double_t coord_eps = 0.01; //100 um
    if( ((z > ZStartModulePosition) || (fabs(z - ZStartModulePosition) < coord_eps)) &&
        ((z < (ZStartModulePosition + ModuleThickness)) || (fabs(z - (ZStartModulePosition + ModuleThickness))) < coord_eps) ) {
        if( IsPointInsideModule(x, y, isLocal) ) return true;
    }
    return false;
}

Bool_t BmnVSPModule::IsPointInsideZThickness(Double_t z) {
    Double_t coord_eps = 0.01; //100 um
    if( ((z > ZStartModulePosition) || (fabs(z - ZStartModulePosition) < coord_eps)) &&
        ((z < (ZStartModulePosition + ModuleThickness)) || (fabs(z - (ZStartModulePosition + ModuleThickness))) < coord_eps) ) {
        return true;
    }
    return false;
}

//Add a single point without smearing
Bool_t BmnVSPModule::AddRealPointSimple(Double_t x, Double_t y, Double_t z,
                                            Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    Double_t x_loc = ConvertRotatedToLocalX(x, y);
    Double_t y_loc = ConvertRotatedToLocalY(x, y);
    //Double_t z_loc = z;

    Double_t px_loc = ConvertRotatedToLocalX(px, py);
    Double_t py_loc = ConvertRotatedToLocalY(px, py);
    Double_t pz_loc = pz;

    //Effective XY position at the middle of the detector thickness (dz/2) -----

    //Distance from entry point (x,y,z) to the midde of the detector (along z-axis)
    Double_t z_dist_mid = ModuleThickness*0.5;
    Double_t vector_coeff_mid = fabs(z_dist_mid/pz_loc); //for middle point

    Double_t x_vec_from_in_to_mid = vector_coeff_mid*px_loc;
    Double_t y_vec_from_in_to_mid = vector_coeff_mid*py_loc;
    //Double_t z_vec_from_in_to_mid = vector_coeff_mid*pz_loc;

    //Middle point coordinates (x_mid, y_mid, z_mid)
    Double_t x_mid = x_loc + x_vec_from_in_to_mid;
    Double_t y_mid = y_loc + y_vec_from_in_to_mid;
    //Double_t z_mid = z_loc + z_vec_from_in_to_mid;
    //--------------------------------------------------------------------------

    if( IsPointInsideModule(x, y) ) {

        size_t n_strip_layers = StripLayers.size();

        //Add a signal to the strips layers ------------------------------------
        for(size_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            if( !StripLayers[ilayer].IsPointInsideStripLayer(x_mid, y_mid) ) continue;

            Double_t strip_pos = StripLayers[ilayer].ConvertPointToStripPosition(x_mid, y_mid);
            Int_t zone_id = StripLayers[ilayer].GetZoneID();

            AddStripSignalInLayerByZoneId(zone_id, (Int_t)strip_pos, signal);

            //strip match
            AddStripMatchInLayerByZoneId(zone_id, (Int_t)strip_pos, signal, refID);

        }
        //----------------------------------------------------------------------

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);
        RealPointsMC.push_back(refID);

        return true;
    }
    else {
        if(Verbosity) cerr << "WARNING: BmnVSPModule::AddRealPointSimple(): Point (" << x << " : " << y << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

//Add single point with Gaussian smearing
Bool_t BmnVSPModule::AddRealPointFullOne(Double_t x, Double_t y, Double_t z,
                               Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    Double_t xloc = ConvertRotatedToLocalX(x, y);
    Double_t yloc = ConvertRotatedToLocalY(x, y);

    if( IsPointInsideModule(x, y) ) {

        if(signal <= 0.0) signal = 1e-16;

        Double_t AvalancheRadius = 0.01; //cm

        Double_t radius = AvalancheRadius;
        if(radius <= 0.0) return false;

        //AZ-260322 gRandom->SetSeed(0);

        Int_t cycle_cnt = 0;
        while(true) {
            radius = gRandom->Gaus(AvalancheRadius, 0.005);
            if(radius > AvalancheRadius/2.0  && radius < AvalancheRadius*2.0 && radius > 0.0) break;
            cycle_cnt++;

            if(cycle_cnt > 5) {
                radius = AvalancheRadius;
                break;
            }
        }

        //Make strip cluster in each strip layer -------------------------------
        Int_t n_strip_layers = StripLayers.size();
        vector<StripCluster> cluster_layers(n_strip_layers, StripCluster());

        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            if( !StripLayers[ilayer].IsPointInsideStripLayer(xloc, yloc) ) continue;
            cluster_layers[ilayer] = MakeCluster(ilayer, xloc, yloc, signal, radius);
        }
        //----------------------------------------------------------------------

        //Add the correct clusters to the strip layers -------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            for(size_t ielement = 0; ielement < cluster_layers[ilayer].Strips.size(); ++ielement) {
                Int_t strip_num = cluster_layers[ilayer].Strips.at(ielement);
                Double_t strip_signal = cluster_layers[ilayer].Signals.at(ielement);
                Int_t zone_id = StripLayers[ilayer].GetZoneID();
                AddStripSignalInLayerByZoneId(zone_id, strip_num, strip_signal);
            }
        }
        //----------------------------------------------------------------------

        //Fill strip matches ---------------------------------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            for(size_t ielement = 0; ielement < cluster_layers[ilayer].Strips.size(); ++ielement) {
                Int_t strip_num = cluster_layers[ilayer].Strips.at(ielement);
                Double_t strip_signal = cluster_layers[ilayer].Signals.at(ielement);
                Int_t zone_id = StripLayers[ilayer].GetZoneID();
                AddStripMatchInLayerByZoneId(zone_id, strip_num, strip_signal, refID);
            }
        }
        //----------------------------------------------------------------------

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);
        RealPointsMC.push_back(refID);

        return true;
    }
    else {
        if(Verbosity) cerr << "WARNING: BmnVSPModule::AddRealPointFullOne(): Point (" << x << " : " << y << " : " << z << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

//Add single point with Gaussian smearing + taking into account particle track incline
Bool_t BmnVSPModule::AddRealPointFullOne_WithIncline(Double_t x, Double_t y, Double_t z,
                               Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    Double_t x_loc = ConvertRotatedToLocalX(x, y);
    Double_t y_loc = ConvertRotatedToLocalY(x, y);
    //Double_t z_loc = z;

    Double_t px_loc = ConvertRotatedToLocalX(px, py);
    Double_t py_loc = ConvertRotatedToLocalY(px, py);
    Double_t pz_loc = pz;

    //Effective XY position at the middle of the detector thickness (dz/2) -----

    //Distance from entry point (x,y,z) to the midde of the detector (along z-axis)
    Double_t z_dist_mid = ModuleThickness*0.5;
    Double_t vector_coeff_mid = fabs(z_dist_mid/pz_loc); //for middle point

    Double_t x_vec_from_in_to_mid = vector_coeff_mid*px_loc;
    Double_t y_vec_from_in_to_mid = vector_coeff_mid*py_loc;
    //Double_t z_vec_from_in_to_mid = vector_coeff_mid*pz_loc;

    //Middle point coordinates (x_mid, y_mid, z_mid)
    Double_t x_mid = x_loc + x_vec_from_in_to_mid;
    Double_t y_mid = y_loc + y_vec_from_in_to_mid;
    //Double_t z_mid = z_loc + z_vec_from_in_to_mid;
    //--------------------------------------------------------------------------

    if( IsPointInsideModule(x, y) ) {

        if(signal <= 0.0) signal = 1e-16;

        Double_t AvalancheRadius = 0.015; //cm

        Double_t radius = AvalancheRadius;
        if(radius <= 0.0) return false;

        //AZ-260322 gRandom->SetSeed(0);

        Int_t cycle_cnt = 0;
        while(true) {
            radius = gRandom->Gaus(AvalancheRadius, 0.005);
            if(radius > AvalancheRadius/2.0  && radius < AvalancheRadius*2.0 && radius > 0.0) break;
            cycle_cnt++;

            if(cycle_cnt > 5) {
                radius = AvalancheRadius;
                break;
            }
        }

        //Make strip cluster in each strip layer -------------------------------
        Int_t n_strip_layers = StripLayers.size();
        vector<StripCluster> cluster_layers(n_strip_layers, StripCluster());

        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            if( !StripLayers[ilayer].IsPointInsideStripLayer(x_mid, y_mid) ) continue;
            cluster_layers[ilayer] = MakeCluster(ilayer, x_mid, y_mid, signal, radius);
        }
        //----------------------------------------------------------------------

        //Add the correct clusters to the strip layers -------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            for(size_t ielement = 0; ielement < cluster_layers[ilayer].Strips.size(); ++ielement) {
                Int_t strip_num = cluster_layers[ilayer].Strips.at(ielement);
                Double_t strip_signal = cluster_layers[ilayer].Signals.at(ielement);
                Int_t zone_id = StripLayers[ilayer].GetZoneID();
                AddStripSignalInLayerByZoneId(zone_id, strip_num, strip_signal);
            }
        }
        //----------------------------------------------------------------------

        //Fill strip matches ---------------------------------------------------
        for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
            for(size_t ielement = 0; ielement < cluster_layers[ilayer].Strips.size(); ++ielement) {
                Int_t strip_num = cluster_layers[ilayer].Strips.at(ielement);
                Double_t strip_signal = cluster_layers[ilayer].Signals.at(ielement);
                Int_t zone_id = StripLayers[ilayer].GetZoneID();
                AddStripMatchInLayerByZoneId(zone_id, strip_num, strip_signal, refID);
            }
        }
        //----------------------------------------------------------------------

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);
        RealPointsMC.push_back(refID);

        return true;
    }
    else {
        if(Verbosity) cerr << "WARNING: BmnVSPModule::AddRealPointFullOne(): Point (" << x << " : " << y << " : " << z << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

Bool_t BmnVSPModule::AddRealPointFull(Double_t x, Double_t y, Double_t z,
                               Double_t px, Double_t py, Double_t pz, Double_t signal, Int_t refID) {

    //Convert coordinates if rotation around Z axis is used ---------------------
    Double_t x_loc = ConvertRotatedToLocalX(x, y);
    Double_t y_loc = ConvertRotatedToLocalY(x, y);
    Double_t z_loc = z;

    Double_t px_loc = ConvertRotatedToLocalX(px, py);
    Double_t py_loc = ConvertRotatedToLocalY(px, py);
    Double_t pz_loc = pz;
    //--------------------------------------------------------------------------

    //Parameters of the signal simulation of one charge carrier (electron or hole)
    //Double_t chargedCarrierCloudRadius = 0.005; //Signal radius (not diameter!) for ONE STEP (cm) ~50 micron (for large step)
    Double_t chargedCarrierCloudRadius = gRandom->Uniform(0.0001, 0.0005); //Signal radius (not diameter!) for ONE STEP (cm) 1-5 micron (for small step)
    Double_t step = 0.0010; //simulation step (is not mean collision dist. for semiconductors!)
    //--------------------------------------------------------------------------

    if( IsPointInsideModule(x_loc, y_loc, true) ) {

        //Distance from entry point (x,y,z) to exit point (along z-axis)
        Double_t z_distance_from_in_to_out;

        //Condition: track direction along z-axis
        if(pz_loc > 0.0) {
            z_distance_from_in_to_out = ModuleThickness - (z_loc-ZStartModulePosition);
        }
        else {
            z_distance_from_in_to_out = z_loc - ZStartModulePosition;
        }

        //Condition: track distance along z-axis must be not zero
        if(z_distance_from_in_to_out <= 0.0) {
            if(Verbosity) cerr << "WARNING: BmnVSPModule::AddRealPointFull(): Point (" << x << " : " << y << " : " << z << ") has a track with an incorrect length\n";
            return false;
        }

        //Scale coefficient of the track vector (the path from entry point to exit point)
        Double_t vector_coeff = fabs(z_distance_from_in_to_out/pz_loc);

        //Components of the track vector
        Double_t x_vec_from_in_to_out = vector_coeff*px_loc;
        Double_t y_vec_from_in_to_out = vector_coeff*py_loc;
        Double_t z_vec_from_in_to_out = vector_coeff*pz_loc;

        //Exit point coordinates (x_out, y_out, z_out)
        Double_t x_out = x_loc + x_vec_from_in_to_out;
        Double_t y_out = y_loc + y_vec_from_in_to_out;
        //Double_t z_out = z_loc + z_vec_from_in_to_out;

        //Check if the exit point is outside the module then calculate new x_out, y_out, z_out values
        //Condition: x-coordinate of the exit point is inside the module
        if(x_out < XMinModule || x_out > XMaxModule ) {
            if(x_out < XMinModule ) x_out = XMinModule;
            if(x_out > XMaxModule ) x_out = XMaxModule;

            x_vec_from_in_to_out = x_out - x_loc;
            vector_coeff = x_vec_from_in_to_out/px_loc;

            y_vec_from_in_to_out= vector_coeff*py_loc;
            z_vec_from_in_to_out = vector_coeff*pz_loc;

            y_out = y_loc + y_vec_from_in_to_out;
            //z_out = z_loc + z_vec_from_in_to_out;
        }
        //Condition: y-coordinate of the exit point is inside the module
        if(y_out < YMinModule  || y_out > YMaxModule ) {
            if(y_out < YMinModule ) y_out = YMinModule;
            if(y_out > YMaxModule ) y_out = YMaxModule;

            y_vec_from_in_to_out = y_out - y_loc;
            vector_coeff = y_vec_from_in_to_out/py_loc;

            x_vec_from_in_to_out= vector_coeff*px_loc;
            z_vec_from_in_to_out = vector_coeff*pz_loc;

            x_out = x_loc + x_vec_from_in_to_out;
            //z_out = z_loc + z_vec_from_in_to_out;
        }
        //----------------------------------------------------------------------

        //Common track length (Fix: a square root!)
        Double_t track_length = std::sqrt( (x_vec_from_in_to_out)*(x_vec_from_in_to_out) + (y_vec_from_in_to_out)*(y_vec_from_in_to_out) + (z_vec_from_in_to_out)*(z_vec_from_in_to_out) );

        Double_t current_length = 0.0;  //traversed length (counter)
        Double_t current_length_ratio = 0.0; //ratio of the traversed length to common track length (counter)

        Int_t collisions_cnt = 0; //total collision counter
        Double_t current_step = 0.0; //current distance between two collision points

        //Collection of collision points
        std::vector<CollPoint> collision_points;

        while(current_length < track_length) {

            //current_step = rand.Exp(step);
            current_step = step;
            current_length += current_step;

            if(current_length > track_length) break;

            current_length_ratio = current_length/track_length;

            Double_t current_x = x_loc + current_length_ratio*x_vec_from_in_to_out;
            Double_t current_y = y_loc + current_length_ratio*y_vec_from_in_to_out;
            Double_t current_z = z_loc + current_length_ratio*z_vec_from_in_to_out;

            collision_points.push_back(CollPoint(current_x, current_y, current_z));
            collisions_cnt++;
        }
        //----------------------------------------------------------------------

        if(signal <= 0.0) signal = 0;

        Int_t n_collisions = collision_points.size();
        Double_t signal_per_step = signal / n_collisions;

        for(Int_t icollpoint = 0; icollpoint < n_collisions; ++icollpoint) {

            //generate the current signal radius on strips for the current charged carrier
            Double_t current_radius = chargedCarrierCloudRadius;
            if(chargedCarrierCloudRadius <= 0.0) {
                if(Verbosity) cerr << "WARNING: BmnVSPModule::AddRealPointFull(): incorrect chargedCarrierCloudRadius!\n";
                return false;
            }

            Double_t current_x = collision_points[icollpoint].x;
            Double_t current_y = collision_points[icollpoint].y;

            Int_t n_strip_layers = GetNStripLayers();

            for(Int_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
                if( !StripLayers[ilayer].IsPointInsideStripLayer(current_x, current_y) ) continue;

                Double_t pitch = StripLayers[ilayer].GetPitch();
                Double_t radius_in_strips = current_radius/pitch;
                Int_t zone_id = StripLayers[ilayer].GetZoneID();

                Int_t first_strip_in_layer = StripLayers[ilayer].GetFirstStripNumber();
                Int_t last_strip_in_layer = StripLayers[ilayer].GetLastStripNumber();


                Double_t central_strip_pos = StripLayers[ilayer].ConvertPointToStripPosition(current_x, current_y);
                Double_t left_strip_pos = central_strip_pos - radius_in_strips;
                Double_t right_strip_pos = central_strip_pos + radius_in_strips;

                //cout << " layer: " << ilayer << "\n";
                //cout << "   current_x = " << current_x << "\n";
                //cout << "   current_y = " << current_y << "\n";
                //cout << "   central_strip_pos = " << central_strip_pos << "\n";

                Double_t central_strip_signal = signal_per_step;
                Double_t left_strip_signal = 0.0;
                Double_t right_strip_signal = 0.0;

                Double_t central_strip_fraction = 1.0;
                Double_t left_strip_fraction = 0.0;
                Double_t right_strip_fraction = 0.0;

                //calculate signal fraction for the left neighbour strip
                if( (Int_t)left_strip_pos != (Int_t)central_strip_pos ) {
                    if( left_strip_pos >= first_strip_in_layer ) {
                        left_strip_fraction = (Int_t)central_strip_pos - left_strip_pos;
                        if(left_strip_fraction > 1.0) left_strip_fraction = 1.0;
                    }
                }
                //calculate signal fraction for the right neighbour strip
                if( (Int_t)right_strip_pos != (Int_t)central_strip_pos ) {
                    if( right_strip_pos < (last_strip_in_layer+1) ) {
                        right_strip_fraction = right_strip_pos - (Int_t)(central_strip_pos+1);
                        if(right_strip_fraction > 1.0) right_strip_fraction = 1.0;
                    }
                }

                //signal for the triplet strips (one central and two possible neighbour strips)
                central_strip_signal = signal_per_step*central_strip_fraction / (central_strip_fraction + left_strip_fraction + right_strip_fraction);
                left_strip_signal = signal_per_step*left_strip_fraction / (central_strip_fraction + left_strip_fraction + right_strip_fraction);
                right_strip_signal = signal_per_step*right_strip_fraction / (central_strip_fraction + left_strip_fraction + right_strip_fraction);

                AddStripSignalInLayerByZoneId(zone_id, (Int_t)central_strip_pos, central_strip_signal);
                if(left_strip_signal > 0.0) AddStripSignalInLayerByZoneId(zone_id, (Int_t)left_strip_pos, left_strip_signal);
                if(right_strip_signal > 0.0) AddStripSignalInLayerByZoneId(zone_id, (Int_t)right_strip_pos, right_strip_signal);

                //strip match
                AddStripMatchInLayerByZoneId(zone_id, (Int_t)central_strip_pos, central_strip_signal, refID);
                if(left_strip_signal > 0.0) AddStripMatchInLayerByZoneId(zone_id, (Int_t)left_strip_pos, left_strip_signal, refID);
                if(right_strip_signal > 0.0) AddStripMatchInLayerByZoneId(zone_id, (Int_t)right_strip_pos, right_strip_signal, refID);
            }
        }

        RealPointsX.push_back(x);
        RealPointsY.push_back(y);
        RealPointsMC.push_back(refID);

        return true;
    }
    else {
        if(Verbosity) cerr << "WARNING: BmnVSPModule::AddRealPointFull(): Point (" << x << " : " << y << " : " << z << ") is out of the readout plane or inside a dead zone\n";
        return false;
    }
}

StripCluster BmnVSPModule::MakeCluster(Int_t layer_num, Double_t xcoord, Double_t ycoord, Double_t signal, Double_t radius) {

    Double_t ClusterDistortion = 0.0; //signal variation (0.1 is 10%)

    Double_t Gain = 1.0;

    StripCluster cluster;

    if(radius <= 0.0) return cluster;

    Double_t Pitch = StripLayers[layer_num].GetPitch();

    Double_t RadiusInZones = radius/Pitch;
    Double_t Sigma = RadiusInZones/3.33;

    TF1 gausF("gausF", "gaus", -4*Sigma, 4*Sigma);
    gausF.SetParameter(0, 1.0); // constant (altitude)
    gausF.SetParameter(1, 0.0); // mean (center position)
    gausF.SetParameter(2, Sigma); //sigma

    Double_t SCluster = gausF.Integral(-4*Sigma, 4*Sigma); //square of the one side distribution (more exactly)

    TRandom rand(0);
    Double_t var_level = ClusterDistortion; //signal variation (0.1 is 10%)

    Int_t NStripsInLayer = StripLayers[layer_num].GetNStrips();
    Int_t FirstStripInLayer = StripLayers[layer_num].GetFirstStripNumber();
    Int_t LastStripInLayer = StripLayers[layer_num].GetLastStripNumber();

    if(NStripsInLayer == 0) return cluster;

    Double_t CenterZonePos = StripLayers[layer_num].ConvertPointToStripPosition(xcoord, ycoord);

    if( CenterZonePos < FirstStripInLayer || CenterZonePos >= LastStripInLayer+1 ) return cluster;


    gausF.SetParameter(1, CenterZonePos);
    Double_t total_signal = 0.0;

    Double_t LeftZonePos = CenterZonePos - RadiusInZones;
    Double_t RightZonePos = CenterZonePos + RadiusInZones;
    Double_t OutLeftBorder = 0.0;
    Double_t OutRightBorder = 0.0;

    if(LeftZonePos < FirstStripInLayer)  {
        OutLeftBorder = LeftZonePos;
        LeftZonePos = FirstStripInLayer;
    }
    if(RightZonePos < FirstStripInLayer) {
        OutRightBorder = RightZonePos;
        RightZonePos = FirstStripInLayer;
    }
    if(LeftZonePos >= LastStripInLayer+1) {
        OutLeftBorder = LeftZonePos;
        LeftZonePos = LastStripInLayer+1 - 0.001;
    }
    if(RightZonePos >= LastStripInLayer+1) {
        OutRightBorder = RightZonePos;
        RightZonePos = LastStripInLayer+1 - 0.001;
    }

    Double_t h = 0.0;
    Double_t dist = 0.0;

    //avalanche is inside of the one strip
    if((Int_t)LeftZonePos == (Int_t)RightZonePos) {

        Int_t NumCurrentZone = (Int_t) LeftZonePos;

        h = RightZonePos - LeftZonePos;
        if(h < 0.0) h = 0.0;

        Double_t xp = LeftZonePos + dist;
        Double_t S  = gausF.Integral(xp, xp+h);
        Double_t Energy = (signal*Gain)*(S/SCluster);
        Energy += rand.Gaus(0.0, var_level*Energy);
        if(Energy < 0.0) Energy = 0.0;

        if(NumCurrentZone >= FirstStripInLayer && NumCurrentZone < LastStripInLayer+1 && Energy > 0.0) {
            cluster.AddStrip(NumCurrentZone, Energy);
            total_signal += Energy;
        }

        dist += h;

    }
    else {
        //left border strip
        Int_t NumCurrentZone = (Int_t) LeftZonePos;

        h = ((Int_t)LeftZonePos + 1) - LeftZonePos;
        if(h < 0.0) h = 0.0;

        Double_t xp = LeftZonePos + dist;
        Double_t S  = gausF.Integral(xp, xp+h);
        Double_t Energy = (signal*Gain)*(S/SCluster);
        Energy += rand.Gaus(0.0, var_level*Energy);
        if(Energy < 0.0) Energy = 0.0;

        if(NumCurrentZone >= FirstStripInLayer && NumCurrentZone < LastStripInLayer+1 && Energy > 0.0) {
            cluster.AddStrip(NumCurrentZone, Energy);
            total_signal += Energy;
        }

        dist += h;

        //inner strips
        for(Int_t i = (Int_t)LeftZonePos + 1; i < (Int_t)RightZonePos; ++i) {

            NumCurrentZone = i;

            h = 1.0;

            xp = LeftZonePos + dist;
            S  = gausF.Integral(xp, xp+h);
            Energy = (signal*Gain)*(S/SCluster);
            Energy += rand.Gaus(0.0, var_level*Energy);
            if(Energy < 0.0) Energy = 0.0;

            if(NumCurrentZone >= FirstStripInLayer && NumCurrentZone < LastStripInLayer+1 && Energy > 0.0) {
                cluster.AddStrip(NumCurrentZone, Energy);
                total_signal += Energy;
            }

            dist += h;
        }
        //right border strip
        NumCurrentZone = (Int_t)RightZonePos;

        h = RightZonePos - (Int_t)RightZonePos;
        if(h < 0.0) h = 0.0;

        xp = LeftZonePos + dist;
        S  = gausF.Integral(xp, xp+h);
        Energy = (signal*Gain)*(S/SCluster);
        Energy += rand.Gaus(0.0, var_level*Energy);
        if(Energy < 0.0) Energy = 0.0;

        if(NumCurrentZone >= FirstStripInLayer && NumCurrentZone < LastStripInLayer+1 && Energy > 0.0) {
            cluster.AddStrip(NumCurrentZone, Energy);
            total_signal += Energy;
        }

        dist += h;
    }

    if (cluster.GetClusterSize() <= 0) {
        return cluster;
    }

    //find the mean value of the avalanche position (fitting by gaus function)
    Double_t mean_fit_pos = 0.0;

    Int_t NOutLeftBins = 0;
    Int_t NOutRightBins = 0;
    if(OutLeftBorder != 0.0) {
        NOutLeftBins = (Int_t)(fabs(OutLeftBorder)) + 1;
    }
    if(OutRightBorder != 0.0) {
        NOutRightBins = (Int_t)(OutRightBorder - RightZonePos) + 1;
    }

    Int_t begin_strip_num = cluster.Strips.at(0);
    Int_t last_strip_num = cluster.Strips.at(cluster.GetClusterSize()-1);
    Int_t nstrips = last_strip_num - begin_strip_num + 1;

    TH1F hist("hist_for_fit", "hist_for_fit", nstrips+NOutLeftBins+NOutRightBins, begin_strip_num-NOutLeftBins, last_strip_num+1+NOutRightBins);
    Int_t hist_index = 0;

    for(Int_t i = 0; i < cluster.GetClusterSize(); ++i) {
        Double_t value = cluster.Signals.at(i);
        hist.SetBinContent(hist_index+1+NOutLeftBins, value);
        hist_index++;
    }

    //on the left border
    if(NOutLeftBins > 0.0) {
        Double_t first = OutLeftBorder;
        Double_t last = (Int_t)OutLeftBorder;
        Double_t S = gausF.Integral(first, last);
        Double_t Energy = (signal*Gain)*(S/SCluster);
        Energy += rand.Gaus(0.0, var_level*Energy);
        if(Energy < 0.0) Energy = 0.0;
        Double_t value = Energy;
        hist.SetBinContent(1, value);
        dist = 0.0;

        for(Int_t i = 1; i < NOutLeftBins; ++i) {
            h = 1.0;
            first = (Int_t)OutLeftBorder+dist;
            last = first + h;
            S = gausF.Integral(first, last);
            Energy = (signal*Gain)*(S/SCluster);
            Energy += rand.Gaus(0.0, var_level*Energy);
            if(Energy < 0.0) Energy = 0.0;
            value = Energy;
            hist.SetBinContent(1+i, value);
            dist += h;
        }
    }

    //on the right border
    if(NOutRightBins > 0.0) {
        dist = 0.0;
        for(Int_t i = hist_index; i < hist_index+NOutRightBins-1; ++i) {
            h = 1.0;
            Double_t first = NStripsInLayer+dist;
            Double_t last = first + h;
            Double_t S = gausF.Integral(first, last);
            Double_t Energy = (signal*Gain)*(S/SCluster);
            Energy += rand.Gaus(0.0, var_level*Energy);
            if(Energy < 0.0) Energy = 0.0;
            Double_t value = Energy;
            hist.SetBinContent(1+i, value);
            dist += h;
        }

        Double_t first = (Int_t)OutRightBorder;
        Double_t last = first + (OutRightBorder - (Int_t)OutRightBorder);
        Double_t S = gausF.Integral(first, last);
        Double_t Energy = (signal*Gain)*(S/SCluster);
        Energy += rand.Gaus(0.0, var_level*Energy);
        if(Energy < 0.0) Energy = 0.0;
        Double_t value = Energy;
        hist.SetBinContent(hist_index+NOutRightBins, value);
    }

    TF1* gausFitFunction = 0;
        TString fit_params = "WQ0";

    #ifdef DRAW_REAL_CLUSTER_HISTOGRAMS
        fit_params = "WQ";
    #endif

    if(nstrips > 1) {
        hist.Fit("gaus", fit_params); //Q - quit mode (without information on the screen); 0 - not draw
        gausFitFunction = hist.GetFunction("gaus");
        if(gausFitFunction) {
            mean_fit_pos = gausFitFunction->GetParameter(1);
        }
        else {
            mean_fit_pos = hist.GetMean();
        }
    }
    else {
        mean_fit_pos = hist.GetMean();
    }

    cluster.MeanPosition = mean_fit_pos;
    cluster.TotalSignal = total_signal;
    cluster.PositionResidual = mean_fit_pos - CenterZonePos; //residual between mean and origin positions (lower cluster): residual = finded(current) - orig(average)

    cluster.IsCorrect = true; // set correct status of the cluster
    return cluster;
}

void BmnVSPModule::CalculateStripHitIntersectionPoints() {

    ResetIntersectionPoints();
    ResetPseudoIntersections();

    size_t n_strip_layers = StripLayers.size();

    // Find strip clusters and hits in the strip layers -------------------------
    for (size_t ilayer = 0; ilayer < n_strip_layers; ++ilayer)
        StripLayers[ilayer].FindClustersAndStripHits();

    map<Int_t, Bool_t> UsageStatus_LowerClusters;
    map<Int_t, Bool_t> UsageStatus_UpperClusters;

    //Saving all unique id marks of clusters in a module
    for (size_t ilayer = 0; ilayer < n_strip_layers; ++ilayer)
    {
        for (Int_t i_strip_hit = 0; i_strip_hit < StripLayers[ilayer].GetNStripHits(); ++i_strip_hit)
        {
            StripCluster cluster = StripLayers[ilayer].GetStripClusters()[i_strip_hit];
            if (cluster.GetType() == LowerStripLayer)
                UsageStatus_LowerClusters[cluster.GetUniqueID()] = false;
            if (cluster.GetType() == UpperStripLayer)
                UsageStatus_UpperClusters[cluster.GetUniqueID()] = false;
        }
    }

    for(size_t ilayer = 0; ilayer < n_strip_layers; ++ilayer) {
        for(size_t jlayer = ilayer+1; jlayer < n_strip_layers; ++jlayer) {
            //cout << "(i-layer : j-layer) = ( " << ilayer << " : " << jlayer << " )\n";

            if(StripLayers[ilayer].GetType() == StripLayers[jlayer].GetType()) continue;

            Double_t iAngleRad = StripLayers[ilayer].GetAngleRad();
            Double_t jAngleRad = StripLayers[jlayer].GetAngleRad();

            if( Abs(iAngleRad - jAngleRad) < 0.01 ) continue;

            Double_t i_PiOver2MinusAngleRad = PiOver2() - iAngleRad;
            Double_t j_PiOver2MinusAngleRad = PiOver2() - jAngleRad;

            for(Int_t i_strip_hit = 0; i_strip_hit < StripLayers[ilayer].GetNStripHits(); ++i_strip_hit) {
                for(Int_t j_strip_hit = 0; j_strip_hit < StripLayers[jlayer].GetNStripHits(); ++j_strip_hit) {

                    Double_t iStripHitPos = StripLayers[ilayer].GetStripHitPos(i_strip_hit);
                    Double_t jStripHitPos = StripLayers[jlayer].GetStripHitPos(j_strip_hit);

                    Double_t iStripHitError = StripLayers[ilayer].GetStripHitError(i_strip_hit);
                    Double_t jStripHitError = StripLayers[jlayer].GetStripHitError(j_strip_hit);

                    Double_t xcoord;
                    Double_t ycoord;

                    if( Abs(iAngleRad) >= 0.01 && Abs(jAngleRad) >= 0.01 ) {

                        Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
                        Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);

                        xcoord = (jB - iB)/(Tan(i_PiOver2MinusAngleRad) - Tan(j_PiOver2MinusAngleRad));
                        ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
                    }
                    else
                    {
                        if( Abs(iAngleRad) < 0.01 ) {

                            if(StripLayers[ilayer].GetStripNumberingOrder() == LeftToRight ) {
                                xcoord = StripLayers[ilayer].GetXLeftStripBorderPoint() + (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
                            }
                            else {
                                xcoord = StripLayers[ilayer].GetXRightStripBorderPoint() - (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
                            }

                            Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);
                            ycoord = Tan(j_PiOver2MinusAngleRad)*xcoord + jB;
                        }

                        if( Abs(jAngleRad) < 0.01 ) {

                            if(StripLayers[jlayer].GetStripNumberingOrder() == LeftToRight ) {
                                xcoord = StripLayers[jlayer].GetXLeftStripBorderPoint() + (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
                            }
                            else {
                                xcoord = StripLayers[jlayer].GetXRightStripBorderPoint() - (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
                            }

                            Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
                            ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
                        }
                    }

                    //check: a point belongs to both strip layers together
                    if( StripLayers[ilayer].IsPointInsideStripLayer(xcoord, ycoord) && StripLayers[jlayer].IsPointInsideStripLayer(xcoord, ycoord) ) {

                        Double_t xrot = ConvertLocalToRotatedX(xcoord, ycoord);
                        Double_t yrot = ConvertLocalToRotatedY(xcoord, ycoord);

                        IntersectionPointsX.push_back(xrot);
                        IntersectionPointsY.push_back(yrot);

                        //Intersection (x,y)-errors from the strip-errors ------
                        Double_t AngleIntersecRad = Abs(iAngleRad - jAngleRad);

                        //a rhomb zone is the intersection of the strip errors
                        Double_t RhombSide1 = (iStripHitError*StripLayers[ilayer].GetPitch())/Sin(AngleIntersecRad);
                        Double_t RhombSide2 = (jStripHitError*StripLayers[jlayer].GetPitch())/Sin(AngleIntersecRad);

                        //x-strip error
                        Double_t xerr1 = Sin(Abs(jAngleRad))*RhombSide1;
                        Double_t xerr2 = Sin(Abs(iAngleRad))*RhombSide2;
                        Double_t xcoord_err = xerr1 + xerr2;

                        //y-strip error
                        Double_t yerr1 = Cos(Abs(jAngleRad))*RhombSide1;
                        Double_t yerr2 = Cos(Abs(iAngleRad))*RhombSide2;
                        Double_t ycoord_err = yerr1 + yerr2;

                        IntersectionPointsXErrors.push_back(xcoord_err);
                        IntersectionPointsYErrors.push_back(ycoord_err);
                        //------------------------------------------------------

                        //intersection MC-matching -----------------------------
                        BmnMatch iStripMatch = StripLayers[ilayer].GetStripMatch((Int_t)iStripHitPos);
                        BmnMatch jStripMatch = StripLayers[jlayer].GetStripMatch((Int_t)jStripHitPos);

                        BmnMatch intersection_match;
                        intersection_match.AddLink(iStripMatch);
                        intersection_match.AddLink(jStripMatch);

                        IntersectionPointMatches.push_back(intersection_match);
                        //------------------------------------------------------

                        //intersection digit number matching -------------------
                        BmnMatch iStripDigitNumberMatch = StripLayers[ilayer].GetStripDigitNumberMatch((Int_t)iStripHitPos);
                        BmnMatch jStripDigitNumberMatch = StripLayers[jlayer].GetStripDigitNumberMatch((Int_t)jStripHitPos);

                        BmnMatch intersection_digit_num_match;
                        intersection_digit_num_match.AddLink(iStripDigitNumberMatch);
                        intersection_digit_num_match.AddLink(jStripDigitNumberMatch);

                        IntersectionPointDigitNumberMatches.push_back(intersection_digit_num_match);
                        //------------------------------------------------------

                        //Additional information about the intersection --------
                            //cluster size (number strips) in both strip layers for each intersection
                            Int_t iLayerClusterSize = StripLayers[ilayer].GetStripHitClusterSize(i_strip_hit);
                            Int_t jLayerClusterSize = StripLayers[jlayer].GetStripHitClusterSize(j_strip_hit);

                            //strip position in both strip layers for each intersection
                            Double_t iLayerStripPosition = StripLayers[ilayer].ConvertPointToStripPosition(xcoord, ycoord);
                            Double_t jLayerStripPosition = StripLayers[jlayer].ConvertPointToStripPosition(xcoord, ycoord);

                            if(StripLayers[ilayer].GetType() == LowerStripLayer) {
                                IntersectionPoints_LowerLayerClusterSize.push_back(iLayerClusterSize);
                                IntersectionPoints_UpperLayerClusterSize.push_back(jLayerClusterSize);
                                IntersectionPoints_LowerLayerStripPosition.push_back(iLayerStripPosition);
                                IntersectionPoints_UpperLayerStripPosition.push_back(jLayerStripPosition);
                                IntersectionPoints_LowerLayerStripTotalSignal.push_back(StripLayers[ilayer].GetStripHitTotalSignal(i_strip_hit));
                                IntersectionPoints_UpperLayerStripTotalSignal.push_back(StripLayers[jlayer].GetStripHitTotalSignal(j_strip_hit));
                                LowerClusters.push_back(StripLayers[ilayer].GetStripClusters().at(i_strip_hit));
                                UpperClusters.push_back(StripLayers[jlayer].GetStripClusters().at(j_strip_hit));
                                UsageStatus_LowerClusters[StripLayers[ilayer].GetStripClusters()[i_strip_hit].GetUniqueID()] = true;
                                UsageStatus_UpperClusters[StripLayers[jlayer].GetStripClusters()[j_strip_hit].GetUniqueID()] = true;
                            }
                            else {
                                IntersectionPoints_LowerLayerClusterSize.push_back(jLayerClusterSize);
                                IntersectionPoints_UpperLayerClusterSize.push_back(iLayerClusterSize);
                                IntersectionPoints_LowerLayerStripPosition.push_back(jLayerStripPosition);
                                IntersectionPoints_UpperLayerStripPosition.push_back(iLayerStripPosition);
                                IntersectionPoints_LowerLayerStripTotalSignal.push_back(StripLayers[jlayer].GetStripHitTotalSignal(j_strip_hit));
                                IntersectionPoints_UpperLayerStripTotalSignal.push_back(StripLayers[ilayer].GetStripHitTotalSignal(i_strip_hit));
                                LowerClusters.push_back(StripLayers[jlayer].GetStripClusters().at(j_strip_hit));
                                UpperClusters.push_back(StripLayers[ilayer].GetStripClusters().at(i_strip_hit));
                                UsageStatus_LowerClusters[StripLayers[jlayer].GetStripClusters()[j_strip_hit].GetUniqueID()] = true;
                                UsageStatus_UpperClusters[StripLayers[ilayer].GetStripClusters()[i_strip_hit].GetUniqueID()] = true;
                            }
                        //------------------------------------------------------
                    }
                }
            }
        }
    }

    //for(auto it : UsageStatus_UpperClusters) {
        //if(it.second == false) cout << " upper_cluster[" << it.first << "] = " << it.second << "\n";
    //}

    for(auto it : UsageStatus_LowerClusters)
    {
        //if(it.second == false) cout << " lower_cluster[" << it.first << "] = " << it.second << "\n";

        if(it.second == false) {

            for (size_t ilayer = 0; ilayer < n_strip_layers; ++ilayer)
            {
                if(StripLayers[ilayer].GetType() != LowerStripLayer) continue;

                for (size_t icluster = 0; icluster < StripLayers[ilayer].GetStripClusters().size(); ++icluster)
                {
                    StripCluster cluster = StripLayers[ilayer].GetStripClusters()[icluster];

                    if ((Int_t)cluster.GetUniqueID() != it.first) continue;

                    Double_t xcoord;
                    Double_t ycoord;

                    if(StripLayers[ilayer].GetStripNumberingOrder() == LeftToRight ) {
                        xcoord = StripLayers[ilayer].GetXLeftStripBorderPoint() + cluster.MeanPosition*StripLayers[ilayer].GetPitch();
                    }
                    else {
                        xcoord = StripLayers[ilayer].GetXRightStripBorderPoint() - cluster.MeanPosition*StripLayers[ilayer].GetPitch();
                    }

                    ycoord = (StripLayers[ilayer].GetYMinLayer() + StripLayers[ilayer].GetYMaxLayer())*0.5;

                    Double_t xerr = cluster.Error*StripLayers[ilayer].GetPitch();
                    Double_t yerr = StripLayers[ilayer].GetYSize()*0.5;

                    //cout << " low_cluster[" << icluster << "]:\n";
                    //cout << "  ID = " << cluster.GetUniqueID() << "\n";
                    //cout << "  xcoord = " << xcoord << "\n";
                    //cout << "   xmin = " << StripLayers[ilayer].GetXMinLayer() << ", xmax = " << StripLayers[ilayer].GetXMaxLayer() << "\n";
                    //cout << "  ycoord = " << ycoord << "\n";
                    //cout << "   ymin = " << StripLayers[ilayer].GetYMinLayer() << ", ymax = " << StripLayers[ilayer].GetYMaxLayer() << "\n";
                    //cout << "  xerr = " << xerr << "\n";
                    //cout << "  yerr = " << yerr << "\n";
                    //cout << "\n";

                    PseudoIntersectionsX.push_back(xcoord);
                    PseudoIntersectionsY.push_back(ycoord);

                    PseudoIntersectionsXErrors.push_back(xerr);
                    PseudoIntersectionsYErrors.push_back(yerr);

                    PseudoIntersections_LowerLayerClusterSize.push_back(cluster.Width);
                    PseudoIntersections_UpperLayerClusterSize.push_back(0);

                    PseudoIntersections_LowerLayerStripPosition.push_back(StripLayers[ilayer].ConvertPointToStripPosition(xcoord, ycoord));
                    PseudoIntersections_UpperLayerStripPosition.push_back(0);

                    PseudoIntersections_LowerLayerStripTotalSignal.push_back(cluster.TotalSignal);
                    PseudoIntersections_UpperLayerStripTotalSignal.push_back(0);

                    PseudoIntersectionMatches.push_back(StripLayers[ilayer].GetStripMatch((Int_t)cluster.MeanPosition)); //mc-match
                    PseudoIntersectionDigitNumberMatches.push_back(StripLayers[ilayer].GetStripDigitNumberMatch((Int_t)cluster.MeanPosition)); //digit number matches

                    LowerClusters_PseudoIntersections.push_back(cluster);
                    UpperClusters_PseudoIntersections.push_back(StripCluster()); //empty cluster with default parameters
                }
            }
        }
   }
}

//need for a separated test (find x,y intersection coords from strip positions)
Bool_t BmnVSPModule::SearchIntersectionPoint(Double_t &x, Double_t &y, Double_t strip_pos_layerA, Double_t strip_pos_layerB, Int_t layerA_index, Int_t layerB_index) {

    Int_t ilayer = layerA_index;
    Int_t jlayer = layerB_index;

    if(StripLayers[ilayer].GetType() == StripLayers[jlayer].GetType()) return false;

    Double_t iAngleRad = StripLayers[ilayer].GetAngleRad();
    Double_t jAngleRad = StripLayers[jlayer].GetAngleRad();

    if( Abs(iAngleRad - jAngleRad) < 0.01 ) return false;

    Double_t i_PiOver2MinusAngleRad = PiOver2() - iAngleRad;
    Double_t j_PiOver2MinusAngleRad = PiOver2() - jAngleRad;

    Double_t iStripHitPos = strip_pos_layerA;
    Double_t jStripHitPos = strip_pos_layerB;

    Double_t xcoord;
    Double_t ycoord;

    if( Abs(iAngleRad) >= 0.01 && Abs(jAngleRad) >= 0.01 ) {

        Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
        Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);

        xcoord = (jB - iB)/(Tan(i_PiOver2MinusAngleRad) - Tan(j_PiOver2MinusAngleRad));
        ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
    }
    else {

        if( Abs(iAngleRad) < 0.01 ) {

            if(StripLayers[ilayer].GetStripNumberingOrder() == LeftToRight ) {
                xcoord = StripLayers[ilayer].GetXLeftStripBorderPoint() + (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
            }
            else {
                xcoord = StripLayers[ilayer].GetXRightStripBorderPoint() - (iStripHitPos-StripLayers[ilayer].GetFirstStripNumber())*StripLayers[ilayer].GetPitch();
            }

            Double_t jB = StripLayers[jlayer].CalculateStripEquationB(jStripHitPos);
            ycoord = Tan(j_PiOver2MinusAngleRad)*xcoord + jB;
        }

        if( Abs(jAngleRad) < 0.01 ) {

            if(StripLayers[jlayer].GetStripNumberingOrder() == LeftToRight ) {
                xcoord = StripLayers[jlayer].GetXLeftStripBorderPoint() + (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
            }
            else {
                xcoord = StripLayers[jlayer].GetXRightStripBorderPoint() - (jStripHitPos-StripLayers[jlayer].GetFirstStripNumber())*StripLayers[jlayer].GetPitch();
            }

            Double_t iB = StripLayers[ilayer].CalculateStripEquationB(iStripHitPos);
            ycoord = Tan(i_PiOver2MinusAngleRad)*xcoord + iB;
        }
    }

    Double_t xrot = ConvertLocalToRotatedX(xcoord, ycoord);
    Double_t yrot = ConvertLocalToRotatedY(xcoord, ycoord);

    x = xrot;
    y = yrot;

    //check: a point belongs to both strip layers together
    if( StripLayers[ilayer].IsPointInsideStripLayer(xcoord, ycoord) && StripLayers[jlayer].IsPointInsideStripLayer(xcoord, ycoord) ) {
        return true;
    }

    return false;
}

Double_t BmnVSPModule::ConvertRotatedToLocalX(Double_t xrot, Double_t yrot) {
    //Getting local X-coordinate by applying an inverse rotation to rotated coordinates
    Double_t xloc = ModuleRotationCenterX + (xrot-ModuleRotationCenterX)*cos(ModuleRotationAlgleRad) - (yrot-ModuleRotationCenterY)*sin(ModuleRotationAlgleRad);
    return xloc;
}

Double_t BmnVSPModule::ConvertRotatedToLocalY(Double_t xrot, Double_t yrot) {
    //Getting local X-coordinate by applying an inverse rotation to rotated coordinates
    Double_t yloc = ModuleRotationCenterY + (xrot-ModuleRotationCenterX)*sin(ModuleRotationAlgleRad) + (yrot-ModuleRotationCenterY)*cos(ModuleRotationAlgleRad);
    return yloc;
}

Double_t BmnVSPModule::ConvertLocalToRotatedX(Double_t xloc, Double_t yloc) {
    //Getting rotated X-coordinate by applying a rotation to local (non-rotated) coordinates
    Double_t xrot = ModuleRotationCenterX + (xloc-ModuleRotationCenterX)*cos(-ModuleRotationAlgleRad) - (yloc-ModuleRotationCenterY)*sin(-ModuleRotationAlgleRad);
    return xrot;
}

Double_t BmnVSPModule::ConvertLocalToRotatedY(Double_t xloc, Double_t yloc) {
    //Getting rotated Y-coordinate by applying a rotation to local (non-rotated) coordinates
    Double_t yrot = ModuleRotationCenterY + (xloc-ModuleRotationCenterX)*sin(-ModuleRotationAlgleRad) + (yloc-ModuleRotationCenterY)*cos(-ModuleRotationAlgleRad);
    return yrot;
}

void BmnVSPModule::ResetIntersectionPoints() {
    IntersectionPointsX.clear();
    IntersectionPointsY.clear();
    IntersectionPointsXErrors.clear();
    IntersectionPointsYErrors.clear();
    IntersectionPoints_LowerLayerClusterSize.clear();
    IntersectionPoints_UpperLayerClusterSize.clear();
    IntersectionPoints_LowerLayerStripPosition.clear();
    IntersectionPoints_UpperLayerStripPosition.clear();
    IntersectionPoints_LowerLayerStripTotalSignal.clear();
    IntersectionPoints_UpperLayerStripTotalSignal.clear();
    IntersectionPointMatches.clear();
    IntersectionPointDigitNumberMatches.clear();
    UpperClusters.clear();
    LowerClusters.clear();
}

void BmnVSPModule::ResetPseudoIntersections() {
    PseudoIntersectionsX.clear();
    PseudoIntersectionsY.clear();
    PseudoIntersectionsXErrors.clear();
    PseudoIntersectionsYErrors.clear();
    PseudoIntersections_LowerLayerClusterSize.clear();
    PseudoIntersections_UpperLayerClusterSize.clear();
    PseudoIntersections_LowerLayerStripPosition.clear();
    PseudoIntersections_UpperLayerStripPosition.clear();
    PseudoIntersections_LowerLayerStripTotalSignal.clear();
    PseudoIntersections_UpperLayerStripTotalSignal.clear();
    PseudoIntersectionMatches.clear();
    PseudoIntersectionDigitNumberMatches.clear();
    UpperClusters_PseudoIntersections.clear();
    LowerClusters_PseudoIntersections.clear();
 }

void BmnVSPModule::DefineModuleBorders() {

    if( StripLayers.size() == 0 ) return;

    XMinModule = 1.0E+10;
    XMaxModule = -1.0E+10;
    YMinModule = 1.0E+10;
    YMaxModule = -1.0E+10;

    for (size_t ilayer = 0; ilayer < StripLayers.size(); ++ilayer)
    {
        if( StripLayers[ilayer].GetXMinLayer() < XMinModule ) XMinModule = StripLayers[ilayer].GetXMinLayer();
        if( StripLayers[ilayer].GetXMaxLayer() > XMaxModule ) XMaxModule = StripLayers[ilayer].GetXMaxLayer();
        if( StripLayers[ilayer].GetYMinLayer() < YMinModule ) YMinModule = StripLayers[ilayer].GetYMinLayer();
        if( StripLayers[ilayer].GetYMaxLayer() > YMaxModule ) YMaxModule = StripLayers[ilayer].GetYMaxLayer();
    }

    return;
}
