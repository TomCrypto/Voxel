#pragma once

#include <cstdlib>

#include <vector>
#include <cassert>

#include "geometry/sphere_test.hpp"

#include "compile_settings.hpp"

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

template <typename Integrator>
class Renderer
{
	private:
	    Raster *raster;
	    
	    SphereTest *geometry;
	    Integrator *integrator;
	
	    /*const Projection& projection;
	    const Integrator& integrator;*/

	public:
		Renderer(size_t width, size_t height/*,
                 const Projection& projection,
                 const Integrator& integrator*/)
        {
            raster = new Raster(width, height);
            
            geometry = new SphereTest();
            
            integrator = new FlatIntegrator(geometry);
        }
                 
        ~Renderer()
        {
            delete integrator;

            delete geometry;

            delete raster;
        }

        // Renders the frame, and returns a raster of pixels (TBD)
		const Raster& render() const
        {
            // this is where the rendering happens:
            // 1. project a camera ray for every pixel
            // (according to some subpixel sampling distribution)
            // 2. integrate the camera ray to assign a color
            // 3. post-process as needed
            // 4. output the rest

            // this is just a test render
            
            for (size_t y = 0; y < raster->height; ++y)
                for (size_t x = 0; x < raster->width; ++x)
                {
                    float px = ((float)x / raster->width - 0.5f) * 2;
                    float py = ((float)y / raster->height - 0.5f) * 2;
                    
                    float3 camDir = normalize(float3(px, py, 0.5f));
                    float3 camPos(0, 0, 0);
                    
                    float3 color = integrator->integrate(camPos, camDir);
                    
                    (*raster)(x, y).r = color.x;
                    (*raster)(x, y).g = color.y;
                    (*raster)(x, y).b = color.z;
                }
            
            return *raster;
        }
};
