#include <filesystem>
#include <iostream>
#include <string>

#include "helpers/file-helper.h"
#include "helpers/match.h"
#include "helpers/string-helper.h"
#include "meta-command.h"
#include "statement.h"
#include "table.h"

using namespace funcdb;

enum ERROR { FILE_NAME_NOT_SPECIFIED = 1, FILE_ERROR };

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Filename not specified\n";
    return FILE_NAME_NOT_SPECIFIED;
  }

  auto filepath = argv[1];

  if (!CreateFileIfNotExist(filepath)) {
    std::cerr << "Failed to create file " << filepath << "\n";
    return FILE_ERROR;
  }

  switch (CheckFileTypeAndPerms(filepath)) {
    case FileTypeAndPermsResult::Valid:
      /* do nothing */
      break;
    case FileTypeAndPermsResult::NoReadPermission:
      std::cerr << "No permission to read from " << filepath << "\n";
      return FILE_ERROR;
    case FileTypeAndPermsResult::NotRegularFile:
      std::cerr << filepath << " is not a regular file\n";
      return FILE_ERROR;
    case FileTypeAndPermsResult::NoWritePermission:
      std::cerr << "No permission to write to " << filepath << "\n";
      return FILE_ERROR;
  }

  auto table = Table(filepath);

  auto const prompt = std::string("\n[db]: ");

  while (true) {
    auto input = ReadInput(prompt);

    if (input.empty()) continue;

    if (input[0] == '.') {
      auto result = DoMetaCommand(input);

      if (result == MetaCommandResult::Success)
        /* do nothing */;
      else if (result == MetaCommandResult::Exit)
        break;
      else if (result == MetaCommandResult::UnrecognizedCommand)
        std::cerr << "Unrecognized command [| " << input << " |]\n";

      continue;
    }

    auto statementRes = Statement::Prepare(table.GetRowInfo(), input);

    Match(statementRes,
          With{[&table](Statement &statement) {
                 switch (statement.Execute(table)) {
                   case Statement::ExecuteResult::Success:
                     /* do nothing */
                     break;
                   case Statement::ExecuteResult::KeyAlreadyExists:
                     std::cerr << "Error: Key already exists.\n";
                     break;
                   case Statement::ExecuteResult::KeyDoesntExist:
                     std::cerr << "Error: Key doesn't exist.\n";
                     break;
                   case Statement::ExecuteResult::TableAlreadyCreated:
                     std::cerr << "A table already exists.\n";
                     break;
                   case Statement::ExecuteResult::TableNotCreated:
                     std::cerr << "Table doesn't exist. Create a Table.\n";
                 }
               },
               [&input](Statement::PrepareError &error) {
                 switch (error) {
                   case Statement::PrepareError::UnrecognizedStatement:
                     std::cerr << "Unrecognized token at the start of [| "
                               << input << " |]\n";
                     break;
                   case Statement::PrepareError::StringTooLong:
                     std::cerr << "String too long.\n";
                     break;
                   case Statement::PrepareError::SyntaxError:
                     std::cerr << "Syntax error. Couldn't parse statement.\n";
                     break;
                   case Statement::PrepareError::EmptyColumnName:
                     std::cerr << "Empty column name given.\n";
                     break;
                   case Statement::PrepareError::RepeatedColumnNames:
                     std::cerr << "Column names should be unqiue.\n";
                     break;
                   case Statement::PrepareError::UnknowDataType:
                     std::cerr << "Unkown datatype given.\n";
                     break;
                   case Statement::PrepareError::KeyNotFirst:
                     std::cerr
                         << "Key of type i32 should be the first column.\n";
                     break;
                   case Statement::PrepareError::WrongDataType:
                     std::cerr << "Wrong Datatype entered.\n";
                 }
               }});
  }
}
