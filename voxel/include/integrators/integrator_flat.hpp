#pragma once

/* Default flat-color integrator, which returns white if and only if
 * the ray hits something (anything) and black otherwise. */

#include "geometry/sphere_test.hpp"

class FlatIntegrator
{
    private:
        const SphereTest *geometry;

    public:
        FlatIntegrator(const SphereTest *geometry)
        {
            this->geometry = geometry;
        }
    
        float3 integrate(float3 origin, float3 direction)
        {
            float distance;
            Contact contact;
        
            if (geometry->traverse(origin, direction, &distance, &contact))
            {
                return float3(1, 1, 1);
            }
            else
            {
                return float3(0, 0, 0);
            }
        }
};
