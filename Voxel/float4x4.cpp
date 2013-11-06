#include "float4x4.h"

float4x4::float4x4()
{
    v[0] = float4(1, 0, 0, 0);
    v[1] = float4(0, 1, 0, 0);
    v[2] = float4(0, 0, 1, 0);
    v[3] = float4(0, 0, 0, 1);
}

float4x4::float4x4(const float4& a, const float4& b, const float4& c, const float4& d)
{
    v[0] = a;
    v[1] = b;
    v[2] = c;
    v[3] = d;
}

float4x4::float4x4(float m00, float m10, float m20, float m30,
            float m01, float m11, float m21, float m31,
            float m02, float m12, float m22, float m32,
            float m03, float m13, float m23, float m33)
{
    v[0] = float4(m00, m01, m02, m03);
    v[1] = float4(m10, m11, m12, m13);
    v[2] = float4(m20, m21, m22, m23);
    v[3] = float4(m30, m31, m32, m33);
}

float4 float4x4::operator[](const size_t& x) const
{
    assert(x < 4);
    return v[x];
}

float4 operator*(const float4x4& a, const float4& u)
{
    return u.x * a[0] + u.y * a[1] + u.z * a[2] + u.w * a[3];
}

float4x4 operator+(const float4x4& a, const float4x4& b)
{
    return float4x4(a[0] + b[0],
                    a[1] + b[1],
                    a[2] + b[2],
                    a[3] + b[3]);
}

float4x4 operator-(const float4x4& a, const float4x4& b)
{
    return float4x4(a[0] - b[0],
                    a[1] - b[1],
                    a[2] - b[2],
                    a[3] - b[3]);
}

float4x4 transpose(const float4x4& a)
{
    return float4x4(a[0][0], a[0][1], a[0][2], a[0][3],
                    a[1][0], a[1][1], a[1][2], a[1][3],
                    a[2][0], a[2][1], a[2][2], a[2][3],
                    a[3][0], a[3][1], a[3][2], a[3][3]);
}

float4x4 operator*(const float4x4& a, const float4x4& b)
{
    float4x4 aT = transpose(a);

    return float4x4(dot(aT[0], b[0]), dot(aT[0], b[1]), dot(aT[0], b[2]), dot(aT[0], b[3]),
                    dot(aT[1], b[0]), dot(aT[1], b[1]), dot(aT[1], b[2]), dot(aT[1], b[3]),
                    dot(aT[2], b[0]), dot(aT[2], b[1]), dot(aT[2], b[2]), dot(aT[2], b[3]),
                    dot(aT[3], b[0]), dot(aT[3], b[1]), dot(aT[3], b[2]), dot(aT[3], b[3]));
}