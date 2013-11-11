#pragma once

/* Voxel test with procedural data. */

// THIS WILL BE REFACTORED AND IMPROVED SOON

#include "math/vector3.hpp"
#include <cmath>

#include <algorithm>

struct Voxel
{
    math::float3 normal;
    math::float3 rgb;
};

struct SVONode
{
    void *children[8];
};

static int voxel_count = 0;

bool ray_aabb(math::float3 origin, math::float3 direction,
              math::float3 bmin, math::float3 bmax,
              float* near, float* far)
{
    math::float3 bot = (bmin - origin) / (direction + math::float3(1e-6f, 1e-6f, 1e-6f)); // TODO: epsilons needed to avoid errors (find solution)
    math::float3 top = (bmax - origin) / (direction + math::float3(1e-6f, 1e-6f, 1e-6f));

    //math::float3 tmin = std::min(top, bot);
    //math::float3 tmax = std::max(top, bot);
    
    math::float3 tmin, tmax;
    
    tmin.x = std::min(top.x, bot.x);
    tmin.y = std::min(top.y, bot.y);
    tmin.z = std::min(top.z, bot.z);
    
    tmax.x = std::max(top.x, bot.x);
    tmax.y = std::max(top.y, bot.y);
    tmax.z = std::max(top.z, bot.z);

    *near = std::max(std::max(tmin.x, tmin.y), tmin.z);
    *far  = std::min(std::min(tmax.x, tmax.y), tmax.z);

    return !(*near > *far) && *far > 0;
}

class VoxelTest
{
public:
    math::float3 light() const
    {
        return math::float3(0.2f, -0.4f, 0.3f);
    }
    
    bool traverse(const math::float3 &origin, const math::float3 &direction,
		          float &distance, Contact &contact) const
	{
	    return traverse(root, svo_depth, origin, direction, math::float3(-1, -1, -1), math::float3(1, 1, 1), distance, contact);
	}
	
	VoxelTest()
	{
	    printf("Building SVO (this is done only once).\n");
	    printf("This will take a long time because my voxel containment test is extraordinarily bad. "
	           "Go make yourself a coffee or something while it finds out which voxels\n"
	           "should be filled.\n");
	    root = (SVONode*)build_SVO(svo_depth, math::float3(-1, -1, -1), math::float3(1, 1, 1));
	    printf("SVO built (%d voxels).\n", voxel_count);
	}
	
private:
    const size_t svo_depth = 12;
    
    bool traverse(void *node, int depth, const math::float3 &origin, const math::float3 &direction,
		          math::float3 min, math::float3 max, float &distance, Contact &contact) const
	{
	    if (depth == 0)
	    {
	        // we've reached a leaf node, so we intersect it - return that
	        Voxel *voxel = (Voxel*)node;
	        contact.rgb = voxel->rgb;
	        contact.normal = voxel->normal;
	        float tmp;
	        return ray_aabb(origin, direction, min, max, &distance, &tmp);
	    }
	    
	    // otherwise, test intersection against each child
	    
	    SVONode *parent = (SVONode*)node;
	    
	    distance = std::numeric_limits<float>::infinity();
	    
	    for (int t = 0; t < 8; ++t)
	    {
	        if (parent->children[t] == nullptr) continue;
	    
	        math::float3 min_child, max_child;
	        subdivide(min, max, t, min_child, max_child);
	    
	        float near, far;
	        Contact contact2;
	    
	        if (ray_aabb(origin, direction, min_child, max_child, &near, &far))
	        {
	            float d;
	            if (traverse(parent->children[t], depth - 1, origin, direction, min_child, max_child, d, contact2))
	            {
	                if ((d > 0) && (d < distance))
	                {
	                    contact = contact2;
	                    distance = d;
	                }
	            }
	        }
	    }
	    
	    return ((distance > 0) && (distance < std::numeric_limits<float>::infinity()));
	}

    void *build_SVO(int depth, math::float3 min_node, math::float3 max_node)
    {
        if (depth == 0)
        {
            ++voxel_count;
        
            // this is a leaf - add voxel
            Voxel *voxel = new Voxel();
            get_voxel_data(min_node, max_node, voxel);
            return voxel;
        }
        
        SVONode *node = new SVONode();
        
        // build children here
        for (int t = 0; t < 8; ++t)
        {
            math::float3 min_child, max_child;
            subdivide(min_node, max_node, t, min_child, max_child);
            
            if (!contains_voxels(min_child, max_child)) node->children[t] = nullptr;
            else node->children[t] = build_SVO(depth - 1, min_child, max_child);
        }
        
        return node;
    }
    
    void subdivide(math::float3 min_node, math::float3 max_node, int index,
                   math::float3 &min_child, math::float3 &max_child) const
    {
        math::float3 extent = (max_node - min_node) / 2;
        
        math::float3 origin = (max_node + min_node) / 2;
        math::float3 new_origin;
    
        new_origin.x = origin.x + extent.x * (index&4 ? .5f : -.5f);
        new_origin.y = origin.y + extent.y * (index&2 ? .5f : -.5f);
        new_origin.z = origin.z + extent.z * (index&1 ? .5f : -.5f);
        
        min_child = new_origin - extent * .5f;
        max_child = new_origin + extent * .5f;
    }
    
    float heightmap(float x, float z) const
    {
        //return 0.05f * (sin(x) + cos(z)) - 0.7f;
        
        if ((x <= -0.525f) || (x >= 0.6f)) return std::numeric_limits<float>::infinity();
        if ((z <= 0.2f)) return std::numeric_limits<float>::infinity();
        
        
        return -0.7f + 0.02f * (sin(15 * z) + sin(10 * x + 1));
        
        //return -2.0f/3.0f;
    }
    
    math::float3 normal(float x, float z) const
    {
        float dx = 0.2 * cos(10 * x + 1);
        float dz = 0.3 * cos(15 * z);
    
        return normalize(math::float3(dx, 1, dz));
    }
    
    bool contains_voxels(math::float3 min, math::float3 max) const
    {
        int r = 10;
        
        for (int px = 0; px < r; ++px)
            for (int pz = 0; pz < r; ++pz)
            {
                float x = min.x + (max.x - min.x) * (float)px / r;
                float z = min.z + (max.z - min.z) * (float)pz / r;
                
                float height = heightmap(x, z);
                
                if ((min.y <= height) && (height <= max.y)) return true;
            }
        
        return false;
    }
    
    void get_voxel_data(math::float3 min, math::float3 max, Voxel *voxel) const
    {
        int r = 10;
        
        for (int px = 0; px < r; ++px)
            for (int pz = 0; pz < r; ++pz)
            {
                float x = min.x + (max.x - min.x) * (float)px / r;
                float z = min.z + (max.z - min.z) * (float)pz / r;
                
                float height = heightmap(x, z);
                
                if ((min.y <= height) && (height <= max.y))
                {
                    voxel->normal = normal(x, z);
                    voxel->rgb = math::float3(1, 1, 1) * std::min(std::max(0.0f, std::abs(height)), 1.0f);
                }
            }
    }
    
    SVONode *root;
};
