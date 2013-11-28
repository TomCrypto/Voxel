#pragma once

/** @file fast_stack.hpp
  *
  * @brief SVO Traversal Stack
  *
  * Implements a fast stack for SVO traversal. The stack has no bounds checking
  * whatsoever, and relies entirely on the correctness of the calling code.
**/

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "geometry/voxel_types.hpp"
#include "geometry/aabb.hpp"

/** @struct stack_item
  *
  * An item of the traversal stack which holds information relevant to the SVO
  * traversal algorithm (such as the current node offset, its position, ...).
**/
struct stack_item
{
    public:
        /** The offset of the node held by the stack_item.
        **/
        uint32_t offset;

        /** The hit distance of the node to the ray.
        **/
        distance hit;

        /** The bounding box of the node.
        **/
        aabb cube;

        /** Constructs a stack item to be modified later.
          *
          * @remarks The hit distance is defined to be negative infinity, this
          *          is to maintain consistency with the traversal stack.
        **/
        inline stack_item()
        {
            hit = neg_inf();
        }

        /** Constructs a stack item from a node offset and its bounding box.
          *
          * @param offset  The node offset.
          * @param cube    The bounding box.
        **/
        inline stack_item(uint32_t offset, const aabb &cube)
        {
            this->offset = offset;
            this->hit = neg_inf();
            this->cube = cube;
        }

        /** Subdivides the node held by this stack item.
          *
          * @param offset  The offset of the child node.
          * @param index   The subdivision index, from 0 to 7.
          *
          * @return A \c stack_item holding the corresponding child node.
        **/
        inline stack_item subdivide(uint32_t offset, size_t index)
        {
            return stack_item(offset, split_node(cube, index));
        }

    private:
        static inline distance neg_inf()
        {
            return -std::numeric_limits<distance>::infinity();
        }
};

/** @struct traversal_stack
  *
  * The fast stack, for SVO traversal (backed by the hardware stack).
  *
  * @tparam capacity  The maximum capacity of the stack.
**/
template <size_t capacity>
struct traversal_stack
{
    public:
        /** Constructs a new stack and initializes it with a stack item.
          *
          * @remarks This is typically used for pushing the root node.
          *
          * @param offset  Offset for the first stack item.
          * @param cube    Bounding box for the stack item.
        **/
        traversal_stack(const uint32_t &offset, const aabb &cube)
        {
            push(stack_item(offset, cube));
        }

        /** Gets the current position of the last element of the stack.
          *
          * @return The current number of elements in the stack.
        **/
        size_t position()
        {
            return pointer;
        }

        /** Pops the last item off the stack.
          *
          * @return The top of the stack.
        **/
        stack_item pop()
        {
            /** @todo We may have a problem here regarding the value of "hit",
              * investigate whether any bug occurs (it's possible it cannot)
            **/
            assert(pointer != 0);
            return stack[--pointer];
        }

        /** Pushes an item on top of the stack.
          *
          * @param item  The item to push.
        **/
        void push(const stack_item &item)
        {
            assert(pointer != capacity);
            stack[pointer++] = item;
        }

        /** Checks whether the stack is empty.
          *
          * @return \c true if the stack is empty, \c false otherwise.
        **/
        bool empty() const
        {
            return (pointer == 0);
        }

        /** Performs an algorithm-specific sort, sorting four stack items into
          * descending order (in terms of their hit distance).
          *
          * @remarks This is used for preemptively sorting the traversal stack
          *          in order to heuristically reach closer voxels first. They
          *          are sorted in descending order so the stack item with the
          *          lowest hit distance is popped off the stack first.
          *
          * @remarks Because there are only four items (at most) to sort, this
          *          uses an optimal sorting network to achieve optimal speed.
          *
          * @remarks It is assumed empty stack items have a hit distance equal
          *          to negative infinity for the function to be consistent.
          *
          * @param last  Index of the first element to sort.
          *
          * @remarks After this function returns, stack elements from \c last,
          *          to \c last \c + \c 3, will be sorted in descending order.
        **/
        void special_sort(size_t last)
        {
            assert(last + 4 <= capacity);
            auto *start = stack + last;
            sort(start[0], start[2]);
            sort(start[1], start[3]);
            sort(start[0], start[1]);
            sort(start[2], start[3]);
            sort(start[1], start[2]);
        }

    private:
        stack_item stack[capacity];
        size_t pointer = 0;

        static inline void sort(stack_item &a, stack_item &b)
        {
            if (a.hit < b.hit)
            {
                auto tmp = a;
                a = b;
                b = tmp;
            }
        }
};
