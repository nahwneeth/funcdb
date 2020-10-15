#if !defined(FUNCDB_LEAF_NODE_HPP)
#define FUNCDB_LEAF_NODE_HPP

#include "node.hpp"

namespace funcdb
{    namespace LeafNode
    {   
        ////////////////////////////////////////////////////////////////
        /*| A `LeafNode` is a `Node` with (type = NodeType::Leaf).   |*/
        /*| Functions defined here extract from or write to raw_mem. |*/
        /*| No validation is done whether the `NodeObj*` is a Leaf   |*/
        /*| node or nullptr. Checking should be done before calling. |*/
        ////////////////////////////////////////////////////////////////

        // [ layout ] ////////////////////////////////////////////////////

        uint32_t constexpr kNumCellsOffset    = Node::kCommonHeaderSize;
        uint32_t constexpr kNumCellsSize      = sizeof(uint32_t);

        uint32_t constexpr kHeaderSize        = kNumCellsOffset + kNumCellsSize;

        uint32_t constexpr kKeyOffsetInCell   = 0;
        uint32_t constexpr kKeySize           = sizeof(uint32_t);

        uint32_t constexpr kValueOffsetInCell = kKeyOffsetInCell + kKeySize;
        uint32_t constexpr kValueSize         = 255;

        uint32_t constexpr kCellSize          = kValueOffsetInCell + kValueSize;
        uint32_t constexpr kCellSpace         = kNodeSize - kHeaderSize;
        uint32_t constexpr kMaxCells           = kCellSpace / kCellSize;

        // [ helpers ] ///////////////////////////////////////////////////

        uint32_t constexpr 
        ith_cell_offset(uint32_t i)
        {
            return kHeaderSize + (i * kCellSize);
        }

        uint32_t constexpr
        ith_key_offset(uint32_t i)
        {
            return ith_cell_offset(i) + kKeyOffsetInCell;
        }

        uint32_t constexpr
        ith_value_offset(uint32_t i)
        {
            return ith_cell_offset(i) + kValueOffsetInCell;
        }

        // [ getters ] ///////////////////////////////////////////////////

        uint32_t 
        num_cells(NodeObj* self);

        uint32_t
        key(NodeObj* self, uint32_t i);

        char*
        value(NodeObj* self, uint32_t i);

        // [ setters ] ///////////////////////////////////////////////////

        void 
        set_num_cells(NodeObj* self, uint32_t val);

        void
        set_key(NodeObj* self, uint32_t i, uint32_t val);

        void
        set_value(NodeObj* self, uint32_t i, const char* val);

        // [ constructors ] /////////////////////////////////////////////////

        NodeObj*
        init(bool is_root, uint32_t parent_index, uint32_t index);

    } // namespace LeafNode
} // namespace funcdb
#endif // FUNCDB_LEAF_NODE_HPP
