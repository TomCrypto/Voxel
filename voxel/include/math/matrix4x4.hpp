#pragma once

#include <iostream>
#include <cassert>
#include <cstddef>

#include "math/vector4.hpp"

namespace math
{
    template <typename Ty>
    struct basic_matrix4x4
    {
	    typedef Ty scalar;

	    basic_matrix4x4()
        {
            col[0] = basic_vector4<scalar>(1, 0, 0, 0);
            col[1] = basic_vector4<scalar>(0, 1, 0, 0);
            col[2] = basic_vector4<scalar>(0, 0, 1, 0);
            col[3] = basic_vector4<scalar>(0, 0, 0, 1);
        }

	    basic_matrix4x4(const basic_matrix4x4 &copy)
	    {
            col = copy.col;
	    }

	    basic_matrix4x4(const basic_vector4<scalar> &a,
                        const basic_vector4<scalar> &b,
                        const basic_vector4<scalar> &c,
                        const basic_vector4<scalar> &d)
	    {
            col[0] = a;
            col[1] = b;
            col[2] = c;
            col[3] = d;
	    }

	    friend basic_matrix4x4 operator +(const basic_matrix4x4 &a,
                                          const basic_matrix4x4 &b)
	    {
            return basic_matrix4x4(a[0] + b[0],
                                   a[1] + b[1],
                                   a[2] + b[2],
                                   a[3] + b[3]);
	    }

	    friend basic_matrix4x4 operator -(const basic_matrix4x4 &a,
                                          const basic_matrix4x4 &b)
	    {
            return basic_matrix4x4(a[0] - b[0],
                                   a[1] - b[1],
                                   a[2] - b[2],
                                   a[3] - b[3]);
	    }

	    friend basic_matrix4x4 operator *(const basic_matrix4x4 &a,
                                          const basic_matrix4x4 &b)
	    {
            basic_matrix4x4 c, aT = transpose(a);

            for (size_t i = 0; i < 4; ++i)
                for (size_t j = 0; j < 4; ++j)
                    c[i][j] = dot(aT[i], b[j]);
		
            return c;
	    }

        friend basic_vector4<scalar> operator *(const basic_matrix4x4 &a,
                                                const basic_vector4<scalar> &b)
        {
            return b[0] * a[0]
                 + b[1] * a[1]
                 + b[2] * a[2]
                 + b[3] * a[3];
        }

	    basic_vector4<scalar> &operator [](size_t t)
        {
            assert(t < 4);
            return col[t];
        }
	    const basic_vector4<scalar> &operator [](size_t t) const
        {
            assert(t < 4);
            return col[t];
        }

        friend basic_matrix4x4 transpose(const basic_matrix4x4 &a)
        {
            return basic_matrix4x4(
                    basic_vector4<scalar>(a[0].x, a[1].x, a[2].x, a[3].x),
                    basic_vector4<scalar>(a[0].y, a[1].y, a[2].y, a[3].y),
                    basic_vector4<scalar>(a[0].z, a[1].z, a[2].z, a[3].z),
                    basic_vector4<scalar>(a[0].w, a[1].w, a[2].w, a[3].w));
        }

        friend std::ostream &operator <<(std::ostream &stream,
                                         const basic_matrix4x4<scalar> &a)
        {
            return stream << "{" << a[0] << ","
                                 << a[1] << ","
                                 << a[2] << ","
                                 << a[3] << "}";
        }

	    static basic_matrix4x4 identity()
        {
            return basic_matrix4x4();
        }

    private:
        basic_vector4<scalar> col[4];
    };

    typedef basic_matrix4x4<float>   float4x4;
    typedef basic_matrix4x4<double> double4x4;
};
