#pragma once

#include <cstdlib>

#include <vector>
#include <stdexcept>

#include "math/vector3.hpp"

//#include "projection.hpp"
//#include "integrator.hpp"

// this needs to be improved to be able to have different precisions
// and perhaps different color systems (if applicable)
struct color
{
	float r, g, b, a;
};

// A screen raster, which holds a pixel buffer
struct Raster
{
	Raster(size_t width, size_t height)
	  : m_width(width), m_height(height)
	{
		m_data.resize(m_width * m_height);
	}

	const color *operator[](size_t y) const
		{ return &m_data[y * m_width]; }
	color *operator[](size_t y)
		{ return &m_data[y * m_width]; }

	size_t width() const { return m_width; }
	size_t height() const { return m_height; }

private:
	std::vector<color> m_data;
	const size_t m_width, m_height;
};

/* Indicative interface for the Renderer */

template <typename Intr, typename Geom>
struct Renderer
{
	Renderer(const Intr &integrator)
	  : integrator(geometry)
        {
        }

        // Renders the frame, and returns a raster of pixels (TBD)
	void render(Raster &raster) const
        {
		// this is where the rendering happens:
		// 1. project a camera ray for every pixel
		// (according to some subpixel sampling distribution)
		// 2. integrate the camera ray to assign a color
		// 3. post-process as needed
		// 4. output the rest

		// this is just a test render
		for (size_t y = 0; y < raster.height(); ++y) {
			for (size_t x = 0; x < raster.width(); ++x) {
			    float px = ((float)x / raster.width() - 0.5f) * 2;
			    float py = ((float)y / raster.height() - 0.5f) * 2;
			    
			    math::float3 cam_dir = normalize(math::float3(px, py, 0.5f));
			    math::float3 cam_pos(0, 0, 0);
			    
			    math::float3 color = integrator.integrate(cam_pos, cam_dir);
			    
			    raster[y][x].r = color.x;
			    raster[y][x].g = color.y;
			    raster[y][x].b = color.z;
			}
		}
        }

private:
	Geom geometry;
	Intr integrator;
};
