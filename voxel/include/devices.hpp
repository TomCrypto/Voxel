/** @file devices.hpp
  *
  * @brief Device Selection Utilities
  *
  * This file contains functions to discover and select various OpenCL devices,
  * for the renderer to initialize, and render frames on.
**/

#pragma once

#include <CL/cl.hpp>
#include <string>

/** Pretty-prints the available devices to standard output.
  *
  * @remarks This is meant for the user's convenience.
**/
bool print_devices(void);

/** Selects a device to use for rendering.
  *
  * @param name    The device name.
  * @param device  The corresponding device.
  *
  * @return \c true if the device exists, \c false otherwise.
**/
bool select_device(std::string name, cl::Device &device);
