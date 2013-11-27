/* OpenCL 1.2 --- core/frame_io.cl                             IMPLEMENTATION */

#include <core/frame_io.cl>

bool has_work(constant struct FRM_INFO *frm_info)
{
    return true;
}

float2 resolve(constant struct FRM_INFO *frm_info)
{
    return (float2)(get_global_id(0) % frm_info->dim.x,
                    frm_info->dim.y - 1 - get_global_id(0) / frm_info->dim.x);
}

float2 get_uv(constant struct FRM_INFO *frm_info, float2 v)
{
    return (v / convert_float2(frm_info->dim.xy));
}

float get_ratio(constant struct FRM_INFO *frm_info)
{
    return (float)frm_info->dim.x / (float)frm_info->dim.y;
}

void accumulate(constant struct FRM_INFO *frm_info,
                global            float4 *frm_data,
                                  float3  computed)
{
    frm_data[get_global_id(0)] += (float4)(computed, 1);
}

float3 get_color(constant struct FRM_INFO *frm_info,
                 global            float4 *frm_data)
{
    float4 color = frm_data[get_global_id(0)];
    return color.xyz / color.w; // RGBn format
}

ulong4 guid(constant struct FRM_INFO *frm_info)
{
    ulong frame_id = ((ulong)frm_info->dim.x << 32)  // 0..2^32 - 1 (width)
                   | ((ulong)frm_info->dim.y <<  0); // 0..2^32 - 1 (height)

    return (ulong4)(get_global_id(0),
                    get_global_id(1),
                    frm_info->ctr,
                    frame_id);
}
