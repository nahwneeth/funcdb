#if !defined(FUNCDB_ROW_INFO_H)
#define FUNCDB_ROW_INFO_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "node.h"

namespace funcdb {

enum class DataType : char { i32, c100 };

std::size_t DTSize(DataType dt);

extern std::string const k_i32_Str;
extern std::string const k_c100_Str;

struct Column {
  DataType type;
  std::string columnName;
  std::size_t offset;
};

class RowInfo {
 public:
  RowInfo() = default;

  RowInfo(char (&rawMem)[kNodeSize]);

  RowInfo(std::vector<std::pair<DataType, std::string>> const& cols);

 public:
  std::size_t Size() const;

  std::size_t ValueSize() const;

  std::unique_ptr<char[]> Serialize() const;

  void SetOffsetForColumns();

 public:
  std::vector<Column> mColumns;
};

}  // namespace funcdb

#endif  // FUNCDB_ROW_INFO_H
