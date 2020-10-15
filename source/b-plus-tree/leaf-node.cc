#include "leaf-node.hpp"

#include <cstring>

namespace funcdb
{   namespace LeafNode
    {
        // [ getters ] ///////////////////////////////////////////////////

        uint32_t 
        num_cells(NodeObj* self)
        {
            uint32_t val;
            std::memcpy(&val, self->raw_mem + kNumCellsOffset, kNumCellsSize);
            return val;
        }

        uint32_t
        key(NodeObj* self, uint32_t i)
        {
            uint32_t val;
            std::memcpy(&val, self->raw_mem + ith_key_offset(i), kKeySize);
            return val;
        }

        char*
        value(NodeObj* self, uint32_t i)
        {
            return self->raw_mem + ith_value_offset(i);
        }

        // [ setters ] ///////////////////////////////////////////////////

        void 
        set_num_cells(NodeObj* self, uint32_t val)
        {
            std::memcpy(self->raw_mem + kNumCellsOffset, &val, kNumCellsSize);
        }

        void
        set_key(NodeObj* self, uint32_t i, uint32_t val)
        {
            std::memcpy(self->raw_mem + ith_key_offset(i), &val, kKeySize);
        }

        void
        set_value(NodeObj* self, uint32_t i, const char* val)
        {
            std::strncpy(self->raw_mem + ith_value_offset(i), val, kValueSize);
        }

        // [ constructors ] /////////////////////////////////////////////////

        NodeObj*
        init(bool is_root, uint32_t parent_index, uint32_t index)
        {
            auto self = Node::init(NodeType::Leaf, is_root, parent_index, index);

            /*| `calloc` used in `Node::init`. Setting 0s not required. |*/
            // set_num_cells(self, 0);
            
            return self;
        }

    } // namespace LeafNode
} // namespace funcdb