#if !defined(FUNCDB_NODE_H)
#define FUNCDB_NODE_H

#include <cstddef>

namespace funcdb {

std::size_t constexpr kNodeSize = 4096;

enum class NodeType : char { Leaf, Internal };

std::size_t constexpr kCommonHeaderSize =
    /*mType*/ sizeof(NodeType) +
    /*mParentIndex*/ sizeof(std::size_t);

class Node {
 public:
  Node(std::size_t index, std::size_t parentIndex);

  Node(Node const&) = delete;
  Node(Node&&) = delete;

 public:
  std::size_t const mIndex;
  std::size_t mParentIndex;
};

}  // namespace funcdb

#endif  // FUNCDB_NODE_H
