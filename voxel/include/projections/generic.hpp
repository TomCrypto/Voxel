#pragma once

/** @file projections/generic.hpp
  * 
  * @brief Generic Projections
  *
  * Provides access to the most common projection models. 
**/

#include <cmath>

#include "math/common.hpp"
#include "math/vector3.hpp"
#include "math/matrix3x3.hpp"

/** @namespace projections
  * 
  * @brief Namespace for the projections.
  *
  * A projection is a function which takes normalized screen coordinates within
  * \f$[-1, 1]\f$ as well as the renderer's aspect ratio and also some observer
  * related information (such as the observer's position, or the direction that
  * it is currently looking towards, ...) and produces the corresponding camera
  * ray, as an origin point and a normalized direction vector.
  *
  * This allows for arbitrary projection models, from an orthographic camera to
  * a full-blown perspective camera with depth-of-field effects and lensing, or
  * fisheye/spherical projections.
  *
  * Projection models which do not use the full image surface should return the
  * value \c false to indicate this screen coordinate must be left unprocessed,
  * but most other projection models will just return \c true unconditionally.
  *
  * Note that depending on the subsampler used, the screen coordinates might be
  * outside the indicated range by a small margin - projections must attempt to
  * handle this gracefully by implementing appropriate boundary conditions.
**/
namespace projections
{
    using namespace math;

    /** Implements a 3D perspective camera following the pinhole model, support
      * for custom field of view and aspect ratio is available.
      *
      * @param observer   Observer to project rays from.
      * @param u          Normalized horizontal coordinate.
      * @param v          Normalized vertical coordinate.
      * @param ratio      Aspect ratio (width over height).
      * @param origin     Origin of this camera ray.
      * @param direction  Direction of this camera ray.
      *
      * @return \c true if a camera ray was produced, \c false otherwise.
      * 
      * @remarks The returned direction must be normalized.
    **/
    bool perspective(const Observer &observer,
                     float u, float v, float ratio,
                     math::float3 &origin, math::float3 &direction)
    {
        float3x3 view = basis(observer.direction);

        float3 camera_space(ratio * -u, -v, 1.0f / tan(observer.fov / 2));
        float3 world_space_ray = view * normalize(camera_space);

        origin = observer.position;
        direction = world_space_ray;

        return true;
    }

    /** Implements a fisheye projection model.
      * 
      * @remarks The aspect ratio is ignored, as this projection is meant for a
      *          rectangular image, twice as wide as it is high.
      *
      * @param observer   Observer to project rays from.
      * @param u          Normalized horizontal coordinate.
      * @param v          Normalized vertical coordinate.
      * @param origin     Origin of this camera ray.
      * @param direction  Direction of this camera ray.
      * 
      * @return \c true if a camera ray was produced, \c false otherwise.
      *
      * @remarks The returned direction must be normalized.
    **/
    bool fisheye(const Observer &observer,
                 float u, float v, float /*ratio*/,
                 math::float3 &origin, math::float3 &direction)
    {
        float phi = M_PI * (0.5f - u);
        float theta = M_PI_2 * (1 + v); // voodoo magic
        
        direction = spherical(phi, theta);
        float3x3 view = basis(observer.direction);
        direction  = view * direction;
        origin = observer.position;

        return true;
    }
};
