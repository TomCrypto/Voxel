#pragma once

#include <CL/cl.hpp>

#include <functional>
#include <map>

#include "geometry/voxel_test.hpp"

class World
{
    public:
        World();

        void notify_cb(std::map<std::string, cl::Kernel> &kernels);

        void turn_h(const float amount);
        void turn_v(const float amount);
        void forward(const float amount);

    private:

        VoxelTest geometry_o;
        cl::Buffer geometry;
};
