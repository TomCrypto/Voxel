#pragma once

#include <CL/cl.hpp>
#include <string>
#include <map>

#include "math/vector4.hpp"
#include "math/vector3.hpp"
#include "math/matrix3x3.hpp"
#include "math/common.hpp"

class Observer
{
    public:
        Observer();

        void move_to(const math::float3 &pos);
        void look_at(const math::float3 &dir);
        void forward(const float depth);

        void turn_h(const float amount);
        void turn_v(const float amount);
        void roll(float amount);

        void set_fov(float fov);

        void notify_cb(std::map<std::string, cl::Kernel> &kernels);

    private:
        struct Buffer
        {
            cl_float3 pos, dir;
            cl_float3 plane[4];
        } __attribute__((packed));

        struct Data
        {
            math::float3 pos, dir;
            float yaw, pitch, roll;
            float fov;
        };

        Buffer buffer;
        Data data;

        cl::Buffer mem;

        void update(void);
};
