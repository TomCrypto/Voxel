#pragma once

/** @file subsamplers/generic.hpp
  *
  * @brief Generic Subsamplers
  *
  * Provides access to the most common subsampler types.
**/

#include <CL/cl.hpp>
#include <cstddef>
#include <string>

#include "scheduler.hpp"

/** @namespace subsamplers
  *
  * @brief Namespace for the subsamplers.
  *
  * @see include/modules/subsampler.cl
**/
namespace subsamplers
{
    namespace details
    {
        template <size_t base>
        float halton(size_t index)
        {
            float f = 1.0f / base;
            float result = 0;

            while (index > 0)
            {
                result += f * (index % base);
                index /= base;
                f /= base;
            }

            return result;
        }
    };

    /** Generates low-discrepancy sample points, using a quasi-random sequence,
      * such as a Halton sequence, providing good and fast sampling quality and
      * little noise.
      *
      * @remarks Unlike pseudorandom sampling, quasirandom sampling will reduce
      *          noise over \f$n\f$ samples at a rate of \f$O(n)\f$, instead of
      *          \f$O(\sqrt{n})\f$.
    **/
    template <size_t order, size_t bA = 2, size_t bB = 3>
    cl::Program low_discrepancy(void)
    {
        static_assert(order > 0, "Subsamplers cannot have order zero.");
        std::string def = "-D ORDER=" + std::to_string(order);
        std::string prefix = "#define SAMPLES ";

        for (size_t t = 0; t < order; ++t)
        {
            float dx = details::halton<bA>(t) - 0.5f; // x-dimension offset
            float dy = details::halton<bB>(t) - 0.5f; // y-dimension offset
            prefix += "(float2)(" + std::to_string(dx) + ","
                                  + std::to_string(dy) + "),";
        }

        return scheduler::acquire("modules/subsamplers/low_discrepancy",
                                  def, prefix);
    }

    /** Generates only a single sampling point: this corresponds to the trivial
      * subsampler, which is the fastest but generally results in aliasing.
    **/
    cl::Program none(void)
    {
        return scheduler::acquire("modules/subsamplers/none");
    }
};
