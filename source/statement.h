#if !defined(FUNCDB_STATEMENT_HPP)
#define FUNCDB_STATEMENT_HPP

#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "table.h"

namespace funcdb {

class Statement {
 public:
  enum class PrepareError {
    EmptyColumnName,
    KeyNotFirst,
    RepeatedColumnNames,
    StringTooLong,
    SyntaxError,
    UnknowDataType,
    UnrecognizedStatement,
    WrongDataType
  };

  static std::variant<PrepareError, Statement> Prepare(
      RowInfo const& info, std::string const& input);

 public:
  enum class ExecuteResult {
    // ColumnDoesntExist,
    KeyAlreadyExists,
    KeyDoesntExist,
    Success,
    TableAlreadyCreated,
    TableNotCreated,
  };

  ExecuteResult Execute(Table& table) const;

 private:
  using PossibleDataTypes =
      std::variant<int32_t, std::vector<std::variant<int32_t, std::string>>,
                   std::vector<std::pair<DataType, std::string>>,
                   std::nullptr_t>;

  enum class Type {
    Create,
    Commit,
    Insert,
    Remove,
    Replace,
    Rollback,
    Select,
    SelectAll
  };

  Statement(Type type, PossibleDataTypes data = nullptr);

  static std::variant<PrepareError, Statement> PrepareCreate(
      std::vector<std::string> const& words);

  static std::variant<PrepareError, Statement> PrepareInsertOrReplace(
      RowInfo const& info, std::vector<std::string> const& words);

  static std::variant<PrepareError, Statement> PrepareSelectOrRemove(
      std::vector<std::string> const& words);

 private:
  static std::string const kCommandCreate;
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
