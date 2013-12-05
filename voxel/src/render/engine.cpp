#include "setup/scheduler.hpp"
#include "render/engine.hpp"

Engine::Engine(const cl::Program &subsampler,
               const cl::Program &projection,
               const cl::Program &integrator,
               const cl::ImageGL &image)
    : frame(image)
{
    core.push_back(scheduler::acquire("core/observer"));
    core.push_back(scheduler::acquire("core/geometry"));
    core.push_back(scheduler::acquire("core/frame_io"));
    core.push_back(scheduler::acquire("core/math_lib"));
    core.push_back(scheduler::acquire("core/prng_lib"));
    core.push_back(scheduler::acquire("main"));
    modules[Module::SUBSAMPLER] = subsampler;
    modules[Module::PROJECTION] = projection;
    modules[Module::INTEGRATOR] = integrator;

    attach(frame);
    link();
}

void Engine::set_module(Module type, const cl::Program &module)
{
    modules[type] = module;
    link(); // new programs
}

void Engine::resize_frame(const cl::ImageGL &image)
{
    frame.resize(image);
    frame.notify_cb(kernels);
}

void Engine::clear_frame(void)
{
    frame.clear();
}

void Engine::sample(void)
{
    frame.next();
    scheduler::run(kernels["render"], cl::NDRange(frame.width() * frame.height()));
}

void Engine::draw(void)
{
    scheduler::run(kernels["interop_copy"], cl::NDRange(frame.width() * frame.height()));
}

void Engine::attach(const std::function
                    <void(std::map<std::string, cl::Kernel>&)>
                    &callback)
{
    callbacks.push_back(callback);
    if (!kernels.empty()) callback(kernels);
}

void Engine::notify(void)
{
    for (auto &callback : callbacks) callback(kernels);
}

void Engine::link(void)
{
    std::vector<cl::Program> programs(core); // link all programs
    for(auto &it : modules) programs.push_back(it.second);
    program = scheduler::link(programs, "renderer");
    kernels = scheduler::get_all(program);
    clear_frame(); // this is necessary
    notify(); // notify all objects
}
