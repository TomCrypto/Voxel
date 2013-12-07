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

/** @struct Hit_Info
  *
  * Describes some additional intersection information, such as the material of
  * the intersected geometry, its surface normal, and so on.
**/
typedef struct Hit_Info
{
    struct Basis basis;
} Hit_Info;

/** @struct Geometry
  *
  * Represents the geometry data.
**/
struct Geometry;

/** Performs a range occlusion test which will test if the given ray intersects
  * any geometry up to some range (an infinite range is perfectly valid).
  *
  * @param geometry  The geometry data.
  * @param ray       The incident ray.
  * @param range     The occlusion range.
  *
  * @return \c true if the ray intersects the geometry, at a distance less than
  *         \c range units, \c false otherwise.
**/
bool occludes(global struct Geometry *geometry, const struct Ray ray,
              float range);

/** Performs a range nearest intersection test which will test if the given ray
  * intersects geometry up to some range, and return the nearest intersection.
  *
  * @param geometry  The geometry data.
  * @param ray       The incident ray.
  * @param range     The occlusion range.
  * @param nearest   The nearest intersection (distance along the ray).
  * @param hit_info  The structure containing intersection information.
  *
  * @return \c true if the ray intersects the geometry, at a distance less than
  *         \c range units, \c false otherwise.
  *
  * @remarks If the function returns \c false, the \c *nearest and \c *hit_info
  *          variables are undefined and should not be used.
  *
  * @remarks You may pass zero in \c hit_info if you don't require it (this may
  *          improve performance in some cases).
**/
bool intersects(global struct Geometry *geometry, const struct Ray ray,
                float range, float *nearest, Hit_Info *hit_info);
