#if !defined(FUNCDB_INTERNAL_NODE_HPP)
#define FUNCDB_INTERNAL_NODE_HPP

#include "node.hpp"

namespace funcdb
{   namespace InternalNode
    {
        ///////////////////////////////////////////////////////////////////////
        /*| An `InternalNode` is a `Node` with (type = NodeType::Internal). |*/
        /*| Functions defined here extract from or write to raw_mem.        |*/
        /*| No validation is done whether the `NodeObj*` is a Internal      |*/
        /*| node or nullptr. Checking should be done before calling.        |*/
        ///////////////////////////////////////////////////////////////////////

        // [ layout ] ////////////////////////////////////////////////////

        uint32_t constexpr kLeftChildOffset   = Node::kCommonHeaderSize;
        uint32_t constexpr kLeftChildSize     = sizeof(uint32_t);

        uint32_t constexpr kNumKeysOffset     = kLeftChildOffset + kLeftChildSize;
        uint32_t constexpr kNumKeysSize       = sizeof(uint32_t);

        uint32_t constexpr kHeaderSize        = kNumKeysOffset + kNumKeysSize;

        uint32_t constexpr kKeyOffsetInCell   = 0;
        uint32_t constexpr kKeySize           = sizeof(uint32_t);

        uint32_t constexpr kChildOffsetInCell = kKeyOffsetInCell + kKeySize;
        uint32_t constexpr kChildSize         = sizeof(uint32_t);

        uint32_t constexpr kCellSize          = kKeySize + kChildSize;
        uint32_t constexpr kCellSpace         = kNodeSize - kHeaderSize;
        uint32_t constexpr kMaxCells          = kCellSpace / kCellSize;

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
        ith_child_offset(uint32_t i)
        {
            return ith_cell_offset(i) + kChildOffsetInCell;
        }

        // [ getters ] ///////////////////////////////////////////////////

        uint32_t 
        num_keys(NodeObj* self);

        uint32_t
        left_child(NodeObj* self);

        uint32_t
        key(NodeObj* self, uint32_t i);

        uint32_t
        child(NodeObj* self, uint32_t i);

        // [ setters ] ///////////////////////////////////////////////////

        void
        set_num_keys(NodeObj* self, uint32_t val);

        void
        set_left_child(NodeObj* self, uint32_t val);

        void
        set_key(NodeObj* self, uint32_t i, uint32_t val);

        void
        set_child(NodeObj* self, uint32_t i, uint32_t val);

        // [ constructors ] /////////////////////////////////////////////////

        NodeObj*
        init(bool is_root, uint32_t parent_index, uint32_t index);

    } // namespace InternalNode
} // namespace funcdb
#endif // FUNCDB_INTERNAL_NODE_HPP
