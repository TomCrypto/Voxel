#pragma once

#include <cstddef>

struct aabb
{
    math::float3 min, max;
};

// precomputed split offsets for efficiency
static const math::float3 offset[8] =
{
    math::float3(0, 0, 0),
    math::float3(0, 0, 1),
    math::float3(0, 1, 0),
    math::float3(0, 1, 1),
    math::float3(1, 0, 0),
    math::float3(1, 0, 1),
    math::float3(1, 1, 0),
    math::float3(1, 1, 1),
};

// this is also needed by the traversal code (no sense storing the
// bounding box of each voxel when it can be readily inferred from
// its location in the octree datastructure)

// to optimize
inline aabb split_node(const aabb &node, size_t index)
{
    math::float3 extent = (node.max - node.min) * 0.5f;
    math::float3 p = node.min + extent * offset[index];
    return aabb{p, p + extent};
}
