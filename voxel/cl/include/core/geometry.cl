/* OpenCL 1.2 --- core/geometry.cl                                  INTERFACE */

/** @file include/core/geometry.cl
  *
  * @brief Kernel Geometry Unit
  *
  * This unit provides traversal and occlusion facilities to the integrator, in
  * order to help evaluate the light transport integral. Its only input is a 1D
  * array of sparse voxel octree nodes, provided by the host.
**/

#pragma once

#include <core/math_lib.cl>

typedef struct Contact
{
    float3 normal;
} Contact;

struct Geometry;

float3 get_light(void);

bool traverse(global struct Geometry *geometry,
              const struct Ray ray, float range,
              float *nearest, Contact *contact);

bool occlude(global struct Geometry *geometry,
             const struct Ray ray, float range);

bool occludes(__global struct Geometry *geometry, const struct Ray ray, float range);

bool depth_test(__global struct Geometry *geometry, const struct Ray ray, float range, float *depth, Contact *contact);
