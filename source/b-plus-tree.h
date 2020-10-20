#if !defined(FUNCDB_B_PLUS_TREE_H)
#define FUNCDB_B_PLUS_TREE_H

#include <filesystem>
#include <variant>
#include <vector>

#include "match.h"
#include "internal-node.h"
#include "leaf-node.h"

namespace funcdb {

class BPlusTree {
 public:
  BPlusTree(std::filesystem::path path);

  void Print(std::ostream& ostreamObj);

  bool PrintValue(std::ostream& ostreamObj, int32_t key);

  bool Insert(Element elem);

  bool Replace(Element elem);

  void Commit();

  void Rollback();

 private:
  using NodeVariant = std::variant<LeafNode, InternalNode>;

  void InitFromFile();

  void AddNodeFromFile(std::size_t index);

  NodeVariant& Get(std::size_t index);

  void Print(std::ostream& ostreamObj, NodeVariant const& node);

  std::size_t LeafNodeIndexForKey(int32_t key, std::size_t* height = nullptr);

  std::size_t AddNewRootNode(std::size_t leftMostChild);

  std::size_t InsertInternal(InternalNode& node, int32_t key,
                             std::size_t child);

  bool Insert(LeafNode& node, Element& elem);

 public:
  std::vector<std::unique_ptr<NodeVariant> > mNodes;
  std::size_t mRootIndex;

  std::filesystem::path const mPath;
  std::fstream mFstream;
  std::size_t mNumNodesInFile;
};

}  // namespace funcdb

#endif  // FUNCDB_B_PLUS_TREE_H
