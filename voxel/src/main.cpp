#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <functional>

#include "contact.hpp" // for build_table()

#include "observer.hpp"

#include "renderer.hpp"
//#include "display.hpp"

#include "geometry/cornell_box.hpp"
#include "geometry/voxel_test.hpp"

#include "integrators/generic.hpp"
#include "projections/generic.hpp"
#include "subsamplers/generic.hpp"

#include "display/x11_display.hpp"

//#include "compile_settings.hpp" // no need for this for now

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

int main(int /*argc*/, char */*argv*/[])
{
	build_table(); // for the 16-bit normals

	Raster raster(512, 512);
	VoxelTest geometry;
	X11Display disp(512, 512, "Voxel Engine");
	
	// configure the observer (will be done elsewhere later)
	
	Observer observer;
	observer.position = math::float3(0.1f, -0.4f, -0.4f);
	observer.direction = math::float3(-0.1f, -0.2f, 1);
	observer.fov = 75 * (M_PI / 180.0f);
	
	#if 0
	auto t1 = Clock::now();
	
    render(std::bind(integrators::direct<decltype(geometry)>, geometry, _1, _2),
           std::bind(projections::perspective, observer, _1, _2, _3, _4, _5),
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
	int frames = 0;
	
	const int MAX_FRAMES = 500;
	
	
	
	do
	{
		auto t1 = Clock::now();

	    observer.direction.x = sin(time * 1.1f) / 3.0f;
	    observer.position.z = cos(time * 1.0f) / 2.0f;
	    
	    light_pos.x = cos(time) * 0.5f; // hack to get some moving light sources
	    light_pos.z = sin(time) * 0.5f;
	    
	    using namespace std::placeholders;
	
	    // note that rendering is entirely read-only -> no synchronization needed
	    render(std::bind(integrators::direct<decltype(geometry)>, geometry, _1, _2),
               std::bind(projections::perspective, observer, _1, _2, _3, _4, _5),
               std::bind(subsamplers::none, _1, _2, _3),
               raster);
        
        auto t2 = Clock::now();
    
        milliseconds ms = std::chrono::duration_cast<milliseconds>(t2 - t1);
        
        frames++;
        time += 0.02f;
        
        std::cout << ms.count() << " ms" << std::endl;
	} while (disp.draw(raster) && (frames < MAX_FRAMES));
	#endif
		
	return EXIT_SUCCESS;
}
