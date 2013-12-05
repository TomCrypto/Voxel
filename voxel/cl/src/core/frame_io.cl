/* OpenCL 1.2 --- core/frame_io.cl                             IMPLEMENTATION */

#include <core/frame_io.cl>

struct Frm_Info
{
    uint2 dim;
    ulong ctr;
};

bool has_work(constant struct Frm_Info *frm_info)
{
    return get_global_id(0) < frm_info->dim.x * frm_info->dim.y;
}

ulong get_counter(constant struct Frm_Info *frm_info)
{
    return frm_info->ctr;
}

float2 resolve(constant struct Frm_Info *frm_info)
{
    return (float2)(get_global_id(0) % frm_info->dim.x,
                    frm_info->dim.y - 1 - get_global_id(0) / frm_info->dim.x);
}

float2 get_uv(constant struct Frm_Info *frm_info, float2 p)
{
    return (p / convert_float2(frm_info->dim.xy));
}

float get_ratio(constant struct Frm_Info *frm_info)
{
    return (float)frm_info->dim.x / (float)frm_info->dim.y;
}

void accumulate(constant struct Frm_Info *frm_info,
                global            float4 *frm_data,
                                  float3  computed)
{
    frm_data[get_global_id(0)] += (float4)(computed, 1);
}

float3 get_color(constant struct Frm_Info *frm_info,
                 global            float4 *frm_data)
{
    float4 color = frm_data[get_global_id(0)];
    return color.xyz / color.w; // RGBn format
}

void tex_copy(constant struct Frm_Info *frm_info,
              global            float4 *frm_data,
              write_only     image2d_t  tex_data)
{
    write_imagef(tex_data, convert_int2(resolve(frm_info)),
                 (float4)(get_color(frm_info, frm_data), 1));
}

ulong4 guid(constant struct Frm_Info *frm_info)
{
    ulong frame_id = upsample(frm_info->dim.x, frm_info->dim.y);

    return (ulong4)(get_global_id(0),
                    get_global_id(1),
                    frm_info->ctr,
                    frame_id);
}
