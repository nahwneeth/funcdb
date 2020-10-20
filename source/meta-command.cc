#include "meta-command.h"

#include <iostream>

namespace funcdb {

MetaCommandResult DoMetaCommand(std::string const& command) {
  if (command == ".exit") {
    return MetaCommandResult::Exit;
  } else if (command == ".help") {
    std::cout << "\n///// [ Commands ] /////////////////////////////////\n\n";

    std::cout << "Insert new key value pair into database.\n";
    std::cout << "[]: insert [uint32_t key] [char[255] value]\n\n";

    std::cout << "Replace value of an existing key.\n";
    std::cout << "[]: replace [uint32_t key] [char[255] new_value]\n\n";

    std::cout << "Display value of a particular key.\n";
    std::cout << "[]: select [uint32_t key]\n\n";

    std::cout << "Display all key and values in database.\n";
    std::cout << "[]: select\n\n";

    std::cout << "Save changes made to file.\n";
    std::cout << "[]: commit\n\n";

    std::cout << "Discard all changes after last commit.\n";
    std::cout << "[]: rollback\n\n\n";

    std::cout << "///// [ Meta Commands ] ////////////////////////////\n\n";

    std::cout << "Exit without saving changes\n";
    std::cout << "[]: .exit\n\n";

    std::cout << "Show help\n";
    std::cout << "[]: .help\n\n";
    return MetaCommandResult::Success;
  } else {
    return MetaCommandResult::UnrecognizedCommand;
  }
}

}  // namespace funcdb