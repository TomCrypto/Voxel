#pragma once

/** @file integrators/generic.hpp
  *
  * @brief Generic Integrators
  *
  * Provides access to the most common integrators.
**/

#include <CL/cl.hpp>
#include <stdexcept>

#include "setup/scheduler.hpp"

/** @namespace integrators
  *
  * @brief Namespace for the integrators.
  *
  * @see include/modules/integrator.cl
**/
namespace integrators
{
    inline cl::Program depth(void)
    {
        return scheduler::acquire("modules/integrators/depth");
    }

    inline cl::Program ambient_occlusion(void)
    {
        return scheduler::acquire("modules/integrators/ao");
    }

    /**************************************************************************/

    /** @enum generic
      *
      * Defines some generic, fully qualified integrators.
    **/
    enum generic
    {
        DEPTH,
        AO,

        COUNT_
    };

    /** Returns the integrator corresponding to a \c generic enum value.
      *
      * @param integrator  Enum value.
      *
      * @return The integrator program.
    **/
    inline cl::Program get_generic(const generic &integrator)
    {
        switch (integrator)
        {
               case    DEPTH   : return depth();
               case       AO   : return ambient_occlusion();
            default            : throw std::logic_error("Unknown integrator");
        }
    }
};
