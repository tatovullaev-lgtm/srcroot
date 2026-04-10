/*
 * Baranov D.
 * 13.01.2022 (created)
 * 12.12.2022 (updated): new module type in the first station and
 *                       positions have been corrected in accordance with
 *                       the measurement scheme
 *
 * 18.01.2023 (updated): shifts to z-positions of modules have been added to compensate
 *                       for rotations of half-planes around the Y-axis
 *
 * 20.01.2023 (updated): rotation angles around the Y-axis for each half-plane
 *                       have been added
 *
 * Use this macro to create ROOT geometry for the RUN-8 config. of SILICON
 * detector including 4 Si-Stations (6-10-14-18 modules)
 *
 * WARNING: all units is in cm!!!
 *
 */

using namespace TMath;

//Set Parameters of SILICON detector -------------------------------------------
const Int_t NStations = 4;      //stations in the detector
const Int_t NMaxModules = 22;    //max. number of modules in a station

const Int_t NModulesInStations[NStations] = {6, 10, 14, 18};

//(X-Y-Z)Positions of stations (sensitive volumes)
const Double_t XStationPositions[NStations] = { +0.4025/*0*/, +0.726/*1*/, 0.8415/*2*/, +0.877/*3*/ }; //центр отверстия нижней п.плоскости
const Double_t YStationPositions[NStations] = { -0.159/*0*/, -0.165/*1*/, -0.122/*2*/, -0.111/*3*/}; //центр отверстия нижней п.плоскости
const Double_t ZStationPositions[NStations] = { +14.2735/*0*/, +23.022/*1*/, 31.594/*2*/, 40.407/*3*/ }; //реперная точка (средняя измеренная позиция по нижним п.плоскостям (ближним к мишени))

const Double_t z_shift_up = +2.68; //см, z-смещение от реперной точки (точки измерения) до локальной СК базовой точки для верхних полуплоскостей
const Double_t z_shift_low = -0.61; //см, z-смещение от реперной точки (точки измерения) до локальной СК базовой точки для нижних полуплоскостей

const Double_t z_mod_thick = 0.03; //см

const Double_t x_dist_cent_base[NStations] = {21.7/*0*/, 21.7/*1*/, 27.7/*2*/, 33.7/*3*/}; // x-расcтояние от центра полуплоскости до базы
const Double_t y_dist_cent_base[NStations] = {12.99/*0*/, 16.29/*1*/, 16.29/*2*/, 16.29/*3*/}; // y-расстояние от центра полуплоскости до базы


const Double_t z_shift_hp[NStations] = {+5.0/*0*/, 5.134/*1*/, 5.2825/*2*/, 5.106/*3*/}; //см, z-сдвиг верхней полуплоскости относительно нижней для разных станций

const Double_t x_shift_hp[NStations] = {+0.1255/*0*/, -0.0105/*1*/, +0.013/*2*/, +0.239/*3*/}; //см, x-сдвиг центра отверстия верхней полуплоскости относительно нижней

const Double_t y_shift_hp[NStations] = {+0.1495/*0*/, +0.0425/*1*/, +0.0265/*2*/, +0.5025/*3*/}; //см, y-сдвиг центра отверстия верхней полуплоскости относительно нижней


