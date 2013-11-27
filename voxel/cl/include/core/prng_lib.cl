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
