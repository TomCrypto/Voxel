#pragma once

// provides common utility math stuff

#include "math/matrix3x3.hpp"
#include "math/vector3.hpp"

namespace math
{
    template <typename scalar>
    math::basic_vector3<scalar> spherical(scalar phi, scalar theta)
    {
        return math::basic_vector3<scalar>(sin(theta) * cos(phi),
                                           cos(theta),
                                           sin(theta) * sin(phi));
    }

    // computes an upright orthonormal basis from a "forward" direction
    // (the basis is returned as a 3x3 column matrix)
    template <typename scalar>
    basic_matrix3x3<scalar> basis(const basic_vector3<scalar> &forward)
    {
        basic_vector3<scalar> up(0, 1, 0);

        auto z = normalize(forward);
        auto x = normalize(cross(up, z));
        auto y = normalize(cross(z, x));

        return basic_matrix3x3<scalar>(x, y, z);
    }
};
