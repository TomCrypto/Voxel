#pragma once

#include <cstdlib>

#include <omp.h>

#include <vector>
#include <stdexcept>
#include <functional>

#include "math/vector3.hpp"

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

template <typename Integrator,
          typename Projection,
          typename Subsampler>
void render(Integrator &&integrator,
            Projection &&projection,
            Subsampler &&subsampler,
            Raster &raster)
{
    // integration step (this solves the rendering equation,
    // and is where near 100% of the runtime will be spent)

	// need to rewrite this to use float2's and maybe a ray structure

    float ratio = (float)raster.width() / raster.height();

	#pragma omp parallel for schedule(dynamic, 1)
	for (size_t y = 0; y < raster.height(); ++y)
	{
		for (size_t x = 0; x < raster.width(); ++x)
		{
		    raster[y][x] = math::float3();
		    size_t s = 0;
		    float dx, dy;
		
		    while (subsampler(s++, dx, dy))
		    {
		        float px = ((x + dx) / raster.width() - 0.5f) * 2;
		        float py = ((y + dy) / raster.height() - 0.5f) * 2;
		        
		        // (px, py) in screen space coordinates (NOT accounting for aspect ratio)
		        
		        math::float3 origin, direction;
		        
		        projection(px, py, ratio, origin, direction);

		        raster[y][x] += integrator(origin, direction);
		    }
		    
		    raster[y][x] /= s - 1; // s = index after last sample point, so count is s - 1
		}
	}
	
	// post-processing goes here
}
