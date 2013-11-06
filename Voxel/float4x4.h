#pragma once

#include <cassert>
#include <cmath>

#include "float4.h"

/* A 4×4 transformation matrix, stored in column-major order. */
struct float4x4
{
    private:
        float4 v[4];

    public:
        /* Constructs the identity matrix. */
        float4x4();

        /* Constructs a matrix from four column vectors. */
        float4x4(const float4& a, const float4& b, const float4& c, const float4& d);

        /* Constructs a matrix from sixteen entries. */
        float4x4(float m00, float m10, float m20, float m30,
                    float m01, float m11, float m21, float m31,
                    float m02, float m12, float m22, float m32,
                    float m03, float m13, float m23, float m33);

        float4 operator[](const size_t& x) const;
};

float4 operator*(const float4x4& a, const float4& u);

float4x4 operator+(const float4x4& a, const float4x4& b);

float4x4 operator-(const float4x4& a, const float4x4& b);

float4x4 transpose(const float4x4& a);

float4x4 operator*(const float4x4& a, const float4x4& b);