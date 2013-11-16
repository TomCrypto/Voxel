#pragma once

/* Voxel test with procedural data. */

#include "material.hpp"

#include "math/vector3.hpp"
#include <cmath>

#include <cstdint>

#include <cstddef>

#include <algorithm>

#include "contact.hpp"

#include "geometry/voxel_types.hpp"

#include "geometry/fast_stack.hpp"

#include "geometry/aabb.hpp"

// the encoding currently used for voxels imposes a maximum of 2 billion nodes
// but it may make sense to use uint64_t and use the extra 32 bits in the leaf
// nodes for fast-changing material properties (or other)
// (we'd need to know how many voxels constitute a typical world with LOD, and
//  see if double the memory usage is worth a richer & faster material system)

struct Node
{
    uint32_t child[8];
};

uint32_t encode_leaf(const uint16_t &normal, const uint16_t &material)
{
    return (((material & 0x7FFF) << 16) | normal) | 0x80000000;
}

// assumes the high bit marker has already been removed
void decode_leaf(const uint32_t &leaf, uint16_t &normal, uint16_t &material)
{
    material = leaf >> 16;
    normal = leaf & 0xFFFF;
}

#define svo_depth 9 // TEMPORARY (will not be hardcoded later)

#define STACK_SIZE (4 * svo_depth) // can probably bring this down with some mathematical analysis of worst-case SVO traversal
                                   // (until we figure out how stackless traversal works, anyway)
                                   // (it doesn't really matter on the CPU due to how the stack works, but will be a killer on
                                   // the GPU as it will increase register pressure and kill parallelism ---> need stackless)

static distance3 light_pos = distance3(0.2f, -0.35f, 0.3f);

class VoxelTest
{
public:
    distance3 light() const
    {
        return light_pos;
    }
    
    bool intersects(const distance3 &origin, const distance3 &direction,
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
	    printf("Building SVO (this is done only once).\n");
	    init_mem();
	    root = build_SVO(svo_depth, world);
	    printf("SVO built (%zu nodes, depth = %d).\n",
	           node_offset, svo_depth);
	}
	
private:
    const aabb world = aabb{distance3(-1, -1, -1), distance3(1, 1, 1)};

    // looks up a voxel through its memory address (this could be an offset later on
    // for virtualization) and fills in an appropriate Contact structure to indicate
    // the material and surface normal, when applicable
    
    // this function has some pretty strong invariants to maintain, pay attention.
    // (read: if the invariants are not maintained, traversal will FAIL horribly)
    
    // (procedural data generation can happen here as well)
    inline
    bool lookup(const  distance3 &origin, const distance3 &direction,
                const stack_item   &leaf,       distance    &nearest,
                                                    Contact &contact) const
    {
        // default implementation, voxels are cubic and solid
        
        (void)origin;
        (void)direction;
        
        decode_leaf(leaf.offset, contact.normal, contact.material);
        
        nearest = leaf.hit; // IMPORTANT: you must set "nearest" to the
        // nearest intersection within the voxel's bounding box, IF AND
        // ONLY IF this intersection is closer than the currently recorded
        // "nearest" value (and if this is the case, populate "contact",
        // else LEAVE IT ALONE).

        return true; // return if and only if "nearest" was modified
    } __attribute__ ((hot))

    // this is the heart of the voxel renderer, the SVO traversal code
    // this is where optimization efforts should be focused
    // current optimizations and todo's:
    // DONE: convert to iteration and use a stack
    // TODO: implement and benchmark stackless traversal algorithms
    // DONE: uses early rejection by sorting children
    // DONE: fix remaining bugs

    // CURRENT REASONS FOR BEING SLOW:
    // 1. no SSE vector instructions  [WIP]
    // 2. unnecessary ray_aabb call in leaf [fixed]
    // 3. recursion [fixed]
    // 4. "intersections" buffer not optimized [fixed]
    // 5. tree nodes allocated via malloc(), poor locality [fixed]
    // 6. ray_aabb is not optimized at all  [fixed]
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
        traversal_stack<STACK_SIZE> stack(root, world);
        const distance3 &invdir = 1 / direction;

        bool hit = false;
        nearest = range;

