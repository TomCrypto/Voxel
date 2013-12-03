// manages the frame buffer

#pragma once

#include <CL/cl.hpp>
#include <string>
#include <map>

#include "setup/scheduler.hpp"

struct FrameInfo
{
    cl_uint width, height;
    cl_ulong counter;
} __attribute__((packed));

class Frame
{
    public:
        Frame(const cl::ImageGL &image);

        void next();

        void notify_cb(std::map<std::string, cl::Kernel> &kernels);

        void resize(const cl::ImageGL &image);

        void clear(void);

        size_t width();
        size_t height();

    private:
        cl::ImageGL image;
        cl::Buffer frame_buffer;
        cl::Buffer frame_info;
        FrameInfo info;
};
