#pragma once

/** @file display.hpp
  * @brief Display Device Interface
  *
  * Lets the program use different display devices under a unified interface in
  * order to present the renderer's output to the user in some fashion.
**/

#include "compile_settings.hpp"

#include "renderer.hpp"

#include <vector>
#include <string>

/** Checks whether a given display device exists.
  *
  * @param description The description of the display device. This contains the
  *                    name of the device in the first element, while any other
  *                    command-line arguments for the display device follow.
  *
  * @return \c true if the device exists, \c false otherwise.
**/
bool has_display(const std::vector<std::string> &description);

/** Launches a display device with a given renderer (and world: TODO).
  * 
  * @param description The description of the display device.
  *
  * @param renderer The renderer to present on the device.
  *
  * @return \c false if the device failed, \c true otherwise.
  *
  * @remarks This function fails if \c has_display(description) is \c false.
**/
bool start_display(const std::vector<std::string> &description,
                   const Renderer<Integrator> &renderer);
