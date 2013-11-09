#include <gtest/gtest.h>

#include "math/vector3.hpp"

#define GROUP vector3

/* UNIT TESTS FOLLOW */

TEST(GROUP, eq)
{
    math::basic_vector3<float> u(1.38584f * 1e-35f,
                                 3.58681f * 1e-28f,
                                 2.12202f * 1e-41f);

    math::basic_vector3<float> v = u * (0.5f - 1e-42f) + u * 1e-42f + u * 0.5f;

    ASSERT_EQ(u, v);
}

TEST(GROUP, add)
{
    math::basic_vector3<float> u(1, 2, 3);
    math::basic_vector3<float> v(4, 5, 6);
    math::basic_vector3<float> w(5, 7, 9);

    ASSERT_EQ(w, u + v);
}

TEST(GROUP, sub)
{
    math::basic_vector3<float> u(1, 2, 3);
    math::basic_vector3<float> v(4, 5, 6);
    math::basic_vector3<float> w(-3, -3, -3);

    ASSERT_EQ(w, u - v);
}

TEST(GROUP, dot)
{
    math::basic_vector3<float> u(1, 2, 3);
    math::basic_vector3<float> v(4, 5, 6);

    ASSERT_EQ(math::dot(u, v), (1*4+2*5+3*6));
}
