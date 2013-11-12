#pragma once

/* Voxel test with procedural data. */

// THIS WILL BE REFACTORED AND IMPROVED SOON

#include "math/vector3.hpp"
#include <cmath>

#include <algorithm>

////////

typedef float distance;
typedef math::float3 distance3;

////////

struct Voxel
{
    math::float3 normal; // distance3?
    math::float3 rgb; // that's a color
};

struct SVONode
{
    void *children[8];
};

struct Intersection
{
    size_t index;
    distance near, far;
};

static void swap(Intersection *a, Intersection *b)
{
    if (a->near > b->near)
    {
        Intersection tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

static void sort_network(Intersection *list)
{
    swap(&list[0], &list[2]);
    swap(&list[1], &list[3]);
    swap(&list[0], &list[1]);
    swap(&list[2], &list[3]);
    swap(&list[1], &list[2]);
}

static int voxel_count = 0;

bool ray_aabb(distance3 origin, distance3 direction,
              distance3 bmin, distance3 bmax,
              distance* near, distance* far)
{
    // TODO: epsilons needed to avoid errors (find solution)
    distance3 bot = (bmin - origin) / (direction + math::float3(1e-6f, 1e-6f, 1e-6f));
    distance3 top = (bmax - origin) / (direction + math::float3(1e-6f, 1e-6f, 1e-6f));

    //math::float3 tmin = std::min(top, bot);
    //math::float3 tmax = std::max(top, bot);
    
    distance3 tmin, tmax;
    
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
    distance3 light() const
    {
        return distance3(0.2f, -0.4f, 0.3f);
    }
    
    bool traverse(const distance3 &origin, const distance3 &direction,
		          distance &dist, Contact &contact) const
	{
	    return traverse(root, svo_depth, origin, direction,
                        distance3(-1, -1, -1), distance3(1, 1, 1),
                        dist, std::numeric_limits<distance>::infinity(), contact);
	}

    bool occludes(const distance3 &origin, const distance3 &direction,
                  distance &dist, distance target_dist, Contact &contact) const
    {
        return traverse(root, svo_depth, origin, direction,
                        distance3(-1, -1, -1), distance3(1, 1, 1),
                        dist, target_dist, contact);
    }

    bool occludes(const distance3 &origin, const distance3 &direction,
                  distance target_dist) const
    {
        float dist;
        Contact contact;

        return traverse(root, svo_depth, origin, direction,
                        distance3(-1, -1, -1), distance3(1, 1, 1),
                        dist, target_dist, contact);
    }
	
	VoxelTest()
	{
	    printf("Building SVO (this is done only once).\n");
	    printf("This will take a long time because my voxel containment test is extraordinarily bad. "
	           "Go make yourself a coffee or something while it finds out which voxels "
	           "should be filled.\n");
	    root = (SVONode*)build_SVO(svo_depth, distance3(-1, -1, -1), distance3(1, 1, 1));
	    printf("SVO built (%d voxels, depth = %d).\n", voxel_count, svo_depth);
	}
	
private:
    const size_t svo_depth = 9; // TEMPORARY (will not be hardcoded later)
    
    // this is the heart of the voxel renderer, the SVO traversal code
    // this is where optimization efforts should be focused
    // current optimizations and todo's:
    // TODO: convert to iteration and use a stack
    // TODO: implement and benchmark stackless traversal algorithms
    // DONE: uses early rejection by sorting children
    // TODO: fix remaining bugs

    // CURRENT REASONS FOR BEING SLOW:
    // 1. no SSE vector instructions
    // 2. unnecessary ray_aabb call in leaf
    // 3. recursion
    // 4. "intersections" buffer not optimized
    // 5. tree nodes allocated via malloc(), poor locality
    // 6. ray_aabb is not optimized at all
    // 7. possible algorithmic improvements?

    // QUESTIONS TO RESOLVE:
    // 1. do we need "far" in the "intersections" buffer?
    // 2. are there any bugs? need a known voxel data set to test against
    //    (do that once we have interactivity, to better locate glitches)

    // ADDITIONAL INFO:
    // - final tree will potentially be very deep, with up to 64 levels
    //   (32 levels --> 64km^3 with 0.015 mm resolution)
    // - procedural data generation can be done outside the tree, once
    //   a voxel is intersected, by generating procedural data *inside*
    //   the intersected voxel, depending on its material
    //   (this is actually a REALLY GOOD idea as it completely gets rid
    //    of any cubic artifacts and improves subpixel performance)
    // - 64 levels --> 1800 AU^3 with 0.015 mm resolution
    //   (that's 274 billion km^3, I trust this will be enough)
    // - possibly use a hybrid approach, with two trees, one handling large
    //   64km^3 "chunks", optimized for ultra fast traversal, and another
    //   which focuses on culling chunks to improve performance
    bool traverse(void *node, int depth, const distance3 &origin, const distance3 &direction,
		          distance3 min, distance3 max, distance &dist, distance
                  max_dist, Contact &contact) const
	{
	    if (depth == 0)
	    {
	        // we've reached a leaf node, so we intersect it - return that
	        Voxel *voxel = (Voxel*)node;
	        contact.rgb = voxel->rgb;
	        contact.normal = voxel->normal;
	        distance tmp;

            // this is a waste - do it at the before-last depth
	        return ray_aabb(origin, direction, min, max, &dist, &tmp);
	    }
	    
	    // otherwise, test intersection against each child
	    
	    SVONode *parent = (SVONode*)node;
	    
	    dist = max_dist;
        Intersection intersections[4]; // at most

        for (size_t t = 0; t < 4; ++t)
        {
            intersections[t].near = std::numeric_limits<distance>::infinity();
            intersections[t].far = std::numeric_limits<distance>::infinity();
        }

        size_t count = 0;

        for (size_t t = 0; t < 8; ++t) // NOT RECURSIVE
        {
            if (parent->children[t] == nullptr) continue;

            distance3 min_child, max_child;
            subdivide(min, max, t, min_child, max_child);

            distance near, far;

            if (ray_aabb(origin, direction, min_child, max_child, &near, &far))
            {
                if (near < max_dist)
                {
                    intersections[count].index = t;
                    intersections[count].near = near;
                    intersections[count].far = far;
                    ++count;
                }
            }
        }

        // this will sort the intersections by nearest, so we can bail as
        // soon as we find an intersection
        sort_network(intersections);

        for (size_t t = 0; t < count; ++t)
        {
            size_t child_index = intersections[t].index;

            distance3 min_child, max_child;
            subdivide(min, max, child_index, min_child, max_child);

            float d;
            Contact tmp;

            if (traverse(parent->children[child_index], depth - 1,
                         origin, direction,
                         min_child, max_child,
                         d, dist, tmp))
            {
                if (d < dist) // is this closer?
                {
                    dist = d;
                    contact = tmp;
                }

                // is this intersection distance optimal?
                // (no point searching further away children)
                if ((t == count - 1) || (d < intersections[t + 1].near))
                    return true;
            }
        }

        return (dist > 0) && (dist < max_dist);
	}

    // this will be done in a separate program later on (SVO's will be
    // loaded on the fly, no time to build them while streaming voxels)
    void *build_SVO(int depth, distance3 min_node, distance3 max_node)
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
            distance3 min_child, max_child;
            subdivide(min_node, max_node, t, min_child, max_child);
            
            if (!contains_voxels(min_child, max_child)) node->children[t] = nullptr;
            else node->children[t] = build_SVO(depth - 1, min_child, max_child);
        }
        
        return node;
    }
    
    // this is also needed by the traversal code (no sense storing the
    // bounding box of each voxel when it can be readily inferred from
    // its location in the octree datastructure)
    void subdivide(distance3 min_node, distance3 max_node, int index,
                   distance3 &min_child, distance3 &max_child) const
    {
        distance3 extent = (max_node - min_node) / 2;
        
        distance3 origin = (max_node + min_node) / 2;
        distance3 new_origin;
    
        new_origin.x = origin.x + extent.x * (index&4 ? .5f : -.5f);
        new_origin.y = origin.y + extent.y * (index&2 ? .5f : -.5f);
        new_origin.z = origin.z + extent.z * (index&1 ? .5f : -.5f);
        
        min_child = new_origin - extent * .5f;
        max_child = new_origin + extent * .5f;
    }
    
    float heightmap(distance x, distance z) const // TEMPORARY
    {
        //return 0.05f * (sin(x) + cos(z)) - 0.7f;
        
        if ((x <= -0.525f) || (x >= 0.6f)) return std::numeric_limits<distance>::infinity();
        if ((z <= 0.2f)) return std::numeric_limits<distance>::infinity();
        
        
        return -0.7f + 0.03f * (sin(15 * z) + sin(10 * x + 1));
        
        //return -2.0f/3.0f + 0.2f * x * x;
    }
    
    math::float3 normal(distance x, distance z) const // TEMPORARY
    {
        distance dx = 0.2 * cos(10 * x + 1);
        distance dz = 0.3 * cos(15 * z);

        //distance dx = 0.4f * x;
        //distance dz = 0;
    
        return normalize(distance3(dx, 1, dz));
    }
    
    bool contains_voxels(distance3 min, distance3 max) const // TEMPORARY
    {
        int r = 10;
        
        for (int px = 0; px < r; ++px)
            for (int pz = 0; pz < r; ++pz)
            {
                distance x = min.x + (max.x - min.x) * (distance)px / r;
                distance z = min.z + (max.z - min.z) * (distance)pz / r;
                
                distance height = heightmap(x, z);
                
                if ((min.y <= height) && (height <= max.y)) return true;
            }
        
        return false;
    }
    
    void get_voxel_data(distance3 min, distance3 max, Voxel *voxel) const
        // TEMPORARY
    {
        int r = 10;
        
        for (int px = 0; px < r; ++px)
            for (int pz = 0; pz < r; ++pz)
            {
                distance x = min.x + (max.x - min.x) * (distance)px / r;
                distance z = min.z + (max.z - min.z) * (distance)pz / r;
                
                distance height = heightmap(x, z);
                
                if ((min.y <= height) && (height <= max.y))
                {
                    voxel->normal = normal(x, z);
                    voxel->rgb = math::float3(0.25, 0.75, 0.25) * std::min(std::max(0.0f, std::abs(height)), 1.0f);
                }
            }
    }
    
    SVONode *root;
};
