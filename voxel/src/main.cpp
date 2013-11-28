#include <algorithm>

#include <CL/cl.hpp>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#ifndef CL_VERSION_1_2
    #error "OpenCL 1.2 is required to build this software!"
#endif

/*
#error "TODO (ideas so far, non-exhaustive):"
#error "1. move SFML window code to its own namespace (gui or something)"
#error "2. handle window resize (change hardcoded sizes as needed)"
#error "3. clean up observer and frame"
#error "4. maybe work on the command-line parsing"
#error "5. try and see if integrating ATB is feasible"
#error "6. once platform-specific stuff is working, fix makefile to have a"
#error "   Windows configuration and make everything work on Linux"
*/

#include "scheduler.hpp"
#include "devices.hpp"
#include "interop.hpp"
#include "log.hpp"

#include "frame.hpp"

#include "observer.hpp"

#include "integrators/generic.hpp"
#include "subsamplers/generic.hpp"
#include "projections/generic.hpp"

#include "geometry/voxel_test.hpp"

#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>

using namespace math;

static void do_stuff(sf::Window &window, cl::ImageGL &image);

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
                print_info("Initializing user interface");
                sf::Window window(sf::VideoMode(512, 512), "Voxel");
                window.setFramerateLimit(60);
                window.setPosition(sf::Vector2i(256, 256));
                print_info("User interface ready, starting");

                print_info("Detecting best CL/GL interop environment");
                interop::initialize(device, window.getSystemHandle());
                print_info("Scheduler ready, interop is available");

                try
                {
                    cl::ImageGL image = interop::get_image(512, 512);

                    print_info("Initializing renderer");
                    do_stuff(window, image);
                    print_info("Renderer up and running");
                }
                catch (const cl::Error &e)
                {
                    print_exception("OpenCL runtime error", e);
                    return EXIT_FAILURE; // Perhaps driver bug?
                }
            }
            catch (const cl::Error &e)
            {
                print_exception("OpenCL initialization failure", e);
                return EXIT_FAILURE; // Selected device unsupported?
            }
        }
        catch (const std::exception &e)
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

void do_stuff(sf::Window &window, cl::ImageGL &image)
{
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
    cl::Program subsampler = subsamplers::none();
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
    cl::Kernel buf2tex = scheduler::get(linked, "buf2tex");

    frame.bind_to(kernel);
    observer::bind_to(kernel);

    scheduler::set_arg(kernel, "geometry", geometry_buffer);

    sf::Vector2i deltas;
    sf::Vector2i fixed(window.getSize());
    float x = 0, y = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.key.code == sf::Keyboard::Escape)
                return;
            if (event.key.code == sf::Keyboard::W)
            {
                observer::forward(0.01f);
                frame.clear();
            }
            if (event.key.code == sf::Keyboard::R)
            {
                observer::roll(0.05f);
                frame.clear();
            }
        }

        deltas = fixed-sf::Mouse::getPosition();
        if(deltas != sf::Vector2i(0, 0))
        {
            sf::Mouse::setPosition(fixed);
            frame.clear();
            x = (float)deltas.x / 256;
            y = -(float)deltas.y / 256;
            //observer::move_to(math::float3(x, y, -0.20));
            observer::turn_h(x * 0.1f);
            observer::turn_v(y * 0.1f);
        }

        frame.next();
        scheduler::run(kernel, cl::NDRange(512 * 512));

        // post-process into texture here

        frame.bind_to(buf2tex);
        scheduler::set_arg(buf2tex, "tex_data", image);

        scheduler::acquireGL(image);

        scheduler::run(buf2tex, cl::NDRange(512 * 512));

        scheduler::releaseGL(image);

        interop::draw_image(image);

        window.display();
    }

    delete[] buf;
}
