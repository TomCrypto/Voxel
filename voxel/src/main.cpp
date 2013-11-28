#include <CL/cl.hpp>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#ifndef CL_VERSION_1_2
    #error "OpenCL 1.2 is required to build this software!"
#endif

/*
#error "TODO (ideas so far, non-exhaustive):"
#error "1. clean up observer and frame"
#error "2. try and see if integrating ATB is feasible"
#error "3. once platform-specific stuff is working, fix makefile to have a"
#error "   Windows configuration and make everything work on Linux"
*/

#include "devices.hpp"
#include "display.hpp"
#include "interop.hpp"
#include "log.hpp"

int main(int argc, char *argv[])
{
    if ((argc == 2) && !strcmp(argv[1], "--list-devices"))
        return print_devices() ? EXIT_SUCCESS : EXIT_FAILURE;

    if ((argc == 3) && !strcmp(argv[1], "--use-device"))
    {
        try
        {
            cl::Device device; // The device is selected by the user
            if (!select_device(argv[2], device)) return EXIT_FAILURE;

            try
            {
                print_info("Initializing graphical user interface");
                auto window = display::initialize("Voxel Renderer");

                print_info("Loading preferred CL/GL interop environment");
                interop::initialize(device, window->getSystemHandle());
                print_info("Scheduler ready, interop is available");

                try
                {
                    print_info("Handing over control to display");
                    display::run(window); // May throw cl::Error
                    display::finalize(window); // All done, exit
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

        print_info("Exiting");
        return EXIT_SUCCESS;
    }

    printf("Usage:\n\t%s %s [name]", argv[0], "--use-device");
    printf(      "\n\t%s %s\n", argv[0], "--list-devices");
    printf("\nThis software requires OpenCL 1.2.\n");
    return EXIT_FAILURE; // Argument parsing error.
}
