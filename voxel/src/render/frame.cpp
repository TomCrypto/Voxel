#include "render/frame.hpp"

Frame::Frame(const cl::ImageGL &image)
{
    resize(image);
}

void Frame::resize(const cl::ImageGL &image)
{
    this->image = image;
    info.width = width();
    info.height = height();
    info.counter = 0;

    frame_buffer = scheduler::alloc_buffer(width() * height() * 16, CL_MEM_READ_WRITE);
    frame_info = scheduler::alloc_buffer(sizeof(FrameInfo), CL_MEM_READ_ONLY);
    clear();
}

void Frame::next(void)
{
    ++info.counter;

    scheduler::write(frame_info, 0, sizeof(FrameInfo), &info);
}

void Frame::notify_cb(std::map<std::string, cl::Kernel> &kernels)
{
    scheduler::set_arg(kernels["render"], "frm_data", frame_buffer);
    scheduler::set_arg(kernels["render"], "frm_info", frame_info);

    scheduler::set_arg(kernels["buf2tex"], "frm_data", frame_buffer);
    scheduler::set_arg(kernels["buf2tex"], "frm_info", frame_info);
    scheduler::set_arg(kernels["buf2tex"], "tex_data", image);
}

void Frame::clear(void)
{
    scheduler::clear_buffer(frame_buffer, width() * height() * 16);
}

size_t Frame::width()
{
    return image.getImageInfo<CL_IMAGE_WIDTH>();
}

size_t Frame::height()
{
    return image.getImageInfo<CL_IMAGE_HEIGHT>();
}
