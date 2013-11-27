#include "frame.hpp"

Frame::Frame(size_t width, size_t height)
{
    info.width = width;
    info.height = height;
    info.counter = 0;

    frame_buffer = scheduler::alloc_buffer(width * height * 16, CL_MEM_READ_WRITE);
    frame_info = scheduler::alloc_buffer(sizeof(FrameInfo), CL_MEM_READ_ONLY |
                                     CL_MEM_COPY_HOST_PTR,
                                     &info);
}

void Frame::next(void)
{
    ++info.counter;

    scheduler::write(frame_info, 0, sizeof(FrameInfo), &info, false);
}

void Frame::bind_to(cl::Kernel &kernel)
{
    scheduler::set_arg(kernel, "frm_data", frame_buffer);
    scheduler::set_arg(kernel, "frm_info", frame_info);
}

void Frame::read(void *ptr)
{
    scheduler::read(frame_buffer, 0, info.width * info.height * 16, ptr, true);
}

void Frame::clear(void)
{
    scheduler::clear_buffer(frame_buffer);
}
