#include "setup/scheduler.hpp"
#include "gui/log.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

static cl::Device device;
static cl::Context context;
static cl::CommandQueue queue;

cl_platform_id scheduler::get_platform(const cl::Device &dev)
{
    return dev.getInfo<CL_DEVICE_PLATFORM>();
}

cl::CommandQueue scheduler::get_queue(void)
{
    return queue;
}

void scheduler::setup(const cl::Device &dev, cl_context_properties *options)
{
    context = cl::Context(std::vector<cl::Device>(1, dev), options);
    queue   = cl::CommandQueue(context, dev);
    device  = dev;
}

static std::string load(const std::string &path)
{
    std::ifstream file(path);
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

cl::Program scheduler::acquire(const std::string &name,
                               const std::string &args,
                               const std::string &prefix)
{
    cl::Program program;
    std::string source;

    try
    {
        if (prefix.empty()) source = "#include <../src/" + name + ".cl>";
        else source = prefix + "\n\n" + load("cl/src/" + name + ".cl");
        program = cl::Program(context, source, false);
        std::string options = args + " -cl-std=CL1.2 "
                            + "-cl-kernel-arg-info "
                            + "-Icl/include/";

        program.compile(options.c_str());
    }
    catch (const cl::Error &e)
    {
        print_error("Failed to load or compile the program '" + name + "'");
        std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        if (log.empty()) print_error("No compilation log available");
        else print_error("CLC compilation log follows");
        std::cout << log;
        throw;
    }

    print_info("Successfully acquired '" + name + "'");
    return program;
}

cl::Program scheduler::link(const std::vector<cl::Program> &programs,
                            const std::string &name)
{
    print_info("Attempting to link '" + name + "'");
    cl::Program program = cl::linkProgram(programs);
    print_info("Successfully linked");
    return program;
}

cl::Kernel scheduler::get(const cl::Program &program,
                          const std::string &name)
{
    return cl::Kernel(program, name.c_str());
}

std::size_t scheduler::get_arg(const cl::Kernel &kernel,
                               const std::string &name)
{
#ifdef NO_ARGUMENT_LOOKUP
    std::string kernel_name = kernel.getInfo<CL_KERNEL_FUNCTION_NAME>();

    if (kernel_name == "render")
    {
        if (name == "frm_data") return 0;
        if (name == "frm_info") return 1;
        if (name == "geometry") return 2;
        if (name == "observer") return 3;
    }
    else if (kernel_name == "buf2tex")
    {
        if (name == "frm_data") return 0;
        if (name == "frm_info") return 1;
        if (name == "tex_data") return 2;
    }
#else
    std::size_t num_args = kernel.getInfo<CL_KERNEL_NUM_ARGS>();

    try
    {
        for (std::size_t t = 0; t < num_args; ++t)
            if (kernel.getArgInfo<CL_KERNEL_ARG_NAME>(t) == name)
                return t;
    }
    catch (const cl::Error &e)
    {
        return (std::size_t)-1;
    }
#endif

    throw new std::logic_error("No kernel argument by name '" + name + "'");
}

void scheduler::run(const cl::Kernel &kernel,
                    const cl::NDRange &dimensions)
{
    size_t local = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[0];

    // round up to nearest local size
    cl::NDRange global = dimensions[0] - dimensions[0] % local + local;

    queue.finish();
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, global,
                               cl::NullRange /* Find local size */);
}

void scheduler::flush(void)
{
    queue.finish();
}

cl::ImageGL scheduler::alloc_gl_image(cl_mem_flags flags, GLuint texture)
{
    return cl::ImageGL(context, flags, GL_TEXTURE_2D, 0, texture);
}

void scheduler::clear_gl_image(cl::Image &image, std::size_t width, std::size_t height)
{
    cl_float4 u;
    u.s[0] = 0;
    u.s[1] = 0;
    u.s[2] = 0;
    u.s[3] = 0;

    cl::size_t<3> origin;
    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;

    cl::size_t<3> region;
    region[0] = width;
    region[1] = height;
    region[2] = 1;

    queue.enqueueFillImage(image, u, origin, region);
}

void scheduler::clear_buffer(cl::Buffer &buffer, std::size_t size)
{
    cl_float4 u;
    u.s[0] = 0;
    u.s[1] = 0;
    u.s[2] = 0;
    u.s[3] = 0;

    queue.enqueueFillBuffer(buffer, u, 0, size);
}

cl::Buffer scheduler::alloc_buffer(std::size_t size, cl_mem_flags flags,
                                   void *ptr)
{
    return cl::Buffer(context, flags, size, ptr);
}

void scheduler::write(const cl::Buffer &buffer, std::size_t offset, std::size_t size,
                      const void *ptr, bool blocking)
{
    queue.enqueueWriteBuffer(buffer, blocking, offset, size, ptr);
}

void scheduler::read(const cl::Buffer &buffer, std::size_t offset, std::size_t size,
                     void *ptr, bool blocking)
{
    queue.enqueueReadBuffer(buffer, blocking, offset, size, ptr);
}
