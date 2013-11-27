// manages the frame buffer

#pragma once

#include <CL/cl.hpp>

#include "scheduler.hpp"

struct FrameInfo
{
    cl_uint width, height;
    cl_ulong counter;
} __attribute__((packed));

class Frame
{
    public:
        Frame(size_t width, size_t height);

        void next();

        void bind_to(cl::Kernel &kernel);

        void read(void *ptr);

        void clear(void);

    private:
        cl::Buffer frame_buffer;
        cl::Buffer frame_info;
        FrameInfo info;
};
