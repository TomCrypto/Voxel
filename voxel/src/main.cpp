#include <cstdlib>
#include <cstring>
#include <iostream>

#include "renderer.hpp"

#include "display/ppm_file.hpp"

// this will do the argument checking, and create the MVC objects
int main(int argc, char* argv[])
{
    //World world(world params here);
    
    //Renderer renderer(initial renderer parameters, world); // world is read-only
    Renderer renderer(1024, 768);
    
    //DisplayDevice device = DisplayFactory.create(initial device params, world); // world is read-write
    DisplayDevice *device = new PPMDisplay("test.ppm");
    int retval = (device->present(renderer) ? EXIT_SUCCESS : EXIT_FAILURE); // renderer is read-only
    
    delete device;
    return retval;
}
