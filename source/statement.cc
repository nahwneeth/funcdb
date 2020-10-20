#include "statement.h"

#include "string-helper.h"
#include "table.h"

namespace funcdb {

std::string const Statement::kCommandInsert = "insert";
std::string const Statement::kCommandSelect = "select";
std::string const Statement::kCommandReplace = "replace";
std::string const Statement::kCommandCommit = "commit";
std::string const Statement::kCommandRollback = "rollback";

Statement::Statement(Type type, PossibleDataTypes data)
    : mType{type}, mData{data} {}

std::variant<Statement::PrepareError, Statement> Statement::Prepare(
    std::string const& input) {
  auto words = Split(input);

  if (words.size() <= 0) return PrepareError::UnrecognizedStatement;

  bool is_insert = false;
  if ((is_insert = words[0] == kCommandInsert) || words[0] == kCommandReplace) {
    if (words.size() == 1 + 2) {
      auto key = 0LL;

      try {
        key = std::stoul(words[1]);
      } catch (const std::exception& e) {
        return PrepareError::SyntaxError;
      }

      if (words[2].size() > 255) {
        return PrepareError::StringTooLong;
      }

      auto row = std::pair<uint32_t, std::string>();
      row.first = key;
      row.second = words[2];

      return Statement((is_insert ? Type::Insert : Type::Replace), row);
    } else
      return PrepareError::SyntaxError;
  } else if (words[0] == kCommandSelect) {
    if (words.size() == 1) {
      return Statement(Type::SelectAll);
    } else if (words.size() == 1 + 1) {
      auto key = 0LL;

      try {
        key = std::stoul(words[1]);
      } catch (const std::exception& e) {
        return PrepareError::SyntaxError;
      }

      return Statement(Type::Select, key);

    } else
      return PrepareError::SyntaxError;
  } else if (words[0] == kCommandCommit) {
    return Statement(Type::Commit);
  } else if (words[0] == kCommandRollback) {
    return Statement(Type::Rollback);
  } else {
    return PrepareError::UnrecognizedStatement;
  }
}

Statement::ExecuteResult Statement::Execute(Table& table) const {
  if (mType == Type::Insert) {
    auto const& data = std::get<std::pair<uint32_t, std::string>>(mData);
    if (!table.Insert(data.first, data.second))
      return ExecuteResult::KeyAlreadyExists;
  } else if (mType == Type::Replace) {
    auto const& data = std::get<std::pair<uint32_t, std::string>>(mData);
    if (!table.Replace(data.first, data.second))
      return ExecuteResult::KeyDoesntExist;
  } else if (mType == Type::Select) {
    auto const& key = std::get<uint32_t>(mData);
    if (!table.Select(key)) return ExecuteResult::KeyDoesntExist;
  } else if (mType == Type::Commit) {
    table.Commit();
  } else if (mType == Type::Rollback) {
    table.Rollback();
  } else {
    table.SelectAll();
  }

  return ExecuteResult::Success;
}

}  // namespace funcdb
