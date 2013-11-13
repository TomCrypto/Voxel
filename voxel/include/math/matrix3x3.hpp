#pragma once

#include <iostream>
#include <cassert>
#include <cstddef>

#include "math/vector3.hpp"

namespace math
{
    template <typename Ty>
    struct basic_matrix3x3
    {
	    typedef Ty scalar;

	    basic_matrix3x3()
        {
            col[0] = basic_vector3<scalar>(1, 0, 0);
            col[1] = basic_vector3<scalar>(0, 1, 0);
            col[2] = basic_vector3<scalar>(0, 0, 1);
        }

	    basic_matrix3x3(const basic_matrix3x3 &copy)
	    {
            col[0] = copy.col[0];
            col[1] = copy.col[1];
            col[2] = copy.col[2];
	    }

	    basic_matrix3x3(const basic_vector3<scalar> &a,
                        const basic_vector3<scalar> &b,
                        const basic_vector3<scalar> &c)
	    {
            col[0] = a;
            col[1] = b;
            col[2] = c;
	    }

	    friend basic_matrix3x3 operator +(const basic_matrix3x3 &a,
                                          const basic_matrix3x3 &b)
	    {
            return basic_matrix3x3(a[0] + b[0],
                                   a[1] + b[1],
                                   a[2] + b[2]);
	    }

	    friend basic_matrix3x3 operator -(const basic_matrix3x3 &a,
                                          const basic_matrix3x3 &b)
	    {
            return basic_matrix3x3(a[0] - b[0],
                                   a[1] - b[1],
                                   a[2] - b[2]);
	    }

	    friend basic_matrix3x3 operator *(const basic_matrix3x3 &a,
                                          const basic_matrix3x3 &b)
	    {
            basic_matrix3x3 c, aT = transpose(a);

            for (size_t i = 0; i < 3; ++i)
                for (size_t j = 0; j < 3; ++j)
                    c[i][j] = dot(aT[i], b[j]);
		
            return c;
	    }

        friend basic_vector3<scalar> operator *(const basic_matrix3x3 &a,
                                                const basic_vector3<scalar> &b)
        {
            return b[0] * a[0]
                 + b[1] * a[1]
                 + b[2] * a[2];
        }

	    basic_vector3<scalar> &operator [](size_t t)
        {
            assert(t < 3);
            return col[t];
        }
	    const basic_vector3<scalar> &operator [](size_t t) const
        {
            assert(t < 3);
            return col[t];
        }

        friend basic_matrix3x3 transpose(const basic_matrix3x3 &a)
        {
            return basic_matrix3x3(
                    basic_vector3<scalar>(a[0].x, a[1].x, a[2].x),
                    basic_vector3<scalar>(a[0].y, a[1].y, a[2].y),
                    basic_vector3<scalar>(a[0].z, a[1].z, a[2].z));
        }

        friend std::ostream &operator <<(std::ostream &stream,
                                         const basic_matrix3x3<scalar> &a)
        {
            return stream << "{" << a[0] << ","
                                 << a[1] << ","
                                 << a[2] << "}";
        }

	    static basic_matrix3x3 identity()
        {
            return basic_matrix3x3();
        }

    private:
        basic_vector3<scalar> col[4];
    };

    typedef basic_matrix3x3<float>   float3x3;
    typedef basic_matrix3x3<double> double3x3;
};
