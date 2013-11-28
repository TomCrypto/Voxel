/** @file interop.hpp
  *
  * @brief CL/GL Interop Services
  *
  * This unit assists in portable OpenCL/OpenGL interopability which is used to
  * efficiently draw the render to the screen in realtime without going through
  * system memory (i.e. no GPU-CPU-GPU copy is made, improving performance).
**/

#pragma once

#include <SFML/Window.hpp>
#include <CL/cl.hpp>
#include <cstddef>

/** @namespace interop
  *
  * @brief Namespace for CL/GL interop
  *
  * This namespace contains functions relating to OpenCL/OpenGL interop.
**/
namespace interop
{
    /** Initializes the OpenCL environment with OpenCL/OpenGL interop.
      *
      * @param device  The device to use OpenCL with.
      * @param handle  A platform-dependent window handle.
    **/
    void initialize(const cl::Device &device, sf::WindowHandle handle);

    /** Creates an OpenGL texture mapped to an OpenCL image.
      *
      * @param width  The width of the texture, in pixels.
      * @param handle  The height of the texture, in pixels.
      *
      * @remarks Note the returned image is write-only to the CL context, so it
      *          must only be used for outputting the final render.
    **/
    cl::ImageGL get_image(size_t width, size_t height);

    /** Frees the OpenCL image and the underlying OpenGL texture (this does not
      * release the OpenCL environment, and is meant for clean shutdown only).
      *
      * @param image  The OpenCL/OpenGL image.
    **/
    void free_image(const cl::ImageGL &image);

    /** Draws the OpenGL texture onto the window handle.
      *
      * @param image  The OpenCL/OpenGL image.
      *
      * @remarks The texture should have been previously written to through the
      *          OpenCL image, and must have been released by the CL context.
    **/
    void draw_image(const cl::ImageGL &image);
};
