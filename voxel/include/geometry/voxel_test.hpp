// this will be refactored soon

#pragma once

/* Voxel test with procedural data. */

#include "math/vector3.hpp"
#include <cmath>

#include <cstdint>

#include <cstddef>

#include <algorithm>

#include "geometry/aabb.hpp"

struct Voxel
{
    uint16_t material;
};

struct Node
{
    uint32_t child[8];
};

inline uint32_t encode_leaf(const uint16_t &material)
{
    return (((material & 0x7FFF) << 16)) | 0x80000000;
}

#define RESOLUTION 64

#define svo_depth 5 // TEMPORARY (will not be hardcoded later)

class VoxelTest
{
public:
	VoxelTest()
	{
	    init_mem();
	    root = build_SVO(svo_depth, world, math::basic_vector3<int>(0, 0, 0), math::basic_vector3<int>(RESOLUTION - 1, RESOLUTION - 1, RESOLUTION - 1));
	    free_mem();
	}

	std::size_t bufSize()
	{
	    return node_offset * sizeof(Node);
	}

	void *getPtr()
	{
        return nodes;
    }

private:
    const aabb world = aabb{math::float3(-1, -1, -1), math::float3(1, 1, 1)};

	void split_int(const math::basic_vector3<int> &min, const math::basic_vector3<int> &max, int t,
	               math::basic_vector3<int> &child_min, math::basic_vector3<int> &child_max)
	{
	    static const math::basic_vector3<int> offset_int[8] =
        {
            math::basic_vector3<int>(0, 0, 0),
            math::basic_vector3<int>(0, 0, 1),
            math::basic_vector3<int>(0, 1, 0),
            math::basic_vector3<int>(0, 1, 1),
            math::basic_vector3<int>(1, 0, 0),
            math::basic_vector3<int>(1, 0, 1),
            math::basic_vector3<int>(1, 1, 0),
            math::basic_vector3<int>(1, 1, 1),
        };

	    math::basic_vector3<int> extent = (max - min) / 2;
        math::basic_vector3<int> p = min + extent * offset_int[t];
        child_min = p;
        child_max = p + extent;
	}

    // this will be done in a separate program later on (SVO's will be
    // loaded on the fly, no time to build them while streaming voxels)
    uint32_t build_SVO(int depth, const aabb &cube, const math::basic_vector3<int> &min, const math::basic_vector3<int> &max)
    {
        if (depth == 0)
        {
            // this is a leaf - add voxel

            uint16_t material;
            get_voxel_data(min, max, material);

            // encode leaf data into 32-bit offset
            return encode_leaf(material);
        }

        Node *node = alloc_node();

        // build children here
        for (int t = 0; t < 8; ++t)
        {
            aabb child = split_node(cube, t);

            math::basic_vector3<int> child_min, child_max;
            split_int(min, max, t, child_min, child_max);

            if (!contains_voxels(child_min, child_max)) node->child[t] = 0;
            else node->child[t] = build_SVO(depth - 1, child, child_min, child_max);
        }

        return (uint32_t)(node - nodes);
    }

    float heightmap(float x, float z) const // TEMPORARY
    {
        return -0.9f + 0.03f * (sin(15 * z) + sin(10 * x + 1));
    }

    math::float3 get_normal(float x, float z) const // TEMPORARY
    {
        float dx = 0.2 * cos(10 * x + 1);
        float dz = 0.3 * cos(15 * z);

        return normalize(math::float3(dx, 1, dz));
    }

    bool contains_voxels(const math::basic_vector3<int> &min, const math::basic_vector3<int> &max) const // TEMPORARY
    {
        for (int x = min.x; x < max.x; ++x)
            for (int y = min.y; y < max.y; ++y)
                for (int z = min.z; z < max.z; ++z)
                    if (data[x][y][z].material != 0xFFFF) return true;

        return false;
    }

    void get_voxel_data(const math::basic_vector3<int> &min, const math::basic_vector3<int> &/*max*/, uint16_t &material) const
        // TEMPORARY
    {
        int x = min.x;
        int y = min.y;
        int z = min.z;

        material = data[x][y][z].material;
    }

    Voxel ***data;

    Voxel*** alloc_ptr()
    {
        Voxel*** ptr = new Voxel**[RESOLUTION];

        for (int x = 0; x < RESOLUTION; ++x)
        {
            ptr[x] = new Voxel*[RESOLUTION];

            for (int y = 0; y < RESOLUTION; ++y)
            {
                ptr[x][y] = new Voxel[RESOLUTION];
            }
        }

        return ptr;
    }

    void free_ptr(Voxel*** ptr)
    {
        for (int x = 0; x < RESOLUTION; ++x)
            for (int y = 0; y < RESOLUTION; ++y)
                delete[] ptr[x][y];

        for (int x = 0; x < RESOLUTION; ++x)
            delete[] ptr[x];

        delete[] ptr;
    }

    void alloc_data()
    {
        data = alloc_ptr();
    }

    void free_mem()
    {
        free_ptr(data);
    }

    void clear_data()
    {
        for (int x = 0; x < RESOLUTION; ++x)
            for (int y = 0; y < RESOLUTION; ++y)
                for (int z = 0; z < RESOLUTION; ++z)
                    data[x][y][z].material = 0xFFFF;
    }

    void filter_data()
    {
        Voxel*** tmp = alloc_ptr();

        for (int x = 1; x < RESOLUTION - 1; ++x)
            for (int y = 1; y < RESOLUTION - 1; ++y)
                for (int z = 1; z < RESOLUTION - 1; ++z)
                {
                    if ((data[x - 1][y][z].material != 0xFFFF)
                     && (data[x + 1][y][z].material != 0xFFFF)
                     && (data[x][y - 1][z].material != 0xFFFF)
                     && (data[x][y + 1][z].material != 0xFFFF)
                     && (data[x][y][z - 1].material != 0xFFFF)
                     && (data[x][y][z + 1].material != 0xFFFF))
                    {
                        tmp[x][y][z].material = 0xFFFF;
                    }
                }

        for (int x = 1; x < RESOLUTION - 1; ++x)
            for (int y = 1; y < RESOLUTION - 1; ++y)
                for (int z = 1; z < RESOLUTION - 1; ++z)
                    data[x][y][z].material = 0xFFFF - tmp[x][y][z].material; // wtf??

        free_ptr(tmp);
    }

    void gen_data()
    {
        for (int x = 0; x < RESOLUTION; ++x)
            for (int y = 0; y < RESOLUTION; ++y)
                for (int z = 0; z < RESOLUTION; ++z)
                {
                    float px = ((float)x / RESOLUTION - 0.5f) * 2;
                    float py = ((float)y / RESOLUTION - 0.5f) * 2;
                    float pz = ((float)z / RESOLUTION - 0.5f) * 2;

                    if (py <= heightmap(px, pz))
                    {
                        data[x][y][z].material = 0;
                    }
                }
    }

    void load_model()
    {
        alloc_data();

        clear_data();

        gen_data();

        //filter_data();
    }

    // pray your libc overcommits :)
    #define NODE_MEMORY (1024 * 1024 * 1024)

    Node *nodes;
    std::size_t node_offset;

    void init_mem()
    {
        nodes = (Node*)malloc(NODE_MEMORY);
        node_offset = 0;
        load_model();
    }

    Node *alloc_node()
    {
        Node *ptr = nodes + node_offset;
        node_offset++;
        return ptr;
    }

    uint32_t root;
};
