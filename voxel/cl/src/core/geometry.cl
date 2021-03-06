/* OpenCL 1.2 --- core/geometry.cl                             IMPLEMENTATION */

#include <core/geometry.cl>
#include <core/math_lib.cl>

struct Geometry
{
    uint child[8];
};

typedef struct Geometry SVO_NODE;

typedef struct STACK_ITEM
{
    uint offset;
    float hit;
    struct Box cube;
} STACK_ITEM;

bool traverse(global struct Geometry *geometry,
              const struct Ray ray, float range,
              float *nearest, struct Hit_Info *hit_info)
{
    STACK_ITEM stack[20];
    size_t sp = 0;

    stack[0].offset = 0;
    stack[0].hit = -INFINITY;
    stack[0].cube = (struct Box){(float3)(-1, -1, -1), (float3)(+1, +1, +1)};
    ++sp;

    float3 invdir = native_recip(ray.d); // ??

    *nearest = range;

    STACK_ITEM ns;

    while (sp)
    {
        STACK_ITEM s = stack[--sp];

        if (s.hit < *nearest)
        {
            if (s.offset & 0x80000000)
            {
                s.offset ^= 0x80000000; /* Decode this leaf offset. */
                *nearest = s.hit;
                ns = s;
            }
            else
            {
                SVO_NODE current = geometry[s.offset];
                for (size_t t = 0; t < 8; ++t)
                {
                    uint child = current.child[t];
                    if (child == 0x00000000) continue;

                    STACK_ITEM node;
                    node.offset = child;
                    node.cube = subdivide(s.cube, t);

                    if (intersect(ray, node.cube, invdir, &node.hit))
                        if (node.hit < *nearest) stack[sp++] = node;
                }
            }
        }
    }

    if (hit_info)
    {
        hit_info->basis = box_basis(ray, ns.cube, invdir);
    }

    return *nearest < range;
}

bool occlude(global struct Geometry *geometry,
             const struct Ray ray, float range)
{
    STACK_ITEM stack[20];
    size_t sp = 0;

    stack[0].offset = 0;
    stack[0].hit = 0;
    stack[0].cube = (struct Box){(float3)(-1, -1, -1), (float3)(+1, +1, +1)};
    ++sp;

    float3 invdir = native_recip(ray.d);

    while (sp)
    {
        STACK_ITEM s = stack[--sp];

        if (s.hit < range)
        {
            if (s.offset & 0x80000000)
            {
                return true;
            }
            else
            {
                SVO_NODE current = geometry[s.offset];
                for (size_t t = 0; t < 8; ++t)
                {
                    uint child = current.child[t];
                    if (child == 0x00000000) continue;

                    STACK_ITEM node;
                    node.offset = child;
                    node.cube = subdivide(s.cube, t);

                    if (intersect(ray, node.cube, invdir, &node.hit))
                        if (node.hit < range) stack[sp++] = node;
                }
            }
        }
    }

    return false;
}

bool occludes(global struct Geometry *geometry, const struct Ray ray,
              float range)
{
    return occlude(geometry, ray, range);
}

bool intersects(global struct Geometry *geometry, const struct Ray ray,
                float range, float *nearest, struct Hit_Info *hit_info)
{
    return traverse(geometry, ray, range, nearest, hit_info);
}
