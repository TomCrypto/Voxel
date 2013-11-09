#include <cstdlib>
#include <cstring>
#include <vector>

#include "renderer.hpp"
//#include "display.hpp"

#include "geometry/sphere.hpp"
#include "integrators/flat_integrator.hpp"
#include "compile_settings.hpp"

template <typename Raster>
void draw(const Raster &raster)
{
	FILE *file = fopen("out.ppm", "w");

	fprintf(file, "P3\n\n%d %d 255\n",
	    (int)raster.width(),
	    (int)raster.height());

	for (size_t y = 0; y < raster.height(); ++y)
	for (size_t x = 0; x < raster.width(); ++x)
	    fprintf(file, "%d %d %d ",
		    (int)(std::min(raster[y][x].r, 1.0f) * 255.0f),
		    (int)(std::min(raster[y][x].g, 1.0f) * 255.0f),
		    (int)(std::min(raster[y][x].b, 1.0f) * 255.0f));

	fclose(file);
}

int main(int argc, char *argv[])
{
	Sphere world;
	Renderer<Integrator<Sphere>,Sphere> renderer(world);

	Raster raster(768, 768);
	renderer.render(raster);
	draw(raster);


	return EXIT_SUCCESS;
}
