// this will contain the world later on (e.g. the voxel disk/memory streaming
// code, the geometry buffer, the observer, the material database, etc....)

#pragma once

#include <CL/cl.hpp>
#include <string>
#include <map>

#include "geometry/voxel_test.hpp"
#include "world/observer.hpp"

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
        Observer observer;
};
