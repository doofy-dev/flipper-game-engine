/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#ifndef NOVAPHYSICS_CONSTANTS_H
#define NOVAPHYSICS_CONSTANTS_H

#include <math.h>


/**
 * constants.h
 * 
 * Constants
 */


#define NV_PI 3.141592653589793238462643383279502884


#ifndef INFINITY
    #define NV_INF (1.0 / 0.0)
#else
    #define NV_INF INFINITY
#endif


#define NV_NEARLY_EQUAL_THRESHOLD 0.0005 // 0.5 mm


#define NV_CORRECTION_SLOP 0.002 // 2 mm
#define NV_CORRECTION_PERCENT 0.8 // 80%


#define NV_GRAV_CONST 6.6743e-11 // G = 6.6743 * 10⁻¹¹
#define NV_GRAV_SCALE 1.0e11


#define NV_GRAV_EARTH 9.81
#define NV_GRAV_MOON 1.62
#define NV_GRAV_MARS 3.7
#define NV_GRAV_JUPITER 24.5
#define NV_GRAV_SUN 275.0
#define NV_GRAV_VOID 0.0


#endif