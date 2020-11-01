#include "node.h"

namespace funcdb {

Node::Node(std::size_t index, std::size_t parentIndex)
    : mIndex{index}, mParentIndex{parentIndex} {}

}  // namespace funcdb
