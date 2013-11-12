#include <gtest/gtest.h>

#include <cmath>

#include "math/vector3.hpp"

using namespace math;

/******************************************************************************
****************************** UNIT TESTS FOLLOW ******************************
******************************************************************************/
#define MODULE                                                          vector3

template <typename scalar>
void ASSERT_VECTOR_EQ(const basic_vector3<scalar> &u,
                      const basic_vector3<scalar> &v)
{
    ASSERT_FLOAT_EQ(u.x, v.x);
    ASSERT_FLOAT_EQ(u.y, v.y);
    ASSERT_FLOAT_EQ(u.z, v.z);
}

TEST(MODULE, float_component_access)
{
    float3 u(1, 2, 3);
    ASSERT_FLOAT_EQ(u[0], 1);
    ASSERT_FLOAT_EQ(u[1], 2);
    ASSERT_FLOAT_EQ(u[2], 3);
    ASSERT_FLOAT_EQ(u.x, 1);
    ASSERT_FLOAT_EQ(u.y, 2);
    ASSERT_FLOAT_EQ(u.z, 3);
}

TEST(MODULE, float_vector_addition_in_place)
{
    float3 u(1, 2, 3);
    float3 v(4, 5, 6);
    u += v;
    ASSERT_VECTOR_EQ(u, float3(5, 7, 9));
}

TEST(MODULE, float_vector_subtraction_in_place)
{
    float3 u(1, 2, 3);
    float3 v(4, 5, 6);
    u -= v;
    ASSERT_VECTOR_EQ(u, float3(-3, -3, -3));
}

TEST(MODULE, float_vector_multiplication_in_place)
{
    float3 u(1, 2, 3);
    float3 v(4, 5, 6);
    u *= v;
    ASSERT_VECTOR_EQ(u, float3(4, 10, 18));
}

TEST(MODULE, float_vector_division_in_place)
{
    float3 u(1, 2, 3);
    float3 v(4, 5, 6);
    u /= v;
    ASSERT_VECTOR_EQ(u, float3(1.0f / 4.0f, 2.0f / 5.0f, 3.0f / 6.0f));
}

TEST(MODULE, float_scalar_multiplication_in_place)
{
    float3 u(1, 2, 3);
    float s = 4;
    u *= s;
    ASSERT_VECTOR_EQ(u, float3(4, 8, 12));
}

TEST(MODULE, float_scalar_division_in_place)
{
    float3 u(1, 2, 3);
    float s = 4;
    u /= s;
    ASSERT_VECTOR_EQ(u, float3(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f));
}

TEST(MODULE, float_vector_addition_constructive)
{
    float3 u(1, 2, 3);
    float3 v(4, 5, 6);
    ASSERT_VECTOR_EQ(u + v, float3(5, 7, 9));
}

TEST(MODULE, float_vector_subtraction_constructive)
{
    float3 u(1, 2, 3);
    float3 v(4, 5, 6);
    ASSERT_VECTOR_EQ(u - v, float3(-3, -3, -3));
}

TEST(MODULE, float_vector_multiplication_constructive)
{
    float3 u(1, 2, 3);
    float3 v(4, 5, 6);
    ASSERT_VECTOR_EQ(u * v, float3(4, 10, 18));
}

TEST(MODULE, float_vector_division_constructive)
{
    float3 u(1, 2, 3);
    float3 v(4, 5, 6);
    ASSERT_VECTOR_EQ(u / v, float3(1.0f / 4.0f, 2.0f / 5.0f, 3.0f / 6.0f));
}

TEST(MODULE, float_scalar_multiplication_constructive)
{
    float3 u(1, 2, 3);
    float s = 4;
    ASSERT_VECTOR_EQ(u * s, float3(4, 8, 12));
}

TEST(MODULE, float_scalar_division_constructive)
{
    float3 u(1, 2, 3);
    float s = 4;
    ASSERT_VECTOR_EQ(u / s, float3(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f));
}

TEST(MODULE, float_dot)
{
    float3 u(1, 2, 3);
    float3 v(4, 5, 6);
    ASSERT_FLOAT_EQ(dot(u, v), 32);
}

TEST(MODULE, float_length)
{
    float3 u(1, 2, 3);
    ASSERT_FLOAT_EQ(length(u), 3.74165738677394138558f);
}

TEST(MODULE, float_normalize)
{
    float3 u(1, 2, 3);
    ASSERT_VECTOR_EQ(normalize(u), float3(0.267261242f,
                                          0.534522484f,
                                          0.801783726f));
}