        while (!stack.empty())
        {
            auto s = stack.pop();
            if (s.hit < nearest)
            {
                if (s.offset & 0x80000000)
                {
                    s.offset ^= 0x80000000; /* Decode this leaf offset. */
                    hit |= lookup(origin, direction, s, nearest, contact);
                    if (occlusion && hit) return true; /* Voxel is hit. */
                }
                else
                {
                    size_t last = stack.position();
                    auto current = nodes[s.offset];
                    for (size_t t = 0; t < 8; ++t)
                    {
                        uint32_t child = current.child[t];
                        if (child == 0x00000000) continue;
                        auto node = s.subdivide(child, t);
                        
                        if (intersect(origin, invdir, node.cube, node.hit))
                            if (node.hit < nearest) stack.push(node);
                    }
                    
                    /* Push back to front. */
                    stack.special_sort(last);
                }
            }
        }

        return hit;
	} __attribute__ ((hot))

    // this will be done in a separate program later on (SVO's will be
    // loaded on the fly, no time to build them while streaming voxels)
    uint32_t build_SVO(int depth, const aabb &cube)
    {
        if (depth == 0)
        {
            // this is a leaf - add voxel
            
            uint16_t normal, material;
            get_voxel_data(cube, normal, material);
            
            // encode leaf data into 32-bit offset
            return encode_leaf(normal, material);
        }
        
        Node *node = alloc_node();
        
        // build children here
        for (int t = 0; t < 8; ++t)
        {
            aabb child = split_node(cube, t);
            
            if (!contains_voxels(child)) node->child[t] = 0;
            else node->child[t] = build_SVO(depth - 1, child);
        }
        
        return (uint32_t)(node - nodes);
    }
    
    float heightmap(distance x, distance z) const // TEMPORARY
    {
        //return 0.05f * (sin(x) + cos(z)) - 0.7f;
        
        if ((x <= -0.525f) || (x >= 0.6f)) return std::numeric_limits<distance>::infinity();
        if ((z <= 0.2f)) return std::numeric_limits<distance>::infinity();
        
        
        return -0.7f + 0.03f * (sin(15 * z) + sin(10 * x + 1));
        
        //return -2.0f/3.0f + 0.2f * x * x;
    }
    
    math::float3 get_normal(distance x, distance z) const // TEMPORARY
    {
        distance dx = 0.2 * cos(10 * x + 1);
        distance dz = 0.3 * cos(15 * z);

        //distance dx = 0.4f * x;
        //distance dz = 0;
    
        return normalize(distance3(dx, 1, dz));
    }
    
    bool contains_voxels(const aabb &node) const // TEMPORARY
    {
        int r = 10;
        
        for (int px = 0; px < r; ++px)
            for (int pz = 0; pz < r; ++pz)
            {
                distance x = node.min.x + (node.max.x - node.min.x) * (distance)px / r;
                distance z = node.min.z + (node.max.z - node.min.z) * (distance)pz / r;
                
                distance height = heightmap(x, z);
                
                if ((node.min.y <= height) && (height <= node.max.y)) return true;
            }
        
        return false;
    }
    
    void get_voxel_data(const aabb &node, uint16_t &normal, uint16_t &material) const
        // TEMPORARY
    {
        int r = 10;
        
        for (int px = 0; px < r; ++px)
            for (int pz = 0; pz < r; ++pz)
            {
                distance x = node.min.x + (node.max.x - node.min.x) * (distance)px / r;
                distance z = node.min.z + (node.max.z - node.min.z) * (distance)pz / r;
                
                distance height = heightmap(x, z);
                
                if ((node.min.y <= height) && (height <= node.max.y))
                {
                    normal = encode_normal(get_normal(x, z));
                    material = (x < 0) ? 0 : 1;
                    
                    return;
                }
            }
    }
    
    // pray your libc overcommits :)
    #define NODE_MEMORY (1024 * 1024 * 1024)
    
    Node *nodes;
    size_t node_offset;
    
    void init_mem()
    {
        nodes = (Node*)malloc(NODE_MEMORY);
        node_offset = 0;
    }
    
    Node *alloc_node()
    {
        Node *ptr = nodes + node_offset;
        node_offset++;
        return ptr;
    }
    
    uint32_t root;
};
