#include "node-commons.hpp"

#include <cstring>

#include "leaf-node.hpp"
#include "internal-node.hpp"

namespace funcdb
{   namespace Node
    {
        // [ getters ] ////////////////////////////////////////////////////
        
        uint32_t ith_cell_offset(NodeObj* self, uint32_t i)
        {
            if (self->type == NodeType::Leaf) 
                return LeafNode::ith_key_offset(i);
            else return InternalNode::ith_key_offset(i);
        }

        uint32_t cell_size(NodeObj* self)
        {
            if (self->type == NodeType::Leaf)
                return LeafNode::kCellSize;
            else return InternalNode::kCellSize;
        }

        uint32_t ith_key(NodeObj* self, uint32_t i)
        {
            if (self->type == NodeType::Leaf)
                return LeafNode::key(self, i);
            else return InternalNode::key(self, i);
        }

        uint32_t num_elems(NodeObj* self)
        {
            if (self->type == NodeType::Leaf)
                return LeafNode::num_cells(self);
            else return InternalNode::num_keys(self);
        }

        uint32_t max_cells(NodeObj* self)
        {
            if (self->type == NodeType::Leaf)
                return LeafNode::kMaxCells;
            else return InternalNode::kMaxCells;
        }

        // [ setters ] ////////////////////////////////////////////////////

        void set_ith_key(NodeObj* self, uint32_t i, uint32_t val)
        {
            if (self->type == NodeType::Leaf)
                LeafNode::set_key(self, i, val);
            else InternalNode::set_key(self, i, val);
        }

        void set_num_elems(NodeObj* self, uint32_t val)
        {
            if (self->type == NodeType::Leaf)
                LeafNode::set_num_cells(self, val);
            else InternalNode::set_num_keys(self, val);
        }

        void set_ith_value(NodeObj* self, uint32_t i, const void* vorc)
        {
            if (self->type == NodeType::Leaf)
                LeafNode::set_value(self, i, (char*) vorc);
            else {
                uint32_t child;
                std::memmove(&child, vorc, InternalNode::kChildSize);
                InternalNode::set_child(self, i, child);
            }
        }

    } // namespace Node
} // namespace funcdb