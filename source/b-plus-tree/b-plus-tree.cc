#include "b-plus-tree.hpp"

#include <cstring>
#include <cstdlib>

#include "leaf-node.hpp"
#include "internal-node.hpp"
#include "node-commons.hpp"

namespace funcdb
{    namespace BPlusTree
    {

        // [ destructor ] /////////////////////////////////////////////////

        void 
        free(BPlusTreeObj* self)
        {
            /*| Close `file` and free all `nodes`. |*/
            std::fclose(self->file);
            for(auto&& node : self->nodes) if (node) Node::free(node);
        }

        // [ helpers ] ///////////////////////////////////////////////////

        NodeObj*
        get_node(BPlusTreeObj* self, uint32_t i)
        {
            if (!self->nodes[i]) {
                /*| Node not loaded. Load from file. |*/
                self->nodes[i] = Node::init_from_file(self->file, i);
            }
            
            /*| Node loaded. Return pointer. |*/
            return self->nodes[i];
        }

        NodeObj*
        leaf_node_for_key(BPlusTreeObj* self, uint32_t key)
        {
            auto node = get_node(self, self->root_index);

            if (node->type == NodeType::Internal) {
                auto node_index = node->index;
                uint32_t child_ind;
                while ((node = get_node(self, node_index))->type != NodeType::Leaf) {
                    binary_search(node, key, &child_ind);
                    node_index = InternalNode::child(node, child_ind);
                }
            }

            return node;
        }

        char*
        value(BPlusTreeObj* self, uint32_t key)
        {
            auto node = leaf_node_for_key(self, key);

            uint32_t pos;
            if (!binary_search(node, key, &pos)) return nullptr;

            return LeafNode::value(node, pos);
        }
        
        void
        set_from_file(BPlusTreeObj* self)
        {
            /*| Initialize all `NodeObj*` in `self->nodes` to nullptr. |*/
            for(auto&& node : self->nodes) node = nullptr;

            /*| To obtain file length, move to the end of file using `fseek` |*/
            /*| and request its location using `ftell`. The location is the  |*/
            /*| length of the file.                                          |*/
            std::fseek(self->file, 0, SEEK_END);
            auto file_length = std::ftell(self->file);

            /*| Initially `n_nodes` and `n_nodes_file` are equal. n_nodes will   |*/
            /*| change when we insert nodes. `n_nodes_file` helps us verify is a |*/
            /*| node exits in file then read or create a new node accordingly.   |*/
            self->n_nodes = self->n_nodes_file = file_length / kNodeSize;

            /*| If a new file was created then `n_nodes` will be 0. |*/
            /*| So, we'll make a new leaf node at index 0 as root.  |*/
            if (self->n_nodes == 0) {
                self->nodes[0] = LeafNode::init(/*is_root*/ true, /*parent_index*/ 0, /*index*/ 0);
                self->n_nodes = 1;
            }

            /*| We'll start with 0th node and traverse up the tree until we reach the root     |*/
            /*| node. Use `get_node()` function to load nodes from file if not already loaded. |*/
            auto root_node = get_node(self, 0);
            while (!root_node->is_root) {
                root_node = get_node(self, root_node->parent_index);
            }

            /*| Initialize root index. |*/
            self->root_index = root_node->index;
        }

        // [ constructors ] /////////////////////////////////////////////////

        BPlusTreeObj*
        init(const char* filename)
        {
            auto self = (BPlusTreeObj*) std::malloc(sizeof(BPlusTreeObj));

            /*| Open file in "r+" mode which allows read, write and append. |*/
            self->file = std::fopen(filename, "r+");

            set_from_file(self);            

            return self;
        }

        // [ other funcs. ] /////////////////////////////////////////////////

        void 
        write(BPlusTreeObj* self)
        {
            /*| If node was loaded or created, write to file. |*/
            for(uint32_t i = 0; i < kMaxNodes; ++i) {
                if (self->nodes[i]){
                    std::fseek(self->file, i * kNodeSize, SEEK_SET);
                    std::fwrite(self->nodes[i]->raw_mem, sizeof(char), kNodeSize, self->file);

                    /*| Writes from iobuf to file. |*/
                    std::fflush(self->file);
                }
            }

            /*| After writing the nodes in file and `n_nodes` are equal. |*/
            self->n_nodes_file = self->n_nodes;
        }

        NodeObj*
        add_node(BPlusTreeObj* self, NodeType type, bool is_root, uint32_t parent_index)
        {
            auto new_node = self->nodes[self->n_nodes] = 
                Node::init(type, is_root, parent_index, self->n_nodes);
            
            /*| Update number of nodes. |*/
            self->n_nodes += 1;

            /*| If the new node is the root node then set the |*/
            /*| current root node's `is_root` field to false. |*/
            if (is_root) {
                Node::set_is_root(get_node(self, self->root_index), false);
                self->root_index = new_node->index;
            }

            return new_node;
        }

        // [ binary search ] /////////////////////////////////////////////////

        bool
        binary_search(NodeObj* node, uint32_t key, uint32_t* pos)
        {
            uint32_t l = 1;
            uint32_t u = Node::num_elems(node);

            uint32_t m;
            while (l <= u) {
                m = (u + l) / 2;

                auto mkey = Node::ith_key(node, m - 1);
                if (mkey == key) {
                    *pos = m - 1;
                    return true;
                } else if (key < mkey) {
                    u = m - 1;
                } else {
                    l = m + 1;
                }
            }

            *pos = l - 1;
            return false;
        }

        // [ insertion ] ////////////////////////////////////////////////////

        bool
        insert_within(NodeObj* node, uint32_t key, const void* vorc)
        {
            uint32_t pos;
            if (binary_search(node, key, &pos)) return false; // key exists

            /*| Make one `cell_size` space for insertion at `pos` |*/
            
            auto n = Node::num_elems(node);
            auto cell_size = Node::cell_size(node);
            auto move_size = (n - pos) * cell_size;
            auto insert_offset = Node::ith_cell_offset(node, pos);
            auto move_offset = insert_offset + cell_size;
            
            std::memmove(node->raw_mem + move_offset, node->raw_mem + insert_offset, move_size);
            
            /*| Set key and value / child |*/
            
            Node::set_ith_key(node, pos, key);
            Node::set_ith_value(node, pos, vorc);
            
            /*| Update number of elements |*/
            Node::set_num_elems(node, n + 1);
            
            return true;
        }

        NodeObj*
        insert_recursive_split(BPlusTreeObj* self, 
                               NodeObj* node, 
                               uint32_t key, 
                               const void* vorc)
        {
            auto n = Node::num_elems(node);
            auto max_cells = Node::max_cells(node);

            if (n < max_cells) {
                if (insert_within(node, key, vorc))
                    return node;
                else return nullptr;
            }

            auto mid = max_cells / 2;
            auto mKey = Node::ith_key(node, mid);
            auto split_ind = (key > mKey) ? (mid) : (mid + 1);

            NodeObj* parent_node;
            if (node->is_root) {
                parent_node = add_node(self, NodeType::Internal, true, 0);
                Node::set_parent_index(node, parent_node->index);
                InternalNode::set_left_child(parent_node, node->index);
            } else {
                parent_node = self->nodes[node->parent_index];
            }

            auto split_key = Node::ith_key(node, split_ind);

            auto another_node = add_node(self, node->type, false, parent_node->index);
            
            if (another_node->type == NodeType::Internal) {
                Node::set_ith_value(another_node, 0, vorc);
            }

            parent_node = insert_recursive_split(self, parent_node, split_key, &another_node->index);

            Node::set_parent_index(another_node, parent_node->index);

            // split key moved to parent and not to another
            uint32_t inc = (node->type == NodeType::Leaf) ? 0 : 1;
            auto src_offset = Node::ith_cell_offset(node, split_ind + inc);
            auto num_cells_moved = max_cells - (split_ind + 1);
            auto move_size = num_cells_moved * Node::cell_size(node);
            auto dest_offset = Node::ith_cell_offset(another_node, 0);

            std::memmove(another_node->raw_mem + dest_offset, node->raw_mem + src_offset, move_size);

            Node::set_num_elems(another_node, num_cells_moved);
            Node::set_num_elems(node, n - num_cells_moved - inc);

            return insert_recursive_split(
                self,
                (key < split_key) ? node : another_node,
                key, vorc);
        }

        bool 
        insert(BPlusTreeObj* self, uint32_t key, const char* value)
        {
            auto node = leaf_node_for_key(self, key);

            if(insert_recursive_split(self, node, key, value)) return true;
            else return false;
        }

        // [ replace ] ////////////////////////////////////////////////////

        bool 
        replace(BPlusTreeObj* self, uint32_t key, const char* value)
        {
            auto node = leaf_node_for_key(self, key);

            uint32_t pos;
            if(!binary_search(node, key, &pos)) return false;

            LeafNode::set_value(node, pos, value);
            return true;
        }

        // [ printer ] /////////////////////////////////////////////////////////

        void 
        print_all_values(BPlusTreeObj* self, std::FILE* file, NodeObj* node)
        {
            if (!node) node = get_node(self, self->root_index);
            for (uint32_t i = 0; i < self->n_nodes; ++i) {
                if (node->type == NodeType::Leaf) {
                    for(uint32_t j = 0; j < LeafNode::num_cells(node); ++j) {
                        auto n = fprintf(file, "%d \t: %.*s\n", 
                            LeafNode::key(node, j), 
                            LeafNode::kValueSize, 
                            LeafNode::value(node, j));
                        n = n + 1;
                    }
                } else {
                    for (uint32_t j = 0; j <= InternalNode::num_keys(node); ++j) {
                        print_all_values(self, file, get_node(self, j));
                    }
                }
            }
        }

        bool
        print_value(BPlusTreeObj* self, std::FILE* file, uint32_t key)
        {
            auto val = value(self, key);

            if (val) {
                fprintf(file, "%d \t: %.*s\n", key, LeafNode::kValueSize, val);
                return true;
            } else return false;
        }

    } // namespace BPlusTree
} // namespace funcdb
