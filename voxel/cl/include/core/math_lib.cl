/* OpenCL 1.2 --- core/math_lib.cl                                  INTERFACE */

/** @file include/core/math_lib.cl
  *
  * @brief Kernel Math Library
  *
  * This file provides math structures and functions useful to the rest of this
  * OpenCL program. For instance, it defines rays, axis-aligned bounding boxes,
  * and several useful utility functions.
**/

#pragma once

/** Performs a linear interpolation on two vectors.
  *
  * @param a  The first vector.
  * @param b  The second vector.
  * @param t  The interpolation factor.
  *
  * @return The interpolated vector.
**/
float3 lerp(float3 a, float3 b, float t);

/** @struct Ray
  *
  * Defines a ray, with an origin and a (normalized) direction.
**/
struct Ray
{
    float3 o, d;
};

/** Advances a ray to a certain distance and then changes its direction.
  *
  * @param ray  The ray to modify.
  * @param t    The distance along to ray to which to advance.
  * @param d    The new direction of the ray (normalized).
**/
void advance(struct Ray *ray, float t, float3 d);

/** @struct Box
  *
  * An axis-aligned bounding box, represented by two corners.
**/
struct Box
{
    float3 l, h;
};

/** Subdivides a bounding box, returning one of the eight octants.
  *
  * @param box    The box to subdivide.
  * @param index  The index of the octant.
  *
  * @return The corresponding octant of the box.
**/
struct Box subdivide(const struct Box box, size_t index);

/** This function is the same as \c intersect(), except that it allows the user
  * to pass the (presumably precomputed) inverse direction of the ray, that is,
  * the ray's direction, with all components inverted.
  *
  * @remarks This function ignores \c ray.d.
  *
  * @see \c intersect()
**/
bool intersect(const struct Ray ray, const struct Box box,
               const float3 inv_dir, float *t);

/** Perfoms a ray-box intersection test.
  *
  * @param ray  The ray.
  * @param box  The box.
  * @param t    The near intersection distance.
  *
  * @return \c true if an intersection occurs.
  *
  * @remarks If the function returns \c false, \c *t is undefined.
**/
bool intersect_f(const struct Ray ray, const struct Box box,
                 float *t);