TEST(MODULE, float_cross)
{
    float3 u(1, 2, 3);
    float3 v(4, 5, 6);
    ASSERT_VECTOR_EQ(cross(u, v), float3(-3, 6, -3));
}

TEST(MODULE, double_component_access)
{
    double3 u(1, 2, 3);
    ASSERT_DOUBLE_EQ(u[0], 1);
    ASSERT_DOUBLE_EQ(u[1], 2);
    ASSERT_DOUBLE_EQ(u[2], 3);
    ASSERT_DOUBLE_EQ(u.x, 1);
    ASSERT_DOUBLE_EQ(u.y, 2);
    ASSERT_DOUBLE_EQ(u.z, 3);
}

TEST(MODULE, double_vector_addition_in_place)
{
    double3 u(1, 2, 3);
    double3 v(4, 5, 6);
    u += v;
    ASSERT_VECTOR_EQ(u, double3(5, 7, 9));
}

TEST(MODULE, double_vector_subtraction_in_place)
{
    double3 u(1, 2, 3);
    double3 v(4, 5, 6);
    u -= v;
    ASSERT_VECTOR_EQ(u, double3(-3, -3, -3));
}

TEST(MODULE, double_vector_multiplication_in_place)
{
    double3 u(1, 2, 3);
    double3 v(4, 5, 6);
    u *= v;
    ASSERT_VECTOR_EQ(u, double3(4, 10, 18));
}

TEST(MODULE, double_vector_division_in_place)
{
    double3 u(1, 2, 3);
    double3 v(4, 5, 6);
    u /= v;
    ASSERT_VECTOR_EQ(u, double3(1.0 / 4.0, 2.0 / 5.0, 3.0 / 6.0));
}

TEST(MODULE, double_scalar_multiplication_in_place)
{
    double3 u(1, 2, 3);
    double s = 4;
    u *= s;
    ASSERT_VECTOR_EQ(u, double3(4, 8, 12));
}

TEST(MODULE, double_scalar_division_in_place)
{
    double3 u(1, 2, 3);
    double s = 4;
    u /= s;
    ASSERT_VECTOR_EQ(u, double3(1.0 / 4.0, 2.0 / 4.0, 3.0 / 4.0));
}

TEST(MODULE, double_vector_addition_constructive)
{
    double3 u(1, 2, 3);
    double3 v(4, 5, 6);
    ASSERT_VECTOR_EQ(u + v, double3(5, 7, 9));
}

TEST(MODULE, double_vector_subtraction_constructive)
{
    double3 u(1, 2, 3);
    double3 v(4, 5, 6);
    ASSERT_VECTOR_EQ(u - v, double3(-3, -3, -3));
}

TEST(MODULE, double_vector_multiplication_constructive)
{
    double3 u(1, 2, 3);
    double3 v(4, 5, 6);
    ASSERT_VECTOR_EQ(u * v, double3(4, 10, 18));
}

TEST(MODULE, double_vector_division_constructive)
{
    double3 u(1, 2, 3);
    double3 v(4, 5, 6);
    ASSERT_VECTOR_EQ(u / v, double3(1.0 / 4.0, 2.0 / 5.0, 3.0 / 6.0));
}

TEST(MODULE, double_scalar_multiplication_constructive)
{
    double3 u(1, 2, 3);
    double s = 4;
    ASSERT_VECTOR_EQ(u * s, double3(4, 8, 12));
}

TEST(MODULE, double_scalar_division_constructive)
{
    double3 u(1, 2, 3);
    double s = 4;
    ASSERT_VECTOR_EQ(u / s, double3(1.0 / 4.0, 2.0 / 4.0, 3.0 / 4.0));
}

TEST(MODULE, double_dot)
{
    double3 u(1, 2, 3);
    double3 v(4, 5, 6);
    ASSERT_DOUBLE_EQ(dot(u, v), 32);
}

TEST(MODULE, double_length)
{
    double3 u(1, 2, 3);
    ASSERT_DOUBLE_EQ(length(u), 3.74165738677394138558);
}

TEST(MODULE, double_normalize)
{
    double3 u(1, 2, 3);
    ASSERT_VECTOR_EQ(normalize(u), double3(0.267261242,
                                           0.534522484,
                                           0.801783726));
}

TEST(MODULE, double_cross)
{
    double3 u(1, 2, 3);
    double3 v(4, 5, 6);
    ASSERT_VECTOR_EQ(cross(u, v), double3(-3, 6, -3));
}



