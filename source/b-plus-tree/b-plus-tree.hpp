#if !defined(FUNCDB_B_PLUS_TREE_HPP)
#define FUNCDB_B_PLUS_TREE_HPP

#include "node.hpp"

namespace funcdb
{

    uint32_t constexpr kMaxNodes = 1000;
    
    struct BPlusTreeObj
    {
        std::FILE*  file;
        uint32_t    n_nodes;
        uint32_t    n_nodes_file;
        NodeObj*    nodes[kMaxNodes];

        uint32_t    root_index;
    };

    namespace BPlusTree
    {    
        // [ destructor ] /////////////////////////////////////////////////

        void 
        free(BPlusTreeObj* self);

        // [ helpers ] ///////////////////////////////////////////////////

        /*| The validity of index `i` is not checked.       |*/
        /*| Possible checking to be done before calling:    |*/
        /*| - 1 - [ i < self->n_nodes ]                     |*/
        /*| - 2 - [ i < self->n_nodes_file ] if not loaded. |*/
        /*| - 3 - [ i < kMaxNodes ] ie. BPlusTree is full.  |*/
        /*| However, checking can be avoided most of the    |*/
        /*| time as we deal with nodes that already exist.  |*/
        /*| Checking is only required during insertion.     |*/
        NodeObj*
        get_node(BPlusTreeObj* self, uint32_t i);

        NodeObj*
        leaf_node_with_key(BPlusTreeObj* self, uint32_t key);

        char*
        value(BPlusTreeObj* self, uint32_t key);

        /*| Discards all changes and reads nodes from file. RollBack. |*/
        void
        set_from_file(BPlusTreeObj* self);
        
        // [ constructors ] /////////////////////////////////////////////////

        /*| The file should exist and read and write permission are required. |*/
        /*| No checking done if the file exits or if the premissions are met. |*/
        BPlusTreeObj*
        init(const char* filename);

        // [ other funcs. ] /////////////////////////////////////////////////

        void 
        write(BPlusTreeObj* self);

        /*| Adds a new node to the end of `nodes` and returns a pointer. |*/
        /*| `nullptr` is returned if `kMaxNodes` nodes already exist.    |*/
        /*| Other than the first node, all other nodes should be created |*/
        /*| by a call to this function.                                  |*/
        NodeObj*
        add_node(BPlusTreeObj* self, NodeType type, bool is_root, uint32_t parent_index);

        // [ binary search ] /////////////////////////////////////////////////

        /*| `binary_search` returns whether a `key` exists or not.   |*/
        /*| If `key` exists then `pos` is the position of the `key`, |*/
        /*| otherwise `pos` is the position where the new key should |*/
        /*| should be inserted. No checking if `pos` is `nullptr`.   |*/
        /*| If `key` exist then `pos` = {0, n - 1}                   |*/
        /*| Otherwise `pos` = {0, n}                                 |*/
        /*| n = {0, kMaxCells}                                       |*/
        bool
        binary_search(NodeObj* node, uint32_t key, uint32_t* pos);

        // [ insertion ] ////////////////////////////////////////////////////

        /*| Check if there is space to insert before calling. |*/
        /*| [num_cells or num_keys] < kMaxCells               |*/
        /*| vorc = value or child                             |*/
        bool
        insert_within(NodeObj* node, uint32_t key, const void* vorc);

        NodeObj*
        insert_recursive_split(BPlusTreeObj* self, 
                               NodeObj* node, 
                               uint32_t key, 
                               const void* vorc);

        bool 
        insert(BPlusTreeObj* self, uint32_t key, const char* value);

        // [ replace ] ////////////////////////////////////////////////////

        bool 
        replace(BPlusTreeObj* self, uint32_t key, const char* value);

        // [ printer ] ////////////////////////////////////////////////////

        void 
        print_all_values(BPlusTreeObj* self, std::FILE* file, NodeObj* node = nullptr);

        bool
        print_value(BPlusTreeObj* self, std::FILE* file, uint32_t key);

    } // namespace BPlusTree
} // namespace funcdb
#endif // FUNCDB_B_PLUS_TREE_HPP