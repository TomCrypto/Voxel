#pragma once

/** @file modules/integrators.hpp
  *
  * @brief Integrator Modules
  *
  * Provides access to the integrators.
**/

#include <CL/cl.hpp>
#include <stdexcept>

#include "setup/scheduler.hpp"

/** @namespace integrators
  *
  * @brief Namespace for the integrators
  *
  * @see include/modules/integrator.cl
**/
namespace integrators
{
    inline cl::Program depth(void)
    {
        return scheduler::acquire("modules/integrators/depth");
    }

    inline cl::Program normal(void)
    {
        return scheduler::acquire("modules/integrators/normal");
    }

    inline cl::Program ambient_occlusion(void)
    {
        return scheduler::acquire("modules/integrators/ao");
    }

    /**************************************************************************/

    /** @enum modules
      *
      * Defines some fully qualified integrators.
    **/
    enum modules
    {
        DEPTH,
        NORMAL,
        AO,

        COUNT_
    };

    /** Returns the integrator corresponding to an enum value.
      *
      * @param integrator  Enum value.
      *
      * @return The integrator program.
    **/
    inline cl::Program get(const modules &integrator)
    {
        switch (integrator)
        {
               case       DEPTH: return depth();
               case      NORMAL: return normal();
               case          AO: return ambient_occlusion();
            default            : throw std::logic_error("Unknown integrator");
        }
    }
};
