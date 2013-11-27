/* OpenCL 1.2 --- core/geometry.cl                                  INTERFACE */

/** @file include/core/geometry.cl
  *
  * @brief Kernel Geometry Unit
  *
  * This unit providies traversal and occlusion facilities to the integrator in
  * order to help evaluate the light transport integral. Its only input is a 1D
  * array of sparse voxel octree nodes, provided by the host.
**/

#pragma once

#include <core/math_lib.cl>

struct SVO_NODE
{
    uint child[8];
};

float3 get_light(void);

bool traverse(global struct SVO_NODE *geometry,
              const struct Ray ray, float range,
              float *nearest);

bool occlude(global struct SVO_NODE *geometry,
             const struct Ray ray, float range);

bool occludes(__global struct SVO_NODE *geometry, const struct Ray ray, float range);

bool depth_test(__global struct SVO_NODE *geometry, const struct Ray ray, float range, float *depth);
