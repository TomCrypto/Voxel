/* OpenCL 1.2 --- main.cl                                      IMPLEMENTATION */

/** @file main.cl
  *
  * @brief Kernel Pipeline
  *
  * Implements the pipeline for the renderer, and contains some setup code such
  * that modules are properly recognized and integrated into the pipeline.
**/

/** Files in the `core` folder are always included and are used by various bits
  * of the code - they are required for the renderer to work, and do not depend
  * on any other file (besides other core files).
**/

#include <core/math_lib.cl>
#include <core/prng_lib.cl>
#include <core/frame_io.cl>
#include <core/observer.cl>
#include <core/geometry.cl>

/** Files in the `modules` folder are interfaces to the various types of module
  * such as projection models, integrators, and so on, which directly integrate
  * with the rendering pipeline (this excludes post-processing kernels).
  *
  * @remarks Modules are made available to the renderer at link time.
**/

#include <modules/subsampler.cl>
#include <modules/projection.cl>
#include <modules/integrator.cl>

/** This is the main kernel for the renderer but it only includes the rendering
  * part (it does not perform tone-mapping or any other form of post-processing
  * which is normally done by other kernels afterwards).
  *
  * @remarks This implements a programmable ray tracing pipeline using a sparse
  *          voxel octree and explicit BRDF's, via a templated module selection
  *          system, implemented in OpenCL 1.2 via link-time module resolution.
  *
  * @param frm_data  The renderer's frame buffer encoded in RGBn format.
  * @param frm_info  The frame information structure, see `frame_io.cl`.
  * @param geometry  The sparse voxel octree (in compact tree encoding).
  * @param observer  The observer, which contains view-dependent params.
  * @param material  The material tables (as a 2D isotropic BRDF array).
  * @param sampling  The material data as inverse-sampled distributions.
**/
kernel void render(global             float4 *frm_data,
                   constant  struct FRM_INFO *frm_info,
                   global    struct SVO_NODE *geometry,
                   constant  struct OBSERVER *observer
                   /*read_only image2d_array_t  material,
                   read_only image2d_array_t  sampling*/)
{
    if (has_work(frm_info))
    {
        ulong4 id = guid(frm_info);
        struct PRNG rng = prng_init(id);
        float2 coords = resolve(frm_info);
        float ratio = get_ratio(frm_info);

        for (size_t t = 0; t < sample_count(); ++t)
        {
            float2 uv = get_uv(frm_info, coords + sample(t));
            struct Ray ray = project(observer, uv.x, uv.y, ratio);

            // TODO: pass materials/lights to integrator

            float3 color = integrate(ray, geometry, &rng);
            accumulate(frm_info, frm_data, color);
        }
    }
}

kernel void buf2tex(global             float4 *frm_data,
                    constant  struct FRM_INFO *frm_info,
                    write_only      image2d_t  tex_data)
{
    write_imagef(tex_data, convert_int2(resolve(frm_info)),
                 (float4)(get_color(frm_info, frm_data), 1));
}
