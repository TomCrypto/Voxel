#pragma once

#include <CL/cl.hpp>

#include "scheduler.hpp"

#include "math/vector4.hpp"
#include "math/vector3.hpp"
#include "math/matrix3x3.hpp"
#include "math/common.hpp"

namespace observer
{
    using namespace math;

    void setup(void);

    void move_to(const float3 &pos);
    void look_at(const float3 &dir);

    void set_fov(float fov);

    void bind_to(cl::Kernel &kernel);
};
