#include "scheduler.hpp"

#include <iostream>

#include "log.hpp"

static cl::Device device;
static cl::Context context;
static cl::CommandQueue queue;

cl_platform_id scheduler::get_platform(const cl::Device &dev)
{
    return dev.getInfo<CL_DEVICE_PLATFORM>();
}

void scheduler::setup(const cl::Device &dev, cl_context_properties *options)
{
    context = cl::Context(std::vector<cl::Device>(1, dev), options);
    queue   = cl::CommandQueue(context);
    device  = dev;
}

cl::Program scheduler::acquire(const std::string &name,
                               const std::string &args)
{
    cl::Program program;

    try
    {
        std::string src = "#include <../src/" + name + ".cl>";
        program = cl::Program(context, src, false);
        std::string options = args + " -cl-std=CL1.2 "
                            + "-cl-kernel-arg-info "
                            + "-Icl/include/";

        program.compile(options.c_str());
    }
    catch (cl::Error &e)
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

size_t scheduler::get_arg(const cl::Kernel &kernel,
                          const std::string &name)
{
#ifdef NO_ARGUMENT_LOOKUP
    #error Not implemented yet!
#else
    size_t num_args = kernel.getInfo<CL_KERNEL_NUM_ARGS>();

    try
    {
        for (size_t t = 0; t < num_args; ++t)
            if (kernel.getArgInfo<CL_KERNEL_ARG_NAME>(t) == name)
                return t;
    }
    catch (cl::Error &e)
    {
        return (size_t)-1;
    }
#endif

    throw new std::logic_error("No kernel argument by name '" + name + "'");
}

void scheduler::run(const cl::Kernel &kernel,
                    const cl::NDRange &dimensions)
{
    queue.finish();
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, dimensions,
                               cl::NullRange /* Find local size */);
}

void scheduler::flush(void)
{
    queue.finish();
}

cl::Buffer scheduler::alloc_buffer(size_t size, cl_mem_flags flags,
                                   void *ptr)
{
    return cl::Buffer(context, flags, size, ptr);
}

void scheduler::write(const cl::Buffer &buffer, size_t offset, size_t size,
                      const void *ptr, bool blocking)
{
    queue.enqueueWriteBuffer(buffer, blocking, offset, size, ptr);
}

void scheduler::read(const cl::Buffer &buffer, size_t offset, size_t size,
                     void *ptr, bool blocking)
{
    queue.enqueueReadBuffer(buffer, blocking, offset, size, ptr);
}

