#if !defined(FUNCDB_NODE_COMMONS_HPP)
#define FUNCDB_NODE_COMMONS_HPP

#include "node.hpp"

namespace funcdb
{    namespace Node
    {
        // [ getters ] ////////////////////////////////////////////////////
        
        uint32_t ith_cell_offset(NodeObj* self, uint32_t i);

        uint32_t cell_size(NodeObj* self);

        uint32_t ith_key(NodeObj* self, uint32_t i);

        uint32_t num_elems(NodeObj* self);

        uint32_t max_cells(NodeObj* self);

        // [ setters ] ////////////////////////////////////////////////////

        void set_ith_key(NodeObj* self, uint32_t i, uint32_t val);

        void set_num_elems(NodeObj* self, uint32_t val);

        void set_ith_value(NodeObj* self, uint32_t i, const void* vorc);

    } // namespace Node
    
} // namespace funcdb

#endif // FUNCDB_NODE_COMMONS_HPP