Double_t XModuleShifts[NStations][NMaxModules] = {
    {-x_dist_cent_base[0] + x_shift_hp[0] + 30.850/*0*/,
     -x_dist_cent_base[0] + x_shift_hp[0] + 24.852/*1*/,
     -x_dist_cent_base[0] + x_shift_hp[0] + 12.556/*2*/,

     -x_dist_cent_base[0] + 30.848/*3*/,
     -x_dist_cent_base[0] + 24.845/*4*/,
     -x_dist_cent_base[0] + 12.548/*5*/},

    {-x_dist_cent_base[1] + x_shift_hp[1] + 36.866/*0*/,
     -x_dist_cent_base[1] + x_shift_hp[1] + 30.8505/*1*/,
     -x_dist_cent_base[1] + x_shift_hp[1] + 24.8645/*2*/,
     -x_dist_cent_base[1] + x_shift_hp[1] + 12.5575/*3*/,
     -x_dist_cent_base[1] + x_shift_hp[1] + 6.5635/*4*/,

     -x_dist_cent_base[1] + 36.8425/*5*/,
     -x_dist_cent_base[1] + 30.8505/*6*/,
     -x_dist_cent_base[1] + 24.845/*7*/,
     -x_dist_cent_base[1] + 12.555/*8*/,
     -x_dist_cent_base[1] + 6.585/*9*/},

    {-x_dist_cent_base[2] + x_shift_hp[2] + 48.8465/*0*/,
     -x_dist_cent_base[2] + x_shift_hp[2] + 42.8315/*1*/,
     -x_dist_cent_base[2] + x_shift_hp[2] + 36.8485/*2*/,
     -x_dist_cent_base[2] + x_shift_hp[2] + 30.8425/*3*/,
     -x_dist_cent_base[2] + x_shift_hp[2] + 18.5555/*4*/,
     -x_dist_cent_base[2] + x_shift_hp[2] + 12.5395/*5*/,
     -x_dist_cent_base[2] + x_shift_hp[2] + 6.553/*6*/,

     -x_dist_cent_base[2] + 48.87/*7*/,
     -x_dist_cent_base[2] + 42.859/*8*/,
     -x_dist_cent_base[2] + 36.857/*9*/,
     -x_dist_cent_base[2] + 30.8515/*10*/,
     -x_dist_cent_base[2] + 18.556/*11*/,
     -x_dist_cent_base[2] + 12.555/*12*/,
     -x_dist_cent_base[2] + 6.5535/*13*/},

    {-x_dist_cent_base[3] + x_shift_hp[3] + 60.862/*0*/,
     -x_dist_cent_base[3] + x_shift_hp[3] + 54.8545/*1*/,
     -x_dist_cent_base[3] + x_shift_hp[3] + 48.862/*2*/,
     -x_dist_cent_base[3] + x_shift_hp[3] + 42.849/*3*/,
     -x_dist_cent_base[3] + x_shift_hp[3] + 36.851/*4*/,
     -x_dist_cent_base[3] + x_shift_hp[3] + 24.5565/*5*/,
     -x_dist_cent_base[3] + x_shift_hp[3] + 18.549/*6*/,
     -x_dist_cent_base[3] + x_shift_hp[3] + 12.5515/*7*/,
     -x_dist_cent_base[3] + x_shift_hp[3] + 6.651/*8*/,

     -x_dist_cent_base[3] + 60.8615/*9*/,
     -x_dist_cent_base[3] + 54.85/*10*/,
     -x_dist_cent_base[3] + 48.863/*11*/,
     -x_dist_cent_base[3] + 42.848/*12*/,
     -x_dist_cent_base[3] + 36.849/*13*/,
     -x_dist_cent_base[3] + 24.548/*14*/,
     -x_dist_cent_base[3] + 18.559/*15*/,
     -x_dist_cent_base[3] + 12.5545/*16*/,
     -x_dist_cent_base[3] + 6.553/*17*/}
};
Double_t YModuleShifts[NStations][NMaxModules] = {
    {+y_dist_cent_base[0] + y_shift_hp[0] - 13.112/*0*/,
     +y_dist_cent_base[0] + y_shift_hp[0] - 10.113/*1*/,
     +y_dist_cent_base[0] + y_shift_hp[0] - 13.086/*2*/,

     -y_dist_cent_base[0] + 13.100/*3*/,
     -y_dist_cent_base[0] + 10.110/*4*/,
     -y_dist_cent_base[0] + 13.098/*5*/},

    {+y_dist_cent_base[1] + y_shift_hp[1] - 16.427/*0*/,
     +y_dist_cent_base[1] + y_shift_hp[1] - 16.425/*1*/,
     +y_dist_cent_base[1] + y_shift_hp[1] - 13.423/*2*/,
     +y_dist_cent_base[1] + y_shift_hp[1] - 16.423/*3*/,
     +y_dist_cent_base[1] + y_shift_hp[1] - 16.425/*4*/,

     -y_dist_cent_base[1] + 16.432/*5*/,
     -y_dist_cent_base[1] + 16.423/*6*/,
     -y_dist_cent_base[1] + 13.428/*7*/,
     -y_dist_cent_base[1] + 16.425/*8*/,
     -y_dist_cent_base[1] + 16.429/*9*/},

    {+y_dist_cent_base[2] + y_shift_hp[2] - 16.424/*0*/,
     +y_dist_cent_base[2] + y_shift_hp[2] - 16.427/*1*/,
     +y_dist_cent_base[2] + y_shift_hp[2] - 16.427/*2*/,
     +y_dist_cent_base[2] + y_shift_hp[2] - 13.411/*3*/,
     +y_dist_cent_base[2] + y_shift_hp[2] - 16.431/*4*/,
     +y_dist_cent_base[2] + y_shift_hp[2] - 16.420/*5*/,
     +y_dist_cent_base[2] + y_shift_hp[2] - 16.430/*6*/,

     -y_dist_cent_base[2] + 16.420/*7*/,
     -y_dist_cent_base[2] + 16.426/*8*/,
     -y_dist_cent_base[2] + 16.427/*9*/,
     -y_dist_cent_base[2] + 13.428/*10*/,
     -y_dist_cent_base[2] + 16.418/*11*/,
     -y_dist_cent_base[2] + 16.426/*12*/,
     -y_dist_cent_base[2] + 16.417/*13*/},

    {+y_dist_cent_base[3] + y_shift_hp[3] - 16.414/*0*/,
     +y_dist_cent_base[3] + y_shift_hp[3] - 16.414/*1*/,
     +y_dist_cent_base[3] + y_shift_hp[3] - 16.413/*2*/,
     +y_dist_cent_base[3] + y_shift_hp[3] - 16.420/*3*/,
     +y_dist_cent_base[3] + y_shift_hp[3] - 13.417/*4*/,
     +y_dist_cent_base[3] + y_shift_hp[3] - 16.426/*5*/,
     +y_dist_cent_base[3] + y_shift_hp[3] - 16.418/*6*/,
     +y_dist_cent_base[3] + y_shift_hp[3] - 16.425/*7*/,
     +y_dist_cent_base[3] + y_shift_hp[3] - 16.427/*8*/,

     -y_dist_cent_base[3] + 16.430/*9*/,
     -y_dist_cent_base[3] + 16.433/*10*/,
     -y_dist_cent_base[3] + 16.433/*11*/,
     -y_dist_cent_base[3] + 16.430/*12*/,
     -y_dist_cent_base[3] + 13.435/*13*/,
     -y_dist_cent_base[3] + 16.430/*14*/,
     -y_dist_cent_base[3] + 16.434/*15*/,
     -y_dist_cent_base[3] + 16.419/*16*/,
     -y_dist_cent_base[3] + 16.426/*17*/}
};
Double_t ZModuleShifts[NStations][NMaxModules] = {
    {+z_shift_hp[0]+z_shift_up - 2.60/*0*/,
     +z_shift_hp[0]+z_shift_up - 1.45/*1*/,
     +z_shift_hp[0]+z_shift_up - 2.70/*2*/,

     +z_shift_low - z_mod_thick + 2.61/*3*/,
     +z_shift_low - z_mod_thick + 1.40/*4*/,
     +z_shift_low - z_mod_thick + 2.79/*5*/},

    {+z_shift_hp[1]+z_shift_up - 1.430/*0*/,
     +z_shift_hp[1]+z_shift_up - 2.605/*1*/,
     +z_shift_hp[1]+z_shift_up - 1.435/*2*/,
     +z_shift_hp[1]+z_shift_up - 2.750/*3*/,
     +z_shift_hp[1]+z_shift_up - 1.580/*4*/,

     +z_shift_low - z_mod_thick + 1.425/*5*/,
     +z_shift_low - z_mod_thick + 2.595/*6*/,
     +z_shift_low - z_mod_thick + 1.445/*7*/,
     +z_shift_low - z_mod_thick + 2.730/*8*/,
     +z_shift_low - z_mod_thick + 1.510/*9*/},

    {+z_shift_hp[2]+z_shift_up - 2.595/*0*/,
     +z_shift_hp[2]+z_shift_up - 1.395/*1*/,
     +z_shift_hp[2]+z_shift_up - 2.665/*2*/,
     +z_shift_hp[2]+z_shift_up - 1.425/*3*/,
     +z_shift_hp[2]+z_shift_up - 2.735/*4*/,
     +z_shift_hp[2]+z_shift_up - 1.560/*5*/,
     +z_shift_hp[2]+z_shift_up - 2.705/*6*/,

     +z_shift_low - z_mod_thick + 2.580/*7*/,
     +z_shift_low - z_mod_thick + 1.410/*8*/,
     +z_shift_low - z_mod_thick + 2.565/*9*/,
     +z_shift_low - z_mod_thick + 1.390/*10*/,
     +z_shift_low - z_mod_thick + 2.755/*11*/,
     +z_shift_low - z_mod_thick + 1.570/*12*/,
     +z_shift_low - z_mod_thick + 2.765/*13*/},

    {+z_shift_hp[3]+z_shift_up - 1.405/*0*/,
     +z_shift_hp[3]+z_shift_up - 2.610/*1*/,
     +z_shift_hp[3]+z_shift_up - 1.460/*2*/,
     +z_shift_hp[3]+z_shift_up - 2.605/*3*/,
     +z_shift_hp[3]+z_shift_up - 1.420/*4*/,
     +z_shift_hp[3]+z_shift_up - 2.755/*5*/,
     +z_shift_hp[3]+z_shift_up - 1.580/*6*/,
     +z_shift_hp[3]+z_shift_up - 2.760/*7*/,
     +z_shift_hp[3]+z_shift_up - 1.575/*8*/,

     +z_shift_low - z_mod_thick + 1.450/*9*/,
     +z_shift_low - z_mod_thick + 2.575/*10*/,
     +z_shift_low - z_mod_thick + 1.445/*11*/,
     +z_shift_low - z_mod_thick + 2.600/*12*/,
     +z_shift_low - z_mod_thick + 1.455/*13*/,
     +z_shift_low - z_mod_thick + 2.745/*14*/,
     +z_shift_low - z_mod_thick + 1.575/*15*/,
     +z_shift_low - z_mod_thick + 2.700/*16*/,
     +z_shift_low - z_mod_thick + 1.545/*17*/}
};

//alignment corrections --------------------------------------------------------
const Double_t XAlignmentCorrections[NStations][NMaxModules] = {
    {2.68835e-01,2.39831e-01,2.52506e-01,2.05206e-01,2.11052e-01,0.20737190},
    {3.15120e-01,2.74671e-01,2.55372e-01,2.83446e-01,2.79317e-01, 3.48623e-01,3.18654e-01,3.00181e-01,3.16157e-01,3.80363e-01},
    {1.65037e-01,2.71806e-01,2.66914e-01,2.40166e-01,2.59614e-01,2.66288e-01,2.83516e-01, 2.88311e-01,2.61280e-01,3.02609e-01,2.96469e-01,2.98769e-01,2.66650e-01,3.72627e-01},
    {3.01013e-01,3.62381e-01,3.03249e-01,3.06502e-01,2.75747e-01, 2.95530e-01,3.05152e-01,3.42706e-01,3.09787e-01, 2.24795e-01, 2.31845e-01, 2.59297e-01, 2.17342e-01, 2.20424e-01, 2.06861e-01, 2.49563e-01,2.64846e-01,2.02358e-01}
};

