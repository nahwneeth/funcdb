#if !defined(FUNCDB_TABLE_HPP)
#define FUNCDB_TABLE_HPP

#include <filesystem>
#include <functional>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include "b-plus-tree.h"
#include "row-info.h"

namespace funcdb {

class Table {
 public:
  Table(std::string filepath);

  Table(Table&&) = delete;
  Table(Table const&) = delete;

 public:
  RowInfo const& GetRowInfo() const;

  bool SetColumns(std::vector<std::pair<DataType, std::string>> const& cols);

  bool Insert(std::vector<std::variant<int32_t, std::string>> const& row);

  void SelectAll();

  bool Select(int32_t key);

  bool Replace(std::vector<std::variant<int32_t, std::string>> const& row);

  bool Remove(int32_t key);

  void Commit();

  void Rollback();

  bool IsCreated();

 private:
  Element GetElement(std::vector<std::variant<int32_t, std::string>> const& row);

 private:
  BPlusTree mTree;
};

}  // namespace funcdb

#endif  // FUNCDB_TABLE_HPP