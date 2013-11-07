#pragma once

#include "math/vector3.hpp"

#include "testing.hpp"

namespace Testing
{
    inline bool vector3_dot(void)
    {
        math::basic_vector3<float> u(1, 2, 3);
        math::basic_vector3<float> v(4, 5, 6);
    
        TEST_ASSERT(abs(math::dot(u, v) - (1*4+2*5+3*6)) < 1e-5);
    
        return true;
    }
}
