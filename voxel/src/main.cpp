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

#include "display/x11_display.hpp"

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
		    raster[y][x].r,
		    raster[y][x].g,
		    raster[y][x].b);

	fclose(file);
}

int main(int argc, char *argv[])
{
	using namespace std::placeholders;

	Raster raster(512, 512);
	VoxelTest geometry;
	X11Display disp(512, 512, "Voxel Engine");

	render(std::bind(integrate_depth<VoxelTest>, geometry, _1, _2),
           std::bind(project_perspective, _1, _2, _3, _4, _5),
           std::bind(aa_offset, _1, _2, _3),
	       raster);

	draw(raster);

	while (disp.draw(raster))
		;

	return EXIT_SUCCESS;
}
