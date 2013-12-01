#include "setup/scheduler.hpp"
#include "setup/interop.hpp"

#include <SFML/OpenGL.hpp>
#include <CL/cl.hpp>

#if defined _WIN32
    #include <Wingdi.h>
#elif defined __linux__
    #include <GL/glx.h>
#endif

static GLuint texture;

void interop::initialize(const cl::Device &device, sf::WindowHandle /*handle*/)
{
#if defined _WIN32
    cl_context_properties props[] =
    {
        CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
        CL_WGL_HDC_KHR,    (cl_context_properties)wglGetCurrentDC(),
        0
    };

    scheduler::setup(device, props);
#elif defined __linux__
    cl_context_properties props[] =
    {
       CL_GL_CONTEXT_KHR,(cl_context_properties)glXGetCurrentContext(),
       CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
       0
    };

    scheduler::setup(device, props);
#else
    #error "Interop not supported for your platform!"
#endif
}

cl::ImageGL interop::get_image(std::size_t width, std::size_t height)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glViewport(0, 0, (GLint)width, (GLint)height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
                                0, GL_RGBA,  GL_UNSIGNED_BYTE, 0);
    return scheduler::alloc_gl_image(CL_MEM_WRITE_ONLY, texture);
}

void interop::free_image(const cl::ImageGL &/*image*/)
{
    glDeleteTextures(1, &texture);
}

void interop::draw_image(const cl::ImageGL &/*image*/)
{
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glTexCoord2d(0, 0); glVertex2d(-1, -1);
    glTexCoord2d(1, 0); glVertex2d(+1, -1);
    glTexCoord2d(1, 1); glVertex2d(+1, +1);
    glTexCoord2d(0, 1); glVertex2d(-1, +1);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void interop::synchronize_cl(const cl::ImageGL &image)
{
    std::vector<cl::Memory> img(1, image);
    auto v_queue = scheduler::get_queue();
    v_queue.enqueueAcquireGLObjects(&img);
}

void interop::synchronize_gl(const cl::ImageGL &image)
{
    std::vector<cl::Memory> img(1, image);
    auto v_queue = scheduler::get_queue();
    v_queue.enqueueReleaseGLObjects(&img);
}
