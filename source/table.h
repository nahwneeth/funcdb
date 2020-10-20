#if !defined(FUNCDB_TABLE_HPP)
#define FUNCDB_TABLE_HPP

#include <filesystem>
#include <memory>
#include <variant>
#include <vector>

#include "b-plus-tree.h"

namespace funcdb {

class Table {
 public:
  Table(std::string filepath);

  Table(Table&&) = delete;
  Table(Table const&) = delete;

 public:
  bool Insert(uint32_t key, std::string value);

  void SelectAll();

  bool Select(uint32_t key);

  bool Replace(uint32_t key, std::string value);

  void Commit();

  void Rollback();

 private:
  std::unique_ptr<char[]> SerializeRow(std::string str);

 private:
  BPlusTree mTree;
};

}  // namespace funcdb

#endif  // FUNCDB_TABLE_HPP