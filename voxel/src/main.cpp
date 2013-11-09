#include <cstdlib>
#include <cstring>
#include <vector>

#include "renderer.hpp"
#include "display.hpp"

#include "compile_settings.hpp"

static bool is_command(const std::string &str)
{
    return (str.compare(0, 2, "--") == 0);
}

static std::string get_command(const std::string &str)
{
    return str.substr(2, str.length() - 2);
}

static std::vector<std::string> parse_range(int argc, char *argv[],
                                            const std::string &cmd)
{
    std::vector<std::string> args(argv, argv + argc);
    std::vector<std::string> range;
    bool in_range = false;
    
    for (auto iter = args.begin(); iter != args.end(); ++iter)
    {
        if (is_command(*iter)) in_range = get_command(*iter) == cmd;
        else if (in_range) range.push_back(*iter); /* Range copy. */
    }
    
    return range;
}

static std::vector<std::string> parse_display(int argc, char *argv[])
{
    return parse_range(argc, argv, "display");
}

// this will do the argument checking, and create the MVC objects
int main(int argc, char *argv[])
{
    /*const std::vector<std::string> device = parse_display(argc, argv);
    if (device.empty() || (!has_display(device))) return EXIT_FAILURE;*/

    // TEMPORARY (for convenience until we have better display)
    std::vector<std::string> device;
    device.push_back("ppm");
    device.push_back("test.ppm");
    
    //World world(world params here);

    //Renderer renderer(initial renderer parameters, world); // world is read-only
    Renderer<Integrator> renderer(1024, 768); // TEMPORARY
    
    bool retval = start_display(device, renderer);
    return (retval ? EXIT_SUCCESS : EXIT_FAILURE);
}
