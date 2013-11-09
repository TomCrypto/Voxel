#pragma once

#include "math/vector3.hpp"

TEST(vector3test, dot)
{
    math::basic_vector3<float> u(1, 2, 3);
    math::basic_vector3<float> v(4, 5, 6);

    ASSERT_EQ(math::dot(u, v), (1*4+2*5+3*6));
}
