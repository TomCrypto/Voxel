#include "renderer.hpp"

#include <cmath>

Renderer::Renderer(size_t width, size_t height/*,
                 const Projection& projection,
                 const Integrator& integrator*/)
/*    : projection(projection),
      integrator(integrator)*/
{
    raster = new Raster(width, height);
}

Renderer::~Renderer()
{
    delete raster;
}

const Raster& Renderer::render() const
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
            
            float dist = 1 - sqrt(px * px + py * py);
            if (dist < 0) dist = 0;
        
            (*raster)(x, y).r = dist * (sin(1 * (px + py)) * 0.5f + 0.5f);
            (*raster)(x, y).g = dist * (sin(2 * (px + py)) * 0.5f + 0.5f);
            (*raster)(x, y).b = dist * (sin(3 * (px + py)) * 0.5f + 0.5f);
        }
    
    return *raster;
}
