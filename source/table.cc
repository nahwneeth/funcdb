#include "table.h"

#include <cstring>
#include <iostream>

namespace funcdb {

Table::Table(std::string filepath)
    : mTree(filepath) {}

bool Table::IsCreated() { return !mTree.mRowInfo.mColumns.empty(); }

bool Table::SetColumns(
    std::vector<std::pair<DataType, std::string>> const& cols) {
  auto rowInfo = RowInfo(cols);
  return mTree.SetRowInfo(rowInfo);
}

RowInfo const& Table::GetRowInfo() const { return mTree.mRowInfo; }

Record Table::GetElement(
    std::vector<std::variant<int32_t, std::string>> const& row) {
  auto elem = Record();
  elem.key = std::get<int32_t>(row[0]);
  elem.value = std::make_unique<char[]>(gValueSize);

  auto const i32Size = DTSize(DataType::i32);
  auto const c100Size = DTSize(DataType::c100);

  auto&& cols = mTree.mRowInfo.mColumns;
  auto const keyOffset = DTSize(cols[0].type);

  for (std::size_t i = 1; i < row.size(); ++i) {
#ifdef LOGS
    std::cerr << "Table::GetElement()\n";
    std::cerr << "\t i = " << i << "\t";
    std::cerr << "\t dest-offset = " << cols[i].offset - keyOffset << "\n";
#endif

    if (cols[i].type == DataType::i32) {
      std::memcpy(elem.value.get() + cols[i].offset - keyOffset,
                  &std::get<int32_t>(row[i]), i32Size);
    } else {
      std::strncpy(elem.value.get() + cols[i].offset - keyOffset,
                  std::get<std::string>(row[i]).c_str(), c100Size);
    }
  }

  return elem;
}

bool Table::Insert(std::vector<std::variant<int32_t, std::string>> const& row) {
  return mTree.Insert(GetElement(row));
}

bool Table::Replace(
    std::vector<std::variant<int32_t, std::string>> const& row) {
  return mTree.Replace(GetElement(row));
}

bool Table::Remove(int32_t key) { return mTree.Remove(key); }

void Table::SelectAll() { mTree.Print(std::cout); }

bool Table::Select(int32_t key) {
  return mTree.PrintValue(std::cout, key);
}

void Table::Commit() { mTree.Commit(); }

void Table::Rollback() { mTree.Rollback(); }

}  // namespace funcdb