const Double_t YAlignmentCorrections[NStations][NMaxModules] = {
    {-0.272688,-0.300649,-0.343639,-0.313878,-0.331129,-0.299529},
    {-0.342847,-0.343267,-0.328436,-0.354188,-0.332445, -0.349645,-0.332025,-0.294098,-0.249696,-0.217585},
    {-0.398784,-0.342968,-0.325462,-0.325316,-0.338778,-0.338966,-0.324549, -0.339105,-0.314266,-0.301756,-0.288295,-0.272864,-0.272913,-0.21294},
    {-0.269827,-0.227419,-0.295412,-0.274608,-0.322856,-0.33417,-0.361013,-0.397192,-0.368905, -0.217688,-0.240085,-0.266281,-0.285738,-0.269306,-0.292405,-0.297714,-0.323987,-0.379705}
};

const Double_t ZAlignmentCorrections[NStations][NMaxModules] = {
    {-0.4,-0.3,-0.23,-0.27,-0.3,-0.23},
    {-0.5,-0.4,-0.4,-0.28,-0.25, -0.45,-0.45,-0.3,-0.23,-0.2},
    {-0.3,-0.5,-0.45,-0.4,-0.3,-0.2,-0.2,-0.25,-0.2,-0.3,-0.3,-0.4,-0.5,-0.27},
    {-0.5,-0.6,-0.4,-0.5,-0.3,-0.35,-0.2,-0.1,-0.4,  -0.6,-0.6,-0.6,-0.5,-0.35,-0.2,-0.1,0.1,0.}
};
//------------------------------------------------------------------------------

//const Double_t ZModuleShifts[NStations][NMaxModules] = {
//    {+z_shift_hp[0]+z_shift_up - 2.60 + 0.105/*0*/, //the last value is a shift to compensate for a rotation of half-plane around the Y-axis
//     +z_shift_hp[0]+z_shift_up - 1.45 + 0.084/*1*/,
//     +z_shift_hp[0]+z_shift_up - 2.70 + 0.063/*2*/,
//
//     +z_shift_low - z_mod_thick + 2.61 + 0.121/*3*/,
//     +z_shift_low - z_mod_thick + 1.40 + 0.097/*4*/,
//     +z_shift_low - z_mod_thick + 2.79 + 0.072/*5*/},
//
//    {+z_shift_hp[1]+z_shift_up - 1.430 + 0.247/*0*/,
//     +z_shift_hp[1]+z_shift_up - 2.605 + 0.206/*1*/,
//     +z_shift_hp[1]+z_shift_up - 1.435 + 0.164/*2*/,
//     +z_shift_hp[1]+z_shift_up - 2.750 + 0.122/*3*/,
//     +z_shift_hp[1]+z_shift_up - 1.580 + 0.081/*4*/,
//
//     +z_shift_low - z_mod_thick + 1.425 + 0.107/*5*/,
//     +z_shift_low - z_mod_thick + 2.595 + 0.089/*6*/,
//     +z_shift_low - z_mod_thick + 1.445 + 0.071/*7*/,
//     +z_shift_low - z_mod_thick + 2.730 + 0.053/*8*/,
//     +z_shift_low - z_mod_thick + 1.510 + 0.035/*9*/},
//
//    {+z_shift_hp[2]+z_shift_up - 2.595 + 0.128/*0*/,
//     +z_shift_hp[2]+z_shift_up - 1.395 + 0.112/*1*/,
//     +z_shift_hp[2]+z_shift_up - 2.665 + 0.096/*2*/,
//     +z_shift_hp[2]+z_shift_up - 1.425 + 0.080/*3*/,
//     +z_shift_hp[2]+z_shift_up - 2.735 + 0.063/*4*/,
//     +z_shift_hp[2]+z_shift_up - 1.560 + 0.043/*5*/,
//     +z_shift_hp[2]+z_shift_up - 2.705 + 0.031/*6*/,
//
//     +z_shift_low - z_mod_thick + 2.580 + 0.484/*7*/,
//     +z_shift_low - z_mod_thick + 1.410 + 0.423/*8*/,
//     +z_shift_low - z_mod_thick + 2.565 + 0.362/*9*/,
//     +z_shift_low - z_mod_thick + 1.390 + 0.302/*10*/,
//     +z_shift_low - z_mod_thick + 2.755 + 0.241/*11*/,
//     +z_shift_low - z_mod_thick + 1.570 + 0.180/*12*/,
//     +z_shift_low - z_mod_thick + 2.765 + +0.119/*13*/},
//
//    {+z_shift_hp[3]+z_shift_up - 1.405 + 0.112/*0*/,
//     +z_shift_hp[3]+z_shift_up - 2.610 + 0.101/*1*/,
//     +z_shift_hp[3]+z_shift_up - 1.460 + 0.090/*2*/,
//     +z_shift_hp[3]+z_shift_up - 2.605 + 0.078/*3*/,
//     +z_shift_hp[3]+z_shift_up - 1.420 + 0.067/*4*/,
//     +z_shift_hp[3]+z_shift_up - 2.755 + 0.056/*5*/,
//     +z_shift_hp[3]+z_shift_up - 1.580 + 0.044/*6*/,
//     +z_shift_hp[3]+z_shift_up - 2.760 + 0.033/*7*/,
//     +z_shift_hp[3]+z_shift_up - 1.575 + 0.022/*8*/,
//
//     +z_shift_low - z_mod_thick + 1.450 - 0.381/*9*/,
//     +z_shift_low - z_mod_thick + 2.575 - 0.343/*10*/,
//     +z_shift_low - z_mod_thick + 1.445 - 0.305/*11*/,
//     +z_shift_low - z_mod_thick + 2.600 - 0.266/*12*/,
//     +z_shift_low - z_mod_thick + 1.455 - 0.228/*13*/,
//     +z_shift_low - z_mod_thick + 2.745 - 0.119/*14*/,
//     +z_shift_low - z_mod_thick + 1.575 - 0.151/*15*/,
//     +z_shift_low - z_mod_thick + 2.700 - 0.113/*16*/,
//     +z_shift_low - z_mod_thick + 1.545 - 0.075/*17*/}
//};

//Geometry parameters of modules
enum ModuleType {RIGHT_ANGLED_MODULE, BEVEL_MODULE, RIGHT_ANGLED_MODULE_HALF, BEVEL_MODULE_HALF};

const ModuleType ModulesTypes[NStations][NMaxModules] = {
    {RIGHT_ANGLED_MODULE_HALF, RIGHT_ANGLED_MODULE_HALF, RIGHT_ANGLED_MODULE_HALF, RIGHT_ANGLED_MODULE_HALF, RIGHT_ANGLED_MODULE_HALF, RIGHT_ANGLED_MODULE_HALF},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE}
};

//rotations of modules around y-axis by 180 deg.
const Bool_t ModulesYRotations[NStations][NMaxModules] = {
    {true/*0*/, true/*1*/, false/*2*/, true/*3*/, true/*4*/, false/*5*/},
    {true/*0*/, true/*1*/, true/*2*/, false/*3*/, false/*4*/, true/*5*/, true/*6*/, true/*7*/, false/*8*/, false/*9*/},
    {true/*0*/, true/*1*/, true/*2*/, true/*3*/, false/*4*/, false/*5*/, false/*6*/, true/*7*/, true/*8*/, true/*9*/, true/*10*/, false/*11*/, false/*12*/, false/*13*/},
    {true/*0*/, true/*1*/, true/*2*/, true/*3*/, true/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/, false/*14*/, false/*15*/, false/*16*/, false/*17*/}
};

//rotations of modules around x-axis by 180 deg.
const Bool_t ModulesXRotations[NStations][NMaxModules] = {
    {false/*0*/, false/*1*/, false/*2*/, true/*3*/, true/*4*/, true/*5*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, true/*5*/, true/*6*/, true/*7*/, true/*8*/, true/*9*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, true/*7*/, true/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/, true/*14*/, true/*15*/, true/*16*/, true/*17*/}
};

