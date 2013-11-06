#pragma once

#include "constants.h"
#include <cassert>
#include <cmath>

/* A four-dimensional vector. */
struct float4
{
	public:
        union
        {
            struct
            {
		        float x, y, z, w;
            };

            float c[4];
        };

        /* Constructs the zero vector. */
        float4();

        /* Constructs a vector from four components. */
		float4(float x, float y, float z, float w);

        float operator[](const size_t& t) const;

        const float4& operator+=(const float4& u);

        const float4& operator-=(const float4& u);
};

float4 operator+(const float4& u, const float4& v);

float4 operator-(const float4& u, const float4& v);

float4 operator*(const float4& u, const float& s);

float4 operator*(const float& s, const float4& u);

float dot(const float4& u, const float4& v);

/******************************************************************************
********************************** UNIT TESTS *********************************
******************************************************************************/

#include "testing.h"

namespace Testing
{
    inline bool equals(float a, float b)
    {
        return (abs(a - b) <= EPS);
    }

    inline bool equals(const float4& a, const float4& b)
    {
        for (size_t t = 0; t < 4; ++t)
            if (!equals(a[t], b[t])) return false;

        return true;
    }

    inline bool float4_access()
    {
        float4 a = float4(1, 2, 3, 4);
        if (!equals(a.x, 1)) return false;
        if (!equals(a.y, 2)) return false;
        if (!equals(a.z, 3)) return false;
        if (!equals(a.w, 4)) return false;
        if (!equals(a[0], a.x)) return false;
        if (!equals(a[1], a.y)) return false;
        if (!equals(a[2], a.z)) return false;
        if (!equals(a[3], a.w)) return false;

        TEST_ASSERT(equals(a[3], a.w));

        return true;
    }

    inline bool float4_add()
    {
        if (!equals(float4(0, 0, 0, 0) + float4(1, 2, 3, 4), float4(1, 2, 3, 4))) return false;
        if (!equals(float4(1, 1, 1, 1) + float4(2, 2, 2, 2), float4(3, 3, 3, 3))) return false;

        if (!equals(float4(1, 2, 3, 4) + float4(2, 3, 4, 1) + float4(3, 4, 1, 2),
                    float4(2, 3, 4, 1) + float4(1, 2, 3, 4) + float4(3, 4, 1, 2))) return false; // commutativity

        return true;
    }

    inline bool float4_sub()
    {
        if (!equals(float4(0, 0, 0, 0) - float4(1, 2, 3, 4), float4(-1, -2, -3, -4))) return false;
        if (!equals(float4(1, 1, 1, 1) - float4(2, 2, 2, 2), float4(-1, -1, -1, -1))) return false;

        return true;
    }
}