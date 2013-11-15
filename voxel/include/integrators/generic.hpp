#pragma once

/** @file integrators/generic.hpp
  * 
  * @brief Generic Integrators
  *
  * Provides access to the most common integrators. 
**/

/** @todo At the moment some of the basic integrators rely
  *       on HDR, but as there is no HDR implemented yet
  *       they have little tweaks in them to keep them in
  *       the visible range - this will be fixed later.
  *
  * @todo Start thinking about how to handle lights flexibly.
  *
  * @todo Colors should not be float3's. Use an abstraction
  *       which can handle different color spaces and ranges.
**/

#include <algorithm>
#include <cmath>

#include <cstdint>

#include "math/common.hpp"
#include "math/vector3.hpp"

#include "contact.hpp"

/** @namespace integrators
  * 
  * @brief Namespace for the integrators.
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
  * this camera ray. Integrators may call themselves recursively if they wish.
**/
namespace integrators
{
    // later this will probably compute some kind of BRDF
    math::float3 decode_material(uint16_t material)
    {
        switch (material)
        {
            case 0: return math::float3(0.25, 0.75, 0.25);
            case 1: return math::float3(0.55, 0.75, 0.55);
            case 2: return math::float3(1, 1, 1);
            case 3: return math::float3(0.25, 0.25, 0.75);
            case 4: return math::float3(0.75, 0.25, 0.25);
            case 5: return math::float3(0.75, 0.75, 0.25);
            case 6: return math::float3(0.25, 0.75, 0.75);
        }
        
        return math::float3(0, 0, 0);
    }

    /** Returns a flat color corresponding to the voxel color.
    **/
    template <typename Geometry>
    math::float3 flat(const Geometry &geometry,
                      const math::float3 &origin,
                      const math::float3 &direction)
    {
	    float distance;
	    Contact contact;

	    if (geometry.intersects(origin, direction, distance, contact))
	    {
            return decode_material(contact.material);
	    }
	    else
	    {
	        return math::float3(0, 0, 0);
	    }
    }

    /** Returns the "depth" of each pixel (distance of the intersection from
      * the camera.
    **/
    template <typename Geometry>
    math::float3 depth(const Geometry &geometry,
                       const math::float3 &origin,
                       const math::float3 &direction)
    {
	    float distance;
	    Contact contact;

	    if (geometry.intersects(origin, direction, distance, contact))
	    {
            distance = std::max(0.0f, 1 - distance * 0.5f);
            return decode_material(contact.material) * distance;
	    }
	    else
	    {
	        return math::float3(0, 0, 0);
	    }
    }
    
    inline math::float3 saturate(const math::float3 &x)
    {
        return std::min(math::float3(1, 1, 1),
               std::max(math::float3(0, 0, 0),
               x));
    }

    /** Implements simple direct lighting with a single point light source.
      * This will be refactored in the future once we have the lighting
      * sorted out to handle arbitrarily many lights.
    **/
    template <typename Geometry>
    math::float3 direct(const Geometry &geometry,
                        const math::float3 &origin,
                        const math::float3 &direction)
    {
	    float distance;
	    Contact contact;

	    if (geometry.intersects(origin, direction, distance, contact))
	    {
	        math::float3 rgb = decode_material(contact.material);
	        math::float3 normal = decode_normal(contact.normal);
	    
	        // compute hit point (push back outside the object a little)
	        math::float3 hit = origin + direction * distance
                             + 1e-5f * normal;

	        const float ambient = 0.25f;
	        
	        math::float3 dir_to_light = geometry.light() - hit;
	        float distance_to_light = length(dir_to_light);
	        dir_to_light /= distance_to_light;
	        
	        if (geometry.occludes(hit, dir_to_light, distance_to_light))
            {
                return saturate(ambient * rgb); // light is occluded
            }
	        else
	        {
                float NdotL = dot(normal, dir_to_light);
                float falloff = pow(distance_to_light, 2) * 8.5f;
	            float diffuse = std::max(0.0f, NdotL) / falloff;
	            
	            return saturate((ambient + diffuse) * rgb);
	        }
		
        }
	    else return math::float3(0, 0, 0); // no intersection
    }
    
    /** Performs an occlusion test and returns white if an object is hit, black
      * otherwise (occlusion map).
    **/
    template <typename Geometry>
    math::float3 occlusion(const Geometry &geometry,
                           const math::float3 &origin,
                           const math::float3 &direction)
    {
        float range = std::numeric_limits<float>::infinity();
	    if (geometry.occludes(origin, direction, range))
	    {
	        return math::float3(1, 1, 1);
	    }
	    else
	    {
	        return math::float3(0, 0, 0);
	    }
    }
};