/*
/rotations of half-planes around y-axis (angles in degrees)
const Int_t NMaxHalfPlanes = 2;
const Double_t HalfPlanesYRotations[NStations][NMaxHalfPlanes] = {
    {-0.205, -0.235}, // st.0 (upper [0] and lower [1] half-planes)
    {-0.399, -0.173}, // st.1
    {-0.151, -0.584}, // st.2
    {-0.108, +0.367}  // st.3
};
*/
//------------------------------------------------------------------------------

//GeoManager
TGeoManager* gGeoMan = 0;


//Module parameters ------------------------------------------------------------
enum ModuleSize {STANDARD_MODULE, LONG_MODULE}; //STANDARD_MODULE is (63x126)mm, LONG_MODULE is (63x186)mm
const unsigned int NModuleSizes = 2;

Double_t YGapBetweenPartsOfModule[NModuleSizes] = { 0.0035/*STANDARD_MODULE*/, 0.0035/*LONG_MODULE*/ }; //Mrs. Slit

Double_t XModuleSize[NModuleSizes] = { 6.3/*STANDARD_MODULE*/, 6.3/*LONG_MODULE*/ };

Double_t YModuleSizeUpperPart[NModuleSizes] = { 6.3/*STANDARD_MODULE*/, 9.3/*LONG_MODULE*/ };
Double_t YModuleSizeLowerPart[NModuleSizes] = { 6.3/*STANDARD_MODULE*/, 9.3/*LONG_MODULE*/ };

Double_t YModuleSize[NModuleSizes] = {
    YModuleSizeUpperPart[STANDARD_MODULE] + YModuleSizeLowerPart[STANDARD_MODULE] + YGapBetweenPartsOfModule[STANDARD_MODULE], /*STANDARD_MODULE*/
    YModuleSizeUpperPart[LONG_MODULE] + YModuleSizeLowerPart[LONG_MODULE] + YGapBetweenPartsOfModule[LONG_MODULE] /*LONG_MODULE*/
};

Double_t ZModuleSize[NModuleSizes] = { z_mod_thick/*STANDARD_MODULE*/, z_mod_thick/*LONG_MODULE*/ };

Double_t XBevelSize[NModuleSizes] = { 3.82/*STANDARD_MODULE*/, 3.82/*LONG_MODULE*/ }; //x-bevel for a angled cut
Double_t YBevelSize[NModuleSizes] = { 3.82/*STANDARD_MODULE*/, 3.82/*LONG_MODULE*/ }; //y-bevel for a angled cut

Double_t BevelAngleRad[NModuleSizes] = {
    TMath::ATan(XBevelSize[STANDARD_MODULE]/YBevelSize[STANDARD_MODULE]), /*STANDARD_MODULE*/
    TMath::ATan(XBevelSize[LONG_MODULE]/YBevelSize[LONG_MODULE]) /*LONG_MODULE*/
};

Double_t BevelAngleDeg[NModuleSizes] = {
    BevelAngleRad[STANDARD_MODULE]*TMath::RadToDeg(), /*STANDARD_MODULE*/
    BevelAngleRad[LONG_MODULE]*TMath::RadToDeg() /*LONG_MODULE*/
};

Double_t XLeftBorder[NModuleSizes] = { 0.1145/*STANDARD_MODULE*/, 0.1118/*LONG_MODULE*/ };
Double_t XRightBorder[NModuleSizes] = { 0.1118/*STANDARD_MODULE*/, 0.1118/*LONG_MODULE*/ };
Double_t YTopBorder[NModuleSizes] = { 0.1143/*STANDARD_MODULE*/, 0.1148/*LONG_MODULE*/ };
Double_t YMiddleBorderUpperPart[NModuleSizes] = { 0.1121/*STANDARD_MODULE*/, 0.1148/*LONG_MODULE*/ };
Double_t YMiddleBorderLowerPart[NModuleSizes] = { 0.1147/*STANDARD_MODULE*/, 0.1148/*LONG_MODULE*/ };
Double_t YBottomBorder[NModuleSizes] = { 0.1148/*STANDARD_MODULE*/, 0.1148/*LONG_MODULE*/ };
Double_t XYBevelBorder[NModuleSizes] = { 0.1146/*STANDARD_MODULE*/, 0.1146/*LONG_MODULE*/ };

//Sensor zones ------------------------------------
Double_t XSensorZoneSize[NModuleSizes] = {
    XModuleSize[STANDARD_MODULE] - (XLeftBorder[STANDARD_MODULE] + XRightBorder[STANDARD_MODULE]),/*STANDARD_MODULE*/
    XModuleSize[LONG_MODULE] - (XLeftBorder[LONG_MODULE] + XRightBorder[LONG_MODULE]) /*LONG_MODULE*/
};

Double_t YSensorZoneSizeUpperPart[NModuleSizes] = {
    YModuleSizeUpperPart[STANDARD_MODULE] - (YTopBorder[STANDARD_MODULE] + YMiddleBorderUpperPart[STANDARD_MODULE]), /*STANDARD_MODULE*/
    YModuleSizeUpperPart[LONG_MODULE] - (YTopBorder[LONG_MODULE] + YMiddleBorderUpperPart[LONG_MODULE]) /*LONG_MODULE*/
};

Double_t YSensorZoneSizeLowerPart[NModuleSizes] = {
    YModuleSizeLowerPart[STANDARD_MODULE] - (YBottomBorder[STANDARD_MODULE] + YMiddleBorderLowerPart[STANDARD_MODULE]), /*STANDARD_MODULE*/
    YModuleSizeLowerPart[LONG_MODULE] - (YBottomBorder[LONG_MODULE] + YMiddleBorderLowerPart[LONG_MODULE]) /*LONG_MODULE*/
};

Double_t ZSensorZoneSize[NModuleSizes] = {
    ZModuleSize[STANDARD_MODULE], /*STANDARD_MODULE*/
    ZModuleSize[LONG_MODULE] /*LONG_MODULE*/
};
//-------------------------------------------------

//Module frames -----------------------------------
Double_t XModuleFrameSize[NModuleSizes] = {
    XModuleSize[STANDARD_MODULE], /*STANDARD_MODULE*/
    XModuleSize[LONG_MODULE] /*LONG_MODULE*/
};

Double_t YModuleFrameSize[NModuleSizes] = {
    YModuleSize[STANDARD_MODULE], /*STANDARD_MODULE*/
    YModuleSize[LONG_MODULE] /*LONG_MODULE*/
};

Double_t ZModuleFrameSize[NModuleSizes] = { 0.18/*STANDARD_MODULE*/, 0.18/*LONG_MODULE*/ };
//-------------------------------------------------

//Y-shift from center of the gap to the global x-axis (for y-alignment of the module)
Double_t YShiftFromCenterToAxis[NModuleSizes] = {
    (YModuleSizeUpperPart[STANDARD_MODULE]-YModuleSizeLowerPart[STANDARD_MODULE])*0.5, /*STANDARD_MODULE*/
    (YModuleSizeUpperPart[LONG_MODULE]-YModuleSizeLowerPart[LONG_MODULE])*0.5 /*LONG_MODULE*/
};

//X-shift from center of the module to the origin of the local x-axis
Double_t XShiftFromCenterToLocalX0[NModuleSizes] = {
    +XModuleSize[STANDARD_MODULE]*0.5, /*STANDARD_MODULE*/
    +XModuleSize[LONG_MODULE]*0.5 /*LONG_MODULE*/
};

