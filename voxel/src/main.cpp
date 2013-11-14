#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <functional>

#include "renderer.hpp"
//#include "display.hpp"

#include "geometry/cornell_box.hpp"
#include "geometry/voxel_test.hpp"

#include "integrators/generic.hpp"
#include "projections/generic.hpp"
#include "subsamplers/generic.hpp"

#include "display/x11_display.hpp"

#include "compile_settings.hpp"

#include <chrono>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

void draw(const Raster &raster, const char *path)
{
	FILE *file = fopen(path, "w");

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
	
	#if 0
	auto t1 = Clock::now();
	
    render(std::bind(integrators::direct<VoxelTest>, geometry, _1, _2),
           std::bind(projections::perspective, _1, _2, _3, _4, _5),
           std::bind(subsamplers::none, _1, _2, _3),
           raster);
	
	auto t2 = Clock::now();
    
    milliseconds ms = std::chrono::duration_cast<milliseconds>(t2 - t1);
    
    std::cout << "Time to render: " << ms.count() << " ms." << std::endl;

	draw(raster, "out.ppm");
	
	while (disp.draw(raster))
		;

    #else

	std::cout << "!!! WARNING !!!" << std::endl;
	std::cout << "The animation will take 100% of your CPU while it is running - "
	             "please ensure your processor has adequate cooling hardware, or "
	             "it may overheat." << std::endl << std::endl;
	std::cout << "Press <ENTER> to begin..." << std::endl;
	
	std::cin.get();
	
	float time = 0;
	
	do
	{
	    camera_dir.x = sin(time * 1.1f) / 3.0f;
	    camera_pos.z = cos(time * 1.0f) / 2.0f;
	
	    render(std::bind(integrators::direct<decltype(geometry)>, geometry, _1, _2),
               std::bind(projections::perspective, _1, _2, _3, _4, _5),
               std::bind(subsamplers::none, _1, _2, _3),
               raster);
               
        time += 0.04f;
	} while (disp.draw(raster));
	#endif
		
	return EXIT_SUCCESS;
}
