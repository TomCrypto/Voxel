/* OpenCL 1.2 --- core/observer.cl                                  INTERFACE */

/** @file include/core/observer.cl
  *
  * @brief Kernel Observer Interface
  *
  * This file provides access to the observer, and contains functions to obtain
  * information about the observer's state (such as its position, etc...)
**/

#pragma once

/** @struct Observer
  *
  * Represents the observer.
**/
struct Observer;

/** Retrieves the absolute position of the observer.
  *
  * @param observer  The observer.
  *
  * @return Its position, in world space.
**/
float3 get_position(constant struct Observer *observer);

/** Retrieves the direction the observer is looking towards.
  *
  * @param observer  The observer.
  *
  * @return The view direction, normalized.
**/
float3 get_direction(constant struct Observer *observer);

float get_fov(constant struct Observer *observer);
float get_yaw(constant struct Observer *observer);
float get_pitch(constant struct Observer *observer);

/** Retrieves a point on the observer's focal plane.
  *
  * @param observer  The observer.
  * @param u         The u-coordinate.
  * @param v         The v-coordinate.
  *
  * @return The point on the focal plane, in world space.
**/
float3 get_focal_plane(constant struct Observer *observer, float u, float v);
