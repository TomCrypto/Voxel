// holds OpenCL context/queue and schedules stuff

#pragma once

#include <CL/cl.hpp>
#include <stdexcept>
#include <vector>
#include <string>

namespace scheduler
{
    cl_platform_id get_platform(const cl::Device &dev);

    void setup(const cl::Device &device, cl_context_properties *options = 0);

    cl::Program acquire(const std::string &name,
                        const std::string &args = "",
                        const std::string &prefix = "");

    cl::Program link(const std::vector<cl::Program> &programs,
                     const std::string &name);

    cl::Kernel get(const cl::Program &program,
                   const std::string &name);

    size_t get_arg(const cl::Kernel &kernel,
                   const std::string &name);

    template <typename T>
    void set_arg(cl::Kernel &kernel, const std::string &name,
                 T value)
    {
        size_t index = get_arg(kernel, name);
        if (index == (size_t)-1)
            #ifndef NO_ARGUMENT_LOOKUP
            throw std::logic_error("OpenCL implementation does not appear "
                                   "to support kernel argument query, try "
                                   "compiling with 'NO_ARGUMENT_LOOKUP'");
            #else
            throw std::logic_error("No such argument '" + name + "'");
            #endif

        kernel.setArg(index, value);
    }

    void run(const cl::Kernel &kernel,
             const cl::NDRange &dimensions);

    void flush(void);

    cl::ImageGL alloc_gl_image(cl_mem_flags flags, GLuint texture);

    void clear_gl_image(cl::Image &image, size_t width, size_t height);

    void clear_buffer(cl::Buffer &buffer, size_t size);

    void acquireGL(const cl::Memory &object);
    void releaseGL(const cl::Memory &object);

    cl::Buffer alloc_buffer(size_t size, cl_mem_flags flags, void *ptr
                            = nullptr);

    void write(const cl::Buffer &buffer, size_t offset, size_t size,
               const void *ptr, bool blocking = false);

    void read(const cl::Buffer &buffer, size_t offset, size_t size,
              void *ptr, bool blocking = false);
};