//Y-shift from center of the module to the origin of the local y-axis
Double_t YShiftFromCenterToLocalY0[NModuleSizes] = {
    +(YModuleSizeLowerPart[STANDARD_MODULE] + YGapBetweenPartsOfModule[STANDARD_MODULE]*0.5), /*STANDARD_MODULE*/
    +(YModuleSizeLowerPart[LONG_MODULE] + YGapBetweenPartsOfModule[LONG_MODULE]*0.5) /*LONG_MODULE*/
};
//------------------------------------------------------------------------------
/*
     ASCII module picture (view from target)

        |<---- XModuleSize ---------->|                     ^
        |                             |                     | Y-axis
        ------------------------------  ---------
        |   |    top border       |   |         ^
        ------------------------------          |
        |   |                     |   |         |
        | l |    rectangle        | r |         |
        | e |      sensor         | i |         |
        | f |       zone          | g |         |
        | t |                     | h |         |
        |   |                     | t |         |
        |   |                     |   |         |
        ------------------------------          |
        |   | middle (upper part) |   |
        ------ gap between parts -----      YModuleSize
        |   | middle (lower part) |   |
        ------------------------------          |
        |   |                     |   |         |
        | b |   bevel (or rect.)  | r |         |
        | e |      sensor         | i |         |
         \ v \      zone          | g |         |
          \ e \                   | h |         |
           \ l \                  | t |         |
            \   \                 |   |         |
             -------------------------          |
              \   \  bottom       |   |         v
                ----------------------  ---------

 *  x-axis <----------------------------------(x0,y0) local point
 */

//media
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedCopper = 0;
TGeoMedium *pMedSilicon = 0;

class FairGeoMedia;
class FairGeoBuilder;

//function declarations
TGeoVolume *CreateStation(TString station_name);
TGeoVolume *CreateModule(TString module_name, ModuleType module_type, ModuleSize module_size);


void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild) {

    //air medium
    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    pMedAir = gGeoManager->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");

    //carbon medium
    FairGeoMedium* mCarbon = geoMedia->getMedium("carbon");
    if ( ! mCarbon  ) Fatal("Main", "FairMedium carbon not found");
    geoBuild->createMedium(mCarbon);
    pMedCarbon = gGeoManager->GetMedium("carbon");
    if ( ! pMedCarbon ) Fatal("Main", "Medium carbon not found");

    //fiberglass medium
    FairGeoMedium* mFiberGlass = geoMedia->getMedium("fiberglass");
    if ( ! mFiberGlass  ) Fatal("Main", "FairMedium fiberglass not found");
    geoBuild->createMedium(mFiberGlass);
    pMedFiberGlass = gGeoManager->GetMedium("fiberglass");
    if ( ! pMedFiberGlass  ) Fatal("Main", "Medium fiberglass not found");

    //copper medium
    FairGeoMedium* mCopper = geoMedia->getMedium("copper");
    if ( ! mCopper  ) Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    pMedCopper= gGeoManager->GetMedium("copper");
    if ( ! pMedCopper  ) Fatal("Main", "Medium copper not found");

    //silicon medium
    FairGeoMedium* mSilicon = geoMedia->getMedium("silicon");
    if ( ! mSilicon  ) Fatal("Main", "FairMedium silicon not found");
    geoBuild->createMedium(mSilicon);
    pMedSilicon= gGeoManager->GetMedium("silicon");
    if ( ! pMedSilicon  ) Fatal("Main", "Medium silicon not found");
}


