/* OpenCL 1.2 --- core/prng_lib.cl                                  INTERFACE */

/** @file include/core/prng_lib.cl
  *
  * @brief Kernel PRNG Provider
  *
  * This unit provides pseudorandom number generation capabilities to a kernel,
  * by implementing an efficient PRNG suitable for parallel architectures.
  *
  * The algorithm implemented here is a variation of Threefish-256.
**/

#pragma once

/** @struct PRNG
  *
  * This structure contains the internal state of a PRNG, in order to calculate
  * pseudorandom numbers efficiently - each work item should have its own.
**/
struct PRNG
{
    ulong4 state, key;
};

/** This function creates a new PRNG state, and initializes it to zero.
  *
  * @param instance_id  The ID to create the PRNG with, must be unique.
  *
  * @returns A new PRNG state, ready for use.
**/
struct PRNG prng_init(ulong4 instance_id);

/********************************* FLOAT RAND *********************************/

/** This function computes up to four uniform pseudorandom reals.
  *
  * @param prng  A pointer to a PRNG state.
  *
  * @returns Four unbiased uniform pseudorandom numbers in [0..1).
**/
float4 rand4(struct PRNG *prng);

/** Utility function to get three pseudorandom reals.
  *
  * @see rand4()
**/
float3 rand3(struct PRNG *prng);

/** Utility function to get two pseudorandom reals.
  *
  * @see rand4()
**/
float2 rand2(struct PRNG *prng);

/** Utility function to get a single pseudorandom real.
  *
  * @see rand4()
**/
float rand(struct PRNG *prng);

/******************************** INTEGER RAND ********************************/

/** This function computes up to four uniform pseudorandom integers in a range.
  *
  * @param prng  A pointer to a PRNG state.
  * @param n     The maximum value (exclusive).
  *
  * @returns Four uniform pseudorandom numbers in [0..n).
  *
  * @remarks \c n must be greater than 0 (and should be greater than 1).
  *
  * @remarks There may be a small bias for non-power of two values of \c n.
**/
uint4 rand4i(struct PRNG *prng, uint n);

/** Utility function to get three pseudorandom integers in a range.
  *
  * @see rand4()
**/
uint3 rand3i(struct PRNG *prng, uint n);

/** Utility function to get two pseudorandom integers in a range.
  *
  * @see rand4()
**/
uint2 rand2i(struct PRNG *prng, uint n);

/** Utility function to get a single pseudorandom integer in a range.
  *
  * @see rand4()
**/
uint randi(struct PRNG *prng, uint n);

/********************************** LONG RAND *********************************/

/** This function computes up to four uniform pseudorandom integers in a range.
  *
  * @param prng  A pointer to a PRNG state.
  * @param n     The maximum value (exclusive).
  *
  * @returns Four uniform pseudorandom numbers in [0..n).
  *
  * @remarks \c n must be greater than 0 (and should be greater than 1).
  *
  * @remarks There may be a small bias for non-power of two values of \c n.
**/
ulong4 rand4l(struct PRNG *prng, ulong n);

/** Utility function to get three pseudorandom integers in a range.
  *
  * @see rand4()
**/
ulong3 rand3l(struct PRNG *prng, ulong n);

/** Utility function to get two pseudorandom integers in a range.
  *
  * @see rand4()
**/
ulong2 rand2l(struct PRNG *prng, ulong n);

/** Utility function to get a single pseudorandom integer in a range.
  *
  * @see rand4()
**/
ulong randl(struct PRNG *prng, ulong n);
