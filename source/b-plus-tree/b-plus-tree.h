#if !defined(FUNCDB_B_PLUS_TREE_H)
#define FUNCDB_B_PLUS_TREE_H

#include <filesystem>
#include <variant>
#include <vector>
#include <functional>

#include "b-plus-tree/node/internal-node.h"
#include "b-plus-tree/node/leaf-node.h"
#include "b-plus-tree/row-info.h"

namespace funcdb {

class BPlusTree {
 public:
  using PrintFunc = std::function<void(int32_t, const char*)>;

  static std::size_t constexpr kStartNodeIndex = 1;

  BPlusTree(std::filesystem::path path);

  RowInfo GetRowInfo();

  bool SetRowInfo(RowInfo const& info);

  void Print(std::ostream& ostreamObj);

  bool PrintValue(std::ostream& ostreamObj, int32_t key);

  bool Insert(Element elem);

  bool Replace(Element elem);

  bool Remove(int32_t key);

  void Commit();

  void Rollback();

 private:
  using NodeVariant = std::variant<LeafNode, InternalNode>;

  void InitFromFile();

  void AddNodeFromFile(std::size_t index);

  NodeVariant& Get(std::size_t index);

  void Print(std::ostream& ostreamObj, NodeVariant const& node);

  void Printer(std::ostream& ostreamObj, int32_t key, const char* value);

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

  RowInfo mRowInfo;
};

}  // namespace funcdb

#endif  // FUNCDB_B_PLUS_TREE_H