void PrintInfoAboutModuleParameters() {
    cout << "\n";
    cout << "Module parameters information [STANDARD_MODULE]: ------------------" << "\n";
    cout << "  XModuleSize = " << XModuleSize[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YModuleSize = " << YModuleSize[STANDARD_MODULE] << " cm" << " ( " << YModuleSizeUpperPart[STANDARD_MODULE] << " + " << YGapBetweenPartsOfModule[STANDARD_MODULE] << " + " << YModuleSizeLowerPart[STANDARD_MODULE] << " )"   << "\n";
    cout << "     YModuleSizeUpperPart = " << YModuleSizeUpperPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "     YModuleSizeLowerPart = " << YModuleSizeLowerPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "     YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule[STANDARD_MODULE] << " cm" << "\n";
    cout << "  ZModuleSize = " << ZModuleSize[STANDARD_MODULE] << " cm" << "\n";
    cout << "\n";
    cout << "  XBevelSize = " << XBevelSize[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YBevelSize = " << YBevelSize[STANDARD_MODULE] << " cm" << "\n";
    cout << "     BevelAngleDeg = " << BevelAngleDeg[STANDARD_MODULE] << " deg" << "\n";
    cout << "\n";
    cout << "  XLeftBorder = " << XLeftBorder[STANDARD_MODULE] << " cm" << "\n";
    cout << "  XRightBorder = " << XRightBorder[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YTopBorder = " << YTopBorder[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YMiddleBorderUpperPart = " << YMiddleBorderUpperPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule[STANDARD_MODULE] << " cm" << " (gap between two parts)\n";
    cout << "  YMiddleBorderLowerPart = " << YMiddleBorderLowerPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YBottomBorder = " << YBottomBorder[STANDARD_MODULE] << " cm" << "\n";
    cout << "  XYBevelBorder = " << XYBevelBorder[STANDARD_MODULE] << " cm" << "\n";
    cout << "\n";
    cout << "  XSensorZoneSize = " << XSensorZoneSize[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YSensorZoneSizeUpperPart = " << YSensorZoneSizeUpperPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YSensorZoneSizeLowerPart = " << YSensorZoneSizeLowerPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "  ZSensorZoneSize = " << ZSensorZoneSize[STANDARD_MODULE] << " cm" << "\n";

    cout << "-------------------------------------------------------------\n";

    cout << "\n";
    cout << "Module parameters information [LONG_MODULE]: ------------------" << "\n";
    cout << "  XModuleSize = " << XModuleSize[LONG_MODULE] << " cm" << "\n";
    cout << "  YModuleSize = " << YModuleSize[LONG_MODULE] << " cm" << " ( " << YModuleSizeUpperPart[LONG_MODULE] << " + " << YGapBetweenPartsOfModule[LONG_MODULE] << " + " << YModuleSizeLowerPart[LONG_MODULE] << " )"   << "\n";
    cout << "     YModuleSizeUpperPart = " << YModuleSizeUpperPart[LONG_MODULE] << " cm" << "\n";
    cout << "     YModuleSizeLowerPart = " << YModuleSizeLowerPart[LONG_MODULE] << " cm" << "\n";
    cout << "     YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule[LONG_MODULE] << " cm" << "\n";
    cout << "  ZModuleSize = " << ZModuleSize[LONG_MODULE] << " cm" << "\n";
    cout << "\n";
    cout << "  XBevelSize = " << XBevelSize[LONG_MODULE] << " cm" << "\n";
    cout << "  YBevelSize = " << YBevelSize[LONG_MODULE] << " cm" << "\n";
    cout << "     BevelAngleDeg = " << BevelAngleDeg[LONG_MODULE] << " deg" << "\n";
    cout << "\n";
    cout << "  XLeftBorder = " << XLeftBorder[LONG_MODULE] << " cm" << "\n";
    cout << "  XRightBorder = " << XRightBorder[LONG_MODULE] << " cm" << "\n";
    cout << "  YTopBorder = " << YTopBorder[LONG_MODULE] << " cm" << "\n";
    cout << "  YMiddleBorderUpperPart = " << YMiddleBorderUpperPart[LONG_MODULE] << " cm" << "\n";
    cout << "  YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule[LONG_MODULE] << " cm" << " (gap between two parts)\n";
    cout << "  YMiddleBorderLowerPart = " << YMiddleBorderLowerPart[LONG_MODULE] << " cm" << "\n";
    cout << "  YBottomBorder = " << YBottomBorder[LONG_MODULE] << " cm" << "\n";
    cout << "  XYBevelBorder = " << XYBevelBorder[LONG_MODULE] << " cm" << "\n";
    cout << "\n";
    cout << "  XSensorZoneSize = " << XSensorZoneSize[LONG_MODULE] << " cm" << "\n";
    cout << "  YSensorZoneSizeUpperPart = " << YSensorZoneSizeUpperPart[LONG_MODULE] << " cm" << "\n";
    cout << "  YSensorZoneSizeLowerPart = " << YSensorZoneSizeLowerPart[LONG_MODULE] << " cm" << "\n";
    cout << "  ZSensorZoneSize = " << ZSensorZoneSize[LONG_MODULE] << " cm" << "\n";

    cout << "-------------------------------------------------------------\n";
}

void PrintInformationAboutStations() {
    cout << "\n";
    for(Int_t istation = 0; istation < NStations; ++istation) {
        cout << "station " << istation << ":\n";
        cout << "   XStationPositions[" << istation << "] = " << XStationPositions[istation] << " cm\n";
        cout << "   YStationPositions[" << istation << "] = " << YStationPositions[istation] << " cm\n";
        cout << "   ZStationPositions[" << istation << "] = " << ZStationPositions[istation] << " cm\n";
        cout << "\n";

        for(Int_t imodule = 0; imodule < NModulesInStations[istation]; ++imodule) {
            cout << "   module " << imodule << ":\n";
            cout << "      XModuleShifts[" << istation << "][" << imodule << "] = " << XModuleShifts[istation][imodule] << " cm\n";
            cout << "      YModuleShifts[" << istation << "][" << imodule << "] = " << YModuleShifts[istation][imodule] << " cm\n";
            cout << "      ZModuleShifts[" << istation << "][" << imodule << "] = " << ZModuleShifts[istation][imodule] << " cm\n";
            cout << "\n";

            Double_t XGlobalModulePos = XModuleShifts[istation][imodule] + XStationPositions[istation];
            Double_t YGlobalModulePos = YModuleShifts[istation][imodule] + YStationPositions[istation];
            Double_t ZGlobalModulePos = ZModuleShifts[istation][imodule] + ZStationPositions[istation];

            cout << "      XGlobalModulePos = " << XGlobalModulePos << " cm\n";
            cout << "      YGlobalModulePos = " << YGlobalModulePos << " cm\n";
            cout << "      ZGlobalModulePos = " << ZGlobalModulePos << " cm\n";
            cout << "\n";

            cout << "      ModulesTypes[" << istation << "][" << imodule << "] = ";
                if(ModulesTypes[istation][imodule] == 0) cout << "RIGHT_ANGLED_MODULE\n";
                if(ModulesTypes[istation][imodule] == 1) cout << "BEVEL_MODULE\n";
                if(ModulesTypes[istation][imodule] == 2) cout << "RIGHT_ANGLED_MODULE_HALF\n";
                if(ModulesTypes[istation][imodule] == 3) cout << "BEVEL_MODULE_HALF\n";
            cout << "      ModulesYRotations[" << istation << "][" << imodule << "] = ";
                if(ModulesYRotations[istation][imodule] == false) cout << "not rotated around Y\n";
                if(ModulesYRotations[istation][imodule] == true) cout << "rotated around Y\n";
            cout << "      ModulesXRotations[" << istation << "][" << imodule << "] = ";
                if(ModulesXRotations[istation][imodule] == false) cout << "not rotated around X\n";
                if(ModulesXRotations[istation][imodule] == true) cout << "rotated around X\n";
            cout << "\n";
        }
        cout << "-------------------------------------------------------------\n";
    }
    cout << "-------------------------------------------------------------\n";
}


void create_rootgeom_Silicon_Run8_aligned() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "Silicon";
    const TString geoDetectorVersion = "Run8_aligned";
    const TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";

    // ----  global geometry parameters  ---------------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();

    // -------   Load media from media file   ----------------------------------
    TString medFile = gPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    DefineRequiredMedia(geoMedia, geoBuild);
    // -------------------------------------------------------------------------

    //Print information about parameters of SILICON
    PrintInfoAboutModuleParameters();
    PrintInformationAboutStations();

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* SILICON = new TGeoVolumeAssembly(geoDetectorName);
    SILICON->SetMedium(pMedAir);


    //Let's create SILICON stations filled with modules
    for(Int_t istation = 0; istation < NStations; ++istation) {
    //for(Int_t istation = 0; istation < 1; ++istation) {


        //module size in a station
        ModuleSize module_size = STANDARD_MODULE;

        if(istation == 0) module_size = LONG_MODULE;

        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(istation, 10));

        Int_t NModules = NModulesInStations[istation];

        TGeoVolume *modules[NModules];
        TGeoCombiTrans *modTransforms[NModules];

        for(Int_t imodule = 0; imodule < NModules; ++imodule) {

            TString module_name = TString("module") + TString::Itoa(imodule, 10) + TString("_") + station->GetName();
            modules[imodule] = CreateModule(module_name, ModulesTypes[istation][imodule], module_size);

            modTransforms[imodule] = new TGeoCombiTrans();

            //Applying alignment corrections to module shifts
            XModuleShifts[istation][imodule] -= XAlignmentCorrections[istation][imodule];
            YModuleShifts[istation][imodule] -= YAlignmentCorrections[istation][imodule];
            ZModuleShifts[istation][imodule] += ZAlignmentCorrections[istation][imodule];

            if( ModulesYRotations[istation][imodule] ) {
                modTransforms[imodule]->RotateY(180.0);
                modTransforms[imodule]->SetDx(XModuleShifts[istation][imodule] - XShiftFromCenterToLocalX0[module_size]);
            }
            else {
                modTransforms[imodule]->SetDx(XModuleShifts[istation][imodule] + XShiftFromCenterToLocalX0[module_size]);
            }

            if( ModulesXRotations[istation][imodule] ) {
                modTransforms[imodule]->RotateX(180.0);
                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF || ModulesTypes[istation][imodule] == ModuleType::RIGHT_ANGLED_MODULE_HALF) {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] - YModuleSizeUpperPart[module_size]*0.5);
                }
                else {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] + YShiftFromCenterToAxis[module_size] - YShiftFromCenterToLocalY0[module_size]);
                }
            }
            else {
                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF || ModulesTypes[istation][imodule] == ModuleType::RIGHT_ANGLED_MODULE_HALF) {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] + YModuleSizeUpperPart[module_size]*0.5);
                }
                else {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] + YShiftFromCenterToAxis[module_size] + YShiftFromCenterToLocalY0[module_size]);
                }
            }

            Double_t z_module_shift = +ZModuleSize[module_size]*0.5; //To z-module position is at the beginning of the sensitive volume
            modTransforms[imodule]->SetDz(ZModuleShifts[istation][imodule] + z_module_shift);

            //Rotate half-planes
            if(imodule < NModulesInStations[istation]/2) { //modules of upper half-plane
                modTransforms[imodule]->SetDx(modTransforms[imodule]->GetTranslation()[0] - x_shift_hp[istation]);
                modTransforms[imodule]->SetDy(modTransforms[imodule]->GetTranslation()[1] - y_shift_hp[istation]);
                modTransforms[imodule]->SetDz(modTransforms[imodule]->GetTranslation()[2] - z_shift_hp[istation]);

                //modTransforms[imodule]->RotateY(HalfPlanesYRotations[istation][0]);

                modTransforms[imodule]->SetDx(modTransforms[imodule]->GetTranslation()[0] + x_shift_hp[istation]);
                modTransforms[imodule]->SetDy(modTransforms[imodule]->GetTranslation()[1] + y_shift_hp[istation]);
                modTransforms[imodule]->SetDz(modTransforms[imodule]->GetTranslation()[2] + z_shift_hp[istation]);
            }
            else { //modules of lower half-plane
                modTransforms[imodule]->SetDz(ZModuleShifts[istation][imodule] + z_module_shift);
                //modTransforms[imodule]->RotateY(HalfPlanesYRotations[istation][1]);
            }

            cout << "station = " << istation << "\n";
            cout << "module = " << imodule << "\n";
            cout << "  modTransforms[imodule]->GetTranslation()[2] = " << modTransforms[imodule]->GetTranslation()[2] << "\n";
            cout << "  ZStationPositions[istation] = " << ZStationPositions[istation] << "\n";
            station->AddNode(modules[imodule], 0, modTransforms[imodule]);
        }

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[istation], YStationPositions[istation], ZStationPositions[istation]);

        SILICON->AddNode(station, 0, station_transform);
    }


    top->AddNode(SILICON, 0);
    top->SetVisContainers(kTRUE);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.0001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
}
//------------------------------------------------------------------------------


