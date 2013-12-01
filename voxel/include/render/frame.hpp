// manages the frame buffer

#pragma once

#include <CL/cl.hpp>

#include "setup/scheduler.hpp"

struct FrameInfo
{
    cl_uint width, height;
    cl_ulong counter;
} __attribute__((packed));

class Frame
{
    public:
        Frame(std::size_t width, std::size_t height);

        void next();

        void bind_to(cl::Kernel &kernel);

        void read(void *ptr);

        void clear(void);

        size_t width();
        size_t height();

    private:
        cl::Buffer frame_buffer;
        cl::Buffer frame_info;
        FrameInfo info;
};
