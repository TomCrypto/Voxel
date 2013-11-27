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

    template <size_t order, size_t bA = 2, size_t bB = 3>
    cl::Program low_discrepancy(void)
    {
        std::string def = "-DORDER=" + std::to_string(order) + " ";
        
        def += "-DSAMPLES=\"";
        
        for (size_t t = 0; t < order; ++t)
        {
            float dx = details::halton<bA>(t) - 0.5f; // x-dimension offset
            float dy = details::halton<bB>(t) - 0.5f; // y-dimension offset
            def += "(" + std::to_string(dx) + "," + std::to_string(dy) + "),";
        }
        
        def += "\"";

        return scheduler::acquire("modules/subsamplers/low_discrepancy", def);
    }

    #if 0

    /** Generates only a single sampling point: this corresponds to the trivial
      * subsampler, which is the fastest but generally results in aliasing.
      *
      * @param  s   Sample point index.
      * @param  dx  Horizontal offset.
      * @param  dy  Vertical offset.
      * 
      * @return \c true if the sample point exists, \c false otherwise.
    **/
    bool none(size_t s, float &dx, float &dy)
    {
        if (s < 1)
        {
            dx = dy = 0;
            return true;
        }

        return false;
    }

    /** Produces uniformly distributed sample points, using pseudorandom number
      * generators: this is fast but is nondeterministic and may result in some
      * noise with a low number of sample points.
      *
      * @remarks The \c #low_discrepancy subsampler is to be preferred over the
      *          \c #uniform subsampler, as it is more efficient and results in
      *          considerably better image quality.
      * 
      * @tparam order  The number of sample points.
      * @param  s      Sample point index.
      * @param  dx     Horizontal offset.
      * @param  dy     Vertical offset.
      * 
      * @return \c true if the sample point exists, \c false otherwise.
    **/
    template <size_t order>
    bool uniform(size_t s, float &dx, float &dy)
    {
        if (s < order)
        {
            dx = math::uniform() - 0.5f;
            dy = math::uniform() - 0.5f;

            return true;
        }

        return false;
    }


    /** Generates low-discrepancy sample points, using a quasi-random sequence,
      * such as a Halton sequence, providing good and fast sampling quality and
      * little noise.
      *
      * @remarks Unlike pseudorandom sampling, quasirandom sampling will reduce
      *          noise over \f$n\f$ samples at a rate of \f$O(n)\f$, instead of
      *          \f$O(\sqrt{n})\f$.
      *
      * @tparam order  The number of sample points.
      * @param  s      Sample point index.
      * @param  dx     Horizontal offset.
      * @param  dy     Vertical offset.
      *
      * @return \c true if the sample point exists, \c false otherwise.
    **/
    template <size_t order>
    bool low_discrepancy(size_t s, float &dx, float &dy)
    {
        if (s < order)
        {
            dx = math::halton(s, 2) - 0.5f;
            dy = math::halton(s, 3) - 0.5f;
        
            return true;
        }

        return false;
    }

    #endif
};
