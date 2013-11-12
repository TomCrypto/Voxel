#pragma once

#include <iostream>
#include <cassert>
#include <cmath>

namespace math
{
    template <typename Ty>
    struct basic_vector3
    {
	    typedef Ty scalar;

	    basic_vector3()
            : x(0), y(0), z(0)
        {

        }

	    basic_vector3(const basic_vector3 &copy)
		    : x(copy.x), y(copy.y), z(copy.z)
        {

        }

	    basic_vector3(scalar x, scalar y, scalar z)
		    : x(x), y(y), z(z)
        {

        }

	    friend basic_vector3 operator +(const basic_vector3 &a,
                                        const basic_vector3 &b)
		{
            return basic_vector3(a.x + b.x,
                                 a.y + b.y,
                                 a.z + b.z);
        }

	    friend basic_vector3 operator -(const basic_vector3 &a,
                                        const basic_vector3 &b)
        {
            return basic_vector3(a.x - b.x,
                                 a.y - b.y,
                                 a.z - b.z);
        }

	    friend basic_vector3 operator *(const basic_vector3 &a,
                                        const scalar &b)
        {
		    return basic_vector3(a.x * b,
                                 a.y * b,
                                 a.z * b);
        }

	    friend basic_vector3 operator *(const scalar &b,
                                        const basic_vector3 &a)
        {
            return basic_vector3(a.x * b,
                                 a.y * b,
                                 a.z * b);
        }

	    friend basic_vector3 operator /(const basic_vector3 &a,
                                        const scalar &b)
		{
            return basic_vector3(a.x / b,
                                 a.y / b,
                                 a.z / b);
        }

        friend basic_vector3 operator *(const basic_vector3 &a,
                                        const basic_vector3 &b)
	    {
            return basic_vector3(a.x * b.x,
                                 a.y * b.y,
                                 a.z * b.z);
        }

        friend basic_vector3 operator /(const basic_vector3 &a,
                                        const basic_vector3 &b)
	    {
            return basic_vector3(a.x / b.x,
                                 a.y / b.y,
                                 a.z / b.z);
        }

	    basic_vector3 &operator =(const basic_vector3 &b)
	    {
            x = b.x;
            y = b.y;
            z = b.z;
            
            return *this;
        }
	    basic_vector3 &operator +=(const basic_vector3 &b)
		{
            x += b.x;
            y += b.y;
            z += b.z;
            
            return *this;
        }

	    basic_vector3 &operator -=(const basic_vector3 &b)
	    {
            x -= b.x;
            y -= b.y;
            z -= b.z;
            
            return *this;
        }

	    basic_vector3 &operator *=(const scalar &b)
	    {
            x *= b;
            y *= b;
            z *= b;
            
            return *this;
        }

	    basic_vector3 &operator /=(const scalar &b)
	    {
            x /= b;
            y /= b;
            z /= b;
            
            return *this;
        }

        basic_vector3 &operator *=(const basic_vector3 &b)
	    {
            x *= b.x;
            y *= b.y;
            z *= b.z;
            
            return *this;
        }

        basic_vector3 &operator /=(const basic_vector3 &b)
	    {
            x /= b.x;
            y /= b.y;
            z /= b.z;
            
            return *this;
        }

	    scalar &operator [](size_t t)
        {
            assert(t < 3);
            return row[t];
        }

	    const scalar &operator [](size_t t) const
        {
            assert(t < 3);
            return row[t];
        }

	    friend bool operator ==(const basic_vector3 &a,
                                const basic_vector3 &b)
	    {
            return a.x == b.x
                && a.y == b.y
                && a.z == b.z;
        }

	    friend bool operator !=(const basic_vector3 &a,
                                const basic_vector3 &b)
	    {
            return a.x != b.x
                || a.y != b.y
                || a.z != b.z;
        }

	    union
	    {
	        struct { scalar x, y, z; };
	        scalar row[3];
	    };
    };

    template <typename scalar>
    scalar dot(const basic_vector3<scalar> &a,
               const basic_vector3<scalar> &b)
    {
        return a.x * b.x
             + a.y * b.y
             + a.z * b.z;
    }

    template <typename scalar>
    scalar length(const basic_vector3<scalar> &a)
    {
        return sqrt(dot(a, a));
    }

    template <typename scalar>
    basic_vector3<scalar> normalize(const basic_vector3<scalar> &a)
	{
        return a / length(a);
    }
    
    template <typename scalar>
    basic_vector3<scalar> cross(const basic_vector3<scalar> &a,
                                const basic_vector3<scalar> &b)
    {
        return basic_vector3<scalar>(a.y * b.z - b.y * a.z,
                                     b.x * a.z - a.x * b.z,
                                     a.x * b.y - a.y * b.x);
    }

    template <typename scalar>
    std::ostream &operator <<(std::ostream &stream,
                              const basic_vector3<scalar> &a)
    {
        return stream << "{" << a[0] << ","
                             << a[1] << ","
                             << a[2] << "}";
    }

