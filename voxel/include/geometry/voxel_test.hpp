#pragma once

/* Voxel test with procedural data. */

#include "math/vector3.hpp"
#include <cmath>

#include <cstddef>

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

// this is also needed by the traversal code (no sense storing the
// bounding box of each voxel when it can be readily inferred from
// its location in the octree datastructure)

// to optimize
inline
static void subdivide(distance3 min_node, distance3 max_node, int index,
                      distance3 &min_child, distance3 &max_child)
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


struct SVONode
{
    void *child[8];
};

// this needs to be cleaned up
struct stack_item
{
    void *memptr;
    size_t depth;

    distance3 min, max; // bounding box for this node
    distance hit; // nearest intersection for this node

    stack_item()
    {
        hit = std::numeric_limits<distance>::infinity();
    }

    stack_item &operator =(const stack_item &record)
    {
        this->memptr = record.memptr;
        this->depth = record.depth;
        this->min = record.min;
        this->max = record.max;
        this->hit = record.hit;

        return *this;
    }

    stack_item(void *memptr, size_t depth, distance3 min, 
                 distance3 max)
    {
        this->memptr = memptr;
        this->depth = depth;
        this->min = min;
        this->max = max;
        this->hit = 0;
    }

    stack_item(void *memptr, size_t depth, distance3 min, distance3 max,
                 distance near)
    {
        this->memptr = memptr;
        this->depth = depth;
        this->min = min;
        this->max = max;
        this->hit = near;
    }

    // this should derive a stack_item from its parent
    stack_item(void *memptr, const stack_item &parent, size_t index)
    {
        this->memptr = memptr;
        this->depth = parent.depth - 1;
        this->hit = 0;

        subdivide(parent.min, parent.max, index,
                  this->min, this->max);
    }
};

static inline stack_item pop(const stack_item *stack, size_t &ptr)
{
    return stack[--ptr];
}

static inline void push(stack_item *stack, size_t &ptr, const stack_item &item)
{
    stack[ptr++] = item;
}

