/** @file material.cl
  *
  * @brief Kernel Material Utilities
**/

#pragma once

// for surface BRDF's

// direct sampling (returns the reflectance for two angles)
float3 reflectance(float NdV, float NdL, read_only image2d_array_t material)
{
    return 0;
}

// importance sampling (takes an incident, a uniform, and returns an exitant
// angle such that angles with the greatest reflectance are preferred. also
// returns said reflectance in the XYZ components, W = exitant angle)
float4 reflectance(float NdV, read_only image2d_array_t sampling)
{
    return 0;
}

// for phase BRDF's

// to be decided
