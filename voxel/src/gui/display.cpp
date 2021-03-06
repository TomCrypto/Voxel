#include "external/icon.h"
#include "gui/display.hpp"
#include "gui/atb.hpp"
#include "gui/log.hpp"

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window.hpp>
#include <sstream>
#include <iomanip>
#include <cstddef>
#include <cstdint>

#include "modules/integrators.hpp"
#include "modules/subsamplers.hpp"
#include "modules/projections.hpp"

#include "render/engine.hpp"
#include "setup/interop.hpp"

using std::unique_ptr;

static std::string window_title;
static const double SPP_REFRESH_RATE = 1.5;
static const std::size_t initial_w = 800, initial_x = 200;
static const std::size_t initial_h = 600, initial_y = 200;
static const auto default_subsampler = subsamplers::modules::AAx4;
static const auto default_projection = projections::modules::PERSPECTIVE;
static const auto default_integrator = integrators::modules::DEPTH;

static void setup_tweak_bar(void)
{
    atb::add_var("subsampler", "Subsampler", atb::subsamplers(),
                 "group='Modules'");
    atb::add_var("projection", "Projection", atb::projections(),
                 "group='Modules'");
    atb::add_var("integrator", "Integrator", atb::integrators(),
                 "group='Modules'");
    atb::add_var("work_ratio", "Samples/Frame", TW_TYPE_UINT32,
                 "min=1 max=16 group='Miscellaneous'");
    atb::add_var("rot_speed", "Rotation Speed", TW_TYPE_FLOAT,
                 "min=0.5 max=10 step=0.1 group='Miscellaneous'");
    atb::add_var("move_speed", "Movement Speed", TW_TYPE_FLOAT,
                 "min=0.1 max=10 step=0.1 group='Miscellaneous'");

    atb::set_var("subsampler", default_subsampler);
    atb::set_var("projection", default_projection);
    atb::set_var("integrator", default_integrator);
    atb::set_var("work_ratio", 1);
    atb::set_var("rot_speed", 5.0f);
    atb::set_var("move_speed", 3.0f);
}

unique_ptr<sf::Window> display::initialize(const std::string &name)
{
    window_title = name; // Save the window name (for appending later on)
    auto mode = sf::VideoMode(initial_w, initial_h); // Window dimensions
    auto style = sf::Style::Resize | sf::Style::Titlebar | sf::Style::Close;
    auto window = unique_ptr<sf::Window>(new sf::Window(mode, name, style));
    window->setIcon(icon::width, icon::height, (unsigned char *)icon::data);
    window->setPosition(sf::Vector2i(initial_x, initial_y));
    atb::initialize("Configuration Options");
    setup_tweak_bar();
    return window;
}

static void update_title(unique_ptr<sf::Window> &window,
                         const std::string &title,
                         double samples)
{
    std::stringstream fmt(title);
    fmt << " (samples/second: ";
    fmt << std::setprecision(1);
    fmt << std::fixed;
    fmt << samples;
    fmt << ")";

    window->setTitle(fmt.str());
}

static void check_modules(Engine &engine)
{
    if (atb::has_changed("subsampler"))
    {
        auto module_id = atb::get_var<subsamplers::modules>("subsampler");
        const cl::Program &module = subsamplers::get(module_id);
        engine.set_module(Engine::Module::SUBSAMPLER, module);
    }

    if (atb::has_changed("projection"))
    {
        auto module_id = atb::get_var<projections::modules>("projection");
        const cl::Program &module = projections::get(module_id);
        engine.set_module(Engine::Module::PROJECTION, module);
    }

    if (atb::has_changed("integrator"))
    {
        auto module_id = atb::get_var<integrators::modules>("integrator");
        const cl::Program &module = integrators::get(module_id);
        engine.set_module(Engine::Module::INTEGRATOR, module);
    }
}

static void process_input(Engine &engine, World &world)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        world.forward(+atb::get_var<float>("move_speed") * 1e-2f);
        engine.clear_frame();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        world.forward(-atb::get_var<float>("move_speed") * 1e-2f);
        engine.clear_frame();
    }
}

void display::run(unique_ptr<sf::Window> &window, World &world)
{
    print_info("Attempting to create initial interop image");
    atb::window_resize(initial_w, initial_h); // must resize now
    cl::ImageGL image = interop::get_image(initial_w, initial_h);

    print_info("Starting rendering engine and event loop");
    Engine engine(subsamplers::get(default_subsampler),
                  projections::get(default_projection),
                  integrators::get(default_integrator),
                  image);
    engine.attach(world);

    sf::Vector2u cursor_pos;
    bool mouse_down = false;
    size_t sample_count = 0;
    sf::Clock clock;

    while (window->isOpen())
    {
        sf::Event event; // event loop
        while (window->pollEvent(event))
        {
            /* Was this meant for ATB? */
            if (atb::handle_event(event))
                mouse_down = false;
            /* Track mouse movement (to update view direction). */
            else if (event.type == sf::Event::MouseButtonReleased)
                mouse_down = false;
            else if (event.type == sf::Event::MouseLeft)
                mouse_down = false;
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Button::Left)
                {
                    cursor_pos = sf::Vector2u(event.mouseButton.x,
                                              event.mouseButton.y);
                    mouse_down = true;
                }
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                if (mouse_down) /* Is the user dragging the mouse? */
                {
                    sf::Vector2u pos = sf::Vector2u(event.mouseMove.x,
                                                    event.mouseMove.y);

                    if (pos != cursor_pos)
                    {
                        auto delta = sf::Vector2i(pos - cursor_pos);
                        float dx = (float)delta.x / window->getSize().x;
                        float dy = (float)delta.y / window->getSize().y;
                        cursor_pos = pos;

                        float speed = atb::get_var<float>("rot_speed");
                        world.turn_h(-dx * speed);
                        world.turn_v(+dy * speed);
                        engine.clear_frame();
                    }
                }
            }
            /* Has window been resized by the user? */
            else if (event.type == sf::Event::Resized)
            {
                interop::free_image(image);
                atb::window_resize(event.size.width, event.size.height);
                std::size_t w = event.size.width, h = event.size.height;
                engine.resize_frame(image = interop::get_image(w, h));
            }
            /* We handle any exit conditions here. */
            else if (event.type == sf::Event::Closed)
                return;
            else if ((event.type == sf::Event::KeyPressed)
                  && (event.key.code == sf::Keyboard::Escape))
                return;

            /* This is just for better presentation. */
            window->setMouseCursorVisible(!mouse_down);
        }

        process_input(engine, world);
        check_modules(engine);

        if (clock.getElapsedTime().asSeconds() >= SPP_REFRESH_RATE)
        {
            double elapsed = clock.getElapsedTime().asSeconds();
            double samples = (double)sample_count / elapsed;
            update_title(window, window_title, samples);
            sample_count = 0;
            clock.restart();
        }

        interop::synchronize_cl(image); /* NOW RENDERING | OpenCL ----------- */

        size_t samples = atb::get_var<uint32_t>("work_ratio");
        for (size_t t = 0; t < samples; ++t) engine.sample();
        sample_count += samples;
        engine.draw();

        interop::synchronize_gl(image); /* NOW DISPLAYING | OpenGL ---------- */

        interop::draw_image(image);
        atb::draw_tweak_bar();
        window->display();
    }
}

void display::finalize(unique_ptr<sf::Window> &window)
{
    atb::finalize();
    window->close();
}
