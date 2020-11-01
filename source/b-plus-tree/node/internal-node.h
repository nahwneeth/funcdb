#if !defined(FUNCDB_INTERNAL_NODE_H)
#define FUNCDB_INTERNAL_NODE_H

#include <cstdint>
#include <fstream>
#include <memory>
#include <vector>

#include "b-plus-tree/node/node.h"

namespace funcdb {

std::size_t constexpr kInternalNodeMaxKeys =
    ((kNodeSize - kCommonHeaderSize) - /*numKeys*/ sizeof(std::size_t) -
     /*leafChild*/ sizeof(std::size_t)) /
    (/*key + Child*/ 2 * sizeof(std::size_t));

class InternalNode : public Node {
 public:
  InternalNode(std::size_t index, std::size_t parentIndex,
               std::size_t firstChildIndex);

  InternalNode(std::size_t index, std::fstream& fstream,
               std::size_t beginOffset = sizeof(NodeType));

  std::unique_ptr<char[]> Serialize() const;

  void Insert(std::size_t index, std::size_t key, std::size_t rightChild);

  struct SearchResult {
    bool exists;
    std::size_t index;
  };

  SearchResult Search(int32_t key) const;

 public:
  std::vector<int32_t> mKeys;
  std::vector<std::size_t> mChildren;

  static NodeType constexpr mType = NodeType::Internal;
};

}  // namespace funcdb

#endif  // FUNCDB_INTERNAL_NODE_H
