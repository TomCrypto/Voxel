/* OpenCL 1.2 --- modules/projection.cl                             INTERFACE */

/** @file projection.cl
  *
  * @brief Kernel Projection Interface
  *
  * A projection is a function which takes normalized screen coordinates within
  * \f$[0, 1]\f$ as well as the renderer's aspect ratio, and also some observer
  * related information (such as the observer's position, or the direction that
  * it is currently looking towards, ...) and produces the corresponding camera
  * ray, as an origin point and a normalized direction vector.
  *
  * This allows for arbitrary projection models, from an orthographic camera to
  * a full-blown perspective camera with depth-of-field effects and lensing, or
  * fisheye/spherical projections.
  *
  * Note that depending on the subsampler used, the screen coordinates might be
  * outside the indicated range by a small margin - projections must attempt to
  * handle this gracefully by implementing appropriate boundary conditions.
**/

#pragma once

#include <core/observer.cl>
#include <core/math_lib.cl>

/** Projects a camera ray from given (u, v) normalized screen coordinates using
  * the projection model this kernel was linked with.
  *
  * @param observer  Observer to project rays from.
  * @param u         The horizontal coordinate.
  * @param v         The vertical coordinate.
  * @param ratio     Aspect ratio (width over height).
  *
  * @return The corresponding camera ray.
  * 
  * @remarks The returned direction must be normalized.
**/
struct Ray project(constant struct OBSERVER *observer,
                   float u, float v, float ratio);
