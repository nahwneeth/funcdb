#if !defined(FUNCDB_TABLE_HPP)
#define FUNCDB_TABLE_HPP

#include <filesystem>
#include <memory>
#include <variant>
#include <vector>

#include "b-plus-tree/b-plus-tree.hpp"

namespace funcdb
{

    class Table
    {

    public:

        enum class InitError
        {
            FailedToCreateNewFile,
            NoReadPermission,
            NotRegularFile,
            NoWritePermission
        };

        static 
        std::variant<Table, InitError>
        init(const char* filepath);

        Table(const char* path);
        
        ~Table();

        Table(Table&&) = delete;

        Table& operator=(Table&&) = delete;

        Table(Table const&) = delete;

        Table& operator=(Table const&) = delete;

    public:

        bool insert(uint32_t key, std::string value);

        void select_all();

        bool select(uint32_t key);

        bool replace(uint32_t key, std::string value);

        void commit();

        void rollback();

    private:
        BPlusTreeObj* tree;
    };

} // namespace funcdb

#endif // FUNCDB_TABLE_HPP