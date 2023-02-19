/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#ifndef NOVAPHYSICS_H
#define NOVAPHYSICS_H


/**
 * heatphysics.h
 * 
 * Main include header
 */


// Stringify macro
#define _NV_STRINGIFY(x) #x
#define NV_STRINGIFY(x) _NV_STRINGIFY(x)

// Version in MAJOR.MINOR.PATCH format
#define NV_VERSION_MAJOR 0
#define NV_VERSION_MINOR 3
#define NV_VERSION_PATCH 1
// Version string
#define NV_VERSTR                          \
        NV_STRINGIFY(NV_VERSION_MAJOR) "." \
        NV_STRINGIFY(NV_VERSION_MINOR) "." \
        NV_STRINGIFY(NV_VERSION_PATCH)


#include "./vector.h"
#include "./math.h"
#include "./aabb.h"
#include "./constants.h"
#include "./space.h"
#include "./body.h"
#include "./material.h"
#include "./constraint.h"
#include "./resolution.h"
#include "./collision.h"
#include "./contact.h"
#include "./solver.h"


#endif