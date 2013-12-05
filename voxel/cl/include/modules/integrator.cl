/* OpenCL 1.2 --- modules/integrator.cl                             INTERFACE */

/** @file integrator.cl
  *
  * @brief Kernel Integrator Interface
  *
  * An integrator is a function which takes the geometry of the world (equipped
  * with efficient ray intersection and occlusion algorithms) and calculates an
  * approximation to the "light transport equation", which describes the steady
  * state flow of light in the world - said approximation can be as accurate as
  * the integrator desires, therefore integrators have a lot of freedom.
  *
  * Integrators are such named because in their purest form, the equation which
  * they attempt to solve is an integral. However, as noted further above, they
  * can range from unphysical integrators, such as computing depth or occlusion
  * maps, to full-blown photorealistic integrators.
  *
  * Integrators take as an input the geometry and a camera ray, and will return
  * an RGB color describing the color and intensity of the light perceived from
  * this camera ray.
**/

#pragma once

#include <core/geometry.cl>
#include <core/math_lib.cl>
#include <core/prng_lib.cl>

/** Integrates perceived light intensity along a camera ray.
  *
  * @remarks This will be called multiple times and the results averaged.
  *
  * @param geometry  The geometry data.
  * @param ray       The camera ray.
  *
  * @return A vector representing RGB intensity along this ray.
**/
float3 integrate(struct Ray ray, global struct Geometry *geometry,
                 struct PRNG *prng);
