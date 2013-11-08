#pragma once

#include <cstdlib>

#include <vector>
#include <cassert>

//#include "projection.hpp"
//#include "integrator.hpp"

// this needs to be improved to be able to have different precisions
// and perhaps different color systems (if applicable)
struct Pixel
{
	float r, g, b, a;
};

// A screen raster, which holds a pixel buffer
struct Raster
{
	private:
		std::vector<Pixel> m_data;

	public:
		Raster(size_t width, size_t height) : width(width), height(height)
		{
			assert((width > 0) && (height > 0));
			m_data.reserve(width * height);
		}
		
		const Pixel& operator()(size_t x, size_t y) const
		{
			assert((x < width) && (y < height));
			return m_data[y * width + x];
		}
		
		Pixel& operator()(size_t x, size_t y)
		{
			assert((x < width) && (y < height));
			return m_data[y * width + x];
		}
		
		const size_t width, height;
};

/* Indicative interface for the Renderer */

class Renderer
{
	private:
	    Raster *raster;
	
	    /*const Projection& projection;
	    const Integrator& integrator;*/

	public:
		Renderer(size_t width, size_t height/*,
                 const Projection& projection,
                 const Integrator& integrator*/);
                 
        ~Renderer();

        // Renders the frame, and returns a raster of pixels (TBD)
		const Raster& render() const;
};