    /*************************************************************************/

    #include <emmintrin.h>
    #include <pmmintrin.h>
    #include <smmintrin.h>

    template<>
    struct basic_vector3<float>
    {
        basic_vector3()
            : sse(_mm_set_ps(0, 0, 0, 0))
        {
            
        }
        
	    basic_vector3(const basic_vector3 &copy)
		    : sse(copy.sse)
		{
		    
		}
		
	    basic_vector3(float x, float y, float z)
		    : sse(_mm_set_ps(0, z, y, x))
		{
		    
		}
		
        basic_vector3(const __m128& sse)
            : sse(sse)
        {
            
        }

	    friend basic_vector3 operator +(const basic_vector3 &a,
                                        const basic_vector3 &b)
	    {
            return _mm_add_ps(a.sse, b.sse);
        }

	    friend basic_vector3 operator -(const basic_vector3 &a,
                                        const basic_vector3 &b)
	    {
            return _mm_sub_ps(a.sse, b.sse);
        }

	    friend basic_vector3 operator *(const basic_vector3 &a,
                                        float b)
	    {
            return _mm_mul_ps(a.sse, _mm_set_ps(b, b, b, b));
        }

	    friend basic_vector3 operator *(float b,
                                        const basic_vector3 &a)
	    {
            return _mm_mul_ps(a.sse, _mm_set_ps(b, b, b, b));
        }

	    friend basic_vector3 operator /(const basic_vector3 &a,
                                        float b)
	    {
            return _mm_div_ps(a.sse, _mm_set_ps(b, b, b, b));
        }

        friend basic_vector3 operator *(const basic_vector3 &a,
                                        const basic_vector3 &b)
	    {
            return _mm_mul_ps(a.sse, b.sse);
        }
		
        friend basic_vector3 operator /(const basic_vector3 &a,
                                        const basic_vector3 &b)
	    {
            return _mm_div_ps(a.sse, b.sse);
        }

	    basic_vector3 &operator =(const basic_vector3 &b)
	    {
            sse = b.sse;
            return *this;
        }

	    basic_vector3 &operator +=(const basic_vector3 &b)
	    {
            sse = _mm_add_ps(sse, b.sse);
            return *this;
        }

	    basic_vector3 &operator -=(const basic_vector3 &b)
	    {
            sse = _mm_sub_ps(sse, b.sse);
            return *this;
        }

	    basic_vector3 &operator *=(float b)
	    {
            sse = _mm_mul_ps(sse, _mm_set_ps(b, b, b, b));
            return *this;
        }

	    basic_vector3 &operator /=(float b)
	    {
            sse = _mm_div_ps(sse, _mm_set_ps(b, b, b, b));
            return *this;
        }

        basic_vector3 &operator *=(basic_vector3 b)
	    {
            sse = _mm_mul_ps(sse, b.sse);
            return *this;
        }

        basic_vector3 &operator /=(basic_vector3 b)
	    {
            sse = _mm_div_ps(sse, b.sse);
            return *this;
        }

	    float &operator [](size_t t)
        {
            assert(t < 3);
            return (&x)[t];
        }

	    const float &operator [](size_t t) const
        {
            assert(t < 3);
            return (&x)[t];
        }

	    friend bool operator ==(const basic_vector3 &a,
                                const basic_vector3 &b)
	    {
            return a.x == b.x
                && a.y == b.y
                && a.z == b.z;
        }

	    friend bool operator !=(const basic_vector3 &a,
                                const basic_vector3 &b)
	    {
            return a.x != b.x
                || a.y != b.y
                || a.z != b.z;
        }

	    union
	    {
	        struct { float x, y, z; };
	        __m128 sse;
	    };
    };

    template <>
    float dot<float>(const basic_vector3<float> &a,
                     const basic_vector3<float> &b)
    {
        float result;
    
        _mm_store_ss(&result, _mm_dp_ps(a.sse, b.sse, 0x77));

        return result;
    }
    
    template <>
    basic_vector3<float> cross<float>(const basic_vector3<float> &a,
                                      const basic_vector3<float> &b)
    {
        static const int swizzle1 = _MM_SHUFFLE(3, 0, 2, 1);
        static const int swizzle2 = _MM_SHUFFLE(3, 1, 0, 2);

        __m128 v1 = _mm_shuffle_ps(a.sse, a.sse, swizzle1);
        __m128 v2 = _mm_shuffle_ps(b.sse, b.sse, swizzle2);
        __m128 v3 = _mm_shuffle_ps(a.sse, a.sse, swizzle2);
        __m128 v4 = _mm_shuffle_ps(b.sse, b.sse, swizzle1);

        __m128 p1 = _mm_mul_ps(v1, v2);
        __m128 p2 = _mm_mul_ps(v3, v4);

        return basic_vector3<float>(_mm_sub_ps(p1, p2));
    }

    typedef basic_vector3<float>   float3;
    typedef basic_vector3<double> double3;
};
