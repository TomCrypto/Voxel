#include "observer.hpp"

#include <CL/cl_platform.h>
#include <cmath>

using namespace math;

static cl::Buffer mem;

static struct
{
    cl_float3 pos, dir;
    cl_float3 plane[4];
} __attribute__((packed)) buffer;

static struct
{
    float3 pos, dir;
    float fov;
} data;

static cl_float3 cl_vec(float x, float y, float z)
{
    cl_float3 val;
    val.s[0] = x;
    val.s[1] = y;
    val.s[2] = z;
    return val;
}

static void update(void)
{
    float3x3 view = basis(data.dir);
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

void observer::setup(void)
{
    mem = scheduler::alloc_buffer(sizeof(buffer), CL_MEM_READ_ONLY, 0);
}

void observer::move_to(const float3 &pos)
{
    data.pos = pos;
    update();
}


void observer::look_at(const float3 &dir)
{
    data.dir = dir;
    update();
}

void observer::set_fov(float fov)
{
    data.fov = fov * M_PI / 180;
    update();    
}

void observer::bind_to(cl::Kernel &kernel)
{
    scheduler::set_arg(kernel, "observer", mem);
}