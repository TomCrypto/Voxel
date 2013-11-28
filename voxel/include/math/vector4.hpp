#pragma once

#include <iostream>
#include <cassert>
#include <cstddef>
#include <cmath>

#include <emmintrin.h>
#include <pmmintrin.h>
#include <smmintrin.h>

namespace math
{
    /** @struct basic_vector4
      *
      * A four-dimensional vector which handles arbitrary scalar types. As long
      * as the scalar type forms (or at least approximates) a field, the vector
      * implementation will work.
    **/
    template <typename Ty>
    struct basic_vector4
    {
	    typedef Ty scalar;

	    basic_vector4()
            : x(0), y(0), z(0), w(0)
        {

        }

	    basic_vector4(const basic_vector4 &copy)
		    : x(copy.x), y(copy.y), z(copy.z), w(copy.w)
        {

        }

	    basic_vector4(scalar x, scalar y, scalar z, scalar w)
		    : x(x), y(y), z(z), w(w)
        {

        }

	    friend basic_vector4 operator +(const basic_vector4 &a,
                                        const basic_vector4 &b)
		{
            return basic_vector4(a.x + b.x,
                                 a.y + b.y,
                                 a.z + b.z,
                                 a.w + b.w);
        }

	    friend basic_vector4 operator -(const basic_vector4 &a,
                                        const basic_vector4 &b)
        {
            return basic_vector4(a.x - b.x,
                                 a.y - b.y,
                                 a.z - b.z,
                                 a.w - b.w);
        }

	    friend basic_vector4 operator *(const basic_vector4 &a,
                                        const scalar &b)
        {
		    return basic_vector4(a.x * b,
                                 a.y * b,
                                 a.z * b,
                                 a.w * b);
        }

	    friend basic_vector4 operator *(const scalar &b,
                                        const basic_vector4 &a)
        {
            return basic_vector4(a.x * b,
                                 a.y * b,
                                 a.z * b,
                                 a.w * b);
        }

	    friend basic_vector4 operator /(const basic_vector4 &a,
                                        const scalar &b)
		{
            return basic_vector4(a.x / b,
                                 a.y / b,
                                 a.z / b,
                                 a.w / b);
        }

        friend basic_vector4 operator *(const basic_vector4 &a,
                                        const basic_vector4 &b)
	    {
            return basic_vector4(a.x * b.x,
                                 a.y * b.y,
                                 a.z * b.z,
                                 a.w * b.w);
        }

        friend basic_vector4 operator /(const basic_vector4 &a,
                                        const basic_vector4 &b)
	    {
            return basic_vector4(a.x / b.x,
                                 a.y / b.y,
                                 a.z / b.z,
                                 a.w / b.w);
        }

        friend basic_vector4 operator -(const basic_vector4 &a)
        {
            return basic_vector4(-a.x,
                                 -a.y,
                                 -a.z,
                                 -a.w);
        }

	    basic_vector4 &operator =(const basic_vector4 &b)
	    {
            x = b.x;
            y = b.y;
            z = b.z;
            w = b.w;

            return *this;
        }
	    basic_vector4 &operator +=(const basic_vector4 &b)
		{
            x += b.x;
            y += b.y;
            z += b.z;
            w += b.w;

            return *this;
        }

	    basic_vector4 &operator -=(const basic_vector4 &b)
	    {
            x -= b.x;
            y -= b.y;
            z -= b.z;
            w -= b.w;

            return *this;
        }

	    basic_vector4 &operator *=(const scalar &b)
	    {
            x *= b;
            y *= b;
            z *= b;
            w *= b;

            return *this;
        }

	    basic_vector4 &operator /=(const scalar &b)
	    {
            x /= b;
            y /= b;
            z /= b;
            w /= b;

            return *this;
        }

        basic_vector4 &operator *=(const basic_vector4 &b)
	    {
            x *= b.x;
            y *= b.y;
            z *= b.z;
            w *= b.w;

            return *this;
        }

        basic_vector4 &operator /=(const basic_vector4 &b)
	    {
            x /= b.x;
            y /= b.y;
            z /= b.z;
            w /= b.w;

            return *this;
        }

	    scalar &operator [](size_t t)
        {
            assert(t < 4);
            return row[t];
        }

	    const scalar &operator [](size_t t) const
        {
            assert(t < 4);
            return row[t];
        }

	    friend bool operator ==(const basic_vector4 &a,
                                const basic_vector4 &b)
	    {
            return a.x == b.x
                && a.y == b.y
                && a.z == b.z
                && a.w == b.w;
        }

	    friend bool operator !=(const basic_vector4 &a,
                                const basic_vector4 &b)
	    {
            return a.x != b.x
                || a.y != b.y
                || a.z != b.z
                || a.w != b.w;
        }

	    union
	    {
	        struct { scalar x, y, z, w; };
	        scalar row[4];
	    };
    };

    template <typename scalar>
    scalar dot(const basic_vector4<scalar> &a,
               const basic_vector4<scalar> &b)
    {
        return a.x * b.x
             + a.y * b.y
             + a.z * b.z
             + a.w * b.w;
    }

    template <typename scalar>
    scalar length(const basic_vector4<scalar> &a)
    {
        return sqrt(dot(a, a));
    }

    template <typename scalar>
    basic_vector4<scalar> normalize(const basic_vector4<scalar> &a)
	{
        return a / length(a);
    }

    template <typename scalar>
    std::ostream &operator <<(std::ostream &stream,
                              const basic_vector4<scalar> &a)
    {
        return stream << "{" << a[0] << ","
                             << a[1] << ","
                             << a[2] << ","
                             << a[3] << "}";
    }

