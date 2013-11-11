#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <functional>

#include "renderer.hpp"
//#include "display.hpp"

#include "geometry/cornell_box.hpp"
#include "geometry/voxel_test.hpp"

#include "integrators/flat_integrator.hpp"
#include "integrators/direct_integrator.hpp"
#include "integrators/depth_integrator.hpp"

#include "projections/test_perspective.hpp"
#include "projections/fisheye.hpp"

#include "subsampler/sample_naive.hpp"
#include "subsampler/sample_aa.hpp"

#include "compile_settings.hpp"

void draw(const Raster &raster)
{
	FILE *file = fopen("out.ppm", "w");

	fprintf(file, "P3\n\n%d %d 255\n",
	    (int)raster.width(),
	    (int)raster.height());

	for (size_t y = 0; y < raster.height(); ++y)
	for (size_t x = 0; x < raster.width(); ++x)
	    fprintf(file, "%d %d %d ",
		    (int)(std::min(raster[y][x].x, 1.0f) * 255.0f),
		    (int)(std::min(raster[y][x].y, 1.0f) * 255.0f),
		    (int)(std::min(raster[y][x].z, 1.0f) * 255.0f));

	fclose(file);
}

#include <chrono>
#include <iostream>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

int main(int argc, char *argv[])
{
	using namespace std::placeholders;

	Raster raster(768, 768);
	VoxelTest geometry;

    auto t1 = Clock::now();

    size_t trials = 10;
    
    for (size_t t = 0; t < trials; ++t)
	render(std::bind(integrate_depth<VoxelTest>,
                     geometry, _1, _2),
           std::bind(project_perspective,
                     _1, _2, _3, _4, _5),
           std::bind(aa_offset,
                     _1, _2, _3),
           raster);
           
    auto t2 = Clock::now();
    
    milliseconds ms = std::chrono::duration_cast<milliseconds>(t2 - t1);
    
    std::cout << "Time to render: " << ms.count() / trials << " ms." << std::endl;

	draw(raster);

	return EXIT_SUCCESS;
}
