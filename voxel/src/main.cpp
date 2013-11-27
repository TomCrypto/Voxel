#include <algorithm>

#include <CL/cl.hpp>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#ifndef CL_VERSION_1_2
    #error "OpenCL 1.2 is required to build this software!"
#endif

#include "scheduler.hpp"
#include "devices.hpp"
#include "log.hpp"

#include "frame.hpp"

#include "observer.hpp"

#include "integrators/generic.hpp"
#include "subsamplers/generic.hpp"
#include "projections/generic.hpp"

#include "math/vector4.hpp"
#include "geometry/voxel_test.hpp"

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/glx.h>

using namespace math;

void draw(const float4 *data, size_t width, size_t height, const char *path)
{
	FILE *file = fopen(path, "w");

	fprintf(file, "P3\n\n%zu %zu 255\n",
	        width, height);

	for (size_t y = 0; y < height; ++y)
	for (size_t x = 0; x < width; ++x)
	{
	    fprintf(file, "%d %d %d ",
		    (int)(std::min(1.0f, std::max(0.0f, data->x / data->w)) * 255),
		    (int)(std::min(1.0f, std::max(0.0f, data->y / data->w)) * 255),
		    (int)(std::min(1.0f, std::max(0.0f, data->z / data->w)) * 255));
        ++data;
    }

	fclose(file);
}

static void do_stuff(void);

static const char *list_devices_cmd = "--list-devices";
static const char *use_device_cmd   = "--use-device";

static bool has_argument(int argc, char *argv[], const char *arg)
{
    for (int t = 0; t < argc - 1; ++t)
        if (!strcmp(argv[t], arg)) return true;
    
    return false;
}

static char *get_argument(int argc, char *argv[], const char *arg)
{
    for (int t = 0; t < argc - 1; ++t)
        if (!strcmp(argv[t], arg)) return argv[t + 1];
    
    return nullptr; /* Should not happen. */
}

int main(int argc, char *argv[])
{
    if ((argc == 2) && !strcmp(argv[1], list_devices_cmd))
        return print_devices() ? EXIT_SUCCESS : EXIT_FAILURE;

    if (has_argument(argc, argv, use_device_cmd))
    {
        try
        {
            cl::Device device; // This is selected by the user
            auto name = get_argument(argc, argv, use_device_cmd);
            if (!select_device(name, device)) return EXIT_FAILURE;

            try
            {
                print_info("Initializing OpenCL scheduler");
                
                sf::Window window(sf::VideoMode(800, 600), "OpenGL");
                
                printf("Context = %p.\n", glXGetCurrentContext());
                printf("Display = %p.\n", glXGetCurrentDisplay());
                
                cl_context_properties props[] = 
                {
                    CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
                    CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
                    CL_CONTEXT_PLATFORM, (cl_context_properties)scheduler::get_platform(device),
                    0
                };
                
                scheduler::setup(device, props);
                print_info("Scheduler ready");

                try
                {
                    print_info("Initializing renderer");
                    do_stuff();
                    print_info("Renderer up and running");
    
                    print_info("Initializing user interface");
                    
                    print_info("User interface ready, starting");
                    
                    window.display();
                    
                    int x;
                    std::cin >> x;
                }
                catch (cl::Error &e)
                {
                    print_exception("OpenCL runtime error", e);
                    return EXIT_FAILURE; // Perhaps driver bug?
                }
            }
            catch (cl::Error &e)
            {
                print_exception("OpenCL initialization failure", e);
                return EXIT_FAILURE; // Selected device unsupported?
            }
        }
        catch (std::exception &e)
        {
            print_exception("A fatal error occurred", e);
            return EXIT_FAILURE; // Not an OpenCL error..
        }
        
        return EXIT_SUCCESS;
    }
    
    printf("Usage:\n\t%s %s [name]", argv[0], use_device_cmd);
    printf(      "\n\t%s %s\n", argv[0], list_devices_cmd);
    printf("\nThis software requires OpenCL 1.2.\n");
    return EXIT_FAILURE; // Argument parsing error.
}

void do_stuff(void)
{
    // start rendering here
        
    Frame frame(512, 512);
    
    VoxelTest geometry_o;
    
    cl::Buffer geometry_buffer = scheduler::alloc_buffer(geometry_o.bufSize(),
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 geometry_o.getPtr());
   
    observer::setup();

    observer::move_to(math::float3(-0.15, -0.60, -0.20));
    observer::look_at(math::float3(0, -0.5, 1));
    observer::set_fov(90);

    math::float4 *buf = new math::float4[512 * 512];

    cl::Program main_prog = scheduler::acquire("main");

    cl::Program projection = projections::perspective();
    cl::Program subsampler = subsamplers::low_discrepancy<1>();
    cl::Program integrator = integrators::ambient_occlusion();
    
    cl::Program geometry = scheduler::acquire("core/geometry");
    cl::Program frame_io = scheduler::acquire("core/frame_io");
    cl::Program math_lib = scheduler::acquire("core/math_lib");
    cl::Program prng_lib = scheduler::acquire("core/prng_lib");
    
    std::vector<cl::Program> programs;
    programs.push_back(main_prog);
    programs.push_back(projection);
    programs.push_back(subsampler);
    programs.push_back(integrator);
    programs.push_back(math_lib);
    programs.push_back(prng_lib);
    programs.push_back(geometry);
    programs.push_back(frame_io);
    
    cl::Program linked = scheduler::link(programs, "renderer");
    
    cl::Kernel kernel = scheduler::get(linked, "render");

    frame.bind_to(kernel);
    observer::bind_to(kernel);

    scheduler::set_arg(kernel, "geometry", geometry_buffer);
    
    for (size_t t = 0; t < 1; ++t)
    {
        frame.next();

        scheduler::run(kernel, cl::NDRange(512 * 512));
    }
    
    frame.read(buf);
   
    // postprocess

    draw(buf, 512, 512, "out.ppm");
    
    delete[] buf;
}
