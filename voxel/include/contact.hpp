#pragma once

#include "math/common.hpp"
#include "math/vector3.hpp"

#include <cstdint>
#include <cmath>

struct Contact
{
    uint16_t normal;
    uint16_t material;
};

static math::float3 normals[65536];

inline uint16_t encode_normal(const math::float3 &normal)
{
    float phi = atan2(normal.z, normal.x);
    if (phi < 0) phi += 2 * M_PI;
    float theta = acos(normal.y);
    
    uint8_t p = (uint8_t)(phi / (2 * M_PI) * 255);
    uint8_t q = (uint8_t)(theta / M_PI * 255);
    
    return (p << 8) + q;
}

inline math::float3 decode_normal(const uint16_t &normal)
{
    return normals[normal];
}

inline math::float3 decode_normal_slow(const uint16_t &normal)
{
    uint8_t x = normal >> 8;
    uint8_t y = normal & 255;

    float phi = (float)x / 255 * 2 * M_PI;
    float theta = (float)y / 255 * M_PI;
    
    if (phi > M_PI) phi -= 2 * M_PI;
    
    return math::spherical(phi, theta);
}

static void build_table()
{
    for (int t = 0; t < 65536; ++t) normals[t] = decode_normal_slow(t);
}

/** KNOWN FAST

struct Contact
{
    math::float3 normal;
    //uint16_t normal;
    uint16_t material;
};

inline math::float3 encode_normal(const math::float3 &normal)
{
    return normal;
}

inline math::float3 decode_normal(const math::float3 &normal)
{
    return normal;
}

**/

// using 16-bit normals make decoding far too expensive
// (maybe an alternative encoding scheme that is expensive to
//  encode but very fast to decode could be used)
// alternatively, just build a table of all 65536 normals
// and index into it instead of recalculating it...

// (need to measure how many voxels we can really fit in memory,
//  and how many are realistically needed)

#if 0
inline uint16_t encode_normal(const math::float3 &normal)
{
    float phi = atan2(normal.z, normal.x);
    if (phi < 0) phi += 2 * M_PI;
    float theta = acos(normal.y);
    
    uint8_t p = (uint8_t)(phi / (2 * M_PI) * 255);
    uint8_t q = (uint8_t)(theta / M_PI * 255);
    
    return (p << 8) + q;
}

inline math::float3 decode_normal(const uint16_t &normal)
{
    uint8_t x = normal >> 8;
    uint8_t y = normal & 255;

    float phi = (float)x / 255 * 2 * M_PI;
    float theta = (float)y / 255 * M_PI;
    
    if (phi > M_PI) phi -= 2 * M_PI;
    
    return math::spherical(phi, theta);
}
#endif
