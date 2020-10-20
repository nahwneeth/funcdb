#include "internal-node.h"

#include <cstring>
#include <iostream>

namespace funcdb {

InternalNode::InternalNode(std::size_t index, std::size_t parentIndex,
                           std::size_t firstChildIndex)
    : Node(index, parentIndex) {
  mChildren.push_back(firstChildIndex);
}

InternalNode::InternalNode(std::size_t index, std::fstream& fstream,
                           std::size_t beginOffset)
    : Node(index, 0) {
  fstream.seekg(index * kNodeSize + beginOffset, std::ios::beg);
  fstream.read((char*)&mParentIndex, sizeof(mParentIndex));

  std::size_t numKeys;
  fstream.read((char*)&numKeys, sizeof(numKeys));

  mKeys.reserve(numKeys);
  mChildren.reserve(numKeys + 1);

  for (std::size_t i = 0; i < numKeys; ++i) {
    int32_t key;
    fstream.read((char*)&key, sizeof(key));
    mKeys.push_back(key);
  }

  for (std::size_t i = 0; i < numKeys + 1; ++i) {
    std::size_t child;
    fstream.read((char*)&child, sizeof(child));
    mChildren.push_back(child);
  }
}

std::unique_ptr<char[]> InternalNode::Serialize() const {
  auto buffer = std::make_unique<char[]>(kNodeSize);

  std::size_t offset = 0;

  std::memcpy(buffer.get() + offset, &mType, sizeof(mType));
  offset += sizeof(mType);

  std::memcpy(buffer.get() + offset, &mParentIndex, sizeof(mParentIndex));
  offset += sizeof(mParentIndex);

  std::size_t numKeys = mKeys.size();

  std::memcpy(buffer.get() + offset, &numKeys, sizeof(numKeys));
  offset += sizeof(numKeys);

  std::size_t keySize = sizeof(int32_t);
  std::size_t childSize = sizeof(std::size_t);

  for (std::size_t i = 0; i < numKeys; ++i) {
    std::memcpy(buffer.get() + offset, &mKeys[i], keySize);
    offset += keySize;
  }

  for (std::size_t i = 0; i < numKeys + 1; ++i) {
    std::memcpy(buffer.get() + offset, &mChildren[i], childSize);
    offset += childSize;
  }

  return buffer;
}

void InternalNode::Insert(std::size_t index, std::size_t key,
                          std::size_t rightChild) {
  mKeys.insert(std::next(mKeys.begin(), index), key);
  mChildren.insert(std::next(mChildren.begin(), index + 1), rightChild);
}

InternalNode::SearchResult InternalNode::Search(int32_t key) const {
  SearchResult res;

  std::size_t l = 1;
  std::size_t u = mKeys.size();

  std::size_t m;

  while (l <= u) {
    m = (u + l) / 2;

    if (mKeys[m - 1] == key) {
      res.exists = true;
      res.index = m - 1;
      return res;
    } else if (mKeys[m - 1] < key)
      l = m + 1;
    else
      u = m - 1;
  }

  res.exists = false;
  res.index = l - 1;

  return res;
}

}  // namespace funcdb
