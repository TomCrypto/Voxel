#include "setup/scheduler.hpp"
#include "render/engine.hpp"

#include "world/observer.hpp" // to move to world

Engine::Engine(cl::Program subsampler,
               cl::Program projection,
               cl::Program integrator,
               std::size_t width,
               std::size_t height,
               cl::ImageGL &image)
    : image(image), frame(width, height)
{
    core.push_back(scheduler::acquire("core/geometry"));
    core.push_back(scheduler::acquire("core/frame_io"));
    core.push_back(scheduler::acquire("core/math_lib"));
    core.push_back(scheduler::acquire("core/prng_lib"));
    core.push_back(scheduler::acquire("main"));
    modules[Module::SUBSAMPLER] = subsampler;
    modules[Module::PROJECTION] = projection;
    modules[Module::INTEGRATOR] = integrator;

    geometry = scheduler::alloc_buffer(geometry_o.bufSize(),
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 geometry_o.getPtr());

    link();
}

void Engine::set_module(Module type, cl::Program module)
{
    modules[type] = module;
    clear_frame();
    link();
}

void Engine::resize_frame(std::size_t width, std::size_t height,
                          cl::ImageGL &image)
{
    frame = Frame(width, height);
    this->image = image;

    frame.bind_to(this->render_k);
    frame.bind_to(this->buf2tex_k);
    scheduler::set_arg(buf2tex_k, "tex_data", this->image);
}

void Engine::clear_frame(void)
{
    frame.clear();
}

void Engine::sample(void)
{
    frame.next();
    scheduler::run(render_k, cl::NDRange(frame.width() * frame.height()));
}

void Engine::draw(void)
{
    scheduler::run(buf2tex_k, cl::NDRange(frame.width() * frame.height()));
}

void Engine::link(void)
{
    std::vector<cl::Program> programs(this->core);
    for(auto it = modules.begin(); it != modules.end(); ++it)
        programs.push_back(it->second);

    this->program = scheduler::link(programs, "renderer");
    this->render_k = scheduler::get(this->program, "render");
    this->buf2tex_k = scheduler::get(this->program, "buf2tex");

    frame.bind_to(this->render_k);
    frame.bind_to(this->buf2tex_k);
    observer::bind_to(this->render_k);
    scheduler::set_arg(render_k, "geometry", geometry);
    scheduler::set_arg(buf2tex_k, "tex_data", image);
}
