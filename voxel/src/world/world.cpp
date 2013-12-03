#include "world/world.hpp"

#include "world/observer.hpp"
#include "setup/scheduler.hpp"

World::World()
{
    observer::setup();

    observer::move_to(math::float3(-0.15, -0.60, -0.20));
    observer::look_at(math::float3(0, -0.5, 1));
    observer::set_fov(90);

    geometry = scheduler::alloc_buffer(geometry_o.bufSize(),
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 geometry_o.getPtr());
}

void World::notify_cb(std::map<std::string, cl::Kernel> &kernels)
{
    observer::bind_to(kernels["render"]);
    scheduler::set_arg(kernels["render"], "geometry", geometry);
}

void World::turn_h(const float amount)
{
    observer::turn_h(amount);
}

void World::turn_v(const float amount)
{
    observer::turn_v(amount);
}

void World::forward(const float amount)
{
    observer::forward(amount);
}
