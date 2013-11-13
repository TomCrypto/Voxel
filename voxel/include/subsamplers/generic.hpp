#pragma once

/** @file subsamplers/generic.hpp
  * 
  * @brief Generic Subsamplers
  *
  * Provides access to the most common subsampler types. 
**/

#include <cstddef>

#include "math/random.hpp"

/** @namespace subsamplers
  * 
  * @brief Namespace for the subsamplers.
  *
  * A subsampler is a function which, provided some integer between \f$0\f$ and
  * \f$n\f$, returns a two-dimensional offset \f$(\mathrm{d}x, \mathrm{d}y)\f$,
  * called a sample point, which offets the position of a pixel on an image and
  * allows the renderer to perform subpixel sampling, reducing aliasing.
  *
  * The value of \f$n\f$ is denoted the order of the subsampler, and is a rough
  * measure of quality (in that a higher order results in more subpixel samples
  * and hence a better estimate of the true color of a pixel).
**/
namespace subsamplers
{
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
};
