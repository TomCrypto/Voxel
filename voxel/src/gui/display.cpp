#include "external/icon.h"
#include "gui/display.hpp"
#include "gui/atb.hpp"

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window.hpp>
#include <cstddef>

#include "setup/scheduler.hpp"
#include "setup/interop.hpp"

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
static const auto default_subsampler = subsamplers::generic::NONE;
static const auto default_projection = projections::generic::PERSPECTIVE;
static const auto default_integrator = integrators::generic::DEPTH;

static void setup_tweak_bar(void)
{
    atb::add_var("subsampler", "Subsampler", atb::subsamplers());
    atb::add_var("projection", "Projection", atb::projections());
    atb::add_var("integrator", "Integrator", atb::integrators());
    atb::set_var("subsampler", default_subsampler);
    atb::set_var("projection", default_projection);
    atb::set_var("integrator", default_integrator);
}

unique_ptr<sf::Window> display::initialize(const std::string &name)
{
    auto mode = sf::VideoMode(initial_w, initial_h); // Window dimensions
    auto style = sf::Style::Resize | sf::Style::Titlebar | sf::Style::Close;
    auto window = unique_ptr<sf::Window>(new sf::Window(mode, name, style));
    window->setIcon(icon::width, icon::height, (unsigned char *)icon::data);
    window->setPosition(sf::Vector2i(initial_x, initial_y));
    atb::initialize("Configuration");
    setup_tweak_bar();
    return window;
}

static void do_stuff(sf::Window &window)
{
    bool relink = false;

    // following code belongs somewhere else (renderer class?)

    atb::window_resize(window.getSize().x, window.getSize().y);

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

    cl::Program projection;
    cl::Program subsampler;
    cl::Program integrator;

    cl::Program geometry = scheduler::acquire("core/geometry");
    cl::Program frame_io = scheduler::acquire("core/frame_io");
    cl::Program math_lib = scheduler::acquire("core/math_lib");
    cl::Program prng_lib = scheduler::acquire("core/prng_lib");

    cl::Program linked;

    cl::Kernel kernel;
    cl::Kernel buf2tex;

    // end

    bool mouse_down = false;
    int last_x = 0, last_y = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (atb::handle_event(event))
            {
                window.setMouseCursorVisible(true);
                mouse_down = false;
                continue;
            }

            if (event.type == sf::Event::Closed)
                return;

            if (event.type == sf::Event::KeyPressed)
            {
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
                    frame.clear();
                }
            }

            if (event.type == sf::Event::Resized)
            {
                atb::window_resize(window.getSize().x, window.getSize().y);
                frame = Frame(event.size.width, event.size.height);
                interop::free_image(image);
                image = interop::get_image(event.size.width, event.size.height);
                frame.clear();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Button::Left)
                {
                    mouse_down = true;
                    window.setMouseCursorVisible(false);
                    last_x = event.mouseButton.x;
                    last_y = event.mouseButton.y;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                window.setMouseCursorVisible(true);
                mouse_down = false;
            }

            if (event.type == sf::Event::MouseLeft)
            {
                window.setMouseCursorVisible(true);
                mouse_down = false;
            }

            if (event.type == sf::Event::MouseMoved)
            {
                if (mouse_down)
                {
                    if ((event.mouseMove.x == last_x) && (event.mouseMove.y == last_y)) continue;

                    frame.clear();
                    float x = -(float)(event.mouseMove.x - last_x) / window.getSize().x * 2;
                    float y = (float)(event.mouseMove.y - last_y) / window.getSize().y * 2;
                    last_x = event.mouseMove.x;
                    last_y = event.mouseMove.y;
                    observer::turn_h(x * 1.5f);
                    observer::turn_v(y * 1.5f);
                }
            }
        }

        if (atb::has_changed("integrator"))
        {
            auto value = atb::get_var<integrators::generic>("integrator");
            integrator = integrators::get_generic(value);
            relink = true;
        }

        if (atb::has_changed("subsampler"))
        {
            auto value = atb::get_var<subsamplers::generic>("subsampler");
            subsampler = subsamplers::get_generic(value);
            relink = true;
        }

        if (atb::has_changed("projection"))
        {
            auto value = atb::get_var<projections::generic>("projection");
            projection = projections::get_generic(value);
            relink = true;
        }

        if (relink)
        {
            std::vector<cl::Program> programs;
            programs.push_back(main_prog);
            programs.push_back(projection);
            programs.push_back(subsampler);
            programs.push_back(integrator);
            programs.push_back(math_lib);
            programs.push_back(prng_lib);
            programs.push_back(geometry);
            programs.push_back(frame_io);

            linked = scheduler::link(programs, "renderer");
            kernel = scheduler::get(linked, "render");
            buf2tex = scheduler::get(linked, "buf2tex");
            frame.clear();
            relink = false;
        }

        frame.next();

        observer::bind_to(kernel);

        scheduler::set_arg(kernel, "geometry", geometry_buffer);

        frame.bind_to(kernel);
        scheduler::run(kernel, cl::NDRange(window.getSize().x * window.getSize().y));

        // post-process into texture here

        frame.bind_to(buf2tex);
        scheduler::set_arg(buf2tex, "tex_data", image);

        scheduler::acquireGL(image);

        scheduler::run(buf2tex, cl::NDRange(window.getSize().x * window.getSize().y));

        scheduler::releaseGL(image);

        interop::draw_image(image);

        atb::draw();

        window.display();
    }
}

void display::run(unique_ptr<sf::Window> &window)
{
    do_stuff(*window);
}

void display::finalize(unique_ptr<sf::Window> &window)
{
    atb::finalize();
    window->close();
}
