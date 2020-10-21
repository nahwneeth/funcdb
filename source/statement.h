#if !defined(FUNCDB_STATEMENT_HPP)
#define FUNCDB_STATEMENT_HPP

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "table.h"

namespace funcdb {

class Statement {
 public:
  enum class PrepareError { UnrecognizedStatement, StringTooLong, SyntaxError };

  static std::variant<PrepareError, Statement> Prepare(
      std::string const& input);

 public:
  enum class ExecuteResult {
    Success,
    KeyAlreadyExists,
    KeyDoesntExist,
  };

  ExecuteResult Execute(Table& table) const;

 private:
  using PossibleDataTypes =
      std::variant<int32_t, std::pair<int32_t, std::string>, std::nullptr_t>;

  enum class Type {
    Commit,
    Insert,
    Remove,
    Replace,
    Rollback,
    Select,
    SelectAll
  };

  Statement(Type type, PossibleDataTypes data = nullptr);

  static std::variant<PrepareError, Statement> PrepareInsertOrReplace(
      std::vector<std::string> const& words);

  static std::variant<PrepareError, Statement> PrepareSelectOrRemove(
      std::vector<std::string> const& words);

 private:
  static std::string const kCommandInsert;
  static std::string const kCommandSelect;
  static std::string const kCommandRemove;
  static std::string const kCommandReplace;
  static std::string const kCommandCommit;
  static std::string const kCommandRollback;

  Type mType;
  PossibleDataTypes mData;
};

}  // namespace funcdb

#endif  // FUNCDB_STATEMENT_HPP
