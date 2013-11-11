#pragma once

/* Voxel test with procedural data. */

#include "math/vector3.hpp"
#include <cmath>

#include <algorithm>

struct Voxel
{
    math::float3 normal;
    math::float3 rgb;
};

class VoxelTest
{
public:
    math::float3 light() const
    {
        return math::float3(0, 0, 0);
    }
    
    bool traverse(const math::float3 &origin, const math::float3 &direction,
		          float &distance, Contact &contact) const
	{
	    return false;
	}
	
	VoxelTest()
	{
	    //
	}
	
private:
    
};
