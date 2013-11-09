#include <gtest/gtest.h>

#include "math/vector3.hpp"
#include <cmath>

using namespace math;

/******************************************************************************
****************************** UNIT TESTS FOLLOW ******************************
******************************************************************************/
#define MODULE                                                          vector3

void ASSERT_FLOAT3_EQ(const float3 &u, const float3 &v)
{
    ASSERT_FLOAT_EQ(u.x, v.x);
    ASSERT_FLOAT_EQ(u.y, v.y);
    ASSERT_FLOAT_EQ(u.z, v.z);
}

TEST(MODULE, component_access)
{
    size_t trials = 50000;
    
    for (size_t t = 0; t < trials; ++t)
    {
        float x = 1 * (float)t / trials;
        float y = 2 * (float)t / trials;
        float z = 3 * (float)t / trials;
        
        float3 u(x, y, z);
        
        ASSERT_FLOAT_EQ(x, u.x);
        ASSERT_FLOAT_EQ(y, u.y);
        ASSERT_FLOAT_EQ(z, u.z);
        
        ASSERT_FLOAT_EQ(u.x, u[0]);
        ASSERT_FLOAT_EQ(u.y, u[1]);
        ASSERT_FLOAT_EQ(u.z, u[2]);
    }
}

TEST(MODULE, vector_addition)
{
    size_t trials = 50000;
    size_t terms = 25;
    
    for (size_t t = 0; t < trials; ++t)
    {
        float s1 = 1 * (float)t / trials;
        float s2 = 2 * (float)t / trials;
        float s3 = 3 * (float)t / trials;
        
        float3 sum(0, 0, 0);
        
        for (size_t t = 0; t < terms; ++t)
            sum += float3(s1 * t, s2 * t, s3 * t);
            
        float output = terms * (terms - 1) / 2;

        ASSERT_FLOAT3_EQ(sum, float3(s1 * output,
                                     s2 * output,
                                     s3 * output));
    }
}

TEST(MODULE, vector_subtraction)
{
    size_t trials = 50000;
    
    for (size_t t = 0; t < trials; ++t)
    {
        float x = 1 * (float)t / trials;
        float y = 2 * (float)t / trials;
        float z = 3 * (float)t / trials;
        
        float3 u(+1, +2, +3);
        float3 v(+x, +y, +z);
        float3 w(-x, -y, -z);
        
        ASSERT_FLOAT3_EQ(u - v, u + w);
    }
}

TEST(MODULE, scalar_multiplication)
{
    size_t trials = 50000;
    
    for (size_t t = 0; t < trials; ++t)
    {
        float s = (float)t / trials;
        
        float3 u(1    , 2,     3    );
        float3 v(1 * s, 2 * s, 3 * s);
        
        ASSERT_FLOAT3_EQ(u * s, v);
        ASSERT_FLOAT3_EQ(s * u, v);
    }
}

TEST(MODULE, scalar_division)
{
    size_t trials = 50000;
    
    for (size_t t = 0; t < trials; ++t)
    {
        float s = 1 - (float)t / trials;
        
        float3 u(1    , 2,     3    );
        float3 v(1 / s, 2 / s, 3 / s);
        
        ASSERT_FLOAT3_EQ(u / s, v);
        ASSERT_FLOAT3_EQ(s / u, v);
    }
}

TEST(MODULE, dot_product)
{
    size_t trials = 50000;
    
    for (size_t t = 0; t < trials; ++t)
    {
        float s = (float)t / trials;
    
        float3 u(1 * s, 2 * s, 3 * s);
        float3 v(4 * s, 5 * s, 6 * s);
        float result = 32 * pow(s, 2);
        
        ASSERT_FLOAT_EQ(dot(u, v), result);
        ASSERT_FLOAT_EQ(dot(u, v), dot(v, u));
        ASSERT_FLOAT_EQ(dot(u * s, u), s * dot(u, u));
        ASSERT_FLOAT_EQ(dot(u, u + v), dot(u, u) + dot(u, v));
        ASSERT_FLOAT_EQ(dot(float3(1, 0, 1), float3(0, 1, 0)), 0);
    }
}


