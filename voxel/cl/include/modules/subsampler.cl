/* OpenCL 1.2 --- modules/subsampler.cl                             INTERFACE */

/** @file subsampler.cl
  *
  * @brief Kernel Subsampler Interface
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

#pragma once

/** Gets the number of sample points for this subsampler.
  *
  * @return The number of available sample points.
  *
  * @remarks This function returns nonzero.
**/
size_t sample_count(void);

/** Gets a sample point from this subsampler.
  *
  * @param index  The sample point index.
  *
  * @return The sample point, as an integer coordinate offset.
**/
float2 sample(size_t index);
