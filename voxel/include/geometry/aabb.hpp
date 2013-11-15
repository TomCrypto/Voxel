#pragma once

#include <cstddef>

#include "geometry/voxel_types.hpp"

struct aabb
{
    distance3 min, max;
};

// precomputed split offsets for efficiency
static const distance3 offset[8] =
{
    distance3(0, 0, 0),
    distance3(0, 0, 1),
    distance3(0, 1, 0),
    distance3(0, 1, 1),
    distance3(1, 0, 0),
    distance3(1, 0, 1),
    distance3(1, 1, 0),
    distance3(1, 1, 1),
};

// this is also needed by the traversal code (no sense storing the
// bounding box of each voxel when it can be readily inferred from
// its location in the octree datastructure)

// to optimize
inline aabb split_node(const aabb &node, size_t index)
{
    distance3 extent = (node.max - node.min) * 0.5f;
    distance3 p = node.min + extent * offset[index];
    return aabb{p, p + extent};
}

// to optimize
inline bool intersect(const distance3 &origin,
                      const distance3 &invdir,
                      const aabb &cube,
                            distance &near)
{
    distance3 bot = (cube.min - origin) * invdir;
    distance3 top = (cube.max - origin) * invdir;

    math::float3 tmin = std::min(top, bot);
    math::float3 tmax = std::max(top, bot);
    
    near = std::max(std::max(tmin.x, tmin.y), tmin.z);
    distance far = std::min(std::min(tmax.x, tmax.y), tmax.z);

    return near < far && far > 0;
}
