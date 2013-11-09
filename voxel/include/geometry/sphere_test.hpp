#pragma once

/* A basic geometry class which simply tests against a single sphere, for
 * testing purposes only. */

#include "math/vector3.hpp"
#include <cmath>

#include <algorithm>

struct Contact
{
    bool hit;
};

using namespace math;

class SphereTest
{
    private:
        static float RaySphere(float3 origin, float3 direction)
        {
            const float3 spherePos(0, 0, 2);
            const float radius2 = 1 * 1;

            origin = spherePos - origin;
            float b = dot(origin, direction);
            float det = b * b - dot(origin, origin) + radius2;

            if (det >= 0.0f)
            {
                float l = sqrt(det);
                float p = b - l, q = b + l;
                if (p < 1e-5f) return q;
                if (q < 1e-5f) return p;
                return std::min(p, q);
            }

            return -1.0f;

        }

    public:
        bool traverse(float3 origin, float3 direction,
                      float *distance, Contact *contact) const
        {
            *distance = RaySphere(origin, direction);
            contact->hit = (*distance > 0);
            return contact->hit;
        }
};
