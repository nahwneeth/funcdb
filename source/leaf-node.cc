#include "leaf-node.h"

#include <cstring>
#include <iostream>

namespace funcdb {

std::size_t leafNodeMaxElems() {
  return ((kNodeSize - kCommonHeaderSize) - /*numElems*/ sizeof(std::size_t)) /
         (/*key*/ sizeof(std::size_t) + gValueSize);
}

LeafNode::LeafNode(std::size_t index, std::size_t parentIndex)
    : Node(index, parentIndex) {}

LeafNode::LeafNode(std::size_t index, std::fstream& fstream,
                   std::size_t beginOffset)
    : Node(index, 0) {
  fstream.seekg(index * kNodeSize + beginOffset, std::ios::beg);
  fstream.read((char*)&mParentIndex, sizeof(mParentIndex));

  std::size_t numElems;
  fstream.read((char*)&numElems, sizeof(numElems));

  mElems.reserve(numElems);

  for (std::size_t i = 0; i < numElems; ++i) {
    Element elem;
    fstream.read((char*)&elem.key, sizeof(elem.key));

    elem.value = std::make_unique<char[]>(gValueSize);
    fstream.read(elem.value.get(), gValueSize);

    mElems.push_back(std::move(elem));
  }
}

std::unique_ptr<char[]> LeafNode::Serialize() const {
  auto buffer = std::make_unique<char[]>(kNodeSize);

  std::size_t offset = 0;

  std::memcpy(buffer.get() + offset, &mType, sizeof(mType));
  offset += sizeof(mType);

  std::memcpy(buffer.get() + offset, &mParentIndex, sizeof(mParentIndex));
  offset += sizeof(mParentIndex);

  std::size_t numElems = mElems.size();
  std::memcpy(buffer.get() + offset, &numElems, sizeof(numElems));
  offset += sizeof(numElems);

  std::size_t keySize = sizeof(decltype(Element::key));

  for (std::size_t i = 0; i < numElems; ++i) {
    std::memcpy(buffer.get() + offset, &mElems[i].key, keySize);
    offset += keySize;

    std::memcpy(buffer.get() + offset, mElems[i].value.get(), gValueSize);
    offset += gValueSize;
  }

  return buffer;
}

void LeafNode::Insert(std::size_t index, Element& elem) {
  mElems.insert(std::next(mElems.begin(), index), std::move(elem));
}

LeafNode::SearchResult LeafNode::Search(int32_t key) const {
  SearchResult res;

  std::size_t l = 1;
  std::size_t u = mElems.size();

  std::size_t m;

  while (l <= u) {
    m = (u + l) / 2;

    if (mElems[m - 1].key == key) {
      res.exists = true;
      res.index = m - 1;
      return res;
    } else if (mElems[m - 1].key < key)
      l = m + 1;
    else
      u = m - 1;
  }

  res.exists = false;
  res.index = l - 1;

  return res;
}

}  // namespace funcdb
