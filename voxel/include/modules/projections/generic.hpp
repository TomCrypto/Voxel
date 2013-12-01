#pragma once

/** @file projections/generic.hpp
  *
  * @brief Generic Projections
  *
  * Provides access to the most common projection models.
**/

#include <CL/cl.hpp>
#include <stdexcept>

#include "setup/scheduler.hpp"

/** @namespace projections
  *
  * @brief Namespace for the projections.
  *
  * @see include/modules/projection.cl
**/
namespace projections
{
    inline cl::Program perspective(void)
    {
        return scheduler::acquire("modules/projections/perspective");
    }

    /**************************************************************************/

    /** @enum generic
      *
      * Defines some generic, fully qualified projections.
    **/
    enum generic
    {
        PERSPECTIVE,                             // Perspective projection

        COUNT_
    };

    /** Returns the projection corresponding to a \c generic enum value.
      *
      * @param projection  Enum value.
      *
      * @return The projection program.
    **/
    inline cl::Program get_generic(const generic &projection)
    {
        switch (projection)
        {
               case PERSPECTIVE: return perspective();
            default            : throw std::logic_error("Unknown projection");
        }
    }
};
