#pragma once

#include <cstdio>
#include <string>

#include "display.hpp"
#include "renderer.hpp"

/* Display device which prints the output to a PPM image. */

class PPMDisplay : public DisplayDevice
{
    private:
        FILE *file;

    public:
        PPMDisplay(const std::string &path)
        {
            file = fopen(path.c_str(), "w");
        }

        ~PPMDisplay()
        {
            fclose(file);
        }

        virtual bool present(const Renderer &renderer)
        {
            const Raster &raster = renderer.render();
        
            fprintf(file, "P3\n\n%d %d 255\n",
                    (int)raster.width,
                    (int)raster.height);

            for (size_t y = 0; y < raster.height; ++y)
                for (size_t x = 0; x < raster.width; ++x)
                    fprintf(file, "%d %d %d ",
                            (int)(std::min(raster(x, y).r, 1.0f) * 255.0f),
                            (int)(std::min(raster(x, y).g, 1.0f) * 255.0f),
                            (int)(std::min(raster(x, y).b, 1.0f) * 255.0f));

            return true;
        }
};
