#include "statement.h"

#include "string-helper.h"
#include "table.h"

namespace funcdb {

std::string const Statement::kCommandInsert = "insert";
std::string const Statement::kCommandSelect = "select";
std::string const Statement::kCommandRemove = "remove";
std::string const Statement::kCommandReplace = "replace";
std::string const Statement::kCommandCommit = "commit";
std::string const Statement::kCommandRollback = "rollback";

Statement::Statement(Type type, PossibleDataTypes data)
    : mType{type}, mData{data} {}

std::variant<Statement::PrepareError, Statement>
Statement::PrepareInsertOrReplace(std::vector<std::string> const& words) {
  auto isInsert = words[0] == kCommandInsert;

  if (words.size() == 1 + 2) {
    auto key = 0LL;

    try {
      key = std::stoi(words[1]);
    } catch (const std::exception& e) {
      return PrepareError::SyntaxError;
    }

    if (words[2].size() > 255) {
      return PrepareError::StringTooLong;
    }

    auto row = std::pair<int32_t, std::string>();
    row.first = key;
    row.second = words[2];

    return Statement((isInsert ? Type::Insert : Type::Replace), row);
  } else
    return PrepareError::SyntaxError;
}

std::variant<Statement::PrepareError, Statement>
Statement::PrepareSelectOrRemove(std::vector<std::string> const& words) {
  auto isSelect = words[0] == kCommandSelect;

  if (words.size() == 1 && isSelect) {
    return Statement(Type::SelectAll);
  } else if (words.size() == 1 + 1) {
    auto key = 0LL;

    try {
      key = std::stoul(words[1]);
    } catch (const std::exception& e) {
      return PrepareError::SyntaxError;
    }

    return Statement(isSelect ? Type::Select : Type::Remove, key);
  } else
    return PrepareError::SyntaxError;
}

std::variant<Statement::PrepareError, Statement> Statement::Prepare(
    std::string const& input) {
  auto words = Split(input);

  if (words.size() <= 0) return PrepareError::UnrecognizedStatement;

  if ((words[0] == kCommandInsert) || words[0] == kCommandReplace) {
    return PrepareInsertOrReplace(words);
  } else if (words[0] == kCommandSelect || words[0] == kCommandRemove) {
    return PrepareSelectOrRemove(words);
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
    auto const& data = std::get<std::pair<int32_t, std::string>>(mData);
    if (!table.Insert(data.first, data.second))
      return ExecuteResult::KeyAlreadyExists;
  } else if (mType == Type::Replace) {
    auto const& data = std::get<std::pair<int32_t, std::string>>(mData);
    if (!table.Replace(data.first, data.second))
      return ExecuteResult::KeyDoesntExist;
  } else if (mType == Type::Remove) {
    auto const& data = std::get<int32_t>(mData);
    if (!table.Remove(data)) return ExecuteResult::KeyDoesntExist;
  } else if (mType == Type::Select) {
    auto const& key = std::get<int32_t>(mData);
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
