#if !defined(FUNCDB_LEAF_NODE_H)
#define FUNCDB_LEAF_NODE_H

#include <cstdint>
#include <fstream>
#include <memory>
#include <vector>

#include "b-plus-tree/node/node.h"

namespace funcdb {

extern std::size_t gValueSize;
std::size_t leafNodeMaxElems();

struct Element {
  int32_t key;
  std::unique_ptr<char[]> value;
};

class LeafNode : public Node {
 public:
  LeafNode(std::size_t index, std::fstream& fstream,
           std::size_t beginOffset = sizeof(NodeType));

  LeafNode(std::size_t index, std::size_t parentIndex);

  std::unique_ptr<char[]> Serialize() const;

  void Insert(std::size_t index, Element& elem);

  struct SearchResult {
    bool exists;
    std::size_t index;
  };

  SearchResult Search(int32_t key) const;

 public:
  std::vector<Element> mElems;

  static NodeType constexpr mType = NodeType::Leaf;
};

}  // namespace funcdb

#endif  // FUNCDB_LEAF_NODE_H
