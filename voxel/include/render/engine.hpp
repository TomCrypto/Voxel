// manages rendering (NOT FINISHED YET)

#pragma once

#include <CL/cl.hpp>
#include <cstddef>
#include <vector>
#include <map>

#include "geometry/voxel_test.hpp"
#include "render/frame.hpp"

class Engine
{
    public:
        enum Module
        {
            SUBSAMPLER,
            PROJECTION,
            INTEGRATOR,

            COUNT_
        };

        /** Creates a new Engine with an initial set of parameters.
          *
          * @param subsampler  Initial subsampler module.
          * @param projection  Initial projection module.
          * @param integrator  Initial integrator module.
          * @param width       Initial frame width.
          * @param height      Initial frame height.
          * @param image       OpenGL/OpenCL image to draw into.
        **/
        Engine(cl::Program subsampler,
               cl::Program projection,
               cl::Program integrator,
               std::size_t width,
               std::size_t height,
               cl::ImageGL &image);

        /** Resizes the frame to new dimensions.
          *
          * @param width   New frame width.
          * @param height  New frame height.
          * @param image   New image.
        **/
        void resize_frame(std::size_t width, std::size_t height,
                          cl::ImageGL &image);

        /** Sets up a new module for rendering, replacing the previous one.
          *
          * @param type    The module type.
          * @param module  The loaded module program.
        **/
        void set_module(Module type, cl::Program module);

        /** Clears the frame completely.
        **/
        void clear_frame(void);

        /** Adds another sample to the frame.
        **/
        void sample(void);

        void draw(void); // this is where the post-processing goes

    private:
        /** Links all currently loaded programs and generates kernels.
        **/
        void link(void);

        std::map<Module, cl::Program> modules;
        std::vector<cl::Program> core;
        cl::Program program;

        cl::ImageGL image;
        Frame frame;

        cl::Kernel render_k, buf2tex_k;

        //// BELOW SHOULD GO IN "WORLD" LATER ON

        VoxelTest geometry_o;
        cl::Buffer geometry;
};
