#include "meta-command.h"

#include <iostream>

namespace funcdb {

MetaCommandResult DoMetaCommand(std::string const& command) {
  auto const bold = "\033[1m";
  auto const reset = "\033[0m";

  if (command == ".exit") {
    return MetaCommandResult::Exit;
  } else if (command == ".help") {
    std::cout << "\n///// [ Data Types ] ///////////////////////////////\n\n";

    std::cout << bold << "i32" << reset << " : Signed Integer of 32-bits.\n";
    std::cout << bold << "c100" << reset << " : String of atmost 100 chars.\n\n";

    std::cout << "\n///// [ Commands ] /////////////////////////////////\n\n";

    std::cout << bold << "create <ColName1>(i32) <ColName2>(<DataType2>) ...\n" << reset;
    std::cout << "The first column is always a unique key of type `i32`. This is the key used to store data in the B+tree. The rest of the columm can have different types and names. Column names cannot be repeated. A table can be created only once for a file.\n\n";

    std::cout << bold << "insert <key> <value> ...\n" << reset;
    std::cout << "If the key doesn't already exist, the values are inserted into the table with the given key.\n\n";

    std::cout << bold << "select [key]\n" << reset;
    std::cout << "If key is specified and the key exists, the corresponding values are printed. If key is not specified then all the keys and values in the table are printed.\n\n";

    std::cout << bold <<  "replace <key> <value> ...\n" << reset;
    std::cout << "If the key exists, the values of the corresponding key will be replaced with the new ones.\n\n";

    std::cout << bold << "commit\n" << reset;
    std::cout << "Save changes to file.\n\n";

    std::cout << bold << "remove <key>\n" << reset;
    std::cout << "If the key exists, it and it's corresponding values are removed from the table.\n\n";

    std::cout << bold << "rollback\n" << reset;
    std::cout << "Discard changes made after the last commit.\n\n";

    std::cout << "///// [ Meta Commands ] ////////////////////////////\n\n";

    std::cout << bold << ".exit\n" << reset ;
    std::cout << "Exit without saving changes\n\n";

    std::cout << bold << ".help\n" << reset;
    std::cout << "Show help\n";
    return MetaCommandResult::Success;
  } else {
    return MetaCommandResult::UnrecognizedCommand;
  }
}

}  // namespace funcdb