    /** @struct basic_vector4<float>
      *
      * The following code specializes the \c basic_vector4 implementation with
      * single-precision floating-point scalar type, and uses SIMD instructions
      * to improve performance.
      *
      * @remarks The typedef \c float4 is defined for convenience.
    **/
    template<>
    struct basic_vector4<float>
    {
        basic_vector4()
            : sse(_mm_set_ps(0, 0, 0, 0))
        {

        }

	    basic_vector4(const basic_vector4 &copy)
		    : sse(copy.sse)
		{

		}

	    basic_vector4(float x, float y, float z, float w)
		    : sse(_mm_set_ps(w, z, y, x))
		{

		}

        basic_vector4(const __m128& sse)
            : sse(sse)
        {

        }

	    friend basic_vector4 operator +(const basic_vector4 &a,
                                        const basic_vector4 &b)
	    {
            return _mm_add_ps(a.sse, b.sse);
        }

	    friend basic_vector4 operator -(const basic_vector4 &a,
                                        const basic_vector4 &b)
	    {
            return _mm_sub_ps(a.sse, b.sse);
        }

	    friend basic_vector4 operator *(const basic_vector4 &a,
                                        float b)
	    {
            return _mm_mul_ps(a.sse, _mm_set_ps(b, b, b, b));
        }

	    friend basic_vector4 operator *(float b,
                                        const basic_vector4 &a)
	    {
            return _mm_mul_ps(a.sse, _mm_set_ps(b, b, b, b));
        }

	    friend basic_vector4 operator /(const basic_vector4 &a,
                                        float b)
	    {
            return _mm_div_ps(a.sse, _mm_set_ps(b, b, b, b));
        }

        friend basic_vector4 operator *(const basic_vector4 &a,
                                        const basic_vector4 &b)
	    {
            return _mm_mul_ps(a.sse, b.sse);
        }

        friend basic_vector4 operator /(const basic_vector4 &a,
                                        const basic_vector4 &b)
	    {
            return _mm_div_ps(a.sse, b.sse);
        }

        friend basic_vector4 operator -(const basic_vector4 &a)
        {
            return _mm_sub_ps(_mm_set_ps(0, 0, 0, 0), a.sse);
        }

	    basic_vector4 &operator =(const basic_vector4 &b)
	    {
            sse = b.sse;
            return *this;
        }

	    basic_vector4 &operator +=(const basic_vector4 &b)
	    {
            sse = _mm_add_ps(sse, b.sse);
            return *this;
        }

	    basic_vector4 &operator -=(const basic_vector4 &b)
	    {
            sse = _mm_sub_ps(sse, b.sse);
            return *this;
        }

	    basic_vector4 &operator *=(float b)
	    {
            sse = _mm_mul_ps(sse, _mm_set_ps(b, b, b, b));
            return *this;
        }

	    basic_vector4 &operator /=(float b)
	    {
            sse = _mm_div_ps(sse, _mm_set_ps(b, b, b, b));
            return *this;
        }

        basic_vector4 &operator *=(basic_vector4 b)
	    {
            sse = _mm_mul_ps(sse, b.sse);
            return *this;
        }

        basic_vector4 &operator /=(basic_vector4 b)
	    {
            sse = _mm_div_ps(sse, b.sse);
            return *this;
        }

	    float &operator [](size_t t)
        {
            assert(t < 4);
            return (&x)[t];
        }

	    const float &operator [](size_t t) const
        {
            assert(t < 4);
            return (&x)[t];
        }

	    friend bool operator ==(const basic_vector4 &a,
                                const basic_vector4 &b)
	    {
            return a.x == b.x
                && a.y == b.y
                && a.z == b.z
                && a.w == b.w;
        }

	    friend bool operator !=(const basic_vector4 &a,
                                const basic_vector4 &b)
	    {
            return a.x != b.x
                || a.y != b.y
                || a.z != b.z
                || a.w != b.w;
        }

	    union
	    {
	        struct { float x, y, z, w; };
	        __m128 sse;
	    };
    };

    template <>
    inline
    float dot<float>(const basic_vector4<float> &a,
                     const basic_vector4<float> &b)
    {
        float result;

        _mm_store_ss(&result, _mm_dp_ps(a.sse, b.sse, 0x77));

        return result;
    }

    typedef basic_vector4<float>   float4;
    typedef basic_vector4<double> double4;
};

namespace std
{
    template <typename scalar>
    math::basic_vector4<scalar> min(const math::basic_vector4<scalar> &a,
                                    const math::basic_vector4<scalar> &b)
    {
        return math::basic_vector4<scalar>(min(a.x, b.x),
                                           min(a.y, b.y),
                                           min(a.z, b.z),
                                           min(a.w, b.w));
    }

    template <typename scalar>
    math::basic_vector4<scalar> max(const math::basic_vector4<scalar> &a,
                                    const math::basic_vector4<scalar> &b)
    {
        return math::basic_vector4<scalar>(max(a.x, b.x),
                                           max(a.y, b.y),
                                           max(a.z, b.z),
                                           max(a.w, b.w));
    }

    template <>
    inline
    math::basic_vector4<float> min(const math::basic_vector4<float> &a,
                                   const math::basic_vector4<float> &b)
    {
        return math::basic_vector4<float>(_mm_min_ps(a.sse, b.sse));
    }

    template <>
    inline
    math::basic_vector4<float> max(const math::basic_vector4<float> &a,
                                   const math::basic_vector4<float> &b)
    {
        return math::basic_vector4<float>(_mm_max_ps(a.sse, b.sse));
    }
};
