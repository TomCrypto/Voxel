#pragma once

#include "math/vector3.hpp"

/** @struct Material
  *
  * Defines the materials encountered by rays during ray-geometry intersection,
  * and describes the appearance of objects in the world.
**/
struct Material
{
    math::float3 rgb; // TEMPORARY: for now, just a solid color
    // TODO: later on, this will be a single integer indexing a large material
    // database. material lookup and calculations are not a bottleneck anyway
};