static inline void swap(stack_item *a, stack_item *b)
{
    if (a->hit > b->hit)
    {
        stack_item tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

static inline void sort_network(stack_item *list)
{
    swap(&list[0], &list[2]);
    swap(&list[1], &list[3]);
    swap(&list[0], &list[1]);
    swap(&list[2], &list[3]);
    swap(&list[1], &list[2]);
}

#define svo_depth 9 // TEMPORARY (will not be hardcoded later)

static int voxel_count = 0;

// to optimize
inline
static bool ray_aabb(distance3 origin, distance3 direction,
                     distance3 bmin, distance3 bmax,
                     distance &near)
{
    // TODO: epsilons needed to avoid errors (find solution)
    distance3 bot = (bmin - origin) / (direction + math::float3(1e-6f, 1e-6f, 1e-6f));
    distance3 top = (bmax - origin) / (direction + math::float3(1e-6f, 1e-6f, 1e-6f));

    math::float3 tmin = std::min(top, bot);
    math::float3 tmax = std::max(top, bot);

    near = std::max(std::max(tmin.x, tmin.y), tmin.z);
    distance far = std::min(std::min(tmax.x, tmax.y), tmax.z);

    return !(near > far) && far > 0;
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
        return traverse(origin, direction,
                        std::numeric_limits<distance>::infinity(), dist, contact, false);
	}

    bool occludes(const distance3 &origin, const distance3 &direction,
                  distance &dist, distance target_dist) const
    {
        Contact contact;
    
        return traverse(origin, direction,
                        target_dist, dist, contact, true);
    }

    bool occludes(const distance3 &origin, const distance3 &direction,
                  distance target_dist) const
    {
        distance dist;
        Contact contact;

        return traverse(origin, direction,
                        target_dist, dist, contact, true);
    }
    
    bool occludes(const distance3 &origin, const distance3 &direction) const
    {
        return occludes(origin, direction, std::numeric_limits<distance>::infinity());
    }

	
	VoxelTest()
	{
        world_min = distance3(-1, -1, -1);
        world_max = distance3(1, 1, 1);

	    printf("Building SVO (this is done only once).\n");
	    root = (SVONode*)build_SVO(svo_depth, distance3(-1, -1, -1), distance3(1, 1, 1));
	    printf("SVO built (%d voxels, depth = %d).\n", voxel_count, svo_depth);
	}
	
private:
    // looks up a voxel through its memory address (this could be an offset later on
    // for virtualization) and fills in an appropriate Contact structure to indicate
    // the material and surface normal, when applicable
    
    // this function has some pretty strong invariants to maintain, pay attention.
    
    // (procedural data generation can happen here as well)
    inline
    bool lookup(const  distance3 &origin, const distance3 &direction,
                const stack_item   &item,       distance    &nearest,
                                                    Contact &contact) const
    {
        // default implementation, voxels are cubic and solid
        
        (void)origin;
        (void)direction;
        
        Voxel *voxel = (Voxel*)item.memptr;
        contact.rgb = voxel->rgb;
        contact.normal = voxel->normal;
        nearest = item.hit; // IMPORTANT: you must set "nearest" to the
        // nearest intersection within the voxel's bounding box, IF AND
        // ONLY IF this intersection is closer than the currently recorded
        // "nearest" value (and if this is the case, populate "contact",
        // else LEAVE IT ALONE).

        return true; // return if and only if "nearest" was modified
    }

    // this is the heart of the voxel renderer, the SVO traversal code
    // this is where optimization efforts should be focused
    // current optimizations and todo's:
    // DONE: convert to iteration and use a stack
    // TODO: implement and benchmark stackless traversal algorithms
    // DONE: uses early rejection by sorting children
    // TODO: fix remaining bugs

    // CURRENT REASONS FOR BEING SLOW:
    // 1. no SSE vector instructions  [WIP]
    // 2. unnecessary ray_aabb call in leaf [fixed]
    // 3. recursion [fixed]
    // 4. "intersections" buffer not optimized [fixed]
    // 5. tree nodes allocated via malloc(), poor locality
    // 6. ray_aabb is not optimized at all  [WIP]
    // 7. possible algorithmic improvements?

    // QUESTIONS TO RESOLVE:
    // 1. do we need "far" in the "intersections" buffer? [NO]
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
    bool traverse(const distance3 &origin, const distance3 &direction,
                  const distance   &range,       distance    &nearest,
                        Contact  &contact, const bool       occlusion) const
    {
        stack_item stack[4 * svo_depth];
        size_t ptr = 0;

        bool hit = false;
        nearest = range;

        push(stack, ptr, stack_item(root, svo_depth, world_min, world_max));

        while (ptr)
        {
            /* Reject early if possible. */
            stack_item s = pop(stack, ptr);
            if (s.hit >= nearest) continue;

            if (!s.depth)
            {
                hit = lookup(origin, direction, s, nearest, contact);
                if (occlusion && hit) return true; /* We are done. */
            }
            else
            {
                SVONode *parent = (SVONode*)s.memptr;
                stack_item children[4];
                size_t hits = 0;

                for (size_t t = 0; t < 8; ++t)
                {
                    void *child = parent->child[t];
                    if (child == nullptr) continue;

                    stack_item c(child, s, t); /* Subdivide this node. */
                    if (ray_aabb(origin, direction, c.min, c.max, c.hit))
                        if (c.hit < nearest) push(children, hits, c);
                }

                sort_network(children); /* Improve early reject. */
                while (hits) push(stack, ptr, pop(children, hits));
            }
        }

        return hit;
	} __attribute__ ((hot))

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
            
            if (!contains_voxels(min_child, max_child)) node->child[t] = nullptr;
            else node->child[t] = build_SVO(depth - 1, min_child, max_child);
        }
        
        return node;
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
                    
                    float r = std::min(std::max(0.0f, std::abs(x)), 1.0f);
                    float g = std::min(std::max(0.0f, std::abs(z)), 1.0f);
                    float b = std::min(std::max(0.0f, std::abs(x + z)), 1.0f);
                    
                    voxel->rgb = math::float3(r, g, b) + math::float3(0.25f, 0.25f, 0.25f);
                }
            }
    }
    
    SVONode *root;

    distance3 world_min, world_max;
};
