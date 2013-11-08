#ifndef VOXEL_MATH_MATRIX4X4_H
#define VOXEL_MATH_MATRIX4X4_H

namespace math
{

template <typename Ty>
struct basic_matrix4x4
{
	typedef Ty scalar;

	basic_matrix4x4() {}
	basic_matrix4x4(const basic_matrix4x4 &copy)
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				(*this)[j][i] = copy[j][i];
	}
	basic_matrix4x4(const scalar *els)
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				(*this)[j][i] = els[j*4+i];
	}

	friend basic_matrix4x4 operator +(const basic_matrix4x4 &a, const basic_matrix4x4 &b)
	{
		// TODO: Optimize, could start with an unroll :)
		basic_matrix4x4 c;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				c[i][j] = a[i][j] + b[i][j];
		return c;
	}
	friend basic_matrix4x4 operator -(const basic_matrix4x4 &a, const basic_matrix4x4 &b)
	{
		// TODO: Optimize, could start with an unroll :)
		basic_matrix4x4 c;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				c[i][j] = a[i][j] - b[i][j];
		return c;
	}
	friend basic_matrix4x4 operator *(const basic_matrix4x4 &a, const basic_matrix4x4 &b)
	{
		// TODO: Optimize, could start with an unroll :)
		basic_matrix4x4 c = zero();
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				c[i][j] += a[i][j] * b[j][i];
		return c;
	}

	scalar *operator [](size_t i) { return els[i]; }
	const scalar *operator [](size_t i) const { return els[i]; }

	static const basic_matrix4x4 zero()
		{ const scalar zeros[16] = {0};
		  return basic_matrix4x4(zeros); }

private:
	scalar els[16];
};

}; // math

#endif // VOXEL_MATH_MATRIX4X4_H
