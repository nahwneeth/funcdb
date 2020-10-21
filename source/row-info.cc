#include "row-info.h"

#include <cassert>
#include <cstring>

namespace funcdb {

std::string const k_i32_Str = "i32";
std::string const k_c100_Str = "c100";

std::size_t DTSize(DataType dt) {
  switch (dt) {
    case DataType::i32:
      return sizeof(int32_t);
    case DataType::c100:
      return 100;
  }
  assert(false);
  return 0;
}

RowInfo::RowInfo(char (&rawMem)[kNodeSize]) {
  auto colNameSize = DTSize(DataType::c100);

  std::size_t offset = 0;
  std::size_t numCols;

  std::memcpy(&numCols, rawMem + offset, sizeof(std::size_t));
  offset += sizeof(std::size_t);

  mColumns.reserve(numCols);

  for (std::size_t i = 0; i < numCols; ++i) {
    auto col = Column();

    std::memcpy(&col.type, rawMem + offset, sizeof(DataType));
    offset += sizeof(DataType);

    col.columnName = std::string(rawMem + offset, colNameSize);
    offset += colNameSize;

    mColumns.push_back(std::move(col));
  }

  SetOffsetForColumns();
}

RowInfo::RowInfo(std::vector<std::pair<DataType, std::string>> const& cols) {
  for (auto&& col : cols) {
    auto column = Column();
    column.type = col.first;
    column.columnName = col.second;
    mColumns.push_back(column);
  }
  SetOffsetForColumns();
}

std::unique_ptr<char[]> RowInfo::Serialize() const {
  auto buff = std::make_unique<char[]>(kNodeSize);
  std::size_t colNameSize = DTSize(DataType::c100);

  std::size_t offset = 0;

  std::size_t numCols = mColumns.size();
  std::memcpy(buff.get() + offset, &numCols, sizeof(std::size_t));
  offset += sizeof(std::size_t);

  for (auto&& col : mColumns) {
    std::memcpy(buff.get() + offset, &col.type, sizeof(DataType));
    offset += sizeof(DataType);

    std::strncpy(buff.get() + offset, col.columnName.c_str(), colNameSize);
    offset += colNameSize;
  }

  return buff;
}

std::size_t RowInfo::Size() const {
  std::size_t size = 0;
  for (auto&& column : mColumns) {
    size += DTSize(column.type);
  }
  return size;
}

std::size_t RowInfo::ValueSize() const {
  if (mColumns.empty()) return 0;
  return Size() - DTSize(mColumns[0].type);
}

void RowInfo::SetOffsetForColumns() {
  std::size_t valueOffset = 0;
  for (auto&& col : mColumns) {
    col.offset = valueOffset;
    valueOffset += DTSize(col.type);
  }
}

}  // namespace funcdb
