#include "table.h"

#include <cstring>
#include <iostream>

namespace funcdb {

Table::Table(std::string filepath) : mTree(filepath) {
#ifdef LOGS
  std::cerr << "funcdb::gValueSize = " << funcdb::gValueSize << "\n";
  std::cerr << "funcdb::leafNodeMaxElems() = " << funcdb::leafNodeMaxElems()
            << "\n";
#endif
}

std::unique_ptr<char[]> Table::SerializeRow(std::string str) {
  auto buffer = std::make_unique<char[]>(kNodeSize);
  std::strncpy(buffer.get(), str.c_str(), kNodeSize);
  return buffer;
}

bool Table::Insert(uint32_t key, std::string value) {
  Element elem;
  elem.key = key;
  elem.value = SerializeRow(value);
  return mTree.Insert(std::move(elem));
}

bool Table::Replace(uint32_t key, std::string value) {
  Element elem;
  elem.key = key;
  elem.value = SerializeRow(value);
  return mTree.Replace(std::move(elem));
}

void Table::SelectAll() { mTree.Print(std::cout); }

bool Table::Select(uint32_t key) { return mTree.PrintValue(std::cout, key); }

void Table::Commit() { mTree.Commit(); }

void Table::Rollback() { mTree.Rollback(); }

}  // namespace funcdb
