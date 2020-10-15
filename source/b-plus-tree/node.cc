#include "node.hpp"

#include <cstdlib>
#include <cstring>

namespace funcdb
{   namespace Node
    {        
        // [ destructor ] /////////////////////////////////////////////////

        void 
        free(NodeObj* self)
        {
            std::free(self);
        }

        // [ setters ] ////////////////////////////////////////////////////
        // Change the variable and modify the raw memory as well.        //
        ///////////////////////////////////////////////////////////////////

        void
        set_type(NodeObj* self, NodeType type)
        {
            self->type = type;
            std::memcpy(self->raw_mem + kTypeOffset, &self->type, kTypeSize);
        }

        void 
        set_is_root(NodeObj* self, bool is_root)
        {
            self->is_root = is_root;
            std::memcpy(self->raw_mem + kIsRootOffset, &self->is_root, kIsRootSize);
        }

        void 
        set_parent_index(NodeObj* self, uint32_t parent_index)
        {
            self->parent_index = parent_index;
            std::memcpy(self->raw_mem + kParentIndexOffset, &self->parent_index, kParentIndexSize);
        }

        // [ constructors ] /////////////////////////////////////////////////

        NodeObj*
        init(NodeType type, bool is_root, uint32_t parent_index, uint32_t index)
        {
            auto self = (NodeObj*) std::calloc(1, sizeof(NodeObj));
            set_type(self, type);
            set_is_root(self, is_root);
            set_parent_index(self, parent_index);
            self->index = index;
            return self;
        }

        NodeObj* 
        init_from_file(std::FILE* file, uint32_t index)
        {
            auto self = (NodeObj*) std::malloc(sizeof(NodeObj));

            /*| Read object from file. |*/
            std::fseek(file, index * kNodeSize, SEEK_SET);
            auto charsread = std::fread(self->raw_mem, sizeof(char), kNodeSize, file);

            if (charsread != kNodeSize) {
                fprintf(stderr, "Error Reading data from file.\n");
            }

            /*| Set header variables from read data. |*/
            std::memcpy(&self->type,          self->raw_mem + kTypeOffset,         kTypeSize);
            std::memcpy(&self->is_root,       self->raw_mem + kIsRootOffset,       kIsRootSize);
            std::memcpy(&self->parent_index,  self->raw_mem + kParentIndexOffset,  kParentIndexSize);

            /*| `index` is the position this node should be read or written in file. |*/
            /*| `index` once set for a node, never changes.                          |*/
            self->index = index;
            return self;
        }

    } // namespace Node
} // namespace funcdb
