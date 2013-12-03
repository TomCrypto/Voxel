// manages rendering (NOT FINISHED YET)

#pragma once

#include <functional>
#include <CL/cl.hpp>
#include <cstddef>
#include <vector>
#include <map>

#include "render/frame.hpp"

class Engine
{
    public:
        enum Module
        {
            SUBSAMPLER,
            PROJECTION,
            INTEGRATOR,
        };

        /** Creates a new Engine with an initial set of parameters.
          *
          * @param subsampler  Initial subsampler module.
          * @param projection  Initial projection module.
          * @param integrator  Initial integrator module.
          * @param image       OpenGL/OpenCL image to draw into.
        **/
        Engine(const cl::Program &subsampler,
               const cl::Program &projection,
               const cl::Program &integrator,
               const cl::ImageGL &image);

        /** Resizes the frame to new dimensions.
          *
          * @param image   New image.
        **/
        void resize_frame(const cl::ImageGL &image);

        /** Sets up a new module for rendering, replacing the previous one.
          *
          * @param type    The module type.
          * @param module  The loaded module program.
        **/
        void set_module(Module type, const cl::Program &module);

        /** Clears the frame completely.
        **/
        void clear_frame(void);

        /** Adds another sample to the frame.
        **/
        void sample(void);

        void draw(void); // this is where the post-processing goes

        /** Convenience function which attaches an arbitrary object, as long as
          * it has an accessible \c notify_cb callback member function with the
          * right parameters.
        **/
        template <typename T>
        void attach(T &obj)
        {
            attach(std::bind(&T::notify_cb, &obj, std::placeholders::_1));
        }

    private:
        /** Registers a callback function to be called whenever the kernels are
          * regenerated (e.g. after linking) for the caller to rebind itself.
          *
          * @param callback  Callback function to be called.
          *
          * @remarks The argument to this callback function will be a const map
          *          mapping kernel names to kernel objects - the object should
          *          bind its OpenCL resources to the appropriate kernel.
        **/
        void attach(const std::function
                    <void(std::map<std::string, cl::Kernel>&)>
                    &callback);

        /** Notifies all registered callbacks kernels have been regenerated.
        **/
        void notify(void);

        /** The list of callbacks registered by \c attach().
        **/
        std::vector<std::function
                    <void(std::map<std::string, cl::Kernel>&)>> callbacks;

        /** Links all currently loaded programs and generates kernels.
        **/
        void link(void);

        std::map<std::string, cl::Kernel> kernels;
        std::map<Module, cl::Program> modules;
        std::vector<cl::Program> core;
        cl::Program program;
        Frame frame;
};
