/* OpenCL 1.2 --- core/geometry.cl                             IMPLEMENTATION */

#include <core/geometry.cl>
#include <core/math_lib.cl>

typedef struct STACK_ITEM
{
    uint offset;
    float hit;
    struct Box cube;
} STACK_ITEM;

float3 get_light(void)
{
    return (float3)(0.2f, -0.35f, 0.3f);
}

bool traverse(global struct SVO_NODE *geometry,
              const struct Ray ray, float range,
              float *nearest)
{
    STACK_ITEM stack[32];
    size_t sp = 0;
    
    stack[0].offset = 0;
    stack[0].hit = -INFINITY;
    stack[0].cube = (struct Box){(float3)(-1, -1, -1), (float3)(+1, +1, +1)};
    ++sp;
    
    float3 invdir = native_recip(ray.d); // ??

    *nearest = range;

    while (sp)
    {
        STACK_ITEM s = stack[--sp];
        
        if (s.hit < *nearest)
        {
            if (s.offset & 0x80000000)
            {
                s.offset ^= 0x80000000; /* Decode this leaf offset. */
                *nearest = s.hit;
            }
            else
            {
                struct SVO_NODE current = geometry[s.offset];
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
    
    return *nearest < range;
}

bool occlude(global struct SVO_NODE *geometry,
             const struct Ray ray, float range)
{
    STACK_ITEM stack[32];
    size_t sp = 0;
    
    stack[0].offset = 0;
    stack[0].hit = 0;
    stack[0].cube = (struct Box){(float3)(-1, -1, -1), (float3)(+1, +1, +1)};
    ++sp;
    
    float3 invdir = 1.0f / ray.d;

    while (sp)
    {
        STACK_ITEM s = stack[--sp];
        
        if (s.offset & 0x80000000)
        {
            return true;
        }
        else
        {
            struct SVO_NODE current = geometry[s.offset];
            for (size_t t = 0; t < 8; ++t)
            {
                uint child = current.child[t];
                if (child == 0x00000000) continue;
                
                STACK_ITEM node;
                node.offset = child;
                node.cube = subdivide(s.cube, t);
                
                if (intersect(ray, node.cube, invdir, &node.hit))
                    stack[sp++] = node;
            }
        }
    }

    return false;
}

bool occludes(__global struct SVO_NODE *geometry, const struct Ray ray, float range)
{
    return occlude(geometry, ray, range);
}

bool depth_test(__global struct SVO_NODE *geometry, const struct Ray ray, float range, float *depth)
{
    return traverse(geometry, ray, range, depth);
}