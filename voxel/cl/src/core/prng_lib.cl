/* OpenCL 1.2 --- core/prng_lib.cl                             IMPLEMENTATION */

#include <core/prng_lib.cl>

static void renew(ulong4 *state, ulong4 seed)
{
    ulong4 block = *state + seed;

    #pragma unroll
    for (size_t t = 0; t < 4; t++)
    {
        block.lo += block.hi; block.hi = rotate(block.hi, (ulong2)(14, 16));
        block.hi ^= block.lo; block = block.xywz;
        block.lo += block.hi; block.hi = rotate(block.hi, (ulong2)(52, 57));
        block.hi ^= block.lo; block = block.xywz;
        block.lo += block.hi; block.hi = rotate(block.hi, (ulong2)(23, 40));
        block.hi ^= block.lo; block = block.xywz;
        block.lo += block.hi; block.hi = rotate(block.hi, (ulong2)( 5, 37));
        block.hi ^= block.lo; block = block.xywz;

        block += seed;

        block.lo += block.hi; block.hi = rotate(block.hi, (ulong2)(25, 33));
        block.hi ^= block.lo; block = block.xywz;
        block.lo += block.hi; block.hi = rotate(block.hi, (ulong2)(46, 12));
        block.hi ^= block.lo; block = block.xywz;
        block.lo += block.hi; block.hi = rotate(block.hi, (ulong2)(58, 22));
        block.hi ^= block.lo; block = block.xywz;
        block.lo += block.hi; block.hi = rotate(block.hi, (ulong2)(32, 32));
        block.hi ^= block.lo; block = block.xywz;

        block += seed;
    }

    *state ^= block;
}

struct PRNG prng_init(ulong4 instance_id)
{
    return (struct PRNG){0, instance_id};
}

float4 rand4(struct PRNG *prng)
{
    renew(&prng->state, prng->key);
    const ulong hi = 18446744073709551615UL;
    return convert_float4(prng->state) / hi;
}

float3 rand3(struct PRNG *prng)
{
    return rand4(prng).xyz;
}

float2 rand2(struct PRNG *prng)
{
    return rand4(prng).xy;
}

float rand(struct PRNG *prng)
{
    return rand4(prng).x;
}

uint4 rand4i(struct PRNG *prng, uint n)
{
    renew(&prng->state, prng->key);
    return convert_uint4(prng->state) % n;
}

uint3 rand3i(struct PRNG *prng, uint n)
{
    return rand4i(prng, n).xyz;
}

uint2 rand2i(struct PRNG *prng, uint n)
{
    return rand4i(prng, n).xy;
}

uint randi(struct PRNG *prng, uint n)
{
    return rand4i(prng, n).x;
}

ulong4 rand4l(struct PRNG *prng, ulong n)
{
    renew(&prng->state, prng->key);
    return prng->state % n;
}

ulong3 rand3l(struct PRNG *prng, ulong n)
{
    return rand4l(prng, n).xyz;
}

ulong2 rand2l(struct PRNG *prng, ulong n)
{
    return rand4l(prng, n).xy;
}

ulong randl(struct PRNG *prng, ulong n)
{
    return rand4l(prng, n).x;
}
