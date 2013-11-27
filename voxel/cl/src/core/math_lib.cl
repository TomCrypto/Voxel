/* OpenCL 1.2 --- core/math_lib.cl                             IMPLEMENTATION */

#include <core/math_lib.cl>

float3 lerp(float3 a, float3 b, float t)
{
    return a + (b - a) * t;
}

void advance(struct Ray *ray, float t, float3 d)
{
    ray->o += ray->d * t;
    ray->d = d;
}

constant float3 off[8] =
{
    (float3)(0, 0, 0),
    (float3)(0, 0, 1),
    (float3)(0, 1, 0),
    (float3)(0, 1, 1),
    (float3)(1, 0, 0),
    (float3)(1, 0, 1),
    (float3)(1, 1, 0),
    (float3)(1, 1, 1),
};

struct Box subdivide(const struct Box box, size_t index)
{
    float3 e = (box.h - box.l) * 0.5f;
    float3 l = box.l + e * off[index];
    return (struct Box){l, l + e};
}

bool intersect(const struct Ray ray, const struct Box box,
               const float3 inv_dir, float *t)
{
    float3 a = (box.l - ray.o) * inv_dir;
    float3 b = (box.h - ray.o) * inv_dir;
    float3 u = min(a, b), v = max(a, b);

    *t      = max(max(u.x, u.y), u.z);
    float f = min(min(v.x, v.y), v.z);

    return *t < f && f > 0;
}

bool intersect_f(const struct Ray ray, const struct Box box,
                 float *t)
{
    return intersect(ray, box, native_recip(ray.d), t);
}

