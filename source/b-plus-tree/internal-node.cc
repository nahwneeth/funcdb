#include "internal-node.hpp"

#include <cstring>

namespace funcdb
{   namespace InternalNode
    {
        // [ getters ] ///////////////////////////////////////////////////

        uint32_t 
        num_keys(NodeObj* self)
        {
            uint32_t val;
            std::memcpy(&val, self->raw_mem + kNumKeysOffset, kNumKeysSize);
            return val;
        }

        uint32_t
        left_child(NodeObj* self)
        {
            uint32_t val;
            std::memcpy(&val, self->raw_mem + kLeftChildOffset, kLeftChildSize);
            return val;
        }

        uint32_t
        key(NodeObj* self, uint32_t i)
        {
            uint32_t val;
            std::memcpy(&val, self->raw_mem + ith_key_offset(i), kKeySize);
            return val;
        }

        uint32_t
        child(NodeObj* self, uint32_t i)
        {
            if (i == 0) return left_child(self);

            uint32_t val;
            std::memcpy(&val, self->raw_mem + ith_child_offset(i), kChildSize);
            return val;
        }

        // [ setters ] ///////////////////////////////////////////////////

        void
        set_num_keys(NodeObj* self, uint32_t val)
        {
            std::memcpy(self->raw_mem + kNumKeysOffset, &val, kNumKeysSize);
        }

        void
        set_left_child(NodeObj* self, uint32_t val)
        {
            std::memcpy(self->raw_mem + kLeftChildOffset, &val, kLeftChildSize);
        }

        void
        set_key(NodeObj* self, uint32_t i, uint32_t val)
        {
            std::memcpy(self->raw_mem + ith_key_offset(i), &val, kKeySize);
        }

        void
        set_child(NodeObj* self, uint32_t i, uint32_t val)
        {
            if (i == 0) return set_left_child(self, val);

            std::memcpy(self->raw_mem + ith_child_offset(i), &val, kChildSize);
        }

        // [ constructors ] /////////////////////////////////////////////////

        NodeObj*
        init(bool is_root, uint32_t parent_index, uint32_t index)
        {
            auto self = Node::init(NodeType::Internal, is_root, parent_index, index);

            /*| `calloc` used in `Node::init`. Setting 0s not required |*/
            // set_num_keys(self, 0);
            // set_left_child(self, 0);

            return self;
        }

    } // namespace InternalNode
} // namespace funcdb
