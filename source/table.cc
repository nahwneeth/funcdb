#include "table.hpp"

#include <iostream>

#include "b-plus-tree/leaf-node.hpp"

namespace funcdb
{
    namespace stdfs = std::filesystem;

    std::variant<Table, Table::InitError>
    Table::init(const char* filepath)
    {
        auto path = stdfs::path{filepath};

        if (stdfs::exists(path)) {
            if (!stdfs::is_regular_file(path)) {
                return InitError::NotRegularFile;
            }

            auto permissions = stdfs::status(path).permissions();

            if ((permissions & stdfs::perms::owner_read) == stdfs::perms::none) {
                return InitError::NoReadPermission;
            } else if ((permissions & stdfs::perms::owner_write) == stdfs::perms::none) {
                return InitError::NoWritePermission;
            }
        } else {
            /*| Open and close in "a+" mode to create file if doesn't exist. |*/
            auto file = std::fopen(filepath, "a+");
            auto new_file_created = file != NULL;
            std::fclose(file);

            if (!new_file_created) {
                return InitError::FailedToCreateNewFile;
            }
        }

        return std::variant<Table, Table::InitError>(std::in_place_type<Table>, filepath);
    }
    
    Table::Table(const char* filepath)
    {
        tree = BPlusTree::init(filepath);
    }

    Table::~Table()
    {
        BPlusTree::free(tree);
    }

    bool
    Table::insert(uint32_t key, std::string value)
    {
        char cptr[LeafNode::kValueSize]{};
        value.copy(cptr, sizeof(cptr));
        return BPlusTree::insert(tree, key, cptr);
    }

    bool
    Table::replace(uint32_t key, std::string value)
    {
        char cptr[LeafNode::kValueSize]{};
        value.copy(cptr, sizeof(cptr));
        return BPlusTree::replace(tree, key, cptr);
    }

    void
    Table::select_all()
    {
        BPlusTree::print_all_values(tree, stdout);
    }

    bool 
    Table::select(uint32_t key)
    {
        return BPlusTree::print_value(tree, stdout, key);
    }

    void
    Table::commit()
    {
        BPlusTree::write(tree);
    }

    void 
    Table::rollback()
    {
        BPlusTree::set_from_file(tree);
    }

} // namespace funcdb
