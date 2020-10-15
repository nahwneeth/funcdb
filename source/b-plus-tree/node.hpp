#if !defined(FUNCDB_NODE_HPP)
#define FUNCDB_NODE_HPP

#include <cstdint>
#include <cstdio>

namespace funcdb
{

    uint32_t constexpr kNodeSize = 4096;

    enum class NodeType : char
    {
        Leaf,
        Internal
    };
    
    struct NodeObj
    {
        NodeType    type;
        bool        is_root;
        uint32_t    parent_index;

        char        raw_mem[kNodeSize];

        uint32_t    index;
    };

    namespace Node
    {
        // [ layout ] ////////////////////////////////////////////////////

        uint32_t constexpr kTypeOffset        = 0;
        uint32_t constexpr kTypeSize          = sizeof(char);

        uint32_t constexpr kIsRootOffset      = kTypeOffset + kTypeSize;
        uint32_t constexpr kIsRootSize        = sizeof(bool);

        uint32_t constexpr kParentIndexOffset = kIsRootOffset + kIsRootSize;
        uint32_t constexpr kParentIndexSize   = sizeof(uint32_t);

        uint32_t constexpr kCommonHeaderSize  = kParentIndexOffset + kParentIndexSize;

        uint32_t constexpr kNodeData          = kNodeSize - kCommonHeaderSize;
        
        // [ destructor ] /////////////////////////////////////////////////

        void 
        free(NodeObj* self);

        // [ setters ] ////////////////////////////////////////////////////
        // Change the variable and modify the raw memory as well.        //
        ///////////////////////////////////////////////////////////////////

        void
        set_type(NodeObj* self, NodeType type);

        void 
        set_is_root(NodeObj* self, bool is_root);

        void 
        set_parent_index(NodeObj* self, uint32_t parent_index);

        // [ constructors ] /////////////////////////////////////////////////

        NodeObj*
        init(NodeType type, bool is_root, uint32_t parent_index, uint32_t index);

        NodeObj* 
        init_from_file(std::FILE* file, uint32_t index);

    } // namespace Node
} // namespace funcdb
#endif // FUNCDB_NODE_HPP