// 11/6/2013

#ifndef VOXEL_MATH_VECTOR3_H
#define VOXEL_MATH_VECTOR3_H

namespace math
{

template <typename Ty>
struct basic_vector3
{
	typedef Ty scalar;

	basic_vector3() {}
	basic_vector3(const basic_vector3 &copy) : x(copy.x), y(copy.y), z(copy.z) {}
	basic_vector3(scalar x, scalar y, scalar z) : x(x), y(y), z(z) {}

	friend basic_vector3 operator +(const basic_vector3 &a, const basic_vector3 &b)
		{ return basic_vector3(a.x+b.x, a.y+b.y, a.z+b.z); }
	friend basic_vector3 operator -(const basic_vector3 &a, const basic_vector3 &b)
		{ return basic_vector3(a.x-b.x, a.y-b.y, a.z-b.z); }
	friend basic_vector3 operator *(const basic_vector3 &a, scalar b)
		{ return basic_vector3(a.x*b, a.y*b, a.z*b); }
	friend basic_vector3 operator /(const basic_vector3 &a, scalar b)
		{ return basic_vector3(a.x/b, a.y/b, a.z/b); }

	basic_vector3 &operator +=(const basic_vector3 &b)
		{ x+=b.x, y+=b.y, z+=b.z; return *this; }
	basic_vector3 &operator -=(const basic_vector3 &b)
		{ x-=b.x, y-=b.y, z-=b.z; return *this; }
	basic_vector3 &operator *=(scalar b)
		{ x*=b, y*=b, z*=b; return *this; }
	basic_vector3 &operator /=(scalar b)
		{ x/=b, y/=b, z/=b; return *this; }

	scalar &operator [](size_t i) { return (&x)[i]; }
	scalar operator [](size_t i) const { return (&x)[i]; }

	scalar x, y, z;
};

template <typename Ty>
Ty dot(const basic_vector3<Ty> &a, const basic_vector3<Ty> &b)
	{ return a.x*b.x + a.y*b.y + a.z*b.z; }

template <typename Ty>
basic_vector3<Ty> cross(const basic_vector3<Ty> &a, const basic_vector3<Ty> &b)
	{ return basic_vector3<Ty>(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x); }

typedef basic_vector3<float> vector3f;

};

#endif // VOXEL_MATH_VECTOR_H