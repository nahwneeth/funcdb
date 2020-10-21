#include "b-plus-tree.h"

#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>

namespace funcdb {

std::size_t gValueSize = 0;

BPlusTree::BPlusTree(std::filesystem::path path)
    : mNodes{}, mPath{path}, mFstream{} {
  mFstream.open(mPath, std::ios::in | std::ios::out | std::ios::binary);
  InitFromFile();
}

void BPlusTree::InitFromFile() {
  auto fileLength = std::filesystem::file_size(mPath);
  mNumNodesInFile = fileLength / kNodeSize;

  mNodes.resize(mNumNodesInFile);

  mRowInfo = GetRowInfo();
  gValueSize = mRowInfo.ValueSize();

  if (mNumNodesInFile <= 1) {
    if (mNumNodesInFile == 0) mNodes.push_back(nullptr);
    mNodes.push_back(std::make_unique<NodeVariant>(std::in_place_type<LeafNode>,
                                                   kStartNodeIndex, 0));
  }

  auto const getParentIndex = [](NodeVariant const& node) {
    return std::visit([](auto&& node) { return node.mParentIndex; }, node);
  };

  std::size_t rootIndex = kStartNodeIndex;
  std::size_t parentIndex = 0;
  while ((parentIndex = getParentIndex(Get(rootIndex))) != 0)
    rootIndex = parentIndex;

  mRootIndex = rootIndex;
}

RowInfo BPlusTree::GetRowInfo() {
  if (mNumNodesInFile == 0) return RowInfo();

  char rawMem[kNodeSize];
  mFstream.seekg(0, std::ios::beg);
  mFstream.read(rawMem, kNodeSize);

  return RowInfo(rawMem);
}

bool BPlusTree::SetRowInfo(RowInfo const& info) {
  if (!mRowInfo.mColumns.empty()) return false;

  auto buff = info.Serialize();
  mFstream.seekp(0, std::ios::beg);
  mFstream.write(buff.get(), kNodeSize);

  mRowInfo = info;
  gValueSize = mRowInfo.ValueSize();
  return true;
}

void BPlusTree::AddNodeFromFile(std::size_t index) {
  mFstream.seekg(index * kNodeSize, std::ios::beg);

  NodeType type;
  mFstream.read((char*)&type, sizeof(type));

  mNodes[index] = type == NodeType::Leaf
                      ? std::make_unique<NodeVariant>(
                            std::in_place_type<LeafNode>, index, mFstream)
                      : std::make_unique<NodeVariant>(
                            std::in_place_type<InternalNode>, index, mFstream);
}

BPlusTree::NodeVariant& BPlusTree::Get(std::size_t index) {
  if (!mNodes[index]) AddNodeFromFile(index);
  return *mNodes[index];
}

void BPlusTree::Print(std::ostream& ostreamObj) {
  Print(ostreamObj, Get(mRootIndex));
}

bool BPlusTree::PrintValue(std::ostream& ostreamObj, int32_t key) {
  auto leafNodeIndex = LeafNodeIndexForKey(key);
  auto& node = std::get<LeafNode>(Get(leafNodeIndex));

  auto searchRes = node.Search(key);
  if (!searchRes.exists) return false;

  Printer(ostreamObj, key, node.mElems[searchRes.index].value.get());
  return true;
}

void BPlusTree::Printer(std::ostream& ostreamObj, int32_t key,
                        const char* value) {
  auto&& cols = mRowInfo.mColumns;
  auto const keyOffset = DTSize(cols[0].type);

  ostreamObj << cols[0].columnName << "\t: " << key << "\n";

  auto const i32Size = DTSize(DataType::i32);
  auto const c100Size = DTSize(DataType::c100);

  for (std::size_t i = 1; i < cols.size(); ++i) {
#ifdef LOGS
    std::cerr << "mPrinterLambda\n";
    std::cerr << "\t i = " << i << "\n";
    std::cerr << "\t offset = " << cols[i].offset - keyOffset << "\n";
#endif
    ostreamObj << cols[i].columnName << "\t: ";
    if (cols[i].type == DataType::i32) {
      int32_t val;
      std::memcpy(&val, value + cols[i].offset - keyOffset, i32Size);
      ostreamObj << val << "\n";
    } else {
      ostreamObj.write(value + cols[i].offset - keyOffset, c100Size) << "\n";
    }
  }
  ostreamObj << "\n";
}

bool BPlusTree::Insert(Element elem) {
  std::size_t height;
  auto leafNodeIndex = LeafNodeIndexForKey(elem.key, &height);

  // nodes have to reserved as reallocation invalidates references.
  mNodes.reserve(mNodes.size() + height + 1);

  return Insert(std::get<LeafNode>(Get(leafNodeIndex)), elem);
}

bool BPlusTree::Replace(Element elem) {
  std::size_t leafNodeIndex = LeafNodeIndexForKey(elem.key);
  auto& node = std::get<LeafNode>(Get(leafNodeIndex));

  auto searchRes = node.Search(elem.key);
  if (!searchRes.exists) return false;

  node.mElems[searchRes.index] = std::move(elem);
  return true;
}

bool BPlusTree::Remove(int32_t key) {
  auto leafNodeIndex = LeafNodeIndexForKey(key);
  auto& node = std::get<LeafNode>(Get(leafNodeIndex));

  auto res = node.Search(key);
  if (!res.exists) return false;

  node.mElems.erase(std::next(node.mElems.begin(), res.index));
  return true;
}

void BPlusTree::Commit() {
  for (std::size_t i = kStartNodeIndex; i < mNodes.size(); ++i) {
    if (!mNodes[i]) continue;

    auto&& serializedBuffer =
        Match(*mNodes[i],
              With{[](LeafNode const& lN) { return lN.Serialize(); },
                   [](InternalNode const& iN) { return iN.Serialize(); }});

    mFstream.seekp(i * kNodeSize, std::ios::beg);
    mFstream.write(serializedBuffer.get(), kNodeSize);
    mFstream.flush();
  }

  mNumNodesInFile = mNodes.size();
}

void BPlusTree::Rollback() {
  for (auto&& node : mNodes) node = nullptr;
  InitFromFile();
}

void BPlusTree::Print(std::ostream& ostreamObj, NodeVariant const& node) {
  Match(node, With{[&](LeafNode const& lN) {
                     for (auto&& elem : lN.mElems) {
                       Printer(ostreamObj, elem.key, elem.value.get());
                     }
                   },
                   [&, this](InternalNode const& iN) {
                     for (auto&& child : iN.mChildren)
                       Print(ostreamObj, Get(child));
                   }});
}

std::size_t BPlusTree::LeafNodeIndexForKey(int32_t key, std::size_t* height) {
  auto nodeIndex = mRootIndex;
  std::size_t h = 1;

  while (std::holds_alternative<InternalNode>(Get(nodeIndex))) {
    auto& iN = std::get<InternalNode>(Get(nodeIndex));
    auto res = iN.Search(key);
    nodeIndex = iN.mChildren[res.index + (res.exists ? 1 : 0)];
    ++h;
  }

  if (height != nullptr) *height = h;

  return nodeIndex;
}

std::size_t BPlusTree::AddNewRootNode(std::size_t leftMostChild) {
  auto index = mNodes.size();

  mNodes.push_back(std::make_unique<NodeVariant>(
      std::in_place_type<InternalNode>, index, 0, leftMostChild));

  std::visit(With{[index](auto& node) { node.mParentIndex = index; }},
             Get(mRootIndex));

  mRootIndex = index;
  return mRootIndex;
}

std::size_t BPlusTree::InsertInternal(InternalNode& node, int32_t key,
                                      std::size_t child) {
  auto searchRes = node.Search(key);

  assert(searchRes.exists == false);

  if (node.mKeys.size() < kInternalNodeMaxKeys) {
    node.Insert(searchRes.index, key, child);
    return node.mIndex;
  }

  auto indexToMoveKeys = node.mKeys.size() / 2;
  if (key > node.mKeys[indexToMoveKeys]) ++indexToMoveKeys;

  std::size_t nodeFinalKeysSize = indexToMoveKeys;
  std::size_t indexToMoveChildren = indexToMoveKeys + 1;

  int32_t keyForParent;
  std::size_t anotherNodeFirstChild;

  if (node.mKeys[indexToMoveKeys - 1] < key &&
      key < node.mKeys[indexToMoveKeys]) {
    keyForParent = key;
    anotherNodeFirstChild = child;
  } else {
    keyForParent = node.mKeys[indexToMoveKeys];
    ++indexToMoveKeys;
    anotherNodeFirstChild = node.mChildren[indexToMoveChildren];
    ++indexToMoveChildren;
  }

  std::size_t parentIndex = mRootIndex != node.mIndex
                                ? node.mParentIndex
                                : AddNewRootNode(node.mIndex);

  std::size_t const anotherNodeIndex = mNodes.size();
  mNodes.push_back(std::make_unique<NodeVariant>(
      std::in_place_type<InternalNode>, anotherNodeIndex, parentIndex,
      anotherNodeFirstChild));

  parentIndex = InsertInternal(std::get<InternalNode>(Get(parentIndex)),
                               keyForParent, anotherNodeIndex);

  auto& anotherNode = std::get<InternalNode>(Get(anotherNodeIndex));
  anotherNode.mParentIndex = parentIndex;

  std::move(std::next(node.mKeys.begin(), indexToMoveKeys), node.mKeys.end(),
            std::back_inserter(anotherNode.mKeys));

  node.mKeys.resize(nodeFinalKeysSize);

  std::move(std::next(node.mChildren.begin(), indexToMoveChildren),
            node.mChildren.end(), std::back_inserter(anotherNode.mChildren));

  node.mChildren.resize(nodeFinalKeysSize + 1);

  if (key == keyForParent) {
    return anotherNodeIndex;
  } else {
    if (key < keyForParent) {
      node.Insert(searchRes.index, key, child);
      return node.mIndex;
    } else {
      anotherNode.Insert(searchRes.index - indexToMoveKeys, key, child);
      return anotherNodeIndex;
    }
  }
}

bool BPlusTree::Insert(LeafNode& node, Element& elem) {
  auto searchRes = node.Search(elem.key);
  if (searchRes.exists) return false;

  if (node.mElems.size() < leafNodeMaxElems()) {
    node.Insert(searchRes.index, elem);
    return true;
  }

  auto indexToMoveElems = node.mElems.size() / 2;
  if (elem.key > node.mElems[indexToMoveElems].key) ++indexToMoveElems;

  std::size_t parentIndex = mRootIndex != node.mIndex
                                ? node.mParentIndex
                                : AddNewRootNode(node.mIndex);

  std::size_t const anotherNodeIndex = mNodes.size();
  mNodes.push_back(std::make_unique<NodeVariant>(
      std::in_place_type<LeafNode>, anotherNodeIndex, parentIndex));

  parentIndex =
      InsertInternal(std::get<InternalNode>(Get(parentIndex)),
                     node.mElems[indexToMoveElems].key, anotherNodeIndex);

  auto& anotherNode = std::get<LeafNode>(Get(anotherNodeIndex));
  anotherNode.mParentIndex = parentIndex;

  std::move(std::next(node.mElems.begin(), indexToMoveElems), node.mElems.end(),
            std::back_inserter(anotherNode.mElems));

  node.mElems.resize(indexToMoveElems);

  if (elem.key < anotherNode.mElems[0].key) {
    anotherNode.Insert(searchRes.index, elem);
  } else {
    anotherNode.Insert(searchRes.index - indexToMoveElems, elem);
  }

  return true;
}

}  // namespace funcdb
