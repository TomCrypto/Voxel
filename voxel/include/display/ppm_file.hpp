#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include "renderer.hpp"

/* Display device which prints the output to a PPM image. */

bool run_ppm(const std::vector<std::string> &description,
             const Renderer &renderer)
{
    if (description.size() < 2) return false;

    FILE *file = fopen(description[1].c_str(), "w");

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

    fclose(file);

    return true;
}
