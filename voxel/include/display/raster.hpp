#pragma once

#include "math/vector3.hpp"
#include <vector>
#include <cstddef>

struct color { unsigned char r, g, b, a; };

// A screen raster, which holds a pixel buffer
struct Raster
{
	Raster(size_t width, size_t height)
	  : m_width(width), m_height(height), m_data(width * height)
	{
	}

	const color *operator[](size_t y) const
		{ return &m_data[y * m_width]; }
	color *operator[](size_t y)
		{ return &m_data[y * m_width]; }

	size_t width() const { return m_width; }
	size_t height() const { return m_height; }
	const color *pixels() const { return &m_data[0]; };
	color *pixels() { return &m_data[0]; };

private:
	const size_t m_width, m_height;
	std::vector<color> m_data;
};
