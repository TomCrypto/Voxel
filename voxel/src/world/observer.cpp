#include "world/observer.hpp"

#include "setup/scheduler.hpp"

#include <cmath>

// sigh

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 (3.14159265358979323846 * 0.5)
#endif

using namespace math;

static cl_float3 cl_vec(float x, float y, float z)
{
    cl_float3 val;
    val.s[0] = x;
    val.s[1] = y;
    val.s[2] = z;
    return val;
}

void Observer::update(void)
{
    float3x3 view = basis2(data.dir, float3(-sin(data.roll), cos(data.roll), 0));
    float3 plane[4];

    float z = 1.0f / tan(data.fov * 0.5f);
    plane[0] = view * normalize(float3(-1.0f, +1.0f, z));
    plane[1] = view * normalize(float3(+1.0f, +1.0f, z));
    plane[2] = view * normalize(float3(+1.0f, -1.0f, z));
    plane[3] = view * normalize(float3(-1.0f, -1.0f, z));

    buffer.pos = cl_vec(data.pos.x, data.pos.y, data.pos.z);
    buffer.dir = cl_vec(data.dir.x, data.dir.y, data.dir.z);

    for (size_t t = 0; t < 4; ++t)
        buffer.plane[t] = cl_vec(plane[t].x, plane[t].y, plane[t].z);

    scheduler::write(mem, 0, sizeof(buffer), &buffer);
}

Observer::Observer()
{
    mem = scheduler::alloc_buffer(sizeof(buffer), CL_MEM_READ_ONLY);
}

void Observer::move_to(const float3 &pos)
{
    data.pos = pos;
    update();
}


void Observer::look_at(const float3 &dir)
{
    data.dir = dir;
    data.yaw = atan2(dir.z, dir.x);
    data.pitch = acos(dir.y);
    data.roll = 0;
    update();
}

void Observer::forward(const float depth)
{
    data.pos += data.dir * depth;
    update();
}

void Observer::turn_h(const float amount)
{
    data.yaw += amount;
    data.dir = spherical(data.yaw, data.pitch);
    update();
}

void Observer::turn_v(const float amount)
{
    data.pitch += amount;
    if (data.pitch < 0.01f) data.pitch = 0.01f;
    if (data.pitch > M_PI - 0.01f) data.pitch = M_PI - 0.01f;
    data.dir = spherical(data.yaw, data.pitch);
    update();
}

void Observer::roll(float amount)
{
    data.roll += amount;
    update();
}

void Observer::set_fov(float fov)
{
    data.fov = fov * M_PI / 180;
    update();
}

void Observer::notify_cb(std::map<std::string, cl::Kernel> &kernels)
{
    scheduler::set_arg(kernels["render"], "observer", mem);
}