TGeoVolume *CreateStation(TString station_name) {

    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    return stationA;
}

TGeoVolume *CreateModule(TString module_name, ModuleType module_type, ModuleSize module_size) {

    //Shapes -------------------------------------------------------------------
    //module shape
    TGeoShape *moduleS = 0;

    switch(module_type) {
        case RIGHT_ANGLED_MODULE: {
            moduleS = new TGeoBBox("moduleS", XModuleSize[module_size]*0.5, YModuleSize[module_size]*0.5, ZModuleSize[module_size]*0.5);
            break;
        }
        case RIGHT_ANGLED_MODULE_HALF: {
            moduleS = new TGeoBBox("moduleS", XModuleSize[module_size]*0.5, YModuleSizeUpperPart[module_size]*0.5, ZModuleSize[module_size]*0.5);
            break;
        }
        case BEVEL_MODULE: {
            moduleS = new TGeoXtru(2);
            moduleS->SetName("moduleS");
                Double_t x_pos[5] = {+XModuleSize[module_size]*0.5, -XModuleSize[module_size]*0.5, -XModuleSize[module_size]*0.5, +XModuleSize[module_size]*0.5-XBevelSize[module_size], +XModuleSize[module_size]*0.5};
                Double_t y_pos[5] = {+YModuleSize[module_size]*0.5, +YModuleSize[module_size]*0.5, -YModuleSize[module_size]*0.5, -YModuleSize[module_size]*0.5, -YModuleSize[module_size]*0.5+YBevelSize[module_size]};
            ((TGeoXtru*)moduleS)->DefinePolygon(5, x_pos, y_pos);
            ((TGeoXtru*)moduleS)->DefineSection(0, -ZModuleSize[module_size]*0.5);
            ((TGeoXtru*)moduleS)->DefineSection(1, +ZModuleSize[module_size]*0.5);
            break;
        }
        case BEVEL_MODULE_HALF: {
            moduleS = new TGeoXtru(2);
            moduleS->SetName("moduleS");
                Double_t x_pos[5] = {+XModuleSize[module_size]*0.5, -XModuleSize[module_size]*0.5, -XModuleSize[module_size]*0.5, +XModuleSize[module_size]*0.5-XBevelSize[module_size], +XModuleSize[module_size]*0.5};
                Double_t y_pos[5] = {+YModuleSizeUpperPart[module_size]*0.5, +YModuleSizeUpperPart[module_size]*0.5, -YModuleSizeUpperPart[module_size]*0.5, -YModuleSizeUpperPart[module_size]*0.5, -YModuleSizeUpperPart[module_size]*0.5+YBevelSize[module_size]};
            ((TGeoXtru*)moduleS)->DefinePolygon(5, x_pos, y_pos);
            ((TGeoXtru*)moduleS)->DefineSection(0, -ZModuleSize[module_size]*0.5);
            ((TGeoXtru*)moduleS)->DefineSection(1, +ZModuleSize[module_size]*0.5);
            break;
        }
        default:
            Error("CreateModule()", "Incorrect module type selected!!!");
    }

    //sensor zone shape (upper part of the module: rectangle zone)
    TGeoShape *sensor_zone_upper_part_rectS = new TGeoBBox("sensor_zone_upper_part_rectS", XSensorZoneSize[module_size]*0.5, YSensorZoneSizeUpperPart[module_size]*0.5, ZSensorZoneSize[module_size]*0.5);

    //sensor zone shape (upper part of the module: bevel zone)
    TGeoShape *sensor_zone_upper_part_bevelS = new TGeoXtru(2);
        sensor_zone_upper_part_bevelS->SetName("sensor_zone_upper_part_bevelS");
        //Calculate coordinates of the bottom and the top points of the inner bevel
        {
            //find x and y projections of the bevel border thickness
            Double_t x_delta = XYBevelBorder[module_size]/TMath::Cos(BevelAngleRad[module_size]);
            Double_t y_delta = XYBevelBorder[module_size]/TMath::Sin(BevelAngleRad[module_size]);

            //find shifts of the sensitive zone from module borders via the equation of the straight line for the inner bevel
            Double_t x_delta_top_BP = XLeftBorder[module_size];
            Double_t y_delta_top_BP = TMath::Tan(TMath::PiOver2()+BevelAngleRad[module_size])*x_delta_top_BP + (YBevelSize[module_size]+y_delta);

            Double_t y_delta_bottom_BP = YMiddleBorderUpperPart[module_size];
            Double_t x_delta_bottom_BP = (y_delta_bottom_BP - (YBevelSize[module_size] + y_delta))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad[module_size]));
            //----------------------------------------------------------------------

            Double_t x_pos_sensor_zone_bevel[5] = { +XModuleSize[module_size]*0.5-XLeftBorder[module_size], -XModuleSize[module_size]*0.5+XRightBorder[module_size], -XModuleSize[module_size]*0.5+XRightBorder[module_size],       +XModuleSize[module_size]*0.5-x_delta_bottom_BP, +XModuleSize[module_size]*0.5-XLeftBorder[module_size]};
            Double_t y_pos_sensor_zone_bevel[5] = { +YModuleSizeUpperPart[module_size]*0.5-YTopBorder[module_size], +YModuleSizeUpperPart[module_size]*0.5-YTopBorder[module_size], -YModuleSizeUpperPart[module_size]*0.5+YMiddleBorderUpperPart[module_size],      -YModuleSizeUpperPart[module_size]*0.5+YMiddleBorderUpperPart[module_size], -YModuleSizeUpperPart[module_size]*0.5+y_delta_top_BP};

            ((TGeoXtru*)sensor_zone_upper_part_bevelS)->DefinePolygon(5, x_pos_sensor_zone_bevel, y_pos_sensor_zone_bevel);
            ((TGeoXtru*)sensor_zone_upper_part_bevelS)->DefineSection(0, -ZModuleSize[module_size]*0.5);
            ((TGeoXtru*)sensor_zone_upper_part_bevelS)->DefineSection(1, +ZModuleSize[module_size]*0.5);
        }


    //sensor zone shape (lower part of the module: rectangle zone)
    TGeoShape *sensor_zone_lower_part_rectS = new TGeoBBox("sensor_zone_lower_part_rectS", XSensorZoneSize[module_size]*0.5, YSensorZoneSizeLowerPart[module_size]*0.5, ZSensorZoneSize[module_size]*0.5);

    //sensor zone shape (lower part of the module: bevel zone)
    TGeoShape *sensor_zone_lower_part_bevelS = new TGeoXtru(2);
        sensor_zone_lower_part_bevelS->SetName("sensor_zone_lower_part_bevelS");
        //Calculate coordinates of the bottom and the top points of the inner bevel
        {
            //find x and y projections of the bevel border thickness
            Double_t x_delta = XYBevelBorder[module_size]/TMath::Cos(BevelAngleRad[module_size]);
            Double_t y_delta = XYBevelBorder[module_size]/TMath::Sin(BevelAngleRad[module_size]);

            //find shifts of the sensitive zone from module borders via the equation of the straight line for the inner bevel
            Double_t x_delta_top_BP = XLeftBorder[module_size];
            Double_t y_delta_top_BP = TMath::Tan(TMath::PiOver2()+BevelAngleRad[module_size])*x_delta_top_BP + (YBevelSize[module_size]+y_delta);

            Double_t y_delta_bottom_BP = YBottomBorder[module_size];
            Double_t x_delta_bottom_BP = (y_delta_bottom_BP - (YBevelSize[module_size] + y_delta))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad[module_size]));
            //----------------------------------------------------------------------

            Double_t x_pos_sensor_zone_bevel[5] = { +XModuleSize[module_size]*0.5-XLeftBorder[module_size], -XModuleSize[module_size]*0.5+XRightBorder[module_size], -XModuleSize[module_size]*0.5+XRightBorder[module_size],       +XModuleSize[module_size]*0.5-x_delta_bottom_BP, +XModuleSize[module_size]*0.5-XLeftBorder[module_size]};
            Double_t y_pos_sensor_zone_bevel[5] = { +YModuleSizeLowerPart[module_size]*0.5-YMiddleBorderLowerPart[module_size], +YModuleSizeLowerPart[module_size]*0.5-YMiddleBorderLowerPart[module_size], -YModuleSizeLowerPart[module_size]*0.5+YBottomBorder[module_size],      -YModuleSizeLowerPart[module_size]*0.5+YBottomBorder[module_size], -YModuleSizeLowerPart[module_size]*0.5+y_delta_top_BP};

            ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefinePolygon(5, x_pos_sensor_zone_bevel, y_pos_sensor_zone_bevel);
            ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefineSection(0, -ZModuleSize[module_size]*0.5);
            ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefineSection(1, +ZModuleSize[module_size]*0.5);
        }

    //--------------------------------------------------------------------------

    //Volumes ------------------------------------------------------------------
    //module volume
    //TString module_name = "moduleV_";

    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //sensor zone volume (upper part of the module)
    TGeoVolume *sensor_zone_upper_part_rectV = new TGeoVolume("Sensor_zone_upper_part_rectV", sensor_zone_upper_part_rectS);

    //sensor zone volume (lower part of the module)
    TGeoVolume *sensor_zone_upper_part_bevelV = new TGeoVolume("Sensor_zone_upper_part_bevelV", sensor_zone_upper_part_bevelS);

    //sensor zone volume (lower part of the module: rectangle zone)
    TGeoVolume *sensor_zone_lower_part_rectV = new TGeoVolume("Sensor_zone_lower_part_rectV", sensor_zone_lower_part_rectS);

    //sensor zone volume (lower part of the module: bevel zone)
    TGeoVolume *sensor_zone_lower_part_bevelV = new TGeoVolume("Sensor_zone_lower_part_bevelV", sensor_zone_lower_part_bevelS);


    //volumes media
    TGeoMedium *silicon_medium = pMedSilicon;
    if(silicon_medium) {
        moduleV->SetMedium(silicon_medium);
        sensor_zone_upper_part_rectV->SetMedium(silicon_medium);
        sensor_zone_upper_part_bevelV->SetMedium(silicon_medium);
        sensor_zone_lower_part_rectV->SetMedium(silicon_medium);
        sensor_zone_lower_part_bevelV->SetMedium(silicon_medium);
    }
    else Fatal("Main", "Invalid medium for modules and sensor zones!");


    //volumes visual property (transparency)
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    sensor_zone_upper_part_rectV->SetLineColor(TColor::GetColor("#e0b0ff"));
    sensor_zone_upper_part_bevelV->SetLineColor(TColor::GetColor("#e0b0ff"));
    sensor_zone_lower_part_rectV->SetLineColor(TColor::GetColor("#e0b0ff"));
    sensor_zone_lower_part_bevelV->SetLineColor(TColor::GetColor("#e0b0ff"));
    moduleV->SetTransparency(50);


    switch(module_type) {

        case RIGHT_ANGLED_MODULE: {

            //position of the sensor zone in the upper part of the module
            TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
                sensor_zone_upper_part_pos->SetDx(+(XLeftBorder[module_size]+XRightBorder[module_size])*0.5 - XLeftBorder[module_size]);
                sensor_zone_upper_part_pos->SetDy(+YSensorZoneSizeUpperPart[module_size]*0.5 + YGapBetweenPartsOfModule[module_size]*0.5 + YMiddleBorderUpperPart[module_size] - YShiftFromCenterToAxis[module_size]);
                sensor_zone_upper_part_pos->SetDz(0.0);

            //position of the sensor zone in the lower part of the module
            TGeoTranslation *sensor_zone_lower_part_pos = new TGeoTranslation();
                sensor_zone_lower_part_pos->SetDx(+(XLeftBorder[module_size]+XRightBorder[module_size])*0.5 - XLeftBorder[module_size]);
                sensor_zone_lower_part_pos->SetDy(-YSensorZoneSizeLowerPart[module_size]*0.5 - YGapBetweenPartsOfModule[module_size]*0.5 - YMiddleBorderLowerPart[module_size] - YShiftFromCenterToAxis[module_size]);
                sensor_zone_lower_part_pos->SetDz(0.0);

            moduleV->AddNode(sensor_zone_upper_part_rectV, 0, sensor_zone_upper_part_pos);
            moduleV->AddNode(sensor_zone_lower_part_rectV, 0, sensor_zone_lower_part_pos);
            break;
        }

        case RIGHT_ANGLED_MODULE_HALF: {
            //position of the sensor zone in the upper part of the module
            TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
                sensor_zone_upper_part_pos->SetDx(+(XLeftBorder[module_size]+XRightBorder[module_size])*0.5 - XLeftBorder[module_size]);
                sensor_zone_upper_part_pos->SetDy(+(YTopBorder[module_size]+YMiddleBorderUpperPart[module_size])*0.5 - YTopBorder[module_size]);
                sensor_zone_upper_part_pos->SetDz(0.0);

            moduleV->AddNode(sensor_zone_upper_part_rectV, 0, sensor_zone_upper_part_pos);
            break;
        }

        case BEVEL_MODULE: {

            //position of the sensor zone in the upper part of the module
            TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
                sensor_zone_upper_part_pos->SetDx(+(XLeftBorder[module_size]+XRightBorder[module_size])*0.5 - XLeftBorder[module_size]);
                sensor_zone_upper_part_pos->SetDy(+YSensorZoneSizeUpperPart[module_size]*0.5 + YGapBetweenPartsOfModule[module_size]*0.5 + YMiddleBorderUpperPart[module_size] - YShiftFromCenterToAxis[module_size]);
                sensor_zone_upper_part_pos->SetDz(0.0);

            //position of the sensor zone in the lower part of the module
            TGeoTranslation *sensor_zone_lower_part_bevel_pos = new TGeoTranslation();
                sensor_zone_lower_part_bevel_pos->SetDx(0.0);
                sensor_zone_lower_part_bevel_pos->SetDy(-YModuleSizeLowerPart[module_size]*0.5 - YGapBetweenPartsOfModule[module_size]*0.5 - YShiftFromCenterToAxis[module_size]);
                sensor_zone_lower_part_bevel_pos->SetDz(0.0);

            moduleV->AddNode(sensor_zone_upper_part_rectV, 0, sensor_zone_upper_part_pos);
            moduleV->AddNode(sensor_zone_lower_part_bevelV, 0, sensor_zone_lower_part_bevel_pos);
            break;
        }

        case BEVEL_MODULE_HALF: {

            //position of the sensor zone in the upper part of the module
            TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
                sensor_zone_upper_part_pos->SetDx(0);
                sensor_zone_upper_part_pos->SetDy(0);
                sensor_zone_upper_part_pos->SetDz(0.0);

            moduleV->AddNode(sensor_zone_upper_part_bevelV, 0, sensor_zone_upper_part_pos);
            break;
        }

        default:
            Error("CreateModule()", "Incorrect module type selected!!!");
    }

    return moduleV;
}
