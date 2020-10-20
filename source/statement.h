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
      std::variant<uint32_t, std::pair<uint32_t, std::string>, std::nullptr_t>;

  enum class Type { Commit, Insert, Replace, Rollback, Select, SelectAll };

  Statement(Type type, PossibleDataTypes data = nullptr);

 private:
  static std::string const kCommandInsert;
  static std::string const kCommandSelect;
  static std::string const kCommandReplace;
  static std::string const kCommandCommit;
  static std::string const kCommandRollback;

  Type mType;
  PossibleDataTypes mData;
};

}  // namespace funcdb

#endif  // FUNCDB_STATEMENT_HPP
