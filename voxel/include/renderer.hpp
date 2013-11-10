#pragma once

#include <cstdlib>

#include <omp.h>

#include <vector>
#include <stdexcept>
#include <functional>

#include "math/vector3.hpp"

//#include "projection.hpp"
//#include "integrator.hpp"

// A screen raster, which holds a pixel buffer
struct Raster
{
	Raster(size_t width, size_t height)
	  : m_width(width), m_height(height), m_data(width * height)
	{
	}

	const math::float3 *operator[](size_t y) const
		{ return &m_data[y * m_width]; }
	math::float3 *operator[](size_t y)
		{ return &m_data[y * m_width]; }

	size_t width() const { return m_width; }
	size_t height() const { return m_height; }

private:
	const size_t m_width, m_height;
	std::vector<math::float3> m_data;
};

template <typename IntegratorTy, typename ProjectionTy>
void render(Raster &raster, IntegratorTy &&integrator,
            ProjectionTy &&projection)
{
	// this is where the rendering happens:
	// 1. project a camera ray for every pixel
	// (according to some subpixel sampling distribution)
	// 2. integrate the camera ray to assign a color
	// 3. post-process as needed
	// 4. output the rest

	// this is just a test render
	#pragma omp parallel for schedule(dynamic, 1)
	for (size_t y = 0; y < raster.height(); ++y)
	{
		for (size_t x = 0; x < raster.width(); ++x)
		{
		    float px = ((float)x / raster.width() - 0.5f) * 2;
		    float py = ((float)y / raster.height() - 0.5f) * 2;
		    
		    // (px, py) in [-1..1] (clip space)
		    
		    math::float3 origin, direction;
		    
		    projection(px, py, &origin, &direction);

		    raster[y][x] = integrator(origin, direction);
		}
	}
}
