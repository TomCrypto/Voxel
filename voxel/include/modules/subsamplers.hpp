#pragma once

/** @file modules/subsamplers.hpp
  *
  * @brief Subsampler Modules
  *
  * Provides access to the subsamplers.
**/

#include <CL/cl.hpp>
#include <stdexcept>
#include <cstddef>
#include <string>

#include "setup/scheduler.hpp"

/** @namespace subsamplers
  *
  * @brief Namespace for the subsamplers
  *
  * @see include/modules/subsampler.cl
**/
namespace subsamplers
{
    namespace details
    {
        template <std::size_t base>
        float halton(std::size_t index)
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

    /** Generates only a single sampling point: this corresponds to the trivial
      * subsampler, which is the fastest but generally results in aliasing.
    **/
    inline cl::Program none(void)
    {
        return scheduler::acquire("modules/subsamplers/none");
    }

    /** Generates low-discrepancy sample points, using a quasi-random sequence,
      * such as a Halton sequence, providing good and fast sampling quality and
      * little noise.
      *
      * @remarks Unlike pseudorandom sampling, quasirandom sampling will reduce
      *          noise over \f$n\f$ samples at a rate of \f$O(n)\f$, instead of
      *          \f$O(\sqrt{n})\f$.
    **/
    template <std::size_t order, std::size_t bA = 2, std::size_t bB = 3>
    inline cl::Program low_discrepancy(void)
    {
        static_assert(order > 0, "Subsamplers cannot have order zero.");
        std::string def = "-D ORDER=" + std::to_string(order);
        std::string prefix = "#define SAMPLES ";

        for (std::size_t t = 0; t < order; ++t)
        {
            float dx = details::halton<bA>(t) - 0.5f; // x-dimension offset
            float dy = details::halton<bB>(t) - 0.5f; // y-dimension offset
            prefix += "(float2)(" + std::to_string(dx) + ","
                                  + std::to_string(dy) + "),";
        }

        return scheduler::acquire("modules/subsamplers/low_discrepancy",
                                  def, prefix);
    }

    /**************************************************************************/

    /** @enum modules
      *
      * Defines some fully qualified subsamplers.
    **/
    enum modules
    {
        NONE,                                    // No subsampler (default)
        AAx2,                                    // 2xAA (low_discrepancy)
        AAx4,                                    // 4xAA (low_discrepancy)
        AAx8,                                    // 8xAA (low_discrepancy)

        COUNT_
    };

    /** Returns the subsampler corresponding to an enum value.
      *
      * @param subsampler  Enum value.
      *
      * @return The subsampler program.
    **/
    inline cl::Program get(const modules &subsampler)
    {
        switch (subsampler)
        {
               case        NONE: return none();
               case        AAx2: return low_discrepancy<2>();
               case        AAx4: return low_discrepancy<4>();
               case        AAx8: return low_discrepancy<8>();
            default            : throw std::logic_error("Unknown subsampler");
        }
    }
};
