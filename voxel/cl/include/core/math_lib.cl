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

#include <core/prng_lib.cl>

/** Represents the color white.
**/
#define C_WHITE (float3)(1, 1, 1)

/** Represents the color black.
**/
#define C_BLACK (float3)(0, 0, 0)

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

/** This function is the same as \c intersect_f(), except it allows the user to
  * pass the (presumably precomputed) inverse direction of the ray (for speed).
  *
  * @remarks This function ignores \c ray.d.
  *
  * @see \c #intersect_f()
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

/** @struct Basis
  *
  * A basis, represented as a 3x3 TBN matrix (the surface normal is \c n).
**/
struct Basis
{
    float3 t;
    float3 n;
    float3 b;
};

/** This function is the same as \c box_basis_f(), except it allows the user to
  * pass the (presumably precomputed) inverse direction of the ray (for speed).
  *
  * @see #box_basis_f()
**/
struct Basis box_basis(const struct Ray ray, const struct Box box,
                  const float3 inv_dir);

/** Returns the surface basis of a box, at the intersection point made by a ray
  * with said box (this will thus only return a axis-aligned basis).
  *
  * @param ray  The ray.
  * @param box  The box.
  *
  * @return The axis-aligned basis.
  *
  * @remarks The basis is defined only if the ray actually intersects the box.
**/
struct Basis box_basis_f(const struct Ray ray, const struct Box box);

/** Transforms a vector by a 3x3 linear transformation matrix.
  *
  * @param v      The vector to transform.
  * @param basis  The basis to transform the vector by.
  *
  * @return The transformed vector.
**/
float3 transform(float3 v, struct Basis basis);

/** Produces an exitant vector, in normal space, according to a cosine-weighted
  * distribution (following Lambert's cosine law).
  *
  * @return A cosine-weighted vector.
  *
  * @remarks The returned vector will probably have to be transformed according
  *          to some basis, e.g. the surface normal it is emitted relative to.
  *
  * @param prng  A pseudorandom number generator instance.
**/
float3 cosine(struct PRNG *prng);
