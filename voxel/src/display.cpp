#include "display.hpp"
#include "misc/icon.h"

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window.hpp>
#include <cstddef>

#include "scheduler.hpp"
#include "interop.hpp"

#include "frame.hpp"

#include "observer.hpp"

#include "integrators/generic.hpp"
#include "subsamplers/generic.hpp"
#include "projections/generic.hpp"

#include "geometry/voxel_test.hpp"

using namespace math;
using std::unique_ptr;

static const std::size_t initial_w = 512, initial_x = 200;
static const std::size_t initial_h = 512, initial_y = 200;

unique_ptr<sf::Window> display::initialize(const std::string &name)
{
    auto mode = sf::VideoMode(initial_w, initial_h); // Window dimensions
    auto style = sf::Style::Resize | sf::Style::Titlebar | sf::Style::Close;
    auto window = unique_ptr<sf::Window>(new sf::Window(mode, name, style));
    window->setIcon(icon::width, icon::height, (unsigned char *)icon::data);
    window->setPosition(sf::Vector2i(initial_x, initial_y));
    return window;
}

static void do_stuff(sf::Window &window)
{
    // following code belongs somewhere else (renderer class?)

    cl::ImageGL image = interop::get_image(window.getSize().x, window.getSize().y);

    Frame frame(window.getSize().x, window.getSize().y);

    VoxelTest geometry_o;

    cl::Buffer geometry_buffer = scheduler::alloc_buffer(geometry_o.bufSize(),
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 geometry_o.getPtr());

    observer::setup();

    observer::move_to(math::float3(-0.15, -0.60, -0.20));
    observer::look_at(math::float3(0, -0.5, 1));
    observer::set_fov(90);

    cl::Program main_prog = scheduler::acquire("main");

    cl::Program projection = projections::perspective();
    cl::Program subsampler = subsamplers::none();
    cl::Program integrator = integrators::depth();

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

    observer::bind_to(kernel);

    scheduler::set_arg(kernel, "geometry", geometry_buffer);

    // end

    sf::Vector2i deltas;
    sf::Vector2i fixed(sf::Vector2i(window.getPosition().x + window.getSize().x / 2, window.getPosition().y + window.getSize().y / 2));
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
            if (event.key.code == sf::Keyboard::S)
            {
                observer::forward(-0.01f);
                frame.clear();
            }
            if (event.key.code == sf::Keyboard::R)
            {
                observer::roll(0.05f);
                frame.clear();
            }
            if (event.key.code == sf::Keyboard::U)
            {
                window.setSize(sf::Vector2u(800, 600));
                observer::roll(0.05f);
                frame.clear();
            }
            if (event.type == sf::Event::Resized)
            {
                frame = Frame(event.size.width, event.size.height);
                interop::free_image(image);
                image = interop::get_image(event.size.width, event.size.height);
                frame.clear();
            }
        }

        deltas = fixed-sf::Mouse::getPosition();
        if(deltas != sf::Vector2i(0, 0))
        {
            sf::Mouse::setPosition(fixed);
            frame.clear();
            x = (float)deltas.x / window.getSize().x * 2;
            y = -(float)deltas.y / window.getSize().y * 2;
            observer::turn_h(x * 0.1f);
            observer::turn_v(y * 0.1f);
        }

        frame.next();

        frame.bind_to(kernel);
        scheduler::run(kernel, cl::NDRange(window.getSize().x * window.getSize().y));

        // post-process into texture here

        frame.bind_to(buf2tex);
        scheduler::set_arg(buf2tex, "tex_data", image);

        scheduler::acquireGL(image);

        scheduler::run(buf2tex, cl::NDRange(window.getSize().x * window.getSize().y));

        scheduler::releaseGL(image);

        interop::draw_image(image);

        window.display();
    }
}

void display::run(unique_ptr<sf::Window> &window)
{
    do_stuff(*window);
}

void display::finalize(unique_ptr<sf::Window> &window)
{
    window->close();
}
