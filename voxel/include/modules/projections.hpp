#pragma once

/** @file modules/projections.hpp
  *
  * @brief Projection Modules
  *
  * Provides access to the projections.
**/

#include <CL/cl.hpp>
#include <stdexcept>

#include "setup/scheduler.hpp"

/** @namespace projections
  *
  * @brief Namespace for the projections
  *
  * @see include/modules/projection.cl
**/
namespace projections
{
    inline cl::Program perspective(void)
    {
        return scheduler::acquire("modules/projections/perspective");
    }

    inline cl::Program fisheye(void)
    {
        return scheduler::acquire("modules/projections/fisheye");
    }

    /**************************************************************************/

    /** @enum modules
      *
      * Defines some fully qualified projections.
    **/
    enum modules
    {
        PERSPECTIVE,                             // Perspective projection
        FISHEYE,                                 // Fisheye projection

        COUNT_
    };

    /** Returns the projection corresponding to an enum value.
      *
      * @param projection  Enum value.
      *
      * @return The projection program.
    **/
    inline cl::Program get(const modules &projection)
    {
        switch (projection)
        {
               case PERSPECTIVE: return perspective();
               case     FISHEYE: return fisheye();
            default            : throw std::logic_error("Unknown projection");
        }
    }
};
