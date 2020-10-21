#include "statement.h"

#include <iostream>
#include <set>

#include "string-helper.h"
#include "table.h"

namespace funcdb {

std::string const Statement::kCommandCreate = "create";
std::string const Statement::kCommandInsert = "insert";
std::string const Statement::kCommandSelect = "select";
std::string const Statement::kCommandRemove = "remove";
std::string const Statement::kCommandReplace = "replace";
std::string const Statement::kCommandCommit = "commit";
std::string const Statement::kCommandRollback = "rollback";

Statement::Statement(Type type, PossibleDataTypes data)
    : mType{type}, mData{data} {}

std::variant<Statement::PrepareError, Statement> Statement::PrepareCreate(
    std::vector<std::string> const& words) {
  auto colNameSize = DTSize(DataType::c100);

  auto n = words.size();
  if (n == 1) return PrepareError::SyntaxError;

  auto cols = std::vector<std::pair<DataType, std::string>>();
  for (std::size_t i = 1; i < n; ++i) {
    auto&& word = words[i];
    auto size = word.size();
    if (word[size - 1] != ')') return PrepareError::SyntaxError;

    auto open = word.find('(');
    if (open == 0)
      return PrepareError::EmptyColumnName;
    else if (open == std::string::npos)
      return PrepareError::SyntaxError;

    auto colName = word.substr(0, open);
    auto dataTypeStr = word.substr(open + 1, (size - 2) - open);

#ifdef LOGS
    std::cerr << "word = " << word << "\n";
    std::cerr << "open = " << open << "\n";
    std::cerr << "colName = " << colName << "\n";
    std::cerr << "dataTypeStr = " << dataTypeStr << "\n";
#endif

    DataType dataType;
    if (dataTypeStr == k_i32_Str)
      dataType = DataType::i32;
    else if (dataTypeStr == k_c100_Str)
      dataType = DataType::c100;
    else
      return PrepareError::UnknowDataType;

    if (colName.size() > colNameSize) return PrepareError::StringTooLong;

    cols.emplace_back(dataType, colName);
  }

  auto uniqueColName = std::set<std::string>();
  for (auto&& col : cols) {
    if (!uniqueColName.insert(col.second).second)
      return PrepareError::RepeatedColumnNames;
  }

  if (cols[0].first != DataType::i32) return PrepareError::KeyNotFirst;

  return Statement(Type::Create, cols);
}

std::variant<Statement::PrepareError, Statement>
Statement::PrepareInsertOrReplace(RowInfo const& info,
                                  std::vector<std::string> const& words) {
  auto isInsert = words[0] == kCommandInsert;

  if (words.size() <= 1 || words.size() - 1 != info.mColumns.size())
    return PrepareError::SyntaxError;

  auto const c100Size = DTSize(DataType::c100);

  auto row = std::vector<std::variant<int32_t, std::string>>();

  for (std::size_t i = 1; i < words.size(); ++i) {
    if (info.mColumns[i - 1].type == DataType::i32) {
      int32_t val;
      try {
        val = std::stoi(words[i]);
      } catch (const std::exception& e) {
        return PrepareError::WrongDataType;
      }
      row.push_back(val);
    } else {
      if (words[i].size() > c100Size)
        return PrepareError::StringTooLong;
      else
        row.push_back(words[i]);
    }
  }
  return Statement((isInsert ? Type::Insert : Type::Replace), row);
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
    RowInfo const& info, std::string const& input) {
  auto words = Split(input);

  if (words.size() <= 0) return PrepareError::UnrecognizedStatement;

  if (words[0] == kCommandCreate) {
    return PrepareCreate(words);
  } else if ((words[0] == kCommandInsert) || words[0] == kCommandReplace) {
    return PrepareInsertOrReplace(info, words);
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
  if (!table.IsCreated() && mType != Type::Create)
    return ExecuteResult::TableNotCreated;

  if (mType == Type::Create) {
    auto const& data =
        std::get<std::vector<std::pair<DataType, std::string>>>(mData);
    if (!table.SetColumns(data)) return ExecuteResult::TableAlreadyCreated;
  } else if (mType == Type::Insert) {
    auto const& data =
        std::get<std::vector<std::variant<int32_t, std::string>>>(mData);
    if (!table.Insert(data)) return ExecuteResult::KeyAlreadyExists;
  } else if (mType == Type::Replace) {
    auto const& data =
        std::get<std::vector<std::variant<int32_t, std::string>>>(mData);
    if (!table.Replace(data)) return ExecuteResult::KeyDoesntExist;
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
