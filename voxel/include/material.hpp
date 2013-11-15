#pragma once

#include "math/vector3.hpp"

/** @struct Material
  *
  * Defines the materials encountered by rays during ray-geometry intersection,
  * and describes the appearance of objects in the world.
**/
struct Material
{
    math::float3 rgb; // TEMPORARY: for now, just a solid color
    // TODO: later on, this will be a single integer indexing a large material
    // database. material lookup and calculations are not a bottleneck anyway
};

// NOT SURE ABOUT THIS YET
// need to consider how materials are going to work and how to mix spectral and RGB
// rendering (maybe going RGB only would be sufficient for the scope of this project)
// and also how to implement different BRDF's and phase functions efficiently without
// incurring insane branching costs (a fully data-driven approach may not be flexible
// enough, but perhaps we can compromise)

#if 0

// two types of light-matter interaction:
// SURFACE: surface_* (take a surface normal)
// SCATTER: scatter_* (take no surface normal)
// direct -> returns the reflectance (evaluates the BRDF) for an incident and exitant direction
// sample -> samples an exitant direction from an incident direction such that the probability
//           that a given exitant direction E is selected is equal to direct(E)

float brdf_direct(const uint16_t &material, math::float3 incident, math::float3 exitant, math::float3 normal, float wavelength)
{
    ///
}

math::float3 exitant brdf_sample(const uint16_t &material, math::float3 incident, math::float3 normal, float wavelength)
{
    ///
}

#endif
