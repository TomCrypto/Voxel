/* OpenCL 1.2 --- core/frame_io.cl                                  INTERFACE */

/** @file include/core/frame_io.cl
  *
  * @brief Kernel Frame Buffer I/O Control
  *
  * This unit provides functionality to control the frame buffer passed to each
  * kernel (note this code is generic and can easily be reused). Its goal is to
  * indicate to each work item which frame buffer pixel it should work on.
  *
  * This is the only unit which depends on how the kernel is called on the host
  * side (this is necessary since the kernel needs to know what it is doing).
**/

#pragma once

/** @struct Frm_Info
  *
  * This structure contains details about the frame buffer, its dimensions, and
  * other data relating to the current frame being rendered (it should not need
  * to be parsed by any unit other than this source file).
**/
struct Frm_Info;

/** Checks whether this work item has work to do.
  *
  * @param frm_info  The frame information structure.
  *
  * @return \c false if no work is available for this work item - in which case
  *         it should just return immediately - and \c true otherwise.
**/
bool has_work(constant struct Frm_Info *frm_info);

/** Computes the frame counter, a monotonically increasing integer, incremented
  * each frame (useful for selecting which subsampler sample point to use).
  *
  * @param frm_info  The frame information structure.
  *
  * @return The frame counter for this frame.
  *
  * @remarks The value returned will be the same for all work items.
**/
ulong get_counter(constant struct Frm_Info *frm_info);

/** Resolves the (x, y) integer screen coordinates of a work item.
  *
  * @param frm_info  The frame information structure.
  *
  * @return The work item's coordinates.
**/
float2 resolve(constant struct Frm_Info *frm_info);

/** Converts an (x, y) integer coordinate to an (u, v) normalized coordinate.
  *
  * @param frm_info  The frame information structure.
  * @param p         The integer coordinate.
  *
  * @return The corresponding normalized coordinate.
**/
float2 get_uv(constant struct Frm_Info *frm_info, float2 p);

/** Calculates the aspect ratio of the frame buffer.
  *
  * @param frm_info  The frame information structure.
  *
  * @return The aspect ratio (width over height).
**/
float get_ratio(constant struct Frm_Info *frm_info);

/** Accumulates a color produced by the kernel into the frame buffer.
  *
  * @param frm_info  The frame information structure.
  * @param frm_data  The frame buffer.
  * @param computed  The computed color.
  *
  * @remarks This function invokes undefined behaviour if the work item did not
  *          get a valid screen coordinate via the \c resolve() function.
**/
void accumulate(constant struct Frm_Info *frm_info,
                global            float4 *frm_data,
                                  float3  computed);

/** Gets the color stored in the frame buffer for a given kernel work item.
  *
  * @param frm_info  The frame information structure.
  * @param frm_data  The frame buffer.
  *
  * @return The color in the frame buffer.
**/
float3 get_color(constant struct Frm_Info *frm_info,
                 global            float4 *frm_data);

/** Copies this work item's frame buffer value to the output texture.
  *
  * @param frm_info  The frame information structure.
  * @param frm_data  The frame buffer.
  * @param tex_data  The output texture.
**/
void tex_copy(constant struct Frm_Info *frm_info,
              global            float4 *frm_data,
              write_only     image2d_t  tex_data);

/** This function returns an ID, uniquely identifying the current frame and the
  * current work item such that no two work items from any frame share the same
  * ID.
  *
  * @param frm_info  The frame information structure.
  *
  * @return A unique 256-bit integer, as a \c ulong4 vector.
  *
  * @remarks This is used (among other things) to produce a unique seed for the
  *          PRNG, in order to get different pseudorandom numbers every time.
**/
ulong4 guid(constant struct Frm_Info *frm_info);
