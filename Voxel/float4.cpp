#include "float4.h"

float4::float4()
    : x(0), y(0), z(0), w(0)
{

}

float4::float4(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w)
{

}

float float4::operator[](const size_t& t) const
{
    assert(t < 4);
    return c[t];
}

const float4& float4::operator+=(const float4& u)
{
    x += u.x;
    y += u.y;
    z += u.z;
    w += u.w;

    return *this;
}

const float4& float4::operator-=(const float4& u)
{
    x -= u.x;
    y -= u.y;
    z -= u.z;
    w -= u.w;

    return *this;
}

float4 operator+(const float4& u, const float4& v)
{
    return float4(u.x + v.x,
				  u.y + v.y,
                  u.z + v.z,
                  u.w + v.w);
}

float4 operator-(const float4& u, const float4& v)
{
    return float4(u.x - v.x,
				  u.y - v.y,
                  u.z - v.z,
                  u.w - v.w);
}

float4 operator*(const float4& u, const float& s)
{
    return float4(u.x + s,
				  u.y + s,
                  u.z + s,
                  u.w + s);
}

float4 operator*(const float& s, const float4& u)
{
    return float4(u.x + s,
				  u.y + s,
                  u.z + s,
                  u.w + s);
}

float dot(const float4& u, const float4& v)
{
    return u.x * v.x
         + u.y * v.y
         + u.z * v.z
         + u.w * v.w;
}